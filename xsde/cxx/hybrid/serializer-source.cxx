// file      : xsde/cxx/hybrid/serializer-source.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/hybrid/serializer-source.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      // Some built-in types are passed by pointer to built-in serializer
      // implementations.
      //
      struct TypePass: Traversal::Fundamental::NameTokens,
                       Traversal::Fundamental::QName,
                       Traversal::Fundamental::IdRefs,
                       Traversal::Fundamental::Base64Binary,
                       Traversal::Fundamental::HexBinary,
                       Traversal::Fundamental::Entities,
                       Context
      {
        TypePass (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NameTokens&)
        {
          os << "&";
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::QName&)
        {
          if (!stl)
            os << "&";
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::IdRefs&)
        {
          os << "&";
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Base64Binary&)
        {
          os << "&";
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::HexBinary&)
        {
          os << "&";
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Entities&)
        {
          os << "&";
        }
      };

      //
      //
      struct PreOverride: Traversal::Complex, Context
      {
        PreOverride (Context& c)
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

              String const& scope (esimpl_custom (*scope_));

              os << "void " << scope << "::" << endl
                 << "pre (" << sarg_type (b) << " x)"
                 << "{"
                 << "this->pre (static_cast< " << sarg_type (c) << " > (x));"
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
      struct Enumeration: Traversal::Enumeration, Context
      {
        Enumeration (Context& c, Traversal::Complex& complex)
            : Context (c),
              complex_ (complex),
              pre_override_ (c),
              type_pass_(c)
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

          String const& name (esimpl_custom (e));

          if (!name)
            return;

          String state;

          if (!base_enum)
            state = esstate (e);

          os << "// " << name << endl
             << "//" << endl
             << endl;

          // c-tor
          //
          if (tiein)
          {
            os << name << "::" << endl
               << name << " ()" << endl
               << ": " << esskel (e) << " (" <<
              (base_enum ? "&base_impl_" : "0") << ")"
               << "{"
               << "}";
          }

          // pre
          //
          String const& arg (sarg_type (e));

          if (polymorphic (e))
            pre_override_.dispatch (e);

          os << "void " << name << "::" << endl
             << "pre (" << arg << " x)"
             << "{";

          if (base_enum)
          {
            SemanticGraph::Type& b (e.inherits ().base ());

            if (tiein)
              os << "this->base_impl_.pre (";
            else
              os << esimpl (b) << "::pre (";

            type_pass_.dispatch (b);

            os << "x);";
          }
          else
            os << "this->" << state << " = &x;";

          os << "}";

          // _serialize_content
          //
          if (!base_enum)
          {
            String const& string (e.context ().get<String> ("string"));

            os << "void " << name << "::" << endl
               << "_serialize_content ()"
               << "{";

            if (!options.value<CLI::suppress_validation> () &&
                !options.value<CLI::suppress_serializer_val> ())
            {
              // Do facet validation.
              //
              os << "if (::xsde::cxx::serializer::validating::" <<
                "string_common::validate_facets (" << endl
                 << "this->" << state << "->" << string << " ()," << endl
                 << "this->_facets ()," << endl
                 << "this->_context ()))" << endl;
            }

            os << "this->_characters (this->" << state << "->" <<
              string << " ());"
               << "}";
          }
        }

      private:
        Traversal::Complex& complex_;
        PreOverride pre_override_;
        TypePass type_pass_;
      };

      //
      //
      struct List: Traversal::List, Context
      {
        List (Context& c)
            : Context (c), type_pass_ (c)
        {
        }

        virtual Void
        traverse (Type& l)
        {
          String const& name (esimpl_custom (l));

          if (!name)
            return;

          SemanticGraph::Type& t (l.argumented ().type ());

          String const& skel (esskel (l));
          String const& arg (sarg_type (l));
          String const& ret (sret_type (t));

          String item (unclash (skel, "item"));
          String item_next (unclash (skel, "item_next"));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          String const& state (esstate (l));

          // pre
          //
          os << "void " << name << "::" << endl
             << "pre (" << arg << " x)"
             << "{"
             << "this->" << state << ".i_ = x.begin ();"
             << "this->" << state << ".end_ = x.end ();"
             << "}";

          // item_next
          //
          os << "bool " << name << "::" << endl
             << item_next << " ()"
             << "{"
             << "return this->" << state << ".i_ != this->" <<
            state << ".end_;"
             << "}";

          // item
          //
          os << ret << " " << name << "::" << endl
             << item << " ()"
             << "{"
             << "return ";

          type_pass_.dispatch (t);

          os << "*this->" << state << ".i_++;"
             << "}";
        }

      private:
        TypePass type_pass_;
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
          String const& name (esimpl_custom (u));

          if (!name)
            return;

          String const& state (esstate (u));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          // pre
          //
          String const& arg (sarg_type (u));

          os << "void " << name << "::" << endl
             << "pre (" << arg << " x)"
             << "{"
             << "this->" << state << " = &x;"
             << "}";

          // _serialize_content
          //
          String const& value (u.context ().get<String> ("value"));

          os << "void " << name << "::" << endl
             << "_serialize_content ()"
             << "{";

          if (stl)
          {
            os << "const ::std::string& s = this->" << state << "->" <<
              value << " ();"
               << "this->_characters (s.c_str (), s.size ());";
          }
          else
            os << "this->_characters (" <<
              "this->" << state << "->" << value << " ());";

          os << "}";
        }
      };

      //
      //
      struct SerializerContext: Context
      {
        SerializerContext (Context& c)
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
            if (element && p.max () != 1)
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
                    String const& iter (esstate_member (*c));

                    if (!r)
                    {
                      r = iter;
                      r += L"->";
                    }
                    else
                    {
                      String tmp;
                      tmp.swap (r);
                      r = iter;
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
            String const& s (esstate_member (t));

            if (!r)
            {
              r = s;
              r += L"->";
            }
            else
            {
              String tmp;
              tmp.swap (r);
              r = s;
              r += L"->";
              r += tmp;
            }
          }

          String tmp;
          tmp.swap (r);

          if (!recursive (t))
          {
            r = L"this->";
            r += esstate (t);
            r += L".";
          }
          else
          {
            r = L"static_cast< ";
            r += esstate_type (t);
            r += L"* > (this->";
            r += esstate (t);
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
            r += esstate (t);
            r += L".";
          }
          else
          {
            r = L"static_cast< ";
            r += esstate_type (t);
            r += L"* > (this->";
            r += esstate (t);
            r += L".top ())->";
          }

          r += esstate_member (t);
          r += L"->";

          return r;
        }
      };

      //
      // Test for presence of sequences.
      //

      struct CompositorTest: Traversal::Choice,
                             Traversal::Sequence
      {
        CompositorTest (Boolean& seq)
            : seq_ (seq)
        {
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.max () != 1)
            seq_ = true;
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          if (s.max () != 1)
            seq_ = true;
          else if (s.min () == 1)
          {
            // Test nested particles of the see-through sequence.
            //
            Sequence::traverse (s);
          }
        }

      private:
        Boolean& seq_;
      };

      struct ParticleTest: Traversal::Element
      {
        ParticleTest (Boolean& seq)
            : seq_ (seq)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          if (e.max () != 1)
            seq_ = true;
        }

      private:
        Boolean& seq_;
      };

      //
      // State initializers.
      //

      struct CompositorInit: Traversal::Choice,
                             Traversal::Sequence,
                             SerializerContext
      {
        CompositorInit (Context& c)
            : SerializerContext (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.max () != 1)
          {
            String access (access_seq (c));
            String access_s (access_seq (c, false));
            String const& iter (esstate_member (c));
            String const& end (esstate_member_end (c));

            // Initialize the iterator.
            //
            os << access << end << " = " << endl
               << access_s << ename (c) << " ().end ();"
               << access << iter << " = " << endl
               << access << end << ";";
          }
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          if (s.max () != 1)
          {
            String access (access_seq (s));
            String access_s (access_seq (s, false));
            String const& iter (esstate_member (s));
            String const& end (esstate_member_end (s));

            // Initialize the iterator.
            //
            os << access << end << " = " << endl
               << access_s << ename (s) << " ().end ();"
               << access << iter << " = " << endl
               << access << end << ";";
          }
          else if (s.min () == 1)
          {
            // Initialize nested particles of the see-through sequence.
            //
            Sequence::traverse (s);
          }
        }
      };

      struct ParticleInit: Traversal::Element, SerializerContext
      {
        ParticleInit (Context& c)
            : SerializerContext (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          if (e.max () != 1)
          {
            String access (access_seq (e));
            String access_s (access_seq (e, false));
            String const& iter (esstate_member (e));
            String const& end (esstate_member_end (e));
            String const& name (ename (e));

            // Initialize the iterator.
            //
            os << access << iter << " = " << endl
               << access_s << name << " ().begin ();"
               << access << end << " = " << endl
               << access_s << name << " ().end ();";
          }
        }
      };

      //
      // Callbacks.
      //

      struct CompositorCallback: Traversal::All,
                                 Traversal::Choice,
                                 Traversal::Sequence,
                                 SerializerContext
      {
        CompositorCallback (Context& c, Traversal::ContainsParticle& init)
            : SerializerContext (c),
              init_ (init),
              compositor_test_ (seq_),
              particle_test_ (seq_)
        {
          compositor_test_ >> contains_particle_test_;
          contains_particle_test_ >> compositor_test_;
          contains_particle_test_ >> particle_test_;
        }

        virtual Void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}.
          //
          if (a.min () == 0)
          {
            String const& s (esimpl_custom (scope (a)));

            os << "bool " << s << "::" << endl
               << espresent (a) << " ()"
               << "{"
               << "return " << access_seq (a) << epresent (a) << " ();"
               << "}";
          }

          All::traverse (a);
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          SemanticGraph::Complex& t (scope (c));

          String access (access_seq (c));
          String const& s (esimpl_custom (t));
          String const& arm_tag (esarm_tag (c));

          if (c.max () != 1)
          {
            String access_s (access_seq (c, false));
            String const& iter (esstate_member (c));
            String const& end (esstate_member_end (c));

            // When iterating over a compositor sequence, there is no
            // simple place to increment the iterator. So we will need
            // to increment it in *_next() and make sure we handle the
            // first call in a special way.
            //
            os << "bool " << s << "::" << endl
               << esnext (c) << " ()"
               << "{"
               << "if (" << access << iter << " != " << endl
               << access << end << ")" << endl
               << access << iter << "++;"
               << "else" << endl
               << access << iter << " = " << endl
               << access_s << ename (c) << " ().begin ();"
               << endl
               << "return " << access << iter << " != " << endl
               << access << end << ";"
               << "}";

            os << esskel (t) << "::" << arm_tag << " " << s << "::" << endl
               << esarm (c) << " ()"
               << "{"
               << arm_tag << " t (static_cast< " << arm_tag << " > (" << endl
               << access << iter << "->" << earm (c) << " ()));";
          }
          else if (c.min () == 0)
          {
            os << "bool " << s << "::" << endl
               << espresent (c) << " ()"
               << "{"
               << "return " << access << epresent (c) << " ();"
               << "}";

            os << esskel (t) << "::" << arm_tag << " " << s << "::" << endl
               << esarm (c) << " ()"
               << "{"
               << arm_tag << " t (static_cast< " << arm_tag << " > (" << endl
               << access << ename (c) << " ()." << earm (c) << " ()));";
          }
          else
          {
            os << esskel (t) << "::" << arm_tag << " " << s << "::" << endl
               << esarm (c) << " ()"
               << "{"
               << arm_tag << " t (static_cast< " << arm_tag << " > (" << endl;


            // We may be in a choice in which case we get a nested
            // type (and accessor function) even for min == max == 1.
            //
            if (c.context ().count ("type"))
              os << access << ename (c) << " ()." << earm (c) << " ()));";
            else
              os << access << earm (c) << " ()));";
          }

          // Test whether we have any arms that need initialization.
          //
          seq_ = false;

          for (SemanticGraph::Choice::ContainsIterator
                 i (c.contains_begin ()), e (c.contains_end ());
               !seq_ && i != e; ++i)
          {
            contains_particle_test_.dispatch (*i);
          }

          if (seq_)
          {
            os << "switch (t)"
               << "{";

            for (SemanticGraph::Choice::ContainsIterator
                   i (c.contains_begin ()), e (c.contains_end ());
                 i != e; ++i)
            {
              // Test if this arm needs initialization.
              //
              seq_ = false;
              contains_particle_test_.dispatch (*i);

              if (seq_)
              {
                SemanticGraph::Particle& p (i->particle ());

                os << "case " << estag (p) << ":"
                   << "{";
                init_.dispatch (*i);
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

          os << "return t;"
             << "}";

          Choice::traverse (c);
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          String const& sc (esimpl_custom (scope (s)));

          if (s.max () != 1)
          {
            String access (access_seq (s));
            String access_s (access_seq (s, false));
            String const& iter (esstate_member (s));
            String const& end (esstate_member_end (s));

            // When iterating over a compositor sequence, there is no
            // simple place to increment the iterator. So we will need
            // to increment it in *_next() and make sure we handle the
            // first call in a special way.
            //
            os << "bool " << sc << "::" << endl
               << esnext (s) << " ()"
               << "{"
               << "if (" << access << iter << " != " << endl
               << access << end << ")" << endl
               << access << iter << "++;"
               << "else" << endl
               << access << iter << " = " << endl
               << access_s << ename (s) << " ().begin ();"
               << endl
               << "if (" << access << iter << " != " << endl
               << access << end << ")"
               << "{";

            Sequence::contains (s, init_);

            os << "return true;"
               << "}"
               << "else" << endl
               << "return false;"
               << "}";
          }
          else if (s.min () == 0)
          {
            os << "bool " << sc << "::" << endl
               << espresent (s) << " ()"
               << "{"
               << "if (" << access_seq (s) << epresent (s) << " ())"
               << "{";

            Sequence::contains (s, init_);

            os << "return true;"
               << "}"
               << "else" << endl
               << "return false;"
               << "}";
          }

          Sequence::traverse (s);
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
        Traversal::ContainsParticle& init_;

        Boolean seq_;
        CompositorTest compositor_test_;
        ParticleTest particle_test_;
        Traversal::ContainsParticle contains_particle_test_;
      };

      struct ParticleCallback: Traversal::Element, SerializerContext
      {
        ParticleCallback (Context& c)
            : SerializerContext (c), type_pass_ (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          String const& s (
            esimpl_custom (
              dynamic_cast<SemanticGraph::Complex&> (e.scope ())));

          SemanticGraph::Type& t (e.type ());
          String const& ret (sret_type (t));
          String access (access_seq (e));

          if (e.max () != 1)
          {
            String const& iter (esstate_member (e));

            os << "bool " << s << "::" << endl
               << esnext (e) << " ()"
               << "{"
               << "return " << access << iter << " != " << endl
               << access << esstate_member_end (e) << ";"
               << "}";

            os << ret << " " << s << "::" << endl
               << esname (e) << " ()"
               << "{";

            if (polymorphic (t))
            {
              String skel (fq_name (t, "s:impl"));

              if (stl)
              {
                os << "const ::std::string& dt = " << access << iter <<
                  "->_dynamic_type ();"
                   << "if (dt != " << skel << "::_static_type ())" << endl
                   << "this->_context ().type_id (dt.c_str ());"
                   << endl;
              }
              else
              {
                os << "const char* dt = " << access << iter <<
                  "->_dynamic_type ();"
                   << "if (strcmp (dt, " << skel <<
                  "::_static_type ()) != 0)" << endl
                   << "this->_context ().type_id (dt);"
                   << endl;
              }
            }

            if (ret != L"void")
            {
              os << "return ";
              type_pass_.dispatch (t);
              os << "*" << access << iter << "++;";
            }
            else
              os << access << iter << "++;";

            os << "}";
          }
          else
          {
            if (e.min () == 0)
            {
              os << "bool " << s << "::" << endl
                 << espresent (e) << " ()"
                 << "{"
                 << "return " << access << epresent (e) << " ();"
                 << "}";
            }

            os << ret << " " << s << "::" << endl
               << esname (e) << " ()"
               << "{";

            if (polymorphic (t))
            {
              String skel (fq_name (t, "s:impl"));

              if (stl)
              {
                os << "const ::std::string& dt = " << access << ename (e) <<
                  " ()._dynamic_type ();"
                   << "if (dt != " << skel << "::_static_type ())" << endl
                   << "this->_context ().type_id (dt.c_str ());"
                   << endl;
              }
              else
              {
                os << "const char* dt = " << access << ename (e) <<
                  " ()._dynamic_type ();"
                   << "if (strcmp (dt, " << skel <<
                  "::_static_type ()) != 0)" << endl
                   << "this->_context ().type_id (dt);"
                   << endl;
              }
            }

            if (ret != L"void")
            {
              os << "return ";
              type_pass_.dispatch (t);
              os << access << ename (e) << " ();";
            }

            os << "}";
          }
        }

      private:
        TypePass type_pass_;
      };

      struct AttributeCallback: Traversal::Attribute, SerializerContext
      {
        AttributeCallback (Context& c)
            : SerializerContext (c), type_pass_ (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          String const& s (
            esimpl_custom (
              dynamic_cast<SemanticGraph::Complex&> (a.scope ())));

          String access (access_seq (a));

          if (a.optional_p ())
          {
            os << "bool " << s << "::" << endl
               << espresent (a) << " ()"
               << "{";

            if (a.default_p ())
            {
              Boolean omit (options.value<CLI::omit_default_attributes> ());

              if (a.fixed_p ())
                os << "return " << (omit ? "false" : "true") << ";";
              else
              {
                if (omit)
                  os << "return !" << access << edefault (a) << " ();";
                else
                  os << "return true;";
              }
            }
            else
              os << "return " << access << epresent (a) << " ();";

            os << "}";
          }

          SemanticGraph::Type& t (a.type ());
          String const& ret (sret_type (t));

          os << ret << " " << s << "::" << endl
             << esname (a) << " ()"
             << "{";

          if (ret != L"void")
          {
            os << "return ";
            type_pass_.dispatch (t);
              os << access << ename (a) << " ();";
          }

          os << "}";
        }

      private:
        TypePass type_pass_;
      };

      //
      //
      struct Complex: Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c),
              type_pass_ (c),
              pre_override_ (c),

              // Initializers.
              //
              compositor_init_ (c),
              particle_init_ (c),

              // Callbacks.
              //
              compositor_callback_ (c, contains_particle_init_),
              particle_callback_ (c),
              attribute_callback_ (c)
        {
          // Initializers.
          //
          contains_compositor_init_ >> compositor_init_;
          compositor_init_ >> contains_particle_init_;
          contains_particle_init_ >> compositor_init_;
          contains_particle_init_ >> particle_init_;

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
          String const& name (esimpl_custom (c));

          if (!name)
            return;

          Boolean hb (c.inherits_p ());
          Boolean rec (recursive (c));
          Boolean restriction (restriction_p (c));

          Boolean validation (!options.value<CLI::suppress_validation> () &&
                              !options.value<CLI::suppress_serializer_val> ());

          String state;
          String member;
          String state_type;

          String top_member;

          if (!restriction)
          {
            state = esstate (c);
            member = esstate_member (c);
            state_type = esstate_type (c);

            if (rec)
            {
              top_member = L"static_cast< " + state_type + L"* > (this->" +
                state + L".top ())->" + member;
            }
          }

          os << "// " << name << endl
             << "//" << endl
             << endl;

          // c-tor
          //
          if ((rec && !restriction) || (tiein && hb))
          {
            os << name << "::" << endl
               << name << " ()";

            String d ("\n: ");

            if (tiein && hb)
            {
              os << d << esskel (c) << " (&base_impl_)";
              d = ",\n  ";
            }

            if (rec && !restriction)
            {
              os << d << state << " (sizeof (" << state_type <<
                " ), &" << esstate_first (c) << ")";
            }

            os << "{"
               << "}";
          }

          // pre
          //
          if (polymorphic (c))
            pre_override_.dispatch (c);

          String const& arg (sarg_type (c));

          os << "void " << name << "::" << endl
             << "pre (" << arg << " x)"
             << "{";

          if (hb)
          {
            SemanticGraph::Type& b (c.inherits ().base ());

            // If we are recursive but our base is not, we need to call
            // _post() and post() to end serialization. Note that if we
            // are recursive and this is inheritance by restriction, our
            // base must be recursive.
            //
            if (rec && !recursive (b))
            {
              os << "if (!this->" << state << ".empty ())"
                 << "{";

              if (tiein)
                os << "this->base_impl_.";
              else
                os << esimpl (b) << "::";

              os << "_post ();";

              if (!exceptions || validation)
              {
                os << endl
                   << "if (this->_context ().error_type ())" << endl
                   << "return;"
                   << endl;
              }

              if (tiein)
                os << "this->base_impl_.";
              else
                os << esimpl (b) << "::";

              os << "post ();";

              os << "}"
                 << "else" << endl
                 << "this->" << esstate_top (c) << " = true;"
                 << endl;
            }

            // Call base pre().
            //
            if (tiein)
              os << "this->base_impl_.pre (";
            else
              os << esimpl (b) << "::pre (";

            type_pass_.dispatch (b);

            os << "x);";
          }

          if (!restriction)
          {
            if (!rec)
              os << "this->" << state << "." << member << " = &x;";
            else
            {
              if (exceptions)
                os << "this->" << state << ".push ();";
              else
                os << "if (this->" << state << ".push ())" << endl
                   << "this->_sys_error (::xsde::cxx::sys_error::no_memory);"
                   << endl;

              os << top_member << " = &x;";
            }

            contains_compositor (c, contains_compositor_init_);
          }

          os << "}";

          // Member callbacks.
          //
          if (!restriction)
          {
            names (c, names_attribute_callback_);
            contains_compositor (c, contains_compositor_callback_);
          }

          if (rec && !restriction)
          {
            // _post
            //
            if (hb && !recursive (c.inherits ().base ()))
            {
              // If we are recursive but our base is not, we only call
              // base _post() if it is the first _post call.
              //
              os << "void " << name << "::" << endl
                 << "_post ()"
                 << "{"
                 << "if (this->" << esstate_top (c) << ")" << endl;

              if (tiein)
                os << "this->base_impl_.";
              else
                os << esimpl (c.inherits ().base ()) << "::";

              os << "_post ();"
                 << "}";
            }

            // post
            //
            os << "void " << name << "::" << endl
               << "post ()"
               << "{"
               << "this->" << state << ".pop ();";

            if (hb)
            {
              SemanticGraph::Type& b (c.inherits ().base ());

              // If we are recursive but our base is not, we only call
              // base post() if it is the first post call.
              //
              if (!recursive (b))
              {
                os << "if (this->" << esstate_top (c) << ")"
                   << "{"
                   << "this->" << esstate_top (c) << " = false;";
              }

              if (tiein)
                os << "this->base_impl_.";
              else
                os << esimpl (c.inherits ().base ()) << "::";

              os << "post ();";

              if (!recursive (b))
                os << "}";
            }

            os << "}";
          }

          // reset
          //
          if (reset && ((rec && !restriction) ||
                        (mixin && recursive_base (c))))
          {
            os << "void " << name << "::" << endl
               << "_reset ()"
               << "{";

            if (mixin  && hb)
              os << esimpl (c.inherits ().base ()) << "::_reset ();";

            os << esskel (c) << "::_reset ();";

            if (rec && !restriction)
              os << "for (; !this->" << state << ".empty (); " <<
                "this->" << state << ".pop ()) ;"; // Space is for g++-4.3.

            os << "}";
          }
        }

      private:
        TypePass type_pass_;
        PreOverride pre_override_;

        // Initializers.
        //
        CompositorInit compositor_init_;
        ParticleInit particle_init_;
        Traversal::ContainsCompositor contains_compositor_init_;
        Traversal::ContainsParticle contains_particle_init_;

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
    generate_serializer_source (Context& ctx, Regex const& hxx_obj_expr)
    {
      if (ctx.poly_code && !ctx.stl)
        ctx.os << "#include <string.h>" << endl
               << endl;

      if (ctx.enum_ &&
          !ctx.options.value<CLI::suppress_validation> () &&
          !ctx.options.value<CLI::suppress_serializer_val> ())
      {
        // We need this functionality for enum mapping.
        //
        ctx.os << "#include <xsde/cxx/serializer/validating/string-common.hxx>" << endl
               << endl;
      }

      {
        // Emit "weak" header includes for the object model types.
        // Otherwise they will only be forward-declared.
        //
        Traversal::Schema schema;
        Includes includes (ctx, Includes::source, &hxx_obj_expr);

        schema >> includes;

        schema.dispatch (ctx.schema_root);
      }

      if (ctx.poly_code)
      {
        // Also emit "weak" header includes for the serializer
        // implementations. These are needed for the _static_type if we are
        // generating polymorphic code.
        //
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
      Enumeration enumeration (ctx, complex);

      names >> list;
      names >> union_;
      names >> complex;
      names >> enumeration;

      schema.dispatch (ctx.schema_root);
    }
  }
}
