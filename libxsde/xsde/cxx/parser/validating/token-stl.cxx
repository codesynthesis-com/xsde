// file      : xsde/cxx/parser/validating/token-stl.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/parser/validating/token-stl.hxx>

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
          str_.erase ();
        }

        void token_pimpl::
        _characters (const ro_string& s)
        {
          if (str_.size () == 0)
          {
            ro_string tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        std::string token_pimpl::
        post_token ()
        {
          std::string::size_type size = str_.size ();
          std::string::size_type j = 0;

          bool subs = false;

          for (std::string::size_type i = 0; i < size; ++i)
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

          str_.resize (j);

          std::string r;
          r.swap (str_);
          return r;
        }
      }
    }
  }
}
