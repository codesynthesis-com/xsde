// file      : xsde/cxx/serializer/type-processor.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_TYPE_PROCESSOR_HXX
#define XSDE_CXX_SERIALIZER_TYPE_PROCESSOR_HXX

#include <libxsd-frontend/semantic-graph.hxx>

#include <xsde/types.hxx>
#include <xsde/type-map/type-map.hxx>
#include <xsde/cxx/serializer/options.hxx>

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

#endif // XSDE_CXX_SERIALIZER_TYPE_PROCESSOR_HXX
