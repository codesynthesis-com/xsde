// file      : xsde/cxx/hybrid/parser-aggregate-header.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/hybrid/parser-aggregate-header.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

#include <cult/containers/map.hxx>
#include <cult/containers/set.hxx>

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      typedef
      Cult::Containers::Map<SemanticGraph::Type*, String>
      TypeInstanceMap;

      typedef Cult::Containers::Set<String> InstanceSet;

      // For base types we only want member's types, but not the
      // base itself.
      //
      struct BaseType: Traversal::Complex, Context
      {
        BaseType (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Complex& c)
        {
          inherits (c);

          if (!restriction_p (c))
          {
            names (c);
            contains_compositor (c);
          }
        }
      };

      struct ParserDef: Traversal::Type,
                        Traversal::List,
                        Traversal::Complex,

                        Traversal::AnyType,
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
        ParserDef (Context& c, TypeInstanceMap& map, InstanceSet& set)
            : Context (c), map_ (map), set_ (set), base_ (c)
        {
          *this >> inherits_ >> base_ >> inherits_;

          *this >> contains_compositor_;
          base_ >> contains_compositor_;

          *this >> names_;
          base_ >> names_;

          contains_compositor_ >> compositor_;
          compositor_ >> contains_particle_;
          contains_particle_ >> compositor_;
          contains_particle_ >> particle_;

          names_ >> attribute_;

          particle_ >> belongs_;
          attribute_ >> belongs_;
          belongs_ >> *this;
        }

        virtual Void
        traverse (SemanticGraph::Type& t)
        {
          if (map_.find (&t) == map_.end ())
          {
            String inst (find_instance_name (t));
            map_[&t] = inst;
          }
        }

        virtual Void
        traverse (SemanticGraph::List& l)
        {
          if (map_.find (&l) == map_.end ())
          {
            String inst (find_instance_name (l));
            map_[&l] = inst;

            dispatch (l.argumented ().type ());
          }
        }

        virtual Void
        traverse (SemanticGraph::Complex& c)
        {
          if (map_.find (&c) == map_.end ())
          {
            String inst (find_instance_name (c));
            map_[&c] = inst;

            // Use base type's parsers in case of a restriction
            // since we are not capable of using a derived type
            // in place of a base (no virtual d-tor, for one).
            //
            inherits (c);

            if (!restriction_p (c))
            {
              names (c);
              contains_compositor (c);
            }
          }
        }

        // anyType & anySimpleType.
        //
        virtual Void
        traverse (SemanticGraph::AnyType& t)
        {
          fund_type (t, "any_type");
        }

        virtual Void
        traverse (SemanticGraph::AnySimpleType& t)
        {
          fund_type (t, "any_simple_type");
        }

        // Boolean.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Boolean& t)
        {
          fund_type (t, "boolean");
        }

        // Integral types.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Byte& t)
        {
          fund_type (t, "byte");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedByte& t)
        {
          fund_type (t, "unsigned_byte");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Short& t)
        {
          fund_type (t, "short");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedShort& t)
        {
          fund_type (t, "unsigned_short");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Int& t)
        {
          fund_type (t, "int");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedInt& t)
        {
          fund_type (t, "unsigned_int");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Long& t)
        {
          fund_type (t, "long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedLong& t)
        {
          fund_type (t, "unsigned_long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Integer& t)
        {
          fund_type (t, "integer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NonPositiveInteger& t)
        {
          fund_type (t, "non_positive_integer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NonNegativeInteger& t)
        {
          fund_type (t, "non_negative_integer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::PositiveInteger& t)
        {
          fund_type (t, "positive_integer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NegativeInteger& t)
        {
          fund_type (t, "negative_integer");
        }

        // Floats.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Float& t)
        {
          fund_type (t, "float");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Double& t)
        {
          fund_type (t, "double");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Decimal& t)
        {
          fund_type (t, "decimal");
        }

        // Strings.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::String& t)
        {
          fund_type (t, "string");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NormalizedString& t)
        {
          fund_type (t, "normalized_string");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Token& t)
        {
          fund_type (t, "token");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NameToken& t)
        {
          fund_type (t, "nmtoken");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NameTokens& t)
        {
          fund_type (t, "nmtokens");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Name& t)
        {
          fund_type (t, "name");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NCName& t)
        {
          fund_type (t, "ncname");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Language& t)
        {
          fund_type (t, "language");
        }


        // Qualified name.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::QName& t)
        {
          fund_type (t, "qname");
        }


        // ID/IDREF.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Id& t)
        {
          fund_type (t, "id");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::IdRef& t)
        {
          fund_type (t, "idref");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::IdRefs& t)
        {
          fund_type (t, "idrefs");
        }

        // URI.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::AnyURI& t)
        {
          fund_type (t, "uri");
        }

        // Binary.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Base64Binary& t)
        {
          fund_type (t, "base64_binary");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::HexBinary& t)
        {
          fund_type (t, "hex_binary");
        }


        // Date/time.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Date& t)
        {
          fund_type (t, "date");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::DateTime& t)
        {
          fund_type (t, "date_time");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Duration& t)
        {
          fund_type (t, "duration");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Day& t)
        {
          fund_type (t, "day");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Month& t)
        {
          fund_type (t, "month");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::MonthDay& t)
        {
          fund_type (t, "month_day");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Year& t)
        {
          fund_type (t, "year");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::YearMonth& t)
        {
          fund_type (t, "year_month");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Time& t)
        {
          fund_type (t, "time");
        }

        // Entity.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Entity& t)
        {
          fund_type (t, "entity");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Entities& t)
        {
          fund_type (t, "entities");
        }

      private:
        virtual Void
        fund_type (SemanticGraph::Type& t, String const& name)
        {
          if (map_.find (&t) == map_.end ())
          {
            String inst (find_instance_name (name));
            map_[&t] = inst;
          }
        }

        String
        find_instance_name (String const& raw_name)
        {
          String name (escape (raw_name + L"_p_"));

          for (UnsignedLong i (1); set_.find (name) != set_.end (); ++i)
          {
            std::wostringstream os;
            os << i;
            name = escape (raw_name + L"_p" + os.str () + L"_");
          }

          set_.insert (name);
          return name;
        }

        String
        find_instance_name (SemanticGraph::Type& t)
        {
          return find_instance_name (t.name ());
        }

        TypeInstanceMap& map_;
        InstanceSet& set_;

        BaseType base_;
        Traversal::Inherits inherits_;

        Traversal::Compositor compositor_;
        Traversal::Element particle_;
        Traversal::ContainsCompositor contains_compositor_;
        Traversal::ContainsParticle contains_particle_;

        Traversal::Names names_;
        Traversal::Attribute attribute_;

        Traversal::Belongs belongs_;
      };

      struct GlobalType: Traversal::Type, Context
      {
        GlobalType (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Type& t)
        {
          SemanticGraph::Context& tc (t.context ());

          if (!tc.count ("paggr"))
            return;

          String const& name (tc.get<String> ("paggr"));

          String pre (unclash (name, "pre"));
          String post (unclash (name, "post"));
          String root_parser (unclash (name, "root_parser"));
          String error, reset;

          InstanceSet set;
          set.insert (pre);
          set.insert (post);
          set.insert (name);
          set.insert (root_parser);

          if (!exceptions)
          {
            error = unclash (name, "_error");
            set.insert (error);
          }

          if (Context::reset)
          {
            reset = unclash (name, "reset");
            set.insert (reset);
          }

          tc.set ("paggr-map", TypeInstanceMap ());
          TypeInstanceMap& map (tc.get<TypeInstanceMap> ("paggr-map"));

          ParserDef def (*this, map, set);
          def.dispatch (t);

          String const& root_member (map.find (&t)->second);

          os << "// Parser aggregate for the " << comment (t.name ()) <<
            " type." << endl
             << "//" << endl;

          os << "class " << name
             << "{"
             << "public:" << endl;

          // c-tor ()
          //
          os << name << " ();"
             << endl;

          // pre ()
          //
          os << "void" << endl
             << pre << " ()"
             << "{"
             << "this->" << root_member << ".pre ();"
             << "}";

          // post ()
          //
          String const& ret (pret_type (t));

          os << ret << endl
             << post << " ()"
             << "{"
             << (ret == L"void" ? "" : "return ") << "this->" <<
            root_member << "." << post_name (t) << " ();"
             << "}";

          // root_parser ()
          //
          os << fq_name (t, "p:impl") << "&" << endl
             << root_parser << " ()"
             << "{"
             << "return this->" << root_member << ";"
             << "}";

          // _error ()
          //
          if (error)
          {
            os << xs_ns_name () << "::parser_error" << endl
               << error << " ()"
               << "{"
               << "return this->" << root_member << "._error ();"
               << "}";
          }

          // reset ()
          //
          if (reset)
          {
            os << "void" << endl
               << reset << " ()"
               << "{"
               << "this->" << root_member << "._reset ();"
               << "}";
          }

          os << "public:" << endl;

          for (TypeInstanceMap::Iterator i (map.begin ()), end (map.end ());
               i != end; ++i)
            os << fq_name (*i->first, "p:impl") << " " << i->second << ";";

          os << "};";
        }
      };

      struct GlobalElement: Traversal::Element, Context
      {
        GlobalElement (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          SemanticGraph::Context& ec (e.context ());

          if (!ec.count ("paggr"))
            return;

          SemanticGraph::Type& t (e.type ());
          String const& name (ec.get<String> ("paggr"));

          String pre (unclash (name, "pre"));
          String post (unclash (name, "post"));
          String root_parser (unclash (name, "root_parser"));
          String root_name (unclash (name, "root_name"));
          String root_namespace (unclash (name, "root_namespace"));
          String error, reset;

          InstanceSet set;
          set.insert (pre);
          set.insert (post);
          set.insert (name);
          set.insert (root_parser);
          set.insert (root_name);
          set.insert (root_namespace);

          if (!exceptions)
          {
            error = unclash (name, "_error");
            set.insert (error);
          }

          if (Context::reset)
          {
            reset = unclash (name, "reset");
            set.insert (reset);
          }

          ec.set ("paggr-map", TypeInstanceMap ());
          TypeInstanceMap& map (ec.get<TypeInstanceMap> ("paggr-map"));

          ParserDef def (*this, map, set);
          def.dispatch (t);

          String const& root_member (map.find (&t)->second);

          os << "// Parser aggregate for the " << comment (e.name ()) <<
            " element." << endl
             << "//" << endl;

          os << "class " << name
             << "{"
             << "public:" << endl;

          // c-tor ()
          //
          os << name << " ();"
             << endl;

          // pre ()
          //
          os << "void" << endl
             << pre << " ()"
             << "{"
             << "this->" << root_member << ".pre ();"
             << "}";

          // post ()
          //
          String const& ret (pret_type (t));

          os << ret << endl
             << post << " ()"
             << "{"
             << (ret == L"void" ? "" : "return ") << "this->" <<
            root_member << "." << post_name (t) << " ();"
             << "}";

          // root_parser ()
          //
          os << fq_name (t, "p:impl") << "&" << endl
             << root_parser << " ()"
             << "{"
             << "return this->" << root_member << ";"
             << "}";

          // root_name ()
          //
          os << "static const char*" << endl
             << root_name << " ();"
             << endl;

          // root_namespace ()
          //
          os << "static const char*" << endl
             << root_namespace << " ();"
             << endl;

          // _error ()
          //
          if (error)
          {
            os << xs_ns_name () << "::parser_error" << endl
               << error << " ()"
               << "{"
               << "return this->" << root_member << "._error ();"
               << "}";
          }

          // reset ()
          //
          if (reset)
          {
            os << "void" << endl
               << reset << " ()"
               << "{"
               << "this->" << root_member << "._reset ();"
               << "}";
          }

          os << "public:" << endl;

          for (TypeInstanceMap::Iterator i (map.begin ()), end (map.end ());
               i != end; ++i)
            os << fq_name (*i->first, "p:impl") << " " << i->second << ";";

          os << "};";
        }
      };
    }

    Void
    generate_parser_aggregate_header (Context& ctx)
    {
      Boolean gen (false);

      {
        Traversal::Schema schema;
        Traversal::Sources sources;

        schema >> sources >> schema;

        Traversal::Names schema_names;
        Traversal::Namespace ns;
        Traversal::Names names;
        AggregateTest test (gen, "paggr");

        schema >> schema_names >> ns >> names >> test;

        schema.dispatch (ctx.schema_root);
      }

      if (gen)
      {
        // Emit "weak" header includes that are used in the file-per-type
        // compilation model.
        //
        {
          Traversal::Schema schema;
          Includes includes (ctx, Includes::source);

          schema >> includes;

          schema.dispatch (ctx.schema_root);
        }

        Traversal::Schema schema;
        Traversal::Sources sources;

        schema >> sources >> schema;

        Traversal::Names schema_names;
        Namespace ns (ctx);
        Traversal::Names names;

        schema >> schema_names >> ns >> names;

        GlobalType type (ctx);
        GlobalElement element (ctx);

        names >> type;
        names >> element;

        schema.dispatch (ctx.schema_root);
      }
    }
  }
}
