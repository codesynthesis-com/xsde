// file      : tests/cxx/parser/polyrecur/driver.cxx
// copyright : Copyright (c) 2006-2017 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

// Test recursive polymorphic parsing.
//
#include <iostream>

#include "test-pskel.hxx"
#include "test-pimpl.hxx"

using namespace std;
using namespace test;

int
main (int argc, char* argv[])
{
  if (argc != 2)
  {
    cerr << "usage: " << argv[0] << " test.xml" << endl;
    return 1;
  }

#ifdef XSDE_EXCEPTIONS
  try
  {
#endif

    // Instantiate individual parsers.
    //
    xml_schema::int_pimpl int_p;

    root_pimpl root_p;
    expression_pimpl expression_p;
    recursive_pimpl recursive_p;
    value_a_pimpl value_a_p;
    value_b_pimpl value_b_p;

    xml_schema::parser_map_impl expression_map (5);

    // Connect the parsers together.
    //
    expression_map.insert (value_a_p);
    expression_map.insert (value_b_p);
    expression_map.insert (recursive_p);

    root_p.parsers (expression_p);
    root_p.expression_parser (expression_map);

    recursive_p.parsers (expression_p);
    recursive_p.expression_parser (expression_map);

    value_a_p.parsers (int_p);
    value_b_p.parsers (int_p);

    xml_schema::document_pimpl doc_p (root_p, "test", "root", true);

    root_p.pre ();
    doc_p.parse (argv[1]);

#ifndef XSDE_EXCEPTIONS
    if (doc_p._error ())
    {
      cerr << "error" << endl;
      return 1;
    }
#endif

    root_p.post_root ();

#ifdef XSDE_EXCEPTIONS
  }
  catch (xml_schema::parser_exception const& e)
  {
    cerr << e << endl;
    return 1;
  }
  catch (std::ios_base::failure const&)
  {
    cerr << "io failure" << endl;
    return 1;
  }
#endif

  return 0;
}
