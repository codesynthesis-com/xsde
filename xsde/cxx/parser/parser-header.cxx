// file      : xsde/cxx/parser/parser-header.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/parser/parser-header.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

namespace CXX
{
  namespace Parser
  {
    namespace
    {
      struct Enumeration: Traversal::Enumeration, Context
      {
        Enumeration (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& e)
        {
          String const& name (ename (e));
          SemanticGraph::Type& base (e.inherits ().base ());
          String fq_base (fq_name (base));

          Boolean enum_facets (false); // Whether we need to set enum facets.
          if (validation)
          {
            StringBasedType t (enum_facets);
            t.dispatch (e);
          }

          Boolean facets (enum_facets || has_facets (e));

          os << "class " << name << ": public " <<
            (mixin ? "virtual " : "") << fq_base
             << "{"
             << "public:" << endl
             << "// Parser callbacks. Override them in your " <<
            "implementation." << endl
             << "//" << endl;

          os << "// virtual void" << endl
             << "// pre ();" << endl
             << endl;

          String const& ret (ret_type (e));
          String const& base_ret (ret_type (base));

          Boolean same (ret == base_ret);

          os << "virtual " << ret << endl
             << post_name (e) << " ()" <<
            (same || ret == L"void" ? ";" : " = 0;");

          if (poly_code)
          {
            os << endl
               << "public:" << endl
               << "static const char*" << endl
               << "_static_type ();"
               << endl
               << "virtual const char*" << endl
               << "_dynamic_type () const;";
          }

          if (facets || tiein)
          {
            os << endl
               << "// Constructor." << endl
               << "//" << endl;

            if (tiein)
              os << name << " (" << fq_base << "* tiein);";
            else
              os << name << " ();";
          }

          if (tiein)
          {
            os << endl
               << "// Implementation details." << endl
               << "//" << endl;

            // If our base has pure virtual post, override it here.
            //
            Boolean base_same (
              base.inherits_p () &&
              base_ret == ret_type (base.inherits ().base ()));

            if (!(base_same || base_ret == L"void"))
              os << "virtual " << base_ret << endl
                 << post_name (base) << " ();"
                 << endl;

            os << "protected:" << endl
               << name << "* " << etiein (e) << ";"
               << name << " (" << name << "*, void*);";
          }

          if (enum_facets)
          {
            UnsignedLong enum_count (0);

            for (Type::NamesIterator i (e.names_begin ()), end (e.names_end ());
                 i != end; ++i)
              ++enum_count;

            os << endl
               << "protected:" << endl
               << "static const char* const _xsde_" << name << "_enums_[" <<
              enum_count << "UL];";
          }

          os << "};";
        }
      };


      //
      //
      struct List: Traversal::List, Context
      {
        List (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& l)
        {
          String const& name (ename (l));
          SemanticGraph::Type& t (l.argumented ().type ());
          String item_type (fq_name (t));

          String item (unclash (name, "item"));

          os << "class " << name << ": public " << list_base
             << "{"
             << "public:" << endl
             << "// Parser callbacks. Override them in your " <<
            "implementation." << endl
             << "//" << endl;

          // pre
          //
          os << "// virtual void" << endl
             << "// pre ();" << endl
             << endl;

          // item
          //
          String const& arg (arg_type (t));

          os << "virtual void" << endl
             << item;

          if (arg == L"void")
            os << " ();";
          else
            os << " (" << arg << ");";

          os << endl;

          // post
          //
          String const& ret (ret_type (l));

          os << "virtual " << ret << endl
             << post_name (l) << " ()" << (ret == L"void" ? ";" : " = 0;")
             << endl;


          //
          //
          os << "// Parser construction API." << endl
             << "//" << endl;

          // item_parser
          //
          os << "void" << endl
             << unclash (name, "item_parser") << " (" << item_type << "&);"
             << endl;

          // parsers
          //
          os << "void" << endl
             << "parsers (" << item_type << "& /* item */);"
             << endl;

          if (reset)
            os << "virtual void" << endl
               << "_reset ();"
               << endl;

          // c-tor
          //
          os << "// Constructor." << endl
             << "//" << endl
             << name << " ();"
             << endl;

          if (poly_code)
          {
            os << "public:" << endl
               << "static const char*" << endl
               << "_static_type ();"
               << endl
               << "virtual const char*" << endl
               << "_dynamic_type () const;"
               << endl;
          }

          //
          //
          os << "// Implementation details." << endl
             << "//" << endl
             << "protected:" << endl;

          if (tiein)
          {
            os << name << "* " << etiein (l) << ";"
               << name << " (" << name << "*, void*);"
               << endl;
          }

          os << "virtual void" << endl
             << "_xsde_parse_item (const " << string_type << "&);"
             << endl;

          os << "protected:" << endl
             << item_type << "* _xsde_" << item << "_;"
             << "};";
        }
      };


