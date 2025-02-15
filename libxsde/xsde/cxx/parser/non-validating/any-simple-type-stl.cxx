// file      : xsde/cxx/parser/non-validating/any-simple-type-stl.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/parser/non-validating/any-simple-type-stl.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace non_validating
      {
        void any_simple_type_pimpl::
        _pre ()
        {
          str_.erase ();
        }

        void any_simple_type_pimpl::
        _characters (const ro_string& s)
        {
          str_ += s;
        }

        std::string any_simple_type_pimpl::
        post_any_simple_type ()
        {
          std::string r;
          r.swap (str_);
          return r;
        }
      }
    }
  }
}
