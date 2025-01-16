// file      : cxx/parser/people/people-pimpl-mixin.hxx
// copyright : not copyrighted - public domain

#ifndef PEOPLE_PIMPL_HXX
#define PEOPLE_PIMPL_HXX

#include "gender.hxx"
#include "people-pskel.hxx"

struct gender_pimpl: virtual gender_pskel, xml_schema::string_pimpl
{
  virtual gender
  post_gender ()
  {
    char* s = post_string ();

    if (!_error ())
    {
      gender g = strcmp (s, "male") ? female : male;

      delete[] s;
      return g;
    }
    else
      return male; // This value will never be used.
  }
};

struct person_pimpl: virtual person_pskel
{
  virtual void
  first_name (char* n)
  {
    printf ("first: %s\n", n);
    delete[] n;
  }

  virtual void
  last_name (char* n)
  {
    printf ("last: %s\n", n);
    delete[] n;
  }

  virtual void
  gender (::gender g)
  {
    switch (g)
    {
    case male:
      {
        printf ("gender: male\n");
        break;
      }
    case female:
      {
        printf ("gender: female\n");
        break;
      }
    }
  }

  virtual void
  age (unsigned short a)
  {
    printf ("age: %hu\n", a);
  }
};

struct people_pimpl: virtual people_pskel
{
  virtual void
  person ()
  {
    // Add an extra newline after each person record.
    //
    printf ("\n");
  }
};

#endif // PEOPLE_PIMPL_HXX
