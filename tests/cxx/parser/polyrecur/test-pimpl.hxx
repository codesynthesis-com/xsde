// file      : tests/cxx/parser/polyrecur/test-pimpl.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef TEST_PIMPL_HXX
#define TEST_PIMPL_HXX

#include "test-pskel.hxx"

namespace test
{
  class root_pimpl: public root_pskel
  {
  public:
    virtual void
    pre ();

    virtual void
    expression ();

    virtual void
    post_root ();
  };

  class expression_pimpl: public expression_pskel
  {
  public:
    virtual void
    pre ();

    virtual void
    post_expression ();
  };

  class recursive_pimpl: public recursive_pskel
  {
  public:
    recursive_pimpl (): recursive_pskel (&base_impl_) {}

    virtual void
    pre ();

    virtual void
    expression ();

    virtual void
    post_expression ();

    virtual void
    post_recursive ();

    expression_pimpl base_impl_;
  };

  class value_pimpl: public value_pskel
  {
  public:
    value_pimpl (): value_pskel (&base_impl_) {}

    virtual void
    pre ();

    virtual void
    constant (int);

    virtual void
    post_expression ();

    virtual void
    post_value ();

    expression_pimpl base_impl_;
  };

  class value_a_pimpl: public value_a_pskel
  {
  public:
    value_a_pimpl (): value_a_pskel (&base_impl_) {}

    virtual void
    pre ();

    virtual void
    post_expression ();

    virtual void
    post_value ();

    virtual void
    post_value_a ();

    value_pimpl base_impl_;
  };

  class value_b_pimpl: public value_b_pskel
  {
  public:
    value_b_pimpl (): value_b_pskel (&base_impl_) {}

    virtual void
    pre ();

    virtual void
    post_expression ();

    virtual void
    post_value ();

    virtual void
    post_value_b ();

    value_pimpl base_impl_;
  };
}

#endif // TEST_PIMPL_HXX
