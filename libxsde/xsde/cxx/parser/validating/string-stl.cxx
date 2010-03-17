// file      : xsde/cxx/parser/validating/string-stl.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/parser/validating/string-stl.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        void string_pimpl::
        _pre ()
        {
          str_.erase ();
        }

        void string_pimpl::
        _characters (const ro_string& s)
        {
          str_ += s;
        }

        void string_pimpl::
        _post ()
        {
          // Check facets.
          //
          const facets& f = _facets ();

          if (f.length_set_ && str_.size () != f.length_)
          {
            _schema_error (schema_error::length_not_equal_prescribed);
            return;
          }

          if (f.min_length_set_ && str_.size () < f.min_length_)
          {
            _schema_error (schema_error::length_less_than_min);
            return;
          }

          if (f.max_length_set_ && str_.size () > f.max_length_)
          {
            _schema_error (schema_error::length_greater_than_max);
            return;
          }
        }

        std::string string_pimpl::
        post_string ()
        {
          std::string r;
          r.swap (str_);
          return r;
        }
      }
    }
  }
}
