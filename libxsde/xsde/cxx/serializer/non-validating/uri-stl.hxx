// file      : xsde/cxx/serializer/non-validating/uri-stl.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_NON_VALIDATING_URI_STL_HXX
#define XSDE_CXX_SERIALIZER_NON_VALIDATING_URI_STL_HXX

#include <string>

#include <xsde/cxx/serializer/non-validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct uri_simpl: virtual uri_sskel
#else
        struct uri_simpl: uri_sskel
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

#endif  // XSDE_CXX_SERIALIZER_NON_VALIDATING_URI_STL_HXX
