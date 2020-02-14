// file      : xsde/cxx/serializer/non-validating/any-type.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_NON_VALIDATING_ANY_TYPE_HXX
#define XSDE_CXX_SERIALIZER_NON_VALIDATING_ANY_TYPE_HXX

#include <xsde/cxx/serializer/non-validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        // any_type
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct any_type_simpl: virtual any_type_sskel
#else
        struct any_type_simpl: any_type_sskel
#endif
        {
        };
      }
    }
  }
}

#endif // XSDE_CXX_SERIALIZER_NON_VALIDATING_ANY_TYPE_HXX
