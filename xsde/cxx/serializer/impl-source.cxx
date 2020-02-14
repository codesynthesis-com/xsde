// file      : xsde/cxx/serializer/impl-source.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/serializer/impl-source.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

namespace CXX
{
  namespace Serializer
  {
    namespace
    {
      struct Enumeration: Traversal::Enumeration, Context
      {
        Enumeration (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& e)
        {
          String const& name (eimpl (e));
          String const& arg (arg_type (e));
          SemanticGraph::Type& base (e.inherits ().base ());

          os << "// " << name << endl
             << "//" << endl
             << endl;

          // c-tor
          //
          if (tiein)
            os << name << "::" << endl
               << name << " ()" << endl
               << ": " << ename (e) << " (&base_impl_)"
               << "{"
               << "}";

          // pre
          //
          os << "void " << name << "::" << endl;

          if (arg == L"void")
            os << "pre ()";
          else
            os << "pre (" << arg << " v)";

          os << "{";

          if (arg == arg_type (base))
          {
            if (tiein)
            {
              os << "base_impl_.pre (" <<
                (arg != L"void" ? "v" : "") << ");";
            }
            else if (mixin)
            {
              os << eimpl (base) << "::pre (" <<
                (arg != L"void" ? "v" : "") << ");";
            }
          }
          else
          {
            if (tiein)
            {
              os << "// TODO: call base_impl_.pre ()." << endl
                 << "//" << endl;
            }
            else if (mixin)
            {
              os << "// TODO: call " << eimpl (base) << "::pre ()." << endl
                 << "//" << endl;
            }
          }

          os << "}";

          // post
          //
          os << "void " << name << "::" << endl
             << "post ()"
             << "{"
             << "}";
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
          String const& name (eimpl (l));
          SemanticGraph::Type& t (l.argumented ().type ());

          String const& arg (arg_type (l));
          String const& ret (ret_type (t));

          String const& skel (ename (l));
          String item (unclash (skel, "item"));
          String item_next (unclash (skel, "item_next"));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          // pre
          //
          os << "void " << name << "::" << endl;

          if (arg == L"void")
            os << "pre ()"
               << "{"
               << "}";
          else
            os << "pre (" << arg << " v)"
               << "{"
               << "// TODO" << endl
               << "//" << endl
               << "}";

          // item
          //
          os << "bool " << name << "::" << endl
             << item_next << " ()"
             << "{"
             << "// TODO: return true if there is another item to" << endl
             << "// serialize." << endl
             << "//" << endl
             << "}";


          os << ret << " " << name << "::" << endl
             << item << " ()"
             << "{"
             << "// TODO: return the next item." << endl
             << "//" << endl
             << "}";

          // post
          //
          os << "void " << name << "::" << endl
             << "post ()"
             << "{"
             << "}";
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
          String const& name (eimpl (u));
          String const& arg (arg_type (u));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          // pre
          //
          os << "void " << name << "::" << endl;

          if (arg == L"void")
            os << "pre ()"
               << "{"
               << "}";
          else
            os << "pre (" << arg << " v)"
               << "{"
               << "// TODO" << endl
               << "//" << endl
               << "}";

          // _serialize_content
          //
          os << "void " << name << "::" << endl
             << "_serialize_content ()"
             << "{"
             << "// TODO: call the _characters() function to serialize" << endl
             << "// text content." << endl
             << "//" << endl
             << "}";

          // post
          //
          os << "void " << name << "::" << endl
             << "post ()"
             << "{"
             << "}";
        }
      };

      //
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
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}.
          //
          if (a.min () == 0)
          {
            String const& s (eimpl (scope (a)));

            os << "bool " << s << "::" << endl
               << epresent (a) << " ()"
               << "{"
               << "// TODO: return true if the content corresponding" << endl
               << "// to the all compositor is present." << endl
               << "//" << endl
               << "}";
          }

