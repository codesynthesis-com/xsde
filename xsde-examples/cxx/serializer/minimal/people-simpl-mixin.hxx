// file      : cxx/serializer/people/people-simpl-mixin.hxx
// copyright : not copyrighted - public domain

#ifndef PEOPLE_SIMPL_HXX
#define PEOPLE_SIMPL_HXX

#include "people.hxx"
#include "people-sskel.hxx"

struct gender_simpl: virtual gender_sskel, xml_schema::string_simpl
{
  virtual void
  pre (gender g)
  {
    string_simpl::pre (g == male ? "male" : "female");
  }
};

struct person_simpl: virtual person_sskel
{
  virtual void
  pre (const person& p)
  {
    person_ = &p;
  }

  virtual const char*
  first_name ()
  {
    return person_->first_name_;
  }

  virtual const char*
  last_name ()
  {
    return person_->last_name_;
  }

  virtual ::gender
  gender ()
  {
    return person_->gender_;
  }

  virtual unsigned short
  age ()
  {
    return person_->age_;
  }

private:
  const person* person_;
};

struct people_simpl: virtual people_sskel
{
  virtual void
  pre (const people& p)
  {
    i_ = 0;
    people_ = &p;
  }

  virtual bool
  person_next ()
  {
    return i_ < people_->size_;
  }

  virtual const ::person&
  person ()
  {
    return people_->people_[i_++];
  }

private:
  size_t i_;
  const people* people_;
};

#endif // PEOPLE_SIMPL_HXX
