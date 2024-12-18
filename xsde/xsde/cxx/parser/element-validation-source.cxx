// file      : xsde/cxx/parser/element-validation-source.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <vector>

#include <xsde/cxx/parser/element-validation-source.hxx>

#include <libxsd-frontend/semantic-graph.hxx>
#include <libxsd-frontend/traversal.hxx>

using namespace std;

namespace CXX
{
  namespace Parser
  {
    namespace
    {
      typedef vector<SemanticGraph::Particle*> Particles;

      // Find particle that can be absent.
      //
      struct OptionalParticleTest: Traversal::Choice
      {
        OptionalParticleTest (SemanticGraph::Particle*& result)
            : optional_ (false), is_optional_ (optional_), result_ (result)
        {
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          using SemanticGraph::Choice;
          result_ = 0;
          optional_ = false;

          for (Choice::ContainsIterator i (c.contains_begin ());
               result_ == 0 && i != c.contains_end ();
               ++i)
          {
            is_optional_.dispatch (i->particle ());
            if (optional_)
              result_ = &i->particle ();
          }
        }

        struct IsOptional: Traversal::Choice,
                           Traversal::Sequence,
                           Traversal::Element,
                           Traversal::Any
        {
          IsOptional (bool& r)
              : r_ (r)
          {
          }

          virtual void
          traverse (SemanticGraph::Choice& c)
          {
            if (!r_ && c.min () == 0)
              r_ = true;

            // We need at least one particle to be optional for the whole
            // choice to be optional.
            //
            using SemanticGraph::Choice;

            for (Choice::ContainsIterator i (c.contains_begin ());
                 !r_ && i != c.contains_end ();
                 ++i)
            {
              dispatch (i->particle ());
            }
          }

          virtual void
          traverse (SemanticGraph::Sequence& s)
          {
            if (!r_ && s.min () == 0)
              r_ = true;

            // We need all particles to be optional for the whole sequence
            // to be optional.
            //
            using SemanticGraph::Sequence;

            for (Sequence::ContainsIterator i (s.contains_begin ());
                 !r_ && i != s.contains_end ();
                 ++i)
            {
              bool r (false);
              IsOptional test (r);
              test.dispatch (i->particle ());
              if (!r)
                return;
            }

            r_ = true;
          }

          virtual void
          traverse (SemanticGraph::Element& e)
          {
            if (!r_ && e.min () == 0)
              r_ = true;
          }

          virtual void
          traverse (SemanticGraph::Any& a)
          {
            if (!r_ && a.min () == 0)
              r_ = true;
          }

        private:
          bool& r_;
        };

      private:
        bool optional_;
        IsOptional is_optional_;
        SemanticGraph::Particle*& result_;
      };

      //
      //
      void
      sequence_next_call (SemanticGraph::Sequence* s, Context* ctx);

      void
      choice_arm_call (SemanticGraph::Particle* p,
                       SemanticGraph::Choice* c,
                       Context* ctx)
      {
        using SemanticGraph::Choice;
        using SemanticGraph::Sequence;

        ctx->os << "this->" << Context::earm (*c) << " (" <<
          Context::etag (*p) << ");";

        if (Choice* pc = dynamic_cast<Choice*> (p))
        {
          if (pc->min () != 0)
          {
            OptionalParticleTest test (p);
            test.traverse (*pc);

            if (p)
              choice_arm_call (p, pc, ctx);
          }
        }
        else if (Sequence* ps = dynamic_cast<Sequence*> (p))
        {
          // Effective min should be 0 otherwise we wouldn't be here.
          //
          if (ps->min () != 0 && ps->max () != 1)
            sequence_next_call (ps, ctx);
        }
      }

      void
      sequence_next_call (SemanticGraph::Sequence* s, Context* ctx)
      {
        using SemanticGraph::Choice;
        using SemanticGraph::Sequence;

        ctx->os << "this->" << Context::enext (*s) << " ();";

        // See if we have any nested compositors that also need a
        // next or arm call.
        //
        for (Sequence::ContainsIterator ci (s->contains_begin ());
             ci != s->contains_end (); ++ci)
        {
          SemanticGraph::Particle* p (&ci->particle ());

          if (Sequence* ps = dynamic_cast<Sequence*> (p))
          {
            // Effective min should be 0 otherwise we wouldn't be here.
            //
            if (ps->min () != 0 && ps->max () != 1)
              sequence_next_call (ps, ctx);
          }
          else if (Choice* pc = dynamic_cast<Choice*> (p))
          {
            if (pc->min () != 0)
            {
              OptionalParticleTest test (p);
              test.traverse (*pc);

              if (p)
                choice_arm_call (p, pc, ctx);
            }
          }
        }
      }

      //
      //
      struct ParticleTest: Traversal::Compositor,
                           Traversal::Element,
                           Traversal::Any,
                           Context
      {
        ParticleTest (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          String const& name (e.name ());
          bool subst (poly_code && e.global_p ());

          if (subst)
            os << "(";

          if (e.qualified_p () && e.namespace_ ().name ())
          {
            String const& ns (e.namespace_ ().name ());

            os << "n == " << L << strlit (name) << " &&" << endl
               << "ns == " << L << strlit (ns);
          }
          else
            os << "n == " << L << strlit (name) << " && ns.empty ()";


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

        }

        virtual void
        traverse (SemanticGraph::Any& a)
        {
          String const& ns (a.definition_namespace ().name ());

          // Note that we need to make sure the "flush" element (both name
          // and namespace are empty) does not match any compositor.
          //
          for (SemanticGraph::Any::NamespaceIterator i (a.namespace_begin ()),
                 e (a.namespace_end ()); i != e;)
          {
            if (*i == L"##any")
            {
              os << "!n.empty ()";
            }
            else if (*i == L"##other")
            {
              if (ns)
              {
                // Note that here I assume that ##other does not include
                // unqualified names in a schema with target namespace.
                // This is not what the spec says but that seems to be
                // the consensus.
                //
                os << "(!ns.empty () && ns != " << L << strlit (ns) << ")";
              }
              else
                os << "!ns.empty ()";
            }
            else if (*i == L"##local")
            {
              os << "(ns.empty () && !n.empty ())";
            }
            else if (*i == L"##targetNamespace")
            {
              os << "ns == " << L << strlit (ns);
            }
            else
            {
              os << "ns == " << L << strlit (*i);
            }

            if (++i != e)
              os << " ||" << endl;
          }
        }

        virtual void
        traverse (SemanticGraph::Compositor& c)
        {
          // This compositor should already have been tested for
          // triviality (empty).
          //
          Particles const& p (c.context ().get<Particles> ("p:prefixes"));

          bool paren (p.size () != 1);

          for (Particles::const_iterator i (p.begin ()), e (p.end ());
               i != e;)
          {
            if (paren)
              os << "(";

            dispatch (**i);

            if (paren)
              os << ")";

            if (++i != e)
              os << " ||" << endl;
          }
        }
      };


