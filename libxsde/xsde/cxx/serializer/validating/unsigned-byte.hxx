// file      : xsde/cxx/serializer/validating/unsigned-byte.hxx
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_UNSIGNED_BYTE_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_UNSIGNED_BYTE_HXX

#include <xsde/cxx/serializer/validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        // 8-bit unsigned integer
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct unsigned_byte_simpl: virtual unsigned_byte_sskel
#else
        struct unsigned_byte_simpl: unsigned_byte_sskel
#endif
        {
          virtual void
          pre (unsigned char);

          virtual void
          _serialize_content ();

        protected:
          unsigned char value_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_SERIALIZER_VALIDATING_UNSIGNED_BYTE_HXX
