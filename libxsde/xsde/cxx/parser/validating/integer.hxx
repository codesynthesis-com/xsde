// file      : xsde/cxx/parser/validating/integer.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_VALIDATING_INTEGER_HXX
#define XSDE_CXX_PARSER_VALIDATING_INTEGER_HXX

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
        // Arbitrary-length integer. Mapped to long.
        //
#ifdef XSDE_REUSE_STYLE_MIXIN
        struct integer_pimpl: virtual integer_pskel, number
#else
        struct integer_pimpl: integer_pskel, number
#endif
        {
          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual void
          _post ();

          virtual long
          post_integer ();

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

#endif  // XSDE_CXX_PARSER_VALIDATING_INTEGER_HXX
