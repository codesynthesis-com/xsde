// file      : cxx/hybrid/wildcard/body.cxx
// copyright : not copyrighted - public domain

#include "body.hxx"

namespace email
{
  void body::
  body_type (body::type t)
  {
    if (body_type_ == type_binary)
    {
      delete binary_;
      binary_ = 0;
    }

    body_type_ = t;
  }

  void
  destroy_body (void* p, size_t)
  {
    delete static_cast<body*> (p);
  }
}
