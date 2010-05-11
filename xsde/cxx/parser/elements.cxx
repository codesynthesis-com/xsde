// file      : xsde/cxx/parser/elements.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/parser/elements.hxx>

namespace CXX
{
  namespace Parser
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
                        "p:name",
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
                        ops.value<CLI::custom_allocator> (),
                        ops.value<CLI::reserved_name> ()),
          options (ops),
          xml_parser (xml_parser_),
          simple_base (simple_base_),
          complex_base (complex_base_),
          list_base (list_base_),
          parser_map (parser_map_),
          validation (!ops.value<CLI::suppress_validation> ()),
          exceptions (!ops.value<CLI::no_exceptions> ()),
          poly_code (ops.value<CLI::generate_polymorphic> ()),
          poly_runtime (poly_code || ops.value<CLI::runtime_polymorphic> ()),
          reset (!ops.value<CLI::suppress_reset> ()),
          mixin (ops.value<CLI::reuse_style_mixin> ()),
          tiein (!mixin && !ops.value<CLI::reuse_style_none> ()),
          hxx_expr (he),
          ixx_expr (ie),
          hxx_impl_expr (hie),
          xml_parser_ ("expat")
    {
      string_type = L"::xsde::cxx::ro_string";

      simple_base_ = L"::xsde::cxx::parser::";
      simple_base_ += (validation ? L"validating" : L"non_validating");
      simple_base_ += L"::simple_content";

      complex_base_ = L"::xsde::cxx::parser::";
      complex_base_ += (validation ? L"validating" : L"non_validating");
      complex_base_ += L"::complex_content";

      list_base_ = L"::xsde::cxx::parser::";
      list_base_ += (validation ? L"validating" : L"non_validating");
      list_base_ += L"::list_base";

      if (poly_code)
        parser_map_ = xs_ns_name () + L"::parser_map";
    }

    String Context::
    real_fq_name (SemanticGraph::Nameable& n)
    {
      SemanticGraph::Context& c (n.context ());

      if (c.count ("p:real-name"))
        return c.get<String> ("p:real-name");
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
      return t.context ().get<String> ("p:ret-type");
    }

    String const& Context::
    arg_type (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("p:arg-type");
    }

    String const& Context::
    post_name (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("p:post");
    }

    String const& Context::
    epresent (SemanticGraph::Compositor& c)
    {
      return c.context ().get<String> ("p:present");
    }

    String const& Context::
    enext (SemanticGraph::Sequence& s)
    {
      return s.context ().get<String> ("p:next");
    }

    String const& Context::
    etag (SemanticGraph::Particle& p)
    {
      return p.context ().get<String> ("p:tag");
    }

    String const& Context::
    earm (SemanticGraph::Choice& c)
    {
      return c.context ().get<String> ("p:arm");
    }

    String const& Context::
    earm_tag (SemanticGraph::Choice& c)
    {
      return c.context ().get<String> ("p:arm-tag");
    }

    String const& Context::
    eparser (SemanticGraph::Member& m)
    {
      return m.context ().get<String> ("p:parser");
    }

    String const& Context::
    emember (SemanticGraph::Member& m)
    {
      return m.context ().get<String> ("p:member");
    }

    String const& Context::
    emember_cache (SemanticGraph::Member& m)
    {
      return m.context ().get<String> ("p:member-cache");
    }

    String const& Context::
    emember_map (SemanticGraph::Member& m)
    {
      return m.context ().get<String> ("p:member-map");
    }

    String const& Context::
    etiein (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("p:tiein");
    }

    String const& Context::
    eimpl (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("p:impl");
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
