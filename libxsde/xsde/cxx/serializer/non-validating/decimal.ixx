// file      : xsde/cxx/serializer/non-validating/decimal.ixx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        inline void decimal_simpl::
        precision (unsigned int p)
        {
          precision_ = p;
        }

        inline decimal_simpl::
        decimal_simpl (unsigned int p)
            : precision_ (p)
        {
        }
      }
    }
  }
}