      // Generates particle namespace-name pair. Used to generate
      // the _expected_element call.
      //
      struct ParticleName: Traversal::Compositor,
                           Traversal::Element,
                           Traversal::Any,
                           Context
      {
        ParticleName (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          String ns (e.qualified_p () ? e.namespace_ ().name () : String ());

          os << L << strlit (ns) << ", " << L << strlit (e.name ());
        }

        virtual void
        traverse (SemanticGraph::Any& a)
        {
          String const& ns (*a.namespace_begin ());

          os << L << strlit (ns) << ", " << L << "\"*\"";
        }

        virtual void
        traverse (SemanticGraph::Compositor& c)
        {
          Particles const& p (c.context ().get<Particles> ("p:prefixes"));

          dispatch (**p.begin ());
        }
      };


      // Common base for the ParticleIn{All, Choice, Sequence} treversers.
      //
      struct ParticleInCompositor: Context
      {
      protected:
        ParticleInCompositor (Context& c, SemanticGraph::Complex& type)
            : Context (c), type_ (type), particle_test_ (c)
        {
        }


        // Generate sub-parser setup code as well as the pre/post calls.
        //
        void
        pre_post_calls (SemanticGraph::Particle& p)
        {
          using SemanticGraph::Element;
          using SemanticGraph::Complex;

          if (Element* e = dynamic_cast<Element*> (&p))
          {
            SemanticGraph::Type& type (e->type ());
            bool poly (poly_code && !anonymous (type));

            String const& name (ename (*e));
            String fq_type (fq_name (type));

            String def_parser, map, inst, cast;

            if (poly)
            {
              def_parser = emember (*e);
              map = emember_map (*e);
              inst = "p";
            }
            else
              inst = L"this->" +  emember (*e);

            if (poly)
            {
              cast  = mixin ? L"dynamic_cast" : L"static_cast";

              os << fq_type << "* p = 0;"
                 << endl
                 << "if (t == 0 && this->" << def_parser << " != 0)" << endl
                 << inst << " = this->" << def_parser << ";"
                 << "else"
                 << "{"
                 << "const char* ts = " << fq_type << "::_static_type ();"
                 << endl
                 << "if (t == 0)" << endl
                 << "t = ts;"
                 << endl
                 << "if (this->" << def_parser << " != 0 && " <<
                "strcmp (t, ts) == 0)" << endl
                 << inst << " = this->" << def_parser << ";"
                 << "else"
                 << "{";

              // Check that the types are related by inheritance.
              //
              os << "if (t != ts &&" << endl
                 << "!::xsde::cxx::parser::validating::" <<
                "inheritance_map_instance ().check (t, ts))"
                 << "{"
                 << "ctx.schema_error (::xsde::cxx::schema_error::not_derived);"
                 << "return;"
                 << "}";

              os << "if (this->" << map << " != 0)" << endl
                 << inst << " = " << cast << "< " << fq_type << "* > (" << endl
                 << "this->" << map << "->find (t));"
                 << "}"
                 << "}";
            }

            String const& post (post_name (type));

            os << "if (" << inst << ")"
               << "{"
               << inst << "->pre ();";

            if (!exceptions)
            {
              os << endl
                 << "if (" << inst << "->_error_type ())" << endl
                 << inst << "->_copy_error (ctx);"
                 << endl;
            }

            os << "ctx.nested_parser (" << inst << ");" << endl
               << "}"
               << "}"
               << "else" // start
               << "{";

            if (poly)
              os << fq_type << "* p =" << endl
                 << cast << "< " << fq_type << "* > (ctx.nested_parser ());"
                 << endl;

            os << "if (" << inst << " != 0)"
               << "{";

            String const& ret (ret_type (type));

            if (ret == L"void")
              os << inst << "->" << post << " ();";
            else
              os << arg_type (type) << " tmp = " << inst << "->" <<
                post << " ();";

            if (!exceptions)
            {
              // Note that after post() we need to check both parser and
              // context error states because of the recursive parsing.
              //
              os << endl
                 << "if (" << inst << "->_error_type ())" << endl
                 << inst << "->_copy_error (ctx);"
                 << endl
                 << "if (!ctx.error_type ())" << endl;
            }

            if (ret == L"void")
              os << "this->" << name << " ();";
            else
              os << "this->" << name << " (tmp);";

            os << "}";
          }
          else
          {
            os << "ctx.start_wildcard_content ();"
               << "this->_start_any_element (ns, n" <<
              (poly_runtime ? (poly_code ? ", t" : ", 0") : "") << ");"
               << "}"
               << "else" // start
               << "{"
               << "this->_end_any_element (ns, n);";
          }
        }


      protected:
        SemanticGraph::Complex& type_;
        ParticleTest particle_test_;
      };



      // The 'all' compositor can only contain elements with min={0,1}, max=1.
      //
      struct ParticleInAll: Traversal::Element,
                            ParticleInCompositor
      {
        ParticleInAll (Context& c, SemanticGraph::Complex& type)
            : ParticleInCompositor (c, type)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          size_t state (e.context ().get<size_t> ("p:state"));

          SemanticGraph::Compositor& c (
            e.contained_particle ().compositor ());

          if (state != 0)
            os << "else ";

          os << "if (";

          particle_test_.traverse (e);

          os << ")"
             << "{"
             << "if (count[" << state << "UL] == 0)"
             << "{"
             << "if (start)"
             << "{";

          if (c.min () == 0)
          {
            // Make the call to _present if we haven't seen any
            // elements yet.
            //
            size_t state_count (c.context().get<size_t> ("p:state-count"));

            if (state_count > 1)
            {
              os << "if (";

              bool sub (false);

              for (size_t i (0); i < state_count; ++i)
              {
                if (i == state)
                  continue;

                if (sub)
                  os << " &&" << endl;
                else
                  sub = true;

                os << "count[" << i << "UL] == 0";
              }

              os << ")" << endl
                 << "this->" << epresent (c) << " ();"
                 << endl;
            }
          }

          pre_post_calls (e);

          os << "count[" << state << "UL] = 1;"
             << "}"
             << "}"
             << "else" // count != 0
             << "{"
             << "assert (start);" // Assuming well-formed XML.

            // Since there is never more content after 'all', we could have
            // as well thrown here. But instead we will let the code in
            // start_element handle this along with other unexpected
            // elements.
            //
             << "state = ~0UL;"
             << "}"
             << "}";
        }
      };


