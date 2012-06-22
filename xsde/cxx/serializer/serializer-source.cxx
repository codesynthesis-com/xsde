// file      : xsde/cxx/serializer/serializer-source.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <set>

#include <cxx/serializer/serializer-source.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

using namespace std;

namespace CXX
{
  namespace Serializer
  {
    namespace
    {
      // Override classes override pure virtual functions in the base.
      // Should be in sync with definition generators below. Used in
      // tiein implementation.
      //

      struct CompositorCallbackOverride: Traversal::Choice,
                                         Traversal::Sequence,
                                         Context
      {
        CompositorCallbackOverride (Context& c, String const& scope)
            : Context (c), scope_ (scope)
        {
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.contains_begin () != c.contains_end ())
          {
            size_t min (c.min ()), max (c.max ());

            if (min != 0)
            {
              SemanticGraph::Type& t (
                dynamic_cast<SemanticGraph::Type&> (scope (c)));

              String const& impl (etiein (t));

              if (max != 1)
              {
                String const& next (enext (c));

                os << "bool " << scope_ << "::" << endl
                   << next << " ()"
                   << "{"
                   << "assert (this->" << impl << ");"
                   << "return this->" << impl << "->" << next << " ();"
                   << "}";
              }

              String const& arm (earm (c));

              os << fq_name (t) << "::" << earm_tag (c) << " " <<
                scope_ << "::" << endl
                 << arm << " ()"
                 << "{"
                 << "assert (this->" << impl << ");"
                 << "return this->" << impl << "->" << arm << " ();"
                 << "}";
            }

            Traversal::Choice::traverse (c);
          }
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          size_t min (s.min ()), max (s.max ());

          if (max != 1 && min != 0)
          {
            String const& impl (
              etiein (dynamic_cast<SemanticGraph::Type&> (scope (s))));

            String const& next (enext (s));

            os << "bool " << scope_ << "::" << endl
               << next << " ()"
               << "{"
               << "assert (this->" << impl << ");"
               << "return this->" << impl << "->" << next << " ();"
               << "}";
          }

          Traversal::Sequence::traverse (s);
        }

      private:
        SemanticGraph::Scope&
        scope (SemanticGraph::Compositor& c)
        {
          SemanticGraph::Compositor* root (&c);

          while (root->contained_particle_p ())
            root = &root->contained_particle ().compositor ();

          return dynamic_cast<SemanticGraph::Scope&> (
            root->contained_compositor ().container ());
        }

      private:
        String const& scope_;
      };

      struct ParticleCallbackOverride: Traversal::Element,
                                       Traversal::Any,
                                       Context
      {
        ParticleCallbackOverride (Context& c, String const& scope)
            : Context (c), scope_ (scope)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          size_t min (e.min ()), max (e.max ());

          String const& impl (
            etiein (dynamic_cast<SemanticGraph::Type&> (e.scope ())));

          if (max != 1 && min != 0)
          {
            String const& next (enext (e));

            os << "bool " << scope_ << "::" << endl
               << next << " ()"
               << "{"
               << "assert (this->" << impl << ");"
               << "return this->" << impl << "->" << next << " ();"
               << "}";
          }

          String const& ret (ret_type (e.type ()));

          if (ret != L"void")
          {
            String const& name (ename (e));

            os << ret << " " << scope_ << "::" << endl
               << name << " ()"
               << "{"
               << "assert (this->" << impl << ");"
               << "return this->" << impl << "->" << name << " ();"
               << "}";
          }
        }

        virtual void
        traverse (SemanticGraph::Any& a)
        {
          size_t min (a.min ()), max (a.max ());

          if (min != 0 &&
              !a.contained_particle ().compositor ().is_a<
              SemanticGraph::Choice> ())
          {
            String const& impl (
              etiein (dynamic_cast<SemanticGraph::Type&> (a.scope ())));

            if (max != 1)
            {
              String const& next (enext (a));

              os << "bool " << scope_ << "::" << endl
                 << next << " ()"
                 << "{"
                 << "assert (this->" << impl << ");"
                 << "return this->" << impl << "->" << next << " ();"
                 << "}";
            }

            String const& name (ename (a));

            if (stl)
            {
              os << "void " << scope_ << "::" << endl
                 << name << " (::std::string& ns, ::std::string& name)"
                 << "{"
                 << "assert (this->" << impl << ");"
                 << "this->" << impl << "->" << name << " (ns, name);"
                 << "}";
            }
            else
            {
              os << "void " << scope_ << "::" << endl
                 << name << " (const char*& ns, const char*& name, " <<
                "bool& free)"
                 << "{"
                 << "assert (this->" << impl << ");"
                 << "this->" << impl << "->" << name << " (ns, name, free);"
                 << "}";
            }

            String const& serialize (eserialize (a));

            os << "void " << scope_ << "::" << endl
               << serialize << " ()"
               << "{"
               << "assert (this->" << impl << ");"
               << "this->" << impl << "->" << serialize << " ();"
               << "}";
          }
        }

      private:
        String const& scope_;
      };

      struct AttributeCallbackOverride: Traversal::Attribute, Context
      {
        AttributeCallbackOverride (Context& c, String const& scope)
            : Context (c), scope_ (scope)
        {
        }

        virtual void
        traverse (SemanticGraph::Attribute& a)
        {
          String const& ret (ret_type (a.type ()));

          if (ret != L"void")
          {
            String const& impl (
              etiein (dynamic_cast<SemanticGraph::Type&> (a.scope ())));

            String const& name (ename (a));

            os << ret << " " << scope_ << "::" << endl
               << name << " ()"
               << "{"
               << "assert (this->" << impl << ");"
               << "return this->" << impl << "->" << name << " ();"
               << "}";
          }
        }

      private:
        String const& scope_;
      };

      struct BaseOverride: Traversal::Type,
                           Traversal::Enumeration,
                           Traversal::List,
                           Traversal::Union,
                           Traversal::Complex,
                           Context
      {
        BaseOverride (Context& c, String const& scope)
            : Context (c),
              scope_ (scope),
              compositor_callback_ (c, scope),
              particle_callback_ (c, scope),
              attribute_callback_ (c, scope)
        {
          contains_compositor_callback_ >> compositor_callback_;
          compositor_callback_ >> contains_particle_callback_;
          contains_particle_callback_ >> compositor_callback_;
          contains_particle_callback_ >> particle_callback_;

          names_attribute_callback_ >> attribute_callback_;
        }

        virtual void
        traverse (SemanticGraph::Type& t)
        {
          // pre
          //
          String const& arg (arg_type (t));

          if (arg != L"void")
          {
            String const& impl (etiein (t));

            os << "void " << scope_ << "::" << endl
               << "pre (" << arg << " x)"
               << "{"
               << "assert (this->" << impl << ");"
               << "this->" << impl << "->pre (x);"
               << "}";
          }
        }

        virtual void
        traverse (SemanticGraph::Enumeration& e)
        {
          SemanticGraph::Type& t (e);
          traverse (t);
        }

        virtual void
        traverse (SemanticGraph::List& l)
        {
          SemanticGraph::Type& t (l);
          traverse (t);

          // item
          //
          String const& ret (ret_type (l.argumented ().type ()));

          if (ret != L"void")
          {
            String item (unclash (ename (l), "item"));
            String const& impl (etiein (l));

            os << ret << " " << scope_ << "::" << endl
               << item << " ()"
               << "{"
               << "assert (this->" << impl << ");"
               << "return this->" << impl << "->" << item << " ();"
               << "}";
          }
        }

        virtual void
        traverse (SemanticGraph::Union& u)
        {
          SemanticGraph::Type& t (u);
          traverse (t);

          // serialize_content
          //
          String const& impl (etiein (u));

          os << "void " << scope_ << "::" << endl
             << "_serialize_content ()"
             << "{"
             << "assert (this->" << impl << ");"
             << "this->" << impl << "->_serialize_content ();"
             << "}";
        }

        virtual void
        traverse (SemanticGraph::Complex& c)
        {
          SemanticGraph::Type& t (c);
          traverse (t);

          // Member callbacks.
          //
          if (!restriction_p (c))
          {
            Traversal::Complex::names (c, names_attribute_callback_);
            Traversal::Complex::contains_compositor (
              c, contains_compositor_callback_);
          }
        }

      private:
        String const& scope_;

        CompositorCallbackOverride compositor_callback_;
        ParticleCallbackOverride particle_callback_;
        Traversal::ContainsCompositor contains_compositor_callback_;
        Traversal::ContainsParticle contains_particle_callback_;

        AttributeCallbackOverride attribute_callback_;
        Traversal::Names names_attribute_callback_;
      };

