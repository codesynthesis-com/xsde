// file      : xsde/cxx/serializer/validating/id.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // strlen

#include <xsde/cxx/xml/ncname.hxx>

#include <xsde/cxx/serializer/validating/id.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        id_simpl::
        ~id_simpl ()
        {
          if (free_)
            delete[] const_cast<char*> (value_);
        }

        void id_simpl::
        pre (const char* value)
        {
          value_ = value;
        }

        void id_simpl::
        _serialize_content ()
        {
          size_t n = strlen (value_);

          if (xml::valid_ncname (value_, n))
            _characters (value_, n);
          else
            _schema_error (schema_error::invalid_id_value);

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
