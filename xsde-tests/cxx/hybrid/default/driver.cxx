// file      : cxx/hybrid/default/driver.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

// Test default attribute values.
//

#include <iostream>

#include "test.hxx"
#include "test-pimpl.hxx"
#include "test-simpl.hxx"

#undef NDEBUG
#include <cassert>

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

  // Parse.
  //
  root_paggr root_p;

  xml_schema::document_pimpl doc_p (
    root_p.root_parser (),
    root_p.root_namespace (),
    root_p.root_name ());

  root_p.pre ();
  doc_p.parse (argv[1]);
  type* r = root_p.post ();

  // Serialize.
  //
  root_saggr root_s;

  xml_schema::document_simpl doc_s (
    root_s.root_serializer (),
    root_s.root_namespace (),
    root_s.root_name ());

  doc_s.add_prefix ("t", "test");

  root_s.pre (*r);
  doc_s.serialize (cout);
  root_s.post ();

#ifndef XSDE_CUSTOM_ALLOCATOR
  delete r;
#else
  r->~type ();
  xsde::cxx::free (r);
#endif

  return 0;
}
