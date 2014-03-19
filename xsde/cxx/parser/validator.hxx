// file      : xsde/cxx/parser/validator.hxx
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_PARSER_VALIDATOR_HXX
#define CXX_PARSER_VALIDATOR_HXX

#include <xsd-frontend/semantic-graph/schema.hxx>

#include <cxx/parser/options.hxx>

#include <xsde.hxx>
#include <types.hxx>

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

#endif  // CXX_PARSER_VALIDATOR_HXX
