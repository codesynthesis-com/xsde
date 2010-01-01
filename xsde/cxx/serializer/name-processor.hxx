// file      : xsde/cxx/serializer/name-processor.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2006-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_SERIALIZER_NAME_PROCESSOR_HXX
#define CXX_SERIALIZER_NAME_PROCESSOR_HXX

#include <cult/types.hxx>

#include <xsd-frontend/semantic-graph.hxx>

#include <cxx/serializer/cli.hxx>

namespace CXX
{
  namespace Serializer
  {
    using namespace Cult::Types;

    class NameProcessor
    {
    public:
      Void
      process (CLI::Options const&,
               XSDFrontend::SemanticGraph::Schema&,
               XSDFrontend::SemanticGraph::Path const&,
               Boolean deep);
    };
  }
}

#endif // CXX_SERIALIZER_NAME_PROCESSOR_HXX
