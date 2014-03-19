// file      : xsde/cxx/hybrid/parser-aggregate-header.cxx
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <set>

#include <cxx/hybrid/parser-aggregate-header.hxx>
#include <cxx/hybrid/aggregate-elements.hxx>
#include <cxx/hybrid/aggregate-include.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

using namespace std;

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      typedef set<String> InstanceSet;

      // For base types we only want member's types, but not the
      // base itself.
      //
      struct BaseType: Traversal::Complex,
                       Traversal::List,
                       Context
      {
        BaseType (Context& c, Traversal::NodeBase& def)
            : Context (c), def_ (def)
        {
        }

        virtual void
        traverse (SemanticGraph::Complex& c)
        {
          inherits (c);

          if (!restriction_p (c))
          {
            names (c);
            contains_compositor (c);
          }
        }

        virtual void
        traverse (SemanticGraph::List& l)
        {
          def_.dispatch (l.argumented ().type ());
        }

      private:
        Traversal::NodeBase& def_;
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
        ParserDef (Context& c,
                   TypeInstanceMap& map,
                   TypeIdInstanceMap& tid_map,
                   InstanceSet& set)
            : Context (c),
              map_ (map),
              tid_map_ (tid_map),
              set_ (set),
              base_ (c, *this)
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

        virtual void
        traverse (SemanticGraph::Type& t)
        {
          if (map_.find (&t) == map_.end ())
          {
            map_[&t] = find_instance_name (t);

            if (polymorphic (t))
              collect (t);
          }
        }

        virtual void
        traverse (SemanticGraph::List& l)
        {
          if (map_.find (&l) == map_.end ())
          {
            map_[&l] = find_instance_name (l);

            dispatch (l.argumented ().type ());

            if (polymorphic (l))
              collect (l);
          }
        }

        virtual void
        traverse (SemanticGraph::Complex& c)
        {
          if (map_.find (&c) == map_.end ())
          {
            map_[&c] = find_instance_name (c);

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

            if (polymorphic (c))
              collect (c);
          }
        }

        virtual void
        collect (SemanticGraph::Type& t)
        {
          using SemanticGraph::Type;

          for (Type::BegetsIterator i (t.begets_begin ());
               i != t.begets_end ();
               ++i)
          {
            Type& d (i->derived ());

            String id (d.name ());
            if (String ns = xml_ns_name (d))
            {
              id += L' ';
              id += ns;
            }

            dispatch (d);

            if (tid_map_.find (id) == tid_map_.end ())
            {
              tid_map_[id].type = &d;
              tid_map_[id].name = map_.find (&d)->second;
              collect (d);
            }
          }
        }

        // anyType & anySimpleType.
        //
        virtual void
        traverse (SemanticGraph::AnyType& t)
        {
          if (fund_type (t, "any_type"))
          {
            if (polymorphic (t))
              collect (t);
          }
        }

        virtual void
        traverse (SemanticGraph::AnySimpleType& t)
        {
          fund_type (t, "any_simple_type");
        }

        // Boolean.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Boolean& t)
        {
          fund_type (t, "boolean");
        }

        // Integral types.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Byte& t)
        {
          fund_type (t, "byte");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedByte& t)
        {
          fund_type (t, "unsigned_byte");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Short& t)
        {
          fund_type (t, "short");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedShort& t)
        {
          fund_type (t, "unsigned_short");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Int& t)
        {
          fund_type (t, "int");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedInt& t)
        {
          fund_type (t, "unsigned_int");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Long& t)
        {
          fund_type (t, "long");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedLong& t)
        {
          fund_type (t, "unsigned_long");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Integer& t)
        {
          fund_type (t, "integer");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NonPositiveInteger& t)
        {
          fund_type (t, "non_positive_integer");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NonNegativeInteger& t)
        {
          fund_type (t, "non_negative_integer");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::PositiveInteger& t)
        {
          fund_type (t, "positive_integer");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NegativeInteger& t)
        {
          fund_type (t, "negative_integer");
        }

        // Floats.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Float& t)
        {
          fund_type (t, "float");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Double& t)
        {
          fund_type (t, "double");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Decimal& t)
        {
          fund_type (t, "decimal");
        }

        // Strings.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::String& t)
        {
          fund_type (t, "string");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NormalizedString& t)
        {
          fund_type (t, "normalized_string");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Token& t)
        {
          fund_type (t, "token");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NameToken& t)
        {
          fund_type (t, "nmtoken");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NameTokens& t)
        {
          fund_type (t, "nmtokens");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Name& t)
        {
          fund_type (t, "name");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NCName& t)
        {
          fund_type (t, "ncname");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Language& t)
        {
          fund_type (t, "language");
        }


        // Qualified name.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::QName& t)
        {
          fund_type (t, "qname");
        }


        // ID/IDREF.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Id& t)
        {
          fund_type (t, "id");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::IdRef& t)
        {
          fund_type (t, "idref");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::IdRefs& t)
        {
          fund_type (t, "idrefs");
        }

        // URI.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::AnyURI& t)
        {
          fund_type (t, "uri");
        }

        // Binary.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Base64Binary& t)
        {
          fund_type (t, "base64_binary");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::HexBinary& t)
        {
          fund_type (t, "hex_binary");
        }


        // Date/time.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Date& t)
        {
          fund_type (t, "date");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::DateTime& t)
        {
          fund_type (t, "date_time");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Duration& t)
        {
          fund_type (t, "duration");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Day& t)
        {
          fund_type (t, "day");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Month& t)
        {
          fund_type (t, "month");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::MonthDay& t)
        {
          fund_type (t, "month_day");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Year& t)
        {
          fund_type (t, "year");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::YearMonth& t)
        {
          fund_type (t, "year_month");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Time& t)
        {
          fund_type (t, "time");
        }

        // Entity.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Entity& t)
        {
          fund_type (t, "entity");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Entities& t)
        {
          fund_type (t, "entities");
        }

      private:
        virtual bool
        fund_type (SemanticGraph::Type& t, String const& name)
        {
          if (map_.find (&t) == map_.end ())
          {
            map_[&t] = find_instance_name (name);
            return true;
          }

          return false;
        }

        String
        find_instance_name (String const& raw_name)
        {
          String name (escape (raw_name + L"_p_"));

          for (size_t i (1); set_.find (name) != set_.end (); ++i)
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
        TypeIdInstanceMap& tid_map_;
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

        virtual void
        traverse (SemanticGraph::Type& t)
        {
          SemanticGraph::Context& tc (t.context ());

          if (!tc.count ("paggr"))
            return;

          bool poly (polymorphic (t));
          String const& name (tc.get<String> ("paggr"));

          String pre (unclash (name, "pre"));
          String post (unclash (name, "post"));
          String root_parser (unclash (name, "root_parser"));
          String root_map;
          String error, reset;
          String poly_name;
          String parser_map, parser_map_entries;

          InstanceSet set;
          set.insert (name);
          set.insert (pre);
          set.insert (post);
          set.insert (root_parser);

          if (poly)
          {
            root_map = unclash (name, "root_map");
            set.insert (root_map);
          }

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

          if (poly_runtime)
          {
            poly_name = unclash (name, "polymorphic");
            set.insert (poly_name);
          }

          if (poly_code)
          {
            parser_map = unclash (name, "parser_map_");
            parser_map_entries = unclash (name, "parser_map_entries_");

            tc.set ("paggr-parser-map", parser_map);
            tc.set ("paggr-parser-map-entries", parser_map_entries);
          }

          tc.set ("paggr-map", TypeInstanceMap ());
          TypeInstanceMap& map (tc.get<TypeInstanceMap> ("paggr-map"));
          TypeIdInstanceMap tid_map;

          ParserDef def (*this, map, tid_map, set);
          def.dispatch (t);

          if (poly_code)
          {
            if (!tid_map.empty ())
              tc.set ("paggr-tid-map", tid_map);
            else if (poly)
              poly = false; // Polymorphic root without substitutions.
          }

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

          if (!poly)
          {
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
          }

          // root_parser ()
          //
          os << fq_name (t, "p:impl") << "&" << endl
             << root_parser << " ()"
             << "{"
             << "return this->" << root_member << ";"
             << "}";

          if (poly)
          {
            // root_map ()
            //
            os << "const " << xs_ns_name () + L"::parser_map&" << endl
               << root_map << " ()"
               << "{"
               << "return this->" << parser_map << ";"
               << "}";
          }

          // _error ()
          //
          if (!poly && error)
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
               << "this->" << root_member << "._reset ();";

            if (poly)
              os << "this->" << parser_map << ".reset ();";

            os << "}";
          }

          // polymorphic ()
          //
          if (poly_runtime)
          {
            os << "static bool" << endl
               << poly_name << " ()"
               << "{"
               << "return " << (tid_map.size () > 0 ? "true" : "false") << ";"
               << "}";
          }

          os << "public:" << endl;

          for (TypeInstanceMap::iterator i (map.begin ()), end (map.end ());
               i != end; ++i)
            os << fq_name (*i->first, "p:impl") << " " << i->second << ";";

          if (tid_map.size () > 0)
          {
            os << endl
               << "::xsde::cxx::hybrid::parser_map_impl " << parser_map << ";"
               << "::xsde::cxx::hybrid::parser_map_impl::entry " <<
              parser_map_entries << "[" << tid_map.size () << "UL];";
          }
          os << "};";
        }
      };

      struct GlobalElement: Traversal::Element, Context
      {
        GlobalElement (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          SemanticGraph::Context& ec (e.context ());

          if (!ec.count ("paggr"))
            return;

          SemanticGraph::Type& t (e.type ());
          bool poly (polymorphic (t));
          String const& name (ec.get<String> ("paggr"));

          String pre (unclash (name, "pre"));
          String post (unclash (name, "post"));
          String root_parser (unclash (name, "root_parser"));
          String root_map;
          String root_name (unclash (name, "root_name"));
          String root_namespace (unclash (name, "root_namespace"));
          String error, reset;
          String poly_name;
          String parser_map, parser_map_entries;

          InstanceSet set;
          set.insert (name);
          set.insert (pre);
          set.insert (post);
          set.insert (root_parser);

          if (poly)
          {
            root_map = unclash (name, "root_map");
            set.insert (root_map);
          }

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

          if (poly_runtime)
          {
            poly_name = unclash (name, "polymorphic");
            set.insert (poly_name);
          }

          if (poly_code)
          {
            parser_map = unclash (name, "parser_map_");
            parser_map_entries = unclash (name, "parser_map_entries_");

            ec.set ("paggr-parser-map", parser_map);
            ec.set ("paggr-parser-map-entries", parser_map_entries);
          }

          ec.set ("paggr-map", TypeInstanceMap ());
          TypeInstanceMap& map (ec.get<TypeInstanceMap> ("paggr-map"));
          TypeIdInstanceMap tid_map;

          ParserDef def (*this, map, tid_map, set);
          def.dispatch (t);

          if (poly_code)
          {
            if (!tid_map.empty ())
              ec.set ("paggr-tid-map", tid_map);
            else if (poly)
              poly = false; // Polymorphic root without substitutions.
          }

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

          if (!poly)
          {
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
          }

          // root_parser ()
          //
          os << fq_name (t, "p:impl") << "&" << endl
             << root_parser << " ()"
             << "{"
             << "return this->" << root_member << ";"
             << "}";

          if (poly)
          {
            // root_map ()
            //
            os << "const " << xs_ns_name () + L"::parser_map&" << endl
               << root_map << " ()"
               << "{"
               << "return this->" << parser_map << ";"
               << "}";
          }

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
          if (!poly && error)
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
               << "this->" << root_member << "._reset ();";

            if (poly)
              os << "this->" << parser_map << ".reset ();";

            os << "}";
          }

          // polymorphic ()
          //
          if (poly_runtime)
          {
            os << "static bool" << endl
               << poly_name << " ()"
               << "{"
               << "return " << (tid_map.size () > 0 ? "true" : "false") << ";"
               << "}";
          }

          os << "public:" << endl;

          for (TypeInstanceMap::iterator i (map.begin ()), end (map.end ());
               i != end; ++i)
            os << fq_name (*i->first, "p:impl") << " " << i->second << ";";

          if (tid_map.size () > 0)
          {
            os << endl
               << "::xsde::cxx::hybrid::parser_map_impl " << parser_map << ";"
               << "::xsde::cxx::hybrid::parser_map_impl::entry " <<
              parser_map_entries << "[" << tid_map.size () << "UL];";
          }

          os << "};";
        }
      };
    }

    void
    generate_parser_aggregate_header (Context& ctx)
    {
      bool gen (false);

      {
        Traversal::Schema schema;
        Sources sources;

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
        if (ctx.poly_code)
          ctx.os << "#include <xsde/cxx/hybrid/parser-map.hxx>" << endl
                 << endl;

        // Emit "weak" header includes that are used in the file-per-type
        // compilation model.
        //
        {
          Traversal::Schema schema;
          Includes includes (ctx, Includes::source);

          schema >> includes;

          schema.dispatch (ctx.schema_root);
        }

        // Emit includes for additional schemas that define derived
        // polymorphic types.
        //
        if (ctx.poly_code)
        {
          Traversal::Schema schema;
          Sources sources;

          schema >> sources >> schema;

          Traversal::Names schema_names;
          Traversal::Namespace ns;
          Traversal::Names names;
          AggregateInclude include (ctx, "paggr");

          schema >> schema_names >> ns >> names >> include;

          schema.dispatch (ctx.schema_root);
        }

        // Generate code.
        //
        Traversal::Schema schema;
        Sources sources;

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
