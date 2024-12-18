// file      : xsde/cxx/hybrid/tree-size-processor.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <set>
#include <map>
#include <vector>
#include <iostream>

#include <xsde/cxx/hybrid/elements.hxx>
#include <xsde/cxx/hybrid/tree-size-processor.hxx>

#include <libxsd-frontend/semantic-graph.hxx>
#include <libxsd-frontend/traversal.hxx>

using namespace std;

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      typedef set<String> TypeSet;

      struct CustomType
      {
        bool fixed;
        String base;
      };

      typedef map<String, CustomType> CustomTypeMap;

      bool
      test (SemanticGraph::Type& t)
      {
        return t.context ().count ("fixed") != 0;
      }

      void
      set (SemanticGraph::Type& t, bool v)
      {
        t.context ().set ("fixed", v);
      }

      void
      set (SemanticGraph::Compositor& c, bool v)
      {
        c.context ().set ("fixed", v);
      }

      bool
      get (SemanticGraph::Type& t)
      {
        return t.context ().get<bool> ("fixed");
      }

      //
      //
      struct Particle: Traversal::Element
      {
        Particle (bool& fixed,
                  bool& poly,
                  Traversal::NodeBase& type_traverser)
            : fixed_ (fixed), poly_ (poly), type_traverser_ (type_traverser)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          // Check the type. We need to do it even if fixed_ is
          // false to detect recursive types.
          //
          SemanticGraph::Type& t (e.type ());
          type_traverser_.dispatch (t);

          if (t.context ().count ("polymorphic"))
            poly_ = true;

          if (fixed_)
          {
            if (e.max () != 1)
              fixed_ = false;
            else
              fixed_ = get (t);
          }
        }

        /*
        virtual void
        traverse (SemanticGraph::Any&)
        {
          // Types with wildcards are always variable length.
          //
          if (fixed_)
            fixed_ = false;
        }
        */

      private:
        bool& fixed_;
        bool& poly_;
        Traversal::NodeBase& type_traverser_;
      };

      struct Compositor: Traversal::All,
                         Traversal::Choice,
                         Traversal::Sequence
      {
        Compositor (bool& fixed,
                    bool& poly,
                    Traversal::NodeBase& type_traverser)
            : fixed_ (fixed), poly_ (poly), type_traverser_ (type_traverser)
        {
        }

        virtual void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}
          // and it can only contain particles. In case of an optional
          // compositor we need to assign the size for the nested class.
          //
          if (a.min () == 0)
          {
            bool fixed = true;

            {
              Particle particle (fixed, poly_, type_traverser_);
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

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          // Here we need to assign the size for the nested class in
          // all three cases because this choice might be in choice.
          //
          bool fixed = true;

          {
            Particle particle (fixed, poly_, type_traverser_);
            Compositor compositor (fixed, poly_, type_traverser_);
            Traversal::ContainsParticle contains_particle;

            contains_particle >> compositor >> contains_particle;
            contains_particle >> particle;

            Choice::contains (c, contains_particle);
          }

          set (c, fixed);

          if (c.max () != 1 || !fixed)
            fixed_ = false;
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          // Here we need to assign the size for the nested class in
          // all three cases because this sequence might be in choice.
          //
          bool fixed = true;

          {
            Particle particle (fixed, poly_, type_traverser_);
            Compositor compositor (fixed, poly_, type_traverser_);
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
        bool& fixed_;
        bool& poly_;
        Traversal::NodeBase& type_traverser_;
      };

      struct Attribute: Traversal::Attribute
      {
        Attribute (bool& fixed, Traversal::NodeBase& type_traverser)
            : fixed_ (fixed), type_traverser_ (type_traverser)
        {
        }

        virtual void
        traverse (SemanticGraph::Attribute& a)
        {
          // Simple types cannot be recursive.
          //
          if (fixed_)
          {
            SemanticGraph::Type& t (a.type ());

            if (!test (t))
              type_traverser_.dispatch (t);

            fixed_ = get (t);
          }
        }

        /*
        virtual void
        traverse (SemanticGraph::AnyAttribute&)
        {
          // Types with wildcards are always variable length.
          //
          if (fixed_)
            fixed_ = false;
        }
        */

      private:
        bool& fixed_;
        Traversal::NodeBase& type_traverser_;
      };

      //
      //
      struct Type: Traversal::List,
                   Traversal::Union,
                   Traversal::Complex,
                   Traversal::Enumeration
      {
        Type (bool& valid,
              TypeSet& custom_data,
              CustomTypeMap& custom_type_map,
              TypeSet& poly_types,
              bool stl_,
              bool poly_,
              bool enum_mapping)
            : valid_ (valid),
              custom_data_ (custom_data),
              custom_type_map_ (custom_type_map),
              poly_types_ (poly_types),
              stl (stl_),
              poly (poly_),
              enum_ (enum_mapping)
        {
        }

        virtual void
        traverse (SemanticGraph::List& l)
        {
          if (!test (l))
            set (l, false);
        }

        virtual void
        traverse (SemanticGraph::Union& u)
        {
          if (!test (u))
          {
            bool fixed (stl);

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

        virtual void
        traverse (SemanticGraph::Enumeration& e)
        {
          if (!test (e))
          {
            // First process our base since enum_mapping() needs the
            // polymorphic property determined.
            //
            SemanticGraph::Type& b (e.inherits ().base ());

            if (!test (b))
              dispatch (b);

            SemanticGraph::Enumeration* base_enum (0);

            if (!enum_ || !Context::enum_mapping (e, &base_enum))
            {
              traverse (static_cast<SemanticGraph::Complex&> (e));
              return;
            }

            bool fixed (true);

            if (base_enum && !get (b))
              fixed = false;

            // Check for custom data.
            //
            if (fixed)
            {
              if (custom_data_.find (e.name ()) != custom_data_.end ())
                fixed = false;
            }

            set (e, fixed);
          }
        }

        virtual void
        traverse (SemanticGraph::Complex& c)
        {
          SemanticGraph::Context& ctx (c.context ());

          if (ctx.count ("recurse"))
          {
            if (!test (c))
              set (c, false);

            ctx.set ("recursive", true);

            // Mark all the types involved in the cycle as recursive.
            //
            for (Path::reverse_iterator i (path_.rbegin ()); *i != &c; ++i)
            {
              (*i)->context ().set ("recursive", true);
            }
          }
          else if (!test (c))
          {
            ctx.set ("recurse", true);
            path_.push_back (&c);

            bool fixed = true;

            // First check our base.
            //
            if (c.inherits_p ())
            {
              SemanticGraph::Type& b (c.inherits ().base ());

              if (!test (b))
                dispatch (b);

              fixed = get (b);

              // If our base is polymorphic and recursive, then we
              // are also automatically recursive, since we can be
              // substiuted for the base.
              //
              if (b.context ().count ("polymorphic") &&
                  b.context ().count ("recursive"))
                ctx.set ("recursive", true);
            }

            // Check particles. Do this even if fixed is already false
            // because we need to calculate sizes for nested classes.
            //
            if (c.contains_compositor_p ())
            {
              bool poly (false);

              Particle particle (fixed, poly, *this);
              Compositor compositor (fixed, poly, *this);
              Traversal::ContainsCompositor contains_compositor;
              Traversal::ContainsParticle contains_particle;

              contains_compositor >> compositor;

              contains_particle >> compositor >> contains_particle;
              contains_particle >> particle;

              Complex::contains_compositor (c, contains_compositor);

              // If this element's type is polymorphic then mark this type as
              // recursive. The set of derived types is open-ended and one of
              // them may create a cycle involving this type.
              //
              if (poly)
                ctx.set ("recursive", true);
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

            path_.pop_back ();
            ctx.remove ("recurse");
          }
        }

      private:
        void
        set (SemanticGraph::Type& t, bool v)
        {
          using SemanticGraph::Complex;

          String const& name (t.name ());

          // Check if this type is polymorphic.
          //
          if (poly)
          {
            SemanticGraph::Context& ctx (t.context ());

            if (!ctx.count ("polymorphic"))
            {
              // If our base is polymorphic then we are as well.
              //
              bool pb (false);
              if (Complex* c = dynamic_cast<Complex*> (&t))
              {
                pb = c->inherits_p () &&
                  c->inherits ().base ().context ().count ("polymorphic");
              }

              if (pb || poly_types_.find (name) != poly_types_.end ())
              {
                ctx.set ("polymorphic", true);
                v = false;
              }
            }
            else
              v = false;
          }

          // Check if this is a custom type.
          //
          CustomTypeMap::iterator i = custom_type_map_.find (name);

          if (i != custom_type_map_.end ())
          {
            if (i->second.base && i->second.fixed && !v)
            {
              wcerr << t.file () << ":" << t.line () << ":" << t.column ()
                    << ": error: generated base type '" << i->second.base
                    << "' is variable-length while the custom type is "
                    << "declared fixed-length" << endl;

              valid_ = false;
            }

            Hybrid::set (t, i->second.fixed);
          }
          else
            Hybrid::set (t, v);
        }

      private:
        bool& valid_;
        TypeSet& custom_data_;
        CustomTypeMap& custom_type_map_;
        TypeSet& poly_types_;
        bool stl;
        bool poly;
        bool enum_;

        typedef vector<SemanticGraph::Complex*> Path;
        Path path_;
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
        FundType (bool stl_, TypeSet& poly_types)
            : stl (stl_), poly_types_ (poly_types)
        {
        }

        // anyType & anySimpleType.
        //
        virtual void
        traverse (SemanticGraph::AnyType& t)
        {
          set (t, false);

          // Check if this type is marked polymorphic.
          //
          if (poly_types_.find (t.name ()) != poly_types_.end ())
            t.context ().set ("polymorphic", true);
        }

        virtual void
        traverse (SemanticGraph::AnySimpleType& t)
        {
          set (t, stl);
        }

        // Boolean.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Boolean& t)
        {
          set (t, true);
        }

        // Integral types.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Byte& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedByte& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Short& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedShort& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Int& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedInt& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Long& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedLong& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Integer& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NonPositiveInteger& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NonNegativeInteger& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::PositiveInteger& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NegativeInteger& t)
        {
          set (t, true);
        }

        // Floats.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Float& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Double& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Decimal& t)
        {
          set (t, true);
        }

        // Strings.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::String& t)
        {
          set (t, stl);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NormalizedString& t)
        {
          set (t, stl);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Token& t)
        {
          set (t, stl);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NameToken& t)
        {
          set (t, stl);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NameTokens& t)
        {
          set (t, false);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Name& t)
        {
          set (t, stl);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NCName& t)
        {
          set (t, stl);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Language& t)
        {
          set (t, stl);
        }


        // Qualified name.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::QName& t)
        {
          set (t, stl);
        }


        // ID/IDREF.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Id& t)
        {
          set (t, stl);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::IdRef& t)
        {
          set (t, stl);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::IdRefs& t)
        {
          set (t, false);
        }

        // URI.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::AnyURI& t)
        {
          set (t, stl);
        }

        // Binary.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Base64Binary& t)
        {
          set (t, false);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::HexBinary& t)
        {
          set (t, false);
        }


        // Date/time.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Date& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::DateTime& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Duration& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Day& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Month& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::MonthDay& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Year& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::YearMonth& t)
        {
          set (t, true);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Time& t)
        {
          set (t, true);
        }

        // Entity.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Entity& t)
        {
          set (t, stl);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Entities& t)
        {
          set (t, false);
        }

      private:
        bool stl;
        TypeSet& poly_types_;
      };

      struct GlobalElement: Traversal::Element
      {
        GlobalElement (TypeSet& poly_types,
                       bool& valid,
                       const WarningSet& disabled_warnings)
            : poly_types_ (poly_types), valid_ (valid), warning_ (true)
        {
          if (disabled_warnings.find ("all") != disabled_warnings.end () ||
              disabled_warnings.find ("H004") != disabled_warnings.end ())
            warning_ = false;
        }

        virtual void
        traverse (Type& e)
        {
          using SemanticGraph::Schema;

          if (!e.substitutes_p ())
            return;

          // If we are a substitution for some element, then mark
          // that element's type as polymorphic.
          //
          Type& r (e.substitutes ().root ());
          SemanticGraph::Type& rt (r.type ());
          SemanticGraph::Context& ctx (rt.context ());

          if (ctx.count ("polymorphic"))
            return;

          // Only user-defined and anyType can be declared polymorphic.
          //
          if (rt.is_a<SemanticGraph::Fundamental::Type> () ||
              rt.is_a<SemanticGraph::AnySimpleType> ())
          {
            wcerr << r.file () << ":" << r.line () << ":" << r.column ()
                  << ": error: built-in type '" << rt.name () << "' "
                  << "is expected to be polymorphic" << endl;

            wcerr << e.file () << ":" << e.line () << ":" << e.column ()
                  << ": info: because type '" << rt.name () << "' is "
                  << "used in a substitution group declared here" << endl;

            wcerr << r.file () << ":" << r.line () << ":" << r.column ()
                  << ": info: only user-defined types and anyType can "
                  << "be polymorphic in this mapping" << endl;

            valid_ = false;
            return;
          }

          ctx.set ("polymorphic", true);

          if (!warning_)
            return;

          Schema& es (dynamic_cast<Schema&> (e.scope ().scope ()));
          Schema& rts (dynamic_cast<Schema&> (rt.scope ().scope ()));

          // If the root type and this element are in different schemas
          // and the root type is not explicitly marked as polymorphic,
          // then issue a warning.
          //
          if (&es != &rts &&
              !sources_p (es, rts) &&
              poly_types_.find (rt.name ()) == poly_types_.end ())
          {
            wcerr << rt.file () << ":" << rt.line () << ":" << rt.column ()
                  << ": warning H004: assuming type '" << rt.name () << "' "
                  << "is polymorphic" << endl;

            wcerr << e.file () << ":" << e.line () << ":" << e.column ()
                  << ": info: because type '" << rt.name () << "' is "
                  << "used in a substitution group declared here" << endl;

            wcerr << rt.file () << ":" << rt.line () << ":" << rt.column ()
                  << ": info: use --polymorphic-type to indicate this type "
                  << "is polymorphic when compiling schemas that "
                  << "reference it" << endl;
          }
        }

      private:
        // Return true if root sources s.
        //
        bool
        sources_p (SemanticGraph::Schema& root, SemanticGraph::Schema& s)
        {
          using SemanticGraph::Schema;
          using SemanticGraph::Sources;

          for (Schema::UsesIterator i (root.uses_begin ());
               i != root.uses_end (); ++i)
          {
            if (i->is_a<Sources> ())
            {
              if (&i->schema () == &s || sources_p (i->schema (), s))
                return true;
            }
          }

          return false;
        }

      private:
        TypeSet& poly_types_;
        bool& valid_;
        bool warning_;
      };

      // Go into sourced/included/imported schemas while making sure
      // we don't process the same stuff more than once.
      //
      struct Uses: Traversal::Sources,
                   Traversal::Includes,
                   Traversal::Imports
      {
        Uses (char const* seen_key)
            : seen_key_ (seen_key)
        {
        }

        virtual void
        traverse (SemanticGraph::Sources& sr)
        {
          SemanticGraph::Schema& s (sr.schema ());

          if (!s.context ().count (seen_key_))
          {
            s.context ().set (seen_key_, true);
            Traversal::Sources::traverse (sr);
          }
        }

        virtual void
        traverse (SemanticGraph::Includes& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count (seen_key_))
          {
            s.context ().set (seen_key_, true);
            Traversal::Includes::traverse (i);
          }
        }

        virtual void
        traverse (SemanticGraph::Imports& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count (seen_key_))
          {
            s.context ().set (seen_key_, true);
            Traversal::Imports::traverse (i);
          }
        }

      private:
        char const* seen_key_;
      };

      char const* pass_one_key = "cxx-hybrid-size-processor-seen-one";
      char const* pass_two_key = "cxx-hybrid-size-processor-seen-two";

      bool
      process_impl (options const& ops,
                    SemanticGraph::Schema& tu,
                    SemanticGraph::Path const&,
                    const WarningSet& disabled_warnings)
      {
        bool valid (true);
        bool stl (!ops.no_stl ());
        bool poly (ops.generate_polymorphic ());

        // Prepare a set of polymorphic types.
        //

        TypeSet poly_types;
        if (poly)
          poly_types.insert (ops.polymorphic_type ().begin (),
                             ops.polymorphic_type ().end ());

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
          FundType fund_type (stl, poly_types);

          schema >> schema_names >> ns >> ns_names >> fund_type;

          schema.dispatch (tu);
        }
        else
        {
          // First assign sizes to fundamental types.
          //
          {
            Traversal::Schema schema;
            Traversal::Implies implies;
            Traversal::Schema xs_schema;

            schema >> implies >> xs_schema;

            Traversal::Names xs_schema_names;
            Traversal::Namespace ns;
            Traversal::Names ns_names;
            FundType fund_type (stl, poly_types);

            xs_schema >> xs_schema_names >> ns >> ns_names >> fund_type;

            schema.dispatch (tu);
          }

          // Note that we check first if this schema has already been
          // processed which may happen in the file-per-type compilation
          // mode.
          //
          if (!tu.context ().count (pass_two_key))
          {
            // Prepare a set of types with custom data. Here we are
            // only interested in detecting global types. If a type
            // has a nested compositor type which has custom data
            // then the type itself will automatically be variable
            // size.
            //
            TypeSet custom_data_types;

            {
              NarrowStrings const& cd (ops.custom_data ());

              for (NarrowStrings::const_iterator i (cd.begin ());
                   i != cd.end (); ++i)
              {
                String n (*i);
                custom_data_types.insert (String (n, 0, n.find (L"::")));
              }
            }

            // Prepare a map of custom types that specify type length.
            //
            CustomTypeMap custom_type_map;

            {
              NarrowStrings const& v (ops.custom_type ());

              for (NarrowStrings::const_iterator i (v.begin ()), e (v.end ());
                   i != e; ++i)
              {
                String s (*i);

                if (s.empty ())
                  continue;

                // Split the string in two parts at the last '='.
                //
                size_t pos (s.rfind ('='));

                if (pos == String::npos)
                  continue;

                String name (s, 0, pos);
                String fb (s, pos + 1);

                pos = fb.find ('/');

                String flags, base;

                if (pos != String::npos)
                {
                  flags.assign (fb, 0, pos);

                  // Skip the type component.
                  //
                  pos = fb.find ('/', pos + 1);

                  if (pos != String::npos)
                  {
                    String b (fb, pos + 1);

                    // See if we've got the include component.
                    //
                    pos = b.find ('/');

                    if (pos != String::npos)
                      base.assign (b, 0, pos);
                    else
                      base = b;
                  }
                }
                else
                  flags = fb;

                if (!flags)
                  continue;

                if (flags != L"f" && flags != L"v")
                {
                  wcerr << "error: invalid custom type flag: '" <<
                    flags << "'" << endl;

                  valid = false;
                }

                custom_type_map[name].base = base;
                custom_type_map[name].fixed = (flags == L"f");
              }
            }

            // Pass one - check substitution groups.
            //
            if (valid && poly)
            {
              Traversal::Schema schema;
              Uses uses (pass_one_key);

              schema >> uses >> schema;

              Traversal::Names schema_names;
              Traversal::Namespace ns;
              Traversal::Names ns_names;
              GlobalElement element (poly_types, valid, disabled_warnings);

              schema >> schema_names >> ns >> ns_names >> element;

              // Some twisted schemas do recusive self-inclusion.
              //
              tu.context ().set (pass_one_key, true);

              schema.dispatch (tu);
            }

            // Pass two - process types.
            //
            if (valid)
            {
              Traversal::Schema schema;
              Uses uses (pass_two_key);

              schema >> uses >> schema;

              Traversal::Names schema_names;
              Traversal::Namespace ns;
              Traversal::Names ns_names;
              Type type (valid,
                         custom_data_types,
                         custom_type_map,
                         poly_types,
                         stl,
                         poly,
                         !ops.suppress_enum ());

              schema >> schema_names >> ns >> ns_names >> type;

              // Some twisted schemas do recusive self-inclusion.
              //
              tu.context ().set (pass_two_key, true);

              schema.dispatch (tu);
            }
          }
        }

        return valid;
      }
    }

    bool TreeSizeProcessor::
    process (options const& ops,
             SemanticGraph::Schema& tu,
             SemanticGraph::Path const& file,
             const WarningSet& disabled_warnings)
    {
      return process_impl (ops, tu, file, disabled_warnings);
    }
  }
}
