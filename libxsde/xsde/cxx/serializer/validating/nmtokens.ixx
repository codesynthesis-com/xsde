// file      : xsde/cxx/serializer/validating/nmtokens.ixx
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
        inline nmtokens_simpl::
        nmtokens_simpl (bool free)
            : free_ (free), value_ (0), serializer_ (false)
        {
        }
      }
    }
  }
}
