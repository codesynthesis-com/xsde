// file      : xsde/cxx/parser/validating/nmtoken-stl.hxx
// copyright : Copyright (c) 2005-2014 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_VALIDATING_NMTOKEN_STL_HXX
#define XSDE_CXX_PARSER_VALIDATING_NMTOKEN_STL_HXX

#include <string>

#include <xsde/cxx/parser/validating/xml-schema-pskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct nmtoken_pimpl: virtual nmtoken_pskel
#else
        struct nmtoken_pimpl: nmtoken_pskel
#endif
        {
          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual void
          _post ();

          virtual std::string
          post_nmtoken ();

        protected:
          std::string str_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_VALIDATING_NMTOKEN_STL_HXX
