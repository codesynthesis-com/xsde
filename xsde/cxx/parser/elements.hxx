// file      : xsde/cxx/parser/elements.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_PARSER_ELEMENTS_HXX
#define CXX_PARSER_ELEMENTS_HXX

#include <sstream>

#include <backend-elements/regex.hxx>

#include <cxx/elements.hxx>

#include <cxx/parser/cli.hxx>

namespace CXX
{
  namespace Parser
  {
    struct Content
    {
      enum Value
      {
        simple,
        complex,
        mixed
      };
    };

    //
    //
    class Context: public CXX::Context
    {
    public:
      typedef BackendElements::Regex::Expression<Char> Regex;

    public:
      Context (std::wostream&,
               SemanticGraph::Schema&,
               SemanticGraph::Path const&,
               CLI::Options const&,
               Regex const* hxx_expr,
               Regex const* ixx_expr,
               Regex const* hxx_impl_expr);

    protected:
      Context (Context& c)
          : CXX::Context (c),
            options (c.options),
            xml_parser (c.xml_parser),
            simple_base (c.simple_base),
            complex_base (c.complex_base),
            list_base (c.list_base),
            parser_map (c.parser_map),
            validation (c.validation),
            exceptions (c.exceptions),
            poly_code (c.poly_code),
            poly_runtime (c.poly_runtime),
            reset (c.reset),
            mixin (c.mixin),
            tiein (c.tiein),
            hxx_expr (c.hxx_expr),
            ixx_expr (c.ixx_expr),
            hxx_impl_expr (c.hxx_impl_expr)
      {
      }

      Context (Context& c, std::wostream& o)
          : CXX::Context (c, o),
            options (c.options),
            xml_parser (c.xml_parser),
            simple_base (c.simple_base),
            complex_base (c.complex_base),
            list_base (c.list_base),
            parser_map (c.parser_map),
            validation (c.validation),
            exceptions (c.exceptions),
            poly_code (c.poly_code),
            poly_runtime (c.poly_runtime),
            reset (c.reset),
            mixin (c.mixin),
            tiein (c.tiein),
            hxx_expr (c.hxx_expr),
            ixx_expr (c.ixx_expr),
            hxx_impl_expr (c.hxx_impl_expr)
      {
      }

    public:
      Boolean
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

      // Real (e.g., non-typedef) fq-name.
      //
      String
      real_fq_name (SemanticGraph::Nameable& n);

    public:
      static Content::Value
      content (SemanticGraph::Complex&);

    public:
      static String const&
      ret_type (SemanticGraph::Type&);

      static String const&
      arg_type (SemanticGraph::Type&);

      static String const&
      post_name (SemanticGraph::Type&);

    public:
      // Optional.
      //
      static String const&
      epresent (SemanticGraph::Compositor&);

      // Sequence.
      //
      static String const&
      enext (SemanticGraph::Sequence&);

      // Choice.
      //
      static String const&
      etag (SemanticGraph::Particle&);

      static String const&
      earm (SemanticGraph::Choice&);

      static String const&
      earm_tag (SemanticGraph::Choice&);

    public:
      static String const&
      eparser (SemanticGraph::Member&);

      static String const&
      emember (SemanticGraph::Member&);

      static String const&
      emember_cache (SemanticGraph::Member&);

      static String const&
      emember_map (SemanticGraph::Member&);

      static String const&
      etiein (SemanticGraph::Type&);

    public:
      static String const&
      eimpl (SemanticGraph::Type&);

    public:
      CLI::Options const& options;
      String& xml_parser;
      String& simple_base;
      String& complex_base;
      String& list_base;
      String& parser_map;

      Boolean validation;
      Boolean exceptions;
      Boolean poly_code;
      Boolean poly_runtime;
      Boolean reset;
      Boolean mixin;
      Boolean tiein;

      Regex const* hxx_expr;
      Regex const* ixx_expr;
      Regex const* hxx_impl_expr;

    private:
      String xml_parser_;
      String simple_base_;
      String complex_base_;
      String list_base_;
      String parser_map_;
    };

    // Check whether this is a string-based type (including ID, IDFER,
    // anyURI, and ENTITY).
    //
    struct StringBasedType: Traversal::Complex,
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
      StringBasedType (Boolean& r)
          : r_ (r)
      {
        *this >> inherits_ >> *this;
      }

      virtual Void
      traverse (SemanticGraph::Complex& c)
      {
        inherits (c, inherits_);
      }

      // Strings.
      //
      virtual Void
      traverse (SemanticGraph::Fundamental::String&)
      {
        r_ = true;
      }

      virtual Void
      traverse (SemanticGraph::Fundamental::NormalizedString&)
      {
        r_ = true;
      }

      virtual Void
      traverse (SemanticGraph::Fundamental::Token&)
      {
        r_ = true;
      }

      virtual Void
      traverse (SemanticGraph::Fundamental::NameToken&)
      {
        r_ = true;
      }

      virtual Void
      traverse (SemanticGraph::Fundamental::Name&)
      {
        r_ = true;
      }

      virtual Void
      traverse (SemanticGraph::Fundamental::NCName&)
      {
        r_ = true;
      }

      virtual Void
      traverse (SemanticGraph::Fundamental::Language&)
      {
        r_ = true;
      }

      virtual Void
      traverse (SemanticGraph::Fundamental::Id&)
      {
        r_ = true;
      }

