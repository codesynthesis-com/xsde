// file      : xsde/cxx/serializer/validating/short.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_SHORT_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_SHORT_HXX

#include <xsde/cxx/serializer/validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        // 16-bit signed integer
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct short_simpl: virtual short_sskel
#else
        struct short_simpl: short_sskel
#endif
        {
          virtual void
          pre (short);

          virtual void
          _serialize_content ();

        protected:
          short value_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_SERIALIZER_VALIDATING_SHORT_HXX
