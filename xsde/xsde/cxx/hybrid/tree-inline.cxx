// file      : xsd/cxx/hybrid/tree-inline.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/hybrid/tree-inline.hxx>
#include <xsde/cxx/hybrid/default-value.hxx>

#include <libxsd-frontend/semantic-graph.hxx>
#include <libxsd-frontend/traversal.hxx>

namespace CXX
{
  namespace Hybrid
  {
    namespace
    {
      struct Enumeration : Traversal::Enumeration, Context
      {
        Enumeration (Context& c, Traversal::Complex& complex)
            : Context (c),
              complex_ (complex),
              base_name_ (c, TypeName::base)
        {
        }

        virtual void
        traverse (Type& e)
        {
          // First see if we should delegate this one to the Complex
          // generator.
          //
          Type* base_enum (0);

          if (!enum_ || !enum_mapping (e, &base_enum))
          {
            complex_.traverse (e);
            return;
          }

          String const& name (ename_custom (e));

          // We may not need to generate the class if this type is
          // being customized.
          //
          if (!name)
            return;

          SemanticGraph::Context& ec (e.context ());
          String const& vt (ec.get<String> ("value-type"));

          os << "// " << comment (e.name ()) << endl
             << "//" << endl
             << endl;

          // c-tors
          //
          os << inl
             << name << "::" << endl
             << name << " ()"
             << "{"
             << "}";

          os << inl
             << name << "::" << endl
             << name << " (" << vt << " v)"
             << "{";

          if (base_enum)
          {
            os << "this->" << base_enum->context ().get<String> ("value") <<
              " (v);";
          }
          else
            os << ec.get<String> ("value-member") << " = v;";

          os << "}";

          // value (value_type)
          //
          if (!base_enum)
          {
            os << inl
               << "void " << name << "::" << endl
               << ec.get<String> ("value") << " (" << vt << " v)"
               << "{"
               << ec.get<String> ("value-member") << " = v;"
               << "}";
          }

          // Custom data.
          //
          if (ec.count ("cd-name"))
          {
            String const& cd_name (ecd_name (e));
            String const& member (ecd_member (e));
            String const& sequence (ecd_sequence (e));

            // const seq&
            // name () const
            //
            os << inl
               << "const " << name << "::" << sequence << "& " <<
              name << "::" << endl
               << cd_name << " () const"
               << "{"
               << "return this->" << member << ";"
               << "}";

            // seq&
            // name ()
            //
            os << inl
               << name << "::" << sequence << "& " << name << "::" << endl
               << cd_name << " ()"
               << "{"
               << "return this->" << member << ";"
               << "}";
          }
        }

      private:
        Traversal::Complex& complex_;
        TypeName base_name_;
      };

      struct List : Traversal::List, Context
      {
        List (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& l)
        {
          String const& name (ename_custom (l));

          // We may not need to generate the class if this type is
          // being customized.
          //
          if (!name)
            return;

          os << "// " << comment (l.name ()) << endl
             << "//" << endl
             << endl;

          // c-tor
          //
          os << inl
             << name << "::" << endl
             << name << " ()"
             << "{"
             << "}";

          // Custom data.
          //
          if (l.context ().count ("cd-name"))
          {
            String const& cd_name (ecd_name (l));
            String const& member (ecd_member (l));
            String const& sequence (ecd_sequence (l));

            // const seq&
            // name () const
            //
            os << inl
               << "const " << name << "::" << sequence << "& " <<
              name << "::" << endl
               << cd_name << " () const"
               << "{"
               << "return this->" << member << ";"
               << "}";

            // seq&
            // name ()
            //
            os << inl
               << name << "::" << sequence << "& " << name << "::" << endl
               << cd_name << " ()"
               << "{"
               << "return this->" << member << ";"
               << "}";
          }
        }
      };

      //
      //
      struct Union : Traversal::Union, Context
      {
        Union (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (Type& u)
        {
          String const& name (ename_custom (u));

          // We may not need to generate the class if this type is
          // being customized.
          //
          if (!name)
            return;

          os << "// " << comment (u.name ()) << endl
             << "//" << endl
             << endl;

          SemanticGraph::Context& uc (u.context ());
          String const& value (uc.get<String> ("value"));
          String const& member (uc.get<String> ("value-member"));

          // c-tor
          //
          os << inl
             << name << "::" << endl
             << name << " ()"
             << "{";

          if (!stl)
            os << "this->" << member << " = 0;";

          os << "}";

          if (stl)
          {
            // const std::string&
            // name () const
            //
            os << inl
               << "const ::std::string& " << name << "::" << endl
               << value << " () const"
               << "{"
               << "return this->" << member << ";"
               << "}";

            // std::string&
            // name ()
            //
            os << inl
               << "::std::string& " << name << "::" << endl
               << value << " ()"
               << "{"
               << "return this->" << member << ";"
               << "}";

            // void
            // name (const std::string&)
            //
            os << inl
               << "void " << name << "::" << endl
               << value << " (const ::std::string& x)"
               << "{"
               << "this->" << member << " = x;"
               << "}";
          }
          else
          {
            String p (L"this->" + member);

            // const char*
            // name () const
            //
            os << inl
               << "const char* " << name << "::" << endl
               << value << " () const"
               << "{"
               << "return " << p << ";"
               << "}";

            // char*
            // name ()
            //
            os << inl
               << "char* " << name << "::" << endl
               << value << " ()"
               << "{"
               << "return " << p << ";"
               << "}";

            // void
            // name (char*)
            //
            os << inl
               << "void " << name << "::" << endl
               << value << " (char* x)"
               << "{";

            if (!custom_alloc)
              os << "delete[] " << p << ";";
            else
              os << "::xsde::cxx::free (" << p << ");";

            os << p << " = x;"
               << "}";

            // char*
            // detach ()
            //
            if (detach)
            {
              os << inl
                 << "char* " << name << "::" << endl
                 << uc.get<String> ("value-detach") << " ()"
                 << "{"
                 << "char* r = " << p << ";"
                 << p << " = 0;"
                 << "return r;"
                 << "}";
            }
          }

          // Custom data.
          //
          if (uc.count ("cd-name"))
          {
            String const& cd_name (ecd_name (u));
            String const& member (ecd_member (u));
            String const& sequence (ecd_sequence (u));

            // const seq&
            // name () const
            //
            os << inl
               << "const " << name << "::" << sequence << "& " <<
              name << "::" << endl
               << cd_name << " () const"
               << "{"
               << "return this->" << member << ";"
               << "}";

            // seq&
            // name ()
            //
            os << inl
               << name << "::" << sequence << "& " << name << "::" << endl
               << cd_name << " ()"
               << "{"
               << "return this->" << member << ";"
               << "}";
          }
        }
      };

