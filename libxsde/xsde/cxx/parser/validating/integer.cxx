// file      : xsde/cxx/parser/validating/integer.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <stdlib.h> // strtoul
#include <limits.h> // LONG_MIN, LONG_MAX

#include <xsde/cxx/errno.hxx>

#include <xsde/cxx/parser/validating/integer.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        void integer_pimpl::
        _pre ()
        {
          size_ = 0;
          sign_ = none;
          state_ = leading_ws;
        }

        void integer_pimpl::
        _characters (const ro_string& s)
        {
          if (!parse (s, str_, 20))
            _schema_error (schema_error::invalid_integer_value);
        }

        void integer_pimpl::
        _post ()
        {
          ro_string tmp (str_, size_);
          size_t size = trim_right (tmp);

          if (size != 0 && tmp[0] != '-' && tmp[0] != '+')
          {
            str_[size] = '\0';

            char* p;
            set_errno (0);
            unsigned long ul = strtoul (str_, &p, 10);

            bool neg = (sign_ == minus);

            if (*p != '\0' ||
                get_errno () != 0 ||
                (neg && ul > static_cast<unsigned long> (LONG_MIN)) ||
                (!neg && ul > LONG_MAX))
              _schema_error (schema_error::invalid_integer_value);

            value_ = neg
              ? (ul == static_cast<unsigned long> (LONG_MIN)
                 ? LONG_MIN: -static_cast<long> (ul))
              : static_cast<long> (ul);

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
          }
          else
            _schema_error (schema_error::invalid_integer_value);
        }

        long integer_pimpl::
        post_integer ()
        {
          return value_;
        }
      }
    }
  }
}
