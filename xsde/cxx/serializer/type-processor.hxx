// file      : xsde/cxx/serializer/type-processor.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2006-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_SERIALIZER_TYPE_PROCESSOR_HXX
#define CXX_SERIALIZER_TYPE_PROCESSOR_HXX

#include <cult/types.hxx>

#include <xsd-frontend/semantic-graph.hxx>

#include <type-map/type-map.hxx>

#include <cxx/serializer/options.hxx>

namespace CXX
{
  namespace Serializer
  {
    using namespace Cult::Types;

    class TypeProcessor
    {
    public:
      Void
      process (options const&,
               XSDFrontend::SemanticGraph::Schema&,
               TypeMap::Namespaces&);
    };
  }
}

#endif // CXX_SERIALIZER_TYPE_PROCESSOR_HXX
