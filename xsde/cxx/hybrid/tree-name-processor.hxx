// file      : xsde/cxx/hybrid/tree-name-processor.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_TREE_NAME_PROCESSOR_HXX
#define CXX_HYBRID_TREE_NAME_PROCESSOR_HXX

#include <xsd-frontend/semantic-graph.hxx>

#include <types.hxx>
#include <cxx/hybrid/options.hxx>

namespace CXX
{
  namespace Hybrid
  {
    class TreeNameProcessor
    {
    public:
      void
      process (options const& options,
               XSDFrontend::SemanticGraph::Schema&,
               XSDFrontend::SemanticGraph::Path const& file,
               bool deep);
    };
  }
}

#endif // CXX_HYBRID_TREE_NAME_PROCESSOR_HXX
