// file      : xsde/cxx/serializer/name-processor.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <set>
#include <map>
#include <sstream>
#include <iostream>

#include <xsde/cxx/elements.hxx>
#include <xsde/cxx/serializer/name-processor.hxx>

#include <libxsd-frontend/semantic-graph.hxx>
#include <libxsd-frontend/traversal.hxx>

using namespace std;

namespace CXX
{
  namespace Serializer
  {
    namespace
    {
      //
      //
      typedef set<String> NameSet;

      class Context: public CXX::Context
      {
      public:
        Context (Serializer::options const& ops,
                 SemanticGraph::Schema& root,
                 SemanticGraph::Path const& path)
            : CXX::Context (std::wcerr, root, path, ops, "s:name", "char"),
              skel_suffix_ (ops.skel_type_suffix ()),
              impl_suffix_ (ops.impl_type_suffix ()),
              impl (ops.generate_empty_impl () || ops.generate_test_driver ()),
              tiein (!ops.reuse_style_mixin () && !ops.reuse_style_none ()),
              skel_suffix (skel_suffix_),
              impl_suffix (impl_suffix_),
              global_type_names (global_type_names_),
              validation (!ops.suppress_validation ()),
              polymorphic (ops.generate_polymorphic ())
        {
        }

      protected:
        Context (Context& c)
            : CXX::Context (c),
              impl (c.impl),
              tiein (c.tiein),
              skel_suffix (c.skel_suffix),
              impl_suffix (c.impl_suffix),
              global_type_names (c.global_type_names),
              validation (c.validation),
              polymorphic (c.polymorphic)
        {
        }

      public:
        String
        find_name (String const& n, String const& suffix, NameSet& set)
        {
          String name (escape (n + suffix));

          for (size_t i (1); set.find (name) != set.end (); ++i)
          {
            std::wostringstream os;
            os << i;
            name = escape (n + os.str () + suffix);
          }

          set.insert (name);
          return name;
        }

        String
        find_name (String const& n, NameSet& set)
        {
          return find_name (n, L"", set);
        }

      private:
        String const skel_suffix_;
        String const impl_suffix_;

        map<String, NameSet*> global_type_names_;

      public:
        bool const impl;
        bool const tiein;
        String const& skel_suffix;
        String const& impl_suffix;

        map<String, NameSet*>& global_type_names;

        bool validation;
        bool polymorphic;
      };

      // Primary names.
      //
      struct PrimaryParticle: Traversal::Element,
                              Traversal::Any,
                              Context
      {
        PrimaryParticle (Context& c, NameSet& set, bool restriction)
            : Context (c), set_ (set), restriction_ (restriction)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          using SemanticGraph::Element;

          SemanticGraph::Context& ec (e.context ());

          if (!restriction_)
            ec.set ("s:name", find_name (e.name (), set_));
          else
          {
            Element* prot = ec.get<Element*> (
              "xsd-frontend-restriction-correspondence");

            ec.set ("s:name", prot->context ().get<String> ("s:name"));
          }
        }

        virtual void
        traverse (SemanticGraph::Any& a)
        {
          using SemanticGraph::Any;

          SemanticGraph::Context& ac (a.context ());

          if (!restriction_)
          {
            ac.set ("s:name", find_name (L"any", set_));
          }
          else
          {
            Any* prot = ac.get<Any*> (
              "xsd-frontend-restriction-correspondence");

            ac.set ("s:name", prot->context ().get<String> ("s:name"));
          }
        }

      private:
        NameSet& set_;
        bool restriction_;
      };

