// file      : xsde/cxx/serializer/non-validating/decimal.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
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
