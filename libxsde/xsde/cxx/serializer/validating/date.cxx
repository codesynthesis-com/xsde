// file      : xsde/cxx/serializer/validating/date.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <stdio.h> // sprintf/snprintf

#include <xsde/cxx/serializer/validating/date.hxx>
#include <xsde/cxx/serializer/validating/time-zone.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        void date_simpl::
        pre (const date& value)
        {
          value_ = value;
        }

        void date_simpl::
        _serialize_content ()
        {
          // We only need strlen("-2147483649-MM-DD+hh:mm") + 1 characters to
          // hold all representations of date.
          //
          char str[24];

          int y = value_.year ();
          unsigned short m = value_.month ();
          unsigned short d = value_.day ();

          // Validate day according to the XML Schema 1.1 specification:
          //
          // The day value must be no more than 30 if month is one of 4, 6, 9,
          // or 11, no more than 28 if month is 2 and year is not divisible by
          // 4, or is divisible by 100 but not by 400, and no more than 29 if
          // month is 2 and year is divisible by 400, or by 4 but not by 100.
          //
          unsigned short max_day = 31;
          switch (m)
          {
          case 4:
          case 6:
          case 9:
          case 11:
            max_day = 30;
            break;
          case 2:
            max_day = ((y % 400 == 0) ||
                       (y % 4 == 0 && y % 100 != 0) ? 29 : 28);
            break;
          default:
            break;
          }

          if (y != 0 && m > 0 && m < 13 && d > 0 && d <= max_day &&
              (!value_.zone_present () || bits::valid_time_zone (value_)))
          {
#ifdef XSDE_SNPRINTF
            int n = snprintf (str, 18, "%.4d-%.2u-%.2u",
                              value_.year (), value_.month (), value_.day ());
#else
            int n = sprintf (str, "%.4d-%.2u-%.2u",
                             value_.year (), value_.month (), value_.day ());
#endif
            if (n > 0 && n < 18)
            {
              if (value_.zone_present ())
              {
                if (int z = bits::serialize_time_zone (str + n, value_))
                  n += z;
                else
                {
                  _schema_error (schema_error::invalid_date_value);
                  return;
                }
              }

              _characters (str, static_cast<size_t> (n));
            }
            else
              _schema_error (schema_error::invalid_date_value);
          }
          else
            _schema_error (schema_error::invalid_date_value);
        }
      }
    }
  }
}
