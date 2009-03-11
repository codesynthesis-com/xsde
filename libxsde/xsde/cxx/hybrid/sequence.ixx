// file      : xsde/cxx/hybrid/sequence.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // memcpy

#include <new>      // placement new

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
      //
      // pod_seq
      //

      template <typename T>
      inline size_t pod_seq<T>::
      max_size () const
      {
        return size_t (-1) / sizeof (T);
      }

      template <typename T>
      inline void pod_seq<T>::
      swap (pod_seq& x)
      {
        swap_ (x);
      }

      template <typename T>
      inline T* pod_seq<T>::
      begin ()
      {
        return static_cast<T*> (data_);
      }

      template <typename T>
      inline const T* pod_seq<T>::
      begin () const
      {
        // g++ 2.95 does not like static_cast here.
        //
        return (const T*) (data_);
      }

      template <typename T>
      inline T* pod_seq<T>::
      end ()
      {
        return static_cast<T*> (data_) + size_;
      }

      template <typename T>
      inline const T* pod_seq<T>::
      end () const
      {
        return ((const T*) (data_)) + size_;
      }

      template <typename T>
      inline T& pod_seq<T>::
      front ()
      {
        return *static_cast<T*> (data_);
      }

      template <typename T>
      inline const T& pod_seq<T>::
      front () const
      {
        return *((const T*) (data_));
      }

      template <typename T>
      inline T& pod_seq<T>::
      back ()
      {
        return static_cast<T*> (data_)[size_ - 1];
      }

      template <typename T>
      inline const T& pod_seq<T>::
      back () const
      {
        return ((const T*) (data_))[size_ - 1];
      }

      template <typename T>
      inline T& pod_seq<T>::
      operator[] (size_t i)
      {
        return static_cast<T*> (data_)[i];
      }

      template <typename T>
      inline const T& pod_seq<T>::
      operator[] (size_t i) const
      {
        return ((const T*) (data_))[i];
      }

      template <typename T>
      inline void pod_seq<T>::
      clear ()
      {
        size_ = 0;
      }

      template <typename T>
      inline void pod_seq<T>::
      pop_back ()
      {
        --size_;
      }

      template <typename T>
      inline T* pod_seq<T>::
      erase (T* i)
      {
        if (i != static_cast<T*> (data_) + (size_ - 1))
          erase_ (i, sizeof (T), 0);
        else
          --size_;

        return i;
      }

#ifdef XSDE_EXCEPTIONS
      template <typename T>
      inline void pod_seq<T>::
      push_back (const T& x)
      {
        if (capacity_ < size_ + 1)
          grow_ (0, sizeof (T), 0);

        static_cast<T*> (data_)[size_++] = x;
      }

      template <typename T>
      inline T* pod_seq<T>::
      insert (T* i, const T& x)
      {
        T* p = static_cast<T*> (insert_ (i, sizeof (T), 0, 0));
        *p = x;
        return p;
      }

      template <typename T>
      inline void pod_seq<T>::
      reserve (size_t n)
      {
        if (capacity_ < n)
          grow_ (n, sizeof (T), 0);
      }

      template <typename T>
      inline void pod_seq<T>::
      assign (const T* p, size_t n)
      {
        if (capacity_ < n)
          grow_ (n, sizeof (T), 0);

        memcpy (data_, p, n * sizeof (T));
        size_ = n;
      }
#else
      template <typename T>
      inline sequence_base::error pod_seq<T>::
      push_back (const T& x)
      {
        error r = error_none;

        if (capacity_ < size_ + 1)
          r = grow_ (0, sizeof (T), 0);

        if (r == error_none)
          static_cast<T*> (data_)[size_++] = x;

        return r;
      }

      template <typename T>
      inline sequence_base::error pod_seq<T>::
      insert (T* i, const T& x)
      {
        T* p = static_cast<T*> (insert_ (i, sizeof (T), 0, 0));

        if (p)
        {
          *p = x;
          return error_none;
        }
        else
          return error_no_memory;
      }

      template <typename T>
      inline sequence_base::error pod_seq<T>::
      insert (T* i, const T& x, T*& r)
      {
        T* p = static_cast<T*> (insert_ (i, sizeof (T), 0, 0));

        if (p)
        {
          *p = x;
          r = p;
          return error_none;
        }
        else
          return error_no_memory;
      }

      template <typename T>
      inline sequence_base::error pod_seq<T>::
      reserve (size_t n)
      {
        error r = error_none;
        if (capacity_ < n)
          r = grow_ (n, sizeof (T), 0);
        return r;
      }

      template <typename T>
      inline sequence_base::error pod_seq<T>::
      assign (const T* p, size_t n)
      {
        if (capacity_ < n)
        {
          if (error r = grow_ (n, sizeof (T), 0))
            return r;
        }

        memcpy (data_, p, n * sizeof (T));
        size_ = n;
        return error_none;
      }
