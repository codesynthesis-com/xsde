// file      : cxx/hybrid/binary/cdr/driver.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

// Test CDR insertion and extraction.
//

#include <memory>   // std::{unique,auto}_ptr
#include <iostream>

#include <ace/CDR_Stream.h>

#include "test.hxx"
#include "test-pimpl.hxx"
#include "test-simpl.hxx"

#undef NDEBUG
#include <cassert>

using std::cerr;
using std::endl;

using namespace test;

int
main (int argc, char* argv[])
{
  /*
  try
  {
  */

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

#ifdef XSDE_CXX11
  std::unique_ptr<type> r (root_p.post ());
#else
  std::auto_ptr<type> r (root_p.post ());
#endif

  // Save to a CDR stream.
  //
  ACE_OutputCDR ace_ocdr;
  xml_schema::ocdrstream ocdr (ace_ocdr);
  ocdr << *r;

  // Load from a CDR stream.
  //
  ACE_InputCDR ace_icdr (ace_ocdr);
  xml_schema::icdrstream icdr (ace_icdr);

#ifdef XSDE_CXX11
  std::unique_ptr<type> c (new type);
#else
  std::auto_ptr<type> c (new type);
#endif

  icdr >> *c;

  // Serialize.
  //
  root_saggr root_s;

  xml_schema::document_simpl doc_s (
    root_s.root_serializer (),
    root_s.root_namespace (),
    root_s.root_name ());

  doc_s.add_prefix ("t", "test");

  root_s.pre (*c);
  doc_s.serialize (std::cout);
  root_s.post ();

  /*
  }
  catch (const xml_schema::xdr_exception&)
  {
    cerr << "XDR operation filed" << endl;
    return 1;
  }
  catch (const xml_schema::parser_exception& e)
  {
    cerr << argv[0] << ":" << e.line () << ":" << e.column () << ": "
         << e.text () << endl;
    return 1;
  }
  catch (const xml_schema::serializer_exception& e)
  {
    cerr << "error: " << e.text () << endl;
    return 1;
  }
  catch (const std::ios_base::failure&)
  {
    cerr << argv[0] << ": unable to open or read/write failure" << endl;
    return 1;
  }
  */

  return 0;
}
