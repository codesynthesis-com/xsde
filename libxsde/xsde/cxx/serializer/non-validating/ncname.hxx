// file      : xsde/cxx/serializer/non-validating/ncname.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_NON_VALIDATING_NCNAME_HXX
#define XSDE_CXX_SERIALIZER_NON_VALIDATING_NCNAME_HXX

#include <xsde/cxx/serializer/non-validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct ncname_simpl: virtual ncname_sskel
#else
        struct ncname_simpl: ncname_sskel
#endif
        {
          virtual
          ~ncname_simpl ();

          ncname_simpl (bool free = false);

          virtual void
          pre (const char*);

          virtual void
          _serialize_content ();

        protected:
          bool free_;
          const char* value_;
        };
      }
    }
  }
}

#include <xsde/cxx/serializer/non-validating/ncname.ixx>

#endif  // XSDE_CXX_SERIALIZER_NON_VALIDATING_NCNAME_HXX