      struct Union: Traversal::Union, Context
      {
        Union (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& u)
        {
          String const& name (ename (u));

          os << "class " << name << ": public " << simple_base
             << "{"
             << "public:" << endl
             << "// Parser callbacks. Override them in your " <<
            "implementation." << endl
             << "//" << endl;

          os << "// virtual void" << endl
             << "// pre ();" << endl
             << "//" << endl
             << "// virtual void" << endl
             << "// _characters (const " << string_type << "&);" << endl
             << endl;

          String const& ret (ret_type (u));

          os << "virtual " << ret << endl
             << post_name (u) << " ()" << (ret == L"void" ? ";" : " = 0;");

          if (poly_code)
          {
            os << endl
               << "public:" << endl
               << "static const char*" << endl
               << "_static_type ();"
               << endl
               << "virtual const char*" << endl
               << "_dynamic_type () const;";
          }

          if (tiein)
          {
            // c-tor
            //
            os << endl
               << "// Constructor." << endl
               << "//" << endl
               << name << " ();"
               << endl;

            //
            //
            os << "// Implementation details." << endl
               << "//" << endl
               << "protected:" << endl
               << name << "* " << etiein (u) << ";"
               << name << " (" << name << "*, void*);";
          }

          os << "};";
        }
      };

      //
      // Callbacks.
      //

      struct ParticleTag: Traversal::Particle, Context
      {
        ParticleTag (Context& c)
            : Context (c), first_ (true)
        {
        }

        virtual Void
        traverse (Type& p)
        {
          if (first_)
            first_ = false;
          else
            os << "," << endl;

          os << etag (p);
        }

      private:
        Boolean first_;
      };

      struct CompositorCallback: Traversal::All,
                                 Traversal::Choice,
                                 Traversal::Sequence,
                                 Context
      {
        CompositorCallback (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::All& a)
        {
          if (correspondent (a) == 0)
          {
            // For the all compositor, maxOccurs=1 and minOccurs={0,1}.
            //
            if (a.min () == 0)
            {
              os << "virtual void" << endl
                 << epresent (a) << " ();"
                 << endl;
            }
          }

          Traversal::All::traverse (a);
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.contains_begin () == c.contains_end ())
            return;

          if (correspondent (c) == 0)
          {
            os << "enum " << earm_tag (c)
               << "{";

            {
              ParticleTag particle (*this);
              Traversal::ContainsParticle contain_particle (particle);
              Traversal::Choice::contains (c, contain_particle);
            }

            os << "};";

            os << "virtual void" << endl
               << earm (c) << " (" << earm_tag (c) << ");"
               << endl;
          }

          Traversal::Choice::traverse (c);
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          // Root compositor that models inheritance by extension
          // may not have an association so we may fall through
          // in to the 'if' case even though this is a restriction.
          // This is ok since such a compositor always has max ==
          // min == 1 and so nothing is generated.
          //
          if (SemanticGraph::Compositor* b = correspondent (s))
          {
            // Add the *_present callback if this is a restriction
            // of sequence to optional.
            //
            if (b->max () != 1 && s.min () == 0)
            {
              os << "virtual void" << endl
                 << epresent (s) << " ();"
                 << endl;
            }
          }
          else
          {
            if (s.max () != 1)
            {
              os << "virtual void" << endl
                 << enext (s) << " ();"
                 << endl;
            }
            else if (s.min () == 0)
            {
              os << "virtual void" << endl
                 << epresent (s) << " ();"
                 << endl;
            }
          }

          Traversal::Sequence::traverse (s);
        }
      };

      struct ParticleCallback: Traversal::Element, Context
      {
        ParticleCallback (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          if (correspondent (e) == 0)
          {
            String const& arg (arg_type (e.type ()));

            os << "virtual void" << endl
               << ename (e);

            if (arg == L"void")
              os << " ();";
            else
              os << " (" << arg << ");";

            os << endl;
          }
        }
      };

      struct AttributeCallback: Traversal::Attribute, Context
      {
        AttributeCallback (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          String const& arg (arg_type (a.type ()));

          os << "virtual void" << endl
             << ename (a);

          if (arg == L"void")
            os << " ();";
          else
            os << " (" << arg << ");";

          os << endl;
        }
      };


      //
      //
      struct ParticleAccessor: Traversal::Element, Context
      {
        ParticleAccessor (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          String const& parser (eparser (e));

          os << "void" << endl
             << parser << " (" << fq_name (e.type ()) << "&);"
             << endl;

          if (poly_code && !anonymous (e.type ()))
          {
            os << "void" << endl
               << parser << " (" << parser_map << "&);"
               << endl;
          }
        }
      };

      struct AttributeAccessor: Traversal::Attribute, Context
      {
        AttributeAccessor (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& a)
        {
          os << "void" << endl
             << eparser (a) << " (" << fq_name (a.type ()) << "&);"
             << endl;
        }
      };


