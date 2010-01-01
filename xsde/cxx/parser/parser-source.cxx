// file      : xsde/cxx/parser/parser-source.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/parser/parser-source.hxx>

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
          String const& name (ename (e));
          String const& ret (ret_type (e));
          SemanticGraph::Type& base (e.inherits ().base ());
          String const& base_ret (ret_type (base));

          Boolean same (ret == base_ret);
          Boolean base_same (
            base.inherits_p () &&
            base_ret == ret_type (base.inherits ().base ()));

          if (same || ret == L"void" || poly_code ||
              (tiein && !(base_same || base_ret == L"void")))
          {
            os << "// " << name << endl
               << "//" << endl
               << endl;
          }

          if (same || ret == L"void")
          {
            String const& post (post_name (e));

            os << ret << " " << name << "::" << endl
               << post << " ()"
               << "{";

            if (tiein)
            {
              String const& impl (etiein (e));

              os << "if (this->" << impl << ")" << endl
                 << (ret != L"void" ? "return " : "") << "this->" <<
                impl << "->" << post << " ();";
            }

            if (same)
            {
              if (tiein)
                os << "else" << endl;

              if (ret == L"void")
                os << post_name (base) << " ();";
              else
                os << "return " << post_name (base) << " ();";
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
              Boolean gen (!anonymous (e));

              // We normally don't need to enter anonymous types into
              // the inheritance map. The only exception is when an
              // anonymous types is defined inside an element that
              // is a member of a substitution group.
              //
              if (!gen)
              {
                // The first instance that this anonymous type classifies
                // is the prototype for others if any.
                //
                SemanticGraph::Instance& i (
                  e.classifies_begin ()->instance ());

                if (SemanticGraph::Element* e =
                    dynamic_cast<SemanticGraph::Element*> (&i))
                {
                  if (e->substitutes_p ())
                    gen = true;
                }
              }

              if (gen)
              {
                os << "static" << endl
                   << "const ::xsde::cxx::parser::validating::" <<
                  "inheritance_map_entry" << endl
                   << "_xsde_" << name << "_inheritance_map_entry_ (" << endl
                   << name << "::_static_type ()," << endl
                   << fq_name (base) << "::_static_type ());"
                   << endl;
              }
            }
          }

          if (tiein && !(base_same || base_ret == L"void"))
          {
            String const& impl (etiein (base));
            String const& base_post (post_name (base));

            os << base_ret << " " << name << "::" << endl
               << base_post << " ()"
               << "{"
               << "assert (this->" << impl << ");"
               << "return this->" << impl << "->" << base_post << " ();"
               << "}";
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

        virtual Void
        traverse (Type& l)
        {
          String const& name (ename (l));
          SemanticGraph::Type& t (l.argumented ().type ());

          String item (unclash (name, "item"));
          String inst (L"_xsde_" + item + L"_");
          String const& post (post_name (t));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          // item
          //
          String const& arg (arg_type (t));

          os << "void " << name << "::" << endl
             << item;

          if (arg == L"void")
            os << " ()";
          else
            os << " (" << arg << (tiein ? " x" : "") << ")";

          os << "{";

          if (tiein)
          {
            String const& impl (etiein (l));

            os << "if (this->" << impl << ")" << endl
               << "this->" << impl << "->" << item << " (" <<
              (arg != L"void" ? "x" : "") << ");";
          }

          os << "}";

          // post
          //
          if (ret_type (l) == L"void")
          {
            String const& post (post_name (l));

            os << "void " << name << "::" << endl
               << post << " ()"
               << "{";

            if (tiein)
            {
              String const& impl (etiein (l));

              os << "if (this->" << impl << ")" << endl
                 << "this->" << impl << "->" << post << " ();";
            }

            os << "}";
          }

          // reset
          //
          if (reset)
          {
            os << "void " << name << "::" << endl
               << "_reset ()"
               << "{"
               << list_base << "::_reset ();"
               << endl
               << "if (this->" << inst << ")" << endl
               << "this->" << inst << "->_reset ();"
               << "}";
          }

          // parse_item
          //
          os << "void " << name << "::" << endl
             << "_xsde_parse_item (const " << string_type << "& v)"
             << "{"
             << "if (this->" << inst << ")"
             << "{"
             << "::xsde::cxx::parser::context& ctx = this->_context ();"
             << endl;

          // This implementation should work for both validating
          // and non-validating cases.
          //
          if (!exceptions || validation)
          {
            String const& ret (ret_type (t));

            os << "this->" << inst << "->pre ();";

            if (!exceptions)
              os << endl
                 << "if (this->" << inst << "->_error_type ())" << endl
                 << "this->" << inst << "->_copy_error (ctx);"
                 << "else" << endl;

            os << "this->" << inst << "->_pre_impl (ctx);"
               << endl
               << "if (!ctx.error_type ())" << endl
               << "this->" << inst << "->_characters (v);"
               << endl
               << "if (!ctx.error_type ())" << endl
               << "this->" << inst << "->_post_impl ();"
               << endl
               << "if (!ctx.error_type ())" << endl;

            if (ret == L"void")
              os << "this->" << inst << "->" << post << " ();"
                 << endl;
            else
              os << "{"
                 << arg_type (t) << " tmp = this->" << inst << "->" <<
                post << " ();"
                 << endl;

            if (!exceptions)
              os << "if (this->" << inst << "->_error_type ())" << endl
                 << "this->" << inst << "->_copy_error (ctx);"
                 << "else" << endl;

            if (ret == L"void")
              os << "this->" << item << " ();";
            else
              os << "this->" << item << " (tmp);"
                 << "}";
          }
          else
          {
            os << "this->" << inst << "->pre ();"
               << "this->" << inst << "->_pre_impl (ctx);"
               << "this->" << inst << "->_characters (v);"
               << "this->" << inst << "->_post_impl ();";

            if (ret_type (t) == L"void")
              os << "this->" << inst << "->" << post << " ();"
                 << "this->" << item << " ();";
            else
              os << "this->" << item << " (this->" << inst << "->" <<
                post << " ());";
          }

          os << "}"
             << "}";

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

        virtual Void
        traverse (Type& u)
        {
          String const& name (ename (u));
          String const& ret (ret_type (u));

          if (ret == L"void" || poly_code)
          {
            os << "// " << name << endl
               << "//" << endl
               << endl;
          }

          if (ret == L"void")
          {
            String const& post (post_name (u));

            os << "void " << name << "::" << endl
               << post << " ()"
               << "{";

            if (tiein)
            {
              String const& impl (etiein (u));

              os << "if (this->" << impl << ")" << endl
                 << "this->" << impl << "->" << post << " ();";
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

        virtual Void
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

        virtual Void
        traverse (Type& a)
        {
          String const& m (emember (a));

          os << "if (this->" << m << ")" << endl
             << "this->" << m << "->_reset ();"
             << endl;
        }
      };

      //
      //
      struct StartElement : Traversal::Element, Context
      {
        StartElement (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& e)
        {
          Boolean poly (poly_code && !anonymous (e.type ()));
          Boolean subst (poly && e.global ());
          String const& inst (poly ? emember_cache (e) : emember (e));

          if (e.qualified () && e.namespace_ ().name ())
          {
            os << "if (" << (subst ? "(" : "") <<
              "n == " << strlit (e.name ()) << " &&" << endl
               << "ns == " << strlit (e.namespace_ ().name ());
          }
          else
          {
            os << "if (" << (subst ? "(" : "") <<
              "n == " << L << strlit (e.name ()) << " && ns.empty ()";
          }

          // Only a globally-defined element can be a subst-group root.
          //
          if (subst)
          {
            String root_id (e.name ());

            if (String const& ns = e.namespace_ ().name ())
            {
              root_id += L' ';
              root_id += ns;
            }

            os << ") ||" << endl
               << "::xsde::cxx::parser::substitution_map_instance ()" <<
              ".check (" << endl
               << "ns, n, " << strlit (root_id) << ", t)";
          }

          os << ")"
             << "{";

          if (poly)
          {
            // In case of mixin we use virtual inheritance and only
            // dynamic_cast can be used.
            //
            String cast (mixin ? L"dynamic_cast" : L"static_cast");
            String fq_type (fq_name (e.type ()));
            String const& member (emember (e));
            String const& member_map (emember_map (e));

            os << "if (t == 0 && this->" << member << " != 0)" << endl
               << "this->" << inst << " = this->" << member << ";"
               << "else"
               << "{"
               << "const char* ts = " << fq_type << "::_static_type ();"
               << endl
               << "if (t == 0)" << endl
               << "t = ts;"
               << endl
               << "if (this->" << member << " != 0 && " <<
              "strcmp (t, ts) == 0)" << endl
               << "this->" << inst << " = this->" << member << ";"
               << "else if (this->" << member_map << " != 0)" << endl
               << "this->" << inst << " = " << cast << "< " << fq_type <<
              "* > (" << endl
               << "this->" << member_map << "->find (t));"
               << "else" << endl
               << "this->" << inst << " = 0;"
               << "}";
          }

          os << "if (this->" << inst << ")"
             << "{";

          if (exceptions)
          {
            os << "this->" << inst << "->pre ();"
               << "this->" << inst << "->_pre_impl (ctx);";
          }
          else
          {
            // Note that after pre() we need to check both parser and
            // context error states because of the recursive parsing.
            //
            os << "this->" << inst << "->pre ();"
               << endl
               << "if (this->" << inst << "->_error_type ())" << endl
               << "this->" << inst << "->_copy_error (ctx);"
               << endl
               << "if (!ctx.error_type ())" << endl
               << "this->" << inst << "->_pre_impl (ctx);";
          }

          os << "}"
             << "else" << endl
             << "ctx.current_.depth_++;" // Ignoring document fragment.
             << endl
             << "return true;"
             << "}";
        }
      };


      //
      //
      struct EndElement : Traversal::Element, Context
      {
        EndElement (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& e)
        {
          String const& name (ename (e));
          Boolean poly (poly_code && !anonymous (e.type ()));
          Boolean subst (poly && e.global ());
          String const& inst (poly ? emember_cache (e) : emember (e));

          if (e.qualified () && e.namespace_ ().name ())
          {
            os << "if (" << (subst ? "(" : "") <<
              "n == " << strlit (e.name ()) << " &&" << endl
               << "ns == " << strlit (e.namespace_ ().name ());
          }
          else
          {
            os << "if (" << (subst ? "(" : "") <<
              "n == " << strlit (e.name ()) << " && ns.empty ()";
          }

          // Only a globally-defined element can be a subst-group root.
          //
          if (subst)
          {
            String root_id (e.name ());

            if (String const& ns = e.namespace_ ().name ())
            {
              root_id += L' ';
              root_id += ns;
            }

            os << ") ||" << endl
               << "::xsde::cxx::parser::substitution_map_instance ()" <<
              ".check (" << endl
               << "ns, n, " << strlit (root_id) << ")";
          }

          os << ")"
             << "{";

          SemanticGraph::Type& type (e.type ());
          String const& post (post_name (type));

          os << "if (this->" << inst << ")"
             << "{";

          if (exceptions)
          {
            if (ret_type (type) == L"void")
              os << "this->" << inst << "->" << post << " ();"
                 << "this->" << name << " ();";
            else
              os << "this->" << name << " (this->" << inst << "->" <<
                post << " ());";
          }
          else
          {
            // Note that after post() we need to check both parser and
            // context error states because of the recursive parsing.
            //
            if (ret_type (type) == L"void")
            {
              os << "this->" << inst << "->" << post << " ();"
                 << endl
                 << "if (this->" << inst << "->_error_type ())" << endl
                 << "this->" << inst << "->_copy_error (ctx);"
                 << endl
                 << "if (!ctx.error_type ())" << endl
                 << "this->" << name << " ();";
            }
            else
            {
              os << arg_type (type) << " tmp = this->" << inst << "->" <<
                post << " ();"
                 << endl
                 << "if (this->" << inst << "->_error_type ())" << endl
                 << "this->" << inst << "->_copy_error (ctx);"
                 << endl
                 << "if (!ctx.error_type ())" << endl
                 << "this->" << name << " (tmp);";
            }
          }

          os << "}"
             << "return true;"
             << "}";
        }
      };


      //
      //
      struct Attribute : Traversal::Attribute, Context
      {
        Attribute (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& a)
        {
          String const& name (ename (a));
          String const& inst (emember (a));

          if (a.qualified () && a.namespace_ ().name ())
          {
            os << "if (n == " << L << strlit (a.name ()) << " &&" << endl
               << "ns == " << L << strlit (a.namespace_ ().name ()) << ")"
               << "{";
          }
          else
          {
            os << "if (n == " << L << strlit (a.name ()) << " && ns.empty ())"
               << "{";
          }

          SemanticGraph::Type& type (a.type ());
          String const& post (post_name (type));
          String const& ret (ret_type (type));

          os << "if (this->" << inst << ")"
             << "{";

          if (exceptions)
          {
            os << "this->" << inst << "->pre ();"
               << "this->" << inst << "->_pre_impl (ctx);"
               << "this->" << inst << "->_characters (v);"
               << "this->" << inst << "->_post_impl ();";

            if (ret == L"void")
              os << "this->" << inst << "->" << post << " ();"
                 << "this->" << name << " ();";
            else
              os << "this->" << name << " (this->" << inst << "->" <<
                post << " ());";
          }
          else
          {
            os << "this->" << inst << "->pre ();"
               << endl
               << "if (!this->" << inst << "->_error_type ())" << endl
               << "this->" << inst << "->_pre_impl (ctx);"
               << "else" << endl
               << "this->" << inst << "->_copy_error (ctx);"
               << endl
               << "if (!ctx.error_type ())" << endl
               << "this->" << inst << "->_characters (v);"
               << endl
               << "if (!ctx.error_type ())" << endl
               << "this->" << inst << "->_post_impl ();"
               << endl
               << "if (!ctx.error_type ())" << endl;

            if (ret == L"void")
              os << "this->" << inst << "->" << post << " ();"
                 << endl;
            else
              os << "{"
                 << arg_type (type) << " tmp = this->" << inst << "->" <<
                post << " ();"
                 << endl;

            os << "if (this->" << inst << "->_error_type ())" << endl
               << "this->" << inst << "->_copy_error (ctx);"
               << "else" << endl;

            if (ret == L"void")
              os << "this->" << name << " ();";
            else
              os << "this->" << name << " (tmp);"
                 << "}";
          }

          os << "}"
             << "return true;"
             << "}";
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
          if (correspondent (a) == 0)
          {
            // For the all compositor, maxOccurs=1 and minOccurs={0,1}.
            //
            if (a.min () == 0)
            {
              SemanticGraph::Scope& s (scope (a));
              String const& present (epresent (a));

              os << "void " << ename (s) << "::" << endl
                 << present << " ()"
                 << "{";

              if (tiein)
              {
                String const& impl (
                  etiein (dynamic_cast<SemanticGraph::Type&> (s)));

                os << "if (this->" << impl << ")" << endl
                   << "this->" << impl << "->" << present << " ();";
              }

              os << "}";
            }
          }

          Traversal::All::traverse (a);
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.contains_begin () == c.contains_end ())
            return;

          if (correspondent (c) == 0)
          {
            SemanticGraph::Scope& s (scope (c));
            String const& arm (earm (c));

            os << "void " << ename (s) << "::" << endl
               << arm << " (" << earm_tag (c) << (tiein ? " x" : "") << ")"
               << "{";

              if (tiein)
              {
                String const& impl (
                  etiein (dynamic_cast<SemanticGraph::Type&> (s)));

                os << "if (this->" << impl << ")" << endl
                   << "this->" << impl << "->" << arm << " (x);";
              }

              os << "}";

          }

          Traversal::Choice::traverse (c);
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          // Root compositor that models inheritance by extension
          // may not have an association so we may fall through
          // in to the 'if' case even though this is a restriction.
          // This is ok since such a compositor always has max ==
          // min == 1 and so nothing is generated.
          //
          if (SemanticGraph::Compositor* b = correspondent (s))
          {
            // Add the *_present callback if this is a restriction
            // of sequence to optional.
            //
            if (b->max () != 1 && s.min () == 0)
            {
              SemanticGraph::Scope& ss (scope (s));

              os << "void " << ename (ss) << "::" << endl
                 << epresent (s) << " ()"
                 << "{";

              if (tiein)
              {
                String const& impl (
                  etiein (dynamic_cast<SemanticGraph::Type&> (ss)));

                os << "if (this->" << impl << ")" << endl
                   << "this->" << impl << "->" << enext (s) << " ();";
              }
              else
                os << "this->" << enext (s) << " ();";

              os << "}";
            }
          }
          else
          {
            if (s.max () != 1)
            {
              SemanticGraph::Scope& ss (scope (s));
              String const& next (enext (s));

              os << "void " << ename (ss) << "::" << endl
                 << next << " ()"
                 << "{";

              if (tiein)
              {
                String const& impl (
                  etiein (dynamic_cast<SemanticGraph::Type&> (ss)));

                os << "if (this->" << impl << ")" << endl
                   << "this->" << impl << "->" << next << " ();";
              }

              os << "}";
            }
            else if (s.min () == 0)
            {
              SemanticGraph::Scope& ss (scope (s));
              String const& present (epresent (s));

              os << "void " << ename (ss) << "::" << endl
                 << present << " ()"
                 << "{";

              if (tiein)
              {
                String const& impl (
                  etiein (dynamic_cast<SemanticGraph::Type&> (ss)));

                os << "if (this->" << impl << ")" << endl
                   << "this->" << impl << "->" << present << " ();";
              }

              os << "}";
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

      struct ParticleCallback: Traversal::Element, Context
      {
        ParticleCallback (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          if (correspondent (e) == 0)
          {
            String const& name (ename (e));
            String const& arg (arg_type (e.type ()));

            os << "void " << ename (e.scope ()) << "::" << endl
               << name;

            if (arg == L"void")
              os << " ()";
            else
              os << " (" << arg << (tiein ? " x" : "") << ")";

            os << "{";

            if (tiein)
            {
              String const& impl (
                etiein (dynamic_cast<SemanticGraph::Type&> (e.scope ())));

              os << "if (this->" << impl << ")" << endl
                 << "this->" << impl << "->" << name << " (" <<
                (arg != L"void" ? "x" : "") << ");";
            }

            os << "}";
          }
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
          String const& name (ename (a));
          String const& arg (arg_type (a.type ()));

          os << "void " << ename (a.scope ()) << "::" << endl
             << name;

          if (arg == L"void")
            os << " ()";
          else
            os << " (" << arg << (tiein ? " x" : "") << ")";

          os << "{";

          if (tiein)
          {
            String const& impl (
              etiein (dynamic_cast<SemanticGraph::Type&> (a.scope ())));

            os << "if (this->" << impl << ")" << endl
               << "this->" << impl << "->" << name << " (" <<
              (arg != L"void" ? "x" : "") << ");";
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
              compositor_callback_val_ (c),
              particle_callback_ (c),
              attribute_callback_ (c),
              particle_reset_ (c),
              attribute_reset_ (c),
              start_element_ (c),
              end_element_ (c),
              attribute_ (c)
        {
          // Callback.
          //
          if (validation)
          {
            contains_compositor_callback_ >> compositor_callback_val_;
            compositor_callback_val_ >> contains_particle_callback_;
            contains_particle_callback_ >> compositor_callback_val_;
          }
          else
          {
            contains_compositor_callback_ >> compositor_callback_non_val_;
            compositor_callback_non_val_ >> contains_particle_callback_;
            contains_particle_callback_ >> compositor_callback_non_val_;
          }

          contains_particle_callback_ >> particle_callback_;

          names_attribute_callback_ >> attribute_callback_;

          // Reset.
          //
          contains_compositor_reset_ >> compositor_reset_;
          compositor_reset_ >> contains_particle_reset_;
          contains_particle_reset_ >> compositor_reset_;
          contains_particle_reset_ >> particle_reset_;

          names_attribute_reset_ >> attribute_reset_;

          //
          //
          contains_compositor_start_ >> start_compositor_;
          start_compositor_ >> contains_particle_start_;
          contains_particle_start_ >> start_compositor_;
          contains_particle_start_ >> start_element_;

          //
          //
          contains_compositor_end_ >> end_compositor_;
          end_compositor_ >> contains_particle_end_;
          contains_particle_end_ >> end_compositor_;
          contains_particle_end_ >> end_element_;

          //
          //
          names_attribute_ >> attribute_;
        }

        virtual Void
        traverse (Type& c)
        {
          Boolean hb (c.inherits_p ());
          Boolean restriction (restriction_p (c));
          Boolean he (has<Traversal::Element> (c));
          Boolean ha (has<Traversal::Attribute> (c));

          Boolean hae (has_particle<Traversal::Any> (c));

          Boolean hra (false); // Has required attribute.
          if (ha)
          {
            RequiredAttributeTest test (hra);
            Traversal::Names names_test (test);
            names (c, names_test);
          }

          String const& name (ename (c));
          String const& ret (ret_type (c));
          Boolean same (hb && ret == ret_type (c.inherits ().base ()));

          String base_ret;
          Boolean base_same (true);

          if (tiein && hb)
          {
            SemanticGraph::Type& base (c.inherits ().base ());

            base_ret = ret_type (base);
            base_same = base.inherits_p () &&
              base_ret == ret_type (base.inherits ().base ());
          }

          if (he || ha || same || ret == L"void" || poly_code ||
              (tiein && !(base_same || base_ret == L"void")))
          {
            os << "// " << name << endl
               << "//" << endl
               << endl;
          }

          // Member callbacks.
          //
          if (!restriction)
          {
            if (ha)
              names (c, names_attribute_callback_);
          }

          if (!restriction || validation)
          {
            if (he || hae)
              contains_compositor (c, contains_compositor_callback_);
          }

          // post
          //
          if (same || ret == L"void")
          {
            String const& post (post_name (c));

            os << ret << " " << name << "::" << endl
               << post << " ()"
               << "{";

            if (tiein)
            {
              String const& impl (etiein (c));

              os << "if (this->" << impl << ")" << endl
                 << (ret != L"void" ? "return " : "") << "this->" <<
                impl << "->" << post << " ();";
            }

            if (same)
            {
              if (tiein)
                os << "else" << endl;

              SemanticGraph::Type& base (c.inherits ().base ());

              if (ret == L"void")
                os << post_name (base) << " ();";
              else
                os << "return " << post_name (base) << " ();";
            }

            os << "}";
          }

          // reset
          //
          if (!restriction && (he || ha) && reset)
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

            // Reset validation state.
            //
            if (validation)
            {
              if (he || hae)
              {
                os << "this->v_state_stack_.clear ();"
                   << endl;

                SemanticGraph::Compositor& comp (
                  c.contains_compositor ().compositor ());

                if (comp.is_a<SemanticGraph::All> () &&
                    comp.context().count ("p:comp-number"))
                {
                  os << "this->v_all_count_.clear ();"
                     << endl;
                }
              }

              if (hra)
                os << "this->v_state_attr_stack_.clear ();"
                   << endl;
            }

            // Reset member parsers.
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
              Boolean gen (!anonymous (c));

              // We normally don't need to enter anonymous types into
              // the inheritance map. The only exception is when an
              // anonymous types is defined inside an element that
              // is a member of a substitution group.
              //
              if (!gen)
              {
                // The first instance that this anonymous type classifies
                // is the prototype for others if any.
                //
                SemanticGraph::Instance& i (
                  c.classifies_begin ()->instance ());

                if (SemanticGraph::Element* e =
                    dynamic_cast<SemanticGraph::Element*> (&i))
                {
                  if (e->substitutes_p ())
                    gen = true;
                }
              }

              if (gen)
              {
                SemanticGraph::Type& base (c.inherits ().base ());

                os << "static" << endl
                   << "const ::xsde::cxx::parser::validating::" <<
                  "inheritance_map_entry" << endl
                   << "_xsde_" << name << "_inheritance_map_entry_ (" << endl
                   << name << "::_static_type ()," << endl
                   << fq_name (base) << "::_static_type ());"
                   << endl;
              }
            }
          }

          // Base post
          //
          if (tiein && !(base_same || base_ret == L"void"))
          {
            SemanticGraph::Type& base (c.inherits ().base ());

            String const& impl (etiein (base));
            String const& base_post (post_name (base));

            os << base_ret << " " << name << "::" << endl
               << base_post << " ()"
               << "{"
               << "assert (this->" << impl << ");"
               << "return this->" << impl << "->" << base_post << " ();"
               << "}";
          }

          // The rest is parsing/validation code which is generated in
          // *-validation-source.cxx.
          //
          if (validation)
            return;

          // Don't use the restriction_p result from here since we don't
          // want special treatment of anyType.
          //
          restriction = hb && c.inherits ().is_a<SemanticGraph::Restricts> ();

          // _start_element_impl & _end_element_impl
          //
          if (he)
          {
            // _start_element_impl
            //

            os << "bool " << name << "::" << endl
               << "_start_element_impl (const " << string_type << "& ns," << endl
               << "const " << string_type << "& n";

            if (poly_runtime)
              os << "," << endl
                 << "const char*" << (poly_code ? " t" : "");

            os << ")"
               << "{";

            if (poly_code)
              os << "XSDE_UNUSED (t);"
                 << endl;

            if (!restriction)
            {
              // We cannot use the fully-qualified base name directly
              // because of some broken compilers (EVC 4.0).
              //
              String base (unclash (name, "base"));

              os << "typedef ";

              if (hb)
                os << fq_name (c.inherits ().base ());
              else
                os << complex_base;

              os << " " << base << ";"
                 << "if (" << base << "::";

              if (poly_runtime)
                os << "_start_element_impl (ns, n, " <<
                  (poly_code ? "t" : "0") << "))" << endl;
              else
                os << "_start_element_impl (ns, n))" << endl;

              os << "return true;"
                 << endl;
            }

            os << "::xsde::cxx::parser::context& ctx = this->_context ();"
               << endl;

            contains_compositor (c, contains_compositor_start_);

            os << "return false;"
               << "}";


            // _end_element_impl
            //
            os << "bool " << name << "::" << endl
               << "_end_element_impl (const " << string_type << "& ns," << endl
               << "const " << string_type << "& n)"
               << "{";


            if (!restriction)
            {
              // We cannot use the fully-qualified base name directly
              // because of some broken compilers (EVC 4.0).
              //
              String base (unclash (name, "base"));

              os << "typedef ";

              if (hb)
                os << fq_name (c.inherits ().base ());
              else
                os << complex_base;

              os << " " << base << ";"
                 << "if (" << base << "::_end_element_impl (ns, n))" << endl
                 << "return true;"
                 << endl;
            }

            if (!exceptions)
              os << "::xsde::cxx::parser::context& ctx = this->_context ();"
                 << endl;

            contains_compositor (c, contains_compositor_end_);

            os << "return false;"
               << "}";
          }


          if (ha)
          {
            // _attribute_impl
            //

            os << "bool " << name << "::" << endl
               << "_attribute_impl (const " << string_type << "& ns," << endl
               << "const " << string_type << "& n," << endl
               << "const " << string_type << "& v)"
               << "{";

            if (!restriction)
            {
              // We cannot use the fully-qualified base name directly
              // because of some broken compilers (EVC 4.0).
              //
              String base (unclash (name, "base"));

              os << "typedef ";

              if (hb)
                os << fq_name (c.inherits ().base ());
              else
                os << complex_base;

              os << " " << base << ";"
                 << "if (" << base << "::_attribute_impl (ns, n, v))"
                 << "{"
                 << "return true;"
                 << "}";
            }

            os << "::xsde::cxx::parser::context& ctx = this->_context ();"
               << endl;

            names (c, names_attribute_);

            os << "return false;"
               << "}";
          }
        }

      private:
        //
        //
        CompositorCallback compositor_callback_val_;
        Traversal::Compositor compositor_callback_non_val_;
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
        Traversal::Compositor start_compositor_;
        StartElement start_element_;
        Traversal::ContainsCompositor contains_compositor_start_;
        Traversal::ContainsParticle contains_particle_start_;

        //
        //
        Traversal::Compositor end_compositor_;
        EndElement end_element_;
        Traversal::ContainsCompositor contains_compositor_end_;
        Traversal::ContainsParticle contains_particle_end_;

        //
        //
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

        virtual Void
        traverse (Type& e)
        {
          if (e.substitutes_p ())
          {
            String name (escape (e.name ()));
            Type& r (e.substitutes ().root ());

            SemanticGraph::Type& type (e.type ());

            String m_id (e.name ());
            String r_id (r.name ());

            if (String const& ns = e.namespace_ ().name ())
            {
              m_id += L' ';
              m_id += ns;
            }

            if (String const& ns = r.namespace_ ().name ())
            {
              r_id += L' ';
              r_id += ns;
            }

            os << "// Substitution map entry for " << comment (e.name ()) <<
              "." << endl
               << "//" << endl
               << "static" << endl
               << "const ::xsde::cxx::parser::substitution_map_entry" << endl
               << "_xsde_" << name << "_substitution_map_entry_ (" << endl
               << strlit (m_id) << "," << endl
               << strlit (r_id) << "," << endl
               << fq_name (type) << "::_static_type ());"
               << endl;
          }
        }
      };
    }

    Void
    generate_parser_source (Context& ctx)
    {
      if (ctx.tiein)
        ctx.os << "#include <assert.h>" << endl
               << endl;

      if (ctx.poly_code)
      {
        ctx.os << "#include <string.h>" << endl
               << "#include <xsde/cxx/parser/substitution-map.hxx>" << endl;

        if (ctx.validation)
          ctx.os << "#include <xsde/cxx/parser/validating/inheritance-map.hxx>" << endl
                 << endl;
        else
          ctx.os << endl;

        ctx.os << "static" << endl
               << "const ::xsde::cxx::parser::substitution_map_init" << endl
               << "_xsde_substitution_map_init_;"
               << endl;

        if (ctx.validation)
        {
          ctx.os << "static" << endl
                 << "const ::xsde::cxx::parser::validating::" <<
            "inheritance_map_init" << endl
                 << "_xsde_inheritance_map_init_;"
                 << endl;
        }
      }

      // Emit "weak" header includes that are used in the file-per-type
      // compilation model.
      //
      if (ctx.options.value<CLI::generate_inline> ())
      {
        Traversal::Schema schema;
        Includes includes (ctx, Includes::source);

        schema >> includes;
        schema.dispatch (ctx.schema_root);
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
