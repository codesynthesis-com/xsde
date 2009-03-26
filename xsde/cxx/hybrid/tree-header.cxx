// file      : xsd/cxx/hybrid/tree-header.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
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
      struct List : Traversal::List, Context
      {
        List (Context& c)
            : Context (c), base_name_ (c, TypeName::seq)
        {
        }

        virtual Void
        traverse (Type& l)
        {
          SemanticGraph::Context& lc (l.context ());
          String const& name (ename_custom (l));

          // We may not need to generate the class if this type is
          // being customized.
          //
          if (name)
          {
            Boolean cd (lc.count ("cd-name"));
            Boolean poly (polymorphic (l));

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
               << name << " ();";

            // d-tor
            //
            if (poly)
              os << "virtual ~" << name << " ();";

            os << endl;

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

        virtual Void
        traverse (Type& u)
        {
          SemanticGraph::Context& uc (u.context ());
          String const& name (ename_custom (u));

          // We may not need to generate the class if this type is
          // being customized.
          //
          if (name)
          {
            Boolean fl (fixed_length (u));
            Boolean poly (polymorphic (u));
            Boolean cd (uc.count ("cd-name"));

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
            os << name << " ();";

            // d-tor
            //
            if (!stl || poly)
              os << (poly ? "virtual " : "") << "~" << name << " ();";

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

                        Traversal::AnyType,
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
            : Context (c), nested_ (false)
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

        virtual Void
        dispatch (SemanticGraph::Node& n)
        {
          found_ = false;
          NodeBase::dispatch (n);
        }

        virtual Void
        traverse (SemanticGraph::Compositor& c)
        {
          Boolean top (false);

          if (!nested_)
          {
            nested_ = true;
            top = true;
          }

          Traversal::Compositor::traverse (c);

          if (top)
          {
            nested_ = false;

            if (!found_)
              os << "char"; // Empty compositor.
          }
        }

        virtual Void
        traverse (SemanticGraph::List&)
        {
          align_type ("size_t");
        }

        virtual Void
        traverse (SemanticGraph::Union&)
        {
          align_type ("size_t"); // std::string
        }

        virtual Void
        traverse (SemanticGraph::Complex& c)
        {
          // Keep track of the nested calls for base types.
          //
          Boolean top (false);

          if (!nested_)
          {
            nested_ = true;
            top = true;
          }

          Complex::inherits (c, inherits_);

          if (!found_)
          {
            Complex::names (c, attribute_names_);

            if (!found_ && c.contains_compositor_p ())
              Complex::contains_compositor (c, contains_compositor_);
          }

          if (top)
          {
            nested_ = false;

            if (!found_)
              os << "char"; // Empty class.
          }
        }

        // anyType & anySimpleType.
        //
        virtual Void
        traverse (SemanticGraph::AnyType&)
        {
          align_type ("char");
        }

        virtual Void
        traverse (SemanticGraph::AnySimpleType&)
        {
          align_type ("char");
        }

        // Boolean.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Boolean&)
        {
          align_type ("bool");
        }

        // Integral types.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Byte&)
        {
          align_type ("signed char");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedByte&)
        {
          align_type ("unsigned char");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Short&)
        {
          align_type ("short");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedShort&)
        {
          align_type ("unsigned short");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Int&)
        {
          align_type ("int");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedInt&)
        {
          align_type ("unsigned int");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Long&)
        {
          align_type (options.value<CLI::no_long_long> ()
                      ? "long"
                      : "long long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedLong&)
        {
          align_type (options.value<CLI::no_long_long> ()
                      ? "unsigned long"
                      : "unsigned long long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Integer&)
        {
          align_type ("long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NonPositiveInteger&)
        {
          align_type ("long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NonNegativeInteger&)
        {
          align_type ("unsigned long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::PositiveInteger&)
        {
          align_type ("unsigned long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NegativeInteger&)
        {
          align_type ("long");
        }

        // Floats.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Float&)
        {
          align_type ("float");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Double&)
        {
          align_type ("double");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Decimal&)
        {
          align_type ("double");
        }

        // Strings.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::String&)
        {
          align_type ("size_t"); // std::string
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NormalizedString&)
        {
          align_type ("size_t"); // std::string
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Token&)
        {
          align_type ("size_t"); // std::string
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NameToken&)
        {
          align_type ("size_t"); // std::string
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Name&)
        {
          align_type ("size_t"); // std::string
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NCName&)
        {
          align_type ("size_t"); // std::string
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Language&)
        {
          align_type ("size_t"); // std::string
        }

        // Qualified name.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::QName&)
        {
          align_type ("size_t"); // std::string
        }


        // ID/IDREF.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Id&)
        {
          align_type ("size_t"); // std::string
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::IdRef&)
        {
          align_type ("size_t"); // std::string
        }

        // URI.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::AnyURI&)
        {
          align_type ("size_t"); // std::string
        }


        // Date/time.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Date&)
        {
          align_type ("bool");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::DateTime&)
        {
          align_type ("bool");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Duration&)
        {
          align_type ("bool");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Day&)
        {
          align_type ("bool");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Month&)
        {
          align_type ("bool");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::MonthDay&)
        {
          align_type ("bool");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Year&)
        {
          align_type ("bool");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::YearMonth&)
        {
          align_type ("bool");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Time&)
        {
          align_type ("bool");
        }

        // Entity.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Entity&)
        {
          align_type ("size_t"); // std::string
        }

      private:
        Void
        align_type (Char const* t)
        {
          if (!found_)
          {
            os << t;
            found_ = true;
          }
        }

      private:
        Boolean found_;
        Boolean nested_;

      private:
        Traversal::Inherits inherits_;

        Traversal::Attribute attribute_;
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

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          SemanticGraph::Type& t (a.type ());

          var_.dispatch (t);
          os << " " << emember (a) << ";";

          if (a.optional () && fixed_length (t))
            os << "bool " << epresent_member (a) << ";";
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

        virtual Void
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
              os << "bool " << epresent_member (e) << ";";
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

        virtual Void
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

        virtual Void
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
                 << "bool " << epresent_member (a) << ";";
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

        virtual Void
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
                 << "bool " << epresent_member (c) << ";";
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

        virtual Void
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

        virtual Void
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
                 << "bool " << epresent_member (s) << ";";
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

        virtual Void
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

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          os << "// " << comment (a.name ()) << endl
             << "// " << endl;

          String const& name (ename (a));
          SemanticGraph::Type& t (a.type ());

          if (a.optional ())
          {
            String const& present (epresent (a));

            os << "bool" << endl
               << present << " () const;"
               << endl;

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
          if (detach && !fixed_length (t))
          {
            arg_.dispatch (t);
            os << endl
               << edetach (a) << " ();"
               << endl;
          }
        }

      private:
        TypeName ro_ret_;
        TypeName ret_;
        TypeName arg_;
      };

      struct Element: Traversal::Element, virtual Context
      {
        Element (Context& c)
            : Context (c),
              seq_ (c, TypeName::seq),
              ro_ret_ (c, TypeName::ro_ret),
              ret_ (c, TypeName::ret),
              arg_ (c, TypeName::arg)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          os << "// " << comment (e.name ()) << endl
             << "// " << endl;

          String const& name (ename (e));
          SemanticGraph::Type& t (e.type ());

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
            if (detach && !fixed_length (t))
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

        virtual Void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}
          // and it can only contain particles.
          //
          if (a.min () == 0)
          {
            Boolean fl (fixed_length (a));
            Boolean cd (a.context ().count ("cd-name"));

            String const& name (ename (a));
            String const& type (etype (a));
            String const& present (epresent (a));

            os << "// " << comment (name) << " (" <<
              (fl ? "fixed-length" : "variable-length") << ")" << endl
               << "// " << endl;

            os << "class " << type
               << "{";

            // c-tor & d-tor
            //
            os << "public:" << endl
               << type << " ();"
               << "~" << type << " ();";

            // copy c-tor & operator=
            //
            if (!fl)
              os << endl
                 << "private:" << endl;

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

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          emit_tag (etag (e));
        }

        virtual Void
        traverse (SemanticGraph::Any& a)
        {
          emit_tag (etag (a));
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          emit_tag (etag (c));
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          emit_tag (etag (s));
        }

        virtual Void
        emit_tag (String const& tag)
        {
          if (first_)
            first_ = false;
          else
            os << "," << endl;

          os << tag;
        }

      private:
        Boolean first_;
      };

      struct ChoiceInSequence: Traversal::Choice, Context
      {
        ChoiceInSequence (Context& c,
                          Traversal::ContainsParticle& contains_data)
            : Context (c), contains_data_ (contains_data)
        {
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          UnsignedLong min (c.min ()), max (c.max ());

          String const& name (ename (c));

          Boolean fl;
          String type;

          if (max != 1 || min == 0)
          {
            type = etype (c);

            fl = fixed_length (c);

            os << "// " << comment (name) << " (" <<
            (fl ? "fixed-length" : "variable-length") << ")" << endl
             << "// " << endl;

            os << "class " << type
               << "{";

            // c-tor & d-tor
            //
            os << "public:" << endl
               << type << " ();"
               << "~" << type << " ();";

            // copy c-tor & operator=
            //
            if (!fl)
              os << endl
                 << "private:" << endl;

            os << type << " (const " << type << "&);"
               << type << "& operator= (const " << type << "&);"
               << endl;

            if (!fl)
              os << "public:" << endl;
          }
          else
          {
            os << "// " << comment (name) << endl
               << "// " << endl;
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
            Boolean cd (c.context ().count ("cd-name"));

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

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          // When shoice is in choice we generate nested class even
          // for min == max == 1.
          //
          UnsignedLong min (c.min ()), max (c.max ());

          Boolean fl (fixed_length (c));
          Boolean cd (c.context ().count ("cd-name"));

          String const& name (ename (c));
          String const& type (etype (c));

          os << "// " << comment (name) << " (" <<
            (fl ? "fixed-length" : "variable-length") << ")" << endl
             << "// " << endl;

          os << "class " << type
             << "{";

          // c-tor & d-tor
          //
          os << "public:" << endl
             << type << " ();"
             << "~" << type << " ();";

          // copy c-tor & operator=
          //
          if (!fl)
            os << endl
               << "private:" << endl;

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

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          UnsignedLong min (s.min ()), max (s.max ());

          if (max == 1 && min == 1)
          {
            Sequence::contains (s);
            return;
          }

          Boolean fl (fixed_length (s));
          Boolean cd (s.context ().count ("cd-name"));

          String const& name (ename (s));
          String const& type (etype (s));

          os << "// " << comment (name) << " (" <<
            (fl ? "fixed-length" : "variable-length") << ")" << endl
             << "// " << endl;

          os << "class " << type
             << "{";

          // c-tor & d-tor
          //
          os << "public:" << endl
             << type << " ();"
             << "~" << type << " ();";

          // copy c-tor & operator=
          //
          if (!fl)
            os << endl
               << "private:" << endl;

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

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          // When sequence is in choice we generate nested class even
          // for min == max == 1.
          //
          Boolean fl (fixed_length (s));
          Boolean cd (s.context ().count ("cd-name"));

          String const& name (ename (s));
          String const& type (etype (s));

          os << "// " << comment (name) << " (" <<
            (fl ? "fixed-length" : "variable-length") << ")" << endl
             << "// " << endl;

          os << "class " << type
             << "{";

          // c-tor & d-tor
          //
          os << "public:" << endl
             << type << " ();"
             << "~" << type << " ();";

          // copy c-tor & operator=
          //
          if (!fl)
            os << endl
               << "private:" << endl;

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

      struct Complex : Traversal::Complex, Context
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

        virtual Void
        traverse (Type& c)
        {
          SemanticGraph::Context& cc (c.context ());
          String const& name (ename_custom (c));

          // We may not need to generate the class if this type is
          // being customized.
          //
          if (name)
          {
            Boolean fl (fixed_length (c));
            Boolean poly (polymorphic (c));
            Boolean restriction (restriction_p (c));
            Boolean cd (cc.count ("cd-name"));

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

            // c-tor
            //
            os << "public:" << endl
               << name << " ();";

            // d-tor
            //
            if (!restriction || poly)
              os << (poly ? "virtual " : "") << "~" << name << " ();";

            // copy c-tor & operator=
            //
            if (!fl)
              os << endl
                 << "private:" << endl;

            if (!fl || !restriction)
              os << name << " (const " << name << "&);"
                 << name << "& operator= (const " << name << "&);"
                 << endl;

            if ((!restriction && !fl) || cd)
              os << "public:" << endl;

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

    Void
    generate_tree_header (Context& ctx)
    {
      Boolean inline_ (ctx.options.value<CLI::generate_inline> ());

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

        Traversal::Sources sources;
        Traversal::Names names_ns, names;

        Namespace ns (ctx, true);

        List list (ctx);
        Union union_ (ctx);
        Complex complex (ctx);
        //Enumeration enumeration (ctx);

        schema >> sources >> schema;
        schema >> names_ns >> ns >> names;

        names >> list;
        names >> union_;
        names >> complex;
        //names >> enumeration;

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
