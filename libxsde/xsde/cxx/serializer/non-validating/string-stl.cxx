// file      : xsde/cxx/serializer/non-validating/string-stl.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/non-validating/string-stl.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        void string_simpl::
        pre (const std::string& value)
        {
          value_ = value;
        }

        void string_simpl::
        _serialize_content ()
        {
          // Make sure we don't hold any references to the string.
          //
          std::string tmp;
          tmp.swap (value_);

          _characters (tmp.c_str (), tmp.size ());
        }
      }
    }
  }
}
