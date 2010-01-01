// file      : xsde/cxx/hybrid/default-value.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/hybrid/default-value.hxx>

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      Void
      normalize (String& s)
      {
        Size n (s.size ());

        for (Size i (0); i < n; ++i)
        {
          WideChar& c (s[i]);

          if (c == 0x0D || // carriage return
              c == 0x09 || // tab
              c == 0x0A)
            c = 0x20;
        }
      }

      Void
      collapse (String& s)
      {
        Size n (s.size ()), j (0);
        Boolean subs (false), trim (true);

        for (Size i (0); i < n; ++i)
        {
          WideChar c (s[i]);

          if (c == 0x20 || c == 0x09 || c == 0x0A)
            subs = true;
          else
          {
            if (subs)
            {
              subs = false;

              if (!trim)
                s[j++] = 0x20;
            }

            if (trim)
              trim = false;

            s[j++] = c;
          }
        }

        s.resize (j);
      }

      Void
      strip_zeros (String& s)
      {
        Size n (s.size ()), i (0);

        if (n > 0 && (s[i] == '-' || s[i] == '+'))
          i++;

        Size j (i);

        Boolean strip (true);

        for (; i < n; ++i)
        {
          WideChar c (s[i]);

          if (c == '0')
          {
            if (!strip)
              s[j++] = c;
          }
          else
          {
            s[j++] = c;

            if (strip)
              strip = false;
          }
        }

        if (strip && j < n)
          s[j++] = '0'; // There was nothing except zeros so add one back.

        s.resize (j);
      }

      Void
      make_float (String& s)
      {
        if (s.find ('.') == String::npos &&
            s.find ('e') == String::npos &&
            s.find ('E') == String::npos)
          s += L".0";
      }

    }

    //
    // LiteralValue
    //
    LiteralValue::
    LiteralValue (Context& c)
        : Context (c), str_ (!stl)
    {
    }

    LiteralValue::
    LiteralValue (Context& c, Boolean str)
        : Context (c), str_ (str)
    {
    }

    String LiteralValue::
    dispatch (SemanticGraph::Node& type, String const& value)
    {
      literal_.clear ();
      value_ = value;
      Traversal::NodeBase::dispatch (type);
      return literal_;
    }

    // Boolean.
    //
    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::Boolean&)
    {
      collapse (value_);
      literal_ = (value_ == L"true" || value_ == L"1") ? "true" : "false";
    }

    // Integral types.
    //
    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::Byte&)
    {
      collapse (value_);
      strip_zeros (value_);
      literal_ = value_;
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::UnsignedByte&)
    {
      collapse (value_);
      strip_zeros (value_);
      literal_ = value_;
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::Short&)
    {
      collapse (value_);
      strip_zeros (value_);
      literal_ = value_;
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::UnsignedShort&)
    {
      collapse (value_);
      strip_zeros (value_);
      literal_ = value_;
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::Int&)
    {
      collapse (value_);
      strip_zeros (value_);
      literal_ = value_;
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::UnsignedInt&)
    {
      collapse (value_);
      strip_zeros (value_);
      literal_ = value_;
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::Long&)
    {
      collapse (value_);
      strip_zeros (value_);
      literal_ = value_;
      literal_ += options.value<CLI::no_long_long> () ? L"L" : L"LL";
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::UnsignedLong&)
    {
      collapse (value_);
      strip_zeros (value_);
      literal_ = value_;
      literal_ += options.value<CLI::no_long_long> () ? L"UL" : L"ULL";
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::Integer&)
    {
      collapse (value_);
      strip_zeros (value_);
      literal_ = value_ + L"L";
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::NonPositiveInteger&)
    {
      collapse (value_);
      strip_zeros (value_);
      literal_ = value_ + L"L";
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::NonNegativeInteger&)
    {
      collapse (value_);
      strip_zeros (value_);
      literal_ = value_ + L"UL";
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::PositiveInteger&)
    {
      collapse (value_);
      strip_zeros (value_);
      literal_ = value_ + L"UL";
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::NegativeInteger&)
    {
      collapse (value_);
      strip_zeros (value_);
      literal_ = value_ + L"L";
    }

    // Floats.
    //
    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::Float&)
    {
      collapse (value_);

      if (value_ == L"NaN")
      {
        literal_ = "static_cast< float > (strtod (\"NAN\", 0))";
      }
      else if (value_ == L"INF")
      {
        literal_ = "static_cast< float > (strtod (\"INF\", 0))";
      }
      else if (value_ == L"-INF")
      {
        literal_ = "static_cast< float > (strtod (\"-INF\", 0))";
      }
      else
      {
        strip_zeros (value_);
        make_float (value_);
        literal_ = value_ + L"F";
      }
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::Double&)
    {
      collapse (value_);

      if (value_ == L"NaN")
      {
        literal_ = "strtod (\"NAN\", 0)";
      }
      else if (value_ == L"INF")
      {
        literal_ = "strtod (\"INF\", 0)";
      }
      else if (value_ == L"-INF")
      {
        literal_ = "strtod (\"-INF\", 0)";
      }
      else
      {
        strip_zeros (value_);
        make_float (value_);
        literal_ = value_;
      }
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::Decimal&)
    {
      collapse (value_);
      strip_zeros (value_);
      make_float (value_);
      literal_ = value_;
    }

    // Strings.
    //
    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::String&)
    {
      if (str_)
        literal_ = strlit (value_);
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::NormalizedString&)
    {
      if (str_)
      {
        normalize (value_);
        literal_ = strlit (value_);
      }
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::Token&)
    {
      if (str_)
      {
        collapse (value_);
        literal_ = strlit (value_);
      }
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::NameToken&)
    {
      if (str_)
      {
        collapse (value_);
        literal_ = strlit (value_);
      }
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::Name&)
    {
      if (str_)
      {
        collapse (value_);
        literal_ = strlit (value_);
      }
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::NCName&)
    {
      if (str_)
      {
        collapse (value_);
        literal_ = strlit (value_);
      }
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::Language&)
    {
      if (str_)
      {
        collapse (value_);
        literal_ = strlit (value_);
      }
    }


    // ID/IDREF.
    //
    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::Id&)
    {
      if (str_)
      {
        collapse (value_);
        literal_ = strlit (value_);
      }
    }

    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::IdRef&)
    {
      if (str_)
      {
        collapse (value_);
        literal_ = strlit (value_);
      }
    }

    // URI.
    //
    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::AnyURI&)
    {
      if (str_)
      {
        collapse (value_);
        literal_ = strlit (value_);
      }
    }

    // Entity.
    //
    Void LiteralValue::
    traverse (SemanticGraph::Fundamental::Entity&)
    {
      if (str_)
      {
        collapse (value_);
        literal_ = strlit (value_);
      }
    }

    //
    // InitValue
    //

    InitValue::
    InitValue (Context& c)
        : Context (c),
          member_ ("this->value_."),
          var_ (c, TypeName::var),
          var_value_ (c, TypeName::var_value),
          literal_value_ (c, true),
          literal_value_list_ (c)
    {
    }

    Void InitValue::
    dispatch (SemanticGraph::Node& type, String const& value)
    {
      value_ = value;
      Traversal::NodeBase::dispatch (type);
    }

    Void InitValue::
    traverse (SemanticGraph::List& l)
    {
      SemanticGraph::Type& t (l.argumented ().type ());
      Boolean fl (fixed_length (t));

      collapse (value_);

      if (!value_)
        return;

      String ov (value_);
      String om (member_);

      Size b (0);

      for (Size e (ov.find (' ')); ; e = ov.find (' ', b))
      {
        String v (ov, b, e != String::npos ? e - b : e);

        os << "{";
        var_.dispatch (t);
        os << " tmp;";

        String literal (literal_value_list_.dispatch (t, v));

        if (literal)
        {
          // Variable-length literal is a string.
          //
          if (fl)
            os << "tmp = " << literal << ";";
          else
          {
            os << "tmp = ::xsde::cxx::strdupx (" << literal << ");";

            if (!exceptions)
            {
              os << endl
                 << "if (!tmp)"
                 << "{"
                 << "assert (false);"
                 << "exit (1);"
                 << "}";
            }
          }
        }
        else
        {
          if (fl)
            member_ = "tmp.";
          else
          {
            os << "tmp = new " << fq_name (t) << ";";

            if (!exceptions)
            {
              os << endl
                 << "if (!tmp)"
                 << "{"
                 << "assert (false);"
                 << "exit (1);"
                 << "}";
            }

            member_ = "tmp->";
          }

          value_ = v;
          Traversal::NodeBase::dispatch (t);
        }

        if (exceptions)
          os << om << "push_back (tmp);";
        else
          os << "if (" << om << "push_back (tmp))"
             << "{"
             << "assert (false);"
             << "exit (1);"
             << "}";

        os << "}";

        if (e == String::npos)
          break;

        b = e + 1;
      }

      member_ = om;
      value_ = ov;
    }

    Void InitValue::
    traverse (SemanticGraph::Union& u)
    {
      String const& value (u.context ().get<String> ("value"));

      if (stl)
        os << member_ << value << " (" << strlit (value_) << ");";
      else
      {
        if (exceptions)
          os << member_ << value << " (";
        else
          os << "char* v = ";

        os << "::xsde::cxx::strdupx (" << strlit (value_) << ")";

        if (exceptions)
          os << ");";
        else
        {
          os << ";"
             << endl
             << "if (!v)"
             << "{"
             << "assert (false);"
             << "exit (1);"
             << "}"
             << member_ << value << " (v);";
        }
      }
    }

    Void InitValue::
    traverse (SemanticGraph::Complex& c)
    {
      Traversal::NodeBase::dispatch (ultimate_base (c));
    }

    Void InitValue::
    traverse (SemanticGraph::Enumeration& e)
    {
      using SemanticGraph::Enumerator;
      using SemanticGraph::Enumeration;

      // First see if we should delegate this one to the Complex
      // generator.
      //
      Enumeration* base_enum (0);

      if (!enum_ || !enum_mapping (e, &base_enum))
      {
        traverse (static_cast<SemanticGraph::Complex&> (e));
        return;
      }

      Enumeration& x (base_enum ? *base_enum : e);

      os << member_ << x.context ().get<String> ("value") << "(";

      Enumeration::NamesIteratorPair ip (x.find (value_));

      if (ip.first != ip.second)
      {
        Enumerator& er (dynamic_cast<Enumerator&> (ip.first->named ()));
        os << fq_name (e) << "::" << ename (er);
      }

      os << ");";
    }

    Void InitValue::
    traverse (SemanticGraph::Type& t)
    {
      // This is a fall-back case where we handle all other (literal)
      // types.
      //
      os << member_ << "base_value (" <<
        literal_value_.dispatch (t, value_) << ");";
    }

    // anySimpleType.
    //
    Void InitValue::
    traverse (SemanticGraph::AnySimpleType&)
    {
    }

    // Strings.
    //
    Void InitValue::
    traverse (SemanticGraph::Fundamental::String& t)
    {
      string_type (t);
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::NormalizedString& t)
    {
      string_type (t);
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::Token& t)
    {
      string_type (t);
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::NameToken& t)
    {
      string_type (t);
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::NameTokens&)
    {
      string_sequence_type ();
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::Name& t)
    {
      string_type (t);
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::NCName& t)
    {
      string_type (t);
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::Language& t)
    {
      string_type (t);
    }

    // Qualified name.
    //
    Void InitValue::
    traverse (SemanticGraph::Fundamental::QName&)
    {
      collapse (value_);

      String prefix, name;
      Size p (value_.find (':'));

      if (p != String::npos)
      {
        prefix.assign (value_, 0, p);
        name.assign (value_, p + 1, String::npos);
      }
      else
        name = value_;

      if (stl)
      {
        os << member_ << "prefix (" << strlit (prefix) << ");"
           << member_ << "name (" << strlit (name) << ");";
      }
      else
      {
        if (exceptions)
          os << member_ << "prefix_copy (" << strlit (prefix) << ");"
             << member_ << "name_copy (" << strlit (name) << ");";
        else
          os << "if (" << member_ << "prefix_copy (" << strlit (prefix) <<
            ") || " << member_ << "name_copy (" << strlit (name) << "))"
             << "{"
             << "assert (false);"
             << "exit (1);"
             << "}";
      }
    }

    // ID/IDREF.
    //
    Void InitValue::
    traverse (SemanticGraph::Fundamental::Id& t)
    {
      string_type (t);
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::IdRef& t)
    {
      string_type (t);
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::IdRefs&)
    {
      string_sequence_type ();
    }

    // URI.
    //
    Void InitValue::
    traverse (SemanticGraph::Fundamental::AnyURI& t)
    {
      string_type (t);
    }

    // Binary.
    //
    Void InitValue::
    traverse (SemanticGraph::Fundamental::Base64Binary&)
    {
      collapse (value_);

      if (value_)
        os << "#error base64Binary default values are not yet supported"
           << endl;
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::HexBinary&)
    {
      collapse (value_);

      if (value_)
        os << "#error hexBinary default values are not yet supported"
           << endl;
    }


    // Date/time.
    //
    Void InitValue::
    traverse (SemanticGraph::Fundamental::Date&)
    {
      // date := [-]CCYY[N]*-MM-DD[Z|(+|-)HH:MM]
      //
      collapse (value_);

      Size b (0);
      Size e (value_.find ('-', value_[0] == '-' ? 5 : 4));
      String year (value_, 0, e);

      b = e + 1;
      String month (value_, b, 2);

      b += 3;
      String day (value_, b, 2);

      strip_zeros (year);
      strip_zeros (month);
      strip_zeros (day);

      os << member_ << "year (" << year << ");"
         << member_ << "month (" << month << ");"
         << member_ << "day (" << day << ");";

      time_zone (b + 2);
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::DateTime&)
    {
      // date_time := [-]CCYY[N]*-MM-DDTHH:MM:SS[.S+][Z|(+|-)HH:MM]
      //
      collapse (value_);

      Size b (0);
      Size e (value_.find ('-', value_[0] == '-' ? 5 : 4));
      String year (value_, 0, e);
      b = e + 1;

      String month (value_, b, 2);
      b += 3;

      String day (value_, b, 2);
      b += 3;

      String hours (value_, b, 2);
      b += 3;

      String minutes (value_, b, 2);
      b += 3;

      e = b + 2;
      for (; e < value_.size (); ++e)
      {
        WideChar c (value_[e]);

        if (c == 'Z' || c == '+' || c == '-')
          break;
      }

      String seconds (value_, b, e - b);

      strip_zeros (year);
      strip_zeros (month);
      strip_zeros (day);
      strip_zeros (hours);
      strip_zeros (minutes);
      strip_zeros (seconds);
      make_float (seconds);

      os << member_ << "year (" << year << ");"
         << member_ << "month (" << month << ");"
         << member_ << "day (" << day << ");"
         << member_ << "hours (" << hours << ");"
         << member_ << "minutes (" << minutes << ");"
         << member_ << "seconds (" << seconds << ");";

      time_zone (e);
    }

    namespace
    {
      Size
      find_delim (String const& s, Size pos)
      {
        for (; pos < s.size (); ++pos)
        {
          WideChar c (s[pos]);

          if (c == 'Y' || c == 'D' || c == 'M' || c == 'H' ||
              c == 'M' || c == 'S' || c == 'T')
            break;
        }

        return pos;
      }
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::Duration&)
    {
      // duration := [-]P[nY][nM][nD][TnHnMn[.n+]S]
      //
      collapse (value_);

      Size b (1), e, n (value_.size ());

      if (value_[0] == '-')
      {
        os << member_ << "negative (true);";
        b++;
      }

      e = find_delim (value_, b);

      if (e < n && value_[e] == 'Y')
      {
        String v (value_, b, e - b);
        strip_zeros (v);
        os << member_ << "years (" << v << ");";

        b = e + 1;
        e = find_delim (value_, b);
      }

      if (e < n && value_[e] == 'M')
      {
        String v (value_, b, e - b);
        strip_zeros (v);
        os << member_ << "months (" << v << ");";

        b = e + 1;
        e = find_delim (value_, b);
      }

      if (e < n && value_[e] == 'D')
      {
        String v (value_, b, e - b);
        strip_zeros (v);
        os << member_ << "days (" << v << ");";

        b = e + 1;
        e = find_delim (value_, b);
      }

      if (e < n && value_[e] == 'T')
      {
        b = e + 1;
        e = find_delim (value_, b);
      }

      if (e < n && value_[e] == 'H')
      {
        String v (value_, b, e - b);
        strip_zeros (v);
        os << member_ << "hours (" << v << ");";

        b = e + 1;
        e = find_delim (value_, b);
      }

      if (e < n && value_[e] == 'M')
      {
        String v (value_, b, e - b);
        strip_zeros (v);
        os << member_ << "minutes (" << v << ");";

        b = e + 1;
        e = find_delim (value_, b);
      }

      if (e < n && value_[e] == 'S')
      {
        String v (value_, b, e - b);
        strip_zeros (v);
        make_float (v);
        os << member_ << "seconds (" << v << ");";

        b = e + 1;
        e = find_delim (value_, b);
      }
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::Day&)
    {
      // gday := ---DD[Z|(+|-)HH:MM]
      //
      collapse (value_);

      String day (value_, 3, 2);
      strip_zeros (day);

      os << member_ << "day (" << day << ");";

      time_zone (5);
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::Month&)
    {
      // gmonth := --MM[Z|(+|-)HH:MM]
      //
      collapse (value_);

      String month (value_, 2, 2);
      strip_zeros (month);

      os << member_ << "month (" << month << ");";

      time_zone (4);
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::MonthDay&)
    {
      // gmonth_day := --MM-DD[Z|(+|-)HH:MM]
      //
      collapse (value_);

      String month (value_, 2, 2);
      String day (value_, 5, 2);

      strip_zeros (month);
      strip_zeros (day);

      os << member_ << "month (" << month << ");";
      os << member_ << "day (" << day << ");";

      time_zone (7);
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::Year&)
    {
      // gyear := [-]CCYY[N]*[Z|(+|-)HH:MM]
      //
      collapse (value_);

      Size pos (value_[0] == '-' ? 5 : 4);
      for (; pos < value_.size (); ++pos)
      {
        WideChar c (value_[pos]);

        if (c == 'Z' || c == '+' || c == '-')
          break;
      }

      String year (value_, 0, pos);
      strip_zeros (year);

      os << member_ << "year (" << year << ");";

      time_zone (pos);
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::YearMonth&)
    {
      // gyear_month := [-]CCYY[N]*-MM[Z|(+|-)HH:MM]
      //
      collapse (value_);

      Size pos (value_.find ('-', value_[0] == '-' ? 5 : 4));

      String year (value_, 0, pos);
      String month (value_, pos + 1, 2);

      strip_zeros (year);
      strip_zeros (month);

      os << member_ << "year (" << year << ");";
      os << member_ << "month (" << month << ");";

      time_zone (pos + 3);
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::Time&)
    {
      // time := HH:MM:SS[.S+][Z|(+|-)HH:MM]
      //
      collapse (value_);

      String hours (value_, 0, 2);
      String minutes (value_, 3, 2);

      Size e (8);
      for (; e < value_.size (); ++e)
      {
        WideChar c (value_[e]);

        if (c == 'Z' || c == '+' || c == '-')
          break;
      }

      String seconds (value_, 6, e - 6);

      strip_zeros (hours);
      strip_zeros (minutes);
      strip_zeros (seconds);
      make_float (seconds);

      os << member_ << "hours (" << hours << ");"
         << member_ << "minutes (" << minutes << ");"
         << member_ << "seconds (" << seconds << ");";

      time_zone (e);
    }

    Void InitValue::
    time_zone (Size pos)
    {
      // time_zone := Z|(+|-)HH:MM
      //
      if (pos < value_.size ())
      {
        String h, m;

        if (value_[pos] == 'Z')
        {
          h = "0";
          m = "0";
        }
        else
        {
          if (value_[pos] == '-')
          {
            h = "-";
            m = "-";
          }

          h.append (value_, pos + 1, 2);
          m.append (value_, pos + 4, 2);

          strip_zeros (h);
          strip_zeros (m);
        }

        os << member_ << "zone_hours (" << h << ");"
           << member_ << "zone_minutes (" << m << ");";
      }
    }

    // Entity.
    //
    Void InitValue::
    traverse (SemanticGraph::Fundamental::Entity& t)
    {
      string_type (t);
    }

    Void InitValue::
    traverse (SemanticGraph::Fundamental::Entities&)
    {
      string_sequence_type ();
    }

    Void InitValue::
    string_type (SemanticGraph::Fundamental::Type& t)
    {
      // In case STL is disabled, strings are returned as literals
      // so we end up here only if the type is derived from a string.
      // Otherwise, use assign() which handles both derivations as
      // well as the straight std::string value.
      //
      if (stl)
        os << member_ << "assign (" << literal_value_.dispatch (t, value_) <<
          ");";
      else
      {
        if (exceptions)
          os << member_ << "base_value (";
        else
          os << "char* v = ";

        os << "::xsde::cxx::strdupx (" <<
          literal_value_.dispatch (t, value_) << ")";

        if (exceptions)
          os << ");";
        else
        {
          os << ";"
             << endl
             << "if (!v)"
             << "{"
             << "assert (false);"
             << "exit (1);"
             << "}"
             << member_ << "base_value (v);";
        }
      }
    }

    Void InitValue::
    string_sequence_type ()
    {
      collapse (value_);

      if (!value_)
        return;

      Size b (0);

      for (Size e (value_.find (' ')); ; e = value_.find (' ', b))
      {
        String v (value_, b, e != String::npos ? e - b : e);

        os << "{";

        if (stl)
          os << "::std::string tmp (";
        else
          os << "char* tmp = ::xsde::cxx::strdupx (";

        os << strlit (v) << ");";

        if (!exceptions && !stl)
        {
          os << endl
             << "if (!tmp)"
             << "{"
             << "assert (false);"
             << "exit (1);"
             << "}";
        }

        if (exceptions)
          os << member_ << "push_back (tmp);";
        else
          os << "if (" << member_ << "push_back (tmp))"
             << "{"
             << "assert (false);"
             << "exit (1);"
             << "}";

        os << "}";

        if (e == String::npos)
          break;

        b = e + 1;
      }
    }

    //
    // CompareValue
    //

    CompareValue::
    CompareValue (Context& c)
        : Context (c)
    {
    }

    Void CompareValue::
    dispatch (SemanticGraph::Node& type, String const& lhs, String const& rhs)
    {
      lhs_ = &lhs;
      rhs_ = &rhs;
      Traversal::NodeBase::dispatch (type);
    }

    Void CompareValue::
    traverse (SemanticGraph::Union& u)
    {
      String const& value (u.context ().get<String> ("value"));

      os << *lhs_ << "." << value << " () == " << *rhs_ << "." << value <<
        " ()";
    }

    Void CompareValue::
    traverse (SemanticGraph::Complex& c)
    {
      Traversal::NodeBase::dispatch (ultimate_base (c));
    }

    Void CompareValue::
    traverse (SemanticGraph::Enumeration& e)
    {
      // First see if we should delegate this one to the Complex
      // generator.
      //
      if (!enum_ || !enum_mapping (e))
      {
        traverse (static_cast<SemanticGraph::Complex&> (e));
        return;
      }

      os << *lhs_ << " == " << *rhs_;
    }

    Void CompareValue::
    traverse (SemanticGraph::Type&)
    {
      // This is a fall-back case where we handle all other types.
      //
      os << *lhs_ << " == " << *rhs_;
    }
  }
}
