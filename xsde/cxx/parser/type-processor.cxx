// file      : xsde/cxx/parser/type-processor.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2006-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <set>

#include <cxx/parser/elements.hxx>
#include <cxx/parser/type-processor.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

using namespace std;

namespace CXX
{
  namespace Parser
  {
    namespace
    {
      //
      //
      struct Type: Traversal::Type
      {
        Type (SemanticGraph::Schema& schema,
              TypeMap::Namespaces& type_map,
              bool add_includes)
            : schema_ (schema),
              type_map_ (type_map),
              add_includes_ (add_includes)
        {
        }

        virtual void
        traverse (SemanticGraph::Type& type)
        {
          using TypeMap::Namespace;
          using TypeMap::Namespaces;

          SemanticGraph::Context& tc (type.context ());

          // There are two situations where we may try to process the
          // same type more than once. The first is when the type is
          // used in several element declarations in the same schema.
          // The second situation only occurs when we are in the file-
          // per-type mode. In this case the type was processed as part
          // of another root schema. In the second case, while the ret
          // and arg types are assumed to be the same, we need to re-
          // match the type in order to add include directives to the
          // new root schema.
          //
          bool set (true);

          if (tc.count ("p:ret-type"))
          {
            SemanticGraph::Schema* s (
              tc.get<SemanticGraph::Schema*> ("p:root-schema"));

            if (&schema_ == s)
              return;

            set = false;
          }

          SemanticGraph::Namespace& ns (
            dynamic_cast<SemanticGraph::Namespace&> (type.scope ()));

          String const& ns_name (ns.name ());
          String const& t_name (type.name ());

          // std::wcerr << "traversing: " << ns_name << "#" << t_name << endl;

          for (Namespaces::const_iterator n (type_map_.begin ());
               n != type_map_.end (); ++n)
          {
            // Check if the namespace matches.
            //
            bool ns_match;

            if (!n->xsd_name ().empty ())
            {
              ns_match = n->xsd_name ().match (ns_name);
            }
            else
              ns_match = ns_name.empty ();

            // std::wcerr << "considering ns expr: " << n->xsd_name () << endl;

            if (ns_match)
            {
              // Namespace matched. See if there is a type that matches.
              //
              for (Namespace::TypesIterator t (n->types_begin ());
                   t != n->types_end (); ++t)
              {
                if (t->xsd_name ().match (t_name))
                {
                  if (set)
                  {
                    // Got a match. See if the namespace has the C++
                    // namespace mapping.
                    //
                    String cxx_ns;

                    if (n->has_cxx_name ())
                    {
                      if (!n->xsd_name ().empty ())
                      {
                        cxx_ns = n->xsd_name ().replace (
                          ns_name, n->cxx_name (), true);
                      }
                      else
                        cxx_ns = n->cxx_name ();

                      cxx_ns += L"::";
                    }

                    // Figure out ret and arg type names.
                    //
                    String ret_type (cxx_ns);

                    ret_type += t->xsd_name ().replace (
                      t_name, t->cxx_ret_name (), true);

                    String arg_type;

                    if (t->cxx_arg_name ())
                    {
                      arg_type = cxx_ns;
                      arg_type += t->xsd_name ().replace (
                        t_name, t->cxx_arg_name (), true);
                    }
                    else
                    {
                      if (ret_type == L"void")
                        arg_type = ret_type;
                      else
                      {
                        wchar_t last (ret_type[ret_type.size () - 1]);

                        // If it is already a pointer or reference then use
                        // it as is.
                        //
                        if (last == L'*' || last == L'&')
                          arg_type = ret_type;
                        else
                          arg_type = L"const " + ret_type + L"&";
                      }
                    }

                    tc.set ("p:ret-type", ret_type);
                    tc.set ("p:arg-type", arg_type);
                  }

                  tc.set ("p:root-schema", &schema_);

                  //std::wcerr << t_name << " -> " << ret_type << endl;

                  // See of we need to add any includes to the translations
                  // unit.
                  //
                  if (add_includes_)
                  {
                    if (n->includes_begin () != n->includes_end ())
                    {
                      typedef std::set<String> Includes;

                      SemanticGraph::Context& sc (schema_.context ());

                      if (!sc.count ("p:includes"))
                        sc.set ("p:includes", Includes ());

                      Includes& is (sc.get<Includes> ("p:includes"));

                      for (Namespace::IncludesIterator i (n->includes_begin ());
                           i != n->includes_end (); ++i)
                      {
                        is.insert (*i);
                      }
                    }
                  }

                  return;
                }
              }
            }
          }
        }

