// file      : xsde/cxx/hybrid/validator.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_HYBRID_VALIDATOR_HXX
#define XSDE_CXX_HYBRID_VALIDATOR_HXX

#include <libxsd-frontend/semantic-graph/schema.hxx>

#include <xsde/xsde.hxx>
#include <xsde/types.hxx>

#include <xsde/cxx/hybrid/options.hxx>

namespace CXX
{
  namespace Hybrid
  {
    class Validator
    {
    public:
      bool
      validate (options const& options,
                XSDFrontend::SemanticGraph::Schema&,
                XSDFrontend::SemanticGraph::Path const& tu,
                const WarningSet& disabled_warnings);
    };
  }
}

#endif  // XSDE_CXX_HYBRID_VALIDATOR_HXX
