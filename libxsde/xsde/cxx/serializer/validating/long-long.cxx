// file      : xsde/cxx/serializer/validating/long-long.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <stdio.h> // sprintf/snprintf

#include <xsde/cxx/serializer/validating/long-long.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        void long_simpl::
        pre (long long value)
        {
          value_ = value;
        }

        void long_simpl::
        _serialize_content ()
        {
          // Check facets.
          //
          const facets& f = _facets ();

          if (f.min_set_ &&
              (value_ < f.min_ || (!f.min_inc_ && value_ == f.min_)))
          {
            _schema_error (schema_error::value_less_than_min);
            return;
          }

          if (f.max_set_ &&
              (value_ > f.max_ || (!f.max_inc_ && value_ == f.max_)))
          {
            _schema_error (schema_error::value_greater_than_max);
            return;
          }

          // We only need strlen("-9223372036854775808") + 1 characters to
          // hold all representations of long long.
          //
          char str[21];

#ifdef XSDE_SNPRINTF
          int n = snprintf (str, 21, "%lld", value_);
#else
          int n = sprintf (str, "%lld", value_);
#endif
          if (n > 0 && n < 21)
            _characters (str, static_cast<size_t> (n));
          else
            _schema_error (schema_error::invalid_long_value);
        }
      }
    }
  }
}
