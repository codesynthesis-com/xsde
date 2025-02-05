// file      : cxx/parser/validation/built-in/qname/driver.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

// Test the built-in QName type validation.
//

#include <xsde/config.h>

// Let the runtime header sort out which version (stl/no-stl) to
// include.
//
#include <xsde/cxx/parser/validating/xml-schema-pimpl.hxx>

#undef NDEBUG
#include <cassert>

using namespace xsde::cxx;
using namespace xsde::cxx::parser;
using namespace xsde::cxx::parser::validating;

#ifdef XSDE_STL
bool
compare (const qname& x, const char* p, const char* n)
{
  return x == qname (p, n);
}
#else
bool
compare (qname* x, const char* p, const char* n)
{
  qname y;
  y.prefix_copy (p);
  y.name_copy (n);

  bool r = *x == y;

#ifndef XSDE_CUSTOM_ALLOCATOR
  delete x;
#else
  x->~qname ();
  xsde::cxx::free (x);
#endif

  return r;
}
#endif

int
main ()
{
  // Good.
  //
  {
    context c;
    qname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" xsi");
    p._characters (":");
    p._characters ("schemaLocation");
    p._post ();
    assert (!c.error_type () &&
            compare (p.post_qname (), "xsi", "schemaLocation"));
  }

  {
    context c;
    qname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("schemaLocation");
    p._post ();
    assert (!c.error_type () &&
            compare (p.post_qname (), "", "schemaLocation"));
  }


  // Bad
  //
  {
    context c;
    qname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    //p._characters ("");
    p._post ();
    assert (c.schema_error () == schema_error::invalid_qname_value);
  }

  {
    context c;
    qname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (":");
    p._post ();
    assert (c.schema_error () == schema_error::invalid_qname_value);
  }

  {
    context c;
    qname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("xsi:");
    p._post ();
    assert (c.schema_error () == schema_error::invalid_qname_value);
  }

  {
    context c;
    qname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (":schemaLocation");
    p._post ();
    assert (c.schema_error () == schema_error::invalid_qname_value);
  }

  {
    context c;
    qname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("x?i:schemaLocation");
    p._post ();
    assert (c.schema_error () == schema_error::invalid_qname_value);
  }

  {
    context c;
    qname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("xsi:schema Location");
    p._post ();
    assert (c.schema_error () == schema_error::invalid_qname_value);
  }

  return 0;
}
