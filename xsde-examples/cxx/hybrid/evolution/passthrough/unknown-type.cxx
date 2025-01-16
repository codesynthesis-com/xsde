// file      : cxx/hybrid/evolution/passthrough/unknown-type.cxx
// copyright : not copyrighted - public domain

// Include transform.hxx (which includes unknown-type.hxx) instead of
// unknown-type.hxx.
//
#ifndef TRANSFORM_V2
#  include "transform.hxx"
#else
#  include "transform-v2.hxx"
#endif

namespace transform
{
  unknown_type::
  unknown_type ()
      : name_ ("unknown"), content_ ("unknown", 0) // Dummy names.
  {
  }
}
