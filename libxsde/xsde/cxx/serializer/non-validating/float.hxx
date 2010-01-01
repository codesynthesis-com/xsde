// file      : xsde/cxx/serializer/non-validating/float.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_NON_VALIDATING_FLOAT_HXX
#define XSDE_CXX_SERIALIZER_NON_VALIDATING_FLOAT_HXX

#include <float.h>

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
        struct float_simpl: virtual float_sskel
#else
        struct float_simpl: float_sskel
#endif
        {
          enum notation
          {
            notation_auto,
            notation_fixed,
            notation_scientific
          };

#ifdef FLT_DIG
          float_simpl (notation = notation_auto,
                       unsigned int precision = FLT_DIG);
#else
          float_simpl (notation = notation_auto,
                       unsigned int precision = 6)
#endif

          virtual void
          pre (float);

          virtual void
          _serialize_content ();

        protected:
          notation notation_;
          unsigned int precision_;
          float value_;
        };
      }
    }
  }
}

#include <xsde/cxx/serializer/non-validating/float.ixx>

#endif  // XSDE_CXX_SERIALIZER_NON_VALIDATING_FLOAT_HXX
