// file      : xsde/cxx/parser/validating/token.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/config.hxx>

#include <xsde/cxx/parser/validating/token.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        void token_pimpl::
        _pre ()
        {
#ifdef XSDE_EXCEPTIONS
          str_.assign ("", 0);
#else
          if (str_.assign ("", 0))
            _sys_error (sys_error::no_memory);
#endif
        }

        void token_pimpl::
        _characters (const ro_string& s)
        {
          if (str_.size () == 0)
          {
            ro_string tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
            {
#ifdef XSDE_EXCEPTIONS
              str_.append (tmp.data (), tmp.size ());
#else
              if (str_.append (tmp.data (), tmp.size ()))
                _sys_error (sys_error::no_memory);
#endif
            }
          }
          else
          {
#ifdef XSDE_EXCEPTIONS
            str_.append (s.data (), s.size ());
#else
            if (str_.append (s.data (), s.size ()))
              _sys_error (sys_error::no_memory);
#endif
          }
        }

        char* token_pimpl::
        post_token ()
        {
          typedef string::size_type size_type;

          size_type size = str_.size ();
          size_type j = 0;

          bool subs = false;

          for (size_type i = 0; i < size; ++i)
          {
            char c = str_[i];

            if (c == 0x20 || c == 0x0A || c == 0x0D || c == 0x09)
            {
              subs = true;
            }
            else
            {
              if (subs)
              {
                subs = false;
                str_[j++] = 0x20;
              }

              str_[j++] = c;
            }
          }

          str_.truncate (j);

          return str_.detach ();
        }
      }
    }
  }
}
