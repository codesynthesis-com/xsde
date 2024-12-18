// file      : xsde/cxx/serializer/serializer-header.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_SERIALIZER_HEADER_HXX
#define XSDE_CXX_SERIALIZER_SERIALIZER_HEADER_HXX

#include <xsde/cxx/serializer/elements.hxx>

namespace CXX
{
  namespace Serializer
  {
    void
    generate_serializer_header (Context&, bool generate_xml_schema);
  }
}

#endif  // XSDE_CXX_SERIALIZER_SERIALIZER_HEADER_HXX
