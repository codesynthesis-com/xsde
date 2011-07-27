// file      : xsde/cxx/serializer/driver-source.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cxx/serializer/driver-source.hxx>

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

#include <cult/containers/map.hxx>
#include <cult/containers/set.hxx>

#include <sstream>

namespace CXX
{
  namespace Serializer
  {
    namespace
    {
      typedef
      Cult::Containers::Map<SemanticGraph::Type*, String>
      TypeInstanceMap;

      typedef Cult::Containers::Set<String> InstanceSet;

      // For base types we only want member's types, but not the
      // base itself.
      //
      struct BaseType: Traversal::Complex, Context
      {
        BaseType (Context& c)
            : Context (c)
        {
        }

        virtual Void
        traverse (SemanticGraph::Complex& c)
        {
          inherits (c);

          if (!restriction_p (c))
          {
            names (c);
            contains_compositor (c);
          }
        }
      };

      struct SerializerDef: Traversal::Type,
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
        SerializerDef (Context& c, TypeInstanceMap& map, InstanceSet& set)
            : Context (c), map_ (map), set_ (set), base_ (c)
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

        virtual Void
        traverse (SemanticGraph::Type& t)
        {
          if (map_.find (&t) == map_.end ())
          {
            String inst (find_instance_name (t));
            map_[&t] = inst;

            os << fq_name (t, "s:impl") << " " << inst << ";";
          }
        }

        virtual Void
        traverse (SemanticGraph::List& l)
        {
          if (map_.find (&l) == map_.end ())
          {
            String inst (find_instance_name (l));
            map_[&l] = inst;

            os << fq_name (l, "s:impl") << " " << inst << ";";

            dispatch (l.argumented ().type ());
          }
        }

