// file      : xsde/cxx/parser/non-validating/idref-stl.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/parser/non-validating/idref-stl.hxx>
#include <xsde/cxx/parser/non-validating/string-common.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace non_validating
      {
        void idref_pimpl::
        _pre ()
        {
          str_.erase ();
        }

        void idref_pimpl::
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

        std::string idref_pimpl::
        post_idref ()
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
