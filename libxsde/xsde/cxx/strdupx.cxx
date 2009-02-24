// file      : xsde/cxx/strdupx.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // strlen, memcpy

#include <xsde/cxx/config.hxx>
#include <xsde/cxx/strdupx.hxx>

namespace xsde
{
  namespace cxx
  {
    char*
    strdupx (const char* s)
    {
      size_t n = strlen (s);
      char* r = new char[n + 1];

#ifndef XSDE_EXCEPTIONS
      if (r)
#endif
        memcpy (r, s, n + 1);

      return r;
    }

    char*
    strndupx (const char* s, size_t n)
    {
      char* r = new char[n + 1];

#ifndef XSDE_EXCEPTIONS
      if (r)
      {
#endif
        memcpy (r, s, n);
        r[n] = '\0';

#ifndef XSDE_EXCEPTIONS
      }
#endif

      return r;
    }
  }
}
