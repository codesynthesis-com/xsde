// file      : xsde/cxx/hybrid/xdr/exceptions.hxx
// copyright : Copyright (c) 2005-2014 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_HYBRID_XDR_EXCEPTIONS_HXX
#define XSDE_CXX_HYBRID_XDR_EXCEPTIONS_HXX

#include <xsde/cxx/exceptions.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
      struct xdr_exception: xsde::cxx::exception
      {
        virtual const char*
        what () const throw ();
      };
    }
  }
}

#endif  // XSDE_CXX_HYBRID_XDR_EXCEPTIONS_HXX
