// file      : cxx/parser/basic/driver.cxx
// copyright : GNU GPL v2 + exceptions; see accompanying LICENSE file

#if defined(XSDE_CUSTOM_ALLOCATOR) && !defined(XSDE_DEFAULT_ALLOCATOR)
#  include <stdlib.h>
#endif

#include <ios>
#include <iostream>

#ifndef XSDE_IOSTREAM
#  include <string>
#  include <ostream>
#  include <fstream>
#endif

#include <xsde/cxx/ro-string.hxx>

#include <xsde/cxx/parser/exceptions.hxx>

#ifdef XSDE_PARSER_VALIDATION
#  include <xsde/cxx/parser/validating/parser.hxx>
#  include <xsde/cxx/parser/validating/xml-schema-pskel.hxx>
#  include <xsde/cxx/parser/validating/xml-schema-pimpl.hxx>
#else
#  include <xsde/cxx/parser/non-validating/parser.hxx>
#  include <xsde/cxx/parser/non-validating/xml-schema-pskel.hxx>
#  include <xsde/cxx/parser/non-validating/xml-schema-pimpl.hxx>
#endif

#include <xsde/cxx/parser/expat/document.hxx>

#undef NDEBUG
#include <cassert>

#if defined(XSDE_CUSTOM_ALLOCATOR) && !defined(XSDE_DEFAULT_ALLOCATOR)
extern "C" void*
xsde_alloc (size_t n)
{
  return malloc (n);
}

extern "C" void*
xsde_realloc (void* p, size_t n)
{
  return realloc (p, n);
}

extern "C" void
xsde_free (void* p)
{
  free (p);
}
#endif

using namespace std;
using namespace xsde::cxx;

#ifdef XSDE_PARSER_VALIDATION
using parser::validating::string_pskel;
using parser::validating::string_pimpl;

class hello_pimpl: public parser::validating::complex_content
#else
using parser::non_validating::string_pskel;
using parser::non_validating::string_pimpl;

class hello_pimpl: public parser::non_validating::complex_content
#endif
{
public:
  hello_pimpl (string_pskel& greeting_parser, string_pskel& name_parser)
      : greeting_parser_ (greeting_parser),
        name_parser_ (name_parser)
  {
  }

private:

#ifndef XSDE_POLYMORPHIC
  virtual bool
  _start_element_impl (const ro_string&, const ro_string&) override;
#else
  virtual bool
  _start_element_impl (const ro_string&,
                       const ro_string&,
                       const char*) override;
#endif

  virtual bool
  _end_element_impl (const ro_string&, const ro_string&) override;

private:
  string_pskel& greeting_parser_;
  string_pskel& name_parser_;
};

#ifndef XSDE_IOSTREAM
static void
write (ostream& o, const ro_string& s)
{
  if (!s.empty ())
    o.write (s.data (), s.size ());
}
#endif

#ifndef XSDE_POLYMORPHIC

bool hello_pimpl::
_start_element_impl (const ro_string& ns, const ro_string& n)
{
  if (complex_content::_start_element_impl (ns, n))
    return true;

#else

bool hello_pimpl::
_start_element_impl (const ro_string& ns, const ro_string& n, const char* t)
{
  if (complex_content::_start_element_impl (ns, n, t))
    return true;

#endif

  if (n == "greeting" && ns.empty ())
  {
    greeting_parser_.pre ();
    _context ().nested_parser (&greeting_parser_);
    return true;
  }

  if (n == "name" && ns.empty ())
  {
    name_parser_.pre ();
    _context ().nested_parser (&name_parser_);
    return true;
  }

  return false;
}

bool hello_pimpl::
_end_element_impl (const ro_string& ns, const ro_string& n)
{
  if (complex_content::_end_element_impl (ns, n))
    return true;

  if (n == "greeting" && ns.empty ())
  {
#ifdef XSDE_IOSTREAM
    cout << n << ' ' << greeting_parser_.post_string () << endl;
#else
    write (cout, n);
    cout << ' ';
    write (cout, greeting_parser_.post_string ());
    cout << endl;
#endif
    return true;
  }

  if (n == "name" && ns.empty ())
  {
#ifdef XSDE_IOSTREAM
    cout << n << ' ' << name_parser_.post_string () << endl;
#else
    write (cout, n);
    cout << ' ';
    write (cout, name_parser_.post_string ());
    cout << endl;
#endif
    return true;
  }

  return false;
}

// Usage: argv[0] <xml-file>
//
// Parse the specified XML file and print the element names and values to
// stdout.
//
int
main (int argc, char* argv[])
{
  assert (argc == 2);

  const char* input (argv[1]);

  try
  {
    string_pimpl string_p;
    hello_pimpl hello_p (string_p, string_p);

    parser::expat::document_pimpl doc_p (hello_p, "hello");

#ifdef XSDE_IOSTREAM
    doc_p.parse (input);
#else
    ifstream is;
    is.exceptions (ifstream::badbit | ifstream::failbit);
    is.open (input);

    std::string s;
    getline (is, s, '\0');

    doc_p.parse (s.c_str (), s.size (), true /* last */);
#endif
  }
  catch (const parser::exception& e)
  {
    cerr << input << ":" << e.line () << ":" << e.column () << ": "
         << e.text () << endl;
    return 1;
  }
  catch (const ios_base::failure&)
  {
    cerr << input << ": unable to open or read failure" << endl;
    return 1;
  }

  return 0;
}
