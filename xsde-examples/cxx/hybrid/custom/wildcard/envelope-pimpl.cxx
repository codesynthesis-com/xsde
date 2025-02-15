// file      : cxx/hybrid/custom/wildcard/envelope-pimpl.cxx
// copyright : not copyrighted - public domain

#include <memory> // std::{unique,auto}_ptr

#include "body.hxx"

// Include email-pimpl.hxx (which includes envelope-pimpl.hxx)
// instead of envelope-pimpl.hxx.
//
#include "email-pimpl.hxx"

namespace email
{
  void envelope_pimpl::
#ifndef XSDE_POLYMORPHIC
  _start_any_element (const xml_schema::ro_string& ns,
                      const xml_schema::ro_string& name)
#else
  _start_any_element (const xml_schema::ro_string& ns,
                      const xml_schema::ro_string& name,
                      const char*)
#endif
  {
    // We use the depth_ counter to filter out nested elements
    // and attributes for the content matched by the wildcard
    // but which we don't know how to handle.
    //
    if (depth_++ == 0)
    {
      // Top-level (relative to this type) element matched by the
      // any wildcard.
      //
      xml_schema::parser_base* p = 0;

      if (ns == "http://www.codesynthesis.com/email")
      {
        if (name == "text")
          p = text_p_;
        else if (name == "binary")
          p = binary_p_;

        if (p != 0)
        {
          // If you are not using exceptions then you will need to add
          // error propagation code. For more information on how to do
          // this see the wildcard example for the C++/Parser mapping
          // (examples/parser/ directory).
          //
          xml_schema::parser_context& ctx = _context ();
          p->pre ();
          ctx.nested_parser (p);
        }
      }
    }
  }

  void envelope_pimpl::
  _end_any_element (const xml_schema::ro_string& ns,
                    const xml_schema::ro_string& name)
  {
    if (--depth_ == 0)
    {
      if (ns == "http://www.codesynthesis.com/email")
      {
        envelope* env = envelope_base_pimpl_state_.envelope_;

        if (name == "text")
        {
#ifdef XSDE_CXX11
          std::unique_ptr<body> b (new body);
#else
          std::auto_ptr<body> b (new body);
#endif

          b->text (text_p_->post_string ());
          env->body ().push_back (b.release ());
        }
        else if (name == "binary")
        {
#ifdef XSDE_CXX11
          std::unique_ptr<body> b (new body);
#else
          std::auto_ptr<body> b (new body);
#endif

          b->binary (binary_p_->post_binary ());
          env->body ().push_back (b.release ());
        }
      }
    }
  }

  void envelope_pimpl::
  _reset ()
  {
    // Note that we always need to call _reset() from the base.
    //
    envelope_base_pimpl::_reset ();

    depth_ = 0;
    text_p_->_reset ();
    binary_p_->_reset ();
  }
}
