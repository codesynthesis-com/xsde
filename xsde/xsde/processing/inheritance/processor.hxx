// file      : processing/inheritance/processor.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_PROCESSING_INHERITANCE_PROCESSOR_HXX
#define XSDE_PROCESSING_INHERITANCE_PROCESSOR_HXX

#include <libxsd-frontend/semantic-graph/elements.hxx> // Path
#include <libxsd-frontend/semantic-graph/schema.hxx>

#include <xsde/types.hxx>

namespace Processing
{
  namespace Inheritance
  {
    class Processor
    {
    public:
      struct Failed {};

      // If a type of an element or attribute has a context entry
      // with the by_value_key key and it is true, then this type
      // is rearranged to appear before the type containing this
      // element/attribute.
      //
      void
      process (XSDFrontend::SemanticGraph::Schema&,
               XSDFrontend::SemanticGraph::Path const& file,
               char const* by_value_key = 0);
    };
  }
}

#endif // XSDE_PROCESSING_INHERITANCE_PROCESSOR_HXX
