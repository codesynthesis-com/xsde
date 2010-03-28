// file      : xsde/cxx/parser/validating/string-stl.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/parser/validating/string-stl.hxx>
#include <xsde/cxx/parser/validating/string-common.hxx>

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
          str_.erase ();
        }

        void string_pimpl::
        _characters (const ro_string& s)
        {
          str_ += s;
        }

        void string_pimpl::
        _post ()
        {
          string_common::validate_facets (
            str_.c_str (), str_.size (), _facets (), _context ());
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
