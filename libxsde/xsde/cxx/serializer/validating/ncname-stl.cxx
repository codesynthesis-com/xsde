// file      : xsde/cxx/serializer/validating/ncname-stl.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/xml/ncname.hxx>

#include <xsde/cxx/serializer/validating/ncname-stl.hxx>
#include <xsde/cxx/serializer/validating/string-common.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        void ncname_simpl::
        pre (const std::string& value)
        {
          value_ = value;
        }

        void ncname_simpl::
        _serialize_content ()
        {
          std::string tmp;
          tmp.swap (value_);

          if (!xml::valid_ncname (tmp.c_str (), tmp.size ()))
          {
            _schema_error (schema_error::invalid_ncname_value);
            return;
          }

          if (!string_common::validate_facets (
                tmp.c_str (), tmp.size (), _facets (), _context ()))
            return;

          _characters (tmp.c_str (), tmp.size ());
        }
      }
    }
  }
}
