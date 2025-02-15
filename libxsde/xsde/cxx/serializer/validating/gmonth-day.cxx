// file      : xsde/cxx/serializer/validating/gmonth-day.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <stdio.h> // sprintf/snprintf

#include <xsde/cxx/serializer/validating/gmonth-day.hxx>
#include <xsde/cxx/serializer/validating/time-zone.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        void gmonth_day_simpl::
        pre (const gmonth_day& value)
        {
          value_ = value;
        }

        void gmonth_day_simpl::
        _serialize_content ()
        {
          // We only need strlen("--MM-DD+hh:mm") + 1 characters to hold all
          // representations of gMonthDay.
          //
          char str[14];

          unsigned short m = value_.month ();
          unsigned short d = value_.day ();

          // Validate day according to the XML Schema 1.1 specification:
          //
          // The day value must be no more than 30 if month is one of 4, 6, 9,
          // or 11, and no more than 29 if month is 2.
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
            max_day = 29;
            break;
          default:
            break;
          }

          if (m > 0 && m < 13 && d > 0 && d <= max_day &&
              (!value_.zone_present () || bits::valid_time_zone (value_)))
          {
#ifdef XSDE_SNPRINTF
            int n = snprintf (str, 8, "--%.2u-%.2u", m, d);
#else
            int n = sprintf (str, "--%.2u-%.2u", m, d);
#endif
            if (n > 0 && n < 8)
            {
              if (value_.zone_present ())
              {
                if (int z = bits::serialize_time_zone (str + n, value_))
                  n += z;
                else
                {
                  _schema_error (schema_error::invalid_gmonth_day_value);
                  return;
                }
              }

              _characters (str, static_cast<size_t> (n));
            }
            else
              _schema_error (schema_error::invalid_gmonth_day_value);
          }
          else
            _schema_error (schema_error::invalid_gmonth_day_value);
        }
      }
    }
  }
}