      //
      //
      struct AttributeFunc: Traversal::Attribute, Context
      {
        AttributeFunc (Context& c)
            : Context (c),
              ro_ret_ (c, TypeName::ro_ret),
              ret_ (c, TypeName::ret),
              arg_ (c, TypeName::arg),
              deref_ (c),
              delete_ (c),
              compare_value_ (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Attribute& a)
        {
          bool def (a.default_p ());
          bool fix (a.fixed_p ());

          String const& name (ename (a));
          String member;

          if (!fix)
            member = emember (a);

          SemanticGraph::Type& t (a.type ());
          bool fl (fixed_length (t));
          String scope (Context::scope (a));

          if (a.optional_p () && !fix)
          {
            String const& name (def ? edefault (a) : epresent (a));

            // bool
            // preset () const;
            //
            os << inl
               << "bool " << scope << "::" << endl
               << name << " () const"
               << "{";

            if (def)
            {
              if (fl)
              {
                os << "return ";
                compare_value_.dispatch (
                  t, L"this->" + member, edefault_value (a) + L" ()");
                os << ";";
              }
              else
                os << "return this->" << member << " == 0;";
            }
            else
            {
              if (fl)
                os << "return this->" << epresent_member (a) << ";";
              else
                os << "return this->" << member << " != 0;";
            }

            os << "}";

            // void
            // preset (bool);
            //
            if (fl)
            {
              os << inl
                 << "void " << scope << "::" << endl
                 << name << " (bool x)"
                 << "{";

              if (def)
              {
                os << "if (x)" << endl
                   << "this->" << member << " = " << edefault_value (a) << " ();";
              }
              else
              {
                os << "this->" << epresent_member (a) << " = x;";
              }

              os << "}";
            }
          }

          // const type&
          // name () const
          //
          os << inl;
          ro_ret_.dispatch (t);
          os << " " << scope << "::" << endl
             << name << " () const"
             << "{";

          if (fix)
          {
            os << "return " << edefault_value (a) << " ();";
          }
          else if (def)
          {
            if (fl)
              os << "return this->" << member << ";";
            else
            {
              os << "return this->" << member << " ? ";
              deref_.dispatch (t);
              os << "this->" << member << " : " <<
                edefault_value (a) << " ();";
            }
          }
          else
          {
            os << "return ";
            deref_.dispatch (t);
            os << "this->" << member << ";";
          }

          os << "}";

          // Do not generate modifiers for fixed attributes.
          //
          if (!fix)
          {
            // type&
            // name ()
            //
            os << inl;
            ret_.dispatch (t);
            os << " " << scope << "::" << endl
               << name << " ()"
               << "{";

            if (def)
            {
              if (fl)
                os << "return this->" << member << ";";
              else
              {
                os << "return this->" << member << " ? ";
                deref_.dispatch (t);
                os << "this->" << member << " : const_cast< ";
                ret_.dispatch (t);
                os << " > (" << edefault_value (a) << " ());";
              }
            }
            else
            {
              os << "return ";
              deref_.dispatch (t);
              os << "this->" << member << ";";
            }

            os << "}";


            // void
            // name (const type& | type*)
            //
            os << inl
               << "void " << scope << "::" << endl
               << name << " (";
            arg_.dispatch (t);
            os << " x)"
               << "{";

            if (!fl)
              delete_.dispatch (t, L"this->" + member);

            os << "this->" << member << " = x;";

            if (fl && !def && a.optional_p ())
              os << "this->" << epresent_member (a) << " = true;";

            os << "}";


            // type*
            // detach ()
            //
            if (detach && !fl)
            {
              os << inl;
              arg_.dispatch (t);
              os << " " << scope << "::" << endl
                 << edetach (a) << " ()"
                 << "{";
              arg_.dispatch (t);
              os << " r = this->" << member << ";"
                 << "this->" << member << " = 0;"
                 << "return r;"
                 << "}";
            }
          }
        }

      private:
        TypeName ro_ret_;
        TypeName ret_;
        TypeName arg_;
        TypeDeref deref_;
        TypeDelete delete_;
        CompareValue compare_value_;
      };

