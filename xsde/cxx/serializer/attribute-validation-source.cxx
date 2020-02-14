// file      : xsde/cxx/serializer/attribute-validation-source.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/serializer/attribute-validation-source.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

namespace CXX
{
  namespace Serializer
  {
    namespace
    {
      struct AnyAttributeTest: Traversal::AnyAttribute, Context
      {
        AnyAttributeTest (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::AnyAttribute& a)
        {
          String const& ns (a.definition_namespace ().name ());

          for (SemanticGraph::AnyAttribute::NamespaceIterator
                 i (a.namespace_begin ()), e (a.namespace_end ()); i != e;)
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

      struct Attribute: Traversal::Attribute,
                        Traversal::AnyAttribute,
                        Context
      {
        Attribute (Context& c)
            : Context (c), any_attribute_test_ (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Attribute& a)
        {
          String const& name (ename (a));

          os << "// " << name << endl
             << "//" << endl;

          if (a.optional_p ())
          {
            os << "if (this->" << epresent (a) << " ())";
          }

          os << "{";

          String const& inst (emember (a));
          String const& ret (ret_type (a.type ()));
          String const& arg (arg_type (a.type ()));

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

          os << "if (this->" << inst << ")"
             << "{";

          if (ret == L"void")
            os << "this->" << inst << "->pre ();";
          else
            os << "this->" << inst << "->pre (r);";

          if (!exceptions)
            os << endl
               << "if (this->" << inst << "->_error_type ())" << endl
               << "this->" << inst << "->_copy_error (ctx);"
               << endl
               << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;

          if (exceptions)
          {
            if (a.qualified_p () && a.namespace_ ().name ())
              os << "this->_start_attribute (" <<
                strlit (a.namespace_ ().name ()) << ", " <<
                strlit (a.name ()) << ");";
            else
              os << "this->_start_attribute (" << strlit (a.name ()) << ");";
          }
          else
          {
            os << "if (!";

            if (a.qualified_p () && a.namespace_ ().name ())
              os << "this->_start_attribute (" <<
                strlit (a.namespace_ ().name ()) << ", " <<
                strlit (a.name ()) << ")";
            else
              os << "this->_start_attribute (" << strlit (a.name ()) << ")";

            os << ")" << endl
               << "return;"
               << endl;
          }

          os << "this->" << inst << "->_pre_impl (ctx);";

          os << endl
             << "if (ctx.error_type ())" << endl
             << "return;"
             << endl;

          os << "this->" << inst << "->_serialize_content ();";

          os << endl
             << "if (ctx.error_type ())" << endl
             << "return;"
             << endl;

          os << "this->" << inst << "->_post_impl ();";

          os << endl
             << "if (ctx.error_type ())" << endl
             << "return;"
             << endl;

          if (exceptions)
            os << "this->_end_attribute ();";
          else
            os << "if (!this->_end_attribute ())" << endl
               << "return;"
               << endl;

          os << "this->" << inst << "->post ();";

          if (!exceptions)
            os << endl
               << "if (this->" << inst << "->_error_type ())" << endl
               << "this->" << inst << "->_copy_error (ctx);"
               << endl
               << "if (ctx.error_type ())" << endl
               << "return;";

          os << "}"; // if (inst)

          if (!a.optional_p ())
          {
            os << "else"
               << "{"
               << "this->_schema_error (" <<
              "::xsde::cxx::schema_error::expected_attribute);"
               << "return;"
               << "}";
          }

          os << "}";

          if (a.optional_p () && !exceptions)
          {
            os << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;
          }
        }

        virtual void
        traverse (SemanticGraph::AnyAttribute& a)
        {
          os << "while (this->" << enext (a) << " ())"
             << "{";

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

            any_attribute_test_.dispatch (a);

            os << ")"
               << "{";

            os << "if (ns.empty ())"
               << "{";

            if (exceptions)
              os << "this->_start_attribute (name.c_str ());";
            else
              os << "if (!this->_start_attribute (name.c_str ()))" << endl
                 << "return;";

            os << "}"
               << "else"
               << "{";

            if (exceptions)
              os << "this->_start_attribute (ns.c_str (), name.c_str ());";
            else
              os << "if (!this->_start_attribute (ns.c_str (), " <<
                "name.c_str ()))" << endl
                 << "return;";

            os << "}"
               << "this->" << eserialize (a) << " ();"
               << endl
               << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;

            if (exceptions)
              os << "this->_end_attribute ();";
            else
              os << "if (!this->_end_attribute ())" << endl
                 << "return;";

            os << "}" // test
               << "else"
               << "{"
               << "this->_schema_error (" <<
              "::xsde::cxx::schema_error::unexpected_attribute);"
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

            any_attribute_test_.dispatch (a);

            os << ")"
               << "{";

            if (exceptions)
              os << "if (ns == 0 || *ns == '\\0')" << endl
                 << "this->_start_attribute (name);"
                 << "else" << endl
                 << "this->_start_attribute (ns, name);"
                 << endl;
            else
            {
              os << "bool r;"
                 << "if (ns == 0 || *ns == '\\0')" << endl
                 << "r = this->_start_attribute (name);"
                 << "else" << endl
                 << "r = this->_start_attribute (ns, name);"
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
              os << "this->_end_attribute ();";
            else
              os << "if (!this->_end_attribute ())" << endl
                 << "return;";

            os << "}" // test
               << "else"
               << "{"
               << "this->_schema_error (" <<
              "::xsde::cxx::schema_error::unexpected_attribute);"
               << "return;"
               << "}";
          }

          os << "}";

          if (!exceptions)
            os << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;
        }

      private:
        AnyAttributeTest any_attribute_test_;
      };

      struct Complex : Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c),
              attribute_ (c)
        {
          names_attribute_ >> attribute_;
        }

        virtual void
        traverse (Type& c)
        {
          if (!has<Traversal::Attribute> (c) &&
              !has<Traversal::AnyAttribute> (c))
            return;

          // Don't use restriction_p here since we don't want special
          // treatment of anyType.
          //
          bool restriction (
            c.inherits_p () &&
            c.inherits ().is_a<SemanticGraph::Restricts> ());

          String const& name (ename (c));

          os <<"// Attribute validation and serialization for " <<
            name << "." << endl
             <<"//" << endl;

          os << "void " << name << "::" << endl
             << "_serialize_attributes ()"
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
               << base << "::_serialize_attributes ();"
               << endl
               << "if (ctx.error_type ())" << endl
               << "return;"
               << endl;
          }

          names (c, names_attribute_);

          os << "}";
        }

      private:
        Attribute attribute_;
        Traversal::Names names_attribute_;
      };
    }

    void
    generate_attribute_validation_source (Context& ctx)
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
