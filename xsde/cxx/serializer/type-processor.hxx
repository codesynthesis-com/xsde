// file      : xsde/cxx/serializer/type-processor.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_SERIALIZER_TYPE_PROCESSOR_HXX
#define CXX_SERIALIZER_TYPE_PROCESSOR_HXX

#include <xsd-frontend/semantic-graph.hxx>

#include <types.hxx>
#include <type-map/type-map.hxx>
#include <cxx/serializer/options.hxx>

namespace CXX
{
  namespace Serializer
  {
    class TypeProcessor
    {
    public:
      void
      process (options const&,
               XSDFrontend::SemanticGraph::Schema&,
               TypeMap::Namespaces&);
    };
  }
}

#endif // CXX_SERIALIZER_TYPE_PROCESSOR_HXX
