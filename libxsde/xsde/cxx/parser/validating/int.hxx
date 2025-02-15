// file      : xsde/cxx/parser/validating/int.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_VALIDATING_INT_HXX
#define XSDE_CXX_PARSER_VALIDATING_INT_HXX

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
        // 32-bit signed integer
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct int_pimpl: virtual int_pskel, number
#else
        struct int_pimpl: int_pskel, number
#endif
        {
          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual void
          _post ();

          virtual int
          post_int ();

        protected:
          // We only need strlen("02147483648") + 1 characters to hold all
          // valid and trimmed string representations of int.
          //
          char str_[12];
          int value_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_VALIDATING_INT_HXX
