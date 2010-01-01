// file      : xsde/cxx/serializer/validating/language.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_LANGUAGE_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_LANGUAGE_HXX

#include <xsde/cxx/serializer/validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct language_simpl: virtual language_sskel
#else
        struct language_simpl: language_sskel
#endif
        {
          virtual
          ~language_simpl ();

          language_simpl (bool free = false);

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

#include <xsde/cxx/serializer/validating/language.ixx>

#endif // XSDE_CXX_SERIALIZER_VALIDATING_LANGUAGE_HXX
