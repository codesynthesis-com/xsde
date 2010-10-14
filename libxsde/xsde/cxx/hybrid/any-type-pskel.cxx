// file      : xsde/cxx/hybrid/any-type-pskel.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/hybrid/any-type-pskel.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
#ifdef XSDE_POLYMORPHIC
      bool any_type_pskel::
      _start_element_impl (const ro_string& ns,
                           const ro_string& name,
                           const char* type)
      {
#ifdef XSDE_PARSER_VALIDATION
        parser::context& ctx = _context ();
        ctx.current_.any_ = true;
        ctx.current_.depth_++;

        _start_any_element (ns, name, type);
        return true;
#else
        return false;
#endif
      }
#else
      bool any_type_pskel::
      _start_element_impl (const ro_string& ns, const ro_string& name)
      {
#ifdef XSDE_PARSER_VALIDATION
        parser::context& ctx = _context ();
        ctx.current_.any_ = true;
        ctx.current_.depth_++;

        _start_any_element (ns, name);
        return true;
#else
        return false;
#endif
      }
#endif

      bool any_type_pskel::
      _end_element_impl (const ro_string& ns, const ro_string& name)
      {
#ifdef XSDE_PARSER_VALIDATION
        _end_any_element (ns, name);
        return true;
#else
        return false;
#endif
      }

#ifdef XSDE_PARSER_VALIDATION
      bool any_type_pskel::
      _attribute_impl_phase_two (const ro_string& ns,
                                 const ro_string& name,
                                 const ro_string& value)
      {
        _any_attribute (ns, name, value);
        return true;
      }
#else
      bool any_type_pskel::
      _attribute_impl (const ro_string&,
                       const ro_string&,
                       const ro_string&)
      {
        return false;
      }
#endif

      bool any_type_pskel::
      _characters_impl (const ro_string& s)
      {
#ifdef XSDE_PARSER_VALIDATION
        _any_characters (s);
        return true;
#else
        return false;
#endif
      }

#ifdef XSDE_POLYMORPHIC
      const char* any_type_pskel::
      _static_type ()
      {
        return "anyType http://www.w3.org/2001/XMLSchema";
      }

      const char* any_type_pskel::
      _dynamic_type () const
      {
        return _static_type ();
      }
#endif
    }
  }
}
