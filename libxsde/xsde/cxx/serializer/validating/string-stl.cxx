// file      : xsde/cxx/serializer/validating/string-stl.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/validating/string-stl.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        void string_simpl::
        pre (const std::string& value)
        {
          value_ = value;
        }

        void string_simpl::
        _serialize_content ()
        {
          // Check facets.
          //
          const facets& f = _facets ();

          if (f.length_set_ && value_.size () != f.length_)
          {
            _schema_error (schema_error::length_not_equal_prescribed);
            return;
          }

          if (f.min_length_set_ && value_.size () < f.min_length_)
          {
            _schema_error (schema_error::length_less_than_min);
            return;
          }

          if (f.max_length_set_ && value_.size () > f.max_length_)
          {
            _schema_error (schema_error::length_greater_than_max);
            return;
          }

          // Make sure we don't hold any references to the string.
          //
          std::string tmp;
          tmp.swap (value_);

          _characters (tmp.c_str (), tmp.size ());
        }
      }
    }
  }
}
