// file      : xsde/cxx/stack.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // memcpy

#include <xsde/cxx/stack.hxx>

namespace xsde
{
  namespace cxx
  {
#ifdef XSDE_EXCEPTIONS
    void stack::
#else
    stack::error stack::
#endif
    grow ()
    {
      size_t c = capacity_ ? capacity_ * 2 : 8;
      char* d = new char[c * el_size_];

#ifndef XSDE_EXCEPTIONS
      if (d == 0)
        return error_no_memory;
#endif

      if (size_ > 1)
        memcpy (d, data_, (size_ - 1) * el_size_);

      delete[] data_;

      data_ = d;
      capacity_ = c;

#ifndef XSDE_EXCEPTIONS
      return error_none;
#endif
    }
  }
}