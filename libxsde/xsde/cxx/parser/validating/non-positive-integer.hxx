// file      : xsde/cxx/parser/validating/non-positive-integer.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_VALIDATING_NON_POSITIVE_INTEGER_HXX
#define XSDE_CXX_PARSER_VALIDATING_NON_POSITIVE_INTEGER_HXX

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
        // Arbitrary-length non-positive integer. Mapped to long.
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct non_positive_integer_pimpl: virtual non_positive_integer_pskel,
#else
        struct non_positive_integer_pimpl: non_positive_integer_pskel,
#endif
                                           number
        {
          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual void
          _post ();

          virtual long
          post_non_positive_integer ();

        protected:
          // We only need strlen("09223372036854775808") + 1 characters to
          // hold all valid and trimmed string representations of long,
          // assuming we are on a 64 bit machine.
          //
          char str_[21];
          long value_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_VALIDATING_NON_POSITIVE_INTEGER_HXX
