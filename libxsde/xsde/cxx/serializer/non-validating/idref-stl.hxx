// file      : xsde/cxx/serializer/non-validating/idref-stl.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_NON_VALIDATING_IDREF_STL_HXX
#define XSDE_CXX_SERIALIZER_NON_VALIDATING_IDREF_STL_HXX

#include <string>

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
        struct idref_simpl: virtual idref_sskel
#else
        struct idref_simpl: idref_sskel
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

#endif  // XSDE_CXX_SERIALIZER_NON_VALIDATING_IDREF_STL_HXX
