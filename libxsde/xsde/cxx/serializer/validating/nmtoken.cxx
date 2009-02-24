// file      : xsde/cxx/serializer/validating/nmtoken.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/xml/char-table.hxx>

#include <xsde/cxx/serializer/validating/nmtoken.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        nmtoken_simpl::
        ~nmtoken_simpl ()
        {
          if (free_)
            delete[] const_cast<char*> (value_);
        }

        void nmtoken_simpl::
        pre (const char* value)
        {
          value_ = value;
        }

        void nmtoken_simpl::
        _serialize_content ()
        {
          const char* s = value_;
          bool ok = (*s != '\0');

          // For now we are only checking the US-ASCII characters.
          //
          for (; ok && *s != '\0'; ++s)
          {
            unsigned char c = static_cast<unsigned char> (*s);

            if (c < 0x80 && !(xml::char_table[c] & xml::name_char_mask))
              ok = false;
          }

          if (ok)
            _characters (value_);
          else
            _schema_error (schema_error::invalid_nmtoken_value);

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
