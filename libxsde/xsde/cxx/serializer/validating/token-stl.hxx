// file      : xsde/cxx/serializer/validating/token-stl.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_TOKEN_STL_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_TOKEN_STL_HXX

#include <string>

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
        struct token_simpl: virtual token_sskel
#else
        struct token_simpl: token_sskel
#endif
        {
          virtual void
          pre (const std::string&);

          virtual void
          _serialize_content ();

        protected:
          std::string value_;
        };
      }
    }
  }
}

#endif // XSDE_CXX_SERIALIZER_VALIDATING_TOKEN_STL_HXX