      virtual Void
      traverse (SemanticGraph::Fundamental::IdRef&)
      {
        r_ = true;
      }

      virtual Void
      traverse (SemanticGraph::Fundamental::AnyURI&)
      {
        r_ = true;
      }

      virtual Void
      traverse (SemanticGraph::Fundamental::Entity&)
      {
        r_ = true;
      }

    private:
      Boolean& r_;
      Traversal::Inherits inherits_;
    };

    //
    //
    struct RequiredAttributeTest: Traversal::Attribute
    {
      RequiredAttributeTest (Boolean& result)
          : result_ (result)
      {
      }

      virtual Void
      traverse (Type& a)
      {
        if (!result_ && !a.optional ())
          result_ = true;
      }

    private:
      Boolean& result_;
    };


    //
    //
    struct ParticleParamDecl: Traversal::Element, Context
    {
      ParticleParamDecl (Context& c, Boolean& first, Boolean name_arg)
          : Context (c), first_ (first), name_arg_ (name_arg)
      {
      }

      virtual Void
      traverse (SemanticGraph::Element& e)
      {
        if (!first_)
          os << "," << endl;
        else
          first_ = false;

        os << fq_name (e.type ()) << "&";

        if (name_arg_)
          os << " " << ename (e);
        else
          os << " /* " << comment (e.name ()) << " */";
      }

    private:
      Boolean& first_;
      Boolean name_arg_;
    };

    struct AttributeParamDecl: Traversal::Attribute, Context
    {
      AttributeParamDecl (Context& c, Boolean& first, Boolean name_arg)
          : Context (c), first_ (first), name_arg_ (name_arg)
      {
      }

      virtual Void
      traverse (Type& a)
      {
        if (!first_)
          os << "," << endl;
        else
          first_ = false;

        os << fq_name (a.type ()) << "&";

        if (name_arg_)
          os << " " << ename (a);
        else
          os << " /* " << comment (a.name ()) << " */";
      }

    private:
      Boolean& first_;
      Boolean name_arg_;
    };

    struct ParserParamDecl : Traversal::Complex,
                             Traversal::List,
                             Context
    {
      ParserParamDecl (Context& c, Boolean name_arg)
          : Context (c),
            particle_ (c, first_, name_arg),
            attribute_ (c, first_, name_arg),
            first_ (true),
            name_arg_ (name_arg)
      {
        inherits_ >> *this;

        contains_compositor_ >> compositor_ >> contains_particle_;
        contains_particle_ >> particle_;
        contains_particle_ >> compositor_;

        names_ >> attribute_;
      }

      virtual Void
      traverse (SemanticGraph::Complex& c)
      {
        inherits (c, inherits_);

        if (!restriction_p (c))
        {
          names (c, names_);
          contains_compositor (c, contains_compositor_);
        }
      }

      virtual Void
      traverse (SemanticGraph::List& l)
      {
        if (!first_)
          os << "," << endl;
        else
          first_ = false;

        os << fq_name (l.argumented ().type ()) << "&";

        if (name_arg_)
          os << " " << ename (l) << "_item";
        else
          os << " /* " << comment (l.name ()) << " item */";
      }

    private:
      Traversal::Inherits inherits_;

      Traversal::Compositor compositor_;
      ParticleParamDecl particle_;
      Traversal::ContainsCompositor contains_compositor_;
      Traversal::ContainsParticle contains_particle_;

      AttributeParamDecl attribute_;
      Traversal::Names names_;

      Boolean first_;
      Boolean name_arg_;
    };

    //
    //
    struct TypeForward: Traversal::Type, Context
    {
      TypeForward (Context& c, Char const* name_key)
          : Context (c), name_key_ (name_key)
      {
      }

      virtual Void
      traverse (SemanticGraph::Type& t);

    private:
      Char const* name_key_;
    };

    struct Includes : Traversal::Imports,
                      Traversal::Includes
    {
      enum Type
      {
        header,
        source,
        impl_header
      };

      Includes (Context& c, Type t)
          : ctx_ (c),
            type_ (t),
            namespace_ (c),
            type_forward_ (c, t == header ? "p:name" : "p:impl")
      {
        schema_ >> schema_names_ >> namespace_ >> names_ >> type_forward_;
      }

      virtual Void
      traverse (SemanticGraph::Imports& i)
      {
        traverse_ (i);
      }

      virtual Void
      traverse (SemanticGraph::Includes& i)
      {
        traverse_ (i);
      }

    private:
      Void
      traverse_ (SemanticGraph::Uses&);

    private:
      Context& ctx_;
      Type type_;

      Traversal::Schema schema_;
      Traversal::Names schema_names_;
      Namespace namespace_;
      Traversal::Names names_;
      TypeForward type_forward_;
    };

    // Find root element for the test driver.
    //
    struct RootElement: Traversal::Element
    {
      RootElement (CLI::Options const& options,
                   SemanticGraph::Element*& element)
          : options_ (options), element_ (element)
      {
      }

      virtual Void
      traverse (Type& e)
      {
        if (options_.value<CLI::root_element_first> ())
        {
          if (element_ == 0)
            element_ = &e;
        }
        else if (String name = options_.value<CLI::root_element> ())
        {
          if (e.name () == name)
            element_ = &e;
        }
        else
          element_ = &e; // Cover root-element-last and no option.
      }

    private:
      CLI::Options const& options_;
      SemanticGraph::Element*& element_;
    };
  }
}

#endif  // CXX_PARSER_ELEMENTS_HXX
