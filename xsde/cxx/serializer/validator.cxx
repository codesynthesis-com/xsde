// file      : xsde/cxx/serializer/validator.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/serializer/validator.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

#include <cxx/serializer/elements.hxx>

#include <iostream>

using std::wcerr;

namespace CXX
{
  namespace Serializer
  {
    namespace
    {
      class ValidationContext: public Context
      {
      public:
        ValidationContext (SemanticGraph::Schema& root,
                           SemanticGraph::Path const& path,
                           Serializer::options const& ops,
                           const WarningSet& disabled_warnings,
                           Boolean& valid_)
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
        Boolean
        is_disabled (Char const* w)
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
        Boolean disabled_warnings_all_;
        Boolean& valid;
        Boolean& subst_group_warning_issued;
        Boolean subst_group_warning_issued_;
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

        virtual Void
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

        virtual Void
        traverse (SemanticGraph::Type& t)
        {
          if (t.named_p ())
          {
            types_.insert (t.scope ().name () + L"#" + t.name ());
          }
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          if (is_disabled ("S001"))
            return;

          if (e.substitutes_p () &&
              !options.generate_polymorphic () &&
              !subst_group_warning_issued)
          {
            subst_group_warning_issued = true;

            os << e.file () << ":" << e.line () << ":" << e.column ()
               << ": warning S001: substitution groups are used but "
               << "--generate-polymorphic was not specified" << endl;

            os << e.file () << ":" << e.line () << ":" << e.column ()
               << ": info: generated code may not be able to serialize "
               << "some conforming instances" << endl;
          }
        }

        // Return true if root sources s.
        //
        Boolean
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
        Containers::Set<String> types_;

        Sources sources_;

        Traversal::Names schema_names_;
        Traversal::Namespace ns_;

        Traversal::Names names_;
      };

      //
      //
      struct AnonymousMember: protected ValidationContext
      {
        AnonymousMember (ValidationContext& c, Boolean& error_issued)
            : ValidationContext (c), error_issued_ (error_issued)
        {
        }

        Boolean
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
        Boolean& error_issued_;
      };

      struct AnonymousElement: Traversal::Element,
                               AnonymousMember
      {
        AnonymousElement (ValidationContext& c, Boolean& error_issued)
            : AnonymousMember (c, error_issued)
        {
        }

        virtual Void
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
        AnonymousAttribute (ValidationContext& c, Boolean& error_issued)
            : AnonymousMember (c, error_issued)
        {
        }

        virtual Void
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
        Boolean error_issued_;

        Containers::Set<String> types_;

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

      struct GlobalElement: Traversal::Element, ValidationContext
      {
        GlobalElement (ValidationContext& c, SemanticGraph::Element*& element)
            : ValidationContext (c), element_ (element)
        {
        }

        virtual Void
        traverse (Type& e)
        {
          if (!valid)
            return;

          if (options.root_element_first ())
          {
            if (element_ == 0)
              element_ = &e;
          }
          else if (options.root_element_last ())
          {
            element_ = &e;
          }
          else if (String name = options.root_element ())
          {
            if (e.name () == name)
              element_ = &e;
          }
          else
          {
            if (element_ == 0)
              element_ = &e;
            else
            {
              wcerr << schema_root.file () << ": error: unable to generate "
                    << "the test driver without a unique document root"
                    << endl;

              wcerr << schema_root.file () << ": info: use --root-element-* "
                    << "options to specify the document root" << endl;

              valid = false;
            }
          }
        }

      private:
        SemanticGraph::Element*& element_;
      };
    }

    Boolean Validator::
    validate (options const& ops,
              SemanticGraph::Schema& root,
              SemanticGraph::Path const& path,
              Boolean gen_driver,
              const WarningSet& disabled_warnings)
    {
      Boolean valid (true);
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
      {
        Boolean ref (ops.root_element_first ());
        Boolean rel (ops.root_element_last ());
        Boolean re (ops.root_element ());

        if ((ref && rel) || (ref && re) || (rel && re))
        {
          wcerr << "error: mutually exclusive options specified: "
                << "--root-element-last, --root-element-first, and "
                << "--root-element"
                << endl;

          return false;
        }
      }

      //
      //
      if (ops.reuse_style_mixin () && ops.reuse_style_none ())
      {
        wcerr << "error: mutually exclusive options specified: "
              << "--reuse-style-mixin and --reuse-style-none"
              << endl;

        return false;
      }

      //
      //
      if (ops.reuse_style_none () &&
          ops.generate_empty_impl () &&
          !ctx.is_disabled ("S002"))
      {
        wcerr << "warning S002: generating sample implementation without "
              << "serializer reuse support: the resulting code may not "
              << "compile"
              << endl;

        return false;
      }

      // Test for anonymout types.
      //
      {
        AnonymousType traverser (ctx);
        traverser.dispatch (root);
      }

      // Test the rest.
      //
      if (valid)
      {
        Traverser traverser (ctx);
        traverser.dispatch (root);
      }

      // Test that the document root is unique.
      //
      if (valid && gen_driver)
      {
        SemanticGraph::Element* element (0);

        Traversal::Schema schema;
        Sources sources;

        schema >> sources >> schema;

        Traversal::Names schema_names;
        Traversal::Namespace ns;
        Traversal::Names ns_names;
        GlobalElement global_element (ctx, element);

        schema >> schema_names >> ns >> ns_names >> global_element;

        schema.dispatch (root);

        if (valid && element == 0)
        {
          wcerr << root.file () << ": error: unable to generate the "
                << "test driver without a global element (document root)"
                << endl;

          valid = false;
        }
      }

      return valid;
    }
  }
}
