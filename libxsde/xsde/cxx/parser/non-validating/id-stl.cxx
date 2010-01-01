// file      : xsde/cxx/parser/non-validating/id-stl.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/parser/non-validating/id-stl.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace non_validating
      {
        void id_pimpl::
        _pre ()
        {
          str_.erase ();
        }

        void id_pimpl::
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

        std::string id_pimpl::
        post_id ()
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

