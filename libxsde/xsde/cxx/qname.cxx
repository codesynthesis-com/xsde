// file      : xsde/cxx/qname.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // strlen, memcpy
#include <stddef.h> // size_t

#include <xsde/cxx/qname.hxx>

namespace xsde
{
  namespace cxx
  {
#ifndef XSDE_EXCEPTIONS
    qname::error qname::
#else
    void qname::
#endif
    prefix_copy (const char* prefix)
    {
      delete[] prefix_;

      if (prefix)
      {
        size_t size = strlen (prefix) + 1;
        prefix_ = new char[size];

#ifndef XSDE_EXCEPTIONS
        if (prefix_ == 0)
          return error_no_memory;
#endif
        memcpy (prefix_, prefix, size);
      }
      else
        prefix_ = 0;

#ifndef XSDE_EXCEPTIONS
      return error_none;
#endif
    }

#ifndef XSDE_EXCEPTIONS
    qname::error qname::
#else
    void qname::
#endif
    name_copy (const char* name)
    {
      delete[] name_;

      if (name)
      {
        size_t size = strlen (name) + 1;
        name_ = new char[size];

#ifndef XSDE_EXCEPTIONS
        if (name_ == 0)
          return error_no_memory;
#endif
        memcpy (name_, name, size);
      }
      else
        name_ = 0;

#ifndef XSDE_EXCEPTIONS
      return error_none;
#endif
    }
  }
}
