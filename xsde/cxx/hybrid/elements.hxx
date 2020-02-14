// file      : xsde/cxx/hybrid/elements.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_ELEMENTS_HXX
#define CXX_HYBRID_ELEMENTS_HXX

#include <deque>
#include <sstream>

#include <cutl/re.hxx>

#include <cxx/elements.hxx>

#include <cxx/hybrid/options.hxx>

namespace CXX
{
  namespace Hybrid
  {
    typedef cutl::re::regexsub Regex;

    //
    //
    class Context: public CXX::Context
    {
    public:
      typedef Hybrid::Regex Regex;

      typedef Hybrid::options options_type;

    public:
      Context (std::wostream&,
               SemanticGraph::Schema&,
               SemanticGraph::Path const&,
               options_type const&,
               Regex const* fwd_expr,
               Regex const* hxx_expr,
               Regex const* ixx_expr);

    protected:
      Context (Context& c)
          : CXX::Context (c),
            options (c.options),
            exceptions (c.exceptions),
            stl (c.stl),
            poly_code (c.poly_code),
            poly_runtime (c.poly_runtime),
            reset (c.reset),
            clone (c.clone),
            detach (c.detach),
            typeinfo (c.typeinfo),
            mixin (c.mixin),
            tiein (c.tiein),
            enum_ (c.enum_),
            fwd_expr (c.fwd_expr),
            hxx_expr (c.hxx_expr),
            ixx_expr (c.ixx_expr),
            ns_stack (c.ns_stack),
            pod_seq (c.pod_seq),
            fix_seq (c.fix_seq),
            var_seq (c.var_seq),
            str_seq (c.str_seq),
            data_seq (c.data_seq),
            istreams (c.istreams),
            ostreams (c.ostreams),
            icdrstream (c.icdrstream),
            ocdrstream (c.ocdrstream),
            ixdrstream (c.ixdrstream),
            oxdrstream (c.oxdrstream)
      {
      }

      Context (Context& c, std::wostream& o)
          : CXX::Context (c, o),
            options (c.options),
            exceptions (c.exceptions),
            stl (c.stl),
            poly_code (c.poly_code),
            poly_runtime (c.poly_runtime),
            reset (c.reset),
            clone (c.clone),
            detach (c.detach),
            typeinfo (c.typeinfo),
            mixin (c.mixin),
            tiein (c.tiein),
            enum_ (c.enum_),
            fwd_expr (c.fwd_expr),
            hxx_expr (c.hxx_expr),
            ixx_expr (c.ixx_expr),
            ns_stack (c.ns_stack),
            pod_seq (c.pod_seq),
            fix_seq (c.fix_seq),
            var_seq (c.var_seq),
            str_seq (c.str_seq),
            data_seq (c.data_seq),
            istreams (c.istreams),
            ostreams (c.ostreams),
            icdrstream (c.icdrstream),
            ocdrstream (c.ocdrstream),
            ixdrstream (c.ixdrstream),
            oxdrstream (c.oxdrstream)
      {
      }

    public:
      using CXX::Context::ename;

      static String const&
      ename (SemanticGraph::Compositor& c)
      {
        return c.context ().get<String> ("name");
      }

      static String const&
      ename_custom (SemanticGraph::Type& t)
      {
        SemanticGraph::Context& c (t.context ());

        if (!c.count ("name-base"))
          return c.get<String> ("name");
        else
          return c.get<String> ("name-base");
      }

      static String const&
      etype (SemanticGraph::Compositor& c)
      {
        return c.context ().get<String> ("type");
      }

      static String const&
      emember (SemanticGraph::Particle& p)
      {
        return p.context ().get<String> ("member");
      }

      static String const&
      emember (SemanticGraph::Attribute& a)
      {
        return a.context ().get<String> ("member");
      }

      // Detach.
      //
      static String const&
      edetach (SemanticGraph::Particle& p)
      {
        return p.context ().get<String> ("detach");
      }

      static String const&
      edetach (SemanticGraph::Attribute& a)
      {
        return a.context ().get<String> ("detach");
      }

