// file      : xsde/cxx/serializer/validating/xml-schema-sskel.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_SERIALIZER_VALIDATING_XML_SCHEMA_SSKEL_HXX
#define XSDE_CXX_SERIALIZER_VALIDATING_XML_SCHEMA_SSKEL_HXX

#include <xsde/cxx/config.hxx>

#ifdef XSDE_STL
#  include <string>
#endif

#ifdef XSDE_REGEXP
#  include <xsde/c/regexp/xmlregexp.h>
#endif

#include <xsde/cxx/serializer/xml-schema.hxx>

#include <xsde/cxx/serializer/validating/serializer.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        // anyType and anySimpleType.
        //
        struct any_type_sskel: complex_content
        {
          virtual void
          pre ();

          // Override the following two functions to implement
          // your logic.
          //

          // virtual void
          // _serialize_attributes ();

          // virtual void
          // _serialize_content ();

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          any_type_sskel ();
          any_type_sskel (any_type_sskel* impl, void*);

        protected:
          any_type_sskel* any_type_impl_;
#endif
        };

        struct any_simple_type_sskel: simple_content
        {
          virtual void
#ifdef XSDE_STL
          pre (const std::string&) = 0;
#else
          pre (const char*) = 0;
#endif

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          any_simple_type_sskel ();
          any_simple_type_sskel (any_simple_type_sskel* impl, void*);

        protected:
          any_simple_type_sskel* any_simple_type_impl_;
#endif
        };

        // Boolean.
        //
        struct boolean_sskel: simple_content
        {
          virtual void
          pre (bool) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          boolean_sskel ();
          boolean_sskel (boolean_sskel* impl, void*);

        protected:
          boolean_sskel* boolean_impl_;
#endif
        };

        // 8-bit
        //

        struct byte_sskel: simple_content
        {
          virtual void
          pre (signed char) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif
          byte_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          byte_sskel (byte_sskel* impl, void*);

        protected:
          byte_sskel* byte_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (signed char, bool inclusive);

          void
          _min_facet (signed char, bool inclusive);

        protected:
          struct facets
          {
            signed char min_;
            signed char max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };

        struct unsigned_byte_sskel: simple_content
        {
          virtual void
          pre (unsigned char) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif
          unsigned_byte_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          unsigned_byte_sskel (unsigned_byte_sskel* impl, void*);

        protected:
          unsigned_byte_sskel* unsigned_byte_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (unsigned char, bool inclusive);

          void
          _min_facet (unsigned char, bool inclusive);

        protected:
          struct facets
          {
            unsigned char min_;
            unsigned char max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };


        // 16-bit
        //

        struct short_sskel: simple_content
        {
          virtual void
          pre (short) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

          short_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          short_sskel (short_sskel* impl, void*);

        protected:
          short_sskel* short_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (short, bool inclusive);

          void
          _min_facet (short, bool inclusive);

        protected:
          struct facets
          {
            short min_;
            short max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };

        struct unsigned_short_sskel: simple_content
        {
          virtual void
          pre (unsigned short) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

          unsigned_short_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          unsigned_short_sskel (unsigned_short_sskel* impl, void*);

        protected:
          unsigned_short_sskel* unsigned_short_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (unsigned short, bool inclusive);

          void
          _min_facet (unsigned short, bool inclusive);

        protected:
          struct facets
          {
            unsigned short min_;
            unsigned short max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };

        // 32-bit
        //

        struct int_sskel: simple_content
        {
          virtual void
          pre (int) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif
          int_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          int_sskel (int_sskel* impl, void*);

        protected:
          int_sskel* int_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (int, bool inclusive);

          void
          _min_facet (int, bool inclusive);

        protected:
          struct facets
          {
            int min_;
            int max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };

        struct unsigned_int_sskel: simple_content
        {
          virtual void
          pre (unsigned int) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

          unsigned_int_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          unsigned_int_sskel (unsigned_int_sskel* impl, void*);

        protected:
          unsigned_int_sskel* unsigned_int_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (unsigned int, bool inclusive);

          void
          _min_facet (unsigned int, bool inclusive);

        protected:
          struct facets
          {
            unsigned int min_;
            unsigned int max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };

        // 64-bit
        //

#ifdef XSDE_LONGLONG
        struct long_sskel: simple_content
        {
          virtual void
          pre (long long) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif
          long_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          long_sskel (long_sskel* impl, void*);

        protected:
          long_sskel* long_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (long long, bool inclusive);

          void
          _min_facet (long long, bool inclusive);

        protected:
          struct facets
          {
            long long min_;
            long long max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };

        struct unsigned_long_sskel: simple_content
        {
          virtual void
          pre (unsigned long long) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif
          unsigned_long_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          unsigned_long_sskel (unsigned_long_sskel* impl, void*);

        protected:
          unsigned_long_sskel* unsigned_long_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (unsigned long long, bool inclusive);

          void
          _min_facet (unsigned long long, bool inclusive);

        protected:
          struct facets
          {
            unsigned long long min_;
            unsigned long long max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };
#else
        struct long_sskel: simple_content
        {
          virtual void
          pre (long) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif
          long_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          long_sskel (long_sskel* impl, void*);

        protected:
          long_sskel* long_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (long, bool inclusive);

          void
          _min_facet (long, bool inclusive);

        protected:
          struct facets
          {
            long min_;
            long max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };

        struct unsigned_long_sskel: simple_content
        {
          virtual void
          pre (unsigned long) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif
          unsigned_long_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          unsigned_long_sskel (unsigned_long_sskel* impl, void*);

        protected:
          unsigned_long_sskel* unsigned_long_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (unsigned long, bool inclusive);

          void
          _min_facet (unsigned long, bool inclusive);

        protected:
          struct facets
          {
            unsigned long min_;
            unsigned long max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };
#endif

        // Arbitrary-length integers.
        //

        struct integer_sskel: simple_content
        {
          virtual void
          pre (long) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif
          integer_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          integer_sskel (integer_sskel* impl, void*);

        protected:
          integer_sskel* integer_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (long, bool inclusive);

          void
          _min_facet (long, bool inclusive);

        protected:
          struct facets
          {
            long min_;
            long max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };

        struct negative_integer_sskel: simple_content
        {
          virtual void
          pre (long) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif
          negative_integer_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          negative_integer_sskel (negative_integer_sskel* impl, void*);

        protected:
          negative_integer_sskel* negative_integer_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (long, bool inclusive);

          void
          _min_facet (long, bool inclusive);

        protected:
          struct facets
          {
            long min_;
            long max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };

        struct non_positive_integer_sskel: simple_content
        {
          virtual void
          pre (long) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif
          non_positive_integer_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          non_positive_integer_sskel (non_positive_integer_sskel* impl, void*);

        protected:
          non_positive_integer_sskel* non_positive_integer_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (long, bool inclusive);

          void
          _min_facet (long, bool inclusive);

        protected:
          struct facets
          {
            long min_;
            long max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };

        struct positive_integer_sskel: simple_content
        {
          virtual void
          pre (unsigned long) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif
          positive_integer_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          positive_integer_sskel (positive_integer_sskel* impl, void*);

        protected:
          positive_integer_sskel* positive_integer_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (unsigned long, bool inclusive);

          void
          _min_facet (unsigned long, bool inclusive);

        protected:
          struct facets
          {
            unsigned long min_;
            unsigned long max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };

        struct non_negative_integer_sskel: simple_content
        {
          virtual void
          pre (unsigned long) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif
          non_negative_integer_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          non_negative_integer_sskel (non_negative_integer_sskel* impl, void*);

        protected:
          non_negative_integer_sskel* non_negative_integer_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (unsigned long, bool inclusive);

          void
          _min_facet (unsigned long, bool inclusive);

        protected:
          struct facets
          {
            unsigned long min_;
            unsigned long max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };

        // Floats.
        //

        struct float_sskel: simple_content
        {
          virtual void
          pre (float) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif
          float_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          float_sskel (float_sskel* impl, void*);

        protected:
          float_sskel* float_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (float, bool inclusive);

          void
          _min_facet (float, bool inclusive);

        protected:
          struct facets
          {
            float min_;
            float max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };

        struct double_sskel: simple_content
        {
          virtual void
          pre (double) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif
          double_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          double_sskel (double_sskel* impl, void*);

        protected:
          double_sskel* double_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (double, bool inclusive);

          void
          _min_facet (double, bool inclusive);

        protected:
          struct facets
          {
            double min_;
            double max_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };

        struct decimal_sskel: simple_content
        {
          virtual void
          pre (double) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif
          decimal_sskel ();

#ifdef XSDE_REUSE_STYLE_TIEIN
          decimal_sskel (decimal_sskel* impl, void*);

        protected:
          decimal_sskel* decimal_impl_;
#endif
          // Facets.
          //
        public:
          void
          _max_facet (double, bool inclusive);

          void
          _min_facet (double, bool inclusive);

          void
          _fraction_digits_facet (unsigned int);

        protected:
          struct facets
          {
            double min_;
            double max_;
            unsigned int fraction_digits_;

            unsigned int min_set_ : 1;
            unsigned int min_inc_ : 1;
            unsigned int max_set_ : 1;
            unsigned int max_inc_ : 1;
            unsigned int fraction_digits_set_ : 1;
          };

          const facets&
          _facets () const;

        private:
          facets facets_;
        };

        //
        // String-based types.
        //

        struct string_facets
        {
          string_facets ();
#ifdef XSDE_REGEXP
          ~string_facets ();
#endif

          void
          _length_facet (size_t);

          void
          _max_length_facet (size_t);

          void
          _min_length_facet (size_t);

          void
          _enumeration_facet (const char* const*, size_t count);

          void
          _pattern_facet (const char*);

        public:
          struct facets
          {
            size_t length_;
            size_t min_length_;
            size_t max_length_;

            const char* const* enum_;
            size_t enum_count_;

#ifdef XSDE_REGEXP
            union
            {
              const char* str;
              xmlRegexpPtr regexp;
            } pattern_;
#endif
            unsigned int length_set_ : 1;
            unsigned int min_length_set_ : 1;
            unsigned int max_length_set_ : 1;

#ifdef XSDE_REGEXP
            // 0 - not set
            // 1 - string
            // 2 - compiled
            //
            unsigned int pattern_set_: 2;
#endif
          };

        protected:
          facets facets_;
        };

        struct string_sskel: simple_content, string_facets
        {
          virtual void
#ifdef XSDE_STL
          pre (const std::string&) = 0;
#else
          pre (const char*) = 0;
#endif

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          string_sskel ();
          string_sskel (string_sskel* impl, void*);

        protected:
          string_sskel* string_impl_;
#endif

        protected:
          const string_facets::facets&
          _facets () const;
        };

#ifdef XSDE_REUSE_STYLE_MIXIN
        struct normalized_string_sskel: virtual string_sskel
#else
        struct normalized_string_sskel: string_sskel
#endif
        {
#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          normalized_string_sskel ();
          normalized_string_sskel (normalized_string_sskel* impl, void*);

        protected:
          normalized_string_sskel* normalized_string_impl_;
#endif
        };

#ifdef XSDE_REUSE_STYLE_MIXIN
        struct token_sskel: virtual normalized_string_sskel
#else
        struct token_sskel: normalized_string_sskel
#endif
        {
#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          token_sskel ();
          token_sskel (token_sskel* impl, void*);

        protected:
          token_sskel* token_impl_;
#endif
        };

#ifdef XSDE_REUSE_STYLE_MIXIN
        struct name_sskel: virtual token_sskel
#else
        struct name_sskel: token_sskel
#endif
        {
#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          name_sskel ();
          name_sskel (name_sskel* impl, void*);

        protected:
          name_sskel* name_impl_;
#endif
        };

#ifdef XSDE_REUSE_STYLE_MIXIN
        struct nmtoken_sskel: virtual token_sskel
#else
        struct nmtoken_sskel: token_sskel
#endif
        {
#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          nmtoken_sskel ();
          nmtoken_sskel (nmtoken_sskel* impl, void*);

        protected:
          nmtoken_sskel* nmtoken_impl_;
#endif
        };

        struct nmtokens_sskel: simple_content
        {
          virtual void
          pre (const string_sequence*) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          nmtokens_sskel ();
          nmtokens_sskel (nmtokens_sskel* impl, void*);

        protected:
          nmtokens_sskel* nmtokens_impl_;
#endif
        };

#ifdef XSDE_REUSE_STYLE_MIXIN
        struct ncname_sskel: virtual name_sskel
#else
        struct ncname_sskel: name_sskel
#endif
        {
#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          ncname_sskel ();
          ncname_sskel (ncname_sskel* impl, void*);

        protected:
          ncname_sskel* ncname_impl_;
#endif
        };

#ifdef XSDE_REUSE_STYLE_MIXIN
        struct id_sskel: virtual ncname_sskel
#else
        struct id_sskel: ncname_sskel
#endif
        {
#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          id_sskel ();
          id_sskel (id_sskel* impl, void*);

        protected:
          id_sskel* id_impl_;
#endif
        };

#ifdef XSDE_REUSE_STYLE_MIXIN
        struct idref_sskel: virtual ncname_sskel
#else
        struct idref_sskel: ncname_sskel
#endif
        {
#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          idref_sskel ();
          idref_sskel (idref_sskel* impl, void*);

        protected:
          idref_sskel* idref_impl_;
#endif
        };

        struct idrefs_sskel: simple_content
        {
          virtual void
          pre (const string_sequence*) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          idrefs_sskel ();
          idrefs_sskel (idrefs_sskel* impl, void*);

        protected:
          idrefs_sskel* idrefs_impl_;
#endif
        };

#ifdef XSDE_REUSE_STYLE_MIXIN
        struct language_sskel: virtual token_sskel
#else
        struct language_sskel: token_sskel
#endif
        {
#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          language_sskel ();
          language_sskel (language_sskel* impl, void*);

        protected:
          language_sskel* language_impl_;
#endif
        };

        struct uri_sskel: simple_content, string_facets
        {
          virtual void
#ifdef XSDE_STL
          pre (const std::string&) = 0;
#else
          pre (const char*) = 0;
#endif

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          uri_sskel ();
          uri_sskel (uri_sskel* impl, void*);

        protected:
          uri_sskel* uri_impl_;
#endif
        protected:
          const string_facets::facets&
          _facets () const;
        };

        struct qname_sskel: simple_content
        {
          virtual void
#ifdef XSDE_STL
          pre (const qname&) = 0;
#else
          pre (const qname*) = 0;
#endif

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          qname_sskel ();
          qname_sskel (qname_sskel* impl, void*);

        protected:
          qname_sskel* qname_impl_;
#endif
        };

        // base64Binary
        //
        struct base64_binary_sskel: simple_content
        {
          virtual void
          pre (const buffer*) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          base64_binary_sskel ();
          base64_binary_sskel (base64_binary_sskel* impl, void*);

        protected:
          base64_binary_sskel* base64_binary_impl_;
#endif
        };

        // hexBinary
        //
        struct hex_binary_sskel: simple_content
        {
          virtual void
          pre (const buffer*) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          hex_binary_sskel ();
          hex_binary_sskel (hex_binary_sskel* impl, void*);

        protected:
          hex_binary_sskel* hex_binary_impl_;
#endif
        };

        // Time and date types.
        //
        struct gday_sskel: simple_content
        {
          virtual void
          pre (const gday&) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          gday_sskel ();
          gday_sskel (gday_sskel* impl, void*);

        protected:
          gday_sskel* gday_impl_;
#endif
        };

        struct gmonth_sskel: simple_content
        {
          virtual void
          pre (const gmonth&) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          gmonth_sskel ();
          gmonth_sskel (gmonth_sskel* impl, void*);

        protected:
          gmonth_sskel* gmonth_impl_;
#endif
        };

        struct gyear_sskel: simple_content
        {
          virtual void
          pre (const gyear&) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          gyear_sskel ();
          gyear_sskel (gyear_sskel* impl, void*);

        protected:
          gyear_sskel* gyear_impl_;
#endif
        };

        struct gmonth_day_sskel: simple_content
        {
          virtual void
          pre (const gmonth_day&) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          gmonth_day_sskel ();
          gmonth_day_sskel (gmonth_day_sskel* impl, void*);

        protected:
          gmonth_day_sskel* gmonth_day_impl_;
#endif
        };

        struct gyear_month_sskel: simple_content
        {
          virtual void
          pre (const gyear_month&) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          gyear_month_sskel ();
          gyear_month_sskel (gyear_month_sskel* impl, void*);

        protected:
          gyear_month_sskel* gyear_month_impl_;
#endif
        };

        struct date_sskel: simple_content
        {
          virtual void
          pre (const date&) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          date_sskel ();
          date_sskel (date_sskel* impl, void*);

        protected:
          date_sskel* date_impl_;
#endif
        };

        struct time_sskel: simple_content
        {
          virtual void
          pre (const time&) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          time_sskel ();
          time_sskel (time_sskel* impl, void*);

        protected:
          time_sskel* time_impl_;
#endif
        };

        struct date_time_sskel: simple_content
        {
          virtual void
          pre (const date_time&) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          date_time_sskel ();
          date_time_sskel (date_time_sskel* impl, void*);

        protected:
          date_time_sskel* date_time_impl_;
#endif
        };

        struct duration_sskel: simple_content
        {
          virtual void
          pre (const duration&) = 0;

#ifdef XSDE_POLYMORPHIC
          static const char*
          _static_type ();

          virtual const char*
          _dynamic_type () const;
#endif

#ifdef XSDE_REUSE_STYLE_TIEIN
          duration_sskel ();
          duration_sskel (duration_sskel* impl, void*);

        protected:
          duration_sskel* duration_impl_;
#endif
        };
      }
    }
  }
}

#include <xsde/cxx/serializer/validating/xml-schema-sskel.ixx>

#endif  // XSDE_CXX_SERIALIZER_VALIDATING_XML_SCHEMA_SSKEL_HXX
