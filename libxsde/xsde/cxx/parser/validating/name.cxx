// file      : xsde/cxx/parser/validating/name.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/config.hxx>

#include <xsde/cxx/xml/char-table.hxx>

#include <xsde/cxx/parser/validating/name.hxx>
#include <xsde/cxx/parser/validating/string-common.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        void name_pimpl::
        _pre ()
        {
#ifdef XSDE_EXCEPTIONS
          str_.assign ("", 0);
#else
          if (str_.assign ("", 0))
            _sys_error (sys_error::no_memory);
#endif
        }

        void name_pimpl::
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

        void name_pimpl::
        _post ()
        {
          if (!string_common::validate_facets (str_, _facets (), _context ()))
            return;

          typedef string::size_type size_type;
          size_type size = str_.size ();

          // For now we are only checking the US-ASCII characters.
          //

          bool ok = (size != 0);

          if (ok)
          {
            unsigned char c = static_cast<unsigned char> (str_[0]);

            ok = c >= 0x80 || (xml::char_table[c] & xml::name_first_char_mask);

            if (ok)
            {
              for (ro_string::size_type i = 1; i < size; ++i)
              {
                c = static_cast<unsigned char> (str_[i]);

                if (c < 0x80 && !(xml::char_table[c] & xml::name_char_mask))
                {
                  ok = false;
                  break;
                }
              }
            }
          }

          if (!ok)
            _schema_error (schema_error::invalid_name_value);
        }

        char* name_pimpl::
        post_name ()
        {
          return str_.detach ();
        }
      }
    }
  }
}
