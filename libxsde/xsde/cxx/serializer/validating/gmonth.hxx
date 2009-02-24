// file      : xsde/cxx/serializer/validating/gmonth.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_GMONTH_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_GMONTH_HXX

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
        struct gmonth_simpl: virtual gmonth_sskel
#else
        struct gmonth_simpl: gmonth_sskel
#endif
        {
          gmonth_simpl ();

          virtual void
          pre (const gmonth&);

          virtual void
          _serialize_content ();

        protected:
          gmonth value_;
        };
      }
    }
  }
}

#include <xsde/cxx/serializer/validating/gmonth.ixx>

#endif // XSDE_CXX_SERIALIZER_VALIDATING_GMONTH_HXX
