// file      : xsde/cxx/hybrid/tree-size-processor.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_HYBRID_TREE_SIZE_PROCESSOR_HXX
#define XSDE_CXX_HYBRID_TREE_SIZE_PROCESSOR_HXX

#include <libxsd-frontend/semantic-graph.hxx>

#include <xsde/xsde.hxx>
#include <xsde/types.hxx>

#include <xsde/cxx/hybrid/options.hxx>

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

#endif // XSDE_CXX_HYBRID_TREE_SIZE_PROCESSOR_HXX
