// file      : xsde/cxx/parser/validating/decimal.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_VALIDATING_DECIMAL_HXX
#define XSDE_CXX_PARSER_VALIDATING_DECIMAL_HXX

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
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct decimal_pimpl: virtual decimal_pskel, number
#else
        struct decimal_pimpl: decimal_pskel, number
#endif
        {
          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual void
          _post ();

          virtual double
          post_decimal ();

        protected:
          // Assume decimal values (without leading and trailing whitespaces)
          // cannot be longer than 127 characters.
          //
          char str_[128];
          double value_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_VALIDATING_DECIMAL_HXX
