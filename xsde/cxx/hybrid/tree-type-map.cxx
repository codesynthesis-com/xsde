// file      : xsde/cxx/hybrid/tree-type-map.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <set>
#include <iostream>

#include <cxx/elements.hxx>
#include <cxx/hybrid/tree-type-map.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

using namespace std;

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      class Context: public CXX::Context
      {
      public:
        Context (Hybrid::options const& ops,
                 SemanticGraph::Schema& root,
                 SemanticGraph::Path const& path)
            : CXX::Context (std::wcerr, root, path, ops, "name", "char")
        {
        }

      protected:
        Context (Context& c)
            : CXX::Context (c)
        {
        }
      };

      //
      //
      struct GlobalType: Traversal::Type, Context
      {
        GlobalType (Context& c,
                    TypeMap::Namespace* parser,
                    TypeMap::Namespace* serializer)
            : Context (c), parser_ (parser), serializer_ (serializer)
        {
        }

        virtual void
        traverse (SemanticGraph::Type& t)
        {
          String const& xn (t.name ());
          String qn (fq_name (t));

          if (parser_ != 0)
          {
            if (t.context ().get<bool> ("fixed"))
              parser_->types_push_back (xn, qn);
            else
              parser_->types_push_back (xn, qn + L"*");
          }

          if (serializer_ != 0)
            serializer_->types_push_back (xn, L"const " + qn + L"&");
        }

      private:
        TypeMap::Namespace* parser_;
        TypeMap::Namespace* serializer_;
      };


      struct Namespace: Traversal::Namespace, Context
      {
        Namespace (Context& c,
                   String const* hxx_name,
                   TypeMap::Namespaces* parser_map,
                   TypeMap::Namespaces* serializer_map)
            : Context (c),
              hxx_name_ (hxx_name),
              parser_map_ (parser_map),
              serializer_map_ (serializer_map)
        {
        }

        virtual void
        traverse (Type& ns)
        {
          String include;

          if (hxx_name_ != 0)
            include = process_include_path (*hxx_name_);

          String const& name (ns.name ());

          if (parser_map_ != 0)
          {
            if (name)
              parser_map_->push_back (TypeMap::Namespace (name));
            else
              parser_map_->push_back (
                TypeMap::Namespace (TypeMap::Pattern ()));

            if (include)
              parser_map_->back ().includes_push_back (include);
          }

          if (serializer_map_ != 0)
          {
            if (name)
              serializer_map_->push_back (TypeMap::Namespace (name));
            else
              serializer_map_->push_back (
                TypeMap::Namespace (TypeMap::Pattern ()));

            if (include)
              serializer_map_->back ().includes_push_back (include);
          }

          GlobalType type (*this,
            (parser_map_ ? &parser_map_->back () : 0),
            (serializer_map_ ? &serializer_map_->back () : 0));

          Traversal::Names names (type);
          Namespace::names (ns, names);
        }

      private:
        String const* hxx_name_;
        TypeMap::Namespaces* parser_map_;
        TypeMap::Namespaces* serializer_map_;
      };

      // Go into sourced/included/imported schemas while making sure
      // we don't process the same stuff more than once.
      //
      struct Uses: Traversal::Sources,
                   Traversal::Includes,
                   Traversal::Imports
      {
        Uses (SemanticGraph::Schema& root)
        {
          schema_set_.insert (&root);
        }

        virtual void
        traverse (SemanticGraph::Sources& sr)
        {
          SemanticGraph::Schema& s (sr.schema ());
          SemanticGraph::Context& sc (s.context ());

          if (!sc.count ("cxx-hybrid-tree-type-map-seen"))
          {
            sc.set ("cxx-hybrid-tree-type-map-seen", true);
            Traversal::Sources::traverse (sr);
            sc.remove ("cxx-hybrid-tree-type-map-seen");
          }
        }

        virtual void
        traverse (SemanticGraph::Includes& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (schema_set_.find (&s) == schema_set_.end ())
          {
            schema_set_.insert (&s);
            Traversal::Includes::traverse (i);
          }
        }

        virtual void
        traverse (SemanticGraph::Imports& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (schema_set_.find (&s) == schema_set_.end ())
          {
            schema_set_.insert (&s);
            Traversal::Imports::traverse (i);
          }
        }

      private:
        set<SemanticGraph::Schema*> schema_set_;
      };
    }

    void
    generate_tree_type_map (options const& ops,
                            XSDFrontend::SemanticGraph::Schema& tu,
                            XSDFrontend::SemanticGraph::Path const& path,
                            String const& hxx_name,
                            TypeMap::Namespaces& parser_map,
                            TypeMap::Namespaces& serializer_map)
    {
      if (tu.names_begin ()->named ().name () !=
          L"http://www.w3.org/2001/XMLSchema")
      {
        Context ctx (ops, tu, path);

        // We don't want include in the included/imported/sources
        // schema so split the traversal into two part.
        //
        Traversal::Schema schema;
        Traversal::Schema used_schema;
        Uses uses (tu);

        schema >> uses >> used_schema >> uses;

        Traversal::Names schema_names;
        Namespace ns (
          ctx, &hxx_name,
          (ops.generate_parser () ? &parser_map : 0),
          (ops.generate_serializer () ? &serializer_map : 0));

        schema >> schema_names >> ns;

        Traversal::Names used_schema_names;
        Namespace used_ns (
          ctx, 0,
          (ops.generate_parser () ? &parser_map : 0),
          (ops.generate_serializer () ? &serializer_map : 0));

        used_schema >> used_schema_names >> used_ns;

        // Some twisted schemas do recusive self-inclusion.
        //
        SemanticGraph::Context& tuc (tu.context ());

        tuc.set ("cxx-hybrid-tree-type-map-seen", true);
        schema.dispatch (tu);
        tuc.remove ("cxx-hybrid-tree-type-map-seen");
      }
    }
  }
}
