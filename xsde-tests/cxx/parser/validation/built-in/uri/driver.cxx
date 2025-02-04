// file      : cxx/parser/validation/built-in/uri/driver.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

// Test the built-in anyURI type validation.
//
#include <string>

#include <xsde/config.h>

#ifdef XSDE_STL
#  include <xsde/cxx/string.hxx>
#endif

// Let the runtime header sort out which version (stl/no-stl) to
// include.
//
#include <xsde/cxx/parser/validating/xml-schema-pimpl.hxx>

#undef NDEBUG
#include <cassert>

using namespace xsde::cxx::parser;
using namespace xsde::cxx::parser::validating;

struct string_wrapper
{
#ifdef XSDE_STL
  std::string value;
    string_wrapper (const std::string& s): value (s) {}
#else
  xsde::cxx::string value;
  string_wrapper (char* s) {value.attach (s);}
#endif
};

int
main ()
{
  using std::string;

  // Good.
  //
  {
    context c;
    uri_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("  ");
    p._post ();
    assert (!c.error_type () && string_wrapper (p.post_uri ()).value == "");
  }

  {
    context c;
    uri_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("relative");
    p._post ();
    assert (!c.error_type () &&
            string_wrapper (p.post_uri ()).value == "relative");
  }

  {
    context c;
    uri_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("#id");
    p._post ();
    assert (!c.error_type () && string_wrapper (p.post_uri ()).value == "#id");
  }

  {
    context c;
    uri_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("http://www.example.com/foo#bar");
    p._post ();
    assert (!c.error_type () &&
            string_wrapper (p.post_uri ()).value ==
            "http://www.example.com/foo#bar");
  }

  return 0;
}
