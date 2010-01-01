// file      : xsde/cxx/serializer/validating/language.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/validating/language.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        language_simpl::
        ~language_simpl ()
        {
          if (free_)
            delete[] const_cast<char*> (value_);
        }

        void language_simpl::
        pre (const char* value)
        {
          value_ = value;
        }

        void language_simpl::
        _serialize_content ()
        {
          bool ok = true;
          const char* s = value_;

          // language := ALPHA{1,8} *(-(ALPHA | DIGIT){1,8})
          //
          for (size_t tag = 0; ; ++tag)
          {
            size_t n = 0;

            for (; *s != '\0' && n < 8; ++n, ++s)
            {
              char c = *s;

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

            if (*s == '\0')
              break;

            if (*s++ != '-')
            {
              ok = false;
              break;
            }
          }

          if (ok)
            _characters (value_);
          else
            _schema_error (schema_error::invalid_language_value);

          if (free_)
          {
            delete[] const_cast<char*> (value_);
            value_ = 0;
          }
        }
      }
    }
  }
}
