// file      : xsde/cxx/serializer/non-validating/string.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/non-validating/string.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        string_simpl::
        ~string_simpl ()
        {
          if (free_)
            delete[] const_cast<char*> (value_);
        }

        void string_simpl::
        pre (const char* value)
        {
          value_ = value;
        }

        void string_simpl::
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
