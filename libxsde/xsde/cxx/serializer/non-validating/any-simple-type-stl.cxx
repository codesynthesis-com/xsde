// file      : xsde/cxx/serializer/non-validating/any-simple-type-stl.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/non-validating/any-simple-type-stl.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        void any_simple_type_simpl::
        pre (const std::string& value)
        {
          value_ = value;
        }

        void any_simple_type_simpl::
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
