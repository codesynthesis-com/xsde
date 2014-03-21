// file      : xsde/cxx/serializer/non-validating/base64-binary.ixx
// copyright : Copyright (c) 2005-2014 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        inline base64_binary_simpl::
        base64_binary_simpl (bool free)
            : free_ (free), value_ (0)
        {
        }
      }
    }
  }
}
