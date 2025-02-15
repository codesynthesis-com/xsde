// file      : xsde/cxx/parser/non-validating/parser.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <assert.h>

#include <xsde/cxx/parser/non-validating/parser.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace non_validating
      {
        // empty_content
        //

#ifdef XSDE_POLYMORPHIC
        bool empty_content::
        _start_element_impl (const ro_string&,
                             const ro_string&,
                             const char*)
        {
          return false;
        }
#else
        bool empty_content::
        _start_element_impl (const ro_string&,
                             const ro_string&)
        {
          return false;
        }
#endif

        bool empty_content::
        _end_element_impl (const ro_string&,
                           const ro_string&)
        {
          return false;
        }

        bool empty_content::
        _attribute_impl (const ro_string&,
                         const ro_string&,
                         const ro_string&)
        {
          return false;
        }

        bool empty_content::
        _characters_impl (const ro_string&)
        {
          return false;
        }

        //
        //
#ifdef XSDE_POLYMORPHIC
        void empty_content::
        _start_element (const ro_string& ns,
                        const ro_string& name,
                        const char* type)
        {
          if (!_start_element_impl (ns, name, type))
            _start_any_element (ns, name, type);
        }
#else
        void empty_content::
        _start_element (const ro_string& ns,
                        const ro_string& name)
        {
          if (!_start_element_impl (ns, name))
            _start_any_element (ns, name);
        }
#endif

        void empty_content::
        _end_element (const ro_string& ns,
                      const ro_string& name)
        {
          if (!_end_element_impl (ns, name))
            _end_any_element (ns, name);
        }

        static const char
        xmlns_namespace_[] = "http://www.w3.org/2000/xmlns/";

        static const char
        xsi_namespace_[] = "http://www.w3.org/2001/XMLSchema-instance";

        static const char
        type_[] = "type";

        static const char
        nil_[] = "nil";

        static const char
        schema_location_[] = "schemaLocation";

        static const char
        no_namespace_schema_location_[] = "noNamespaceSchemaLocation";

        void empty_content::
        _attribute (const ro_string& ns,
                    const ro_string& name,
                    const ro_string& value)
        {
          // Weed out special attributes: xsi:type, xsi:nil,
          // xsi:schemaLocation and noNamespaceSchemaLocation.
          // See section 3.2.7 in Structures for details.
          //
          if (ns == xsi_namespace_ &&
              (name == schema_location_ ||
               name == no_namespace_schema_location_ ||
               name == type_ ||
               name == nil_))
            return;

          // Also some parsers supply us with namespace-prefix
          // mapping attributes.
          //
          if (ns == xmlns_namespace_)
            return;

          if (!_attribute_impl (ns, name, value))
            _any_attribute (ns, name, value);
        }

        void empty_content::
        _characters (const ro_string& s)
        {
#ifdef XSDE_REUSE_STYLE_TIEIN
          if (impl_)
            impl_->_characters (s);
          else
#endif
            if (!_characters_impl (s))
              _any_characters (s);
        }


        // simple_content
        //

        void simple_content::
        _attribute (const ro_string& ns,
                    const ro_string& name,
                    const ro_string& value)
        {
          // Weed out special attributes: xsi:type, xsi:nil,
          // xsi:schemaLocation and noNamespaceSchemaLocation.
          // See section 3.2.7 in Structures for details.
          //
          if (ns == xsi_namespace_ &&
              (name == schema_location_ ||
               name == no_namespace_schema_location_ ||
               name == type_ ||
               name == nil_))
            return;

          // Also some parsers supply us with namespace-prefix
          // mapping attributes.
          //
          if (ns == xmlns_namespace_)
            return;

          if (!_attribute_impl (ns, name, value))
            _any_attribute (ns, name, value);
        }

        void simple_content::
        _characters (const ro_string& s)
        {
#ifdef XSDE_REUSE_STYLE_TIEIN
          if (impl_)
            impl_->_characters (s);
          else
#endif
            if (!_characters_impl (s))
              _any_characters (s);
        }


        // complex_content
        //

#ifdef XSDE_POLYMORPHIC
        void complex_content::
        _start_element (const ro_string& ns,
                        const ro_string& name,
                        const char* type)
        {
          if (!_start_element_impl (ns, name, type))
          {
            _context ().start_wildcard_content ();
            _start_any_element (ns, name, type);
          }
        }
#else
        void complex_content::
        _start_element (const ro_string& ns,
                        const ro_string& name)
        {
          if (!_start_element_impl (ns, name))
          {
            _context ().start_wildcard_content ();
            _start_any_element (ns, name);
          }
        }
#endif

        void complex_content::
        _end_element (const ro_string& ns,
                      const ro_string& name)
        {
          if (!_end_element_impl (ns, name))
            _end_any_element (ns, name);
        }

        void complex_content::
        _attribute (const ro_string& ns,
                    const ro_string& name,
                    const ro_string& value)
        {
          // Weed out special attributes: xsi:type, xsi:nil,
          // xsi:schemaLocation and noNamespaceSchemaLocation.
          // See section 3.2.7 in Structures for details.
          //
          if (ns == xsi_namespace_ &&
              (name == schema_location_ ||
               name == no_namespace_schema_location_ ||
               name == type_ ||
               name == nil_))
            return;

          // Also some parsers supply us with namespace-prefix
          // mapping attributes.
          //
          if (ns == xmlns_namespace_)
            return;

          if (!_attribute_impl (ns, name, value))
            _any_attribute (ns, name, value);
        }

        void complex_content::
        _characters (const ro_string& s)
        {
#ifdef XSDE_REUSE_STYLE_TIEIN
          if (impl_)
            impl_->_characters (s);
          else
#endif
            if (!_characters_impl (s))
              _any_characters (s);
        }

        void complex_content::
        _pre_impl (context& c)
        {
#ifdef XSDE_REUSE_STYLE_TIEIN
          assert (parent_ == 0);

          // Set the parent_ pointers in the tied-in implementations.
          //
          _set_parent_chain ();
#endif
          if (depth_++ == 0)
            context_ = &c;

#ifdef XSDE_EXCEPTIONS
          parser_stack_.push (c.current_);
#else
          if (parser_stack_.push (c.current_))
          {
            _sys_error (sys_error::no_memory);
            return;
          }
#endif
          c.current_.parser_ = this;
          c.current_.depth_ = 0;
          c.current_.any_ = false;

          _pre ();
        }

        void complex_content::
        _post_impl ()
        {
#ifndef XSDE_EXCEPTIONS
          if (!context_->error_type ())
#endif
            _post ();

          context_->current_ = parser_stack_.top ();
          parser_stack_.pop ();

          if (--depth_ == 0)
            context_ = 0;
        }

        void complex_content::
        _reset ()
        {
          empty_content::_reset ();

          depth_ = 0;
          parser_stack_.clear ();
        }

        // list_base
        //

        // Find first non-space character.
        //
        static ro_string::size_type
        find_ns (const char* s,
                 ro_string::size_type size,
                 ro_string::size_type pos)
        {
          while (pos < size &&
                 (s[pos] == 0x20 || s[pos] == 0x0A ||
                  s[pos] == 0x0D || s[pos] == 0x09))
            ++pos;

          return pos < size ? pos : ro_string::npos;
        }

        // Find first space character.
        //
        static ro_string::size_type
        find_s (const char* s,
                ro_string::size_type size,
                ro_string::size_type pos)
        {
          while (pos < size &&
                 s[pos] != 0x20 && s[pos] != 0x0A &&
                 s[pos] != 0x0D && s[pos] != 0x09)
            ++pos;

          return pos < size ? pos : ro_string::npos;
        }

        // Relevant XML Schema Part 2: Datatypes sections: 4.2.1.2, 4.3.6.
        //

        void list_base::
        _pre_impl (context& c)
        {
          simple_content::_pre_impl (c);

#ifdef XSDE_EXCEPTIONS
          buf_.assign ("", 0);
#else
          if (!c.error_type ())
          {
            if (buf_.assign ("", 0))
              _sys_error (sys_error::no_memory);
          }
#endif
        }

        void list_base::
        _characters (const ro_string& s)
        {
          typedef ro_string::size_type size_type;

          const char* data = s.data ();
          size_type size = s.size ();

          // Handle the previous chunk if we start with a ws.
          //
          if (!buf_.empty () &&
              (data[0] == 0x20 || data[0] == 0x0A ||
               data[0] == 0x0D || data[0] == 0x09))
          {
            ro_string tmp (buf_); // Private copy ctor.
            _xsde_parse_item (tmp);
            buf_.assign ("", 0); // Can't fail.
          }

#ifndef XSDE_EXCEPTIONS
          context& ctx = _context ();
#endif

          // Traverse the data while logically collapsing spaces.
          //
          for (size_type i = find_ns (data, size, 0); i != ro_string::npos;)
          {
            size_type j = find_s (data, size, i);

            if (j != ro_string::npos)
            {
              if (buf_.empty ())
              {
                ro_string tmp (data + i, j - i); // Private copy ctor.
                _xsde_parse_item (tmp);
              }
              else
              {
                // Assemble the first item in str from buf_ and s.
                //
                string str;
                str.swap (buf_);

#ifdef XSDE_EXCEPTIONS
                str.append (data + i, j - i);
#else
                if (str.append (data + i, j - i))
                {
                  _sys_error (sys_error::no_memory);
                  break;
                }
#endif
                ro_string tmp (str); // Private copy ctor.
                _xsde_parse_item (tmp);
              }

#ifndef XSDE_EXCEPTIONS
              if (ctx.error_type ())
                break;
#endif

              i = find_ns (data, size, j);
            }
            else
            {
              // Last fragment, append it to buf_.
              //
#ifdef XSDE_EXCEPTIONS
              buf_.append (data + i, size - i);
#else
              if (buf_.append (data + i, size - i))
                _sys_error (sys_error::no_memory);
#endif
              break;
            }
          }
        }

        void list_base::
        _xsde_parse_item (const ro_string&)
        {
        }

        void list_base::
        _post_impl ()
        {
          // Handle the last item.
          //
          if (!buf_.empty ())
          {
            ro_string tmp (buf_); // Private copy ctor.
            _xsde_parse_item (tmp);
          }

          simple_content::_post_impl ();
        }
      }
    }
  }
}
