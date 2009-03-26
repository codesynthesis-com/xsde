// file      : xsde/cxx/hybrid/parser-name-processor.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_PARSER_NAME_PROCESSOR_HXX
#define CXX_HYBRID_PARSER_NAME_PROCESSOR_HXX

#include <cult/types.hxx>

#include <xsd-frontend/semantic-graph.hxx>

#include <cxx/hybrid/cli.hxx>

namespace CXX
{
  namespace Hybrid
  {
    using namespace Cult::Types;

    class ParserNameProcessor
    {
    public:
      Void
      process (CLI::Options const& options,
               XSDFrontend::SemanticGraph::Schema&,
               XSDFrontend::SemanticGraph::Path const& file,
               Boolean deep);
    };
  }
}

#endif // CXX_HYBRID_PARSER_NAME_PROCESSOR_HXX
