// file      : xsde/cxx/parser/non-validating/id-stl.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_NON_VALIDATING_ID_STL_HXX
#define XSDE_CXX_PARSER_NON_VALIDATING_ID_STL_HXX

#include <string>

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
        struct id_pimpl: virtual id_pskel
#else
        struct id_pimpl: id_pskel
#endif
        {
          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual std::string
          post_id ();

        protected:
          std::string str_;
        };
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_NON_VALIDATING_ID_STL_HXX
