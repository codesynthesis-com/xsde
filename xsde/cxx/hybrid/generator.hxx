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

      struct Failed {};

      static Void
      calculate_size (
        CLI::Options const& options,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const& file);

      static UnsignedLong
      generate_tree (
        CLI::Options const& options,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const& file,
        const WarningSet& disabled_warnings,
        TypeMap::Namespaces& parser_type_map,
        TypeMap::Namespaces& serializer_type_map,
        FileList& file_list,
        AutoUnlinks& unlinks);

      static UnsignedLong
      generate_parser (
        CLI::Options const& options,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const& file,
        const WarningSet& disabled_warnings,
        FileList& file_list,
        AutoUnlinks& unlinks);

      static UnsignedLong
      generate_serializer (
        CLI::Options const& options,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const& file,
        const WarningSet& disabled_warnings,
        FileList& file_list,
        AutoUnlinks& unlinks);

    private:
      Generator ();
    };
  }
}

#endif // CXX_HYBRID_GENERATOR_HXX
