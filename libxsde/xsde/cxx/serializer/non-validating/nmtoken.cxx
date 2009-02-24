// file      : xsde/cxx/serializer/non-validating/nmtoken.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/non-validating/nmtoken.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        nmtoken_simpl::
        ~nmtoken_simpl ()
        {
          if (free_)
            delete[] const_cast<char*> (value_);
        }

        void nmtoken_simpl::
        pre (const char* value)
        {
          value_ = value;
        }

        void nmtoken_simpl::
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
