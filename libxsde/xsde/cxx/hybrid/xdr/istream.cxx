// file      : xsde/cxx/hybrid/xdr/istream.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/hybrid/xdr/istream.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
#ifdef XSDE_EXCEPTIONS

#ifdef XSDE_STL
      void ixdrstream::
      operator>> (std::string& x)
      {
        unsigned int n;

        if (!xdr_u_int (&xdr_, &n))
          throw xdr_exception ();

        x.clear ();
	
	if (n != 0)
	{
          x.resize (n);
          char* p = const_cast<char*> (x.c_str ());

          if (!xdr_opaque (&xdr_, p, n))
            throw xdr_exception ();
        }
      }
#else
      void ixdrstream::
      operator>> (char*& x)
      {
        unsigned int n;

        if (!xdr_u_int (&xdr_, &n))
          throw xdr_exception ();

        x = new char[n + 1];

        if (!xdr_opaque (&xdr_, x, n))
        {
          delete[] x;
          throw xdr_exception ();
        }

        x[n] = '\0';
      }
#endif

      void ixdrstream::
      operator>> (buffer& x)
      {
        unsigned int n;

        if (!xdr_u_int (&xdr_, &n))
          throw xdr_exception ();

        x.size (n);

        if (!xdr_opaque (&xdr_, x.data (), n))
          throw xdr_exception ();
      }

#else // XSDE_EXCEPTIONS

#ifdef XSDE_STL
      bool ixdrstream::
      operator>> (std::string& x)
      {
        unsigned int n;

        if (!xdr_u_int (&xdr_, &n))
          return false;

        x.clear ();
	
	if (n != 0)
	{
          x.resize (n);
          char* p = const_cast<char*> (x.c_str ());
          return xdr_opaque (&xdr_, p, n);
	}
	
	return true;
      }
#else
      bool ixdrstream::
      operator>> (char*& x)
      {
        unsigned int n;

        if (!xdr_u_int (&xdr_, &n))
          return false;

        x = new char[n + 1];

        if (x == 0)
          return false;

        if (!xdr_opaque (&xdr_, x, n))
        {
          delete[] x;
          return false;
        }

        x[n] = '\0';

        return true;
      }
#endif

      bool ixdrstream::
      operator>> (buffer& x)
      {
        unsigned int n;

        if (!xdr_u_int (&xdr_, &n))
          return false;

        x.size (n);
        return xdr_opaque (&xdr_, x.data (), n);
      }

#endif // XSDE_EXCEPTIONS

    }
  }
}
