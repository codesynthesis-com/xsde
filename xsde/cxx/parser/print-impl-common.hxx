// file      : xsde/cxx/parser/print-impl-common.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_PARSER_PRINT_IMPL_COMMON_HXX
#define CXX_PARSER_PRINT_IMPL_COMMON_HXX

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

#include <cxx/parser/elements.hxx>

namespace CXX
{
  namespace Parser
  {
    struct PrintCall: Traversal::Type,

                      Traversal::AnySimpleType,

                      Traversal::Fundamental::Boolean,

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

                      Context
    {
      PrintCall (Context& c, String const& tag, String const& arg)
          : Context (c), tag_ (tag), arg_ (arg)
      {
      }

      virtual void
      traverse (SemanticGraph::Type&)
      {
        gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::AnySimpleType& t)
      {
        gen_string (t);
      }

      // Boolean.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Boolean& t)
      {
        if (default_type (t, "bool"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %u\n") << ", " <<
              arg_ << ");";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
              arg_ << " << std::endl;";
        }
        else
          gen_user_type ();
      }

      // Integral types.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Byte& t)
      {
        if (default_type (t, "signed char"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %d\n") << ", " <<
              arg_ << ");";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
              "static_cast<short> (" << arg_ << ") << std::endl;";
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::UnsignedByte& t)
      {
        if (default_type (t, "unsigned char"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %u\n") << ", " <<
              arg_ << ");";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
              "static_cast<unsigned short> (" << arg_ << ") << std::endl;";
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Short& t)
      {
        if (default_type (t, "short"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %d\n") << ", " <<
              arg_ << ");";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
              arg_ << " << std::endl;";
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::UnsignedShort& t)
      {
        if (default_type (t, "unsigned short"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %u\n") << ", " <<
              arg_ << ");";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
              arg_ << " << std::endl;";
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Int& t)
      {
        if (default_type (t, "int"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %d\n") << ", " <<
              arg_ << ");";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
              arg_ << " << std::endl;";
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::UnsignedInt& t)
      {
        if (default_type (t, "unsigned int"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %u\n") << ", " <<
              arg_ << ");";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
              arg_ << " << std::endl;";
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Long& t)
      {
        if (options.no_long_long ())
        {
          if (default_type (t, "long"))
          {
            if (options.no_iostream ())
              os << "printf (" << strlit (tag_ + L": %ld\n") << ", " <<
                arg_ << ");";
            else
              os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
                arg_ << " << std::endl;";
          }
          else
            gen_user_type ();
        }
        else
        {
          if (default_type (t, "long long"))
          {
            if (options.no_iostream ())
              os << "printf (" << strlit (tag_ + L": %lld\n") << ", " <<
                arg_ << ");";
            else
              os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
                arg_ << " << std::endl;";
          }
          else
            gen_user_type ();
        }
      }

      virtual void
      traverse (SemanticGraph::Fundamental::UnsignedLong& t)
      {
        if (options.no_long_long ())
        {
          if (default_type (t, "unsigned long"))
          {
            if (options.no_iostream ())
              os << "printf (" << strlit (tag_ + L": %lu\n") << ", " <<
                arg_ << ");";
            else
              os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
                arg_ << " << std::endl;";
          }
          else
            gen_user_type ();
        }
        else
        {
          if (default_type (t, "unsigned long long"))
          {
            if (options.no_iostream ())
              os << "printf (" << strlit (tag_ + L": %llu\n") << ", " <<
                arg_ << ");";
            else
              os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
                arg_ << " << std::endl;";
          }
          else
            gen_user_type ();
        }
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Integer& t)
      {
        if (default_type (t, "long"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %ld\n") << ", " <<
              arg_ << ");";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
              arg_ << " << std::endl;";
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NegativeInteger& t)
      {
        if (default_type (t, "long"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %ld\n") << ", " <<
              arg_ << ");";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
              arg_ << " << std::endl;";
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NonPositiveInteger& t)
      {
        if (default_type (t, "long"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %ld\n") << ", " <<
              arg_ << ");";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
              arg_ << " << std::endl;";
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::PositiveInteger& t)
      {
        if (default_type (t, "unsigned long"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %lu\n") << ", " <<
              arg_ << ");";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
              arg_ << " << std::endl;";
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NonNegativeInteger& t)
      {
        if (default_type (t, "unsigned long"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %lu\n") << ", " <<
              arg_ << ");";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
              arg_ << " << std::endl;";
        }
        else
          gen_user_type ();
      }

      // Floats.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Float& t)
      {
        if (default_type (t, "float"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %g\n") << ", " <<
              arg_ << ");";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
              arg_ << " << std::endl;";
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Double& t)
      {
        if (default_type (t, "double"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %g\n") << ", " <<
              arg_ << ");";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
              arg_ << " << std::endl;";
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Decimal& t)
      {
        if (default_type (t, "double"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %g\n") << ", " <<
              arg_ << ");";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
              arg_ << " << std::endl;";
        }
        else
          gen_user_type ();
      }

      // Strings.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::String& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NormalizedString& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Token& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NameToken& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Name& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NCName& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Language& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Id& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::IdRef& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::AnyURI& t)
      {
        gen_string (t);
      }

      // String sequences.
      //

      virtual void
      traverse (SemanticGraph::Fundamental::NameTokens& t)
      {
        gen_sequence (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::IdRefs& t)
      {
        gen_sequence (t);
      }

      // QName
      //

      virtual void
      traverse (SemanticGraph::Fundamental::QName& t)
      {
        if (options.no_stl ())
        {
          if (default_type (t, xs_ns_name () + L"::qname*"))
          {
            if (options.no_iostream ())
              os << "if (" << arg_ << "->prefix ()[0] == '\\0')" << endl
                 << "printf (" << strlit (tag_ + L": %s\n") << ", " <<
                arg_ << "->name ());"
                 << "else" << endl
                 << "printf (" << strlit (tag_ + L": %s:%s\n") << "," << endl
                 << arg_ << "->prefix ()," << endl
                 << arg_ << "->name ());";
            else
              os << "if (" << arg_ << "->prefix ()[0] == '\\0')" << endl
                 << "std::cout << " << strlit (tag_ + L": ") << " << " <<
                arg_ << "->name () << std::endl;"
                 << "else" << endl
                 << "std::cout << " << strlit (tag_ + L": ") << " << " <<
                arg_ << "->prefix ()" << endl
                 << "  << ':' << " << arg_ << "->name () << std::endl;";
          }
          else
            gen_user_type ();
        }
        else
        {
          if (default_type (t, xs_ns_name () + L"::qname"))
          {
            if (options.no_iostream ())
              os << "if (" << arg_ << ".prefix ().empty ())" << endl
                 << "printf (" << strlit (tag_ + L": %s\n") << ", " <<
                arg_ << ".name ().c_str ());"
                 << "else" << endl
                 << "printf (" << strlit (tag_ + L": %s:%s\n") << "," << endl
                 << arg_ << ".prefix ().c_str ()," << endl
                 << arg_ << ".name ().c_str ());";
            else
              os << "if (" << arg_ << ".prefix ().empty ())" << endl
                 << "std::cout << " << strlit (tag_ + L": ") << " << " <<
                arg_ << ".name () << std::endl;"
                 << "else" << endl
                 << "std::cout << " << strlit (tag_ + L": ") << " << " <<
                arg_ << ".prefix ()" << endl
                 << "  << ':' << " << arg_ << ".name () << std::endl;";
          }
          else
            gen_user_type ();
        }
      }

      // Binary.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Base64Binary& t)
      {
        gen_buffer (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::HexBinary& t)
      {
        gen_buffer (t);
      }

      // Date/time.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Date& t)
      {
        if (default_type (t, xs_ns_name () + L"::date"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %d-%u-%u") << "," << endl
               << arg_ << ".year ()," << endl
               << arg_ << ".month ()," << endl
               << arg_ << ".day ());" << endl;
          else
            os << "std::cout << " << strlit (tag_ + L": ") << endl
               << " << " << arg_ << ".year () << '-'" << endl
               << " << " << arg_ << ".month () << '-'" << endl
               << " << " << arg_ << ".day ();";

          gen_time_zone ();
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::DateTime& t)
      {
        if (default_type (t, xs_ns_name () + L"::date_time"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %d-%u-%uT%u:%u:%g") <<
              "," << endl
               << arg_ << ".year ()," << endl
               << arg_ << ".month ()," << endl
               << arg_ << ".day ()," << endl
               << arg_ << ".hours ()," << endl
               << arg_ << ".minutes ()," << endl
               << arg_ << ".seconds ());";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << endl
               << " << " << arg_ << ".year () << '-'" << endl
               << " << " << arg_ << ".month () << '-'" << endl
               << " << " << arg_ << ".day () << 'T'" << endl
               << " << " << arg_ << ".hours () << ':'" << endl
               << " << " << arg_ << ".minutes () << ':'" << endl
               << " << " << arg_ << ".seconds ();";

          gen_time_zone ();
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Duration& t)
      {
        if (default_type (t, xs_ns_name () + L"::duration"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": ") << ");"
               << endl
               << "if (" << arg_ << ".negative ())" << endl
               << "printf (\"-\");"
               << endl
               << "printf (\"P%uY%uM%uDT%uH%uM%gS\\n\"," << endl
               << arg_ << ".years ()," << endl
               << arg_ << ".months ()," << endl
               << arg_ << ".days ()," << endl
               << arg_ << ".hours ()," << endl
               << arg_ << ".minutes ()," << endl
               << arg_ << ".seconds ());";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << ";"
               << endl
               << "if (" << arg_ << ".negative ())" << endl
               << "std::cout << '-';"
               << endl
               << "std::cout << 'P'" << endl
               << " << " << arg_ << ".years () << 'Y'" << endl
               << " << " << arg_ << ".months () << 'M'" << endl
               << " << " << arg_ << ".days () << \"DT\"" << endl
               << " << " << arg_ << ".hours () << 'H'" << endl
               << " << " << arg_ << ".minutes () << 'M'" << endl
               << " << " << arg_ << ".seconds () << 'S'"
               << " << std::endl;";
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Day& t)
      {
        if (default_type (t, xs_ns_name () + L"::gday"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": ---%u") << ", " <<
              arg_ << ".day ());";
          else
            os << "std::cout << " << strlit (tag_ + L": ---") <<
              " << " << arg_ << ".day ();";

          gen_time_zone ();
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Month& t)
      {
        if (default_type (t, xs_ns_name () + L"::gmonth"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": --%u") << ", " <<
              arg_ << ".month ());";
          else
            os << "std::cout << " << strlit (tag_ + L": --") <<
              " << " << arg_ << ".month ();";

          gen_time_zone ();
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::MonthDay& t)
      {
        if (default_type (t, xs_ns_name () + L"::gmonth_day"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": --%u-%u") << "," << endl
               << arg_ << ".month ()," << endl
               << arg_ << ".day ());";
          else
            os << "std::cout << " << strlit (tag_ + L": --") << endl
               << " << " << arg_ << ".month () << '-'" << endl
               << " << " << arg_ << ".day ();";

          gen_time_zone ();
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Year& t)
      {
        if (default_type (t, xs_ns_name () + L"::gyear"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %d") << ", " <<
              arg_ << ".year ());";
          else
            os << "std::cout << " << strlit (tag_ + L": ") <<
              " << " << arg_ << ".year ();";

          gen_time_zone ();
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::YearMonth& t)
      {
        if (default_type (t, xs_ns_name () + L"::gyear_month"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %d-%u") << "," << endl
               << arg_ << ".year ()," << endl
               << arg_ << ".month ());";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << endl
               << " << " << arg_ << ".year () << '-'" << endl
               << " << " << arg_ << ".month ();";

          gen_time_zone ();
        }
        else
          gen_user_type ();
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Time& t)
      {
        if (default_type (t, xs_ns_name () + L"::time"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %u:%u:%g") << "," << endl
               << arg_ << ".hours ()," << endl
               << arg_ << ".minutes ()," << endl
               << arg_ << ".seconds ());";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << endl
               << " << " << arg_ << ".hours () << ':'" << endl
               << " << " << arg_ << ".minutes () << ':'" << endl
               << " << " << arg_ << ".seconds ();";

          gen_time_zone ();
        }
        else
          gen_user_type ();
      }

    private:
      bool
      default_type (SemanticGraph::Type& t, String const& def_type)
      {
        return ret_type (t) == def_type;
      }

      void
      gen_user_type ()
      {
        os << "// TODO" << endl
           << "//" << endl;
      }

      void
      gen_string (SemanticGraph::Type& t)
      {
        if (options.no_stl ())
        {
          if (default_type (t, "char*"))
          {
            if (options.no_iostream ())
              os << "printf (" << strlit (tag_ + L": %s\n") << ", " <<
                arg_ << ");";
            else
              os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
                arg_ << " << std::endl;";
          }
          else
            gen_user_type ();
        }
        else
        {
          if (default_type (t, "::std::string"))
          {
            if (options.no_iostream ())
              os << "printf (" << strlit (tag_ + L": %s\n") << ", " <<
                arg_ << ".c_str ());";
            else
              os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
                arg_ << " << std::endl;";
          }
          else
            gen_user_type ();
        }
      }

      void
      gen_sequence (SemanticGraph::Type& t)
      {
        String type (xs_ns_name () + L"::string_sequence");

        if (default_type (t, type + L"*"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": ") << ");"
               << endl;
          else
            os << "std::cout << " << strlit (tag_ + L": ") << ";"
               << endl;

          os << "for (" << type << "::const_iterator i (" << arg_ <<
            "->begin ()), e (" << arg_ << "->end ());" << endl
             << "i != e;)"
             << "{";

          if (options.no_iostream ())
          {
            if (options.no_stl ())
              os << "printf (\"%s\", *i++);";
            else
              os << "printf (\"%s\", (i++)->c_str ());";

            os << "if (i != e)" << endl
               << "printf (\" \");"
               << "}"
               << "printf (\"\\n\");";
          }
          else
            os << "std::cout << *i++;"
               << "if (i != e)" << endl
               << "std::cout << ' ';"
               << "}"
               << "std::cout << std::endl;";
        }
        else
          gen_user_type ();
      }

      void
      gen_buffer (SemanticGraph::Type& t)
      {
        if (default_type (t, xs_ns_name () + L"::buffer*"))
        {
          if (options.no_iostream ())
            os << "printf (" << strlit (tag_ + L": %zu bytes\n") << ", " <<
              arg_ << "->size ());";
          else
            os << "std::cout << " << strlit (tag_ + L": ") << " << " <<
              arg_ << "->size () << \" bytes\" << std::endl;";
        }
        else
          gen_user_type ();
      }

      void
      gen_time_zone ()
      {
        os << endl
           << "if (" << arg_ << ".zone_present ())"
           << "{";

        if (options.no_iostream ())
          os << "if (" << arg_ << ".zone_hours () < 0)" << endl
             << "printf (\"%d:%d\", " << arg_ << ".zone_hours (), -" <<
            arg_ << ".zone_minutes ());"
             << "else" << endl
             << "printf (\"+%d:%d\", " << arg_ << ".zone_hours (), " <<
            arg_ << ".zone_minutes ());";
        else
          os << "if (" << arg_ << ".zone_hours () < 0)" << endl
             << "std::cout << " << arg_ << ".zone_hours () << ':' << -" <<
            arg_ << ".zone_minutes ();"
             << "else" << endl
             << "std::cout << '+' << " << arg_ << ".zone_hours () << " <<
            "':' << " << arg_ << ".zone_minutes ();";

        os << "}";

        if (options.no_iostream ())
          os << "printf (\"\\n\");";
        else
          os << "std::cout << std::endl;";
      }

    private:
      String tag_;
      String arg_;
    };

    struct DeleteCall: Traversal::AnySimpleType,

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

                       Context
    {
      DeleteCall (Context& c, String const& arg)
          : Context (c), arg_ (arg)
      {
      }

      virtual void
      traverse (SemanticGraph::AnySimpleType& t)
      {
        gen_string (t);
      }

      // Strings.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::String& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NormalizedString& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Token& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NameToken& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Name& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NCName& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Language& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Id& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::IdRef& t)
      {
        gen_string (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::AnyURI& t)
      {
        gen_string (t);
      }

      // String sequences.
      //

      virtual void
      traverse (SemanticGraph::Fundamental::NameTokens& t)
      {
        gen_sequence (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::IdRefs& t)
      {
        gen_sequence (t);
      }

      // QName
      //

      virtual void
      traverse (SemanticGraph::Fundamental::QName& t)
      {
        if (options.no_stl () &&
            default_type (t, xs_ns_name () + L"::qname*"))
        {
          os << endl;

          if (!custom_alloc)
            os << "delete " << arg_ << ";";
          else
            os << arg_ << "->~qname ();"
               << xs_ns_name () << "::free (" << arg_ << ");";
        }
      }

      // Binary.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Base64Binary& t)
      {
        gen_buffer (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::HexBinary& t)
      {
        gen_buffer (t);
      }

    private:
      bool
      default_type (SemanticGraph::Type& t, String const& def_type)
      {
        return ret_type (t) == def_type;
      }

      void
      gen_string (SemanticGraph::Type& t)
      {
        if (options.no_stl () && default_type (t, "char*"))
        {
          os << endl;

          if (!custom_alloc)
            os << "delete[] " << arg_ << ";";
          else
            os << xs_ns_name () << "::free (" << arg_ << ");";
        }
      }

      void
      gen_sequence (SemanticGraph::Type& t)
      {
        if (default_type (t, xs_ns_name () + L"::string_sequence*"))
        {
          os << endl;

          if (!custom_alloc)
            os << "delete " << arg_ << ";";
          else
            os << arg_ << "->~string_sequence ();"
               << xs_ns_name () << "::free (" << arg_ << ");";
        }
      }

      void
      gen_buffer (SemanticGraph::Type& t)
      {
        if (default_type (t, xs_ns_name () + L"::buffer*"))
        {
          os << endl;

          if (!custom_alloc)
            os << "delete " << arg_ << ";";
          else
            os << arg_ << "->~buffer ();"
               << xs_ns_name () << "::free (" << arg_ << ");";
        }
      }

    private:
      String arg_;
    };
  }
}

#endif // CXX_PARSER_PRINT_IMPL_COMMON_HXX
