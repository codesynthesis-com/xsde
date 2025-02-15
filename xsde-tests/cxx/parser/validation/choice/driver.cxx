// file      : cxx/parser/validation/choice/driver.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

// Test the choice compositor validation.
//

#include <string>
#include <iostream>

#include "test-pskel.hxx"

#undef NDEBUG
#include <cassert>

using namespace std;
using namespace test;
using xml_schema::ro_string;

struct choice_pimpl: choice_pskel
{
  virtual void
  pre ()
  {
    cout << "{" << endl;
  }

#ifdef XSDE_STL
  virtual void
  a (string const& v)
  {
    cout << "  a = " << v << endl;
  }

  virtual void
  b (string const& v)
  {
    cout << "  b = " << v << endl;
  }

  virtual void
  c (string const& v)
  {
    cout << "  c = " << v << endl;
  }

  virtual void
  d (string const& v)
  {
    cout << "  d = " << v << endl;
  }
#else
  virtual void
  a (char* v)
  {
    cout << "  a = " << v << endl;

#ifndef XSDE_CUSTOM_ALLOCATOR
    delete[] v;
#else
    xsde::cxx::free (v);
#endif
  }

  virtual void
  b (char* v)
  {
    cout << "  b = " << v << endl;

#ifndef XSDE_CUSTOM_ALLOCATOR
    delete[] v;
#else
    xsde::cxx::free (v);
#endif
  }

  virtual void
  c (char* v)
  {
    cout << "  c = " << v << endl;

#ifndef XSDE_CUSTOM_ALLOCATOR
    delete[] v;
#else
    xsde::cxx::free (v);
#endif
  }

  virtual void
  d (char* v)
  {
    cout << "  d = " << v << endl;

#ifndef XSDE_CUSTOM_ALLOCATOR
    delete[] v;
#else
    xsde::cxx::free (v);
#endif
  }
#endif

  virtual void
#ifndef XSDE_POLYMORPHIC
  _start_any_element (ro_string const& ns, ro_string const& name)
#else
  _start_any_element (ro_string const& ns, ro_string const& name, const char*)
#endif
  {
    cout << "  any: " << ns << "#" << name << endl
         << "  {" << endl;
  }

  virtual void
  _any_characters (ro_string const& v)
  {
    cout << "    chars = " << v << endl;
  }

  virtual void
  _end_any_element (ro_string const&, ro_string const&)
  {
    cout << "  }" << endl;
  }

  virtual void
  post_choice ()
  {
    cout << "}" << endl
         << endl;
  }
};

struct type_pimpl: type_pskel
{
};

int
main (int argc, char* argv[])
{
  if (argc != 2)
  {
    cerr << "usage: " << argv[0] << " test.xml" << endl;
    return 1;
  }

  try
  {
    xml_schema::string_pimpl string_p;
    choice_pimpl choice_p;
    type_pimpl type_p;

    choice_p.parsers (string_p, string_p, string_p, string_p);
    type_p.parsers (choice_p);

    xml_schema::document_pimpl doc_p (type_p, "test", "root");

    try
    {
      type_p.pre ();
      doc_p.parse (argv[1]);
      type_p.post_type ();
    }
    catch (xml_schema::parser_exception const& e)
    {
      cout << "  " << e.line () << ":" << e.column () << " " << e << endl
           << "}" << endl
           << endl;
    }
  }
  catch (xml_schema::parser_exception const& e)
  {
    cerr << e << endl;
    return 1;
  }
  catch (ios_base::failure const&)
  {
    cerr << "io failure" << endl;
    return 1;
  }

  return 0;
}
