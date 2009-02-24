// file      : xsde/cxx/serializer/validating/idrefs-stl.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/validating/idrefs-stl.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
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
          // Should have at least one element.
          //
          if (!value_->empty ())
          {
            bool first = true;
            context& ctx = _context ();

            for (string_sequence::const_iterator i (value_->begin ()),
                   e (value_->end ()); i != e; ++i)
            {
              serializer_.pre (*i);

#ifndef XSDE_EXCEPTIONS
              if (serializer_._error_type ())
                serializer_._copy_error (ctx);

              if (ctx.error_type ())
                break;
#endif

              if (!first)
              {
#ifdef XSDE_EXCEPTIONS
                _characters (" ", 1);
#else
                if (!_characters (" ", 1))
                  break;
#endif
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

#ifndef XSDE_EXCEPTIONS
              if (serializer_._error_type ())
                serializer_._copy_error (ctx);

              if (ctx.error_type ())
                break;
#endif
            }
          }
          else
            _schema_error (schema_error::invalid_idrefs_value);

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
