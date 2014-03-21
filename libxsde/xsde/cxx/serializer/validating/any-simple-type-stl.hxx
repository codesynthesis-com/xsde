// file      : xsde/cxx/serializer/validating/any-simple-type-stl.hxx
// copyright : Copyright (c) 2005-2014 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_ANY_SIMPLE_TYPE_STL_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_ANY_SIMPLE_TYPE_STL_HXX

#include <string>

#include <xsde/cxx/serializer/validating/xml-schema-sskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct any_simple_type_simpl: virtual any_simple_type_sskel
#else
        struct any_simple_type_simpl: any_simple_type_sskel
#endif
        {
          virtual void
          pre (const std::string&);

          virtual void
          _serialize_content ();

        protected:
          std::string value_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_SERIALIZER_VALIDATING_ANY_SIMPLE_TYPE_STL_HXX
