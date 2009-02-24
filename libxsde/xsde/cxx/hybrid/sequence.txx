// file      : xsde/cxx/hybrid/sequence.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <new> // placement new

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
      //
      // fix_seq
      //

      template <typename T>
      void fix_seq<T>::
      clear ()
      {
        for (size_t i = 0;  i < size_; ++i)
          static_cast<T*> (data_)[i].~T ();

        size_ = 0;
      }

#ifdef XSDE_EXCEPTIONS
      template <typename T>
      void fix_seq<T>::
      move_ (void* dst, void* src, size_t n)
      {
        T* d = static_cast<T*> (dst);
        T* s = static_cast<T*> (src);

        // The copy c-tor can throw in which case we need to destroy
        // whatever objects we already copied into d.
        //
        size_t i = 0;
        guard g (d, i);

        for (; i < n; i++)
          new (d + i) T (s[i]);

        g.release ();

        for (size_t j = 0; j < n; j++)
          s[j].~T ();
      }
#else
      template <typename T>
      void fix_seq<T>::
      move_ (void* dst, void* src, size_t n)
      {
        T* d = static_cast<T*> (dst);
        T* s = static_cast<T*> (src);

        for (size_t i = 0; i < n; i++)
        {
          new (d + i) T (s[i]);
          s[i].~T ();
        }
      }
#endif

      template <typename T>
      void fix_seq<T>::
      move_forward_ (void* p, size_t n)
      {
        // We are moving a sequence of elements one position to the left.
        // The tricky part is to make sure we are in at least destructable
        // state if things turn bad. We assume that there is a valid
        // element at position p.
        //
        T* d = static_cast<T*> (p);

        for (size_t i = 0; i < n; i++)
          d[i] = d[i + 1];

        d[n].~T ();
      }

#ifdef XSDE_EXCEPTIONS
      template <typename T>
      void fix_seq<T>::
      move_backward_ (void* p, size_t n, size_t& size)
      {
        // We are moving a sequence of elements one position to the right.
        // The tricky part is to make sure we are in at least destructable
        // state if things turn bad.
        //
        T* d = static_cast<T*> (p);
        T* e = d + n;

        new (e) T;
        size++;

        for (size_t i = n; i > 0; i--)
          d[i] = d[i - 1];
      }
#else
      template <typename T>
      void fix_seq<T>::
      move_backward_ (void* p, size_t n)
      {
        // We are moving a sequence of elements one position to the right.
        //
        T* d = static_cast<T*> (p);
        T* e = d + n;

        new (e) T;

        for (size_t i = n; i > 0; i--)
          d[i] = d[i - 1];
      }
#endif

      //
      // var_seq
      //

      template <typename T>
      void var_seq<T>::
      clear ()
      {
        for (size_t i = 0;  i < size_; ++i)
          delete static_cast<T**> (data_)[i];

        size_ = 0;
      }
    }
  }
}
