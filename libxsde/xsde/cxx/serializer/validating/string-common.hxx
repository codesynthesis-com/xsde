// file      : xsde/cxx/serializer/validating/string-common.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_STRING_COMMON_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_STRING_COMMON_HXX

#include <xsde/cxx/config.hxx>
#include <xsde/cxx/serializer/validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        struct string_common
        {
          static void
          validate_facets (const char* s,
#ifdef XSDE_STL
                           size_t n,
#endif
                           const string_facets::facets&,
                           context&);
        };
      }
    }
  }
}

#endif  // XSDE_CXX_SERIALIZER_VALIDATING_STRING_COMMON_HXX
