// file      : xsde/cxx/hybrid/parser-source.hxx
// copyright : Copyright (c) 2005-2014 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_PARSER_SOURCE_HXX
#define CXX_HYBRID_PARSER_SOURCE_HXX

#include <cxx/hybrid/elements.hxx>

namespace CXX
{
  namespace Hybrid
  {
    void
    generate_parser_source (Context&, Regex const& hxx_obj_expr);
  }
}

#endif // CXX_HYBRID_PARSER_SOURCE_HXX
