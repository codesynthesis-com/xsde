// file      : xsde/cxx/hybrid/tree-size-processor.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/hybrid/elements.hxx>
#include <cxx/hybrid/tree-size-processor.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

#include <cult/containers/set.hxx>

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      typedef Cult::Containers::Set<String> TypeSet;

      Boolean
      test (SemanticGraph::Type& t)
      {
        return t.context ().count ("fixed") != 0;
      }

      Void
      set (SemanticGraph::Type& t, Boolean v)
      {
        t.context ().set ("fixed", v);
      }

      Void
      set (SemanticGraph::Compositor& c, Boolean v)
      {
        c.context ().set ("fixed", v);
      }

      Boolean
      get (SemanticGraph::Type& t)
      {
        return t.context ().get<Boolean> ("fixed");
      }

      //
      //
      struct Particle: Traversal::Element
      {
        Particle (Boolean& fixed, Traversal::NodeBase& type_traverser)
            : fixed_ (fixed), type_traverser_ (type_traverser)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          if (fixed_)
          {
            // Check cardinality.
            //
            if (e.max () != 1)
            {
              fixed_ = false;
              return;
            }

            // Check the type.
            //
            SemanticGraph::Type& t (e.type ());

            if (!test (t))
              type_traverser_.dispatch (t);

            fixed_ = get (t);
          }
        }

        /*
        virtual Void
        traverse (SemanticGraph::Any&)
        {
          // Types with wildcards are always variable length.
          //
          if (fixed_)
            fixed_ = false;
        }
        */

      private:
        Boolean& fixed_;
        Traversal::NodeBase& type_traverser_;
      };

      struct Compositor: Traversal::All,
                         Traversal::Choice,
                         Traversal::Sequence
      {
        Compositor (Boolean& fixed, Traversal::NodeBase& type_traverser)
            : fixed_ (fixed), type_traverser_ (type_traverser)
        {
        }

        virtual Void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}
          // and it can only contain particles. In case of an optional
          // compositor we need to assign the size for the nested class.
          //
          if (a.min () == 0)
          {
            Boolean fixed = true;

            {
              Particle particle (fixed, type_traverser_);
              Traversal::ContainsParticle contains_particle;

              contains_particle >> particle;

              All::contains (a, contains_particle);
            }

            set (a, fixed);

            if (!fixed)
              fixed_ = false;
          }
          else
            All::contains (a);
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          // Here we need to assign the size for the nested class in
          // all three cases because this choice might be in choice.
          //
          Boolean fixed = true;

          {
            Particle particle (fixed, type_traverser_);
            Compositor compositor (fixed, type_traverser_);
            Traversal::ContainsParticle contains_particle;

            contains_particle >> compositor >> contains_particle;
            contains_particle >> particle;

            Choice::contains (c, contains_particle);
          }

          set (c, fixed);

          if (c.max () != 1 || !fixed)
            fixed_ = false;
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          // Here we need to assign the size for the nested class in
          // all three cases because this sequence might be in choice.
          //
          Boolean fixed = true;

          {
            Particle particle (fixed, type_traverser_);
            Compositor compositor (fixed, type_traverser_);
            Traversal::ContainsParticle contains_particle;

            contains_particle >> compositor >> contains_particle;
            contains_particle >> particle;

            Sequence::contains (s, contains_particle);
          }

          set (s, fixed);

          if (s.max () != 1 || !fixed)
            fixed_ = false;
        }

      private:
        Boolean& fixed_;
        Traversal::NodeBase& type_traverser_;
      };

      struct Attribute: Traversal::Attribute
      {
        Attribute (Boolean& fixed, Traversal::NodeBase& type_traverser)
            : fixed_ (fixed), type_traverser_ (type_traverser)
        {
        }

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          if (fixed_)
          {
            SemanticGraph::Type& t (a.type ());

            if (!test (t))
              type_traverser_.dispatch (t);

            fixed_ = get (t);
          }
        }

        /*
        virtual Void
        traverse (SemanticGraph::AnyAttribute&)
        {
          // Types with wildcards are always variable length.
          //
          if (fixed_)
            fixed_ = false;
        }
        */

      private:
        Boolean& fixed_;
        Traversal::NodeBase& type_traverser_;
      };

      //
      //
      struct Type: Traversal::List,
                   Traversal::Union,
                   Traversal::Complex
      {
        Type (TypeSet& custom_data, Boolean stl_)
            : custom_data_ (custom_data), stl (stl_)
        {
        }

        virtual Void
        traverse (SemanticGraph::List& l)
        {
          if (!test (l))
            set (l, false);
        }

        virtual Void
        traverse (SemanticGraph::Union& u)
        {
          if (!test (u))
          {
            Boolean fixed (stl);

            // Check for custom data.
            //
            if (fixed)
            {
              if (custom_data_.find (u.name ()) != custom_data_.end ())
                fixed = false;
            }

            set (u, fixed);
          }
        }

        virtual Void
        traverse (SemanticGraph::Complex& c)
        {
          SemanticGraph::Context& ctx (c.context ());

          if (test (c))
            return;

          if (ctx.count ("recurse"))
            set (c, false);
          else
          {
            ctx.set ("recurse", true);

            Boolean fixed = true;

            // First check our base.
            //
            if (c.inherits_p ())
            {
              SemanticGraph::Type& b (c.inherits ().base ());

              if (!test (b))
                dispatch (b);

              fixed = get (b);
            }

            // Check particles. Do this even if fixed is already false
            // because we need to calculate sizes for nested classes.
            //
            if (c.contains_compositor_p ())
            {
              Particle particle (fixed, *this);
              Compositor compositor (fixed, *this);
              Traversal::ContainsCompositor contains_compositor;
              Traversal::ContainsParticle contains_particle;

              contains_compositor >> compositor;

              contains_particle >> compositor >> contains_particle;
              contains_particle >> particle;

              Complex::contains_compositor (c, contains_compositor);
            }

            // Check attributes.
            //
            if (fixed)
            {
              Attribute attr (fixed, *this);
              Traversal::Names names (attr);

              Complex::names (c, names);
            }

            // Check for custom data.
            //
            if (fixed)
            {
              if (custom_data_.find (c.name ()) != custom_data_.end ())
                fixed = false;
            }

            // If we recursed then we should have the value already set.
            //
            if (!test (c))
              set (c, fixed);

            ctx.remove ("recurse");
          }
        }

      private:
        TypeSet& custom_data_;
        Boolean stl;
      };

      struct FundType : Traversal::AnyType,
                        Traversal::AnySimpleType,

                        Traversal::Fundamental::Byte,
                        Traversal::Fundamental::UnsignedByte,
                        Traversal::Fundamental::Short,
                        Traversal::Fundamental::UnsignedShort,
                        Traversal::Fundamental::Int,
                        Traversal::Fundamental::UnsignedInt,
                        Traversal::Fundamental::Long,
                        Traversal::Fundamental::UnsignedLong,
                        Traversal::Fundamental::Integer,
                        Traversal::Fundamental::NonPositiveInteger,
                        Traversal::Fundamental::NonNegativeInteger,
                        Traversal::Fundamental::PositiveInteger,
                        Traversal::Fundamental::NegativeInteger,

                        Traversal::Fundamental::Boolean,

                        Traversal::Fundamental::Float,
                        Traversal::Fundamental::Double,
                        Traversal::Fundamental::Decimal,

                        Traversal::Fundamental::String,
                        Traversal::Fundamental::NormalizedString,
                        Traversal::Fundamental::Token,
                        Traversal::Fundamental::Name,
                        Traversal::Fundamental::NameToken,
                        Traversal::Fundamental::NameTokens,
                        Traversal::Fundamental::NCName,
                        Traversal::Fundamental::Language,

                        Traversal::Fundamental::QName,

                        Traversal::Fundamental::Id,
                        Traversal::Fundamental::IdRef,
                        Traversal::Fundamental::IdRefs,

                        Traversal::Fundamental::AnyURI,

                        Traversal::Fundamental::Base64Binary,
                        Traversal::Fundamental::HexBinary,

                        Traversal::Fundamental::Date,
                        Traversal::Fundamental::DateTime,
                        Traversal::Fundamental::Duration,
                        Traversal::Fundamental::Day,
                        Traversal::Fundamental::Month,
                        Traversal::Fundamental::MonthDay,
                        Traversal::Fundamental::Year,
                        Traversal::Fundamental::YearMonth,
                        Traversal::Fundamental::Time,

                        Traversal::Fundamental::Entity,
                        Traversal::Fundamental::Entities

      {
        FundType (Boolean stl_)
            : stl (stl_)
        {
        }

        // anyType & anySimpleType.
        //
        virtual Void
        traverse (SemanticGraph::AnyType& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::AnySimpleType& t)
        {
          set (t, true);
        }

        // Boolean.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Boolean& t)
        {
          set (t, true);
        }

        // Integral types.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Byte& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedByte& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Short& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedShort& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Int& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedInt& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Long& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedLong& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Integer& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NonPositiveInteger& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NonNegativeInteger& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::PositiveInteger& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NegativeInteger& t)
        {
          set (t, true);
        }

        // Floats.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Float& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Double& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Decimal& t)
        {
          set (t, true);
        }

        // Strings.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::String& t)
        {
          set (t, stl);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NormalizedString& t)
        {
          set (t, stl);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Token& t)
        {
          set (t, stl);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NameToken& t)
        {
          set (t, stl);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NameTokens& t)
        {
          set (t, false);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Name& t)
        {
          set (t, stl);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NCName& t)
        {
          set (t, stl);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Language& t)
        {
          set (t, stl);
        }


        // Qualified name.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::QName& t)
        {
          set (t, stl);
        }


        // ID/IDREF.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Id& t)
        {
          set (t, stl);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::IdRef& t)
        {
          set (t, stl);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::IdRefs& t)
        {
          set (t, false);
        }

        // URI.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::AnyURI& t)
        {
          set (t, stl);
        }

        // Binary.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Base64Binary& t)
        {
          set (t, false);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::HexBinary& t)
        {
          set (t, false);
        }


        // Date/time.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Date& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::DateTime& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Duration& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Day& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Month& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::MonthDay& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Year& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::YearMonth& t)
        {
          set (t, true);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Time& t)
        {
          set (t, true);
        }

        // Entity.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Entity& t)
        {
          set (t, stl);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Entities& t)
        {
          set (t, false);
        }

      private:
        Boolean stl;
      };

      // Go into sourced/included/imported schemas while making sure
      // we don't process the same stuff more than once.
      //
      struct Uses: Traversal::Sources,
                   Traversal::Includes,
                   Traversal::Imports
      {
        virtual Void
        traverse (SemanticGraph::Sources& sr)
        {
          SemanticGraph::Schema& s (sr.schema ());

          if (!s.context ().count ("cxx-hybrid-size-processor-seen"))
          {
            s.context ().set ("cxx-hybrid-size-processor-seen", true);
            Traversal::Sources::traverse (sr);
          }
        }

        virtual Void
        traverse (SemanticGraph::Includes& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count ("cxx-hybrid-size-processor-seen"))
          {
            s.context ().set ("cxx-hybrid-size-processor-seen", true);
            Traversal::Includes::traverse (i);
          }
        }

        virtual Void
        traverse (SemanticGraph::Imports& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count ("cxx-hybrid-size-processor-seen"))
          {
            s.context ().set ("cxx-hybrid-size-processor-seen", true);
            Traversal::Imports::traverse (i);
          }
        }
      };

      Void
      process_impl (CLI::Options const& ops,
                    SemanticGraph::Schema& tu,
                    SemanticGraph::Path const&)
      {
        Boolean stl (!ops.value<CLI::no_stl> ());

        // Root schema in the file-per-type mode is just a bunch
        // of includes without a namespace.
        //
        SemanticGraph::Schema::NamesIterator i (tu.names_begin ());

        if (i != tu.names_end () &&
            i->named ().name () == L"http://www.w3.org/2001/XMLSchema")
        {
          // XML Schema namespace.
          //
          Traversal::Schema schema;

          Traversal::Names schema_names;
          Traversal::Namespace ns;
          Traversal::Names ns_names;
          FundType fund_type (stl);

          schema >> schema_names >> ns >> ns_names >> fund_type;

          schema.dispatch (tu);
        }
        else
        {
          // Pass one - assign sizes to fundamental types.
          //
          {
            Traversal::Schema schema;
            Traversal::Implies implies;
            Traversal::Schema xs_schema;

            schema >> implies >> xs_schema;

            Traversal::Names xs_schema_names;
            Traversal::Namespace ns;
            Traversal::Names ns_names;
            FundType fund_type (stl);

            xs_schema >> xs_schema_names >> ns >> ns_names >> fund_type;

            schema.dispatch (tu);
          }

          // Note that we check first if this schema has already been
          // processed which may happen in the file-per-type compilation
          // mode.
          //
          if (!tu.context ().count ("cxx-hybrid-size-processor-seen"))
          {
            // Prepare a set of types with custom data. Here we are
            // only interested in detecting global types. If a type
            // has a nested compositor type which has custom data
            // then the type itself will automatically be variable
            // size.
            //
            TypeSet custom_data_types;

            {
              typedef Cult::Containers::Vector<NarrowString> CustomData;
              CustomData const& cd (ops.value<CLI::custom_data> ());

              for (CustomData::ConstIterator i (cd.begin ());
                   i != cd.end (); ++i)
              {
                String n (*i);
                custom_data_types.insert (String (n, 0, n.find (L"::")));
              }
            }

            Traversal::Schema schema;
            Uses uses;

            schema >> uses >> schema;

            Traversal::Names schema_names;
            Traversal::Namespace ns;
            Traversal::Names ns_names;
            Type type (custom_data_types, stl);

            schema >> schema_names >> ns >> ns_names >> type;

            // Some twisted schemas do recusive self-inclusion.
            //
            tu.context ().set ("cxx-hybrid-size-processor-seen", true);

            schema.dispatch (tu);
          }
        }
      }
    }

    Void TreeSizeProcessor::
    process (CLI::Options const& ops,
             SemanticGraph::Schema& tu,
             SemanticGraph::Path const& file)
    {
      process_impl (ops, tu, file);
    }
  }
}
