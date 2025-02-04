// file      : cxx/parser/validation/built-in/string/driver.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

// Test the built-in string & friends types validation.
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

using namespace xsde::cxx;
using namespace xsde::cxx::parser;
using namespace xsde::cxx::parser::validating;

bool
compare (const string_sequence* x, const string_sequence& y)
{
  bool r = *x == y;
  delete x;
  return r;
}

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
  // We are going to leak a bit of memory in the no-STL case.
  //
  using std::string;

  // Good.
  //

  // string
  //
  {
    context c;
    string_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" \n\t");
    p._characters (" aaa ");
    p._characters ("bbb");
    p._characters (" ");
    p._post ();
    assert (!c.error_type () &&
            string_wrapper (p.post_string ()).value == " \n\t aaa bbb ");
  }

  // normalized_string
  //
  {
    context c;
    normalized_string_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" \n\t");
    p._characters (" aaa \n\t ");
    p._characters (" bbb");
    p._characters ("  ");
    p._post ();
    assert (!c.error_type () &&
            string_wrapper (p.post_normalized_string ()).value ==
            "    aaa     bbb  ");
  }

  // token
  //
  {
    context c;
    token_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" \n\t");
    p._characters (" aaa \n\t ");
    p._characters (" bbb \n\t");
    p._characters ("  ");
    p._post ();
    assert (!c.error_type () &&
            string_wrapper (p.post_token ()).value == "aaa bbb");
  }

  // name
  //
  {
    context c;
    name_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" \n\t");
    p._characters (" a:b-c_d123 ");
    p._characters ("  ");
    p._post ();
    assert (!c.error_type () &&
            string_wrapper (p.post_name ()).value == "a:b-c_d123");
  }

  {
    context c;
    name_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" \n\t");
    p._characters (" _12 ");
    p._characters ("  ");
    p._post ();
    assert (!c.error_type () &&
            string_wrapper (p.post_name ()).value == "_12");
  }

  {
    context c;
    name_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" \n\t");
    p._characters (" :12 ");
    p._characters ("  ");
    p._post ();
    assert (!c.error_type () &&
            string_wrapper (p.post_name ()).value == ":12");
  }

  // nmtoken
  //
  {
    context c;
    nmtoken_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" \n\t");
    p._characters (" 123a:b-c_d123 ");
    p._characters (" \n\t");
    p._characters ("  ");
    p._post ();
    assert (!c.error_type () &&
            string_wrapper (p.post_nmtoken ()).value == "123a:b-c_d123");
  }

  // nmtokens
  //
  {
    context c;
    string_sequence s;
#ifdef XSDE_STL
    s.push_back ("123");
    s.push_back ("abc");
#else
    s.push_back_copy ("123");
    s.push_back_copy ("abc");
#endif

    nmtokens_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" \n\t");
    p._characters (" 123 ");
    p._characters (" \n\t abc ");
    p._characters ("  ");
    p._post ();
    assert (!c.error_type () && compare (p.post_nmtokens (), s));
  }

  // ncname
  //
  {
    context c;
    ncname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" \n\t");
    p._characters (" a.b-c_d123 ");
    p._characters ("  ");
    p._post ();
    assert (!c.error_type () &&
            string_wrapper (p.post_ncname ()).value == "a.b-c_d123");
  }

  // id
  //
  {
    context c;
    id_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" \n\t");
    p._characters (" a.b-c_d123 ");
    p._characters ("  ");
    p._post ();
    assert (!c.error_type () &&
            string_wrapper (p.post_id ()).value == "a.b-c_d123");
  }

  // idref
  //
  {
    context c;
    idref_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" \n\t");
    p._characters (" a.b-c_d123 ");
    p._characters ("  ");
    p._post ();
    assert (!c.error_type () &&
            string_wrapper (p.post_idref ()).value == "a.b-c_d123");
  }

  // idrefs
  //
  {
    context c;
    string_sequence s;
#ifdef XSDE_STL
    s.push_back ("a123");
    s.push_back ("abc");
#else
    s.push_back_copy ("a123");
    s.push_back_copy ("abc");
#endif

    idrefs_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" \n\t");
    p._characters (" a123 ");
    p._characters (" \n\t abc ");
    p._characters ("  ");
    p._post ();
    assert (!c.error_type () && compare (p.post_idrefs (), s));
  }

  // language
  //
  {
    context c;
    language_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" x ");
    p._post ();
    assert (!c.error_type () &&
            string_wrapper (p.post_language ()).value == "x");
  }

  {
    context c;
    language_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" en ");
    p._post ();
    assert (!c.error_type () &&
            string_wrapper (p.post_language ()).value == "en");
  }

  {
    context c;
    language_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" en");
    p._characters ("-us ");
    p._post ();
    assert (!c.error_type () &&
            string_wrapper (p.post_language ()).value == "en-us");
  }

  {
    context c;
    language_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("one-two-three-four44-seven77-eight888");
    p._post ();
    assert (!c.error_type () &&
            string_wrapper (p.post_language ()).value ==
            "one-two-three-four44-seven77-eight888");
  }

  // Bad
  //

  // name
  //
  {
    context c;
    name_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    name_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (".a");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    name_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("-a");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    name_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("1a");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    name_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("a,b");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    name_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("a b");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    name_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("a<b");
    p._post ();
    assert (c.schema_error ());
  }

  // nmtoken
  //
  {
    context c;
    nmtoken_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    nmtoken_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("a,b");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    nmtoken_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("a b");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    nmtoken_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("a<b");
    p._post ();
    assert (c.schema_error ());
  }

  // nmtokens
  //
  {
    context c;
    nmtokens_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" ");
    p._characters (" \t\n  ");
    p._post_impl ();
    assert (c.schema_error () == schema_error::invalid_nmtokens_value);
  }

  {
    context c;
    nmtokens_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("ab a,b");
    p._post_impl ();
    assert (c.schema_error () == schema_error::invalid_nmtoken_value);
  }

  // ncname
  //
  {
    context c;
    ncname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    ncname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (".a");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    ncname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("-a");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    ncname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (":a");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    ncname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("1a");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    ncname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("a:b");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    ncname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("a,b");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    ncname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("a b");
    p._post ();
    assert (c.schema_error ());
  }

  {
    context c;
    ncname_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("a<b");
    p._post ();
    assert (c.schema_error ());
  }

  // id
  //
  {
    context c;
    id_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("a b");
    p._post ();
    assert (c.schema_error () == schema_error::invalid_id_value);
  }

  // idref
  //
  {
    context c;
    idref_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("a b");
    p._post ();
    assert (c.schema_error () == schema_error::invalid_idref_value);
  }

  // idrefs
  //
  {
    context c;
    idrefs_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("  ");
    p._characters (" \t\n ");
    p._post_impl ();
    assert (c.schema_error () == schema_error::invalid_idrefs_value);
  }

  {
    context c;
    idrefs_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("ab a<b");
    p._post_impl ();
    assert (c.schema_error () == schema_error::invalid_idref_value);
  }

  // language
  //
  {
    context c;
    language_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters (" ");
    p._post ();
    assert (c.schema_error () == schema_error::invalid_language_value);
  }

  {
    context c;
    language_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("en-");
    p._post ();
    assert (c.schema_error () == schema_error::invalid_language_value);
  }

  {
    context c;
    language_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("a1");
    p._post ();
    assert (c.schema_error () == schema_error::invalid_language_value);
  }

  {
    context c;
    language_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("en+us");
    p._post ();
    assert (c.schema_error () == schema_error::invalid_language_value);
  }

  {
    context c;
    language_pimpl p;
    p.pre ();
    p._pre_impl (c);
    p._characters ("en-nine99999");
    p._post ();
    assert (c.schema_error () == schema_error::invalid_language_value);
  }

  return 0;
}
