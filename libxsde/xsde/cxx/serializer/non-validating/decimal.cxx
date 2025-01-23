// file      : xsde/cxx/serializer/non-validating/decimal.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <stdio.h> // sprintf/snprintf

#include <xsde/cxx/serializer/non-validating/decimal.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        void decimal_simpl::
        pre (double value)
        {
          value_ = value;
        }

        void decimal_simpl::
        _serialize_content ()
        {
          // Check facets.
          //
          const facets& f = _facets ();

          // Assume double values cannot be longer than 127 characters.
          //
          char str[128];

          unsigned int p (f.fraction_digits_set_
                          ? f.fraction_digits_
                          : precision_);

#ifdef XSDE_SNPRINTF
          int n = snprintf (str, 128, "%.*f",
                            static_cast<int> (p), value_);
#else
          int n = sprintf (str, "%.*f",
                           static_cast<int> (p), value_);
#endif
          if (n > 0 && n < 128)
          {
            // Remove trailing '0' and '.' if necessary.
            //
            while (str[n - 1] == '0')
              n--;

            if (str[n - 1] == '.')
              n--;

            _characters (str, static_cast<size_t> (n));
          }
        }
      }
    }
  }
}
