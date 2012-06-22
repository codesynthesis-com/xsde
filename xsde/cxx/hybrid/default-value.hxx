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

      void
      dispatch (SemanticGraph::Node& type, String const& value);

      virtual void
      traverse (SemanticGraph::List&);

      virtual void
      traverse (SemanticGraph::Union&);

      virtual void
      traverse (SemanticGraph::Complex&);

      virtual void
      traverse (SemanticGraph::Enumeration&);

      virtual void
      traverse (SemanticGraph::Type& t);

      // anySimpleType.
      //
      virtual void
      traverse (SemanticGraph::AnySimpleType&);

      // Strings.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::String&);

      virtual void
      traverse (SemanticGraph::Fundamental::NormalizedString&);

      virtual void
      traverse (SemanticGraph::Fundamental::Token&);

      virtual void
      traverse (SemanticGraph::Fundamental::NameToken&);

      virtual void
      traverse (SemanticGraph::Fundamental::NameTokens&);

      virtual void
      traverse (SemanticGraph::Fundamental::Name&);

      virtual void
      traverse (SemanticGraph::Fundamental::NCName&);

      virtual void
      traverse (SemanticGraph::Fundamental::Language&);

      // Qualified name.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::QName&);

      // ID/IDREF.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Id&);

      virtual void
      traverse (SemanticGraph::Fundamental::IdRef&);

      virtual void
      traverse (SemanticGraph::Fundamental::IdRefs&);

      // URI.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::AnyURI&);

      // Binary.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Base64Binary&);

      virtual void
      traverse (SemanticGraph::Fundamental::HexBinary&);

      // Date/time.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Date&);

      virtual void
      traverse (SemanticGraph::Fundamental::DateTime&);

      virtual void
      traverse (SemanticGraph::Fundamental::Duration&);

      virtual void
      traverse (SemanticGraph::Fundamental::Day&);

      virtual void
      traverse (SemanticGraph::Fundamental::Month&);

      virtual void
      traverse (SemanticGraph::Fundamental::MonthDay&);

      virtual void
      traverse (SemanticGraph::Fundamental::Year&);

      virtual void
      traverse (SemanticGraph::Fundamental::YearMonth&);

      virtual void
      traverse (SemanticGraph::Fundamental::Time&);

      // Entity.
      //
      virtual void
      traverse (SemanticGraph::Fundamental::Entity&);

      virtual void
      traverse (SemanticGraph::Fundamental::Entities&);

    private:
      void
      string_type (SemanticGraph::Type&);

      void
      string_sequence_type ();

      void
      time_zone (size_t pos);

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

      void
      dispatch (SemanticGraph::Node& type,
                String const& lhs,
                String const& rhs);

      virtual void
      traverse (SemanticGraph::Union&);

      virtual void
      traverse (SemanticGraph::Complex&);

      virtual void
      traverse (SemanticGraph::Enumeration&);

      virtual void
      traverse (SemanticGraph::Type& t);

    private:
      String const* lhs_;
      String const* rhs_;
    };
  }
}

#endif // CXX_HYBRID_DEFAULT_VALUE_HXX