      //
      //
      struct ParticleInChoice: Traversal::Particle,
                               Traversal::Compositor,
                               ParticleInCompositor
      {
        ParticleInChoice (Context& c, SemanticGraph::Complex& type)
            : ParticleInCompositor (c, type), particle_name_ (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Particle& p)
        {
          using SemanticGraph::Element;

          size_t state (p.context ().get<size_t> ("p:state"));

          size_t min (p.min ()), max (p.max ());

          os << "case " << state << "UL:" << endl
             << "{";

          if (max != 1) // We don't need the test if max == 1.
          {
            os << "if (";

            particle_test_.dispatch (p);

            os << ")"
               << "{";
          }

          os << "if (start)"
             << "{";

          pre_post_calls (p);

          switch (max)
          {
          case 0:
            {
              os << "count++;";
              break;
            }
          case 1:
            {
              // We do not need to increment count because min <= max and
              // we do not generate min check for min <= 1 (see below).
              //
              os << "state = ~0UL;";
              break;
            }
          default:
            {
              os << "if (++count == " << max << "UL)" << endl
                 << "state = ~0UL;";
            }
          };

          os << "}"; // start

          // We've already moved to the final state if max == 1.
          //
          if (max != 1)
          {
            os << "}"
               << "else"
               << "{"
               << "assert (start);"; // Assuming well-formed XML

            // Check if min cardinality requirements have been met. Since
            // count is always >= 1, don't generate dead code if min <= 1.
            //
            if (min > 1)
            {
              os << "if (count < " << min << "UL)" << endl
                 << "this->_schema_error (" <<
                "::xsde::cxx::schema_error::expected_element);";

                /*
                 << "this->_expected_element (" << endl;
                 particle_name_.dispatch (p);
                 os << "," << endl
                 << "ns, n);";
                */
            }


            os << "state = ~0UL;"
               << "}";
          }

          os << "break;"
             << "}"; // case
        }

        virtual void
        traverse (SemanticGraph::Compositor& c)
        {
          using SemanticGraph::Choice;
          using SemanticGraph::Sequence;
          using SemanticGraph::Compositor;

          bool choice (c.is_a<Choice> ());
          SemanticGraph::Context& cc (c.context ());

          size_t max (c.max ());
          size_t min (cc.get<size_t> ("p:effective-min"));
          size_t n (cc.get<size_t> ("p:comp-number"));
          size_t state (cc.get<size_t> ("p:state"));

          String func (choice ? "choice_" : "sequence_");

          os << "case " << state << "UL:" << endl
             << "{"
             << "unsigned long s = ~0UL;"
             << endl;

          bool first (true);

          for (Compositor::ContainsIterator ci (c.contains_begin ());
               ci != c.contains_end (); ++ci)
          {
            SemanticGraph::Particle& p (ci->particle ());

            if (p.is_a<Compositor> () && !cc.count ("p:comp-number"))
              continue; // Empty compositor.

            if (!p.context ().count ("p:prefix"))
              break;

            size_t state (p.context ().get<size_t> ("p:state"));

            if (first)
              first = false;
            else
              os << "else ";

            os << "if (";

            particle_test_.dispatch (p);

            os << ")" << endl
               << "s = " << state << "UL;";
          }

          if (!choice)
          {
            for (Compositor::ContainsIterator ci (c.contains_begin ());
                 ci != c.contains_end (); ++ci)
            {
              SemanticGraph::Particle* p (&ci->particle ());

              if (Choice* pc = dynamic_cast<Choice*> (p))
              {
                if (pc->min () != 0 &&
                    pc->context ().get<size_t> ("p:effective-min") == 0)
                {
                  // This is a required choice with effective-min == 0 (i.e.,
                  // it contains optional particle). We need to call the arm
                  // callback with that optional particle's tag.
                  //
                  OptionalParticleTest test (p);
                  test.traverse (*pc);

                  if (p)
                  {
                    size_t state (pc->context ().get<size_t> ("p:state"));

                    os << endl
                       << "if (s > " << state << "UL)" << endl
                       << "{";
                    choice_arm_call (p, pc, this);
                    os << "}";
                  }
                }
              }
              else if (Sequence* ps = dynamic_cast<Sequence*> (p))
              {
                // This is a required sequence with effective-min == 0 (i.e.,
                // it contains optional particle) and maxOccurs > 1. We need
                // to call the next callback.
                //
                if (ps->min () != 0 && ps->max () != 1 &&
                    ps->context ().get<size_t> ("p:effective-min") == 0)
                {
                  size_t state (ps->context ().get<size_t> ("p:state"));

                  os << endl
                     << "if (s > " << state << "UL)" << endl
                     << "{";
                  sequence_next_call (ps, this);
                  os << "}";
                }
              }
            }
          }

          // This compositor.
          //
          os << endl
             << "if (s != ~0UL)"
             << "{"
             << "assert (start);"; // End is handled by the sub-machine.

          switch (max)
          {
          case 0:
            {
              os << "count++;";
              break;
            }
          case 1:
            {
              // We do not need to increment count because min <= max and
              // we do not generate min check for min <= 1 (see below).
              //
              os << "state = ~0UL;";
              break;
            }
          default:
            {
              os << "if (++count == " << max << "UL)" << endl
                 << "state = ~0UL;";
            }
          };

          // Delegate to the sub-machine and call _arm, _present, or _next
          // if necessary.
          //

          os << endl
             << "v_state_& vs = *static_cast< v_state_* > (" <<
            "this->v_state_stack_.top ());"
             << "v_state_descr_& vd = vs.data[vs.size++];" // push
             << endl
             << "vd.func = &" << ename (type_) << "::" << func << n << ";"
             << "vd.state = s;"
             << "vd.count = 0;"
             << endl;

          if (Choice* pc = dynamic_cast<Choice*> (&c))
          {
            os << "this->" << earm (*pc) << " (static_cast< " <<
              earm_tag (*pc) << " > (s));";
          }
          else
          {
            Sequence& s (dynamic_cast<Sequence&> (c));

            if (max != 1)
              os << "this->" << enext (s) << " ();";
            else if (c.min () == 0)
              os << "this->" << epresent (s) << " ();";
          }

          os << "this->" << func << n << " (vd.state, vd.count, ns, n, " <<
            (poly_runtime ? (poly_code ? "t, " : "0, ") : "") << "true);"
             << "}";


          // Not this compositor. We've elready moved to the final state
          // if max == 1.
          //
          if (max != 1)
          {
            os << "else"
               << "{"
               << "assert (start);"; // Assuming well-formed XML

            // Check if min cardinality requirements have been met. Since
            // count is always >= 1, don't generate dead code if min <= 1.
            //
            if (min > 1)
            {
              os << "if (count < " << min << "UL)" << endl
                 << "this->_schema_error (" <<
                "::xsde::cxx::schema_error::expected_element);";

                /*
                  << "this->_expected_element (" << endl;
                  particle_name_.dispatch (c);
                  os << "," << endl
                  << "ns, n);";
                */
            }

            os << "state = ~0UL;"
               << "}";
          }

          os << "break;"
             << "}"; // case
        }

      private:
        ParticleName particle_name_;
      };


