// file      : xsde/cxx/serializer/non-validating/any-simple-type.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_NON_VALIDATING_ANY_SIMPLE_TYPE_HXX
#define XSDE_CXX_SERIALIZER_NON_VALIDATING_ANY_SIMPLE_TYPE_HXX

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
        struct any_simple_type_simpl: virtual any_simple_type_sskel
#else
        struct any_simple_type_simpl: any_simple_type_sskel
#endif
        {
          virtual
          ~any_simple_type_simpl ();

          any_simple_type_simpl (bool free = false);

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

#include <xsde/cxx/serializer/non-validating/any-simple-type.ixx>

#endif  // XSDE_CXX_SERIALIZER_NON_VALIDATING_ANY_SIMPLE_TYPE_HXX
