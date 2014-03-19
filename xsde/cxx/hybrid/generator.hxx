// file      : xsde/cxx/hybrid/generator.hxx
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_GENERATOR_HXX
#define CXX_HYBRID_GENERATOR_HXX

#include <memory>  // std::auto_ptr

#include <xsd-frontend/semantic-graph/elements.hxx> // Path
#include <xsd-frontend/semantic-graph/schema.hxx>

#include <xsde.hxx>
#include <types.hxx>
#include <type-map/type-map.hxx>

#include <cxx/parser/options.hxx>
#include <cxx/serializer/options.hxx>
#include <cxx/hybrid/options.hxx>

namespace CXX
{
  namespace Hybrid
  {
    class Generator
    {
    public:
      struct Failed {};

      static void
      usage ();

      static std::auto_ptr<Parser::options>
      parser_options (options const&,
                      XSDFrontend::SemanticGraph::Schema&,
                      XSDFrontend::SemanticGraph::Path const&);

      static std::auto_ptr<Serializer::options>
      serializer_options (options const&,
                          XSDFrontend::SemanticGraph::Schema&,
                          XSDFrontend::SemanticGraph::Path const&);

      // Calculate type sizes.
      //
      static void
      calculate_size (
        options const&,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const&,
        WarningSet const& disabled_warnings);

      // Assign names to global declarations.
      //
      static void
      process_tree_names (
        options const&,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const&);

      static void
      process_parser_names (
        options const&,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const&);

      static void
      process_serializer_names (
        options const&,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const&);

      // Generate code.
      //
      static size_t
      generate_tree (
        options const&,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const&,
        bool file_per_type,
        WarningSet const& disabled_warnings,
        TypeMap::Namespaces& parser_type_map,
        TypeMap::Namespaces& serializer_type_map,
        FileList&,
        AutoUnlinks&);

      static size_t
      generate_parser (
        options const&,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const&,
        bool file_per_type,
        WarningSet const& disabled_warnings,
        FileList&,
        AutoUnlinks&);

      static size_t
      generate_serializer (
        options const&,
        XSDFrontend::SemanticGraph::Schema&,
        XSDFrontend::SemanticGraph::Path const&,
        bool file_per_type,
        WarningSet const& disabled_warnings,
        FileList&,
        AutoUnlinks&);

    private:
      Generator ();
    };
  }
}

#endif // CXX_HYBRID_GENERATOR_HXX
