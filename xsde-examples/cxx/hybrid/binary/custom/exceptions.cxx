// file      : cxx/hybrid/binary/custom/exceptions.cxx
// copyright : not copyrighted - public domain

#include "exceptions.hxx"

const char* raw_exception::
what () const throw ()
{
  return "RAW stream operation failed";
}
