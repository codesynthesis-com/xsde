// file      : cxx/parser/validation/restriction/driver.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

// Test the restriction compositor validation.
//

#include <string>
#include <iostream>

#include "test-pskel.hxx"

#undef NDEBUG
#include <cassert>

using namespace std;
using namespace test;

struct base_a_pimpl: base_a_pskel
{
};

struct restriction_a_pimpl: restriction_a_pskel
{
#ifdef XSDE_REUSE_STYLE_TIEIN
  restriction_a_pimpl ()
      : restriction_a_pskel (0)
  {
  }
#endif
};

struct extension_b_pimpl: extension_b_pskel
{
#ifdef XSDE_REUSE_STYLE_TIEIN
  extension_b_pimpl ()
      : extension_b_pskel (0)
  {
  }
#endif
};

struct restriction_b_pimpl: restriction_b_pskel
{
#ifdef XSDE_REUSE_STYLE_TIEIN
  restriction_b_pimpl ()
      : restriction_b_pskel (0)
  {
  }
#endif
};

struct type_b_pimpl: type_b_pskel
{
};

struct type_r_pimpl: type_r_pskel
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
    xml_schema::int_pimpl int_p;
    base_a_pimpl base_a_p;
    restriction_a_pimpl restriction_a_p;
    extension_b_pimpl extension_b_p;
    restriction_b_pimpl restriction_b_p;
    type_b_pimpl type_b_p;
    type_r_pimpl type_r_p;

    base_a_p.parsers (int_p, int_p, int_p,
                      int_p, int_p, int_p);

    restriction_a_p.parsers (int_p, int_p, int_p,
                             int_p, int_p, int_p);

    extension_b_p.parsers (int_p, int_p, int_p,
                           int_p, int_p);

    restriction_b_p.parsers (int_p, int_p, int_p,
                             int_p, int_p);

    type_b_p.parsers (base_a_p, extension_b_p);
    type_r_p.parsers (restriction_a_p, restriction_b_p);

    xml_schema::document_pimpl doc_b_p (type_b_p, "test", "root");
    xml_schema::document_pimpl doc_r_p (type_r_p, "test", "root");

    type_b_p.pre ();
    doc_b_p.parse (argv[1]);
    type_b_p.post_type_b ();

    try
    {
      type_r_p.pre ();
      doc_r_p.parse (argv[1]);
      type_r_p.post_type_r ();
    }
    catch (xml_schema::parser_exception const& e)
    {
      cout << e.line () << ":" << e.column () << " " << e << endl;
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
