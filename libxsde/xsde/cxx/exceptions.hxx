// file      : xsde/cxx/exceptions.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_EXCEPTIONS_HXX
#define XSDE_CXX_EXCEPTIONS_HXX

#include <exception> // std::exception

namespace xsde
{
  namespace cxx
  {
    struct exception: std::exception
    {
    };
  }
}

#endif  // XSDE_CXX_EXCEPTIONS_HXX
