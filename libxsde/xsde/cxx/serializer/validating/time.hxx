// file      : xsde/cxx/serializer/validating/time.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_TIME_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_TIME_HXX

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
        struct time_simpl: virtual time_sskel
#else
        struct time_simpl: time_sskel
#endif
        {
          virtual void
          pre (const time&);

          virtual void
          _serialize_content ();

          time_simpl ();

        protected:
          time value_;
        };
      }
    }
  }
}

#include <xsde/cxx/serializer/validating/time.ixx>

#endif // XSDE_CXX_SERIALIZER_VALIDATING_TIME_HXX
