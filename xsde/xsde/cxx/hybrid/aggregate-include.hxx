// file      : xsde/cxx/hybrid/aggregate-include.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_HYBRID_AGGREGATE_INCLUDE_HXX
#define XSDE_CXX_HYBRID_AGGREGATE_INCLUDE_HXX

#include <set>

#include <libxsd-frontend/semantic-graph.hxx>
#include <libxsd-frontend/traversal.hxx>

#include <xsde/cxx/hybrid/elements.hxx>

namespace CXX
{
  namespace Hybrid
  {
    // Parser/serializer implementation includes for additional
    // schemas (polymorphic code).
    //

    // For base types we only want member's types, but not the
    // base itself.
    //
    struct BaseInclude: Traversal::Complex, Context
    {
      BaseInclude (Context& c)
          : Context (c)
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
    };

    struct TypeInclude: Traversal::Type,
                        Traversal::Complex,
                        Context
    {
      TypeInclude (Context& c)
          : Context (c), base_ (c)
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
        if (types_.find (&t) != types_.end ())
          return;

        types_.insert (&t);

        if (polymorphic (t))
          collect (t);
      }

      virtual void
      traverse (SemanticGraph::Complex& c)
      {
        if (types_.find (&c) != types_.end ())
          return;

        types_.insert (&c);

        if (polymorphic (c))
          collect (c);

        inherits (c);

        if (!restriction_p (c))
        {
          names (c);
          contains_compositor (c);
        }
      }

    private:
      virtual void
      collect (SemanticGraph::Type& t)
      {
        using SemanticGraph::Type;

        for (Type::BegetsIterator i (t.begets_begin ());
             i != t.begets_end ();
             ++i)
        {
          Type& d (i->derived ());
          emit (d);
          dispatch (d);
          collect (d);
        }
      }

      virtual void
      emit (SemanticGraph::Type& t)
      {
        using SemanticGraph::Schema;

        Schema* s (&dynamic_cast<Schema&> (t.scope ().scope ()));

        // If this is not a top-level schema, get one that
        // includes/import/sources this schema. Top-level schema
        // is either not used by any other schema or is imported
        // into a special schema that doesn't have a namespace.
        //
        for (;;)
        {
          if (!s->used_p ())
            break;

          SemanticGraph::Uses& u (*s->used_begin ());
          Schema& us (u.user ());

          if (us.names_begin () == us.names_end ())
            break;

          s = &us;
        }


        if (s != &schema_root && schemas_.find (s) == schemas_.end ())
        {
          schemas_.insert (s);

          SemanticGraph::Path path (s->used_begin ()->path ());
          path.normalize ();

          // Try to use the portable representation of the path. If that
          // fails, fall back to the native representation.
          //
          NarrowString path_str;
          try
          {
            path_str = path.posix_string ();
          }
          catch (SemanticGraph::InvalidPath const&)
          {
            path_str = path.string ();
          }

          String inc_path (hxx_expr->replace (path_str));
          os << "#include " << process_include_path (inc_path) << endl
             << endl;
        }
      }

    private:
      std::set<SemanticGraph::Type*> types_;
      std::set<SemanticGraph::Schema*> schemas_;

      BaseInclude base_;
      Traversal::Inherits inherits_;

      Traversal::Compositor compositor_;
      Traversal::Element particle_;
      Traversal::ContainsCompositor contains_compositor_;
      Traversal::ContainsParticle contains_particle_;

      Traversal::Names names_;
      Traversal::Attribute attribute_;

      Traversal::Belongs belongs_;
    };

    struct AggregateInclude: Traversal::Type,
                             Traversal::Element,
                             Context
    {
      AggregateInclude (Context& c, char const* key)
          : Context (c), key_ (key), type_include_ (c)
      {
      }

      virtual void
      traverse (SemanticGraph::Type& t)
      {
        if (t.context ().count (key_))
          type_include_.dispatch (t);
      }

      virtual void
      traverse (SemanticGraph::Element& e)
      {
        if (e.context ().count (key_))
          type_include_.dispatch (e.type ());
      }

    private:
      char const* key_;
      TypeInclude type_include_;
    };
  }
}

#endif // XSDE_CXX_HYBRID_AGGREGATE_INCLUDE_HXX