      //
      //

      struct Enumeration: Traversal::Enumeration, Context
      {
        Enumeration (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& e)
        {
          String const& name (ename (e));
          String const& arg (arg_type (e));
          SemanticGraph::Type& base (e.inherits ().base ());

          bool enum_facets (false); // Whether we need to set enum facets.
          if (validation)
          {
            StringBasedType t (enum_facets);
            t.dispatch (e);
          }

          os << "// " << name << endl
             << "//" << endl
             << endl;

          if (arg != arg_type (base) && arg == L"void")
          {
            os << "void " << name << "::" << endl
               << "pre ()"
               << "{";

            if (tiein)
            {
              String const& impl (etiein (e));

              os << "if (this->" << impl << ")" << endl
                 << "this->" << impl << "->pre ();";
            }

            os << "}";
          }

          if (poly_code)
          {
            String id (e.name ());

            if (String ns = xml_ns_name (e))
            {
              id += L' ';
              id += ns;
            }

            os << "const char* " << name << "::" << endl
               << "_static_type ()"
               << "{"
               << "return " << strlit (id) << ";"
               << "}";

            os << "const char* " << name << "::" << endl
               << "_dynamic_type () const"
               << "{"
               << "return _static_type ();"
               << "}";

            if (validation)
            {
              bool gen (!anonymous (e));

              // We normally don't need to enter anonymous types into
              // the inheritance map. The only exception is when an
              // anonymous types is defined inside an element that
              // is a member of a substitution group.
              //
              if (!gen)
              {
                // The first instance that this anonymous type classifies
                // is the prototype for others if any. If this type does
                // not classify anything (e.g., it is a base), then we
                // don't need to do anything.
                //
                if (e.classifies_begin () != e.classifies_end ())
                {
                  SemanticGraph::Instance& i (
                    e.classifies_begin ()->instance ());

                  if (SemanticGraph::Element* e =
                      dynamic_cast<SemanticGraph::Element*> (&i))
                  {
                    if (e->substitutes_p ())
                      gen = true;
                  }
                }
              }

              if (gen)
              {
                os << "static" << endl
                   << "const ::xsde::cxx::serializer::validating::" <<
                  "inheritance_map_entry" << endl
                   << "_xsde_" << name << "_inheritance_map_entry_ (" << endl
                   << name << "::_static_type ()," << endl
                   << fq_name (base) << "::_static_type ());"
                   << endl;
              }
            }
          }

          if (tiein)
          {
            // If our base has pure virtual functions, override them here.
            //
            BaseOverride t (*this, name);
            t.dispatch (base);
          }

          if (enum_facets)
          {
            typedef set<String> Enums;
            Enums enums;

            for (Type::NamesIterator i (e.names_begin ()),
                   end (e.names_end ()); i != end; ++i)
              enums.insert (i->name ());

            os << "const char* const " << name << "::" << "_xsde_" << name <<
              "_enums_[" << enums.size () << "UL] = "
               << "{";

            for (Enums::iterator b (enums.begin ()), i (b), end (enums.end ());
                 i != end; ++i)
              os << (i != b ? ",\n" : "") << strlit (*i);

            os << "};";
          }
        }
      };

      //
      //
      struct List: Traversal::List, Context
      {
        List (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& l)
        {
          String const& name (ename (l));
          SemanticGraph::Type& t (l.argumented ().type ());

          String const& ret (ret_type (t));
          String const& arg (arg_type (t));

          String item (unclash (name, "item"));
          String item_next (unclash (name, "item_next"));
          String inst (L"_xsde_" + item + L"_");

          os << "// " << name << endl
             << "//" << endl
             << endl;

          String impl;

          if (tiein)
            impl = etiein (l);

          // pre
          //
          if (arg_type (l) == L"void")
          {
            os << "void " << name << "::" << endl
               << "pre ()"
               << "{";

            if (tiein)
              os << "if (this->" << impl << ")" << endl
                 << "this->" << impl << "->pre ();";

            os << "}";
          }

          // item
          //
          os << "bool " << name << "::" << endl
             << item_next << " ()"
             << "{";

          if (tiein)
            os << "return this->" << impl << " ? this->" << impl << "->" <<
              item_next << " () : false;";
          else
            os << "return false;";

          os << "}";

          if (ret == L"void")
          {
            os << ret << " " << name << "::" << endl
               << item << " ()"
               << "{";

            if (tiein)
              os << "if (this->" << impl << ")" << endl
                 << "return this->" << impl << "->" << item << " ();";

            os << "}";
          }

          // reset
          //
          if (reset)
          {
            os << "void " << name << "::" << endl
               << "_reset ()"
               << "{"
               << simple_base << "::_reset ();"
               << endl
               << "if (this->" << inst << ")" << endl
               << "this->" << inst << "->_reset ();"
               << "}";
          }

          // serialize_content
          //
          os << "void " << name << "::" << endl
             << "_serialize_content ()"
             << "{";

          os << "bool first = true;"
             << "::xsde::cxx::serializer::context& ctx = this->_context ();"
             << endl;

          if (exceptions && !validation)
          {
            os << "while (this->" << item_next << " ())"
               << "{"
               << "if (this->" << inst << ")"
               << "{";

            if (ret == L"void")
              os << "this->" << item << " ();"
                 << "this->" << inst << "->pre ();";
            else
              os << arg << " r = this->" << item << " ();"
                 << "this->" << inst << "->pre (r);";

            os << endl
               << "if (!first)" << endl
               << "this->_characters (\" \", 1);"
               << "else" << endl
               << "first = false;"
               << endl;

            os << "this->" << inst << "->_pre_impl (ctx);"
               << "this->" << inst << "->_serialize_content ();"
               << "this->" << inst << "->_post_impl ();"
               << "this->" << inst << "->post ();";

            os << "}"
               << "}";
          }
          else
          {
            os << "while (this->" << item_next << " ())"
               << "{"
               << "if (this->" << inst << ")"
               << "{";

            if (ret == L"void")
              os << "this->" << item << " ();";
            else
              os << arg << " r = this->" << item << " ();";

            os << endl
               << "if (ctx.error_type ())" << endl
               << "break;"
               << endl;

            if (ret == L"void")
              os << "this->" << inst << "->pre ();";
            else
              os << "this->" << inst << "->pre (r);";

            if (!exceptions)
            {
              os << endl
                 << "if (this->" << inst << "->_error_type ())" << endl
                 << "this->" << inst << "->_copy_error (ctx);"
                 << endl;

              os << "if (ctx.error_type ())" << endl
                 << "break;";
            }

            if (!exceptions)
              os << endl
                 << "if (!first)"
                 << "{"
                 << "if (!this->_characters (\" \", 1))" << endl
                 << "break;"
                 << "}";
            else
              os << endl
                 << "if (!first)" << endl
                 << "this->_characters (\" \", 1);";

            os << "else" << endl
               << "first = false;"
               << endl;

            os << "this->" << inst << "->_pre_impl (ctx);";

            os << endl
               << "if (ctx.error_type ())" << endl
               << "break;"
               << endl;

            os << "this->" << inst << "->_serialize_content ();";

            os << endl
               << "if (ctx.error_type ())" << endl
               << "break;"
               << endl;

            os << "this->" << inst << "->_post_impl ();";

            os << endl
               << "if (ctx.error_type ())" << endl
               << "break;"
               << endl;

            os << "this->" << inst << "->post ();";

            if (!exceptions)
            {
              os << endl
                 << "if (this->" << inst << "->_error_type ())" << endl
                 << "this->" << inst << "->_copy_error (ctx);"
                 << endl;

              os << "if (ctx.error_type ())" << endl
                 << "break;";
            }

            os << "}" // No check for error here since we return anyway.
               << "}";
          }

          os << "}"; // _serialize_content

          //
          //
          if (poly_code)
          {
            String id (l.name ());

            if (String ns = xml_ns_name (l))
            {
              id += L' ';
              id += ns;
            }

            os << "const char* " << name << "::" << endl
               << "_static_type ()"
               << "{"
               << "return " << strlit (id) << ";"
               << "}";

            os << "const char* " << name << "::" << endl
               << "_dynamic_type () const"
               << "{"
               << "return _static_type ();"
               << "}";
          }
        }
      };