      // Optional.
      //
      static String const&
      epresent (SemanticGraph::Particle& p)
      {
        return p.context ().get<String> ("present");
      }

      static String const&
      epresent (SemanticGraph::Attribute& a)
      {
        return a.context ().get<String> ("present");
      }

      static String const&
      edefault (SemanticGraph::Attribute& a)
      {
        return a.context ().get<String> ("default");
      }

      static String const&
      edefault_value (SemanticGraph::Attribute& a)
      {
        return a.context ().get<String> ("default-value");
      }

      static String const&
      epresent_member (SemanticGraph::Particle& p)
      {
        return p.context ().get<String> ("present-member");
      }

      static String const&
      epresent_member (SemanticGraph::Attribute& a)
      {
        return a.context ().get<String> ("present-member");
      }

      // Sequence.
      //
      static String const&
      esequence (SemanticGraph::Element& e)
      {
        return e.context ().get<String> ("sequence");
      }

      static String const&
      esequence (SemanticGraph::Compositor& c)
      {
        return c.context ().get<String> ("sequence");
      }

      static String const&
      eiterator (SemanticGraph::Element& e)
      {
        return e.context ().get<String> ("iterator");
      }

      static String const&
      eiterator (SemanticGraph::Compositor& c)
      {
        return c.context ().get<String> ("iterator");
      }

      static String const&
      econst_iterator (SemanticGraph::Element& e)
      {
        return e.context ().get<String> ("const-iterator");
      }

      static String const&
      econst_iterator (SemanticGraph::Compositor& c)
      {
        return c.context ().get<String> ("const-iterator");
      }

      // Choice.
      //
      static String const&
      etag (SemanticGraph::Particle& p)
      {
        return p.context ().get<String> ("tag");
      }

      static String const&
      earm (SemanticGraph::Choice& c)
      {
        return c.context ().get<String> ("arm");
      }

      static String const&
      earm_tag (SemanticGraph::Choice& c)
      {
        return c.context ().get<String> ("arm-tag");
      }

      static size_t const&
      arm_tag_count (SemanticGraph::Choice& c)
      {
        return c.context ().get<size_t> ("arm-tag-count");
      }

      static String const&
      earm_member (SemanticGraph::Choice& c)
      {
        return c.context ().get<String> ("arm-member");
      }

      // Custom data.
      //
      static String const&
      ecd_name (SemanticGraph::Type& t)
      {
        return t.context ().get<String> ("cd-name");
      }

      static String const&
      ecd_name (SemanticGraph::Compositor& c)
      {
        return c.context ().get<String> ("cd-name");
      }

      static String const&
      ecd_member (SemanticGraph::Type& t)
      {
        return t.context ().get<String> ("cd-member");
      }

      static String const&
      ecd_member (SemanticGraph::Compositor& c)
      {
        return c.context ().get<String> ("cd-member");
      }

      static String const&
      ecd_sequence (SemanticGraph::Type& t)
      {
        return t.context ().get<String> ("cd-sequence");
      }

      static String const&
      ecd_sequence (SemanticGraph::Compositor& c)
      {
        return c.context ().get<String> ("cd-sequence");
      }

      static String const&
      ecd_iterator (SemanticGraph::Type& t)
      {
        return t.context ().get<String> ("cd-iterator");
      }

      static String const&
      ecd_iterator (SemanticGraph::Compositor& c)
      {
        return c.context ().get<String> ("cd-iterator");
      }

      static String const&
      ecd_const_iterator (SemanticGraph::Type& t)
      {
        return t.context ().get<String> ("cd-const-iterator");
      }

      static String const&
      ecd_const_iterator (SemanticGraph::Compositor& c)
      {
        return c.context ().get<String> ("cd-const-iterator");
      }

      // Parser
      //
    public:
      static String const&
      pret_type (SemanticGraph::Type&);

      static String const&
      parg_type (SemanticGraph::Type&);

      static String const&
      post_name (SemanticGraph::Type&);

      static String const&
      pre_impl_name (SemanticGraph::Type&);

      // Names
      //
      static String const&
      epname (SemanticGraph::Particle&);

