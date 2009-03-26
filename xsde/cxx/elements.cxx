// file      : xsde/cxx/elements.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/elements.hxx>

#include <backend-elements/regex.hxx>

#include <sstream>
#include <iostream>
#include <algorithm>

using std::wcerr;
using std::endl;

namespace CXX
{
  //
  //
  wchar_t
  upcase (wchar_t c)
  {
    return std::toupper (c);
  }

  // Context
  //

  Context::
  Context (std::wostream& o,
           SemanticGraph::Schema& root,
           Char const* name_key,
           NarrowString const& char_type__,
           Boolean include_with_brackets__,
           NarrowString const& include_prefix__,
           NarrowString const& esymbol,
           Containers::Vector<NarrowString> const& nsm,
           Containers::Vector<NarrowString> const& nsr,
           Boolean trace_namespace_regex_,
           Containers::Vector<NarrowString> const& ir,
           Boolean trace_include_regex_,
           Boolean inline_,
           Containers::Vector<NarrowString> const& reserved_name)
      : os (o),
        schema_root (root),
        ename_key (ename_key_),
        char_type (char_type_),
        L (L_),
        string_type (string_type_),
        include_with_brackets (include_with_brackets_),
        include_prefix (include_prefix_),
        type_exp (type_exp_),
        inst_exp (inst_exp_),
        inl (inl_),
        ns_mapping_cache (ns_mapping_cache_),
        xs_ns_ (0),
        ename_key_ (name_key),
        char_type_ (char_type__),
        L_ (char_type == L"wchar_t" ? L"L" : L""),
        include_with_brackets_ (include_with_brackets__),
        include_prefix_ (include_prefix__),
        type_exp_ (esymbol ? esymbol + " " : esymbol),
        inst_exp_ (esymbol ? esymbol + "\n" : esymbol),
        inl_ (inline_ ? L"inline\n" : L""),
        cxx_id_expr_ (L"^(::)?([a-zA-Z_]\\w*)(::[a-zA-Z_]\\w*)*$"),
        cxx_id_expr (cxx_id_expr_),
        trace_namespace_regex (trace_namespace_regex_),
        nsr_mapping (nsr_mapping_),
        nsm_mapping (nsm_mapping_),
        include_mapping (include_mapping_),
        trace_include_regex (trace_include_regex_),
        reserved_name_map (reserved_name_map_)
  {
    // Resolve and cache XML Schema namespace.
    //
    {
      SemanticGraph::Nameable* n;

      if (schema_root.names_begin ()->name () ==
          L"http://www.w3.org/2001/XMLSchema")
      {
        // schema_root is the XML Schema itself.
        //
        n = &schema_root.names_begin ()->named ();
      }
      else
      {
        // Otherwise, the first used schema is implied XML Schema.
        //
        SemanticGraph::Uses& u = *schema_root.uses_begin ();
        assert (u.is_a<SemanticGraph::Implies> ());
        n = &u.schema ().names_begin ()->named ();
      }

      xs_ns_ = dynamic_cast<SemanticGraph::Namespace*> (n);
    }

    //
    //
    if (char_type == L"char")
      string_type_ = L"::std::string";
    else if (char_type == L"wchar_t")
      string_type_ = L"::std::wstring";
    else
      string_type_ = L"::std::basic_string< " + char_type + L" >";

    // Default mapping.
    //
    nsr_mapping_.push_back (
      Regex (L"#^.* (.*?/)??"L"(([a-zA-Z_]\\w*)(/[a-zA-Z_]\\w*)*)/?$#$2#"));
    nsr_mapping_.push_back (
      Regex (L"#^.* http://www\\.w3\\.org/2001/XMLSchema$#xml_schema#"));

    // Custom regex mapping.
    //
    for (Containers::Vector<NarrowString>::ConstIterator
           i (nsr.begin ()), e (nsr.end ()); i != e; ++i)
    {
      nsr_mapping_.push_back (Regex (*i));
    }

    // Custom direct mapping.
    //
    for (Containers::Vector<NarrowString>::ConstIterator
           i (nsm.begin ()), e (nsm.end ()); i != e; ++i)
    {
      String s (*i);

      // Split the string in two parts at the last '='.
      //
      Size pos (s.rfind ('='));

      if (pos == String::npos)
        throw InvalidNamespaceMapping (s, "delimiter ('=') not found");

      // Empty xml_ns designates the no-namespace case.
      //
      String xml_ns (s, 0, pos);
      String cxx_ns (s, pos + 1);

      if (!cxx_ns.empty () && !cxx_id_expr.match (cxx_ns))
        throw InvalidNamespaceMapping (s, "invalid C++ identifier");

      nsm_mapping_[xml_ns] = cxx_ns;
    }

    // Include path regex
    //
    for (Containers::Vector<NarrowString>::ConstIterator
           i (ir.begin ()), e (ir.end ()); i != e; ++i)
    {
      include_mapping_.push_back (Regex (*i));
    }

    // Reserved names.
    //
    for (Containers::Vector<NarrowString>::ConstIterator
           i (reserved_name.begin ()), e (reserved_name.end ()); i != e; ++i)
    {
      String s (*i);

      // Split the string in two parts at '='.
      //
      Size pos (s.find ('='));

      if (pos == String::npos)
        reserved_name_map_[s] = L"";
      else
        reserved_name_map_[String (s, 0, pos)] = String (s, pos + 1);
    }
  }

