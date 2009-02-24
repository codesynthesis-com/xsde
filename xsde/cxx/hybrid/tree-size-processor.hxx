// file      : xsde/cxx/hybrid/tree-size-processor.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_TREE_SIZE_PROCESSOR_HXX
#define CXX_HYBRID_TREE_SIZE_PROCESSOR_HXX

#include <cult/types.hxx>

#include <xsd-frontend/semantic-graph.hxx>

#include <cxx/hybrid/cli.hxx>

namespace CXX
{
  namespace Hybrid
  {
    using namespace Cult::Types;

    class TreeSizeProcessor
    {
    public:
      Void
      process (CLI::Options const& options,
               XSDFrontend::SemanticGraph::Schema&,
               XSDFrontend::SemanticGraph::Path const& file);
    };
  }
}

#endif // CXX_HYBRID_TREE_SIZE_PROCESSOR_HXX
