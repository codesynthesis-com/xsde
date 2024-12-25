// file      : xsde/cxx/parser/driver-source.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <set>
#include <map>
#include <sstream>

#include <xsde/cxx/parser/driver-source.hxx>
#include <xsde/cxx/parser/print-impl-common.hxx>

#include <libxsd-frontend/semantic-graph.hxx>
#include <libxsd-frontend/traversal.hxx>

using namespace std;

namespace CXX
{
  namespace Parser
  {
    namespace
    {
      typedef map<SemanticGraph::Type*, String> TypeInstanceMap;
      typedef set<String> InstanceSet;

      // For base types we only want member's types, but not the
      // base itself.
      //
      struct BaseType: Traversal::Complex,
                       Traversal::List,
                       Context
      {
        BaseType (Context& c, Traversal::NodeBase& def)
            : Context (c), def_ (def)
        {
        }

        virtual void
        traverse (SemanticGraph::Complex& c)
        {
          inherits (c);

          if (!restriction_p (c))
          {
            names (c);
            contains_compositor (c);
          }
        }

        virtual void
        traverse (SemanticGraph::List& l)
        {
          def_.dispatch (l.argumented ().type ());
        }

      private:
        Traversal::NodeBase& def_;
      };

      struct ParserDef: Traversal::Type,
                        Traversal::List,
                        Traversal::Complex,

                        Traversal::AnyType,
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
        ParserDef (Context& c, TypeInstanceMap& map, InstanceSet& set)
            : Context (c), map_ (map), set_ (set), base_ (c, *this)
        {
          *this >> inherits_ >> base_ >> inherits_;

          *this >> contains_compositor_;
          base_ >> contains_compositor_;

          *this >> names_;
          base_ >> names_;

          contains_compositor_ >> compositor_;
          compositor_ >> contains_particle_;
          contains_particle_ >> compositor_;
          contains_particle_ >> particle_;

          names_ >> attribute_;

          particle_ >> belongs_;
          attribute_ >> belongs_;
          belongs_ >> *this;
        }

        virtual void
        traverse (SemanticGraph::Type& t)
        {
          if (map_.find (&t) == map_.end ())
          {
            String inst (find_instance_name (t));
            map_[&t] = inst;

            os << fq_name (t, "p:impl") << " " << inst << ";";
          }
        }

        virtual void
        traverse (SemanticGraph::List& l)
        {
          if (map_.find (&l) == map_.end ())
          {
            String inst (find_instance_name (l));
            map_[&l] = inst;

            os << fq_name (l, "p:impl") << " " << inst << ";";

            dispatch (l.argumented ().type ());
          }
        }

        virtual void
        traverse (SemanticGraph::Complex& c)
        {
          if (map_.find (&c) == map_.end ())
          {
            String inst (find_instance_name (c));
            map_[&c] = inst;

            os << fq_name (c, "p:impl") << " " << inst << ";";

            inherits (c);

            if (!restriction_p (c))
            {
              names (c);
              contains_compositor (c);
            }
          }
        }

        // anyType & anySimpleType.
        //
        virtual void
        traverse (SemanticGraph::AnyType& t)
        {
          fund_type (t, "any_type");
        }

        virtual void
        traverse (SemanticGraph::AnySimpleType& t)
        {
          fund_type (t, "any_simple_type");
        }

        // Boolean.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Boolean& t)
        {
          fund_type (t, "boolean");
        }

        // Integral types.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Byte& t)
        {
          fund_type (t, "byte");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedByte& t)
        {
          fund_type (t, "unsigned_byte");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Short& t)
        {
          fund_type (t, "short");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedShort& t)
        {
          fund_type (t, "unsigned_short");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Int& t)
        {
          fund_type (t, "int");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedInt& t)
        {
          fund_type (t, "unsigned_int");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Long& t)
        {
          fund_type (t, "long");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::UnsignedLong& t)
        {
          fund_type (t, "unsigned_long");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Integer& t)
        {
          fund_type (t, "integer");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NonPositiveInteger& t)
        {
          fund_type (t, "non_positive_integer");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NonNegativeInteger& t)
        {
          fund_type (t, "non_negative_integer");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::PositiveInteger& t)
        {
          fund_type (t, "positive_integer");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NegativeInteger& t)
        {
          fund_type (t, "negative_integer");
        }

