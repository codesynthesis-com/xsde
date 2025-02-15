// file      : xsde/cxx/serializer/validating/id.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // strlen

#include <xsde/cxx/xml/ncname.hxx>

#include <xsde/cxx/serializer/validating/id.hxx>
#include <xsde/cxx/serializer/validating/string-common.hxx>

#ifdef XSDE_CUSTOM_ALLOCATOR
#  include <xsde/cxx/allocator.hxx>
#endif

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
          if (free_ && value_)
          {
            char* v = const_cast<char*> (value_);
#ifndef XSDE_CUSTOM_ALLOCATOR
            delete[] v;
#else
            cxx::free (v);
#endif
          }
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
          {
            if (string_common::validate_facets (
                  value_, n, _facets (), _context ()))
            {
              _characters (value_, n);
            }
          }
          else
            _schema_error (schema_error::invalid_id_value);

          if (free_)
          {
            char* v = const_cast<char*> (value_);
#ifndef XSDE_CUSTOM_ALLOCATOR
            delete[] v;
#else
            cxx::free (v);
#endif
            value_ = 0;
          }
        }
      }
    }
  }
}