      static String const&
      epname (SemanticGraph::Attribute&);

      static String const&
      epimpl (SemanticGraph::Type&);

      static String const&
      epimpl_custom (SemanticGraph::Type&);

      static String const&
      epskel (SemanticGraph::Type&);

      // Optional.
      //
      static String const&
      eppresent (SemanticGraph::Compositor&);

      // Sequence.
      //
      static String const&
      epnext (SemanticGraph::Compositor&);

      // Choice.
      //
      static String const&
      eptag (SemanticGraph::Particle&);

      static String const&
      eparm (SemanticGraph::Choice&);

      static String const&
      eparm_tag (SemanticGraph::Choice&);

      // State
      //
      static String const&
      epstate (SemanticGraph::Type&);

      static String const&
      epstate_type (SemanticGraph::Type&);

      static String const&
      epstate_base (SemanticGraph::Type&);

      static String const&
      epstate_first (SemanticGraph::Type&);

      static String const&
      epstate_top (SemanticGraph::Type&);

      static String const&
      epstate_member (SemanticGraph::Type&);

      static String const&
      epstate_member (SemanticGraph::Compositor&);

      // Serializer
      //
    public:
      static String const&
      sret_type (SemanticGraph::Type&);

      static String const&
      sarg_type (SemanticGraph::Type&);

      // Names
      //
      static String const&
      esname (SemanticGraph::Particle&);

      static String const&
      esname (SemanticGraph::Attribute&);

      static String const&
      esimpl (SemanticGraph::Type&);

      static String const&
      esimpl_custom (SemanticGraph::Type&);

      static String const&
      esskel (SemanticGraph::Type&);

      // Optional.
      //
      static String const&
      espresent (SemanticGraph::Particle&);

      static String const&
      espresent (SemanticGraph::Attribute&);

      // Sequence.
      //
      static String const&
      esnext (SemanticGraph::Particle&);

      // Choice.
      //
      static String const&
      estag (SemanticGraph::Particle&);

      static String const&
      esarm (SemanticGraph::Choice&);

      static String const&
      esarm_tag (SemanticGraph::Choice&);

      // State
      //
      static String const&
      esstate (SemanticGraph::Type&);

      static String const&
      esstate_type (SemanticGraph::Type&);

      static String const&
      esstate_first (SemanticGraph::Type&);

      static String const&
      esstate_top (SemanticGraph::Type&);

      static String const&
      esstate_member (SemanticGraph::Type&);

      static String const&
      esstate_member (SemanticGraph::Particle&);

      static String const&
      esstate_member_end (SemanticGraph::Particle&);

    public:
      String
      scope (SemanticGraph::Compositor& c) const
      {
        return scope (c, false);
      }

      String
      scope (SemanticGraph::Element& e) const
      {
        return scope (e, false);
      }

      String
      scope (SemanticGraph::Attribute& a) const
      {
        return scope (a, false);
      }

      String
      fq_scope (SemanticGraph::Compositor& c) const
      {
        return scope (c, true);
      }

      String
      fq_scope (SemanticGraph::Element& e) const
      {
        return scope (e, true);
      }

      String
      fq_scope (SemanticGraph::Attribute& a) const
      {
        return scope (a, true);
      }

    private:
      String
      scope (SemanticGraph::Compositor&, bool fq) const;

      String
      scope (SemanticGraph::Element&, bool fq) const;

      String
      scope (SemanticGraph::Attribute&, bool fq) const;

    public:
      bool
      restriction_p (SemanticGraph::Complex& c) const
      {
        if (c.inherits_p () &&
            c.inherits ().is_a<SemanticGraph::Restricts> ())
        {
          // Restriction of anyType is a special case.
          //
          return !c.inherits ().base ().is_a<SemanticGraph::AnyType> ();
        }

        return false;
      }

    public:
      static bool
      fixed_length (SemanticGraph::Type& t)
      {
        return t.context ().get<bool> ("fixed");
      }

      static bool
      fixed_length (SemanticGraph::Compositor& c)
      {
        return c.context ().get<bool> ("fixed");
      }

