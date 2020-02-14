// file      : xsde/cxx/parser/non-validating/nmtoken-stl.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/parser/non-validating/nmtoken-stl.hxx>
#include <xsde/cxx/parser/non-validating/string-common.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace non_validating
      {
        void nmtoken_pimpl::
        _pre ()
        {
          str_.erase ();
        }

        void nmtoken_pimpl::
        _characters (const ro_string& s)
        {
          if (str_.size () == 0)
          {
            ro_string tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        std::string nmtoken_pimpl::
        post_nmtoken ()
        {
          string_common::process_facets (str_, _facets ());

          std::string r;
          r.swap (str_);
          return r;
        }
      }
    }
  }
}
