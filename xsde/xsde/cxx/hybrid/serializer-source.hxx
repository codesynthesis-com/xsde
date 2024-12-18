// file      : xsde/cxx/hybrid/serializer-source.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_HYBRID_SERIALIZER_SOURCE_HXX
#define XSDE_CXX_HYBRID_SERIALIZER_SOURCE_HXX

#include <xsde/cxx/hybrid/elements.hxx>

namespace CXX
{
  namespace Hybrid
  {
    void
    generate_serializer_source (Context&, Regex const& hxx_obj_expr);
  }
}

#endif // XSDE_CXX_HYBRID_SERIALIZER_SOURCE_HXX
