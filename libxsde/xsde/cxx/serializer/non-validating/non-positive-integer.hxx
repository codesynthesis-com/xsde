// file      : xsde/cxx/serializer/non-validating/non-positive-integer.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_NON_VALIDATING_NON_POSITIVE_INTEGER_HXX
#define XSDE_CXX_SERIALIZER_NON_VALIDATING_NON_POSITIVE_INTEGER_HXX

#include <xsde/cxx/serializer/non-validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        // Arbitrary-length non-positive integer. Mapped to long.
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct non_positive_integer_simpl: virtual non_positive_integer_sskel
#else
        struct non_positive_integer_simpl: non_positive_integer_sskel
#endif
        {
          virtual void
          pre (long);

          virtual void
          _serialize_content ();

        protected:
          long value_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_SERIALIZER_NON_VALIDATING_NON_POSITIVE_INTEGER_HXX
