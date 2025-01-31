// file      : xsde/cxx/qname-stl.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_QNAME_STL_HXX
#define XSDE_CXX_QNAME_STL_HXX

#include <string>

namespace xsde
{
  namespace cxx
  {
    class qname
    {
    public:
      qname ();
      explicit
      qname (const std::string& name);
      qname (const std::string& prefix, const std::string& name);

      void
      swap (qname&);

      const std::string&
      prefix () const;

      std::string&
      prefix ();

      void
      prefix (const std::string&);

      const std::string&
      name () const;

      std::string&
      name ();

      void
      name (const std::string&);

    private:
      std::string prefix_;
      std::string name_;
    };

    bool
    operator== (const qname&, const qname&);

    bool
    operator!= (const qname&, const qname&);
  }
}

#include <xsde/cxx/qname-stl.ixx>

#endif  // XSDE_CXX_QNAME_STL_HXX
