// file      : xsde/cxx/serializer/non-validating/nmtokens-stl.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/non-validating/nmtokens-stl.hxx>

#ifdef XSDE_CUSTOM_ALLOCATOR
#  include <xsde/cxx/allocator.hxx>
#endif

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        nmtokens_simpl::
        ~nmtokens_simpl ()
        {
          if (free_ && value_)
          {
            string_sequence* v = const_cast<string_sequence*> (value_);
#ifndef XSDE_CUSTOM_ALLOCATOR
            delete v;
#else
            v->~string_sequence ();
            cxx::free (v);
#endif
          }
        }

        void nmtokens_simpl::
        pre (const string_sequence* value)
        {
          value_ = value;
        }

        void nmtokens_simpl::
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
            string_sequence* v = const_cast<string_sequence*> (value_);
#ifndef XSDE_CUSTOM_ALLOCATOR
            delete v;
#else
            v->~string_sequence ();
            cxx::free (v);
#endif
            value_ = 0;
          }
        }

        void nmtokens_simpl::
        _reset ()
        {
          nmtokens_sskel::_reset ();
          serializer_._reset ();
        }
      }
    }
  }
}
