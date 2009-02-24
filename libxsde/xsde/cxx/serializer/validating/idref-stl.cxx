// file      : xsde/cxx/serializer/validating/idref-stl.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/xml/ncname.hxx>

#include <xsde/cxx/serializer/validating/idref-stl.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        void idref_simpl::
        pre (const std::string& value)
        {
          value_ = value;
        }

        void idref_simpl::
        _serialize_content ()
        {
          std::string tmp;
          tmp.swap (value_);

          if (xml::valid_ncname (tmp.c_str (), tmp.size ()))
            _characters (tmp.c_str (), tmp.size ());
          else
            _schema_error (schema_error::invalid_idref_value);
        }
      }
    }
  }
}