  String Context::
  ns_name (SemanticGraph::Namespace& ns) const
  {
    using SemanticGraph::Schema;
    using SemanticGraph::Includes;
    using SemanticGraph::Imports;
    using SemanticGraph::Implies;
    using SemanticGraph::Sources;

    String tmp;
    MapMapping::ConstIterator i (nsm_mapping.find (ns.name ()));

    if (i != nsm_mapping.end ())
    {
      tmp = i->second;
    }
    else
    {
      SemanticGraph::Path path;
      Schema& schema (dynamic_cast<Schema&> (ns.scope ()));

      if (schema.used ())
      {
        // Here we need to detect a special multi-schema compilation
        // case where the root schemas are imported into a special
        // schema that doesn't have a namespace.
        //
        SemanticGraph::Uses& u (*schema.used_begin ());
        SemanticGraph::Schema& s (u.user ());

        if (s.names_begin () != s.names_end ())
          path = u.path ();
      }

      String pair;

      if (!path.empty ())
      {
        // Try to use the portable representation of the path. If that
        // fails, fall back to the native representation.
        //
        try
        {
          pair = path.string ();
        }
        catch (SemanticGraph::InvalidPath const&)
        {
          pair = path.native_file_string ();
        }
      }

      pair += L' ' + ns.name ();

      // Check cache first
      //
      MappingCache::ConstIterator i (ns_mapping_cache.find (pair));

      if (i != ns_mapping_cache.end ())
      {
        tmp = i->second;
      }
      else
      {
        if (trace_namespace_regex)
          wcerr << "namespace: '" << pair << "'" << endl;

        Boolean found (false);
        Regex colon (L"#/#::#");

        for (RegexMapping::ConstReverseIterator e (nsr_mapping.rbegin ());
             e != nsr_mapping.rend (); ++e)
        {
          if (trace_namespace_regex)
            wcerr << "try: '" << e->pattern () << "' : ";

          if (e->match (pair))
          {
            tmp = e->merge (pair);
            tmp = colon.merge (tmp); // replace `/' with `::'

            // Check the result.
            //
            found = cxx_id_expr.match (tmp);

            if (trace_namespace_regex)
              wcerr << "'" << tmp << "' : ";
          }

          if (trace_namespace_regex)
            wcerr << (found ? '+' : '-') << endl;

          if (found)
            break;
        }

        if (!found)
        {
          // Check if the name is valid by itself.
          //
          if (ns.name ().empty ())
          {
            // Empty name denotes a no-namespace case.
            //
            tmp = ns.name ();
          }
          else
          {
            tmp = colon.merge (ns.name ()); // replace `/' with `::'

            if (!cxx_id_expr.match (tmp))
            {
              throw NoNamespaceMapping (
                ns.file (), ns.line (), ns.column (), ns.name ());
            }
          }
        }

        // Add the mapping to the cache.
        //
        ns_mapping_cache[pair] = tmp;
      }
    }


    // Parse resulting namespace string and id() each name.
    //
    String r;
    String::size_type b (0), e;

    do
    {
      e = tmp.find (L"::", b);

      String name (tmp, b, e == tmp.npos ? e : e - b);

      if (!name.empty ())
        r += L"::" + escape (name);

      b = e;

      if (b == tmp.npos)
        break;

      b += 2;

    } while (true);

    return r;
  }

