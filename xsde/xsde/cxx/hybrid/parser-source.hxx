// file      : xsde/cxx/hybrid/parser-source.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_HYBRID_PARSER_SOURCE_HXX
#define XSDE_CXX_HYBRID_PARSER_SOURCE_HXX

#include <xsde/cxx/hybrid/elements.hxx>

namespace CXX
{
  namespace Hybrid
  {
    void
    generate_parser_source (Context&, Regex const& hxx_obj_expr);
  }
}

#endif // XSDE_CXX_HYBRID_PARSER_SOURCE_HXX
