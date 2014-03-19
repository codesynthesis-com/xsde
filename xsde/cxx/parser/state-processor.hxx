// file      : xsde/cxx/parser/state-processor.hxx
// copyright : Copyright (c) 2006-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_PARSER_STATE_PROCESSOR_HXX
#define CXX_PARSER_STATE_PROCESSOR_HXX

#include <xsd-frontend/semantic-graph.hxx>

#include <types.hxx>

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

#endif // CXX_PARSER_STATE_PROCESSOR_HXX
