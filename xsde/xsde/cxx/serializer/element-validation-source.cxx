// file      : xsde/cxx/serializer/element-validation-source.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/element-validation-source.hxx>

#include <libxsd-frontend/semantic-graph.hxx>
#include <libxsd-frontend/traversal.hxx>

namespace CXX
{
  namespace Serializer
  {
    namespace
    {
      struct AnyTest: Traversal::Any, Context
      {
        AnyTest (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Any& a)
        {
          String const& ns (a.definition_namespace ().name ());

          for (SemanticGraph::Any::NamespaceIterator i (a.namespace_begin ()),
                 e (a.namespace_end ()); i != e;)
          {
            if (*i == L"##any")
            {
              if (stl)
                os << "!name.empty ()";
              else
                os << "(name != 0 && *name != '\\0')";
            }
            else if (*i == L"##other")
            {
              if (ns)
              {
                // Note that here I assume that ##other does not include
                // unqualified names in a schema with target namespace.
                // This is not what the spec says but that seems to be
                // the consensus.
                //
                if (stl)
                  os << "(!ns.empty () && ns != " << strlit (ns) << ")";
                else
                  os << "(ns != 0 && *ns != '\\0' && " <<
                    "strcmp (ns, " << strlit (ns) << ") != 0)";
              }
              else
              {
                if (stl)
                  os << "!ns.empty ()";
                else
                  os << "(ns != 0 && *ns != '\\0')";
              }
            }
            else if (*i == L"##local")
            {
              if (stl)
                os << "(ns.empty () && !name.empty ())";
              else
                os << "((ns == 0 || *ns == '\\0') && " <<
                  "name != 0 && *name != '\\0')";
            }
            else if (*i == L"##targetNamespace")
            {
              if (stl)
                os << "ns == " << strlit (ns);
              else
                os << "(ns != 0 && strcmp (ns, " << strlit (ns) << ") == 0)";
            }
            else
            {
              if (stl)
                os << "ns == " << strlit (*i);
              else
                os << "(ns != 0 && strcmp (ns, " << strlit (*i) << ") == 0)";
            }

            if (++i != e)
              os << " ||" << endl;
          }
        }
      };

