// file      : xsde/options-parser.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_OPTIONS_PARSER_HXX
#define XSDE_OPTIONS_PARSER_HXX

#include <xsde/types.hxx>
#include <xsde/options.hxx>

namespace cli
{
  template <>
  struct parser<NarrowString>
  {
    static void
    parse (NarrowString& x, scanner& s)
    {
      const char* o (s.next ());

      if (s.more ())
        x = s.next ();
      else
        throw missing_value (o);
    }
  };
}

#endif // XSDE_OPTIONS_PARSER_HXX
