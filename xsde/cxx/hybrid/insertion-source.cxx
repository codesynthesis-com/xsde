// file      : xsd/cxx/hybrid/insertion-source.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/hybrid/insertion-source.hxx>

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

          for (Streams::ConstIterator i (ostreams.begin ());
               i != ostreams.end (); ++i)
          {
            os << (exceptions ? "void" : "bool") << endl
               << "operator<< (" << ostream (*i) << "& s," << endl
               << "const " << name << "& x)"
               << "{"
               << "unsigned int i = x;"
               << (exceptions ? "" : "return ") << "s << i;"
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

          for (Streams::ConstIterator i (ostreams.begin ());
               i != ostreams.end (); ++i)
          {
            os << (exceptions ? "void" : "bool") << endl
               << "operator<< (" << ostream (*i) << "& s," << endl
               << "const " << name << "& x)"
               << "{"
               << "const ";

            base_name_.dispatch (l.argumented ().type ());

            os << "& b = x;"
               << (exceptions ? "" : "return ") << "s << b;"
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

          for (Streams::ConstIterator i (ostreams.begin ());
               i != ostreams.end (); ++i)
          {
            os << (exceptions ? "void" : "bool") << endl
               << "operator<< (" << ostream (*i) << "& s," << endl
               << "const " << name << "& x)"
               << "{"
               << (exceptions ? "" : "return ") << "s << x." << value << " ();"
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
          // Don't waste space on fixed attributes.
          //
          if (a.fixed_p ())
            return;

          if (a.optional_p ())
          {
            if (!a.default_p ())
            {
              String const& present (epresent (a));

              if (exceptions)
                os << "s << x." << present << " ();";
              else
                os << "if (!(s << x." << present << " ()))" << endl
                   << "return false;";

              os << endl
                 << "if (x." << present << " ())"
                 << "{";
            }
            else
            {
              String const& default_ (edefault (a));

              if (exceptions)
                os << "s << x." << default_ << " ();";
              else
                os << "if (!(s << x." << default_ << " ()))" << endl
                   << "return false;";

              os << endl
                 << "if (!x." << default_ << " ())"
                 << "{";
            }
          }

          String const& name (ename (a));

          if (exceptions)
            os << "s << x." << name << " ();";
          else
            os << "if (!(s << x." << name << " ()))" << endl
               << "return false;";

          if (a.optional_p ())
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
          if (e.max () == 1 && e.min () == 0)
          {
            String const& present (epresent (e));

            if (exceptions)
              os << "s << x." << present << " ();";
            else
              os << "if (!(s << x." << present << " ()))" << endl
                 << "return false;";

            os << endl
               << "if (x." << present << " ())"
               << "{";
          }

          String const& name (ename (e));

          if (exceptions)
            os << "s << x." << name << " ();";
          else
            os << "if (!(s << x." << name << " ()))" << endl
               << "return false;";

          if (e.max () == 1 && e.min () == 0)
            os << "}";
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
            String const& name (ename (a));
            String const& present (epresent (a));

            if (exceptions)
              os << "s << x." << present << " ();";
            else
              os << "if (!(s << x." << present << " ()))" << endl
                 << "return false;";

            os << endl
               << "if (x." << present << " ())"
               << "{";

            if (exceptions)
              os << "s << x." << name << " ();";
            else
              os << "if (!(s << x." << name << " ()))" << endl
                 << "return false;";

            os << "}";
          }
          else
            All::contains (a);
        }
      };

      struct ChoiceParticleData: ElementData,
                                 Traversal::Compositor
      {
        ChoiceParticleData (Context& c)
            : ElementData (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          os << "case " << scope (e) << "::" << etag (e) << ":"
             << "{";

          ElementData::traverse (e);

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
          if (c.max () == 1 && c.min () == 0)
          {
            String const& present (epresent (c));

            if (exceptions)
              os << "s << x." << present << " ();";
            else
              os << "if (!(s << x." << present << " ()))" << endl
                 << "return false;";

            os << endl
               << "if (x." << present << " ())"
               << "{";
          }

          String const& name (ename (c));

          if (exceptions)
            os << "s << x." << name << " ();";
          else
            os << "if (!(s << x." << name << " ()))" << endl
               << "return false;";

          if (c.max () == 1 && c.min () == 0)
            os << "}";

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
              os << "s << x." << name << " ();";
            else
              os << "if (!(s << x." << name << " ()))" << endl
                 << "return false;";
          }
          else if (c.min () == 0)
          {
            String const& name (ename (c));
            String const& present (epresent (c));

            if (exceptions)
              os << "s << x." << present << " ();";
            else
              os << "if (!(s << x." << present << " ()))" << endl
                 << "return false;";

            os << endl
               << "if (x." << present << " ())"
               << "{";

            if (exceptions)
              os << "s << x." << name << " ();";
            else
              os << "if (!(s << x." << name << " ()))" << endl
                 << "return false;";

            os << "}";
          }
          else
          {
            String const& arm (earm (c));

            if (exceptions)
              os << "s << static_cast< unsigned int > (x." << arm << " ());";
            else
              os << "if (!(s << static_cast< unsigned int > (x." <<
                arm << " ())))" << endl
                 << "return false;";

            os << "switch (x." << arm << " ())"
               << "{";

            Choice::contains (c, contains_data_);

            os << "default:" << endl
               << "break;"
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
              os << "s << x." << name << " ();";
            else
              os << "if (!(s << x." << name << " ()))" << endl
                 << "return false;";
          }
          else if (s.min () == 0)
          {
            String const& name (ename (s));
            String const& present (epresent (s));

            if (exceptions)
              os << "s << x." << present << " ();";
            else
              os << "if (!(s << x." << present << " ()))" << endl
                 << "return false;";

            os << endl
               << "if (x." << present << " ())"
               << "{";

            if (exceptions)
              os << "s << x." << name << " ();";
            else
              os << "if (!(s << x." << name << " ()))" << endl
                 << "return false;";

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

            for (Streams::ConstIterator i (ostreams.begin ());
                 i != ostreams.end (); ++i)
            {
              os << (exceptions ? "void" : "bool") << endl
                 << "operator<< (" << ostream (*i) << "& s," << endl
                 << "const " << scope << "::" << type << "& x)"
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

            for (Streams::ConstIterator i (ostreams.begin ());
                 i != ostreams.end (); ++i)
            {
              os << (exceptions ? "void" : "bool") << endl
                 << "operator<< (" << ostream (*i) << "& s," << endl
                 << "const " << scope << "::" << type << "& x)"
                 << "{"
                 << "XSDE_UNUSED (s);"
                 << endl;

              if (exceptions)
                os << "s << static_cast< unsigned int > (x." << arm << " ());";
              else
                os << "if (!(s << static_cast< unsigned int > (x." <<
                  arm << " ())))" << endl
                 << "return false;";

              os << "switch (x." << earm (c) << " ())"
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

            for (Streams::ConstIterator i (ostreams.begin ());
                 i != ostreams.end (); ++i)
            {
              os << (exceptions ? "void" : "bool") << endl
                 << "operator<< (" << ostream (*i) << "& s," << endl
                 << "const " << scope << "::" << type << "& x)"
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

              // Use ro_ret instead of base to get auto-conversion
              // for fundamental types from the wrappers.
              //
              base_name_ (c, TypeName::ro_ret),

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

          for (Streams::ConstIterator i (ostreams.begin ());
               i != ostreams.end (); ++i)
          {
            os << (exceptions ? "void" : "bool") << endl
               << "operator<< (" << ostream (*i) << "& s," << endl
               << "const " << name << "& x)"
               << "{"
               << "XSDE_UNUSED (s);"
               << "XSDE_UNUSED (x);"
               << endl;

            if (c.inherits_p ())
            {
              base_name_.dispatch (c.inherits ().base ());
              os << " b = x;";

              if (exceptions)
                os << "s << b;";
              else
                os << "if (!(s << b))" << endl
                   << "return false;"
                   << endl;
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
    generate_insertion_source (Context& ctx)
    {
      Traversal::Schema schema;
      Traversal::Sources sources;
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
