// file      : xsde/cxx/serializer/non-validating/nmtokens-stl.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_NON_VALIDATING_NMTOKENS_STL_HXX
#define XSDE_CXX_SERIALIZER_NON_VALIDATING_NMTOKENS_STL_HXX

#include <xsde/cxx/serializer/non-validating/xml-schema-sskel.hxx>
#include <xsde/cxx/serializer/non-validating/nmtoken-stl.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct nmtokens_simpl: virtual nmtokens_sskel
#else
        struct nmtokens_simpl: nmtokens_sskel
#endif
        {
          virtual
          ~nmtokens_simpl ();

          nmtokens_simpl (bool free = false);

          virtual void
          pre (const string_sequence*);

          virtual void
          _serialize_content ();

          virtual void
          _reset ();

        protected:
          bool free_;
          const string_sequence* value_;
          nmtoken_simpl serializer_;
        };
      }
    }
  }
}

#include <xsde/cxx/serializer/non-validating/nmtokens-stl.ixx>

#endif  // XSDE_CXX_SERIALIZER_NON_VALIDATING_NMTOKENS_STL_HXX