      //
      //
      struct ParticleMember: Traversal::Element, Context
      {
        ParticleMember (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          String type (fq_name (e.type ()));

          os << type << "* " << emember (e) << ";";

          if (poly_code && !anonymous (e.type ()))
          {
            os << type << "* " << emember_cache (e) << ";"
               << parser_map << "* " << emember_map (e) << ";"
               << endl;
          }
        }
      };

      struct AttributeMember: Traversal::Attribute, Context
      {
        AttributeMember (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& a)
        {
          os << fq_name (a.type ()) << "* " << emember (a) << ";";
        }
      };

      //
      //
      struct Particle: Traversal::All,
                       Traversal::Choice,
                       Traversal::Sequence,
                       Context
      {
        Particle (Context& c)
            : Context (c)
        {
          *this >> contains_particle_ >> *this;
        }


        virtual Void
        traverse (SemanticGraph::All& a)
        {
          if (!a.context().count ("p:comp-number"))
            return;

          UnsignedLong state_count (
            a.context().get<UnsignedLong> ("p:state-count"));

          os << "void" << endl
             << "all_0 (unsigned long&," << endl
             << "unsigned char*," << endl
             << "const " << string_type << "&," << endl
             << "const " << string_type << "&," << endl;

          if (poly_runtime)
            os << "const char*," << endl;

          os << "bool);"
             << endl
             << "unsigned char v_all_first_[" << state_count << "UL];"
             << "::xsde::cxx::parser::validating::all_stack v_all_count_;"
             << endl;
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          if (!c.context().count ("p:comp-number"))
            return;

          UnsignedLong n (c.context ().get<UnsignedLong> ("p:comp-number"));

          os << "void" << endl
             << "choice_" << n << " (unsigned long&," << endl
             << "unsigned long&," << endl
             << "const " << string_type << "&," << endl
             << "const " << string_type << "&," << endl;

          if (poly_runtime)
            os << "const char*," << endl;

          os << "bool);"
             << endl;

          Traversal::Choice::traverse (c);
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          if (!s.context().count ("p:comp-number"))
            return;

          UnsignedLong n (s.context ().get<UnsignedLong> ("p:comp-number"));

          os << "void" << endl
             << "sequence_" << n << " (unsigned long&," << endl
             << "unsigned long&," << endl
             << "const " << string_type << "&," << endl
             << "const " << string_type << "&," << endl;

          if (poly_runtime)
            os << "const char*," << endl;

          os << "bool);"
             << endl;

          Traversal::Sequence::traverse (s);
        }

      private:
        Traversal::ContainsParticle contains_particle_;
      };


      //
      //
      struct AttributeValidationState: Traversal::Attribute, Context
      {
        AttributeValidationState (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& a)
        {
          if (!a.optional_p ())
          {
            os << "bool " << ename (a) << ";";
          }
        }
      };

      //
      //
      struct Complex : Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c),
              compositor_callback_val_ (c),
              particle_callback_ (c),
              attribute_callback_ (c),
              particle_accessor_ (c),
              attribute_accessor_ (c),
              particle_member_ (c),
              attribute_member_ (c),
              attribute_validation_state_ (c)
        {
          // Callback.
          //
          if (validation)
          {
            contains_compositor_callback_ >> compositor_callback_val_;
            compositor_callback_val_ >> contains_particle_callback_;
            contains_particle_callback_ >> compositor_callback_val_;
          }
          else
          {
            contains_compositor_callback_ >> compositor_callback_non_val_;
            compositor_callback_non_val_ >> contains_particle_callback_;
            contains_particle_callback_ >> compositor_callback_non_val_;
          }

          contains_particle_callback_ >> particle_callback_;

          names_attribute_callback_ >> attribute_callback_;

          // Accessor.
          //
          contains_compositor_accessor_ >> compositor_accessor_;
          compositor_accessor_ >> contains_particle_accessor_;
          contains_particle_accessor_ >> compositor_accessor_;
          contains_particle_accessor_ >> particle_accessor_;

          names_attribute_accessor_ >> attribute_accessor_;

          // Member.
          //
          contains_compositor_member_ >> compositor_member_;
          compositor_member_ >> contains_particle_member_;
          contains_particle_member_ >> compositor_member_;
          contains_particle_member_ >> particle_member_;

          names_attribute_member_ >> attribute_member_;

          //
          //
          names_attribute_validation_state_ >> attribute_validation_state_;
        }

