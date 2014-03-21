// file      : xsde/cxx/serializer/elements.hxx
// copyright : Copyright (c) 2005-2014 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_SERIALIZER_ELEMENTS_HXX
#define CXX_SERIALIZER_ELEMENTS_HXX

#include <sstream>

#include <cutl/re.hxx>

#include <cxx/elements.hxx>

#include <cxx/serializer/options.hxx>

namespace CXX
{
  namespace Serializer
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
      typedef cutl::re::regexsub Regex;

      typedef Serializer::options options_type;

    public:
      Context (std::wostream&,
               SemanticGraph::Schema&,
               SemanticGraph::Path const&,
               options_type const&,
               Regex const* hxx_expr,
               Regex const* ixx_expr,
               Regex const* hxx_impl_expr);

    protected:
      Context (Context& c)
          : CXX::Context (c),
            options (c.options),
            xml_serializer (c.xml_serializer),
            serializer_base (c.serializer_base),
            simple_base (c.simple_base),
            complex_base (c.complex_base),
            serializer_map (c.serializer_map),
            validation (c.validation),
            exceptions (c.exceptions),
            stl (c.stl),
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
            xml_serializer (c.xml_serializer),
            serializer_base (c.serializer_base),
            simple_base (c.simple_base),
            complex_base (c.complex_base),
            serializer_map (c.serializer_map),
            validation (c.validation),
            exceptions (c.exceptions),
            stl (c.stl),
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

    public:
      // Optional.
      //
      static String const&
      epresent (SemanticGraph::Particle&);

      static String const&
      epresent (SemanticGraph::Attribute&);

      // Sequence.
      //
      static String const&
      enext (SemanticGraph::Particle&);

      static String const&
      enext (SemanticGraph::AnyAttribute&);

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
      eserializer (SemanticGraph::Member&);

      static String const&
      emember (SemanticGraph::Member&);

      static String const&
      emember_map (SemanticGraph::Member&);

      static String const&
      etiein (SemanticGraph::Type&);

      // serialize_*
      //
    public:
      static String const&
      eserialize (SemanticGraph::Any&);

      static String const&
      eserialize (SemanticGraph::AnyAttribute&);

    public:
      static String const&
      eimpl (SemanticGraph::Type&);

    public:
      bool
      has_facets (SemanticGraph::Complex& c);

    public:
      options_type const& options;
      String& xml_serializer;
      String& serializer_base;
      String& simple_base;
      String& complex_base;
      String& serializer_map;

      bool validation;
      bool exceptions;
      bool stl;
      bool poly_code;
      bool poly_runtime;
      bool reset;
      bool mixin;
      bool tiein;

      Regex const* hxx_expr;
      Regex const* ixx_expr;
      Regex const* hxx_impl_expr;

