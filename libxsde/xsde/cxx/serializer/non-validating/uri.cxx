// file      : xsde/cxx/serializer/non-validating/uri.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/non-validating/uri.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        uri_simpl::
        ~uri_simpl ()
        {
          if (free_)
            delete[] const_cast<char*> (value_);
        }

        void uri_simpl::
        pre (const char* value)
        {
          value_ = value;
        }

        void uri_simpl::
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
