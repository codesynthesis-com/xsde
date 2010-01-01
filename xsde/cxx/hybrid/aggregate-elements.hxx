// file      : xsde/cxx/hybrid/aggregate-elements.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_AGGREGATE_ELEMENTS_HXX
#define CXX_HYBRID_AGGREGATE_ELEMENTS_HXX

#include <cult/containers/map.hxx>

#include <cxx/hybrid/elements.hxx>

namespace CXX
{
  namespace Hybrid
  {
    typedef
    Cult::Containers::Map<SemanticGraph::Type*, String>
    TypeInstanceMap;

    struct InstanceInfo
    {
      SemanticGraph::Type* type;
      String name;
    };

    typedef Cult::Containers::Map<String, InstanceInfo> TypeIdInstanceMap;
  }
}

#endif // CXX_HYBRID_AGGREGATE_ELEMENTS_HXX
