// file      : xsd/cxx/hybrid/extraction-source.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/hybrid/extraction-source.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      struct Enumeration : Traversal::Enumeration, Context
      {
        Enumeration (Context& c, Traversal::Complex& complex)
            : Context (c), complex_ (complex)
        {
        }

        virtual Void
        traverse (Type& e)
        {
          // First see if we should delegate this one to the Complex
          // generator.
          //
          if (!enum_ || !enum_mapping (e))
          {
            complex_.traverse (e);
            return;
          }

          String const& name (ename_custom (e));

          // We may not need to generate the class if this type is
          // being customized.
          //
          if (!name)
            return;

          for (Streams::ConstIterator i (istreams.begin ());
               i != istreams.end (); ++i)
          {
            os << (exceptions ? "void" : "bool") << endl
               << "operator>> (" << istream (*i) << "& s," << endl
               << name << "& x)"
               << "{"
               << "unsigned int i;";

            if (exceptions)
              os << "s >> i;";
            else
              os << "if (!(s >> i))" << endl
                 << "return false;";

            os << "x = static_cast< " << name << "::" <<
              e.context ().get<String> ("value-type") << " > (i);"
               << (exceptions ? "" : "return true;")
               << "}"
               << endl;
          }
        }

      private:
        Traversal::Complex& complex_;
      };

      struct List : Traversal::List, Context
      {
        List (Context& c)
            : Context (c), base_name_ (c, TypeName::seq)
        {
        }

        virtual Void
        traverse (Type& l)
        {
          String const& name (ename_custom (l));

          // We may not need to generate the class if this type is
          // being customized.
          //
          if (!name)
            return;

          for (Streams::ConstIterator i (istreams.begin ());
               i != istreams.end (); ++i)
          {
            os << (exceptions ? "void" : "bool") << endl
               << "operator>> (" << istream (*i) << "& s," << endl
               << name << "& x)"
               << "{";

            base_name_.dispatch (l.argumented ().type ());

            os << "& b = x;"
               << (exceptions ? "" : "return ") << "s >> b;"
               << "}";
          }
        }

      private:
        TypeName base_name_;
      };

      struct Union : Traversal::Union, Context
      {
        Union (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& u)
        {
          String const& name (ename_custom (u));

          // We may not need to generate the class if this type is
          // being customized.
          //
          if (!name)
            return;

          String const& value (u.context ().get<String> ("value"));

          for (Streams::ConstIterator i (istreams.begin ());
               i != istreams.end (); ++i)
          {
            os << (exceptions ? "void" : "bool") << endl
               << "operator>> (" << istream (*i) << "& s," << endl
               << name << "& x)"
               << "{"
               << (stl ? "::std::string" : "char*") << " i;";

            if (exceptions)
              os << "s >> i;";
            else
              os << "if (!(s >> i))" << endl
                 << "return false;";

            os << "x." << value << " (i);"
               << (exceptions ? "" : "return true;")
               << "}";
          }
        }
      };

      //
      // Data.
      //

      struct AttributeData: Traversal::Attribute, Context
      {
        AttributeData (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          // Nothing is serialized for fixed attributes.
          //
          if (a.fixed_p ())
            return;

          SemanticGraph::Type& t (a.type ());

          Boolean fl (fixed_length (t));
          Boolean st (false);

          String const& name (ename (a));

          if (!fl)
          {
            StringType test (st);
            test.dispatch (t);
          }

          os << "{";

          if (a.optional_p ())
          {
            if (!a.default_p ())
            {
              os << "bool p;";

              if (exceptions)
                os << "s >> p;";
              else
                os << endl
                   << "if (!(s >> p))" << endl
                   << "return false;";

              os << endl
                 << "if (p)"
                 << "{";

              if (fl)
                os << "x." << epresent (a) << " (true);";
            }
            else
            {
              os << "bool d;";

              if (exceptions)
                os << "s >> d;";
              else
                os << endl
                   << "if (!(s >> d))" << endl
                   << "return false;";

              os << endl
                 << "if (!d)"
                 << "{";
            }
          }

          if (st)
          {
            // C-string requires special handling.
            //
            os << "char* i;";

            if (exceptions)
              os << "s >> i;";
            else
              os << "if (!(s >> i))" << endl
                 << "return false;";

            os << "x." << name << " (i);";
          }
          else
          {
            if (!fl)
            {
              String fq (fq_name (t));

              if (!custom_alloc)
                os << fq << "* i = new " << fq << ";";
              else
                os << fq << "* i = static_cast< " << fq << "* > (" << endl
                   << "::xsde::cxx::alloc (sizeof (" << fq << ")));";

              if (!exceptions)
                os << endl
                   << "if (i == 0)" << endl
                   << "return false;"
                   << endl;

              if (custom_alloc)
              {
                if (exceptions)
                  os << "::xsde::cxx::alloc_guard ig (i);";

                os << "new (i) " << fq << ";";

                if (exceptions)
                  os << "ig.release ();";
              }

              os << "x." << name << " (i);";
            }

            if (exceptions)
              os << "s >> x." << name << " ();";
            else
              os << "if (!(s >> x." << name << " ()))" << endl
                 << "return false;";
          }

          if (a.optional_p ())
          {
            if (!a.default_p ())
            {
              os << "}"
                 << "else" << endl;

              if (fl)
                os << "x." << epresent (a) << " (false);";
              else
                os << "x." << name << " (0);";

            }
            else
            {
              os << "}"
                 << "else" << endl;

              if (fl)
                os << "x." << edefault (a) << " (true);";
              else
                os << "x." << name << " (0);";
            }
          }

          os << "}";
        }
      };


      struct ElementData: Traversal::Element, Context
      {
        ElementData (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          // See also ChoiceParticleData.
          //
          String const& name (ename (e));

          if (e.max () != 1)
          {
            if (exceptions)
              os << "s >> x." << name << " ();";
            else
              os << "if (!(s >> x." << name << " ()))" << endl
                 << "return false;";
          }
          else
          {
            SemanticGraph::Type& t (e.type ());

            Boolean fl (fixed_length (t));
            Boolean st (false);

            if (!fl)
            {
              StringType test (st);
              test.dispatch (t);
            }

            os << "{";

            if (e.min () == 0)
            {
              os << "bool p;";

              if (exceptions)
                os << "s >> p;";
              else
                os << endl
                   << "if (!(s >> p))" << endl
                   << "return false;";

              os << endl
                 << "if (p)"
                 << "{";

              if (fl)
                os << "x." << epresent (e) << " (true);";
            }

            if (st)
            {
              // C-string requires special handling.
              //
              os << "char* i;";

              if (exceptions)
                os << "s >> i;";
              else
                os << "if (!(s >> i))" << endl
                   << "return false;";

              os << "x." << name << " (i);";
            }
            else
            {
              if (!fl)
              {
                String fq (fq_name (t));

                if (!custom_alloc)
                  os << fq << "* i = new " << fq << ";";
                else
                  os << fq << "* i = static_cast< " << fq << "* > (" << endl
                     << "::xsde::cxx::alloc (sizeof (" << fq << ")));";

                if (!exceptions)
                  os << endl
                     << "if (i == 0)" << endl
                     << "return false;"
                     << endl;

                if (custom_alloc)
                {
                  if (exceptions)
                    os << "::xsde::cxx::alloc_guard ig (i);";

                  os << "new (i) " << fq << ";";

                  if (exceptions)
                    os << "ig.release ();";
                }

                os << "x." << name << " (i);";
              }

              if (exceptions)
                os << "s >> x." << name << " ();";
              else
                os << "if (!(s >> x." << name << " ()))" << endl
                   << "return false;";
            }

            if (e.min () == 0)
            {
              os << "}"
                 << "else" << endl;

              if (fl)
                os << "x." << epresent (e) << " (false);";
              else
                os << "x." << name << " (0);";
            }

            os << "}";
          }
        }
      };

      struct AllData: Traversal::All, Context
      {
        AllData (Context& c)
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
            Boolean fl (fixed_length (a));

            String const& name (ename (a));
            String const& present (epresent (a));

            os << "{"
               << "bool p;";

            if (exceptions)
              os << "s >> p;";
            else
              os << endl
                 << "if (!(s >> p))" << endl
                 << "return false;";

            os << endl
               << "if (p)"
               << "{";

            if (fl)
              os << "x." << present << " (true);";

            if (!fl)
            {
              String fq (scope (a) + L"::" + etype (a));

              if (!custom_alloc)
                os << fq << "* i = new " << fq << ";";
              else
                os << fq << "* i = static_cast< " << fq << "* > (" << endl
                   << "::xsde::cxx::alloc (sizeof (" << fq << ")));";

              if (!exceptions)
                os << endl
                   << "if (i == 0)" << endl
                   << "return false;"
                   << endl;

              if (custom_alloc)
              {
                if (exceptions)
                  os << "::xsde::cxx::alloc_guard ig (i);";

                os << "new (i) " << fq << ";";

                if (exceptions)
                  os << "ig.release ();";
              }

              os << "x." << name << " (i);";
            }

            if (exceptions)
              os << "s >> x." << name << " ();";
            else
              os << "if (!(s >> x." << name << " ()))" << endl
                 << "return false;";

            os << "}"
               << "else" << endl;

            if (fl)
              os << "x." << present << " (false);";
            else
              os << "x." << name << " (0);";

            os << "}";
          }
          else
            All::contains (a);
        }
      };

      struct ChoiceParticleData: Traversal::Element,
                                 Traversal::Compositor,
                                 Context
      {
        ChoiceParticleData (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          os << "case " << scope (e) << "::" << etag (e) << ":"
             << "{";

          // Same as ElementData except that we don't need the extra scope.
          //
          String const& name (ename (e));

          if (e.max () != 1)
          {
            if (exceptions)
              os << "s >> x." << name << " ();";
            else
              os << "if (!(s >> x." << name << " ()))" << endl
                 << "return false;";
          }
          else
          {
            SemanticGraph::Type& t (e.type ());

            Boolean fl (fixed_length (t));
            Boolean st (false);

            if (!fl)
            {
              StringType test (st);
              test.dispatch (t);
            }

            if (e.min () == 0)
            {
              os << "bool p;";

              if (exceptions)
                os << "s >> p;";
              else
                os << endl
                   << "if (!(s >> p))" << endl
                   << "return false;";

              os << endl
                 << "if (p)"
                 << "{";

              if (fl)
                os << "x." << epresent (e) << " (true);";
            }

            if (st)
            {
              // C-string requires special handling.
              //
              os << "char* i;";

              if (exceptions)
                os << "s >> i;";
              else
                os << "if (!(s >> i))" << endl
                   << "return false;";

              os << "x." << name << " (i);";
            }
            else
            {
              if (!fl)
              {
                String fq (fq_name (t));

                if (!custom_alloc)
                  os << fq << "* i = new " << fq << ";";
                else
                  os << fq << "* i = static_cast< " << fq << "* > (" << endl
                     << "::xsde::cxx::alloc (sizeof (" << fq << ")));";

                if (!exceptions)
                  os << endl
                     << "if (i == 0)" << endl
                     << "return false;"
                     << endl;

                if (custom_alloc)
                {
                  if (exceptions)
                    os << "::xsde::cxx::alloc_guard ig (i);";

                  os << "new (i) " << fq << ";";

                  if (exceptions)
                    os << "ig.release ();";
                }

                os << "x." << name << " (i);";
              }

              if (exceptions)
                os << "s >> x." << name << " ();";
              else
                os << "if (!(s >> x." << name << " ()))" << endl
                   << "return false;";
            }

            if (e.min () == 0)
            {
              os << "}"
                 << "else" << endl;

              if (fl)
                os << "x." << epresent (e) << " (false);";
              else
                os << "x." << name << " (0);";
            }
          }

          os << "break;"
             << "}";
        }

        virtual Void
        traverse (SemanticGraph::Compositor& c)
        {
          os << "case " << scope (c) << "::" << etag (c) << ":"
             << "{";

          // A compositor in choice always results in a nested class.
          //
          String const& name (ename (c));

          if (c.max () != 1)
          {
            String const& name (ename (c));

            if (exceptions)
              os << "s >> x." << name << " ();";
            else
              os << "if (!(s >> x." << name << " ()))" << endl
                 << "return false;";
          }
          else
          {
            Boolean fl (fixed_length (c));

            if (c.min () == 0)
            {
              os << "bool p;";

              if (exceptions)
                os << "s >> p;";
              else
                os << endl
                   << "if (!(s >> p))" << endl
                   << "return false;";

              os << endl
                 << "if (p)"
                 << "{";

              if (fl)
                os << "x." << epresent (c) << " (true);";
            }

            if (!fl)
            {
              String fq (scope (c) + L"::" + etype (c));

              if (!custom_alloc)
                os << fq << "* i = new " << fq << ";";
              else
                os << fq << "* i = static_cast< " << fq << "* > (" << endl
                   << "::xsde::cxx::alloc (sizeof (" << fq << ")));";

              if (!exceptions)
                os << endl
                   << "if (i == 0)" << endl
                   << "return false;"
                   << endl;

              if (custom_alloc)
              {
                if (exceptions)
                  os << "::xsde::cxx::alloc_guard ig (i);";

                os << "new (i) " << fq << ";";

                if (exceptions)
                  os << "ig.release ();";
              }

              os << "x." << name << " (i);";
            }

            if (exceptions)
              os << "s >> x." << name << " ();";
            else
              os << "if (!(s >> x." << name << " ()))" << endl
                 << "return false;";

            if (c.min () == 0)
            {
              os << "}"
                 << "else" << endl;

              if (fl)
                os << "x." << epresent (c) << " (false);";
              else
                os << "x." << name << " (0);";
            }
          }

          os << "break;"
             << "}";
        }
      };

      struct ChoiceInSequenceData: Traversal::Choice, Context
      {
        ChoiceInSequenceData (Context& c)
            : Context (c), particle_data_ (c)
        {
          contains_data_ >> particle_data_;
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.max () != 1)
          {
            String const& name (ename (c));

            if (exceptions)
              os << "s >> x." << name << " ();";
            else
              os << "if (!(s >> x." << name << " ()))" << endl
                 << "return false;";
          }
          else if (c.min () == 0)
          {
            Boolean fl (fixed_length (c));

            String const& name (ename (c));
            String const& present (epresent (c));

            os << "{"
               << "bool p;";

            if (exceptions)
              os << "s >> p;";
            else
              os << endl
                 << "if (!(s >> p))" << endl
                 << "return false;";

            os << endl
               << "if (p)"
               << "{";

            if (fl)
              os << "x." << present << " (true);";

            if (!fl)
            {
              String fq (scope (c) + L"::" + etype (c));

              if (!custom_alloc)
                os << fq << "* i = new " << fq << ";";
              else
                os << fq << "* i = static_cast< " << fq << "* > (" << endl
                   << "::xsde::cxx::alloc (sizeof (" << fq << ")));";

              if (!exceptions)
                os << endl
                   << "if (i == 0)" << endl
                   << "return false;"
                   << endl;

              if (custom_alloc)
              {
                if (exceptions)
                  os << "::xsde::cxx::alloc_guard ig (i);";

                os << "new (i) " << fq << ";";

                if (exceptions)
                  os << "ig.release ();";
              }

              os << "x." << name << " (i);";
            }

            if (exceptions)
              os << "s >> x." << name << " ();";
            else
              os << "if (!(s >> x." << name << " ()))" << endl
                 << "return false;";

            os << "}"
               << "else" << endl;

            if (fl)
              os << "x." << present << " (false);";
            else
              os << "x." << name << " (0);";

            os << "}";
          }
          else
          {
            String const& arm (earm (c));

            os << "{"
               << "unsigned int i;";

            if (exceptions)
              os << "s >> i;";
            else
              os << "if (!(s >> i))" << endl
                 << "return false;";

            os << "x." << arm << " (static_cast< " << scope (c) << "::" <<
              earm_tag (c) << " > (i));";

            os << "switch (x." << arm << " ())"
               << "{";

            Choice::contains (c, contains_data_);

            os << "default:" << endl
               << "break;"
               << "}"
               << "}";
          }
        }

      private:
        ChoiceParticleData particle_data_;
        Traversal::ContainsParticle contains_data_;
      };

      struct SequenceInSequenceData: Traversal::Sequence, Context
      {
        SequenceInSequenceData (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          if (s.max () != 1)
          {
            String const& name (ename (s));

            if (exceptions)
              os << "s >> x." << name << " ();";
            else
              os << "if (!(s >> x." << name << " ()))" << endl
                 << "return false;";
          }
          else if (s.min () == 0)
          {
            Boolean fl (fixed_length (s));

            String const& name (ename (s));
            String const& present (epresent (s));

            os << "{"
               << "bool p;";

            if (exceptions)
              os << "s >> p;";
            else
              os << endl
                 << "if (!(s >> p))" << endl
                 << "return false;";

            os << endl
               << "if (p)"
               << "{";

            if (fl)
              os << "x." << present << " (true);";

            if (!fl)
            {
              String fq (scope (s) + L"::" + etype (s));

              if (!custom_alloc)
                os << fq << "* i = new " << fq << ";";
              else
                os << fq << "* i = static_cast< " << fq << "* > (" << endl
                   << "::xsde::cxx::alloc (sizeof (" << fq << ")));";

              if (!exceptions)
                os << endl
                   << "if (i == 0)" << endl
                   << "return false;"
                   << endl;

              if (custom_alloc)
              {
                if (exceptions)
                  os << "::xsde::cxx::alloc_guard ig (i);";

                os << "new (i) " << fq << ";";

                if (exceptions)
                  os << "ig.release ();";
              }

              os << "x." << name << " (i);";
            }

            if (exceptions)
              os << "s >> x." << name << " ();";
            else
              os << "if (!(s >> x." << name << " ()))" << endl
                 << "return false;";

            os << "}"
               << "else" << endl;

            if (fl)
              os << "x." << present << " (false);";
            else
              os << "x." << name << " (0);";

            os << "}";
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
        All (Context& c, Traversal::ContainsParticle& contains_data)
            : Context (c), contains_data_ (contains_data)
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

            for (Streams::ConstIterator i (istreams.begin ());
                 i != istreams.end (); ++i)
            {
              os << (exceptions ? "void" : "bool") << endl
                 << "operator>> (" << istream (*i) << "& s," << endl
                 << scope << "::" << type << "& x)"
                 << "{"
                 << "XSDE_UNUSED (s);"
                 << "XSDE_UNUSED (x);"
                 << endl;

              All::contains (a, contains_data_);

              os << (exceptions ? "" : "return true;")
                 << "}";
            }
          }
        }

      private:
        Traversal::ContainsParticle& contains_data_;
      };

      struct Choice: Traversal::Choice, Context
      {
        Choice (Context& c, Boolean in_choice)
            : Context (c), in_choice_ (in_choice), particle_data_ (c)
        {
          contains_data_ >> particle_data_;
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          // When choice is in choice we generate nested class even
          // for min == max == 1.
          //
          if (in_choice_ || c.max () != 1 || c.min () == 0)
          {
            String const& arm (earm (c));
            String const& type (etype (c));
            String const& scope (Context::scope (c));

            for (Streams::ConstIterator i (istreams.begin ());
                 i != istreams.end (); ++i)
            {
              os << (exceptions ? "void" : "bool") << endl
                 << "operator>> (" << istream (*i) << "& s," << endl
                 << scope << "::" << type << "& x)"
                 << "{"
                 << "XSDE_UNUSED (s);"
                 << endl
                 << "unsigned int i;";

              if (exceptions)
                os << "s >> i;";
              else
                os << "if (!(s >> i))" << endl
                   << "return false;";

              os << "x." << arm << " (static_cast< " << scope << "::" <<
                type << "::" << earm_tag (c) << " > (i));";

              os << "switch (x." << arm << " ())"
                 << "{";

              Choice::contains (c, contains_data_);

              os << "default:" << endl
                 << "break;"
                 << "}"
                 << (exceptions ? "" : "return true;")
                 << "}";
            }
          }

          Choice::contains (c);
        }

      private:
        Boolean in_choice_;

        ChoiceParticleData particle_data_;
        Traversal::ContainsParticle contains_data_;
      };


      struct Sequence: Traversal::Sequence, Context
      {
        Sequence (Context& c,
                  Boolean in_choice,
                  Traversal::ContainsParticle& contains_data)
            : Context (c),
              in_choice_ (in_choice),
              contains_data_ (contains_data)
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

            for (Streams::ConstIterator i (istreams.begin ());
                 i != istreams.end (); ++i)
            {
              os << (exceptions ? "void" : "bool") << endl
                 << "operator>> (" << istream (*i) << "& s," << endl
                 << scope << "::" << type << "& x)"
                 << "{"
                 << "XSDE_UNUSED (s);"
                 << "XSDE_UNUSED (x);"
                 << endl;

              Sequence::contains (s, contains_data_);

              os << (exceptions ? "" : "return true;")
                 << "}";
            }
          }

          Sequence::contains (s);
        }

      private:
        Boolean in_choice_;
        Traversal::ContainsParticle& contains_data_;
      };

      struct Complex : Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c),

              // Use ret instead of base to get auto-conversion
              // for fundamental types from the wrappers.
              //
              base_name_ (c, TypeName::ret),

              // Data
              //
              attribute_data_ (c),
              element_data_ (c),
              all_data_ (c),
              choice_in_sequence_data_ (c),
              sequence_in_sequence_data_ (c),

              // Nested classes.
              //
              all_ (c, all_contains_data_),
              choice_in_choice_ (c, true),
              choice_in_sequence_ (c, false),
              sequence_in_choice_ (c, true, sequence_contains_data_),
              sequence_in_sequence_ (c, false, sequence_contains_data_)
        {
          // Data.
          //
          attribute_names_data_ >> attribute_data_;

          all_data_ >> all_contains_data_ >> element_data_;

          sequence_in_sequence_data_ >> sequence_contains_data_;
          sequence_contains_data_ >> element_data_;
          sequence_contains_data_ >> choice_in_sequence_data_;
          sequence_contains_data_ >> sequence_in_sequence_data_;

          contains_compositor_data_ >> all_data_;
          contains_compositor_data_ >> choice_in_sequence_data_;
          contains_compositor_data_ >> sequence_in_sequence_data_;

          // Nested classes.
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

          for (Streams::ConstIterator i (istreams.begin ());
               i != istreams.end (); ++i)
          {
            os << (exceptions ? "void" : "bool") << endl
               << "operator>> (" << istream (*i) << "& s," << endl
               << name << "& x)"
               << "{"
               << "XSDE_UNUSED (s);"
               << "XSDE_UNUSED (x);"
               << endl;

            if (c.inherits_p ())
            {
              SemanticGraph::Type& b (c.inherits ().base ());

              Boolean c_str (false);

              if (!stl)
              {
                StringType test (c_str);
                test.dispatch (b);
              }

              // Special handling for C-string base.
              //
              if (c_str)
              {
                os << "char* b;";

                if (exceptions)
                  os << "s >> b;";
                else
                  os << "if (!(s >> b))" << endl
                     << "return false;"
                     << endl;

                os << "x.base_value (b);"
                   << endl;
              }
              else
              {
                base_name_.dispatch (b);
                os << " b = x;";

                if (exceptions)
                  os << "s >> b;";
                else
                  os << "if (!(s >> b))" << endl
                     << "return false;"
                     << endl;
              }
            }

            if (!restriction)
            {
              Complex::names (c, attribute_names_data_);

              if (c.contains_compositor_p ())
                Complex::contains_compositor (c, contains_compositor_data_);
            }

            os << (exceptions ? "" : "return true;")
               << "}";
          }

          // Operators for nested classes.
          //
          if (!restriction && c.contains_compositor_p ())
            Complex::contains_compositor (c, contains_compositor_);
        }

      private:
        TypeName base_name_;

        // Data.
        //
        AttributeData attribute_data_;
        Traversal::Names attribute_names_data_;

        ElementData element_data_;
        AllData all_data_;
        ChoiceInSequenceData choice_in_sequence_data_;
        SequenceInSequenceData sequence_in_sequence_data_;
        Traversal::ContainsParticle all_contains_data_;
        Traversal::ContainsParticle sequence_contains_data_;

        Traversal::ContainsCompositor contains_compositor_data_;

        // Nested classes.
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
    generate_extraction_source (Context& ctx)
    {
      Traversal::Schema schema;
      Sources sources;
      Traversal::Names names_ns, names;

      Namespace ns (ctx);

      List list (ctx);
      Union union_ (ctx);
      Complex complex (ctx);
      Enumeration enumeration (ctx, complex);

      schema >> sources >> schema;
      schema >> names_ns >> ns >> names;

      names >> list;
      names >> union_;
      names >> complex;
      names >> enumeration;

      schema.dispatch (ctx.schema_root);
    }
  }
}
