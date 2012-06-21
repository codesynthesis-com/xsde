// file      : xsde/cxx/serializer/generator.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_SERIALIZER_GENERATOR_HXX
#define CXX_SERIALIZER_GENERATOR_HXX

#include <cult/types.hxx>

#include <cult/containers/vector.hxx>

#include <xsd-frontend/semantic-graph/elements.hxx> // Path
#include <xsd-frontend/semantic-graph/schema.hxx>

#include <xsde.hxx>

#include <type-map/type-map.hxx>

#include <cxx/serializer/options.hxx>

namespace CXX
{
  namespace Serializer
  {
    using namespace Cult::Types;

    class Generator
    {
    public:
      static Void
      usage ();

      // Assign names to global declarations.
      //
      static Void
      process_names (options const&,
                     XSDFrontend::SemanticGraph::Schema&,
                     XSDFrontend::SemanticGraph::Path const&);

      // Generate code.
      //
      struct Failed {};

      static UnsignedLong
      generate (options const&,
                XSDFrontend::SemanticGraph::Schema&,
                XSDFrontend::SemanticGraph::Path const&,
                Boolean file_per_type,
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

#endif // CXX_SERIALIZER_GENERATOR_HXX
