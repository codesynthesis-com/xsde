// file      : xsde/cxx/parser/validating/base64-binary.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_VALIDATING_BASE64_BINARY_HXX
#define XSDE_CXX_PARSER_VALIDATING_BASE64_BINARY_HXX

#include <xsde/cxx/string.hxx>

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
        struct base64_binary_pimpl: virtual base64_binary_pskel
#else
        struct base64_binary_pimpl: base64_binary_pskel
#endif
        {
          ~base64_binary_pimpl ();
          base64_binary_pimpl (bool base = false);

          void
          pre_impl (buffer*);

          virtual void
          _pre ();

          virtual void
          _characters (const ro_string&);

          virtual void
          _post ();

          virtual buffer*
          post_base64_binary ();

          virtual void
          _reset ();

        protected:
          bool base_;
          string str_;
          buffer* buf_;
        };
      }
    }
  }
}

#endif // XSDE_CXX_PARSER_VALIDATING_BASE64_BINARY_HXX
