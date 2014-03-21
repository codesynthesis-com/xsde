// file      : xsde/cxx/parser/impl-header.cxx
// copyright : Copyright (c) 2005-2014 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/parser/impl-header.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

namespace CXX
{
  namespace Parser
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
          String const& ret (ret_type (e));
          SemanticGraph::Type& base (e.inherits ().base ());

          os << "class " << name << ": " << "public " <<
            (mixin ? "virtual " : "") << ename (e);

          if (mixin)
            os << "," << endl
               << "  public " << fq_name (base, "p:impl");

          os << "{"
             << "public:" << endl;

          if (tiein)
            os << name << " ();"
               << endl;

          os << "virtual void" << endl
             << "pre ();"
             << endl
             << "virtual " << ret << endl
             << post_name (e) << " ();";

          if (tiein)
            os << endl
               << "private:" << endl
               << fq_name (base, "p:impl") << " base_impl_;";

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

          String item (unclash (ename (l), "item"));

          os << "class " << name << ": public " <<
            (mixin ? "virtual " : "") << ename (l)
             << "{"
             << "public:" << endl
             << "virtual void" << endl
             << "pre ();"
             << endl;

          // item
          //
          String const& arg (arg_type (t));

          os << "virtual void" << endl
             << item;

          if (arg == L"void")
            os << " ();";
          else
            os << " (" << arg << ");";

          os << endl;

          // post
          //
          String const& ret (ret_type (l));

          os << "virtual " << ret << endl
             << post_name (l) << " ();"
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
          String const& ret (ret_type (u));

          os << "class " << name << ": public " <<
            (mixin ? "virtual " : "") << ename (u)
             << "{"
             << "public:" << endl
             << "virtual void" << endl
             << "pre ();"
             << endl
             << "virtual void" << endl
             << "_characters (const " << string_type << "&);"
             << endl
             << "virtual " << ret << endl
             << post_name (u) << " ();"
             << "};";
        }
      };


      //
      //
      struct ParticleCallback: Traversal::Element, Context
      {
        ParticleCallback (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          String const& arg (arg_type (e.type ()));

          os << "virtual void" << endl
             << ename (e);

          if (arg == L"void")
            os << " ();";
          else
            os << " (" << arg << ");";

          os << endl;
        }
      };

      struct AttributeCallback: Traversal::Attribute, Context
      {
        AttributeCallback (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Attribute& a)
        {
          String const& arg (arg_type (a.type ()));

          os << "virtual void" << endl
             << ename (a);

          if (arg == L"void")
            os << " ();";
          else
            os << " (" << arg << ");";

          os << endl;
        }
      };


      //
      //
      struct Complex : Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c),
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
          bool he (has<Traversal::Element> (c));
          bool ha (has<Traversal::Attribute> (c));

          String const& name (eimpl (c));
          String const& ret (ret_type (c));

          os << "class " << name << ": public " <<
            (mixin ? "virtual " : "") << ename (c);

          if (mixin && hb)
            os << "," << endl
               << "  public " << fq_name (c.inherits ().base (), "p:impl");

          os << "{"
             << "public:" << endl;

          if (tiein && hb)
            os << name << " ();"
               << endl;

          os << "virtual void" << endl
             << "pre ();"
             << endl;

          // In case of an inheritance-by-restriction, we don't need to
          // generate parser callbacks, etc. since they are the same as
          // in the base.
          //
          if (!restriction_p (c))
          {
            if (ha)
            {
              os << "// Attributes." << endl
                 << "//" << endl;

              names (c, names_attribute_callback_);
            }

            if (he)
            {
              os << "// Elements." << endl
                 << "//" << endl;

              contains_compositor (c, contains_compositor_callback_);
            }
          }

          os << "virtual " << ret << endl
             << post_name (c) << " ();";

          if (tiein && hb)
            os << endl
               << "private:" << endl
               << fq_name (c.inherits ().base (), "p:impl") << " base_impl_;";

          os << "};";
        }

      private:
        Traversal::Compositor compositor_callback_;
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
