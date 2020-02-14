// file      : xsde/cxx/parser/expat/xml-error.cxx
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
        static const char* const text_[] =
        {
          "character unrepresentable in application encoding"
        };

        const char*
        xml_error_text (xml_error e)
        {
          return e < 0x10000
            ? XML_ErrorString (static_cast<XML_Error> (e))
            : text_[e & 0xFFFF];
        }
      }
    }
  }
}
