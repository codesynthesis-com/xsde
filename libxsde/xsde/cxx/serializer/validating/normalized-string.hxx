// file      : xsde/cxx/serializer/validating/normalized-string.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_NORMALIZED_STRING_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_NORMALIZED_STRING_HXX

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
        struct normalized_string_simpl: virtual normalized_string_sskel
#else
        struct normalized_string_simpl: normalized_string_sskel
#endif
        {
          virtual
          ~normalized_string_simpl ();

          normalized_string_simpl (bool free = false);

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

#include <xsde/cxx/serializer/validating/normalized-string.ixx>

#endif  // XSDE_CXX_SERIALIZER_VALIDATING_NORMALIZED_STRING_HXX
