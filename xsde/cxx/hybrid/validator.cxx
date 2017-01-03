// file      : xsde/cxx/hybrid/validator.cxx
// copyright : Copyright (c) 2005-2017 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <set>
#include <iostream>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

#include <cxx/hybrid/elements.hxx>
#include <cxx/hybrid/validator.hxx>

using namespace std;

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      // H004 is used by tree-size-processor.
      //

      class ValidationContext: public Context
      {
      public:
        ValidationContext (SemanticGraph::Schema& root,
                           SemanticGraph::Path const& path,
                           Hybrid::options const& ops,
                           const WarningSet& disabled_warnings,
                           bool& valid_)
            : Context (std::wcerr, root, path, ops, 0, 0, 0),
              disabled_warnings_ (disabled_warnings),
              disabled_warnings_all_ (false),
              valid (valid_),
              subst_group_warning_issued (subst_group_warning_issued_),
              subst_group_warning_issued_ (false)
        {
          if (disabled_warnings_.find ("all") != disabled_warnings_.end ())
            disabled_warnings_all_ = true;
        }

      public:
        bool
        is_disabled (char const* w)
        {
          return disabled_warnings_all_ ||
            disabled_warnings_.find (w) != disabled_warnings_.end ();
        }

      public:
        String
        xpath (SemanticGraph::Nameable& n)
        {
          if (n.is_a<SemanticGraph::Namespace> ())
            return L"<namespace-level>"; // There is a bug if you see this.

          assert (n.named_p ());

          SemanticGraph::Scope& scope (n.scope ());

          if (scope.is_a<SemanticGraph::Namespace> ())
            return n.name ();

          return xpath (scope) + L"/" + n.name ();
        }

      protected:
        ValidationContext (ValidationContext& c)
            :  Context (c),
               disabled_warnings_ (c.disabled_warnings_),
               disabled_warnings_all_ (c.disabled_warnings_all_),
               valid (c.valid),
               subst_group_warning_issued (c.subst_group_warning_issued)
        {
        }

      protected:
        const WarningSet& disabled_warnings_;
        bool disabled_warnings_all_;
        bool& valid;
        bool& subst_group_warning_issued;
        bool subst_group_warning_issued_;
      };

      //
      //
      struct Traverser : Traversal::Schema,
                         Traversal::Complex,
                         Traversal::Type,
                         Traversal::Element,
                         ValidationContext
      {
        Traverser (ValidationContext& c)
            : ValidationContext (c)
        {
          *this >> sources_ >> *this;
          *this >> schema_names_ >> ns_ >> names_ >> *this;
        }

        virtual void
        traverse (SemanticGraph::Complex& c)
        {
          using SemanticGraph::Schema;

          traverse (static_cast<SemanticGraph::Type&> (c));

          if (c.inherits_p ())
          {
            SemanticGraph::Type& t (c.inherits ().base ());

            if (t.named_p () &&
                types_.find (
                  t.scope ().name () + L"#" + t.name ()) == types_.end ())
            {
              // Don't worry about types that are in included/imported
              // schemas.
              //
              Schema& s (dynamic_cast<Schema&> (t.scope ().scope ()));

              if (&s == &schema_root || sources_p (schema_root, s))
              {
                valid = false;

                wcerr << c.file () << ":" << c.line () << ":" << c.column ()
                      << ": error: type '" << xpath (c) << "' inherits from "
                      << "yet undefined type '" << xpath (t) << "'" << endl;

                wcerr << t.file () << ":" << t.line () << ":" << t.column ()
                      << ": info: '" << xpath (t) << "' is defined here"
                      << endl;

                wcerr << c.file () << ":" << c.line () << ":" << c.column ()
                      << ": info: inheritance from a yet-undefined type is "
                      << "not supported" << endl;

                wcerr << c.file () << ":" << c.line () << ":" << c.column ()
                      << ": info: re-arrange your schema and try again"
                      << endl;
              }
            }
          }
        }

        virtual void
        traverse (SemanticGraph::Type& t)
        {
          if (t.named_p ())
          {
            types_.insert (t.scope ().name () + L"#" + t.name ());
          }
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          if (is_disabled ("H001"))
            return;

          if (e.substitutes_p () &&
              !options.generate_polymorphic () &&
              !subst_group_warning_issued)
          {
            subst_group_warning_issued = true;

            os << e.file () << ":" << e.line () << ":" << e.column ()
               << ": warning H001: substitution groups are used but "
               << "--generate-polymorphic was not specified" << endl;

            os << e.file () << ":" << e.line () << ":" << e.column ()
               << ": info: generated code may not be able to handle "
               << "some conforming instances" << endl;
          }
        }

        // Return true if root sources s.
        //
        bool
        sources_p (SemanticGraph::Schema& root, SemanticGraph::Schema& s)
        {
          using SemanticGraph::Schema;
          using SemanticGraph::Sources;

          for (Schema::UsesIterator i (root.uses_begin ());
               i != root.uses_end (); ++i)
          {
            if (i->is_a<Sources> ())
            {
              if (&i->schema () == &s || sources_p (i->schema (), s))
                return true;
            }
          }

          return false;
        }

      private:
        set<String> types_;

        Sources sources_;

        Traversal::Names schema_names_;
        Traversal::Namespace ns_;

        Traversal::Names names_;
      };

      //
      //
      struct AnonymousMember: protected ValidationContext
      {
        AnonymousMember (ValidationContext& c, bool& error_issued)
            : ValidationContext (c), error_issued_ (error_issued)
        {
        }

        bool
        traverse_common (SemanticGraph::Member& m)
        {
          SemanticGraph::Type& t (m.type ());

          if (!t.named_p ()
              && !t.is_a<SemanticGraph::Fundamental::IdRef> ()
              && !t.is_a<SemanticGraph::Fundamental::IdRefs> ())
          {
            if (!error_issued_)
            {
              valid = false;
              error_issued_ = true;

              wcerr << t.file ()
                    << ": error: anonymous types detected"
                    << endl;

              wcerr << t.file ()
                    << ": info: "
                    << "anonymous types are not supported in this mapping"
                    << endl;

              wcerr << t.file ()
                    << ": info: consider explicitly naming these types or "
                    << "remove the --preserve-anonymous option to "
                    << "automatically name them"
                    << endl;

              if (!options.show_anonymous ())
                wcerr << t.file ()
                      << ": info: use --show-anonymous option to see these "
                      << "types" << endl;
            }

            return true;
          }

          return false;
        }

      private:
        bool& error_issued_;
      };

      struct AnonymousElement: Traversal::Element,
                               AnonymousMember
      {
        AnonymousElement (ValidationContext& c, bool& error_issued)
            : AnonymousMember (c, error_issued)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          if (traverse_common (e))
          {
            if (options.show_anonymous ())
            {
              wcerr << e.file () << ":" << e.line () << ":" << e.column ()
                    << ": error: element '" << xpath (e) << "' "
                    << "is of anonymous type" << endl;
            }
          }
          else
            Traversal::Element::traverse (e);
        }
      };

      struct AnonymousAttribute: Traversal::Attribute,
                                 AnonymousMember
      {
        AnonymousAttribute (ValidationContext& c, bool& error_issued)
            : AnonymousMember (c, error_issued)
        {
        }

        virtual void
        traverse (Type& a)
        {
          if (traverse_common (a))
          {
            if (options.show_anonymous ())
            {
              wcerr << a.file () << ":" << a.line () << ":" << a.column ()
                    << ": error: attribute '" << xpath (a) << "' "
                    << "is of anonymous type" << endl;
            }
          }
          else
            Traversal::Attribute::traverse (a);
        }
      };

      struct AnonymousType : Traversal::Schema,
                             Traversal::Complex,
                             ValidationContext
      {
        AnonymousType (ValidationContext& c)
            : ValidationContext (c),
              error_issued_ (false),
              element_ (c, error_issued_),
              attribute_ (c, error_issued_)
        {
          *this >> sources_ >> *this;
          *this >> schema_names_ >> ns_ >> names_ >> *this;

          *this >> contains_compositor_ >> compositor_;
          compositor_ >> contains_particle_;
          contains_particle_ >> compositor_;
          contains_particle_ >> element_;

          *this >> names_attribute_ >> attribute_;
        }

      private:
        bool error_issued_;

        set<String> types_;

        Sources sources_;

        Traversal::Names schema_names_;
        Traversal::Namespace ns_;
        Traversal::Names names_;

        Traversal::Compositor compositor_;
        AnonymousElement element_;
        Traversal::ContainsCompositor contains_compositor_;
        Traversal::ContainsParticle contains_particle_;

        AnonymousAttribute attribute_;
        Traversal::Names names_attribute_;
      };

      struct GlobalElementCount: Traversal::Element
      {
        GlobalElementCount (size_t& count)
            : count_ (count)
        {
        }

        virtual void
        traverse (SemanticGraph::Element&)
        {
          count_++;
        }

      private:
        size_t& count_;
      };

      struct ParserValidation: Traversal::All,
                               Traversal::Choice,
                               Traversal::Sequence,
                               Traversal::Complex,
                               ValidationContext

      {
        ParserValidation (ValidationContext& c)
            : ValidationContext (c), issued_ (false)
        {
          contains_compositor_ >> *this;
          *this >> contains_particle_ >> *this;
        }

        virtual void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}.
          //
          if (!issued_ && a.min () == 0)
          {
            valid = false;
            issued_ = true;

            wcerr << a.file () << ":" << a.line () << ":" << a.column ()
                  << ": error: complex type '" << type (a).name () << "' "
                  << "contains optional all compositor" << endl;

            wcerr << a.file () << ":" << a.line () << ":" << a.column ()
                  << ": error: parser validation is required to handle this "
                  << "construct" << endl;
          }
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          if (!issued_)
          {
            valid = false;
            issued_ = true;

            wcerr << c.file () << ":" << c.line () << ":" << c.column ()
                  << ": error: complex type '" << type (c).name () << "' "
                  << "contains choice compositor" << endl;

            wcerr << c.file () << ":" << c.line () << ":" << c.column ()
                  << ": error: parser validation is required to handle this "
                  << "construct" << endl;
          }
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          if (!issued_)
          {
            if (s.max () != 1)
            {
              issued_ = true;
              wcerr << s.file () << ":" << s.line () << ":" << s.column ()
                    << ": error: complex type '" << type (s).name () << "' "
                    << "contains sequence of sequence compositor" << endl;
            }
            else if (s.min () == 0)
            {
              issued_ = true;
              wcerr << s.file () << ":" << s.line () << ":" << s.column ()
                    << ": error: complex type '" << type (s).name () << "' "
                    << "contains optional sequence compositor" << endl;
            }

            if (issued_)
            {
              valid = false;
              wcerr << s.file () << ":" << s.line () << ":" << s.column ()
                    << ": error: parser validation is required to handle "
                    << "this construct" << endl;
            }
          }

          Traversal::Sequence::traverse (s);
        }

        virtual void
        traverse (SemanticGraph::Complex& c)
        {
          if (!issued_ && !restriction_p (c))
            contains_compositor (c, contains_compositor_);
        }

      private:
        SemanticGraph::Complex&
        type (SemanticGraph::Compositor& c)
        {
          SemanticGraph::Compositor* root (&c);

          while (root->contained_particle_p ())
            root = &root->contained_particle ().compositor ();

          return dynamic_cast<SemanticGraph::Complex&> (
            root->contained_compositor ().container ());
        }

      private:
        bool issued_;

        Traversal::ContainsCompositor contains_compositor_;
        Traversal::ContainsParticle contains_particle_;
      };
    }

    bool Validator::
    validate (options const& ops,
              SemanticGraph::Schema& root,
              SemanticGraph::Path const& path,
              const WarningSet& disabled_warnings)
    {
      bool valid (true);
      ValidationContext ctx (root, path, ops, disabled_warnings, valid);

      //
      //
      NarrowString enc (ops.char_encoding ());

      if (enc != "utf8" && enc != "iso8859-1")
      {
        wcerr << "error: unknown encoding '" << enc.c_str () << "'" << endl;
        return false;
      }

      //
      //
      bool par (ops.generate_parser ());
      bool ser (ops.generate_serializer ());
      bool agg (ops.generate_aggregate ());

      if (agg && !par && !ser && !ctx.is_disabled ("H002"))
      {
        wcerr << root.file () << ": warning H002: --generate-aggregate " <<
            "is specified but neither parsing nor serialization code is " <<
          "generated" << endl;
      }

      // Issue a warning if there are more than one global element and we
      // are generating aggregate types.
      //
      if (agg &&
          !ops.root_element_first () &&
          !ops.root_element_last () &&
          !ops.root_element_all () &&
          !ops.root_element_none () &&
          ops.root_element ().empty () &&
          !ctx.is_disabled ("H003"))
      {
        size_t count (0);

        {
          Traversal::Schema schema;
          Sources sources;

          schema >> sources >> schema;

          Traversal::Names schema_names;
          Traversal::Namespace ns;
          Traversal::Names names;

          schema >> schema_names >> ns >> names;

          GlobalElementCount element (count);

          names >> element;

          schema.dispatch (root);
        }

        if (count > 1)
        {
          wcerr << root.file () << ": warning H003: generating aggregate " <<
            "types for " << count << " global elements" << endl;

          wcerr << root.file () << ": info: use --root-element-* options "
                << "to specify document root(s)" << endl;
        }
      }

      // Test for anonymout types.
      //
      {
        AnonymousType traverser (ctx);
        traverser.dispatch (root);
      }

      // Test for constructs that require validation in parser.
      //
      if (valid && par &&
          (ops.suppress_validation () || ops.suppress_parser_val ()))
      {
        Traversal::Schema schema;
        Sources sources;

        schema >> sources >> schema;

        Traversal::Names schema_names;
        Traversal::Namespace ns;
        Traversal::Names names;

        schema >> schema_names >> ns >> names;

        ParserValidation  type (ctx);

        names >> type;

        schema.dispatch (root);
      }

      // Test the rest.
      //
      if (valid)
      {
        Traverser traverser (ctx);
        traverser.dispatch (root);
      }

      return valid;
    }
  }
}
