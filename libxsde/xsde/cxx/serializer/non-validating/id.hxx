// file      : xsde/cxx/serializer/non-validating/id.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_NON_VALIDATING_ID_HXX
#define XSDE_CXX_SERIALIZER_NON_VALIDATING_ID_HXX

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
        struct id_simpl: virtual id_sskel
#else
        struct id_simpl: id_sskel
#endif
        {
          virtual
          ~id_simpl ();

          id_simpl (bool free = false);

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

#include <xsde/cxx/serializer/non-validating/id.ixx>

#endif  // XSDE_CXX_SERIALIZER_NON_VALIDATING_ID_HXX
