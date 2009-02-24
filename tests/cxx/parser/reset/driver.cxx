// file      : tests/cxx/parser/reset/driver.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

// Test parser resetting.
//

#include <assert.h>

#include <string>
#include <iostream>

#include "test-pskel.hxx"

using namespace std;
using namespace test;

bool fail = true;

struct error {};

#ifdef XSDE_REUSE_STYLE_MIXIN
struct base_pimpl: virtual base_pskel
#else
struct base_pimpl: base_pskel
#endif
{
  base_pimpl (unsigned long i)
      : i_ (i)
  {
  }

  virtual void
  pre ()
  {
#ifndef XSDE_EXCEPTIONS
    assert (!_error ());
#endif

    if (fail && i_ == 3)
    {
#ifdef XSDE_EXCEPTIONS
      throw error ();
#else
      _app_error (1);
#endif
    }
  }

private:
  unsigned long i_;
};

#ifdef XSDE_REUSE_STYLE_MIXIN
struct inner_pimpl: inner_pskel, base_pimpl
#else
struct inner_pimpl: inner_pskel
#endif
{
  inner_pimpl (unsigned long i)
#ifdef XSDE_REUSE_STYLE_MIXIN
      : base_pimpl (i), i_ (i)
#else
      : inner_pskel (&base_impl_), base_impl_ (i), i_ (i)
#endif
  {
  }

  virtual void
  b (int v)
  {
    if (fail && i_ == 4)
    {
#ifdef XSDE_EXCEPTIONS
      throw error ();
#else
      _app_error (1);
#endif
    }

    if (!fail)
      cout << i_ << ": " << v << endl;
  }

  virtual void
  post_inner ()
  {
    if (fail && i_ == 5)
    {
#ifdef XSDE_EXCEPTIONS
      throw error ();
#else
      _app_error (1);
#endif
    }
  }

private:
#ifdef XSDE_REUSE_STYLE_TIEIN
  base_pimpl base_impl_;
#endif
  unsigned long i_;
};

struct type_pimpl: type_pskel
{
  type_pimpl (unsigned long i)
      : i_ (i)
  {
  }

  virtual void
  pre ()
  {
    if (fail && i_ == 0)
    {
#ifdef XSDE_EXCEPTIONS
      throw error ();
#else
      _app_error (1);
#endif
    }
  }

  virtual void
  a ()
  {
    if (fail && i_ == 1)
    {
#ifdef XSDE_EXCEPTIONS
      throw error ();
#else
      _app_error (1);
#endif
    }
  }

  virtual void
  post_type ()
  {
    if (fail && i_ == 2)
    {
#ifdef XSDE_EXCEPTIONS
      throw error ();
#else
      _app_error (1);
#endif
    }
  }

private:
  unsigned long i_;
};

int
main (int argc, char* argv[])
{
  if (argc != 4)
  {
    cerr << "usage: " << argv[0] << " pass.xml fail-xml.xml fail-schema.xml"
         << endl;
    return 1;
  }

  const char* pass = argv[1];
  const char* fail_xml = argv[2];
  const char* fail_schema = argv[3];

  try
  {
    for (unsigned long i (0); i < 8; ++i)
    {
#ifndef XSDE_PARSER_VALIDATION
      if (i == 7)
      {
        cout << "7: 1" << endl;
	continue;
      }
#endif

      xml_schema::int_pimpl int_p;
      inner_pimpl inner_p (i);
      type_pimpl type_p (i);

      inner_p.parsers (int_p);
      type_p.parsers (inner_p);

      xml_schema::document_pimpl doc_p (type_p, "test", "root");

#ifdef XSDE_EXCEPTIONS
      try
      {
        fail = true;

        type_p.pre ();

        switch (i)
        {
        case 6:
          {
            doc_p.parse (fail_xml);
            break;
          }
        case 7:
          {
            doc_p.parse (fail_schema);
            break;
          }
        default:
          {
            doc_p.parse (pass);
            break;
          }
        }

        type_p.post_type ();
        assert (false);
      }
      catch (error const&)
      {
      }
      catch (xml_schema::parser_exception const&)
      {
      }

      fail = false;
      doc_p.reset ();

      type_p.pre ();
      doc_p.parse (pass);
      type_p.post_type ();
#else
      do
      {
        fail = true;

        type_p.pre ();

        if (type_p._error ())
          break;

        switch (i)
        {
        case 6:
          {
            doc_p.parse (fail_xml);
            break;
          }
        case 7:
          {
            doc_p.parse (fail_schema);
            break;
          }
        default:
          {
            doc_p.parse (pass);
            break;
          }
        }

        if (doc_p._error ())
          break;

        type_p.post_type ();

        if (type_p._error ())
          break;

        assert (false);
      }
      while (false);

      fail = false;
      doc_p.reset ();

      type_p.pre ();
      assert (!type_p._error ());

      doc_p.parse (pass);
      assert (!doc_p._error ());

      type_p.post_type ();
      assert (!type_p._error ());
#endif
    }
  }
#ifdef XSDE_EXCEPTIONS
  catch (xml_schema::parser_exception const& e)
  {
    cerr << e << endl;
    return 1;
  }
#endif
  catch (std::ios_base::failure const&)
  {
    cerr << "io failure" << endl;
    return 1;
  }
}
