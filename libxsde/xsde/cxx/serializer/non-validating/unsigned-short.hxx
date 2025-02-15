// file      : xsde/cxx/serializer/non-validating/unsigned-short.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_NON_VALIDATING_UNSIGNED_SHORT_HXX
#define XSDE_CXX_SERIALIZER_NON_VALIDATING_UNSIGNED_SHORT_HXX

#include <xsde/cxx/serializer/non-validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        // 16-bit unsigned integer
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct unsigned_short_simpl: virtual unsigned_short_sskel
#else
        struct unsigned_short_simpl: unsigned_short_sskel
#endif
        {
          virtual void
          pre (unsigned short);

          virtual void
          _serialize_content ();

        protected:
          unsigned short value_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_SERIALIZER_NON_VALIDATING_UNSIGNED_SHORT_HXX