        virtual Void
        traverse (Type& c)
        {
          String const& name (ename (c));

          // In case of an inheritance-by-restriction, we don't need to
          // generate parser callbacks, etc. since they are the same as
          // in the base. We only need the parsing/validation code.
          //
          Boolean hb (c.inherits_p ());
          Boolean restriction (restriction_p (c));
          Boolean facets (has_facets (c));

          Boolean he (has<Traversal::Element> (c));
          Boolean ha (has<Traversal::Attribute> (c));

          Boolean hae (has_particle<Traversal::Any> (c));
          Boolean haa (has<Traversal::AnyAttribute> (c));

          Boolean hra (false); // Has required attribute.
          if (ha)
          {
            RequiredAttributeTest test (hra);
            Traversal::Names names_test (test);
            names (c, names_test);
          }

          //
          //
          os << "class " << name << ": public ";

          if (hb)
            os << (mixin ? "virtual " : "") << fq_name (c.inherits ().base ());
          else
            os << complex_base;

          os << "{"
             << "public:" << endl
             << "// Parser callbacks. Override them in your " <<
            "implementation." << endl
             << "//" << endl;

          // pre
          //
          os << "// virtual void" << endl
             << "// pre ();" << endl
             << endl;

          // Member callbacks.
          //
          if (!restriction)
          {
            if (ha)
            {
              os << "// Attributes." << endl
                 << "//" << endl;

              names (c, names_attribute_callback_);
            }
          }

          if (!restriction || validation)
          {
            if (he || hae)
            {
              if (!restriction)
                os << "// Elements." << endl
                   << "//" << endl;

              contains_compositor (c, contains_compositor_callback_);
            }
          }

          // post
          //
          String const& ret (ret_type (c));
          Boolean same (hb && ret == ret_type (c.inherits ().base ()));

          os << "virtual " << ret << endl
             << post_name (c) << " ()" <<
            (same || ret == L"void" ? ";" : " = 0;")
             << endl;

          //
          //
          if (!restriction && (he || ha))
          {
            os << "// Parser construction API." << endl
               << "//" << endl;

            os << "void" << endl
               << "parsers (";

            {
              ParserParamDecl decl (*this, false);
              decl.traverse (c);
            }

            os << ");"
               << endl;

            if (ha)
            {
              os << "// Individual attribute parsers." << endl
                 << "//" << endl;

              names (c, names_attribute_accessor_);
            }

            if (he)
            {
              os << "// Individual element parsers." << endl
                 << "//" << endl;

              contains_compositor (c, contains_compositor_accessor_);
            }
          }

          if (!restriction && (he || ha) && reset)
          {
            os << "virtual void" << endl
               << "_reset ();"
               << endl;
          }

          // Default c-tor.
          //
          if (tiein || facets ||
              (!restriction && (he || ha)) ||
              (validation && (he || hae || hra)))
          {
            os << "// Constructor." << endl
               << "//" << endl;

            if (hb && tiein)
              os << name << " (" << fq_name (c.inherits ().base ()) <<
                "* tiein);"
                 << endl;
            else
              os << name << " ();"
                 << endl;
          }

          if (poly_code)
          {
            os << "public:" << endl
               << "static const char*" << endl
               << "_static_type ();"
               << endl
               << "virtual const char*" << endl
               << "_dynamic_type () const;"
               << endl;
          }

          // Implementation.
          //
          if (tiein || he || ha || (validation && (hae || haa)))
          {
            os << "// Implementation details." << endl
               << "//" << endl;
          }

          if (tiein)
          {
            if (hb)
            {
              // If our base has pure virtual post, override it here.
              //
              SemanticGraph::Type& base (c.inherits ().base ());

              String const& base_ret (ret_type (base));

              Boolean base_same (
                base.inherits_p () &&
                base_ret == ret_type (base.inherits ().base ()));

              if (!(base_same || base_ret == L"void"))
                os << "virtual " << base_ret << endl
                   << post_name (base) << " ();"
                   << endl;
            }

            os << "protected:" << endl
               << name << "* " << etiein (c) << ";"
               << name << " (" << name << "*, void*);"
               << endl;
          }

          // element
          //
          if (he || (validation && hae))
          {
            os << "protected:" << endl;

            // _start_element_impl
            //
            os << "virtual bool" << endl
               << "_start_element_impl (const " << string_type << "&," << endl
               << "const " << string_type << "&";

            if (poly_runtime)
              os << "," << endl
                 << "const char*";

            os << ");"
               << endl;

            // end_element
            //
            os << "virtual bool" << endl
               << "_end_element_impl (const " << string_type << "&," << endl
               << "const " << string_type << "&);"
               << endl;
          }

          // attribute
          //
          if (validation)
          {
            if (ha || haa)
              os << "protected:" << endl;

            if (ha)
            {
              os << "virtual bool" << endl
                 << "_attribute_impl_phase_one (const " << string_type <<
                "&," << endl
                 << "const " << string_type << "&," << endl
                 << "const " << string_type << "&);" << endl
                 << endl;
            }

            if (haa)
            {
              os << "virtual bool" << endl
                 << "_attribute_impl_phase_two (const " << string_type <<
                "&," << endl
                 << "const " << string_type << "&," << endl
                 << "const " << string_type << "&);"
                 << endl;
            }
          }
          else
          {
            if (ha)
            {
              os << "protected:" << endl
                 << "virtual bool" << endl
                 << "_attribute_impl (const " << string_type << "&," << endl
                 << "const " << string_type << "&," << endl
                 << "const " << string_type << "&);"
                 << endl;
            }
          }

          // characters
          //
          if (validation && c.mixed_p ())
          {
            os << "protected:" << endl
               << "virtual bool" << endl
               << "_characters_impl (const " << string_type << "&);"
               << endl;
          }

          if (!restriction && (he || ha))
          {
            os << "protected:" << endl;

            if (ha)
              names (c, names_attribute_member_);

            if (he)
              contains_compositor (c, contains_compositor_member_);

            os << endl;
          }

          if (validation && (he || hae))
          {
            UnsignedLong depth (c.context ().get<UnsignedLong> ("p:depth"));

            // Has to be public for some broken compilers (e.g., GH C++).
            //
            os << "public:" << endl;

            os << "struct v_state_descr_"
               << "{"
               << "void (" << fq_name (c) << "::*func) (" << endl
               << "unsigned long&," << endl
               << "unsigned long&," << endl
               << "const " << string_type << "&," << endl
               << "const " << string_type << "&," << endl;

            if (poly_runtime)
              os << "const char*," << endl;

            os << "bool);"
               << "unsigned long state;"
               << "unsigned long count;"
               << "};";

            // Allocate one extra slot for the special state.
            //
            os << "struct v_state_"
               << "{"
               << "v_state_descr_ data[" << depth + 1 << "UL];"
               << "unsigned long size;"
               << "};";

            os << "protected:" << endl;

            os << "v_state_ v_state_first_;"
               << "::xsde::cxx::stack v_state_stack_;"
               << endl;

            os << "virtual void" << endl
               << "_pre_e_validate ();"
               << endl;

            os << "virtual void" << endl
               << "_post_e_validate ();"
               << endl;

            Particle t (*this);
            t.dispatch (c.contains_compositor ().compositor ());
          }

          if (validation && hra)
          {
            os << "protected:" << endl;

            os << "struct v_state_attr_"
               << "{";

            names (c, names_attribute_validation_state_);

            os << "};";

            os << "v_state_attr_ v_state_attr_first_;"
               << "::xsde::cxx::stack v_state_attr_stack_;"
               << endl;

            os << "virtual void" << endl
               << "_pre_a_validate ();"
               << endl;

            os << "virtual void" << endl
               << "_post_a_validate ();"
               << endl;
          }

          os << "};";
        }

