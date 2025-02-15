// file      : xsde/cxx/parser/non-validating/positive-integer.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_NON_VALIDATING_POSITIVE_INTEGER_HXX
#define XSDE_CXX_PARSER_NON_VALIDATING_POSITIVE_INTEGER_HXX

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
        // Arbitrary-length positive integer. Mapped to unsigned long.
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct positive_integer_pimpl: virtual positive_integer_pskel, number
#else
        struct positive_integer_pimpl: positive_integer_pskel, number
#endif
        {
          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual unsigned long
          post_positive_integer ();

        protected:
          // We only need strlen("018446744073709551615") + 1 characters to
          // hold all valid and trimmed string representations of unsigned
          // long, assuming we are on a 64 bit machine.
          //
          char str_[22];
        };
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_NON_VALIDATING_POSITIVE_INTEGER_HXX
