// file      : xsde/cxx/serializer/validating/double.ixx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        inline void double_simpl::
        format (notation n, unsigned int p)
        {
          notation_ = n;
          precision_ = p;
        }

        inline double_simpl::
        double_simpl (notation n, unsigned int p)
            : notation_ (n), precision_ (p)
        {
        }
      }
    }
  }
}
