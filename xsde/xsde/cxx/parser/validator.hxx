// file      : xsde/cxx/parser/validator.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_VALIDATOR_HXX
#define XSDE_CXX_PARSER_VALIDATOR_HXX

#include <libxsd-frontend/semantic-graph/schema.hxx>

#include <xsde/cxx/parser/options.hxx>

#include <xsde/xsde.hxx>
#include <xsde/types.hxx>

namespace CXX
{
  namespace Parser
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

#endif  // XSDE_CXX_PARSER_VALIDATOR_HXX
