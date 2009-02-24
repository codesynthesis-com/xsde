// file      : xsde/cxx/hybrid/parser-aggregate-source.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/hybrid/parser-aggregate-source.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

#include <cult/containers/map.hxx>

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      typedef
      Cult::Containers::Map<SemanticGraph::Type*, String>
      TypeInstanceMap;

      //
      //
      struct ParticleArg: Traversal::Element, Context
      {
        ParticleArg (Context& c, TypeInstanceMap& map, Boolean& first)
            : Context (c), map_ (map), first_ (first)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          if (!first_)
            os << "," << endl;
          else
            first_ = false;

          os << "this->" << map_[&e.type ()];
        }

      private:
        TypeInstanceMap& map_;
        Boolean& first_;
      };

      struct AttributeArg: Traversal::Attribute, Context
      {
        AttributeArg (Context& c, TypeInstanceMap& map, Boolean& first)
            : Context (c), map_ (map), first_ (first)
        {
        }

        virtual Void
        traverse (Type& a)
        {
          if (!first_)
            os << "," << endl;
          else
            first_ = false;

          os << "this->" << map_[&a.type ()];
        }

      private:
        TypeInstanceMap& map_;
        Boolean& first_;
      };

      struct ArgList : Traversal::Complex,
                       Traversal::List,
                       Context
      {
        ArgList (Context& c, TypeInstanceMap& map)
            : Context (c),
              map_ (map),
              particle_ (c, map, first_),
              attribute_ (c, map, first_),
              first_ (true)
        {
          inherits_ >> *this;

          contains_compositor_ >> compositor_;
          compositor_ >> contains_particle_;
          contains_particle_ >> compositor_;
          contains_particle_ >> particle_;

          names_ >> attribute_;
        }

        virtual Void
        traverse (SemanticGraph::Complex& c)
        {
          inherits (c, inherits_);

          if (!restriction_p (c))
          {
            names (c, names_);
            contains_compositor (c, contains_compositor_);
          }
        }

        virtual Void
        traverse (SemanticGraph::List& l)
        {
          if (!first_)
            os << "," << endl;
          else
            first_ = false;

          os << "this->" << map_[&l.argumented ().type ()];
        }

      private:
        TypeInstanceMap& map_;

        Traversal::Inherits inherits_;

        Traversal::Compositor compositor_;
        ParticleArg particle_;
        Traversal::ContainsCompositor contains_compositor_;
        Traversal::ContainsParticle contains_particle_;

        Traversal::Names names_;
        AttributeArg attribute_;

        Boolean first_;
      };

      struct ParserConnect: Traversal::List,
                            Traversal::Complex,
                            Context
      {
        ParserConnect (Context& c, TypeInstanceMap& map)
            : Context (c), map_ (map)
        {
        }

        virtual Void
        traverse (SemanticGraph::List& l)
        {
          os << "this->" << map_[&l] << ".parsers (this->" <<
            map_[&l.argumented ().type ()] << ");"
             << endl;
        }

        virtual Void
        traverse (SemanticGraph::Complex& c)
        {
          if (has_members (c))
          {
            os << "this->" << map_[&c] << ".parsers (";

            ArgList args (*this, map_);
            args.dispatch (c);

            os << ");"
               << endl;
          }
        }

      private:
        Boolean
        has_members (SemanticGraph::Complex& c)
        {
          using SemanticGraph::Complex;

          if (has<Traversal::Member> (c))
            return true;

          if (c.inherits_p ())
          {
            SemanticGraph::Type& b (c.inherits ().base ());

            if (Complex* cb = dynamic_cast<Complex*> (&b))
              return has_members (*cb);

            return b.is_a<SemanticGraph::List> ();
          }

          return false;
        }

      private:
        TypeInstanceMap& map_;
      };

      struct GlobalType: Traversal::Type, Context
      {
        GlobalType (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Type& t)
        {
          SemanticGraph::Context& tc (t.context ());

          if (!tc.count ("paggr"))
            return;

          String const& name (tc.get<String> ("paggr"));
          TypeInstanceMap& map (tc.get<TypeInstanceMap> ("paggr-map"));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          // c-tor ()
          //
          os << name << "::" << endl
             << name << " ()"
             << "{";

          ParserConnect connect (*this, map);

          for (TypeInstanceMap::Iterator i (map.begin ()), end (map.end ());
               i != end; ++i)
            connect.dispatch (*i->first);

          os << "}";
        }
      };

      struct GlobalElement: Traversal::Element, Context
      {
        GlobalElement (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          SemanticGraph::Context& ec (e.context ());

          if (!ec.count ("paggr"))
            return;

          String const& name (ec.get<String> ("paggr"));
          TypeInstanceMap& map (ec.get<TypeInstanceMap> ("paggr-map"));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          // c-tor ()
          //
          os << name << "::" << endl
             << name << " ()"
             << "{";

          ParserConnect connect (*this, map);

          for (TypeInstanceMap::Iterator i (map.begin ()), end (map.end ());
               i != end; ++i)
            connect.dispatch (*i->first);

          os << "}";

          // root_name ()
          //
          String root_name (unclash (name, "root_name"));

          os << "const char* " << name << "::" << endl
             << root_name << " ()"
             << "{"
             << "return " << strlit (e.name ()) << ";"
             << "}";

          // root_namespace ()
          //
          String root_namespace (unclash (name, "root_namespace"));

          os << "const char* " << name << "::" << endl
             << root_namespace << " ()"
             << "{"
             << "return " << strlit (e.namespace_ ().name ()) << ";"
             << "}";
        }
      };
    }

    Void
    generate_parser_aggregate_source (Context& ctx)
    {
      Boolean gen (false);

      {
        Traversal::Schema schema;
        Traversal::Sources sources;

        schema >> sources >> schema;

        Traversal::Names schema_names;
        Traversal::Namespace ns;
        Traversal::Names names;
        AggregateTest test (gen, "paggr");

        schema >> schema_names >> ns >> names >> test;

        schema.dispatch (ctx.schema_root);
      }

      if (gen)
      {
        Traversal::Schema schema;
        Traversal::Sources sources;

        schema >> sources >> schema;

        Traversal::Names schema_names;
        Namespace ns (ctx);
        Traversal::Names names;

        schema >> schema_names >> ns >> names;

        GlobalType type (ctx);
        GlobalElement element (ctx);

        names >> type;
        names >> element;

        schema.dispatch (ctx.schema_root);
      }
    }
  }
}
