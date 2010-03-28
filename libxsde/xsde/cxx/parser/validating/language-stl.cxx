// file      : xsde/cxx/parser/validating/language-stl.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/parser/validating/language-stl.hxx>
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
          str_.erase ();
        }

        void language_pimpl::
        _characters (const ro_string& s)
        {
          if (str_.size () == 0)
          {
            ro_string tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        void language_pimpl::
        _post ()
        {
          typedef ro_string::size_type size_type;

          ro_string tmp (str_);
          size_type size = trim_right (tmp);

          // language := ALPHA{1,8} *(-(ALPHA | DIGIT){1,8})
          //
          bool ok = true;
          size_type i = 0;

          for (size_type tag = 0; ; ++tag)
          {
            size_type n = 0;

            for (; i < size && n < 8; ++n, ++i)
            {
              char c = tmp[i];

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

            if (tmp[i++] != '-')
            {
              ok = false;
              break;
            }
          }

          if (ok)
            str_.resize (size);
          else
          {
            _schema_error (schema_error::invalid_language_value);
            return;
          }

          string_common::validate_facets (
            str_.c_str (), str_.size (), _facets (), _context ());
        }

        std::string language_pimpl::
        post_language ()
        {
          std::string r;
          r.swap (str_);
          return r;
        }
      }
    }
  }
}
