// file      : xsde/cxx/serializer/validating/gday.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_GDAY_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_GDAY_HXX

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
        struct gday_simpl: virtual gday_sskel
#else
        struct gday_simpl: gday_sskel
#endif
        {
          virtual void
          pre (const gday&);

          virtual void
          _serialize_content ();

          gday_simpl (); // Keep it last.

        protected:
          gday value_;
        };
      }
    }
  }
}

#include <xsde/cxx/serializer/validating/gday.ixx>

#endif // XSDE_CXX_SERIALIZER_VALIDATING_GDAY_HXX