  SemanticGraph::Namespace& Context::
  xs_ns ()
  {
    return *xs_ns_;
  }

  String Context::
  xs_ns_name ()
  {
    return ns_name (*xs_ns_);
  }

  SemanticGraph::Namespace& Context::
  namespace_ (SemanticGraph::Nameable& n)
  {
    // The basic idea goes like this: go up Names edges until you
    // reach Namespace. There are, however, anonymous types which
    // need special handling. In the case of an anonymous type we
    // will go up the first Belongs edge (because the first edge
    // is where the type was defined.
    //

    if (n.named ())
    {
      SemanticGraph::Scope& s (n.scope ());

      SemanticGraph::Namespace* ns (
        dynamic_cast<SemanticGraph::Namespace*> (&n));

      return ns ? *ns : namespace_ (s);
    }
    else
    {
      SemanticGraph::Type& t (dynamic_cast<SemanticGraph::Type&> (n));

      SemanticGraph::Belongs& b (*t.classifies_begin ());

      return namespace_ (b.instance ());
    }
  }

  String Context::
  xml_ns_name (SemanticGraph::Nameable& n)
  {
    return namespace_ (n).name ();
  }

  String Context::
  fq_name (SemanticGraph::Nameable& n, Char const* name_key) const
  {
    using namespace SemanticGraph;

    String r;

    if (dynamic_cast<Schema*> (&n))
    {
      return L""; // Map to global namespace.
    }
    else if (SemanticGraph::Namespace* ns =
             dynamic_cast<SemanticGraph::Namespace*> (&n))
    {
      r = ns_name (*ns);
    }
    else
    {
      r = fq_name (n.scope ());
      r += L"::";
      r += n.context ().get<String> (
        name_key ? name_key : ename_key.c_str ());
    }

    return r;
  }

  SemanticGraph::Type& Context::
  ultimate_base (SemanticGraph::Complex& c)
  {
    using namespace SemanticGraph;

    Type* b (&c.inherits ().base ());

    while (true)
    {
      Complex* cb (dynamic_cast<Complex*> (b));

      if (cb != 0 && cb->inherits_p ())
      {
        b = &cb->inherits ().base ();
        continue;
      }

      break;
    }

    return *b;
  }

  // Restriction correspondance.
  //
  SemanticGraph::Element* Context::
  correspondent (SemanticGraph::Element& r)
  {
    SemanticGraph::Context& c (r.context ());

    if (c.count ("xsd-frontend-restriction-correspondence"))
    {
      return c.get<SemanticGraph::Element*> (
        "xsd-frontend-restriction-correspondence");
    }

    return 0;
  }

  SemanticGraph::Any* Context::
  correspondent (SemanticGraph::Any& r)
  {
    SemanticGraph::Context& c (r.context ());

    if (c.count ("xsd-frontend-restriction-correspondence"))
    {
      return c.get<SemanticGraph::Any*> (
        "xsd-frontend-restriction-correspondence");
    }

    return 0;
  }

