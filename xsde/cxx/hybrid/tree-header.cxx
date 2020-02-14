// file      : xsd/cxx/hybrid/tree-header.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/hybrid/tree-header.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      struct Enumerator: Traversal::Enumerator, Context
      {
        Enumerator (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& e)
        {
          os << ename (e);
        }
      };

      struct Enumeration : Traversal::Enumeration, Context
      {
        Enumeration (Context& c, Traversal::Complex& complex)
            : Context (c),
              complex_ (complex),
              base_name_ (c, TypeName::base),
              enumerator_ (c)
        {
          names_ >> enumerator_;
        }

        virtual void
        traverse (Type& e)
        {
          // First see if we should delegate this one to the Complex
          // generator.
          //
          Type* base_enum (0);

          if (!enum_ || !enum_mapping (e, &base_enum))
          {
            complex_.traverse (e);
            return;
          }

          SemanticGraph::Context& ec (e.context ());
          String const& name (ename_custom (e));

          // We may not need to generate the class if this type is
          // being customized.
          //
          if (name)
          {
            bool fl (fixed_length (e));
            bool cd (ec.count ("cd-name"));
            bool poly (polymorphic (e));
            String const& vt (ec.get<String> ("value-type"));

            os << "// " << comment (e.name ()) << " (" <<
              (fl ? "fixed-length" : "variable-length") << ")" << endl
               << "//" << endl;

            os << "class " << name;

            if (base_enum)
            {
              os << ": public ";
              base_name_.dispatch (e.inherits ().base ());
            }

            os << "{";

            if (!fl)
              os << "private:" << endl
                 << name << " (const " << name << "&);"
                 << name << "& operator= (const " << name << "&);"
                 << endl;

            os << "public:" << endl;

            // value_type
            //
            if (base_enum)
            {
              os << "typedef ";
              base_name_.dispatch (*base_enum);
              os << "::" << base_enum->context ().get<String> ("value-type") <<
                " " << vt << ";"
                 << endl;
            }
            else
            {
              os << "enum " << vt
                 << "{";
              names<Enumeration> (e, names_, 0, 0, 0, &Enumeration::comma);
              os << "};";
            }

            // c-tors
            //
            os << name << " ();"
               << name << " (" << vt << ");"
               << endl;

            // _clone
            //
            if (!fl && clone)
              os << (poly ? "virtual " : "") << name << "*" << endl
                 << "_clone () const;"
                 << endl;

            // value (value_type)
            //
            if (!base_enum)
              os << "void" << endl
                 << ec.get<String> ("value") << " (" << vt << ");"
                 << endl;

            // d-tor
            //
            if (poly)
              os << "virtual" << endl
                 << "~" << name << " ();"
                 << endl;

            if (!base_enum)
            {
              // operator value()
              //
              // Name lookup differences in various compilers make generation
              // of this operator outside of the class a really hard task. So
              // we are going to make it always inline.
              //
              os << "operator " << vt << " () const"
                 << "{"
                 << "return " << ec.get<String> ("value-member") << ";"
                 << "}";

              // string()
              //
              os << "const char*" << endl
                 << ec.get<String> ("string") << " () const;"
                 << endl;
            }

            // Custom data.
            //
            if (cd)
            {
              String const& name (ecd_name (e));
              String const& sequence (ecd_sequence (e));
              String const& iterator (ecd_iterator (e));
              String const& const_iterator (ecd_const_iterator (e));

              os << "// Custom data." << endl
                 << "//" << endl;

              // sequence & iterators
              //
              os << "typedef " << data_seq << " " << sequence << ";"
                 << "typedef " << sequence << "::iterator " << iterator << ";"
                 << "typedef " << sequence << "::const_iterator " <<
                const_iterator << ";"
                 << endl;

              // const seq&
              // name () const
              //
              os << "const " << sequence << "&" << endl
                 << name << " () const;"
                 << endl;

              // seq&
              // name ()
              //
              os << sequence << "&" << endl
                 << name << " ();"
                 << endl;
            }

            if (poly && typeinfo)
            {
              os << "// Type information." << endl
                 << "//" << endl;

              os << "static const " <<
                (stl ? "::std::string&" : "char*") << endl
                 << "_static_type ();"
                 << endl;

              os << "virtual const " <<
                (stl ? "::std::string&" : "char*") << endl
                 << "_dynamic_type () const;"
                 << endl;
            }

            // _copy
            //
            if (!fl && clone)
              os << (exceptions ? "void" : "bool") << endl
                 << "_copy (" << name << "&) const;"
                 << endl;

            if (!base_enum || cd)
              os << "private:" << endl;

            if (!base_enum)
              os << vt << " " << ec.get<String> ("value-member") << ";";

            if (cd)
              os << ecd_sequence (e) << " " << ecd_member (e) << ";";

            os << "};";
          }

          // Generate include for custom type.
          //
          if (ec.count ("name-include"))
          {
            close_ns ();

            os << "#include " << process_include_path (
              ec.get<String> ("name-include")) << endl
               << endl;

            open_ns ();
          }
        }

        virtual void
        comma (Type&)
        {
          os << "," << endl;
        }

      private:
        Traversal::Complex& complex_;
        TypeName base_name_;

        Traversal::Names names_;
        Enumerator enumerator_;
      };

      struct List : Traversal::List, Context
      {
        List (Context& c)
            : Context (c), base_name_ (c, TypeName::seq)
        {
        }

        virtual void
        traverse (Type& l)
        {
          SemanticGraph::Context& lc (l.context ());
          String const& name (ename_custom (l));

          // We may not need to generate the class if this type is
          // being customized.
          //
          if (name)
          {
            bool cd (lc.count ("cd-name"));
            bool poly (polymorphic (l));

            os << "// " << comment (l.name ()) << " (variable-length)" << endl
               << "//" << endl;

            os << "class " << name << ": public ";

            base_name_.dispatch (l.argumented ().type ());

            os << "{"
               << "private:" << endl
               << name << " (const " << name << "&);"
               << name << "& operator= (const " << name << "&);"
               << endl;

            // c-tor
            //
            os << "public:" << endl
               << name << " ();"
               << endl;

            // _clone
            //
            if (clone)
              os << (poly ? "virtual " : "") << name << "*" << endl
                 << "_clone () const;"
                 << endl;

            // d-tor
            //
            if (poly)
              os << "virtual" << endl
                 << "~" << name << " ();"
                 << endl;

            // Custom data.
            //
            if (cd)
            {
              String const& name (ecd_name (l));
              String const& sequence (ecd_sequence (l));
              String const& iterator (ecd_iterator (l));
              String const& const_iterator (ecd_const_iterator (l));

              os << "// Custom data." << endl
                 << "//" << endl;

              // sequence & iterators
              //
              os << "typedef " << data_seq << " " << sequence << ";"
                 << "typedef " << sequence << "::iterator " << iterator << ";"
                 << "typedef " << sequence << "::const_iterator " <<
                const_iterator << ";"
                 << endl;

              // const seq&
              // name () const
              //
              os << "const " << sequence << "&" << endl
                 << name << " () const;"
                 << endl;

              // seq&
              // name ()
              //
              os << sequence << "&" << endl
                 << name << " ();"
                 << endl;
            }

            if (poly && typeinfo)
            {
              os << "// Type information." << endl
                 << "//" << endl;

              os << "static const " <<
                (stl ? "::std::string&" : "char*") << endl
                 << "_static_type ();"
                 << endl;

              os << "virtual const " <<
                (stl ? "::std::string&" : "char*") << endl
                 << "_dynamic_type () const;"
                 << endl;
            }

            // _copy
            //
            if (clone)
              os << (exceptions ? "void" : "bool") << endl
                 << "_copy (" << name << "&) const;"
                 << endl;

            if (cd)
            {
              os << "private:" << endl
                 << ecd_sequence (l) << " " << ecd_member (l) << ";";
            }

            os << "};";
          }

          // Generate include for custom type.
          //
          if (lc.count ("name-include"))
          {
            close_ns ();

            os << "#include " << process_include_path (
              lc.get<String> ("name-include")) << endl
               << endl;

            open_ns ();
          }
        }

      private:
        TypeName base_name_;
      };

      struct Union : Traversal::Union, Context
      {
        Union (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& u)
        {
          SemanticGraph::Context& uc (u.context ());
          String const& name (ename_custom (u));

          // We may not need to generate the class if this type is
          // being customized.
          //
          if (name)
          {
            bool fl (fixed_length (u));
            bool poly (polymorphic (u));
            bool cd (uc.count ("cd-name"));

            os << "// " << comment (u.name ()) << " (" <<
              (fl ? "fixed-length" : "variable-length") << ")" << endl
               << "//" << endl;

            os << "class " << name
               << "{";

            if (!fl)
              os << "private:" << endl
                 << name << " (const " << name << "&);"
                 << name << "& operator= (const " << name << "&);"
                 << endl;

            os << "public:" << endl;

            // c-tor
            //
            os << name << " ();"
               << endl;

            // _clone
            //
            if (!fl && clone)
              os << (poly ? "virtual " : "") << name << "*" << endl
                 << "_clone () const;"
                 << endl;

            // d-tor
            //
            if (!stl || poly)
              os << (poly ? "virtual\n" : "") << "~" << name << " ();";

            os << endl;

            String const& value (uc.get<String> ("value"));
            String const& member (uc.get<String> ("value-member"));

            if (stl)
            {
              // const std::string&
              // name () const
              //
              os << "const ::std::string&" << endl
                 << value << " () const;"
                 << endl;

              // std::string&
              // name ()
              //
              os << "::std::string&" << endl
                 << value << " ();"
                 << endl;

              // void
              // name (const std::string&)
              //
              os << "void" << endl
                 << value << " (const ::std::string&);"
                 << endl;
            }
            else
            {
              // const char*
              // name () const
              //
              os << "const char*" << endl
                 << value << " () const;"
                 << endl;

              // char*
              // name ()
              //
              os << "char*" << endl
                 << value << " ();"
                 << endl;

              // void
              // name (char*)
              //
              os << "void" << endl
                 << value << " (char*);"
                 << endl;

              // char*
              // detach ()
              //
              if (detach)
              {
                os << "char*" << endl
                   << uc.get<String> ("value-detach") << " ();"
                   << endl;
              }
            }

            // Custom data.
            //
            if (cd)
            {
              String const& name (ecd_name (u));
              String const& sequence (ecd_sequence (u));
              String const& iterator (ecd_iterator (u));
              String const& const_iterator (ecd_const_iterator (u));

              os << "// Custom data." << endl
                 << "//" << endl;

              // sequence & iterators
              //
              os << "typedef " << data_seq << " " << sequence << ";"
                 << "typedef " << sequence << "::iterator " << iterator << ";"
                 << "typedef " << sequence << "::const_iterator " <<
                const_iterator << ";"
                 << endl;

              // const seq&
              // name () const
              //
              os << "const " << sequence << "&" << endl
                 << name << " () const;"
                 << endl;

              // seq&
              // name ()
              //
              os << sequence << "&" << endl
                 << name << " ();"
                 << endl;
            }

            if (poly && typeinfo)
            {
              os << "// Type information." << endl
                 << "//" << endl;

              os << "static const " <<
                (stl ? "::std::string&" : "char*") << endl
                 << "_static_type ();"
                 << endl;

              os << "virtual const " <<
                (stl ? "::std::string&" : "char*") << endl
                 << "_dynamic_type () const;"
                 << endl;
            }

            // _copy
            //
            if (!fl && clone)
              os << (exceptions ? "void" : "bool") << endl
                 << "_copy (" << name << "&) const;"
                 << endl;

            if (stl)
            {
              os << "private:" << endl
                 << "::std::string " << member << ";";
            }
            else
            {
              os << "private:" << endl
                 << "char* " << member << ";";
            }

            // Custom data.
            //
            if (cd)
              os << ecd_sequence (u) << " " << ecd_member (u) << ";";

            os << "};";
          }

          // Generate include for custom type.
          //
          if (uc.count ("name-include"))
          {
            close_ns ();

            os << "#include " << process_include_path (
              uc.get<String> ("name-include")) << endl
               << endl;

            open_ns ();
          }
        }
      };

      //
      // Data.
      //

      struct AlignType: Traversal::Compositor,

                        Traversal::List,
                        Traversal::Union,
                        Traversal::Complex,

                        Traversal::AnySimpleType,

                        Traversal::Fundamental::Byte,
                        Traversal::Fundamental::UnsignedByte,
                        Traversal::Fundamental::Short,
                        Traversal::Fundamental::UnsignedShort,
                        Traversal::Fundamental::Int,
                        Traversal::Fundamental::UnsignedInt,
                        Traversal::Fundamental::Long,
                        Traversal::Fundamental::UnsignedLong,
                        Traversal::Fundamental::Integer,
                        Traversal::Fundamental::NonPositiveInteger,
                        Traversal::Fundamental::NonNegativeInteger,
                        Traversal::Fundamental::PositiveInteger,
                        Traversal::Fundamental::NegativeInteger,

                        Traversal::Fundamental::Boolean,

                        Traversal::Fundamental::Float,
                        Traversal::Fundamental::Double,
                        Traversal::Fundamental::Decimal,

                        Traversal::Fundamental::String,
                        Traversal::Fundamental::NormalizedString,
                        Traversal::Fundamental::Token,
                        Traversal::Fundamental::Name,
                        Traversal::Fundamental::NameToken,
                        Traversal::Fundamental::NCName,
                        Traversal::Fundamental::Language,

                        Traversal::Fundamental::QName,

                        Traversal::Fundamental::Id,
                        Traversal::Fundamental::IdRef,

                        Traversal::Fundamental::AnyURI,

                        Traversal::Fundamental::Date,
                        Traversal::Fundamental::DateTime,
                        Traversal::Fundamental::Duration,
                        Traversal::Fundamental::Day,
                        Traversal::Fundamental::Month,
                        Traversal::Fundamental::MonthDay,
                        Traversal::Fundamental::Year,
                        Traversal::Fundamental::YearMonth,
                        Traversal::Fundamental::Time,

                        Traversal::Fundamental::Entity,

                        Context
      {
        AlignType (Context& c)
            : Context (c)
        {
          *this >> inherits_ >> *this;

          *this >> attribute_names_ >> attribute_;

          *this >> contains_particle_;
          contains_particle_ >> particle_;
          contains_particle_ >> *this;
          contains_compositor_ >> *this;

          attribute_ >> belongs_;
          particle_ >> belongs_;
          belongs_ >> *this;
        }

        // Type alignment ranking (the higher the rank, the stricter
        // the alignment requirements):
        //
        // char      1
        // short     2
        // bool      4
        // int       4
        // float     4
        // long      5
        // void*     5
        // size_t    5
        // double    7
        // long long 8
        //
        // Note also that this traverser only needs to handle fixed-
        // length types.
        //

        void
        dispatch (SemanticGraph::Node& n)
        {
          rank_ = 1;
          type_ = "char";

          NodeBase::dispatch (n);

          os << type_;
        }

        virtual void
        traverse (SemanticGraph::List&)
        {
          align_type ("size_t", 5);
        }

        virtual void
        traverse (SemanticGraph::Union&)
        {
          align_type ("size_t", 5); // std::string
        }

        virtual void
        traverse (SemanticGraph::Complex& c)
        {
          Complex::inherits (c, inherits_);

          Complex::names (c, attribute_names_);

          if (c.contains_compositor_p ())
            Complex::contains_compositor (c, contains_compositor_);
        }

        // anySimpleType
        //
        virtual void
        traverse (SemanticGraph::AnySimpleType&)
        {
          align_type ("size_t", 5); // std::string
        }

        // Boolean.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Boolean&)
        {
          // In worst case scenario bool is 4 bytes. Assume that.
          //
          align_type ("int", 4);
        }

        // Integral types.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Byte&)
        {
          align_type ("signed char", 1);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedByte&)
        {
          align_type ("unsigned char", 1);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Short&)
        {
          align_type ("short", 2);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedShort&)
        {
          align_type ("unsigned short", 2);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Int&)
        {
          align_type ("int", 4);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedInt&)
        {
          align_type ("unsigned int", 4);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Long&)
        {
          if (options.no_long_long ())
            align_type ("long", 5);
          else
            align_type ("long long", 8);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedLong&)
        {
          if (options.no_long_long ())
            align_type ("unsigned long", 5);
          else
            align_type ("unsigned long long", 8);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Integer&)
        {
          align_type ("long", 5);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NonPositiveInteger&)
        {
          align_type ("long", 5);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NonNegativeInteger&)
        {
          align_type ("unsigned long", 5);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::PositiveInteger&)
        {
          align_type ("unsigned long", 5);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NegativeInteger&)
        {
          align_type ("long", 5);
        }

        // Floats.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Float&)
        {
          align_type ("float", 4);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Double&)
        {
          align_type ("double", 7);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Decimal&)
        {
          align_type ("double", 7);
        }

        // Strings.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::String&)
        {
          align_type ("size_t", 5); // std::string
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NormalizedString&)
        {
          align_type ("size_t", 5); // std::string
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Token&)
        {
          align_type ("size_t", 5); // std::string
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NameToken&)
        {
          align_type ("size_t", 5); // std::string
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Name&)
        {
          align_type ("size_t", 5); // std::string
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NCName&)
        {
          align_type ("size_t", 5); // std::string
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Language&)
        {
          align_type ("size_t", 5); // std::string
        }

        // Qualified name.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::QName&)
        {
          align_type ("size_t", 5); // std::string
        }

        // ID/IDREF.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Id&)
        {
          align_type ("size_t", 5); // std::string
        }

        virtual void
        traverse (SemanticGraph::Fundamental::IdRef&)
        {
          align_type ("size_t", 5); // std::string
        }

        // URI.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::AnyURI&)
        {
          align_type ("size_t", 5); // std::string
        }

        // Date/time.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Date&)
        {
          align_type ("int", 4);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::DateTime&)
        {
          align_type ("double", 7);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Duration&)
        {
          align_type ("double", 7);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Day&)
        {
          align_type ("short", 2);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Month&)
        {
          align_type ("short", 2);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::MonthDay&)
        {
          align_type ("short", 2);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Year&)
        {
          align_type ("int", 4);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::YearMonth&)
        {
          align_type ("int", 4);
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Time&)
        {
          align_type ("double", 7);
        }

        // Entity.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Entity&)
        {
          align_type ("size_t", 5); // std::string
        }

      private:
        void
        align_type (char const* type, unsigned short rank)
        {
          if (rank > rank_)
          {
            rank_ = rank;
            type_ = type;
          }
        }

      private:
        String type_;
        unsigned short rank_;

      private:
        Traversal::Inherits inherits_;

        struct Attribute: Traversal::Attribute
        {
          virtual void
          traverse (Type& a)
          {
            if (!a.fixed_p ())
              Traversal::Attribute::traverse (a);
          }
        };

        Attribute attribute_;
        Traversal::Names attribute_names_;

        Traversal::Element particle_;
        Traversal::ContainsParticle contains_particle_;
        Traversal::ContainsCompositor contains_compositor_;

        Traversal::Belongs belongs_;
      };

      struct AttributeData: Traversal::Attribute, Context
      {
        AttributeData (Context& c)
            : Context (c), var_ (c, TypeName::var)
        {
        }

        virtual void
        traverse (SemanticGraph::Attribute& a)
        {
          if (!a.fixed_p ())
          {
            SemanticGraph::Type& t (a.type ());

            var_.dispatch (t);
            os << " " << emember (a) << ";";

            if (a.optional_p () && !a.default_p () && fixed_length (t))
              os << "unsigned char " << epresent_member (a) << ";";
          }
        }

      private:
        TypeName var_;
      };


      struct ElementData: Traversal::Element, Context
      {
        ElementData (Context& c)
            : Context (c), var_ (c, TypeName::var)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          if (e.max () != 1)
          {
            os << esequence (e) << " " << emember (e) << ";";
          }
          else
          {
            SemanticGraph::Type& t (e.type ());

            var_.dispatch (t);
            os << " " << emember (e) << ";";

            if (e.min () == 0 && fixed_length (t))
              os << "unsigned char " << epresent_member (e) << ";";
          }
        }

      private:
        TypeName var_;
      };

      struct ElementInChoiceData: Traversal::Element, Context
      {
        ElementInChoiceData (Context& c)
            : Context (c), var_ (c, TypeName::var), align_type_ (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          if (e.max () != 1)
          {
            os << "union"
               << "{"
               << "size_t align_;"
               << "char data_[sizeof (" << esequence (e) << ")];"
               << "} " << emember (e) << ";";
          }
          else
          {
            SemanticGraph::Type& t (e.type ());

            if (fixed_length (t))
            {
              os << "union"
                 << "{";
              align_type_.dispatch (t);
              os << " align_;"
                 << "char data_[sizeof (";

              var_.dispatch (t);

              os << ")";

              // Reserve an extra byte for the present flag.
              //
              if (e.min () == 0)
                os << " + 1";

              os << "];"
                 << "} " << emember (e) << ";";
            }
            else
            {
              var_.dispatch (t);
              os << " " << emember (e) << ";";
            }
          }
        }

      private:
        TypeName var_;
        AlignType align_type_;
      };

      struct AllData: Traversal::All, Context
      {
        AllData (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}
          // and it can only contain particles.
          //
          if (a.min () == 0)
          {
            String const& type (etype (a));
            String const& member (emember(a));

            if (fixed_length (a))
            {
              os << type << " " << member << ";"
                 << "unsigned char " << epresent_member (a) << ";";
            }
            else
              os << type << "* " << member << ";";
          }
          else
            All::contains (a);
        }
      };

      struct ChoiceInSequenceData: Traversal::Choice, Context
      {
        ChoiceInSequenceData (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.max () != 1)
          {
            os << esequence (c) << " " << emember (c) << ";";
          }
          else if (c.min () == 0)
          {
            String const& type (etype (c));
            String const& member (emember (c));

            if (fixed_length (c))
              os << type << " " << member << ";"
                 << "unsigned char " << epresent_member (c) << ";";
            else
              os << type << "* " << member << ";";
          }
          else
          {
            os << "union"
               << "{";

            Choice::contains (c);

            os << "} " << emember (c) << ";"
               << earm_tag (c) << " " << earm_member (c) << ";";
          }
        }
      };

      struct ChoiceInChoiceData: Traversal::Choice, Context
      {
        ChoiceInChoiceData (Context& c)
            : Context (c), align_type_ (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          // For choice in choice we always have a nested class.
          //
          if (c.max () != 1)
          {
            os << "union"
               << "{"
               << "size_t align_;"
               << "char data_[sizeof (" << esequence (c) << ")];"
               << "} " << emember (c) << ";";
          }
          else
          {
            if (fixed_length (c))
            {
              os << "union"
                 << "{";
              align_type_.dispatch (c);
              os << " align_;"
                 << "char data_[sizeof (" << etype (c) << ")";

              // Reserve an extra byte for the present flag.
              //
              if (c.min () == 0)
                os << " + 1";

              os << "];"
                 << "} " << emember (c) << ";";
            }
            else
              os << etype (c) << "* " << emember (c) << ";";
          }
        }

      private:
        AlignType align_type_;
      };

      struct SequenceInSequenceData: Traversal::Sequence, Context
      {
        SequenceInSequenceData (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          if (s.max () != 1)
          {
            os << esequence (s) << " " << emember (s) << ";";
          }
          else if (s.min () == 0)
          {
            String const& type (etype (s));
            String const& member (emember (s));

            if (fixed_length (s))
              os << type << " " << member << ";"
                 << "unsigned char " << epresent_member (s) << ";";
            else
              os << type << "* " << member << ";";
          }
          else
            Sequence::contains (s);
        }
      };

      struct SequenceInChoiceData: Traversal::Sequence, Context
      {
        SequenceInChoiceData (Context& c)
            : Context (c), align_type_ (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          // For sequence in choice we always have a nested class.
          //
          if (s.max () != 1)
          {
            os << "union"
               << "{"
               << "size_t align_;"
               << "char data_[sizeof (" << esequence (s) << ")];"
               << "} " << emember (s) << ";";
          }
          else
          {
            if (fixed_length (s))
            {
              os << "union"
                 << "{";
              align_type_.dispatch (s);
              os << " align_;"
                 << "char data_[sizeof (" << etype (s) << ")";

              // Reserve an extra byte for the present flag.
              //
              if (s.min () == 0)
                os << " + 1";

              os << "];"
                 << "} " << emember (s) << ";";
            }
            else
              os << etype (s) << "* " << emember (s) << ";";
          }
        }

      private:
        AlignType align_type_;
      };

      //
      // Accessors/modifiers.
      //

      struct Attribute: Traversal::Attribute, Context
      {
        Attribute (Context& c)
            : Context (c),
              ro_ret_ (c, TypeName::ro_ret),
              ret_ (c, TypeName::ret),
              arg_ (c, TypeName::arg)
        {
        }

        virtual void
        traverse (SemanticGraph::Attribute& a)
        {
          os << "// " << comment (a.name ()) << endl
             << "//" << endl;

          bool def (a.default_p ());
          bool fix (a.fixed_p ());

          String const& name (ename (a));
          SemanticGraph::Type& t (a.type ());
          bool fl (fixed_length (t));


          if (a.optional_p () && !fix)
          {
            String const& name (def ? edefault (a) : epresent (a));

            os << "bool" << endl
               << name << " () const;"
               << endl;

            if (fl)
              os << "void" << endl
                 << name << " (bool);"
                 << endl;
          }

          // const type&
          // name () const
          //
          ro_ret_.dispatch (t);
          os << endl
             << name << " () const;"
             << endl;

          // Do not generate modifiers for fixed attributes.
          //
          if (!fix)
          {
            // type&
            // name ()
            //
            ret_.dispatch (t);
            os << endl
               << name << " ();"
               << endl;


            // void
            // name (const type& | type*)
            //
            os << "void" << endl
               << name << " (";
            arg_.dispatch (t);
            os << ");"
               << endl;

            // type*
            // detach ()
            //
            if (detach && !fl)
            {
              arg_.dispatch (t);
              os << endl
                 << edetach (a) << " ();"
                 << endl;
            }
          }

          if (def)
          {
            // static const type&
            // name_{default|fixed}_value ()
            //
            os << "static ";
            ro_ret_.dispatch (t);
            os << endl
               << edefault_value (a) << " ();"
               << endl;
          }
        }

      private:
        TypeName ro_ret_;
        TypeName ret_;
        TypeName arg_;
      };

      struct Element: Traversal::Element, Context
      {
        Element (Context& c)
            : Context (c),
              seq_ (c, TypeName::seq),
              ro_ret_ (c, TypeName::ro_ret),
              ret_ (c, TypeName::ret),
              arg_ (c, TypeName::arg)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          os << "// " << comment (e.name ()) << endl
             << "//" << endl;

          String const& name (ename (e));
          SemanticGraph::Type& t (e.type ());
          bool fl (fixed_length (t));

          if (e.max () != 1)
          {
            String const& sequence (esequence (e));
            String const& iterator (eiterator (e));
            String const& const_iterator (econst_iterator (e));

            // sequence & iterators
            //
            os << "typedef ";
            seq_.dispatch (t);
            os << " " << sequence << ";"
               << "typedef " << sequence << "::iterator " << iterator << ";"
               << "typedef " << sequence << "::const_iterator " <<
              const_iterator << ";"
               << endl;

            // const seq&
            // name () const
            //
            os << "const " << sequence << "&" << endl
               << name << " () const;"
               << endl;

            // seq&
            // name ()
            //
            os << sequence << "&" << endl
               << name << " ();"
               << endl;
          }
          else
          {
            if (e.min () == 0)
            {
              // optional
              //
              String const& present (epresent (e));

              os << "bool" << endl
                 << present << " () const;"
                 << endl;

              if (fl)
                os << "void" << endl
                   << present << " (bool);"
                   << endl;
            }

            // const type&
            // name () const
            //
            ro_ret_.dispatch (t);
            os << endl
               << name << " () const;"
               << endl;

            // type&
            // name ()
            //
            ret_.dispatch (t);
            os << endl
               << name << " ();"
               << endl;


            // void
            // name (const type& | type*)
            //
            os << "void" << endl
               << name << " (";
            arg_.dispatch (t);
            os << ");"
               << endl;

            // type*
            // detach ()
            //
            if (detach && !fl)
            {
              arg_.dispatch (t);
              os << endl
                 << edetach (e) << " ();"
                 << endl;
            }
          }
        }

      private:
        TypeName seq_;
        TypeName ro_ret_;
        TypeName ret_;
        TypeName arg_;
      };

      struct All: Traversal::All, Context
      {
        All (Context& c, Traversal::ContainsParticle& contains_data)
            : Context (c), contains_data_ (contains_data)
        {
        }

        virtual void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}
          // and it can only contain particles.
          //
          if (a.min () == 0)
          {
            bool fl (fixed_length (a));
            bool cd (a.context ().count ("cd-name"));

            String const& name (ename (a));
            String const& type (etype (a));
            String const& present (epresent (a));

            os << "// " << comment (name) << " (" <<
              (fl ? "fixed-length" : "variable-length") << ")" << endl
               << "//" << endl;

            os << "class " << type
               << "{";

            // c-tor
            //
            os << "public:" << endl
               << type << " ();"
               << endl;

            // _clone
            //
            if (!fl && clone)
              os << type << "*" << endl
                 << "_clone () const;"
                 << endl;

            // d-tor
            //
            os << "~" << type << " ();"
               << endl;

            // copy c-tor & operator=
            //
            if (!fl)
              os << "private:" << endl;

            os << type << " (const " << type << "&);"
               << type << "& operator= (const " << type << "&);"
               << endl;

            if (!fl)
              os << "public:" << endl;

            All::contains (a);

            // Custom data.
            //
            if (cd)
            {
              String const& name (ecd_name (a));
              String const& sequence (ecd_sequence (a));
              String const& iterator (ecd_iterator (a));
              String const& const_iterator (ecd_const_iterator (a));

              os << "// Custom data." << endl
                 << "//" << endl;

              // sequence & iterators
              //
              os << "typedef " << data_seq << " " << sequence << ";"
                 << "typedef " << sequence << "::iterator " << iterator << ";"
                 << "typedef " << sequence << "::const_iterator " <<
                const_iterator << ";"
                 << endl;

              // const seq&
              // name () const
              //
              os << "const " << sequence << "&" << endl
                 << name << " () const;"
                 << endl;

              // seq&
              // name ()
              //
              os << sequence << "&" << endl
                 << name << " ();"
                 << endl;
            }

            // _copy
            //
            if (!fl && clone)
              os << (exceptions ? "void" : "bool") << endl
                 << "_copy (" << type << "&) const;"
                 << endl;

            os << "private:" << endl;

            All::contains (a, contains_data_);

            // Custom data.
            //
            if (cd)
              os << ecd_sequence (a) << " " << ecd_member (a) << ";";

            os << "};";

            // name_present
            //
            os << "bool" << endl
               << present << " () const;"
               << endl;

            if (fl)
              os << "void" << endl
                 << present << " (bool);"
                 << endl;

            // const type&
            // name () const
            //
            os << "const " << type << "&" << endl
               << name << " () const;"
               << endl;

            // type&
            // name ()
            //
            os << type << "&" << endl
               << name << " ();"
               << endl;

            // void
            // name (const type& | type*)
            //
            os << "void" << endl
               << name << " (";

            if (fl)
              os << "const " << type << "&";
            else
              os << type << "*";

            os << ");"
               << endl;

            // type*
            // detach ()
            //
            if (detach && !fl)
            {
              os << type << "*" << endl
                 << edetach (a) << " ();"
                 << endl;
            }
          }
          else
            All::contains (a);
        }

      private:
        Traversal::ContainsParticle& contains_data_;
      };

      struct ParticleTag: Traversal::Element,
                          Traversal::Any,
                          Traversal::Choice,
                          Traversal::Sequence,
                          Context
      {
        ParticleTag (Context& c)
            : Context (c), first_ (true)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          emit_tag (etag (e));
        }

        virtual void
        traverse (SemanticGraph::Any& a)
        {
          emit_tag (etag (a));
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          emit_tag (etag (c));
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          emit_tag (etag (s));
        }

        virtual void
        emit_tag (String const& tag)
        {
          if (first_)
            first_ = false;
          else
            os << "," << endl;

          os << tag;
        }

      private:
        bool first_;
      };

      struct ChoiceInSequence: Traversal::Choice, Context
      {
        ChoiceInSequence (Context& c,
                          Traversal::ContainsParticle& contains_data)
            : Context (c), contains_data_ (contains_data)
        {
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          size_t min (c.min ()), max (c.max ());

          String const& name (ename (c));

          bool fl;
          String type;

          if (max != 1 || min == 0)
          {
            type = etype (c);

            fl = fixed_length (c);

            os << "// " << comment (name) << " (" <<
            (fl ? "fixed-length" : "variable-length") << ")" << endl
             << "//" << endl;

            os << "class " << type
               << "{";

            // c-tor
            //
            os << "public:" << endl
               << type << " ();"
               << endl;

            // _clone
            //
            if (!fl && clone)
              os << type << "*" << endl
                 << "_clone () const;"
                 << endl;

            // d-tor
            //
            os << "~" << type << " ();"
               << endl;

            // copy c-tor & operator=
            //
            if (!fl)
              os << "private:" << endl;

            os << type << " (const " << type << "&);"
               << type << "& operator= (const " << type << "&);"
               << endl;

            if (!fl)
              os << "public:" << endl;
          }
          else
          {
            os << "// " << comment (name) << endl
               << "//" << endl;
          }

          String const& arm_tag (earm_tag (c));
          String const& arm (earm (c));

          os << "enum " << arm_tag
             << "{";

          {
            ParticleTag particle (*this);
            Traversal::ContainsParticle contains_particle (particle);
            Traversal::Choice::contains (c, contains_particle);
          }

          os << "};";

          // arm_tag
          // arm () const;
          //
          os << arm_tag << endl
             << arm << " () const;"
             << endl;

          // void
          // arm (arm_tag);
          //
          os << "void" << endl
             << arm << " (" << arm_tag << ");"
             << endl;

          Choice::contains (c);

          if (max != 1 || min == 0)
          {
            bool cd (c.context ().count ("cd-name"));

            // Custom data.
            //
            if (cd)
            {
              String const& name (ecd_name (c));
              String const& sequence (ecd_sequence (c));
              String const& iterator (ecd_iterator (c));
              String const& const_iterator (ecd_const_iterator (c));

              os << "// Custom data." << endl
                 << "//" << endl;

              // sequence & iterators
              //
              os << "typedef " << data_seq << " " << sequence << ";"
                 << "typedef " << sequence << "::iterator " << iterator << ";"
                 << "typedef " << sequence << "::const_iterator " <<
                const_iterator << ";"
                 << endl;

              // const seq&
              // name () const
              //
              os << "const " << sequence << "&" << endl
                 << name << " () const;"
                 << endl;

              // seq&
              // name ()
              //
              os << sequence << "&" << endl
                 << name << " ();"
                 << endl;
            }

            // _copy
            //
            if (!fl && clone)
              os << (exceptions ? "void" : "bool") << endl
                 << "_copy (" << type << "&) const;"
                 << endl;

            os << "private:" << endl
               << "union"
               << "{";

            Choice::contains (c, contains_data_);

            os << "} " << emember (c) << ";"
               << arm_tag << " " << earm_member (c) << ";";

            // Custom data.
            //
            if (cd)
              os << ecd_sequence (c) << " " << ecd_member (c) << ";";

            os << "};";
          }

          if (max != 1)
          {
            String const& sequence (esequence (c));
            String const& iterator (eiterator (c));
            String const& const_iterator (econst_iterator (c));

            // sequence & iterators
            //
            os << "typedef " << (fl ? fix_seq : var_seq) << "< " <<
              type << " > " << sequence << ";"
               << "typedef " << sequence << "::iterator " << iterator << ";"
               << "typedef " << sequence << "::const_iterator " <<
              const_iterator << ";"
               << endl;

            // const seq&
            // name () const
            //
            os << "const " << sequence << "&" << endl
               << name << " () const;"
               << endl;

            // seq&
            // name ()
            //
            os << sequence << "&" << endl
               << name << " ();"
               << endl;

          }
          else if (min == 0)
          {
            String const& present (epresent (c));

            // name_present
            //
            os << "bool" << endl
               << present << " () const;"
               << endl;

            if (fl)
              os << "void" << endl
                 << present << " (bool);"
                 << endl;

            // const type&
            // name () const
            //
            os << "const " << type << "&" << endl
               << name << " () const;"
               << endl;

            // type&
            // name ()
            //
            os << type << "&" << endl
               << name << " ();"
               << endl;

            // void
            // name (const type& | type*)
            //
            os << "void" << endl
               << name << " (";

            if (fl)
              os << "const " << type << "&";
            else
              os << type << "*";

            os << ");"
               << endl;

            // type*
            // detach ()
            //
            if (detach && !fl)
            {
              os << type << "*" << endl
                 << edetach (c) << " ();"
                 << endl;
            }
          }
        }

      private:
        Traversal::ContainsParticle& contains_data_;
      };

      struct ChoiceInChoice: Traversal::Choice, Context
      {
        ChoiceInChoice (Context& c,
                        Traversal::ContainsParticle& contains_data)
            : Context (c), contains_data_ (contains_data)
        {
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          // When shoice is in choice we generate nested class even
          // for min == max == 1.
          //
          size_t min (c.min ()), max (c.max ());

          bool fl (fixed_length (c));
          bool cd (c.context ().count ("cd-name"));

          String const& name (ename (c));
          String const& type (etype (c));

          os << "// " << comment (name) << " (" <<
            (fl ? "fixed-length" : "variable-length") << ")" << endl
             << "//" << endl;

          os << "class " << type
             << "{";

          // c-tor
          //
          os << "public:" << endl
             << type << " ();"
             << endl;

          // _clone
          //
          if (!fl && clone)
            os << type << "*" << endl
               << "_clone () const;"
               << endl;

          // d-tor
          //
          os << "~" << type << " ();"
             << endl;

          // copy c-tor & operator=
          //
          if (!fl)
            os << "private:" << endl;

          os << type << " (const " << type << "&);"
             << type << "& operator= (const " << type << "&);"
             << endl;

          if (!fl)
            os << "public:" << endl;

          String const& arm_tag (earm_tag (c));
          String const& arm (earm (c));

          os << "enum " << arm_tag
             << "{";

          {
            ParticleTag particle (*this);
            Traversal::ContainsParticle contains_particle (particle);
            Traversal::Choice::contains (c, contains_particle);
          }

          os << "};";

          // arm_tag
          // arm () const;
          //
          os << arm_tag << endl
             << arm << " () const;"
             << endl;

          // void
          // arm (arm_tag);
          //
          os << "void" << endl
             << arm << " (" << arm_tag << ");"
             << endl;

          Choice::contains (c);

          // Custom data.
          //
          if (cd)
          {
            String const& name (ecd_name (c));
            String const& sequence (ecd_sequence (c));
            String const& iterator (ecd_iterator (c));
            String const& const_iterator (ecd_const_iterator (c));

            os << "// Custom data." << endl
               << "//" << endl;

            // sequence & iterators
            //
            os << "typedef " << data_seq << " " << sequence << ";"
               << "typedef " << sequence << "::iterator " << iterator << ";"
               << "typedef " << sequence << "::const_iterator " <<
                const_iterator << ";"
               << endl;

            // const seq&
            // name () const
            //
            os << "const " << sequence << "&" << endl
               << name << " () const;"
               << endl;

            // seq&
            // name ()
            //
            os << sequence << "&" << endl
               << name << " ();"
               << endl;
          }

          // _copy
          //
          if (!fl && clone)
            os << (exceptions ? "void" : "bool") << endl
               << "_copy (" << type << "&) const;"
               << endl;

          os << "private:" << endl
             << "union"
             << "{";

          Choice::contains (c, contains_data_);

          os << "} " << emember (c) << ";"
             << arm_tag << " " << earm_member (c) << ";";

          // Custom data.
          //
          if (cd)
            os << ecd_sequence (c) << " " << ecd_member (c) << ";";

          os << "};";

          if (max != 1)
          {
            String const& sequence (esequence (c));
            String const& iterator (eiterator (c));
            String const& const_iterator (econst_iterator (c));

            // sequence & iterators
            //
            os << "typedef " << (fl ? fix_seq : var_seq) << "< " <<
              type << " > " << sequence << ";"
               << "typedef " << sequence << "::iterator " << iterator << ";"
               << "typedef " << sequence << "::const_iterator " <<
              const_iterator << ";"
               << endl;

            // const seq&
            // name () const
            //
            os << "const " << sequence << "&" << endl
               << name << " () const;"
               << endl;

            // seq&
            // name ()
            //
            os << sequence << "&" << endl
               << name << " ();"
               << endl;
          }
          else
          {
            if (min == 0)
            {
              String const& present (epresent (c));

              // name_present
              //
              os << "bool" << endl
                 << present << " () const;"
                 << endl;

              if (fl)
                os << "void" << endl
                   << present << " (bool);"
                   << endl;
            }

            // const type&
            // name () const
            //
            os << "const " << type << "&" << endl
               << name << " () const;"
               << endl;

            // type&
            // name ()
            //
            os << type << "&" << endl
               << name << " ();"
               << endl;

            // void
            // name (const type& | type*)
            //
            os << "void" << endl
               << name << " (";

            if (fl)
              os << "const " << type << "&";
            else
              os << type << "*";

            os << ");"
               << endl;

            // type*
            // detach ()
            //
            if (detach && !fl)
            {
              os << type << "*" << endl
                 << edetach (c) << " ();"
                 << endl;
            }
          }
        }

      private:
        Traversal::ContainsParticle& contains_data_;
      };


      struct SequenceInSequence: Traversal::Sequence, Context
      {
        SequenceInSequence (Context& c,
                            Traversal::ContainsParticle& contains_data)
            : Context (c), contains_data_ (contains_data)
        {
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          size_t min (s.min ()), max (s.max ());

          if (max == 1 && min == 1)
          {
            Sequence::contains (s);
            return;
          }

          bool fl (fixed_length (s));
          bool cd (s.context ().count ("cd-name"));

          String const& name (ename (s));
          String const& type (etype (s));

          os << "// " << comment (name) << " (" <<
            (fl ? "fixed-length" : "variable-length") << ")" << endl
             << "//" << endl;

          os << "class " << type
             << "{";

          // c-tor
          //
          os << "public:" << endl
             << type << " ();"
             << endl;

          // _clone
          //
          if (!fl && clone)
            os << type << "*" << endl
               << "_clone () const;"
               << endl;

          // d-tor
          //
          os << "~" << type << " ();"
             << endl;

          // copy c-tor & operator=
          //
          if (!fl)
            os << "private:" << endl;

          os << type << " (const " << type << "&);"
             << type << "& operator= (const " << type << "&);"
             << endl;

          if (!fl)
            os << "public:" << endl;

          Sequence::contains (s);

          // Custom data.
          //
          if (cd)
          {
            String const& name (ecd_name (s));
            String const& sequence (ecd_sequence (s));
            String const& iterator (ecd_iterator (s));
            String const& const_iterator (ecd_const_iterator (s));

            os << "// Custom data." << endl
               << "//" << endl;

            // sequence & iterators
            //
            os << "typedef " << data_seq << " " << sequence << ";"
               << "typedef " << sequence << "::iterator " << iterator << ";"
               << "typedef " << sequence << "::const_iterator " <<
              const_iterator << ";"
               << endl;

            // const seq&
            // name () const
            //
            os << "const " << sequence << "&" << endl
               << name << " () const;"
               << endl;

            // seq&
            // name ()
            //
            os << sequence << "&" << endl
               << name << " ();"
               << endl;
          }

          // _copy
          //
          if (!fl && clone)
            os << (exceptions ? "void" : "bool") << endl
               << "_copy (" << type << "&) const;"
               << endl;

          os << "private:" << endl;

          Sequence::contains (s, contains_data_);

          // Custom data.
          //
          if (cd)
            os << ecd_sequence (s) << " " << ecd_member (s) << ";";

          os << "};";

          if (max != 1)
          {
            String const& sequence (esequence (s));
            String const& iterator (eiterator (s));
            String const& const_iterator (econst_iterator (s));

            // sequence & iterators
            //
            os << "typedef " << (fl ? fix_seq : var_seq) << "< " <<
              type << " > " << sequence << ";"
               << "typedef " << sequence << "::iterator " << iterator << ";"
               << "typedef " << sequence << "::const_iterator " <<
              const_iterator << ";"
               << endl;

            // const seq&
            // name () const
            //
            os << "const " << sequence << "&" << endl
               << name << " () const;"
               << endl;

            // seq&
            // name ()
            //
            os << sequence << "&" << endl
               << name << " ();"
               << endl;

          }
          else if (min == 0)
          {
            String const& present (epresent (s));

            // name_present
            //
            os << "bool" << endl
               << present << " () const;"
               << endl;

            if (fl)
              os << "void" << endl
                 << present << " (bool);"
                 << endl;

            // const type&
            // name () const
            //
            os << "const " << type << "&" << endl
               << name << " () const;"
               << endl;

            // type&
            // name ()
            //
            os << type << "&" << endl
               << name << " ();"
               << endl;

            // void
            // name (const type& | type*)
            //
            os << "void" << endl
               << name << " (";

            if (fl)
              os << "const " << type << "&";
            else
              os << type << "*";

            os << ");"
               << endl;

            // type*
            // detach ()
            //
            if (detach && !fl)
            {
              os << type << "*" << endl
                 << edetach (s) << " ();"
                 << endl;
            }
          }
        }

      private:
        Traversal::ContainsParticle& contains_data_;
      };

      struct SequenceInChoice: Traversal::Sequence, Context
      {
        SequenceInChoice (Context& c,
                          Traversal::ContainsParticle& contains_data)
            : Context (c), contains_data_ (contains_data)
        {
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          // When sequence is in choice we generate nested class even
          // for min == max == 1.
          //
          bool fl (fixed_length (s));
          bool cd (s.context ().count ("cd-name"));

          String const& name (ename (s));
          String const& type (etype (s));

          os << "// " << comment (name) << " (" <<
            (fl ? "fixed-length" : "variable-length") << ")" << endl
             << "//" << endl;

          os << "class " << type
             << "{";

          // c-tor
          //
          os << "public:" << endl
             << type << " ();"
             << endl;

          // _clone
          //
          if (!fl && clone)
            os << type << "*" << endl
               << "_clone () const;"
               << endl;

          // d-tor
          //
          os << "~" << type << " ();"
             << endl;

          // copy c-tor & operator=
          //
          if (!fl)
            os << "private:" << endl;

          os << type << " (const " << type << "&);"
             << type << "& operator= (const " << type << "&);"
             << endl;

          if (!fl)
            os << "public:" << endl;

          Sequence::contains (s);

          if (cd)
          {
            String const& name (ecd_name (s));
            String const& sequence (ecd_sequence (s));
            String const& iterator (ecd_iterator (s));
            String const& const_iterator (ecd_const_iterator (s));

            os << "// Custom data." << endl
               << "//" << endl;

            // sequence & iterators
            //
            os << "typedef " << data_seq << " " << sequence << ";"
               << "typedef " << sequence << "::iterator " << iterator << ";"
               << "typedef " << sequence << "::const_iterator " <<
              const_iterator << ";"
               << endl;

            // const seq&
            // name () const
            //
            os << "const " << sequence << "&" << endl
               << name << " () const;"
               << endl;

            // seq&
            // name ()
            //
            os << sequence << "&" << endl
               << name << " ();"
               << endl;
          }

          // _copy
          //
          if (!fl && clone)
            os << (exceptions ? "void" : "bool") << endl
               << "_copy (" << type << "&) const;"
               << endl;

          os << "private:" << endl;

          Sequence::contains (s, contains_data_);

          // Custom data.
          //
          if (cd)
            os << ecd_sequence (s) << " " << ecd_member (s) << ";";

          os << "};";

          if (s.max () != 1)
          {
            String const& sequence (esequence (s));
            String const& iterator (eiterator (s));
            String const& const_iterator (econst_iterator (s));

            // sequence & iterators
            //
            os << "typedef " << (fl ? fix_seq : var_seq) << "< " <<
              type << " > " << sequence << ";"
               << "typedef " << sequence << "::iterator " << iterator << ";"
               << "typedef " << sequence << "::const_iterator " <<
              const_iterator << ";"
               << endl;

            // const seq&
            // name () const
            //
            os << "const " << sequence << "&" << endl
               << name << " () const;"
               << endl;

            // seq&
            // name ()
            //
            os << sequence << "&" << endl
               << name << " ();"
               << endl;

          }
          else
          {
            if (s.min () == 0)
            {
              String const& present (epresent (s));

              // name_present
              //
              os << "bool" << endl
                 << present << " () const;"
                 << endl;

              if (fl)
                os << "void" << endl
                   << present << " (bool);"
                   << endl;
            }

            // const type&
            // name () const
            //
            os << "const " << type << "&" << endl
               << name << " () const;"
               << endl;

            // type&
            // name ()
            //
            os << type << "&" << endl
               << name << " ();"
               << endl;

            // void
            // name (const type& | type*)
            //
            os << "void" << endl
               << name << " (";

            if (fl)
              os << "const " << type << "&";
            else
              os << type << "*";

            os << ");"
               << endl;

            // type*
            // detach ()
            //
            if (detach && !fl)
            {
              os << type << "*" << endl
                 << edetach (s) << " ();"
                 << endl;
            }
          }
        }

      private:
        Traversal::ContainsParticle& contains_data_;
      };

      struct Complex: Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c),
              base_name_ (c, TypeName::base),

              // Data.
              //
              attribute_data_ (c),
              element_data_ (c),
              element_in_choice_data_ (c),
              all_data_ (c),
              choice_in_choice_data_ (c),
              choice_in_sequence_data_ (c),
              sequence_in_choice_data_ (c),
              sequence_in_sequence_data_ (c),

              // Functions and nested classes.
              //
              attribute_ (c),
              element_ (c),
              all_ (c, all_contains_data_),
              choice_in_choice_ (c, choice_contains_data_),
              choice_in_sequence_ (c, choice_contains_data_),
              sequence_in_choice_ (c, sequence_contains_data_),
              sequence_in_sequence_ (c, sequence_contains_data_)
        {
          // Data
          //
          attribute_names_data_ >> attribute_data_;

          all_data_ >> all_contains_data_ >> element_data_;

          sequence_in_choice_data_ >> sequence_contains_data_;
          sequence_in_sequence_data_ >> sequence_contains_data_;
          sequence_contains_data_ >> element_data_;
          sequence_contains_data_ >> choice_in_sequence_data_;
          sequence_contains_data_ >> sequence_in_sequence_data_;

          choice_in_choice_data_ >> choice_contains_data_;
          choice_in_sequence_data_ >> choice_contains_data_;
          choice_contains_data_ >> element_in_choice_data_;
          choice_contains_data_ >> choice_in_choice_data_;
          choice_contains_data_ >> sequence_in_choice_data_;

          contains_compositor_data_ >> all_data_;
          contains_compositor_data_ >> choice_in_sequence_data_;
          contains_compositor_data_ >> sequence_in_sequence_data_;

          // Functions and nested classes.
          //
          attribute_names_ >> attribute_;

          all_ >> all_contains_ >> element_;

          choice_in_choice_ >> choice_contains_;
          choice_in_sequence_ >> choice_contains_;
          choice_contains_ >> element_;
          choice_contains_ >> choice_in_choice_;
          choice_contains_ >> sequence_in_choice_;

          sequence_in_choice_ >> sequence_contains_;
          sequence_in_sequence_ >> sequence_contains_;
          sequence_contains_ >> element_;
          sequence_contains_ >> choice_in_sequence_;
          sequence_contains_ >> sequence_in_sequence_;

          contains_compositor_ >> all_;
          contains_compositor_ >> choice_in_sequence_;
          contains_compositor_ >> sequence_in_sequence_;
        }

        virtual void
        traverse (Type& c)
        {
          SemanticGraph::Context& cc (c.context ());
          String const& name (ename_custom (c));

          // We may not need to generate the class if this type is
          // being customized.
          //
          if (name)
          {
            bool fl (fixed_length (c));
            bool poly (polymorphic (c));
            bool restriction (restriction_p (c));
            bool cd (cc.count ("cd-name"));

            os << "// " << comment (c.name ()) << " (" <<
              (fl ? "fixed-length" : "variable-length") << ")" << endl
               << "//" << endl;

            os << "class " << name;

            if (c.inherits_p ())
            {
              os << ": public ";
              base_name_.dispatch (c.inherits ().base ());
            }

            os << "{";

            // copy c-tor & operator= (private)
            //
            if (!fl)
              os << "private:" << endl
                 << name << " (const " << name << "&);"
                 << name << "& operator= (const " << name << "&);"
                 << endl;

            // c-tor
            //
            os << "public:" << endl
               << name << " ();"
               << endl;

            // copy c-tor & operator= (public)
            //
            if (fl && !restriction)
              os << name << " (const " << name << "&);"
                 << name << "& operator= (const " << name << "&);"
                 << endl;

            // _clone
            //
            if (!fl && clone)
              os << (poly ? "virtual " : "") << name << "*" << endl
                 << "_clone () const;"
                 << endl;

            // d-tor
            //
            if (!restriction || poly)
              os << (poly ? "virtual\n" : "") << "~" << name << " ();"
                 << endl;

            if (!restriction)
            {
              Complex::names (c, attribute_names_);

              if (c.contains_compositor_p ())
                Complex::contains_compositor (c, contains_compositor_);
            }

            // Custom data.
            //
            if (cd)
            {
              String const& name (ecd_name (c));
              String const& sequence (ecd_sequence (c));
              String const& iterator (ecd_iterator (c));
              String const& const_iterator (ecd_const_iterator (c));

              os << "// Custom data." << endl
                 << "//" << endl;

              // sequence & iterators
              //
              os << "typedef " << data_seq << " " << sequence << ";"
                 << "typedef " << sequence << "::iterator " << iterator << ";"
                 << "typedef " << sequence << "::const_iterator " <<
                const_iterator << ";"
                 << endl;

              // const seq&
              // name () const
              //
              os << "const " << sequence << "&" << endl
                 << name << " () const;"
                 << endl;

              // seq&
              // name ()
              //
              os << sequence << "&" << endl
                 << name << " ();"
                 << endl;
            }

            if (poly && typeinfo)
            {
              os << "// Type information." << endl
                 << "//" << endl;

              os << "static const " <<
                (stl ? "::std::string&" : "char*") << endl
                 << "_static_type ();"
                 << endl;

              os << "virtual const " <<
                (stl ? "::std::string&" : "char*") << endl
                 << "_dynamic_type () const;"
                 << endl;
            }

            // _copy
            //
            if (!fl && clone)
              os << (exceptions ? "void" : "bool") << endl
                 << "_copy (" << name << "&) const;"
                 << endl;

            if (!restriction || cd)
              os << "private:" << endl;

            if (!restriction)
            {
              Complex::names (c, attribute_names_data_);

              if (c.contains_compositor_p ())
                Complex::contains_compositor (c, contains_compositor_data_);
            }

            // Custom data.
            //
            if (cd)
              os << ecd_sequence (c) << " " << ecd_member (c) << ";";

            os << "};";
          }

          // Generate include for custom type.
          //
          if (cc.count ("name-include"))
          {
            close_ns ();

            os << "#include " << process_include_path (
              cc.get<String> ("name-include")) << endl
               << endl;

            open_ns ();
          }
        }

      private:
        TypeName base_name_;

        // Data.
        //
        AttributeData attribute_data_;
        Traversal::Names attribute_names_data_;

        ElementData element_data_;
        ElementInChoiceData element_in_choice_data_;
        AllData all_data_;
        ChoiceInChoiceData choice_in_choice_data_;
        ChoiceInSequenceData choice_in_sequence_data_;
        SequenceInChoiceData sequence_in_choice_data_;
        SequenceInSequenceData sequence_in_sequence_data_;
        Traversal::ContainsParticle all_contains_data_;
        Traversal::ContainsParticle choice_contains_data_;
        Traversal::ContainsParticle sequence_contains_data_;

        Traversal::ContainsCompositor contains_compositor_data_;

        // Functions and nested classes.
        //
        Attribute attribute_;
        Traversal::Names attribute_names_;

        Element element_;
        All all_;
        ChoiceInChoice choice_in_choice_;
        ChoiceInSequence choice_in_sequence_;
        SequenceInChoice sequence_in_choice_;
        SequenceInSequence sequence_in_sequence_;
        Traversal::ContainsParticle all_contains_;
        Traversal::ContainsParticle choice_contains_;
        Traversal::ContainsParticle sequence_contains_;

        Traversal::ContainsCompositor contains_compositor_;
      };
    }

    void
    generate_tree_header (Context& ctx)
    {
      bool inline_ (ctx.options.generate_inline ());

      // Emit header includes.
      //
      {
        if (inline_)
        {
          ctx.os << "#ifndef XSDE_DONT_INCLUDE_INLINE" << endl
                 << "#define XSDE_DONT_INCLUDE_INLINE" << endl
                 << endl;
        }

        Traversal::Schema schema;
        Includes includes (ctx, Includes::header);

        schema >> includes;

        schema.dispatch (ctx.schema_root);

        if (inline_)
        {
          ctx.os << "#undef XSDE_DONT_INCLUDE_INLINE" << endl
                 << "#else" << endl
                 << endl;

          schema.dispatch (ctx.schema_root);

          ctx.os << "#endif // XSDE_DONT_INCLUDE_INLINE" << endl
                 << endl;
        }
      }

      {
        Traversal::Schema schema;

        Sources sources;
        Traversal::Names names_ns, names;

        Namespace ns (ctx, true);

        List list (ctx);
        Union union_ (ctx);
        Complex complex (ctx);
        Enumeration enumeration (ctx, complex);

        schema >> sources >> schema;
        schema >> names_ns >> ns >> names;

        names >> list;
        names >> union_;
        names >> complex;
        names >> enumeration;

        schema.dispatch (ctx.schema_root);
      }

      // Emit inline includes.
      //
      if (inline_)
      {
        ctx.os << "#ifndef XSDE_DONT_INCLUDE_INLINE" << endl
               << endl;

        Traversal::Schema schema;
        Includes ixx_includes (ctx, Includes::inline_);
        schema >> ixx_includes;

        schema.dispatch (ctx.schema_root);

        ctx.os << "#endif // XSDE_DONT_INCLUDE_INLINE" << endl
               << endl;
      }
    }
  }
}
