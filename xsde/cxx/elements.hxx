// file      : xsde/cxx/elements.hxx
// copyright : Copyright (c) 2005-2017 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_ELEMENTS_HXX
#define CXX_ELEMENTS_HXX

#include <set>
#include <map>
#include <vector>
#include <ostream>

#include <cutl/re.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

#include <types.hxx>
#include <elements.hxx>

#include <cxx/options.hxx>

namespace CXX
{
  using std::endl;

  // On some platforms std::toupper can be something other than a
  // function with C++ linkage.
  //
  wchar_t
  upcase (wchar_t c);


  // Exceptions.
  //

  struct UnrepresentableCharacter
  {
    UnrepresentableCharacter (String const& str, size_t pos)
        : str_ (str), pos_ (pos)
    {
    }

    String const&
    string () const
    {
      return str_;
    }

    size_t
    position () const
    {
      return pos_;
    }

  private:
    String str_;
    size_t pos_;
  };

  struct NoNamespaceMapping
  {
    NoNamespaceMapping (SemanticGraph::Path const& file,
                        size_t line,
                        size_t column,
                        String const& ns)
        : file_ (file),
          line_ (line),
          column_ (column),
          ns_ (ns)
    {
    }

    SemanticGraph::Path const&
    file () const
    {
      return file_;
    }

    size_t
    line () const
    {
      return line_;
    }

    size_t
    column () const
    {
      return column_;
    }

    String const&
    ns () const
    {
      return ns_;
    }

  private:
    SemanticGraph::Path file_;
    size_t line_;
    size_t column_;
    String ns_;
  };

  struct InvalidNamespaceMapping
  {
    InvalidNamespaceMapping (String const& mapping,
                             String const& reason)
        : mapping_ (mapping), reason_ (reason)
    {
    }

    String const&
    mapping () const
    {
      return mapping_;
    }

    String const&
    reason () const
    {
      return reason_;
    }

  private:
    String mapping_;
    String reason_;
  };


  //
  //
  class Context
  {
  public:
    typedef cutl::re::wregex RegexPat;
    typedef cutl::re::wregexsub Regex;
    typedef std::vector<Regex> RegexMapping;
    typedef std::map<String, String> MapMapping;
    typedef std::map<String, String> MappingCache;

    typedef std::map<String, String> ReservedNameMap;
    typedef std::set<String> KeywordSet;

    typedef CXX::options options_type;

  public:
    Context (std::wostream& o,
             SemanticGraph::Schema& root,
             SemanticGraph::Path const& path,
             options_type const&,
             char const* name_key,
             NarrowString const& char_type);

  protected:
    Context (Context& c)
        : os (c.os),
          schema_root (c.schema_root),
          schema_path (c.schema_path),
          options (c.options),
          ename_key (c.ename_key),
          char_type (c.char_type),
          char_encoding (c.char_encoding),
          L (c.L),
          string_type (c.string_type),
          type_exp (c.type_exp),
          inst_exp (c.inst_exp),
          inl (c.inl),
          custom_alloc (c.custom_alloc),
          long_long (c.long_long),
          ns_mapping_cache (c.ns_mapping_cache),
          xs_ns_ (c.xs_ns_),
          cxx_id_expr (c.cxx_id_expr),
          urn_mapping (c.urn_mapping),
          nsr_mapping (c.nsr_mapping),
          nsm_mapping (c.nsm_mapping),
          include_mapping (c.include_mapping),
          reserved_name_map (c.reserved_name_map),
          keyword_set (c.keyword_set)
    {
    }

    Context (Context& c, std::wostream& o)
        : os (o),
          schema_root (c.schema_root),
          schema_path (c.schema_path),
          options (c.options),
          ename_key (c.ename_key),
          char_type (c.char_type),
          char_encoding (c.char_encoding),
          L (c.L),
          string_type (c.string_type),
          type_exp (c.type_exp),
          inst_exp (c.inst_exp),
          inl (c.inl),
          custom_alloc (c.custom_alloc),
          long_long (c.long_long),
          ns_mapping_cache (c.ns_mapping_cache),
          xs_ns_ (c.xs_ns_),
          cxx_id_expr (c.cxx_id_expr),
          urn_mapping (c.urn_mapping),
          nsr_mapping (c.nsr_mapping),
          nsm_mapping (c.nsm_mapping),
          include_mapping (c.include_mapping),
          reserved_name_map (c.reserved_name_map),
          keyword_set (c.keyword_set)
    {
    }

