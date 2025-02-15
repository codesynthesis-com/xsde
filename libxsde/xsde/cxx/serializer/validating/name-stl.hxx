// file      : xsde/cxx/serializer/validating/name-stl.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_NAME_STL_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_NAME_STL_HXX

#include <string>

#include <xsde/cxx/serializer/validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct name_simpl: virtual name_sskel
#else
        struct name_simpl: name_sskel
#endif
        {
          virtual void
          pre (const std::string&);

          virtual void
          _serialize_content ();

        protected:
          std::string value_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_SERIALIZER_VALIDATING_NAME_STL_HXX
