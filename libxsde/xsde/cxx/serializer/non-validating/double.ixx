// file      : xsde/cxx/serializer/non-validating/double.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        inline double_simpl::
        double_simpl (notation n, unsigned int p)
            : notation_ (n), precision_ (p)
        {
        }
      }
    }
  }
}
