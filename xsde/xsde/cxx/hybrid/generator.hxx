// file      : xsde/cxx/hybrid/generator.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_HYBRID_GENERATOR_HXX
#define XSDE_CXX_HYBRID_GENERATOR_HXX

#include <memory>  // std::unique_ptr

#include <libxsd-frontend/semantic-graph/elements.hxx> // Path
#include <libxsd-frontend/semantic-graph/schema.hxx>

#include <xsde/xsde.hxx>
#include <xsde/types.hxx>
#include <xsde/type-map/type-map.hxx>

#include <xsde/cxx/parser/options.hxx>
#include <xsde/cxx/serializer/options.hxx>
#include <xsde/cxx/hybrid/options.hxx>

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

      static std::unique_ptr<Parser::options>
      parser_options (options const&,
                      XSDFrontend::SemanticGraph::Schema&,
                      XSDFrontend::SemanticGraph::Path const&);

      static std::unique_ptr<Serializer::options>
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

#endif // XSDE_CXX_HYBRID_GENERATOR_HXX
