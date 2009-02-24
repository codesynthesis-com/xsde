// file      : xsde/cxx/parser/non-validating/nmtoken-stl.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/parser/non-validating/nmtoken-stl.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace non_validating
      {
        void nmtoken_pimpl::
        _pre ()
        {
          str_.erase ();
        }

        void nmtoken_pimpl::
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

        std::string nmtoken_pimpl::
        post_nmtoken ()
        {
          ro_string tmp (str_);
          str_.resize (trim_right (tmp));

          std::string r;
          r.swap (str_);
          return r;
        }
      }
    }
  }
}
