// file      : xsde/cxx/serializer/elements.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/serializer/elements.hxx>

namespace CXX
{
  namespace Serializer
  {
    Context::
    Context (std::wostream& o,
             SemanticGraph::Schema& root,
             SemanticGraph::Path const& path,
             CLI::Options const& ops,
             Regex const* he,
             Regex const* ie,
             Regex const* hie)
        : CXX::Context (o,
                        root,
                        path,
                        "s:name",
                        "char",
                        ops.value<CLI::char_encoding> (),
                        ops.value<CLI::include_with_brackets> (),
                        ops.value<CLI::include_prefix> (),
                        "", // export symbol
                        ops.value<CLI::namespace_map> (),
                        ops.value<CLI::namespace_regex> (),
                        ops.value<CLI::namespace_regex_trace> (),
                        ops.value<CLI::include_regex> (),
                        ops.value<CLI::include_regex_trace> (),
                        ops.value<CLI::generate_inline> (),
                        ops.value<CLI::reserved_name> ()),
          options (ops),
          xml_serializer (xml_serializer_),
          serializer_base (serializer_base_),
          simple_base (simple_base_),
          complex_base (complex_base_),
          serializer_map (serializer_map_),
          validation (!ops.value<CLI::suppress_validation> ()),
          exceptions (!ops.value<CLI::no_exceptions> ()),
          stl (!ops.value<CLI::no_stl> ()),
          poly_code (ops.value<CLI::generate_polymorphic> ()),
          poly_runtime (poly_code || ops.value<CLI::runtime_polymorphic> ()),
          reset (!ops.value<CLI::suppress_reset> ()),
          mixin (ops.value<CLI::reuse_style_mixin> ()),
          tiein (!mixin && !ops.value<CLI::reuse_style_none> ()),
          hxx_expr (he),
          ixx_expr (ie),
          hxx_impl_expr (hie),
          xml_serializer_ ("genx")
    {
      String xs_ns (xs_ns_name ());

      string_type = L"::xsde::cxx::ro_string";

      serializer_base_ = xs_ns + L"::serializer_base";

      simple_base_ = L"::xsde::cxx::serializer::";
      simple_base_ += (validation ? L"validating" : L"non_validating");
      simple_base_ += L"::simple_content";

      complex_base_ = L"::xsde::cxx::serializer::";
      complex_base_ += (validation ? L"validating" : L"non_validating");
      complex_base_ += L"::complex_content";

      if (poly_code)
        serializer_map_ = xs_ns + L"::serializer_map";
    }

    String Context::
    real_fq_name (SemanticGraph::Nameable& n)
    {
      SemanticGraph::Context& c (n.context ());

      if (c.count ("s:real-name"))
        return c.get<String> ("s:real-name");
      else
        return fq_name (n);
    }

    Content::Value Context::
    content (SemanticGraph::Complex& c)
    {
      using namespace SemanticGraph;

      if (c.mixed_p ())
        return Content::mixed;

      if (c.inherits_p ())
      {
        Type& base (c.inherits ().base ());

        if (Complex* cb = dynamic_cast<Complex*> (&base))
          return content (*cb);

        if (base.is_a<AnyType> ())
          return Content::complex;

        // Everyhting else (built-in type and AnySimpleType) is simple
        // content.
        //
        return Content::simple;
      }
      else
        return Content::complex;
    }

    String const& Context::
    ret_type (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("s:ret-type");
    }

    String const& Context::
    arg_type (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("s:arg-type");
    }

    //
    //
    String const& Context::
    epresent (SemanticGraph::Particle& p)
    {
      return p.context ().get<String> ("s:present");
    }

    String const& Context::
    epresent (SemanticGraph::Attribute& a)
    {
      return a.context ().get<String> ("s:present");
    }

    String const& Context::
    enext (SemanticGraph::Particle& p)
    {
      return p.context ().get<String> ("s:next");
    }

    String const& Context::
    enext (SemanticGraph::AnyAttribute& a)
    {
      return a.context ().get<String> ("s:next");
    }

    String const& Context::
    etag (SemanticGraph::Particle& p)
    {
      return p.context ().get<String> ("s:tag");
    }

    String const& Context::
    earm (SemanticGraph::Choice& c)
    {
      return c.context ().get<String> ("s:arm");
    }

    String const& Context::
    earm_tag (SemanticGraph::Choice& c)
    {
      return c.context ().get<String> ("s:arm-tag");
    }

    //
    //
    String const& Context::
    eserializer (SemanticGraph::Member& m)
    {
      return m.context ().get<String> ("s:serializer");
    }

    String const& Context::
    emember (SemanticGraph::Member& m)
    {
      return m.context ().get<String> ("s:member");
    }

    String const& Context::
    emember_map (SemanticGraph::Member& m)
    {
      return m.context ().get<String> ("s:member-map");
    }

    String const& Context::
    etiein (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("s:tiein");
    }

    //
    //
    String const& Context::
    eserialize (SemanticGraph::Any& a)
    {
      return a.context ().get<String> ("s:serialize");
    }

    String const& Context::
    eserialize (SemanticGraph::AnyAttribute& a)
    {
      return a.context ().get<String> ("s:serialize");
    }

    //
    //
    String const& Context::
    eimpl (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("s:impl");
    }

    // Includes
    //
    Void TypeForward::
    traverse (SemanticGraph::Type& t)
    {
      os << "class " << t.context ().get<String> (name_key_) << ";";
    }

    Void Includes::
    traverse_ (SemanticGraph::Uses& u)
    {
      // Support for weak (forward) inclusion used in the file-per-type
      // compilation model.
      //
      SemanticGraph::Schema& s (u.schema ());
      Boolean weak (u.context ().count ("weak"));

      if (weak && (type_ == header || type_ == impl_header))
      {
        // Generate forward declarations. We don't really need them
        // in the impl files.
        //
        if (type_ == header)
          schema_.dispatch (s);

        return;
      }

      if (type_ == source && !weak)
        return;

      SemanticGraph::Path path (
        s.context ().count ("renamed")
        ? s.context ().get<SemanticGraph::Path> ("renamed")
        : u.path ());

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

      String inc_path;

      switch (type_)
      {
      case header:
      case source:
        {
          inc_path = ctx_.hxx_expr->merge (path_str);
          break;
        }
      case impl_header:
        {
          inc_path = ctx_.hxx_impl_expr->merge (path_str);
          break;
        }
      }

      ctx_.os << "#include " << ctx_.process_include_path (inc_path) << endl
              << endl;
    }
  }
}
