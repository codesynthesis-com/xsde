// file      : xsde/cxx/parser/non-validating/short.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_NON_VALIDATING_SHORT_HXX
#define XSDE_CXX_PARSER_NON_VALIDATING_SHORT_HXX

#include <xsde/cxx/parser/non-validating/number.hxx>
#include <xsde/cxx/parser/non-validating/xml-schema-pskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace non_validating
      {
        // 16-bit signed integer
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct short_pimpl: virtual short_pskel, number
#else
        struct short_pimpl: short_pskel, number
#endif
        {
          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual short
          post_short ();

        protected:
          // We only need strlen("032768") + 1 characters to hold all valid
          // and trimmed string representations of short.
          //
          char str_[7];
        };
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_NON_VALIDATING_SHORT_HXX
