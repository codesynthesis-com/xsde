// file      : xsde/cxx/hybrid/tree-size-processor.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_TREE_SIZE_PROCESSOR_HXX
#define CXX_HYBRID_TREE_SIZE_PROCESSOR_HXX

#include <xsd-frontend/semantic-graph.hxx>

#include <xsde.hxx>
#include <types.hxx>

#include <cxx/hybrid/options.hxx>

namespace CXX
{
  namespace Hybrid
  {
    class TreeSizeProcessor
    {
    public:
      bool
      process (options const& options,
               XSDFrontend::SemanticGraph::Schema&,
               XSDFrontend::SemanticGraph::Path const& file,
               const WarningSet& disabled_warnings);
    };
  }
}

#endif // CXX_HYBRID_TREE_SIZE_PROCESSOR_HXX
