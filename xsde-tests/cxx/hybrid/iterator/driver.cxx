// file      : cxx/hybrid/iterator/driver.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

// Test conformance to STL iterator requirements.
//

#include <iostream>
#include <algorithm>

#include "test.hxx"

#undef NDEBUG
#include <cassert>

using namespace std;
using namespace test;

struct fix_pred
{
  fix_pred (int v): v_ (v) {}
  bool operator() (const fix& x) const {return x.a () == v_;}
  int v_;
};

struct var_pred
{
  var_pred (const char* v): v_ (v) {}
  bool operator() (const var& x) const {return x.a () == v_;}
  const char* v_;
};

static inline var*
new_var ()
{
#ifndef XSDE_CUSTOM_ALLOCATOR
  return new var;
#else
  var* r (static_cast<var*> (xsde::cxx::alloc (sizeof (var))));
  return new (r) var;
#endif
}

int
main ()
{
  type::f_sequence fs;

  fix f;
  f.a (1);
  fs.push_back (f);
  f.a (2);
  fs.push_back (f);
  f.a (3);
  fs.push_back (f);

  assert (find_if (fs.begin (), fs.end (), fix_pred (2))->a () == 2);


  type::v_sequence vs;

  var* v;
  v = new_var ();
  v->a ("a");
  vs.push_back (v);
  v = new_var ();
  v->a ("b");
  vs.push_back (v);
  v = new_var ();
  v->a ("c");
  vs.push_back (v);

  assert (find_if (vs.begin (), vs.end (), var_pred ("b"))->a () == "b");

  return 0;
}
