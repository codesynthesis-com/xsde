// file      : xsde/cxx/hybrid/xdr/exceptions.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/hybrid/xdr/exceptions.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
      const char* xdr_exception::
      what () const throw ()
      {
        return "XDR stream operation failed";
      }
    }
  }
}
