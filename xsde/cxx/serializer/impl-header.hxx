// file      : xsde/cxx/serializer/impl-header.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_SERIALIZER_IMPL_HEADER_HXX
#define CXX_SERIALIZER_IMPL_HEADER_HXX

#include <xsd-frontend/semantic-graph/schema.hxx>

#include <cxx/serializer/elements.hxx>

namespace CXX
{
  namespace Serializer
  {
    Void
    generate_impl_header (Context&);
  }
}

#endif // CXX_SERIALIZER_IMPL_HEADER_HXX
