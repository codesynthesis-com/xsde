// file      : xsde/cxx/qname.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/strdupx.hxx>
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
#ifndef XSDE_CUSTOM_ALLOCATOR
      delete[] prefix_;
#else
      cxx::free (prefix_);
#endif

      if (prefix)
      {
        prefix_ = strdupx (prefix);

#ifndef XSDE_EXCEPTIONS
        if (prefix_ == 0)
          return error_no_memory;
#endif
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
#ifndef XSDE_CUSTOM_ALLOCATOR
      delete[] name_;
#else
      cxx::free (name_);
#endif

      if (name)
      {
        name_ = strdupx (name);

#ifndef XSDE_EXCEPTIONS
        if (name_ == 0)
          return error_no_memory;
#endif
      }
      else
        name_ = 0;

#ifndef XSDE_EXCEPTIONS
      return error_none;
#endif
    }
  }
}
