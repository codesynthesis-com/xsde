// file      : xsde/cxx/hybrid/elements.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/hybrid/elements.hxx>

namespace CXX
{
  namespace Hybrid
  {
    Context::
    Context (std::wostream& o,
             SemanticGraph::Schema& root,
             CLI::Options const& ops,
             Regex const* fe,
             Regex const* he,
             Regex const* ie)
        : CXX::Context (o,
                        root,
                        "name",
                        "char",
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
          exceptions (!ops.value<CLI::no_exceptions> ()),
          stl (!ops.value<CLI::no_stl> ()),
          poly_code (false),
          poly_runtime (false),
          reset (!ops.value<CLI::suppress_reset> ()),
          mixin (ops.value<CLI::reuse_style_mixin> ()),
          tiein (!mixin),
          fwd_expr (fe),
          hxx_expr (he),
          ixx_expr (ie),
          ns_stack (ns_stack_)
    {
      String xs_ns (xs_ns_name ());

      string_type = L"::xsde::cxx::ro_string";
    }

    // Parser
    //
    String const& Context::
    pret_type (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("p:ret-type");
    }

    String const& Context::
    parg_type (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("p:arg-type");
    }

    String const& Context::
    post_name (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("p:post");
    }

    static String pre_impl ("pre_impl");

    String const& Context::
    pre_impl_name (SemanticGraph::Type&)
    {
      // @@ Currently not assigned because we don't assign names to
      // types in included/imported schemas which makes assigning
      // this name correctly impossible.
      //
      return pre_impl;
      // return t.context ().get<String> ("ppre-impl");
    }

    String const& Context::
    epname (SemanticGraph::Particle& p)
    {
      return p.context ().get<String> ("p:name");
    }

    String const& Context::
    epname (SemanticGraph::Attribute& a)
    {
      return a.context ().get<String> ("p:name");
    }

    String const& Context::
    epimpl (SemanticGraph::Type& t)
    {
      // Use p:impl instead of pimpl because C++/Parser assigns impl
      // names to the built-in types.
      //
      return t.context ().get<String> ("p:impl");
    }

    String const& Context::
    epimpl_custom (SemanticGraph::Type& t)
    {
      SemanticGraph::Context& c (t.context ());

      if (!c.count ("p:impl-base"))
        return c.get<String> ("p:impl");
      else
        return c.get<String> ("p:impl-base");
    }

    String const& Context::
    epstate (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("pstate");
    }

    String const& Context::
    epstate_type (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("pstate-type");
    }

    String const& Context::
    epstate_member (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("pstate-member");
    }

    String const& Context::
    epstate_base (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("pstate-base");
    }

    String const& Context::
    epstate_member (SemanticGraph::Compositor& c)
    {
      return c.context ().get<String> ("pstate-member");
    }

    String const& Context::
    epskel (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("p:name");
    }

    String const& Context::
    eppresent (SemanticGraph::Compositor& c)
    {
      return c.context ().get<String> ("p:present");
    }

    String const& Context::
    epnext (SemanticGraph::Compositor& c)
    {
      return c.context ().get<String> ("p:next");
    }

    String const& Context::
    eptag (SemanticGraph::Particle& p)
    {
      return p.context ().get<String> ("p:tag");
    }

    String const& Context::
    eparm (SemanticGraph::Choice& c)
    {
      return c.context ().get<String> ("p:arm");
    }

    String const& Context::
    eparm_tag (SemanticGraph::Choice& c)
    {
      return c.context ().get<String> ("p:arm-tag");
    }

    // Serializer
    //
    String const& Context::
    sret_type (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("s:ret-type");
    }

    String const& Context::
    sarg_type (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("s:arg-type");
    }

    String const& Context::
    esname (SemanticGraph::Particle& p)
    {
      return p.context ().get<String> ("s:name");
    }

    String const& Context::
    esname (SemanticGraph::Attribute& a)
    {
      return a.context ().get<String> ("s:name");
    }

    String const& Context::
    esimpl (SemanticGraph::Type& t)
    {
      // Use s:impl instead of simpl because C++/Serializer assigns impl
      // names to the built-in types.
      //
      return t.context ().get<String> ("s:impl");
    }

    String const& Context::
    esimpl_custom (SemanticGraph::Type& t)
    {
      SemanticGraph::Context& c (t.context ());

      if (!c.count ("s:impl-base"))
        return c.get<String> ("s:impl");
      else
        return c.get<String> ("s:impl-base");
    }

    String const& Context::
    esstate (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("sstate");
    }

    String const& Context::
    esstate_type (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("sstate-type");
    }

    String const& Context::
    esstate_member (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("sstate-member");
    }

    String const& Context::
    esstate_member (SemanticGraph::Particle& p)
    {
      return p.context ().get<String> ("sstate-member");
    }

    String const& Context::
    esstate_member_end (SemanticGraph::Particle& p)
    {
      return p.context ().get<String> ("sstate-member-end");
    }

    String const& Context::
    esskel (SemanticGraph::Type& t)
    {
      return t.context ().get<String> ("s:name");
    }

    String const& Context::
    espresent (SemanticGraph::Particle& p)
    {
      return p.context ().get<String> ("s:present");
    }

    String const& Context::
    espresent (SemanticGraph::Attribute& a)
    {
      return a.context ().get<String> ("s:present");
    }

    String const& Context::
    esnext (SemanticGraph::Particle& p)
    {
      return p.context ().get<String> ("s:next");
    }

    String const& Context::
    estag (SemanticGraph::Particle& p)
    {
      return p.context ().get<String> ("s:tag");
    }

    String const& Context::
    esarm (SemanticGraph::Choice& c)
    {
      return c.context ().get<String> ("s:arm");
    }

    String const& Context::
    esarm_tag (SemanticGraph::Choice& c)
    {
      return c.context ().get<String> ("s:arm-tag");
    }

    //
    //
    String Context::
    scope (SemanticGraph::Compositor& c, Boolean fq) const
    {
      using namespace SemanticGraph;

      String r;

      Compositor* p (&c);

      while (!p->contained_compositor_p ())
      {
        p = &p->contained_particle ().compositor ();

        if (!p->context ().count ("type"))
          continue; // See-through compositor.

        if (!r)
          r = etype (*p);
        else
        {
          String tmp;
          tmp.swap (r);
          r = etype (*p);
          r += L"::";
          r += tmp;
        }
      }

      Complex& t (
        dynamic_cast<Complex&> (
          p->contained_compositor ().container ()));

      if (!r)
        r = fq ? fq_name (t) : ename (t);
      else
      {
        String tmp;
        tmp.swap (r);
        r = fq ? fq_name (t) : ename (t);
        r += L"::";
        r += tmp;
      }

      return r;
    }

    String Context::
    scope (SemanticGraph::Element& e, Boolean fq) const
    {
      SemanticGraph::Compositor& c (e.contained_particle ().compositor ());

      if (!c.context ().count ("type"))
        return scope (c, fq);
      else
        return scope (c, fq) + L"::" + etype (c);
    }

    String Context::
    scope (SemanticGraph::Attribute& a, Boolean fq) const
    {
      using SemanticGraph::Complex;

      Complex& t (dynamic_cast<Complex&> (a.scope ()));
      return fq ? fq_name (t) : ename (t);
    }

    Void Context::
    close_ns ()
    {
      for (Size i (0), n (ns_stack.size () - 1); i < n; ++i)
        os << "}";
    }

    Void Context::
    open_ns ()
    {
      for (NamespaceStack::Iterator i (ns_stack.begin () + 1);
           i != ns_stack.end ();
           ++i)
      {
        os << "namespace " << *i
           << "{";
      }
    }

    // Namespace
    //
    Namespace::
    Namespace (Context& c, Boolean track_scope)
        : CXX::Namespace (c, track_scope ? this : 0), ctx_ (c)
    {
    }

    Void Namespace::
    enter (String const& name)
    {
      ctx_.ns_stack.push_back (name);
    }

    Void Namespace::
    leave ()
    {
      ctx_.ns_stack.pop_back ();
    }

    // Includes
    //
    Void TypeForward::
    traverse (SemanticGraph::Type& t)
    {
      os << "class " << ename (t) << ";";
    }

    Void Includes::
    traverse_ (SemanticGraph::Uses& u)
    {
      // Support for weak (forward) inclusion used in the file-per-type
      // compilation model.
      //
      Type t (type_);
      Boolean weak (u.context ().count ("weak"));

      if (weak && (t == header || t == impl_header))
      {
        // Generate forward declarations. We don't really need them
        // in the impl files.
        //
        if (t == impl_header)
          return;

        if (forward_)
          t = forward;
        else
        {
          schema_.dispatch (u.schema ());
          return;
        }
      }

      if (t == source && !weak)
        return;

      SemanticGraph::Path path (u.path ());

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

      switch (t)
      {
      case forward:
        {
          inc_path = ctx_.fwd_expr->merge (path_str);
          break;
        }
      case header:
      case impl_header:
      case source:
        {
          inc_path = ctx_.hxx_expr->merge (path_str);
          break;
        }
      case inline_:
        {
          if (weak)
          {
            inc_path = ctx_.hxx_expr->merge (path_str);
            ctx_.os << "#include " << ctx_.process_include_path (inc_path)
                    << endl;
          }

          inc_path = ctx_.ixx_expr->merge (path_str);
          break;
        }
      }

      ctx_.os << "#include " << ctx_.process_include_path (inc_path) << endl
              << endl;
    }
  }
}
