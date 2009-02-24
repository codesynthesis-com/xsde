// file      : xsde/cxx/parser/non-validating/idrefs-stl.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/pre.hxx>

#include <xsde/cxx/config.hxx>

#include <xsde/cxx/parser/non-validating/idrefs-stl.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace non_validating
      {
        idrefs_pimpl::
        ~idrefs_pimpl ()
        {
          if (!base_)
            delete seq_;
        }

        void idrefs_pimpl::
        _reset ()
        {
          idrefs_pskel::_reset ();

          if (!base_)
          {
            delete seq_;
            seq_ = 0;
          }

          parser_._reset ();
        }

        idrefs_pimpl::
        idrefs_pimpl (bool base)
            : base_ (base), seq_ (0)
        {
        }

        void idrefs_pimpl::
        pre_impl (string_sequence* seq)
        {
          seq_ = seq;
        }

        void idrefs_pimpl::
        _pre ()
        {
          if (seq_ == 0)
          {
            seq_ = new string_sequence ();

#ifndef XSDE_EXCEPTIONS
            if (seq_ == 0)
            {
              _sys_error (sys_error::no_memory);
              return;
            }
#endif
          }

          idrefs_pskel::_pre ();
        }

        string_sequence* idrefs_pimpl::
        post_idrefs ()
        {
          string_sequence* r = seq_;
          seq_ = 0;
          return r;
        }

        void idrefs_pimpl::
        _xsde_parse_item (const ro_string& s)
        {
          parser_.pre ();
          parser_._pre_impl (_context ());
          parser_._characters (s);
          parser_._post_impl ();
          seq_->push_back (parser_.post_idref ());
        }
      }
    }
  }
}

#include <xsde/cxx/post.hxx>