      struct ElementFunc: Traversal::Element, Context
      {
        ElementFunc (Context& c)
            : Context (c),
              ro_ret_ (c, TypeName::ro_ret),
              ret_ (c, TypeName::ret),
              arg_ (c, TypeName::arg),
              deref_ (c),
              delete_ (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          String const& name (ename (e));
          String const& member (emember (e));

          SemanticGraph::Type& t (e.type ());
          bool fl (fixed_length (t));
          String scope (Context::scope (e));

          if (e.max () != 1)
          {
            String const& sequence (esequence (e));

            // const seq&
            // name () const
            //
            os << inl
               << "const " << scope << "::" << sequence << "& " <<
              scope << "::" << endl
               << name << " () const"
               << "{"
               << "return this->" << member << ";"
               << "}";

            // seq&
            // name ()
            //
            os << inl
               << scope << "::" << sequence << "& " << scope << "::" << endl
               << name << " ()"
               << "{"
               << "return this->" << member << ";"
               << "}";
          }
          else
          {
            if (e.min () == 0)
            {
              String const& present (epresent (e));

              // bool
              // preset () const;
              //
              os << inl
                 << "bool " << scope << "::" << endl
                 << present << " () const"
                 << "{";

              if (fl)
                os << "return this->" << epresent_member (e) << ";";
              else
                os << "return this->" << member << " != 0;";

              os << "}";

              // void
              // preset (bool);
              //
              if (fl)
                os << inl
                   << "void " << scope << "::" << endl
                   << present << " (bool x)"
                   << "{"
                   << "this->" << epresent_member (e) << " = x;"
                   << "}";
            }

            // const type&
            // name () const
            //
            os << inl;
            ro_ret_.dispatch (t);
            os << " " << scope << "::" << endl
               << name << " () const"
               << "{"
               << "return ";
            deref_.dispatch (t);
            os << "this->" << member << ";"
               << "}";

            // type&
            // name ()
            //
            os << inl;
            ret_.dispatch (t);
            os << " " << scope << "::" << endl
               << name << " ()"
               << "{"
               << "return ";
            deref_.dispatch (t);
            os << "this->" << member << ";"
               << "}";

            // void
            // name (const type& | type*)
            //
            os << inl
               << "void " << scope << "::" << endl
               << name << " (";
            arg_.dispatch (t);
            os << " x)"
               << "{";

            if (!fl)
              delete_.dispatch (t, L"this->" + member);

            os << "this->" << member << " = x;";

            if (fl && e.min () == 0)
              os << "this->" << epresent_member (e) << " = true;";

            os << "}";

            // type*
            // detach ()
            //
            if (detach && !fl)
            {
              os << inl;
              arg_.dispatch (t);
              os << " " << scope << "::" << endl
                 << edetach (e) << " ()"
                 << "{";
              arg_.dispatch (t);
              os << " r = this->" << member << ";"
                 << "this->" << member << " = 0;"
                 << "return r;"
                 << "}";
            }
          }
        }

      private:
        TypeName ro_ret_;
        TypeName ret_;
        TypeName arg_;
        TypeDeref deref_;
        TypeDelete delete_;
      };

      struct ElementInChoiceFunc: Traversal::Element, Context
      {
        ElementInChoiceFunc (Context& c)
            : Context (c),
              ro_ret_ (c, TypeName::ro_ret),
              ret_ (c, TypeName::ret),
              arg_ (c, TypeName::arg),
              var_ (c, TypeName::var),
              deref_ (c),
              delete_ (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Element& e)
        {
          String const& name (ename (e));
          String const& member (emember (e));

          SemanticGraph::Choice& c (
            dynamic_cast<SemanticGraph::Choice&> (
              e.contained_particle ().compositor ()));

          String const& arm (earm (c));
          String const& umember (emember (c));

          String scope (Context::scope (e));

          if (e.max () != 1)
          {
            String const& sequence (esequence (e));

            // const seq&
            // name () const
            //
            os << inl
               << "const " << scope << "::" << sequence << "& " <<
              scope << "::" << endl
               << name << " () const"
               << "{"
               << "return reinterpret_cast< const " << sequence << "& > (" <<
              "this->" << umember << "." << member << ");"
               << "}";

            // seq&
            // name ()
            //
            os << inl
               << scope << "::" << sequence << "& " << scope << "::" << endl
               << name << " ()"
               << "{"
               << "return reinterpret_cast< " << sequence << "& > (" <<
              "this->" << umember << "." << member << ");"
               << "}";
          }
          else
          {
            size_t min (e.min ());

            SemanticGraph::Type& t (e.type ());
            bool fl (fixed_length (t));

            String const& tag (etag (e));
            String const& arm_member (earm_member (c));

            if (min == 0)
            {
              String const& present (epresent (e));

              // bool
              // preset () const;
              //
              os << inl
                 << "bool " << scope << "::" << endl
                 << present << " () const"
                 << "{";

              if (fl)
              {
                os << "return this->" << umember << "." << member <<
                  ".data_[sizeof (";
                var_.dispatch (t);
                os << ")];";
              }
              else
                os << "return this->" << umember << "." << member << " != 0;";

              os << "}";

              // void
              // preset (bool);
              //
              if (fl)
              {
                os << inl
                   << "void " << scope << "::" << endl
                   << present << " (bool x)"
                   << "{"
                   << "if (this->" << arm_member << " != " << tag << ")" << endl
                   << "this->" << arm << " (" << tag << ");"
                   << endl
                   << "this->" << umember << "." << member << ".data_[sizeof (";
                var_.dispatch (t);
                os << ")] = x;"
                   << "}";
              }
            }

            // const type&
            // name () const
            //
            os << inl;
            ro_ret_.dispatch (t);
            os << " " << scope << "::" << endl
               << name << " () const"
               << "{"
               << "return ";

            if (fl)
            {
              os << "reinterpret_cast< const ";
              var_.dispatch (t);
              os << "& > (this->" << umember << "." << member << ");";
            }
            else
            {
              deref_.dispatch (t);
              os << "this->" << umember << "." << member << ";";
            }

            os << "}";

            // type&
            // name ()
            //
            os << inl;
            ret_.dispatch (t);
            os << " " << scope << "::" << endl
               << name << " ()"
               << "{"
               << "return ";

            if (fl)
            {
              os << "reinterpret_cast< ";
              var_.dispatch (t);
              os << "& > (this->" << umember << "." << member << ");";
            }
            else
            {
              deref_.dispatch (t);
              os << "this->" << umember << "." << member << ";";
            }

            os << "}";

            // void
            // name (const type& | type*)
            //
            os << inl
               << "void " << scope << "::" << endl
               << name << " (";
            arg_.dispatch (t);
            os << " x)"
               << "{"
               << "if (this->" << arm_member << " != " << tag << ")" << endl
               << "this->" << arm << " (" << tag << ");";

            if (fl)
            {
              os << endl
                 << "reinterpret_cast< ";
              var_.dispatch (t);
              os << "& > (this->" << umember << "." << member << ") = x;";

              if (min == 0)
              {
                os << "this->" << umember << "." << member <<
                  ".data_[sizeof (";
                var_.dispatch (t);
                os << ")] = true;";
              }
            }
            else
            {
              os << "else"
                 << "{";
              delete_.dispatch (t, L"this->" + umember + L"." + member);
              os << "}"
                 << "this->" << umember << "." << member << " = x;";
            }

            os << "}";

            // type*
            // detach ()
            //
            if (detach && !fl)
            {
              os << inl;
              arg_.dispatch (t);
              os << " " << scope << "::" << endl
                 << edetach (e) << " ()"
                 << "{";
              arg_.dispatch (t);
              os << " r = this->" << umember << "." << member << ";"
                 << "this->" << umember << "." << member << " = 0;"
                 << "return r;"
                 << "}";
            }
          }
        }

