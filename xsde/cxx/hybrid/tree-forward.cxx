// file      : xsd/cxx/hybrid/tree-forward.cxx
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
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
      struct Enumeration : Traversal::Enumeration, Context
      {
        Enumeration (Context& c, Traversal::Complex& complex)
            : Context (c), complex_ (complex)
        {
        }

        virtual void
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

          SemanticGraph::Context& ctx (e.context ());

          // Forward-declare the base.
          //
          if (ctx.count ("name-base"))
          {
            if (String base = ctx.get<String> ("name-base"))
              os << "class " << base << ";";
          }

          // Typedef or forward-declare the type.
          //
          if (ctx.count ("name-typedef"))
          {
            os << "typedef " << ctx.get<String> ("name-typedef") << " " <<
              ename (e) << ";";
          }
          else
            os << "class " << ename (e) << ";";
        }

      private:
        Traversal::Complex& complex_;
      };

      struct List : Traversal::List, Context
      {
        List (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& l)
        {
          SemanticGraph::Context& ctx (l.context ());

          // Forward-declare the base.
          //
          if (ctx.count ("name-base"))
          {
            if (String base = ctx.get<String> ("name-base"))
              os << "class " << base << ";";
          }

          // Typedef or forward-declare the type.
          //
          if (ctx.count ("name-typedef"))
          {
            os << "typedef " << ctx.get<String> ("name-typedef") << " " <<
              ename (l) << ";";
          }
          else
            os << "class " << ename (l) << ";";
        }
      };

      struct Union : Traversal::Union, Context
      {
        Union (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& u)
        {
          SemanticGraph::Context& ctx (u.context ());

          // Forward-declare the base.
          //
          if (ctx.count ("name-base"))
          {
            if (String base = ctx.get<String> ("name-base"))
              os << "class " << base << ";";
          }

          // Typedef or forward-declare the type.
          //
          if (ctx.count ("name-typedef"))
          {
            os << "typedef " << ctx.get<String> ("name-typedef") << " " <<
              ename (u) << ";";
          }
          else
            os << "class " << ename (u) << ";";
        }
      };

      struct Complex : Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& c)
        {
          SemanticGraph::Context& ctx (c.context ());

          // Forward-declare the base.
          //
          if (ctx.count ("name-base"))
          {
            if (String base = ctx.get<String> ("name-base"))
              os << "class " << base << ";";
          }

          // Typedef or forward-declare the type.
          //
          if (ctx.count ("name-typedef"))
          {
            os << "typedef " << ctx.get<String> ("name-typedef") << " " <<
              ename (c) << ";";
          }
          else
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

          if (options.no_long_long ())
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
        virtual void
        traverse (SemanticGraph::AnyType&)
        {
          gen_using ("::xsde::cxx::hybrid::any_type");
        }

        virtual void
        traverse (SemanticGraph::AnySimpleType&)
        {
          gen_typedef ("any_simple_type", string_type_);

          if (!stl)
            gen_typedef ("any_simple_type_base",
                         "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        // Boolean.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Boolean&)
        {
          gen_typedef ("boolean", "bool");
          gen_using ("::xsde::cxx::hybrid::boolean_base");
          os << endl;
        }

        // Integral types.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Byte&)
        {
          gen_typedef ("byte", "signed char");
          gen_using ("::xsde::cxx::hybrid::byte_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedByte&)
        {
          gen_typedef ("unsigned_byte", "unsigned char");
          gen_using ("::xsde::cxx::hybrid::unsigned_byte_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Short&)
        {
          gen_typedef ("short", "short");
          gen_using ("::xsde::cxx::hybrid::short_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedShort&)
        {
          gen_typedef ("unsigned_short", "unsigned short");
          gen_using ("::xsde::cxx::hybrid::unsigned_short_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Int&)
        {
          gen_typedef ("int", "int");
          gen_using ("::xsde::cxx::hybrid::int_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedInt&)
        {
          gen_typedef ("unsigned_int", "unsigned int");
          gen_using ("::xsde::cxx::hybrid::unsigned_int_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Long&)
        {
          gen_typedef ("long", long_type_);
          gen_using ("::xsde::cxx::hybrid::long_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedLong&)
        {
          gen_typedef ("unsigned_long", unsigned_long_type_);
          gen_using ("::xsde::cxx::hybrid::unsigned_long_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Integer&)
        {
          gen_typedef ("integer", "long");
          gen_using ("::xsde::cxx::hybrid::integer_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NegativeInteger&)
        {
          gen_typedef ("negative_integer", "long");
          gen_using ("::xsde::cxx::hybrid::negative_integer_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NonPositiveInteger&)
        {
          gen_typedef ("non_positive_integer", "long");
          gen_using ("::xsde::cxx::hybrid::non_positive_integer_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::PositiveInteger&)
        {
          gen_typedef ("positive_integer", "unsigned long");
          gen_using ("::xsde::cxx::hybrid::positive_integer_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NonNegativeInteger&)
        {
          gen_typedef ("non_negative_integer", "unsigned long");
          gen_using ("::xsde::cxx::hybrid::non_negative_integer_base");
          os << endl;
        }

        // Floats.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Float&)
        {
          gen_typedef ("float", "float");
          gen_using ("::xsde::cxx::hybrid::float_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Double&)
        {
          gen_typedef ("double", "double");
          gen_using ("::xsde::cxx::hybrid::double_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Decimal&)
        {
          gen_typedef ("decimal", "double");
          gen_using ("::xsde::cxx::hybrid::decimal_base");
          os << endl;
        }

        // Strings.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::String&)
        {
          gen_typedef ("string", string_type_);

          if (!stl)
            gen_using ("::xsde::cxx::hybrid::string_base");

          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NormalizedString&)
        {
          gen_typedef ("normalized_string", string_type_);

          if (!stl)
            gen_typedef ("normalized_string_base",
                         "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Token&)
        {
          gen_typedef ("token", string_type_);
          if (!stl)
            gen_typedef ("token_base", "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NameToken&)
        {
          gen_typedef ("nmtoken", string_type_);
          if (!stl)
            gen_typedef ("nmtoken_base", "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NameTokens&)
        {
          gen_typedef ("nmtokens", "::xsde::cxx::string_sequence");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Name&)
        {
          gen_typedef ("name", string_type_);
          if (!stl)
            gen_typedef ("name_base", "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NCName&)
        {
          gen_typedef ("ncname", string_type_);
          if (!stl)
            gen_typedef ("ncname_base", "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Language&)
        {
          gen_typedef ("language", string_type_);
          if (!stl)
            gen_typedef ("language_base", "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        // Qualified name.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::QName&)
        {
          gen_using ("::xsde::cxx::qname");
          os << endl;
        }

        // ID/IDREF.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Id&)
        {
          gen_typedef ("id", string_type_);
          if (!stl)
            gen_typedef ("id_base", "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::IdRef&)
        {
          gen_typedef ("idref", string_type_);
          if (!stl)
            gen_typedef ("idref_base", "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        virtual void
        traverse (SemanticGraph::Fundamental::IdRefs&)
        {
          gen_typedef ("idrefs", "::xsde::cxx::string_sequence");
          os << endl;
        }

        // URI.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::AnyURI&)
        {
          gen_typedef ("uri", string_type_);
          if (!stl)
            gen_typedef ("uri_base", "::xsde::cxx::hybrid::string_base");
          os << endl;
        }

        // Binary.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Base64Binary&)
        {
          gen_using ("::xsde::cxx::buffer");
          gen_typedef ("base64_binary", "::xsde::cxx::buffer");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::HexBinary&)
        {
          gen_typedef ("hex_binary", "::xsde::cxx::buffer");
          os << endl;
        }


        // Date/time.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Date&)
        {
          gen_using ("::xsde::cxx::time_zone");
          gen_using ("::xsde::cxx::date");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::DateTime&)
        {
          gen_using ("::xsde::cxx::date_time");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Duration&)
        {
          gen_using ("::xsde::cxx::duration");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Day&)
        {
          gen_using ("::xsde::cxx::gday");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Month&)
        {
          gen_using ("::xsde::cxx::gmonth");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::MonthDay&)
        {
          gen_using ("::xsde::cxx::gmonth_day");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Year&)
        {
          gen_using ("::xsde::cxx::gyear");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::YearMonth&)
        {
          gen_using ("::xsde::cxx::gyear_month");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Time&)
        {
          gen_using ("::xsde::cxx::time");
        }

        // Entity.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Entity&)
        {
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Entities&)
        {
        }

      private:
        void
        gen_typedef (String const& name, String const& type)
        {
          os << "typedef " << type << " " << escape (name) << ";";
        }

        void
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

          // Custom allocator.
          //
          if (custom_alloc)
          {
            os << endl
               << "// Custom allocator." << endl
               << "//" << endl
               << "using ::xsde::cxx::alloc;"
               << "using ::xsde::cxx::free;";

            if (exceptions)
              os << "using ::xsde::cxx::alloc_guard;";
          }

          // strdupx
          //
          if (!stl)
          {
            os << endl
               << "using ::xsde::cxx::strdupx;"
               << "using ::xsde::cxx::strndupx;";
          }

          // sequences
          //
          os << endl
             << "using ::xsde::cxx::hybrid::pod_sequence;"
             << "using ::xsde::cxx::hybrid::fix_sequence;"
             << "using ::xsde::cxx::hybrid::var_sequence;"
             << "using ::xsde::cxx::string_sequence;"
             << "using ::xsde::cxx::hybrid::data_sequence;";

          // Data representation stream types.
          //
          bool icdr (false), ocdr (false);
          bool ixdr (false), oxdr (false);

          for (NarrowStrings::const_iterator i (istreams.begin ());
               i != istreams.end (); ++i)
          {
            if (*i == "CDR")
              icdr = true;
            else if (*i == "XDR")
              ixdr = true;
          }

          for (NarrowStrings::const_iterator i (ostreams.begin ());
               i != ostreams.end (); ++i)
          {
            if (*i == "CDR")
              ocdr = true;
            else if (*i == "XDR")
              oxdr = true;
          }

          if (icdr || ocdr)
          {
            os << endl;

            if (exceptions)
              os << "using ::xsde::cxx::hybrid::cdr_exception;";

            if (icdr)
              os << "using ::xsde::cxx::hybrid::icdrstream;";

            if (ocdr)
              os << "using ::xsde::cxx::hybrid::ocdrstream;";
          }

          if (ixdr || oxdr)
          {
            os << endl;

            if (exceptions)
              os << "using ::xsde::cxx::hybrid::xdr_exception;";

            if (ixdr)
              os << "using ::xsde::cxx::hybrid::ixdrstream;";

            if (oxdr)
              os << "using ::xsde::cxx::hybrid::oxdrstream;";
          }

          post (ns);
        }
      };
    }

    void
    generate_tree_forward (Context& ctx, bool generate_xml_schema)
    {
      NarrowString xml_schema (ctx.options.extern_xml_schema ());

      // Inlcude or Emit fundamental types.
      //
      if (!generate_xml_schema && xml_schema)
      {
        String name (ctx.hxx_expr->replace (xml_schema));

        ctx.os << "#include " << ctx.process_include_path (name) << endl
               << endl;
      }
      else
      {
        if (ctx.custom_alloc)
          ctx.os << "#include <xsde/cxx/allocator.hxx>" << endl
                 << endl;

        if (ctx.stl)
          ctx.os << "#include <string>" << endl;
        else
          ctx.os << "#include <xsde/cxx/strdupx.hxx>" << endl;

        ctx.os << "#include <xsde/cxx/hybrid/xml-schema.hxx>" << endl
               << "#include <xsde/cxx/hybrid/sequence.hxx>" << endl
               << endl;

        // Data representation stream includes.
        //
        for (NarrowStrings::const_iterator i (ctx.istreams.begin ());
             i != ctx.istreams.end (); ++i)
        {
          if (*i == "CDR")
            ctx.os << "#include <xsde/cxx/hybrid/cdr/istream.hxx>" << endl
                   << endl;
          else if (*i == "XDR")
            ctx.os << "#include <xsde/cxx/hybrid/xdr/istream.hxx>" << endl
                   << endl;
        }

        for (NarrowStrings::const_iterator i (ctx.ostreams.begin ());
             i != ctx.ostreams.end (); ++i)
        {
          if (*i == "CDR")
            ctx.os << "#include <xsde/cxx/hybrid/cdr/ostream.hxx>" << endl
                   << endl;
          else if (*i == "XDR")
            ctx.os << "#include <xsde/cxx/hybrid/xdr/ostream.hxx>" << endl
                   << endl;
        }

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

      ctx.os << endl;
    }
  }
}
