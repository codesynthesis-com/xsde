// file      : xsde/cxx/hybrid/validator.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_VALIDATOR_HXX
#define CXX_HYBRID_VALIDATOR_HXX

#include <cult/types.hxx>

#include <xsd-frontend/semantic-graph/schema.hxx>

#include <cxx/hybrid/options.hxx>

#include <xsde.hxx>

namespace CXX
{
  namespace Hybrid
  {
    using namespace Cult::Types;

    class Validator
    {
    public:
      Boolean
      validate (options const& options,
                XSDFrontend::SemanticGraph::Schema&,
                XSDFrontend::SemanticGraph::Path const& tu,
                const WarningSet& disabled_warnings);
    };
  }
}

#endif  // CXX_HYBRID_VALIDATOR_HXX