  public:
    static String
    unclash (String const& name, String const& new_name)
    {
      return name == new_name ? (new_name + L'_') : new_name;
    }

  public:
    // Return UTF-32 character starting at this position. Position is
    // advanced by 1 if this Unicode character takes more than one
    // underlying character.
    //
    static unsigned int
    unicode_char (String const& str, size_t& pos);

    static unsigned int
    unicode_char (wchar_t const*& p);

    // Escape C++ keywords and illegal characters.
    //
    String
    escape (String const&) const;

    // Create a string literal so that it can be used in C++ source
    // code. It includes "".
    //
    String
    strlit (String const&);

    // Escape the string so that it can be used in C++ comment.
    //
    String
    comment (String const&);

    // Translate XML namespace name to a C++ identifier.
    //
    String
    ns_name (SemanticGraph::Namespace&) const;

    // XML Schema namespace.
    //
    SemanticGraph::Namespace&
    xs_ns ();

    // C++ namespace for XML Schema.
    //
    String
    xs_ns_name ();

    //
    //
    SemanticGraph::Namespace&
    namespace_ (SemanticGraph::Nameable& n);

    // Original XML namespace name.
    //
    String
    xml_ns_name (SemanticGraph::Nameable& ns);


    // Fully-qualified C++ name.
    //
    String
    fq_name (SemanticGraph::Nameable& n, char const* name_key = 0) const;

  public:
    static SemanticGraph::Type&
    ultimate_base (SemanticGraph::Complex&);

  public:
    String
    process_include_path (String const&) const;

  public:
    static bool
    anonymous (SemanticGraph::Type& t)
    {
      return t.context ().count ("anonymous");
    }

    // Restriction correspondance.
    //
  public:
    static SemanticGraph::Element*
    correspondent (SemanticGraph::Element& r);

    static SemanticGraph::Any*
    correspondent (SemanticGraph::Any& r);

    static SemanticGraph::Compositor*
    correspondent (SemanticGraph::Compositor& r);

  public:
    // Get escaped name.
    //
    String const&
    ename (SemanticGraph::Nameable const& n) const
    {
      return n.context ().get<String> (ename_key.c_str ());
    }

  public:
    std::wostream& os;
    SemanticGraph::Schema& schema_root;
    SemanticGraph::Path const& schema_path;

    options_type const& options;

    NarrowString const& ename_key;

    String& char_type;
    String& char_encoding;
    String& L;                  // string literal prefix
    String& string_type;

    String& type_exp;
    String& inst_exp;
    String& inl;

    bool custom_alloc;
    bool long_long;

  public:
    MappingCache& ns_mapping_cache;

  private:
    SemanticGraph::Path const schema_path_;

    SemanticGraph::Namespace* xs_ns_;

    NarrowString const ename_key_;

    String char_type_;
    String char_encoding_;
    String L_;
    String string_type_;

    String type_exp_;
    String inst_exp_;
    String inl_;

  private:
    RegexPat const cxx_id_expr_;
    RegexPat const& cxx_id_expr;
    Regex urn_mapping_;
    RegexMapping nsr_mapping_;
    MapMapping nsm_mapping_;
    Regex const& urn_mapping;
    RegexMapping const& nsr_mapping;
    MapMapping const& nsm_mapping;
    MappingCache ns_mapping_cache_;

    RegexMapping include_mapping_;
    RegexMapping const& include_mapping;

    ReservedNameMap const& reserved_name_map;
    ReservedNameMap reserved_name_map_;

    KeywordSet const& keyword_set;
    KeywordSet keyword_set_;
  };

