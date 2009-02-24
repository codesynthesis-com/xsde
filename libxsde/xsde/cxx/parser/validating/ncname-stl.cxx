// file      : xsde/cxx/parser/validating/ncname-stl.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/xml/ncname.hxx>

#include <xsde/cxx/parser/validating/ncname-stl.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        void ncname_pimpl::
        _pre ()
        {
          str_.erase ();
        }

        void ncname_pimpl::
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

        void ncname_pimpl::
        _post ()
        {
          ro_string tmp (str_);
          ro_string::size_type size = trim_right (tmp);

          if (xml::valid_ncname (tmp.data (), size))
            str_.resize (size);
          else
            _schema_error (schema_error::invalid_ncname_value);
        }

        std::string ncname_pimpl::
        post_ncname ()
        {
          std::string r;
          r.swap (str_);
          return r;
        }
      }
    }
  }
}
