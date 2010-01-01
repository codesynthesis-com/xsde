// file      : xsde/cxx/serializer/non-validating/ncname.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/non-validating/ncname.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        ncname_simpl::
        ~ncname_simpl ()
        {
          if (free_)
            delete[] const_cast<char*> (value_);
        }

        void ncname_simpl::
        pre (const char* value)
        {
          value_ = value;
        }

        void ncname_simpl::
        _serialize_content ()
        {
          _characters (value_);

          if (free_)
          {
            delete[] const_cast<char*> (value_);
            value_ = 0;
          }
        }
      }
    }
  }
}
