// file      : xsde/cxx/parser/validating/idrefs-stl.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/pre.hxx>

#include <xsde/cxx/config.hxx>

#ifdef XSDE_CUSTOM_ALLOCATOR
#  include <xsde/cxx/allocator.hxx>
#endif

#include <xsde/cxx/parser/validating/idrefs-stl.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        idrefs_pimpl::
        ~idrefs_pimpl ()
        {
          if (!base_ && seq_)
          {
#ifndef XSDE_CUSTOM_ALLOCATOR
            delete seq_;
#else
            seq_->~string_sequence ();
            cxx::free (seq_);
#endif
          }
        }

        void idrefs_pimpl::
        _reset ()
        {
          idrefs_pskel::_reset ();

          if (!base_ && seq_)
          {
#ifndef XSDE_CUSTOM_ALLOCATOR
            delete seq_;
#else
            seq_->~string_sequence ();
            cxx::free (seq_);
#endif
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
#ifndef XSDE_CUSTOM_ALLOCATOR
            seq_ = new string_sequence ();
#else
            seq_ = static_cast<string_sequence*> (
              alloc (sizeof (string_sequence)));

#ifdef XSDE_EXCEPTIONS
            alloc_guard ag (seq_);
            new (seq_) string_sequence ();
            ag.release ();
#else
            if (seq_)
              new (seq_) string_sequence ();
#endif
#endif

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

        void idrefs_pimpl::
        _post ()
        {
          idrefs_pskel::_post ();

          // Should have at least one element.
          //
          if (!_context ().error_type () && seq_->size () < 1)
            _schema_error (schema_error::invalid_idrefs_value);
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
          context& ctx = _context ();

          idref_pskel& p = parser_;

          p.pre ();

#ifndef XSDE_EXCEPTIONS
          if (!p._error_type ())
            p._pre_impl (ctx);
          else
            p._copy_error (ctx);
#else
          p._pre_impl (ctx);
#endif
          if (!ctx.error_type ())
            p._characters (s);

          if (!ctx.error_type ())
            p._post_impl ();

          if (!ctx.error_type ())
          {
            const std::string& x = p.post_idref ();

#ifndef XSDE_EXCEPTIONS
            if (!p._error_type ())
              seq_->push_back (x);
            else
              p._copy_error (ctx);
#else
            seq_->push_back (x);
#endif
          }
        }
      }
    }
  }
}

#include <xsde/cxx/post.hxx>