      private:
        SemanticGraph::Schema& schema_;
        TypeMap::Namespaces& type_map_;
        bool add_includes_;
      };

      //
      //
      struct BaseType: Traversal::Complex
      {
        virtual void
        traverse (SemanticGraph::Complex& c)
        {
          Complex::inherits (c);
        }

        // @@ What about list?
      };

      //
      //
      struct GlobalType: Traversal::Type,
                         Traversal::List,
                         Traversal::Complex,
                         Traversal::Enumeration
      {
        GlobalType (SemanticGraph::Schema& schema,
                    TypeMap::Namespaces& type_map,
                    bool add_includes,
                    bool tiein)
            : type_ (schema, type_map, add_includes)
        {
          inherits_ >> type_;
          names_ >> instance_ >> belongs_ >> type_;
          argumented_ >> type_;

          if (tiein)
          {
            // In case of a tiein support, we also need to process base's
            // base type.
            //
            inherits_base_ >> base_type_ >> inherits_level_2_ >> type_;
          }
        }

        virtual void
        traverse (SemanticGraph::Type& t)
        {
          type_.traverse (t);
        }

        virtual void
        traverse (SemanticGraph::List& l)
        {
          type_.traverse (l);
          Traversal::List::argumented (l, argumented_);
        }

        virtual void
        traverse (SemanticGraph::Complex& c)
        {
          type_.traverse (c);
          Complex::inherits (c, inherits_);
          Complex::inherits (c, inherits_base_);
          Complex::names (c, names_);
        }

        virtual void
        traverse (SemanticGraph::Enumeration& e)
        {
          type_.traverse (e);
          Complex::inherits (e, inherits_);
        }

      private:
        Parser::Type type_;
        BaseType base_type_;
        Traversal::Names names_;
        Traversal::Instance instance_;
        Traversal::Inherits inherits_;
        Traversal::Inherits inherits_base_;
        Traversal::Inherits inherits_level_2_;
        Traversal::Belongs belongs_;
        Traversal::Argumented argumented_;
      };

      void
      process_impl (options const& ops,
                    XSDFrontend::SemanticGraph::Schema& tu,
                    TypeMap::Namespaces& type_map)
      {
        bool tiein (!ops.reuse_style_mixin () && !ops.reuse_style_none ());

        if (tu.names_begin ()->named ().name () ==
            L"http://www.w3.org/2001/XMLSchema")
        {
          // XML Schema namespace.
          //
          Traversal::Schema schema;

          Traversal::Names schema_names;
          Traversal::Namespace ns;
          Traversal::Names ns_names;
          GlobalType global_type (tu, type_map, true, tiein);

          schema >> schema_names >> ns >> ns_names >> global_type;

          schema.dispatch (tu);
        }
        else
        {
          // If --extern-xml-schema is specified, then we don't want
          // includes from the XML Schema type map.
          //
          bool extern_xml_schema (ops.extern_xml_schema ());

          // Besides types defined in this schema, also process those
          // referenced by global elements in case we are generating
          // something for them.
          //
          Traversal::Schema schema;
          Traversal::Schema xs_schema;
          Sources sources;
          Traversal::Implies implies;

          schema >> sources >> schema;
          schema >> implies >> xs_schema;

          Traversal::Names schema_names;
          Traversal::Namespace ns;
          Traversal::Names ns_names;
          Traversal::Element global_element;
          GlobalType global_type (tu, type_map, true, tiein);

          schema >> schema_names >> ns >> ns_names;

          ns_names >> global_element;
          ns_names >> global_type;

          Traversal::Belongs element_belongs;
          global_element >> element_belongs >> global_type;

          Traversal::Names xs_schema_names;
          Traversal::Namespace xs_ns;
          Traversal::Names xs_ns_names;
          GlobalType xs_global_type (tu, type_map, !extern_xml_schema, tiein);

          xs_schema >> xs_schema_names >> xs_ns >> xs_ns_names >>
            xs_global_type;

          schema.dispatch (tu);
        }
      }
    }

    void TypeProcessor::
    process (options const& ops,
             XSDFrontend::SemanticGraph::Schema& s,
             TypeMap::Namespaces& tm)
    {
      process_impl (ops, s, tm);
    }
  }
}