  inline unsigned int Context::
  unicode_char (String const& str, size_t& pos)
  {
    if (sizeof (wchar_t) == 4)
    {
      return str[pos];
    }
    else if (sizeof (wchar_t) == 2)
    {
      wchar_t x (str[pos]);

      if (x < 0xD800 || x > 0xDBFF)
        return x;
      else
        return ((x - 0xD800) << 10) + (str[++pos] - 0xDC00) + 0x10000;
    }
    else
      return 0;
  }

  inline unsigned int Context::
  unicode_char (wchar_t const*& p)
  {
    if (sizeof (wchar_t) == 4)
    {
      return *p;
    }
    else if (sizeof (wchar_t) == 2)
    {
      wchar_t x (*p);

      if (x < 0xD800 || x > 0xDBFF)
        return x;
      else
        return ((x - 0xD800) << 10) + (*(++p) - 0xDC00) + 0x10000;
    }
    else
      return 0;
  }

  // Sources traverser that goes into each schema only once.
  //
  struct Sources: Traversal::Sources
  {
    virtual void
    traverse (SemanticGraph::Sources& s)
    {
      if (schemas_.insert (&s.schema ()).second)
        Traversal::Sources::traverse (s);
    }

  private:
    std::set<SemanticGraph::Schema*> schemas_;
  };

  // Usual namespace mapping.
  //
  struct Namespace : Traversal::Namespace
  {
    struct ScopeTracker
    {
      virtual
      ~ScopeTracker () {}

      virtual void
      enter (String const&) = 0;

      virtual void
      leave () = 0;
    };


    Namespace (Context& c, ScopeTracker* st = 0)
        : ctx_ (c), st_ (st)
    {
    }

    Namespace (Context& c, ScopeTracker& st)
        : ctx_ (c), st_ (&st)
    {
    }

    virtual void
    pre (Type&);

    virtual void
    post (Type&);

  private:
    Context& ctx_;
    ScopeTracker* st_;
  };


  //
  //
  template <typename X>
  struct Has : X
  {
    Has (bool& result)
        : result_ (result)
    {
    }

    virtual void
    traverse (typename X::Type&)
    {
      result_ = true;
    }

  private:
    bool& result_;
  };

  // Checks if scope 'Y' names any of 'X'
  //
  template <typename X, typename Y>
  bool
  has (Y& y)
  {
    using SemanticGraph::Scope;

    bool result (false);
    Has<X> t (result);

    for (Scope::NamesIterator i (y.names_begin ()), e (y.names_end ());
         !result && i != e; ++i)
      t.dispatch (i->named ());

    return result;
  }

  // Checks if the compositor has any particle of 'X'
  //
  template <typename X>
  bool
  has_particle (SemanticGraph::Compositor& y)
  {
    using SemanticGraph::Compositor;

    bool result (false);
    Has<X> t (result);

    for (Compositor::ContainsIterator i (y.contains_begin ()),
           e (y.contains_end ()); !result && i != e; ++i)
    {
      SemanticGraph::Particle& p (i->particle ());

      t.dispatch (p);

      if (!result && p.is_a<Compositor> ())
        result = has_particle<X> (dynamic_cast<Compositor&> (p));
    }

    return result;
  }

  // Specialization for Complex
  //
  template <typename X>
  bool
  has_particle (SemanticGraph::Complex& c)
  {
    return c.contains_compositor_p () &&
      has_particle<X> (c.contains_compositor ().compositor ());
  }

  // Fundamental type mapping helper.
  //
  struct Fundamental : Traversal::Fundamental::Type,
                       Traversal::Fundamental::String,
                       Traversal::Fundamental::NormalizedString,
                       Traversal::Fundamental::Token,
                       Traversal::Fundamental::Name,
                       Traversal::Fundamental::NameToken,
                       Traversal::Fundamental::NCName,
                       Traversal::Fundamental::Id,
                       Traversal::Fundamental::IdRef
  {
    virtual void
    fundamental_type (SemanticGraph::Fundamental::Type& t) = 0;

    virtual void
    fundamental_template (SemanticGraph::Fundamental::Type& t) = 0;

    virtual void
    traverse (SemanticGraph::Fundamental::Type& t)
    {
      fundamental_type (t);
    }

    virtual void
    traverse (SemanticGraph::Fundamental::String& t)
    {
      fundamental_template (t);
    }

    virtual void
    traverse (SemanticGraph::Fundamental::NormalizedString& t)
    {
      fundamental_template (t);
    }

    virtual void
    traverse (SemanticGraph::Fundamental::Token& t)
    {
      fundamental_template (t);
    }

    virtual void
    traverse (SemanticGraph::Fundamental::Name& t)
    {
      fundamental_template (t);
    }

    virtual void
    traverse (SemanticGraph::Fundamental::NameToken& t)
    {
      fundamental_template (t);
    }

    virtual void
    traverse (SemanticGraph::Fundamental::NCName& t)
    {
      fundamental_template (t);
    }

    virtual void
    traverse (SemanticGraph::Fundamental::Id& t)
    {
      fundamental_template (t);
    }

    virtual void
    traverse (SemanticGraph::Fundamental::IdRef& t)
    {
      fundamental_template (t);
    }
  };

