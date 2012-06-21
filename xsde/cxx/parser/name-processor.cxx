// file      : xsde/cxx/parser/name-processor.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2006-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/elements.hxx>
#include <cxx/parser/name-processor.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

#include <cult/containers/set.hxx>

#include <sstream>
#include <iostream>

namespace CXX
{
  namespace Parser
  {
    namespace
    {
      //
      //
      typedef Cult::Containers::Set<String> NameSet;

      class Context: public CXX::Context
      {
      public:
        Context (Parser::options const& ops,
                 SemanticGraph::Schema& root,
                 SemanticGraph::Path const& path)
            : CXX::Context (std::wcerr, root, path, ops, "p:name", "char"),
              skel_suffix_ (ops.skel_type_suffix ()),
              impl_suffix_ (ops.impl_type_suffix ()),
              impl (ops.generate_noop_impl () ||
                    ops.generate_print_impl () ||
                    ops.generate_test_driver ()),
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

          for (UnsignedLong i (1); set.find (name) != set.end (); ++i)
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

        Cult::Containers::Map<String, NameSet*> global_type_names_;

      public:
        Boolean const impl;
        Boolean const tiein;
        String const& skel_suffix;
        String const& impl_suffix;

        Cult::Containers::Map<String, NameSet*>& global_type_names;

        Boolean validation;
        Boolean polymorphic;
      };

      // Primary names.
      //
      struct PrimaryParticle: Traversal::Element, Context
      {
        PrimaryParticle (Context& c, NameSet& set, Boolean restriction)
            : Context (c), set_ (set), restriction_ (restriction)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          using SemanticGraph::Element;

          SemanticGraph::Context& ec (e.context ());

          if (!restriction_)
          {
            ec.set ("p:name", find_name (e.name (), set_));
          }
          else
          {
            Element* prot (
              ec.get<Element*> ("xsd-frontend-restriction-correspondence"));

            ec.set ("p:name", prot->context ().get<String> ("p:name"));
          }
        }

      private:
        NameSet& set_;
        Boolean restriction_;
      };

      struct PrimaryAttribute: Traversal::Attribute, Context
      {
        PrimaryAttribute (Context& c, NameSet& set, Boolean restriction)
            : Context (c), set_ (set), restriction_ (restriction)
        {
        }

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          using SemanticGraph::Attribute;

          SemanticGraph::Context& ac (a.context ());

          if (!restriction_)
          {
            ac.set ("p:name", find_name (a.name (), set_));
          }
          else
          {
            Attribute* prot (
              ac.get<Attribute*> ("xsd-frontend-restriction-correspondence"));

            ac.set ("p:name", prot->context ().get<String> ("p:name"));
          }
        }

      private:
        NameSet& set_;
        Boolean restriction_;
      };


      //
      // Secondary names.
      //

      struct ParticleTag: Traversal::Element,
                          Traversal::Any,
                          Traversal::Choice,
                          Traversal::Sequence,
                          Context
      {
        ParticleTag (Context& c, NameSet& set, Boolean restriction)
            : Context (c), set_ (set), restriction_ (restriction)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          using SemanticGraph::Element;

          SemanticGraph::Context& ec (e.context ());

          if (!restriction_)
          {
            String const base (ec.get<String> ("p:name"));
            ec.set ("p:tag", find_name (base, L"_tag", set_));
          }
          else
          {
            Element* prot = ec.get<Element*> (
              "xsd-frontend-restriction-correspondence");

            ec.set ("p:tag", prot->context ().get<String> ("p:tag"));
          }
        }

        virtual Void
        traverse (SemanticGraph::Any& a)
        {
          using SemanticGraph::Any;

          SemanticGraph::Context& ac (a.context ());

          if (!restriction_)
            ac.set ("p:tag", find_name (L"any", L"_tag", set_));
          else
          {
            Any* prot = ac.get<Any*> (
              "xsd-frontend-restriction-correspondence");

            ac.set ("p:tag", prot->context ().get<String> ("p:tag"));
          }
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          using SemanticGraph::Compositor;

          SemanticGraph::Context& cc (c.context ());

          if (!restriction_)
          {
            cc.set ("p:tag", find_name (L"choice", L"_tag", set_));
          }
          else
          {
            Compositor* prot = cc.get<Compositor*> (
              "xsd-frontend-restriction-correspondence");

            cc.set ("p:tag", prot->context ().get<String> ("p:tag"));
          }
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          using SemanticGraph::Compositor;

          SemanticGraph::Context& sc (s.context ());

          if (!restriction_)
          {
            sc.set ("p:tag", find_name (L"sequence", L"_tag", set_));
          }
          else
          {
            Compositor* prot = sc.get<Compositor*> (
              "xsd-frontend-restriction-correspondence");

            sc.set ("p:tag", prot->context ().get<String> ("p:tag"));
          }
        }

