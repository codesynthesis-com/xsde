// file      : xsde/cxx/serializer/serializer-header.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_SERIALIZER_SERIALIZER_HEADER_HXX
#define CXX_SERIALIZER_SERIALIZER_HEADER_HXX

#include <xsd-frontend/semantic-graph/schema.hxx>

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
