// file      : xsde/cxx/serializer/name-processor.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2006-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_SERIALIZER_NAME_PROCESSOR_HXX
#define CXX_SERIALIZER_NAME_PROCESSOR_HXX

#include <xsd-frontend/semantic-graph.hxx>

#include <types.hxx>
#include <cxx/serializer/options.hxx>

namespace CXX
{
  namespace Serializer
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

#endif // CXX_SERIALIZER_NAME_PROCESSOR_HXX
