// file      : xsde/cxx/parser/state.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      // stack
      //

      inline stack::
      ~stack ()
      {
        delete[] data_;
      }

      inline stack::
      stack (size_t el_size, void* first_el)
          : el_size_ (el_size), first_ (first_el),
            data_ (0), size_ (0), capacity_ (0)
      {
      }

      inline void stack::
      pop ()
      {
        --size_;
      }

      inline stack::error stack::
      push ()
      {
        if (size_ > capacity_)
          if (error e = grow ())
            return e;

        ++size_;

        return error_none;
      }

      inline void* stack::
      top ()
      {
        return size_ == 1 ? first_ : data_ + (size_ - 1) * el_size_;
      }

      inline size_t stack::
      element_size () const
      {
        return el_size_;
      }

      inline void stack::
      clear ()
      {
        size_ = 0;
      }

      // parser_stack
      //
      inline parser_stack::
      parser_stack (parser_state& first)
          : stack_ (sizeof (parser_state), &first)
      {
      }

      inline stack::error parser_stack::
      push (parser_state& s)
      {
        if (stack::error e = stack_.push ())
          return e;

        *static_cast<parser_state*> (stack_.top ()) = s;
        return stack::error_none;
      }

      inline void parser_stack::
      pop ()
      {
        stack_.pop ();
      }

      inline parser_state& parser_stack::
      top ()
      {
        return *static_cast<parser_state*> (stack_.top ());
      }

      inline void parser_stack::
      clear ()
      {
        stack_.clear ();
      }
    }
  }
}

