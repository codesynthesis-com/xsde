// file      : examples/cxx/hybrid/evolution/passthrough/xml.cxx
// copyright : not copyrighted - public domain

#include "xml.hxx"

namespace xml
{
  element::
  ~element ()
  {
    for (elements::iterator i = children_.begin ();
         i != children_.end ();
         ++i)
    {
      delete *i;
    }
  }
}