#endif

      //
      // fix_seq
      //

      template <typename T>
      inline fix_seq<T>::
      ~fix_seq ()
      {
        clear ();
      }

      template <typename T>
      inline size_t fix_seq<T>::
      max_size () const
      {
        return size_t (-1) / sizeof (T);
      }

      template <typename T>
      inline void fix_seq<T>::
      swap (fix_seq& x)
      {
        swap_ (x);
      }

      template <typename T>
      inline T* fix_seq<T>::
      begin ()
      {
        return static_cast<T*> (data_);
      }

      template <typename T>
      inline const T* fix_seq<T>::
      begin () const
      {
        return (const T*) (data_);
      }

      template <typename T>
      inline T* fix_seq<T>::
      end ()
      {
        return static_cast<T*> (data_) + size_;
      }

      template <typename T>
      inline const T* fix_seq<T>::
      end () const
      {
        return ((const T*) (data_)) + size_;
      }

      template <typename T>
      inline T& fix_seq<T>::
      front ()
      {
        return *static_cast<T*> (data_);
      }

      template <typename T>
      inline const T& fix_seq<T>::
      front () const
      {
        return *((const T*) (data_));
      }

      template <typename T>
      inline T& fix_seq<T>::
      back ()
      {
        return static_cast<T*> (data_)[size_ - 1];
      }

      template <typename T>
      inline const T& fix_seq<T>::
      back () const
      {
        return ((const T*) (data_))[size_ - 1];
      }

      template <typename T>
      inline T& fix_seq<T>::
      operator[] (size_t i)
      {
        return static_cast<T*> (data_)[i];
      }

      template <typename T>
      inline const T& fix_seq<T>::
      operator[] (size_t i) const
      {
        return ((const T*) (data_))[i];
      }

      template <typename T>
      inline void fix_seq<T>::
      pop_back ()
      {
        static_cast<T*> (data_)[size_ - 1].~T ();
        --size_;
      }

      template <typename T>
      inline T* fix_seq<T>::
      erase (T* i)
      {
        if (i != static_cast<T*> (data_) + (size_ - 1))
          erase_ (i, sizeof (T), &move_forward_);
        else
        {
          static_cast<T*> (data_)[size_ - 1].~T ();
          --size_;
        }

        return i;
      }

#ifdef XSDE_EXCEPTIONS
      template <typename T>
      inline void fix_seq<T>::
      push_back (const T& x)
      {
        if (capacity_ < size_ + 1)
          grow_ (0, sizeof (T), &move_);

        new (static_cast<T*> (data_) + size_) T (x);
        size_++;
      }

      template <typename T>
      inline T* fix_seq<T>::
      insert (T* i, const T& x)
      {
        T* p = static_cast<T*> (
          insert_ (i, sizeof (T), &move_, &move_backward_));
        *p = x;
        return p;
      }

      template <typename T>
      inline void fix_seq<T>::
      reserve (size_t n)
      {
        if (capacity_ < n)
          grow_ (n, sizeof (T), &move_);
      }
#else
      template <typename T>
      inline sequence_base::error fix_seq<T>::
      push_back (const T& x)
      {
        error r = error_none;

        if (capacity_ < size_ + 1)
          r = grow_ (0, sizeof (T), &move_);

        if (r == error_none)
        {
          new (static_cast<T*> (data_) + size_) T (x);
          size_++;
        }

        return r;
      }

      template <typename T>
      inline sequence_base::error fix_seq<T>::
      insert (T* i, const T& x)
      {
        T* p = static_cast<T*> (
          insert_ (i, sizeof (T), &move_, &move_backward_));

        if (p)
        {
          *p = x;
          return error_none;
        }
        else
          return error_no_memory;
      }

      template <typename T>
      inline sequence_base::error fix_seq<T>::
      insert (T* i, const T& x, T*& r)
      {
        T* p = static_cast<T*> (
          insert_ (i, sizeof (T), &move_, &move_backward_));

        if (p)
        {
          *p = x;
          r = p;
          return error_none;
        }
        else
          return error_no_memory;
      }

      template <typename T>
      inline sequence_base::error fix_seq<T>::
      reserve (size_t n)
      {
        error r = error_none;
        if (capacity_ < n)
          r = grow_ (n, sizeof (T), &move_);
        return r;
      }
