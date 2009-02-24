// file      : xsde/cxx/string-sequence.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // memcpy, strlen, strcmp

#include <xsde/cxx/string-sequence.hxx>

namespace xsde
{
  namespace cxx
  {
    void string_sequence::
    clear ()
    {
      for (size_t i = 0;  i < size_; ++i)
        delete[] static_cast<char**> (data_)[i];

      size_ = 0;
    }

#ifdef XSDE_EXCEPTIONS
    void string_sequence::
    push_back_copy (const char* cs)
    {
      if (capacity_ < size_ + 1)
        grow_ (0, sizeof (char*), 0);

      size_t n = strlen (cs) + 1;
      char* s = new char[n];
      memcpy (s, cs, n);

      static_cast<char**> (data_)[size_++] = s;
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
        size_t n = strlen (cs) + 1;
        char* s = new char[n];

        if (s != 0)
        {
          memcpy (s, cs, n);
          static_cast<char**> (data_)[size_++] = s;
        }
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
