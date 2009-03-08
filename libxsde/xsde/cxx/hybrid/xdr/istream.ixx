// file      : xsde/cxx/hybrid/xdr/istream.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
      inline ixdrstream::
      ixdrstream (XDR& xdr)
          : xdr_ (xdr)
      {
      }

      inline XDR& ixdrstream::
      impl ()
      {
        return xdr_;
      }

#ifdef XSDE_EXCEPTIONS

      inline void ixdrstream::
      operator>> (bool& x)
      {
        bool_t v;

        if (!xdr_bool (&xdr_, &v))
          throw xdr_exception ();

        x = static_cast<bool> (v);
      }

      inline void ixdrstream::
      operator>> (signed char& x)
      {
        int8_t v;

        if (!xdr_int8_t (&xdr_, &v))
          throw xdr_exception ();

        x = static_cast<signed char> (v);
      }


      inline void ixdrstream::
      operator>> (unsigned char& x)
      {
        uint8_t v;

        if (!xdr_uint8_t (&xdr_, &v))
          throw xdr_exception ();

        x = static_cast<unsigned char> (v);
      }

      inline void ixdrstream::
      operator>> (short& x)
      {
        int16_t v;

        if (!xdr_int16_t (&xdr_, &v))
          throw xdr_exception ();

        x = static_cast<short> (v);
      }

      inline void ixdrstream::
      operator>> (unsigned short& x)
      {
        uint16_t v;

        if (!xdr_uint16_t (&xdr_, &v))
          throw xdr_exception ();

        x = static_cast<unsigned short> (v);
      }

      inline void ixdrstream::
      operator>> (int& x)
      {
        int32_t v;

        if (!xdr_int32_t (&xdr_, &v))
          throw xdr_exception ();

        x = static_cast<int> (v);
      }

      inline void ixdrstream::
      operator>> (unsigned int& x)
      {
        uint32_t v;

        if (!xdr_uint32_t (&xdr_, &v))
          throw xdr_exception ();

        x = static_cast<unsigned int> (v);
      }

      inline void ixdrstream::
      operator>> (long& x)
      {
        int32_t v;

        if (!xdr_int32_t (&xdr_, &v))
          throw xdr_exception ();

        x = static_cast<long> (v);
      }

      inline void ixdrstream::
      operator>> (unsigned long& x)
      {
        uint32_t v;

        if (!xdr_uint32_t (&xdr_, &v))
          throw xdr_exception ();

        x = static_cast<unsigned long> (v);
      }

#ifdef XSDE_LONGLONG
      inline void ixdrstream::
      operator>> (long long& x)
      {
        int64_t v;

        if (!xdr_int64_t (&xdr_, &v))
          throw xdr_exception ();

        x = static_cast<long long> (v);
      }

      inline void ixdrstream::
      operator>> (unsigned long long& x)
      {
        uint64_t v;

        if (!xdr_uint64_t (&xdr_, &v))
          throw xdr_exception ();

        x = static_cast<unsigned long long> (v);
      }
#endif

      inline void ixdrstream::
      operator>> (as_size& x)
      {
        // Assume size is 32-bit.
        //
        uint32_t v;

        if (!xdr_uint32_t (&xdr_, &v))
          throw xdr_exception ();

        x.s_ = static_cast<size_t> (v);
      }

      inline void ixdrstream::
      operator>> (float& x)
      {
        if (!xdr_float (&xdr_, &x))
          throw xdr_exception ();
      }

      inline void ixdrstream::
      operator>> (double& x)
      {
        if (!xdr_double (&xdr_, &x))
          throw xdr_exception ();
      }

      inline void
      operator>> (ixdrstream&, any_type&)
      {
      }

      inline void
      operator>> (ixdrstream&, any_simple_type&)
      {
      }

#else // XSDE_EXCEPTIONS

      inline bool ixdrstream::
      operator>> (bool& x)
      {
        bool_t v;

        if (!xdr_bool (&xdr_, &v))
          return false;

        x = static_cast<bool> (v);
        return true;
      }

      inline bool ixdrstream::
      operator>> (signed char& x)
      {
        int8_t v;

        if (!xdr_int8_t (&xdr_, &v))
          return false;

        x = static_cast<signed char> (v);
        return true;
      }


      inline bool ixdrstream::
      operator>> (unsigned char& x)
      {
        uint8_t v;

        if (!xdr_uint8_t (&xdr_, &v))
          return false;

        x = static_cast<unsigned char> (v);
        return true;
      }

      inline bool ixdrstream::
      operator>> (short& x)
      {
        int16_t v;

        if (!xdr_int16_t (&xdr_, &v))
          return false;

        x = static_cast<short> (v);
        return true;
      }

      inline bool ixdrstream::
      operator>> (unsigned short& x)
      {
        uint16_t v;

        if (!xdr_uint16_t (&xdr_, &v))
          return false;

        x = static_cast<unsigned short> (v);
        return true;
      }

      inline bool ixdrstream::
      operator>> (int& x)
      {
        int32_t v;

        if (!xdr_int32_t (&xdr_, &v))
          return false;

        x = static_cast<int> (v);
        return true;
      }

      inline bool ixdrstream::
      operator>> (unsigned int& x)
      {
        uint32_t v;

        if (!xdr_uint32_t (&xdr_, &v))
          return false;

        x = static_cast<unsigned int> (v);
        return true;
      }

      inline bool ixdrstream::
      operator>> (long& x)
      {
        int32_t v;

        if (!xdr_int32_t (&xdr_, &v))
          return false;

        x = static_cast<long> (v);
        return true;
      }

      inline bool ixdrstream::
      operator>> (unsigned long& x)
      {
        uint32_t v;

        if (!xdr_uint32_t (&xdr_, &v))
          return false;

        x = static_cast<unsigned long> (v);
        return true;
      }

#ifdef XSDE_LONGLONG
      inline bool ixdrstream::
      operator>> (long long& x)
      {
        int64_t v;

        if (!xdr_int64_t (&xdr_, &v))
          return false;

        x = static_cast<long long> (v);
        return true;
      }

      inline bool ixdrstream::
      operator>> (unsigned long long& x)
      {
        uint64_t v;

        if (!xdr_uint64_t (&xdr_, &v))
          return false;

        x = static_cast<unsigned long long> (v);
        return true;
      }
#endif

      inline bool ixdrstream::
      operator>> (as_size& x)
      {
        // Assume size is 32-bit.
        //
        uint32_t v;

        if (!xdr_uint32_t (&xdr_, &v))
          return false;

        x.s_ = static_cast<size_t> (v);
        return true;
      }

      inline bool ixdrstream::
      operator>> (float& x)
      {
        return xdr_float (&xdr_, &x);
      }

      inline bool ixdrstream::
      operator>> (double& x)
      {
        return xdr_double (&xdr_, &x);
      }

      inline bool
      operator>> (ixdrstream&, any_type&)
      {
        return true;
      }

      inline bool
      operator>> (ixdrstream&, any_simple_type&)
      {
        return true;
      }

#endif // XSDE_EXCEPTIONS
    }
  }
}
