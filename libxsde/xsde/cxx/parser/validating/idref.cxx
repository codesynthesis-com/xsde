// file      : xsde/cxx/parser/validating/idref.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/config.hxx>

#include <xsde/cxx/xml/ncname.hxx>

#include <xsde/cxx/parser/validating/idref.hxx>
#include <xsde/cxx/parser/validating/string-common.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        void idref_pimpl::
        _pre ()
        {
#ifdef XSDE_EXCEPTIONS
          str_.assign ("", 0);
#else
          if (str_.assign ("", 0))
            _sys_error (sys_error::no_memory);
#endif
        }

        void idref_pimpl::
        _characters (const ro_string& s)
        {
          if (str_.size () == 0)
          {
            ro_string tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
            {
#ifdef XSDE_EXCEPTIONS
              str_.append (tmp.data (), tmp.size ());
#else
              if (str_.append (tmp.data (), tmp.size ()))
                _sys_error (sys_error::no_memory);
#endif
            }
          }
          else
          {
#ifdef XSDE_EXCEPTIONS
            str_.append (s.data (), s.size ());
#else
            if (str_.append (s.data (), s.size ()))
              _sys_error (sys_error::no_memory);
#endif
          }
        }

        void idref_pimpl::
        _post ()
        {
          if (!string_common::validate_facets (str_, _facets (), _context ()))
            return;

          if (!xml::valid_ncname (str_.data (), str_.size ()))
            _schema_error (schema_error::invalid_idref_value);
        }

        char* idref_pimpl::
        post_idref ()
        {
          return str_.detach ();
        }
      }
    }
  }
}
