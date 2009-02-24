// file      : xsde/cxx/parser/state.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // memcpy

#include <xsde/cxx/parser/state.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      stack::error stack::
      grow ()
      {
        size_t c = capacity_ ? capacity_ * 2 : 8;
        char* d = new char[c * el_size_];

        if (d == 0)
          return error_no_memory;

        if (size_ > 1)
          memcpy (d, data_, (size_ - 1) * el_size_);

        delete[] data_;

        data_ = d;
        capacity_ = c;

        return error_none;
      }
    }
  }
}
