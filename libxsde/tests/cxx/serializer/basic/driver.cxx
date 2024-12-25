// file      : cxx/serializer/basic/driver.cxx
// copyright : GNU GPL v2 + exceptions; see accompanying LICENSE file

#if defined(XSDE_CUSTOM_ALLOCATOR) && !defined(XSDE_DEFAULT_ALLOCATOR)
#  include <stdlib.h>
#endif

#include <ios>
#include <string>
#include <vector>
#include <iostream>

#include <xsde/cxx/serializer/exceptions.hxx>

#ifdef XSDE_SERIALIZER_VALIDATION
#  include <xsde/cxx/serializer/validating/serializer.hxx>
#  include <xsde/cxx/serializer/validating/xml-schema-sskel.hxx>
#  include <xsde/cxx/serializer/validating/xml-schema-simpl.hxx>
#else
#  include <xsde/cxx/serializer/non-validating/serializer.hxx>
#  include <xsde/cxx/serializer/non-validating/xml-schema-sskel.hxx>
#  include <xsde/cxx/serializer/non-validating/xml-schema-simpl.hxx>
#endif

#include <xsde/cxx/serializer/genx/document.hxx>

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

#ifdef XSDE_SERIALIZER_VALIDATION
using serializer::validating::string_sskel;
using serializer::validating::string_simpl;

class hello_simpl: public serializer::validating::complex_content
#else
using serializer::non_validating::string_sskel;
using serializer::non_validating::string_simpl;

class hello_simpl: public serializer::non_validating::complex_content
#endif
{
public:
  hello_simpl (string_sskel& greeting_serializer,
               string_sskel& name_serializer)
      : greeting_serializer_ (greeting_serializer),
        name_serializer_ (name_serializer)
  {
    names_.push_back ("sun");
    names_.push_back ("moon");
    names_.push_back ("world");
  }

  void
  pre ()
  {
    i_ = names_.begin ();
  }

  std::string
  greeting ()
  {
    return "Hello";
  }

  bool
  name_next ()
  {
    return i_ != names_.end ();
  }

  std::string
  name ()
  {
    return *i_++;
  }

private:
  virtual void
  _serialize_content () override;

private:
  typedef vector<std::string> names;

  names names_;
  names::iterator i_;

  string_sskel& greeting_serializer_;
  string_sskel& name_serializer_;
};

void hello_simpl::
_serialize_content ()
{
  serializer::context& ctx (_context ());

  // greeting
  //
  {
    const std::string& r (greeting ());

#ifdef XSDE_STL
    greeting_serializer_.pre (r);
#else
    greeting_serializer_.pre (r.c_str ());
#endif

    _start_element ("greeting");
    greeting_serializer_._pre_impl (ctx);

#if defined(XSDE_SERIALIZER_VALIDATION) || !defined(XSDE_EXCEPTIONS)
    if (ctx.error_type ())
      return;
#endif

    greeting_serializer_._serialize_attributes ();

#if defined(XSDE_SERIALIZER_VALIDATION) || !defined(XSDE_EXCEPTIONS)
    if (ctx.error_type ())
      return;
#endif

    greeting_serializer_._serialize_content ();

#if defined(XSDE_SERIALIZER_VALIDATION) || !defined(XSDE_EXCEPTIONS)
    if (ctx.error_type ())
      return;
#endif

    greeting_serializer_._post_impl ();

#if defined(XSDE_SERIALIZER_VALIDATION) || !defined(XSDE_EXCEPTIONS)
    if (ctx.error_type ())
      return;
#endif

    _end_element ();
    greeting_serializer_.post ();
  }

  // name
  //
  {
    while (name_next ())
    {
      const std::string& r (name ());

#ifdef XSDE_STL
      name_serializer_.pre (r);
#else
      name_serializer_.pre (r.c_str ());
#endif

      _start_element ("name");
      name_serializer_._pre_impl (ctx);

#if defined(XSDE_SERIALIZER_VALIDATION) || !defined(XSDE_EXCEPTIONS)
      if (ctx.error_type ())
        return;
#endif

      name_serializer_._serialize_attributes ();

#if defined(XSDE_SERIALIZER_VALIDATION) || !defined(XSDE_EXCEPTIONS)
      if (ctx.error_type ())
        return;
#endif

      name_serializer_._serialize_content ();

#if defined(XSDE_SERIALIZER_VALIDATION) || !defined(XSDE_EXCEPTIONS)
      if (ctx.error_type ())
        return;
#endif

      name_serializer_._post_impl ();

#if defined(XSDE_SERIALIZER_VALIDATION) || !defined(XSDE_EXCEPTIONS)
      if (ctx.error_type ())
        return;
#endif

      _end_element ();
      name_serializer_.post ();
    }
  }
}

struct cout_writer: public serializer::genx::writer
{
#ifdef XSDE_EXCEPTIONS
  virtual void
  write (const char* s) override
  {
    cout << s;
  }

  virtual void
  write (const char* s, size_t n) override
  {
    if (n != 0)
      cout.write (s, n);
  }

  virtual void
  flush () override
  {
    cout.flush ();
  }
#else
  virtual bool
  write (const char* s) override
  {
    cout << s;
    return true;
  }

  virtual bool
  write (const char* s, size_t n) override
  {
    if (n != 0)
      cout.write (s, n);
    return true;
  }

  virtual bool
  flush () override
  {
    cout.flush ();
    return true;
  }
#endif
};

int
main ()
{
  try
  {
    string_simpl string_s;
    hello_simpl hello_s (string_s, string_s);

    serializer::genx::document_simpl doc_s (hello_s, "hello");
    doc_s.add_no_namespace_schema ("hello.xsd");

    hello_s.pre ();

#ifdef XSDE_IOSTREAM
    doc_s.serialize (cout, serializer::genx::document_simpl::pretty_print);
#else
    cout_writer w;
    doc_s.serialize (w, serializer::genx::document_simpl::pretty_print);
#endif

#ifndef XSDE_EXCEPTIONS
    assert (doc_s._error ().type () == serializer::error::none);
#endif

    hello_s.post ();

    cout << endl;
  }
  catch (const serializer::exception& e)
  {
    cerr << "error: " << e.text () << endl;
    return 1;
  }
  catch (const ios_base::failure&)
  {
    cerr << "error: write failure" << endl;
    return 1;
  }

  return 0;
}