      //
      //
      struct Union: Traversal::Union, Context
      {
        Union (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& u)
        {
          String const& name (ename (u));
          String const& arg (arg_type (u));

          if (arg == L"void" || poly_code)
          {
            os << "// " << name << endl
               << "//" << endl
               << endl;
          }

          if (arg == L"void")
          {
            os << "void " << name << "::" << endl
               << "pre ()"
               << "{";

            if (tiein)
            {
              String const& impl (etiein (u));

              os << "if (this->" << impl << ")" << endl
                 << "this->" << impl << "->pre ();";
            }

            os << "}";
          }

          if (poly_code)
          {
            String id (u.name ());

            if (String ns = xml_ns_name (u))
            {
              id += L' ';
              id += ns;
            }

            os << "const char* " << name << "::" << endl
               << "_static_type ()"
               << "{"
               << "return " << strlit (id) << ";"
               << "}";

            os << "const char* " << name << "::" << endl
               << "_dynamic_type () const"
               << "{"
               << "return _static_type ();"
               << "}";
          }
        }
      };

      //
      //
      struct ParticleReset: Traversal::Element, Context
      {
        ParticleReset (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          String const& m (emember (e));

          os << "if (this->" << m << ")" << endl
             << "this->" << m << "->_reset ();"
             << endl;

          if (poly_code && !anonymous (e.type ()))
          {
            String const& map (emember_map (e));

            os << "if (this->" << map << ")" << endl
               << "this->" << map << "->reset ();"
               << endl;
          }
        }
      };

      struct AttributeReset: Traversal::Attribute, Context
      {
        AttributeReset (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& a)
        {
          String const& m (emember (a));

          os << "if (this->" << m << ")" << endl
             << "this->" << m << "->_reset ();"
             << endl;
        }
      };

      // Complex serialization code.
      //
      struct Compositor: Traversal::All,
                         Traversal::Choice,
                         Traversal::Sequence,
                         Context
      {
        Compositor (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}.
          //
          size_t min (a.min ());

          if (min == 0)
            os << "if (this->" << epresent (a) << " ())"
               << "{";

          Traversal::All::traverse (a);

          if (min == 0)
          {
            os << "}";

            if (!exceptions)
            {
              os << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl;
            }
          }
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.contains_begin () != c.contains_end ())
          {
            size_t min (c.min ()), max (c.max ());

            if (min == 0 && max == 1)
            {
              os << "if (this->" << epresent (c) << " ())"
                 << "{";
            }
            else if (max != 1)
            {
              os << "while (this->" << enext (c) << " ())"
                 << "{";
            }
            else if (!exceptions)
            {
              os << "{";
            }

            if (exceptions)
              os << "switch (this->" << earm (c) << " ())";
            else
              os << earm_tag (c) << " t = this->" << earm (c) << " ();"
                 << endl
                 << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl
                 << "switch (t)";


              os << "{";

            for (SemanticGraph::Choice::ContainsIterator
                   i (c.contains_begin ()); i != c.contains_end (); ++i)
            {
              os << "case " << etag (i->particle ()) << ":"
                 << "{";

              edge_traverser ().dispatch (*i);

              os << "break;"
                 << "}";
            }

            // In case of restriction we may not handle all enumerators
            // in which case default will help avoid warnings.
            //
            os << "default:"
               << "{"
               << "break;"
               << "}"
               << "}"; // switch

            if (min == 0 || max != 1)
            {
              os << "}";

              if (!exceptions)
              {
                os << "if (ctx.error_type ())" << endl
                   << "return;"
                   << endl;
              }
            }
            else if (!exceptions)
            {
              os << "}";
            }
          }
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          size_t min (s.min ()), max (s.max ());

          if (min == 0 && max == 1)
          {
            os << "if (this->" << epresent (s) << " ())"
               << "{";
          }
          else if (max != 1)
          {
            os << "while (this->" << enext (s) << " ())"
               << "{";
          }

          Traversal::Sequence::traverse (s);


