// file      : xsde/cxx/parser/validating/string.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/config.hxx>

#include <xsde/cxx/parser/validating/string.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        void string_pimpl::
        _pre ()
        {
#ifdef XSDE_EXCEPTIONS
          str_.assign ("", 0);
#else
          if (str_.assign ("", 0))
            _sys_error (sys_error::no_memory);
#endif
        }

        void string_pimpl::
        _characters (const ro_string& s)
        {
#ifdef XSDE_EXCEPTIONS
          str_.append (s.data (), s.size ());
#else
          if (str_.append (s.data (), s.size ()))
            _sys_error (sys_error::no_memory);
#endif
        }

        void string_pimpl::
        _post ()
        {
          // Check facets.
          //
          const facets& f = _facets ();

          if (f.length_set_ && str_.size () != f.length_)
          {
            _schema_error (schema_error::length_not_equal_prescribed);
            return;
          }

          if (f.min_length_set_ && str_.size () < f.min_length_)
          {
            _schema_error (schema_error::length_less_than_min);
            return;
          }

          if (f.max_length_set_ && str_.size () > f.max_length_)
          {
            _schema_error (schema_error::length_greater_than_max);
            return;
          }
        }

        char* string_pimpl::
        post_string ()
        {
          return str_.detach ();
        }
      }
    }
  }
}
