// file      : xsde/cxx/parser/parser-header.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_PARSER_HEADER_HXX
#define XSDE_CXX_PARSER_PARSER_HEADER_HXX

#include <xsde/cxx/parser/elements.hxx>

namespace CXX
{
  namespace Parser
  {
    void
    generate_parser_header (Context&, bool generate_xml_schema);
  }
}

#endif  // XSDE_CXX_PARSER_PARSER_HEADER_HXX