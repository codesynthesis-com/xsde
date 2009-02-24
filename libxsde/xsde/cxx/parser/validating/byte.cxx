// file      : xsde/cxx/parser/validating/byte.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <stdlib.h> // strtoul

#include <xsde/cxx/parser/validating/byte.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        void byte_pimpl::
        _pre ()
        {
          size_ = 0;
          sign_ = none;
          state_ = leading_ws;
        }

        void byte_pimpl::
        _characters (const ro_string& s)
        {
          if (!parse (s, str_, 4))
            _schema_error (schema_error::invalid_byte_value);
        }

        void byte_pimpl::
        _post ()
        {
          ro_string tmp (str_, size_);
          size_t size = trim_right (tmp);

          if (size != 0 && tmp[0] != '-' && tmp[0] != '+')
          {
            str_[size] = '\0';

            // No need to check errno since our string representation
            // can never overflow an unsigned long.
            //
            char* p;
            unsigned long ul = strtoul (str_, &p, 10);

            bool neg = (sign_ == minus);

            if (*p != '\0' || (neg && ul > 128) || (!neg && ul > 127))
              _schema_error (schema_error::invalid_byte_value);

            value_ = neg
              ? static_cast<signed char> (-static_cast<short> (ul))
              : static_cast<signed char> (ul);
          }
          else
            _schema_error (schema_error::invalid_byte_value);
        }

        signed char byte_pimpl::
        post_byte ()
        {
          return value_;
        }
      }
    }
  }
}

