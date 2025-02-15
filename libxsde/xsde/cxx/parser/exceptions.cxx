// file      : xsde/cxx/parser/exceptions.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/config.hxx>

#ifdef XSDE_IOSTREAM
#  include <iostream>
#endif

#include <xsde/cxx/parser/exceptions.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      // exception
      //

#ifdef XSDE_IOSTREAM
      std::ostream&
      operator<< (std::ostream& os, const exception& e)
      {
        return os << e.text ();
      }
#endif


      // xml
      //
      const char* xml::
      text () const
      {
        return expat::xml_error_text (code_);
      }

      const char* xml::
      what () const throw ()
      {
        return "xml error";
      }


      // schema
      //
#ifdef XSDE_PARSER_VALIDATION
      const char* schema::
      text () const
      {
        return schema_error::text (code_);
      }

      const char* schema::
      what () const throw ()
      {
        return "schema error";
      }
#endif
    }
  }
}