      struct Compositor: Traversal::All,
                         Traversal::Choice,
                         Traversal::Sequence,
                         Context
      {
        Compositor (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}.
          //
          size_t min (a.min ());

          if (min == 0)
            os << "if (this->" << epresent (a) << " ())"
               << "{";

          Traversal::All::traverse (a);

          if (min == 0)
          {
            os << "}";

            if (!exceptions)
              os << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl;
          }
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          if (c.contains_begin () != c.contains_end ())
          {
            size_t min (c.min ()), max (c.max ());

            if (min == 0 && max == 1)
            {
              os << "if (this->" << epresent (c) << " ())"
                 << "{";
            }
            else if (max != 1)
            {
              // We only need to count if max != unbounded || min != 0.
              //
              if (max != 0 || min != 0)
              {
                os << "{"
                   << "size_t i = 0;"
                   << "for (; ";

                if (max != 0)
                  os << "i < " << max << "UL && ";

                os << "this->" << enext (c) << " (); ++i)"
                   << "{";
              }
              else
                os << "while (this->" << enext (c) << " ())"
                   << "{";
            }
            else if (!exceptions)
            {
              // Only sequence can have several choice compositors in a row.
              //
              os << "{";
            }

            if (exceptions)
              os << "switch (this->" << earm (c) << " ())";
            else
              os << earm_tag (c) << " t = this->" << earm (c) << " ();"
                 << endl
                 << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl
                 << "switch (t)";

            os << "{";

            for (SemanticGraph::Choice::ContainsIterator
                   i (c.contains_begin ()); i != c.contains_end (); ++i)
            {
              os << "case " << etag (i->particle ()) << ":"
                 << "{";

              edge_traverser ().dispatch (*i);

              os << "break;"
                 << "}";
            }

            os << "default:"
               << "{"
               << "this->_schema_error (" <<
              "::xsde::cxx::schema_error::unexpected_element);"
               << "return;"
               << "}"
               << "}"; // switch

            if (min == 0 && max == 1)
            {
              os << "}";

              if (!exceptions)
                os << "if (ctx.error_type ())" << endl
                   << "return;"
                   << endl;
            }
            else if (max != 1)
            {
              os << "}";

              if (!exceptions)
                os << "if (ctx.error_type ())" << endl
                   << "return;"
                   << endl;

              if (max != 0 || min != 0)
              {
                if (min != 0)
                {
                  os << "if (i < " << min << "UL)"
                     << "{"
                     << "this->_schema_error (" <<
                    "::xsde::cxx::schema_error::expected_element);"
                     << "return;"
                     << "}";
                }

                os << "}";
              }
            }
            else if (!exceptions)
            {
              os << "}";
            }
          }
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          size_t min (s.min ()), max (s.max ());

          if (min == 0 && max == 1)
          {
            os << "if (this->" << epresent (s) << " ())"
               << "{";
          }
          else if (max != 1)
          {
            // We only need to count if max != unbounded || min != 0.
            //
            if (max != 0 || min != 0)
            {
              os << "{"
                 << "size_t i = 0;"
                 << "for (; ";

              if (max != 0)
                os << "i < " << max << "UL && ";

              os << "this->" << enext (s) << " (); ++i)"
                 << "{";
            }
            else
              os << "while (this->" << enext (s) << " ())"
                 << "{";
          }

          Traversal::Sequence::traverse (s);

          if (min == 0 && max == 1)
          {
            os << "}";

            if (!exceptions)
              os << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl;
          }
          else if (max != 1)
          {
            os << "}";

            if (!exceptions)
              os << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl;

            if (max != 0 || min != 0)
            {
              if (min != 0)
              {
                os << "if (i < " << min << "UL)"
                   << "{"
                   << "this->_schema_error (" <<
                  "::xsde::cxx::schema_error::expected_element);"
                   << "return;"
                   << "}";
              }

              os << "}";
            }
          }
        }
      };

