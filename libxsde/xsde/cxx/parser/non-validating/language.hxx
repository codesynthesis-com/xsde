// file      : xsde/cxx/parser/non-validating/language.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_NON_VALIDATING_LANGUAGE_HXX
#define XSDE_CXX_PARSER_NON_VALIDATING_LANGUAGE_HXX

#include <xsde/cxx/string.hxx>

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
        struct language_pimpl: virtual language_pskel
#else
        struct language_pimpl: language_pskel
#endif
        {
          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual char*
          post_language ();

        protected:
          string str_;
        };
      }
    }
  }
}

#endif // XSDE_CXX_PARSER_NON_VALIDATING_LANGUAGE_HXX
