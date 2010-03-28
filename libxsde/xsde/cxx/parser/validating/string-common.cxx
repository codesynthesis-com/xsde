// file      : xsde/cxx/parser/validating/string-common.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // strcmp
#include <xsde/cxx/parser/validating/string-common.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        void string_common::
        validate_facets (const char* s,
                         size_t n,
                         const string_facets::facets& f,
                         context& ctx)
        {
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
