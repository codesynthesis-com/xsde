// file      : xsde/cxx/parser/generator.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_GENERATOR_HXX
#define XSDE_CXX_PARSER_GENERATOR_HXX

#include <libxsd-frontend/semantic-graph/elements.hxx> // Path
#include <libxsd-frontend/semantic-graph/schema.hxx>

#include <xsde/xsde.hxx>
#include <xsde/types.hxx>
#include <xsde/type-map/type-map.hxx>
#include <xsde/cxx/parser/options.hxx>

namespace CXX
{
  namespace Parser
  {
    class Generator
    {
    public:
      static void
      usage ();

      // Assign names to global declarations.
      //
      static void
      process_names (options const&,
                     XSDFrontend::SemanticGraph::Schema&,
                     XSDFrontend::SemanticGraph::Path const&);

      // Generate code.
      //
      struct Failed {};

      static size_t
      generate (options const&,
                XSDFrontend::SemanticGraph::Schema&,
                XSDFrontend::SemanticGraph::Path const&,
                bool file_per_type,
                TypeMap::Namespaces& type_map,
                bool gen_driver,
                const WarningSet& disabled_warnings,
                FileList&,
                AutoUnlinks&);

    private:
      Generator ();
    };
  }
}

#endif // XSDE_CXX_PARSER_GENERATOR_HXX
