// file      : xsde/cxx/hybrid/aggregate-elements.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_AGGREGATE_ELEMENTS_HXX
#define CXX_HYBRID_AGGREGATE_ELEMENTS_HXX

#include <map>

#include <cxx/hybrid/elements.hxx>

namespace CXX
{
  namespace Hybrid
  {
    typedef std::map<SemanticGraph::Type*, String> TypeInstanceMap;

    struct InstanceInfo
    {
      SemanticGraph::Type* type;
      String name;
    };

    typedef std::map<String, InstanceInfo> TypeIdInstanceMap;
  }
}

#endif // CXX_HYBRID_AGGREGATE_ELEMENTS_HXX
