// file      : xsde/cxx/parser/non-validating/duration.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_NON_VALIDATING_DURATION_HXX
#define XSDE_CXX_PARSER_NON_VALIDATING_DURATION_HXX

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
        struct duration_pimpl: virtual duration_pskel
#else
        struct duration_pimpl: duration_pskel
#endif
        {
          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual duration
          post_duration ();

        protected:
          string str_;
        };
      }
    }
  }
}

#endif // XSDE_CXX_PARSER_NON_VALIDATING_DURATION_HXX
