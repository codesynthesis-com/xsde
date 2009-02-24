// file      : xsde/cxx/parser/impl-source.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/parser/impl-source.hxx>
#include <cxx/parser/print-impl-common.hxx>

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

        virtual Void
        traverse (Type& e)
        {
          String const& name (eimpl (e));
          String const& ret (ret_type (e));
          SemanticGraph::Type& base (e.inherits ().base ());
          String const& base_ret (ret_type (base));

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
          os << "void " << name << "::" << endl
             << "pre ()"
             << "{"
             << "}";

          // post
          //
          os << ret << " " << name << "::" << endl
             << post_name (e) << " ()"
             << "{";

          if (ret == base_ret)
          {
            os << (ret != L"void" ? "return " : "") <<
              post_name (base) << " ();";
          }
          else if (ret == L"void")
          {
            os << arg_type (base) << " v = " << post_name (base) << " ();"
               << endl;

            if (options.value<CLI::no_exceptions> ())
              os << "if (!_error ())"
                 << "{";

            if (options.value<CLI::generate_print_impl> ())
            {
              PrintCall t (*this, e.name (), "v");
              t.dispatch (base);
            }
            else
              os << "// TODO" << endl
                 << "//" << endl;

            {
              DeleteCall t (*this, "v");
              t.dispatch (base);
            }

            if (options.value<CLI::no_exceptions> ())
              os << "}";
          }
          else
          {
            if (base_ret == L"void")
              os << post_name (base) << " ();";
            else
            {
              os << arg_type (base) << " v = " << post_name (base) << " ();"
                 << endl;

              if (options.value<CLI::no_exceptions> ())
                os << "if (!_error ())"
                   << "{";

              os << "// TODO" << endl
                 << "//" << endl
                 << "// return ... ;" << endl;

              {
                DeleteCall t (*this, "v");
                t.dispatch (base);
              }

              if (options.value<CLI::no_exceptions> ())
                os << "}";
            }
          }

          os << "}";
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

        virtual Void
        traverse (Type& l)
        {
          String const& name (eimpl (l));
          SemanticGraph::Type& type (l.argumented ().type ());

          String item (unclash (ename (l), "item"));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          // pre
          //
          os << "void " << name << "::" << endl
             << "pre ()"
             << "{"
             << "}";

          // item
          //
          String const& arg (arg_type (type));

          os << "void " << name << "::" << endl
             << item;

          if (arg == L"void")
            os << " ()";
          else
            os << " (" << arg << " " << item << ")";

          os << "{";

          if (arg != L"void")
          {
            if (options.value<CLI::generate_print_impl> ())
            {
              PrintCall t (*this, type.name (), item);
              t.dispatch (type);
            }
            else
              os << "// TODO" << endl
                 << "//" << endl;

            {
              DeleteCall t (*this, item);
              t.dispatch (type);
            }
          }

          os << "}";

          // post
          //
          String const& ret (ret_type (l));

          os << ret << " " << name << "::" << endl
             << post_name (l) << " ()"
             << "{";

          if (ret != L"void")
            os << "// TODO" << endl
               << "//" << endl
               << "// return ... ;" << endl;

          os << "}";
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

        virtual Void
        traverse (Type& u)
        {
          String const& name (eimpl (u));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          // pre
          //
          os << "void " << name << "::" << endl
             << "pre ()"
             << "{"
             << "}";

          // _characters
          //
          os << "void " << name << "::" << endl
             << "_characters (const " << string_type << "& s)"
             << "{";

          if (options.value<CLI::generate_print_impl> ())
          {
            if (options.value<CLI::no_iostream> ())
              os << "if (s.size () != 0)"
                 << "{"
                 << "printf (" << strlit (u.name () + L": ") << ");"
                 << "fwrite (s.data (), s.size (), 1, stdout);"
                 << "printf (\"\\n\");"
                 << "}";
            else
              os << "std::cout << " << strlit (u.name () + L": ") <<
                " << s << std::endl;";
          }
          else
            os << "// TODO" << endl
               << "//" << endl;

          os << "}";

          // post
          //
          String const& ret (ret_type (u));

          os << ret << " " << name << "::" << endl
             << post_name (u) << " ()"
             << "{";

          if (ret != L"void")
            os << "// TODO" << endl
               << "//" << endl
               << "// return ... ;" << endl;

          os << "}";
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

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          using SemanticGraph::Complex;

          String const& name (ename (e));
          String const& arg (arg_type (e.type ()));
          Complex& c (dynamic_cast<Complex&> (e.scope ()));

          os << "void " << eimpl (c) << "::" << endl
             << name;

          if (arg == L"void")
            os << " ()";
          else
            os << " (" << arg << " " << name << ")";

          os << "{";

          if (arg != L"void")
          {
            if (options.value<CLI::generate_print_impl> ())
            {
              PrintCall t (*this, e.name (), name);
              t.dispatch (e.type ());
            }
            else
              os << "// TODO" << endl
                 << "//" << endl;

            {
              DeleteCall t (*this, name);
              t.dispatch (e.type ());
            }
          }

          os << "}";
        }
      };

      struct AttributeCallback: Traversal::Attribute, Context
      {
        AttributeCallback (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          using SemanticGraph::Complex;

          String const& name (ename (a));
          String const& arg (arg_type (a.type ()));
          Complex& c (dynamic_cast<Complex&> (a.scope ()));

          os << "void " << eimpl (c) << "::" << endl
             << name;

          if (arg == L"void")
            os << " ()";
          else
            os << " (" << arg << " " << name << ")";

          os << "{";

          if (arg != L"void")
          {
            if (options.value<CLI::generate_print_impl> ())
            {
              PrintCall t (*this, a.name (), name);
              t.dispatch (a.type ());
            }
            else
              os << "// TODO" << endl
                 << "//" << endl;

            {
              DeleteCall t (*this, name);
              t.dispatch (a.type ());
            }
          }

          os << "}";
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

        virtual Void
        traverse (Type& c)
        {
          Boolean hb (c.inherits_p ());

          String const& name (eimpl (c));
          String const& ret (ret_type (c));

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
          os << "void " << name << "::" << endl
             << "pre ()"
             << "{"
             << "}";

          // Parser callbacks.
          //
          if (!restriction_p (c))
          {
            names (c, names_attribute_callback_);
            contains_compositor (c, contains_compositor_callback_);
          }

          // post
          //
          os << ret << " " << name << "::" << endl
             << post_name (c) << " ()"
             << "{";

          if (hb)
          {
            SemanticGraph::Type& base (c.inherits ().base ());
            String const& base_ret (ret_type (base));

            if (ret == base_ret)
            {
              os << (ret != L"void" ? "return " : "") <<
                post_name (base) << " ();";
            }
            else if (ret == L"void")
            {
              os << arg_type (base) << " v = " << post_name (base) << " ();"
                 << endl;

              if (options.value<CLI::no_exceptions> ())
                os << "if (!_error ())"
                   << "{";

              if (options.value<CLI::generate_print_impl> ())
              {
                PrintCall t (*this, c.name (), "v");
                t.dispatch (base);
              }
              else
                os << "// TODO" << endl
                   << "//" << endl;

              {
                DeleteCall t (*this, "v");
                t.dispatch (base);
              }

              if (options.value<CLI::no_exceptions> ())
                os << "}";
            }
            else
            {
              if (base_ret == L"void")
                os << post_name (base) << " ();";
              else
              {
                os << arg_type (base) << " v = " << post_name (base) << " ();"
                   << endl;

                if (options.value<CLI::no_exceptions> ())
                  os << "if (!_error ())"
                     << "{";

                os << "// TODO" << endl
                   << "//" << endl
                   << "// return ... ;" << endl;

                {
                  DeleteCall t (*this, "v");
                  t.dispatch (base);
                }

                if (options.value<CLI::no_exceptions> ())
                  os << "}";
              }
            }
          }
          else
          {
            if (ret != L"void")
              os << "// TODO" << endl
                 << "//" << endl
                 << "// return ... ;" << endl;
          }

          os << "}";
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

    Void
    generate_impl_source (Context& ctx)
    {
      if (ctx.options.value<CLI::generate_print_impl> ())
      {
        if (ctx.options.value<CLI::no_iostream> ())
          ctx.os << "#include <stdio.h>" << endl
                 << endl;
        else
          ctx.os << "#include <iostream>" << endl
                 << endl;
      }

      Traversal::Schema schema;

      Traversal::Sources sources;
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
