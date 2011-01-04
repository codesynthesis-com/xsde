// file      : xsde/cxx/hybrid/default-value.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_DEFAULT_VALUE_HXX
#define CXX_HYBRID_DEFAULT_VALUE_HXX

#include <xsd-frontend/semantic-graph.hxx>
#include <xsd-frontend/traversal.hxx>

#include <cxx/hybrid/elements.hxx>

namespace CXX
{
  namespace Hybrid
  {
    struct InitValue: Traversal::List,
                      Traversal::Union,
                      Traversal::Complex,
                      Traversal::Enumeration,
                      Traversal::Type,

                      Traversal::AnySimpleType,

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
      InitValue (Context&);

      Void
      dispatch (SemanticGraph::Node& type, String const& value);

      virtual Void
      traverse (SemanticGraph::List&);

      virtual Void
      traverse (SemanticGraph::Union&);

      virtual Void
      traverse (SemanticGraph::Complex&);

      virtual Void
      traverse (SemanticGraph::Enumeration&);

      virtual Void
      traverse (SemanticGraph::Type& t);

      // anySimpleType.
      //
      virtual Void
      traverse (SemanticGraph::AnySimpleType&);

      // Strings.
      //
      virtual Void
      traverse (SemanticGraph::Fundamental::String&);

      virtual Void
      traverse (SemanticGraph::Fundamental::NormalizedString&);

      virtual Void
      traverse (SemanticGraph::Fundamental::Token&);

      virtual Void
      traverse (SemanticGraph::Fundamental::NameToken&);

      virtual Void
      traverse (SemanticGraph::Fundamental::NameTokens&);

      virtual Void
      traverse (SemanticGraph::Fundamental::Name&);

      virtual Void
      traverse (SemanticGraph::Fundamental::NCName&);

      virtual Void
      traverse (SemanticGraph::Fundamental::Language&);

      // Qualified name.
      //
      virtual Void
      traverse (SemanticGraph::Fundamental::QName&);

      // ID/IDREF.
      //
      virtual Void
      traverse (SemanticGraph::Fundamental::Id&);

      virtual Void
      traverse (SemanticGraph::Fundamental::IdRef&);

      virtual Void
      traverse (SemanticGraph::Fundamental::IdRefs&);

      // URI.
      //
      virtual Void
      traverse (SemanticGraph::Fundamental::AnyURI&);

      // Binary.
      //
      virtual Void
      traverse (SemanticGraph::Fundamental::Base64Binary&);

      virtual Void
      traverse (SemanticGraph::Fundamental::HexBinary&);

      // Date/time.
      //
      virtual Void
      traverse (SemanticGraph::Fundamental::Date&);

      virtual Void
      traverse (SemanticGraph::Fundamental::DateTime&);

      virtual Void
      traverse (SemanticGraph::Fundamental::Duration&);

      virtual Void
      traverse (SemanticGraph::Fundamental::Day&);

      virtual Void
      traverse (SemanticGraph::Fundamental::Month&);

      virtual Void
      traverse (SemanticGraph::Fundamental::MonthDay&);

      virtual Void
      traverse (SemanticGraph::Fundamental::Year&);

      virtual Void
      traverse (SemanticGraph::Fundamental::YearMonth&);

      virtual Void
      traverse (SemanticGraph::Fundamental::Time&);

      // Entity.
      //
      virtual Void
      traverse (SemanticGraph::Fundamental::Entity&);

      virtual Void
      traverse (SemanticGraph::Fundamental::Entities&);

    private:
      Void
      string_type (SemanticGraph::Type&);

      Void
      string_sequence_type ();

      Void
      time_zone (Size pos);

    private:
      String member_;
      String value_;
      TypeName var_;
      TypeName var_value_;
      LiteralValue literal_value_;
      LiteralValue literal_value_list_;
    };

    struct CompareValue: Traversal::Union,
                         Traversal::Complex,
                         Traversal::Enumeration,
                         Traversal::Type,
                         Context
    {
      CompareValue (Context&);

      Void
      dispatch (SemanticGraph::Node& type,
                String const& lhs,
                String const& rhs);

      virtual Void
      traverse (SemanticGraph::Union&);

      virtual Void
      traverse (SemanticGraph::Complex&);

      virtual Void
      traverse (SemanticGraph::Enumeration&);

      virtual Void
      traverse (SemanticGraph::Type& t);

    private:
      String const* lhs_;
      String const* rhs_;
    };
  }
}

#endif // CXX_HYBRID_DEFAULT_VALUE_HXX
