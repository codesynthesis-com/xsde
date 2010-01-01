// file      : xsde/cxx/serializer/non-validating/qname.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/non-validating/qname.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace non_validating
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
          const char* p = value_->prefix ();

          if (p != 0 && *p != '\0')
          {
#ifdef XSDE_EXCEPTIONS
            _characters (p);
            _characters (":", 1);
            _characters (value_->name ());
#else
            if (_characters (p))
            {
              if (_characters (":", 1))
                _characters (value_->name ());
            }
#endif
          }
          else
          {
            _characters (value_->name ());
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
