// file      : xsde/cxx/serializer/non-validating/long-long.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_NON_VALIDATING_LONG_LONG_HXX
#define XSDE_CXX_SERIALIZER_NON_VALIDATING_LONG_LONG_HXX

#include <xsde/cxx/serializer/non-validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        // 64-bit signed integer
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct long_simpl: virtual long_sskel
#else
        struct long_simpl: long_sskel
#endif
        {
          virtual void
          pre (long long);

          virtual void
          _serialize_content ();

        protected:
          long long value_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_SERIALIZER_NON_VALIDATING_LONG_LONG_HXX
