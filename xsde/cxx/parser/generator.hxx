// file      : xsde/cxx/parser/generator.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_PARSER_GENERATOR_HXX
#define CXX_PARSER_GENERATOR_HXX

#include <cult/types.hxx>

#include <cult/containers/vector.hxx>

#include <cult/cli/options.hxx>
#include <cult/cli/options-spec.hxx>

#include <xsd-frontend/semantic-graph/elements.hxx> // Path
#include <xsd-frontend/semantic-graph/schema.hxx>

#include <xsde.hxx>
#include <type-map/type-map.hxx>
#include <cxx/parser/cli.hxx>

namespace CXX
{
  namespace Parser
  {
    using namespace Cult::Types;

    class Generator
    {
    public:
      static Void
      usage ();

      static CLI::OptionsSpec
      options_spec ();

      // Assign names to global declarations.
      //
      static Void
      process_names (CLI::Options const&,
                     XSDFrontend::SemanticGraph::Schema&,
                     XSDFrontend::SemanticGraph::Path const&);

      // Generate code.
      //
      struct Failed {};

      static UnsignedLong
      generate (CLI::Options const&,
                XSDFrontend::SemanticGraph::Schema&,
                XSDFrontend::SemanticGraph::Path const&,
                TypeMap::Namespaces& type_map,
                Boolean gen_driver,
                const WarningSet& disabled_warnings,
                FileList&,
                AutoUnlinks&);

    private:
      Generator ();
    };
  }
}

#endif // CXX_PARSER_GENERATOR_HXX