      //
      //
      struct ParticleInSequence: Traversal::Particle,
                                 Traversal::Compositor,
                                 ParticleInCompositor
      {
        ParticleInSequence (Context& c,
                            size_t state,
                            size_t next_state,
                            SemanticGraph::Complex& type)
            : ParticleInCompositor (c, type),
              state_ (state), particle_name_ (c)
        {
          // next_state == 0 indicates the terminal state (~0UL).
          //
          if (next_state != 0)
          {
            std::wostringstream ostr;
            ostr << next_state;
            next_state_ = ostr.str ();
          }
          else
            next_state_ = L"~0";
        }

        virtual void
        traverse (SemanticGraph::Particle& p)
        {
          size_t min (p.min ()), max (p.max ());

          os << "case " << state_ << "UL:" << endl
             << "{"
             << "if (";

          particle_test_.dispatch (p);

          os << ")"
             << "{";

          // This element.
          //

          os << "if (start)"
             << "{";

          pre_post_calls (p);

          switch (max)
          {
          case 0:
            {
              os << "count++;";
              break;
            }
          case 1:
            {
              os << "count = 0;"
                 << "state = " << next_state_ << "UL;";
              break;
            }
          default:
            {
              os << "if (++count == " << max << "UL)"
                 << "{"
                 << "count = 0;"
                 << "state = " << next_state_ << "UL;"
                 << "}";
            }
          };

          os << "}" // start
             << "break;"
             << "}";

          // Not this element.
          //

          os << "else"
             << "{"
             << "assert (start);"; // Assuming well-formed XML.

          // Check if min cardinality requirements have been met. Since
          // count is always >= 0, don't generate dead code if min == 0.
          //
          if (min != 0)
          {
            os << "if (count < " << min << "UL)"
               << "{"
               << "this->_schema_error (" <<
              "::xsde::cxx::schema_error::expected_element);"
               << "break;"
               << "}";

            /*
              << "this->_expected_element (" << endl;
              particle_name_.dispatch (p);
              os << "," << endl
              << "ns, n);";
            */
          }

          os << "count = 0;"
             << "state = " << next_state_ << "UL;"
             << "// Fall through." << endl
             << "}"  // else
             << "}"; // case
        }

        virtual void
        traverse (SemanticGraph::Compositor& c)
        {
          using SemanticGraph::Choice;
          using SemanticGraph::Sequence;
          using SemanticGraph::Compositor;

          bool choice (c.is_a<Choice> ());
          SemanticGraph::Context& cc (c.context ());

          size_t max (c.max ());
          size_t min (cc.get<size_t> ("p:effective-min"));
          size_t n (cc.get<size_t> ("p:comp-number"));

          String func (choice ? "choice_" : "sequence_");

          os << "case " << state_ << "UL:" << endl
             << "{"
             << "unsigned long s = ~0UL;"
             << endl;

          bool first (true);

          for (Compositor::ContainsIterator ci (c.contains_begin ());
               ci != c.contains_end (); ++ci)
          {
            SemanticGraph::Particle& p (ci->particle ());

            if (p.is_a<Compositor> () && !cc.count ("p:comp-number"))
              continue; // Empty compositor.

            if (!p.context ().count ("p:prefix"))
              break;

            size_t state (p.context ().get<size_t> ("p:state"));

            if (first)
              first = false;
            else
              os << "else ";

            os << "if (";

            particle_test_.dispatch (p);

            os << ")" << endl
               << "s = " << state << "UL;";
          }

          if (!choice)
          {
            for (Compositor::ContainsIterator ci (c.contains_begin ());
                 ci != c.contains_end (); ++ci)
            {
              SemanticGraph::Particle* p (&ci->particle ());

              if (Choice* pc = dynamic_cast<Choice*> (p))
              {
                if (pc->min () != 0 &&
                    pc->context ().get<size_t> ("p:effective-min") == 0)
                {
                  // This is a required choice with effective-min == 0 (i.e.,
                  // it contains optional particle). We need to call the arm
                  // callback with that optional particle's tag.
                  //
                  OptionalParticleTest test (p);
                  test.traverse (*pc);

                  if (p)
                  {
                    size_t state (pc->context ().get<size_t> ("p:state"));

                    os << endl
                       << "if (s > " << state << "UL)" << endl
                       << "{";
                    choice_arm_call (p, pc, this);
                    os << "}";
                  }
                }
              }
              else if (Sequence* ps = dynamic_cast<Sequence*> (p))
              {
                // This is a required sequence with effective-min == 0 (i.e.,
                // it contains optional particle) and maxOccurs > 1. We need
                // to call the next callback.
                //
                if (ps->min () != 0 && ps->max () != 1 &&
                    ps->context ().get<size_t> ("p:effective-min") == 0)
                {
                  size_t state (ps->context ().get<size_t> ("p:state"));

                  os << endl
                     << "if (s > " << state << "UL)" << endl
                     << "{";
                  sequence_next_call (ps, this);
                  os << "}";
                }
              }
            }
          }

          // This element.
          //

          os << endl
             << "if (s != ~0UL)"
             << "{"
             << "assert (start);"; // End is handled by the sub-machine.

          switch (max)
          {
          case 0:
            {
              os << "count++;"
                 << endl;
              break;
            }
          case 1:
            {
              os << "count = 0;"
                 << "state = " << next_state_ << "UL;"
                 << endl;
              break;
            }
          default:
            {
              os << "if (++count == " << max << "UL)"
                 << "{"
                 << "count = 0;"
                 << "state = " << next_state_ << "UL;"
                 << "}";
            }
          };

          // Delegate to the sub-machine and call _arm, _present, or _next
          // if necessary.
          //

          os << "v_state_& vs = *static_cast< v_state_* > (" <<
            "this->v_state_stack_.top ());"
             << "v_state_descr_& vd = vs.data[vs.size++];" // push
             << endl
             << "vd.func = &" << ename (type_) << "::" << func << n << ";"
             << "vd.state = s;"
             << "vd.count = 0;"
             << endl;

          Sequence* ps (0);
          Choice* pc = dynamic_cast<Choice*> (&c);

          if (pc)
          {
            os << "this->" << earm (*pc) << " (static_cast< " <<
              earm_tag (*pc) << " > (s));";
          }
          else
          {
            ps = &dynamic_cast<Sequence&> (c);

            if (max != 1)
              os << "this->" << enext (*ps) << " ();";
            else if (c.min () == 0)
              os << "this->" << epresent (*ps) << " ();";
          }

          os << "this->" << func << n << " (vd.state, vd.count, ns, n, " <<
            (poly_runtime ? (poly_code ? "t, " : "0, ") : "") << "true);"
             << "break;"
             << "}";

          // Not this compositor.
          //

          os << "else"
             << "{"
             << "assert (start);"; // Assuming well-formed XML

          if (pc)
          {
            if (c.min () != 0 && min == 0)
            {
              // This is a required choice with effective-min == 0 (i.e.,
              // it contains optional particle). We need to call the arm
              // callback with that optional particle's tag.
              //
              SemanticGraph::Particle* p (0);
              OptionalParticleTest test (p);
              test.traverse (*pc);

              if (p)
              {
                // For a sequence only make the call if there were no
                // elements.
                //
                if (max != 1)
                  os << "if (count == 0)"
                     << "{";
                choice_arm_call (p, pc, this);
                if (max != 1)
                  os << "}";
              }
            }
          }
          else
          {
            if (ps->min () != 0 && min == 0 && max != 1)
            {
              // This is a required sequence with effective-min == 0 (i.e.,
              // it contains optional particle) and maxOccurs > 1. We need
              // to call the next callback.
              //

              // Make the call if there were no elements.
              //
              os << "if (count == 0)"
                 << "{";
              sequence_next_call (ps, this);
              os << "}";
            }
          }

          // Check if min cardinality requirements have been met. Since
          // count is always >= 0, don't generate dead code if min == 0.
          //
          if (min != 0)
          {
            os << "if (count < " << min << "UL)"
               << "{"
               << "this->_schema_error (" <<
              "::xsde::cxx::schema_error::expected_element);"
               << "break;"
               << "}";
          }

          os << "count = 0;"
             << "state = " << next_state_ << "UL;"
             << "// Fall through." << endl
             << "}"  // else
             << "}"; // case
        }

      private:
        size_t state_;
        String next_state_;

        ParticleName particle_name_;
      };


