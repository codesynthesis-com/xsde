// file      : xsde/cxx/hybrid/parser-header.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/hybrid/parser-header.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      //
      //
      struct PostOverride: Traversal::Complex, Context
      {
        PostOverride (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Complex& c)
        {
          if (c.inherits_p ())
          {
            SemanticGraph::Type& b (c.inherits ().base ());

            if (polymorphic (b))
            {
              if (tiein)
                dispatch (b);

              os << "virtual " << pret_type (b) << endl
                 << post_name (b) << " ();"
                 << endl;
            }
          }
        }
      };

      //
      //
      struct Enumeration: Traversal::Enumeration, Context
      {
        Enumeration (Context& c, Traversal::Complex& complex)
            : Context (c), complex_ (complex), post_override_ (c)
        {
        }

        virtual Void
        traverse (Type& e)
        {
          // First see if we should delegate this one to the Complex
          // generator.
          //
          Type* base_enum (0);

          if (!enum_ || !enum_mapping (e, &base_enum))
          {
            complex_.traverse (e);
            return;
          }

          String const& name (epimpl_custom (e));

          // We may not need to generate the class if this parser is
          // being customized.
          //
          if (name)
          {
            Boolean fl (fixed_length (e));
            Boolean val (!options.value<CLI::suppress_validation> () &&
                         !options.value<CLI::suppress_parser_val> ());

            SemanticGraph::Type& b (e.inherits ().base ());

            os << "class " << name << ": public " <<
              (mixin ? "virtual " : "") << epskel (e);

            // Derive from base pimpl even if base_enum == 0. This is done
            // so that we have implementations for all the (otherwise pure
            // virtual) post_*() functions.
            //
            if (mixin)
              os << "," << endl
                 << "  public " << fq_name (b, "p:impl");

            os << "{"
               << "public:" << endl;

            // c-tor
            //
            if (!fl || tiein)
              os << name << " (" << (fl ? "" : "bool = false") << ");"
                 << endl;

            if (!fl)
            {
              // d-tor
              //
              os << "~" << name << " ();"
                 << endl;

              // reset
              //
              if (reset)
                os << "virtual void" << endl
                   << "_reset ();"
                   << endl;
            }

            // pre
            //
            if (!fl || !base_enum)
              os << "virtual void" << endl
                 << "pre ();"
                 << endl;

            // _pre
            //
            if (mixin && !base_enum)
              os << "virtual void" << endl
                 << "_pre ();"
                 << endl;

            if (!base_enum)
            {
              // _characters
              //
              os << "virtual void" << endl
                 << "_characters (const " << string_type << "&);"
                 << endl;

              // _post
              //
              if (val)
              {
                os << "virtual void" << endl
                   << "_post ();"
                   << endl;
              }
            }


            // post
            //
            String const& ret (pret_type (e));

            if (polymorphic (e))
              post_override_.dispatch (e);

            os << "virtual " << ret << endl
               << post_name (e) << " ();"
               << endl;

            String const& type (fq_name (e));

            // pre_impl
            //
            if (!fl)
              os << (tiein ? "public:" : "protected:") << endl
                 << "void" << endl
                 << pre_impl_name (e) <<  " (" << type << "*);"
                 << endl;

            // Base implementation.
            //
            if (tiein && base_enum)
              os << (tiein ? "public:" : "protected:") << endl
                 << fq_name (b, "p:impl") << " base_impl_;"
                 << endl;

            // State.
            //
            if (!fl || !base_enum)
            {
              String const& state_type (epstate_type (e));

              os << (tiein ? "public:" : "protected:") << endl
                 << "struct " << state_type
                 << "{";

              if (!fl)
                os << type << "* " << "x_;";

              if (!base_enum)
              {
                if (stl)
                  os << "::std::string str_;";
                else
                  os << "::xsde::cxx::string str_;";
              }

              os << "};"
                 << state_type << " " << epstate (e) << ";";
            }

            if (!fl)
              os << "bool " << epstate_base (e) << ";";

            os << "};";
          }

          // Generate include for custom parser.
          //
          if (e.context ().count ("p:impl-include"))
          {
            close_ns ();

            os << "#include " << process_include_path (
              e.context ().get<String> ("p:impl-include")) << endl
               << endl;

            open_ns ();
          }
        }

      private:
        Traversal::Complex& complex_;
        PostOverride post_override_;
      };

      struct List: Traversal::List, Context
      {
        List (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& l)
        {
          String const& name (epimpl_custom (l));

          // We may not need to generate the class if this parser is
          // being customized.
          //
          if (name)
          {
            String item (unclash (epskel (l), "item"));

            os << "class " << name << ": public " <<
              (mixin ? "virtual " : "") << epskel (l)
               << "{"
               << "public:" << endl;

            // d-tor & c-tor
            //
            os << "~" << name << " ();"
               << name << " (bool = false);"
               << endl;

            // reset
            //
            if (reset)
              os << "virtual void" << endl
                 << "_reset ();"
                 << endl;

            // pre
            //
            os << "virtual void" << endl
               << "pre ();"
               << endl;

            // item
            //
            String const& arg (parg_type (l.argumented ().type ()));

            os << "virtual void" << endl
               << item << " (" << arg << ");"
               << endl;

            // post
            //
            String const& ret (pret_type (l));

            os << "virtual " << ret << endl
               << post_name (l) << " ();"
               << endl;

            // pre_impl
            //
            String const& type (fq_name (l));

            os << "void" << endl
               << pre_impl_name (l) <<  " (" << type << "*);"
               << endl;

            os << (tiein ? "public:" : "protected:") << endl;

            // State.
            //
            os << type << "* " << epstate_member (l) << ";"
               << "bool " << epstate_base (l) << ";"
               << "};";
          }

          // Generate include for custom parser.
          //
          if (l.context ().count ("p:impl-include"))
          {
            close_ns ();

            os << "#include " << process_include_path (
              l.context ().get<String> ("p:impl-include")) << endl
               << endl;

            open_ns ();
          }
        }
      };

      struct Union: Traversal::Union, Context
      {
        Union (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& u)
        {
          String const& name (epimpl_custom (u));

          // We may not need to generate the class if this parser is
          // being customized.
          //
          if (name)
          {
            Boolean fl (fixed_length (u));

            os << "class " << name << ": public " <<
              (mixin ? "virtual " : "") << epskel (u)
               << "{"
               << "public:" << endl;

            if (!fl)
            {
              // d-tor & c-tor
              //
              os << "~" << name << " ();"
                 << name << " (bool = false);"
                 << endl;

              // reset
              //
              if (reset)
                os << "virtual void" << endl
                   << "_reset ();"
                   << endl;
            }

            // pre
            //
            os << "virtual void" << endl
               << "pre ();"
               << endl;

            // _characters
            //
            os << "virtual void" << endl
               << "_characters (const " << string_type << "&);"
               << endl;

            // post
            //
            String const& ret (pret_type (u));

            os << "virtual " << ret << endl
               << post_name (u) << " ();"
               << endl;

            String const& type (fq_name (u));

            // pre_impl
            //
            if (!fl)
              os << "void" << endl
                 << pre_impl_name (u) <<  " (" << type << "*);"
                 << endl;

            os << (tiein ? "public:" : "protected:") << endl;

            // State.
            //
            String const& state_type (epstate_type (u));

            os << "struct " << state_type
               << "{";

            if (!fl)
              os << type << "* " << "x_;";

            if (stl)
              os << "::std::string str_;";
            else
              os << "::xsde::cxx::string str_;";

            os << "};"
               << state_type << " " << epstate (u) << ";";

            if (!fl)
              os << "bool " << epstate_base (u) << ";";

            os << "};";
          }

          // Generate include for custom parser.
          //
          if (u.context ().count ("p:impl-include"))
          {
            close_ns ();

            os << "#include " << process_include_path (
              u.context ().get<String> ("p:impl-include")) << endl
               << endl;

            open_ns ();
          }
        }
      };

      //
      // State.
      //

      struct CompositorState: Traversal::Compositor, Context
      {
        CompositorState (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Compositor& c)
        {
          if (c.max () != 1)
          {
            String scope (fq_scope (c));

            os << scope << "::" << etype (c) << "* " <<
              epstate_member (c) << ";";
          }

          Compositor::traverse (c);
        }
      };


      //
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

        virtual Void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}.
          //
          if (a.min () == 0)
          {
            os << "virtual void" << endl
               << eppresent (a) << " ();"
               << endl;
          }

          Traversal::All::traverse (a);
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          os << "virtual void" << endl
             << eparm (c) << " (" << eparm_tag (c) << ");"
             << endl;

          Traversal::Choice::traverse (c);
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          if (s.max () != 1)
          {
            os << "virtual void" << endl
               << epnext (s) << " ();"
               << endl;
          }
          else if (s.min () == 0)
          {
            os << "virtual void" << endl
               << eppresent (s) << " ();"
               << endl;
          }

          Traversal::Sequence::traverse (s);
        }
      };

      struct ParticleCallback: Traversal::Element, Context
      {
        ParticleCallback (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          String const& arg (parg_type (e.type ()));

          os << "virtual void" << endl
             << epname (e) << " (";

          if (arg != L"void")
            os << arg;

          os << ");"
             << endl;
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
          if (a.fixed_p ())
            return;

          String const& arg (parg_type (a.type ()));

          os << "virtual void" << endl
             << epname (a) << " (";

          if (arg != L"void")
            os << arg;

          os << ");"
             << endl;
        }
      };

      //
      //
      struct Complex : Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c),
              post_override_ (c),

              // State.
              //
              compositor_state_ (c),

              // Callbacks.
              //
              compositor_callback_ (c),
              particle_callback_ (c),
              attribute_callback_ (c)
        {
          // State.
          //
          contains_compositor_state_ >> compositor_state_;
          compositor_state_ >> contains_particle_state_;
          contains_particle_state_ >> compositor_state_;

          // Callbacks.
          //
          contains_compositor_callback_ >> compositor_callback_;
          compositor_callback_ >> contains_particle_callback_;
          contains_particle_callback_ >> compositor_callback_;
          contains_particle_callback_ >> particle_callback_;

          names_attribute_callback_ >> attribute_callback_;
        }

        virtual Void
        traverse (Type& c)
        {
          String const& name (epimpl_custom (c));

          // We may not need to generate the class if this parser is
          // being customized.
          //
          if (name)
          {
            Boolean hb (c.inherits_p ());
            Boolean he (has<Traversal::Element> (c));
            Boolean ha (has<Traversal::Attribute> (c));
            Boolean hae (has_particle<Traversal::Any> (c));

            Boolean restriction (restriction_p (c));
            Boolean fixed (fixed_length (c));
            Boolean rec (recursive (c));

            String const& ret (pret_type (c));

            os << "class " << name << ": public " <<
              (mixin ? "virtual " : "") << epskel (c);

            if (mixin && hb)
              os << "," << endl
                 << "  public " << fq_name (c.inherits ().base (), "p:impl");

            os << "{"
               << "public:" << endl;

            // c-tor
            //
            if (!fixed || (hb && tiein))
              os << name << " (" << (fixed ? "" : "bool = false") << ");"
                 << endl;

            if (!fixed)
            {
              // d-tor
              //
              os << "~" << name << " ();"
                 << endl;

              // reset
              //
              if (reset)
                os << "virtual void" << endl
                   << "_reset ();"
                   << endl;
            }

            // pre
            //
            os << "virtual void" << endl
               << "pre ();"
               << endl;

            // In case of an inheritance-by-restriction, we don't need to
            // generate parser callbacks, etc. since they are the same as
            // in the base.
            //
            if (!restriction)
            {
              if (ha)
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

            // _post
            //
            if (rec && hb && !recursive (c.inherits ().base ()))
            {
              os << "virtual void" << endl
                 << "_post ();"
                 << endl;
            }

            // post
            //
            if (polymorphic (c))
              post_override_.dispatch (c);

            os << "virtual " << ret << endl
               << post_name (c) << " ();"
               << endl;

            // pre_impl
            //
            String const& type (fq_name (c));

            if (!fixed)
              os << (tiein ? "public:" : "protected:") << endl
                 << "void" << endl
                 << pre_impl_name (c) <<  " (" << type << "*);"
                 << endl;

            // Base implementation.
            //
            if (tiein && hb)
              os << (tiein ? "public:" : "protected:") << endl
                 << fq_name (c.inherits ().base (), "p:impl") << " base_impl_;"
                 << endl;

            // State.
            //
            String const& state_type (epstate_type (c));
            String const& member (epstate_member (c));

            os << (tiein ? "public:" : "protected:") << endl
               << "struct " << state_type
               << "{";

            if (fixed)
              os << type << " " << member << ";";
            else
              os << type << "* " << member << ";";

            if (!restriction && c.contains_compositor_p ())
              contains_compositor (c, contains_compositor_state_);

            os << "};";

            if (rec)
            {
              os << state_type << " " << epstate_first (c) << ";"
                 << "::xsde::cxx::stack " << epstate (c) << ";";

              if (hb && !recursive (c.inherits ().base ()))
                os << "bool " << epstate_top (c) << ";";
            }
            else
              os << state_type << " " << epstate (c) << ";";

            if (!fixed)
              os << "bool " << epstate_base (c) << ";";

            os << "};";
          }

          // Generate include for custom parser.
          //
          SemanticGraph::Context& ctx (c.context ());

          if (ctx.count ("p:impl-include"))
          {
            close_ns ();

            os << "#include " << process_include_path (
              ctx.get<String> ("p:impl-include")) << endl
               << endl;

            open_ns ();
          }
        }

      private:
        PostOverride post_override_;

        // State.
        //
        CompositorState compositor_state_;
        Traversal::ContainsCompositor contains_compositor_state_;
        Traversal::ContainsParticle contains_particle_state_;

        // Callbacks.
        //
        CompositorCallback compositor_callback_;
        ParticleCallback particle_callback_;
        Traversal::ContainsCompositor contains_compositor_callback_;
        Traversal::ContainsParticle contains_particle_callback_;

        AttributeCallback attribute_callback_;
        Traversal::Names names_attribute_callback_;
      };
    }

    Void
    generate_parser_header (Context& ctx)
    {
      ctx.os << "#include <xsde/cxx/stack.hxx>" << endl
             << endl;

      Traversal::Schema schema;

      Traversal::Sources sources;
      Includes includes (ctx, Includes::impl_header);
      Traversal::Names schema_names;

      Namespace ns (ctx, true);
      Traversal::Names names;

      schema >> includes;
      schema >> sources >> schema;
      schema >> schema_names >> ns >> names;

      List list (ctx);
      Union union_ (ctx);
      Complex complex (ctx);
      Enumeration enumeration (ctx, complex);

      names >> list;
      names >> union_;
      names >> complex;
      names >> enumeration;

      schema.dispatch (ctx.schema_root);
    }
  }
}
