// file      : xsde/cxx/serializer/validating/gmonth-day.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_GMONTH_DAY_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_GMONTH_DAY_HXX

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
        struct gmonth_day_simpl: virtual gmonth_day_sskel
#else
        struct gmonth_day_simpl: gmonth_day_sskel
#endif
        {
          virtual void
          pre (const gmonth_day&);

          virtual void
          _serialize_content ();

          gmonth_day_simpl (); // Keep it last.

        protected:
          gmonth_day value_;
        };
      }
    }
  }
}

#include <xsde/cxx/serializer/validating/gmonth-day.ixx>

#endif // XSDE_CXX_SERIALIZER_VALIDATING_GMONTH_DAY_HXX
