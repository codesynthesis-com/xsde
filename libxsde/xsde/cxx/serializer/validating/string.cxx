// file      : xsde/cxx/serializer/validating/string.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // strlen
#include <xsde/cxx/serializer/validating/string.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        string_simpl::
        ~string_simpl ()
        {
          if (free_)
            delete[] const_cast<char*> (value_);
        }

        void string_simpl::
        pre (const char* value)
        {
          value_ = value;
        }

        void string_simpl::
        _serialize_content ()
        {
          // Check facets.
          //
          if (length_set_ || min_length_set_ || max_length_set_)
          {
            size_t n = strlen (value_);
            const facets& f = _facets ();

            if (f.length_set_ && n != f.length_)
            {
              _schema_error (schema_error::length_not_equal_prescribed);
              return;
            }

            if (f.min_length_set_ && n < f.min_length_)
            {
              _schema_error (schema_error::length_less_than_min);
              return;
            }

            if (f.max_length_set_ && n > f.max_length_)
            {
              _schema_error (schema_error::length_greater_than_max);
              return;
            }
          }

          _characters (value_);

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
