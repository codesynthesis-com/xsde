// file      : xsde/cxx/serializer/validating/unsigned-long-long.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_UNSIGNED_LONG_LONG_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_UNSIGNED_LONG_LONG_HXX

#include <xsde/cxx/serializer/validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        // 64-bit unsigned integer
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct unsigned_long_simpl: virtual unsigned_long_sskel
#else
        struct unsigned_long_simpl: unsigned_long_sskel
#endif
        {
          virtual void
          pre (unsigned long long);

          virtual void
          _serialize_content ();

        protected:
          unsigned long long value_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_SERIALIZER_VALIDATING_UNSIGNED_LONG_LONG_HXX
