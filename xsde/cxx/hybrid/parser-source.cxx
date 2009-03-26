// file      : xsde/cxx/hybrid/parser-source.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/hybrid/parser-source.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
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

          if (!name)
            return;

          String const& type (fq_name (l));
          String const& base (epstate_base (l));
          String const& member (epstate_member (l));
          String item (unclash (epskel (l), "item"));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          // c-tor
          //
          os << name << "::" << endl
             << name << " (bool b)"
             << "{"
             << "this->" << base << " = b;"
             << "this->" << member << " = 0;"
             << "}";

          // d-tor
          //
          os << name << "::" << endl
             << "~" << name << " ()"
             << "{"
             << "if (!this->" << base << ")" << endl
             << "delete this->" << member << ";"
             << "}";

          // reset
          //
          if (reset)
            os << "void " << name << "::" << endl
               << "_reset ()"
               << "{"
               << epskel (l) << "::_reset ();"
               << endl
               << "if (!this->" << base << ")"
               << "{"
               << "delete this->" << member << ";"
               << "this->" << member << " = 0;"
               << "}"
               << "}";

          // pre_impl
          //
          os << "void " << name << "::" << endl
             << pre_impl_name (l) << " (" << type << "* x)"
             << "{"
             << "this->" << member << " = x;"
             << "}";

          // pre
          //
          os << "void " << name << "::" << endl
             << "pre ()"
             << "{";

          if (exceptions)
            os << "this->" << pre_impl_name (l) << " (new " << type << ");";
          else
            os << type << "* x = new " << type << ";"
               << "if (x)" << endl
               << "this->" << pre_impl_name (l) << " (x);"
               << "else" << endl
               << "this->_sys_error (::xsde::cxx::sys_error::no_memory);";

          os << "}";

          // item
          //
          String const& arg (parg_type (l.argumented ().type ()));

          os << "void " << name << "::" << endl
             << item << " (" << arg << " i)"
             << "{";

          if (exceptions)
            os << "this->" << member << "->push_back (i);";
          else
            os << "if (this->" << member << "->push_back (i))" << endl
               << "this->_sys_error (::xsde::cxx::sys_error::no_memory);";

          os << "}";

          // post
          //
          String const& ret (pret_type (l));

          os << ret << " " << name << "::" << endl
             << post_name (l) << " ()"
             << "{"
             << type << "* r = this->" << member << ";"
             << "this->" << member << " = 0;"
             << "return r;"
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

        virtual Void
        traverse (Type& u)
        {
          String const& name (epimpl_custom (u));

          if (!name)
            return;

          String const& type (fq_name (u));
          String const& state (epstate (u));
          String const& ret (pret_type (u));
          String const& value (u.context ().get<String> ("value"));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          if (stl)
          {
            // pre
            //
            os << "void " << name << "::" << endl
               << "pre ()"
               << "{"
               << "this->" << state << ".str_.clear ();"
               << "}";

            // _characters
            //
            os << "void " << name << "::" << endl
               << "_characters (const " << string_type << "& s)"
               << "{"
               << "this->" << state << ".str_.append (s.data (), s.size ());"
               << "}";

            // post
            //
            os << ret << " " << name << "::" << endl
               << post_name (u) << " ()"
               << "{"
               << "::std::string s;"
               << "s.swap (this->" << state << ".str_);"
               << type << " r;"
               << "r." << value << " (s);"
               << "return r;"
               << "}";
          }
          else
          {
            String const& base (epstate_base (u));

            // c-tor
            //
            os << name << "::" << endl
               << name << " (bool b)"
               << "{"
               << "this->" << base << " = b;"
               << "this->" << state << ".x_ = 0;"
               << "}";

            // d-tor
            //
            os << name << "::" << endl
               << "~" << name << " ()"
               << "{"
               << "if (!this->" << base << ")" << endl
               << "delete this->" << state << ".x_;"
               << "}";

            // reset
            //
            if (reset)
              os << "void " << name << "::" << endl
                 << "_reset ()"
                 << "{"
                 << epskel (u) << "::_reset ();"
                 << endl
                 << "if (!this->" << base << ")"
                 << "{"
                 << "delete this->" << state << ".x_;"
                 << "this->" << state << ".x_ = 0;"
                 << "}"
                 << "}";

            // pre_impl
            //
            os << "void " << name << "::" << endl
               << pre_impl_name (u) << " (" << type << "* x)"
               << "{"
               << "this->" << state << ".x_ = x;";

            if (exceptions)
              os << "this->" << state << ".str_.assign (\"\", 0);";
            else
            {
              os << endl
                 << "if (this->" << state << ".str_.assign (\"\", 0))" << endl
                 << "this->_sys_error (::xsde::cxx::sys_error::no_memory);";
            }

            os << "}";

            // pre
            //
            os << "void " << name << "::" << endl
               << "pre ()"
               << "{";

            if (exceptions)
              os << "this->" << pre_impl_name (u) << " (new " << type << ");";
            else
              os << type << "* x = new " << type << ";"
                 << "if (x)" << endl
                 << "this->" << pre_impl_name (u) << " (x);"
                 << "else" << endl
                 << "this->_sys_error (::xsde::cxx::sys_error::no_memory);";

            os << "}";

            // _characters
            //
            os << "void " << name << "::" << endl
               << "_characters (const " << string_type << "& s)"
               << "{";

            if (exceptions)
              os << "this->" << state << ".str_.append (s.data (), s.size ());";
            else
            {
              os << "if (this->" << state << ".str_.append (" <<
                "s.data (), s.size ()))" << endl
                 << "this->_sys_error (::xsde::cxx::sys_error::no_memory);";
            }

            os << "}";

            // post
            //
            os << ret << " " << name << "::" << endl
               << post_name (u) << " ()"
               << "{"
               << type << "* r = this->" << state << ".x_;"
               << "this->" << state << ".x_ = 0;"
               << "r->" << value << " (this->" << state << ".str_.detach ());"
               << "return r;"
               << "}";
          }
        }
      };

      struct ParserContext: Context
      {
        ParserContext (Context& c)
            : Context (c)
        {
        }

        // Return the access sequence up until this particle. If
        // element is false then the access sequence for the
        // container is returned. Otherwise the access sequence
        // for the current element in the container is returned.
        //
        String
        access_seq (SemanticGraph::Particle& p, Boolean element = true)
        {
          using namespace SemanticGraph;

          String r;

          Boolean seq (false);

          Compositor* c;

          if (p.contained_particle_p ())
          {
            c = &p.contained_particle ().compositor ();

            // Check if this particle is a sequence. In this case
            // we just need the top-level struct member.
            //
            if (element && p.max () != 1 && p.is_a<Compositor> ())
            {
              seq = true;
            }
            else
            {
              for (;; c = &c->contained_particle ().compositor ())
              {
                if (c->context ().count ("type"))
                {
                  // Not a see-through compositor.
                  //
                  if (c->max () != 1)
                  {
                    String const& ptr (epstate_member (*c));

                    if (!r)
                    {
                      r = ptr;
                      r += L"->";
                    }
                    else
                    {
                      String tmp;
                      tmp.swap (r);
                      r = ptr;
                      r += L"->";
                      r += tmp;
                    }

                    seq = true;
                    break;
                  }
                  else
                  {
                    String const& func (ename (*c));

                    if (!r)
                    {
                      r = func;
                      r += L" ().";
                    }
                    else
                    {
                      String tmp;
                      tmp.swap (r);
                      r = func;
                      r += L" ().";
                      r += tmp;
                    }
                  }
                }

                if (c->contained_compositor_p ())
                  break;
              }
            }

            // Get to the top in case we bailed out on a sequence.
            //
            while (!c->contained_compositor_p ())
              c = &c->contained_particle ().compositor ();
          }
          else
          {
            // This particle is a top-level compositor.
            //
            c = &dynamic_cast<Compositor&> (p);
            seq = element && c->max () != 1;
          }

          Complex& t (
            dynamic_cast<Complex&> (
              c->contained_compositor ().container ()));

          if (!seq)
          {
            Boolean fixed (fixed_length (t));
            String const& s (epstate_member (t));

            if (!r)
            {
              r = s;
              r += fixed ? L"." : L"->";
            }
            else
            {
              String tmp;
              tmp.swap (r);
              r = s;
              r += fixed ? L"." : L"->";
              r += tmp;
            }
          }

          String tmp;
          tmp.swap (r);

          if (!recursive (t))
          {
            r = L"this->";
            r += epstate (t);
            r += L".";
          }
          else
          {
            r = L"static_cast< ";
            r += epstate_type (t);
            r += L"* > (this->";
            r += epstate (t);
            r += L".top ())->";
          }

          r += tmp;

          return r;
        }

        String
        access_seq (SemanticGraph::Attribute& a)
        {
          using namespace SemanticGraph;

          Complex& t (dynamic_cast<Complex&> (a.scope ()));

          String r;

          if (!recursive (t))
          {
            r = L"this->";
            r += epstate (t);
            r += L".";
          }
          else
          {
            r = L"static_cast< ";
            r += epstate_type (t);
            r += L"* > (this->";
            r += epstate (t);
            r += L".top ())->";
          }

          r += epstate_member (t);
          r += fixed_length (t) ? L"." : L"->";

          return r;
        }
      };

      //
      // Test for presence of var-length compositor in choice
      // that need initialization.
      //

      struct CompositorTest: Traversal::Compositor
      {
        CompositorTest (Boolean& p)
            : p_ (p)
        {
        }

        virtual Void
        traverse (SemanticGraph::Compositor& c)
        {
          // We are only interested in var-length required compositors.
          //
          if (c.max () == 1 &&
              c.min () == 1 &&
              !c.context ().get<Boolean> ("fixed"))
            p_ = true;
        }

      private:
        Boolean& p_;
      };

      //
      // Callbacks.
      //

      struct CompositorCallback: Traversal::All,
                                 Traversal::Choice,
                                 Traversal::Sequence,
                                 ParserContext
      {
        CompositorCallback (Context& c)
            : ParserContext (c), compositor_test_ (init_)
        {
        }

        virtual Void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}.
          //
          if (a.min () == 0)
          {
            String const& s (epimpl_custom (scope (a)));

            os << "void " << s << "::" << endl
               << eppresent (a) << " ()"
               << "{";

            String access (access_seq (a));

            if (fixed_length (a))
              os << access << epresent (a) << " (true);";
            else
            {
              String const& name (ename (a));
              String const& type (etype (a));
              String const& scope (fq_scope (a));

              if (exceptions)
                os << access << name << " (new " <<
                  scope << "::" << type << ");";
              else
                os << scope << "::" << type << "* x = new " <<
                  scope << "::" << type << ";"
                   << "if (x)" << endl
                   << access << name << " (x);"
                   << "else" << endl
                   << "this->_sys_error (::xsde::cxx::sys_error::no_memory);";
            }

            os << "}";
          }

          Traversal::All::traverse (a);
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          String const& s (epimpl_custom (scope (c)));
          String const& access (access_seq (c));
          String const& type_scope (fq_scope (c));

          os << "void " << s << "::" << endl
             << eparm (c) << " (" << eparm_tag (c) << " t)"
             << "{";

          if (c.max () != 1)
          {
            String const& name (ename (c));
            String const& type (etype (c));
            String const& access_s (access_seq (c, false));
            String const& ptr (epstate_member (c));

            if (fixed_length (c))
            {
              if (exceptions)
                os << access_s << name << " ().push_back (" <<
                  type_scope << "::" << type << " ());";
              else
                os << "if (" << access_s << name << " ().push_back (" <<
                  type_scope << "::" << type << " ()))"
                   << "{"
                   << "this->_sys_error (::xsde::cxx::sys_error::no_memory);"
                   << "return;"
                   << "}";

              os << access << ptr << " = &" << access_s << name <<
                " ().back ();";
            }
            else
            {
              os << access << ptr << " = new " << type_scope << "::" <<
                type << ";";

              if (exceptions)
                os << access_s << name << " ().push_back (" <<
                  access << ptr << ");";
              else
                os << "if (!" << access << ptr << " ||" << endl
                   << access_s << name << " ().push_back (" <<
                  access << ptr << "))"
                   << "{"
                   << "this->_sys_error (::xsde::cxx::sys_error::no_memory);"
                   << "return;"
                   << "}";
            }

            os << access << ptr << "->";
          }
          else if (c.min () == 0)
          {
            String const& name (ename (c));

            if (fixed_length (c))
              os << access << epresent (c) << " (true);";
            else
            {
              String const& type (etype (c));

              if (exceptions)
                os << access << name << " (new " << type_scope << "::" <<
                  type << ");";
              else
                os << type_scope << "::" << type << "* x = new " <<
                  type_scope << "::" << type << ";"
                   << "if (x)" << endl
                   << access << name << " (x);"
                   << "else"
                   << "{"
                   << "this->_sys_error (::xsde::cxx::sys_error::no_memory);"
                   << "return;"
                   << "}";
            }

            os << access << name << " ().";
          }
          else
          {
            // We may be in a choice in which case we get a nested
            // type (and accessor function) even for min == max == 1.
            //
            if (c.context ().count ("type"))
              os << access << ename (c) << " ().";
            else
              os << access;
          }

          os << earm (c) << " (" << endl
             << "static_cast< " << type_scope;

          if (c.context ().count ("type"))
            os << "::" << etype (c);

          os << "::" << earm_tag (c) << " > (t));"
             << endl;

          // Test whether we have any arms that need initialization.
          // Those are var-length required compositors.
          //
          init_ = false;

          for (SemanticGraph::Choice::ContainsIterator
                 i (c.contains_begin ()), e (c.contains_end ());
               !init_ && i != e; ++i)
          {
            compositor_test_.dispatch (i->particle ());
          }

          if (init_)
          {
            os << "switch (t)"
               << "{";

            for (SemanticGraph::Choice::ContainsIterator
                   i (c.contains_begin ()), e (c.contains_end ());
                 i != e; ++i)
            {
              // Test if this arm needs initialization.
              //
              init_ = false;
              compositor_test_.dispatch (i->particle ());

              if (init_)
              {
                SemanticGraph::Compositor& p (
                  dynamic_cast<SemanticGraph::Compositor&> (
                    i->particle ()));

                os << "case " << eptag (p) << ":"
                   << "{";

                String const& type (etype (p));
                String const& scope (fq_scope (p));

                if (exceptions)
                  os << access_seq (p) << ename (p) <<
                    " (new " << scope << "::" << type << ");";
                else
                  os << scope << "::" << type << "* x = new " <<
                    scope << "::" << type << ";"
                     << "if (x)" << endl
                     << access_seq (p) << ename (p) << " (x);"
                     << "else" << endl
                     << "this->_sys_error (::xsde::cxx::sys_error::no_memory);";

                os << "break;"
                   << "}";
              }
            }

            os << "default:"
               << "{"
               << "break;"
               << "}"
               << "}";
          }

          os << "}";

          Traversal::Choice::traverse (c);
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          if (s.max () != 1)
          {
            String const& sc (epimpl_custom (scope (s)));

            String const& access (access_seq (s));
            String const& access_s (access_seq (s, false));
            String const& name (ename (s));
            String const& type (etype (s));
            String const& scope (fq_scope (s));
            String const& ptr (epstate_member (s));

            os << "void " << sc << "::" << endl
               << epnext (s) << " ()"
               << "{";

            if (fixed_length (s))
            {
              if (exceptions)
                os << access_s << name << " ().push_back (" <<
                  scope << "::" << type << " ());";
              else
                os << "if (" << access_s << name << " ().push_back (" <<
                  scope << "::" << type << " ()))"
                   << "{"
                   << "this->_sys_error (::xsde::cxx::sys_error::no_memory);"
                   << "return;"
                   << "}";

              os << access << ptr << " = &" << access_s << name <<
                " ().back ();";
            }
            else
            {
              os << access << ptr << " = new " << scope << "::" <<
                type << ";";

              if (exceptions)
                os << access_s << name << " ().push_back (" <<
                  access << ptr << ");";
              else
                os << "if (!" << access << ptr << " ||" << endl
                   << access_s << name << " ().push_back (" <<
                  access << ptr << "))" << endl
                   << "this->_sys_error (::xsde::cxx::sys_error::no_memory);";
            }

            os << "}";
          }
          else if (s.min () == 0)
          {
            String const& sc (epimpl_custom (scope (s)));

            os << "void " << sc << "::" << endl
               << eppresent (s) << " ()"
               << "{";

            String access (access_seq (s));

            if (fixed_length (s))
              os << access << epresent (s) << " (true);";
            else
            {
              String const& name (ename (s));
              String const& type (etype (s));
              String const& scope (fq_scope (s));

              if (exceptions)
                os << access << name << " (new " <<
                  scope << "::" << type << ");";
              else
                os << scope << "::" << type << "* x = new " <<
                  scope << "::" << type << ";"
                   << "if (x)" << endl
                   << access << name << " (x);"
                   << "else" << endl
                   << "this->_sys_error (::xsde::cxx::sys_error::no_memory);";
            }

            os << "}";
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

      private:
        Boolean init_;
        CompositorTest compositor_test_;
      };

      struct ParticleCallback: Traversal::Element, ParserContext
      {
        ParticleCallback (Context& c)
            : ParserContext (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          using SemanticGraph::Complex;

          String const& name (epname (e));
          String const& arg (parg_type (e.type ()));
          Complex& c (dynamic_cast<Complex&> (e.scope ()));

          os << "void " << epimpl_custom (c) << "::" << endl
             << name << " (";

          if (arg != L"void")
          {
            os << arg << " x)"
               << "{";

            if (e.max () != 1)
            {
              if (exceptions)
                os << access_seq (e) << ename (e) << " ().push_back (x);";
              else
                os << "if (" << access_seq (e) << ename (e) <<
                  " ().push_back (x))" << endl
                   << "this->_sys_error (::xsde::cxx::sys_error::no_memory);";
            }
            else
            {
              os << access_seq (e) << ename (e) << " (x);";
            }

            os << "}";
          }
          else
          {
            os << ")"
               << "{"
               << "}";
          }
        }
      };

      struct AttributeCallback: Traversal::Attribute, ParserContext
      {
        AttributeCallback (Context& c)
            : ParserContext (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          using SemanticGraph::Complex;

          String const& name (epname (a));
          String const& arg (parg_type (a.type ()));
          Complex& c (dynamic_cast<Complex&> (a.scope ()));

          os << "void " << epimpl_custom (c) << "::" << endl
             << name << " (";

          if (arg != L"void")
          {
            os << arg << " x)"
               << "{"
               << access_seq (a) << ename (a) << " (x);"
               << "}";
          }
          else
          {
            os << ")"
               << "{"
               << "}";
          }
        }
      };

      //
      //
      struct PostOverride: Traversal::Complex, Context
      {
        PostOverride (Context& c)
            : Context (c), scope_ (0)
        {
        }

        virtual Void
        traverse (SemanticGraph::Complex& c)
        {
          Boolean clear (false);

          if (scope_ == 0)
          {
            scope_ = &c;
            clear = true;
          }

          if (c.inherits_p ())
          {
            SemanticGraph::Type& b (c.inherits ().base ());

            if (polymorphic (b))
            {
              if (tiein)
                dispatch (b);

              String const& scope (epimpl_custom (*scope_));

              os << pret_type (b) << " " << scope << "::" << endl
                 << post_name (b) << " ()"
                 << "{"
                 << "return this->" << post_name (c) << " ();"
                 << "}";
            }
          }

          if (clear)
            scope_ = 0;
        }

      private:
        SemanticGraph::Complex* scope_;
      };

      //
      //
      struct Complex: Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c),
              base_name_ (c, TypeName::base),
              post_override_ (c),
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

        virtual Void
        traverse (Type& c)
        {
          String const& name (epimpl_custom (c));

          if (!name)
            return;

          Boolean hb (c.inherits_p ());
          Boolean restriction (restriction_p (c));
          Boolean fixed (fixed_length (c));
          Boolean rec (recursive (c));

          Boolean validation (!options.value<CLI::suppress_validation> () &&
                              !options.value<CLI::suppress_parser_val> ());

          Boolean c_string_base (false);
          if (!stl && hb)
          {
            StringType test (c_string_base);
            test.dispatch (c.inherits ().base ());
          }

          String const& ret (pret_type (c));

          String const& state (epstate (c));
          String const& member (epstate_member (c));
          String const& state_type (epstate_type (c));
          String const& type (fq_name (c));

          String top_member;
          if (rec)
          {
            top_member = L"static_cast< " + state_type + L"* > (this->" +
              state + L".top ())->" + member;
          }


          os << "// " << name << endl
             << "//" << endl
             << endl;

          // c-tor
          //
          if (!fixed || (hb && tiein))
          {
            os << name << "::" << endl
               << name << " (" << (fixed ? "" : "bool b") << ")";

            String d ("\n: ");

            if (hb)
            {
              if (tiein)
              {
                os << d << epskel (c) << " (&base_impl_)";
                d = ",\n  ";
              }

              SemanticGraph::Type& b (c.inherits ().base ());

              // C-string-based built-in parser implementations don't
              // have c-tor (bool base).
              //
              if (!c_string_base && !fixed_length (b))
              {
                if (tiein)
                  os << d << "base_impl_" << " (true)";
                else
                  os << d << fq_name (b, "p:impl") << " (true)";

                d = ",\n  ";
              }
            }

            if (rec)
            {
              os << d << state << " (sizeof (" << state_type <<
                " ), &" << epstate_first (c) << ")";
            }

            os << "{";

            if (!fixed)
            {
              os << "this->" << epstate_base (c) << " = b;";

              if (!rec)
                os << "this->" << state << "." << member << " = 0;";
            }

            os << "}";
          }

          if (!fixed)
          {
            // d-tor
            //
            os << name << "::" << endl
               << "~" << name << " ()"
               << "{";

            if (!rec)
            {
              os << "if (!this->" << epstate_base (c) << ")" << endl
                 << "delete this->" << state << "." << member << ";";
            }
            else
            {
              os << "for (; !this->" << state << ".empty (); " <<
                "this->" << state << ".pop ())"
                 << "{"
                 << "if (!this->" << epstate_base (c) << ")" << endl
                 << "delete " << top_member << ";"
                 << "}";
            }

            os << "}";

            // reset
            //
            if (reset)
            {
              os << "void " << name << "::" << endl
                 << "_reset ()"
                 << "{";

              if (mixin  && hb)
                os << epimpl (c); //@@ fq-name
              else
                os << epskel (c); //@@ fq-name

              os << "::_reset ();"
                 << endl;

              if (!rec)
              {
                os << "if (!this->" << epstate_base (c) << ")"
                   << "{"
                   << "delete this->" << state << "." << member << ";"
                   << "this->" << state << "." << member << " = 0;"
                   << "}";
              }
              else
              {
                // Same code as in d-tor.
                //
                os << "for (; !this->" << state << ".empty (); " <<
                  "this->" << state << ".pop ())"
                   << "{"
                   << "if (!this->" << epstate_base (c) << ")" << endl
                   << "delete " << top_member << ";"
                   << "}";
              }

              os << "}";
            }
          }

          // pre_impl
          //
          if (!fixed)
          {
            os << "void " << name << "::" << endl
               << pre_impl_name (c) << " (" << type << "* x)"
               << "{";

            if (!rec)
              os << "this->" << state << "." << member << " = x;";
            else
            {
              // If we are recursive but our base is not, we need
              // to call _post() and post() to end parsing and copy
              // the result in case of recursion.
              //
              if (hb && !recursive (c.inherits ().base ()))
              {
                SemanticGraph::Type& b (c.inherits ().base ());

                os << "if (!this->" << state << ".empty ())"
                   << "{";

                if (tiein)
                  os << "this->base_impl_.";
                else
                  os << epimpl (b) << "::"; //@@ fq-name.

                os << "_post ();";

                if (!exceptions || validation)
                {
                  os << endl
                     << "if (this->_context ().error_type ())" << endl
                     << "return;"
                     << endl;
                }

                // The following code is similar to what we have in post().
                //

                // Default parser implementations for anyType and
                // anySimpleType return void.
                //
                if (!b.is_a<SemanticGraph::AnyType> () &&
                    !b.is_a<SemanticGraph::AnySimpleType> ())
                {
                  // If our base is a fixed-length type then copy the data
                  // over. Note that it cannot be a C-string.
                  //
                  if (fixed_length (b))
                  {
                    os << "static_cast< ";

                    base_name_.dispatch (b);

                    os << "& > (" << endl
                       << "*" << top_member << ") = " << endl;
                  }

                  if (tiein)
                    os << "this->base_impl_.";
                  else
                    os << epimpl (b) << "::"; //@@ fq-name.

                  os << post_name (b) << " ();";
                }

                os << "}"
                   << "else" << endl
                   << "this->" << epstate_top (c) << " = true;"
                   << endl;
              }

              if (exceptions)
                os << "this->" << state << ".push ();";
              else
                os << "if (this->" << state << ".push ())" << endl
                   << "this->_sys_error (::xsde::cxx::sys_error::no_memory);"
                   << endl;

              os << top_member << " = x;";
            }

            // Call base pre_impl (var-length) or pre (fix-length). C-string-
            // based built-in parser implementations don't have pre_impl().
            //
            if (hb && !c_string_base)
            {
              SemanticGraph::Type& b (c.inherits ().base ());

              if (tiein)
                os << "this->base_impl_.";
              else
                os << epimpl (b) << "::"; //@@ fq-name.

              if (fixed_length (b))
                os << "pre ();";
              else
                os << pre_impl_name (b) << " (x);";
            }

            os << "}";
          }

          // pre
          //
          os << "void " << name << "::" << endl
             << "pre ()"
             << "{";

          if (fixed)
          {
            if (hb)
            {
              // Our base is also fixed-length so call its pre()
              //
              if (tiein)
                os << "this->base_impl_.";
              else
                os << epimpl (c.inherits ().base ()) << "::"; //@@ fq-name.

              os << "pre ();";
            }

            os << "this->" << state << "." << member << " = " <<
              type << " ();";
          }
          else
          {
            if (exceptions)
              os << "this->" << pre_impl_name (c) << " (new " << type << ");";
            else
              os << type << "* x = new " << type << ";"
                 << "if (x)" << endl
                 << "this->" << pre_impl_name (c) << " (x);"
                 << "else" << endl
                 << "this->_sys_error (::xsde::cxx::sys_error::no_memory);";
          }

          os << "}";

          // Parser callbacks.
          //
          if (!restriction)
          {
            names (c, names_attribute_callback_);
            contains_compositor (c, contains_compositor_callback_);
          }

          // _post
          //
          if (rec && hb && !recursive (c.inherits ().base ()))
          {
            // If we are recursive but our base is not, we only call
            // base _post() if it is the first _post call.
            //
            os << "void " << name << "::" << endl
               << "_post ()"
               << "{"
               << "if (this->" << epstate_top (c) << ")" << endl;

            if (tiein)
              os << "this->base_impl_.";
            else
              os << epimpl (c.inherits ().base ()) << "::"; //@@ fq-name.

            os << "_post ();"
               << "}";
          }

          // post
          //
          if (polymorphic (c))
            post_override_.dispatch (c);

          os << ret << " " << name << "::" << endl
             << post_name (c) << " ()"
             << "{";

          if (hb)
          {
            SemanticGraph::Type& b (c.inherits ().base ());

            // Default parser implementations for anyType and
            // anySimpleType return void.
            //
            if (!b.is_a<SemanticGraph::AnyType> () &&
                !b.is_a<SemanticGraph::AnySimpleType> ())
            {
              // If we are recursive but our base is not, we only call
              // base post() if it is the first post call.
              //
              if (rec && !recursive (b))
              {
                os << "if (this->" << epstate_top (c) << ")"
                   << "{"
                   << "this->" << epstate_top (c) << " = false;";
              }

              // If our base is a fixed-length type or C-string-base, then
              // copy the data over.
              //
              if (fixed_length (b))
              {
                os << "static_cast< ";

                base_name_.dispatch (b);

                os << "& > (";

                if (!rec)
                  os << (fixed ? "" : "*") << "this->" << state << "." <<
                    member;
                else
                  os << endl
                     << "*" << top_member;

                os << ") = " << endl;
              }

              if (c_string_base)
              {
                os << "static_cast< ";

                base_name_.dispatch (b);

                os << "* > (";

                if (!rec)
                  os << "this->" << state << "." << member;
                else
                  os << top_member;

                os << ")->base_value (" << endl;
              }

              if (tiein)
                os << "this->base_impl_.";
              else
                os << epimpl (b) << "::"; //@@ fq-name.

              os << post_name (b) << " ()";

              if (c_string_base)
                os << ")";

              os << ";";

              if (rec && !recursive (b))
                os << "}";
            }
          }

          if (fixed)
            os << "return this->" << state << "." << member << ";";
          else
          {
            if (!rec)
              os << type << "* r = this->" << state << "." << member << ";"
                 << "this->" << state << "." << member << " = 0;";
            else
              os << type << "* r = " << top_member << ";"
                 << "this->" << state << ".pop ();";

            os << "return r;";
          }

          os << "}";
        }

      private:
        TypeName base_name_;
        PostOverride post_override_;

        CompositorCallback compositor_callback_;
        ParticleCallback particle_callback_;
        Traversal::ContainsCompositor contains_compositor_callback_;
        Traversal::ContainsParticle contains_particle_callback_;

        AttributeCallback attribute_callback_;
        Traversal::Names names_attribute_callback_;
      };
    }

    Void
    generate_parser_source (Context& ctx)
    {
      {
        // Emit "weak" header includes for the object model types.
        // Otherwise we cannot delete the objects of forward-declared
        // classes in the parser implementations.
        //
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

      names >> list;
      names >> union_;
      names >> complex;

      schema.dispatch (ctx.schema_root);
    }
  }
}