  SemanticGraph::Compositor* Context::
  correspondent (SemanticGraph::Compositor& r)
  {
    SemanticGraph::Context& c (r.context ());

    if (c.count ("xsd-frontend-restriction-correspondence"))
    {
      return c.get<SemanticGraph::Compositor*> (
        "xsd-frontend-restriction-correspondence");
    }

    return 0;
  }


  //
  //
  namespace
  {
    WideChar const* keywords[] = {
      L"NULL",
      L"and",
      L"asm",
      L"auto",
      L"bitand",
      L"bitor",
      L"bool",
      L"break",
      L"case",
      L"catch",
      L"char",
      L"class",
      L"compl",
      L"const",
      L"const_cast",
      L"continue",
      L"default",
      L"delete",
      L"do",
      L"double",
      L"dynamic_cast",
      L"else",
      L"end_eq",
      L"enum",
      L"explicit",
      L"export",
      L"extern",
      L"false",
      L"float",
      L"for",
      L"friend",
      L"goto",
      L"if",
      L"inline",
      L"int",
      L"long",
      L"mutable",
      L"namespace",
      L"new",
      L"not",
      L"not_eq",
      L"operator",
      L"or",
      L"or_eq",
      L"private",
      L"protected",
      L"public",
      L"register",
      L"reinterpret_cast",
      L"return",
      L"short",
      L"signed",
      L"sizeof",
      L"static",
      L"static_cast",
      L"struct",
      L"switch",
      L"template",
      L"this",
      L"throw",
      L"true",
      L"try",
      L"typedef",
      L"typeid",
      L"typename",
      L"union",
      L"unsigned",
      L"using",
      L"virtual",
      L"void",
      L"volatile",
      L"wchar_t",
      L"while",
      L"xor",
      L"xor_eq"
    };
  }

  String Context::
  escape (String const& name) const
  {
    String r;
    Size n (name.size ());

    // In most common cases we will have that many chars.
    //
    r.reserve (n);

    for (Size i (0); i < n; ++i)
    {
      Boolean first (i == 0);

      UnsignedLong u (unicode_char (name, i)); // May advance i.

      if (first)
      {
        if (!((u >= 'a' && u <= 'z') ||
              (u >= 'A' && u <= 'Z') ||
              u == '_'))
          r = (u >= '0' && u <= '9') ? L"cxx_" : L"cxx";
      }

      if (!((u >= 'a' && u <= 'z') ||
            (u >= 'A' && u <= 'Z') ||
            (u >= '0' && u <= '9') ||
            u == '_'))
        r.push_back ('_');
      else
        r.push_back (static_cast<WideChar> (u));
    }

    if (r.empty ())
      r = L"cxx";

    // Custom reserved words.
    //
    ReservedNameMap::ConstIterator i (reserved_name_map.find (r));

    if (i != reserved_name_map.end ())
    {
      if (i->second)
        return i->second;
      else
        r += L'_';
    }

    // Keywords
    //
    Size const size (sizeof (keywords) / sizeof (WideChar*));

    if (std::binary_search (keywords, keywords + size, r))
    {
      r += L'_';

      // Re-run custom words.
      //
      i = reserved_name_map.find (r);

      if (i != reserved_name_map.end ())
      {
        if (i->second)
          return i->second;
        else
          r += L'_';
      }
    }

    return r;
  }

  // String escaping.
  //

  String
  charlit (UnsignedLong u)
  {
    String r ("\\x");
    Boolean lead (true);

    for (Long i (7); i >= 0; --i)
    {
      UnsignedLong x ((u >> (i * 4)) & 0x0F);

      if (lead)
      {
        if (x == 0)
          continue;

        lead = false;
      }

      r += x < 10 ? ('0' + x) : ('A' + x - 10);
    }

    return r;
  }