      private:
        TypeName ro_ret_;
        TypeName ret_;
        TypeName arg_;
        TypeName var_;
        TypeDeref deref_;
        TypeDelete delete_;
      };

      struct AllFunc: Traversal::All, Context
      {
        AllFunc (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}
          // and it can only contain particles.
          //
          if (a.min () == 0)
          {
            String const& name (ename (a));
            String const& type (etype (a));
            String const& present (epresent (a));
            String const& member (emember (a));

            bool fl (fixed_length (a));
            String scope (Context::scope (a));


            // bool
            // preset () const;
            //
            os << inl
               << "bool " << scope << "::" << endl
               << present << " () const"
               << "{";

            if (fl)
              os << "return this->" << epresent_member (a) << ";";
            else
              os << "return this->" << member << " != 0;";

            os << "}";


            // void
            // present (bool);
            //
            if (fl)
              os << inl
                 << "void " << scope << "::" << endl
                 << present << " (bool x)"
                 << "{"
                 << "this->" << epresent_member (a) << " = x;"
                 << "}";


            // const type&
            // name () const
            //
            os << inl
               << "const " << scope << "::" << type << "& " <<
              scope << "::" << endl
               << name << " () const"
               << "{";

            if (fl)
              os << "return this->" << member << ";";
            else
              os << "return *this->" << member << ";";

            os << "}";


            // type&
            // name ()
            //
            os << inl
               << scope << "::" << type << "& " << scope << "::" << endl
               << name << " ()"
               << "{";

            if (fl)
              os << "return this->" << member << ";";
            else
              os << "return *this->" << member << ";";

            os << "}";


            // void
            // name (const type& | type*)
            //
            os << inl
               << "void " << scope << "::" << endl
               << name << " (";

            if (fl)
              os << "const " << type << "&";
            else
              os << type << "*";

            os << " x)"
               << "{";

            if (!fl)
            {
              String p (L"this->" + member);

              if (!custom_alloc)
                os << "delete " << p << ";";
              else
                os << "if (" << p << ")"
                   << "{"
                   << p << "->~" << type << " ();"
                   << "::xsde::cxx::free (" << p << ");"
                   << "}";
            }

            os << "this->" << member << " = x;";

            if (fl)
              os << "this->" << epresent_member (a) << " = true;";

            os << "}";

            // type*
            // detach ()
            //
            if (detach && !fl)
            {
              os << inl
                 << scope << "::" << type << "* " << scope << "::" << endl
                 << edetach (a) << " ()"
                 << "{"
                 << type << "* r = this->" << member << ";"
                 << "this->" << member << " = 0;"
                 << "return r;"
                 << "}";
            }
          }
          else
            All::contains (a);
        }
      };

