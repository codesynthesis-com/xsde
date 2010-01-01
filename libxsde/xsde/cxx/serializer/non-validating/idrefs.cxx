// file      : xsde/cxx/serializer/non-validating/idrefs.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/non-validating/idrefs.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        idrefs_simpl::
        ~idrefs_simpl ()
        {
          if (free_)
            delete const_cast<string_sequence*> (value_);
        }

        void idrefs_simpl::
        pre (const string_sequence* value)
        {
          value_ = value;
        }

        void idrefs_simpl::
        _serialize_content ()
        {
          bool first = true;
          context& ctx = _context ();

          for (string_sequence::const_iterator i (value_->begin ()),
                 e (value_->end ()); i != e; ++i)
          {
#ifdef XSDE_EXCEPTIONS
            serializer_.pre (*i);

            if (!first)
              _characters (" ", 1);
            else
              first = false;

            serializer_._pre_impl (ctx);
            serializer_._serialize_content ();
            serializer_._post_impl ();
            serializer_.post ();
#else
            serializer_.pre (*i);

            if (serializer_._error_type ())
              serializer_._copy_error (ctx);

            if (ctx.error_type ())
              break;

            if (!first)
            {
              if (!_characters (" ", 1))
                break;
            }
            else
              first = false;

            serializer_._pre_impl (ctx);

            if (ctx.error_type ())
              break;

            serializer_._serialize_content ();

            if (ctx.error_type ())
              break;

            serializer_._post_impl ();

            if (ctx.error_type ())
              break;

            serializer_.post ();

            if (serializer_._error_type ())
              serializer_._copy_error (ctx);

            if (ctx.error_type ())
              break;
#endif
          }

          if (free_)
          {
            delete const_cast<string_sequence*> (value_);
            value_ = 0;
          }
        }

        void idrefs_simpl::
        _reset ()
        {
          idrefs_sskel::_reset ();
          serializer_._reset ();
        }
      }
    }
  }
}

