// file      : xsde/cxx/hybrid/any-type.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_HYBRID_ANY_TYPE_HXX
#define XSDE_CXX_HYBRID_ANY_TYPE_HXX

#include <xsde/cxx/config.hxx>

/*
#ifdef XSDE_STL
#  include <string>
#endif
*/

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
      struct any_type
      {
        /*
#ifdef XSDE_POLYMORPHIC
        virtual
        ~any_type ();

#ifdef XSDE_STL
        virtual const std::string&
        _dynamic_type () const;

        static const std::string&
        _static_type ();
#else
        virtual const char*
        _dynamic_type () const;

        static const char*
        _static_type ();
#endif
#endif
        */
      };

      struct any_simple_type
      {
      };

      inline bool
      operator== (const any_simple_type&, const any_simple_type&)
      {
        return true;
      }

      inline bool
      operator!= (const any_simple_type&, const any_simple_type&)
      {
        return false;
      }
    }
  }
}

#endif  // XSDE_CXX_HYBRID_ANY_TYPE_HXX
