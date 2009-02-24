// file      : xsde/cxx/parser/state.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_PARSER_STATE_HXX
#define XSDE_CXX_PARSER_STATE_HXX

#include <stddef.h> // size_t

#include <xsde/cxx/parser/elements.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      // POD stack with pre-allocated first element. You may
      // need to pad your elements to get the proper alignment.
      //
      struct stack
      {
        enum error
        {
          error_none,
          error_no_memory
        };

        ~stack ();
        stack (size_t element_size, void* first_element);

      public:
        void
        pop ();

        error
        push ();

        void*
        top ();

        size_t
        element_size () const;

        void
        clear ();

      private:
        error
        grow ();

      private:
        size_t el_size_;
        void* first_;
        char* data_;
        size_t size_;
        size_t capacity_;
      };

      // Optimized state stack for non-recursive case (one element).
      //
      struct parser_stack
      {
        parser_stack (parser_state& first);

        stack::error
        push (parser_state&);

        void
        pop ();

        parser_state&
        top ();

        void
        clear ();

      private:
        stack stack_;
      };
    }
  }
}


#include <xsde/cxx/parser/state.ixx>

#endif  // XSDE_CXX_PARSER_STATE_HXX
