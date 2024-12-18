// file      : xsde/cxx/serializer/validator.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATOR_HXX
#define XSDE_CXX_SERIALIZER_VALIDATOR_HXX

#include <libxsd-frontend/semantic-graph/schema.hxx>

#include <xsde/xsde.hxx>
#include <xsde/types.hxx>

#include <xsde/cxx/serializer/options.hxx>

namespace CXX
{
  namespace Serializer
  {
    class Validator
    {
    public:
      bool
      validate (options const&,
                XSDFrontend::SemanticGraph::Schema&,
                XSDFrontend::SemanticGraph::Path const& tu,
                bool gen_driver,
                const WarningSet& disabled_warnings);
    };
  }
}

#endif  // XSDE_CXX_SERIALIZER_VALIDATOR_HXX
