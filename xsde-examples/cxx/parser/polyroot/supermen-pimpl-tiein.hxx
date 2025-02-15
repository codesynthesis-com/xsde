// file      : cxx/parser/polyroot/supermen-pimpl-tiein.hxx
// copyright : not copyrighted - public domain

#ifndef SUPERMEN_PIMPL_HXX
#define SUPERMEN_PIMPL_HXX

#include "supermen-pskel.hxx"

class person_pimpl: public person_pskel
{
public:
  virtual void
  pre ();

  virtual void
  name (const std::string&);

  virtual void
  post_person ();
};

class superman_pimpl: public superman_pskel
{
public:
  superman_pimpl ();

  virtual void
  pre ();

  virtual void
  can_fly (bool);

  // By default, post_superman() calls post_person(). In case of
  // polymorphic parsing we want the opposite: post_person() calls
  // post_superman().
  //
  virtual void
  post_person ();

  virtual void
  post_superman ();

private:
  person_pimpl base_impl_;
};

class batman_pimpl: public batman_pskel
{
public:
  batman_pimpl ();

  virtual void
  pre ();

  virtual void
  wing_span (unsigned int);

  // By default, post_batman() calls post_superman() which calls
  // post_person(). In case of polymorphic parsing we want the
  // opposite: post_person() calls post_superman() which calls
  // post_batman().
  //
  virtual void
  post_person ();

  virtual void
  post_superman ();

  virtual void
  post_batman ();

private:
  superman_pimpl base_impl_;
};

#endif // SUPERMEN_PIMPL_HXX
