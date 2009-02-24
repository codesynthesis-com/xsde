// file      : xsde/cxx/serializer/validator.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_SERIALIZER_VALIDATOR_HXX
#define CXX_SERIALIZER_VALIDATOR_HXX

#include <cult/types.hxx>

#include <xsd-frontend/semantic-graph/schema.hxx>

#include <cxx/serializer/cli.hxx>

#include <xsde.hxx>

namespace CXX
{
  namespace Serializer
  {
    using namespace Cult::Types;

    class Validator
    {
    public:
      Boolean
      validate (CLI::Options const& options,
                XSDFrontend::SemanticGraph::Schema&,
                XSDFrontend::SemanticGraph::Path const& tu,
                Boolean gen_driver,
                const WarningSet& disabled_warnings);
    };
  }
}

#endif  // CXX_SERIALIZER_VALIDATOR_HXX