      struct ChoiceInSequenceFunc: Traversal::Choice, Context
      {
        ChoiceInSequenceFunc (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          size_t min (c.min ()), max (c.max ());

          if (max == 1 && min == 1)
          {
            String scope (Context::scope (c));

            // void
            // arm (arm_tag)
            //
            os << inl
               << scope << "::" << earm_tag (c) << " " << scope << "::" << endl
               << earm (c) << " () const"
               << "{"
               << "return this->" << earm_member (c) << ";"
               << "}";

            Choice::contains (c);
            return;
          }

          String const& name (ename (c));
          String const& member (emember (c));
          String scope (Context::scope (c));

          if (max != 1)
          {
            String const& sequence (esequence (c));

            // const seq&
            // name () const
            //
            os << inl
               << "const " << scope << "::" << sequence << "& " <<
              scope << "::" << endl
               << name << " () const"
               << "{"
               << "return this->" << member << ";"
               << "}";

            // seq&
            // name ()
            //
            os << inl
               << scope << "::" << sequence << "& " << scope << "::" << endl
               << name << " ()"
               << "{"
               << "return this->" << member << ";"
               << "}";
          }
          else if (min == 0)
          {
            bool fl (fixed_length (c));

            String const& type (etype (c));
            String const& present (epresent (c));

            // bool
            // preset () const;
            //
            os << inl
               << "bool " << scope << "::" << endl
               << present << " () const"
               << "{";

            if (fl)
              os << "return this->" << epresent_member (c) << ";";
            else
              os << "return this->" << member << " != 0;";

            os << "}";


            // void
            // preset (bool);
            //
            if (fl)
              os << inl
                 << "void " << scope << "::" << endl
                 << present << " (bool x)"
                 << "{"
                 << "this->" << epresent_member (c) << " = x;"
                 << "}";


            // const type&
            // name () const
            //
            os << inl
               << "const " << scope << "::" << type << "& " <<
              scope << "::" << endl
               << name << " () const"
               << "{";

            if (fl)
              os << "return this->" << member << ";";
            else
              os << "return *this->" << member << ";";

            os << "}";


            // type&
            // name ()
            //
            os << inl
               << scope << "::" << type << "& " << scope << "::" << endl
               << name << " ()"
               << "{";

            if (fl)
              os << "return this->" << member << ";";
            else
              os << "return *this->" << member << ";";

            os << "}";


            // void
            // name (const type& | type*)
            //
            os << inl
               << "void " << scope << "::" << endl
               << name << " (";

            if (fl)
              os << "const " << type << "&";
            else
              os << type << "*";

            os << " x)"
               << "{";

            if (!fl)
            {
              String p (L"this->" + member);

              if (!custom_alloc)
                os << "delete " << p << ";";
              else
                os << "if (" << p << ")"
                   << "{"
                   << p << "->~" << type << " ();"
                   << "::xsde::cxx::free (" << p << ");"
                   << "}";
            }

            os << "this->" << member << " = x;";

            if (fl)
              os << "this->" << epresent_member (c) << " = true;";

            os << "}";

            // type*
            // detach ()
            //
            if (detach && !fl)
            {
              os << inl
                 << scope << "::" << type << "* " << scope << "::" << endl
                 << edetach (c) << " ()"
                 << "{"
                 << type << "* r = this->" << member << ";"
                 << "this->" << member << " = 0;"
                 << "return r;"
                 << "}";
            }
          }
        }
      };

      struct SequenceInSequenceFunc: Traversal::Sequence, Context
      {
        SequenceInSequenceFunc (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          size_t min (s.min ()), max (s.max ());

          if (max == 1 && min == 1)
          {
            Sequence::contains (s);
            return;
          }

          String const& name (ename (s));
          String const& member (emember (s));
          String scope (Context::scope (s));

          if (max != 1)
          {
            String const& sequence (esequence (s));

            // const seq&
            // name () const
            //
            os << inl
               << "const " << scope << "::" << sequence << "& " <<
              scope << "::" << endl
               << name << " () const"
               << "{"
               << "return this->" << member << ";"
               << "}";

            // seq&
            // name ()
            //
            os << inl
               << scope << "::" << sequence << "& " << scope << "::" << endl
               << name << " ()"
               << "{"
               << "return this->" << member << ";"
               << "}";
          }
          else if (min == 0)
          {
            bool fl (fixed_length (s));

            String const& type (etype (s));
            String const& present (epresent (s));

            // bool
            // preset () const;
            //
            os << inl
               << "bool " << scope << "::" << endl
               << present << " () const"
               << "{";

            if (fl)
              os << "return this->" << epresent_member (s) << ";";
            else
              os << "return this->" << member << " != 0;";

            os << "}";


            // void
            // preset (bool);
            //
            if (fl)
              os << inl
                 << "void " << scope << "::" << endl
                 << present << " (bool x)"
                 << "{"
                 << "this->" << epresent_member (s) << " = x;"
                 << "}";


            // const type&
            // name () const
            //
            os << inl
               << "const " << scope << "::" << type << "& " <<
              scope << "::" << endl
               << name << " () const"
               << "{";

            if (fl)
              os << "return this->" << member << ";";
            else
              os << "return *this->" << member << ";";

            os << "}";


            // type&
            // name ()
            //
            os << inl
               << scope << "::" << type << "& " << scope << "::" << endl
               << name << " ()"
               << "{";

            if (fl)
              os << "return this->" << member << ";";
            else
              os << "return *this->" << member << ";";

            os << "}";


            // void
            // name (const type& | type*)
            //
            os << inl
               << "void " << scope << "::" << endl
               << name << " (";

            if (fl)
              os << "const " << type << "&";
            else
              os << type << "*";

            os << " x)"
               << "{";

            if (!fl)
            {
              String p (L"this->" + member);

              if (!custom_alloc)
                os << "delete " << p << ";";
              else
                os << "if (" << p << ")"
                   << "{"
                   << p << "->~" << type << " ();"
                   << "::xsde::cxx::free (" << p << ");"
                   << "}";
            }

            os << "this->" << member << " = x;";

            if (fl)
              os << "this->" << epresent_member (s) << " = true;";

            os << "}";

            // type*
            // detach ()
            //
            if (detach && !fl)
            {
              os << inl
                 << scope << "::" << type << "* " << scope << "::" << endl
                 << edetach (s) << " ()"
                 << "{"
                 << type << "* r = this->" << member << ";"
                 << "this->" << member << " = 0;"
                 << "return r;"
                 << "}";
            }
          }
        }
      };

