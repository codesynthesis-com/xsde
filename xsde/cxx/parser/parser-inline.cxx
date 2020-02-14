// file      : xsde/cxx/parser/parser-inline.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/parser/parser-inline.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

namespace CXX
{
  namespace Parser
  {
    namespace
    {
      void
      facet_calls (SemanticGraph::Complex& c, Context& ctx)
      {
        using SemanticGraph::Restricts;

        std::wostream& os (ctx.os);

        SemanticGraph::Type& ub (ctx.ultimate_base (c));
        Restricts& r (dynamic_cast<Restricts&> (c.inherits ()));

        if (ub.is_a<SemanticGraph::Fundamental::String> () ||
            ub.is_a<SemanticGraph::Fundamental::AnyURI> ())
        {
          for (Restricts::FacetIterator i (r.facet_begin ());
               i != r.facet_end (); ++i)
          {
            if (i->first == L"whiteSpace")
            {
              os << "this->_whitespace_facet (";

              if (i->second == L"preserve")
                os << "0";
              else if (i->second == L"replace")
                os << "1";
              else if (i->second == L"collapse")
                os << "2";

              os << ");";
              continue;
            }

            if (!ctx.validation)
              continue;

            if (i->first == L"length")
            {
              os << "this->_length_facet (" << i->second << "UL);";
            }
            else if (i->first == L"minLength")
            {
              os << "this->_min_length_facet (" << i->second << "UL);";
            }
            else if (i->first == L"maxLength")
            {
              os << "this->_max_length_facet (" << i->second << "UL);";
            }
            else if (i->first == L"pattern")
            {
              os << "this->_pattern_facet (" << ctx.strlit (i->second) << ");";
            }
          }
        }

        if (ub.is_a<SemanticGraph::Fundamental::Byte> ()               ||
            ub.is_a<SemanticGraph::Fundamental::Short> ()              ||
            ub.is_a<SemanticGraph::Fundamental::Int> ()                ||
            ub.is_a<SemanticGraph::Fundamental::Long> ()               ||
            ub.is_a<SemanticGraph::Fundamental::UnsignedByte> ()       ||
            ub.is_a<SemanticGraph::Fundamental::UnsignedShort> ()      ||
            ub.is_a<SemanticGraph::Fundamental::UnsignedInt> ()        ||
            ub.is_a<SemanticGraph::Fundamental::UnsignedLong> ()       ||
            ub.is_a<SemanticGraph::Fundamental::Integer> ()            ||
            ub.is_a<SemanticGraph::Fundamental::NonPositiveInteger> () ||
            ub.is_a<SemanticGraph::Fundamental::NonNegativeInteger> () ||
            ub.is_a<SemanticGraph::Fundamental::PositiveInteger> ()    ||
            ub.is_a<SemanticGraph::Fundamental::NegativeInteger> ()    ||
            ub.is_a<SemanticGraph::Fundamental::Float> ()              ||
            ub.is_a<SemanticGraph::Fundamental::Double> ()             ||
            ub.is_a<SemanticGraph::Fundamental::Decimal> ())
        {
          LiteralValue lv (ctx, false);

          for (Restricts::FacetIterator i (r.facet_begin ());
               i != r.facet_end (); ++i)
          {
            if (!ctx.validation)
              continue;

            if (i->first == L"minInclusive")
            {
              os << "this->_min_facet (" << lv.dispatch (ub, i->second) <<
                ", true);";
            }
            else if (i->first == L"minExclusive")
            {
              os << "this->_min_facet (" << lv.dispatch (ub, i->second) <<
                ", false);";
            }
            else if (i->first == L"maxInclusive")
            {
              os << "this->_max_facet (" << lv.dispatch (ub, i->second) <<
                ", true);";
            }
            else if (i->first == L"maxExclusive")
            {
              os << "this->_max_facet (" << lv.dispatch (ub, i->second) <<
                ", false);";
            }
          }
        }
      }

