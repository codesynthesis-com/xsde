// file      : xsde/cxx/hybrid/validator.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_VALIDATOR_HXX
#define CXX_HYBRID_VALIDATOR_HXX

#include <xsd-frontend/semantic-graph/schema.hxx>

#include <xsde.hxx>
#include <types.hxx>

#include <cxx/hybrid/options.hxx>

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

#endif  // CXX_HYBRID_VALIDATOR_HXX