      struct CompositorInChoiceFunc: Context
      {
        CompositorInChoiceFunc (Context& c)
            : Context (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Compositor& c)
        {
          // When compositor is in choice we generate nested class even
          // for min == max == 1.
          //
          String const& name (ename (c));
          String const& member (emember (c));

          SemanticGraph::Choice& cont (
            dynamic_cast<SemanticGraph::Choice&> (
              c.contained_particle ().compositor ()));

          String const& arm (earm (cont));
          String const& umember (emember (cont));

          String scope (Context::scope (c));

          if (c.max () != 1)
          {
            String const& sequence (esequence (c));

            // const seq&
            // name () const
            //
            os << inl
               << "const " << scope << "::" << sequence << "& " <<
              scope << "::" << endl
               << name << " () const"
               << "{"
               << "return reinterpret_cast< const " << sequence << "& > (" <<
              "this->" << umember << "." << member << ");"
               << "}";

            // seq&
            // name ()
            //
            os << inl
               << scope << "::" << sequence << "& " << scope << "::" << endl
               << name << " ()"
               << "{"
               << "return reinterpret_cast< " << sequence << "& > (" <<
              "this->" << umember << "." << member << ");"
               << "}";
          }
          else
          {
            size_t min (c.min ());

            bool fl (fixed_length (c));

            String const& type (etype (c));
            String const& tag (etag (c));
            String const& arm_member (earm_member (cont));

            if (min == 0)
            {
              String const& present (epresent (c));

              // bool
              // preset () const;
              //
              os << inl
                 << "bool " << scope << "::" << endl
                 << present << " () const"
                 << "{";

              if (fl)
              {
                os << "return this->" << umember << "." << member <<
                  ".data_[sizeof (" << type << ")];";
              }
              else
                os << "return this->" << umember << "." << member << " != 0;";

              os << "}";

              // void
              // preset (bool);
              //
              if (fl)
                os << inl
                   << "void " << scope << "::" << endl
                   << present << " (bool x)"
                   << "{"
                   << "if (this->" << arm_member << " != " << tag << ")" << endl
                   << "this->" << arm << " (" << tag << ");"
                   << endl
                   << "this->" << umember << "." << member <<
                  ".data_[sizeof (" << type << ")] = x;"
                   << "}";
            }

            // const type&
            // name () const
            //
            os << inl
               << "const " << scope << "::" << type << "& " <<
              scope << "::" << endl
               << name << " () const"
               << "{"
               << "return ";

            if (fl)
              os << "reinterpret_cast< const " << type << "& > (this->" <<
                umember << "." << member << ");";
            else
              os << "*this->" << umember << "." << member << ";";

            os << "}";

            // type&
            // name ()
            //
            os << inl
               << scope << "::" << type << "& " << scope << "::" << endl
               << name << " ()"
               << "{"
               << "return ";

            if (fl)
              os << "reinterpret_cast< " << type << "& > (this->" <<
                umember << "." << member << ");";
            else
              os << "*this->" << umember << "." << member << ";";

            os << "}";

            // void
            // name (const type& | type*)
            //
            os << inl
               << "void " << scope << "::" << endl
               << name << " (";

            if (fl)
              os << "const " << type << "&";
            else
              os << type << "*";

            os << " x)"
               << "{"
               << "if (this->" << arm_member << " != " << tag << ")" << endl
               << "this->" << arm << " (" << tag << ");";

            if (fl)
            {
              os << endl
                 << "reinterpret_cast< " << type << "& > (this->" <<
                umember << "." << member << ") = x;";

              if (min == 0)
              {
                os << "this->" << umember << "." << member <<
                  ".data_[sizeof (" << type << ")] = true;";
              }
            }
            else
            {
              String p (L"this->" + umember + L"." + member);

              os << "else";

              if (!custom_alloc)
                os << endl
                   << "delete " << p << ";"
                   << endl;
              else
                os << " if (" << p << ")"
                   << "{"
                   << p << "->~" << type << " ();"
                   << "::xsde::cxx::free (" << p << ");"
                   << "}";

              os << p << " = x;";
            }

            os << "}";


            // type*
            // detach ()
            //
            if (detach && !fl)
            {
              os << inl
                 << scope << "::" << type << "* " << scope << "::" << endl
                 << edetach (c) << " ()"
                 << "{"
                 << type << "* r = this->" << umember << "." << member << ";"
                 << "this->" << umember << "." << member << " = 0;"
                 << "return r;"
                 << "}";
            }
          }
        }
      };

      struct ChoiceInChoiceFunc: Traversal::Choice, CompositorInChoiceFunc
      {
        using CompositorInChoiceFunc::traverse;

        ChoiceInChoiceFunc (Context& c)
            : CompositorInChoiceFunc (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          CompositorInChoiceFunc::traverse (c);
        }
      };

      struct SequenceInChoiceFunc: Traversal::Sequence, CompositorInChoiceFunc
      {
        using CompositorInChoiceFunc::traverse;

        SequenceInChoiceFunc (Context& c)
            : CompositorInChoiceFunc (c)
        {
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          CompositorInChoiceFunc::traverse (s);
        }
      };

      //
      //
      //

      struct All: Traversal::All, Context
      {
        All (Context& c, Traversal::ContainsParticle& contains_func)
            : Context (c), contains_func_ (contains_func)
        {
        }

