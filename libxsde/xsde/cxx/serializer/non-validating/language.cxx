// file      : xsde/cxx/serializer/non-validating/language.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/non-validating/language.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
      {
        language_simpl::
        ~language_simpl ()
        {
          if (free_)
            delete[] const_cast<char*> (value_);
        }

        void language_simpl::
        pre (const char* value)
        {
          value_ = value;
        }

        void language_simpl::
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