      struct Enumeration: Traversal::Enumeration, Context
      {
        Enumeration (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& e)
        {
          String const& name (ename (e));

          bool enum_facets (false); // Whether we need to set enum facets.
          if (validation)
          {
            StringBasedType t (enum_facets);
            t.dispatch (e);
          }

          size_t enum_count (0);
          if (enum_facets)
          {
            for (Type::NamesIterator i (e.names_begin ()), end (e.names_end ());
                 i != end; ++i)
              ++enum_count;
          }

          bool facets (enum_facets || has_facets (e));

          if (facets || tiein)
            os << "// " << name << endl
               << "//" << endl
               << endl;

          if (facets && !tiein)
          {
            os << inl
               << name << "::" << endl
               << name << " ()" << endl
               << "{";
            facet_calls (e, enum_count);
            os << "}";
          }

          if (tiein)
          {
            String const& impl (etiein (e));

            // We have to use "real" (non-typedef) base name in base
            // initializer because of some broken compilers (EVC 4.0).
            //
            SemanticGraph::Type& base (e.inherits ().base ());
            String fq_base (fq_name (base));

            os << inl
               << name << "::" << endl
               << name << " (" << fq_base << "* tiein)" << endl
               << ": " << fq_base << " (tiein, 0)," << endl
               << "  " << impl << " (0)"
               << "{";

            if (facets)
              facet_calls (e, enum_count);

            os << "}";

            os << inl
               << name << "::" << endl
               << name << " (" << name << "* impl, void*)" << endl
               << ": " << fq_base << " (impl, 0)," << endl
               << "  " << impl << " (impl)"
               << "{";

            if (facets)
              facet_calls (e, enum_count);

            os << "}";
          }
        }

      private:
        void
        facet_calls (Type& e, size_t enum_count)
        {
          Parser::facet_calls (e, *this);

          if (enum_count != 0)
            os << "this->_enumeration_facet (_xsde_" << ename (e) <<
              "_enums_, " << enum_count << "UL);";
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

        virtual void
        traverse (Type& l)
        {
          String const& name (ename (l));
          SemanticGraph::Type& t (l.argumented ().type ());
          String item_type (fq_name (t));

          String item (unclash (name, "item"));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          // item_parser
          //
          os << inl
             << "void " << name << "::" << endl
             << unclash (name, "item_parser") << " (" <<
            item_type << "& " << item << ")"
             << "{"
             << "this->_xsde_" << item << "_ = &" << item << ";"
             << "}";

          // parsers
          //
          os << inl
             << "void " << name << "::" << endl
             << "parsers (" << item_type << "& " << item << ")"
             << "{"
             << "this->_xsde_" << item << "_ = &" << item << ";"
             << "}";

          // c-tor
          //
          os << inl
             << name << "::" << endl
             << name << " ()" << endl
             << ": ";

          if (tiein)
            os << etiein (l) << " (0)," << endl
               << "  ";

          os << "_xsde_" << item << "_ (0)"
             << "{"
             << "}";

          if (tiein)
          {
            os << inl
               << name << "::" << endl
               << name << " (" << name << "* impl, void*)" << endl
               << ": " << list_base << " (impl, 0)," << endl
               << "  " << etiein (l) << " (impl)," << endl
               << "  _xsde_" << item << "_ (0)"
               << "{"
               << "}";
          }
        }
      };

      struct Union: Traversal::Union, Context
      {
        Union (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& u)
        {
          if (tiein)
          {
            String const& name (ename (u));
            String const& impl (etiein (u));

            os << "// " << name << endl
               << "//" << endl
               << endl;

            //
            //
            os << inl
               << name << "::" << endl
               << name << " ()" << endl
               << ": " << impl << " (0)"
               << "{"
               << "}";

            //
            //
            os << inl
               << name << "::" << endl
               << name << " (" << name << "* impl, void*)" << endl
               << ": " << simple_base << " (impl, 0)," << endl
               << "  " << impl << " (impl)"
               << "{"
               << "}";
          }
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

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          String const& scope (ename (e.scope ()));
          String const& parser (eparser (e));

          os << inl
             << "void " << scope << "::" << endl
             << parser << " (" << fq_name (e.type ()) << "& p)"
             << "{"
             << "this->" << emember (e) << " = &p;"
             << "}";

          if (poly_code && !anonymous (e.type ()))
          {
            os << inl
               << "void " << scope << "::" << endl
               << parser << " (" << parser_map << "& m)"
               << "{"
               << "this->" << emember_map (e) << " = &m;"
               << "}";
          }
        }
      };

