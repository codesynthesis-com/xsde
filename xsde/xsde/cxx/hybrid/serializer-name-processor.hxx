// file      : xsde/cxx/hybrid/serializer-name-processor.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_HYBRID_SERIALIZER_NAME_PROCESSOR_HXX
#define XSDE_CXX_HYBRID_SERIALIZER_NAME_PROCESSOR_HXX

#include <libxsd-frontend/semantic-graph.hxx>

#include <xsde/types.hxx>
#include <xsde/cxx/hybrid/options.hxx>

namespace CXX
{
  namespace Hybrid
  {
    class SerializerNameProcessor
    {
    public:
      bool
      process (options const& options,
               XSDFrontend::SemanticGraph::Schema&,
               XSDFrontend::SemanticGraph::Path const& file,
               bool deep);
    };
  }
}

#endif // XSDE_CXX_HYBRID_SERIALIZER_NAME_PROCESSOR_HXX
