// file      : xsde/cxx/parser/validating/normalized-string.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/config.hxx>

#include <xsde/cxx/parser/validating/normalized-string.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        void normalized_string_pimpl::
        _pre ()
        {
#ifdef XSDE_EXCEPTIONS
          str_.assign ("", 0);
#else
          if (str_.assign ("", 0))
            _sys_error (sys_error::no_memory);
#endif
        }

        void normalized_string_pimpl::
        _characters (const ro_string& s)
        {
#ifdef XSDE_EXCEPTIONS
          str_.append (s.data (), s.size ());
#else
          if (str_.append (s.data (), s.size ()))
            _sys_error (sys_error::no_memory);
#endif
        }

        char* normalized_string_pimpl::
        post_normalized_string ()
        {
          typedef string::size_type size_type;

          size_type size = str_.size ();

          for (size_type i = 0; i < size; ++i)
          {
            char& c = str_[i];

            if (c == 0x0A || c == 0x0D || c == 0x09)
              c = 0x20;
          }

          return str_.detach ();
        }
      }
    }
  }
}