          Traversal::All::traverse (a);
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.contains_begin () != c.contains_end ())
          {
            size_t min (c.min ()), max (c.max ());

            SemanticGraph::Complex& t (scope (c));
            String const& s (eimpl (t));

            if (min == 0 && max == 1)
            {
              os << "bool " << s << "::" << endl
                 << epresent (c) << " ()"
                 << "{"
                 << "// TODO: return true if the content corresponding" << endl
                 << "// to the choice compositor is present." << endl
                 << "//" << endl
                 << "}";
            }
            else if (max != 1)
            {
              os << "bool " << s << "::" << endl
                 << enext (c) << " ()"
                 << "{"
                 << "// TODO: return true if there is another choice" << endl
                 << "// item to serialize." << endl
                 << "//" << endl
                 << "}";
            }

            os << ename (t) << "::" << earm_tag (c) << " " << s << "::" << endl
               << earm (c) << " ()"
               << "{"
               << "// TODO: return the choice arm that is being" << endl
               << "// serialized." << endl
               << "//" << endl
               << "}";

            Traversal::Choice::traverse (c);
          }
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          size_t min (s.min ()), max (s.max ());

          String const& sc (eimpl (scope (s)));

          if (min == 0 && max == 1)
          {
            os << "bool " << sc << "::" << endl
               << epresent (s) << " ()"
               << "{"
               << "// TODO: return true if the content corresponding" << endl
               << "// to the sequence compositor is present." << endl
               << "//" << endl
               << "}";

          }
          else if (max != 1)
          {
            os << "bool " << sc << "::" << endl
               << enext (s) << " ()"
               << "{"
               << "// TODO: return true if there is another sequence" << endl
               << "// item to serialize." << endl
               << "//" << endl
               << "}";
          }

