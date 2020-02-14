// file      : xsde/cxx/hybrid/cdr/exceptions.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/hybrid/cdr/exceptions.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
      const char* cdr_exception::
      what () const throw ()
      {
        return "CDR stream operation failed";
      }
    }
  }
}
