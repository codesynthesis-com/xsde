// file      : xsde/cxx/serializer/non-validating/byte.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_NON_VALIDATING_BYTE_HXX
#define XSDE_CXX_SERIALIZER_NON_VALIDATING_BYTE_HXX

#include <xsde/cxx/serializer/non-validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        // 8-bit signed integer
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct byte_simpl: virtual byte_sskel
#else
        struct byte_simpl: byte_sskel
#endif
        {
          virtual void
          pre (signed char);

          virtual void
          _serialize_content ();

        protected:
          signed char value_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_SERIALIZER_NON_VALIDATING_BYTE_HXX
