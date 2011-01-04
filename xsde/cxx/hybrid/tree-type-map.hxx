// file      : xsde/cxx/hybrid/tree-type-map.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2006-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_TREE_TYPE_MAP_HXX
#define CXX_HYBRID_TREE_TYPE_MAP_HXX

#include <cult/types.hxx>

#include <xsd-frontend/semantic-graph/schema.hxx>

#include <type-map/type-map.hxx>

#include <cxx/hybrid/cli.hxx>

namespace CXX
{
  namespace Hybrid
  {
    using namespace Cult::Types;

    Void
    generate_tree_type_map (CLI::Options const& options,
                            XSDFrontend::SemanticGraph::Schema&,
                            XSDFrontend::SemanticGraph::Path const&,
                            String const& hxx_name,
                            TypeMap::Namespaces& parser_type_map,
                            TypeMap::Namespaces& serializer_type_map);
  }
}

#endif // CXX_HYBRID_TREE_TYPE_MAP_HXX
