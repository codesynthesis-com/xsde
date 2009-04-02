// file      : xsde/cxx/hybrid/tree-name-processor.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/elements.hxx>
#include <cxx/hybrid/tree-name-processor.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

#include <cult/containers/set.hxx>
#include <cult/containers/map.hxx>

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
      Char const* member_set_key = "cxx-hybrid-name-processor-member-set";

      class Context: public CXX::Context
      {
      public:
        Context (CLI::Options const& ops,
                 SemanticGraph::Schema& root,
                 SemanticGraph::Path const& file)
            : CXX::Context (std::wcerr,
                            root,
                            "name",
                            "char",
                            ops.value<CLI::include_with_brackets> (),
                            ops.value<CLI::include_prefix> (),
                            "", // export symbol
                            ops.value<CLI::namespace_map> (),
                            ops.value<CLI::namespace_regex> (),
                            ops.value<CLI::namespace_regex_trace> (),
                            ops.value<CLI::include_regex> (),
                            ops.value<CLI::include_regex_trace> (),
                            ops.value<CLI::generate_inline> (),
                            ops.value<CLI::reserved_name> ()),
              schema_path_ (file),
              schema (root),
              schema_path (schema_path_),
              stl (!ops.value<CLI::no_stl> ()),
              detach (ops.value<CLI::generate_detach> ()),
              custom_data_map (custom_data_map_),
              custom_type_map (custom_type_map_),
              global_type_names (global_type_names_)
        {
          // Translate the type names with custom data.
          //
          {
            typedef Cult::Containers::Vector<NarrowString> CustomData;
            CustomData const& cd (ops.value<CLI::custom_data> ());

            for (CustomData::ConstIterator i (cd.begin ());
                 i != cd.end (); ++i)
            {
              String name (*i);
              CustomDataMap* map (&custom_data_map);
              String::size_type b (0), e;

              do
              {
                e = name.find (L"::", b);
                String entry (name, b, e == String::npos ? e : e - b);

                Shptr<CustomDataMap>& p ((*map)[entry]);

                if (p == 0)
                  p = Shptr<CustomDataMap> (new CustomDataMap);

                b = e;

                if (b == String::npos)
                {
                  // Last name. Add an empty string to indicate this.
                  //
                  (*p)[L""] = Shptr<CustomDataMap> (0);
                  break;
                }

                map = p.get ();
                b += 2;

              } while (true);
            }
          }

          // Custom type mapping.
          //
          {
            typedef Containers::Vector<NarrowString> Vector;
            Vector const& v (ops.value<CLI::custom_type> ());

            for (Vector::ConstIterator i (v.begin ()), e (v.end ());
                 i != e; ++i)
            {
              String s (*i);

              if (s.empty ())
                continue;

              // Split the string in two parts at the last '='.
              //
              Size pos (s.rfind ('='));

              // If no delimiter found type, base, and include are empty.
              //
              if (pos == String::npos)
              {
                custom_type_map_[s].type.clear ();
                custom_type_map_[s].base.clear ();
                custom_type_map_[s].include.clear ();
                continue;
              }

              String name (s, 0, pos);

              // Skip the flags component.
              //
              pos = s.find ('/', pos + 1);

              if (pos == String::npos)
              {
                custom_type_map_[name].type.clear ();
                custom_type_map_[name].base.clear ();
                custom_type_map_[name].include.clear ();
                continue;
              }

              String bi (s, pos + 1);

              // See if we've got the base/include part after '/'.
              //
              pos = bi.find ('/');

              String type, base, include;

              if (pos != String::npos)
              {
                type.assign (bi, 0, pos);
                String i (bi, pos + 1);

                // See if we've got the include part after '/'.
                //
                pos = i.find ('/');

                if (pos != String::npos)
                {
                  base.assign (i, 0, pos);
                  include.assign (i, pos + 1, String::npos);
                }
                else
                  base = i;
              }
              else
                type = bi;

              custom_type_map_[name].type = type;
              custom_type_map_[name].base = base;
              custom_type_map_[name].include = include;
            }
          }
        }

      protected:
        Context (Context& c)
            : CXX::Context (c),
              schema (c.schema),
              schema_path (c.schema_path),
              stl (c.stl),
              detach (c.detach),
              custom_data_map (c.custom_data_map),
              custom_type_map (c.custom_type_map),
              global_type_names (c.global_type_names)
        {
        }

      public:
        Boolean
        fixed_length (SemanticGraph::Type& t)
        {
          return t.context ().get<Boolean> ("fixed");
        }

        Boolean
        fixed_length (SemanticGraph::Compositor& c)
        {
          return c.context ().get<Boolean> ("fixed");
        }

        Void
        mark_variable (SemanticGraph::Compositor& c)
        {
          SemanticGraph::Compositor* p (&c);

          while (true)
          {
            p->context ().set ("fixed", false);

            if (p->contained_compositor_p ())
              break;

            p = &p->contained_particle ().compositor ();

            if (!p->context ().get<Boolean> ("fixed"))
              break;
          }
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
            name = Hybrid::Context::escape (n + os.str () + suffix);
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
        struct CustomDataMap:
          Cult::Containers::Map<String, Shptr<CustomDataMap> >
        {
        };

      public:
        struct CustomType
        {
          CustomType (String const& t = L"",
                      String const& b = L"",
                      String const& i = L"")
              : type (t), base (b), include (i)
          {
          }

          String type;
          String base;
          String include;
        };

        typedef
        Cult::Containers::Map<String, CustomType>
        CustomTypeMap;

      private:
        SemanticGraph::Path const schema_path_;

        CustomDataMap custom_data_map_;
        CustomTypeMap custom_type_map_;

        Cult::Containers::Map<String, NameSet*> global_type_names_;

      public:
        SemanticGraph::Schema& schema;
        SemanticGraph::Path const& schema_path;

        Boolean stl;
        Boolean detach;

        CustomDataMap& custom_data_map;
        CustomTypeMap& custom_type_map;

        Cult::Containers::Map<String, NameSet*>& global_type_names;
      };

      //
      //
      struct List: Traversal::List, Context
      {
        List (Context& c, Boolean data_members)
            : Context (c), data_members_ (data_members)
        {
        }

        virtual Void
        traverse (Type& l)
        {
          SemanticGraph::Context& lc (l.context ());

          // In case of customization use name-base instead of name.
          // If name is empty then we are not generating anything.
          //
          String const& name (lc.count ("name-base")
                              ? lc.get<String> ("name-base")
                              : lc.get<String> ("name"));
          if (!name)
            return;

          if (!data_members_)
          {
            // Check if this type has custom data.
            //
            CustomDataMap::Iterator i (custom_data_map.find (l.name ()));

            if (i != custom_data_map.end () &&
                i->second->find (L"") != i->second->end ())
            {
              lc.set (member_set_key, NameSet ());
              NameSet& set (lc.get<NameSet> (member_set_key));
              set.insert (name);

              {
                String name (find_name ("custom_data", set));

                lc.set ("cd-name", name);
                lc.set ("cd-sequence", find_name (name + L"_sequence", set));
                lc.set ("cd-iterator", find_name (name + L"_iterator", set));
                lc.set ("cd-const-iterator",
                        find_name (name + L"_const_iterator", set));
              }
            }
          }
          else
          {
            // Custom data.
            //
            if (lc.count ("cd-name"))
            {
              NameSet& set (lc.get<NameSet> (member_set_key));
              String const& base (lc.get<String> ("cd-name"));
              lc.set ("cd-member", find_name (base + L"_", set));
            }
          }
        }

      private:
        Boolean data_members_;
      };

      //
      //
      struct Union: Traversal::Union, Context
      {
        Union (Context& c, Boolean data_members)
            : Context (c), data_members_ (data_members)
        {
        }

        virtual Void
        traverse (Type& u)
        {
          SemanticGraph::Context& uc (u.context ());

          // In case of customization use name-base instead of name.
          // If name is empty then we are not generating anything.
          //
          String const& name (uc.count ("name-base")
                              ? uc.get<String> ("name-base")
                              : uc.get<String> ("name"));
          if (!name)
            return;

          if (!data_members_)
          {
            uc.set (member_set_key, NameSet ());
            NameSet& set (uc.get<NameSet> (member_set_key));
            set.insert (name);

            String v (find_name ("value", set));
            uc.set ("value", v);

            if (detach && !stl)
              uc.set ("value-detach", find_name (v + L"_detach", set));

            // Check if this type has custom data.
            //
            CustomDataMap::Iterator i (custom_data_map.find (u.name ()));

            if (i != custom_data_map.end () &&
                i->second->find (L"") != i->second->end ())
            {
              String name (find_name ("custom_data", set));

              uc.set ("cd-name", name);
              uc.set ("cd-sequence", find_name (name + L"_sequence", set));
              uc.set ("cd-iterator", find_name (name + L"_iterator", set));
              uc.set ("cd-const-iterator",
                      find_name (name + L"_const_iterator", set));
            }
          }
          else
          {
            NameSet& set (uc.get<NameSet> (member_set_key));
            uc.set ("value-member", find_name ("value_", set));

            // Custom data.
            //
            if (uc.count ("cd-name"))
            {
              String const& base (uc.get<String> ("cd-name"));
              uc.set ("cd-member", find_name (base + L"_", set));
            }
          }
        }

      private:
        Boolean data_members_;
      };

      //
      // Primary names.
      //

      struct PrimaryAttribute: Traversal::Attribute, Context
      {
        PrimaryAttribute (Context& c, NameSet& set)
            : Context (c), set_ (set)
        {
        }

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          a.context ().set ("name", find_name (a.name (), set_));
        }

      private:
        NameSet& set_;
      };

      struct PrimaryElement: Traversal::Element, Context
      {
        PrimaryElement (Context& c, NameSet& set)
            : Context (c), set_ (set)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          e.context ().set ("name", find_name (e.name (), set_));
        }

      private:
        NameSet& set_;
      };

      struct PrimaryAll: Traversal::All, Context
      {
        PrimaryAll (Context& c, NameSet& set)
            : Context (c), set_ (set)
        {
        }

        virtual Void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}
          // and it can only contain particles.
          //
          if (a.min () == 0)
            a.context ().set ("name", find_name ("all", set_));
          else
            All::contains (a);
        }

      private:
        NameSet& set_;
      };

      struct PrimaryChoice: Traversal::Choice, Context
      {
        PrimaryChoice (Context& c, NameSet& set, Boolean in_choice)
            : Context (c), set_ (set), in_choice_ (in_choice)
        {
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          // In case of choice assign primary name even if there is
          // no nested class. It is used to derive names for arm
          // functions.
          //
          c.context ().set ("name", find_name ("choice", set_));

          if (!in_choice_ && c.max () == 1 && c.min () == 1)
            Choice::contains (c);
        }

      private:
        NameSet& set_;
        Boolean in_choice_;
      };

      struct PrimarySequence: Traversal::Sequence, Context
      {
        PrimarySequence (Context& c, NameSet& set, Boolean in_choice)
            : Context (c), set_ (set), in_choice_ (in_choice)
        {
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          // When sequence is in choice we have nested class even
          // for min == max == 1.
          //
          if (in_choice_ || s.max () != 1 || s.min () == 0)
            s.context ().set ("name", find_name ("sequence", set_));
          else
            Sequence::contains (s);
        }

      private:
        NameSet& set_;
        Boolean in_choice_;
      };

      //
      // Secondary names.
      //

      struct SecondaryAttribute: Traversal::Attribute, Context
      {
        SecondaryAttribute (Context& c, NameSet& set, Boolean data_members)
            : Context (c), set_ (set), data_members_ (data_members)
        {
        }

        virtual Void
        traverse (SemanticGraph::Attribute& a)
        {
          SemanticGraph::Context& ac (a.context ());

          Boolean def (a.default_ ());
          Boolean fix (a.fixed ());

          String const& base (ac.get<String> ("name"));

          if (!data_members_)
          {
            if (a.optional () && !fix)
            {
              String n;
              if (def)
                n = find_name (base + L"_default", set_);
              else
                n = find_name (base + L"_present", set_);

              ac.set (def ? "default" : "present", n);
            }

            if (detach && !fix && !fixed_length (a.type ()))
              ac.set ("detach", find_name (base + L"_detach", set_));

            if (def)
            {
              ac.set (
                "default-value",
                find_name (base + (fix ? L"_fixed_value" : L"_default_value"),
                           set_));
            }
          }
          else if (!fix)
          {
            if (a.optional () && !a.default_ () && fixed_length (a.type ()))
            {
              ac.set ("present-member",
                      find_name (ac.get<String> ("present") + L"_", set_));
            }

            ac.set ("member", find_name (base + L"_", set_));
          }
        }

      private:
        NameSet& set_;
        Boolean data_members_;
      };

      struct SecondaryElement: Traversal::Element, Context
      {
        SecondaryElement (Context& c, NameSet& set, Boolean data_members)
            : Context (c), set_ (set), data_members_ (data_members)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          SemanticGraph::Context& ec (e.context ());

          if (!data_members_)
          {
            if (e.max () != 1)
            {
              String const& base (ec.get<String> ("name"));

              ec.set ("sequence", find_name (base + L"_sequence", set_));
              ec.set ("iterator", find_name (base + L"_iterator", set_));
              ec.set ("const-iterator",
                      find_name (base + L"_const_iterator", set_));
            }
            else
            {
              if (e.min () == 0)
                ec.set (
                  "present",
                  find_name (ec.get<String> ("name") + L"_present", set_));

              if (detach && !fixed_length (e.type ()))
                ec.set (
                  "detach",
                  find_name (ec.get<String> ("name") + L"_detach", set_));
            }
          }
          else
          {
            String const& base (ec.get<String> ("name"));

            if (e.max () == 1 && e.min () == 0)
            {
              if (fixed_length (e.type ()))
                ec.set ("present-member",
                        find_name (ec.get<String> ("present") + L"_", set_));
            }

            ec.set ("member", find_name (base + L"_", set_));
          }
        }


      private:
        NameSet& set_;
        Boolean data_members_;
      };

      struct SecondaryAll: Traversal::All, Context
      {
        SecondaryAll (Context& c,
                      NameSet& set,
                      CustomDataMap* map,
                      Boolean data_members)
            : Context (c),
              set_ (set),
              map_ (map),
              data_members_ (data_members)
        {
        }

        virtual Void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}
          // and it can only contain particles.
          //
          if (a.min () == 0)
          {
            SemanticGraph::Context& ac (a.context ());
            String const& base (ac.get<String> ("name"));

            if (!data_members_)
            {
              // Check if this type has custom data.
              //
              CustomDataMap* map (0);

              if (map_)
              {
                CustomDataMap::Iterator i (map_->find (base));
                if (i != map_->end ())
                  map = i->second.get ();
              }

              String type (find_name (base + L"_type", set_));
              ac.set ("type", type);

              // Handle the nested class.
              //
              {
                ac.set (member_set_key, NameSet ());
                NameSet& name_set (ac.get<NameSet> (member_set_key));

                name_set.insert (type);

                {
                  PrimaryElement element (*this, name_set);
                  Traversal::ContainsParticle contains_particle (element);
                  All::contains (a, contains_particle);
                }

                {
                  SecondaryElement element (*this, name_set, false);
                  Traversal::ContainsParticle contains_particle (element);
                  All::contains (a, contains_particle);
                }

                // Custom data.
                //
                if (map && map->find (L"") != map->end ())
                {
                  // Make the type var-length if we have custom data.
                  //
                  if (fixed_length (a))
                    mark_variable (a);

                  String name (find_name ("custom_data", name_set));

                  ac.set ("cd-name", name);

                  ac.set ("cd-sequence",
                          find_name (name + L"_sequence", name_set));

                  ac.set ("cd-iterator",
                          find_name (name + L"_iterator", name_set));

                  ac.set ("cd-const-iterator",
                          find_name (name + L"_const_iterator", name_set));
                }
              }

              ac.set ("present", find_name (base + L"_present", set_));

              if (detach && !fixed_length (a))
                ac.set ("detach", find_name (base + L"_detach", set_));
            }
            else
            {
              // Handle the nested class.
              //
              {
                NameSet& name_set (ac.get<NameSet> (member_set_key));

                SecondaryElement element (*this, name_set, true);
                Traversal::ContainsParticle contains_particle (element);
                All::contains (a, contains_particle);

                // Custom data.
                //
                if (ac.count ("cd-name"))
                {
                  String const& base (ac.get<String> ("cd-name"));
                  ac.set ("cd-member", find_name (base + L"_", name_set));
                }
              }

              if (fixed_length (a))
                ac.set ("present-member",
                        find_name (ac.get<String> ("present") + L"_", set_));

              ac.set ("member", find_name (base + L"_", set_));
            }
          }
          else
            All::contains (a);
        }

      private:
        NameSet& set_;
        CustomDataMap* map_;
        Boolean data_members_;
      };

      struct ParticleTag: Traversal::Element,
                          Traversal::Any,
                          Traversal::Choice,
                          Traversal::Sequence,
                          Context
      {
        ParticleTag (Context& c, NameSet& set)
            : Context (c), set_ (set)
        {
        }

        virtual Void
        traverse (SemanticGraph::Element& e)
        {
          String const& base (e.context ().get<String> ("name"));
          e.context ().set ("tag", find_name (base, L"_tag", set_));
        }

        virtual Void
        traverse (SemanticGraph::Any& a)
        {
          a.context ().set ("tag", find_name (L"any", L"_tag", set_));
        }

        virtual Void
        traverse (SemanticGraph::Choice& c)
        {
          String const& base (c.context ().get<String> ("name"));
          c.context ().set ("tag", find_name (base, L"_tag", set_));
        }

        virtual Void
        traverse (SemanticGraph::Sequence& s)
        {
          String const& base (s.context ().get<String> ("name"));
          s.context ().set ("tag", find_name (base, L"_tag", set_));
        }

      private:
        NameSet& set_;
      };

      struct SecondaryChoice: Traversal::Choice, Context
      {
        SecondaryChoice (Context& c,
                         NameSet& set,
                         CustomDataMap* map,
                         Boolean in_choice,
                         Boolean data_members)
            : Context (c),
              set_ (set),
              map_ (map),
              in_choice_ (in_choice),
              data_members_ (data_members)
        {
        }

        virtual Void
        traverse (SemanticGraph::Choice&);

        Void
        traverse_nested (SemanticGraph::Choice&, NameSet&, CustomDataMap*);

      private:
        NameSet& set_;
        CustomDataMap* map_;
        Boolean in_choice_;
        Boolean data_members_;
      };

      struct SecondarySequence: Traversal::Sequence, Context
      {
        SecondarySequence (Context& c,
                           NameSet& set,
                           CustomDataMap* map,
                           Boolean in_choice,
                           Boolean data_members)
            : Context (c),
              set_ (set),
              map_ (map),
              in_choice_ (in_choice),
              data_members_ (data_members)
        {
        }

        virtual Void
        traverse (SemanticGraph::Sequence&);

        Void
        traverse_nested (SemanticGraph::Sequence&, NameSet&, CustomDataMap*);

      private:
        NameSet& set_;
        CustomDataMap* map_;
        Boolean in_choice_;
        Boolean data_members_;
      };


      Void SecondaryChoice::
      traverse (SemanticGraph::Choice& c)
      {
        SemanticGraph::Context& cc (c.context ());
        String const& base (cc.get<String> ("name"));

        // When choice is in choice we have nested class even
        // for min == max == 1.
        //
        if (in_choice_ || c.max () != 1 || c.min () == 0)
        {
          if (!data_members_)
          {
            // Check if this type or any of its nested types have
            // custom data.
            //
            CustomDataMap* map (0);

            if (map_)
            {
              CustomDataMap::Iterator i (map_->find (base));
              if (i != map_->end ())
                map = i->second.get ();
            }

            //
            //
            String type (find_name (base + L"_type", set_));
            cc.set ("type", type);

            // Handle the nested class.
            //
            {
              cc.set (member_set_key, NameSet ());
              NameSet& name_set (cc.get<NameSet> (member_set_key));

              // Add both base and type names so that we get consistent
              // naming for nested choices if any.
              //
              name_set.insert (base);
              name_set.insert (type);

              traverse_nested (c, name_set, map);

              // Tags.
              //
              String arm (find_name (base + L"_arm", name_set));
              cc.set ("arm", arm);
              cc.set ("arm-tag", find_name (arm + L"_tag", name_set));

              {
                UnsignedLong count (name_set.size ());

                ParticleTag particle (*this, name_set);
                Traversal::ContainsParticle contains (particle);

                Choice::contains (c, contains);

                count = name_set.size () - count;
                cc.set ("arm-tag-count", count);
              }

              // Custom data.
              //
              if (map && map->find (L"") != map->end ())
              {
                // Make the type var-length if we have custom data.
                //
                if (fixed_length (c))
                  mark_variable (c);

                String name (find_name ("custom_data", name_set));

                cc.set ("cd-name", name);

                cc.set ("cd-sequence",
                        find_name (name + L"_sequence", name_set));

                cc.set ("cd-iterator",
                        find_name (name + L"_iterator", name_set));

                cc.set ("cd-const-iterator",
                        find_name (name + L"_const_iterator", name_set));
              }
            }

            if (c.max () != 1)
            {
              cc.set ("sequence", find_name (base + L"_sequence", set_));
              cc.set ("iterator", find_name (base + L"_iterator", set_));
              cc.set ("const-iterator",
                      find_name (base + L"_const_iterator", set_));
            }
            else
            {
              if (c.min () == 0)
                cc.set ("present", find_name (base + L"_present", set_));

              if (detach && !fixed_length (c))
                cc.set ("detach", find_name (base + L"_detach", set_));
            }
          }
          else
          {
            // Handle the nested class.
            //
            {
              NameSet& name_set (cc.get<NameSet> (member_set_key));
              traverse_nested (c, name_set, 0);

              // Tags.
              //
              cc.set ("arm-member",
                      find_name (cc.get<String> ("arm") + L"_", name_set));
              cc.set ("member", find_name (base + L"_", name_set));

              // Custom data.
              //
              if (cc.count ("cd-name"))
              {
                String const& base (cc.get<String> ("cd-name"));
                cc.set ("cd-member", find_name (base + L"_", name_set));
              }
            }

            if (c.max () == 1 && c.min () == 0)
            {
              if (fixed_length (c))
                cc.set ("present-member",
                        find_name (cc.get<String> ("present") + L"_", set_));
            }

            cc.set ("member", find_name (base + L"_", set_));
          }
        }
        else
        {
          Choice::contains (c);

          if (!data_members_)
          {
            String arm (find_name (base + L"_arm", set_));
            cc.set ("arm", arm);
            cc.set ("arm-tag", find_name (arm + L"_tag", set_));

            {
              UnsignedLong count (set_.size ());

              ParticleTag particle (*this, set_);
              Traversal::ContainsParticle contains (particle);

              Choice::contains (c, contains);

              count = set_.size () - count;
              cc.set ("arm-tag-count", count);
            }
          }
          else
          {
            cc.set ("arm-member",
                    find_name (cc.get<String> ("arm") + L"_", set_));
            cc.set ("member", find_name (base + L"_", set_));
          }
        }
      }

      Void SecondaryChoice::
      traverse_nested (SemanticGraph::Choice& c,
                       NameSet& name_set,
                       CustomDataMap* map)
      {
        if (!data_members_)
        {
          PrimaryElement element (*this, name_set);
          PrimaryChoice choice_in_sequence (*this, name_set, false);
          PrimarySequence sequence_in_sequence (*this, name_set, false);
          Traversal::ContainsParticle sequence_contains_particle;

          sequence_contains_particle >> element;
          sequence_contains_particle >> choice_in_sequence;
          sequence_contains_particle >> sequence_in_sequence;
          sequence_in_sequence >> sequence_contains_particle;

          PrimaryChoice choice_in_choice (*this, name_set, true);
          PrimarySequence sequence_in_choice (*this, name_set, true);
          Traversal::ContainsParticle choice_contains_particle;

          sequence_in_choice >> sequence_contains_particle;
          choice_contains_particle >> element;
          choice_contains_particle >> choice_in_choice;
          choice_contains_particle >> sequence_in_choice;
          choice_in_choice >> choice_contains_particle;
          choice_in_sequence >> choice_contains_particle;

          Choice::contains (c, choice_contains_particle);
        }

        SecondaryElement element (*this, name_set, data_members_);
        SecondaryChoice choice_in_sequence (
          *this, name_set, map, false, data_members_);
        SecondarySequence sequence_in_sequence (
          *this, name_set, map, false, data_members_);
        Traversal::ContainsParticle sequence_contains_particle;

        sequence_contains_particle >> element;
        sequence_contains_particle >> choice_in_sequence;
        sequence_contains_particle >> sequence_in_sequence;
        sequence_in_sequence >> sequence_contains_particle;

        SecondaryChoice choice_in_choice (
          *this, name_set, map, true, data_members_);
        SecondarySequence sequence_in_choice (
          *this, name_set, map, true, data_members_);
        Traversal::ContainsParticle choice_contains_particle;

        sequence_in_choice >> sequence_contains_particle;
        choice_contains_particle >> element;
        choice_contains_particle >> choice_in_choice;
        choice_contains_particle >> sequence_in_choice;
        choice_in_choice >> choice_contains_particle;
        choice_in_sequence >> choice_contains_particle;

        Choice::contains (c, choice_contains_particle);
      }

      Void SecondarySequence::
      traverse (SemanticGraph::Sequence& s)
      {
        // When sequence is in choice we have nested class even
        // for min == max == 1.
        //
        if (in_choice_ || s.max () != 1 || s.min () == 0)
        {
          SemanticGraph::Context& sc (s.context ());
          String const& base (sc.get<String> ("name"));

          if (!data_members_)
          {
            // Check if this type or any of its nested types have
            // custom data.
            //
            CustomDataMap* map (0);

            if (map_)
            {
              CustomDataMap::Iterator i (map_->find (base));
              if (i != map_->end ())
                map = i->second.get ();
            }

            String type (find_name (base + L"_type", set_));
            sc.set ("type", type);

            // Handle the nested class.
            //
            {
              sc.set (member_set_key, NameSet ());
              NameSet& name_set (sc.get<NameSet> (member_set_key));

              // Add both base and type names so that we get consistent
              // naming for nested sequences if any.
              //
              name_set.insert (base);
              name_set.insert (type);

              traverse_nested (s, name_set, map);

              // Custom data.
              //
              if (map && map->find (L"") != map->end ())
              {
                // Make the type var-length if we have custom data.
                //
                if (fixed_length (s))
                  mark_variable (s);

                String name (find_name ("custom_data", name_set));

                sc.set ("cd-name", name);

                sc.set ("cd-sequence",
                        find_name (name + L"_sequence", name_set));

                sc.set ("cd-iterator",
                        find_name (name + L"_iterator", name_set));

                sc.set ("cd-const-iterator",
                        find_name (name + L"_const_iterator", name_set));
              }
            }

            if (s.max () != 1)
            {
              sc.set ("sequence", find_name (base + L"_sequence", set_));
              sc.set ("iterator", find_name (base + L"_iterator", set_));
              sc.set ("const-iterator",
                      find_name (base + L"_const_iterator", set_));
            }
            else
            {
              if (s.min () == 0)
                sc.set ("present", find_name (base + L"_present", set_));

              if (detach && !fixed_length (s))
                sc.set ("detach", find_name (base + L"_detach", set_));
            }

          }
          else
          {
            // Handle the nested class.
            //
            {
              NameSet& name_set (sc.get<NameSet> (member_set_key));
              traverse_nested (s, name_set, 0);

              // Custom data.
              //
              if (sc.count ("cd-name"))
              {
                String const& base (sc.get<String> ("cd-name"));
                sc.set ("cd-member", find_name (base + L"_", name_set));
              }
            }

            if (s.max () == 1 && s.min () == 0)
            {
              if (fixed_length (s))
                sc.set ("present-member",
                        find_name (sc.get<String> ("present") + L"_", set_));
            }

            sc.set ("member", find_name (base + L"_", set_));
          }
        }
        else
          Sequence::contains (s);
      }

      Void SecondarySequence::
      traverse_nested (SemanticGraph::Sequence& s,
                       NameSet& name_set,
                       CustomDataMap* map)
      {
        if (!data_members_)
        {
          PrimaryElement element (*this, name_set);
          PrimaryChoice choice_in_sequence (*this, name_set, false);
          PrimarySequence sequence_in_sequence (*this, name_set, false);
          Traversal::ContainsParticle sequence_contains_particle;

          sequence_contains_particle >> element;
          sequence_contains_particle >> choice_in_sequence;
          sequence_contains_particle >> sequence_in_sequence;
          sequence_in_sequence >> sequence_contains_particle;

          PrimaryChoice choice_in_choice (*this, name_set, true);
          PrimarySequence sequence_in_choice (*this, name_set, true);
          Traversal::ContainsParticle choice_contains_particle;

          sequence_in_choice >> sequence_contains_particle;
          choice_contains_particle >> element;
          choice_contains_particle >> choice_in_choice;
          choice_contains_particle >> sequence_in_choice;
          choice_in_choice >> choice_contains_particle;
          choice_in_sequence >> choice_contains_particle;

          Sequence::contains (s, sequence_contains_particle);
        }

        SecondaryElement element (*this, name_set, data_members_);
        SecondaryChoice choice_in_sequence (
          *this, name_set, map, false, data_members_);
        SecondarySequence sequence_in_sequence (
          *this, name_set, map, false, data_members_);
        Traversal::ContainsParticle sequence_contains_particle;

        sequence_contains_particle >> element;
        sequence_contains_particle >> choice_in_sequence;
        sequence_contains_particle >> sequence_in_sequence;
        sequence_in_sequence >> sequence_contains_particle;

        SecondaryChoice choice_in_choice (
          *this, name_set, map, true, data_members_);
        SecondarySequence sequence_in_choice (
          *this, name_set, map, true, data_members_);
        Traversal::ContainsParticle choice_contains_particle;

        sequence_in_choice >> sequence_contains_particle;
        choice_contains_particle >> element;
        choice_contains_particle >> choice_in_choice;
        choice_contains_particle >> sequence_in_choice;
        choice_in_choice >> choice_contains_particle;
        choice_in_sequence >> choice_contains_particle;

        Sequence::contains (s, sequence_contains_particle);
      }

      //
      //
      struct Complex: Traversal::Complex, Context
      {
        Complex (Context& c, Boolean data_members)
            : Context (c), data_members_ (data_members)
        {
        }

        virtual Void
        traverse (Type& c)
        {
          if (data_members_)
            assign_data (c);
          else
            assign_names (c);
        }

        virtual Void
        assign_names (Type& c)
        {
          SemanticGraph::Context& cc (c.context ());

          // In case of customization use name-base instead of name.
          // If name is empty then we are not generating anything.
          //
          String const& name (cc.count ("name-base")
                              ? cc.get<String> ("name-base")
                              : cc.get<String> ("name"));
          if (!name)
            return;

          // Check if this type or any of its nested types have
          // custom data.
          //
          CustomDataMap* map (0);
          {
            CustomDataMap::Iterator i (custom_data_map.find (c.name ()));
            if (i != custom_data_map.end ())
              map = i->second.get ();
          }

          cc.set (member_set_key, NameSet ());
          NameSet& member_set (cc.get<NameSet> (member_set_key));

          member_set.insert (name);

          // Add our base's members to the initial list.
          //
          Boolean restriction (false);

          if (c.inherits_p ())
          {
            SemanticGraph::Type& b (c.inherits ().base ());

            if (b.is_a<SemanticGraph::Complex> ())
            {
              SemanticGraph::Context& bc (b.context ());

              if (!bc.count (member_set_key))
                dispatch (b);

              // We may still not have the set if this type is being
              // customized.
              //
              if (bc.count (member_set_key))
              {
                NameSet const& bset (bc.get<NameSet> (member_set_key));
                member_set.insert (bset.begin (), bset.end ());
              }
            }

            // Inheritance by restriction from anyType is a special case.
            //
            restriction = c.inherits ().is_a<SemanticGraph::Restricts> () &&
              !b.is_a<SemanticGraph::AnyType> ();
          }

          if (!restriction)
          {
            // First assign the "primary" names.
            //
            PrimaryAttribute pri_attribute (*this, member_set);
            Traversal::Names pri_names (pri_attribute);

            Complex::names (c, pri_names);

            if (c.contains_compositor_p ())
            {
              PrimaryElement element (*this, member_set);
              PrimaryAll all (*this, member_set);
              Traversal::ContainsParticle all_contains_particle;

              all >> all_contains_particle >> element;

              PrimaryChoice choice_in_sequence (*this, member_set, false);
              PrimarySequence sequence_in_sequence (*this, member_set, false);
              Traversal::ContainsParticle sequence_contains_particle;

              sequence_contains_particle >> element;
              sequence_contains_particle >> choice_in_sequence;
              sequence_contains_particle >> sequence_in_sequence;
              sequence_in_sequence >> sequence_contains_particle;

              PrimaryChoice choice_in_choice (*this, member_set, true);
              PrimarySequence sequence_in_choice (*this, member_set, true);
              Traversal::ContainsParticle choice_contains_particle;

              sequence_in_choice >> sequence_contains_particle;
              choice_contains_particle >> element;
              choice_contains_particle >> choice_in_choice;
              choice_contains_particle >> sequence_in_choice;
              choice_in_choice >> choice_contains_particle;
              choice_in_sequence >> choice_contains_particle;

              Traversal::ContainsCompositor contains_compositor;

              contains_compositor >> all;
              contains_compositor >> choice_in_sequence;
              contains_compositor >> sequence_in_sequence;

              Complex::contains_compositor (c, contains_compositor);
            }

            // Assign "secondary" names.
            //
            SecondaryAttribute sec_attribute (*this, member_set, false);
            Traversal::Names sec_names (sec_attribute);

            Complex::names (c, sec_names);

            if (c.contains_compositor_p ())
            {
              SecondaryElement element (*this, member_set, false);
              SecondaryAll all (*this, member_set, map, false);
              Traversal::ContainsParticle all_contains_particle;

              all >> all_contains_particle >> element;

              SecondaryChoice choice_in_sequence (
                *this, member_set, map, false, false);
              SecondarySequence sequence_in_sequence (
                *this, member_set, map, false, false);
              Traversal::ContainsParticle sequence_contains_particle;

              sequence_contains_particle >> element;
              sequence_contains_particle >> choice_in_sequence;
              sequence_contains_particle >> sequence_in_sequence;
              sequence_in_sequence >> sequence_contains_particle;

              SecondaryChoice choice_in_choice (
                *this, member_set, map, true, false);
              SecondarySequence sequence_in_choice (
                *this, member_set, map, true, false);
              Traversal::ContainsParticle choice_contains_particle;

              sequence_in_choice >> sequence_contains_particle;
              choice_contains_particle >> element;
              choice_contains_particle >> choice_in_choice;
              choice_contains_particle >> sequence_in_choice;
              choice_in_choice >> choice_contains_particle;
              choice_in_sequence >> choice_contains_particle;

              Traversal::ContainsCompositor contains_compositor;

              contains_compositor >> all;
              contains_compositor >> choice_in_sequence;
              contains_compositor >> sequence_in_sequence;

              Complex::contains_compositor (c, contains_compositor);
            }
          }

          // Custom data.
          //
          if (map && map->find (L"") != map->end ())
          {
            String name (find_name ("custom_data", member_set));

            cc.set ("cd-name", name);

            cc.set ("cd-sequence",
                    find_name (name + L"_sequence", member_set));

            cc.set ("cd-iterator",
                    find_name (name + L"_iterator", member_set));

            cc.set ("cd-const-iterator",
                    find_name (name + L"_const_iterator", member_set));
          }
        }

        virtual Void
        assign_data (Type& c)
        {
          SemanticGraph::Context& cc (c.context ());

          // In case of customization use name-base instead of name.
          // If name is empty then we are not generating anything.
          //
          String const& name (cc.count ("name-base")
                              ? cc.get<String> ("name-base")
                              : cc.get<String> ("name"));
          if (!name)
            return;

          //
          //
          Boolean restriction (false);

          if (c.inherits_p ())
            restriction = c.inherits ().is_a<SemanticGraph::Restricts> () &&
              !c.inherits ().base ().is_a<SemanticGraph::AnyType> ();

          NameSet& member_set (cc.get<NameSet> (member_set_key));

          if (!restriction)
          {
            SecondaryAttribute sec_attribute (*this, member_set, true);
            Traversal::Names sec_names (sec_attribute);

            Complex::names (c, sec_names);

            if (c.contains_compositor_p ())
            {
              SecondaryElement element (*this, member_set, true);
              SecondaryAll all (*this, member_set, 0, true);
              Traversal::ContainsParticle all_contains_particle;

              all >> all_contains_particle >> element;

              SecondaryChoice choice_in_sequence (
                *this, member_set, 0, false, true);

              SecondarySequence sequence_in_sequence (
                *this, member_set, 0, false, true);

              Traversal::ContainsParticle sequence_contains_particle;

              sequence_contains_particle >> element;
              sequence_contains_particle >> choice_in_sequence;
              sequence_contains_particle >> sequence_in_sequence;
              sequence_in_sequence >> sequence_contains_particle;

              SecondaryChoice choice_in_choice (
                *this, member_set, 0, true, true);

              SecondarySequence sequence_in_choice (
                *this, member_set, 0, true, true);

              Traversal::ContainsParticle choice_contains_particle;

              sequence_in_choice >> sequence_contains_particle;
              choice_contains_particle >> element;
              choice_contains_particle >> choice_in_choice;
              choice_contains_particle >> sequence_in_choice;
              choice_in_choice >> choice_contains_particle;
              choice_in_sequence >> choice_contains_particle;

              Traversal::ContainsCompositor contains_compositor;

              contains_compositor >> all;
              contains_compositor >> choice_in_sequence;
              contains_compositor >> sequence_in_sequence;

              Complex::contains_compositor (c, contains_compositor);
            }
          }

          // Custom data.
          //
          if (cc.count ("cd-name"))
          {
            String const& base (cc.get<String> ("cd-name"));
            cc.set ("cd-member", find_name (base + L"_", member_set));
          }
        }

      private:
        Boolean data_members_;
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
          String const& name (t.name ());
          SemanticGraph::Context& tc (t.context ());

          tc.set ("name", find_name (name, set_));

          // See if this parser is being customized.
          //
          CustomTypeMap::ConstIterator i (custom_type_map.find (name));

          if (i != custom_type_map.end ())
          {
            if (i->second.type)
              tc.set ("name-typedef", i->second.type);

            // The empty name-base indicates that we don't need to
            // generate anything.
            //
            tc.set ("name-base", i->second.base
                    ? find_name (i->second.base, set_)
                    : i->second.base);

            if (i->second.include)
              tc.set ("name-include", i->second.include);
          }
        }

      private:
        NameSet& set_;
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

          GlobalType type (*this, type_set);
          Traversal::Names names (type);

          Traversal::Namespace::names (ns, names);
        }
      };

      struct FundType : Traversal::AnyType,
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
                        Traversal::Fundamental::Entities,

                        Context

      {
        FundType (Context& c)
            : Context (c)
        {
        }

        // anyType & anySimpleType.
        //
        virtual Void
        traverse (SemanticGraph::AnyType& t)
        {
          set_name (t, "any_type");
        }

        virtual Void
        traverse (SemanticGraph::AnySimpleType& t)
        {
          set_name (t, "any_simple_type");
        }

        // Boolean.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Boolean& t)
        {
          set_name (t, "boolean");
        }

        // Integral types.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Byte& t)
        {
          set_name (t, "byte");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedByte& t)
        {
          set_name (t, "unsigned_byte");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Short& t)
        {
          set_name (t, "short");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedShort& t)
        {
          set_name (t, "unsigned_short");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Int& t)
        {
          set_name (t, "int");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedInt& t)
        {
          set_name (t, "unsigned_int");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Long& t)
        {
          set_name (t, "long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedLong& t)
        {
          set_name (t, "unsigned_long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Integer& t)
        {
          set_name (t, "integer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NonPositiveInteger& t)
        {
          set_name (t, "non_positive_integer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NonNegativeInteger& t)
        {
          set_name (t, "non_negative_integer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::PositiveInteger& t)
        {
          set_name (t, "positive_integer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NegativeInteger& t)
        {
          set_name (t, "negative_integer");
        }

        // Floats.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Float& t)
        {
          set_name (t, "float");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Double& t)
        {
          set_name (t, "double");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Decimal& t)
        {
          set_name (t, "decimal");
        }

        // Strings.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::String& t)
        {
          set_name (t, "string");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NormalizedString& t)
        {
          set_name (t, "normalized_string");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Token& t)
        {
          set_name (t, "token");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NameToken& t)
        {
          set_name (t, "nmtoken");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NameTokens& t)
        {
          set_name (t, "nmtokens");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Name& t)
        {
          set_name (t, "name");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NCName& t)
        {
          set_name (t, "ncname");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Language& t)
        {
          set_name (t, "language");
        }


        // Qualified name.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::QName& t)
        {
          set_name (t, "qname");
        }


        // ID/IDREF.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Id& t)
        {
          set_name (t, "id");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::IdRef& t)
        {
          set_name (t, "idref");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::IdRefs& t)
        {
          set_name (t, "idrefs");
        }

        // URI.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::AnyURI& t)
        {
          set_name (t, "uri");
        }

        // Binary.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Base64Binary& t)
        {
          set_name (t, "base64_binary");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::HexBinary& t)
        {
          set_name (t, "hex_binary");
        }


        // Date/time.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Date& t)
        {
          set_name (t, "date");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::DateTime& t)
        {
          set_name (t, "date_time");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Duration& t)
        {
          set_name (t, "duration");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Day& t)
        {
          set_name (t, "gday");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Month& t)
        {
          set_name (t, "gmonth");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::MonthDay& t)
        {
          set_name (t, "gmonth_day");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Year& t)
        {
          set_name (t, "gyear");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::YearMonth& t)
        {
          set_name (t, "gyear_month");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Time& t)
        {
          set_name (t, "time");
        }

        // Entity.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Entity& t)
        {
          set_name (t, "entity");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Entities& t)
        {
          set_name (t, "entities");
        }

      private:
        Void
        set_name (SemanticGraph::Type& t, String const& name)
        {
          SemanticGraph::Context& c (t.context ());
          c.set ("name", escape (name));
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

          if (!s.context ().count ("cxx-hybrid-name-processor-seen"))
          {
            s.context ().set ("cxx-hybrid-name-processor-seen", true);
            Traversal::Sources::traverse (sr);
          }
        }

        virtual Void
        traverse (SemanticGraph::Includes& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count ("cxx-hybrid-name-processor-seen"))
          {
            s.context ().set ("cxx-hybrid-name-processor-seen", true);
            Traversal::Includes::traverse (i);
          }
        }

        virtual Void
        traverse (SemanticGraph::Imports& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count ("cxx-hybrid-name-processor-seen"))
          {
            s.context ().set ("cxx-hybrid-name-processor-seen", true);
            Traversal::Imports::traverse (i);
          }
        }
      };

      // Go into implied schemas while making sure we don't process
      // the same stuff more than once.
      //
      struct Implies: Traversal::Implies
      {
        virtual Void
        traverse (SemanticGraph::Implies& i)
        {
          SemanticGraph::Schema& s (i.schema ());

          if (!s.context ().count ("cxx-hybrid-name-processor-seen"))
          {
            s.context ().set ("cxx-hybrid-name-processor-seen", true);
            Traversal::Implies::traverse (i);
          }
        }
      };

      Void
      process_impl (CLI::Options const& ops,
                    SemanticGraph::Schema& tu,
                    SemanticGraph::Path const& file,
                    Boolean deep)
      {
        Context ctx (ops, tu, file);

        if (tu.names_begin ()->named ().name () ==
            L"http://www.w3.org/2001/XMLSchema")
        {
          // XML Schema namespace.
          //
          Traversal::Schema schema;

          Traversal::Names schema_names;
          Traversal::Namespace ns;
          Traversal::Names ns_names;
          FundType fund_type (ctx);

          schema >> schema_names >> ns >> ns_names >> fund_type;

          schema.dispatch (tu);
        }
        else
        {
          // Pass one - assign names to fundamental types.
          //
          {
            Traversal::Schema schema;
            Implies implies;
            Traversal::Schema xs_schema;

            schema >> implies >> xs_schema;

            Traversal::Names xs_schema_names;
            Traversal::Namespace ns;
            Traversal::Names ns_names;
            FundType fund_type (ctx);

            xs_schema >> xs_schema_names >> ns >> ns_names >> fund_type;

            schema.dispatch (tu);
          }

          // Pass two - assign names to global types. This pass cannot
          // be combined with pass three because of possible recursive
          // schema inclusions. Also note that we check first if this
          // schema has already been processed which may happen in the
          // file-per-type compilation mode.
          //
          if (!tu.context ().count ("cxx-hybrid-name-processor-seen"))
          {
            Traversal::Schema schema;
            Uses uses;

            schema >> uses >> schema;

            Traversal::Names schema_names;
            Namespace ns (ctx);

            schema >> schema_names >> ns;

            // Some twisted schemas do recusive self-inclusion.
            //
            tu.context ().set ("cxx-hybrid-name-processor-seen", true);

            schema.dispatch (tu);
          }

          if (!deep)
            return;

          // Pass three - assign names inside complex types. Here we don't
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

            List list (ctx, false);
            Union union_ (ctx, false);
            Complex complex (ctx, false);

            ns_names >> list;
            ns_names >> union_;
            ns_names >> complex;

            schema.dispatch (tu);
          }

          // Pass four - assign names to data memeber. Here we aslo don't
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

            List list (ctx, true);
            Union union_ (ctx, true);
            Complex complex (ctx, true);

            ns_names >> list;
            ns_names >> union_;
            ns_names >> complex;

            schema.dispatch (tu);
          }
        }
      }
    }

    Void TreeNameProcessor::
    process (CLI::Options const& ops,
             SemanticGraph::Schema& tu,
             SemanticGraph::Path const& file,
             Boolean deep)
    {
      process_impl (ops, tu, file, deep);
    }
  }
}
