// file      : xsde/cxx/parser/non-validating/nmtoken.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_NON_VALIDATING_NMTOKEN_HXX
#define XSDE_CXX_PARSER_NON_VALIDATING_NMTOKEN_HXX

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
        struct nmtoken_pimpl: virtual nmtoken_pskel
#else
        struct nmtoken_pimpl: nmtoken_pskel
#endif
        {
          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual char*
          post_nmtoken ();

        protected:
          string str_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_NON_VALIDATING_NMTOKEN_HXX
