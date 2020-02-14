// file      : xsde/cxx/parser/validating/token.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_VALIDATING_TOKEN_HXX
#define XSDE_CXX_PARSER_VALIDATING_TOKEN_HXX

#include <xsde/cxx/string.hxx>

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
        struct token_pimpl: virtual token_pskel
#else
        struct token_pimpl: token_pskel
#endif
        {
          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual void
          _post ();

          virtual char*
          post_token ();

        protected:
          string str_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_VALIDATING_TOKEN_HXX
