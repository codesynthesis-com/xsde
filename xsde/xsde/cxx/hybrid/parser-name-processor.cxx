// file      : xsde/cxx/hybrid/parser-name-processor.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <set>
#include <map>
#include <sstream>
#include <iostream>

#include <xsde/cxx/elements.hxx>
#include <xsde/cxx/hybrid/elements.hxx>
#include <xsde/cxx/hybrid/parser-name-processor.hxx>

#include <libxsd-frontend/semantic-graph.hxx>
#include <libxsd-frontend/traversal.hxx>

using namespace std;

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      typedef set<String> NameSet;
      struct Failed {};

      class Context: public CXX::Context
      {
      public:
        typedef Hybrid::options options_type;

      public:
        Context (options_type const& ops,
                 SemanticGraph::Schema& root,
                 SemanticGraph::Path const& path)
            : CXX::Context (std::wcerr, root, path, ops, "name", "char"),
              impl_suffix_ (ops.pimpl_type_suffix ()),
              aggr_suffix_ (ops.paggr_type_suffix ()),
              options (ops),
              aggregate (ops.generate_aggregate ()),
              impl_suffix (impl_suffix_),
              aggr_suffix (aggr_suffix_),
              custom_parser_map (custom_parser_map_),
              global_type_names (global_type_names_)
        {
          // Custom parser mapping.
          //
          NarrowStrings const& v (ops.custom_parser ());

          for (NarrowStrings::const_iterator i (v.begin ()), e (v.end ());
               i != e; ++i)
          {
            String s (*i);

            if (s.empty ())
              continue;

            // Split the string in two parts at the last '='.
            //
            size_t pos (s.rfind ('='));

            // If no delimiter found then both base and include are empty.
            //
            if (pos == String::npos)
            {
              custom_parser_map_[s].base.clear ();
              custom_parser_map_[s].include.clear ();
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

            custom_parser_map_[name].base = base;
            custom_parser_map_[name].include = include;
          }
        }

      protected:
        Context (Context& c)
            : CXX::Context (c),
              options (c.options),
              aggregate (c.aggregate),
              impl_suffix (c.impl_suffix),
              aggr_suffix (c.aggr_suffix),
              custom_parser_map (c.custom_parser_map),
              global_type_names (c.global_type_names)
        {
        }

      public:
        bool
        fixed_length (SemanticGraph::Type& t)
        {
          return t.context ().get<bool> ("fixed");
        }

        bool
        recursive (SemanticGraph::Type& t)
        {
          return t.context ().count ("recursive");
        }

      public:
        String
        find_name (String const& n, String const& suffix, NameSet& set)
        {
          String name (escape (n + suffix));

          for (size_t i (1); set.find (name) != set.end (); ++i)
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
        struct CustomParser
        {
          CustomParser (String const& b = L"", String const& i = L"")
              : base (b), include (i)
          {
          }

          String base;
          String include;
        };

        typedef map<String, CustomParser> CustomParserMap;

      private:
        String const impl_suffix_;
        String const aggr_suffix_;
        CustomParserMap custom_parser_map_;

        map<String, NameSet*> global_type_names_;

      public:
        options_type const& options;
        bool aggregate;
        String const& impl_suffix;
        String const& aggr_suffix;
        CustomParserMap const& custom_parser_map;

        map<String, NameSet*>& global_type_names;
      };

      //
      //
      struct Enumeration: Traversal::Enumeration, Context
      {
        Enumeration (Context& c, Traversal::Complex& complex)
            : Context (c), complex_ (complex)
        {
        }

        virtual void
        traverse (Type& e)
        {
          // First see if we should delegate this one to the Complex
          // generator.
          //
          Type* base_enum (0);

          if (options.suppress_enum () ||
              !Hybrid::Context::enum_mapping (e, &base_enum))
          {
            complex_.traverse (e);
            return;
          }

          SemanticGraph::Context& ec (e.context ());

          // In case of customization use p:impl-base instead of p:impl.
          // If the name is empty then we are not generating anything.
          //
          String const& name (ec.count ("p:impl-base")
                              ? ec.get<String> ("p:impl-base")
                              : ec.get<String> ("p:impl"));
          if (!name)
            return;

          bool fl (fixed_length (e));

          NameSet set;
          set.insert (name);

          if (!fl || !base_enum)
          {
            String state_type (find_name (name + L"_state", set));
            ec.set ("pstate-type", state_type);
            ec.set ("pstate", find_name (state_type, "_", set));
          }

          if (!fl)
            ec.set ("pstate-base", find_name (name + L"_base", "_",  set));
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

        virtual void
        traverse (Type& l)
        {
          SemanticGraph::Context& lc (l.context ());

          // In case of customization use p:impl-base instead of p:impl.
          // If the name is empty then we are not generating anything.
          //
          String const& name (lc.count ("p:impl-base")
                              ? lc.get<String> ("p:impl-base")
                              : lc.get<String> ("p:impl"));
          if (!name)
            return;

          NameSet set;
          set.insert (name);
          set.insert (unclash (lc.get<String> ("p:name"), "item"));

          lc.set ("pstate-member",
                  find_name (lc.get<String> ("name"), "_",  set));

          lc.set ("pstate-base", find_name (name + L"_base", "_",  set));
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

        virtual void
        traverse (Type& u)
        {
          SemanticGraph::Context& uc (u.context ());

          // In case of customization use p:impl-base instead of p:impl.
          // If the name is empty then we are not generating anything.
          //
          String const& name (uc.count ("p:impl-base")
                              ? uc.get<String> ("p:impl-base")
                              : uc.get<String> ("p:impl"));
          if (!name)
            return;

          NameSet set;
          set.insert (name);

          String state_type (find_name (name + L"_state", set));

          uc.set ("pstate-type", state_type);
          uc.set ("pstate", find_name (state_type, "_", set));

          if (!fixed_length (u))
            uc.set ("pstate-base", find_name (name + L"_base", "_", set));
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

        virtual void
        traverse (SemanticGraph::Compositor& c)
        {
          if (c.max () != 1)
          {
            SemanticGraph::Context& cc (c.context ());
            cc.set ("pstate-member", find_name (state_name (c), L"_", set_));
          }

          Compositor::traverse (c);
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

        virtual void
        traverse (Type& c)
        {
          SemanticGraph::Context& cc (c.context ());

          // In case of customization use p:impl-base instead of p:impl.
          // If the name is empty then we are not generating anything.
          //
          String const& base (cc.count ("p:impl-base")
                              ? cc.get<String> ("p:impl-base")
                              : cc.get<String> ("p:impl"));
          if (!base)
            return;

          //
          //
          bool restriction (false);

          if (c.inherits_p ())
            restriction = c.inherits ().is_a<SemanticGraph::Restricts> () &&
              !c.inherits ().base ().is_a<SemanticGraph::AnyType> ();

          // Use skeleton's name set to make sure we don't clash
          // with callbacks which we are overriding.
          //
          NameSet& set (
            cc.get<NameSet> ("cxx-parser-name-processor-member-set"));

          String state_type (find_name (base + L"_state", set));

          cc.set ("pstate-type", state_type);
          cc.set ("pstate", find_name (state_type, "_", set));

          if (recursive (c))
          {
            cc.set ("pstate-first", find_name (state_type, "_first_", set));

            if (c.inherits_p () && !recursive (c.inherits ().base ()))
              cc.set ("pstate-top", find_name (state_type, "_top_", set));
          }

          cc.set ("pstate-base", find_name (base + L"_base", "_",  set));

          // State members are in a nested struct so use a new and
          // empty name set.
          //
          NameSet state_set;

          String member (
            find_name (cc.get<String> ("name"), "_",  state_set));

          cc.set ("pstate-member", member);
          state_set.insert (member);

          // Assign state names.
          //
          if (!restriction && c.contains_compositor_p ())
          {
            CompositorState compositor (*this, state_set);
            Traversal::ContainsCompositor contains_compositor;
            Traversal::ContainsParticle contains_particle;

            contains_compositor >> compositor;
            contains_particle >> compositor >> contains_particle;

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

        virtual void
        traverse (SemanticGraph::Type& t)
        {
          SemanticGraph::Context& tc (t.context ());
          String const& name (t.name ());

          tc.set ("p:impl", find_name (name + impl_suffix, set_));

          // See if this parser is being customized.
          //
          bool custom (false);
          CustomParserMap::const_iterator i (custom_parser_map.find (name));

          if (i != custom_parser_map.end ())
          {
            tc.set ("p:impl-base", i->second.base
                    ? find_name (i->second.base, set_)
                    : i->second.base);

            custom = i->second.base.empty ();

            if (i->second.include)
              tc.set ("p:impl-include", i->second.include);
          }

          // If this type is completely customized then we cannot generate
          // a parser implementation for it.
          //
          if (!custom && tc.count ("name-base") && !tc.get<String> ("name-base"))
          {
            os << t.file () << ":" << t.line () << ":" << t.column ()
               << ": error: unable to generate parser implementation for "
               << "customized object model type '" << t.name () << "'" << endl;

            os << t.file () << ":" << t.line () << ":" << t.column ()
               << ": info: provide custom parser implementation for this "
               << "type with the --custom-parser option" << endl;

            throw Failed ();
          }

          if (aggregate)
          {
            NarrowStrings const& names (options.root_type ());

            // Hopefully nobody will specify more than a handful of names.
            //
            for (NarrowStrings::const_iterator i (names.begin ());
                 i != names.end (); ++i)
            {
              if (name == String (*i))
              {
                tc.set ("paggr", find_name (name + aggr_suffix, set_));
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
          if (last_ != 0 && options.root_element_last ())
            process (*last_);
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          bool p (false);

          if (last_ == 0 && options.root_element_first ())
            p = true;

          last_ = &e;

          if (!p &&
              !options.root_element_first () &&
              !options.root_element_last () &&
              !options.root_element_all () &&
              !options.root_element_none () &&
              options.root_element ().empty ())
          {
            // By default process them all.
            //
            p = true;
          }

          if (!p && options.root_element_all ())
            p = true;

          if (!p)
          {
            NarrowStrings const& names (options.root_element ());

            // Hopefully nobody will specify more than a handful of names.
            //
            for (NarrowStrings::const_iterator i (names.begin ());
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
        void
        process (SemanticGraph::Element& e)
        {
          SemanticGraph::Context& ec (e.context ());

          if (!ec.count ("paggr"))
          {
            ec.set ("paggr", find_name (e.name () + aggr_suffix, set_));
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

        virtual void
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

          // Parser implementations.
          //
          {
            GlobalType type (*this, type_set);
            Traversal::Names names (type);

            Traversal::Namespace::names (ns, names);
          }

          // Parser aggregates.
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
        virtual void
        traverse (SemanticGraph::Sources& sr)
        {
          SemanticGraph::Schema& s (sr.schema ());

          if (!s.context ().count (seen_key))
          {
            s.context ().set (seen_key, true);
            Traversal::Sources::traverse (sr);
          }
        }

        virtual void
        traverse (SemanticGraph::Includes& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count (seen_key))
          {
            s.context ().set (seen_key, true);
            Traversal::Includes::traverse (i);
          }
        }

        virtual void
        traverse (SemanticGraph::Imports& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count (seen_key))
          {
            s.context ().set (seen_key, true);
            Traversal::Imports::traverse (i);
          }
        }

        static char const* seen_key;
      };

      char const* Uses::seen_key = "cxx-hybrid-parser-name-processor-seen";

      void
      process_impl (options const& ops,
                    SemanticGraph::Schema& tu,
                    SemanticGraph::Path const& file,
                    bool deep)
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
          Sources sources;

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

    bool ParserNameProcessor::
    process (options const& ops,
             SemanticGraph::Schema& tu,
             SemanticGraph::Path const& file,
             bool deep)
    {
      try
      {
        process_impl (ops, tu, file, deep);
        return true;
      }
      catch (Failed const&)
      {
        return false;
      }
    }
  }
}