        // Floats.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Float& t)
        {
          fund_type (t, "float");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Double& t)
        {
          fund_type (t, "double");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Decimal& t)
        {
          fund_type (t, "decimal");
        }

        // Strings.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::String& t)
        {
          fund_type (t, "string");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NormalizedString& t)
        {
          fund_type (t, "normalized_string");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Token& t)
        {
          fund_type (t, "token");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NameToken& t)
        {
          fund_type (t, "nmtoken");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NameTokens& t)
        {
          fund_type (t, "nmtokens");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Name& t)
        {
          fund_type (t, "name");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::NCName& t)
        {
          fund_type (t, "ncname");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Language& t)
        {
          fund_type (t, "language");
        }


        // Qualified name.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::QName& t)
        {
          fund_type (t, "qname");
        }


        // ID/IDREF.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Id& t)
        {
          fund_type (t, "id");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::IdRef& t)
        {
          fund_type (t, "idref");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::IdRefs& t)
        {
          fund_type (t, "idrefs");
        }

        // URI.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::AnyURI& t)
        {
          fund_type (t, "uri");
        }

        // Binary.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Base64Binary& t)
        {
          fund_type (t, "base64_binary");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::HexBinary& t)
        {
          fund_type (t, "hex_binary");
        }


        // Date/time.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Date& t)
        {
          fund_type (t, "date");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::DateTime& t)
        {
          fund_type (t, "date_time");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Duration& t)
        {
          fund_type (t, "duration");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Day& t)
        {
          fund_type (t, "day");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Month& t)
        {
          fund_type (t, "month");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::MonthDay& t)
        {
          fund_type (t, "month_day");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Year& t)
        {
          fund_type (t, "year");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::YearMonth& t)
        {
          fund_type (t, "year_month");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Time& t)
        {
          fund_type (t, "time");
        }

        // Entity.
        //
        virtual void
        traverse (SemanticGraph::Fundamental::Entity& t)
        {
          fund_type (t, "entity");
        }

        virtual void
        traverse (SemanticGraph::Fundamental::Entities& t)
        {
          fund_type (t, "entities");
        }

      private:
        virtual void
        fund_type (SemanticGraph::Type& t, String const& name)
        {
          if (map_.find (&t) == map_.end ())
          {
            String inst (find_instance_name (name));
            map_[&t] = inst;

            os << fq_name (t, "p:impl") << " " << inst << ";";
          }
        }

        String
        find_instance_name (String const& raw_name)
        {
          String base_name (escape (raw_name + L"_p"));
          String name (base_name);

          for (size_t i (1); set_.find (name) != set_.end (); ++i)
          {
            std::wostringstream os;
            os << i;
            name = base_name + os.str ();
          }

          set_.insert (name);
          return name;
        }

        String
        find_instance_name (SemanticGraph::Type& t)
        {
          return find_instance_name (t.name ());
        }

        TypeInstanceMap& map_;
        InstanceSet& set_;

        BaseType base_;
        Traversal::Inherits inherits_;

        Traversal::Compositor compositor_;
        Traversal::Element particle_;
        Traversal::ContainsCompositor contains_compositor_;
        Traversal::ContainsParticle contains_particle_;

        Traversal::Names names_;
        Traversal::Attribute attribute_;

        Traversal::Belongs belongs_;
      };

      //
      //
      struct ParticleArg: Traversal::Element, Context
      {
        ParticleArg (Context& c, TypeInstanceMap& map, bool& first)
            : Context (c), map_ (map), first_ (first)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          if (!first_)
            os << "," << endl;
          else
            first_ = false;

          os << map_[&e.type ()];
        }

      private:
        TypeInstanceMap& map_;
        bool& first_;
      };

      struct AttributeArg: Traversal::Attribute, Context
      {
        AttributeArg (Context& c, TypeInstanceMap& map, bool& first)
            : Context (c), map_ (map), first_ (first)
        {
        }