      struct PrimaryAttribute: Traversal::Attribute,
                               Traversal::AnyAttribute,
                               Context
      {
        PrimaryAttribute (Context& c, NameSet& set, bool restriction)
            : Context (c), set_ (set), restriction_ (restriction)
        {
        }

        virtual void
        traverse (SemanticGraph::Attribute& a)
        {
          using SemanticGraph::Attribute;

          SemanticGraph::Context& ac (a.context ());

          if (!restriction_)
          {
            ac.set ("s:name", find_name (a.name (), set_));
          }
          else
          {
            Attribute* prot = ac.get<Attribute*> (
              "xsd-frontend-restriction-correspondence");

            ac.set ("s:name", prot->context ().get<String> ("s:name"));
          }
        }

        virtual void
        traverse (SemanticGraph::AnyAttribute& a)
        {
          using SemanticGraph::AnyAttribute;

          SemanticGraph::Context& ac (a.context ());

          if (!restriction_)
          {
            ac.set ("s:name", find_name (L"any_attribute", set_));
          }
          else
          {
            AnyAttribute* prot = ac.get<AnyAttribute*> (
              "xsd-frontend-restriction-correspondence");

            ac.set ("s:name", prot->context ().get<String> ("s:name"));
          }
        }

      private:
        NameSet& set_;
        bool restriction_;
      };

      // Secondary names.
      //

      struct ParticleTag: Traversal::Element,
                          Traversal::Any,
                          Traversal::Choice,
                          Traversal::Sequence,
                          Context
      {
        ParticleTag (Context& c, NameSet& set, bool restriction)
            : Context (c), set_ (set), restriction_ (restriction)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          using SemanticGraph::Element;

          SemanticGraph::Context& ec (e.context ());

          if (!restriction_)
          {
            String const base (ec.get<String> ("s:name"));
            ec.set ("s:tag", find_name (base, L"_tag", set_));
          }
          else
          {
            Element* prot = ec.get<Element*> (
              "xsd-frontend-restriction-correspondence");

            ec.set ("s:tag", prot->context ().get<String> ("s:tag"));
          }
        }

        virtual void
        traverse (SemanticGraph::Any& a)
        {
          using SemanticGraph::Any;

          SemanticGraph::Context& ac (a.context ());

          if (!restriction_)
          {
            String const& base (ac.get<String> ("s:name"));
            ac.set ("s:tag", find_name (base, L"_tag", set_));
          }
          else
          {
            Any* prot = ac.get<Any*> (
              "xsd-frontend-restriction-correspondence");

            ac.set ("s:tag", prot->context ().get<String> ("s:tag"));
          }
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          using SemanticGraph::Compositor;

          SemanticGraph::Context& cc (c.context ());

          if (!restriction_)
          {
            cc.set ("s:tag", find_name (L"choice", L"_tag", set_));
          }
          else
          {
            Compositor* prot = cc.get<Compositor*> (
              "xsd-frontend-restriction-correspondence");

            cc.set ("s:tag", prot->context ().get<String> ("s:tag"));
          }
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          using SemanticGraph::Compositor;

          SemanticGraph::Context& sc (s.context ());

          if (!restriction_)
          {
            sc.set ("s:tag", find_name (L"sequence", L"_tag", set_));
          }
          else
          {
            Compositor* prot = sc.get<Compositor*> (
              "xsd-frontend-restriction-correspondence");

            sc.set ("s:tag", prot->context ().get<String> ("s:tag"));
          }
        }

      private:
        NameSet& set_;
        bool restriction_;
      };

      struct SecondaryCompositor: Traversal::All,
                                  Traversal::Choice,
                                  Traversal::Sequence,
                                  Context
      {
        SecondaryCompositor (Context& c, NameSet& set, bool restriction)
            : Context (c),
              set_ (set),
              restriction_ (restriction),
              particle_tag_ (c, set, restriction)
        {
          contain_particle_tag_ >> particle_tag_;
        }

        virtual void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}
          // and it cannot be used in restriction.
          //
          if (a.contained_compositor ().min () == 0)
          {
            a.context ().set (
              "s:present", find_name (L"all", L"_present", set_));
          }

