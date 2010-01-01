// file      : xsde/cxx/serializer/non-validating/idref.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/non-validating/idref.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        idref_simpl::
        ~idref_simpl ()
        {
          if (free_)
            delete[] const_cast<char*> (value_);
        }

        void idref_simpl::
        pre (const char* value)
        {
          value_ = value;
        }

        void idref_simpl::
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