      struct AttributeAccessor: Traversal::Attribute, Context
      {
        AttributeAccessor (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& a)
        {
          os << inl
             << "void " << ename (a.scope ()) << "::" << endl
             << eparser (a) << " (" << fq_name (a.type ()) << "& p)"
             << "{"
             << "this->" << emember (a) << " = &p;"
             << "}";
        }
      };

      //
      //
      struct ParticleMemberSet: Traversal::Element, Context
      {
        ParticleMemberSet (Context& c, bool poly)
            : Context (c), poly_ (poly)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          if (poly_)
          {
            if (!anonymous (e.type ()))
              os << "this->" << emember_map (e) << " = &" << ename (e) << ";";
          }
          else
            os << "this->" << emember (e) << " = &" << ename (e) << ";";
        }

      private:
        bool poly_;
      };

      struct AttributeMemberSet: Traversal::Attribute, Context
      {
        AttributeMemberSet (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& a)
        {
          os << "this->" << emember (a) << " = &" << ename (a) << ";";
        }
      };

      struct BaseMemberSet: Traversal::Complex,
                            Traversal::List,
                            Context
      {
        BaseMemberSet (Context& c, bool poly)
            : Context (c), poly_ (poly)
        {
          inherits_ >> *this;
        }

        virtual void
        traverse (SemanticGraph::Complex& c)
        {
          inherits (c, inherits_);

          if (!restriction_p (c))
          {
            names (c);
            contains_compositor (c);
          }
        }

        virtual void
        traverse (SemanticGraph::List& l)
        {
          if (poly_)
            return;

          String const& name (ename (l));
          String item (unclash (name, "item"));

          os << "this->_xsde_" << item << "_ = &" << name << "_item;";
        }

      private:
        Traversal::Inherits inherits_;
        bool poly_;
      };

      //
      //
      struct ParticleMemberInit: Traversal::Element, Context
      {
        ParticleMemberInit (Context& c, bool comma)
            : Context (c), first_ (!comma)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          if (first_)
            first_ = false;
          else
            os << "," << endl << "  ";

          os << emember (e) << " (0)";

          if (poly_code && !anonymous (e.type ()))
          {
            os << "," << endl
               << "  " << emember_map (e) << " (0)";
          }
        }

        bool
        comma () const
        {
          return !first_;
        }

      private:
        bool first_;
      };

      struct AttributeMemberInit: Traversal::Attribute, Context
      {
        AttributeMemberInit (Context& c, bool comma)
            : Context (c), first_ (!comma)
        {
        }

        virtual void
        traverse (Type& a)
        {
          if (first_)
            first_ = false;
          else
            os << "," << endl << "  ";

          os << emember (a) << " (0)";
        }

        bool
        comma () const
        {
          return !first_;
        }

      private:
        bool first_;
      };

      //
      //
      struct Particle: Traversal::All, Context
      {
        Particle (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::All& a)
        {
          if (!a.context().count ("p:comp-number"))
            return;

          size_t state_count (a.context().get<size_t> ("p:state-count"));

          os << "," << endl
             << "  v_all_count_ (" << state_count << "UL, v_all_first_)";
        }
      };


