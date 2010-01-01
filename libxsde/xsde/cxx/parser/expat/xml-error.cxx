// file      : xsde/cxx/parser/expat/xml-error.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/parser/expat/xml-error.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace expat
      {
        const char*
        xml_error_text (xml_error e)
        {
          return XML_ErrorString (e);
        }
      }
    }
  }
}

