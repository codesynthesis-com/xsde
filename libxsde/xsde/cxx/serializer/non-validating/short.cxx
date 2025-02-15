// file      : xsde/cxx/serializer/non-validating/short.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <stdio.h> // sprintf/snprintf

#include <xsde/cxx/serializer/non-validating/short.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        void short_simpl::
        pre (short value)
        {
          value_ = value;
        }

        void short_simpl::
        _serialize_content ()
        {
          // We only need strlen("-32768") + 1 characters to hold all
          // representations of short.
          //
          char str[7];

#ifdef XSDE_SNPRINTF
          int n = snprintf (str, 7, "%d", value_);
#else
          int n = sprintf (str, "%d", value_);
#endif
          if (n > 0 && n < 7)
            _characters (str, static_cast<size_t> (n));
        }
      }
    }
  }
}