      struct Particle: Traversal::Element,
                       Traversal::Any,
                       Context
      {
        Particle (Context& c)
            : Context (c), any_test_ (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          size_t min (e.min ()), max (e.max ());

          String const& name (ename (e));

          os << "// " << name << endl
             << "//" << endl;

          if (min == 0 && max == 1)
          {
            os << "if (this->" << epresent (e) << " ())";
          }
          else if (max != 1)
          {
            // We only need to count if max != unbounded || min != 0.
            //
            if (max != 0 || min != 0)
            {
              os << "{"
                 << "size_t i = 0;"
                 << "for (; ";

              if (max != 0)
                os << "i < " << max << "UL && ";

              os << "this->" << enext (e) << " (); ++i)";
            }
            else
              os << "while (this->" << enext (e) << " ())";
          }

          os << "{";

          String const& ret (ret_type (e.type ()));
          String const& arg (arg_type (e.type ()));
          String fq_type (fq_name (e.type ()));

          bool poly (poly_code && !anonymous (e.type ()));
          String inst (poly ? String (L"s") : L"this->" + emember (e));

          if (poly)
            os << "ctx.type_id (0);";

          if (ret == L"void")
            os << "this->" << name << " ();"
               << endl;
          else
            os << arg << " r = this->" << name << " ();"
               << endl;

          if (!exceptions)
            os << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;

          if (poly)
          {
            // In case of mixin we use virtual inheritance and only
            // dynamic_cast can be used.
            //
            String cast (mixin ? L"dynamic_cast" : L"static_cast");

            os << "const void* t = ctx.type_id ();"
               << "const char* dt = 0;"
               << fq_type << "* " << inst << " = 0;"
               << endl
               << "if (t == 0 && this->" << emember (e) << " != 0)" << endl
               << inst << " = this->" << emember (e) << ";"
               << "else if (this->" << emember_map (e) << " != 0)"
               << "{"
               << serializer_base << "* b = this->" << emember_map (e) <<
              "->find (t);"
               << endl
               << "if (b != 0)"
               << "{"
               << "dt = b->_dynamic_type ();"
               << "const char* st = " << fq_type << "::_static_type ();"
               << endl
               << "if (strcmp (dt, st) == 0)" << endl
               << "dt = 0;"
               << endl;

            // Check that the types are related by inheritance.
            //
            os << "if (dt != 0 && !::xsde::cxx::serializer::validating::" <<
              "inheritance_map_instance ().check (dt, st))"
               << "{"
               << "ctx.schema_error (::xsde::cxx::schema_error::not_derived);"
               << "return;"
               << "}";

            os << inst << " = " << cast << "< " << fq_type << "* > (b);"
               << "}"
               << "}";
          }

          os << "if (" << inst << ")"
             << "{";

          if (ret == L"void")
            os << inst << "->pre ();";
          else
            os << inst << "->pre (r);";

          if (!exceptions)
          {
            // Note that after pre() we need to check both parser and
            // context error states because of the recursive parsing.
            //
            os << endl
               << "if (" << inst << "->_error_type ())" << endl
               << inst << "->_copy_error (ctx);"
               << endl
               << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;
          }

          // Only a globally-defined element can be a subst-group root.
          //
          if (poly && e.global_p ())
          {
            if (e.qualified_p () && e.namespace_ ().name ())
              os << "const char* ns = " <<
                strlit (e.namespace_ ().name ()) << ";";
            else
              os << "const char* ns = 0;";

            os << "const char* n = " << strlit (e.name ()) << ";"
               << endl;

            os << "if (dt != 0 && " <<
              "::xsde::cxx::serializer::substitution_map_instance ()" <<
              ".check (ns, n, dt, " << (ret == L"void" ? "0" : "&r") <<
              "))" << endl
               << "dt = 0;"
               << endl;

            if (exceptions)
            {
              os << "if (ns != 0)" << endl
                 << "this->_start_element (ns, n);"
                 << "else" << endl
                 << "this->_start_element (n);"
                 << endl;
            }
            else
            {
              os << "if (ns != 0)"
                 << "{"
                 << "if (!this->_start_element (ns, n))" << endl
                 << "return;"
                 << "}"
                 << "else"
                 << "{"
                 << "if (!this->_start_element (n))" << endl
                 << "return;"
                 << "}";
            }
          }
          else
          {
            if (exceptions)
            {
              if (e.qualified_p () && e.namespace_ ().name ())
                os << "this->_start_element (" <<
                  strlit (e.namespace_ ().name ()) << ", " <<
                  strlit (e.name ()) << ");";
              else
                os << "this->_start_element (" << strlit (e.name ()) << ");";
            }
            else
            {
              os << "if (!";

              if (e.qualified_p () && e.namespace_ ().name ())
                os << "this->_start_element (" <<
                  strlit (e.namespace_ ().name ()) << ", " <<
                  strlit (e.name ()) << ")";
              else
                os << "this->_start_element (" << strlit (e.name ()) << ")";

              os << ")" << endl
                 << "return;"
                 << endl;
            }
          }

          if (poly)
          {
            // Set xsi:type if necessary.
            //
            if (exceptions)
              os << "if (dt != 0)" << endl
                 << "this->_set_type (dt);"
                 << endl;
            else
              os << "if (dt != 0)"
                 << "{"
                 << "if (!this->_set_type (dt))" << endl
                 << "return;"
                 << "}";
          }

          os << inst << "->_pre_impl (ctx);";

          os << endl
             << "if (ctx.error_type ())" << endl
             << "return;"
             << endl;

          os << inst << "->_serialize_attributes ();";

          os << endl
             << "if (ctx.error_type ())" << endl
             << "return;"
             << endl;

          os << inst << "->_serialize_content ();";

          os << endl
             << "if (ctx.error_type ())" << endl
             << "return;"
             << endl;

          os << inst << "->_post_impl ();";

          os << endl
             << "if (ctx.error_type ())" << endl
             << "return;"
             << endl;

          if (exceptions)
            os << "this->_end_element ();";
          else
            os << "if (!this->_end_element ())" << endl
               << "return;"
               << endl;

          os << inst << "->post ();";

          if (!exceptions)
          {
            // Note that after post() we need to check both parser and
            // context error states because of the recursive parsing.
            //
            os << endl
               << "if (" << inst << "->_error_type ())" << endl
               << inst << "->_copy_error (ctx);"
               << endl
               << "if (ctx.error_type ())" << endl
               << "return;";
          }

          os << "}"; // if (inst)

          if (min != 0)
          {
            os << "else"
               << "{"
               << "this->_schema_error (" <<
              "::xsde::cxx::schema_error::expected_element);"
               << "return;"
               << "}";
          }

          os << "}";

          if (min == 0 && max == 1)
          {
            if (!exceptions)
              os << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl;
          }
          else if (max != 1)
          {
            if (!exceptions)
              os << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl;

            if (max != 0 || min != 0)
            {
              if (min != 0)
              {
                os << "if (i < " << min << "UL)"
                   << "{"
                   << "this->_schema_error (" <<
                  "::xsde::cxx::schema_error::expected_element);"
                   << "return;"
                   << "}";
              }

              os << "}";
            }
          }
        }

        virtual void
        traverse (SemanticGraph::Any& a)
        {
          size_t min (a.min ()), max (a.max ());

          if (min == 0 && max == 1)
          {
            os << "if (this->" << epresent (a) << " ())";

          }
          else if (max != 1)
          {
            // We only need to count if max != unbounded || min != 0.
            //
            if (max != 0 || min != 0)
            {
              os << "{"
                 << "size_t i = 0;"
                 << "for (; ";

              if (max != 0)
                os << "i < " << max << "UL && ";

              os << "this->" << enext (a) << " (); ++i)";
            }
            else
              os << "while (this->" << enext (a) << " ())";
          }

          os << "{";

          if (stl)
          {
            os << "::std::string ns, name;"
               << "this->" << ename (a) << " (ns, name);"
               << endl;

            if (!exceptions)
              os << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl;

            os << "if (";

            any_test_.dispatch (a);

            os << ")"
               << "{";

            os << "if (ns.empty ())"
               << "{";

            if (exceptions)
              os << "this->_start_element (name.c_str ());";
            else
              os << "if (!this->_start_element (name.c_str ()))" << endl
                 << "return;";

            os << "}"
               << "else"
               << "{";

            if (exceptions)
              os << "this->_start_element (ns.c_str (), name.c_str ());";
            else
              os << "if (!this->_start_element (ns.c_str (), " <<
                "name.c_str ()))" << endl
                 << "return;";

            os << "}"
               << "this->" << eserialize (a) << " ();"
               << endl
               << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;

            if (exceptions)
              os << "this->_end_element ();";
            else
              os << "if (!this->_end_element ())" << endl
                 << "return;";

            os << "}" // test
               << "else"
               << "{"
               << "this->_schema_error (" <<
              "::xsde::cxx::schema_error::unexpected_element);"
               << "return;"
               << "}";
          }
          else
          {
            os << "const char* ns = 0;"
               << "const char* name;"
               << "bool free;"
               << "this->" << ename (a) << " (ns, name, free);"
               << endl;

            if (!exceptions)
              os << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl;
            else
              os << "::xsde::cxx::string auto_ns, auto_name;"
                 << "if (free)"
                 << "{"
                 << "auto_ns.attach (const_cast< char* > (ns));"
                 << "auto_name.attach (const_cast< char* > (name));"
                 << "}";

            os << "if (";

            any_test_.dispatch (a);

            os << ")"
               << "{";

            if (exceptions)
              os << "if (ns == 0 || *ns == '\\0')" << endl
                 << "this->_start_element (name);"
                 << "else" << endl
                 << "this->_start_element (ns, name);"
                 << endl;
            else
            {
              os << "bool r;"
                 << "if (ns == 0 || *ns == '\\0')" << endl
                 << "r = this->_start_element (name);"
                 << "else" << endl
                 << "r = this->_start_element (ns, name);"
                 << endl
                 << "if (free)"
                 << "{";

              if (!custom_alloc)
                os << "delete[] ns;"
                   << "delete[] name;";
              else
                os << "::xsde::cxx::free (const_cast< char* > (ns));"
                   << "::xsde::cxx::free (const_cast< char* > (name));";

              os << "}"
                 << "if (!r)" << endl
                 << "return;"
                 << endl;
            }

            os << "this->" << eserialize (a) << " ();"
               << endl
               << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;

            if (exceptions)
              os << "this->_end_element ();";
            else
              os << "if (!this->_end_element ())" << endl
                 << "return;";

            os << "}" // test
               << "else"
               << "{"
               << "this->_schema_error (" <<
              "::xsde::cxx::schema_error::unexpected_element);"
               << "return;"
               << "}";
          }

          os << "}";

          if (min == 0 && max == 1)
          {
            if (!exceptions)
              os << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl;
          }
          else if (max != 1)
          {
            if (!exceptions)
              os << "if (ctx.error_type ())" << endl
                 << "return;"
                 << endl;

            if (max != 0 || min != 0)
            {
              if (min != 0)
              {
                os << "if (i < " << min << "UL)"
                   << "{"
                   << "this->_schema_error (" <<
                  "::xsde::cxx::schema_error::expected_element);"
                   << "return;"
                   << "}";
              }

              os << "}";
            }
          }
        }

      private:
        AnyTest any_test_;
      };

