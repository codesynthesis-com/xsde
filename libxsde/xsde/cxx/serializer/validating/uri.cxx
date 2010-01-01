// file      : xsde/cxx/serializer/validating/uri.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/validating/uri.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
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
          // According to Datatypes 3.2.17 and RFC2396 pretty much anything
          // can be a URI and conforming processors do not need to figure
          // out and verify particular URI schemes.
          //
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
