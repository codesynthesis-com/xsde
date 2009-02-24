// file      : xsde/cxx/parser/validating/inheritance-map.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/config.hxx>

#include <string.h> // strcmp

#ifndef XSDE_EXCEPTIONS
#  include <assert.h> // assert
#  include <stdlib.h> // exit
#endif

#include <xsde/cxx/parser/validating/inheritance-map.hxx>
#include <xsde/cxx/parser/validating/inheritance-map-load.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        inheritance_map* inheritance_map_init::map = 0;
        size_t inheritance_map_init::count = 0;

        bool inheritance_map::
        check (const char* derived, const char* base) const
        {
          if (strcmp (derived, base) == 0)
            return true;

          const void* p = find (derived);

          if (p)
          {
            const char* b = *static_cast<const char* const*> (p);
            return strcmp (base, b) == 0 ? true : check (b, base);
          }

          return false;
        }

        // inheritance_map_init
        //
        inheritance_map_init::
        inheritance_map_init ()
        {
          if (count == 0)
          {
            map = new inheritance_map (XSDE_PARSER_IMAP_BUCKETS);

#ifndef XSDE_EXCEPTIONS
            if (map == 0 || map->_error () != inheritance_map::error_none)
            {
              // This is static initialization so there is nothing we can do.
              // The best thing is to fail fast. abort() would have probably
              // been the better choice here but it is not available on some
              // platforms (notably, WinCE).
              //
              assert (false);
              exit (1);
            }
#endif
          }

          ++count;
        }

        inheritance_map_init::
        ~inheritance_map_init ()
        {
          if (--count == 0)
            delete map;
        }

        // inheritance_map_entry
        //
        inheritance_map_entry::
        inheritance_map_entry (const char* derived, const char* base)
        {
          inheritance_map& m = inheritance_map_instance ();
          m.insert (derived, base);

#ifndef XSDE_EXCEPTIONS
          if (m._error () != inheritance_map::error_none)
          {
            // This is static initialization so there is nothing we can do.
            // The best thing is to fail fast. abort() would have probably
            // been the better choice here but it is not available on some
            // platforms (notably, WinCE).
            //
            assert (false);
            exit (1);
          }
#endif
        }

        //
        //
        size_t
        parser_imap_elements ()
        {
          return inheritance_map_instance ().size ();
        }
      }
    }
  }
}