          Traversal::All::traverse (a);
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.contains_begin () == c.contains_end ())
            return;

          SemanticGraph::Context& cc (c.context ());

          if (!restriction_)
          {
            if (c.max () != 1)
              cc.set ("s:next", find_name (L"choice", L"_next", set_));
            else if (c.min () == 0)
              cc.set ("s:present", find_name (L"choice", L"_present", set_));

            // Tags.
            //
            cc.set ("s:arm-tag", find_name (L"choice", L"_arm_tag", set_));
            Traversal::Choice::contains (c, contain_particle_tag_);
            cc.set ("s:arm", find_name (L"choice", L"_arm", set_));
          }
          else
          {
            SemanticGraph::Compositor& b (
              *cc.get<SemanticGraph::Compositor*> (
                "xsd-frontend-restriction-correspondence"));

            SemanticGraph::Context& bc (b.context ());

            if (b.max () != 1)
            {
              cc.set ("s:next", bc.get<String> ("s:next"));

              // Add the *_present callback if this is a restriction
              // of sequence to optional.
              //
              if (c.min () == 0)
                cc.set ("s:present",
                        find_name (L"choice", L"_present", set_));
            }
            else if (b.min () == 0)
              cc.set ("s:present", bc.get<String> ("s:present"));

            // Tags.
            //
            cc.set ("s:arm-tag", bc.get<String> ("s:arm-tag"));
            Traversal::Choice::contains (c, contain_particle_tag_);
            cc.set ("s:arm", bc.get<String> ("s:arm"));
          }

          Traversal::Choice::traverse (c);
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          SemanticGraph::Context& sc (s.context ());

          if (!restriction_)
          {
            if (s.max () != 1)
              sc.set ("s:next", find_name (L"sequence", L"_next", set_));
            else if (s.min () == 0)
              sc.set ("s:present", find_name (L"sequence", L"_present", set_));
          }
          else
          {
            // Root compositor that models inheritance by extension
            // may not have an association.
            //
            if (sc.count ("xsd-frontend-restriction-correspondence"))
            {
              SemanticGraph::Compositor& b (
                *sc.get<SemanticGraph::Compositor*> (
                  "xsd-frontend-restriction-correspondence"));

              SemanticGraph::Context& bc (b.context ());

              if (b.max () != 1)
              {
                sc.set ("s:next", bc.get<String> ("s:next"));

                // Add the *_present callback if this is a restriction
                // of sequence to optional.
                //
                if (s.min () == 0)
                  sc.set ("s:present",
                           find_name (L"sequence", L"_present", set_));
              }
              else if (b.min () == 0)
                sc.set ("s:present", bc.get<String> ("s:present"));
            }
          }

          Traversal::Sequence::traverse (s);
        }

      private:
        NameSet& set_;
        bool restriction_;

        ParticleTag particle_tag_;
        Traversal::ContainsParticle contain_particle_tag_;
      };

      struct SecondaryParticle: Traversal::Element,
                                Traversal::Any,
                                Context
      {
        SecondaryParticle (Context& c, NameSet& set, bool restriction)
            : Context (c), set_ (set), restriction_ (restriction)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          bool poly (
            polymorphic && !e.type ().context ().count ("anonymous"));

          SemanticGraph::Context& ec (e.context ());

          if (!restriction_)
          {
            size_t min (e.min ()), max (e.max ());

            String const& base (ec.get<String> ("s:name"));

            if (min == 0 && max == 1)
              ec.set ("s:present", find_name (base + L"_present", set_));
            else if (max != 1)
              ec.set ("s:next", find_name (base + L"_next", set_));

            ec.set ("s:serializer", find_name (base + L"_serializer", set_));
            ec.set ("s:member", find_name (base + L"_serializer_", set_));

            if (poly)
              ec.set ("s:member-map",
                       find_name (base + L"_serializer_map_", set_));
          }
          else
          {
            SemanticGraph::Element& b (
              *ec.get<SemanticGraph::Element*> (
                "xsd-frontend-restriction-correspondence"));

            SemanticGraph::Context& bc (b.context ());
            size_t min (b.min ()), max (b.max ());

            if (min == 0 && max == 1)
              ec.set ("s:present", bc.get<String> ("s:present"));
            else if (max != 1)
            {
              ec.set ("s:next", bc.get<String> ("s:next"));

              // Add the *_present callback if this is a restriction
              // of sequence to optional.
              //
              if (e.min () == 0)
              {
                String const& base (bc.get<String> ("s:name"));
                ec.set ("s:present", find_name (base + L"_present", set_));
              }
            }

            ec.set ("s:serializer", bc.get<String> ("s:serializer"));
            ec.set ("s:member", bc.get<String> ("s:member"));

            if (poly)
              ec.set ("s:member-map", bc.get<String> ("s:member-map"));
          }
        }

        virtual void
        traverse (SemanticGraph::Any& a)
        {
          using SemanticGraph::Any;

          SemanticGraph::Context& ac (a.context ());

          if (!restriction_)
          {
            size_t min (a.min ()), max (a.max ());
            String const& base (ac.get<String> ("s:name"));

            if (min == 0 && max == 1)
            {
              ac.set ("s:present", find_name (base + L"_present", set_));
            }
            else if (max != 1)
            {
              ac.set ("s:next", find_name (base + L"_next", set_));
            }

            ac.set ("s:serialize", find_name (L"serialize_" + base, set_));
          }
          else
          {
            Any& b (
              *ac.get<Any*> ("xsd-frontend-restriction-correspondence"));

            SemanticGraph::Context& bc (b.context ());
            size_t min (b.min ()), max (b.max ());

            if (min == 0 && max == 1)
            {
              ac.set ("s:present", bc.get<String> ("s:present"));
            }
            else if (max != 1)
            {
              ac.set ("s:next", bc.get<String> ("s:next"));

              // Add the *_present callback if this is a restriction
              // of sequence to optional.
              //
              if (a.min () == 0)
              {
                String const& base (bc.get<String> ("s:name"));
                ac.set ("s:present", find_name (base + L"_present", set_));
              }
            }

            ac.set ("s:serialize", bc.get<String> ("s:serialize"));
          }
        }

      private:
        NameSet& set_;
        bool restriction_;
      };

      struct SecondaryAttribute: Traversal::Attribute,
                                 Traversal::AnyAttribute,
                                 Context
      {
        SecondaryAttribute (Context& c, NameSet& set, bool restriction)
            : Context (c), set_ (set), restriction_ (restriction)
        {
        }

        virtual void
        traverse (SemanticGraph::Attribute& a)
        {
          using SemanticGraph::Attribute;

          SemanticGraph::Context& ac (a.context ());

          if (!restriction_)
          {
            String const& base (ac.get<String> ("s:name"));

            if (a.optional_p ())
              ac.set ("s:present", find_name (base + L"_present", set_));

            ac.set ("s:serializer", find_name (base + L"_serializer", set_));
            ac.set ("s:member", find_name (base + L"_serializer_", set_));
          }
          else
          {
            Attribute& b (
              *ac.get<Attribute*> ("xsd-frontend-restriction-correspondence"));
            SemanticGraph::Context& bc (b.context ());

            if (a.optional_p ())
              ac.set ("s:present", bc.get<String> ("s:present"));

            ac.set ("s:serializer", bc.get<String> ("s:serializer"));
            ac.set ("s:member", bc.get<String> ("s:member"));
          }
        }

        virtual void
        traverse (SemanticGraph::AnyAttribute& a)
        {
          using SemanticGraph::AnyAttribute;

          SemanticGraph::Context& ac (a.context ());

          if (!restriction_)
          {
            String const& base (ac.get<String> ("s:name"));

            ac.set ("s:next", find_name (base + L"_next", set_));
            ac.set ("s:serialize", find_name (L"serialize_" + base, set_));
          }
          else
          {
            AnyAttribute& b (
              *ac.get<AnyAttribute*> (
                "xsd-frontend-restriction-correspondence"));
            SemanticGraph::Context& bc (b.context ());

            ac.set ("s:next", bc.get<String> ("s:next"));
            ac.set ("s:serialize", bc.get<String> ("s:serialize"));
          }
        }

      private:
        NameSet& set_;
        bool restriction_;
      };

      //
      //
      struct Complex: Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& c)
        {
          SemanticGraph::Context& cc (c.context ());

          // Use processed name.
          //
          String const& name (cc.get<String> ("s:name"));

          // We leave this set around to allow other mappings to use
          // this information.
          //
          cc.set ("cxx-serializer-name-processor-member-set", NameSet ());
          NameSet& member_set (
            cc.get<NameSet> ("cxx-serializer-name-processor-member-set"));

          member_set.insert (name);

          // Add our base's members to the initial list unless we are
          // inheriting by restriction in which case we need to have
          // the same names as our base.
          //
          bool restriction (false);

          if (c.inherits_p ())
          {
            // @@ What if this types name is the same as one of base's
            //    members?
            //
            SemanticGraph::Type& base (c.inherits ().base ());

            if (base.is_a<SemanticGraph::Complex> ())
            {
              if (!base.context ().count (
                    "cxx-serializer-name-processor-member-set"))
              {
                dispatch (base);
              }

              NameSet const& base_set (
                base.context ().get<NameSet> (
                  "cxx-serializer-name-processor-member-set"));

              member_set.insert (base_set.begin (), base_set.end ());

              // Inheritance by restriction from anyType is a special case.
	      //
              restriction = c.inherits ().is_a<SemanticGraph::Restricts> () &&
	        !c.inherits ().base ().is_a<SemanticGraph::AnyType> ();
            }
          }

          // First assign the "primary" names.
          //
          {
            if (c.contains_compositor_p ())
            {
              PrimaryParticle particle (*this, member_set, restriction);
              Traversal::Compositor compositor;
              Traversal::ContainsCompositor contains_compositor;
              Traversal::ContainsParticle contains_particle;

              contains_compositor >> compositor >> contains_particle;

              contains_particle >> compositor;
              contains_particle >> particle;

              Complex::contains_compositor (c, contains_compositor);
            }

            //
            //
            PrimaryAttribute attribute (*this, member_set, restriction);
            Traversal::Names names (attribute);

            Complex::names (c, names);
          }

          // Assign "secondary" names.
          //
          {
            if (c.contains_compositor_p ())
            {
              SecondaryParticle particle (*this, member_set, restriction);
              SecondaryCompositor compositor (*this, member_set, restriction);
              Traversal::ContainsCompositor contains_compositor;
              Traversal::ContainsParticle contains_particle;

              contains_compositor >> compositor >> contains_particle;

              contains_particle >> compositor;
              contains_particle >> particle;

              Complex::contains_compositor (c, contains_compositor);
            }

            //
            //
            SecondaryAttribute attribute (*this, member_set, restriction);
            Traversal::Names names (attribute);

            Complex::names (c, names);
          }
        }
      };


      //
      //
      struct GlobalType: Traversal::Type, Context
      {
        GlobalType (Context& c, NameSet& set)
            : Context (c), set_ (set)
        {
        }

        virtual void
        traverse (SemanticGraph::Type& t)
        {
          String const& n (t.name ());
          SemanticGraph::Context& tc (t.context ());

          String name (find_name (n + skel_suffix, set_));
          tc.set ("s:name", name);

          if (tiein)
            tc.set ("s:tiein", escape (n + L"_impl_"));

          // Note that we do not add this name to the set so that it
          // does not influence other names.
          //
          if (impl)
            tc.set ("s:impl", escape (n + impl_suffix));
        }

      private:
        NameSet& set_;
      };


      struct Namespace: Traversal::Namespace, Context
      {
        Namespace (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& ns)
        {
          SemanticGraph::Context& nsc (ns.context ());
          String const& name (ns.name ());

          // Use a name set associated with this namespace if present.
          // This will make sure that we don't get any conflicts in the
          // multi-mapping translation case. Note that here we assume
          // that all mappings traverse schemas in the same order which
          // is currently the case.
          //
          if (global_type_names.find (name) == global_type_names.end ())
          {
            if (!nsc.count ("name-set"))
              nsc.set ("name-set", NameSet ());

            NameSet& s (nsc.get<NameSet> ("name-set"));
            global_type_names[name] = &s;
          }

          NameSet& type_set (*global_type_names[name]);

          GlobalType type (*this, type_set);
          Traversal::Names names (type);

          Traversal::Namespace::names (ns, names);
        }
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
                        Traversal::Fundamental::Entities,

                        Context

      {
        FundType (Context& c)
            : Context (c)
        {
        }

        // anyType & anySimpleType.
        //
        virtual void
        traverse (SemanticGraph::AnyType& t)
        {
          set_names (t, "any_type");
        }

        virtual void
        traverse (SemanticGraph::AnySimpleType& t)
        {
          set_names (t, "any_simple_type");
        }

        // Boolean.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Boolean& t)
        {
          set_names (t, "boolean");
        }

        // Integral types.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Byte& t)
        {
          set_names (t, "byte");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedByte& t)
        {
          set_names (t, "unsigned_byte");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Short& t)
        {
          set_names (t, "short");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedShort& t)
        {
          set_names (t, "unsigned_short");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Int& t)
        {
          set_names (t, "int");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedInt& t)
        {
          set_names (t, "unsigned_int");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Long& t)
        {
          set_names (t, "long");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedLong& t)
        {
          set_names (t, "unsigned_long");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Integer& t)
        {
          set_names (t, "integer");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NonPositiveInteger& t)
        {
          set_names (t, "non_positive_integer");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NonNegativeInteger& t)
        {
          set_names (t, "non_negative_integer");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::PositiveInteger& t)
        {
          set_names (t, "positive_integer");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NegativeInteger& t)
        {
          set_names (t, "negative_integer");
        }

        // Floats.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Float& t)
        {
          set_names (t, "float");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Double& t)
        {
          set_names (t, "double");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Decimal& t)
        {
          set_names (t, "decimal");
        }

        // Strings.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::String& t)
        {
          set_names (t, "string");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NormalizedString& t)
        {
          set_names (t, "normalized_string");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Token& t)
        {
          set_names (t, "token");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NameToken& t)
        {
          set_names (t, "nmtoken");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NameTokens& t)
        {
          set_names (t, "nmtokens");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Name& t)
        {
          set_names (t, "name");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NCName& t)
        {
          set_names (t, "ncname");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Language& t)
        {
          set_names (t, "language");
        }


        // Qualified name.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::QName& t)
        {
          set_names (t, "qname");
        }


        // ID/IDREF.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Id& t)
        {
          set_names (t, "id");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::IdRef& t)
        {
          set_names (t, "idref");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::IdRefs& t)
        {
          set_names (t, "idrefs");
        }

        // URI.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::AnyURI& t)
        {
          set_names (t, "uri");
        }

        // Binary.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Base64Binary& t)
        {
          set_names (t, "base64_binary");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::HexBinary& t)
        {
          set_names (t, "hex_binary");
        }


        // Date/time.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Date& t)
        {
          set_names (t, "date");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::DateTime& t)
        {
          set_names (t, "date_time");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Duration& t)
        {
          set_names (t, "duration");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Day& t)
        {
          set_names (t, "gday");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Month& t)
        {
          set_names (t, "gmonth");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::MonthDay& t)
        {
          set_names (t, "gmonth_day");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Year& t)
        {
          set_names (t, "gyear");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::YearMonth& t)
        {
          set_names (t, "gyear_month");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Time& t)
        {
          set_names (t, "time");
        }

        // Entity.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Entity& t)
        {
          set_names (t, "entity");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Entities& t)
        {
          set_names (t, "entities");
        }

      private:
        String
        make_skel_name (String const& base)
        {
          return escape (base + skel_suffix);
        }

        String
        make_impl_name (String const& base)
        {
          return escape (base + impl_suffix);
        }

        void
        set_names (SemanticGraph::Type& t, String const& name)
        {
          SemanticGraph::Context& c (t.context ());

          wchar_t const* ns = validation
            ? L"::xsde::cxx::serializer::validating::"
            : L"::xsde::cxx::serializer::non_validating::";

          String skel (make_skel_name (name));
          c.set ("s:name", skel);
          c.set ("s:real-name", ns + skel);

          String impl (make_impl_name (name));
          c.set ("s:impl", impl);
          c.set ("s:real-impl", ns + impl);

          c.set ("s:post", L"post_" + name);

          if (tiein)
            c.set ("s:tiein", name + L"_impl_");
        }
      };

      // Go into sourced/included/imported schemas while making sure
      // we don't process the same stuff more than once.
      //
      struct Uses: Traversal::Sources,
                   Traversal::Includes,
                   Traversal::Imports
      {
        virtual void
        traverse (SemanticGraph::Sources& sr)
        {
          SemanticGraph::Schema& s (sr.schema ());

          if (!s.context ().count ("cxx-serializer-name-processor-seen"))
          {
            s.context ().set ("cxx-serializer-name-processor-seen", true);
            Traversal::Sources::traverse (sr);
          }
        }

        virtual void
        traverse (SemanticGraph::Includes& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count ("cxx-serializer-name-processor-seen"))
          {
            s.context ().set ("cxx-serializer-name-processor-seen", true);
            Traversal::Includes::traverse (i);
          }
        }

        virtual void
        traverse (SemanticGraph::Imports& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count ("cxx-serializer-name-processor-seen"))
          {
            s.context ().set ("cxx-serializer-name-processor-seen", true);
            Traversal::Imports::traverse (i);
          }
        }
      };

      // Go into implied schemas while making sure we don't process
      // the same stuff more than once.
      //
      struct Implies: Traversal::Implies
      {
        virtual void
        traverse (SemanticGraph::Implies& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count ("cxx-serializer-name-processor-seen"))
          {
            s.context ().set ("cxx-serializer-name-processor-seen", true);
            Traversal::Implies::traverse (i);
          }
        }
      };

      void
      process_impl (options const& ops,
                    SemanticGraph::Schema& tu,
                    SemanticGraph::Path const& file,
                    bool deep)
      {
        Context ctx (ops, tu, file);

        if (tu.names_begin ()->named ().name () ==
            L"http://www.w3.org/2001/XMLSchema")
        {
          // XML Schema namespace.
          //
          Traversal::Schema schema;

          Traversal::Names schema_names;
          Traversal::Namespace ns;
          Traversal::Names ns_names;
          FundType fund_type (ctx);

          schema >> schema_names >> ns >> ns_names >> fund_type;

          schema.dispatch (tu);
        }
        else
        {
          // Pass one - assign names to fundamental types.
          //
          {
            Traversal::Schema schema;
            Implies implies;
            Traversal::Schema xs_schema;

            schema >> implies >> xs_schema;

            Traversal::Names xs_schema_names;
            Traversal::Namespace ns;
            Traversal::Names ns_names;
            FundType fund_type (ctx);

            xs_schema >> xs_schema_names >> ns >> ns_names >> fund_type;

            schema.dispatch (tu);
          }

          // Pass two - assign names to global types. This pass cannot
          // be combined with pass three because of possible recursive
          // schema inclusions. Also note that we check first if this
          // schema has already been processed which may happen in the
          // file-per-type compilation mode.
          //
          if (!tu.context ().count ("cxx-serializer-name-processor-seen"))
          {
            Traversal::Schema schema;
            Uses uses;

            schema >> uses >> schema;

            Traversal::Names schema_names;
            Namespace ns (ctx);

            schema >> schema_names >> ns;

            // Some twisted schemas do recusive self-inclusion.
            //
            tu.context ().set ("cxx-serializer-name-processor-seen", true);

            schema.dispatch (tu);
          }

          if (!deep)
            return;

          // Pass three - assign names inside complex types. Here we don't
          // need to go into included/imported schemas.
          //
          {
            Traversal::Schema schema;
            Sources sources;

            schema >> sources >> schema;

            Traversal::Names schema_names;
            Traversal::Namespace ns;
            Traversal::Names ns_names;

            schema >> schema_names >> ns >> ns_names;

            Complex complex (ctx);

            ns_names >> complex;

            schema.dispatch (tu);
          }
        }
      }
    }

    void NameProcessor::
    process (options const& ops,
             SemanticGraph::Schema& tu,
             SemanticGraph::Path const& file,
             bool deep)
    {
      process_impl (ops, tu, file, deep);
    }
  }
}
