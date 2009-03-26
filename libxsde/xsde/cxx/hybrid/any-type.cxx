// file      : xsde/cxx/hybrid/any-type.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/hybrid/any-type.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
      // any_type
      //
#ifdef XSDE_POLYMORPHIC
      any_type::
      ~any_type ()
      {
      }

#ifdef XSDE_STL
      const std::string& any_type::
      _dynamic_type () const
      {
        return _static_type ();
      }

      static const std::string any_type_static_type_ =
        "anyType http://www.w3.org/2001/XMLSchema";

      const std::string& any_type::
      _static_type ()
      {
        return any_type_static_type_;
      }
#else
      const char* any_type::
      _dynamic_type () const
      {
        return _static_type ();
      }

      const char* any_type::
      _static_type ()
      {
        return "anyType http://www.w3.org/2001/XMLSchema";
      }
#endif
#endif
    }
  }
}