  struct LiteralValue: Traversal::AnySimpleType,

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
                       Traversal::Fundamental::NCName,
                       Traversal::Fundamental::Language,

                       Traversal::Fundamental::Id,
                       Traversal::Fundamental::IdRef,
                       Traversal::Fundamental::AnyURI,
                       Traversal::Fundamental::Entity,

                       Context
  {
    LiteralValue (Context&, bool str);

    String
    dispatch (SemanticGraph::Node& type, String const& value);

    virtual void
    traverse (SemanticGraph::AnySimpleType&);

    // Boolean.
    //
    virtual void
    traverse (SemanticGraph::Fundamental::Boolean&);

    // Integral types.
    //
    virtual void
    traverse (SemanticGraph::Fundamental::Byte&);

    virtual void
    traverse (SemanticGraph::Fundamental::UnsignedByte&);

    virtual void
    traverse (SemanticGraph::Fundamental::Short&);

    virtual void
    traverse (SemanticGraph::Fundamental::UnsignedShort&);

    virtual void
    traverse (SemanticGraph::Fundamental::Int&);

    virtual void
    traverse (SemanticGraph::Fundamental::UnsignedInt&);

    virtual void
    traverse (SemanticGraph::Fundamental::Long&);

    virtual void
    traverse (SemanticGraph::Fundamental::UnsignedLong&);

    virtual void
    traverse (SemanticGraph::Fundamental::Integer&);

    virtual void
    traverse (SemanticGraph::Fundamental::NonPositiveInteger&);

    virtual void
    traverse (SemanticGraph::Fundamental::NonNegativeInteger&);

    virtual void
    traverse (SemanticGraph::Fundamental::PositiveInteger&);

    virtual void
    traverse (SemanticGraph::Fundamental::NegativeInteger&);

    // Floats.
    //
    virtual void
    traverse (SemanticGraph::Fundamental::Float&);

    virtual void
    traverse (SemanticGraph::Fundamental::Double&);

    virtual void
    traverse (SemanticGraph::Fundamental::Decimal&);

    // Strings.
    //
    virtual void
    traverse (SemanticGraph::Fundamental::String&);

    virtual void
    traverse (SemanticGraph::Fundamental::NormalizedString&);

    virtual void
    traverse (SemanticGraph::Fundamental::Token&);

    virtual void
    traverse (SemanticGraph::Fundamental::NameToken&);

    virtual void
    traverse (SemanticGraph::Fundamental::Name&);

    virtual void
    traverse (SemanticGraph::Fundamental::NCName&);

    virtual void
    traverse (SemanticGraph::Fundamental::Language&);

    // ID/IDREF.
    //
    virtual void
    traverse (SemanticGraph::Fundamental::Id&);

    virtual void
    traverse (SemanticGraph::Fundamental::IdRef&);

    // URI.
    //
    virtual void
    traverse (SemanticGraph::Fundamental::AnyURI&);

    // Entity.
    //
    virtual void
    traverse (SemanticGraph::Fundamental::Entity&);

  public:
    static void
    normalize (String&);

    static void
    collapse (String&);

    static void
    strip_zeros (String&);

    static void
    make_float (String&);

  private:
    bool str_;
    String value_;
    String literal_;
  };
}

#endif  // CXX_TREE_ELEMENTS_HXX
