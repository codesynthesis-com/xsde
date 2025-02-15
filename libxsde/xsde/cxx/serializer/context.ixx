// file      : xsde/cxx/serializer/context.ixx
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
#ifndef XSDE_EXCEPTIONS
      inline int context::
      app_error () const
      {
        return error_type_ == error_app ? error_code_.app : 0;
      }

      inline void context::
      app_error (int e)
      {
        error_type_ = error_app;
        error_code_.app = e;
      }

      inline context::sys_error_t context::
      sys_error () const
      {
        return error_type_ == error_sys ? error_code_.sys : sys_error::none;
      }

      inline void context::
      sys_error (sys_error_t e)
      {
        error_type_ = error_sys;
        error_code_.sys = e;
      }

      inline context::xml_error_t context::
      xml_error () const
      {
        return error_type_ == error_xml ? error_code_.xml : GENX_SUCCESS;
      }

      inline void context::
      xml_error (xml_error_t e)
      {
        error_type_ = error_xml;
        error_code_.xml = e;
      }
#endif

#ifdef XSDE_SERIALIZER_VALIDATION
      inline context::schema_error_t context::
      schema_error () const
      {
        return error_type_ == error_schema
          ? error_code_.schema
          : schema_error::none;
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