      private:
        //
        //
        CompositorCallback compositor_callback_val_;
        Traversal::Compositor compositor_callback_non_val_;
        ParticleCallback particle_callback_;
        Traversal::ContainsCompositor contains_compositor_callback_;
        Traversal::ContainsParticle contains_particle_callback_;

        AttributeCallback attribute_callback_;
        Traversal::Names names_attribute_callback_;

        //
        //
        Traversal::Compositor compositor_accessor_;
        ParticleAccessor particle_accessor_;
        Traversal::ContainsCompositor contains_compositor_accessor_;
        Traversal::ContainsParticle contains_particle_accessor_;

        AttributeAccessor attribute_accessor_;
        Traversal::Names names_attribute_accessor_;

        //
        //
        Traversal::Compositor compositor_member_;
        ParticleMember particle_member_;
        Traversal::ContainsCompositor contains_compositor_member_;
        Traversal::ContainsParticle contains_particle_member_;

        AttributeMember attribute_member_;
        Traversal::Names names_attribute_member_;

        //
        //
        AttributeValidationState attribute_validation_state_;
        Traversal::Names names_attribute_validation_state_;
      };

      struct FundType : Context,

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
                        Traversal::Fundamental::NameTokens,
                        Traversal::Fundamental::NCName,
                        Traversal::Fundamental::Language,

                        Traversal::Fundamental::QName,

                        Traversal::Fundamental::Id,
                        Traversal::Fundamental::IdRef,
                        Traversal::Fundamental::IdRefs,

                        Traversal::Fundamental::AnyURI,

                        Traversal::Fundamental::Base64Binary,
                        Traversal::Fundamental::HexBinary,

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
                        Traversal::Fundamental::Entities
      {
        FundType (Context& c)
            : Context (c), xs_ns_ (xs_ns_name ())
        {
          impl_ns_ = "::xsde::cxx::parser::";
          impl_ns_ += (validation ? L"validating" : L"non_validating");

          if (options.value<CLI::no_stl> ())
          {
            qname_type_ = xs_ns_ + L"::qname*";
            string_type_ = L"char*";
          }
          else
          {
            qname_type_ = xs_ns_ + L"::qname";
            string_type_ = L"::std::string";
          }

          string_seq_type_ = xs_ns_ + L"::string_sequence*";
          buffer_type_ = xs_ns_ + L"::buffer*";

          if (options.value<CLI::no_long_long> ())
          {
            long_type_ = L"long";
            unsigned_long_type_ = L"unsigned long";
          }
          else
          {
            long_type_ = L"long long";
            unsigned_long_type_ = L"unsigned long long";
          }
        }

        // anyType & anySimpleType.
        //
        virtual Void
        traverse (SemanticGraph::AnyType& t)
        {
          gen_typedef (t, "void");
        }

        virtual Void
        traverse (SemanticGraph::AnySimpleType& t)
        {
          gen_typedef (t, string_type_);
        }

        // Boolean.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Boolean& t)
        {
          gen_typedef (t, "bool");
        }

        // Integral types.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Byte& t)
        {
          gen_typedef (t, "signed char");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedByte& t)
        {
          gen_typedef (t, "unsigned char");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Short& t)
        {
          gen_typedef (t, "short");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedShort& t)
        {
          gen_typedef (t, "unsigned short");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Int& t)
        {
          gen_typedef (t, "int");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedInt& t)
        {
          gen_typedef (t, "unsigned int");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Long& t)
        {
          gen_typedef (t, long_type_);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedLong& t)
        {
          gen_typedef (t, unsigned_long_type_);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Integer& t)
        {
          gen_typedef (t, "long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NegativeInteger& t)
        {
          gen_typedef (t, "long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NonPositiveInteger& t)
        {
          gen_typedef (t, "long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::PositiveInteger& t)
        {
          gen_typedef (t, "unsigned long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NonNegativeInteger& t)
        {
          gen_typedef (t, "unsigned long");
        }

        // Floats.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Float& t)
        {
          gen_typedef (t, "float");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Double& t)
        {
          gen_typedef (t, "double");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Decimal& t)
        {
          gen_typedef (t, "double");
        }

        // Strings.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::String& t)
        {
          gen_typedef (t, string_type_);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NormalizedString& t)
        {
          gen_typedef (t, string_type_);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Token& t)
        {
          gen_typedef (t, string_type_);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NameToken& t)
        {
          nmtoken_ = gen_typedef (t, string_type_);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NameTokens& t)
        {
          // NMTOKENS uses NMTOKEN implementation to parse individual items.
          // As a result, we don't generate NMTOKENS if we didn't generate
          // NMTOKEN. Here we assume NMTOKEN is handled before NMTOKENS.
          //
          if(nmtoken_)
            gen_typedef (t, string_seq_type_);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Name& t)
        {
          gen_typedef (t, string_type_);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NCName& t)
        {
          gen_typedef (t, string_type_);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Language& t)
        {
          gen_typedef (t, string_type_);
        }

        // Qualified name.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::QName& t)
        {
          gen_typedef (t, qname_type_);
        }

        // ID/IDREF.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Id& t)
        {
          gen_typedef (t, string_type_);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::IdRef& t)
        {
          idref_ = gen_typedef (t, string_type_);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::IdRefs& t)
        {
          // IDREFS uses IDREF implementation to parse individual items.
          // As a result, we don't generate IDREFS if we didn't generate
          // IDREF. Here we assume IDREF is handled before IDREFS.
          //
          if (idref_)
            gen_typedef (t, string_seq_type_);
        }

        // URI.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::AnyURI& t)
        {
          gen_typedef (t, string_type_);
        }

        // Binary.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Base64Binary& t)
        {
          gen_typedef (t, buffer_type_);
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::HexBinary& t)
        {
          gen_typedef (t, buffer_type_);
        }


        // Date/time.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Date& t)
        {
          gen_typedef (t, xs_ns_ + L"::date");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::DateTime& t)
        {
          gen_typedef (t, xs_ns_ + L"::date_time");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Duration& t)
        {
          gen_typedef (t, xs_ns_ + L"::duration");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Day& t)
        {
          gen_typedef (t, xs_ns_ + L"::gday");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Month& t)
        {
          gen_typedef (t, xs_ns_ + L"::gmonth");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::MonthDay& t)
        {
          gen_typedef (t, xs_ns_ + L"::gmonth_day");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Year& t)
        {
          gen_typedef (t, xs_ns_ + L"::gyear");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::YearMonth& t)
        {
          gen_typedef (t, xs_ns_ + L"::gyear_month");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Time& t)
        {
          gen_typedef (t, xs_ns_ + L"::time");
        }

        // Entity.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Entity&)
        {
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Entities&)
        {
        }

      private:
        Boolean
        gen_typedef (SemanticGraph::Type& t, String const& type)
        {
          if (ret_type (t) == type)
          {
            SemanticGraph::Context& c (t.context ());

            String const& real_name (c.get<String> ("p:real-name"));
            String const& name (c.get<String> ("p:name"));

            os << "typedef " << real_name << " " << name << ";";

            String const& real_impl (c.get<String> ("p:real-impl"));
            String const& impl (c.get<String> ("p:impl"));

            os << "typedef " << real_impl << " " << impl << ";"
               << endl;

            return true;
          }

          return false;
        }

        String xs_ns_;
        String impl_ns_;
        String qname_type_;
        String string_type_;
        String buffer_type_;
        String string_seq_type_;
        String long_type_;
        String unsigned_long_type_;

        Boolean idref_;
        Boolean nmtoken_;
      };

      struct FundNamespace : Namespace, Context
      {
        FundNamespace (Context& c)
            : Namespace (c), Context (c)
        {
        }

        void
        traverse (Type& ns)
        {
          pre (ns);

          String impl ("::xsde::cxx::parser::");
          impl += (validation ? L"validating" : L"non_validating");

          String const c (char_type);

          // Custom allocator.
          //
          if (custom_alloc)
          {
            os << "// Custom allocator." << endl
               << "//" << endl
               << "using ::xsde::cxx::alloc;"
               << "using ::xsde::cxx::free;";

            if (exceptions)
              os << "using ::xsde::cxx::alloc_guard;";

            os << endl;
          }

          os << "// Built-in XML Schema types mapping." << endl
             << "//" << endl
             << "using ::xsde::cxx::string_sequence;"
             << "using ::xsde::cxx::qname;"
             << "using ::xsde::cxx::buffer;"
             << "using ::xsde::cxx::time_zone;"
             << "using ::xsde::cxx::gday;"
             << "using ::xsde::cxx::gmonth;"
             << "using ::xsde::cxx::gyear;"
             << "using ::xsde::cxx::gmonth_day;"
             << "using ::xsde::cxx::gyear_month;"
             << "using ::xsde::cxx::date;"
             << "using ::xsde::cxx::time;"
             << "using ::xsde::cxx::date_time;"
             << "using ::xsde::cxx::duration;"
             << endl;

          os << "// Base parser skeletons." << endl
             << "//" << endl
             << "using ::xsde::cxx::parser::parser_base;"
             << "typedef " << impl << "::empty_content " <<
            "parser_empty_content;"
             << "typedef " << impl << "::simple_content " <<
            "parser_simple_content;"
             << "typedef " << impl << "::complex_content " <<
            "parser_complex_content;"
             << "typedef " << impl << "::list_base parser_list_base;"
             << endl;

          if (poly_code)
          {
            os << "// Parser map interface and default implementation." << endl
               << "//" << endl
               << "using ::xsde::cxx::parser::parser_map;"
               << "using ::xsde::cxx::parser::parser_map_impl;"
               << endl;

            os << "// Substitution and inheritance hashmaps load querying." << endl
               << "//" << endl
               << "using ::xsde::cxx::parser::parser_smap_buckets;"
               << "using ::xsde::cxx::parser::parser_smap_elements;";

            if (validation)
              os << "using ::xsde::cxx::parser::validating::parser_imap_buckets;"
                 << "using ::xsde::cxx::parser::validating::parser_imap_elements;";

            os << endl;
          }

          os << "// Parser skeletons and implementations for the XML Schema" << endl
             << "// built-in types." << endl
             << "//" << endl;

          names (ns);

          os << "// Read-only string." << endl
             << "//" << endl
             << "using ::xsde::cxx::ro_string;"
             << endl;

          os << "// Error codes." << endl
             << "//" << endl;

          if (!exceptions)
            os << "using xsde::cxx::sys_error;";

          os << "typedef xsde::cxx::parser::expat::xml_error " <<
            "parser_xml_error;";

          if (validation)
            os << "typedef xsde::cxx::schema_error parser_schema_error;";

          os << endl;

          if (exceptions)
          {
            os << "// Exceptions." << endl
               << "//" << endl
               << "typedef xsde::cxx::parser::exception parser_exception;"
               << "typedef xsde::cxx::parser::xml parser_xml;";

            if (validation)
              os << "typedef xsde::cxx::parser::schema parser_schema;";

            os << endl;
          }
          else
            os << "// Error object." << endl
               << "//" << endl
               << "typedef xsde::cxx::parser::error parser_error;"
               << endl;

          os << "// Document parser." << endl
             << "//" << endl
             << "using xsde::cxx::parser::expat::document_pimpl;"
             << endl;

          os << "// Parser context." << endl
             << "//" << endl
             << "typedef xsde::cxx::parser::context parser_context;"
             << endl;

          if (char_encoding == L"iso8859-1")
          {
            os << "// ISO-8859-1 transcoder." << endl
               << "//" << endl
               << "using xsde::cxx::iso8859_1;"
               << endl;
          }

          post (ns);
        }
      };
    }

    Void
    generate_parser_header (Context& ctx, Boolean generate_xml_schema)
    {
      NarrowString extern_xml_schema;

      if (!generate_xml_schema)
        extern_xml_schema = ctx.options.value<CLI::extern_xml_schema> ();

      if (extern_xml_schema)
      {
        String name (ctx.hxx_expr->merge (extern_xml_schema));

        ctx.os << "#include " << ctx.process_include_path (name) << endl
               << endl;

        // Generate includes that came from the type map.
        //
        if (ctx.schema_root.context ().count ("p:includes"))
        {
          typedef Cult::Containers::Set<String> Includes;

          Includes& is (
            ctx.schema_root.context ().get<Includes> ("p:includes"));

          for (Includes::ConstReverseIterator i (is.rbegin ());
               i != is.rend (); ++i)
          {
            ctx.os << "#include " << *i << endl;
          }

          ctx.os << endl;
        }
      }
      else
      {
        if (ctx.custom_alloc)
          ctx.os << "#include <xsde/cxx/allocator.hxx>" << endl;

        ctx.os << "#include <xsde/cxx/ro-string.hxx>" << endl;

        if (ctx.char_encoding == L"iso8859-1")
          ctx.os << "#include <xsde/cxx/iso8859-1.hxx>" << endl;

        ctx.os << endl;

        // Data types.
        //
        ctx.os << "#include <xsde/cxx/parser/xml-schema.hxx>" << endl
               << endl;

        // Error handling.
        //
        if (ctx.exceptions)
          ctx.os << "#include <xsde/cxx/parser/exceptions.hxx>" << endl
                 << endl;
        else
        {
          ctx.os << "#include <xsde/cxx/sys-error.hxx>" << endl;

          if (ctx.validation)
            ctx.os << "#include <xsde/cxx/schema-error.hxx>" << endl;

          ctx.os << "#include <xsde/cxx/parser/error.hxx>" << endl
                 << "#include <xsde/cxx/parser/expat/xml-error.hxx>" << endl
                 << endl;
        }

        // Polymorphism support.
        //
        if (ctx.poly_code)
        {
          ctx.os << "#include <xsde/cxx/parser/map.hxx>" << endl
                 << "#include <xsde/cxx/parser/substitution-map-load.hxx>" << endl;

          if (ctx.validation)
            ctx.os << "#include <xsde/cxx/parser/validating/inheritance-map-load.hxx>" << endl;

          ctx.os << endl;
        }

        // Parsers.
        //
        if (ctx.validation)
          ctx.os << "#include <xsde/cxx/stack.hxx>" << endl
                 << "#include <xsde/cxx/parser/validating/parser.hxx>" << endl
                 << "#include <xsde/cxx/parser/validating/xml-schema-pskel.hxx>" << endl
                 << "#include <xsde/cxx/parser/validating/xml-schema-pimpl.hxx>" << endl
                 << endl;
        else
          ctx.os << "#include <xsde/cxx/parser/non-validating/parser.hxx>" << endl
                 << "#include <xsde/cxx/parser/non-validating/xml-schema-pskel.hxx>" << endl
                 << "#include <xsde/cxx/parser/non-validating/xml-schema-pimpl.hxx>" << endl
                 << endl;

        // Document.
        //
        ctx.os << "#include <xsde/cxx/parser/expat/document.hxx>" << endl
               << endl;

        // Generate includes that came from the type map.
        //
        if (ctx.schema_root.context ().count ("p:includes"))
        {
          typedef Cult::Containers::Set<String> Includes;

          Includes& is (
            ctx.schema_root.context ().get<Includes> ("p:includes"));

          for (Includes::ConstReverseIterator i (is.rbegin ());
               i != is.rend (); ++i)
          {
            ctx.os << "#include " << *i << endl;
          }

          ctx.os << endl;
        }

        // Generate fundamental types.
        //
        if (generate_xml_schema)
        {
          Traversal::Schema schema;
          Traversal::Names names;
          FundNamespace ns (ctx);

          schema >> names >> ns;

          Traversal::Names ns_names;
          FundType type (ctx);

          ns >> ns_names >> type;

          schema.dispatch (ctx.schema_root);
        }
        else
        {
          Traversal::Schema schema, xsd;
          Traversal::Implies implies;
          Traversal::Names names;
          FundNamespace ns (ctx);

          schema >> implies >> xsd >> names >> ns;

          Traversal::Names ns_names;
          FundType type (ctx);

          ns >> ns_names >> type;

          schema.dispatch (ctx.schema_root);
        }
      }

      // Generate user type mapping.
      //
      if (!generate_xml_schema)
      {
        Traversal::Schema schema;

        Traversal::Sources sources;
        Includes includes (ctx, Includes::header);
        Traversal::Names schema_names;

        Namespace ns (ctx);
        Traversal::Names names;

        schema >> includes;
        schema >> sources >> schema;
        schema >> schema_names >> ns >> names;

        List list (ctx);
        Union union_ (ctx);
        Complex complex (ctx);
        Enumeration enumeration (ctx);

        names >> list;
        names >> union_;
        names >> complex;
        names >> enumeration;

        schema.dispatch (ctx.schema_root);
      }
    }
  }
}
