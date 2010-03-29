// file      : xsde/cxx/serializer/validating/string-common.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // strlen
#include <xsde/cxx/string-search.hxx>
#include <xsde/cxx/serializer/validating/string-common.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        bool string_common::
        validate_facets (const char* s,
                         const string_facets::facets& f,
                         context& ctx)
        {
          if (f.length_set_ ||
              f.min_length_set_ ||
              f.max_length_set_ ||
              f.enum_count_ != 0)
          {
            return validate_facets (s, strlen (s), f, ctx);
          }

          return true;
        }

        bool string_common::
        validate_facets (const char* s,
                         size_t n,
                         const string_facets::facets& f,
                         context& ctx)
        {
          if (f.length_set_ && n != f.length_)
          {
            ctx.schema_error (schema_error::length_not_equal_prescribed);
            return false;
          }

          if (f.min_length_set_ && n < f.min_length_)
          {
            ctx.schema_error (schema_error::length_less_than_min);
            return false;
          }

          if (f.max_length_set_ && n > f.max_length_)
          {
            ctx.schema_error (schema_error::length_greater_than_max);
            return false;
          }

          if (f.enum_count_ != 0)
          {
            if (search (f.enum_, f.enum_count_, s) == f.enum_count_)
            {
              ctx.schema_error (schema_error::value_not_in_enumeration);
              return false;
            }
          }

          return true;
        }
      }
    }
  }
}