      static bool
      recursive (SemanticGraph::Type& t)
      {
        return t.context ().count ("recursive");
      }

      static bool
      polymorphic (SemanticGraph::Type& t)
      {
        return t.context ().count ("polymorphic");
      }

      static bool
      recursive_base (SemanticGraph::Complex& c);

    public:
      String
      istream (NarrowString const& is) const;

      String
      ostream (NarrowString const& os) const;

    public:
      void
      close_ns ();

      void
      open_ns ();

    public:
      // Determine whether we are generating the enum mapping for this
      // enumeration. Also optionally return the base enum.
      //
      static bool
      enum_mapping (SemanticGraph::Enumeration& e,
                    SemanticGraph::Enumeration** base = 0);
    public:
      typedef std::deque<String> NamespaceStack;

    public:
      options_type const& options;

      bool exceptions;
      bool stl;
      bool poly_code;
      bool poly_runtime;
      bool reset;
      bool clone;
      bool detach;
      bool typeinfo;
      bool mixin;
      bool tiein;
      bool enum_;

      Regex const* fwd_expr;
      Regex const* hxx_expr;
      Regex const* ixx_expr;

      NamespaceStack& ns_stack;

      String const& pod_seq;
      String const& fix_seq;
      String const& var_seq;
      String const& str_seq;
      String const& data_seq;

      NarrowStrings const& istreams;
      NarrowStrings const& ostreams;

      String const& icdrstream;
      String const& ocdrstream;
      String const& ixdrstream;
      String const& oxdrstream;

    private:
      NamespaceStack ns_stack_;

      String pod_seq_;
      String fix_seq_;
      String var_seq_;
      String str_seq_;
      String data_seq_;

      String icdrstream_;
      String ocdrstream_;
      String ixdrstream_;
      String oxdrstream_;
    };

    //
    //
    struct Namespace: CXX::Namespace, CXX::Namespace::ScopeTracker
    {
      Namespace (Context&, bool track_scope = false);

    protected:
      virtual void
      enter (String const& name);

      virtual void
      leave ();

    protected:
      Context& ctx_;
    };

    //
    //
    struct FundamentalType: Traversal::Fundamental::Byte,
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
                            Traversal::Fundamental::Decimal

    {
      FundamentalType (bool& result)
          : r_ (result)
      {
      }

      // Integral types.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Byte&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::UnsignedByte&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Short&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::UnsignedShort&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Int&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::UnsignedInt&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Long&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::UnsignedLong&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Integer&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NonPositiveInteger&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NonNegativeInteger&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::PositiveInteger&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NegativeInteger&)
      {
        r_ = true;
      }

      // Boolean.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Boolean&)
      {
        r_ = true;
      }

      // Floats.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Float&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Double&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Decimal&)
      {
        r_ = true;
      }

    private:
      bool& r_;
    };

    //
    //
    struct StringType: Traversal::AnySimpleType,

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
                       Traversal::Fundamental::Entity
    {
      StringType (bool& result)
          : r_ (result)
      {
      }

      virtual void
      traverse (SemanticGraph::AnySimpleType&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::String&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NormalizedString&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Token&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NameToken&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Name&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NCName&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Language&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Id&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::IdRef&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::AnyURI&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Entity&)
      {
        r_ = true;
      }

    private:
      bool& r_;
    };

    // Check whether this is a string-based type (excluding ID, IDFER,
    // anyURI, and ENTITY).
    //
    struct StringBasedType: Traversal::Complex,
                            Traversal::Fundamental::String,
                            Traversal::Fundamental::NormalizedString,
                            Traversal::Fundamental::Token,
                            Traversal::Fundamental::Name,
                            Traversal::Fundamental::NameToken,
                            Traversal::Fundamental::NCName,
                            Traversal::Fundamental::Language
    {
      StringBasedType (bool& r)
          : r_ (r)
      {
        *this >> inherits_ >> *this;
      }

      virtual void
      traverse (SemanticGraph::Complex& c)
      {
        inherits (c, inherits_);
      }

      // Strings.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::String&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NormalizedString&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Token&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NameToken&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Name&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NCName&)
      {
        r_ = true;
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Language&)
      {
        r_ = true;
      }

    private:
      bool& r_;
      Traversal::Inherits inherits_;
    };


