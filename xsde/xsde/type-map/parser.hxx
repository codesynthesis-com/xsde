// file      : xsde/type-map/parser.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_TYPE_MAP_PARSER_HXX
#define XSDE_TYPE_MAP_PARSER_HXX

#include <xsde/type-map/type-map.hxx>
#include <xsde/type-map/lexer.hxx>

#include <xsde/types.hxx>

namespace TypeMap
{
  class Parser
  {
  public:
    Parser (Lexer&, String const& path);

    // Merge parsed namespaces.
    //
    bool
    parse (Namespaces&);

  private:
    bool
    namespace_ (Namespaces&);

    bool
    include (Namespace&);

    bool
    type (Lexer::Token, Namespace&);

  private:
    Lexer& lex_;
    String path_;
    std::wostream& e;
  };
}

#endif // XSDE_TYPE_MAP_PARSER_HXX
