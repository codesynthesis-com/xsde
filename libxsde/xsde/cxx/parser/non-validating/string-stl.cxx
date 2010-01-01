// file      : xsde/cxx/parser/non-validating/string-stl.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/parser/non-validating/string-stl.hxx>

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
          str_.erase ();
        }

        void string_pimpl::
        _characters (const ro_string& s)
        {
          str_ += s;
        }

        std::string string_pimpl::
        post_string ()
        {
          std::string r;
          r.swap (str_);
          return r;
        }
      }
    }
  }
}
