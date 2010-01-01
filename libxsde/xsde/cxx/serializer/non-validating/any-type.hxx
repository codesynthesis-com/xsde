// file      : xsde/cxx/serializer/non-validating/any-type.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_NON_VALIDATING_ANY_TYPE_HXX
#define XSDE_CXX_SERIALIZER_NON_VALIDATING_ANY_TYPE_HXX

#include <xsde/cxx/serializer/non-validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        // any_type
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct any_type_simpl: virtual any_type_sskel
#else
        struct any_type_simpl: any_type_sskel
#endif
        {
        };

        // any_simple_type
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct any_simple_type_simpl: virtual any_simple_type_sskel
#else
        struct any_simple_type_simpl: any_simple_type_sskel
#endif
        {
        };
      }
    }
  }
}

#endif // XSDE_CXX_SERIALIZER_NON_VALIDATING_ANY_TYPE_HXX
