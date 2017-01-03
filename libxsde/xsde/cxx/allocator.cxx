// file      : xsde/cxx/allocator.cxx
// copyright : Copyright (c) 2005-2017 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/config.hxx>
#include <xsde/cxx/allocator.hxx>

namespace xsde
{
  namespace cxx
  {
#ifdef XSDE_EXCEPTIONS
    void*
    alloc (size_t n)
    {
      void* p = xsde_alloc (n);

      if (p == 0)
        throw std::bad_alloc ();

      return p;
    }
#endif
  }
}
