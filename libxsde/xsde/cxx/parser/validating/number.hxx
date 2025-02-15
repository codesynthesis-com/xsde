// file      : xsde/cxx/parser/validating/number.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_VALIDATING_NUMBER_HXX
#define XSDE_CXX_PARSER_VALIDATING_NUMBER_HXX

#include <stddef.h> // size_t

#include <xsde/cxx/ro-string.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        class number
        {
        protected:
          bool
          parse (const ro_string&, char* str, size_t max);

        protected:
          size_t size_;
          enum {leading_ws, sign, leading_zeros, literal, trailing_ws} state_;
          enum {none, plus, minus} sign_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_VALIDATING_NUMBER_HXX
