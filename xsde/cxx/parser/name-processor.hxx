// file      : xsde/cxx/parser/name-processor.hxx
// copyright : Copyright (c) 2006-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_PARSER_NAME_PROCESSOR_HXX
#define CXX_PARSER_NAME_PROCESSOR_HXX

#include <xsd-frontend/semantic-graph.hxx>

#include <types.hxx>
#include <cxx/parser/options.hxx>

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

#endif // CXX_PARSER_NAME_PROCESSOR_HXX