        virtual Void
        traverse (SemanticGraph::Complex& c)
        {
          if (map_.find (&c) == map_.end ())
          {
            String inst (find_instance_name (c));
            map_[&c] = inst;

            os << fq_name (c, "s:impl") << " " << inst << ";";

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
        virtual Void
        traverse (SemanticGraph::AnyType& t)
        {
          fund_type (t, "any_type");
        }

        virtual Void
        traverse (SemanticGraph::AnySimpleType& t)
        {
          fund_type (t, "any_simple_type");
        }

        // Boolean.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Boolean& t)
        {
          fund_type (t, "boolean");
        }

        // Integral types.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Byte& t)
        {
          fund_type (t, "byte");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedByte& t)
        {
          fund_type (t, "unsigned_byte");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Short& t)
        {
          fund_type (t, "short");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedShort& t)
        {
          fund_type (t, "unsigned_short");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Int& t)
        {
          fund_type (t, "int");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedInt& t)
        {
          fund_type (t, "unsigned_int");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Long& t)
        {
          fund_type (t, "long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::UnsignedLong& t)
        {
          fund_type (t, "unsigned_long");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Integer& t)
        {
          fund_type (t, "integer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NonPositiveInteger& t)
        {
          fund_type (t, "non_positive_integer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NonNegativeInteger& t)
        {
          fund_type (t, "non_negative_integer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::PositiveInteger& t)
        {
          fund_type (t, "positive_integer");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NegativeInteger& t)
        {
          fund_type (t, "negative_integer");
        }

        // Floats.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Float& t)
        {
          fund_type (t, "float");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Double& t)
        {
          fund_type (t, "double");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Decimal& t)
        {
          fund_type (t, "decimal");
        }

        // Strings.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::String& t)
        {
          fund_type (t, "string");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NormalizedString& t)
        {
          fund_type (t, "normalized_string");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Token& t)
        {
          fund_type (t, "token");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NameToken& t)
        {
          fund_type (t, "nmtoken");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NameTokens& t)
        {
          fund_type (t, "nmtokens");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Name& t)
        {
          fund_type (t, "name");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::NCName& t)
        {
          fund_type (t, "ncname");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Language& t)
        {
          fund_type (t, "language");
        }


        // Qualified name.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::QName& t)
        {
          fund_type (t, "qname");
        }


        // ID/IDREF.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Id& t)
        {
          fund_type (t, "id");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::IdRef& t)
        {
          fund_type (t, "idref");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::IdRefs& t)
        {
          fund_type (t, "idrefs");
        }

        // URI.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::AnyURI& t)
        {
          fund_type (t, "uri");
        }

        // Binary.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Base64Binary& t)
        {
          fund_type (t, "base64_binary");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::HexBinary& t)
        {
          fund_type (t, "hex_binary");
        }


        // Date/time.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Date& t)
        {
          fund_type (t, "date");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::DateTime& t)
        {
          fund_type (t, "date_time");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Duration& t)
        {
          fund_type (t, "duration");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Day& t)
        {
          fund_type (t, "day");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Month& t)
        {
          fund_type (t, "month");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::MonthDay& t)
        {
          fund_type (t, "month_day");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Year& t)
        {
          fund_type (t, "year");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::YearMonth& t)
        {
          fund_type (t, "year_month");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Time& t)
        {
          fund_type (t, "time");
        }

        // Entity.
        //
        virtual Void
        traverse (SemanticGraph::Fundamental::Entity& t)
        {
          fund_type (t, "entity");
        }

        virtual Void
        traverse (SemanticGraph::Fundamental::Entities& t)
        {
          fund_type (t, "entities");
        }

      private:
        virtual Void
        fund_type (SemanticGraph::Type& t, String const& name)
        {
          if (map_.find (&t) == map_.end ())
          {
            String inst (find_instance_name (name));
            map_[&t] = inst;

            os << fq_name (t, "s:impl") << " " << inst << ";";
          }
        }

        String
        find_instance_name (String const& raw_name)
        {
          String base_name (escape (raw_name + L"_s"));
          String name (base_name);

          for (UnsignedLong i (1); set_.find (name) != set_.end (); ++i)
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

          os << map_[&e.type ()];
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

          os << map_[&a.type ()];
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

        Boolean first_;
      };

      struct SerializerConnect: Traversal::List,
                                Traversal::Complex,
                                Context
      {
        SerializerConnect (Context& c, TypeInstanceMap& map)
            : Context (c), map_ (map), base_ (c)
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

        virtual Void
        traverse (SemanticGraph::List& l)
        {
          if (type_set_.find (&l) == type_set_.end ())
          {
            os << map_[&l] << ".serializers (" <<
              map_[&l.argumented ().type ()] << ");"
               << endl;

            type_set_.insert (&l);
          }
        }

        virtual Void
        traverse (SemanticGraph::Complex& c)
        {
          if (type_set_.find (&c) == type_set_.end ())
          {
            if (has_members (c))
            {
              os << map_[&c] << ".serializers (";

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
        Cult::Containers::Set<SemanticGraph::Type*> type_set_;

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

        virtual Void
        traverse (Type& e)
        {
          if (options.value<CLI::root_element_first> ())
          {
            if (element_ == 0)
              element_ = &e;
          }
          else if (String name = options.value<CLI::root_element> ())
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

    Void
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

      set.insert ("doc_s");

      if (ctx.options.value<CLI::no_iostream> ())
        os << "#include <stdio.h>" << endl
           << endl;
      else
        os << "#include <iostream>" << endl
           << endl;

      if (ctx.options.value<CLI::no_iostream> ())
      {
        if (ctx.options.value<CLI::no_exceptions> ())
        {
          os << "struct writer: " << xs << "::writer"
             << "{"
             << "virtual bool" << endl
             << "write (const char* s, size_t n)"
             << "{"
             << "return fwrite (s, n, 1, stdout) == 1;"
             << "}"
             << "virtual bool" << endl
             << "flush ()"
             << "{"
             << "return fflush (stdout) == 0;"
             << "}"
             << "};";
        }
        else
        {
          os << "struct io_failure"
             << "{"
             << "};";

          os << "struct writer: " << xs << "::writer"
             << "{"
             << "virtual void" << endl
             << "write (const char* s, size_t n)"
             << "{"
             << "if (fwrite (s, n, 1, stdout) != 1)" << endl
             << "throw io_failure ();"
             << "}"
             << "virtual void" << endl
             << "flush ()"
             << "{"
             << "if (fflush (stdout) != 0)" << endl
             << "throw io_failure ();"
             << "}"
             << "};";
        }
      }

      os << "int" << endl
         << "main ()"
         << "{";

      if (!ctx.options.value<CLI::no_exceptions> ())
        os << "try"
           << "{";

      os << "// Instantiate individual serializers." << endl
         << "//" << endl;

      {
        SerializerDef def (ctx, map, set);
        def.dispatch (root_type);
      }

      os << endl
         << "// Connect the serializers together." << endl
         << "//" << endl;

      {
        // @@ I can simply iterate over the map instead of traversing
        // the tree all over again.
        //
        SerializerConnect connect (ctx, map);
        connect.dispatch (root_type);
      }

      String const& root_s (map[&root_type]);

      os << "// Serialize the XML document." << endl
         << "//" << endl;

      if (ctx.options.value<CLI::no_iostream> ())
        os << "writer w;";

      if (ctx.options.value<CLI::no_exceptions> ())
        os << xs << "::serializer_error e;"
           << endl
           << "do"
           << "{";

      if (root->namespace_().name ())
        os << xs << "::document_simpl doc_s (" << endl
           << root_s << "," << endl
           << ctx.strlit (root->namespace_().name ()) << "," << endl
           << ctx.strlit (root->name ()) << ");"
           << endl;
      else
        os << xs << "::document_simpl doc_s (" << root_s << ", " <<
          ctx.strlit (root->name ()) << ");"
           << endl;

      if (ctx.options.value<CLI::no_exceptions> ())
        os << "e = doc_s._error ();"
           << "if (e)" << endl
           << "break;"
           << endl;

      if (Context::arg_type (root->type ()) != L"void")
        os << "// TODO: pass the " << root->name () << " element data " <<
          "to pre()" << endl
           << "//" << endl;

      os << root_s << ".pre ();"
         << endl;

      if (ctx.options.value<CLI::no_exceptions> ())
        os << "e = " << root_s << "._error ();"
           << "if (e)" << endl
           << "break;"
           << endl;

      if (ctx.options.value<CLI::no_iostream> ())
        os << "doc_s.serialize (w);"
           << endl;
      else
        os << "doc_s.serialize (std::cout);"
           << endl;

      if (ctx.options.value<CLI::no_exceptions> ())
        os << "e = doc_s._error ();"
           << "if (e)" << endl
           << "break;"
           << endl;

      os << root_s << ".post ();";

      if (ctx.options.value<CLI::no_exceptions> ())
        os << endl
           << "e = " << root_s << "._error ();";

      if (ctx.options.value<CLI::no_exceptions> ())
        os << "}"
           << "while (false);"
           << endl;

      // Error handling.
      //

      if (ctx.options.value<CLI::no_exceptions> ())
      {
        os << "// Handle errors." << endl
           << "//" << endl
           << "if (e)"
           << "{"
           << "switch (e.type ())"
           << "{"
           << "case " << xs << "::serializer_error::sys:"
           << "{";

        if (ctx.options.value<CLI::no_iostream> ())
          os << "fprintf (stderr, \"%s\\n\", e.sys_text ());";
        else
          os << "std::cerr << e.sys_text () << std::endl;";

        os << "break;"
           << "}"
           << "case " << xs << "::serializer_error::xml:"
           << "{";

        if (ctx.options.value<CLI::no_iostream> ())
          os << "fprintf (stderr, \"%s\\n\", e.xml_text ());";
        else
          os << "std::cerr << e.xml_text () << std::endl;";

        os << "break;"
           << "}";

        if (!ctx.options.value<CLI::suppress_validation> ())
        {
          os << "case " << xs << "::serializer_error::schema:"
             << "{";

          if (ctx.options.value<CLI::no_iostream> ())
            os << "fprintf (stderr, \"%s\\n\", e.schema_text ());";
          else
            os << "std::cerr << e.schema_text () << std::endl;";

          os << "break;"
             << "}";
        }

        os << "case " << xs << "::serializer_error::app:"
           << "{";

        if (ctx.options.value<CLI::no_iostream> ())
          os << "fprintf (stderr, \"application error %d\\n\", e.app_code ());";
        else
          os << "std::cerr << \"application error \" << e.app_code () " <<
            "<< std::endl;";

        os << "break;"
           << "}"
           << "default:"
           << "{"
           << "break;"
           << "}"
           << "}" //switch
           << "return 1;"
           << "}"; // if (e)
      }
      else
      {
        os << "}" // try
           << "catch (const " << xs << "::serializer_exception& e)"
           << "{";

        if (ctx.options.value<CLI::no_iostream> ())
          os << "fprintf (stderr, \"error: %s\\n\", e.text ());";
        else
          os << "std::cerr << \"error: \" << e.text () << std::endl;";

        os << "return 1;"
           << "}";

        if (ctx.options.value<CLI::no_iostream> ())
          os << "catch (const io_failure&)"
             << "{"
             << "fprintf (stderr, \"error: write failure\\n\");"
             << "return 1;"
             << "}";
        else
          os << "catch (const std::ios_base::failure&)"
             << "{"
             << "std::cerr << \"error: write failure\" << std::endl;"
             << "return 1;"
             << "}";
      }

      os << "return 0;"
         << "}"; // main
    }
  }
}