          if (min == 0 || max != 1)
          {
            os << "}";

            if (!exceptions)
            {
              os << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl;
            }
          }
        }
      };

      struct Particle: Traversal::Element,
                       Traversal::Any,
                       Context
      {
        Particle (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          size_t min (e.min ()), max (e.max ());

          String const& name (ename (e));

          os << "// " << name << endl
             << "//" << endl;

          if (min == 0 && max == 1)
          {
            os << "if (this->" << epresent (e) << " ())";
          }
          else if (max != 1)
          {
            os << "while (this->" << enext (e) << " ())";
          }

          os << "{";

          String const& ret (ret_type (e.type ()));
          String const& arg (arg_type (e.type ()));
          String fq_type (fq_name (e.type ()));

          bool poly (poly_code && !anonymous (e.type ()));
          String inst (poly ? String (L"s") : L"this->" + emember (e));

          if (poly)
            os << "ctx.type_id (0);";

          if (ret == L"void")
            os << "this->" << name << " ();"
               << endl;
          else
            os << arg << " r = this->" << name << " ();"
               << endl;

          if (!exceptions)
            os << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;

          if (poly)
          {
            // In case of mixin we use virtual inheritance and only
            // dynamic_cast can be used.
            //
            String cast (mixin ? L"dynamic_cast" : L"static_cast");

            os << "const void* t = ctx.type_id ();"
               << fq_type << "* " << inst << " = 0;"
               << endl
               << "if (t == 0 && this->" << emember (e) << " != 0)" << endl
               << inst << " = this->" << emember (e) << ";"
               << "else if (this->" << emember_map (e) << " != 0)" << endl
               << inst << " = " << cast << "< " << fq_type << "* > (" << endl
               << "this->" << emember_map (e) << "->find (t));"
               << endl;
          }

          os << "if (" << inst << ")"
             << "{";

          if (exceptions)
          {
            if (ret == L"void")
              os << inst << "->pre ();";
            else
              os << inst << "->pre (r);";

            if (poly)
            {
              os << endl
                 << "const char* dt = 0;"
                 << "if (t != 0)"
                 << "{"
                 << "dt = " << inst << "->_dynamic_type ();"
                 << "if (strcmp (dt, " << fq_type <<
                "::_static_type ()) == 0)" << endl
                 << "dt = 0;"
                 << "}";
            }

            // Only a globally-defined element can be a subst-group root.
            //
            if (poly && e.global_p ())
            {
              if (e.qualified_p () && e.namespace_ ().name ())
                os << "const char* ns = " <<
                  strlit (e.namespace_ ().name ()) << ";";
              else
                os << "const char* ns = 0;";

              os << "const char* n = " << strlit (e.name ()) << ";"
                 << endl;

              os << "if (dt != 0 && " <<
                 "::xsde::cxx::serializer::substitution_map_instance ()" <<
                ".check (ns, n, dt, " << (ret == L"void" ? "0" : "&r") <<
                "))" << endl
                 << "dt = 0;"
                 << endl;

              os << "if (ns != 0)" << endl
                 << "this->_start_element (ns, n);"
                 << "else" << endl
                 << "this->_start_element (n);";
            }
            else
            {
              if (e.qualified_p () && e.namespace_ ().name ())
                os << "this->_start_element (" <<
                  strlit (e.namespace_ ().name ()) << ", " <<
                  strlit (e.name ()) << ");";
              else
                os << "this->_start_element (" << strlit (e.name ()) << ");";
            }

            if (poly)
            {
              // Set xsi:type if necessary.
              //
              os << endl
                 << "if (dt != 0)" << endl
                 << "this->_set_type (dt);"
                 << endl;
            }

            os << inst << "->_pre_impl (ctx);"
               << inst << "->_serialize_attributes ();"
               << inst << "->_serialize_content ();"
               << inst << "->_post_impl ();"
               << "this->_end_element ();"
               << inst << "->post ();";
          }
          else
          {
            if (ret == L"void")
              os << inst << "->pre ();";
            else
              os << inst << "->pre (r);";

            // Note that after pre() we need to check both the serializer
            // and context error states because of the recursive parsing.
            //
            os << endl
               << "if (" << inst << "->_error_type ())" << endl
               << inst << "->_copy_error (ctx);"
               << endl;

            os << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;

            if (poly)
            {
              os << "const char* dt = 0;"
                 << "if (t != 0)"
                 << "{"
                 << "dt = " << inst << "->_dynamic_type ();"
                 << "if (strcmp (dt, " << fq_type <<
                "::_static_type ()) == 0)" << endl
                 << "dt = 0;"
                 << "}";
            }

            // Only a globally-defined element can be a subst-group root.
            //
            if (poly && e.global_p ())
            {
              if (e.qualified_p () && e.namespace_ ().name ())
                os << "const char* ns = " <<
                  strlit (e.namespace_ ().name ()) << ";";
              else
                os << "const char* ns = 0;";

              os << "const char* n = " << strlit (e.name ()) << ";"
                 << endl;

              os << "if (dt != 0 && " <<
                 "::xsde::cxx::serializer::substitution_map_instance ()" <<
                ".check (ns, n, dt, " << (ret == L"void" ? "0" : "&r") <<
                "))" << endl
                 << "dt = 0;"
                 << endl;

              os << "if (ns != 0)"
                 << "{"
                 << "if (!this->_start_element (ns, n))" << endl
                 << "return;"
                 << "}"
                 << "else"
                 << "{"
                 << "if (!this->_start_element (n))" << endl
                 << "return;"
                 << "}";
            }
            else
            {
              if (e.qualified_p () && e.namespace_ ().name ())
                os << "if (!this->_start_element (" <<
                  strlit (e.namespace_ ().name ()) << ", " <<
                  strlit (e.name ()) << "))" << endl
                   << "return;"
                   << endl;
              else
                os << "if (!this->_start_element (" <<
                  strlit (e.name ()) << "))" << endl
                   << "return;"
                   << endl;
            }

            if (poly)
            {
              // Set xsi:type if necessary.
              //
              os << "if (dt != 0)"
                 << "{"
                 << "if (!this->_set_type (dt))" << endl
                 << "return;"
                 << "}";
            }

            os << inst << "->_pre_impl (ctx);";

            os << endl
               << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;

            os << inst << "->_serialize_attributes ();";

            os << endl
               << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;

            os << inst << "->_serialize_content ();";

            os << endl
               << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;

            os << inst << "->_post_impl ();";

            os << endl
               << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;

            os << "if (!this->_end_element ())" << endl
               << "return;"
               << endl;

            os << inst << "->post ();";

            // Note that after post() we need to check both the serializer
            // and context error states because of the recursive parsing.
            //
            os << endl
               << "if (" << inst << "->_error_type ())" << endl
               << inst << "->_copy_error (ctx);"
               << endl;

            os << "if (ctx.error_type ())" << endl
               << "return;";
          }

          os << "}" // if (inst)
             << "}";

          if ((min == 0 || max != 1) && !exceptions)
          {
            os << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;
          }
        }

        virtual void
        traverse (SemanticGraph::Any& a)
        {
          size_t min (a.min ()), max (a.max ());

          if (min == 0 && max == 1)
          {
            os << "if (this->" << epresent (a) << " ())";

          }
          else if (max != 1)
          {
            os << "while (this->" << enext (a) << " ())";
          }

          os << "{";

          if (stl)
          {
            os << "::std::string ns, name;";

            if (exceptions)
            {
              os << "this->" << ename (a) << " (ns, name);"
                 << endl
                 << "if (ns.empty ())" << endl
                 << "this->_start_element (name.c_str ());"
                 << "else" << endl
                 << "this->_start_element (ns.c_str (), name.c_str ());"
                 << endl
                 << "this->" << eserialize (a) << " ();"
                 << "this->_end_element ();";
            }
            else
            {
              os << "this->" << ename (a) << " (ns, name);"
                 << endl
                 << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl
                 << "if (ns.empty ())"
                 << "{"
                 << "if (!this->_start_element (name.c_str ()))" << endl
                 << "return;"
                 << "}"
                 << "else"
                 << "{"
                 << "if (!this->_start_element (ns.c_str (), " <<
                "name.c_str ()))" << endl
                 << "return;"
                 << "}"
                 << "this->" << eserialize (a) << " ();"
                 << endl
                 << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl
                 << "if (!this->_end_element ())" << endl
                 << "return;";
            }
          }
          else
          {
            os << "const char* ns = 0;"
               << "const char* name;"
               << "bool free;";

            if (exceptions)
            {
              os << "this->" << ename (a) << " (ns, name, free);"
                 << endl
                 << "::xsde::cxx::string auto_ns, auto_name;"
                 << "if (free)"
                 << "{"
                 << "auto_ns.attach (const_cast< char* > (ns));"
                 << "auto_name.attach (const_cast< char* > (name));"
                 << "}"
                 << "if (ns == 0 || *ns == '\\0')" << endl
                 << "this->_start_element (name);"
                 << "else" << endl
                 << "this->_start_element (ns, name);"
                 << endl
                 << "this->" << eserialize (a) << " ();"
                 << "this->_end_element ();";
            }
            else
            {
              os << "this->" << ename (a) << " (ns, name, free);"
                 << endl
                 << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl
                 << "bool r;"
                 << "if (ns == 0 || *ns == '\\0')" << endl
                 << "r = this->_start_element (name);"
                 << "else" << endl
                 << "r = this->_start_element (ns, name);"
                 << endl
                 << "if (free)"
                 << "{";

              if (!custom_alloc)
                os << "delete[] ns;"
                   << "delete[] name;";
              else
                os << "::xsde::cxx::free (const_cast< char* > (ns));"
                   << "::xsde::cxx::free (const_cast< char* > (name));";

              os << "}"
                 << "if (!r)" << endl
                 << "return;"
                 << endl
                 << "this->" << eserialize (a) << " ();"
                 << endl
                 << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl
                 << "if (!this->_end_element ())" << endl
                 << "return;";
            }
          }

          os << "}";

          if (!exceptions && (min == 0 || max != 1))
          {
            os << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;
          }
        }

      };

      struct Attribute: Traversal::Attribute,
                        Traversal::AnyAttribute,
                        Context
      {
        Attribute (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Attribute& a)
        {
          String const& name (ename (a));

          os << "// " << name << endl
             << "//" << endl;

          if (a.optional_p ())
          {
            os << "if (this->" << epresent (a) << " ())";
          }

          os << "{";

          String const& inst (emember (a));
          String const& ret (ret_type (a.type ()));
          String const& arg (arg_type (a.type ()));

          if (ret == L"void")
            os << "this->" << name << " ();"
               << endl;
          else
            os << arg << " r = this->" << name << " ();"
               << endl;

          if (!exceptions)
            os << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;

          os << "if (this->" << inst << ")"
             << "{";

          if (exceptions)
          {
            if (ret == L"void")
              os << "this->" << inst << "->pre ();";
            else
              os << "this->" << inst << "->pre (r);";

            if (a.qualified_p () && a.namespace_ ().name ())
              os << "this->_start_attribute (" <<
                strlit (a.namespace_ ().name ()) << ", " <<
                strlit (a.name ()) << ");";
            else
              os << "this->_start_attribute (" << strlit (a.name ()) << ");";

            os << "this->" << inst << "->_pre_impl (ctx);"
               << "this->" << inst << "->_serialize_content ();"
               << "this->" << inst << "->_post_impl ();"
               << "this->_end_attribute ();"
               << "this->" << inst << "->post ();";
          }
          else
          {
            if (ret == L"void")
              os << "this->" << inst << "->pre ();";
            else
              os << "this->" << inst << "->pre (r);";

            os << endl
               << "if (this->" << inst << "->_error_type ())" << endl
               << "this->" << inst << "->_copy_error (ctx);"
               << endl;

            os << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;

            if (a.qualified_p () && a.namespace_ ().name ())
              os << "if (!this->_start_attribute (" <<
                strlit (a.namespace_ ().name ()) << ", " <<
                strlit (a.name ()) << "))" << endl
                 << "return;"
                 << endl;
            else
              os << "if (!this->_start_attribute (" <<
                strlit (a.name ()) << "))" << endl
                 << "return;"
                 << endl;

            os << "this->" << inst << "->_pre_impl (ctx);";

            os << endl
               << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;

            os << "this->" << inst << "->_serialize_content ();";

            os << endl
               << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;

            os << "this->" << inst << "->_post_impl ();";

            os << endl
               << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;

            os << "if (!this->_end_attribute ())" << endl
               << "return;"
               << endl;

            os << "this->" << inst << "->post ();";

            os << endl
               << "if (this->" << inst << "->_error_type ())" << endl
               << "this->" << inst << "->_copy_error (ctx);"
               << endl;

            os << "if (ctx.error_type ())" << endl
               << "return;";
          }

          os << "}"  // if (inst)
             << "}";

          if (a.optional_p () && !exceptions)
          {
            os << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;
          }
        }

        virtual void
        traverse (SemanticGraph::AnyAttribute& a)
        {
          os << "while (this->" << enext (a) << " ())"
             << "{";

          if (stl)
          {
            os << "::std::string ns, name;";

            if (exceptions)
            {
              os << "this->" << ename (a) << " (ns, name);"
                 << endl
                 << "if (ns.empty ())" << endl
                 << "this->_start_attribute (name.c_str ());"
                 << "else" << endl
                 << "this->_start_attribute (ns.c_str (), name.c_str ());"
                 << endl
                 << "this->" << eserialize (a) << " ();"
                 << "this->_end_attribute ();";
            }
            else
            {
              os << "this->" << ename (a) << " (ns, name);"
                 << endl
                 << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl
                 << "if (ns.empty ())"
                 << "{"
                 << "if (!this->_start_attribute (name.c_str ()))" << endl
                 << "return;"
                 << "}"
                 << "else"
                 << "{"
                 << "if (!this->_start_attribute (ns.c_str (), " <<
                "name.c_str ()))" << endl
                 << "return;"
                 << "}"
                 << "this->" << eserialize (a) << " ();"
                 << endl
                 << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl
                 << "if (!this->_end_attribute ())" << endl
                 << "return;";
            }
          }
          else
          {
            os << "const char* ns = 0;"
               << "const char* name;"
               << "bool free;";

            if (exceptions)
            {
              os << "this->" << ename (a) << " (ns, name, free);"
                 << endl
                 << "::xsde::cxx::string auto_ns, auto_name;"
                 << "if (free)"
                 << "{"
                 << "auto_ns.attach (const_cast< char* > (ns));"
                 << "auto_name.attach (const_cast< char* > (name));"
                 << "}"
                 << "if (ns == 0 || *ns == '\\0')" << endl
                 << "this->_start_attribute (name);"
                 << "else" << endl
                 << "this->_start_attribute (ns, name);"
                 << endl
                 << "this->" << eserialize (a) << " ();"
                 << "this->_end_attribute ();";
            }
            else
            {
              os << "this->" << ename (a) << " (ns, name, free);"
                 << endl
                 << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl
                 << "bool r;"
                 << "if (ns == 0 || *ns == '\\0')" << endl
                 << "r = this->_start_attribute (name);"
                 << "else" << endl
                 << "r = this->_start_attribute (ns, name);"
                 << endl
                 << "if (free)"
                 << "{";

              if (!custom_alloc)
                os << "delete[] ns;"
                   << "delete[] name;";
              else
                os << "::xsde::cxx::free (const_cast< char* > (ns));"
                   << "::xsde::cxx::free (const_cast< char* > (name));";

              os << "}"
                 << "if (!r)" << endl
                 << "return;"
                 << endl
                 << "this->" << eserialize (a) << " ();"
                 << endl
                 << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl
                 << "if (!this->_end_attribute ())" << endl
                 << "return;";
            }
          }

          os << "}";

          if (!exceptions)
          {
            os << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;
          }
        }
      };

      // Callbacks.
      //
      struct CompositorCallback: Traversal::All,
                                 Traversal::Choice,
                                 Traversal::Sequence,
                                 Context
      {
        CompositorCallback (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::All& a)
        {
          if (!a.context ().count ("xsd-frontend-restriction-correspondence"))
          {
            // For the all compositor, maxOccurs=1 and minOccurs={0,1}.
            //
            if (a.min () == 0)
            {
              SemanticGraph::Scope& s (scope (a));
              String const& present (epresent (a));

              os << "bool " << ename (s) << "::" << endl
                 << present << " ()"
                 << "{";

              if (tiein)
              {
                String const& impl (
                  etiein (dynamic_cast<SemanticGraph::Type&> (s)));

                os << "return this->" << impl << " ? this->" << impl <<
                  "->" << present << " () : false;";
              }
              else
                os << "return false;";

              os << "}";
            }
          }

          Traversal::All::traverse (a);
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          if (SemanticGraph::Compositor* b = correspondent (c))
          {
            size_t smin (c.min ());
            size_t bmax (b->max ());

            if (bmax != 1 && smin == 0)
            {
              String const& next (enext (c));
              String const& present (epresent (c));

              SemanticGraph::Scope& scope (this->scope (c));

              os << "bool " << ename (scope) << "::" << endl
                 << present << " ()"
                 << "{";

              if (tiein)
              {
                String const& impl (
                  etiein (dynamic_cast<SemanticGraph::Type&> (scope)));

                os << "return this->" << impl << " ? this->" << impl <<
                  "->" << present << " () : this->" << next << " ();";
              }
              else
                os << "return this->" << next << " ();";

              os << "}";
            }
          }
          else
          {
            size_t min (c.min ()), max (c.max ());

            if (min == 0)
            {
              SemanticGraph::Scope& scope (this->scope (c));
              String const& s (ename (scope));

              String impl;

              if (tiein)
                impl = etiein (dynamic_cast<SemanticGraph::Type&> (scope));

              if (max == 1)
              {
                String const& present (epresent (c));

                os << "bool " << s << "::" << endl
                   << present << " ()"
                   << "{";

                if (tiein)
                  os << "return this->" << impl << " ? this->" << impl <<
                    "->" << present << " () : false;";
                else
                  os << "return false;";

                os << "}";
              }
              else
              {
                String const& next (enext (c));

                os << "bool " << s << "::" << endl
                   << next << " ()"
                   << "{";

                if (tiein)
                  os << "return this->" << impl << " ? this->" << impl <<
                    "->" << next << " () : false;";
                else
                  os << "return false;";

                os << "}";
              }

              String const& arm (earm (c));
              String const& tag (etag (c.contains_begin ()->particle ()));

              os << s << "::" << earm_tag (c) << " " << s << "::" << endl
                 << arm << " ()"
                 << "{";

              if (tiein)
                os << "return this->" << impl << " ? this->" << impl <<
                  "->" << arm << " () : " << tag << ";";
              else
                os << "return " << tag << ";";

              os << "}";
            }
          }

          Traversal::Choice::traverse (c);
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          if (SemanticGraph::Compositor* b = correspondent (s))
          {
            size_t smin (s.min ());
            size_t bmax (b->max ());

            if (bmax != 1 && smin == 0)
            {
              String const& next (enext (s));
              String const& present (epresent (s));

              SemanticGraph::Scope& scope (this->scope (s));

              os << "bool " << ename (scope) << "::" << endl
                 << present << " ()"
                 << "{";

              if (tiein)
              {
                String const& impl (
                  etiein (dynamic_cast<SemanticGraph::Type&> (scope)));

                os << "return this->" << impl << " ? this->" << impl <<
                  "->" << present << " () : this->" << next << " ();";
              }
              else
                os << "return this->" << next << " ();";

              os << "}";
            }
          }
          else
          {
            size_t min (s.min ()), max (s.max ());

            if (min == 0)
            {
              SemanticGraph::Scope& scope (this->scope (s));

              String impl;

              if (tiein)
                impl = etiein (dynamic_cast<SemanticGraph::Type&> (scope));

              if (max == 1)
              {
                String const& present (epresent (s));

                os << "bool " << ename (scope) << "::" << endl
                   << present << " ()"
                   << "{";

                if (tiein)
                  os << "return this->" << impl << " ? this->" << impl <<
                    "->" << present << " () : false;";
                else
                  os << "return false;";

                os << "}";
              }
              else
              {
                String const& next (enext (s));

                os << "bool " << ename (scope) << "::" << endl
                   << next << " ()"
                   << "{";

                if (tiein)
                  os << "return this->" << impl << " ? this->" << impl <<
                    "->" << next << " () : false;";
                else
                  os << "return false;";

                os << "}";
              }
            }
          }

          Traversal::Sequence::traverse (s);
        }

      private:
        SemanticGraph::Scope&
        scope (SemanticGraph::Compositor& c)
        {
          SemanticGraph::Compositor* root (&c);

          while (root->contained_particle_p ())
            root = &root->contained_particle ().compositor ();

          return dynamic_cast<SemanticGraph::Scope&> (
            root->contained_compositor ().container ());
        }
      };

      struct ParticleCallback: Traversal::Element,
                               Traversal::Any,
                               Context
      {
        ParticleCallback (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          if (SemanticGraph::Element* b = correspondent (e))
          {
            if (b->max () != 1 && e.min () == 0)
            {
              String const& next (enext (e));
              String const& present (epresent (e));

              os << "bool " << ename (e.scope ()) << "::" << endl
                 << present << " ()"
                 << "{";

              if (tiein)
              {
                String const& impl (
                  etiein (dynamic_cast<SemanticGraph::Type&> (e.scope ())));

                os << "return this->" << impl << " ? this->" << impl <<
                  "->" << present << " () : this->" << next << " ();";
              }
              else
                os << "return this->" << next << " ();";

              os << "}";
            }
          }
          else
          {
            size_t min (e.min ()), max (e.max ());
            String const& s (ename (e.scope ()));

            String impl;

            if (tiein)
              impl = etiein (dynamic_cast<SemanticGraph::Type&> (e.scope ()));

            if (min == 0)
            {
              if (max == 1)
              {
                String const& present (epresent (e));

                os << "bool " << s << "::" << endl
                   << present << " ()"
                   << "{";

                if (tiein)
                  os << "return this->" << impl << " ? this->" << impl <<
                    "->" << present << " () : false;";
                else
                  os << "return false;";

                os << "}";
              }
              else
              {
                String const& next (enext (e));

                os << "bool " << s << "::" << endl
                   << next << " ()"
                   << "{";

                if (tiein)
                  os << "return this->" << impl << " ? this->" << impl <<
                    "->" << next << " () : false;";
                else
                  os << "return false;";

                os << "}";
              }
            }

            // The callback is non-pure-virtual only if the return type
            // is void.
            //
            if (ret_type (e.type ()) == L"void")
            {
              String const& name (ename (e));

              os << "void " << s << "::" << endl
                 << name << " ()"
                 << "{";

              if (tiein)
                os << "if (this->" << impl << ")" << endl
                   << "this->" << impl << "->" << name << " ();";

              os << "}";
            }
          }
        }

        virtual void
        traverse (SemanticGraph::Any& a)
        {
          if (SemanticGraph::Any* b = correspondent (a))
          {
            if (b->max () != 1 && a.min () == 0)
            {
              String const& next (enext (a));
              String const& present (epresent (a));

              os << "bool " << ename (a.scope ()) << "::" << endl
                 << present << " ()"
                 << "{";

              if (tiein)
              {
                String const& impl (
                  etiein (dynamic_cast<SemanticGraph::Type&> (a.scope ())));

                os << "return this->" << impl << " ? this->" << impl <<
                  "->" << present << " () : this->" << next << " ();";
              }
              else
                os << "return this->" << next << " ();";

              os << "}";
            }
          }
          else
          {
            size_t min (a.min ()), max (a.max ());

            if (min == 0 ||
                a.contained_particle ().compositor ().is_a<
                  SemanticGraph::Choice> ())
            {
              String const& s (ename (a.scope ()));
              String impl;

              if (tiein)
                impl = etiein (dynamic_cast<SemanticGraph::Type&> (a.scope ()));

              if (min == 0 && max == 1)
              {
                String const& present (epresent (a));

                os << "bool " << s << "::" << endl
                   << present << " ()"
                   << "{";

                if (tiein)
                  os << "return this->" << impl << " ? this->" << impl <<
                    "->" << present << " () : false;";
                else
                  os << "return false;";

                os << "}";
              }
              else if (max != 1)
              {
                String const& next (enext (a));

                os << "bool " << s << "::" << endl
                   << next << " ()"
                   << "{";

                if (tiein)
                  os << "return this->" << impl << " ? this->" << impl <<
                    "->" << next << " () : false;";
                else
                  os << "return false;";

                os << "}";
              }

              String const& name (ename (a));

              if (stl)
              {
                if (tiein)
                  os << "void " << s << "::" << endl
                     << name << " (::std::string& ns, ::std::string& n)"
                     << "{"
                     << "if (this->" << impl << ")" << endl
                     << "this->" << impl << "->" << name << " (ns, n);"
                     << "}";

                else
                  os << "void " << s << "::" << endl
                     << name << " (::std::string&, ::std::string&)"
                     << "{"
                     << "}";
              }
              else
              {
                if (tiein)
                  os << "void " << s << "::" << endl
                     << name << " (const char*& ns, const char*& n, bool& f)"
                     << "{"
                     << "if (this->" << impl << ")" << endl
                     << "this->" << impl << "->" << name << " (ns, n, f);"
                     << "}";
                else
                  os << "void " << s << "::" << endl
                     << name << " (const char*&, const char*&, bool&)"
                     << "{"
                     << "}";
              }

              String const& serialize (eserialize (a));

              os << "void " << s << "::" << endl
                 << serialize << " ()"
                 << "{";

              if (tiein)
                os << "if (this->" << impl << ")" << endl
                   << "this->" << impl << "->" << serialize << " ();";

              os << "}";
            }
          }

        }
      };

      struct AttributeCallback: Traversal::Attribute,
                                Traversal::AnyAttribute,
                                Context
      {
        AttributeCallback (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Attribute& a)
        {
          String const& s (ename (a.scope ()));

          String impl;

          if (tiein)
            impl = etiein (dynamic_cast<SemanticGraph::Type&> (a.scope ()));

          if (a.optional_p ())
          {
            String const& present (epresent (a));

            os << "bool " << s << "::" << endl
               << present << " ()"
               << "{";

            if (tiein)
              os << "return this->" << impl << " ? this->" << impl <<
                "->" << present << " () : false;";
            else
              os << "return false;";

            os << "}";
          }

          // The callback is non-pure-virtual only if the return type
          // is void.
          //
          if (ret_type (a.type ()) == L"void")
          {
            String const& name (ename (a));

            os << "void " << s << "::" << endl
               << name << " ()"
               << "{";

            if (tiein)
              os << "if (this->" << impl << ")" << endl
                 << "this->" << impl << "->" << name << " ();";

            os << "}";
          }
        }

        virtual void
        traverse (SemanticGraph::AnyAttribute& a)
        {
          String const& s (ename (a.scope ()));

          String impl;

          if (tiein)
            impl = etiein (dynamic_cast<SemanticGraph::Type&> (a.scope ()));

          String const& next (enext (a));

          os << "bool " << s << "::" << endl
             << next << " ()"
             << "{";

          if (tiein)
            os << "return this->" << impl << " ? this->" << impl <<
              "->" << next << " () : false;";
          else
            os << "return false;";

          os << "}";

          String const& name (ename (a));

          if (stl)
          {
            if (tiein)
              os << "void " << s << "::" << endl
                 << name << " (::std::string& ns, ::std::string& n)"
                 << "{"
                 << "if (this->" << impl << ")" << endl
                 << "this->" << impl << "->" << name << " (ns, n);"
                 << "}";

            else
              os << "void " << s << "::" << endl
                 << name << " (::std::string&, ::std::string&)"
                 << "{"
                 << "}";
          }
          else
          {
            if (tiein)
              os << "void " << s << "::" << endl
                 << name << " (const char*& ns, const char*& n, bool& f)"
                 << "{"
                 << "if (this->" << impl << ")" << endl
                 << "this->" << impl << "->" << name << " (ns, n, f);"
                 << "}";
            else
              os << "void " << s << "::" << endl
                 << name << " (const char*&, const char*&, bool&)"
                 << "{"
                 << "}";
          }

          String const& serialize (eserialize (a));

          os << "void " << s << "::" << endl
             << serialize << " ()"
             << "{";

          if (tiein)
            os << "if (this->" << impl << ")" << endl
               << "this->" << impl << "->" << serialize << " ();";

          os << "}";
        }
      };

      //
      //
      struct Complex : Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c),
              compositor_callback_ (c),
              particle_callback_ (c),
              attribute_callback_ (c),
              particle_reset_ (c),
              attribute_reset_ (c),
              compositor_ (c),
              particle_ (c),
              attribute_ (c)
        {
          // Callback.
          //
          contains_compositor_callback_ >> compositor_callback_;
          compositor_callback_ >> contains_particle_callback_;
          contains_particle_callback_ >> compositor_callback_;
          contains_particle_callback_ >> particle_callback_;

          names_attribute_callback_ >> attribute_callback_;

          // Reset.
          //
          contains_compositor_reset_ >> compositor_reset_;
          compositor_reset_ >> contains_particle_reset_;
          contains_particle_reset_ >> compositor_reset_;
          contains_particle_reset_ >> particle_reset_;

          names_attribute_reset_ >> attribute_reset_;

          // Serialization code.
          //
          contains_compositor_ >> compositor_;
          compositor_ >> contains_particle_;
          contains_particle_ >> compositor_;
          contains_particle_ >> particle_;

          names_attribute_ >> attribute_;
        }

        virtual void
        traverse (Type& c)
        {
          bool hb (c.inherits_p ());
          bool he (has<Traversal::Element> (c));
          bool ha (has<Traversal::Attribute> (c));

          bool hae (has_particle<Traversal::Any> (c));
          bool haa (has<Traversal::AnyAttribute> (c));

          String const& arg (arg_type (c));
          bool same (hb && arg == arg_type (c.inherits ().base ()));

          String const& name (ename (c));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          // pre
          //
          if (!same && arg == L"void")
          {
            os << "void " << name << "::" << endl
               << "pre ()"
               << "{";

            if (tiein)
            {
              String const& impl (etiein (c));

              os << "if (this->" << impl << ")" << endl
                 << "this->" << impl << "->pre ();";
            }

            os << "}";
          }

          // Member callbacks.
          //
          if (!restriction_p (c))
          {
            if (ha || haa)
              names (c, names_attribute_callback_);
          }

          if (he || hae)
            contains_compositor (c, contains_compositor_callback_);

          // reset
          //
          if (!restriction_p (c) && (he || ha) && reset)
          {
            os << "void " << name << "::" << endl
               << "_reset ()"
               << "{";

            // Avoid recursion in case of recursive parsing.
            //
            if (he)
              os << "if (this->resetting_)" << endl
                 << "return;"
                 << endl;

            // Reset the base. We cannot use the fully-qualified base name
            // directly because of some broken compilers (EVC 4.0).
            //
            String base (unclash (name, "base"));

            os << "typedef ";

            if (hb)
              os << fq_name (c.inherits ().base ());
            else
              os << complex_base;

            os << " " << base << ";"
               << base << "::_reset ();"
               << endl;

            // Reset member serializer.
            //

            if (ha)
              names (c, names_attribute_reset_);

            if (he)
            {
              os << "this->resetting_ = true;"
                 << endl;

              contains_compositor (c, contains_compositor_reset_);

              os << "this->resetting_ = false;"
                 << endl;
            }

            os << "}";
          }

          //
          //
          if (poly_code)
          {
            String id (c.name ());

            if (String ns = xml_ns_name (c))
            {
              id += L' ';
              id += ns;
            }

            os << "const char* " << name << "::" << endl
               << "_static_type ()"
               << "{"
               << "return " << strlit (id) << ";"
               << "}";

            os << "const char* " << name << "::" << endl
               << "_dynamic_type () const"
               << "{"
               << "return _static_type ();"
               << "}";

            if (hb && validation)
            {
              bool gen (!anonymous (c));

              // We normally don't need to enter anonymous types into
              // the inheritance map. The only exception is when an
              // anonymous types is defined inside an element that
              // is a member of a substitution group.
              //
              if (!gen)
              {
                // The first instance that this anonymous type classifies
                // is the prototype for others if any. If this type does
                // not classify anything (e.g., it is a base), then we
                // don't need to do anything.
                //
                if (c.classifies_begin () != c.classifies_end ())
                {
                  SemanticGraph::Instance& i (
                    c.classifies_begin ()->instance ());

                  if (SemanticGraph::Element* e =
                      dynamic_cast<SemanticGraph::Element*> (&i))
                  {
                    if (e->substitutes_p ())
                      gen = true;
                  }
                }
              }

              if (gen)
              {
                SemanticGraph::Type& base (c.inherits ().base ());

                os << "static" << endl
                   << "const ::xsde::cxx::serializer::validating::" <<
                  "inheritance_map_entry" << endl
                   << "_xsde_" << name << "_inheritance_map_entry_ (" << endl
                   << name << "::_static_type ()," << endl
                   << fq_name (base) << "::_static_type ());"
                   << endl;
              }
            }
          }

          if (tiein && hb)
          {
            // If our base has pure virtual functions, override them here.
            //
            BaseOverride t (*this, name);
            t.dispatch (c.inherits ().base ());
          }

          // If we are validating, the rest is generated elsewere.
          //
          if (validation)
            return;

          // Don't use restriction_p here since we don't want special
          // treatment of anyType.
          //
          bool restriction (
            hb && c.inherits ().is_a<SemanticGraph::Restricts> ());

          // _serialize_attributes
          //
          if (ha || haa)
          {
            os << "void " << name << "::" << endl
               << "_serialize_attributes ()"
               << "{";

            // We need context for wildcards only if we are using error
            // codes.
            //
            if (ha || !exceptions)
              os << "::xsde::cxx::serializer::context& ctx = this->_context ();"
                 << endl;

            if (hb && !restriction)
            {
              // We cannot use the fully-qualified base name directly
              // because of some broken compilers (EVC 4.0).
              //
              String base (unclash (name, "base"));

              os << "typedef " << fq_name (c.inherits ().base ()) << " " <<
                base << ";"
                 << base << "::_serialize_attributes ();"
                 << endl;

              if (!exceptions)
              {
                os << "if (ctx.error_type ())" << endl
                   << "return;"
                   << endl;
              }
            }

            names (c, names_attribute_);

            os << "}";
          }

          // _serialize_content
          //
          if (he || hae)
          {
            os << "void " << name << "::" << endl
               << "_serialize_content ()"
               << "{";

            // We need context for wildcards only if we are using error
            // codes.
            //
            if (he || !exceptions)
              os << "::xsde::cxx::serializer::context& ctx = this->_context ();"
                 << endl;

            if (hb && !restriction)
            {
              // We cannot use the fully-qualified base name directly
              // because of some broken compilers (EVC 4.0).
              //
              String base (unclash (name, "base"));

              os << "typedef " << fq_name (c.inherits ().base ()) << " " <<
                base << ";"
                 << base << "::_serialize_content ();"
                 << endl;

              if (!exceptions)
              {
                os << "if (ctx.error_type ())" << endl
                   << "return;"
                   << endl;
              }
            }

            contains_compositor (c, contains_compositor_);

            os << "}";
          }
        }

      private:
        //
        //
        CompositorCallback compositor_callback_;
        ParticleCallback particle_callback_;
        Traversal::ContainsCompositor contains_compositor_callback_;
        Traversal::ContainsParticle contains_particle_callback_;

        AttributeCallback attribute_callback_;
        Traversal::Names names_attribute_callback_;

        //
        //
        Traversal::Compositor compositor_reset_;
        ParticleReset particle_reset_;
        Traversal::ContainsCompositor contains_compositor_reset_;
        Traversal::ContainsParticle contains_particle_reset_;

        AttributeReset attribute_reset_;
        Traversal::Names names_attribute_reset_;

        //
        //
        Compositor compositor_;
        Particle particle_;
        Traversal::ContainsCompositor contains_compositor_;
        Traversal::ContainsParticle contains_particle_;

        Attribute attribute_;
        Traversal::Names names_attribute_;
      };


      // Generate substitution group map entries.
      //
      struct GlobalElement: Traversal::Element, Context
      {
        GlobalElement (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& e)
        {
          if (e.substitutes_p ())
          {
            String name (escape (e.name ()));
            Type& r (e.substitutes ().root ());

            SemanticGraph::Type& type (e.type ());

            String r_id (r.name ());

            if (String const& ns = r.namespace_ ().name ())
            {
              r_id += L' ';
              r_id += ns;
            }

            os << "// Substitution map entry for " << comment (e.name ()) <<
              "." << endl
               << "//" << endl
               << "static" << endl
               << "const ::xsde::cxx::serializer::substitution_map_entry" << endl
               << "_xsde_" << name << "_substitution_map_entry_ (" << endl
               << strlit (r_id) << "," << endl;

            if (String const& ns = e.namespace_ ().name ())
              os << strlit (ns) << "," << endl;
            else
              os << "0," << endl;

            os << strlit (e.name ()) << "," << endl
               << fq_name (type) << "::_static_type ());"
               << endl;
          }
        }
      };
    }

    void
    generate_serializer_source (Context& ctx)
    {
      if (ctx.tiein)
        ctx.os << "#include <assert.h>" << endl
               << endl;

      if (ctx.poly_code)
      {
        ctx.os << "#include <string.h>" << endl
               << "#include <xsde/cxx/serializer/substitution-map.hxx>" << endl;

        if (ctx.validation)
          ctx.os << "#include <xsde/cxx/serializer/validating/inheritance-map.hxx>" << endl
                 << endl;
        else
          ctx.os << endl;

        ctx.os << "static" << endl
               << "const ::xsde::cxx::serializer::substitution_map_init" << endl
               << "_xsde_substitution_map_init_;"
               << endl;

        if (ctx.validation)
        {
          ctx.os << "static" << endl
                 << "const ::xsde::cxx::serializer::validating::" <<
            "inheritance_map_init" << endl
                 << "_xsde_inheritance_map_init_;"
                 << endl;
        }
      }

      // Emit "weak" header includes that are used in the file-per-type
      // compilation model.
      //
      if (ctx.options.generate_inline ())
      {
        Traversal::Schema schema;
        Includes includes (ctx, Includes::source);

        schema >> includes;
        schema.dispatch (ctx.schema_root);
      }

      Traversal::Schema schema;

      Sources sources;
      Traversal::Names schema_names;

      Namespace ns (ctx);
      Traversal::Names names;

      schema >> sources >> schema;
      schema >> schema_names >> ns >> names;

      List list (ctx);
      Union union_ (ctx);
      Complex complex (ctx);
      Enumeration enumeration (ctx);
      GlobalElement global_element (ctx);

      names >> list;
      names >> union_;
      names >> complex;
      names >> enumeration;

      if (ctx.poly_code)
        names >> global_element;

      schema.dispatch (ctx.schema_root);
    }
  }
}
