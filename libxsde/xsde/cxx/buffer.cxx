// file      : xsde/cxx/buffer.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // memcpy

#include <xsde/cxx/buffer.hxx>

namespace xsde
{
  namespace cxx
  {
    // c-tors
    //

#ifdef XSDE_EXCEPTIONS
    buffer::
    buffer (size_t size)
        : data_ (0), size_ (0), capacity_ (0)
    {
      capacity (size);
      size_ = size;
    }

    buffer::
    buffer (size_t size, size_t cap)
        : data_ (0), size_ (0), capacity_ (0)
    {
      if (size > cap)
        throw bounds ();

      capacity (cap);
      size_ = size;
    }

    buffer::
    buffer (const void* data, size_t size)
        : data_ (0), size_ (0), capacity_ (0)
    {
      capacity (size);
      size_ = size;

      if (size_)
        memcpy (data_, data, size_);
    }

    buffer::
    buffer (const void* data, size_t size, size_t cap)
        : data_ (0), size_ (0), capacity_ (0)
    {
      if (size > cap)
        throw bounds ();

      capacity (cap);
      size_ = size;

      if (size_)
        memcpy (data_, data, size_);
    }

    buffer::
    buffer (void* data, size_t size, size_t cap, ownership_value)
        : data_ (0), size_ (0), capacity_ (0)
    {
      if (size > cap)
        throw bounds ();

      data_ = reinterpret_cast<char*> (data);
      size_ = size;
      capacity_ = cap;
    }
#endif // XSDE_EXCEPTIONS

    //
    //

#ifdef XSDE_EXCEPTIONS
    bool buffer::
    capacity (size_t capacity, bool copy)
    {
      if (size_ > capacity)
        throw bounds ();

      if (capacity <= capacity_)
      {
        return false; // Do nothing if shrinking is requested.
      }
      else
      {
        char* data = reinterpret_cast<char*> (operator new (capacity));

        if (copy && size_ > 0)
          memcpy (data, data_, size_);

        if (data_)
          operator delete (data_);

        data_ = data;
        capacity_ = capacity;

        return true;
      }
    }
#else
    buffer::error buffer::
    capacity (size_t capacity, bool copy, bool* moved)
    {
      if (size_ > capacity)
        return error_bounds;

      if (capacity <= capacity_)
      {
        // Do nothing if shrinking is requested.
        //
        if (moved)
          *moved = false;
      }
      else
      {
        char* data = reinterpret_cast<char*> (operator new (capacity));

        if (data != 0)
        {
          if (copy && size_ > 0)
            memcpy (data, data_, size_);

          if (data_)
            operator delete (data_);

          data_ = data;
          capacity_ = capacity;

          if (moved)
            *moved = true;
        }
        else
          return error_no_memory;
      }

      return error_none;
    }
#endif
  }
}
