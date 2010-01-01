// file      : xsde/cxx/parser/expat/xml-error.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_EXPAT_XML_ERROR_HXX
#define XSDE_CXX_PARSER_EXPAT_XML_ERROR_HXX

#include <xsde/c/expat/expat.h>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace expat
      {
        typedef XML_Error xml_error;

        const char*
        xml_error_text (xml_error);
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_EXPAT_XML_ERROR_HXX
