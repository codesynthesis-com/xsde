// file      : xsde/cxx/parser/name-processor.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_NAME_PROCESSOR_HXX
#define XSDE_CXX_PARSER_NAME_PROCESSOR_HXX

#include <libxsd-frontend/semantic-graph.hxx>

#include <xsde/types.hxx>
#include <xsde/cxx/parser/options.hxx>

namespace CXX
{
  namespace Parser
  {
    class NameProcessor
    {
    public:
      void
      process (options const&,
               XSDFrontend::SemanticGraph::Schema&,
               XSDFrontend::SemanticGraph::Path const&,
               bool deep);
    };
  }
}

#endif // XSDE_CXX_PARSER_NAME_PROCESSOR_HXX
