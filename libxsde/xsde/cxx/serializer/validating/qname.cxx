// file      : xsde/cxx/serializer/validating/qname.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <string.h> // strlen

#include <xsde/cxx/xml/ncname.hxx>

#include <xsde/cxx/serializer/validating/qname.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        qname_simpl::
        ~qname_simpl ()
        {
          if (free_)
            delete const_cast<qname*> (value_);
        }

        void qname_simpl::
        pre (const qname* value)
        {
          value_ = value;
        }

        void qname_simpl::
        _serialize_content ()
        {
          const char* ns = value_->name ();
          size_t nn = strlen (ns);

          const char* ps = value_->prefix ();

          if (ps != 0 && *ps != '\0')
          {
            size_t pn = strlen (ps);

            if (xml::valid_ncname (ps, pn) && xml::valid_ncname (ns, nn))
            {
#ifdef XSDE_EXCEPTIONS
              _characters (ps, pn);
              _characters (":", 1);
              _characters (ns, nn);
#else
              if (_characters (ps, pn))
              {
                if (_characters (":", 1))
                  _characters (ns, nn);
              }
#endif
            }
            else
              _schema_error (schema_error::invalid_qname_value);
          }
          else
          {
            if (xml::valid_ncname (ns, nn))
              _characters (ns, nn);
            else
              _schema_error (schema_error::invalid_qname_value);
          }

          if (free_)
          {
            delete const_cast<qname*> (value_);
            value_ = 0;
          }
        }
      }
    }
  }
}
