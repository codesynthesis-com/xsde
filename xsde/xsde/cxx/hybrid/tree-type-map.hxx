// file      : xsde/cxx/hybrid/tree-type-map.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_HYBRID_TREE_TYPE_MAP_HXX
#define XSDE_CXX_HYBRID_TREE_TYPE_MAP_HXX

#include <libxsd-frontend/semantic-graph/schema.hxx>

#include <xsde/types.hxx>
#include <xsde/type-map/type-map.hxx>
#include <xsde/cxx/hybrid/options.hxx>

namespace CXX
{
  namespace Hybrid
  {
    void
    generate_tree_type_map (options const& options,
                            XSDFrontend::SemanticGraph::Schema&,
                            XSDFrontend::SemanticGraph::Path const&,
                            String const& hxx_name,
                            TypeMap::Namespaces& parser_type_map,
                            TypeMap::Namespaces& serializer_type_map);
  }
}

#endif // XSDE_CXX_HYBRID_TREE_TYPE_MAP_HXX
