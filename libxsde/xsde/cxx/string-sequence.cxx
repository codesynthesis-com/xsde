// file      : xsde/cxx/string-sequence.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // strcmp

#include <xsde/cxx/strdupx.hxx>
#include <xsde/cxx/string-sequence.hxx>

namespace xsde
{
  namespace cxx
  {
    void string_sequence::
    clear ()
    {
      for (size_t i = 0;  i < size_; ++i)
#ifndef XSDE_CUSTOM_ALLOCATOR
        delete[] static_cast<char**> (data_)[i];
#else
        cxx::free (static_cast<char**> (data_)[i]);
#endif

      size_ = 0;
    }

#ifdef XSDE_EXCEPTIONS
    void string_sequence::
    push_back_copy (const char* cs)
    {
      if (capacity_ < size_ + 1)
        grow_ (0, sizeof (char*), 0);

      static_cast<char**> (data_)[size_++] = strdupx (cs);
    }
#else
    string_sequence::error string_sequence::
    push_back_copy (const char* cs)
    {
      error r = error_none;

      if (capacity_ < size_ + 1)
        r = grow_ (0, sizeof (char*), 0);

      if (r == error_none)
      {
        char* s = strdupx (cs);

        if (s != 0)
          static_cast<char**> (data_)[size_++] = s;
        else
          r = error_no_memory;
      }

      return r;
    }
#endif

    bool
    operator== (const string_sequence& x, const string_sequence& y)
    {
      if (x.size () != y.size ())
        return false;

      for (size_t i = 0; i < x.size (); ++i)
        if (strcmp (x[i], y[i]) != 0)
          return false;

      return true;
    }
  }
}
