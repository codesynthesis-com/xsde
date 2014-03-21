// file      : xsde/cxx/serializer/impl-header.cxx
// copyright : Copyright (c) 2005-2014 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/serializer/impl-header.hxx>

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

          os << "class " << name << ": " << "public " <<
            (mixin ? "virtual " : "") << ename (e);

          if (mixin)
            os << "," << endl
               << "  public " << fq_name (base, "s:impl");

          os << "{"
             << "public:" << endl;

          if (tiein)
            os << name << " ();"
               << endl;

          os << "virtual void" << endl;

          if (arg == L"void")
            os << "pre ();";
          else
            os << "pre (" << arg << ");";

          os << endl
             << "virtual void" << endl
             << "post ();";

          if (tiein)
            os << endl
               << "private:" << endl
               << fq_name (base, "s:impl") << " base_impl_;";

          os << "};";
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

          os << "class " << name << ": public " <<
            (mixin ? "virtual " : "") << ename (l)
             << "{"
             << "public:" << endl
             << "virtual void" << endl;

          if (arg == L"void")
            os << "pre ();";
          else
            os << "pre (" << arg << ");";

          os << endl
             << "virtual bool" << endl
             << item_next << " ();"
             << endl
             << "virtual " << ret << endl
             << item << " ();"
             << endl
             << "virtual void" << endl
             << "post ();"
             << "};";
        }
      };

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

          os << "class " << name << ": public " <<
            (mixin ? "virtual " : "") << ename (u)
             << "{"
             << "public:" << endl
             << "virtual void" << endl;

          if (arg == L"void")
            os << "pre ();";
          else
            os << "pre (" << arg << ");";

          os << endl
             << "virtual void" << endl
             << "_serialize_content ();"
             << endl
             << "virtual void" << endl
             << "post ();"
             << "};";
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
            os << "virtual bool" << endl
               << epresent (a) << " ();"
               << endl;
          }

          Traversal::All::traverse (a);
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.contains_begin () != c.contains_end ())
          {
            size_t min (c.min ()), max (c.max ());

            if (min == 0 && max == 1)
            {
              os << "virtual bool" << endl
                 << epresent (c) << " ();"
                 << endl;
            }
            else if (max != 1)
            {
              os << "virtual bool" << endl
                 << enext (c) << " ();"
                 << endl;
            }

            os << "virtual " << earm_tag (c) << endl
               << earm (c) << " ();"
               << endl;

            Traversal::Choice::traverse (c);
          }
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          size_t min (s.min ()), max (s.max ());

          if (min == 0 && max == 1)
          {
            os << "virtual bool" << endl
               << epresent (s) << " ();"
               << endl;
          }
          else if (max != 1)
          {
            os << "virtual bool" << endl
               << enext (s) << " ();"
               << endl;
          }

          Traversal::Sequence::traverse (s);
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

          if (min == 0 && max == 1)
          {
            os << "virtual bool" << endl
               << epresent (e) << " ();"
               << endl;
          }
          else if (max != 1)
          {
            os << "virtual bool" << endl
               << enext (e) << " ();"
               << endl;
          }

          String const& ret (ret_type (e.type ()));

          os << "virtual " << ret << endl
             << ename (e) << " ();"
             << endl;
        }

        virtual void
        traverse (SemanticGraph::Any& a)
        {
          size_t min (a.min ()), max (a.max ());

          if (min == 0 && max == 1)
          {
            os << "virtual bool" << endl
               << epresent (a) << " ();"
               << endl;
          }
          else if (max != 1)
          {
            os << "virtual bool" << endl
               << enext (a) << " ();"
               << endl;
          }

          if (stl)
          {
            os << "virtual void" << endl
               << ename (a) << " (::std::string& ns, ::std::string& name);"
               << endl;
          }
          else
          {
            os << "virtual void" << endl
               << ename (a) << " (const char*& ns, const char*& name, " <<
              "bool& free);"
               << endl;
          }

          os << "virtual void" << endl
             << eserialize (a) << " ();"
             << endl;

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
          if (a.optional_p ())
          {
            os << "virtual bool" << endl
               << epresent (a) << " ();"
               << endl;
          }

          String const& ret (ret_type (a.type ()));

          os << "virtual " << ret << endl
             << ename (a) << " ();"
             << endl;
        }

        virtual void
        traverse (SemanticGraph::AnyAttribute& a)
        {
          os << "virtual bool" << endl
             << enext (a) << " ();"
             << endl;

          if (stl)
          {
            os << "virtual void" << endl
               << ename (a) << " (::std::string& ns, ::std::string& name);"
               << endl;
          }
          else
          {
            os << "virtual void" << endl
               << ename (a) << " (const char*& ns, const char*& name, " <<
              "bool& free);"
               << endl;
          }

          os << "virtual void" << endl
             << eserialize (a) << " ();"
             << endl;
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
          // In case of an inheritance-by-restriction, we don't need to
          // generate serializer callbacks, etc. since they are the same
          // as in the base. We only need the serialization/validation code.
          //
          bool restriction (restriction_p (c));

          bool hb (c.inherits_p ());
          bool he (has<Traversal::Element> (c));
          bool ha (has<Traversal::Attribute> (c));

          bool hae (has_particle<Traversal::Any> (c));
          bool haa (has<Traversal::AnyAttribute> (c));

          String const& name (eimpl (c));
          String const& arg (arg_type (c));

          os << "class " << name << ": public " <<
            (mixin ? "virtual " : "") << ename (c);

          if (mixin && hb)
            os << "," << endl
               << "  public " << fq_name (c.inherits ().base (), "s:impl");

          os << "{"
             << "public:" << endl;

          // c-tor
          //
          if (tiein && hb)
            os << name << " ();"
               << endl;

          // pre
          //
          os << "virtual void" << endl;

          if (arg == L"void")
            os << "pre ();";
          else
            os << "pre (" << arg << ");";

          os << endl;


          // Member callbacks.
          //
          if (!restriction && (ha || he || hae || haa))
          {
            if (ha || haa)
            {
              os << "// Attributes." << endl
                 << "//" << endl;

              names (c, names_attribute_callback_);
            }

            if (he || hae)
            {
              os << "// Elements." << endl
                 << "//" << endl;

              contains_compositor (c, contains_compositor_callback_);
            }
          }

          // post
          //
          os << "virtual void" << endl
             << "post ();";

          if (tiein && hb)
            os << endl
               << "private:" << endl
               << fq_name (c.inherits ().base (), "s:impl") << " base_impl_;";

          os << "};";
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
    generate_impl_header (Context& ctx)
    {
      Traversal::Schema schema;

      Sources sources;
      Includes includes (ctx, Includes::impl_header);
      Traversal::Names schema_names;

      Namespace ns (ctx);
      Traversal::Names names;

      schema >> includes;
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
