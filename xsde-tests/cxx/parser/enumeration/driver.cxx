// file      : cxx/parser/enumeration/driver.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

// Test xsd:enumeration parsing.
//

#include <string>
#include <iostream>

#ifdef XSDE_STL
#  include <xsde/cxx/string.hxx>
#endif

#include "test-pskel.hxx"

#undef NDEBUG
#include <cassert>

using namespace std;
using namespace xml_schema;

namespace test
{
  struct digit_pimpl: digit_pskel
  {
    digit_pimpl ()
        : digit_pskel (&base_impl_)
    {
    }

    int_pimpl base_impl_;
  };

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

  struct gender_pimpl: gender_pskel
  {
    gender_pimpl ()
        : gender_pskel (&base_impl_)
    {
    }

    virtual ::gender
    post_gender ()
    {
      string_wrapper str (post_string ());

      if (str.value == "male")
        return male;
      else
        return female;
    }

    string_pimpl base_impl_;
  };

  struct type_pimpl: type_pskel
  {
    virtual void
    digit (int i)
    {
      cout << i << endl;
    }

    virtual void
    gender (::gender g)
    {
      cout << g << endl;
    }
  };
}

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
    test::digit_pimpl digit_p;
    test::gender_pimpl gender_p;
    test::type_pimpl type_p;

    type_p.parsers (digit_p, gender_p);

    document_pimpl doc_p (type_p, "test", "root");

    type_p.pre ();
    doc_p.parse (argv[1]);
    type_p.post_type ();
  }
  catch (parser_exception const& e)
  {
    cerr << e << endl;
    return 1;
  }
  catch (std::ios_base::failure const&)
  {
    cerr << "io failure" << endl;
    return 1;
  }

  return 0;
}
