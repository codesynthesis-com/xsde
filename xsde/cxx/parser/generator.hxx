// file      : xsde/cxx/parser/generator.hxx
// copyright : Copyright (c) 2005-2014 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_PARSER_GENERATOR_HXX
#define CXX_PARSER_GENERATOR_HXX

#include <xsd-frontend/semantic-graph/elements.hxx> // Path
#include <xsd-frontend/semantic-graph/schema.hxx>

#include <xsde.hxx>
#include <types.hxx>
#include <type-map/type-map.hxx>
#include <cxx/parser/options.hxx>

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

#endif // CXX_PARSER_GENERATOR_HXX