      //
      //
      struct ParticleFunction: Traversal::All,
                               Traversal::Choice,
                               Traversal::Sequence,
                               Context
      {
        ParticleFunction (Context& c, SemanticGraph::Complex& type)
            : Context (c), type_ (type)
        {
          *this >> contains_particle_ >> *this;
        }


        virtual void
        traverse (SemanticGraph::All& a)
        {
          if (!a.context().count ("p:comp-number")) // Empty compositor.
            return;

          using SemanticGraph::Element;
          using SemanticGraph::Compositor;

          os << "void " << ename (type_) << "::" << endl
             << "all_0 (unsigned long& state," << endl
             << "unsigned char* count," << endl
             << "const " << string_type << "& ns," << endl
             << "const " << string_type << "& n," << endl;

          if (poly_runtime)
            os << "const char*" << (poly_code ? " t" : "") << "," << endl;

          os << "bool start)"
             << "{";

          if (poly_code)
            os << "XSDE_UNUSED (t);"
               << endl;

          os << "::xsde::cxx::parser::context& ctx = this->_context ();"
             << endl;

          for (Compositor::ContainsIterator ci (a.contains_begin ()),
                 ce (a.contains_end ()); ci != ce; ++ci)
          {
            ParticleInAll t (*this, type_);
            t.dispatch (ci->particle ());
          }

          // Handle the flush.
          //
          os << "else if (n.empty () && ns.empty ())"
             << "{";

          for (Compositor::ContainsIterator ci (a.contains_begin ()),
                 ce (a.contains_end ()); ci != ce; ++ci)
          {
            if (ci->min () == 0)
              continue;

            Element& e (dynamic_cast<Element&> (ci->particle ()));
            String ns (e.qualified_p () ? e.namespace_ ().name () : String ());
            size_t state (e.context ().get<size_t> ("p:state"));

            os << "if (count[" << state << "UL] == 0)"
               << "{"
               << "this->_schema_error (" <<
              "::xsde::cxx::schema_error::expected_element);"
               << "return;"
               << "}";

            /*
              << "this->_expected_element (" << endl
              << L << "\"" << ns << "\", " <<
              L << "\"" << e.name () << "\");";
            */
          }

          // Error handling code relies on the fact that there is no
          // code after the if-else block.
          //
          os << "state = ~0UL;"
             << "}"
             << "else" << endl
             << "state = ~0UL;"
             << "}";
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          using SemanticGraph::Compositor;

          SemanticGraph::Context& cc (c.context ());

          if (!cc.count ("p:comp-number")) // Empty compositor.
            return;

          size_t n (cc.get<size_t> ("p:comp-number"));

          os << "void " << ename (type_) << "::" << endl
             << "choice_" << n << " (unsigned long& state," << endl
             << "unsigned long& count," << endl
             << "const " << string_type << "& ns," << endl
             << "const " << string_type << "& n," << endl;

          if (poly_runtime)
            os << "const char*" << (poly_code ? " t" : "") << "," << endl;

          os << "bool start)"
             << "{"
             << "::xsde::cxx::parser::context& ctx = this->_context ();"
             << endl;

          os << "XSDE_UNUSED (count);"
             << "XSDE_UNUSED (ns);"
             << "XSDE_UNUSED (n);"
             << "XSDE_UNUSED (ctx);";

          if (poly_code)
            os << "XSDE_UNUSED (t);";


          os << endl
             << "switch (state)"
             << "{";

          for (Compositor::ContainsIterator ci (c.contains_begin ()),
                 ce (c.contains_end ()); ci != ce; ++ci)
          {
            SemanticGraph::Particle& p (ci->particle ());

            if (p.is_a<Compositor> () && !p.context().count ("p:comp-number"))
              continue; // Empty compositor.

            ParticleInChoice t (*this, type_);
            t.dispatch (p);
          }

          // Error handling code relies on the fact that there is no
          // code after the switch statement.
          //
          os << "}" // switch
             << "}";


          // Generate nested compositor functions.
          //
          Traversal::Choice::traverse (c);
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          if (!s.context().count ("p:comp-number")) // Empty compositor.
            return;

          using SemanticGraph::Compositor;

          size_t n (s.context ().get<size_t> ("p:comp-number"));

          os << "void " << ename (type_) << "::" << endl
             << "sequence_" << n << " (unsigned long& state," << endl
             << "unsigned long& count," << endl
             << "const " << string_type << "& ns," << endl
             << "const " << string_type << "& n," << endl;

          if (poly_runtime)
            os << "const char*" << (poly_code ? " t" : "") << "," << endl;

          os << "bool start)"
             << "{"
             << "::xsde::cxx::parser::context& ctx = this->_context ();"
             << endl;

          if (poly_code)
            os << "XSDE_UNUSED (t);";

          os << "XSDE_UNUSED (ctx);"
             << endl;

          os << "switch (state)"
             << "{";

          size_t state (0);

          for (Compositor::ContainsIterator ci (s.contains_begin ()),
                 ce (s.contains_end ()); ci != ce;)
          {
            SemanticGraph::Particle& p (ci->particle ());

            if (p.is_a<Compositor> () && !p.context().count ("p:comp-number"))
            {
              // Empty compositor.
              //
              ++ci;
              continue;
            }

            // Find the next state.
            //
            do
              ++ci;
            while (ci != ce &&
                   ci->particle ().is_a<Compositor> () &&
                   !ci->particle ().context().count ("p:comp-number"));

            size_t next (ci == ce ? 0 : state + 1);

            ParticleInSequence t (*this, state++, next, type_);
            t.dispatch (p);
          }

          // Error handling code relies on the fact that there is no
          // code after the switch statement.
          //
          os << "case ~0UL:" << endl
             << "break;"
             << "}" // switch
             << "}";

          // Generate nested compositor functions.
          //
          Traversal::Sequence::traverse (s);
        }

      private:
        SemanticGraph::Complex& type_;
        Traversal::ContainsParticle contains_particle_;
      };

