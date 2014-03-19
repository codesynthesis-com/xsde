// file      : xsde/cxx/serializer/validating/float.ixx
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
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
