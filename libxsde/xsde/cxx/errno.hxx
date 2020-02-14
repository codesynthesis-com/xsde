// file      : xsde/cxx/errno.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_ERRNO_HXX
#define XSDE_CXX_ERRNO_HXX

namespace xsde
{
  namespace cxx
  {
    int
    get_errno ();

    void
    set_errno (int);
  }
}

#include <xsde/cxx/errno.ixx>

#endif // XSDE_CXX_ERRNO_HXX