        virtual void
        traverse (SemanticGraph::All& a)
        {
          // For the all compositor, maxOccurs=1 and minOccurs={0,1}
          // and it can only contain particles.
          //
          if (a.min () == 0)
          {
            All::contains (a, contains_func_);

            // Custom data.
            //
            if (a.context ().count ("cd-name"))
            {
              String const& scope (Context::scope (a));
              String const& type (etype (a));

              String const& name (ecd_name (a));
              String const& member (ecd_member (a));
              String const& sequence (ecd_sequence (a));

              // const seq&
              // name () const
              //
              os << inl
                 << "const " << scope << "::" << type << "::" <<
                sequence << "& " << scope << "::" << type << "::" << endl
                 << name << " () const"
                 << "{"
                 << "return this->" << member << ";"
                 << "}";

              // seq&
              // name ()
              //
              os << inl
                 << scope << "::" << type << "::" << sequence << "& " <<
                scope << "::" << type << "::" << endl
                 << name << " ()"
                 << "{"
                 << "return this->" << member << ";"
                 << "}";
            }
          }
        }

      private:
        Traversal::ContainsParticle& contains_func_;
      };

      struct Choice: Traversal::Choice, Context
      {
        Choice (Context& c,
                bool in_choice,
                Traversal::ContainsParticle& contains_func)
            : Context (c),
              in_choice_ (in_choice),
              contains_func_ (contains_func)
        {
        }

        virtual void
        traverse (SemanticGraph::Choice& c)
        {
          // When choice is in choice we generate nested class even
          // for min == max == 1.
          //
          if (in_choice_ || c.max () != 1 || c.min () == 0)
          {
            String scope (Context::scope (c));
            String const& type (etype (c));

            // void
            // arm (arm_tag)
            //
            os << inl
               << scope << "::" << type << "::" << earm_tag (c) << " " <<
              scope << "::" << type << "::" << endl
               << earm (c) << " () const"
               << "{"
               << "return this->" << earm_member (c) << ";"
               << "}";

            Choice::contains (c, contains_func_);

            // Custom data.
            //
            if (c.context ().count ("cd-name"))
            {
              String const& name (ecd_name (c));
              String const& member (ecd_member (c));
              String const& sequence (ecd_sequence (c));

              // const seq&
              // name () const
              //
              os << inl
                 << "const " << scope << "::" << type << "::" <<
                sequence << "& " << scope << "::" << type << "::" << endl
                 << name << " () const"
                 << "{"
                 << "return this->" << member << ";"
                 << "}";

              // seq&
              // name ()
              //
              os << inl
                 << scope << "::" << type << "::" << sequence << "& " <<
                scope << "::" << type << "::" << endl
                 << name << " ()"
                 << "{"
                 << "return this->" << member << ";"
                 << "}";
            }
          }

          Choice::contains (c);
        }

      private:
        bool in_choice_;
        Traversal::ContainsParticle& contains_func_;
      };


      struct SequenceInSequence: Traversal::Sequence, Context
      {
        SequenceInSequence (Context& c,
                            Traversal::ContainsParticle& contains_func)
            : Context (c), contains_func_ (contains_func)
        {
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          if (s.max () != 1 || s.min () == 0)
          {
            Sequence::contains (s, contains_func_);

            // Custom data.
            //
            if (s.context ().count ("cd-name"))
            {
              String const& scope (Context::scope (s));
              String const& type (etype (s));

              String const& name (ecd_name (s));
              String const& member (ecd_member (s));
              String const& sequence (ecd_sequence (s));

              // const seq&
              // name () const
              //
              os << inl
                 << "const " << scope << "::" << type << "::" <<
                sequence << "& " << scope << "::" << type << "::" << endl
                 << name << " () const"
                 << "{"
                 << "return this->" << member << ";"
                 << "}";

              // seq&
              // name ()
              //
              os << inl
                 << scope << "::" << type << "::" << sequence << "& " <<
                scope << "::" << type << "::" << endl
                 << name << " ()"
                 << "{"
                 << "return this->" << member << ";"
                 << "}";
            }
          }

          Sequence::contains (s);
        }

      private:
        Traversal::ContainsParticle& contains_func_;
      };

      struct SequenceInChoice: Traversal::Sequence, Context
      {
        SequenceInChoice (Context& c,
                          Traversal::ContainsParticle& contains_func)
            : Context (c), contains_func_ (contains_func)
        {
        }

        virtual void
        traverse (SemanticGraph::Sequence& s)
        {
          // When sequence is in choice we generate nested class even
          // for min == max == 1.
          //
          Sequence::contains (s, contains_func_);

          // Custom data.
          //
          if (s.context ().count ("cd-name"))
          {
            String const& scope (Context::scope (s));
            String const& type (etype (s));

            String const& name (ecd_name (s));
            String const& member (ecd_member (s));
            String const& sequence (ecd_sequence (s));

            // const seq&
            // name () const
            //
            os << inl
               << "const " << scope << "::" << type << "::" <<
              sequence << "& " << scope << "::" << type << "::" << endl
               << name << " () const"
               << "{"
               << "return this->" << member << ";"
               << "}";

            // seq&
            // name ()
            //
            os << inl
               << scope << "::" << type << "::" << sequence << "& " <<
              scope << "::" << type << "::" << endl
               << name << " ()"
               << "{"
               << "return this->" << member << ";"
               << "}";
          }

          Sequence::contains (s);
        }

      private:
        Traversal::ContainsParticle& contains_func_;
      };

      struct Complex : Traversal::Complex, Context
      {
        Complex (Context& c)
            : Context (c),