      //
      //
      struct CompositorPre: Traversal::All,
                            Traversal::Compositor,
                            Context
      {
        CompositorPre (Context& c, SemanticGraph::Complex& type)
            : Context (c), type_ (type)
        {
        }

        virtual void
        traverse (SemanticGraph::All& a)
        {
          // Clear the counts and push the initial state.
          //
          os << "v_all_count_.push ();"
             << endl;

          SemanticGraph::Compositor& c (a);
          traverse (c);
        }

        virtual void
        traverse (SemanticGraph::Compositor&) // Choice and sequence.
        {
          os << "v_state_& vs = *static_cast< v_state_* > (" <<
            "this->v_state_stack_.top ());"
             << "v_state_descr_& vd = vs.data[vs.size++];" // push
             << endl
             << "vd.func = 0;"
             << "vd.state = 0;"
             << "vd.count = 0;";
        }

      private:
        SemanticGraph::Complex& type_;
      };


      //
      //
      struct CompositorStartElement: Traversal::All,
                                     Traversal::Compositor,
                                     Context
      {
        CompositorStartElement (Context& c, SemanticGraph::Complex& type)
            : Context (c), type_ (type),
              particle_test_ (c), particle_name_ (c)
        {
        }

        virtual void
        traverse (SemanticGraph::All&)
        {
          // The 'all' state machine reaches the final state only
          // on an unknown element, in which case we won't get here
          // again (it would be a validation error). Note that 'all'
          // compositor cannot contain nested compositors so we don't
          // need to re-set vd.
          //
          os << "all_0 (vd->state, v_all_count_.top (), ns, n, " <<
            (poly_runtime ? (poly_code ? "t, " : "0, ") : "") << "true);"
             << endl
             << "if (vd->state != ~0UL || ctx.error_type ())" << endl
             << "vd->count++;"
             << "else" << endl
             << "return false;" // Let our parent handle this.
             << endl;
        }

        virtual void
        traverse (SemanticGraph::Compositor& c) // Choice and sequence.
        {
          using SemanticGraph::Choice;
          using SemanticGraph::Sequence;
          using SemanticGraph::Compositor;

          bool choice (c.is_a<Choice> ());
          SemanticGraph::Context& cc (c.context ());

          size_t max (c.max ());
          size_t min (cc.get<size_t> ("p:effective-min"));
          size_t n (cc.get<size_t> ("p:comp-number"));

          String func (choice ? "choice_" : "sequence_");

          // Invoke the current state machine. If it reaches its
          // terminal state, pop it and invoke the next one until
          // we reach the top, which requires special handling.
          //
          os << "while (vd->func != 0)"
             << "{"
             << "(this->*vd->func) (vd->state, vd->count, ns, n, " <<
            (poly_runtime ? (poly_code ? "t, " : "0, ") : "") << "true);"
             << endl
             << "vd = vs.data + (vs.size - 1);" // re-acquire
             << endl
             << "if (vd->state == ~0UL && !ctx.error_type ())" << endl
             << "vd = vs.data + (--vs.size - 1);" // pop
             << "else" << endl
             << "break;"
             << "}";


          // Check if we got to the top. This code is pretty much the
          // same as the one found in ParticleInSequence.
          //
          os << "if (vd->func == 0)"
             << "{"
             << "if (vd->state != ~0UL)"
             << "{"
             << "unsigned long s = ~0UL;"
             << endl;

          bool first (true);

          // Note that we don't need to worry about the compositor
          // being empty - this case is handled by our caller.
          //
          for (Compositor::ContainsIterator ci (c.contains_begin ());
               ci != c.contains_end (); ++ci)
          {
            SemanticGraph::Particle& p (ci->particle ());

            if (p.is_a<Compositor> () && !cc.count ("p:comp-number"))
              continue; // Empty compositor.

            if (!p.context ().count ("p:prefix"))
              break;

            size_t state (p.context ().get<size_t> ("p:state"));

            if (first)
              first = false;
            else
              os << "else ";

            os << "if (";

            particle_test_.dispatch (p);

            os << ")" << endl
               << "s = " << state << "UL;";
          }

          if (!choice)
          {
            for (Compositor::ContainsIterator ci (c.contains_begin ());
                 ci != c.contains_end (); ++ci)
            {
              SemanticGraph::Particle* p (&ci->particle ());

              if (Choice* pc = dynamic_cast<Choice*> (p))
              {
                if (pc->min () != 0 &&
                    pc->context ().get<size_t> ("p:effective-min") == 0)
                {
                  // This is a required choice with effective-min == 0 (i.e.,
                  // it contains optional particle). We need to call the arm
                  // callback with that optional particle's tag.
                  //
                  OptionalParticleTest test (p);
                  test.traverse (*pc);

                  if (p)
                  {
                    size_t state (pc->context ().get<size_t> ("p:state"));

                    os << endl
                       << "if (s > " << state << "UL)" << endl
                       << "{";
                    choice_arm_call (p, pc, this);
                    os << "}";
                  }
                }
              }
              else if (Sequence* ps = dynamic_cast<Sequence*> (p))
              {
                // This is a required sequence with effective-min == 0 (i.e.,
                // it contains optional particle) and maxOccurs > 1. We need
                // to call the next callback.
                //
                if (ps->min () != 0 && ps->max () != 1 &&
                    ps->context ().get<size_t> ("p:effective-min") == 0)
                {
                  size_t state (ps->context ().get<size_t> ("p:state"));

                  os << endl
                     << "if (s > " << state << "UL)" << endl
                     << "{";
                  sequence_next_call (ps, this);
                  os << "}";
                }
              }
            }
          }

          os << endl
             << "if (s != ~0UL)"
             << "{";

          // This element is a prefix of the root compositor.
          //

          switch (max)
          {
          case 0:
            {
              os << "vd->count++;";
              break;
            }
          case 1:
            {
              os << "vd->count++;"
                 << "vd->state = ~0UL;";
              break;
            }
          default:
            {
              os << "if (++vd->count == " << max << "UL)" << endl
                 << "vd->state = ~0UL;";
            }
          };

          // Delegate to the sub-machine and call _arm, _present, or _next
          // if necessary.
          //

          os << endl
             << "vd = vs.data + vs.size++;" // push
             << "vd->func = &" << ename (type_) << "::" << func << n << ";"
             << "vd->state = s;"
             << "vd->count = 0;"
             << endl;

          Sequence* ps (0);
          Choice* pc (dynamic_cast<Choice*> (&c));

          if (pc)
          {
            os << "this->" << earm (*pc) << " (static_cast< " <<
              earm_tag (*pc) << " > (s));";
          }
          else
          {
            ps = &dynamic_cast<Sequence&> (c);

            if (max != 1)
              os << "this->" << enext (*ps) << " ();";
            else if (c.min () == 0)
              os << "this->" << epresent (*ps) << " ();";
          }

          os << "this->" << func << n << " (vd->state, vd->count, ns, n, " <<
            (poly_runtime ? (poly_code ? "t, " : "0, ") : "") << "true);"
             << "}";

          // This element is not our prefix.
          //

          os << "else"
             << "{";

          if (pc)
          {
            if (c.min () != 0 && min == 0)
            {
              // This is a required choice with effective-min == 0 (i.e.,
              // it contains optional particle). We need to call the arm
              // callback with that optional particle's tag.
              //
              SemanticGraph::Particle* p (0);
              OptionalParticleTest test (p);
              test.traverse (*pc);

              if (p)
              {
                // For a sequence only make the call if there were no
                // elements.
                //
                if (max != 1)
                  os << "if (vd->count == 0)"
                     << "{";
                choice_arm_call (p, pc, this);
                if (max != 1)
                  os << "}";
              }
            }
          }
          else
          {
            if (ps->min () != 0 && min == 0 && max != 1)
            {
              // This is a required sequence with effective-min == 0 (i.e.,
              // it contains optional particle) and maxOccurs > 1. We need
              // to call the next callback.
              //

              // Make the call if there were no elements.
              //
              os << "if (vd->count == 0)"
                 << "{";
              sequence_next_call (ps, this);
              os << "}";
            }
          }

          // Check if min cardinality requirements have been met. Since
          // count is always >= 0, don't generate dead code if min == 0.
          //
          if (min != 0)
          {
            // Note that we are returning true in case of an error to
            // indicate that no further search is needed.
            //
            os << "if (vd->count < " << min << "UL)"
               << "{"
               << "this->_schema_error (" <<
              "::xsde::cxx::schema_error::expected_element);"
               << "return true;"
               << "}";
          }

          // Return false to indicate that we are not handling this element.
          //
          os << "return false;"
             << "}"
             << "}" // if (state != ~0)
             << "else" << endl
             << "return false;"
             << "}"; // if (function == 0)
        }

      private:
        SemanticGraph::Complex& type_;
        ParticleTest particle_test_;
        ParticleName particle_name_;
      };


