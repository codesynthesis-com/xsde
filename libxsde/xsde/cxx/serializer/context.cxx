// file      : xsde/cxx/serializer/context.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/config.hxx>
#include <xsde/cxx/ro-string.hxx>

#ifdef XSDE_EXCEPTIONS
#  include <new> // std::bad_alloc
#  include <xsde/cxx/serializer/exceptions.hxx>
#endif

#include <xsde/cxx/serializer/context.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
#ifdef XSDE_POLYMORPHIC
#ifdef XSDE_EXCEPTIONS
      void context::
#else
      bool context::
#endif
      set_type (const char* type)
      {
        ro_string t (type);
        const char* prefix = 0;

        size_t pos = t.find (' ');

        if (pos != ro_string::npos)
        {
          prefix = lookup_namespace_prefix (type + pos + 1);

          //@@ TODO: what if prefix is not found?
          //
#ifndef XSDE_EXCEPTIONS
          if (prefix == 0)
            return false;
#endif
          if (*prefix == '\0')
            prefix = 0;
        }

#ifdef XSDE_EXCEPTIONS
        start_attribute ("http://www.w3.org/2001/XMLSchema-instance", "type");

        if (prefix)
        {
          characters (prefix);
          characters (":");
        }

        characters (type, pos != ro_string::npos ? pos : t.size ());
        end_attribute ();
#else
        if (!start_attribute (
              "http://www.w3.org/2001/XMLSchema-instance", "type"))
          return false;

        if (prefix)
        {
          if (!characters (prefix) || !characters (":"))
            return false;
        }

        if (!characters (type, pos != ro_string::npos ? pos : t.size ()) ||
            !end_attribute ())
          return false;

        return true;
#endif
      }
#endif // XSDE_POLYMORPHIC

#ifdef XSDE_EXCEPTIONS
      void context::
      throw_xml_error (genx::xml_error e)
      {
        switch (e)
        {
        case GENX_ALLOC_FAILED:
          {
            throw std::bad_alloc ();
          }
        case GENX_IO_ERROR:
          {
            // This should never happen with consistent exception
            // handling usage since the write/flush functions
            // throw exceptions to indicate write failures.
            //
            throw xml (e);
          }
        default:
          {
            throw xml (e);
          }
        }
      }
#endif
    }
  }
}
