// file      : xsde/cxx/strdupx.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_STRDUPX_HXX
#define XSDE_CXX_STRDUPX_HXX

#include <stddef.h> // size_t

namespace xsde
{
  namespace cxx
  {
    // C++ versions of strdup and strndup. They allocate the string
    // using new char[]. In the no-exceptions case return 0 if
    // allocation fails.
    //
    char*
    strdupx (const char*);

    char*
    strndupx (const char*, size_t);
  }
}

#endif  // XSDE_CXX_STRDUPX_HXX
