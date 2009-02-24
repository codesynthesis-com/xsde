// file      : xsde/cxx/serializer/non-validating/uri.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_NON_VALIDATING_URI_HXX
#define XSDE_CXX_SERIALIZER_NON_VALIDATING_URI_HXX

#include <xsde/cxx/serializer/non-validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct uri_simpl: virtual uri_sskel
#else
        struct uri_simpl: uri_sskel
#endif
        {
          virtual
          ~uri_simpl ();

          uri_simpl (bool free = false);

          virtual void
          pre (const char*);

          virtual void
          _serialize_content ();

        protected:
          bool free_;
          const char* value_;
        };
      }
    }
  }
}

#include <xsde/cxx/serializer/non-validating/uri.ixx>

#endif // XSDE_CXX_SERIALIZER_NON_VALIDATING_URI_HXX
