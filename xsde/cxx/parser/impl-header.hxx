// file      : xsde/cxx/parser/impl-header.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_PARSER_IMPL_HEADER_HXX
#define CXX_PARSER_IMPL_HEADER_HXX

#include <xsd-frontend/semantic-graph/schema.hxx>

#include <cxx/parser/elements.hxx>

namespace CXX
{
  namespace Parser
  {
    Void
    generate_impl_header (Context&);
  }
}

#endif  // CXX_PARSER_IMPL_HEADER_HXX
