// file      : cxx/parser/library/library-pimpl-tiein.hxx
// copyright : not copyrighted - public domain

#ifndef LIBRARY_PIMPL_HXX
#define LIBRARY_PIMPL_HXX

#include "library.hxx"
#include "library-pskel.hxx"

namespace library
{
  //
  //
  struct isbn_pimpl: isbn_pskel
  {
    isbn_pimpl ();

    virtual isbn
    post_isbn ();

  private:
    xml_schema::unsigned_int_pimpl base_impl_;
  };

  //
  //
  struct title_pimpl: title_pskel
  {
    title_pimpl ();

    virtual void
    _pre ();

    virtual void
    lang (const std::string&);

    virtual title
    post_title ();

  private:
    title title_;
    xml_schema::string_pimpl base_impl_;
  };

  //
  //
  struct genre_pimpl: genre_pskel
  {
    genre_pimpl ();

    virtual genre
    post_genre ();

  private:
    xml_schema::string_pimpl base_impl_;
  };

  //
  //
  struct person_pimpl: person_pskel
  {
    virtual void
    _pre ();

    virtual void
    name (const std::string&);

    virtual void
    born (const std::string&);

    virtual void
    died (const std::string&);

    virtual person
    post_person ();

  private:
    person person_;
  };

  //
  //
  struct author_pimpl: author_pskel
  {
    author_pimpl ();

    virtual void
    _pre ();

    virtual void
    recommends (const std::string&);

    virtual author
    post_author ();

  private:
    person_pimpl base_impl_;
    author author_;
  };

  //
  //
  struct book_pimpl: book_pskel
  {
    virtual void
    _pre ();

    virtual void
    isbn (library::isbn);

    virtual void
    title (const library::title&);

    virtual void
    genre (library::genre);

    virtual void
    author (const library::author&);

    virtual void
    available (bool);

    virtual void
    id (const std::string&);

    virtual book
    post_book ();

  private:
    book book_;
  };

  //
  //
  struct catalog_pimpl: catalog_pskel
  {
    virtual void
    _pre ();

    virtual void
    book (const library::book&);

    virtual catalog
    post_catalog ();

  private:
    catalog catalog_;
  };
}

#endif // LIBRARY_PIMPL_HXX
