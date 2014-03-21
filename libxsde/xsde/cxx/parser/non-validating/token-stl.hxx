// file      : xsde/cxx/parser/non-validating/token-stl.hxx
// copyright : Copyright (c) 2005-2014 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_NON_VALIDATING_TOKEN_STL_HXX
#define XSDE_CXX_PARSER_NON_VALIDATING_TOKEN_STL_HXX

#include <string>

#include <xsde/cxx/parser/non-validating/xml-schema-pskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace non_validating
      {
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct token_pimpl: virtual token_pskel
#else
        struct token_pimpl: token_pskel
#endif
        {
          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual std::string
          post_token ();

        protected:
          std::string str_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_NON_VALIDATING_TOKEN_STL_HXX
