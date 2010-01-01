// file      : xsde/cxx/hybrid/sequence.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/hybrid/sequence.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
      //
      // data_sequence
      //

      void data_sequence::
      clear ()
      {
        if (destructor_)
        {
          for (size_t i = 0;  i < size_; ++i)
            destructor_ (static_cast<void**> (data_)[i], i);
        }

        size_ = 0;
      }
    }
  }
}