          Traversal::Sequence::traverse (s);
        }

      private:
        SemanticGraph::Complex&
        scope (SemanticGraph::Compositor& c)
        {
          SemanticGraph::Compositor* root (&c);

          while (root->contained_particle_p ())
            root = &root->contained_particle ().compositor ();

          return dynamic_cast<SemanticGraph::Complex&> (
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
          size_t min (e.min ()), max (e.max ());

          String const& s (
            eimpl (dynamic_cast<SemanticGraph::Complex&> (e.scope ())));

          if (min == 0 && max == 1)
          {
            os << "bool " << s << "::" << endl
               << epresent (e) << " ()"
               << "{"
               << "// TODO: return true if the element is present." << endl
               << "//" << endl
               << "}";

          }
          else if (max != 1)
          {
            os << "bool " << s << "::" << endl
               << enext (e) << " ()"
               << "{"
               << "// TODO: return true if there is another element" << endl
               << "// to serialize." << endl
               << "//" << endl
               << "}";
          }

          String const& ret (ret_type (e.type ()));

          os << ret << " " << s << "::" << endl
             << ename (e) << " ()"
             << "{"
             << "// TODO: return the element data." << endl
             << "//" << endl
             << "}";
        }

        virtual void
        traverse (SemanticGraph::Any& a)
        {
          size_t min (a.min ()), max (a.max ());

          String const& s (
            eimpl (dynamic_cast<SemanticGraph::Complex&> (a.scope ())));

          if (min == 0 && max == 1)
          {
            os << "bool " << s << "::" << endl
               << epresent (a) << " ()"
               << "{"
               << "// TODO: return true if the wildcard content is" << endl
               << "// present." << endl
               << "//" << endl
               << "}";
          }
          else if (max != 1)
          {
            os << "bool " << s << "::" << endl
               << enext (a) << " ()"
               << "{"
               << "// TODO: return true if there is another wildcard" << endl
               << "// element to serialize." << endl
               << "//" << endl
               << "}";
          }

          if (stl)
          {
            os << "void " << s << "::" << endl
               << ename (a) << " (::std::string& ns, ::std::string& name)";
          }
          else
          {
            os << "void " << s << "::" << endl
               << ename (a) << " (const char*& ns, const char*& name, " <<
              "bool& free)";
          }

          os << "{"
             << "// TODO: return the name and namespace of the element" << endl
             << "// corresponding to the wildcard." << endl
             << "//" << endl
             << "}";

          os << "void " << s << "::" << endl
             << eserialize (a) << " ()"
             << "{"
             << "// TODO: use the _start_element(), _end_element()," << endl
             << "// _attribute(), and _characters() functions to" << endl
             << "// serialize the wildcard content." << endl
             << "//" << endl
             << "}";

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
          String const& s (
            eimpl (dynamic_cast<SemanticGraph::Complex&> (a.scope ())));

          if (a.optional_p ())
          {
            os << "bool " << s << "::" << endl
               << epresent (a) << " ()"
               << "{"
               << "// TODO: return true if the attribute is present." << endl
               << "//" << endl
               << "}";
          }

          String const& ret (ret_type (a.type ()));

          os << ret << " " << s << "::" << endl
             << ename (a) << " ()"
             << "{"
             << "// TODO: return the attribute data." << endl
             << "//" << endl
             << "}";
        }

        virtual void
        traverse (SemanticGraph::AnyAttribute& a)
        {
          String const& s (
            eimpl (dynamic_cast<SemanticGraph::Complex&> (a.scope ())));

          os << "bool " << s << "::" << endl
             << enext (a) << " ()"
             << "{"
             << "// TODO: return true if there is another wildcard" << endl
             << "// attribute to serialize." << endl
             << "//" << endl
             << "}";

          if (stl)
          {
            os << "void " << s << "::" << endl
               << ename (a) << " (::std::string& ns, ::std::string& name)";
          }
          else
          {
            os << "void " << s << "::" << endl
               << ename (a) << " (const char*& ns, const char*& name, " <<
              "bool& free)";
          }

          os << "{"
             << "// TODO: return the name and namespace of the attribute" << endl
             << "// corresponding to the wildcard." << endl
             << "//" << endl
             << "}";

          os << "void " << s << "::" << endl
             << eserialize (a) << " ()"
             << "{"
             << "// TODO: use the _characters() function to serialize" << endl
             << "// the wildcard content." << endl
             << "//" << endl
             << "}";
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
              attribute_callback_ (c)
        {
          contains_compositor_callback_ >> compositor_callback_;
          compositor_callback_ >> contains_particle_callback_;
          contains_particle_callback_ >> compositor_callback_;
          contains_particle_callback_ >> particle_callback_;

          names_attribute_callback_ >> attribute_callback_;
        }

        virtual void
        traverse (Type& c)
        {
          bool hb (c.inherits_p ());

          String const& name (eimpl (c));
          String const& arg (arg_type (c));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          // c-tor
          //
          if (tiein && hb)
            os << name << "::" << endl
               << name << " ()" << endl
               << ": " << ename (c) << " (&base_impl_)"
               << "{"
               << "}";

          // pre
          //
          os << "void " << name << "::" << endl;

          if (arg == L"void")
            os << "pre ()";
          else
            os << "pre (" << arg << " v)";

          os << "{";

          if (hb)
          {
            SemanticGraph::Type& base (c.inherits ().base ());

            if (arg == arg_type (base))
            {
              if (tiein)
              {
                os << "base_impl_.pre (" <<
                  (arg != L"void" ? "v" : "") << ");";
              }
              else if (mixin)
              {
                os << eimpl (base) << "::pre (" <<
                  (arg != L"void" ? "v" : "") << ");";
              }
            }
            else
            {
              if (tiein)
              {
                os << "// TODO: call " << eimpl (base) << "::pre ()." << endl
                   << "//" << endl;
              }
              else if (mixin)
              {
                os << "// TODO: call base_impl_.pre ()." << endl
                   << "//" << endl;
              }
            }

          }
          else if (arg != L"void")
          {
            os << "// TODO" << endl
               << "//" << endl;
          }

          os << "}";

          // Member callbacks.
          //
          if (!restriction_p (c))
          {
            names (c, names_attribute_callback_);
            contains_compositor (c, contains_compositor_callback_);
          }

          // post
          //
          os << "void " << name << "::" << endl
             << "post ()"
             << "{"
             << "}";
        }

      private:
        CompositorCallback compositor_callback_;
        ParticleCallback particle_callback_;
        Traversal::ContainsCompositor contains_compositor_callback_;
        Traversal::ContainsParticle contains_particle_callback_;

        AttributeCallback attribute_callback_;
        Traversal::Names names_attribute_callback_;
      };
    }

    void
    generate_impl_source (Context& ctx)
    {
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

      names >> list;
      names >> union_;
      names >> complex;
      names >> enumeration;

      schema.dispatch (ctx.schema_root);
    }
  }
}
