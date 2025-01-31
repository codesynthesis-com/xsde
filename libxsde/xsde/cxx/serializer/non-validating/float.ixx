// file      : xsde/cxx/serializer/non-validating/float.ixx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        inline void float_simpl::
        format (notation n, unsigned int p)
        {
          notation_ = n;
          precision_ = p;
        }

        inline float_simpl::
        float_simpl (notation n, unsigned int p)
            : notation_ (n), precision_ (p)
        {
        }
      }
    }
  }
}
