// file      : xsde/cxx/sys-error.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/sys-error.hxx>

namespace xsde
{
  namespace cxx
  {
    static const char* const text_[] =
    {
      "no error",
      "no memory",
      "open failed",
      "read failed",
      "write failed"
    };

    const char* sys_error::
    text (value v)
    {
      return text_[v];
    }
  }
}
