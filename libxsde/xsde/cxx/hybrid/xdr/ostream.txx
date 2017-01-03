// file      : xsde/cxx/hybrid/xdr/ostream.txx
// copyright : Copyright (c) 2005-2017 Code Synthesis Tools CC
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
      operator<< (oxdrstream& s, const pod_sequence<T>& x)
      {
        s << oxdrstream::as_size (x.size ());

        for (typename pod_sequence<T>::const_iterator i = x.begin ();
             i != x.end (); ++i)
        {
          s << *i;
        }
      }

      template <typename T>
      void
      operator<< (oxdrstream& s, const fix_sequence<T>& x)
      {
        s << oxdrstream::as_size (x.size ());

        for (typename fix_sequence<T>::const_iterator i = x.begin ();
             i != x.end (); ++i)
        {
          s << *i;
        }
      }

      template <typename T>
      void
      operator<< (oxdrstream& s, const var_sequence<T>& x)
      {
        s << oxdrstream::as_size (x.size ());

        for (typename var_sequence<T>::const_iterator i = x.begin ();
             i != x.end (); ++i)
        {
          s << *i;
        }
      }

#else // XSDE_EXCEPTIONS

      template <typename T>
      bool
      operator<< (oxdrstream& s, const pod_sequence<T>& x)
      {
        if (!(s << oxdrstream::as_size (x.size ())))
          return false;

        for (typename pod_sequence<T>::const_iterator i = x.begin ();
             i != x.end (); ++i)
        {
          if (!(s << *i))
            return false;
        }

        return true;
      }

      template <typename T>
      bool
      operator<< (oxdrstream& s, const fix_sequence<T>& x)
      {
        if (!(s << oxdrstream::as_size (x.size ())))
          return false;

        for (typename fix_sequence<T>::const_iterator i = x.begin ();
             i != x.end (); ++i)
        {
          if (!(s << *i))
            return false;
        }

        return true;
      }

      template <typename T>
      bool
      operator<< (oxdrstream& s, const var_sequence<T>& x)
      {
        if (!(s << oxdrstream::as_size (x.size ())))
          return false;

        for (typename var_sequence<T>::const_iterator i = x.begin ();
             i != x.end (); ++i)
        {
          if (!(s << *i))
            return false;
        }

        return true;
      }

#endif // XSDE_EXCEPTIONS
    }
  }
}
