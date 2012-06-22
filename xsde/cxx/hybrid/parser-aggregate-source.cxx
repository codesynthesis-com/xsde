// file      : xsde/cxx/hybrid/parser-aggregate-source.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/hybrid/parser-aggregate-source.hxx>
#include <cxx/hybrid/aggregate-elements.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>


namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      //
      //
      struct ParticleArg: Traversal::Element, Context
      {
        ParticleArg (Context& c,
                     TypeInstanceMap& map,
                     bool& first,
                     bool poly,
                     String const& arg)
            : Context (c),
              poly_ (poly),
              map_ (&map),
              first_ (&first),
              result_ (0),
              arg_ (arg)
        {
        }

        ParticleArg (Context& c, bool& result, bool poly)
            : Context (c),
              poly_ (poly),
              map_ (0),
              first_ (0),
              result_ (&result)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          if (poly_ && anonymous (e.type ()))
            return;

          if (result_ != 0)
          {
            *result_ = true;
            return;
          }

          if (!*first_)
            os << "," << endl;
          else
            *first_ = false;

          if (poly_)
            os << "this->" << arg_;
          else
            os << "this->" << (*map_)[&e.type ()];
        }

      private:
        bool poly_;
        TypeInstanceMap* map_;
        bool* first_;
        bool* result_;
        String arg_;
      };

      struct AttributeArg: Traversal::Attribute, Context
      {
        AttributeArg (Context& c, TypeInstanceMap& map, bool& first)
            : Context (c), map_ (&map), first_ (&first), result_ (0)
        {
        }

        AttributeArg (Context& c, bool& result)
            : Context (c), map_ (0), first_ (0), result_ (&result)
        {
        }

        virtual void
        traverse (Type& a)
        {
          if (result_ != 0)
          {
            *result_ = true;
            return;
          }

          if (!*first_)
            os << "," << endl;
          else
            *first_ = false;

          os << "this->" << (*map_)[&a.type ()];
        }

      private:
        TypeInstanceMap* map_;
        bool* first_;
        bool* result_;
      };

      struct ArgList : Traversal::Complex,
                       Traversal::List,
                       Context
      {
        ArgList (Context& c,
                 TypeInstanceMap& map,
                 bool poly,
                 String const& arg)
            : Context (c),
              poly_ (poly),
              map_ (&map),
              particle_ (c, map, first_, poly, arg),
              attribute_ (c, map, first_),
              first_ (true),
              result_ (0)
        {
          inherits_ >> *this;

          contains_compositor_ >> compositor_;
          compositor_ >> contains_particle_;
          contains_particle_ >> compositor_;
          contains_particle_ >> particle_;

          if (!poly_)
            names_ >> attribute_;
        }

        ArgList (Context& c, bool& result, bool poly)
            : Context (c),
              poly_ (poly),
              map_ (0),
              particle_ (c, result, poly),
              attribute_ (c, result),
              result_ (&result)
        {
          inherits_ >> *this;

          contains_compositor_ >> compositor_;
          compositor_ >> contains_particle_;
          contains_particle_ >> compositor_;
          contains_particle_ >> particle_;

          if (!poly_)
            names_ >> attribute_;
        }

        virtual void
        traverse (SemanticGraph::Complex& c)
        {
          inherits (c, inherits_);

          if (!restriction_p (c))
          {
            names (c, names_);
            contains_compositor (c, contains_compositor_);
          }
        }

        virtual void
        traverse (SemanticGraph::List& l)
        {
          if (poly_)
            return;

          if (result_ != 0)
          {
            *result_ = true;
            return;
          }

          if (!first_)
            os << "," << endl;
          else
            first_ = false;

          os << "this->" << (*map_)[&l.argumented ().type ()];
        }

      private:
        bool poly_;
        TypeInstanceMap* map_;

        Traversal::Inherits inherits_;

        Traversal::Compositor compositor_;
        ParticleArg particle_;
        Traversal::ContainsCompositor contains_compositor_;
        Traversal::ContainsParticle contains_particle_;

        Traversal::Names names_;
        AttributeArg attribute_;

        bool first_;
        bool* result_;
      };

      struct ParserConnect: Traversal::List,
                            Traversal::Complex,
                            Context
      {
        ParserConnect (Context& c,
                       TypeInstanceMap& map,
                       bool poly,
                       String const& map_inst = String ())
            : Context (c), poly_ (poly), map_ (map), map_inst_ (map_inst)
        {
        }

        virtual void
        traverse (SemanticGraph::List& l)
        {
          if (poly_)
            return;

          os << "this->" << map_[&l] << ".parsers (this->" <<
            map_[&l.argumented ().type ()] << ");"
             << endl;
        }

        virtual void
        traverse (SemanticGraph::Complex& c)
        {
          if (has_members (c))
          {
            os << "this->" << map_[&c] << "." <<
              (poly_ ? "parser_maps" : "parsers") << " (";

            ArgList args (*this, map_, poly_, map_inst_);
            args.dispatch (c);

            os << ");"
               << endl;
          }
        }

      private:
        bool
        has_members (SemanticGraph::Complex& c)
        {
          bool r (false);
          ArgList test (*this, r, poly_);
          test.traverse (c);
          return r;
        }

      private:
        bool poly_;
        TypeInstanceMap& map_;
        String map_inst_;
      };


      //
      //
      struct GlobalType: Traversal::Type, Context
      {
        GlobalType (Context& c)
            : Context (c)
        {
        }

        virtual void
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

          TypeIdInstanceMap* tid_map (0);

          if (poly_code && tc.count ("paggr-tid-map"))
            tid_map = &tc.get<TypeIdInstanceMap> ("paggr-tid-map");

          // c-tor ()
          //
          os << name << "::" << endl
             << name << " ()";

          if (tid_map)
          {
            os << endl
               << ": " << tc.get<String> ("paggr-parser-map") << " (" <<
              tc.get<String> ("paggr-parser-map-entries") << ", " <<
              tid_map->size () << "UL)";
          }

          os << "{";

          // Populate the polymorphic parser map.
          //
          if (tid_map)
          {
            String const& entry (tc.get<String> ("paggr-parser-map-entries"));

            size_t n (0);

            for (TypeIdInstanceMap::iterator i (tid_map->begin ());
                 i != tid_map->end ();
                 ++i, ++n)
            {
              os << entry << "[" << n << "UL].type_id = " <<
                fq_name (*i->second.type, "p:name") << "::_static_type ();"
                 << entry << "[" << n << "UL].parser = &this->" <<
                i->second.name <<  ";"
                 << endl;
            }
          }

          // Connect parsers.
          //
          ParserConnect connect (*this, map, false);

          for (TypeInstanceMap::iterator i (map.begin ()), end (map.end ());
               i != end; ++i)
            connect.dispatch (*i->first);

          // Connect the parser map.
          //
          if (tid_map)
          {
            ParserConnect connect (
              *this, map, true, tc.get<String> ("paggr-parser-map"));

            for (TypeInstanceMap::iterator i (map.begin ()), end (map.end ());
                 i != end; ++i)
              connect.dispatch (*i->first);
          }

          os << "}";
        }
      };

      struct GlobalElement: Traversal::Element, Context
      {
        GlobalElement (Context& c)
            : Context (c)
        {
        }

        virtual void
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

          TypeIdInstanceMap* tid_map (0);

          if (poly_code && ec.count ("paggr-tid-map"))
            tid_map = &ec.get<TypeIdInstanceMap> ("paggr-tid-map");

          // c-tor ()
          //
          os << name << "::" << endl
             << name << " ()";

          if (tid_map)
          {
            os << endl
               << ": " << ec.get<String> ("paggr-parser-map") << " (" <<
              ec.get<String> ("paggr-parser-map-entries") << ", " <<
              tid_map->size () << "UL)";
          }

          os << "{";

          // Populate the polymorphic parser map.
          //
          if (tid_map)
          {
            String const& entry (ec.get<String> ("paggr-parser-map-entries"));

            size_t n (0);

            for (TypeIdInstanceMap::iterator i (tid_map->begin ());
                 i != tid_map->end ();
                 ++i, ++n)
            {
              os << entry << "[" << n << "UL].type_id = " <<
                fq_name (*i->second.type, "p:name") << "::_static_type ();"
                 << entry << "[" << n << "UL].parser = &this->" <<
                i->second.name <<  ";"
                 << endl;
            }
          }

          // Connect parsers.
          //
          ParserConnect connect (*this, map, false);

          for (TypeInstanceMap::iterator i (map.begin ()), end (map.end ());
               i != end; ++i)
            connect.dispatch (*i->first);

          // Connect the parser map.
          //
          if (tid_map)
          {
            ParserConnect connect (
              *this, map, true, ec.get<String> ("paggr-parser-map"));

            for (TypeInstanceMap::iterator i (map.begin ()), end (map.end ());
                 i != end; ++i)
              connect.dispatch (*i->first);
          }

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

    void
    generate_parser_aggregate_source (Context& ctx)
    {
      bool gen (false);

      {
        Traversal::Schema schema;
        Sources sources;

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
        Sources sources;

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