      //
      //
      struct CompositorEndElement: Traversal::All,
                                   Traversal::Compositor,
                                   Context
      {
        CompositorEndElement (Context& c, SemanticGraph::Complex& type)
            : Context (c), type_ (type)
        {
        }

        virtual void
        traverse (SemanticGraph::All&)
        {
          os << "all_0 (vd.state, v_all_count_.top (), " <<
            "ns, n, " << (poly_runtime ? "0, " : "") << "false);"
             << endl;
        }

        virtual void
        traverse (SemanticGraph::Compositor&) // Choice and sequence.
        {
          os << "assert (vd.func != 0);"
             << "(this->*vd.func) (vd.state, vd.count, ns, n, " <<
            (poly_runtime ? "0, " : "") << "false);"
             << endl
             << "if (vd.state == ~0UL)" << endl
             << "vs.size--;" // pop
             << endl;
        }

      private:
        SemanticGraph::Complex& type_;
      };


      //
      //
      struct CompositorPost: Traversal::All,
                             Traversal::Compositor,
                             Context
      {
        CompositorPost (Context& c, SemanticGraph::Complex& type)
            : Context (c), type_ (type), particle_name_ (c)
        {
        }

        virtual void
        traverse (SemanticGraph::All& a)
        {
          using SemanticGraph::Element;

          os << "v_state_& vs = *static_cast< v_state_* > (" <<
            "this->v_state_stack_.top ());"
             << "v_state_descr_& vd = vs.data[vs.size - 1];"
             << endl;

          // Flush the state machine with the empty element name. This
          // allows us to detect missing content.
          //
          os << "if (vd.count != 0)"
             << "{"
             << string_type << " empty;"
             << "all_0 (vd.state, v_all_count_.top (), empty, empty, " <<
            (poly_runtime ? "0, " : "") << "true);"
             << "}";

          if (a.context ().get<size_t> ("p:effective-min") != 0)
          {
            os << "else" << endl
               << "this->_schema_error (" <<
              "::xsde::cxx::schema_error::expected_element);";

            /*
              << "this->_expected_element (" << endl;
              particle_name_.dispatch (a);
              os << ");";
            */
          }

          os << endl
             << "vs.size--;" // pop
             << "v_all_count_.pop ();";
        }

        virtual void
        traverse (SemanticGraph::Compositor& c) // Choice and sequence.
        {
          using SemanticGraph::Choice;
          using SemanticGraph::Sequence;

          size_t min (c.context ().get<size_t> ("p:effective-min"));

          os << "v_state_& vs = *static_cast< v_state_* > (" <<
            "this->v_state_stack_.top ());"
             << "v_state_descr_* vd = vs.data + (vs.size - 1);"
             << endl;


          // Flush unfinished state machines with the empty element name.
          // This allows us to detect missing content. Note that I am
          // not re-setting vd since no new compositors are pushed on
          // flush.
          //
          os << string_type << " empty;"
             << "while (vd->func != 0)"
             << "{"
             << "(this->*vd->func) (vd->state, vd->count, empty, empty, " <<
            (poly_runtime ? "0, " : "") << "true);"
             << endl
             << "if (ctx.error_type ())" << endl
             << "return;"
             << endl
             << "assert (vd->state == ~0UL);"
             << "vd = vs.data + (--vs.size - 1);" // pop
             << "}";

          if (Choice* pc = dynamic_cast<Choice*> (&c))
          {
            if (c.min () != 0 && min == 0)
            {
              // This is a required choice with effective-min == 0 (i.e.,
              // it contains optional particle). We need to call the arm
              // callback with that optional particle's tag.
              //
              SemanticGraph::Particle* p (0);
              OptionalParticleTest test (p);
              test.traverse (*pc);

              if (p)
              {
                os << "if (vd->count == 0)" << endl
                   << "{";
                choice_arm_call (p, pc, this);
                os << "}";
              }
            }
          }
          else
          {
            Sequence* ps = &dynamic_cast<Sequence&> (c);

            if (ps->min () != 0 && min == 0 && ps->max () != 1)
            {
              // This is a required sequence with effective-min == 0 (i.e.,
              // it contains optional particle) and maxOccurs > 1. We need
              // to call the next callback.
              //

              // Make the call if there were no elements.
              //
              os << "if (vd->count == 0)" << endl
                 << "{";
              sequence_next_call (ps, this);
              os << "}";
            }
          }

          // Check if min cardinality requirements have been met. Since
          // count is always >= 0, don't generate dead code if min == 0.
          //
          if (min != 0)
          {
            os << "if (vd->count < " << min << "UL)" << endl
               << "this->_schema_error (" <<
              "::xsde::cxx::schema_error::expected_element);";
          }
        }

      private:
        SemanticGraph::Complex& type_;
        ParticleName particle_name_;
      };