  const UnsignedLong utf8_first_char_mask[5] =
  {
    0x00, 0x00, 0xC0, 0xE0, 0xF0
  };

  String
  strlit_utf8 (String const& str)
  {
    String r;
    Size n (str.size ());

    // In most common cases we will have that many chars.
    //
    r.reserve (n + 2);

    r += '"';

    Boolean escape (false);

    for (Size i (0); i < n; ++i)
    {
      UnsignedLong u (Context::unicode_char (str, i)); // May advance i.

      // [128 - ]     - UTF-8
      // 127          - \x7F
      // [32  - 126]  - as is
      // [0   - 31]   - \X or \xXX
      //

      if (u < 32 || u == 127)
      {
        switch (u)
        {
        case L'\n':
          {
            r += L"\\n";
            break;
          }
        case L'\t':
          {
            r += L"\\t";
            break;
          }
        case L'\v':
          {
            r += L"\\v";
            break;
          }
        case L'\b':
          {
            r += L"\\b";
            break;
          }
        case L'\r':
          {
            r += L"\\r";
            break;
          }
        case L'\f':
          {
            r += L"\\f";
            break;
          }
        case L'\a':
          {
            r += L"\\a";
            break;
          }
        default:
          {
            r += charlit (u);
            escape = true;
            break;
          }
        }
      }
      else if (u < 127)
      {
        if (escape)
        {
          // Close and open the string so there are no clashes.
          //
          r += '"';
          r += '"';

          escape = false;
        }

        switch (u)
        {
        case L'"':
          {
            r += L"\\\"";
            break;
          }
        case L'\\':
          {
            r += L"\\\\";
            break;
          }
        default:
          {
            r += static_cast<WideChar> (u);
            break;
          }
        }
      }
      else
      {
        UnsignedLong count;
        UnsignedLong tmp[4];

        if (u < 0x800)
          count = 2;
        else if (u < 0x10000)
          count = 3;
        else if (u < 0x110000)
          count = 4;

        switch (count)
        {
        case 4:
          {
            tmp[3] = (u | 0x80UL) & 0xBFUL;
            u >>= 6;
          }
        case 3:
          {
            tmp[2] = (u | 0x80UL) & 0xBFUL;
            u >>= 6;
          }
        case 2:
          {
            tmp[1] = (u | 0x80UL) & 0xBFUL;
            u >>= 6;
          }
        case 1:
          {
            tmp[0] = u | utf8_first_char_mask[count];
          }
        }

        for (UnsignedLong j (0); j < count; ++j)
          r += charlit (tmp[j]);

        escape = true;
      }
    }

    r += '"';

    return r;
  }

  String
  strlit_utf32 (String const& str)
  {
    String r;
    Size n (str.size ());

    // In most common cases we will have that many chars.
    //
    r.reserve (n + 2);

    r += '"';

    Boolean escape (false);

    for (Size i (0); i < n; ++i)
    {
      UnsignedLong u (Context::unicode_char (str, i)); // May advance i.

      // [128 - ]     - \xUUUUUUUU
      // 127          - \x7F
      // [32  - 126]  - as is
      // [0   - 31]   - \X or \xXX
      //

      if (u < 32 || u == 127)
      {
        switch (u)
        {
        case L'\n':
          {
            r += L"\\n";
            break;
          }
        case L'\t':
          {
            r += L"\\t";
            break;
          }
        case L'\v':
          {
            r += L"\\v";
            break;
          }
        case L'\b':
          {
            r += L"\\b";
            break;
          }
        case L'\r':
          {
            r += L"\\r";
            break;
          }
        case L'\f':
          {
            r += L"\\f";
            break;
          }
        case L'\a':
          {
            r += L"\\a";
            break;
          }
        default:
          {
            r += charlit (u);
            escape = true;
            break;
          }
        }
      }
      else if (u < 127)
      {
        if (escape)
        {
          // Close and open the string so there are no clashes.
          //
          r += '"';
          r += '"';

          escape = false;
        }

        switch (u)
        {
        case L'"':
          {
            r += L"\\\"";
            break;
          }
        case L'\\':
          {
            r += L"\\\\";
            break;
          }
        default:
          {
            r += static_cast<WideChar> (u);
            break;
          }
        }
      }
      else
      {
        r += charlit (u);
        escape = true;
      }
    }

    r += '"';

    return r;
  }

