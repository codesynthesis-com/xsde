// file      : xsde/cxx/qname.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // strcmp

namespace xsde
{
  namespace cxx
  {
    // c-tors, d-tor and swap
    //
    inline qname::
    ~qname ()
    {
      delete[] prefix_;
      delete[] name_;
    }

    inline qname::
    qname ()
        : prefix_ (0), name_ (0)
    {
    }

    inline qname::
    qname (char* n)
        : prefix_ (0), name_ (n)
    {
    }

    inline qname::
    qname (char* p, char* n)
        : prefix_ (p), name_ (n)
    {
    }

    inline void qname::
    swap (qname& x)
    {
      char* p = prefix_;
      char* n = name_;

      prefix_ = x.prefix_;
      name_ = x.name_;

      x.prefix_ = p;
      x.name_ = n;
    }

    // preix
    //
    inline char* qname::
    prefix ()
    {
      return prefix_;
    }

    inline const char* qname::
    prefix () const
    {
      return prefix_;
    }

    inline void qname::
    prefix (char* prefix)
    {
      delete[] prefix_;
      prefix_ = prefix;
    }

    inline char* qname::
    prefix_detach ()
    {
      char* r = prefix_;
      prefix_ = 0;
      return r;
    }

    // name
    //
    inline char* qname::
    name ()
    {
      return name_;
    }

    inline const char* qname::
    name () const
    {
      return name_;
    }

    inline void qname::
    name (char* name)
    {
      delete[] name_;
      name_ = name;
    }

    inline char* qname::
    name_detach ()
    {
      char* r = name_;
      name_ = 0;
      return r;
    }

    //
    //
    inline bool
    operator== (const qname& x, const qname& y)
    {
      return strcmp (x.name (), y.name ()) == 0 &&
        strcmp (x.prefix (), y.prefix ()) == 0;
    }

    inline bool
    operator!= (const qname& x, const qname& y)
    {
      return !(x == y);
    }
  }
}
