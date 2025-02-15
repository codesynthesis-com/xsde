// file      : xsde/cxx/parser/non-validating/any-simple-type.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_NON_VALIDATING_ANY_SIMPLE_TYPE_HXX
#define XSDE_CXX_PARSER_NON_VALIDATING_ANY_SIMPLE_TYPE_HXX

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
        struct any_simple_type_pimpl: virtual any_simple_type_pskel
#else
        struct any_simple_type_pimpl: any_simple_type_pskel
#endif
        {
          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual char*
          post_any_simple_type ();

        protected:
          string str_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_NON_VALIDATING_ANY_SIMPLE_TYPE_HXX
