// file      : xsde/cxx/serializer/validating/int.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_INT_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_INT_HXX

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
        struct int_simpl: virtual int_sskel
#else
        struct int_simpl: int_sskel
#endif
        {
          virtual void
          pre (int);

          virtual void
          _serialize_content ();

        protected:
          int value_;
        };
      }
    }
  }
}

#endif // XSDE_CXX_SERIALIZER_VALIDATING_INT_HXX
