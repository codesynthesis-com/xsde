// file      : xsde/cxx/serializer/serializer-header.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/serializer/serializer-header.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

namespace CXX
{
  namespace Serializer
  {
    namespace
    {
      // Override classes override pure virtual functions in the base.
      // Should be in sync with declaration generators below. Used in
      // tiein implementation.
      //

      struct CompositorCallbackOverride: Traversal::Choice,
                                         Traversal::Sequence,
                                         Context
      {
        CompositorCallbackOverride (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.contains_begin () != c.contains_end ())
          {
            UnsignedLong min (c.min ()), max (c.max ());

            if (max != 1 && min != 0)
            {
              os << "virtual bool" << endl
                 << enext (c) << " ();"
                 << endl;
            }

            if (min != 0)
            {
              os << "virtual " << earm_tag (c) << endl
                 << earm (c) << " ();"
                 << endl;
            }

            Traversal::Choice::traverse (c);
          }
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          UnsignedLong min (s.min ()), max (s.max ());

          if (max != 1 && min != 0)
          {
            os << "virtual bool" << endl
               << enext (s) << " ();"
               << endl;
          }

          Traversal::Sequence::traverse (s);
        }
      };

      struct ParticleCallbackOverride: Traversal::Element,
                                       Traversal::Any,
                                       Context
      {
        ParticleCallbackOverride (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          UnsignedLong min (e.min ()), max (e.max ());

          if (max != 1 && min != 0)
          {
            os << "virtual bool" << endl
               << enext (e) << " ();"
               << endl;
          }

          String const& ret (ret_type (e.type ()));

          if (ret != L"void")
          {
            os << "virtual " << ret << endl
               << ename (e) << " ();"
               << endl;
          }
        }

        virtual Void
        traverse (SemanticGraph::Any& a)
        {
          UnsignedLong min (a.min ()), max (a.max ());

          if (min != 0 &&
              !a.contained_particle ().compositor ().is_a<
              SemanticGraph::Choice> ())
          {
            if (max != 1)
            {
              os << "virtual bool" << endl
                 << enext (a) << " ();"
                 << endl;
            }

            if (stl)
            {
              os << "virtual void" << endl
                 << ename (a) << " (::std::string& ns, ::std::string& name);"
                 << endl;
            }
            else
            {
              os << "virtual void" << endl
                 << ename (a) << " (const char*& ns, const char*& name, " <<
                "bool& free);"
                 << endl;
            }

            os << "virtual void" << endl
               << eserialize (a) << " ();"
               << endl;
          }
        }
      };

      struct AttributeCallbackOverride: Traversal::Attribute, Context
      {
        AttributeCallbackOverride (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          String const& ret (ret_type (a.type ()));

          if (ret != L"void")
          {
            os << "virtual " << ret << endl
               << ename (a) << " ();"
               << endl;
          }
        }
      };

      struct BaseOverride: Traversal::Type,
                           Traversal::Enumeration,
                           Traversal::List,
                           Traversal::Union,
                           Traversal::Complex,
                           Context
      {
        BaseOverride (Context& c)
            : Context (c),
              compositor_callback_ (c),
              particle_callback_ (c),
              attribute_callback_ (c)
        {
          contains_compositor_callback_ >> compositor_callback_;
          compositor_callback_ >> contains_particle_callback_;
          contains_particle_callback_ >> compositor_callback_;
          contains_particle_callback_ >> particle_callback_;

          names_attribute_callback_ >> attribute_callback_;
        }

        virtual Void
        traverse (SemanticGraph::Type& t)
        {
          // pre
          //
          String const& arg (arg_type (t));

          if (arg != L"void")
          {
            os << "virtual void" << endl
               << "pre (" << arg << ");"
               << endl;
          }
        }

        virtual Void
        traverse (SemanticGraph::Enumeration& e)
        {
          SemanticGraph::Type& t (e);
          traverse (t);
        }

        virtual Void
        traverse (SemanticGraph::List& l)
        {
          SemanticGraph::Type& t (l);
          traverse (t);

          // item
          //
          String const& ret (ret_type (l.argumented ().type ()));

          if (ret != L"void")
          {
            os << "virtual " << ret << endl
               << unclash (ename (l), "item") << " ();"
               << endl;
          }
        }

        virtual Void
        traverse (SemanticGraph::Union& u)
        {
          SemanticGraph::Type& t (u);
          traverse (t);

          // serialize_content
          //
          os << "virtual void" << endl
             << "_serialize_content ();"
             << endl;
        }

        virtual Void
        traverse (SemanticGraph::Complex& c)
        {
          SemanticGraph::Type& t (c);
          traverse (t);

          // Member callbacks.
          //
          if (!restriction_p (c))
          {
            Traversal::Complex::names (c, names_attribute_callback_);
            Traversal::Complex::contains_compositor (
              c, contains_compositor_callback_);
          }
        }

      private:
        CompositorCallbackOverride compositor_callback_;
        ParticleCallbackOverride particle_callback_;
        Traversal::ContainsCompositor contains_compositor_callback_;
        Traversal::ContainsParticle contains_particle_callback_;

        AttributeCallbackOverride attribute_callback_;
        Traversal::Names names_attribute_callback_;
      };

