// file      : xsde/cxx/serializer/validating/string.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/validating/string.hxx>
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
        string_simpl::
        ~string_simpl ()
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

        void string_simpl::
        pre (const char* value)
        {
          value_ = value;
        }

        void string_simpl::
        _serialize_content ()
        {
          if (string_common::validate_facets (
                value_, _facets (), _context ()))
          {
            _characters (value_);
          }

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
