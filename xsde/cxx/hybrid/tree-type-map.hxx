// file      : xsde/cxx/hybrid/tree-type-map.hxx
// copyright : Copyright (c) 2006-2017 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_TREE_TYPE_MAP_HXX
#define CXX_HYBRID_TREE_TYPE_MAP_HXX

#include <xsd-frontend/semantic-graph/schema.hxx>

#include <types.hxx>
#include <type-map/type-map.hxx>
#include <cxx/hybrid/options.hxx>

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

#endif // CXX_HYBRID_TREE_TYPE_MAP_HXX
