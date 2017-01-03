// file      : xsde/cxx/serializer/genx/xml-error.hxx
// copyright : Copyright (c) 2005-2017 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_GENX_XML_ERROR_HXX
#define XSDE_CXX_SERIALIZER_GENX_XML_ERROR_HXX

#include <xsde/c/genx/genx.h>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace genx
      {
        typedef genxStatus xml_error;

        const char*
        xml_error_text (xml_error);
      }
    }
  }
}

#endif  // XSDE_CXX_SERIALIZER_GENX_XML_ERROR_HXX
