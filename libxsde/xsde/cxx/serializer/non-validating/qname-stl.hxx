// file      : xsde/cxx/serializer/non-validating/qname-stl.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_NON_VALIDATING_QNAME_STL_HXX
#define XSDE_CXX_SERIALIZER_NON_VALIDATING_QNAME_STL_HXX

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
        struct qname_simpl: virtual qname_sskel
#else
        struct qname_simpl: qname_sskel
#endif
        {
          virtual void
          pre (const qname&);

          virtual void
          _serialize_content ();

          qname_simpl (); // Keep it last.

        protected:
          qname value_;
        };
      }
    }
  }
}

#include <xsde/cxx/serializer/non-validating/qname-stl.ixx>

#endif // XSDE_CXX_SERIALIZER_NON_VALIDATING_QNAME_STL_HXX