#endif

      //
      // var_seq
      //

      template <typename T>
      inline var_seq<T>::
      ~var_seq ()
      {
        clear ();
      }

      template <typename T>
      inline size_t var_seq<T>::
      max_size () const
      {
        return size_t (-1) / sizeof (T*);
      }

      template <typename T>
      inline void var_seq<T>::
      swap (var_seq& x)
      {
        swap_ (x);
      }

      template <typename T>
      inline var_iterator<T> var_seq<T>::
      begin ()
      {
        return iterator (static_cast<T**> (data_));
      }

      template <typename T>
      inline var_const_iterator<T> var_seq<T>::
      begin () const
      {
        return const_iterator ((const T**) (data_));
      }

      template <typename T>
      inline var_iterator<T> var_seq<T>::
      end ()
      {
        return iterator (static_cast<T**> (data_) + size_);
      }

      template <typename T>
      inline var_const_iterator<T> var_seq<T>::
      end () const
      {
        return const_iterator (((const T**) (data_)) + size_);
      }

      template <typename T>
      inline T& var_seq<T>::
      front ()
      {
        return **static_cast<T**> (data_);
      }

      template <typename T>
      inline const T& var_seq<T>::
      front () const
      {
        // g++ 2.95 does not like static_cast here.
        //
        return **((const T* const*) (data_));
      }

      template <typename T>
      inline T& var_seq<T>::
      back ()
      {
        return *(static_cast<T**> (data_)[size_ - 1]);
      }

      template <typename T>
      inline const T& var_seq<T>::
      back () const
      {
        return *(((const T* const*) (data_))[size_ - 1]);
      }

      template <typename T>
      inline T& var_seq<T>::
      operator[] (size_t i)
      {
        return *(static_cast<T**> (data_)[i]);
      }

      template <typename T>
      inline const T& var_seq<T>::
      operator[] (size_t i) const
      {
        return *(((const T* const*) (data_))[i]);
      }

      template <typename T>
      inline void var_seq<T>::
      pop_back ()
      {
        delete static_cast<T**> (data_)[size_ - 1];
        --size_;
      }

      template <typename T>
      inline var_iterator<T> var_seq<T>::
      erase (iterator i)
      {
        delete *i.i_;

        if (i.i_ != static_cast<T**> (data_) + (size_ - 1))
          erase_ (i.i_, sizeof (T*), 0);
        else
          --size_;

        return i;
      }

#ifdef XSDE_EXCEPTIONS
      template <typename T>
      inline void var_seq<T>::
      push_back (T* x)
      {
        guard g (x);

        if (capacity_ < size_ + 1)
          grow_ (0, sizeof (T*), 0);

        static_cast<T**> (data_)[size_++] = x;

        g.release ();
      }

      template <typename T>
      inline var_iterator<T> var_seq<T>::
      insert (iterator i, T* x)
      {
        guard g (x);
        T** p = static_cast<T**> (insert_ (i.i_, sizeof (T*), 0, 0));
        *p = x;
        g.release ();
        return iterator (p);
      }

      template <typename T>
      inline void var_seq<T>::
      reserve (size_t n)
      {
        if (capacity_ < n)
          grow_ (n, sizeof (T*), 0);
      }
#else
      template <typename T>
      inline sequence_base::error var_seq<T>::
      push_back (T* x)
      {
        error r = error_none;

        if (capacity_ < size_ + 1)
          r = grow_ (0, sizeof (T*), 0);

        if (r == error_none)
          static_cast<T**> (data_)[size_++] = x;
        else
          delete x;

        return r;
      }

      template <typename T>
      inline sequence_base::error var_seq<T>::
      insert (iterator i, T* x)
      {
        T** p = static_cast<T**> (insert_ (i.i_, sizeof (T*), 0, 0));

        if (p)
        {
          *p = x;
          return error_none;
        }
        else
        {
          delete x;
          return error_no_memory;
        }
      }

      template <typename T>
      inline sequence_base::error var_seq<T>::
      insert (iterator i, T* x, iterator& r)
      {
        T** p = static_cast<T**> (insert_ (i.i_, sizeof (T*), 0, 0));

        if (p)
        {
          *p = x;
          r.i_ = p;
          return error_none;
        }
        else
        {
          delete x;
          return error_no_memory;
        }
      }

      template <typename T>
      inline sequence_base::error var_seq<T>::
      reserve (size_t n)
      {
        error r = error_none;
        if (capacity_ < n)
          r = grow_ (n, sizeof (T*), 0);
        return r;
      }
