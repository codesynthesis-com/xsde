// file      : xsde/cxx/serializer/serializer-inline.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/serializer/serializer-inline.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

namespace CXX
{
  namespace Serializer
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

          Boolean facets (false); // Whether we need to set facets.
          if (validation)
          {
            StringBasedType t (facets);
            t.dispatch (e);
          }

          UnsignedLong enum_count (0);
          if (facets)
          {
            for (Type::NamesIterator i (e.names_begin ()), end (e.names_end ());
                 i != end; ++i)
              ++enum_count;
          }

          if (facets || tiein)
            os << "// " << name << endl
               << "//" << endl
               << endl;

          if (facets && !tiein)
          {
            os << inl
               << name << "::" << endl
               << name << " ()" << endl
               << "{"
               << "this->_enumeration_facet (_xsde_" << name << "_enums_, " <<
              enum_count << "UL);"
               << "}";
          }

          if (tiein)
          {
            String const& impl (etiein (e));

            // We have to use "real" (non-typedef) base name in base
            // initializer because of some broken compilers (EVC 4.0).
            //
            SemanticGraph::Type& base (e.inherits ().base ());
            String fq_base (fq_name (base));
            String real_fq_base (real_fq_name (base));

            os << inl
               << name << "::" << endl
               << name << " (" << fq_base << "* tiein)" << endl
               << ": " << real_fq_base << " (tiein, 0)," << endl
               << "  " << impl << " (0)"
               << "{";

            if (facets)
              os << "this->_enumeration_facet (_xsde_" << name <<
                "_enums_, " << enum_count << "UL);";

            os << "}";

            os << inl
               << name << "::" << endl
               << name << " (" << name << "* impl, void*)" << endl
               << ": " << real_fq_base << " (impl, 0)," << endl
               << "  " << impl << " (impl)"
               << "{";

            if (facets)
              os << "this->_enumeration_facet (_xsde_" << name <<
                "_enums_, " << enum_count << "UL);";

            os << "}";
          }
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

          os << "// " << name << endl
             << "//" << endl
             << endl;

          // item_serializer
          //
          os << inl
             << "void " << name << "::" << endl
             << unclash (name, "item_serializer") << " (" <<
            item_type << "& " << item << ")"
             << "{"
             << "this->_xsde_" << item << "_ = &" << item << ";"
             << "}";

          // serializers
          //
          os << inl
             << "void " << name << "::" << endl
             << "serializers (" << item_type << "& " << item << ")"
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
               << ": " << simple_base << " (impl, 0)," << endl
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

        virtual Void
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

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          String const& scope (ename (e.scope ()));
          String const& serializer (eserializer (e));

          os << inl
             << "void " << scope << "::" << endl
             << serializer << " (" << fq_name (e.type ()) << "& s)"
             << "{"
             << "this->" << emember (e) << " = &s;"
             << "}";

          if (poly_code && !anonymous (e.type ()))
          {
            os << inl
               << "void " << scope << "::" << endl
               << serializer << " (" << serializer_map << "& m)"
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

        virtual Void
        traverse (Type& a)
        {
          String const& name (ename (a));

          os << inl
             << "void " << ename (a.scope ()) << "::" << endl
             << eserializer (a) << " (" << fq_name (a.type ()) << "& " <<
            name << ")"
             << "{"
             << "this->" << emember (a) << " = &" << name << ";"
             << "}";
        }
      };

      //
      //
      struct ParticleMemberSet: Traversal::Element, Context
      {
        ParticleMemberSet (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          os << "this->" << emember (e) << " = &" << ename (e) << ";";
        }
      };

      struct AttributeMemberSet: Traversal::Attribute, Context
      {
        AttributeMemberSet (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& a)
        {
          os << "this->" << emember (a) << " = &" << ename (a) << ";";
        }
      };

      struct BaseMemberSet: Traversal::Complex,
                            Traversal::List,
                            Context
      {
        BaseMemberSet (Context& c)
            : Context (c)
        {
          inherits_ >> *this;
        }

        virtual Void
        traverse (SemanticGraph::Complex& c)
        {
          inherits (c, inherits_);

          if (!restriction_p (c))
          {
            names (c);
            contains_compositor (c);
          }
        }

        virtual Void
        traverse (SemanticGraph::List& l)
        {
          String const& name (ename (l));
          String item (unclash (name, "item"));

          os << "this->_xsde_" << item << "_ = &" << name << "_item;";
        }

      private:
        Traversal::Inherits inherits_;
      };

      //
      //
      struct ParticleMemberInit: Traversal::Element, Context
      {
        ParticleMemberInit (Context& c, Boolean comma)
            : Context (c), first_ (!comma)
        {
        }

        virtual Void
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

      private:
        Boolean first_;
      };

      struct AttributeMemberInit: Traversal::Attribute, Context
      {
        AttributeMemberInit (Context& c, Boolean comma)
            : Context (c), first_ (!comma)
        {
        }

