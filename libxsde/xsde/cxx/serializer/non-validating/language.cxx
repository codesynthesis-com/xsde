// file      : xsde/cxx/serializer/non-validating/language.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/non-validating/language.hxx>

#ifdef XSDE_CUSTOM_ALLOCATOR
#  include <xsde/cxx/allocator.hxx>
#endif

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
