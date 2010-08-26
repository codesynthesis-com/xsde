// file      : xsde/cxx/stack.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifdef XSDE_CUSTOM_ALLOCATOR
#  include <xsde/cxx/allocator.hxx>
#else
#  include <new> // operator new/delete
#endif

namespace xsde
{
  namespace cxx
  {
    inline stack::
    ~stack ()
    {
      if (data_)
#ifndef XSDE_CUSTOM_ALLOCATOR
        operator delete (data_);
#else
        cxx::free (data_);
#endif
    }

    inline stack::
    stack (size_t el_size, void* first_el)
        : el_size_ (el_size),
          first_ (first_el),
          data_ (0),
          size_ (0),
          capacity_ (0)
    {
    }

    inline void stack::
    pop ()
    {
      --size_;
    }

#ifdef XSDE_EXCEPTIONS
    inline void stack::
#else
    inline stack::error stack::
#endif
    push ()
    {
      if (size_ > capacity_)
      {
#ifdef XSDE_EXCEPTIONS
        grow ();
#else
        if (error e = grow ())
          return e;
#endif
      }

      ++size_;

#ifndef XSDE_EXCEPTIONS
      return error_none;
#endif
    }

    inline void* stack::
    top ()
    {
      return size_ == 1 ? first_ : data_ + (size_ - 2) * el_size_;
    }

    inline void stack::
    clear ()
    {
      size_ = 0;
    }

    inline bool stack::
    empty () const
    {
      return size_ == 0;
    }

    inline size_t stack::
    size () const
    {
      return size_;
    }

    inline size_t stack::
    element_size () const
    {
      return el_size_;
    }
  }
}
