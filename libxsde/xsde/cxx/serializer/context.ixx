// file      : xsde/cxx/serializer/context.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      inline context::
      context (genxWriter xml_serializer)
          : xml_serializer_ (xml_serializer)
      {
#if defined(XSDE_SERIALIZER_VALIDATION) || !defined(XSDE_EXCEPTIONS)
        error_type_ = error_none;
#endif
      }

      inline genxWriter context::
      xml_serializer ()
      {
        return xml_serializer_;
      }

#ifdef XSDE_POLYMORPHIC
      inline void context::
      type_id (const void* ti)
      {
        type_id_ = ti;
      }

      inline const void* context::
      type_id ()
      {
        return type_id_;
      }
#endif

      //
      //
#ifdef XSDE_EXCEPTIONS
      inline void context::
#else
      inline bool context::
#endif
      start_element (const char* name)
      {
        genxStatus e = genxStartElementLiteral (
          xml_serializer_,
          0,
          reinterpret_cast<constUtf8> (name));

#ifdef XSDE_EXCEPTIONS
        if (e != GENX_SUCCESS)
          throw_xml_error (e);
#else
        if (e != GENX_SUCCESS)
          xml_error (e);

        return e == GENX_SUCCESS;
#endif
      }

      //
      //
#ifdef XSDE_EXCEPTIONS
      inline void context::
#else
      inline bool context::
#endif
      start_element (const char* ns, const char* name)
      {
        genxStatus e = genxStartElementLiteral (
          xml_serializer_,
          reinterpret_cast<constUtf8> (ns),
          reinterpret_cast<constUtf8> (name));

#ifdef XSDE_EXCEPTIONS
        if (e != GENX_SUCCESS)
          throw_xml_error (e);
#else
        if (e != GENX_SUCCESS)
          xml_error (e);

        return e == GENX_SUCCESS;
#endif
      }

      //
      //
#ifdef XSDE_EXCEPTIONS
      inline void context::
#else
      inline bool context::
#endif
      end_element ()
      {
        genxStatus e = genxEndElement (xml_serializer_);

#ifdef XSDE_EXCEPTIONS
        if (e != GENX_SUCCESS)
          throw_xml_error (e);
#else
        if (e != GENX_SUCCESS)
          xml_error (e);

        return e == GENX_SUCCESS;
#endif
      }

      //
      //
#ifdef XSDE_EXCEPTIONS
      inline void context::
#else
      inline bool context::
#endif
      start_attribute (const char* name)
      {
        genxStatus e = genxStartAttributeLiteral (
          xml_serializer_,
          0,
          reinterpret_cast<constUtf8> (name));

#ifdef XSDE_EXCEPTIONS
        if (e != GENX_SUCCESS)
          throw_xml_error (e);
#else
        if (e != GENX_SUCCESS)
          xml_error (e);

        return e == GENX_SUCCESS;
#endif
      }

      //
      //
#ifdef XSDE_EXCEPTIONS
      inline void context::
#else
      inline bool context::
#endif
      start_attribute (const char* ns, const char* name)
      {
        genxStatus e = genxStartAttributeLiteral (
          xml_serializer_,
          reinterpret_cast<constUtf8> (ns),
          reinterpret_cast<constUtf8> (name));

#ifdef XSDE_EXCEPTIONS
        if (e != GENX_SUCCESS)
          throw_xml_error (e);
#else
        if (e != GENX_SUCCESS)
          xml_error (e);

        return e == GENX_SUCCESS;
#endif
      }

      //
      //
#ifdef XSDE_EXCEPTIONS
      inline void context::
#else
      inline bool context::
#endif
      end_attribute ()
      {
        genxStatus e = genxEndAttribute (xml_serializer_);

#ifdef XSDE_EXCEPTIONS
        if (e != GENX_SUCCESS)
          throw_xml_error (e);
#else
        if (e != GENX_SUCCESS)
          xml_error (e);

        return e == GENX_SUCCESS;
#endif
      }

      //
      //
#ifdef XSDE_EXCEPTIONS
      inline void context::
#else
      inline bool context::
#endif
      attribute (const char* name, const char* value)
      {
        genxStatus e = genxAddAttributeLiteral (
          xml_serializer_,
          0,
          reinterpret_cast<constUtf8> (name),
          reinterpret_cast<constUtf8> (value));

#ifdef XSDE_EXCEPTIONS
        if (e != GENX_SUCCESS)
          throw_xml_error (e);
#else
        if (e != GENX_SUCCESS)
          xml_error (e);

        return e == GENX_SUCCESS;
#endif
      }

      //
      //
#ifdef XSDE_EXCEPTIONS
      inline void context::
#else
      inline bool context::
