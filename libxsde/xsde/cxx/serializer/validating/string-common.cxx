// file      : xsde/cxx/serializer/validating/string-common.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // strlen, strcmp
#include <xsde/cxx/serializer/validating/string-common.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        void string_common::
        validate_facets (const char* s,
#ifdef XSDE_STL
                         size_t n,
#endif
                         const string_facets::facets& f,
                         context& ctx)
        {
#ifndef XSDE_STL
          size_t n;
          if (f.length_set_ || f.min_length_set_ || f.max_length_set_)
            n = strlen (s);
#endif

          if (f.length_set_ && n != f.length_)
          {
            ctx.schema_error (schema_error::length_not_equal_prescribed);
            return;
          }

          if (f.min_length_set_ && n < f.min_length_)
          {
            ctx.schema_error (schema_error::length_less_than_min);
            return;
          }

          if (f.max_length_set_ && n > f.max_length_)
          {
            ctx.schema_error (schema_error::length_greater_than_max);
            return;
          }

          if (f.enum_count_ != 0)
          {
            size_t i = 0;

            for (; i < f.enum_count_; ++i)
            {
              if (strcmp (s, f.enum_[i]) == 0)
                break;
            }

            if (i == f.enum_count_)
            {
              ctx.schema_error (schema_error::value_not_in_enumeration);
              return;
            }
          }
        }
      }
    }
  }
}
