// file      : xsde/cxx/hybrid/xdr/ostream.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
      inline oxdrstream::
      oxdrstream (XDR& xdr)
          : xdr_ (xdr)
      {
      }

      inline XDR& oxdrstream::
      impl ()
      {
        return xdr_;
      }

#ifdef XSDE_EXCEPTIONS

      inline void oxdrstream::
      operator<< (bool x)
      {
        bool_t v = static_cast<bool_t> (x);

        if (!xdr_bool (&xdr_, &v))
          throw xdr_exception ();
      }

      inline void oxdrstream::
      operator<< (signed char x)
      {
        int8_t v = static_cast<int8_t> (x);

        if (!xdr_int8_t (&xdr_, &v))
          throw xdr_exception ();
      }


      inline void oxdrstream::
      operator<< (unsigned char x)
      {
        uint8_t v = static_cast<uint8_t> (x);

        if (!xdr_uint8_t (&xdr_, &v))
          throw xdr_exception ();
      }

      inline void oxdrstream::
      operator<< (short x)
      {
        int16_t v = static_cast<int16_t> (x);

        if (!xdr_int16_t (&xdr_, &v))
          throw xdr_exception ();
      }

      inline void oxdrstream::
      operator<< (unsigned short x)
      {
        uint16_t v = static_cast<uint16_t> (x);

        if (!xdr_uint16_t (&xdr_, &v))
          throw xdr_exception ();
      }

      inline void oxdrstream::
      operator<< (int x)
      {
        int32_t v = static_cast<int32_t> (x);

        if (!xdr_int32_t (&xdr_, &v))
          throw xdr_exception ();
      }

      inline void oxdrstream::
      operator<< (unsigned int x)
      {
        uint32_t v = static_cast<uint32_t> (x);

        if (!xdr_uint32_t (&xdr_, &v))
          throw xdr_exception ();
      }

      inline void oxdrstream::
      operator<< (long x)
      {
        int32_t v = static_cast<int32_t> (x);

        if (!xdr_int32_t (&xdr_, &v))
          throw xdr_exception ();
      }

      inline void oxdrstream::
      operator<< (unsigned long x)
      {
        uint32_t v = static_cast<uint32_t> (x);

        if (!xdr_uint32_t (&xdr_, &v))
          throw xdr_exception ();
      }

#ifdef XSDE_LONGLONG
      inline void oxdrstream::
      operator<< (long long x)
      {
        int64_t v = static_cast<int64_t> (x);

        if (!xdr_int64_t (&xdr_, &v))
          throw xdr_exception ();
      }

      inline void oxdrstream::
      operator<< (unsigned long long x)
      {
        uint64_t v = static_cast<uint64_t> (x);

        if (!xdr_uint64_t (&xdr_, &v))
          throw xdr_exception ();
      }
#endif

      inline void oxdrstream::
      operator<< (as_size x)
      {
        // Assume size is 32-bit.
        //
        uint32_t v = static_cast<uint32_t> (x.s_);

        if (!xdr_uint32_t (&xdr_, &v))
          throw xdr_exception ();
      }

      inline void oxdrstream::
      operator<< (float x)
      {
        if (!xdr_float (&xdr_, &x))
          throw xdr_exception ();
      }

      inline void oxdrstream::
      operator<< (double x)
      {
        if (!xdr_double (&xdr_, &x))
          throw xdr_exception ();
      }

      inline void
      operator<< (oxdrstream&, const any_type&)
      {
      }

      inline void
      operator<< (oxdrstream&, const any_simple_type&)
      {
      }

#else // XSDE_EXCEPTIONS

      inline bool oxdrstream::
      operator<< (bool x)
      {
        bool_t v = static_cast<bool_t> (x);
        return xdr_bool (&xdr_, &v);
      }

      inline bool oxdrstream::
      operator<< (signed char x)
      {
        int8_t v = static_cast<int8_t> (x);
        return xdr_int8_t (&xdr_, &v);
      }


      inline bool oxdrstream::
      operator<< (unsigned char x)
      {
        uint8_t v = static_cast<uint8_t> (x);
        return xdr_uint8_t (&xdr_, &v);
      }

      inline bool oxdrstream::
      operator<< (short x)
      {
        int16_t v = static_cast<int16_t> (x);
        return xdr_int16_t (&xdr_, &v);
      }

      inline bool oxdrstream::
      operator<< (unsigned short x)
      {
        uint16_t v = static_cast<uint16_t> (x);
        return xdr_uint16_t (&xdr_, &v);
      }

      inline bool oxdrstream::
      operator<< (int x)
      {
        int32_t v = static_cast<int32_t> (x);
        return xdr_int32_t (&xdr_, &v);
      }

      inline bool oxdrstream::
      operator<< (unsigned int x)
      {
        uint32_t v = static_cast<uint32_t> (x);
        return xdr_uint32_t (&xdr_, &v);
      }

      inline bool oxdrstream::
      operator<< (long x)
      {
        int32_t v = static_cast<int32_t> (x);
        return xdr_int32_t (&xdr_, &v);
      }

      inline bool oxdrstream::
      operator<< (unsigned long x)
      {
        uint32_t v = static_cast<uint32_t> (x);
        return xdr_uint32_t (&xdr_, &v);
      }

#ifdef XSDE_LONGLONG
      inline bool oxdrstream::
      operator<< (long long x)
      {
        int64_t v = static_cast<int64_t> (x);
        return xdr_int64_t (&xdr_, &v);
      }

      inline bool oxdrstream::
      operator<< (unsigned long long x)
      {
        uint64_t v = static_cast<uint64_t> (x);
        return xdr_uint64_t (&xdr_, &v);
      }
#endif

      inline bool oxdrstream::
      operator<< (as_size x)
      {
        // Assume size is 32-bit.
        //
        uint32_t v = static_cast<uint32_t> (x.s_);
        return xdr_uint32_t (&xdr_, &v);
      }

      inline bool oxdrstream::
      operator<< (float x)
      {
        return xdr_float (&xdr_, &x);
      }

      inline bool oxdrstream::
      operator<< (double x)
      {
        return xdr_double (&xdr_, &x);
      }

      inline bool
      operator<< (oxdrstream&, const any_type&)
      {
        return true;
      }

      inline bool
      operator<< (oxdrstream&, const any_simple_type&)
      {
        return true;
      }

#endif // XSDE_EXCEPTIONS
    }
  }
}
