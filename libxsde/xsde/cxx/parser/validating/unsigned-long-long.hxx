// file      : xsde/cxx/parser/validating/unsigned-long-long.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_VALIDATING_UNSIGNED_LONG_LONG_HXX
#define XSDE_CXX_PARSER_VALIDATING_UNSIGNED_LONG_LONG_HXX

#include <xsde/cxx/parser/validating/number.hxx>
#include <xsde/cxx/parser/validating/xml-schema-pskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        // 64-bit unsigned integer
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct unsigned_long_pimpl: virtual unsigned_long_pskel, number
#else
        struct unsigned_long_pimpl: unsigned_long_pskel, number
#endif
        {
          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual void
          _post ();

          virtual unsigned long long
          post_unsigned_long ();

        protected:
          // We only need strlen("018446744073709551615") + 1 characters to
          // hold all valid and trimmed string representations of unsigned
          // long long.
          //
          char str_[22];
          unsigned long long value_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_VALIDATING_UNSIGNED_LONG_LONG_HXX
