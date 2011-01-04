// file      : processing/inheritance/processor.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2006-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef PROCESSING_INHERITANCE_PROCESSOR_HXX
#define PROCESSING_INHERITANCE_PROCESSOR_HXX

#include <cult/types.hxx>

#include <xsd-frontend/semantic-graph/elements.hxx> // Path
#include <xsd-frontend/semantic-graph/schema.hxx>

namespace Processing
{
  namespace Inheritance
  {
    using namespace Cult::Types;

    class Processor
    {
    public:
      struct Failed {};

      // If a type of an element or attribute has a context entry
      // with the by_value_key key and it is true, then this type
      // is rearranged to appear before the type containing this
      // element/attribute.
      //
      Void
      process (XSDFrontend::SemanticGraph::Schema&,
               XSDFrontend::SemanticGraph::Path const& file,
               Char const* by_value_key = 0);
    };
  }
}

#endif // PROCESSING_INHERITANCE_PROCESSOR_HXX
