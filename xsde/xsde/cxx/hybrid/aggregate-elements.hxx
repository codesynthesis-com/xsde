// file      : xsde/cxx/hybrid/aggregate-elements.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_HYBRID_AGGREGATE_ELEMENTS_HXX
#define XSDE_CXX_HYBRID_AGGREGATE_ELEMENTS_HXX

#include <map>

#include <xsde/cxx/hybrid/elements.hxx>

namespace CXX
{
  namespace Hybrid
  {
    typedef std::map<SemanticGraph::Type*, String> TypeInstanceMap;

    struct InstanceInfo
    {
      SemanticGraph::Type* type;
      String name;
    };

    typedef std::map<String, InstanceInfo> TypeIdInstanceMap;
  }
}

#endif // XSDE_CXX_HYBRID_AGGREGATE_ELEMENTS_HXX