        virtual void
        traverse (Type& a)
        {
          if (!first_)
            os << "," << endl;
          else
            first_ = false;

          os << map_[&a.type ()];
        }

      private:
        TypeInstanceMap& map_;
        bool& first_;
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
          if (!first_)
            os << "," << endl;
          else
            first_ = false;

          os << map_[&l.argumented ().type ()];
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

        bool first_;
      };

      struct ParserConnect: Traversal::List,
                            Traversal::Complex,
                            Context
      {
        ParserConnect (Context& c, TypeInstanceMap& map)
            : Context (c), map_ (map), base_ (c, *this)
        {
          *this >> inherits_ >> base_ >> inherits_;

          *this >> contains_compositor_;
          base_ >> contains_compositor_;

          *this >> names_;
          base_ >> names_;

          contains_compositor_ >> compositor_;
          compositor_ >> contains_particle_;
          contains_particle_ >> compositor_;
          contains_particle_ >> particle_;

          names_ >> attribute_;

          particle_ >> belongs_;
          attribute_ >> belongs_;
          belongs_ >> *this;
        }

        virtual void
        traverse (SemanticGraph::List& l)
        {
          if (type_set_.find (&l) == type_set_.end ())
          {
            os << map_[&l] << ".parsers (" <<
              map_[&l.argumented ().type ()] << ");"
               << endl;

            type_set_.insert (&l);
          }
        }

        virtual void
        traverse (SemanticGraph::Complex& c)
        {
          if (type_set_.find (&c) == type_set_.end ())
          {
            if (has_members (c))
            {
              os << map_[&c] << ".parsers (";

              ArgList args (*this, map_);
              args.dispatch (c);

              os << ");"
                 << endl;
            }

            type_set_.insert (&c);

            inherits (c);

            if (!restriction_p (c))
            {
              names (c);
              contains_compositor (c);
            }
          }
        }

      private:
        bool
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
        set<SemanticGraph::Type*> type_set_;

        BaseType base_;
        Traversal::Inherits inherits_;

        Traversal::Compositor compositor_;
        Traversal::Element particle_;
        Traversal::ContainsCompositor contains_compositor_;
        Traversal::ContainsParticle contains_particle_;

        Traversal::Names names_;
        Traversal::Attribute attribute_;

        Traversal::Belongs belongs_;
      };

      struct GlobalElement: Traversal::Element, Context
      {
        GlobalElement (Context& c, SemanticGraph::Element*& element)
            : Context (c), element_ (element)
        {
        }

        virtual void
        traverse (Type& e)
        {
          if (options.root_element_first ())
          {
            if (element_ == 0)
              element_ = &e;
          }
          else if (String name = options.root_element ())
          {
            if (e.name () == name)
              element_ = &e;
          }
          else
            element_ = &e; // Cover root-element-last and no option.
        }

