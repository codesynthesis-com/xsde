// file      : xsde/cxx/serializer/validating/date.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_DATE_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_DATE_HXX

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
        struct date_simpl: virtual date_sskel
#else
        struct date_simpl: date_sskel
#endif
        {
          virtual void
          pre (const date&);

          virtual void
          _serialize_content ();

          date_simpl (); // Keep it last.

        protected:
          date value_;
        };
      }
    }
  }
}

#include <xsde/cxx/serializer/validating/date.ixx>

#endif // XSDE_CXX_SERIALIZER_VALIDATING_DATE_HXX