    // Check whether this is a enumeration-based type. Return ultimate
    // enumeration base of 0.
    //
    struct EnumBasedType: Traversal::Complex
    {
      EnumBasedType (SemanticGraph::Enumeration*& e)
          : enum_ (e)
      {
        *this >> inherits_;

        inherits_ >> *this;
        inherits_ >> enum_;
      }

      virtual void
      traverse (SemanticGraph::Complex& c)
      {
        inherits (c, inherits_);
      }

    private:
      struct Enumeration: Traversal::Enumeration
      {
        Enumeration (SemanticGraph::Enumeration*& e)
            : e_ (e)
        {
          *this >> inherits_ >> *this;
        }

        virtual void
        traverse (Type& e)
        {
          e_ = &e;
          inherits (e, inherits_);
        }

      private:
        SemanticGraph::Enumeration*& e_;
        Traversal::Inherits inherits_;
      };


    private:
      Enumeration enum_;
      Traversal::Inherits inherits_;
    };


    //
    //
    struct TypeName : Traversal::Type,
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
      enum Use
      {
        base,
        ro_ret,
        ret,
        arg,
        var,
        var_value,
        seq
      };

      TypeName (Context& c, Use use)
          : Context (c), use_ (use)
      {
        if (use == base)
          xs_ns_ = xs_ns_name ();
      }

      virtual void
      traverse (SemanticGraph::Type& t)
      {
        type (t);
      }

      // anyType & anySimpleType.
      //
      virtual void
      traverse (SemanticGraph::AnyType& t)
      {
        type (t);
      }

      virtual void
      traverse (SemanticGraph::AnySimpleType&)
      {
        if (stl || use_ != base)
          string_type ();
        else
          os << xs_ns_ << "::any_simple_type_base";
      }

      // Boolean.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Boolean&)
      {
        if (use_ != base)
          fund_type ("bool");
        else
          os << xs_ns_ << "::boolean_base";
      }

