// file      : xsde/cxx/serializer/validating/token.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/validating/token.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        token_simpl::
        ~token_simpl ()
        {
          if (free_)
            delete[] const_cast<char*> (value_);
        }

        void token_simpl::
        pre (const char* value)
        {
          value_ = value;
        }

        void token_simpl::
        _serialize_content ()
        {
          bool ok = true;
          const char* s = value_;

          // No leading whitespaces.
          //
          if (*s == 0x20)
            ok = false;

          for (; ok && *s != '\0'; ++s)
          {
            if (*s == 0x0A || *s == 0x0D || *s == 0x09 ||
                (*s == 0x20 && *(s + 1) == 0x20))
              ok = false;
          }

          // No trailing whitespaces.
          //
          if (ok && s != value_ && *(s - 1) == 0x20)
            ok = false;

          if (ok)
            _characters (value_);
          else
            _schema_error (schema_error::invalid_token_value);

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
