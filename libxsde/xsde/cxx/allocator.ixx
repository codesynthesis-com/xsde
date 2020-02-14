// file      : xsde/cxx/allocator.ixx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/allocator.h>

namespace xsde
{
  namespace cxx
  {
#ifndef XSDE_EXCEPTIONS
    inline void*
    alloc (size_t n)
    {
      return xsde_alloc (n);
    }
#endif

    inline void
    free (void* p)
    {
      if (p)
        xsde_free (p);
    }
  }
}