      //
      //

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
             << "// Serializer callbacks. Override them in your " <<
            "implementation." << endl
             << "//" << endl
             << endl;

          // pre
          //
          String const& arg (arg_type (e));
          String const& base_arg (arg_type (base));

          if (arg == base_arg)
          {
            os << "// virtual void" << endl;

            if (arg == L"void")
              os << "// pre ();" << endl;
            else
              os << "// pre (" << arg << ") = 0;" << endl;

            os << endl;
          }
          else
          {
            os << "virtual void" << endl;

            if (arg == L"void")
              os << "pre ();";
            else
              os << "pre (" << arg << ") = 0;";

            os << endl;
          }

          // post
          //
          os << "// virtual void" << endl
             << "// post ();" << endl;

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

            // If our base has pure virtual functions, override them here.
            //
            inherits (e);

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
          String item_next (unclash (name, "item_next"));

          os << "class " << name << ": public " << simple_base
             << "{"
             << "public:" << endl
             << "// Serializer callbacks. Override them in your " <<
            "implementation." << endl
             << "//" << endl
             << endl;

          // pre
          //
          String const& arg (arg_type (l));

          os << "virtual void" << endl;

          if (arg == L"void")
            os << "pre ();";
          else
            os << "pre (" << arg << ") = 0;";

          os << endl;

          // item
          //
          os << "virtual bool" << endl
             << item_next << " ();"
             << endl;

          String const& ret (ret_type (t));

          os << "virtual " << ret << endl
             << item << " ()" << (ret != L"void" ? " = 0" : "") <<  ";"
             << endl;

          // post
          //
          os << "// virtual void" << endl
             << "// post ();" << endl
             << endl;

          //
          //
          os << "// Serializer construction API." << endl
             << "//" << endl;

          // item_serializer
          //
          os << "void" << endl
             << unclash (name, "item_serializer") << " (" << item_type << "&);"
             << endl;

          // serializers
          //
          os << "void" << endl
             << "serializers (" << item_type << "& /* item */);"
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
          os << "// Implementation." << endl
             << "//" << endl
             << "public:" << endl;

          os << "virtual void" << endl
             << "_serialize_content ();"
             << endl;

          os << "protected:" << endl;

          if (tiein)
          {
            os << name << "* " << etiein (l) << ";"
               << name << " (" << name << "*, void*);"
               << endl;
          }

          os << item_type << "* _xsde_" << item << "_;"
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
             << "// Serializer callbacks. Override them in your " <<
            "implementation." << endl
             << "//" << endl
             << endl;

          // pre
          //
          String const& arg (arg_type (u));

          os << "virtual void" << endl;

          if (arg == L"void")
            os << "pre ();";
          else
            os << "pre (" << arg << ") = 0;";

          os << endl;

          // serialize_content
          //
          os << "virtual void" << endl
             << "_serialize_content () = 0;"
             << endl;

          // post
          //
          os << "// virtual void" << endl
             << "// post ();" << endl;

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
               << name << " (" << name << "*, void*);"
               << endl;
          }

