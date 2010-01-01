// file      : xsde/cxx/parser/validating/id.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/config.hxx>

#include <xsde/cxx/xml/ncname.hxx>

#include <xsde/cxx/parser/validating/id.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        void id_pimpl::
        _pre ()
        {
#ifdef XSDE_EXCEPTIONS
          str_.assign ("", 0);
#else
          if (str_.assign ("", 0))
            _sys_error (sys_error::no_memory);
#endif
        }

        void id_pimpl::
        _characters (const ro_string& s)
        {
          if (str_.size () == 0)
          {
            ro_string tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
            {
#ifdef XSDE_EXCEPTIONS
              str_.append (tmp.data (), tmp.size ());
#else
              if (str_.append (tmp.data (), tmp.size ()))
                _sys_error (sys_error::no_memory);
#endif
            }
          }
          else
          {
#ifdef XSDE_EXCEPTIONS
            str_.append (s.data (), s.size ());
#else
            if (str_.append (s.data (), s.size ()))
              _sys_error (sys_error::no_memory);
#endif
          }
        }

        void id_pimpl::
        _post ()
        {
          ro_string tmp (str_.data (), str_.size ());
          ro_string::size_type size = trim_right (tmp);

          if (xml::valid_ncname (tmp.data (), size))
            str_.truncate (size);
          else
            _schema_error (schema_error::invalid_id_value);
        }

        char* id_pimpl::
        post_id ()
        {
          return str_.detach ();
        }
      }
    }
  }
}