#endif

      //
      // data_seq
      //

      inline data_seq::
      ~data_seq ()
      {
        clear ();
      }

      inline data_seq::
      data_seq ()
          : destructor_ (0)
      {
      }

      inline void data_seq::
      destructor (data_seq::destroy_func d)
      {
        destructor_ = d;
      }

      inline size_t data_seq::
      max_size () const
      {
        return size_t (-1) / sizeof (void*);
      }

      inline void data_seq::
      swap (data_seq& x)
      {
        swap_ (x);
      }

      inline data_seq::iterator data_seq::
      begin ()
      {
        return static_cast<void**> (data_);
      }

      inline data_seq::const_iterator data_seq::
      begin () const
      {
        // g++ 2.95 does not like static_cast here.
        //
        return (const void* const*) (data_);
      }

      inline data_seq::iterator data_seq::
      end ()
      {
        return static_cast<void**> (data_) + size_;
      }

      inline data_seq::const_iterator data_seq::
      end () const
      {
        return ((const void* const*) (data_)) + size_;
      }

      inline void* data_seq::
      front ()
      {
        return *static_cast<void**> (data_);
      }

      inline const void* data_seq::
      front () const
      {
        return *((const void* const*) (data_));
      }

      inline void* data_seq::
      back ()
      {
        return static_cast<void**> (data_)[size_ - 1];
      }

      inline const void* data_seq::
      back () const
      {
        return ((const void* const*) (data_))[size_ - 1];
      }

      inline void* data_seq::
      operator[] (size_t i)
      {
        return static_cast<void**> (data_)[i];
      }

      inline const void* data_seq::
      operator[] (size_t i) const
      {
        return ((const void* const*) (data_))[i];
      }

      inline void data_seq::
      pop_back ()
      {
        if (destructor_)
          destructor_ (static_cast<void**> (data_)[size_ - 1], size_ - 1);
        --size_;
      }

      inline data_seq::iterator data_seq::
      erase (iterator i)
      {
        if (destructor_)
          destructor_ (*i, i - static_cast<void**> (data_));

        if (i != static_cast<void**> (data_) + (size_ - 1))
          erase_ (i, sizeof (void*), 0);
        else
          --size_;

        return i;
      }

#ifdef XSDE_EXCEPTIONS
      namespace data_seq_bits
      {
        struct guard
        {
          ~guard () { if (p_ && d_) d_ (p_, i_); }
          guard (data_seq::destroy_func d, void* p, size_t i)
              : d_ (d), p_ (p), i_ (i) {}

          void
          release () { p_ = 0; }

        private:
          data_seq::destroy_func d_;
          void* p_;
          size_t i_;
        };
      }

      inline void data_seq::
      push_back (void* x)
      {
        data_seq_bits::guard g (destructor_, x, size_);

        if (capacity_ < size_ + 1)
          grow_ (0, sizeof (void*), 0);

        static_cast<void**> (data_)[size_++] = x;

        g.release ();
      }

      inline data_seq::iterator data_seq::
      insert (iterator i, void* x)
      {
        data_seq_bits::guard g (
          destructor_, x, i - static_cast<void**> (data_));

        void** p = static_cast<void**> (insert_ (i, sizeof (void*), 0, 0));
        *p = x;
        g.release ();
        return p;
      }

      inline void data_seq::
      reserve (size_t n)
      {
        if (capacity_ < n)
          grow_ (n, sizeof (void*), 0);
      }
#else
      inline sequence_base::error data_seq::
      push_back (void* x)
      {
        error r = error_none;

        if (capacity_ < size_ + 1)
          r = grow_ (0, sizeof (void*), 0);

        if (r == error_none)
          static_cast<void**> (data_)[size_++] = x;
        else
        {
          if (destructor_)
            destructor_ (x, size_);
        }

        return r;
      }

      inline sequence_base::error data_seq::
      insert (iterator i, void* x)
      {
        size_t pos = i - static_cast<void**> (data_);
        void** p = static_cast<void**> (insert_ (i, sizeof (void*), 0, 0));

        if (p)
        {
          *p = x;
          return error_none;
        }
        else
        {
          if (destructor_)
            destructor_ (x, pos);
          return error_no_memory;
        }
      }

      inline sequence_base::error data_seq::
      insert (iterator i, void* x, iterator& r)
      {
        size_t pos = i - static_cast<void**> (data_);
        void** p = static_cast<void**> (insert_ (i, sizeof (void*), 0, 0));

        if (p)
        {
          *p = x;
          r = p;
          return error_none;
        }
        else
        {
          if (destructor_)
            destructor_ (x, pos);
          return error_no_memory;
        }
      }

      inline sequence_base::error data_seq::
      reserve (size_t n)
      {
        error r = error_none;
        if (capacity_ < n)
          r = grow_ (n, sizeof (void*), 0);
        return r;
      }
#endif
    }
  }
}