      private:
        NameSet& set_;
        Boolean restriction_;
      };

      struct SecondaryCompositor: Traversal::All,
                                  Traversal::Choice,
                                  Traversal::Sequence,
                                  Context
      {
        SecondaryCompositor (Context& c, NameSet& set, Boolean restriction)
            : Context (c),
              set_ (set),
              restriction_ (restriction),
              particle_tag_ (c, set, restriction)
        {
          contain_particle_tag_ >> particle_tag_;
        }

        virtual Void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}
          // and it cannot be used in restriction.
          //
          if (a.min () == 0)
          {
            a.context ().set (
              "p:present", find_name (L"all", L"_present", set_));
          }

          Traversal::All::traverse (a);
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.contains_begin () == c.contains_end ())
            return;

          SemanticGraph::Context& cc (c.context ());

          if (!restriction_)
          {
            cc.set ("p:arm-tag", find_name (L"choice", L"_arm_tag", set_));
            Traversal::Choice::contains (c, contain_particle_tag_);
            cc.set ("p:arm", find_name (L"choice", L"_arm", set_));
          }
          else
          {
            SemanticGraph::Compositor& b (
              *cc.get<SemanticGraph::Compositor*> (
                "xsd-frontend-restriction-correspondence"));

            SemanticGraph::Context& bc (b.context ());

            cc.set ("p:arm-tag", bc.get<String> ("p:arm-tag"));
            Traversal::Choice::contains (c, contain_particle_tag_);
            cc.set ("p:arm", bc.get<String> ("p:arm"));
          }

          Traversal::Choice::traverse (c);
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          SemanticGraph::Context& sc (s.context ());

          if (!restriction_)
          {
            if (s.max () != 1)
              sc.set ("p:next", find_name (L"sequence", L"_next", set_));
            else if (s.min () == 0)
              sc.set ("p:present", find_name (L"sequence", L"_present", set_));
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
                sc.set ("p:next", bc.get<String> ("p:next"));

                // Add the *_present callback if this is a restriction
                // of sequence to optional.
                //
                if (s.min () == 0)
                  sc.set ("p:present",
                          find_name (L"sequence", L"_present", set_));
              }
              else if (b.min () == 0)
                sc.set ("p:present", bc.get<String> ("p:present"));
            }
          }

          Traversal::Sequence::traverse (s);
        }

      private:
        NameSet& set_;
        Boolean restriction_;

        ParticleTag particle_tag_;
        Traversal::ContainsParticle contain_particle_tag_;
      };

      struct SecondaryParticle: Traversal::Element, Context
      {
        SecondaryParticle (Context& c, NameSet& set, Boolean restriction)
            : Context (c), set_ (set), restriction_ (restriction)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          Boolean poly (
            polymorphic && !e.type ().context ().count ("anonymous"));

          SemanticGraph::Context& ec (e.context ());

          if (!restriction_)
          {
            String const& base (ec.get<String> ("p:name"));

            ec.set ("p:parser", find_name (base + L"_parser", set_));
            ec.set ("p:member", find_name (base + L"_parser_", set_));

            if (poly)
              ec.set (
                "p:member-map", find_name (base + L"_parser_map_", set_));
          }
          else
          {
            SemanticGraph::Context& bc (
              ec.get<SemanticGraph::Element*> (
                "xsd-frontend-restriction-correspondence")->context ());

            ec.set ("p:parser", bc.get<String> ("p:parser"));
            ec.set ("p:member", bc.get<String> ("p:member"));

            if (poly)
              ec.set ("p:member-map", bc.get<String> ("p:member-map"));
          }
        }

      private:
        NameSet& set_;
        Boolean restriction_;
      };

      struct SecondaryAttribute: Traversal::Attribute, Context
      {
        SecondaryAttribute (Context& c, NameSet& set, Boolean restriction)
            : Context (c), set_ (set), restriction_ (restriction)
        {
        }

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          using SemanticGraph::Attribute;

          SemanticGraph::Context& ac (a.context ());

          if (!restriction_)
          {
            String const& base (ac.get<String> ("p:name"));

            a.context ().set (
              "p:parser", find_name (base + L"_parser", set_));

            a.context ().set (
              "p:member", find_name (base + L"_parser_", set_));
          }
          else
          {
            SemanticGraph::Context& bc (
              ac.get<SemanticGraph::Attribute*> (
                "xsd-frontend-restriction-correspondence")->context ());

            ac.set ("p:parser", bc.get<String> ("p:parser"));
            ac.set ("p:member", bc.get<String> ("p:member"));
          }
        }

      private:
        NameSet& set_;
        Boolean restriction_;
      };


      //
      //
      struct Complex: Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& c)
        {
          SemanticGraph::Context& cc (c.context ());

          // Use processed name.
          //
          String const& name (cc.get<String> ("p:name"));

          // We leave this set around to allow other mappings to use
          // this information.
          //
          cc.set ("cxx-parser-name-processor-member-set", NameSet ());
          NameSet& member_set (
            cc.get<NameSet> ("cxx-parser-name-processor-member-set"));

          member_set.insert (name);

          // Add our base's members to the initial list unless we are
          // inheriting by restriction in which case we need to have
          // the same names as our base.
          //
          Boolean restriction (false);

          if (c.inherits_p ())
          {
            // @@ What if this types name is the same as one of base's
            //    members?
            //
            SemanticGraph::Type& base (c.inherits ().base ());

            if (base.is_a<SemanticGraph::Complex> ())
            {
              if (!base.context ().count (
                    "cxx-parser-name-processor-member-set"))
              {
                dispatch (base);
              }

              NameSet const& base_set (
                base.context ().get<NameSet> (
                  "cxx-parser-name-processor-member-set"));

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
              // In case of validation we generate some extra callbacks
              // for compositors.
              //
              if (validation)
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
              else
              {
                SecondaryParticle particle (*this, member_set, restriction);
                Traversal::Compositor compositor;
                Traversal::ContainsCompositor contains_compositor;
                Traversal::ContainsParticle contains_particle;

                contains_compositor >> compositor >> contains_particle;

                contains_particle >> compositor;
                contains_particle >> particle;

                Complex::contains_compositor (c, contains_compositor);
              }
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

        virtual Void
        traverse (SemanticGraph::Type& t)
        {
          SemanticGraph::Context& c (t.context ());
          String const& n (t.name ());

          String name (find_name (n + skel_suffix, set_));
          c.set ("p:name", name);

          // Assign the post_* name.
          //
          c.set ("p:post", find_post_name (t));

          if (tiein)
            c.set ("p:tiein", escape (n + L"_impl_"));

          // Note that we do not add this name to the set so that it
          // does not influence other names.
          //
          if (impl)
            c.set ("p:impl", escape (n + impl_suffix));
        }

      private:
        String
        find_post_name (SemanticGraph::Type& t)
        {
          String const& n (t.name ());

          // It is possible that our base has the same type name (just
          // in a different namespaces). Avoid name clash in this case.
          //
          using SemanticGraph::Complex;

          Complex* c = dynamic_cast<Complex*> (&t);

          if (c == 0 || !c->inherits_p ())
          {
            return escape (L"post_" + n);
          }
          else
          {
            NameSet set;

            // Collect all base's post_*. In some mutual inclusion cases it
            // is possible that our base won't have the post name assigned
            // yet. In this situation will will have to figure it out
            // ourselves (we can do it since we use the "raw" type name).
            //
            SemanticGraph::Type* b (&c->inherits ().base ());

            while (true)
            {
              if (b->context ().count ("p:post"))
                set.insert (b->context ().get<String> ("p:post"));
              else
                set.insert (find_post_name (*b));

              Complex* cb (dynamic_cast<Complex*> (b));

              if (cb != 0 && cb->inherits_p ())
              {
                b = &cb->inherits ().base ();
                continue;
              }

              break;
            }

            String base_name (escape (L"post_" + n));
            String post (base_name);

            for (UnsignedLong i (1); set.find (post) != set.end (); ++i)
            {
              std::wostringstream os;
              os << i;
              post = base_name + os.str ();
            }

            return post;
          }
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

        virtual Void
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
        virtual Void
        traverse (SemanticGraph::AnyType& t)
        {
          set_names (t, "any_type");
        }

        virtual Void
        traverse (SemanticGraph::AnySimpleType& t)
        {
          set_names (t, "any_simple_type");
        }

        // Boolean.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Boolean& t)
        {
          set_names (t, "boolean");
        }

        // Integral types.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Byte& t)
        {
          set_names (t, "byte");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedByte& t)
        {
          set_names (t, "unsigned_byte");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Short& t)
        {
          set_names (t, "short");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedShort& t)
        {
          set_names (t, "unsigned_short");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Int& t)
        {
          set_names (t, "int");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedInt& t)
        {
          set_names (t, "unsigned_int");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Long& t)
        {
          set_names (t, "long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedLong& t)
        {
          set_names (t, "unsigned_long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Integer& t)
        {
          set_names (t, "integer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NonPositiveInteger& t)
        {
          set_names (t, "non_positive_integer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NonNegativeInteger& t)
        {
          set_names (t, "non_negative_integer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::PositiveInteger& t)
        {
          set_names (t, "positive_integer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NegativeInteger& t)
        {
          set_names (t, "negative_integer");
        }

        // Floats.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Float& t)
        {
          set_names (t, "float");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Double& t)
        {
          set_names (t, "double");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Decimal& t)
        {
          set_names (t, "decimal");
        }

        // Strings.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::String& t)
        {
          set_names (t, "string");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NormalizedString& t)
        {
          set_names (t, "normalized_string");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Token& t)
        {
          set_names (t, "token");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NameToken& t)
        {
          set_names (t, "nmtoken");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NameTokens& t)
        {
          set_names (t, "nmtokens");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Name& t)
        {
          set_names (t, "name");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NCName& t)
        {
          set_names (t, "ncname");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Language& t)
        {
          set_names (t, "language");
        }


        // Qualified name.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::QName& t)
        {
          set_names (t, "qname");
        }


        // ID/IDREF.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Id& t)
        {
          set_names (t, "id");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::IdRef& t)
        {
          set_names (t, "idref");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::IdRefs& t)
        {
          set_names (t, "idrefs");
        }

        // URI.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::AnyURI& t)
        {
          set_names (t, "uri");
        }

        // Binary.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Base64Binary& t)
        {
          set_names (t, "base64_binary");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::HexBinary& t)
        {
          set_names (t, "hex_binary");
        }


        // Date/time.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Date& t)
        {
          set_names (t, "date");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::DateTime& t)
        {
          set_names (t, "date_time");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Duration& t)
        {
          set_names (t, "duration");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Day& t)
        {
          set_names (t, "gday");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Month& t)
        {
          set_names (t, "gmonth");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::MonthDay& t)
        {
          set_names (t, "gmonth_day");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Year& t)
        {
          set_names (t, "gyear");;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::YearMonth& t)
        {
          set_names (t, "gyear_month");

        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Time& t)
        {
          set_names (t, "time");
        }

        // Entity.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Entity& t)
        {
          set_names (t, "entity");
        }

        virtual Void
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

        Void
        set_names (SemanticGraph::Type& t, String const& name)
        {
          SemanticGraph::Context& c (t.context ());

          WideChar const* ns = validation
            ? L"::xsde::cxx::parser::validating::"
            : L"::xsde::cxx::parser::non_validating::";

          String skel (make_skel_name (name));
          c.set ("p:name", skel);
          c.set ("p:real-name", ns + skel);

          String impl (make_impl_name (name));
          c.set ("p:impl", impl);
          c.set ("p:real-impl", ns + impl);

          c.set ("p:post", L"post_" + name);

          if (tiein)
            c.set ("p:tiein", name + L"_impl_");
        }
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

          if (!s.context ().count ("cxx-parser-name-processor-seen"))
          {
            s.context ().set ("cxx-parser-name-processor-seen", true);
            Traversal::Sources::traverse (sr);
          }
        }

        virtual Void
        traverse (SemanticGraph::Includes& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count ("cxx-parser-name-processor-seen"))
          {
            s.context ().set ("cxx-parser-name-processor-seen", true);
            Traversal::Includes::traverse (i);
          }
        }

        virtual Void
        traverse (SemanticGraph::Imports& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count ("cxx-parser-name-processor-seen"))
          {
            s.context ().set ("cxx-parser-name-processor-seen", true);
            Traversal::Imports::traverse (i);
          }
        }
      };

      // Go into implied schemas while making sure we don't process
      // the same stuff more than once.
      //
      struct Implies: Traversal::Implies
      {
        virtual Void
        traverse (SemanticGraph::Implies& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count ("cxx-parser-name-processor-seen"))
          {
            s.context ().set ("cxx-parser-name-processor-seen", true);
            Traversal::Implies::traverse (i);
          }
        }
      };

      Void
      process_impl (options const& ops,
                    SemanticGraph::Schema& tu,
                    SemanticGraph::Path const& file,
                    Boolean deep)
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
          if (!tu.context ().count ("cxx-parser-name-processor-seen"))
          {
            Traversal::Schema schema;
            Uses uses;

            schema >> uses >> schema;

            Traversal::Names schema_names;
            Namespace ns (ctx);

            schema >> schema_names >> ns;

            // Some twisted schemas do recusive self-inclusion.
            //
            tu.context ().set ("cxx-parser-name-processor-seen", true);

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

    Void NameProcessor::
    process (options const& ops,
             SemanticGraph::Schema& tu,
             SemanticGraph::Path const& file,
             Boolean deep)
    {
      process_impl (ops, tu, file, deep);
    }
  }
}
