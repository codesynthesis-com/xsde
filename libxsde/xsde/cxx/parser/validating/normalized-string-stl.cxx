// file      : xsde/cxx/parser/validating/normalized-string-stl.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/parser/validating/normalized-string-stl.hxx>

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
          str_.erase ();
        }

        void normalized_string_pimpl::
        _characters (const ro_string& s)
        {
          str_ += s;
        }

        std::string normalized_string_pimpl::
        post_normalized_string ()
        {
          std::string::size_type size = str_.size ();

          for (std::string::size_type i = 0; i < size; ++i)
          {
            char& c = str_[i];

            if (c == 0x0A || c == 0x0D || c == 0x09)
              c = 0x20;
          }

          std::string r;
          r.swap (str_);
          return r;
        }
      }
    }
  }
}
