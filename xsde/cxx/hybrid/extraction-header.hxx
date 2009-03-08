// file      : xsde/cxx/hybrid/extraction-header.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_EXTRACTION_HEADER_HXX
#define CXX_HYBRID_EXTRACTION_HEADER_HXX

#include <xsd-frontend/semantic-graph/schema.hxx>

#include <cxx/hybrid/elements.hxx>

namespace CXX
{
  namespace Hybrid
  {
    Void
    generate_extraction_header (Context&);
  }
}

#endif // CXX_HYBRID_EXTRACTION_HEADER_HXX