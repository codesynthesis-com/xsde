// file      : xsde/cxx/serializer/non-validating/double.ixx
// copyright : Copyright (c) 2005-2017 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
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
