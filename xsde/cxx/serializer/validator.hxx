// file      : xsde/cxx/serializer/validator.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_SERIALIZER_VALIDATOR_HXX
#define CXX_SERIALIZER_VALIDATOR_HXX

#include <xsd-frontend/semantic-graph/schema.hxx>

#include <xsde.hxx>
#include <types.hxx>

#include <cxx/serializer/options.hxx>

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

#endif  // CXX_SERIALIZER_VALIDATOR_HXX
