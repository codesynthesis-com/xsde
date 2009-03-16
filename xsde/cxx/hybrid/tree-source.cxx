// file      : xsd/cxx/hybrid/tree-source.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/hybrid/tree-source.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      struct ChoiceParticle: Traversal::Element,
                             Traversal::Compositor,
                             Context
      {
        enum Action
        {
          alloc,
          free,
          copy,
          assign
        };

        ChoiceParticle (Context& c, Action action)
            : Context (c),
              action_ (action),
              var_ (c, TypeName::var),
              delete_ (c, TypeOps::delete_)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          SemanticGraph::Type& t (e.type ());

          Boolean fund (false);
          {
            FundamentalType test (fund);
            test.dispatch (t);
          }

          // Detect cases where we don't need to generate anything.
          //
          if (fund)
          {
            switch (action_)
            {
            case alloc:
              {
                if (e.max () == 1 && e.min () == 1)
                  return;

                break;
              }
            case free:
              {
                if (e.max () == 1)
                  return;

                break;
              }
            default:
              break;
            }
          }

          String const& member (emember (e));
          String const& umember (
            emember (e.contained_particle ().compositor ()));

          os << "case " << etag (e) << ":"
             << "{";

          switch (action_)
          {
          case alloc:
            {
              if (e.max () != 1)
              {
                os << "new (&this->" << umember << "." << member << ") " <<
                  esequence (e) << ";";
              }
              else
              {
                if (fixed_length (t))
                {
                  if (!fund)
                  {
                    os << "new (&this->" << umember << "." << member << ") ";
                    var_.dispatch (t);
                    os << ";";
                  }

                  if (e.min () == 0)
                  {
                    os << "this->" << umember << "." << member <<
                      ".data_[sizeof (";
                    var_.dispatch (t);
                    os << ")] = false;";
                  }
                }
                else
                  os << "this->" << umember << "." << member << " = 0;";
              }

              break;
            }
          case free:
            {
              if (e.max () != 1)
              {
                String const& sequence (esequence (e));

                os << "typedef " << sequence << " _dtor;"
                   << "reinterpret_cast< " << sequence << "& > (this->" <<
                  umember << "." << member << ").~_dtor ();";
              }
              else
              {
                if (fixed_length (t))
                {
                  os << "typedef ";
                  var_.dispatch (t);
                  os << " _dtor;";

                  os << "reinterpret_cast< ";
                  var_.dispatch (t);
                  os << "& > (this->" << umember << "." << member << ")."
                     << "~_dtor ();";
                }
                else
                {
                  delete_.dispatch (t);
                  os << " this->" << umember << "." << member << ";";
                }
              }

              break;
            }
          case copy:
            {
              // Here we only need to handle the fixed-length situation.
              //
              if (fund)
              {
                // Use POD copying.
                //
                os << "this->" << umember << "." << member << " = x." <<
                  umember << "." << member << ";";
              }
              else
              {
                if (e.min () == 0)
                {
                  os << "this->" << umember << "." << member <<
                    ".data_[sizeof (";
                  var_.dispatch (t);
                  os << ")] = x." << umember << "." << member <<
                    ".data_[sizeof (";
                  var_.dispatch (t);
                  os << ")];"
                     << "if (this->" << umember << "." << member <<
                    ".data_[sizeof (";
                  var_.dispatch (t);
                  os << ")])" << endl;
                }

                os << "new (&this->" << umember << "." << member << ") ";
                var_.dispatch (t);
                os << " (reinterpret_cast< const ";
                var_.dispatch (t);
                os << "& > (x." << umember << "." << member << "));";
              }

              break;
            }
          case assign:
            {
              // Here we only need to handle the fixed-length situation.
              //
              if (fund)
              {
                // Use POD assignment.
                //
                os << "this->" << umember << "." << member << " = x." <<
                  umember << "." << member << ";";
              }
              else
              {
                if (e.min () == 0)
                {
                  os << "this->" << umember << "." << member <<
                    ".data_[sizeof (";
                  var_.dispatch (t);
                  os << ")] = x." << umember << "." << member <<
                    ".data_[sizeof (";
                  var_.dispatch (t);
                  os << ")];"
                     << "if (this->" << umember << "." << member <<
                    ".data_[sizeof (";
                  var_.dispatch (t);
                  os << ")])" << endl;
                }

                os << "reinterpret_cast< ";
                var_.dispatch (t);
                os << "& > (this->" << umember << "." << member << ") = " <<
                  "reinterpret_cast< const ";
                var_.dispatch (t);
                os << "& > (x." << umember << "." << member << ");";
              }

              break;
            }
          }

          os << "break;"
             << "}";
        }

        virtual Void
        traverse (SemanticGraph::Compositor& c)
        {
          String const& member (emember (c));
          String const& umember (
            emember (c.contained_particle ().compositor ()));

          os << "case " << etag (c) << ":"
             << "{";

          switch (action_)
          {
          case alloc:
            {
              if (c.max () != 1)
              {
                os << "new (&this->" << umember << "." << member << ") " <<
                  esequence (c) << ";";
              }
              else
              {
                if (fixed_length (c))
                {
                  String const& type (etype (c));

                  os << "new (&this->" << umember << "." << member << ") " <<
                    type << ";";

                  if (c.min () == 0)
                    os << "this->" << umember << "." << member <<
                      ".data_[sizeof (" << type << ")] = false;";
                }
                else
                  os << "this->" << umember << "." << member << " = 0;";
              }

              break;
            }
          case free:
            {
              if (c.max () != 1)
              {
                String const& sequence (esequence (c));

                os << "typedef " << sequence << " _dtor;"
                   << "reinterpret_cast< " << sequence << "& > (this->" <<
                  umember << "." << member << ").~_dtor ();";
              }
              else
              {
                if (fixed_length (c))
                {
                  String const& type (etype (c));

                  os << "reinterpret_cast< " << type << "& > (this->" <<
                    umember << "." << member << ").~" << type << " ();";
                }
                else
                  os << "delete this->" << umember << "." << member << ";";
              }

              break;
            }
          case copy:
            {
              // Here we only need to handle the fixed-length situation.
              //
              String const& type (etype (c));

              if (c.min () == 0)
              {
                os << "this->" << umember << "." << member <<
                  ".data_[sizeof (" << type << ")] = x." << umember <<
                  "." << member << ".data_[sizeof (" << type << ")];"
                   << "if (this->" << umember << "." << member <<
                  ".data_[sizeof (" << type << ")])" << endl;
              }

              os << "new (&this->" << umember << "." << member << ") " <<
                type << " (reinterpret_cast< const " << type << "& > (x." <<
                umember << "." << member << "));";

              break;
            }
          case assign:
            {
              // Here we only need to handle the fixed-length situation.
              //
              String const& type (etype (c));

              if (c.min () == 0)
              {
                os << "this->" << umember << "." << member <<
                  ".data_[sizeof (" << type << ")] = x." << umember <<
                  "." << member << ".data_[sizeof (" << type << ")];"
                   << "if (this->" << umember << "." << member <<
                  ".data_[sizeof (" << type << ")])" << endl;
              }

              os << "reinterpret_cast< " << type << "& > (this->" <<
                umember << "." << member << ") = " <<
                "reinterpret_cast< const " << type << "& > (x." <<
                umember << "." << member << ");";

              break;
            }
          }

          os << "break;"
             << "}";
        }

      private:
        Action action_;
        TypeName var_;
        TypeOps delete_;
      };

      //
      // Functions.
      //

      struct ChoiceFunc: Traversal::Choice, Context
      {
        ChoiceFunc (Context& c, Boolean in_choice)
            : Context (c),
              in_choice_ (in_choice),
              particle_free_ (c, ChoiceParticle::free),
              particle_alloc_ (c, ChoiceParticle::alloc)
        {
          contains_free_ >> particle_free_;
          contains_alloc_ >> particle_alloc_;
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          if (!in_choice_ && c.max () == 1 && c.min () == 1)
          {
            String const& arm_tag (earm_tag (c));
            String const& arm_member (earm_member (c));
            UnsignedLong bad_tag (arm_tag_count (c));

            // void
            // arm (arm_tag)
            //
            os << "void " << scope (c) << "::" << endl
               << earm (c) << " (" << arm_tag << " x)"
               << "{"
               << "if (this->" << arm_member << " == x)" << endl
               << "return;"
               << endl
               << "switch (this->" << arm_member << ")"
               << "{";

            Choice::contains (c, contains_free_);

            os << "default:" << endl
               << "break;"
               << "}"
               << "this->" << arm_member << " = " <<
              arm_tag << " (" << bad_tag << ");"
               << endl
               << "switch (x)"
               << "{";

            Choice::contains (c, contains_alloc_);

            os << "default:" << endl
               << "break;"
               << "}"
               << "this->" << arm_member << " = x;"
               << "}";
          }
        }

      private:
        Boolean in_choice_;

        ChoiceParticle particle_free_;
        Traversal::ContainsParticle contains_free_;

        ChoiceParticle particle_alloc_;
        Traversal::ContainsParticle contains_alloc_;
      };

      struct SequenceFunc: Traversal::Sequence, Context
      {
        SequenceFunc (Context& c, Boolean in_choice)
            : Context (c), in_choice_ (in_choice)
        {
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          // When sequence is in choice we generate nested class even
          // for min == max == 1.
          //
          if (!in_choice_ && s.max () == 1 && s.min () == 1)
            Sequence::contains (s);
        }

      private:
        Boolean in_choice_;
      };

      //
      // C-tor initialization.
      //

      struct AttributeCtor: Traversal::Attribute, Context
      {
        AttributeCtor (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          Boolean fl (fixed_length (a.type ()));

          if (!fl)
            os << "this->" << emember (a) << " = 0;";

          if (fl && a.optional ())
            os << "this->" << epresent_member (a) << " = false;";
        }
      };


      struct ElementCtor: Traversal::Element, Context
      {
        ElementCtor (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          if (e.max () == 1)
          {
            Boolean fl (fixed_length (e.type ()));

            if (!fl)
              os << "this->" << emember (e) << " = 0;";

            if (fl && e.min () == 0)
              os << "this->" << epresent_member (e) << " = false;";
          }
        }
      };

      struct AllCtor: Traversal::All, Context
      {
        AllCtor (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}
          // and it can only contain particles.
          //
          if (a.min () == 0)
          {
            if (fixed_length (a))
              os << "this->" << epresent_member (a) << " = false;";
            else
              os << "this->" << emember (a) << " = 0;";
          }
          else
            All::contains (a);
        }
      };

      struct ChoiceInSequenceCtor: Traversal::Choice, Context
      {
        ChoiceInSequenceCtor (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.max () == 1)
          {
            if (c.min () == 0)
            {
              if (fixed_length (c))
                os << "this->" << epresent_member (c) << " = false;";
              else
                os << "this->" << emember (c) << " = 0;";
            }
            else
            {
              os << "this->" << earm_member (c) << " = " <<
                earm_tag (c) << " (" << arm_tag_count (c) << ");";
            }
          }
        }
      };

      struct SequenceInSequenceCtor: Traversal::Sequence, Context
      {
        SequenceInSequenceCtor (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          if (s.max () == 1)
          {
            if (s.min () == 0)
            {
              if (fixed_length (s))
                os << "this->" << epresent_member (s) << " = false;";
              else
                os << "this->" << emember (s) << " = 0;";
            }
            else
              Sequence::contains (s);
          }
        }
      };

      //
      // D-tor cleanup.
      //

      struct AttributeDtor: Traversal::Attribute, Context
      {
        AttributeDtor (Context& c)
            : Context (c), delete_ (c, TypeOps::delete_)
        {
        }

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          SemanticGraph::Type& t (a.type ());

          if (!fixed_length (t))
          {
            delete_.dispatch (t);
            os << " this->" << emember (a) << ";";
          }
        }

      private:
        TypeOps delete_;
      };


      struct ElementDtor: Traversal::Element, Context
      {
        ElementDtor (Context& c)
            : Context (c), delete_ (c, TypeOps::delete_)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          if (e.max () == 1)
          {
            SemanticGraph::Type& t (e.type ());

            if (!fixed_length (t))
            {
              delete_.dispatch (t);
              os << " this->" << emember (e) << ";";
            }
          }
        }

      private:
        TypeOps delete_;
      };

      struct AllDtor: Traversal::All, Context
      {
        AllDtor (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}
          // and it can only contain particles.
          //
          if (a.min () == 0)
          {
            if (!fixed_length (a))
              os << "delete this->" << emember (a) << ";";
          }
          else
            All::contains (a);
        }
      };

      struct ChoiceInSequenceDtor: Traversal::Choice, Context
      {
        ChoiceInSequenceDtor (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.max () == 1)
          {
            if (c.min () == 0)
            {
              if (!fixed_length (c))
                os << "delete this->" << emember (c) << ";";
            }
            else
            {
              os << "this->" << earm (c) << " (" <<
                earm_tag (c) << " (" << arm_tag_count (c) << "));";
            }
          }
        }
      };

      struct SequenceInSequenceDtor: Traversal::Sequence, Context
      {
        SequenceInSequenceDtor (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          if (s.max () == 1)
          {
            if (s.min () == 0)
            {
              if (!fixed_length (s))
                os << "delete this->" << emember (s) << ";";
            }
            else
              Sequence::contains (s);
          }
        }
      };

      //
      // Copy c-tor.
      //

      struct AttributeCopy: Traversal::Attribute, Context
      {
        AttributeCopy (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          String const& member (emember (a));

          if (a.optional ())
          {
            String const& present_member (epresent_member (a));

            os << "this->" << present_member << " = x." << present_member << ";"
               << "if (this->" << present_member << ")" << endl;
          }

          os << "this->" << member << " = x." << member << ";";
        }
      };

      struct ElementCopy: Traversal::Element, Context
      {
        ElementCopy (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          String const& member (emember (e));

          if (e.min () == 0)
          {
            String const& present_member (epresent_member (e));

            os << "this->" << present_member << " = x." << present_member << ";"
               << "if (this->" << present_member << ")" << endl;
          }

          os << "this->" << member << " = x." << member << ";";
        }
      };

      struct AllCopy: Traversal::All, Context
      {
        AllCopy (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::All& a)
        {
          if (a.min () == 0)
          {
            String const& member (emember (a));
            String const& present_member (epresent_member (a));

            os << "this->" << present_member << " = x." << present_member << ";"
               << "if (this->" << present_member << ")" << endl
               << "this->" << member << " = x." << member << ";";
          }
          else
            All::contains (a);
        }
      };

      struct ChoiceInSequenceCopy: Traversal::Choice, Context
      {
        ChoiceInSequenceCopy (Context& c)
            : Context (c), particle_copy_ (c, ChoiceParticle::copy)
        {
          contains_copy_ >> particle_copy_;
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.min () == 0)
          {
            String const& member (emember (c));
            String const& present_member (epresent_member (c));

            os << "this->" << present_member << " = x." << present_member << ";"
               << "if (this->" << present_member << ")" << endl
               << "this->" << member << " = x." << member << ";";
          }
          else
          {
            String const& arm_member (earm_member (c));

            os << "this->" << arm_member << " = x." << arm_member << ";"
               << "switch (this->" << arm_member << ")"
               << "{";

            Choice::contains (c, contains_copy_);

            os << "default:" << endl
               << "break;"
               << "}";
          }
        }

      private:
        ChoiceParticle particle_copy_;
        Traversal::ContainsParticle contains_copy_;
      };

      struct SequenceInSequenceCopy: Traversal::Sequence, Context
      {
        SequenceInSequenceCopy (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          if (s.min () == 0)
          {
            String const& member (emember (s));
            String const& present_member (epresent_member (s));

            os << "this->" << present_member << " = x." << present_member << ";"
               << "if (this->" << present_member << ")" << endl
               << "this->" << member << " = x." << member << ";";
          }
          else
            Sequence::contains (s);
        }
      };

      //
      // Assign c-tor.
      //

      struct AttributeAssign: Traversal::Attribute, Context
      {
        AttributeAssign (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          String const& member (emember (a));

          if (a.optional ())
          {
            String const& present_member (epresent_member (a));

            os << "this->" << present_member << " = x." << present_member << ";"
               << "if (this->" << present_member << ")" << endl;
          }

          os << "this->" << member << " = x." << member << ";";
        }
      };

      struct ElementAssign: Traversal::Element, Context
      {
        ElementAssign (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          String const& member (emember (e));

          if (e.min () == 0)
          {
            String const& present_member (epresent_member (e));

            os << "this->" << present_member << " = x." << present_member << ";"
               << "if (this->" << present_member << ")" << endl;
          }

          os << "this->" << member << " = x." << member << ";";
        }
      };

      struct AllAssign: Traversal::All, Context
      {
        AllAssign (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::All& a)
        {
          if (a.min () == 0)
          {
            String const& member (emember (a));
            String const& present_member (epresent_member (a));

            os << "this->" << present_member << " = x." << present_member << ";"
               << "if (this->" << present_member << ")" << endl
               << "this->" << member << " = x." << member << ";";
          }
          else
            All::contains (a);
        }
      };

      struct ChoiceInSequenceAssign: Traversal::Choice, Context
      {
        ChoiceInSequenceAssign (Context& c)
            : Context (c), particle_assign_ (c, ChoiceParticle::assign)
        {
          contains_assign_ >> particle_assign_;
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.min () == 0)
          {
            String const& member (emember (c));
            String const& present_member (epresent_member (c));

            os << "this->" << present_member << " = x." << present_member << ";"
               << "if (this->" << present_member << ")" << endl
               << "this->" << member << " = x." << member << ";";
          }
          else
          {
            String const& arm_member (earm_member (c));

            os << "this->" << earm (c) << " (x." << arm_member << ");"
               << "switch (this->" << arm_member << ")"
               << "{";

            Choice::contains (c, contains_assign_);

            os << "default:" << endl
               << "break;"
               << "}";
          }
        }

      private:
        ChoiceParticle particle_assign_;
        Traversal::ContainsParticle contains_assign_;
      };

      struct SequenceInSequenceAssign: Traversal::Sequence, Context
      {
        SequenceInSequenceAssign (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          if (s.min () == 0)
          {
            String const& member (emember (s));
            String const& present_member (epresent_member (s));

            os << "this->" << present_member << " = x." << present_member << ";"
               << "if (this->" << present_member << ")" << endl
               << "this->" << member << " = x." << member << ";";
          }
          else
            Sequence::contains (s);
        }
      };

      //
      // Nested classes.
      //

      struct All: Traversal::All, Context
      {
        All (Context& c,
             Traversal::ContainsParticle& contains_ctor,
             Traversal::ContainsParticle& contains_dtor,
             Traversal::ContainsParticle& contains_copy,
             Traversal::ContainsParticle& contains_assign)
            : Context (c),
              contains_ctor_ (contains_ctor),
              contains_dtor_ (contains_dtor),
              contains_copy_ (contains_copy),
              contains_assign_ (contains_assign)
        {
        }

        virtual Void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}
          // and it can only contain particles.
          //
          if (a.min () == 0)
          {
            String const& type (etype (a));
            String const& scope (Context::scope (a));

            // c-tor
            //
            os << scope << "::" << type << "::" << endl
               << type << " ()"
               << "{";

            All::contains (a, contains_ctor_);

            os << "}";

            // d-tor
            //
            os << scope << "::" << type << "::" << endl
               << "~" << type << " ()"
               << "{";

            All::contains (a, contains_dtor_);

            os << "}";

            if (fixed_length (a))
            {
              // copy c-tor
              //
              os << scope << "::" << type << "::" << endl
                 << type << " (const " << type << "& x)"
                 << "{"
                 << "XSDE_UNUSED (x);";

              All::contains (a, contains_copy_);

              os << "}";

              // operator=
              //
              os << scope << "::" << type << "& " << scope << "::" <<
                type << "::" << endl
                 << "operator= (const " << type << "& x)"
                 << "{"
                 << "XSDE_UNUSED (x);";

              All::contains (a, contains_assign_);

              os << "return *this;"
                 << "}";
            }
          }
        }

      private:
        Traversal::ContainsParticle& contains_ctor_;
        Traversal::ContainsParticle& contains_dtor_;
        Traversal::ContainsParticle& contains_copy_;
        Traversal::ContainsParticle& contains_assign_;
      };

      struct Choice: Traversal::Choice, Context
      {
        Choice (Context& c,
                Boolean in_choice,
                Traversal::ContainsParticle& contains_func)
            : Context (c),
              in_choice_ (in_choice),
              contains_func_ (contains_func),
              particle_free_ (c, ChoiceParticle::free),
              particle_alloc_ (c, ChoiceParticle::alloc),
              particle_copy_ (c, ChoiceParticle::copy),
              particle_assign_ (c, ChoiceParticle::assign)
        {
          contains_free_ >> particle_free_;
          contains_alloc_ >> particle_alloc_;
          contains_copy_ >> particle_copy_;
          contains_assign_ >> particle_assign_;
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          // When choice is in choice we generate nested class even
          // for min == max == 1.
          //
          if (in_choice_ || c.max () != 1 || c.min () == 0)
          {
            String const& type (etype (c));
            String const& arm (earm (c));
            String const& arm_tag (earm_tag (c));
            String const& arm_member (earm_member (c));
            UnsignedLong bad_tag (arm_tag_count (c));

            String const& scope (Context::scope (c));

            // c-tor ()
            //
            os << scope << "::" << type << "::" << endl
               << type << " ()"
               << "{"
               << "this->" << arm_member << " = " <<
              arm_tag << " (" << bad_tag << ");"
               << "}";

            // d-tor ()
            //
            os << scope << "::" << type << "::" << endl
               << "~" << type << " ()"
               << "{"
               << "this->" << arm << " (" <<
              arm_tag << " (" << bad_tag << "));"
               << "}";

            if (fixed_length (c))
            {
              // copy c-tor
              //
              os << scope << "::" << type << "::" << endl
                 << type << " (const " << type << "& x)"
                 << "{"
                 << "this->" << arm_member << " = x." << arm_member << ";"
                 << "switch (this->" << arm_member << ")"
                 << "{";

              Choice::contains (c, contains_copy_);

              os << "default:" << endl
                 << "break;"
                 << "}"
                 << "}";

              // operator=
              //
              os << scope << "::" << type << "& " << scope << "::" <<
                type << "::" << endl
                 << "operator= (const " << type << "& x)"
                 << "{"
                 << "this->" << arm << " (x." << arm_member << ");"
                 << "switch (this->" << arm_member << ")"
                 << "{";

              Choice::contains (c, contains_assign_);

              os << "default:" << endl
                 << "break;"
                 << "}"
                 << "return *this;"
                 << "}";
            }

            // void
            // arm (arm_tag)
            //
            os << "void " << scope << "::" << type << "::" << endl
               << arm << " (" << arm_tag << " x)"
               << "{"
               << "if (this->" << arm_member << " == x)" << endl
               << "return;"
               << endl
               << "switch (this->" << arm_member << ")"
               << "{";

            Choice::contains (c, contains_free_);

            os << "default:" << endl
               << "break;"
               << "}"
               << "this->" << arm_member << " = " <<
              arm_tag << " (" << bad_tag << ");"
               << endl
               << "switch (x)"
               << "{";

            Choice::contains (c, contains_alloc_);

            os << "default:" << endl
               << "break;"
               << "}"
               << "this->" << arm_member << " = x;"
               << "}";

            Choice::contains (c, contains_func_);
          }

          Choice::contains (c);
        }

      private:
        Boolean in_choice_;
        Traversal::ContainsParticle& contains_func_;

        ChoiceParticle particle_free_;
        Traversal::ContainsParticle contains_free_;

        ChoiceParticle particle_alloc_;
        Traversal::ContainsParticle contains_alloc_;

        ChoiceParticle particle_copy_;
        Traversal::ContainsParticle contains_copy_;

        ChoiceParticle particle_assign_;
        Traversal::ContainsParticle contains_assign_;
      };


      struct Sequence: Traversal::Sequence, Context
      {
        Sequence (Context& c,
                  Boolean in_choice,
                  Traversal::ContainsParticle& contains_ctor,
                  Traversal::ContainsParticle& contains_dtor,
                  Traversal::ContainsParticle& contains_copy,
                  Traversal::ContainsParticle& contains_assign,
                  Traversal::ContainsParticle& contains_func)
            : Context (c),
              in_choice_ (in_choice),
              contains_ctor_ (contains_ctor),
              contains_dtor_ (contains_dtor),
              contains_copy_ (contains_copy),
              contains_assign_ (contains_assign),
              contains_func_ (contains_func)
        {
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          // When sequence is in choice we generate nested class even
          // for min == max == 1.
          //
          if (in_choice_ || s.max () != 1 || s.min () == 0)
          {
            String const& type (etype (s));
            String const& scope (Context::scope (s));

            // c-tor ()
            //
            os << scope << "::" << type << "::" << endl
               << type << " ()"
               << "{";

            Sequence::contains (s, contains_ctor_);

            os << "}";

            // d-tor ()
            //
            os << scope << "::" << type << "::" << endl
               << "~" << type << " ()"
               << "{";

            Sequence::contains (s, contains_dtor_);

            os << "}";

            if (fixed_length (s))
            {
              // copy c-tor
              //
              os << scope << "::" << type << "::" << endl
                 << type << " (const " << type << "& x)"
                 << "{"
                 << "XSDE_UNUSED (x);";

              Sequence::contains (s, contains_copy_);

              os << "}";

              // operator=
              //
              os << scope << "::" << type << "& " << scope << "::" <<
                type << "::" << endl
                 << "operator= (const " << type << "& x)"
                 << "{"
                 << "XSDE_UNUSED (x);";

              Sequence::contains (s, contains_assign_);

              os << "return *this;"
                 << "}";
            }

            Sequence::contains (s, contains_func_);
          }

          Sequence::contains (s);
        }

      private:
        Boolean in_choice_;
        Traversal::ContainsParticle& contains_ctor_;
        Traversal::ContainsParticle& contains_dtor_;
        Traversal::ContainsParticle& contains_copy_;
        Traversal::ContainsParticle& contains_assign_;
        Traversal::ContainsParticle& contains_func_;
      };

      struct Complex : Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c),
              base_name_ (c, TypeName::base),

              // Functions.
              //
              choice_in_choice_func_ (c, true),
              choice_in_sequence_func_ (c, false),
              sequence_in_choice_func_ (c, true),
              sequence_in_sequence_func_ (c, false),

              // C-tor initialization.
              //
              attribute_ctor_ (c),
              element_ctor_ (c),
              all_ctor_ (c),
              choice_in_sequence_ctor_ (c),
              sequence_in_sequence_ctor_ (c),

              // D-tor cleanup.
              //
              attribute_dtor_ (c),
              element_dtor_ (c),
              all_dtor_ (c),
              choice_in_sequence_dtor_ (c),
              sequence_in_sequence_dtor_ (c),

              // Copy.
              //
              attribute_copy_ (c),
              element_copy_ (c),
              all_copy_ (c),
              choice_in_sequence_copy_ (c),
              sequence_in_sequence_copy_ (c),

              // Assign.
              //
              attribute_assign_ (c),
              element_assign_ (c),
              all_assign_ (c),
              choice_in_sequence_assign_ (c),
              sequence_in_sequence_assign_ (c),

              // Nested c-tors, etc.
              //
              all_ (c,
                    all_contains_ctor_,
                    all_contains_dtor_,
                    all_contains_copy_,
                    all_contains_assign_),
              choice_in_choice_ (c, true, choice_contains_func_),
              choice_in_sequence_ (c, false, choice_contains_func_),
              sequence_in_choice_ (
                c,
                true,
                sequence_contains_ctor_,
                sequence_contains_dtor_,
                sequence_contains_copy_,
                sequence_contains_assign_,
                sequence_contains_func_),
              sequence_in_sequence_ (
                c,
                false,
                sequence_contains_ctor_,
                sequence_contains_dtor_,
                sequence_contains_copy_,
                sequence_contains_assign_,
                sequence_contains_func_)
        {
          // Functions.
          //
          sequence_in_choice_func_ >> sequence_contains_func_;
          sequence_in_sequence_func_ >> sequence_contains_func_;
          sequence_contains_func_ >> choice_in_sequence_func_;
          sequence_contains_func_ >> sequence_in_sequence_func_;

          choice_in_choice_func_ >> choice_contains_func_;
          choice_in_sequence_func_ >> choice_contains_func_;
          choice_contains_func_ >> sequence_in_choice_func_;
          choice_contains_func_ >> choice_in_choice_func_;

          contains_compositor_func_ >> choice_in_sequence_func_;
          contains_compositor_func_ >> sequence_in_sequence_func_;

          // C-tor initialization.
          //
          attribute_names_ctor_ >> attribute_ctor_;

          all_ctor_ >> all_contains_ctor_ >> element_ctor_;

          sequence_in_sequence_ctor_ >> sequence_contains_ctor_;
          sequence_contains_ctor_ >> element_ctor_;
          sequence_contains_ctor_ >> choice_in_sequence_ctor_;
          sequence_contains_ctor_ >> sequence_in_sequence_ctor_;

          contains_compositor_ctor_ >> all_ctor_;
          contains_compositor_ctor_ >> choice_in_sequence_ctor_;
          contains_compositor_ctor_ >> sequence_in_sequence_ctor_;

          // D-tor cleanup.
          //
          attribute_names_dtor_ >> attribute_dtor_;

          all_dtor_ >> all_contains_dtor_ >> element_dtor_;

          sequence_in_sequence_dtor_ >> sequence_contains_dtor_;
          sequence_contains_dtor_ >> element_dtor_;
          sequence_contains_dtor_ >> choice_in_sequence_dtor_;
          sequence_contains_dtor_ >> sequence_in_sequence_dtor_;

          contains_compositor_dtor_ >> all_dtor_;
          contains_compositor_dtor_ >> choice_in_sequence_dtor_;
          contains_compositor_dtor_ >> sequence_in_sequence_dtor_;

          // Copy.
          //
          attribute_names_copy_ >> attribute_copy_;

          all_copy_ >> all_contains_copy_ >> element_copy_;

          sequence_in_sequence_copy_ >> sequence_contains_copy_;
          sequence_contains_copy_ >> element_copy_;
          sequence_contains_copy_ >> choice_in_sequence_copy_;
          sequence_contains_copy_ >> sequence_in_sequence_copy_;

          contains_compositor_copy_ >> all_copy_;
          contains_compositor_copy_ >> choice_in_sequence_copy_;
          contains_compositor_copy_ >> sequence_in_sequence_copy_;

          // Assign.
          //
          attribute_names_assign_ >> attribute_assign_;

          all_assign_ >> all_contains_assign_ >> element_assign_;

          sequence_in_sequence_assign_ >> sequence_contains_assign_;
          sequence_contains_assign_ >> element_assign_;
          sequence_contains_assign_ >> choice_in_sequence_assign_;
          sequence_contains_assign_ >> sequence_in_sequence_assign_;

          contains_compositor_assign_ >> all_assign_;
          contains_compositor_assign_ >> choice_in_sequence_assign_;
          contains_compositor_assign_ >> sequence_in_sequence_assign_;

          // Nested c-tors, etc.
          //
          all_ >> all_contains_;

          choice_in_choice_ >> choice_contains_;
          choice_in_sequence_ >> choice_contains_;
          choice_contains_ >> choice_in_choice_;
          choice_contains_ >> sequence_in_choice_;

          sequence_in_choice_ >> sequence_contains_;
          sequence_in_sequence_ >> sequence_contains_;
          sequence_contains_ >> choice_in_sequence_;
          sequence_contains_ >> sequence_in_sequence_;

          contains_compositor_ >> all_;
          contains_compositor_ >> choice_in_sequence_;
          contains_compositor_ >> sequence_in_sequence_;
        }

        virtual Void
        traverse (Type& c)
        {
          String const& name (ename_custom (c));

          // We may not need to generate the class if this type is
          // being customized.
          //
          if (!name)
            return;

          Boolean restriction (restriction_p (c));

          os << "// " << comment (c.name ()) << endl
             << "//" << endl
             << endl;

          // c-tor ()
          //
          os << name << "::" << endl
             << name << " ()"
             << "{";

          if (!restriction)
          {
            Complex::names (c, attribute_names_ctor_);

            if (c.contains_compositor_p ())
              Complex::contains_compositor (c, contains_compositor_ctor_);
          }

          os << "}";


          if (!restriction)
          {
            // d-tor ()
            //
            os << name << "::" << endl
               << "~" << name << " ()"
               << "{";

            Complex::names (c, attribute_names_dtor_);

            if (c.contains_compositor_p ())
              Complex::contains_compositor (c, contains_compositor_dtor_);

            os << "}";

            if (fixed_length (c))
            {
              // copy c-tor
              //
              os << name << "::" << endl
                 << name << " (const " << name << "& x)";

              if (c.inherits_p ())
              {
                os << endl
                   << ": ";
                base_name_.dispatch (c.inherits ().base ());
                os << " (x)";
              }

              os << "{"
                 << "XSDE_UNUSED (x);";

              Complex::names (c, attribute_names_copy_);

              if (c.contains_compositor_p ())
                Complex::contains_compositor (c, contains_compositor_copy_);

              os << "}";

              // operator=
              //
              os << name << "& " << name << "::" << endl
                 << "operator= (const " << name << "& x)"
                 << "{"
                 << "XSDE_UNUSED (x);";

              if (c.inherits_p ())
              {
                os << "static_cast< ";
                base_name_.dispatch (c.inherits ().base ());
                os << "& > (*this) = x;";
              }

              Complex::names (c, attribute_names_assign_);

              if (c.contains_compositor_p ())
                Complex::contains_compositor (c, contains_compositor_assign_);

              os << "return *this;"
                 << "}";
            }

            // Functions.
            //
            if (c.contains_compositor_p ())
              Complex::contains_compositor (c, contains_compositor_func_);

            // Nested c-tors, etc.
            //
            if (c.contains_compositor_p ())
              Complex::contains_compositor (c, contains_compositor_);
          }
        }

      private:
        TypeName base_name_;

        // Functions.
        //
        ChoiceFunc choice_in_choice_func_;
        ChoiceFunc choice_in_sequence_func_;
        SequenceFunc sequence_in_choice_func_;
        SequenceFunc sequence_in_sequence_func_;
        Traversal::ContainsParticle choice_contains_func_;
        Traversal::ContainsParticle sequence_contains_func_;

        Traversal::ContainsCompositor contains_compositor_func_;

        // C-tor initialization.
        //
        AttributeCtor attribute_ctor_;
        Traversal::Names attribute_names_ctor_;

        ElementCtor element_ctor_;
        AllCtor all_ctor_;
        ChoiceInSequenceCtor choice_in_sequence_ctor_;
        SequenceInSequenceCtor sequence_in_sequence_ctor_;
        Traversal::ContainsParticle all_contains_ctor_;
        Traversal::ContainsParticle sequence_contains_ctor_;

        Traversal::ContainsCompositor contains_compositor_ctor_;

        // D-tor cleanup.
        //
        AttributeDtor attribute_dtor_;
        Traversal::Names attribute_names_dtor_;

        ElementDtor element_dtor_;
        AllDtor all_dtor_;
        ChoiceInSequenceDtor choice_in_sequence_dtor_;
        SequenceInSequenceDtor sequence_in_sequence_dtor_;
        Traversal::ContainsParticle all_contains_dtor_;
        Traversal::ContainsParticle sequence_contains_dtor_;

        Traversal::ContainsCompositor contains_compositor_dtor_;

        // Copy.
        //
        AttributeCopy attribute_copy_;
        Traversal::Names attribute_names_copy_;

        ElementCopy element_copy_;
        AllCopy all_copy_;
        ChoiceInSequenceCopy choice_in_sequence_copy_;
        SequenceInSequenceCopy sequence_in_sequence_copy_;
        Traversal::ContainsParticle all_contains_copy_;
        Traversal::ContainsParticle sequence_contains_copy_;

        Traversal::ContainsCompositor contains_compositor_copy_;

        // Assign.
        //
        AttributeAssign attribute_assign_;
        Traversal::Names attribute_names_assign_;

        ElementAssign element_assign_;
        AllAssign all_assign_;
        ChoiceInSequenceAssign choice_in_sequence_assign_;
        SequenceInSequenceAssign sequence_in_sequence_assign_;
        Traversal::ContainsParticle all_contains_assign_;
        Traversal::ContainsParticle sequence_contains_assign_;

        Traversal::ContainsCompositor contains_compositor_assign_;

        // Nested c-tors, etc.
        //
        All all_;
        Choice choice_in_choice_;
        Choice choice_in_sequence_;
        Sequence sequence_in_choice_;
        Sequence sequence_in_sequence_;
        Traversal::ContainsParticle all_contains_;
        Traversal::ContainsParticle choice_contains_;
        Traversal::ContainsParticle sequence_contains_;

        Traversal::ContainsCompositor contains_compositor_;
      };
    }

    Void
    generate_tree_source (Context& ctx)
    {
      // Needed for placement new.
      //
      ctx.os << "#include <new>" << endl
             << endl;

      Traversal::Schema schema;
      Traversal::Sources sources;
      Traversal::Names names_ns, names;

      Namespace ns (ctx);

      //Union union_ (ctx);
      Complex complex (ctx);
      //Enumeration enumeration (ctx);

      schema >> sources >> schema;
      schema >> names_ns >> ns >> names;

      //names >> union_;
      names >> complex;
      //names >> enumeration;

      schema.dispatch (ctx.schema_root);
    }
  }
}
