// file      : xsde/cxx/serializer/non-validating/string.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_NON_VALIDATING_STRING_HXX
#define XSDE_CXX_SERIALIZER_NON_VALIDATING_STRING_HXX

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
        struct string_simpl: virtual string_sskel
#else
        struct string_simpl: string_sskel
#endif
        {
          virtual
          ~string_simpl ();

          string_simpl (bool free = false);

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

#include <xsde/cxx/serializer/non-validating/string.ixx>

#endif  // XSDE_CXX_SERIALIZER_NON_VALIDATING_STRING_HXX