      // Integral types.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Byte&)
      {
        if (use_ != base)
          fund_type ("signed char");
        else
          os << xs_ns_ << "::byte_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::UnsignedByte&)
      {
        if (use_ != base)
          fund_type ("unsigned char");
        else
          os << xs_ns_ << "::unsigned_byte_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Short&)
      {
        if (use_ != base)
          fund_type ("short");
        else
          os << xs_ns_ << "::short_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::UnsignedShort&)
      {
        if (use_ != base)
          fund_type ("unsigned short");
        else
          os << xs_ns_ << "::unsigned_short_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Int&)
      {
        if (use_ != base)
          fund_type ("int");
        else
          os << xs_ns_ << "::int_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::UnsignedInt&)
      {
        if (use_ != base)
          fund_type ("unsigned int");
        else
          os << xs_ns_ << "::unsigned_int_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Long&)
      {
        if (use_ != base)
        {
          if (options.no_long_long ())
            fund_type ("long");
          else
            fund_type ("long long");
        }
        else
          os << xs_ns_ << "::long_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::UnsignedLong&)
      {
        if (use_ != base)
        {
          if (options.no_long_long ())
            fund_type ("unsigned long");
          else
            fund_type ("unsigned long long");
        }
        else
          os << xs_ns_ << "::unsigned_long_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Integer&)
      {
        if (use_ != base)
          fund_type ("long");
        else
          os << xs_ns_ << "::integer_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NonPositiveInteger&)
      {
        if (use_ != base)
          fund_type ("long");
        else
          os << xs_ns_ << "::non_positive_integer_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NonNegativeInteger&)
      {
        if (use_ != base)
          fund_type ("unsigned long");
        else
          os << xs_ns_ << "::non_negative_integer_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::PositiveInteger&)
      {
        if (use_ != base)
          fund_type ("unsigned long");
        else
          os << xs_ns_ << "::positive_integer_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NegativeInteger&)
      {
        if (use_ != base)
          fund_type ("long");
        else
          os << xs_ns_ << "::negative_integer_base";
      }

      // Floats.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Float&)
      {
        if (use_ != base)
          fund_type ("float");
        else
          os << xs_ns_ << "::float_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Double&)
      {
        if (use_ != base)
          fund_type ("double");
        else
          os << xs_ns_ << "::double_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Decimal&)
      {
        if (use_ != base)
          fund_type ("double");
        else
          os << xs_ns_ << "::decimal_base";
      }

      // Strings.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::String&)
      {
        if (stl || use_ != base)
          string_type ();
        else
          os << xs_ns_ << "::string_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NormalizedString&)
      {
        if (stl || use_ != base)
          string_type ();
        else
          os << xs_ns_ << "::normalized_string_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Token&)
      {
        if (stl || use_ != base)
          string_type ();
        else
          os << xs_ns_ << "::token_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NameToken&)
      {
        if (stl || use_ != base)
          string_type ();
        else
          os << xs_ns_ << "::nmtoken_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NameTokens& t)
      {
        type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Name&)
      {
        if (stl || use_ != base)
          string_type ();
        else
          os << xs_ns_ << "::name_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NCName&)
      {
        if (stl || use_ != base)
          string_type ();
        else
          os << xs_ns_ << "::ncname_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Language&)
      {
        if (stl || use_ != base)
          string_type ();
        else
          os << xs_ns_ << "::language_base";
      }


      // Qualified name.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::QName& t)
      {
        type (t);
      }


      // ID/IDREF.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Id&)
      {
        if (stl || use_ != base)
          string_type ();
        else
          os << xs_ns_ << "::id_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::IdRef&)
      {
        if (stl || use_ != base)
          string_type ();
        else
          os << xs_ns_ << "::idref_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::IdRefs& t)
      {
        type (t);
      }

      // URI.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::AnyURI&)
      {
        if (stl || use_ != base)
          string_type ();
        else
          os << xs_ns_ << "::uri_base";
      }

      // Binary.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Base64Binary& t)
      {
        type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::HexBinary& t)
      {
        type (t);
      }


      // Date/time.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Date& t)
      {
        type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::DateTime& t)
      {
        type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Duration& t)
      {
        type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Day& t)
      {
        type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Month& t)
      {
        type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::MonthDay& t)
      {
        type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Year& t)
      {
        type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::YearMonth& t)
      {
        type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Time& t)
      {
        type (t);
      }

      // Entity.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Entity&)
      {
        if (stl || use_ != base)
          string_type ();
        else
          os << xs_ns_ << "::entity_base";
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Entities& t)
      {
        type (t);
      }

    private:
      void
      type (SemanticGraph::Type& t)
      {
        String fq (fq_name (t));

        switch (use_)
        {
        case base:
          {
            os << fq;
            break;
          }
        case ro_ret:
          {
            os << "const " << fq << "&";
            break;
          }
        case ret:
          {
            os << fq << "&";
            break;
          }
        case arg:
          {
            if (fixed_length (t))
              os << "const " << fq << "&";
            else
              os << fq << "*";

            break;
          }
        case var:
          {
            if (fixed_length (t))
              os << fq;
            else
              os << fq << "*";

            break;
          }
        case var_value:
          {
            os << fq;
            break;
          }
        case seq:
          {
            os << (fixed_length (t) ? fix_seq : var_seq) << "< " <<
              fq << " >";
            break;
          }
        }
      }

      void
      fund_type (char const* name)
      {
        switch (use_)
        {
        case ret:
          {
            os << name << "&";
            break;
          }
        case seq:
          {
            os << pod_seq << "< " << name << " >";
            break;
          }
        default:
          {
            os << name;
            break;
          }
        }
      }

      void
      string_type ()
      {
        switch (use_)
        {
        case base:
          {
            // Non-STL case is handled by the caller.
            //
            os << "::std::string";
            break;
          }
        case ro_ret:
          {
            if (stl)
              os << "const ::std::string&";
            else
              os << "const char*";
            break;
          }
        case ret:
          {
            if (stl)
              os << "::std::string&";
            else
              os << "char*";
            break;
          }
        case arg:
          {
            if (stl)
              os << "const ::std::string&";
            else
              os << "char*";
            break;
          }
        case var:
        case var_value:
          {
            if (stl)
              os << "::std::string";
            else
              os << "char*";
            break;
          }
        case seq:
          {
            os << str_seq;
            break;
          }
        }
      }

    private:
      Use use_;
      String xs_ns_;
    };

    struct TypeOpsBase: Traversal::Type,
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
      virtual void
      traverse (SemanticGraph::Type& t)
      {
        general_type (t);
      }

      // anyType & anySimpleType.
      //
      virtual void
      traverse (SemanticGraph::AnyType& t)
      {
        general_type (t);
      }

      virtual void
      traverse (SemanticGraph::AnySimpleType& t)
      {
        string_type (t);
      }

      // Boolean.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Boolean& t)
      {
        fund_type (t);
      }

      // Integral types.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Byte& t)
      {
        fund_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::UnsignedByte& t)
      {
        fund_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Short& t)
      {
        fund_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::UnsignedShort& t)
      {
        fund_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Int& t)
      {
        fund_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::UnsignedInt& t)
      {
        fund_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Long& t)
      {
        fund_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::UnsignedLong& t)
      {
        fund_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Integer& t)
      {
        fund_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NonPositiveInteger& t)
      {
        fund_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NonNegativeInteger& t)
      {
        fund_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::PositiveInteger& t)
      {
        fund_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NegativeInteger& t)
      {
        fund_type (t);
      }

      // Floats.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Float& t)
      {
        fund_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Double& t)
      {
        fund_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Decimal& t)
      {
        fund_type (t);
      }

      // Strings.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::String& t)
      {
        string_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NormalizedString& t)
      {
        string_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Token& t)
      {
        string_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NameToken& t)
      {
        string_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NameTokens& t)
      {
        general_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Name& t)
      {
        string_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::NCName& t)
      {
        string_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Language& t)
      {
        string_type (t);
      }

      // Qualified name.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::QName& t)
      {
        general_type (t);
      }

      // ID/IDREF.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Id& t)
      {
        string_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::IdRef& t)
      {
        string_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::IdRefs& t)
      {
        general_type (t);
      }

      // URI.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::AnyURI& t)
      {
        string_type (t);
      }

      // Binary.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Base64Binary& t)
      {
        general_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::HexBinary& t)
      {
        general_type (t);
      }

      // Date/time.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Date& t)
      {
        general_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::DateTime& t)
      {
        general_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Duration& t)
      {
        general_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Day& t)
      {
        general_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Month& t)
      {
        general_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::MonthDay& t)
      {
        general_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Year& t)
      {
        general_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::YearMonth& t)
      {
        general_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Time& t)
      {
        general_type (t);
      }

      // Entity.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Entity& t)
      {
        string_type (t);
      }

      virtual void
      traverse (SemanticGraph::Fundamental::Entities& t)
      {
        general_type (t);
      }

    protected:
      virtual void
      general_type (SemanticGraph::Type&) = 0;

      virtual void
      fund_type (SemanticGraph::Type&) = 0;

      virtual void
      string_type (SemanticGraph::Type&) = 0;
    };

    struct TypeDeref: TypeOpsBase, Context
    {
      TypeDeref (Context& c)
          : Context (c)
      {
      }

    protected:
      virtual void
      general_type (SemanticGraph::Type& t)
      {
        if (!fixed_length (t))
          os << "*";
      }

      virtual void
      fund_type (SemanticGraph::Type&)
      {
      }

      virtual void
      string_type (SemanticGraph::Type&)
      {
      }
    };

    struct TypeDelete: TypeOpsBase, Context
    {
      TypeDelete (Context& c)
          : Context (c)
      {
      }

      void
      dispatch (SemanticGraph::Node& type, String const& var)
      {
        var_ = var;
        Traversal::NodeBase::dispatch (type);
      }

    protected:
      virtual void
      general_type (SemanticGraph::Type& t)
      {
        if (!custom_alloc)
          os << "delete " << var_ << ";";
        else
        {
          os << "if (" << var_ << ")"
             << "{"
             << "typedef " << fq_name (t) << " _dtor;"
             << var_ << "->~_dtor ();"
             << "::xsde::cxx::free (" << var_ << ");"
             << "}";
        }
      }

      virtual void
      fund_type (SemanticGraph::Type& t)
      {
        general_type (t);
      }

      virtual void
      string_type (SemanticGraph::Type& t)
      {
        if (stl)
          general_type (t);
        else
        {
          if (!custom_alloc)
            os << "delete[] " << var_ << ";";
          else
            os << "::xsde::cxx::free (" << var_ << ");";
        }
      }

    private:
      String var_;
    };

    //
    //
    struct TypeClone: TypeOpsBase, Context
    {
      TypeClone (Context& c)
          : Context (c)
      {
      }

      // Copy member from 'this' to 'c'.
      //
      void
      dispatch (SemanticGraph::Node& type, SemanticGraph::Member& member)
      {
        member_ = &member;
        Traversal::NodeBase::dispatch (type);
      }

    protected:
      virtual void
      general_type (SemanticGraph::Type& t)
      {
        String const& name (ename (*member_));

        os << "{"
           << fq_name (t) << "* m = this->" << name << " ()._clone ();";

        if (!exceptions)
          os << endl
             << "if (m == 0)" << endl
             << "return false;"
             << endl;

        os << "c." << name << " (m);"
           << "}";
      }

      virtual void
      fund_type (SemanticGraph::Type& t)
      {
        general_type (t);
      }

      virtual void
      string_type (SemanticGraph::Type&)
      {
        // We can only get here if STL is disabled.
        //
        String const& name (ename (*member_));

        os << "{"
           << "char* m = ::xsde::cxx::strdupx (this->" << name << " ());";

        if (!exceptions)
          os << endl
             << "if (m == 0)" << endl
             << "return false;"
             << endl;

        os << "c." << name << " (m);"
           << "}";
      }

    private:
      SemanticGraph::Member* member_;
    };

    //
    //
    struct TypeForward: Traversal::Type, Context
    {
      TypeForward (Context& c)
          : Context (c)
      {
      }

      virtual void
      traverse (SemanticGraph::Type& t);
    };

    struct Includes : Traversal::Imports,
                      Traversal::Includes
    {
      enum Type
      {
        forward,
        header,
        impl_header,
        inline_,
        source
      };

      Includes (Context& c, Type type, Regex const* r = 0)
          : ctx_ (c),
            type_ (type),
            forward_ (c.options.generate_forward ()),
            regex_ (r),
            namespace_ (c),
            type_forward_ (c)
      {
        schema_ >> schema_names_ >> namespace_ >> names_ >> type_forward_;
      }

      virtual void
      traverse (SemanticGraph::Imports& i)
      {
        traverse_ (i);
      }

      virtual void
      traverse (SemanticGraph::Includes& i)
      {
        traverse_ (i);
      }

    private:
      void
      traverse_ (SemanticGraph::Uses&);

    private:
      Context& ctx_;
      Type type_;
      bool forward_;
      Regex const* regex_;

      Traversal::Schema schema_;
      Traversal::Names schema_names_;
      Namespace namespace_;
      Traversal::Names names_;
      TypeForward type_forward_;
    };

    // Test whether there are any aggregates to be generated.
    //
    struct AggregateTest: Traversal::Type, Traversal::Element
    {
      AggregateTest (bool& generate, char const* key)
          : gen_ (generate), key_ (key)
      {
      }

      virtual void
      traverse (SemanticGraph::Type& t)
      {
        if (!gen_ && t.context ().count (key_))
          gen_ = true;
      }

      virtual void
      traverse (SemanticGraph::Element& e)
      {
        if (!gen_ && e.context ().count (key_))
          gen_ = true;
      }

    private:
      bool& gen_;
      char const* key_;
    };
  }
}

#endif  // CXX_HYBRID_ELEMENTS_HXX