      //
      //
      struct Complex : Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& c)
        {
          // Nothing to generate if we don't have any elements and wildcards.
          //
          if (!has<Traversal::Element> (c) &&
              !has_particle<Traversal::Any> (c))
            return;

          using SemanticGraph::Compositor;

          String const& name (ename (c));
          Compositor& comp (c.contains_compositor ().compositor ());

          // Don't use restriction_p here since we don't want special
          // treatment of anyType.
          //
          bool restriction (
            c.inherits_p () &&
            c.inherits ().is_a<SemanticGraph::Restricts> ());

          os <<"// Element validation and dispatch functions for " <<
            name << "." << endl
             <<"//" << endl;

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

            os << "::xsde::cxx::parser::context& ctx = this->_context ();"
               << endl;


          os << "v_state_& vs = *static_cast< v_state_* > (" <<
            "this->v_state_stack_.top ());"
             << "v_state_descr_* vd = vs.data + (vs.size - 1);"
             << endl;

          //@@ OPT: I don't really need to call parser_base since it always
          // returns false.
          //
          // In case of an inheritance-by-extension, call our base first.
          // We don't need to generate this code for the 'all' compositor
          // because it can only inherit from the empty content model.
          // States of the root machine for sequence and choice:
          //
          //  0 - calling base
          //  1 - base returned false
          // ~0 - terminal state
          //
          if (!restriction && !comp.is_a<SemanticGraph::All> ())
          {
            os << "if (vd->func == 0 && vd->state == 0)"
               << "{";

            // We cannot use the fully-qualified base name directly
            // because of some broken compilers (EVC 4.0).
            //
            String base (unclash (name, "base"));

            os << "typedef ";

            if (c.inherits_p ())
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
               << "else" << endl
               << "vd->state = 1;"
               << "}";
          }

          {
            CompositorStartElement t (*this, c);
            t.dispatch (comp);
          }

          os << "return true;"
             << "}";


          // _end_element_impl
          //

          os << "bool " << name << "::" << endl
             << "_end_element_impl (const " << string_type << "& ns," << endl
             << "const " << string_type << "& n)"
             << "{";

          os << "v_state_& vs = *static_cast< v_state_* > (" <<
            "this->v_state_stack_.top ());"
             << "v_state_descr_& vd = vs.data[vs.size - 1];"
             << endl;

          //@@ OPT: I don't really need to call parser_base since it always
          // returns false.
          //
          // In case of an inheritance-by-extension, call our base first.
          // We don't need to generate this code for the 'all' compositor
          // because it can only inherit from the empty content model.
          //
          if (!restriction && !comp.is_a<SemanticGraph::All> ())
          {
            os << "if (vd.func == 0 && vd.state == 0)"
               << "{";

            // We cannot use the fully-qualified base name directly
            // because of some broken compilers (EVC 4.0).
            //
            String base (unclash (name, "base"));

            os << "typedef ";

            if (c.inherits_p ())
              os << fq_name (c.inherits ().base ());
            else
              os << complex_base;

            os << " " << base << ";"
               << "if (!" << base << "::_end_element_impl (ns, n))" << endl
               << "assert (false);" // Start and end should match.
               << "return true;"
               << "}";
          }

          {
            CompositorEndElement t (*this, c);
            t.dispatch (comp);
          }

          os << "return true;"
             << "}";


          // _pre_e_validate
          //
          os << "void " << name << "::" << endl
             << "_pre_e_validate ()"
             << "{";

          if (exceptions)
            os << "this->v_state_stack_.push ();";
          else
            os << "if (this->v_state_stack_.push ())"
               << "{"
               << "this->_sys_error (::xsde::cxx::sys_error::no_memory);"
               << "return;"
               << "}";

          os << "static_cast< v_state_* > (this->v_state_stack_.top ())->" <<
            "size = 0;"
             << endl;

          {
            // Assuming that this code cannot trigger an error.
            //
            CompositorPre t (*this, c);
            t.dispatch (comp);
          }

          // In case of an inheritance-by-extension, call our base
	  // _pre_e_validate. We don't need to generate this code for the
	  // 'all' compositor because it can only inherit from the empty
	  // content model.
          //
          if (!restriction && !comp.is_a<SemanticGraph::All> ())
          {
            // We don't need to call parser_base's implementation
            // since it does nothing.
            //
            if (c.inherits_p ())
            {
              // We cannot use the fully-qualified base name directly
              // because of some broken compilers (EVC 4.0).
              //
              String base (unclash (name, "base"));

              os << "typedef " << fq_name (c.inherits ().base ()) << " " <<
                base << ";"
                 << base << "::_pre_e_validate ();";
            }
          }

          os << "}";


          // _post_e_validate
          //
          os << "void " << name << "::" << endl
             << "_post_e_validate ()"
             << "{";

          if (!comp.is_a<SemanticGraph::All> ())
            os << "::xsde::cxx::parser::context& ctx = this->_context ();"
               << endl;

          // In case of an inheritance-by-extension, call our base
	  // _post_e_validate. We don't need to generate this code for
	  // the 'all' compositor because it can only inherit from
	  // the empty content model.
          //
          if (!restriction && !comp.is_a<SemanticGraph::All> ())
          {
            // We don't need to call parser_base's implementation
            // since it does nothing.
            //
            if (c.inherits_p ())
            {
              // We cannot use the fully-qualified base name directly
              // because of some broken compilers (EVC 4.0).
              //
              String base (unclash (name, "base"));

              os << "typedef " << fq_name (c.inherits ().base ()) << " " <<
                base << ";"
                 << base << "::_post_e_validate ();"
                 << endl;

              os << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl;
            }
          }

          {
            CompositorPost t (*this, c);
            t.dispatch (c.contains_compositor ().compositor ());
          }

          os << endl
             << "this->v_state_stack_.pop ();"
             << "}";

          //
          //
          ParticleFunction t (*this, c);
          t.dispatch (c.contains_compositor ().compositor ());
        }
      };
    }

    void
    generate_element_validation_source (Context& ctx)
    {
      ctx.os << "#include <assert.h>" << endl
             << endl;

      Traversal::Schema schema;

      Sources sources;
      Traversal::Names schema_names;

      Namespace ns (ctx);
      Traversal::Names names;

      schema >> sources >> schema;
      schema >> schema_names >> ns >> names;

      Complex complex (ctx);

      names >> complex;

      schema.dispatch (ctx.schema_root);
    }
  }
}
