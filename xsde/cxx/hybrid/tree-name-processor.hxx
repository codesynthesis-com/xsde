// file      : xsde/cxx/hybrid/tree-name-processor.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2006-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_TREE_NAME_PROCESSOR_HXX
#define CXX_HYBRID_TREE_NAME_PROCESSOR_HXX

#include <cult/types.hxx>

#include <xsd-frontend/semantic-graph.hxx>

#include <cxx/hybrid/options.hxx>

namespace CXX
{
  namespace Hybrid
  {
    using namespace Cult::Types;

    class TreeNameProcessor
    {
    public:
      Void
      process (options const& options,
               XSDFrontend::SemanticGraph::Schema&,
               XSDFrontend::SemanticGraph::Path const& file,
               Boolean deep);
    };
  }
}

#endif // CXX_HYBRID_TREE_NAME_PROCESSOR_HXX
