// file      : xsde/cxx/parser/state-processor.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_STATE_PROCESSOR_HXX
#define XSDE_CXX_PARSER_STATE_PROCESSOR_HXX

#include <libxsd-frontend/semantic-graph.hxx>

#include <xsde/types.hxx>

namespace CXX
{
  namespace Parser
  {
    class StateProcessor
    {
    public:
      void
      process (XSDFrontend::SemanticGraph::Schema&,
               XSDFrontend::SemanticGraph::Path const& file);
    };
  }
}

#endif // XSDE_CXX_PARSER_STATE_PROCESSOR_HXX