    private:
      String xml_serializer_;
      String serializer_base_;
      String simple_base_;
      String complex_base_;
      String serializer_map_;
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
      Traversal::Inherits inherits_;
    };


    //
    //
    struct RequiredAttributeTest: Traversal::Attribute
    {
      RequiredAttributeTest (bool& result)
          : result_ (result)
      {
      }

      virtual void
      traverse (Type& a)
      {
        if (!result_ && !a.optional_p ())
          result_ = true;
      }

    private:
      bool& result_;
    };


    //
    //
    struct ParticleParamDecl: Traversal::Element, Context
    {
      ParticleParamDecl (Context& c,
                         bool& first,
                         bool name_arg,
                         bool poly)
          : Context (c),
            first_ (first),
            name_arg_ (name_arg),
            poly_ (poly),
            result_ (0)
      {
      }

      ParticleParamDecl (Context& c, bool* result, bool poly)
          : Context (c), first_ (name_arg_), poly_ (poly), result_ (result)
      {
      }

      virtual void
      traverse (SemanticGraph::Element& e)
      {
        if (poly_ && anonymous (e.type ()))
          return;

        if (result_ != 0)
        {
          *result_ = true;
          return;
        }

        if (!first_)
          os << "," << endl;
        else
          first_ = false;

        if (poly_)
          os << serializer_map << "&";
        else
          os << fq_name (e.type ()) << "&";

        if (name_arg_)
          os << " " << ename (e);
        else
          os << " /* " << comment (e.name ()) << " */";
      }

    private:
      bool& first_;
      bool name_arg_;
      bool poly_;
      bool* result_;
    };

    struct AttributeParamDecl: Traversal::Attribute, Context
    {
      AttributeParamDecl (Context& c, bool& first, bool name_arg)
          : Context (c),
            first_ (first),
            name_arg_ (name_arg),
            result_ (0)
      {
      }

      AttributeParamDecl (Context& c, bool* result)
          : Context (c), first_ (name_arg_), result_ (result)
      {
      }

      virtual void
      traverse (Type& a)
      {
        if (result_ != 0)
        {
          *result_ = true;
          return;
        }

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
      bool& first_;
      bool name_arg_;
      bool* result_;
    };

    struct SerializerParamDecl : Traversal::Complex,
                             Traversal::List,
                             Context
    {
      SerializerParamDecl (Context& c, bool name_arg, bool poly)
          : Context (c),
            particle_ (c, first_, name_arg, poly),
            attribute_ (c, first_, name_arg),
            first_ (true),
            name_arg_ (name_arg),
            poly_ (poly),
            result_ (0)
      {
        inherits_ >> *this;

        contains_compositor_ >> compositor_ >> contains_particle_;
        contains_particle_ >> particle_;
        contains_particle_ >> compositor_;

        if (!poly_)
          names_ >> attribute_;
      }

      SerializerParamDecl (Context& c, bool* result, bool poly)
          : Context (c),
            particle_ (c, result, poly),
            attribute_ (c, result),
            poly_ (poly),
            result_ (result)
      {
        inherits_ >> *this;

        contains_compositor_ >> compositor_ >> contains_particle_;
        contains_particle_ >> particle_;
        contains_particle_ >> compositor_;

        if (!poly_)
          names_ >> attribute_;
      }

      virtual void
      traverse (SemanticGraph::Complex& c)
      {
        inherits (c, inherits_);

        if (!restriction_p (c))
        {
          names (c, names_);
          contains_compositor (c, contains_compositor_);
        }
      }

      virtual void
      traverse (SemanticGraph::List& l)
      {
        if (poly_)
          return;

        if (result_ != 0)
        {
          *result_ = true;
          return;
        }

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

      bool first_;
      bool name_arg_;
      bool poly_;
      bool* result_;
    };

    struct SerializerParamTest
    {
      SerializerParamTest (Context& c, bool& result, bool poly)
          : impl_ (c, &result, poly)
      {
      }

      void
      traverse (SemanticGraph::Complex& c)
      {
        impl_.traverse (c);
      }

    private:
      SerializerParamDecl impl_;
    };

    //
    //
    struct TypeForward: Traversal::Type, Context
    {
      TypeForward (Context& c, char const* name_key)
          : Context (c), name_key_ (name_key)
      {
      }

      virtual void
      traverse (SemanticGraph::Type& t);

    private:
      char const* name_key_;
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
            type_forward_ (c, t == header ? "s:name" : "s:impl")
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
      RootElement (options const& o, SemanticGraph::Element*& e)
          : options_ (o), element_ (e)
      {
      }

      virtual void
      traverse (Type& e)
      {
        if (options_.root_element_first ())
        {
          if (element_ == 0)
            element_ = &e;
        }
        else if (String name = options_.root_element ())
        {
          if (e.name () == name)
            element_ = &e;
        }
        else
          element_ = &e; // Cover root-element-last and no option.
      }

    private:
      options const& options_;
      SemanticGraph::Element*& element_;
    };
  }
}

#endif  // CXX_SERIALIZER_ELEMENTS_HXX