              // Functions.
              //
              attribute_func_ (c),
              element_func_ (c),
              element_in_choice_func_ (c),
              all_func_ (c),
              choice_in_choice_func_ (c),
              choice_in_sequence_func_ (c),
              sequence_in_choice_func_ (c),
              sequence_in_sequence_func_ (c),

              // Nested classes.
              //
              all_ (c, all_contains_func_),
              choice_in_choice_ (c, true, choice_contains_func_),
              choice_in_sequence_ (c, false, choice_contains_func_),
              sequence_in_choice_ (c, sequence_contains_func_),
              sequence_in_sequence_ (c, sequence_contains_func_)
        {
          // Functions.
          //
          attribute_names_func_ >> attribute_func_;

          all_func_ >> all_contains_func_ >> element_func_;

          sequence_in_choice_func_ >> sequence_contains_func_;
          sequence_in_sequence_func_ >> sequence_contains_func_;
          sequence_contains_func_ >> element_func_;
          sequence_contains_func_ >> choice_in_sequence_func_;
          sequence_contains_func_ >> sequence_in_sequence_func_;

          choice_in_choice_func_ >> choice_contains_func_;
          choice_in_sequence_func_ >> choice_contains_func_;
          choice_contains_func_ >> element_in_choice_func_;
          choice_contains_func_ >> sequence_in_choice_func_;
          choice_contains_func_ >> choice_in_choice_func_;

          contains_compositor_func_ >> all_func_;
          contains_compositor_func_ >> choice_in_sequence_func_;
          contains_compositor_func_ >> sequence_in_sequence_func_;

          // Nested classes.
          //
          all_ >> all_contains_;

          choice_in_choice_ >> choice_contains_;
          choice_in_sequence_ >> choice_contains_;
          choice_contains_ >> choice_in_choice_;
          choice_contains_ >> sequence_in_choice_;

          sequence_in_choice_ >> sequence_contains_;
          sequence_in_sequence_ >> sequence_contains_;
          sequence_contains_ >> choice_in_sequence_;
          sequence_contains_ >> sequence_in_sequence_;

          contains_compositor_ >> all_;
          contains_compositor_ >> choice_in_sequence_;
          contains_compositor_ >> sequence_in_sequence_;
        }

        virtual void
        traverse (Type& c)
        {
          String const& scope (ename_custom (c));

          // We may not need to generate the class if this type is
          // being customized.
          //
          if (!scope)
            return;

          os << "// " << comment (c.name ()) << endl
             << "//" << endl
             << endl;

          if (!restriction_p (c))
          {
            Complex::names (c, attribute_names_func_);

            if (c.contains_compositor_p ())
              Complex::contains_compositor (c, contains_compositor_func_);

            // Nested classes.
            //
            if (c.contains_compositor_p ())
              Complex::contains_compositor (c, contains_compositor_);
          }

          // Custom data.
          //
          if (c.context ().count ("cd-name"))
          {
            String const& name (ecd_name (c));
            String const& member (ecd_member (c));
            String const& sequence (ecd_sequence (c));

            // const seq&
            // name () const
            //
            os << inl
               << "const " << scope << "::" << sequence << "& " <<
              scope << "::" << endl
               << name << " () const"
               << "{"
               << "return this->" << member << ";"
               << "}";

            // seq&
            // name ()
            //
            os << inl
               << scope << "::" << sequence << "& " << scope << "::" << endl
               << name << " ()"
               << "{"
               << "return this->" << member << ";"
               << "}";
          }
        }

      private:
        // Functions.
        //
        AttributeFunc attribute_func_;
        Traversal::Names attribute_names_func_;

        ElementFunc element_func_;
        ElementInChoiceFunc element_in_choice_func_;
        AllFunc all_func_;
        ChoiceInChoiceFunc choice_in_choice_func_;
        ChoiceInSequenceFunc choice_in_sequence_func_;
        SequenceInChoiceFunc sequence_in_choice_func_;
        SequenceInSequenceFunc sequence_in_sequence_func_;
        Traversal::ContainsParticle all_contains_func_;
        Traversal::ContainsParticle choice_contains_func_;
        Traversal::ContainsParticle sequence_contains_func_;

        Traversal::ContainsCompositor contains_compositor_func_;

        // Nested classes.
        //
        All all_;
        Choice choice_in_choice_;
        Choice choice_in_sequence_;
        SequenceInChoice sequence_in_choice_;
        SequenceInSequence sequence_in_sequence_;
        Traversal::ContainsParticle all_contains_;
        Traversal::ContainsParticle choice_contains_;
        Traversal::ContainsParticle sequence_contains_;

        Traversal::ContainsCompositor contains_compositor_;
      };
    }

    void
    generate_tree_inline (Context& ctx)
    {
      // Generate includes.
      //
      if (ctx.options.generate_inline ())
      {
        Traversal::Schema schema;
        Includes includes (ctx, Includes::inline_);

        schema >> includes;

        schema.dispatch (ctx.schema_root);
      }
      else
      {
        // Emit "weak" header includes that are used in the file-per-type
        // compilation model.
        //
        Traversal::Schema schema;
        Includes includes (ctx, Includes::source);

        schema >> includes;

        schema.dispatch (ctx.schema_root);
      }

      Traversal::Schema schema;
      Sources sources;
      Traversal::Names names_ns, names;

      Namespace ns (ctx);

      List list (ctx);
      Union union_ (ctx);
      Complex complex (ctx);
      Enumeration enumeration (ctx, complex);

      schema >> sources >> schema;
      schema >> names_ns >> ns >> names;

      names >> list;
      names >> union_;
      names >> complex;
      names >> enumeration;

      schema.dispatch (ctx.schema_root);
    }
  }
}
