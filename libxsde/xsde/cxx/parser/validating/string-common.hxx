// file      : xsde/cxx/parser/validating/string-common.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_VALIDATING_STRING_COMMON_HXX
#define XSDE_CXX_PARSER_VALIDATING_STRING_COMMON_HXX

#include <xsde/cxx/parser/validating/xml-schema-pskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        struct string_common
        {
          static bool
          validate_facets (const char* s,
                           size_t n,
                           const string_facets::facets&,
                           context&);
        };
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_VALIDATING_STRING_COMMON_HXX
