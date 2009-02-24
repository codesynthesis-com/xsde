// file      : xsde/cxx/parser/characters-validation-source.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_PARSER_CHARACTERS_VALIDATION_SOURCE_HXX
#define CXX_PARSER_CHARACTERS_VALIDATION_SOURCE_HXX

#include <xsd-frontend/semantic-graph/schema.hxx>

#include <cxx/parser/elements.hxx>

namespace CXX
{
  namespace Parser
  {
    Void
    generate_characters_validation_source (Context&);
  }
}

#endif  // CXX_PARSER_CHARACTERS_VALIDATION_SOURCE_HXX
