// file      : xsde/cxx/hybrid/serializer-aggregate-source.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/hybrid/serializer-aggregate-source.hxx>
#include <cxx/hybrid/aggregate-elements.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

#include <cult/containers/map.hxx>

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      struct ParticleArg: Traversal::Element, Context
      {
        ParticleArg (Context& c,
                     TypeInstanceMap& map,
                     Boolean& first,
                     Boolean poly,
                     String const& arg)
            : Context (c),
              poly_ (poly),
              map_ (&map),
              first_ (&first),
              result_ (0),
              arg_ (arg)
        {
        }

        ParticleArg (Context& c, Boolean& result, Boolean poly)
            : Context (c),
              poly_ (poly),
              map_ (0),
              first_ (0),
              result_ (&result)
        {
        }

        virtual Void
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
        Boolean poly_;
        TypeInstanceMap* map_;
        Boolean* first_;
        Boolean* result_;
        String arg_;
      };

      struct AttributeArg: Traversal::Attribute, Context
      {
        AttributeArg (Context& c, TypeInstanceMap& map, Boolean& first)
            : Context (c), map_ (&map), first_ (&first), result_ (0)
        {
        }

        AttributeArg (Context& c, Boolean& result)
            : Context (c), map_ (0), first_ (0), result_ (&result)
        {
        }

        virtual Void
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
        Boolean* first_;
        Boolean* result_;
      };

      struct ArgList : Traversal::Complex,
                       Traversal::List,
                       Context
      {
        ArgList (Context& c,
                 TypeInstanceMap& map,
                 Boolean poly,
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

        ArgList (Context& c, Boolean& result, Boolean poly)
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
        Boolean poly_;
        TypeInstanceMap* map_;

        Traversal::Inherits inherits_;

        Traversal::Compositor compositor_;
        ParticleArg particle_;
        Traversal::ContainsCompositor contains_compositor_;
        Traversal::ContainsParticle contains_particle_;

        Traversal::Names names_;
        AttributeArg attribute_;

        Boolean first_;
        Boolean* result_;
      };

      struct SerializerConnect: Traversal::List,
                            Traversal::Complex,
                            Context
      {
        SerializerConnect (Context& c,
                           TypeInstanceMap& map,
                           Boolean poly,
                           String const& map_inst = String ())
            : Context (c), poly_ (poly), map_ (map), map_inst_ (map_inst)
        {
        }

        virtual Void
        traverse (SemanticGraph::List& l)
        {
          if (poly_)
            return;

          os << "this->" << map_[&l] << ".serializers (this->" <<
            map_[&l.argumented ().type ()] << ");"
             << endl;
        }

        virtual Void
        traverse (SemanticGraph::Complex& c)
        {
          if (has_members (c))
          {
            os << "this->" << map_[&c] << "." <<
              (poly_ ? "serializer_maps" : "serializers") << " (";

            ArgList args (*this, map_, poly_, map_inst_);
            args.dispatch (c);

            os << ");"
               << endl;
          }
        }

      private:
        Boolean
        has_members (SemanticGraph::Complex& c)
        {
          Boolean r (false);
          ArgList test (*this, r, poly_);
          test.traverse (c);
          return r;
        }

      private:
        Boolean poly_;
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

        virtual Void
        traverse (SemanticGraph::Type& t)
        {
          SemanticGraph::Context& tc (t.context ());

          if (!tc.count ("saggr"))
            return;

          String const& name (tc.get<String> ("saggr"));
          TypeInstanceMap& map (tc.get<TypeInstanceMap> ("saggr-map"));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          TypeIdInstanceMap* tid_map (0);

          if (poly_code && tc.count ("saggr-tid-map"))
            tid_map = &tc.get<TypeIdInstanceMap> ("saggr-tid-map");

          // c-tor ()
          //
          os << name << "::" << endl
             << name << " ()";

          if (tid_map)
          {
            os << endl
               << ": " << tc.get<String> ("saggr-serializer-map") << " (" <<
              tc.get<String> ("saggr-serializer-map-entries") << ", " <<
              tid_map->size () << "UL)";
          }

          os << "{";

          // Populate the polymorphic serializer map.
          //
          if (tid_map)
          {
            String const& entry (
              tc.get<String> ("saggr-serializer-map-entries"));

            Size n (0);

            for (TypeIdInstanceMap::Iterator i (tid_map->begin ());
                 i != tid_map->end ();
                 ++i, ++n)
            {
              os << entry << "[" << n << "UL].type_id = " <<
                fq_name (*i->second.type, "s:name") << "::_static_type ();"
                 << entry << "[" << n << "UL].serializer = &this->" <<
                i->second.name <<  ";"
                 << endl;
            }
          }

          // Connect parsers.
          //
          SerializerConnect connect (*this, map, false);

          for (TypeInstanceMap::Iterator i (map.begin ()), end (map.end ());
               i != end; ++i)
            connect.dispatch (*i->first);

          // Connect the serializer map.
          //
          if (tid_map)
          {
            SerializerConnect connect (
              *this, map, true, tc.get<String> ("saggr-serializer-map"));

            for (TypeInstanceMap::Iterator i (map.begin ()), end (map.end ());
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

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          SemanticGraph::Context& ec (e.context ());

          if (!ec.count ("saggr"))
            return;

          String const& name (ec.get<String> ("saggr"));
          TypeInstanceMap& map (ec.get<TypeInstanceMap> ("saggr-map"));

          os << "// " << name << endl
             << "//" << endl
             << endl;

          TypeIdInstanceMap* tid_map (0);

          if (poly_code && ec.count ("saggr-tid-map"))
            tid_map = &ec.get<TypeIdInstanceMap> ("saggr-tid-map");

          // c-tor ()
          //
          os << name << "::" << endl
             << name << " ()";

          if (tid_map)
          {
            os << endl
               << ": " << ec.get<String> ("saggr-serializer-map") << " (" <<
              ec.get<String> ("saggr-serializer-map-entries") << ", " <<
              tid_map->size () << "UL)";
          }

          os << "{";

          // Populate the polymorphic serializer map.
          //
          if (tid_map)
          {
            String const& entry (
              ec.get<String> ("saggr-serializer-map-entries"));

            Size n (0);

            for (TypeIdInstanceMap::Iterator i (tid_map->begin ());
                 i != tid_map->end ();
                 ++i, ++n)
            {
              os << entry << "[" << n << "UL].type_id = " <<
                fq_name (*i->second.type, "s:name") << "::_static_type ();"
                 << entry << "[" << n << "UL].serializer = &this->" <<
                i->second.name <<  ";"
                 << endl;
            }
          }

          // Connect parsers.
          //
          SerializerConnect connect (*this, map, false);

          for (TypeInstanceMap::Iterator i (map.begin ()), end (map.end ());
               i != end; ++i)
            connect.dispatch (*i->first);

          // Connect the serializer map.
          //
          if (tid_map)
          {
            SerializerConnect connect (
              *this, map, true, ec.get<String> ("saggr-serializer-map"));

            for (TypeInstanceMap::Iterator i (map.begin ()), end (map.end ());
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

    Void
    generate_serializer_aggregate_source (Context& ctx)
    {
      Boolean gen (false);

      {
        Traversal::Schema schema;
        Traversal::Sources sources;

        schema >> sources >> schema;

        Traversal::Names schema_names;
        Traversal::Namespace ns;
        Traversal::Names names;
        AggregateTest test (gen, "saggr");

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
