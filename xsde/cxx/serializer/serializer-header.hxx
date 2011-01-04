// file      : xsde/cxx/serializer/serializer-header.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_SERIALIZER_SERIALIZER_HEADER_HXX
#define CXX_SERIALIZER_SERIALIZER_HEADER_HXX

#include <cxx/serializer/elements.hxx>

namespace CXX
{
  namespace Serializer
  {
    Void
    generate_serializer_header (Context&, Boolean generate_xml_schema);
  }
}

#endif  // CXX_SERIALIZER_SERIALIZER_HEADER_HXX