      private:
        SemanticGraph::Element*& element_;
      };
    }

    void
    generate_driver_source (Context& ctx)
    {
      // Figure out the root element. Validator should have made sure
      // it is unique.
      //
      SemanticGraph::Element* root (0);
      {
        Traversal::Schema schema;
        Sources sources;

        schema >> sources >> schema;

        Traversal::Names schema_names;
        Traversal::Namespace ns;
        Traversal::Names ns_names;
        RootElement global_element (ctx.options, root);

        schema >> schema_names >> ns >> ns_names >> global_element;

        schema.dispatch (ctx.schema_root);
      }

      String xs (ctx.xs_ns_name ());
      std::wostream& os (ctx.os);

      InstanceSet set;
      TypeInstanceMap map;
      SemanticGraph::Type& root_type (root->type ());

      set.insert ("doc_p");

      if (ctx.options.no_iostream ())
        os << "#include <stdio.h>" << endl;
      else
        os << "#include <iostream>" << endl;

      os << endl
         << "int" << endl
         << "main (int argc, char* argv[])"
         << "{"
         << "const char* input;"
         << endl
         << "if (argc < 2)"
         << "{"
         << "input = \"STDIN\";";

      if (ctx.options.no_iostream ())
        os << "fprintf (stderr, \"XML file not specified, reading " <<
          "from STDIN\\n\");";
      else
        os << "std::cerr << \"XML file not specified, reading " <<
          "from STDIN\" << std::endl;";

      os << "}"
         << "else" << endl
         << "input = argv[1];"
         << endl;

      if (ctx.options.no_iostream ())
      {
        os << "FILE* f = argc > 1 ? fopen (argv[1], \"rb\") : stdin;"
           << endl
           << "if (f == 0)"
           << "{"
           << "fprintf (stderr, \"%s: unable to open\\n\", input);"
           << "return 1;"
           << "}";
      }

      if (ctx.options.no_iostream () || ctx.options.no_exceptions ())
        os << "int r = 0;"
           << endl;

      if (!ctx.options.no_exceptions ())
        os << "try"
           << "{";

      os << "// Instantiate individual parsers." << endl
         << "//" << endl;

      {
        ParserDef def (ctx, map, set);
        def.dispatch (root_type);
      }

      os << endl
         << "// Connect the parsers together." << endl
         << "//" << endl;

      {
        // @@ I can simply iterate over the map instead of traversing
        // the tree all over again.
        //
        ParserConnect connect (ctx, map);
        connect.dispatch (root_type);
      }

      String const& root_p (map[&root_type]);

      os << "// Parse the XML document." << endl
         << "//" << endl;

      if (ctx.options.no_iostream ())
        os << "bool io_error = false;";

      if (ctx.options.no_exceptions ())
        os << xs << "::parser_error e;"
           << endl
           << "do"
           << "{";

      if (root->namespace_().name ())
        os << xs << "::document_pimpl doc_p (" << endl
           << root_p << "," << endl
           << ctx.strlit (root->namespace_().name ()) << "," << endl
           << ctx.strlit (root->name ()) << ");"
           << endl;
      else
        os << xs << "::document_pimpl doc_p (" << root_p << ", " <<
          ctx.strlit (root->name ()) << ");"
           << endl;

      if (ctx.options.no_exceptions ())
        os << "if ((e = doc_p._error ()))" << endl
           << "break;"
           << endl;

      os << root_p << ".pre ();";

      if (ctx.options.no_exceptions ())
        os << endl
           << "if ((e = " << root_p << "._error ()))" << endl
           << "break;";


      if (ctx.options.no_iostream ())
      {
        os << endl
           << "char buf[4096];"
           << endl
           << "do"
           << "{"
           << "size_t s = fread (buf, 1, sizeof (buf), f);"
           << endl
           << "if (s != sizeof (buf) && ferror (f))"
           << "{"
           << "io_error = true;"
           << "break;"
           << "}";

        if (ctx.options.no_exceptions ())
          os << "doc_p.parse (buf, s, feof (f) != 0);"
             << "e = doc_p._error ();"
             << "}"
             << "while (!e && !feof (f));"
             << endl
             << "if (io_error || e)" << endl
             << "break;"
             << endl;
        else
          os << "doc_p.parse (buf, s, feof (f) != 0);"
             << "}"
             << "while (!feof (f));"
             << endl
             << "if (!io_error)"
             << "{";
      }
      else
      {
        os << endl
           << "if (argc < 2)" << endl
           << "doc_p.parse (std::cin);"
           << "else" << endl
           << "doc_p.parse (argv[1]);"
           << endl;

        if (ctx.options.no_exceptions ())
          os << "if ((e = doc_p._error ()))" << endl
             << "break;"
             << endl;
      }

      String const& ret (Context::ret_type (root_type));
      String const& post (Context::post_name (root_type));

      if (ret == L"void")
      {
        os << root_p << "." << post << " ();";

        if (ctx.options.no_exceptions ())
          os << "e = " << root_p << "._error ();";
      }
      else
      {
        os << Context::arg_type (root_type) << " v = " <<
          root_p << "." << post << " ());"
           << endl;

        if (ctx.options.no_exceptions ())
          os << "if ((e = " << root_p << "._error ()))" << endl
             << "break;"
             << endl;

        if (ctx.options.generate_print_impl ())
        {
          PrintCall t (ctx, root->name (), "v");
          t.dispatch (root_type);
        }
        else
        {
          os << "// TODO" << endl
             << "//" << endl;
        }

        {
          DeleteCall t (ctx, "v");
          t.dispatch (root_type);
        }
      }

      if (ctx.options.no_exceptions ())
        os << "}"
           << "while (false);"
           << endl;
      else if (ctx.options.no_iostream ())
        os << "}" // if (!io_error)
           << "else"
           << "{"
           << "fprintf (stderr, \"%s: read failure\\n\", input);"
           << "r = 1;"
           << "}";

      // Error handling.
      //

      if (ctx.options.no_exceptions ())
      {
        os << "// Handle errors." << endl
           << "//" << endl;

        if (ctx.options.no_iostream ())
        {
          os << "if (io_error)"
             << "{";

          if (ctx.options.no_iostream ())
            os << "fprintf (stderr, \"%s: read failure\\n\", input);";
          else
            os << "std::cerr << input << \": read failure\" << std::endl;";

          os << "r =  1;"
             << "}"
             << "else ";
        }

        os << "if (e)"
           << "{"
           << "switch (e.type ())"
           << "{"
           << "case " << xs << "::parser_error::sys:"
           << "{";

        if (ctx.options.no_iostream ())
          os << "fprintf (stderr, \"%s: %s\\n\", input, e.sys_text ());";
        else
          os << "std::cerr << input << \": \" << e.sys_text () << std::endl;";

        os << "break;"
           << "}"
           << "case " << xs << "::parser_error::xml:"
           << "{";

        if (ctx.options.no_iostream ())
          os << "fprintf (stderr, \"%s:%lu:%lu: %s\\n\"," << endl
             << "input, e.line (), e.column (), e.xml_text ());";
        else
          os << "std::cerr << input << ':' << e.line () << ':'" << endl
             << "  << e.column () << \": \" << e.xml_text () << std::endl;";

        os << "break;"
           << "}";

        if (!ctx.options.suppress_validation ())
        {
          os << "case " << xs << "::parser_error::schema:"
             << "{";

          if (ctx.options.no_iostream ())
            os << "fprintf (stderr, \"%s:%lu:%lu: %s\\n\"," << endl
               << "input, e.line (), e.column (), e.schema_text ());";
          else
            os << "std::cerr << input << ':' << e.line () << ':'" << endl
               << "  << e.column () << \": \" << e.schema_text () << std::endl;";

          os << "break;"
             << "}";
        }

        os << "case " << xs << "::parser_error::app:"
           << "{";

        if (ctx.options.no_iostream ())
          os << "fprintf (stderr, \"%s:%lu:%lu: application error %d\\n\"," << endl
             << "input, e.line (), e.column (), e.app_code ());";
        else
          os << "std::cerr << input << ':' << e.line () << ':'" << endl
             << "  << e.column () << \": application error \"" << endl
             << "  << e.app_code () << std::endl;";

        os << "break;"
           << "}"
           << "default:"
           << "{"
           << "break;"
           << "}"
           << "}" //switch
           << "r = 1;"
           << "}"; // else if (e)
      }
      else
      {
        os << "}" // try
           << "catch (const " << xs << "::parser_exception& e)"
           << "{";

        if (ctx.options.no_iostream ())
          os << "fprintf (stderr, \"%s:%lu:%lu: %s\\n\"," << endl
             << "input, e.line (), e.column (), e.text ());"
             << "r = 1;";
        else
          os << "std::cerr << input << ':' << e.line () << ':'" << endl
             << "  << e.column () << \": \" << e.text () << std::endl;"
             << "return 1;";

        os << "}";

        if (!ctx.options.no_iostream ())
        {
          os << "catch (const std::ios_base::failure&)"
             << "{"
             << "std::cerr << input << \": unable to open or " <<
            "read failure\" << std::endl;"
             << "return 1;"
             << "}";
        }
      }

      if (ctx.options.no_iostream ())
        os << "if (argc > 1)" << endl
           << "fclose (f);"
           << endl;

      if (ctx.options.no_iostream () || ctx.options.no_exceptions ())
        os << "return r;";
      else
        os << "return 0;";

      os << "}"; // main
    }
  }
}
