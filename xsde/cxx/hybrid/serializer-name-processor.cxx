// file      : xsde/cxx/hybrid/serializer-name-processor.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2006-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/elements.hxx>
#include <cxx/hybrid/elements.hxx>
#include <cxx/hybrid/serializer-name-processor.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

#include <cult/containers/set.hxx>

#include <sstream>
#include <iostream>

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      //
      //
      typedef Cult::Containers::Set<String> NameSet;

      class Context: public CXX::Context
      {
      public:
        Context (CLI::Options const& ops,
                 SemanticGraph::Schema& root,
                 SemanticGraph::Path const& path)
            : CXX::Context (std::wcerr,
                            root,
                            path,
                            "name",
                            "char",
                            ops.value<CLI::char_encoding> (),
                            ops.value<CLI::include_with_brackets> (),
                            ops.value<CLI::include_prefix> (),
                            "", // export symbol
                            ops.value<CLI::namespace_map> (),
                            ops.value<CLI::namespace_regex> (),
                            ops.value<CLI::namespace_regex_trace> (),
                            ops.value<CLI::include_regex> (),
                            ops.value<CLI::include_regex_trace> (),
                            ops.value<CLI::generate_inline> (),
                            ops.value<CLI::custom_allocator> (),
                            ops.value<CLI::reserved_name> ()),
              impl_suffix_ (ops.value<CLI::simpl_type_suffix> ()),
              aggr_suffix_ (ops.value<CLI::saggr_type_suffix> ()),
              options (ops),
              aggregate (ops.value<CLI::generate_aggregate> ()),
              impl_suffix (impl_suffix_),
              aggr_suffix (aggr_suffix_),
              custom_serializer_map (custom_serializer_map_),
              global_type_names (global_type_names_)
        {
          // Custom serializer mapping.
          //
          typedef Containers::Vector<NarrowString> Vector;
          Vector const& v (ops.value<CLI::custom_serializer> ());

          for (Vector::ConstIterator i (v.begin ()), e (v.end ());
               i != e; ++i)
          {
            String s (*i);

            if (s.empty ())
              continue;

            // Split the string in two parts at the last '='.
            //
            Size pos (s.rfind ('='));

            // If no delimiter found then both base and include are empty.
            //
            if (pos == String::npos)
            {
              custom_serializer_map_[s].base.clear ();
              custom_serializer_map_[s].include.clear ();
              continue;
            }

            String name (s, 0, pos);
            String rest (s, pos + 1);

            // See if we've got the include part after '/'.
            //
            pos = rest.find ('/');

            String base, include;

            if (pos != String::npos)
            {
              base.assign (rest, 0, pos);
              include.assign (rest, pos + 1, String::npos);
            }
            else
              base = rest;

            custom_serializer_map_[name].base = base;
            custom_serializer_map_[name].include = include;
          }
        }

      protected:
        Context (Context& c)
            : CXX::Context (c),
              options (c.options),
              aggregate (c.aggregate),
              impl_suffix (c.impl_suffix),
              aggr_suffix (c.aggr_suffix),
              custom_serializer_map (c.custom_serializer_map),
              global_type_names (c.global_type_names)
        {
        }

      public:
        String
        find_name (String const& n, String const& suffix, NameSet& set)
        {
          String name (escape (n + suffix));

          for (UnsignedLong i (1); set.find (name) != set.end (); ++i)
          {
            std::wostringstream os;
            os << i;
            name = escape (n + os.str () + suffix);
          }

          set.insert (name);
          return name;
        }

        String
        find_name (String const& n, NameSet& set)
        {
          return find_name (n, L"", set);
        }

      public:
        Boolean
        recursive (SemanticGraph::Type& t)
        {
          return t.context ().count ("recursive");
        }

      public:
        using CXX::Context::ename;

        static String const&
        ename (SemanticGraph::Compositor& c)
        {
          return c.context ().get<String> ("name");
        }

        String
        state_name (SemanticGraph::Compositor& c)
        {
          using namespace SemanticGraph;

          String r;

          for (Compositor* p (&c);;
               p = &p->contained_particle ().compositor ())
          {
            if (p->context ().count ("type"))
            {
              // Not a see-through compositor.
              //
              if (!r)
                r = ename (*p);
              else
              {
                String tmp;
                tmp.swap (r);
                r = ename (*p);
                r += L"_";
                r += tmp;
              }
            }

            if (p->contained_compositor_p ())
              break;
          }

          return r;
        }

        String
        state_name (SemanticGraph::Element& e)
        {
          String r (state_name (e.contained_particle ().compositor ()));

          if (!r)
            r = ename (e);
          else
          {
            r += L"_";
            r += ename (e);
          }

          return r;
        }

      public:
        struct CustomSerializer
        {
          CustomSerializer (String const& b = L"", String const& i = L"")
              : base (b), include (i)
          {
          }

          String base;
          String include;
        };

        typedef
        Cult::Containers::Map<String, CustomSerializer>
        CustomSerializerMap;

      private:
        String const impl_suffix_;
        String const aggr_suffix_;
        CustomSerializerMap custom_serializer_map_;

        Cult::Containers::Map<String, NameSet*> global_type_names_;

      public:
        CLI::Options const& options;
        Boolean aggregate;
        String const& impl_suffix;
        String const& aggr_suffix;
        CustomSerializerMap const& custom_serializer_map;

        Cult::Containers::Map<String, NameSet*>& global_type_names;
      };

      //
      //
      struct Enumeration: Traversal::Enumeration, Context
      {
        Enumeration (Context& c, Traversal::Complex& complex)
            : Context (c), complex_ (complex)
        {
        }

        virtual Void
        traverse (Type& e)
        {
          // First see if we should delegate this one to the Complex
          // generator.
          //
          Type* base_enum (0);

          if (options.value<CLI::suppress_enum> () ||
              !Hybrid::Context::enum_mapping (e, &base_enum))
          {
            complex_.traverse (e);
            return;
          }

          SemanticGraph::Context& ec (e.context ());

          // In case of customization use s:impl-base instead of s:impl.
          // If the name is empty then we are not generating anything.
          //
          String const& name (ec.count ("s:impl-base")
                              ? ec.get<String> ("s:impl-base")
                              : ec.get<String> ("s:impl"));
          if (!name)
            return;

          if (!base_enum)
          {
            NameSet set;
            set.insert (name);

            ec.set ("sstate", find_name (name + L"_state", "_",  set));
          }
        }

      private:
        Traversal::Complex& complex_;
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
          SemanticGraph::Context& lc (l.context ());

          // In case of customization use s:impl-base instead of s:impl.
          // If the name is empty then we are not generating anything.
          //
          String const& name (lc.count ("s:impl-base")
                              ? lc.get<String> ("s:impl-base")
                              : lc.get<String> ("s:impl"));
          if (!name)
            return;

          String const& skel (lc.get<String> ("s:name"));

          NameSet set;
          set.insert (name);
          set.insert (unclash (skel, "item"));
          set.insert (unclash (skel, "item_next"));

          String state_type (find_name (name + L"_state", set));
          lc.set ("sstate-type", state_type);
          lc.set ("sstate", find_name (state_type, "_",  set));
        }
      };

      //
      //
      struct Union: Traversal::Union, Context
      {
        Union (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& u)
        {
          SemanticGraph::Context& uc (u.context ());

          // In case of customization use s:impl-base instead of s:impl.
          // If the name is empty then we are not generating anything.
          //
          String const& name (uc.count ("s:impl-base")
                              ? uc.get<String> ("s:impl-base")
                              : uc.get<String> ("s:impl"));
          if (!name)
            return;

          NameSet set;
          set.insert (name);

          uc.set ("sstate", find_name (name + L"_state", "_",  set));
        }
      };

      // State names.
      //

      struct CompositorState: Traversal::Compositor, Context
      {
        CompositorState (Context& c, NameSet& set)
            : Context (c), set_ (set)
        {
        }

        virtual Void
        traverse (SemanticGraph::Compositor& c)
        {
          if (c.max () != 1)
          {
            SemanticGraph::Context& cc (c.context ());
            String b (state_name (c));
            cc.set ("sstate-member", find_name (b, L"_", set_));
            cc.set ("sstate-member-end", find_name (b + L"_end", L"_", set_));
          }

          Compositor::traverse (c);
        }

      private:
        NameSet& set_;
      };

      struct ParticleState: Traversal::Element, Context
      {
        ParticleState (Context& c, NameSet& set)
            : Context (c), set_ (set)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          if (e.max () != 1)
          {
            SemanticGraph::Context& ec (e.context ());
            String b (state_name (e));
            ec.set ("sstate-member", find_name (b, L"_", set_));
            ec.set ("sstate-member-end", find_name (b + L"_end", L"_", set_));
          }
        }

      private:
        NameSet& set_;
      };

      //
      //
      struct Complex: Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& c)
        {
          SemanticGraph::Context& cc (c.context ());

          // In case of customization use s:impl-base instead of s:impl.
          // If the name is empty then we are not generating anything.
          //
          String const& base (cc.count ("s:impl-base")
                              ? cc.get<String> ("s:impl-base")
                              : cc.get<String> ("s:impl"));
          if (!base)
            return;

          //
          //
          Boolean restriction (false);

          if (c.inherits_p ())
            restriction = c.inherits ().is_a<SemanticGraph::Restricts> () &&
              !c.inherits ().base ().is_a<SemanticGraph::AnyType> ();

          // Use skeleton's name set to make sure we don't clash
          // with callbacks which we are overriding.
          //
          NameSet& set (
            cc.get<NameSet> ("cxx-serializer-name-processor-member-set"));

          String state_type (find_name (base + L"_state", set));
          cc.set ("sstate-type", state_type);
          cc.set ("sstate", find_name (state_type, "_",  set));

          if (recursive (c))
          {
            cc.set ("sstate-first", find_name (state_type, "_first_", set));

            if (c.inherits_p () && !recursive (c.inherits ().base ()))
              cc.set ("sstate-top", find_name (state_type, "_top_", set));
          }

          // State members are in a nested struct so use a new and
          // empty name set.
          //
          NameSet state_set;

          String member (
            find_name (cc.get<String> ("name"), "_",  state_set));

          cc.set ("sstate-member", member);
          state_set.insert (member);

          // Assign state names.
          //
          if (!restriction && c.contains_compositor_p ())
          {
            ParticleState particle (*this, state_set);
            CompositorState compositor (*this, state_set);
            Traversal::ContainsCompositor contains_compositor;
            Traversal::ContainsParticle contains_particle;

            contains_compositor >> compositor >> contains_particle;

            contains_particle >> compositor;
            contains_particle >> particle;

            Complex::contains_compositor (c, contains_compositor);
          }
        }
      };

      //
      //
      struct GlobalType: Traversal::Type, Context
      {
        GlobalType (Context& c, NameSet& set)
            : Context (c), set_ (set)
        {
        }

        virtual Void
        traverse (SemanticGraph::Type& t)
        {
          SemanticGraph::Context& tc (t.context ());
          String const& name (t.name ());

          tc.set ("s:impl", find_name (name + impl_suffix, set_));

          // See if this serializer is being customized.
          //
          CustomSerializerMap::ConstIterator i (
            custom_serializer_map.find (name));

          if (i != custom_serializer_map.end ())
          {
            tc.set ("s:impl-base", i->second.base
                    ? find_name (i->second.base, set_)
                    : i->second.base);

            if (i->second.include)
              tc.set ("s:impl-include", i->second.include);
          }

          if (aggregate)
          {
            typedef Cult::Containers::Vector<NarrowString> Names;
            Names const& names (options.value<CLI::root_type> ());

            // Hopefully nobody will specify more than a handful of names.
            //
            for (Names::ConstIterator i (names.begin ());
                 i != names.end (); ++i)
            {
              if (name == String (*i))
              {
                tc.set ("saggr", find_name (name + aggr_suffix, set_));
                break;
              }
            }
          }
        }

      private:
        NameSet& set_;
      };

      //
      //
      struct GlobalElement: Traversal::Element, Context
      {
        GlobalElement (Context& c, NameSet& set)
            : Context (c), set_ (set), last_ (0)
        {
        }

        ~GlobalElement ()
        {
          if (last_ != 0 && options.value<CLI::root_element_last> ())
            process (*last_);
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          Boolean p (false);

          if (last_ == 0 && options.value<CLI::root_element_first> ())
            p = true;

          last_ = &e;

          if (!p &&
              !options.value<CLI::root_element_first> () &&
              !options.value<CLI::root_element_last> () &&
              !options.value<CLI::root_element_all> () &&
              !options.value<CLI::root_element_none> () &&
              options.value<CLI::root_element> ().empty ())
          {
            // By default process them all.
            //
            p = true;
          }

          if (!p && options.value<CLI::root_element_all> ())
            p = true;

          if (!p)
          {
            typedef Cult::Containers::Vector<NarrowString> Names;
            Names const& names (options.value<CLI::root_element> ());

            // Hopefully nobody will specify more than a handful of names.
            //
            for (Names::ConstIterator i (names.begin ());
                 !p && i != names.end ();
                 ++i)
            {
              if (e.name () == String (*i))
                p = true;
            }
          }

          if (p)
            process (e);
        }

      private:
        Void
        process (SemanticGraph::Element& e)
        {
          SemanticGraph::Context& ec (e.context ());

          if (!ec.count ("saggr"))
          {
            ec.set ("saggr", find_name (e.name () + aggr_suffix, set_));
          }
        }

      private:
        NameSet& set_;
        SemanticGraph::Element* last_;
      };

      struct Namespace: Traversal::Namespace, Context
      {
        Namespace (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (Type& ns)
        {
          SemanticGraph::Context& nsc (ns.context ());
          String const& name (ns.name ());

          // Use a name set associated with this namespace if present.
          // This will make sure that we don't get any conflicts in the
          // multi-mapping translation case. Note that here we assume
          // that all mappings traverse schemas in the same order which
          // is currently the case.
          //
          if (global_type_names.find (name) == global_type_names.end ())
          {
            if (!nsc.count ("name-set"))
              nsc.set ("name-set", NameSet ());

            NameSet& s (nsc.get<NameSet> ("name-set"));
            global_type_names[name] = &s;
          }

          NameSet& type_set (*global_type_names[name]);

          // Serializer implementations.
          //
          {
            GlobalType type (*this, type_set);
            Traversal::Names names (type);

            Traversal::Namespace::names (ns, names);
          }

          // Serializer aggregates.
          //
          if (aggregate)
          {
            GlobalElement element (*this, type_set);
            Traversal::Names names (element);

            Traversal::Namespace::names (ns, names);
          }
        }
      };

      // Go into sourced/included/imported schemas while making sure
      // we don't process the same stuff more than once.
      //
      struct Uses: Traversal::Sources,
                   Traversal::Includes,
                   Traversal::Imports
      {
        virtual Void
        traverse (SemanticGraph::Sources& sr)
        {
          SemanticGraph::Schema& s (sr.schema ());

          if (!s.context ().count (seen_key))
          {
            s.context ().set (seen_key, true);
            Traversal::Sources::traverse (sr);
          }
        }

        virtual Void
        traverse (SemanticGraph::Includes& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count (seen_key))
          {
            s.context ().set (seen_key, true);
            Traversal::Includes::traverse (i);
          }
        }

        virtual Void
        traverse (SemanticGraph::Imports& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count (seen_key))
          {
            s.context ().set (seen_key, true);
            Traversal::Imports::traverse (i);
          }
        }

        static Char const* seen_key;
      };

      Char const* Uses::seen_key = "cxx-hybrid-serializer-name-processor-seen";

      Void
      process_impl (CLI::Options const& ops,
                    SemanticGraph::Schema& tu,
                    SemanticGraph::Path const& file,
                    Boolean deep)
      {
        Context ctx (ops, tu, file);

        // Pass one - assign names to global types. This pass cannot
        // be combined with pass two because of possible recursive
        // schema inclusions. Also note that we check first if this
        // schema has already been processed which may happen in the
        // file-per-type compilation mode.
        //
        if (!tu.context ().count (Uses::seen_key))
        {
          Traversal::Schema schema;
          Uses uses;

          schema >> uses >> schema;

          Traversal::Names schema_names;
          Namespace ns (ctx);

          schema >> schema_names >> ns;

          // Some twisted schemas do recusive self-inclusion.
          //
          tu.context ().set (Uses::seen_key, true);

          schema.dispatch (tu);
        }

        if (!deep)
          return;

        // Pass two - assign names inside complex types. Here we don't
        // need to go into included/imported schemas.
        //
        {
          Traversal::Schema schema;
          Traversal::Sources sources;

          schema >> sources >> schema;

          Traversal::Names schema_names;
          Traversal::Namespace ns;
          Traversal::Names ns_names;

          schema >> schema_names >> ns >> ns_names;

          List list (ctx);
          Union union_ (ctx);
          Complex complex (ctx);
          Enumeration enumeration (ctx, complex);

          ns_names >> list;
          ns_names >> union_;
          ns_names >> complex;
          ns_names >> enumeration;

          schema.dispatch (tu);
        }
      }
    }

    Void SerializerNameProcessor::
    process (CLI::Options const& ops,
             SemanticGraph::Schema& tu,
             SemanticGraph::Path const& file,
             Boolean deep)
    {
      process_impl (ops, tu, file, deep);
    }
  }
}