        virtual Void
        traverse (Type& a)
        {
          if (first_)
            first_ = false;
          else
            os << "," << endl << "  ";

          os << emember (a) << " (0)";
        }

        Boolean
        comma () const
        {
          return !first_;
        }

      private:
        Boolean first_;
      };


      //
      //
      struct Complex: Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c),
              particle_accessor_ (c),
              attribute_accessor_ (c),
              base_set_ (c),
              particle_set_ (c),
              attribute_set_ (c)
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
        }

        virtual Void
        traverse (Type& c)
        {
          Boolean hb (c.inherits_p ());
          Boolean he (has<Traversal::Element> (c));
          Boolean ha (has<Traversal::Attribute> (c));
          Boolean restriction (restriction_p (c));

          Boolean facets (false); // Defines facets.
          if (validation && restriction)
          {
            SemanticGraph::Type& ub (ultimate_base (c));

            if (ub.is_a<SemanticGraph::Fundamental::Short> ()         ||
                ub.is_a<SemanticGraph::Fundamental::UnsignedByte> ()  ||
                ub.is_a<SemanticGraph::Fundamental::UnsignedShort> () ||
                ub.is_a<SemanticGraph::Fundamental::UnsignedInt> ()   ||
                ub.is_a<SemanticGraph::Fundamental::String> ())
            {
              using SemanticGraph::Restricts;
              Restricts& r (dynamic_cast<Restricts&> (c.inherits ()));

              if (!r.facet_empty ())
              {
                Restricts::FacetIterator end (r.facet_end ());
                facets =
                  r.facet_find (L"length") != end ||
                  r.facet_find (L"minLength") != end ||
                  r.facet_find (L"maxLength") != end ||
                  r.facet_find (L"minInclusive") != end ||
                  r.facet_find (L"minExclusive") != end ||
                  r.facet_find (L"maxInclusive") != end ||
                  r.facet_find (L"maxExclusive") != end;
              }
            }
          }

          if (!(tiein || facets || (!restriction && (he || ha))))
            return;

          String const& name (ename (c));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          if (!restriction && (he || ha))
          {
            // <name>_serializer ()
            //
            if (ha)
              names (c, names_attribute_accessor_);

            if (he)
              contains_compositor (c, contains_compositor_accessor_);

            // serializer ()
            //

            os << inl
               << "void " << name << "::" << endl
               << "serializers (";

            {
              SerializerParamDecl decl (*this, true);
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
          }

          // We have to use "real" (non-typedef) base name in base
          // initializer because of some broken compilers (EVC 4.0).
          //
          String real_fq_base;

          if (hb && tiein)
            real_fq_base = real_fq_name (c.inherits ().base ());

          // Default c-tor.
          //
          os << inl
             << name << "::" << endl;

          if (hb && tiein)
            os << name << " (" << fq_name (c.inherits ().base ()) <<
              "* tiein)" << endl;
          else
            os << name << " ()" << endl;

          if (tiein || (!restriction && (he || ha)))
            os << ": ";

          Boolean comma (false);

          if (hb && tiein)
          {
            os << real_fq_base << " (tiein, 0)";
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
            }
          }

          os << "{";

          if (facets)
            facet_calls (c);

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
              os << real_fq_base << " (impl, 0)," << endl;
            else
              os << complex_base << " (impl, 0)," << endl;

            os << "  " << etiein (c) << " (impl)";

            Boolean comma (true);

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
              }
            }

            os << "{";

            if (facets)
              facet_calls (c);

            os << "}";
          }
        }

      private:
        Void
        facet_calls (Type& c)
        {
          using SemanticGraph::Restricts;
          Restricts& r (dynamic_cast<Restricts&> (c.inherits ()));

          for (Restricts::FacetIterator i (r.facet_begin ());
               i != r.facet_end (); ++i)
          {
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
            else if (i->first == L"minInclusive")
            {
              os << "this->_min_facet (" << i->second << ", true);";
            }
            else if (i->first == L"minExclusive")
            {
              os << "this->_min_facet (" << i->second << ", false);";
            }
            else if (i->first == L"maxInclusive")
            {
              os << "this->_max_facet (" << i->second << ", true);";
            }
            else if (i->first == L"maxExclusive")
            {
              os << "this->_max_facet (" << i->second << ", false);";
            }
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
      };
    }

    Void
    generate_serializer_inline (Context& ctx)
    {
      // Emit "weak" header includes that are used in the file-per-type
      // compilation model.
      //
      if (!ctx.options.value<CLI::generate_inline> ())
      {
        Traversal::Schema schema;
        Includes includes (ctx, Includes::source);

        schema >> includes;
        schema.dispatch (ctx.schema_root);
      }

      Traversal::Schema schema;

      Traversal::Sources sources;
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
