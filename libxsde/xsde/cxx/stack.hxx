// file      : xsde/cxx/stack.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_STACK_HXX
#define XSDE_CXX_STACK_HXX

#include <stddef.h> // size_t

#include <xsde/cxx/config.hxx>

namespace xsde
{
  namespace cxx
  {
    // POD stack with pre-allocated first element. You may
    // need to pad your elements to get the proper alignment.
    //
    struct stack
    {
#ifndef XSDE_EXCEPTIONS
      enum error
      {
        error_none,
        error_no_memory
      };
#endif

      ~stack ();
      stack (size_t element_size, void* first_element);

    private:
      stack (stack&);
      stack& operator= (stack&);

    public:
      void
      pop ();

#ifdef XSDE_EXCEPTIONS
      void
#else
      error
#endif
      push ();

      void*
      top ();

      void
      clear ();

      bool
      empty () const;

      size_t
      size () const;

      size_t
      element_size () const;

    private:
#ifdef XSDE_EXCEPTIONS
      void
#else
      error
#endif
      grow ();

    private:
      size_t el_size_;
      void* first_;
      char* data_;
      size_t size_;
      size_t capacity_;
    };
  }
}

#include <xsde/cxx/stack.ixx>

#endif // XSDE_CXX_STACK_HXX
