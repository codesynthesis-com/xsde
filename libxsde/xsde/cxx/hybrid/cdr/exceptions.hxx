// file      : xsde/cxx/hybrid/cdr/exceptions.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_HYBRID_CDR_EXCEPTIONS_HXX
#define XSDE_CXX_HYBRID_CDR_EXCEPTIONS_HXX

#include <xsde/cxx/exceptions.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
      struct cdr_exception: xsde::cxx::exception
      {
        virtual const char*
        what () const throw ();
      };
    }
  }
}

#endif  // XSDE_CXX_HYBRID_CDR_EXCEPTIONS_HXX