  String Context::
  strlit (String const& str)
  {
    if (char_type == L"char")
      return strlit_utf8 (str);
    else
      return strlit_utf32 (str);
  }

  String Context::
  comment (String const& str)
  {
    String r;

    WideChar const* s (str.c_str ());
    Size size (str.size ());

    // In most common cases we will have that many chars.
    //
    r.reserve (size);

    for (WideChar const* p (s); p < s + size; ++p)
    {
      UnsignedLong u (unicode_char (p)); // May advance p.

      // We are going to treat \v, \f and \n as unrepresentable
      // here even though they can be present in C++ source code.
      //
      if (u > 127 || (u < 32 && u != '\t'))
        r += L'?';
      else
        r += static_cast<WideChar> (u);
    }

    return r;
  }

  String Context::
  process_include_path (String const& name) const
  {
    String path (include_prefix + name);

    if (trace_include_regex)
      wcerr << "include: '" << path << "'" << endl;

    String r;
    Boolean found (false);

    for (RegexMapping::ConstReverseIterator e (include_mapping.rbegin ());
         e != include_mapping.rend (); ++e)
    {
      if (trace_include_regex)
        wcerr << "try: '" << e->pattern () << "' : ";

      if (e->match (path))
      {
        r = e->merge (path);
        found = true;

        if (trace_include_regex)
          wcerr << "'" << r << "' : ";
      }

      if (trace_include_regex)
        wcerr << (found ? '+' : '-') << endl;

      if (found)
        break;
    }

    if (!found)
      r = path;

    if (!r.empty () && r[0] != L'"' && r[0] != L'<')
    {
      WideChar op (include_with_brackets ? L'<' : L'"');
      WideChar cl (include_with_brackets ? L'>' : L'"');
      r = op + r + cl;
    }

    return r;
  }

  // Namespace
  //

  Void Namespace::
  pre (Type& n)
  {
    String ns (ctx_.ns_name (n));

    String::size_type b (0), e;

    if (st_)
      st_->enter (L"");

    do
    {
      e = ns.find (L"::", b);

      String name (ns, b, e == ns.npos ? e : e - b);

      if (!name.empty ())
      {
        ctx_.os << "namespace " << name << "{";

        if (st_)
          st_->enter (name);
      }


      b = e;

      if (b == ns.npos)
        break;

      b += 2;

    } while (true);
  }

  Void Namespace::
  post (Type& n)
  {
    String ns (ctx_.ns_name (n));

    String::size_type b (0), e;

    do
    {
      e = ns.find (L"::", b);

      String name (ns, b, e == ns.npos ? e : e - b);

      if (!name.empty ())
      {
        ctx_.os << "}";

        if (st_)
          st_->leave ();
      }


      b = e;

      if (b == ns.npos)
        break;

      b += 2;

    }
    while (true);

    if (st_)
      st_->leave ();
  }

  // Include
  //
  Void Includes::
  traverse (SemanticGraph::Path const& path)
  {
    // Try to use the portable representation of the path. If that
    // fails, fall back to the native representation.
    //
    NarrowString path_str;
    try
    {
      path_str = path.string ();
    }
    catch (SemanticGraph::InvalidPath const&)
    {
      path_str = path.native_file_string ();
    }

    String hxx_path (expr_.merge (path_str));

    ctx_.os << "#include " << ctx_.process_include_path (hxx_path) << endl
            << endl;
  }
}
