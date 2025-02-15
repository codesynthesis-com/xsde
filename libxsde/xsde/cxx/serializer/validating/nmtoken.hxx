// file      : xsde/cxx/serializer/validating/nmtoken.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_NMTOKEN_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_NMTOKEN_HXX

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
        struct nmtoken_simpl: virtual nmtoken_sskel
#else
        struct nmtoken_simpl: nmtoken_sskel
#endif
        {
          virtual
          ~nmtoken_simpl ();

          nmtoken_simpl (bool free = false);

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

#include <xsde/cxx/serializer/validating/nmtoken.ixx>

#endif  // XSDE_CXX_SERIALIZER_VALIDATING_NMTOKEN_HXX
