// file      : xsde/cxx/serializer/validating/normalized-string.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/validating/normalized-string.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        normalized_string_simpl::
        ~normalized_string_simpl ()
        {
          if (free_)
            delete[] const_cast<char*> (value_);
        }

        void normalized_string_simpl::
        pre (const char* value)
        {
          value_ = value;
        }

        void normalized_string_simpl::
        _serialize_content ()
        {
          const char* s = value_;

          for (; *s != '\0'; ++s)
          {
            if (*s == 0x0A || *s == 0x0D || *s == 0x09)
              break;
          }

          if (*s == '\0')
            _characters (value_);
          else
            _schema_error (schema_error::invalid_normalized_string_value);

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

