// file      : xsde/cxx/parser/non-validating/long.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <stdlib.h> // strtoul

#include <xsde/cxx/parser/non-validating/long.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace non_validating
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
          parse (s, str_, 20);
        }

        long long_pimpl::
        post_long ()
        {
          ro_string tmp (str_, size_);
          size_t size = trim_right (tmp);
          str_[size] = '\0';

          unsigned long ul = strtoul (str_, 0, 10);

          return  (sign_ == minus)
            ? (ul == 2147483648UL
               ? (-2147483647 - 1)
               : -static_cast<int> (ul))
            : static_cast<int> (ul);
        }
      }
    }
  }
}