#endif
      attribute (const char* ns, const char* name, const char* value)
      {
        genxStatus e = genxAddAttributeLiteral (
          xml_serializer_,
          reinterpret_cast<constUtf8> (ns),
          reinterpret_cast<constUtf8> (name),
          reinterpret_cast<constUtf8> (value));

#ifdef XSDE_EXCEPTIONS
        if (e != GENX_SUCCESS)
          throw_xml_error (e);
#else
        if (e != GENX_SUCCESS)
          xml_error (e);

        return e == GENX_SUCCESS;
#endif
      }

      //
      //
#ifdef XSDE_EXCEPTIONS
      inline void context::
#else
      inline bool context::
#endif
      characters (const char* s)
      {
        genxStatus e = genxAddText (
          xml_serializer_,
          reinterpret_cast<constUtf8> (s));

#ifdef XSDE_EXCEPTIONS
        if (e != GENX_SUCCESS)
          throw_xml_error (e);
#else
        if (e != GENX_SUCCESS)
          xml_error (e);

        return e == GENX_SUCCESS;
#endif
      }

      //
      //
#ifdef XSDE_EXCEPTIONS
      inline void context::
#else
      inline bool context::
#endif
      characters (const char* s, size_t n)
      {
        genxStatus e = genxAddCountedText (
          xml_serializer_,
          reinterpret_cast<constUtf8> (s),
          static_cast<int> (n));

#ifdef XSDE_EXCEPTIONS
        if (e != GENX_SUCCESS)
          throw_xml_error (e);
#else
        if (e != GENX_SUCCESS)
          xml_error (e);

        return e == GENX_SUCCESS;
#endif
      }

      //
      //
#ifdef XSDE_EXCEPTIONS
      inline void context::
#else
      inline bool context::
#endif
      declare_namespace (const char* ns, const char* prefix)
      {
        genxStatus e = genxAddNamespaceLiteral (
          xml_serializer_,
          reinterpret_cast<constUtf8> (ns),
          reinterpret_cast<constUtf8> (prefix));

#ifdef XSDE_EXCEPTIONS
        if (e != GENX_SUCCESS)
          throw_xml_error (e);
#else
        if (e != GENX_SUCCESS)
          xml_error (e);

        return e == GENX_SUCCESS;
#endif
      }

      //
      //
#ifdef XSDE_EXCEPTIONS
      inline void context::
#else
      inline bool context::
#endif
      declare_default_namespace (const char* ns)
      {
        genxStatus e = genxAddNamespaceLiteral (
          xml_serializer_,
          reinterpret_cast<constUtf8> (ns),
          reinterpret_cast<constUtf8> (""));

#ifdef XSDE_EXCEPTIONS
        if (e != GENX_SUCCESS)
          throw_xml_error (e);
#else
        if (e != GENX_SUCCESS)
          xml_error (e);

        return e == GENX_SUCCESS;
#endif
      }

      //
      //
#ifdef XSDE_EXCEPTIONS
      inline void context::
#else
      inline bool context::
#endif
      clear_default_namespace ()
      {
        genxStatus e = genxUnsetDefaultNamespace (xml_serializer_);

#ifdef XSDE_EXCEPTIONS
        if (e != GENX_SUCCESS)
          throw_xml_error (e);
#else
        if (e != GENX_SUCCESS)
          xml_error (e);

        return e == GENX_SUCCESS;
#endif
      }

      //
      //
      inline const char* context::
      lookup_namespace_prefix (const char* ns)
      {
        genxStatus e;
        genxNamespace gns = genxDeclareNamespace (
          xml_serializer_, reinterpret_cast<constUtf8> (ns), 0, &e);

        if (e != GENX_SUCCESS)
        {
#ifdef XSDE_EXCEPTIONS
          throw_xml_error (e);
#else
          xml_error (e);
#endif
          return 0;
        }

        return reinterpret_cast<const char*> (
          genxGetNamespacePrefix (gns));
      }

      //
      //
#ifndef XSDE_EXCEPTIONS
      inline int context::
      app_error () const
      {
        return error_code_.app;
      }

      inline void context::
      app_error (int e)
      {
        error_type_ = error_app;
        error_code_.app = e;
      }

      inline void context::
      sys_error (sys_error_t e)
      {
        error_type_ = error_sys;
        error_code_.sys = e;
      }

      inline context::sys_error_t context::
      sys_error () const
      {
        return error_code_.sys;
      }

      inline void context::
      xml_error (xml_error_t e)
      {
        error_type_ = error_xml;
        error_code_.xml = e;
      }

      inline context::xml_error_t context::
      xml_error () const
      {
        return error_code_.xml;
      }
#endif

#ifdef XSDE_SERIALIZER_VALIDATION
      inline context::schema_error_t context::
      schema_error () const
      {
        return error_code_.schema;
      }

      inline void context::
      schema_error (schema_error_t e)
      {
        error_type_ = error_schema;
        error_code_.schema = e;
      }
#endif

#if defined(XSDE_SERIALIZER_VALIDATION) || !defined(XSDE_EXCEPTIONS)
      inline context::error_type_t context::
      error_type () const
      {
        return error_type_;
      }
#endif

    }
  }
}
