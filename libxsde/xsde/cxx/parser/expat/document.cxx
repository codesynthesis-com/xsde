// file      : xsde/cxx/parser/expat/document.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/config.hxx>

#include <assert.h>
#include <string.h> // strchr, strlen

#ifdef XSDE_EXCEPTIONS
#  include <new> // std::bad_alloc
#endif

#ifdef XSDE_IOSTREAM
#  include <iostream>
#  include <fstream>
#endif

#ifdef XSDE_EXCEPTIONS
#  include <xsde/cxx/parser/exceptions.hxx>
#endif

#ifdef XSDE_POLYMORPHIC
#  include <xsde/cxx/parser/substitution-map.hxx>
#endif

#include <xsde/cxx/parser/expat/document.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace parser
    {
      namespace expat
      {
#ifdef XSDE_POLYMORPHIC
        static substitution_map_init substitution_map_init_;
#endif

        // document_pimpl
        //

	document_pimpl::
	~document_pimpl ()
	{
	  // Prevents auto-generation of this dtor.
        }

#ifdef XSDE_STL

        document_pimpl::
#ifdef XSDE_POLYMORPHIC
        document_pimpl (parser_base& p, const std::string& n, bool poly)
#else
        document_pimpl (parser_base& p, const std::string& n)
#endif
            : first_ (true), xml_parser_ (0), context_ (0), parser_ (&p)
        {
#ifdef XSDE_POLYMORPHIC
          polymorphic_ = poly;
#endif
          init_root_name (0, n.c_str ());
        }

        document_pimpl::
#ifdef XSDE_POLYMORPHIC
        document_pimpl (parser_base& p,
                        const std::string& ns,
                        const std::string& n,
                        bool poly)
#else
        document_pimpl (parser_base& p,
                        const std::string& ns,
                        const std::string& n)
#endif
            : first_ (true), xml_parser_ (0), context_ (0), parser_ (&p)
        {
#ifdef XSDE_POLYMORPHIC
          polymorphic_ = poly;
#endif
          init_root_name (ns.c_str (), n.c_str ());
        }

#endif // XSDE_STL

        document_pimpl::
#ifdef XSDE_POLYMORPHIC
        document_pimpl (parser_base& p, const char* n, bool poly)
#else
        document_pimpl (parser_base& p, const char* n)
#endif
            : first_ (true), xml_parser_ (0), context_ (0), parser_ (&p)
        {
#ifdef XSDE_POLYMORPHIC
          polymorphic_ = poly;
#endif
          init_root_name (0, n);
        }

        document_pimpl::
#ifdef XSDE_POLYMORPHIC
        document_pimpl (parser_base& p,
                        const char* ns,
                        const char* n,
                        bool poly)
#else
        document_pimpl (parser_base& p, const char* ns, const char* n)
#endif
            : first_ (true), xml_parser_ (0), context_ (0), parser_ (&p)
        {
#ifdef XSDE_POLYMORPHIC
          polymorphic_ = poly;
#endif
          init_root_name (ns, n);
        }

        document_pimpl::
        document_pimpl ()
            : first_ (true), xml_parser_ (0), context_ (0), parser_ (0)
        {
#ifdef XSDE_POLYMORPHIC
          polymorphic_ = false;
#endif
        }

#ifdef XSDE_POLYMORPHIC
        document_pimpl::
        document_pimpl (const char* n)
            : first_ (true), xml_parser_ (0), context_ (0), parser_ (0),
              polymorphic_ (true)
        {
          init_root_name (0, n);
        }

        document_pimpl::
        document_pimpl (const char* ns, const char* n)
            : first_ (true), xml_parser_ (0), context_ (0), parser_ (0),
              polymorphic_ (true)
        {
          init_root_name (ns, n);
        }

#ifdef XSDE_STL
        document_pimpl::
        document_pimpl (const std::string& n)
            : first_ (true), xml_parser_ (0), context_ (0), parser_ (0),
              polymorphic_ (true)
        {
          init_root_name (0, n.c_str ());
        }

        document_pimpl::
        document_pimpl (const std::string& ns, const std::string& n)
            : first_ (true), xml_parser_ (0), context_ (0), parser_ (0),
              polymorphic_ (true)
        {
          init_root_name (ns.c_str (), n.c_str ());
        }
#endif // XSDE_STL
#endif // XSDE_POLYMORPHIC

        void document_pimpl::
        init_root_name (const char* ns, const char* name)
        {
#ifdef XSDE_EXCEPTIONS
          root_ns_.assign (ns ? ns : "");
          root_name_.assign (name);
#else
          if (root_ns_.assign (ns ? ns : "") || root_name_.assign (name))
            error_ = error (sys_error::no_memory);
#endif
        }

        //
        //
        parser_base* document_pimpl::
#ifdef XSDE_POLYMORPHIC
        start_root_element (const ro_string& ns,
                            const ro_string& name,
                            const char*)
#else
        start_root_element (const ro_string& ns, const ro_string& name)
#endif
        {
          if (root_name_ == name && root_ns_ == ns)
          {
            return parser_;
          }
          else
          {
#ifdef XSDE_PARSER_VALIDATION
            context_.schema_error (schema_error::unexpected_element);
#endif
            return 0;
          }
        }

        void document_pimpl::
        end_root_element (const ro_string&, const ro_string&, parser_base*)
        {
        }

        void document_pimpl::
        reset ()
        {
#ifndef XSDE_EXCEPTIONS
          error_ = error ();
#endif
          first_ = true;

          if (parser_)
            parser_->_reset ();
        }

        // file
        //

#ifdef XSDE_IOSTREAM
        void document_pimpl::
        parse (const char* file)
        {
          std::ifstream ifs;

#ifdef XSDE_EXCEPTIONS
          ifs.exceptions (std::ifstream::badbit | std::ifstream::failbit);
#endif

          ifs.open (file, std::ifstream::in | std::ifstream::binary);

#ifndef XSDE_EXCEPTIONS
          if (ifs.fail ())
          {
            error_ = error (sys_error::open_failed);
            return;
          }
#endif
          parse (ifs);
        }

#ifdef XSDE_STL
        void document_pimpl::
        parse (const std::string& file)
        {
          parse (file.c_str ());
        }
#endif

        // istream
        //

#ifdef XSDE_EXCEPTIONS
        namespace bits
        {
          struct stream_exception_controller
          {
            ~stream_exception_controller ()
            {
              std::istream::iostate s = is_.rdstate ();
              s &= ~std::istream::failbit;

              // If our error state (sans failbit) intersects with the
              // exception state then that means we have an active
              // exception and changing error/exception state will
              // cause another to be thrown.
              //
              if (!(old_state_ & s))
              {
                // Clear failbit if it was caused by eof.
                //
                if (is_.fail () && is_.eof ())
                  is_.clear (s);

                is_.exceptions (old_state_);
              }
            }

            stream_exception_controller (std::istream& is)
                : is_ (is), old_state_ (is_.exceptions ())
            {
              is_.exceptions (old_state_ & ~std::istream::failbit);
            }

          private:
            stream_exception_controller (const stream_exception_controller&);

            stream_exception_controller&
            operator= (const stream_exception_controller&);

          private:
            std::istream& is_;
            std::istream::iostate old_state_;
          };
        }
#endif

        void document_pimpl::
        parse (std::istream& is)
        {
#ifdef XSDE_EXCEPTIONS
          // Temporarily unset the exception failbit. Also clear the
          // fail bit when we reset the old state if it was caused
          // by eof.
          //
          bits::stream_exception_controller sec (is);
#endif

          char buf[4096]; // Page size.

          do
          {
            is.read (buf, sizeof (buf));

#ifndef XSDE_EXCEPTIONS
            if (is.bad () || (is.fail () && !is.eof ()))
            {
              error_ = error (sys_error::read_failed);
              break;
            }
#endif
            parse (buf, is.gcount (), is.eof ());

#ifndef XSDE_EXCEPTIONS
            if (error_)
              break;
#endif
          } while (!is.eof ());
        }
#endif // XSDE_IOSTREAM


        void document_pimpl::
        parse (const void* data, size_t size, bool last)
        {
          // First call.
          //
          if (first_)
          {
            if (auto_xml_parser_ == 0)
            {
              auto_xml_parser_ = XML_ParserCreateNS (0, XML_Char (' '));

              if (auto_xml_parser_ == 0)
              {
#ifdef XSDE_EXCEPTIONS
                throw std::bad_alloc ();
#else
                error_ = error (sys_error::no_memory);
                return;
#endif
              }
            }
            else
              XML_ParserReset (auto_xml_parser_, 0);

            parse_begin (auto_xml_parser_);
            first_ = false;
          }


          if (XML_Parse (xml_parser_,
                         static_cast<const char*> (data),
                         static_cast<int> (size),
                         last) == XML_STATUS_ERROR ||
              last)
          {
            first_ = true;
            parse_end ();
          }
        }


        // XML_Parser
        //

        void document_pimpl::
        parse_begin (XML_Parser parser)
        {
          context_ = context (parser);

          xml_parser_ = parser;
          set ();
        }

        void document_pimpl::
        parse_end ()
        {
          XML_Error e (XML_GetErrorCode (xml_parser_));

          if (e == XML_ERROR_NONE)
          {
            clear ();
            xml_parser_ = 0;
          }
          else
          {
            unsigned long l = XML_GetCurrentLineNumber (xml_parser_);
            unsigned long c = XML_GetCurrentColumnNumber (xml_parser_);

            clear ();
            xml_parser_ = 0;

            // See if the parser was aborted.
            //
            if (e == XML_ERROR_ABORTED)
            {
              // Got to be either a system, schema, or application
              // level error.
              //
#if defined(XSDE_PARSER_VALIDATION) || !defined (XSDE_EXCEPTIONS)
              switch (context_.error_type ())
              {
#ifndef XSDE_EXCEPTIONS
              case context::error_sys:
                {
                  error_ = error (context_.sys_error ());
                  break;
                }
#endif
#ifdef XSDE_PARSER_VALIDATION
              case context::error_schema:
                {
#ifdef XSDE_EXCEPTIONS
                  throw schema (context_.schema_error (), l, c);
#else
                  error_ = error (context_.schema_error (), l, c);
                  break;
#endif
                }
#endif
#ifndef XSDE_EXCEPTIONS
              case context::error_app:
                {
                  error_ = error (context_.app_error (), l, c);
                  break;
                }
#endif
              default:
                {
                  // Someone aborted the parser without setting an
                  // error. Oh well, let them sort this one out.
                  //
                  break;
                }
              }
#endif // XSDE_PARSER_VALIDATION || !XSDE_EXCEPTIONS
            }
            else if (e == XML_ERROR_NO_MEMORY)
            {
#ifdef XSDE_EXCEPTIONS
              throw std::bad_alloc ();
#else
              error_ = error (sys_error::no_memory);
#endif
            }
            else
            {
#ifdef XSDE_EXCEPTIONS
              throw xml (e, l, c);
#else
              error_ = error (e, l, c);
#endif
            }
          }
        }


        //
        //
        void document_pimpl::
        set ()
        {
          assert (xml_parser_ != 0);

          XML_SetUserData(xml_parser_, this);
          XML_SetStartElementHandler (xml_parser_, &start_element);
          XML_SetEndElementHandler (xml_parser_, &end_element);
          XML_SetCharacterDataHandler (xml_parser_, &characters);

#ifdef XSDE_POLYMORPHIC
          if (polymorphic_)
            XML_SetNamespaceDeclHandler (
              xml_parser_, &start_namespace_decl, &end_namespace_decl);
#endif
        }

        void document_pimpl::
        clear ()
        {
          assert (xml_parser_ != 0);

          XML_SetUserData (xml_parser_, 0);
          XML_SetStartElementHandler (xml_parser_, 0);
          XML_SetEndElementHandler (xml_parser_, 0);
          XML_SetCharacterDataHandler (xml_parser_, 0);

#ifdef XSDE_POLYMORPHIC
          if (polymorphic_)
            XML_SetNamespaceDeclHandler (xml_parser_, 0, 0);
#endif
        }


        // Expat thunks.
        //

        void XMLCALL document_pimpl::
        start_element (void* p, const XML_Char* name, const XML_Char** ats)
        {
          document_pimpl& doc = *reinterpret_cast<document_pimpl*> (p);
          doc.start_element_ (name, ats);
        }

        void XMLCALL document_pimpl::
        end_element (void* p, const XML_Char* name)
        {
          document_pimpl& doc = *reinterpret_cast<document_pimpl*> (p);
          doc.end_element_ (name);
        }

        void XMLCALL document_pimpl::
        characters (void* p, const XML_Char* s, int n)
        {
          document_pimpl& doc = *reinterpret_cast<document_pimpl*> (p);
          doc.characters_ (s, static_cast<size_t> (n));
        }

#ifdef XSDE_POLYMORPHIC
        void XMLCALL document_pimpl::
        start_namespace_decl (void* p,
                              const XML_Char* prefix,
                              const XML_Char* ns)
        {
          document_pimpl& doc = *reinterpret_cast<document_pimpl*> (p);
          doc.start_namespace_decl_ (prefix, ns);
        }

        void XMLCALL document_pimpl::
        end_namespace_decl (void* p, const XML_Char* prefix)
        {
          document_pimpl& doc = *reinterpret_cast<document_pimpl*> (p);
          doc.end_namespace_decl_ (prefix);
        }
#endif // XSDE_POLYMORPHIC

        namespace bits
        {
          inline void
          split_name (const XML_Char* s,
                      const char*& ns, size_t& ns_s,
                      const char*& name, size_t& name_s)
          {
            const char* p = strchr (s, ' ');

            if (p)
            {
              ns = s;
              ns_s = p - s;
              name = p + 1;
            }
            else
            {
              ns = s;
              ns_s = 0;
              name = s;
            }

            name_s = strlen (name);
          }
        }

        void document_pimpl::
        start_element_ (const XML_Char* ns_name, const XML_Char** atts)
        {
          // Current Expat (2.0.0) has a (mis)-feature of a possibility of
	  // calling callbacks even after the non-resumable XML_StopParser
          // call. The following code accounts for this.
          //
          {
            XML_ParsingStatus s;
            XML_GetParsingStatus (xml_parser_, &s);
            if (s.parsing == XML_FINISHED)
              return;
          }

          const char* ns_p;
          const char* name_p;
          size_t ns_s, name_s;

          bits::split_name (ns_name, ns_p, ns_s, name_p, name_s);

          parser_state& cur = context_.current_;

          {
            const ro_string ns (ns_p, ns_s);
            const ro_string name (name_p, name_s);

#ifdef XSDE_POLYMORPHIC
            const char* type = 0;
            string type_holder;

            if (polymorphic_)
            {
              // Search for the xsi:type attribute.
              //
              const XML_Char** p = atts; // VC8 can't handle p (atts)
              for (; *p != 0; p += 2)
              {
                bits::split_name (*p, ns_p, ns_s, name_p, name_s);
                const ro_string ns (ns_p, ns_s), name (name_p, name_s);

                if (name.compare ("type", 4) == 0 &&
                    ns.compare (
                      "http://www.w3.org/2001/XMLSchema-instance", 41) == 0)
                  break;
              }

              if (*p != 0)
              {
                bool valid = true;

                // @@ Need proper QName validation.
                //
                // Get the qualified type name and try to resolve it.
                //
                ro_string qn (*(p + 1));

                ro_string tp, tn;
                size_t pos = qn.find (':');

                if (pos != ro_string::npos)
                {
                  tp.assign (qn.data (), pos);
                  tn.assign (qn.data () + pos + 1);

                  if (tp.empty ())
                  {
#ifdef XSDE_PARSER_VALIDATION
                    context_.schema_error (schema_error::invalid_xsi_type);
#endif
                    valid = false;
                  }
                }
                else
                  tn.assign (qn.data (), qn.size ());

                if (valid && tn.empty ())
                {
#ifdef XSDE_PARSER_VALIDATION
                  context_.schema_error (schema_error::invalid_xsi_type);
#endif
                  valid = false;
                }

                if (valid)
                {
                  // Search our namespace declaration stack. Note that
                  // we need to do this even if prefix is empty.
                  //
                  const char* tns = 0;

                  for (size_t i = 0; i < prefixes_.size (); ++i)
                  {
                    if (tp == prefixes_[i])
                    {
#ifdef XSDE_STL
                      tns = prefix_namespaces_[i].c_str ();
#else
                      tns = prefix_namespaces_[i];
#endif
                      break;
                    }
                  }

                  if (!tp.empty () && tns == 0)
                  {
                    // The 'xml' prefix requires special handling.
                    //
                    if (tp.compare ("xml", 3) == 0)
                      tns = "http://www.w3.org/XML/1998/namespace";
                    else
                    {
#ifdef XSDE_PARSER_VALIDATION
                      context_.schema_error (schema_error::invalid_xsi_type);
#endif
                      valid = false;
                    }
                  }

                  // Construct the compound name.
                  //
                  if (valid)
                  {
                    if (tns)
                    {
#ifdef XSDE_EXCEPTIONS
                      type_holder.assign (tn.data (), tn.size ());
                      type_holder.append (" ", 1);
                      type_holder.append (tns);
#else
                      if (type_holder.assign (tn.data (), tn.size ()) ||
                          type_holder.append (" ", 1) ||
                          type_holder.append (tns))
                      {
                        context_.sys_error (sys_error::no_memory);
                        XML_StopParser (xml_parser_, false);
                      }
#endif
                      type = type_holder.data ();
                    }
                    else
                    {
                      // We know that tn is '\0'-terminated.
                      //
                      type = tn.data ();
                    }
                  }
                }

#ifdef XSDE_PARSER_VALIDATION
                if (!valid)
                {
                  XML_StopParser (xml_parser_, false);
                  return;
                }
#endif
              }
            }
#endif // XSDE_POLYMORPHIC

            // Dispatch.
            //
            if (cur.depth_ > 0)
            {
              if (cur.any_)
              {
                // Handling content matched by a wildcard.
                //
                cur.depth_++;

#ifdef XSDE_POLYMORPHIC
                cur.parser_->_start_any_element (ns, name, type);
#else
                cur.parser_->_start_any_element (ns, name);
#endif
              }
              else
              {
                // Ignoring content for which there is no parser.
                //
                cur.depth_++;
              }
            }
            else if (cur.parser_)
            {
              // The "normal" case: call _start_element which will
              // call pre() and _pre_impl() (which will push the
              // new parser).
              //
#ifdef XSDE_POLYMORPHIC
              cur.parser_->_start_element (ns, name, type);
#else
              cur.parser_->_start_element (ns, name);
#endif
            }
            else
            {
              // Root element. Assume the user will report errors via
              // context.
              //
#ifdef XSDE_POLYMORPHIC
              parser_base* p = 0;

              if (polymorphic_ && (root_name_ != name || root_ns_ != ns))
              {
                // See if we can translate this element into type using
                // substitution map.
                //
                if (substitution_map_instance ().check (
                      ns,
                      name,
                      root_ns_.size () ? root_ns_.data () : 0,
                      root_name_.data (),
                      type))
                {
                  ro_string ro_ns (root_ns_);
                  ro_string ro_name (root_name_);
                  p = start_root_element (ro_ns, ro_name, type);
                }
                else
                  p = start_root_element (ns, name, type);
              }
              else
                p = start_root_element (ns, name, type);
#else
              parser_base* p = start_root_element (ns, name);
#endif

#ifndef XSDE_EXCEPTIONS
              if (!context_.error_type ())
              {
#endif
                if (p)
                {
                  // pre() is called by the user.
                  //
                  p->_pre_impl (context_);
                }
                else
                {
                  // Ignoring.
                  //
                  cur.depth_++;
                }

#ifndef XSDE_EXCEPTIONS
              }
#endif
            }

#if defined(XSDE_PARSER_VALIDATION) || !defined(XSDE_EXCEPTIONS)
            if (context_.error_type ())
            {
              XML_StopParser (xml_parser_, false);
              return;
            }
#endif
          }

          // Dispatch attributes if we are not ignoring.
          //
          if (cur.depth_ == 0 || cur.any_)
          {
            for (; *atts != 0; atts += 2)
            {
              bits::split_name (*atts, ns_p, ns_s, name_p, name_s);

              const ro_string ns (ns_p, ns_s), name (name_p, name_s);
              const ro_string value (*(atts + 1));

              if (!cur.any_)
                cur.parser_->_attribute (ns, name, value);
              else
                cur.parser_->_any_attribute (ns, name, value);

#if defined(XSDE_PARSER_VALIDATION) || !defined(XSDE_EXCEPTIONS)
              if (context_.error_type ())
              {
                XML_StopParser (xml_parser_, false);
                break;
              }
#endif
            }
          }
        }

        void document_pimpl::
        end_element_ (const XML_Char* ns_name)
        {
          // See the comment in start_element_ for what's going on here.
          //
          {
            XML_ParsingStatus s;
            XML_GetParsingStatus (xml_parser_, &s);
            if (s.parsing == XML_FINISHED)
              return;
          }

          const char* ns_p;
          const char* name_p;
          size_t ns_s, name_s;

          bits::split_name (ns_name, ns_p, ns_s, name_p, name_s);

          const ro_string ns (ns_p, ns_s);
          const ro_string name (name_p, name_s);

          parser_state& cur = context_.current_;

          // @@ Error propagation.
          //
          if (cur.depth_ == 0)
          {
            // The "normal" case: call _post to pop the parser and then
            // call _end_element on the "outer" parser which calls post().
            //
            parser_base* p = cur.parser_;
            cur.parser_->_post_impl ();

#if defined(XSDE_PARSER_VALIDATION) || !defined(XSDE_EXCEPTIONS)
            if (!context_.error_type ())
            {
#endif
              // This case is a bit complicated by the fact that we can be
              // popped up into a wildcard.
              //
              if (cur.depth_ == 0)
              {
                if (cur.parser_)
                  cur.parser_->_end_element (ns, name);
                else
                {
                  // End of the root element. post() is called by the user.
                  //
                  end_root_element (ns, name, p);
                }
              }
              else
              {
                // Handling content matched by a wildcard.
                //
                if (--cur.depth_ > 0)
                  cur.parser_->_end_any_element (ns, name);
                else
                {
                  cur.parser_->_end_element (ns, name);
                  cur.any_ = false;
                }
              }

#if defined(XSDE_PARSER_VALIDATION) || !defined(XSDE_EXCEPTIONS)
            }
#endif
          }
          else
          {
            if (cur.any_)
            {
              // Handling content matched by a wildcard.
              //
              if (--cur.depth_ > 0)
                cur.parser_->_end_any_element (ns, name);
              else
              {
                cur.parser_->_end_element (ns, name);
                cur.any_ = false;
              }
            }
            else
            {
              // Ignoring content for which there is no parser.
              //
              if (--cur.depth_ == 0)
              {
                if (cur.parser_)
                  cur.parser_->_end_element (ns, name);
                else
                {
                  // End of the root element for which there was
                  // no parser.
                  //
                  end_root_element (ns, name, 0);
                }
              }
            }
          }

#if defined(XSDE_PARSER_VALIDATION) || !defined(XSDE_EXCEPTIONS)
          if (context_.error_type ())
            XML_StopParser (xml_parser_, false);
#endif
        }

        void document_pimpl::
        characters_ (const XML_Char* s, size_t n)
        {
          // See the comment in start_element_ for what's going on here.
          //
          {
            XML_ParsingStatus s;
            XML_GetParsingStatus (xml_parser_, &s);
            if (s.parsing == XML_FINISHED)
              return;
          }

          // Dispatch characters if we are not ignoring.
          //
          parser_state& cur = context_.current_;

          if (n != 0 && (cur.depth_ == 0 || cur.any_))
          {
            const ro_string str (s, n);

            if (!cur.any_)
              cur.parser_->_characters (str);
            else
              cur.parser_->_any_characters (str);

#if defined(XSDE_PARSER_VALIDATION) || !defined(XSDE_EXCEPTIONS)
            if (context_.error_type ())
              XML_StopParser (xml_parser_, false);
#endif
          }
        }

#ifdef XSDE_POLYMORPHIC
        void document_pimpl::
        start_namespace_decl_ (const XML_Char* p, const XML_Char* ns)
        {
          // prefix is 0 for default namespace
          // namespace is 0 when unsetting default namespace
          //
          if (polymorphic_)
          {
#if defined (XSDE_STL)
            prefixes_.push_back (p ? p : "");
            prefix_namespaces_.push_back (ns ? ns : "");
#else
#if defined (XSDE_EXCEPTIONS)
            prefixes_.push_back_copy (p ? p : "");
            prefix_namespaces_.push_back_copy (ns ? ns : "");
#else
            if (prefixes_.push_back_copy (p ? p : "" ) ||
                prefix_namespaces_.push_back_copy (ns ? ns : ""))
            {
              context_.sys_error (sys_error::no_memory);
              XML_StopParser (xml_parser_, false);
            }
#endif
#endif
          }
        }

        void document_pimpl::
        end_namespace_decl_ (const XML_Char*)
        {
          // prefix is 0 for default namespace
          //
          if (polymorphic_)
          {
            // Here we assume the prefixes are removed in the reverse
            // order of them being added. This appears to how every
            // sensible implementation works.
            //
            prefixes_.pop_back ();
            prefix_namespaces_.pop_back ();
          }
        }
#endif // XSDE_POLYMORPHIC
      }
    }
  }
}
