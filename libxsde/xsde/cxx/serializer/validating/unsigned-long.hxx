// file      : xsde/cxx/serializer/validating/unsigned-long.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_UNSIGNED_LONG_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_UNSIGNED_LONG_HXX

#include <xsde/cxx/serializer/validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        // Fall-back implementation when 64 bit long long is not available.
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct unsigned_long_simpl: virtual unsigned_long_sskel
#else
        struct unsigned_long_simpl: unsigned_long_sskel
#endif
        {
          virtual void
          pre (unsigned long);

          virtual void
          _serialize_content ();

        protected:
          unsigned long value_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_SERIALIZER_VALIDATING_UNSIGNED_LONG_HXX
