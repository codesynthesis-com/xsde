// file      : xsde/cxx/xml/ncname.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_XML_NCNAME_HXX
#define XSDE_CXX_XML_NCNAME_HXX

#include <stddef.h> // size_t

namespace xsde
{
  namespace cxx
  {
    namespace xml
    {
      bool
      valid_ncname (const char* s, size_t size);
    }
  }
}

#endif // XSDE_CXX_XML_NCNAME_HXX
