// file      : xsde/cxx/hybrid/parser-source.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_PARSER_SOURCE_HXX
#define CXX_HYBRID_PARSER_SOURCE_HXX

#include <xsd-frontend/semantic-graph/schema.hxx>

#include <cxx/hybrid/elements.hxx>

namespace CXX
{
  namespace Hybrid
  {
    Void
    generate_parser_source (Context&);
  }
}

#endif // CXX_HYBRID_PARSER_SOURCE_HXX