      //
      //
      struct Complex: Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c),
              particle_accessor_ (c),
              attribute_accessor_ (c),
              base_set_ (c, false),
              particle_set_ (c, false),
              attribute_set_ (c),
              base_set_poly_ (c, true),
              particle_set_poly_ (c, true),
              particle_ (c)
        {
          // Accessor.
          //
          contains_compositor_accessor_ >> compositor_accessor_;
          compositor_accessor_ >> contains_particle_accessor_;
          contains_particle_accessor_ >> compositor_accessor_;
          contains_particle_accessor_ >> particle_accessor_;

          names_attribute_accessor_ >> attribute_accessor_;

          // Member set.
          //
          inherits_base_set_ >> base_set_;
          base_set_ >> contains_compositor_set_;
          base_set_ >> names_attribute_set_;

          contains_compositor_set_ >> compositor_set_;
          compositor_set_ >> contains_particle_set_;
          contains_particle_set_ >> compositor_set_;
          contains_particle_set_ >> particle_set_;

          names_attribute_set_ >> attribute_set_;

          // Member set polymorphic.
          //
          inherits_base_set_poly_ >> base_set_poly_;
          base_set_poly_ >> contains_compositor_set_poly_;

          contains_compositor_set_poly_ >> compositor_set_poly_;
          compositor_set_poly_ >> contains_particle_set_poly_;
          contains_particle_set_poly_ >> compositor_set_poly_;
          contains_particle_set_poly_ >> particle_set_poly_;
        }

        virtual void
        traverse (Type& c)
        {
          bool hb (c.inherits_p ());
          bool facets (has_facets (c));
          bool he (has<Traversal::Element> (c));
          bool ha (has<Traversal::Attribute> (c));

          bool hae (has_particle<Traversal::Any> (c));

          bool hra (false); // Has required attribute.
          if (ha)
          {
            RequiredAttributeTest test (hra);
            Traversal::Names names_test (test);
            names (c, names_test);
          }

          bool restriction (restriction_p (c));

          if (!(tiein || facets ||
                (!restriction && (he || ha)) ||
                (validation && (he || hae || hra))))
            return;

          String const& name (ename (c));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          if (!restriction && (he || ha))
          {
            // <name>_parser ()
            //
            if (ha)
              names (c, names_attribute_accessor_);

            if (he)
              contains_compositor (c, contains_compositor_accessor_);


            // parsers ()
            //

            os << inl
               << "void " << name << "::" << endl
               << "parsers (";

            {
              ParserParamDecl decl (*this, true, false);
              decl.traverse (c);
            }

            os << ")"
               << "{";

            inherits (c, inherits_base_set_);

            if (ha)
              names (c, names_attribute_set_);

            if (he)
              contains_compositor (c, contains_compositor_set_);

            os << "}";

            // parser_maps ()
            //
            if (poly_code && he)
            {
              bool r (false);
              ParserParamTest test (*this, r, true);
              test.traverse (c);

              // Have potentially polymorphic elements.
              //
              if (r)
              {
                os << inl
                   << "void " << name << "::" << endl
                   << "parser_maps (";

                {
                  ParserParamDecl decl (*this, true, true);
                  decl.traverse (c);
                }

                os << ")"
                   << "{";

                inherits (c, inherits_base_set_poly_);
                contains_compositor (c, contains_compositor_set_poly_);

                os << "}";
              }
            }
          }

          String fq_base;

          if (hb && tiein)
            fq_base = fq_name (c.inherits ().base ());

          // Default c-tor.
          //
          os << inl
             << name << "::" << endl;

          if (hb && tiein)
            os << name << " (" << fq_base << "* tiein)" << endl;
          else
            os << name << " ()" << endl;

          if (tiein ||
              (!restriction && (he || ha)) ||
              (validation && (he || hae || hra)))
            os << ": ";

          bool comma (false);

          if (hb && tiein)
          {
            os << fq_base << " (tiein, 0)";
            comma = true;
          }

          if (tiein)
          {
            if (comma)
              os << "," << endl << "  ";

            os << etiein (c) << " (0)";
            comma = true;
          }

          if (!restriction && (he || ha))
          {
            if (ha)
            {
              AttributeMemberInit attribute_init (*this, comma);
              Traversal::Names names_attribute_init;

              names_attribute_init >> attribute_init;

              names (c, names_attribute_init);

              comma = attribute_init.comma ();
            }

            if (he)
            {
              Traversal::Compositor compositor_init;
              ParticleMemberInit particle_init (*this, comma);
              Traversal::ContainsCompositor contains_compositor_init;
              Traversal::ContainsParticle contains_particle_init;

              contains_compositor_init >> compositor_init;
              compositor_init >> contains_particle_init;
              contains_particle_init >> compositor_init;
              contains_particle_init >> particle_init;

              contains_compositor (c, contains_compositor_init);

              comma = particle_init.comma ();
            }
          }

          if (validation && (he || hae))
          {
            if (comma)
              os << "," << endl << "  ";

            os << "v_state_stack_ (sizeof (v_state_), &v_state_first_)";

            particle_.dispatch (c.contains_compositor ().compositor ());

            comma = true;
          }

          if (validation && (hra))
          {
            if (comma)
              os << "," << endl << "  ";

            os << "v_state_attr_stack_ (sizeof (v_state_attr_), " <<
              "&v_state_attr_first_)";
          }

          os << "{";

          if (facets)
            facet_calls (c, *this);

          os << "}";

          // Tiein c-tor.
          //
          if (tiein)
          {
            os << inl
               << name << "::" << endl
               << name << " (" << name << "* impl, void*)" << endl
               << ": ";

            if (hb)
              os << fq_base << " (impl, 0)," << endl;
            else
              os << complex_base << " (impl, 0)," << endl;

            os << "  " << etiein (c) << " (impl)";

            bool comma (true);

            if (!restriction && (he || ha))
            {
              if (ha)
              {
                AttributeMemberInit attribute_init (*this, comma);
                Traversal::Names names_attribute_init;

                names_attribute_init >> attribute_init;

                names (c, names_attribute_init);

                comma = attribute_init.comma ();
              }

              if (he)
              {
                Traversal::Compositor compositor_init;
                ParticleMemberInit particle_init (*this, comma);
                Traversal::ContainsCompositor contains_compositor_init;
                Traversal::ContainsParticle contains_particle_init;

                contains_compositor_init >> compositor_init;
                compositor_init >> contains_particle_init;
                contains_particle_init >> compositor_init;
                contains_particle_init >> particle_init;

                contains_compositor (c, contains_compositor_init);

                comma = particle_init.comma ();
              }
            }

            if (validation && (he || hae))
            {
              if (comma)
                os << "," << endl << "  ";

              os << "v_state_stack_ (sizeof (v_state_), &v_state_first_)";

              particle_.dispatch (c.contains_compositor ().compositor ());

              comma = true;
            }

            if (validation && (hra))
            {
              if (comma)
                os << "," << endl << "  ";

              os << "v_state_attr_stack_ (sizeof (v_state_attr_), " <<
                "&v_state_attr_first_)";
            }

            os << "{";

            if (facets)
              facet_calls (c, *this);

            os << "}";
          }
        }

