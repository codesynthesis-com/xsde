// file      : xsde/cxx/hybrid/xdr/qname.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/hybrid/xdr/istream.hxx>
#include <xsde/cxx/hybrid/xdr/ostream.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
#ifdef XSDE_EXCEPTIONS

      void
      operator<< (oxdrstream& s, const qname& x)
      {
        s << x.prefix ();
        s << x.name ();
      }

      void
      operator>> (ixdrstream& s, qname& x)
      {

#ifdef XSDE_STL
        std::string p, n;
#else
        char* p;
        char* n;
#endif
        s >> p;
        x.prefix (p);

        s >> n; // x will free p in case extraction fails
        x.name (n);
      }

#else // XSDE_EXCEPTIONS

      bool
      operator<< (oxdrstream& s, const qname& x)
      {
        return s << x.prefix () && s << x.name ();
      }

      bool
      operator>> (ixdrstream& s, qname& x)
      {

#ifdef XSDE_STL
        std::string p, n;
#else
        char* p;
        char* n;
#endif
        if (!(s >> p))
          return false;

        x.prefix (p);

        if (!(s >> n))
          return false; // x will free p

        x.name (n);
        return true;
      }

#endif // XSDE_EXCEPTIONS
    }
  }
}
