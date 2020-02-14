// file      : xsde/cxx/parser/validating/time-zone.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_VALIDATING_TIME_ZONE_HXX
#define XSDE_CXX_PARSER_VALIDATING_TIME_ZONE_HXX

#include <stddef.h> // size_t

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        namespace bits
        {
          bool
          parse_time_zone (const char* s,
                           size_t size,
                           short& hours,
                           short& minutes);
        }
      }
    }
  }
}

#endif  // XSDE_CXX_PARSER_VALIDATING_TIME_ZONE_HXX
