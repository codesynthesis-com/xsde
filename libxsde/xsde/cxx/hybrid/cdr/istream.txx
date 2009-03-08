// file      : xsde/cxx/hybrid/cdr/istream.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
#ifdef XSDE_EXCEPTIONS

      template <typename T>
      void
      operator>> (icdrstream& s, pod_seq<T>& x)
      {
        size_t n;
        icdrstream::as_size as_size (n);
        s >> as_size;

        x.clear ();

        if (n > 0)
        {
          x.reserve (n);

          T i;
          while (n--)
          {
            s >> i;
            x.push_back (i);
          }
        }
      }

      template <typename T>
      void
      operator>> (icdrstream& s, fix_seq<T>& x)
      {
        size_t n;
        icdrstream::as_size as_size (n);
        s >> as_size;

        x.clear ();

        if (n > 0)
        {
          x.reserve (n);

          while (n--)
          {
            T i;
            s >> i;
            x.push_back (i);
          }
        }
      }

      template <typename T>
      void
      operator>> (icdrstream& s, var_seq<T>& x)
      {
        size_t n;
        icdrstream::as_size as_size (n);
        s >> as_size;

        x.clear ();

        if (n > 0)
        {
          x.reserve (n);

          while (n--)
          {
            T* p = new T;
            typename var_seq<T>::guard g (p);
            s >> *p;
            g.release ();
            x.push_back (p);
          }
        }
      }

#else // XSDE_EXCEPTIONS

      template <typename T>
      bool
      operator>> (icdrstream& s, pod_seq<T>& x)
      {
        size_t n;
        icdrstream::as_size as_size (n);

        if (!(s >> as_size))
          return false;

        x.clear ();

        if (n > 0)
        {
          if (x.reserve (n))
            return false;

          T i;
          while (n--)
          {
            if (!(s >> i) || x.push_back (i))
              return false;
          }
        }

        return true;
      }

      template <typename T>
      bool
      operator>> (icdrstream& s, fix_seq<T>& x)
      {
        size_t n;
        icdrstream::as_size as_size (n);

        if (!(s >> as_size))
          return false;

        x.clear ();

        if (n > 0)
        {
          if (x.reserve (n))
            return false;

          while (n--)
          {
            T i;
            if (!(s >> i) || x.push_back (i))
              return false;
          }
        }

        return true;
      }

      template <typename T>
      bool
      operator>> (icdrstream& s, var_seq<T>& x)
      {
        size_t n;
        icdrstream::as_size as_size (n);

        if (!(s >> as_size))
          return false;

        x.clear ();

        if (n > 0)
        {
          if (x.reserve (n))
            return false;

          while (n--)
          {
            T* p = new T;

            if (p == 0)
              return false;

            if (!(s >> *p))
            {
              delete p;
              return false;
            }

            if (x.push_back (p))
              return false;
          }
        }

        return true;
      }

#endif // XSDE_EXCEPTIONS
    }
  }
}
