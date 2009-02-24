// file      : xsde/cxx/parser/non-validating/string.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/config.hxx>

#include <xsde/cxx/parser/non-validating/string.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace non_validating
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

        char* string_pimpl::
        post_string ()
        {
          return str_.detach ();
        }
      }
    }
  }
}