      //
      //
      struct Complex : Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c),
              compositor_ (c),
              particle_ (c)
        {
          contains_compositor_ >> compositor_;
          compositor_ >> contains_particle_;
          contains_particle_ >> compositor_;
          contains_particle_ >> particle_;
        }

        virtual void
        traverse (Type& c)
        {
          if (!has<Traversal::Element> (c) &&
              !has_particle<Traversal::Any> (c))
            return;

          // Don't use restriction_p here since we don't want special
          // treatment of anyType.
          //
          bool restriction (
            c.inherits_p () &&
            c.inherits ().is_a<SemanticGraph::Restricts> ());

          String const& name (ename (c));

          os <<"// Element validation and serialization for " <<
            name << "." << endl
             <<"//" << endl;

          os << "void " << name << "::" << endl
             << "_serialize_content ()"
             << "{"
             << "::xsde::cxx::serializer::context& ctx = this->_context ();"
             << endl;

          if (c.inherits_p () && !restriction)
          {
            // We cannot use the fully-qualified base name directly
            // because of some broken compilers (EVC 4.0).
            //
            String base (unclash (name, "base"));

            os << "typedef " << fq_name (c.inherits ().base ()) << " " <<
              base << ";"
               << base << "::_serialize_content ();"
               << endl
               << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;
          }

          contains_compositor (c, contains_compositor_);

          os << "}";
        }

      private:
        Compositor compositor_;
        Particle particle_;
        Traversal::ContainsCompositor contains_compositor_;
        Traversal::ContainsParticle contains_particle_;
      };
    }

    void
    generate_element_validation_source (Context& ctx)
    {
      Traversal::Schema schema;

      Sources sources;
      Traversal::Names schema_names;

      Namespace ns (ctx);
      Traversal::Names names;

      schema >> sources >> schema;
      schema >> schema_names >> ns >> names;

      Complex complex (ctx);

      names >> complex;

      schema.dispatch (ctx.schema_root);
    }
  }
}
