// file      : xsde/cxx/hybrid/parser-map.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // strcmp

#include <xsde/cxx/hybrid/parser-map.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
      parser::parser_base* parser_map_impl::
      find (const char* tid) const
      {
        // Binary search.
        //
        if (size_ == 0)
          return 0;

        int r = 1;
        size_t m;
        size_t l = 0;
        size_t h = size_ - 1;

        while (l <= h)
        {
          m = l + (h - l)/2;
          r = strcmp (entries_[m].type_id, tid);

          if (r == 0 || l == h)
            break;

          if (r < 0)
            l = m + 1;
          else
            h = (m == 0 ? 0 : m - 1);
        }

        return r == 0 ? entries_[m].parser : 0;
      }

      void parser_map_impl::
      reset () const
      {
        if (resetting_)
          return;

        bool& r = const_cast<bool&> (resetting_);
        r = true;

        for (const entry* p = entries_; p != entries_ + size_; ++p)
          p->parser->_reset ();

        r = false;
      }
    }
  }
}
