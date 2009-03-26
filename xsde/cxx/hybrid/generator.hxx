// file      : xsde/cxx/hybrid/generator.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_GENERATOR_HXX
#define CXX_HYBRID_GENERATOR_HXX

#include <cult/types.hxx>

#include <cult/containers/vector.hxx>

#include <cult/cli/options.hxx>
#include <cult/cli/options-spec.hxx>

#include <xsd-frontend/semantic-graph/elements.hxx> // Path
#include <xsd-frontend/semantic-graph/schema.hxx>

#include <xsde.hxx>
#include <type-map/type-map.hxx>
#include <cxx/hybrid/cli.hxx>

#include <cxx/parser/cli.hxx>
#include <cxx/serializer/cli.hxx>

namespace CXX
{
  namespace Hybrid
  {
    using namespace Cult::Types;

    class Generator
    {
    public:
      static Void
      usage ();

      static CLI::OptionsSpec
      options_spec ();

      static Parser::CLI::Options*
      parser_options (CLI::Options const&);

      static Serializer::CLI::Options*
      serializer_options (CLI::Options const&);

      // Calculate type sizes.
      //
      static Void
      calculate_size (
        CLI::Options const&,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const&,
        WarningSet const& disabled_warnings);

      // Assign names to global declarations.
      //
      static Void
      process_tree_names (
        CLI::Options const&,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const&);

      static Void
      process_parser_names (
        CLI::Options const&,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const&);

      static Void
      process_serializer_names (
        CLI::Options const&,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const&);

      // Generate code.
      //
      struct Failed {};

      static UnsignedLong
      generate_tree (
        CLI::Options const&,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const&,
        WarningSet const& disabled_warnings,
        TypeMap::Namespaces& parser_type_map,
        TypeMap::Namespaces& serializer_type_map,
        FileList&,
        AutoUnlinks&);

      static UnsignedLong
      generate_parser (
        CLI::Options const&,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const&,
        WarningSet const& disabled_warnings,
        FileList&,
        AutoUnlinks&);

      static UnsignedLong
      generate_serializer (
        CLI::Options const&,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const&,
        WarningSet const& disabled_warnings,
        FileList&,
        AutoUnlinks&);

    private:
      Generator ();
    };
  }
}

#endif // CXX_HYBRID_GENERATOR_HXX
