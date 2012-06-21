// file      : xsde/cxx/parser/name-processor.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2006-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_PARSER_NAME_PROCESSOR_HXX
#define CXX_PARSER_NAME_PROCESSOR_HXX

#include <cult/types.hxx>

#include <xsd-frontend/semantic-graph.hxx>

#include <cxx/parser/options.hxx>

namespace CXX
{
  namespace Parser
  {
    using namespace Cult::Types;

    class NameProcessor
    {
    public:
      Void
      process (options const&,
               XSDFrontend::SemanticGraph::Schema&,
               XSDFrontend::SemanticGraph::Path const&,
               Boolean deep);
    };
  }
}

#endif // CXX_PARSER_NAME_PROCESSOR_HXX
