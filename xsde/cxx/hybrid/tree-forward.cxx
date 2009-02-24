// file      : xsd/cxx/hybrid/tree-forward.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/hybrid/tree-forward.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      struct List : Traversal::List, Context
      {
        List (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& l)
        {
          os << "class " << ename (l) << ";";
        }
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
          os << "class " << ename (u) << ";";
        }
      };

      struct Complex : Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& c)
        {
          os << "class " << ename (c) << ";";
        }
      };

      struct FundType : Context,

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
                        Traversal::Fundamental::Entities
      {
        FundType (Context& c)
            : Context (c)
        {
          if (stl)
            string_type_ = L"::std::string";
          else
            string_type_ = L"char*";

          if (options.value<CLI::no_long_long> ())
          {
            long_type_ = L"long";
            unsigned_long_type_ = L"unsigned long";
          }
          else
          {
            long_type_ = L"long long";
            unsigned_long_type_ = L"unsigned long long";
          }
        }

        // anyType & anySimpleType.
        //
        virtual Void
        traverse (SemanticGraph::AnyType&)
        {
          gen_using ("::xsde::cxx::hybrid::any_type");
        }

        virtual Void
        traverse (SemanticGraph::AnySimpleType&)
        {
          gen_using ("::xsde::cxx::hybrid::any_simple_type");
        }

        // Boolean.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Boolean&)
        {
          gen_typedef ("boolean", "bool");
          gen_using ("::xsde::cxx::hybrid::boolean_base");
          os << endl;
        }

        // Integral types.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Byte&)
        {
          gen_typedef ("byte", "signed char");
          gen_using ("::xsde::cxx::hybrid::byte_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedByte&)
        {
          gen_typedef ("unsigned_byte", "unsigned char");
          gen_using ("::xsde::cxx::hybrid::unsigned_byte_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Short&)
        {
          gen_typedef ("short", "short");
          gen_using ("::xsde::cxx::hybrid::short_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedShort&)
        {
          gen_typedef ("unsigned_short", "unsigned short");
          gen_using ("::xsde::cxx::hybrid::unsigned_short_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Int&)
        {
          gen_typedef ("int", "int");
          gen_using ("::xsde::cxx::hybrid::int_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedInt&)
        {
          gen_typedef ("unsigned_int", "unsigned int");
          gen_using ("::xsde::cxx::hybrid::unsigned_int_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Long&)
        {
          gen_typedef ("long", long_type_);
          gen_using ("::xsde::cxx::hybrid::long_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedLong&)
        {
          gen_typedef ("unsigned_long", unsigned_long_type_);
          gen_using ("::xsde::cxx::hybrid::unsigned_long_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Integer&)
        {
          gen_typedef ("integer", "long");
          gen_using ("::xsde::cxx::hybrid::integer_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NegativeInteger&)
        {
          gen_typedef ("negative_integer", "long");
          gen_using ("::xsde::cxx::hybrid::negative_integer_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NonPositiveInteger&)
        {
          gen_typedef ("non_positive_integer", "long");
          gen_using ("::xsde::cxx::hybrid::non_positive_integer_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::PositiveInteger&)
        {
          gen_typedef ("positive_integer", "unsigned long");
          gen_using ("::xsde::cxx::hybrid::positive_integer_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NonNegativeInteger&)
        {
          gen_typedef ("non_negative_integer", "unsigned long");
          gen_using ("::xsde::cxx::hybrid::non_negative_integer_base");
          os << endl;
        }

        // Floats.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Float&)
        {
          gen_typedef ("float", "float");
          gen_using ("::xsde::cxx::hybrid::float_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Double&)
        {
          gen_typedef ("double", "double");
          gen_using ("::xsde::cxx::hybrid::double_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Decimal&)
        {
          gen_typedef ("decimal", "double");
          gen_using ("::xsde::cxx::hybrid::decimal_base");
          os << endl;
        }

        // Strings.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::String&)
        {
          gen_typedef ("string", string_type_);
          gen_using ("::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NormalizedString&)
        {
          gen_typedef ("normalized_string", string_type_);

          if (!stl)
            gen_typedef ("normalized_string_base",
                         "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Token&)
        {
          gen_typedef ("token", string_type_);
          if (!stl)
            gen_typedef ("token_base", "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NameToken&)
        {
          gen_typedef ("nmtoken", string_type_);
          if (!stl)
            gen_typedef ("nmtoken_base", "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NameTokens&)
        {
          gen_typedef ("nmtokens", "::xsde::cxx::string_sequence");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Name&)
        {
          gen_typedef ("name", string_type_);
          if (!stl)
            gen_typedef ("name_base", "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NCName&)
        {
          gen_typedef ("ncname", string_type_);
          if (!stl)
            gen_typedef ("ncname_base", "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Language&)
        {
          gen_typedef ("language", string_type_);
          if (!stl)
            gen_typedef ("language_base", "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        // Qualified name.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::QName&)
        {
          gen_using ("::xsde::cxx::qname");
          os << endl;
        }

        // ID/IDREF.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Id&)
        {
          gen_typedef ("id", string_type_);
          if (!stl)
            gen_typedef ("id_base", "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::IdRef&)
        {
          gen_typedef ("idref", string_type_);
          if (!stl)
            gen_typedef ("idref_base", "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::IdRefs&)
        {
          gen_typedef ("idrefs", "::xsde::cxx::string_sequence");
          os << endl;
        }

        // URI.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::AnyURI&)
        {
          gen_typedef ("uri", string_type_);
          if (!stl)
            gen_typedef ("uri_base", "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        // Binary.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Base64Binary&)
        {
          gen_typedef ("base64_binary", "::xsde::cxx::buffer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::HexBinary&)
        {
          gen_typedef ("hex_binary", "::xsde::cxx::buffer");
          os << endl;
        }


        // Date/time.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Date&)
        {
          gen_using ("::xsde::cxx::date");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::DateTime&)
        {
          gen_using ("::xsde::cxx::date_time");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Duration&)
        {
          gen_using ("::xsde::cxx::duration");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Day&)
        {
          gen_using ("::xsde::cxx::gday");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Month&)
        {
          gen_using ("::xsde::cxx::gmonth");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::MonthDay&)
        {
          gen_using ("::xsde::cxx::gmonth_day");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Year&)
        {
          gen_using ("::xsde::cxx::gyear");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::YearMonth&)
        {
          gen_using ("::xsde::cxx::gyear_month");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Time&)
        {
          gen_using ("::xsde::cxx::time");
        }

        // Entity.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Entity&)
        {
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Entities&)
        {
        }

      private:
        Void
        gen_typedef (String const& name, String const& type)
        {
          os << "typedef " << type << " " << escape (name) << ";";
        }

        Void
        gen_using (String const& name)
        {
          os << "using " << name << ";";
        }

        String string_type_;
        String long_type_;
        String unsigned_long_type_;
      };

      struct FundNamespace : Namespace, Context
      {
        FundNamespace (Context& c)
            : Namespace (c), Context (c)
        {
        }

        void
        traverse (Type& ns)
        {
          pre (ns);
          names (ns);

          // strdupx
          //
          if (!stl)
          {
            os << endl
               << "using ::xsde::cxx::strdupx;"
               << "using ::xsde::cxx::strndupx;";
          }

          post (ns);
        }
      };
    }

    Void
    generate_tree_forward (Context& ctx, Boolean generate_xml_schema)
    {
      NarrowString xml_schema (ctx.options.value<CLI::extern_xml_schema> ());

      // Inlcude or Emit fundamental types.
      //
      if (!generate_xml_schema && xml_schema)
      {
        String name (ctx.hxx_expr->merge (xml_schema));

        ctx.os << "#include " << ctx.process_include_path (name) << endl
               << endl;
      }
      else
      {
        if (ctx.stl)
          ctx.os << "#include <string>" << endl;
        else
          ctx.os << "#include <xsde/cxx/strdupx.hxx>" << endl;

        ctx.os << "#include <xsde/cxx/hybrid/xml-schema.hxx>" << endl
               << endl;

        if (generate_xml_schema)
        {
          Traversal::Schema schema;
          Traversal::Names names;
          FundNamespace ns (ctx);

          schema >> names >> ns;

          Traversal::Names ns_names;
          FundType type (ctx);

          ns >> ns_names >> type;

          schema.dispatch (ctx.schema_root);

          return;
        }
        else
        {
          Traversal::Schema schema, xsd;
          Traversal::Implies implies;
          Traversal::Names names;
          FundNamespace ns (ctx);

          schema >> implies >> xsd >> names >> ns;

          Traversal::Names ns_names;
          FundType type (ctx);

          ns >> ns_names >> type;

          schema.dispatch (ctx.schema_root);
        }
      }

      Traversal::Schema schema;

      Traversal::Sources sources;
      Traversal::Names names_ns, names;

      Namespace ns (ctx);

      List list (ctx);
      Union union_ (ctx);
      Complex complex (ctx);
      //Enumeration enumeration (ctx);

      schema >> sources >> schema;
      schema >> names_ns >> ns >> names;

      names >> list;
      names >> union_;
      names >> complex;
      //names >> enumeration;

      schema.dispatch (ctx.schema_root);

      ctx.os << endl;
    }
  }
}