          os << "};";
        }
      };

      //
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
          if (!a.context ().count ("xsd-frontend-restriction-correspondence"))
          {
            // For the all compositor, maxOccurs=1 and minOccurs={0,1}.
            //
            if (a.min () == 0)
            {
              os << "virtual bool" << endl
                 << epresent (a) << " ();"
                 << endl;
            }
          }

          Traversal::All::traverse (a);
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.contains_begin () != c.contains_end ())
          {
            if (SemanticGraph::Compositor* b = correspondent (c))
            {
              // Add the *_present callback if this is a restriction
              // of sequence to optional.
              //
              UnsignedLong cmin (c.min ()), bmax (b->max ());

              if (bmax != 1 && cmin == 0)
              {
                os << "virtual bool" << endl
                   << epresent (c) << " ();"
                   << endl;
              }
            }
            else
            {
              UnsignedLong min (c.min ()), max (c.max ());

              if (min == 0 && max == 1)
              {
                os << "virtual bool" << endl
                   << epresent (c) << " ();"
                   << endl;
              }
              else if (max != 1)
              {
                os << "virtual bool" << endl
                   << enext (c) << " ()" << (min != 0 ? " = 0" : "") << ";"
                   << endl;
              }

              //
              //
              os << "enum " << earm_tag (c)
                 << "{";

              {
                ParticleTag particle (*this);
                Traversal::ContainsParticle contain_particle (particle);
                Traversal::Choice::contains (c, contain_particle);
              }

              os << "};";

              os << "virtual " << earm_tag (c) << endl
                 << earm (c) << " ()" << (min != 0 ? " = 0" : "") << ";"
                 << endl;
            }

            Traversal::Choice::traverse (c);
          }
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          // Root compositor that models inheritance by extension
          // may not have an association so we may fall through
          // in to the 'else' case even though this is a restriction.
          // This is ok since such a compositor always has max ==
          // min == 1 and so nothing is generated.
          //
          if (SemanticGraph::Compositor* b = correspondent (s))
          {
            // Add the *_present callback if this is a restriction
            // of sequence to optional.
            //
            UnsignedLong smin (s.min ());
            UnsignedLong bmax (b->max ());

            if (bmax != 1 && smin == 0)
            {
              os << "virtual bool" << endl
                 << epresent (s) << " ();"
                 << endl;
            }
          }
          else
          {
            UnsignedLong min (s.min ()), max (s.max ());

            if (min == 0 && max == 1)
            {
              os << "virtual bool" << endl
                 << epresent (s) << " ();"
                 << endl;
            }
            else if (max != 1)
            {
              os << "virtual bool" << endl
                 << enext (s) << " ()" << (min != 0 ? " = 0" : "") <<  ";"
                 << endl;
            }
          }

          Traversal::Sequence::traverse (s);
        }
      };

      struct ParticleCallback: Traversal::Element,
                               Traversal::Any,
                               Context
      {
        ParticleCallback (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          if (SemanticGraph::Element* b = correspondent (e))
          {
            // Add the *_present callback if this is a restriction
            // of sequence to optional.
            //
            if (b->max () != 1 && e.min () == 0)
            {
              os << "virtual bool" << endl
                 << epresent (e) << " ();"
                 << endl;
            }
          }
          else
          {
            UnsignedLong min (e.min ()), max (e.max ());

            if (min == 0 && max == 1)
            {
              os << "virtual bool" << endl
                 << epresent (e) << " ();"
                 << endl;
            }
            else if (max != 1)
            {
              os << "virtual bool" << endl
                 << enext (e) << " ()" << (min != 0 ? " = 0" : "") << ";"
                 << endl;
            }

            String const& ret (ret_type (e.type ()));

            // Make it non-pure-virtual only if the return type is void.
            //
            os << "virtual " << ret << endl
               << ename (e) << " ()" << (ret != L"void" ? " = 0;" : ";")
               << endl;
          }
        }

        virtual Void
        traverse (SemanticGraph::Any& a)
        {
          if (SemanticGraph::Any* b = correspondent (a))
          {
            // Add the *_present callback if this is a restriction
            // of sequence to optional.
            //
            if (b->max () != 1 && a.min () == 0)
            {
              os << "virtual bool" << endl
                 << epresent (a) << " ();"
                 << endl;
            }
          }
          else
          {
            UnsignedLong min (a.min ()), max (a.max ());

            // Generate pure virtual callbacks unless we are optional
            // or in choice.
            //
            Boolean pv (
              min != 0 &&
              !a.contained_particle ().compositor ().is_a<
                SemanticGraph::Choice> ());

            if (min == 0 && max == 1)
            {
              os << "virtual bool" << endl
                 << epresent (a) << " ();"
                 << endl;
            }
            else if (max != 1)
            {
              os << "virtual bool" << endl
                 << enext (a) << " ()" << (pv ? " = 0" : "") << ";"
                 << endl;
            }

            if (stl)
            {
              os << "virtual void" << endl
                 << ename (a) << " (::std::string& ns, ::std::string& name)" <<
                (pv ? " = 0;" : ";")
                 << endl;
            }
            else
            {
              os << "virtual void" << endl
                 << ename (a) << " (const char*& ns, const char*& name, " <<
                "bool& free)" << (pv ? " = 0;" : ";")
                 << endl;
            }

            os << "virtual void" << endl
               << eserialize (a) << " ()" << (pv ? " = 0;" : ";")
               << endl;
          }
        }
      };

      struct AttributeCallback: Traversal::Attribute,
                                Traversal::AnyAttribute,
                                Context
      {
        AttributeCallback (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          if (a.optional_p ())
          {
            os << "virtual bool" << endl
               << epresent (a) << " ();"
               << endl;
          }

          String const& ret (ret_type (a.type ()));

          // Make it non-pure-virtual only if the return type is void.
          //
          os << "virtual " << ret << endl
             << ename (a) << " ()" << (ret != L"void" ? " = 0;" : ";")
             << endl;
        }

        virtual Void
        traverse (SemanticGraph::AnyAttribute& a)
        {
          os << "virtual bool" << endl
             << enext (a) << " ();"
             << endl;

          if (stl)
          {
            os << "virtual void" << endl
               << ename (a) << " (::std::string& ns, ::std::string& name);"
               << endl;
          }
          else
          {
            os << "virtual void" << endl
               << ename (a) << " (const char*& ns, const char*& name, " <<
              "bool& free);"
               << endl;
          }

          os << "virtual void" << endl
             << eserialize (a) << " ();"
             << endl;
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
          String const& serializer (eserializer (e));

          os << "void" << endl
             << serializer << " (" << fq_name (e.type ()) << "&);"
             << endl;

          if (poly_code && !anonymous (e.type ()))
          {
            os << "void" << endl
               << eserializer (e) << " (" << serializer_map << "&);"
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
             << eserializer (a) << " (" << fq_name (a.type ()) << "&);"
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
            os << serializer_map << "* " << emember_map (e) << ";"
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
      struct Complex : Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c),
              compositor_callback_ (c),
              particle_callback_ (c),
              attribute_callback_ (c),
              particle_accessor_ (c),
              attribute_accessor_ (c),
              particle_member_ (c),
              attribute_member_ (c)
        {
          // Callback.
          //
          contains_compositor_callback_ >> compositor_callback_;
          compositor_callback_ >> contains_particle_callback_;
          contains_particle_callback_ >> compositor_callback_;
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
        }

        virtual Void
        traverse (Type& c)
        {
          String const& name (ename (c));

          // In case of an inheritance-by-restriction, we don't need to
          // generate serializer callbacks, etc. since they are the same
          // as in the base. We only need the serialization/validation code.
          //
          Boolean restriction (restriction_p (c));
          Boolean facets (has_facets (c));

          Boolean hb (c.inherits_p ());
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
             << "// Serializer callbacks. Override them in your " <<
            "implementation." << endl
             << "//" << endl
             << endl;

          // pre
          //
          String const& arg (arg_type (c));
          Boolean same (hb && arg == arg_type (c.inherits ().base ()));

          if (same)
          {
            os << "// virtual void" << endl;

            if (arg == L"void")
              os << "// pre ();" << endl;
            else
              os << "// pre (" << arg << ") = 0;" << endl;

            os << endl;
          }
          else
          {
            os << "virtual void" << endl;

            if (arg == L"void")
              os << "pre ();";
            else
              os << "pre (" << arg << ") = 0;";

            os << endl;
          }

          // Member callbacks.
          //
          if (!restriction)
          {
            if (ha || haa)
            {
              os << "// Attributes." << endl
                 << "//" << endl;

              names (c, names_attribute_callback_);
            }
          }

          if (he || hae)
          {
            if (!restriction)
            {
              os << "// Elements." << endl
                 << "//" << endl;
            }

            contains_compositor (c, contains_compositor_callback_);
          }

          // post
          //
          os << "// virtual void" << endl
             << "// post ();" << endl
             << endl;

          //
          //
          if (!restriction && (he || ha))
          {
            os << "// Serializer construction API." << endl
               << "//" << endl;

            os << "void" << endl
               << "serializers (";

            {
              SerializerParamDecl decl (*this, false);
              decl.traverse (c);
            }

            os << ");"
               << endl;

            if (ha)
            {
              os << "// Individual attribute serializers." << endl
                 << "//" << endl;

              names (c, names_attribute_accessor_);
            }

            if (he)
            {
              os << "// Individual element serializers." << endl
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
          if (tiein || facets || (!restriction && (he || ha)))
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
          if (tiein || he || ha || hae || haa)
          {
            os << "// Implementation." << endl
               << "//" << endl
               << "public:" << endl;

            // If our base has pure virtual functions, override them here.
            //
            if (tiein && hb)
              inherits (c);

            if (ha || haa)
            {
              os << "virtual void" << endl
                 << "_serialize_attributes ();"
                 << endl;
            }

            if (he || hae)
            {
              os << "virtual void" << endl
                 << "_serialize_content ();"
                 << endl;
            }
          }

          if (tiein)
          {
            os << "protected:" << endl
               << name << "* " << etiein (c) << ";"
               << name << " (" << name << "*, void*);"
               << endl;
          }

          if (!restriction && (he || ha))
          {
            os << "protected:" << endl;

            if (ha)
              names (c, names_attribute_member_);

            if (he)
              contains_compositor (c, contains_compositor_member_);
          }

          os << "};";
        }

      private:
        //
        //
        CompositorCallback compositor_callback_;
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
          impl_ns_ = "::xsde::cxx::serializer::";
          impl_ns_ += (validation ? L"validating" : L"non_validating");

          if (options.value<CLI::no_stl> ())
          {
            qname_type_ = L"const " + xs_ns_ + L"::qname*";
            string_type_ = L"const char*";
          }
          else
          {
            qname_type_ = xs_ns_ + L"::qname";
            string_type_ = L"::std::string";
          }

          string_seq_type_ = L"const " + xs_ns_ + L"::string_sequence*";
          buffer_type_ = L"const " + xs_ns_ + L"::buffer*";

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
          // NMTOKENS uses NMTOKEN implementation to serialize individual
	  // items. As a result, we don't generate NMTOKENS if we didn't
	  // generate NMTOKEN. Here we assume NMTOKEN is handled before
	  // NMTOKENS.
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
          // IDREFS uses IDREF implementation to serialize individual items.
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

            String const& real_name (c.get<String> ("s:real-name"));
            String const& name (c.get<String> ("s:name"));

            String const& real_impl (c.get<String> ("s:real-impl"));
            String const& impl (c.get<String> ("s:impl"));

            if (options.value<CLI::skel_type_suffix> () == "_sskel")
              os << "using " << real_name << ";";
            else
              os << "typedef " << real_name << " " << name << ";";

            if (options.value<CLI::impl_type_suffix> () == "_simpl")
              os << "using " << real_impl << ";";
            else
              os << "typedef " << real_impl << " " << impl << ";";

            os << endl;
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

          String impl ("::xsde::cxx::serializer::");
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

          os << "// Base serializer skeletons." << endl
             << "//" << endl
             << "using ::xsde::cxx::serializer::serializer_base;"
             << "typedef " << impl << "::empty_content " <<
            "serializer_empty_content;"
             << "typedef " << impl << "::simple_content " <<
            "serializer_simple_content;"
             << "typedef " << impl << "::complex_content " <<
            "serializer_complex_content;"
             << endl;

          if (poly_code)
          {
            os << "// Serializer map interface and default implementation." << endl
               << "//" << endl
               << "using ::xsde::cxx::serializer::serializer_map;"
               << "using ::xsde::cxx::serializer::serializer_map_impl;"
               << endl;

            os << "// Substitution and inheritance hashmaps load querying." << endl
               << "//" << endl
               << "using ::xsde::cxx::serializer::serializer_smap_buckets;"
               << "using ::xsde::cxx::serializer::serializer_smap_elements;"
               << "using ::xsde::cxx::serializer::serializer_smap_bucket_buckets;"
               << "using ::xsde::cxx::serializer::serializer_smap_bucket_elements;";

            if (validation)
              os << "using ::xsde::cxx::serializer::validating::serializer_imap_buckets;"
                 << "using ::xsde::cxx::serializer::validating::serializer_imap_elements;";

            os << endl;
          }

          os << "// Serializer skeletons and implementations for the" << endl
             << "// XML Schema built-in types." << endl
             << "//" << endl;

          names (ns);

          os << "// Error codes." << endl
             << "//" << endl;

          if (!exceptions)
            os << "using xsde::cxx::sys_error;";

          os << "typedef xsde::cxx::serializer::genx::xml_error " <<
            "serializer_xml_error;";

          if (validation)
            os << "typedef xsde::cxx::schema_error " <<
              "serializer_schema_error;";

          os << endl;

          if (exceptions)
          {
            os << "// Exceptions." << endl
               << "//" << endl
               << "typedef xsde::cxx::serializer::exception " <<
              "serializer_exception;"
               << "typedef xsde::cxx::serializer::xml serializer_xml;";

            if (validation)
              os << "typedef xsde::cxx::serializer::schema " <<
                "serializer_schema;";

            os << endl;
          }
          else
            os << "// Error object." << endl
               << "//" << endl
               << "typedef xsde::cxx::serializer::error serializer_error;"
               << endl;

          os << "// Document serializer." << endl
             << "//" << endl
             << "using xsde::cxx::serializer::genx::writer;"
             << "using xsde::cxx::serializer::genx::document_simpl;"
             << endl;

          os << "// Serializer context." << endl
             << "//" << endl
             << "typedef xsde::cxx::serializer::context serializer_context;"
             << endl;

          post (ns);
        }
      };
    }

    Void
    generate_serializer_header (Context& ctx, Boolean generate_xml_schema)
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
        if (ctx.schema_root.context ().count ("s:includes"))
        {
          typedef Cult::Containers::Set<String> Includes;

          Includes& is (
            ctx.schema_root.context ().get<Includes> ("s:includes"));

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
          ctx.os << "#include <xsde/cxx/allocator.hxx>" << endl
                 << endl;

        // std::string or xsde::cxx::string is used in wildcard API.
        //
        if (ctx.stl)
        {
          ctx.os << "#include <string>" << endl
                 << endl;
        }
        else
        {
          ctx.os << "#include <xsde/cxx/string.hxx>" << endl
                 << endl;
        }

        // Data types.
        //
        ctx.os << "#include <xsde/cxx/serializer/xml-schema.hxx>" << endl
               << endl;

        // Error handling.
        //
        if (ctx.exceptions)
          ctx.os << "#include <xsde/cxx/serializer/exceptions.hxx>" << endl
                 << endl;
        else
        {
          ctx.os << "#include <xsde/cxx/sys-error.hxx>" << endl;

          if (ctx.validation)
            ctx.os << "#include <xsde/cxx/schema-error.hxx>" << endl;

          ctx.os << "#include <xsde/cxx/serializer/error.hxx>" << endl
                 << "#include <xsde/cxx/serializer/genx/xml-error.hxx>" << endl;

          ctx.os << endl;
        }

        // Polymorphism support.
        //
        if (ctx.poly_code)
        {
          ctx.os << "#include <xsde/cxx/serializer/map.hxx>" << endl
                 << "#include <xsde/cxx/serializer/substitution-map-load.hxx>" << endl;

          if (ctx.validation)
            ctx.os << "#include <xsde/cxx/serializer/validating/inheritance-map-load.hxx>" << endl;

          ctx.os << endl;
        }

        // Serializers.
        //
        if (ctx.validation)
          ctx.os << "#include <xsde/cxx/serializer/validating/serializer.hxx>" << endl
                 << "#include <xsde/cxx/serializer/validating/xml-schema-sskel.hxx>" << endl
                 << "#include <xsde/cxx/serializer/validating/xml-schema-simpl.hxx>" << endl
                 << endl;
        else
          ctx.os << "#include <xsde/cxx/serializer/non-validating/serializer.hxx>" << endl
                 << "#include <xsde/cxx/serializer/non-validating/xml-schema-sskel.hxx>" << endl
                 << "#include <xsde/cxx/serializer/non-validating/xml-schema-simpl.hxx>" << endl
                 << endl;

        // Document.
        //
        ctx.os << "#include <xsde/cxx/serializer/genx/document.hxx>" << endl
               << endl;

        // Generate includes that came from the type map.
        //
        if (ctx.schema_root.context ().count ("s:includes"))
        {
          typedef Cult::Containers::Set<String> Includes;

          Includes& is (
            ctx.schema_root.context ().get<Includes> ("s:includes"));

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

        BaseOverride base_override (ctx);
        Traversal::Inherits inherits_override;

        complex >> inherits_override;
        enumeration >> inherits_override;
        inherits_override >> base_override;

        schema.dispatch (ctx.schema_root);
      }
    }
  }
}
