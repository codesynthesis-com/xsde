// file      : xsde/cxx/serializer/validating/decimal.ixx
// copyright : Copyright (c) 2005-2014 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
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
