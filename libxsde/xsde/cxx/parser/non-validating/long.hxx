// file      : xsde/cxx/parser/non-validating/long.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_NON_VALIDATING_LONG_HXX
#define XSDE_CXX_PARSER_NON_VALIDATING_LONG_HXX

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
        // Fall-back implementation when 64 bit long long is not available.
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct long_pimpl: virtual long_pskel, number
#else
        struct long_pimpl: long_pskel, number
#endif
        {
          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual long
          post_long ();

        protected:
          // We only need strlen("09223372036854775808") + 1 characters to
          // hold all valid and trimmed string representations of long long.
          //
          char str_[21];
        };
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_NON_VALIDATING_LONG_HXX
