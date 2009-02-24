// file      : xsde/cxx/parser/validating/long-long.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <stdlib.h> // strtoull

#include <xsde/cxx/errno.hxx>

#include <xsde/cxx/parser/validating/long-long.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        void long_pimpl::
        _pre ()
        {
          size_ = 0;
          sign_ = none;
          state_ = leading_ws;
        }

        void long_pimpl::
        _characters (const ro_string& s)
        {
          if (!parse (s, str_, 20))
            _schema_error (schema_error::invalid_long_value);
        }

        void long_pimpl::
        _post ()
        {
          ro_string tmp (str_, size_);
          size_t size = trim_right (tmp);

          if (size != 0 && tmp[0] != '-' && tmp[0] != '+')
          {
            str_[size] = '\0';

            char* p;
            set_errno (0);
            unsigned long long ull = strtoull (str_, &p, 10);

            bool neg = (sign_ == minus);

            if (*p != '\0' ||
                get_errno () != 0 ||
                (neg && ull > 9223372036854775808ULL) ||
                (!neg && ull > 9223372036854775807ULL))
              _schema_error (schema_error::invalid_long_value);

            value_ = neg
              ? (ull == 9223372036854775808ULL
                 ? (-9223372036854775807LL - 1)
                 : -static_cast<long long> (ull))
              : static_cast<long long> (ull);
          }
          else
            _schema_error (schema_error::invalid_long_value);
        }

        long long long_pimpl::
        post_long ()
        {
          return value_;
        }
      }
    }
  }
}