      private:
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
        BaseMemberSet base_set_;
        Traversal::Inherits inherits_base_set_;

        Traversal::Compositor compositor_set_;
        ParticleMemberSet particle_set_;
        Traversal::ContainsCompositor contains_compositor_set_;
        Traversal::ContainsParticle contains_particle_set_;

        AttributeMemberSet attribute_set_;
        Traversal::Names names_attribute_set_;

        //
        //
        BaseMemberSet base_set_poly_;
        Traversal::Inherits inherits_base_set_poly_;

        Traversal::Compositor compositor_set_poly_;
        ParticleMemberSet particle_set_poly_;
        Traversal::ContainsCompositor contains_compositor_set_poly_;
        Traversal::ContainsParticle contains_particle_set_poly_;

        //
        //
        Particle particle_;
      };
    }

    void
    generate_parser_inline (Context& ctx)
    {
      // Emit "weak" header includes that are used in the file-per-type
      // compilation model.
      //
      if (!ctx.options.generate_inline ())
      {
        Traversal::Schema schema;
        Includes includes (ctx, Includes::source);

        schema >> includes;
        schema.dispatch (ctx.schema_root);
      }

      Traversal::Schema schema;

      Sources sources;
      Traversal::Names schema_names;

      Namespace ns (ctx);
      Traversal::Names names;

      schema >> sources >> schema;
      schema >> schema_names >> ns >> names;

      Enumeration enumeration (ctx);
      List list (ctx);
      Union union_ (ctx);
      Complex complex (ctx);

      names >> enumeration;
      names >> list;
      names >> union_;
      names >> complex;

      schema.dispatch (ctx.schema_root);
    }
  }
}
