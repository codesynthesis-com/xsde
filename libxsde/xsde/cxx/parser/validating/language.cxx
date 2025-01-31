// file      : xsde/cxx/parser/validating/language.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/config.hxx>

#include <xsde/cxx/parser/validating/language.hxx>
#include <xsde/cxx/parser/validating/string-common.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        void language_pimpl::
        _pre ()
        {
#ifdef XSDE_EXCEPTIONS
          str_.assign ("", 0);
#else
          if (str_.assign ("", 0))
            _sys_error (sys_error::no_memory);
#endif
        }

        void language_pimpl::
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

        void language_pimpl::
        _post ()
        {
          if (!string_common::validate_facets (str_, _facets (), _context ()))
            return;

          typedef string::size_type size_type;
          size_type size = str_.size ();

          // language := ALPHA{1,8} *(-(ALPHA | DIGIT){1,8})
          //
          bool ok = true;
          size_type i = 0;

          for (size_type tag = 0; ; ++tag)
          {
            size_type n = 0;

            for (; i < size && n < 8; ++n, ++i)
            {
              char c = str_[i];

              if (!((c >= 'a' && c <= 'z') ||
                    (c >= 'A' && c <= 'Z') ||
                    (tag != 0 && c >= '0' && c <= '9')))
                break;
            }

            if (n == 0)
            {
              ok = false;
              break;
            }

            if (i == size)
              break;

            if (str_[i++] != '-')
            {
              ok = false;
              break;
            }
          }

          if (!ok)
            _schema_error (schema_error::invalid_language_value);
        }

        char* language_pimpl::
        post_language ()
        {
          return str_.detach ();
        }
      }
    }
  }
}
