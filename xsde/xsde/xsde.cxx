// file      : xsde/xsde.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <set>
#include <map>
#include <vector>
#include <memory>  // std::unique_ptr
#include <cstddef> // std::size_t
#include <iostream>
#include <fstream>

#include <libcutl/re.hxx>

#include <libxsd-frontend/parser.hxx>
#include <libxsd-frontend/transformations/anonymous.hxx>
#include <libxsd-frontend/transformations/enum-synthesis.hxx>
#include <libxsd-frontend/transformations/restriction.hxx>
#include <libxsd-frontend/transformations/schema-per-type.hxx>
#include <libxsd-frontend/transformations/simplifier.hxx>

#include <xsde/version.hxx>

#include <xsde/type-map/type-map.hxx>

#include <xsde/cxx/parser/options.hxx>
#include <xsde/cxx/serializer/options.hxx>
#include <xsde/cxx/hybrid/options.hxx>

#include <xsde/cxx/parser/generator.hxx>
#include <xsde/cxx/serializer/generator.hxx>
#include <xsde/cxx/hybrid/generator.hxx>

#include <xsde/processing/inheritance/processor.hxx>

#include <xsde/xsde.hxx>
#include <xsde/types.hxx>
#include <xsde/options.hxx>

namespace SemanticGraph = XSDFrontend::SemanticGraph;
namespace Transformations = XSDFrontend::Transformations;

using namespace std;

//
//
struct LocationTranslator: XSDFrontend::LocationTranslator
{
  struct Failed {};

  LocationTranslator (NarrowStrings const& map,
                      NarrowStrings const& regex,
                      bool trace);

  virtual NarrowString
  translate (NarrowString const&);

private:
  typedef map<NarrowString, NarrowString> Map;

  typedef cutl::re::regexsub Regex;
  typedef cutl::re::format RegexFormat;
  typedef vector<Regex> RegexVector;

  typedef map<NarrowString, NarrowString> Cache;

  Map map_;
  RegexVector regex_;
  Cache cache_;
  bool trace_;
};

//
//
struct AnonymousNameTranslator: Transformations::AnonymousNameTranslator
{
  struct Failed {};

  AnonymousNameTranslator (NarrowStrings const& regex, bool trace);

  virtual String
  translate (String const& file,
             String const& ns,
             String const& name,
             String const& xpath);

private:
  typedef cutl::re::wregexsub Regex;
  typedef cutl::re::wformat RegexFormat;
  typedef vector<Regex> RegexVector;

  RegexVector regex_;
  bool trace_;

};

//
//
struct SchemaPerTypeTranslator: Transformations::SchemaPerTypeTranslator
{
  struct Failed {};

  SchemaPerTypeTranslator (NarrowStrings const& type_regex,
                           bool type_trace,
                           NarrowStrings const& schema_regex,
                           bool schema_trace);

  virtual String
  translate_type (String const& ns, String const& name);

  virtual NarrowString
  translate_schema (NarrowString const& file);

private:
  typedef cutl::re::wregexsub TypeRegex;
  typedef cutl::re::wformat TypeRegexFormat;
  typedef vector<TypeRegex> TypeRegexVector;

  TypeRegexVector type_regex_;
  bool type_trace_;

  typedef cutl::re::regexsub SchemaRegex;
  typedef cutl::re::format SchemaRegexFormat;
  typedef vector<SchemaRegex> SchemaRegexVector;

  SchemaRegexVector schema_regex_;
  bool schema_trace_;
};

// Expand the \n escape sequence.
//
void
expand_nl (NarrowString& s);

int
main (int argc, char* argv[])
{
  wostream& e (wcerr);

  try
  {
    cli::argv_file_scanner args (argc, argv, "--options-file");
    help_options help_ops (args, cli::unknown_mode::stop);

    // Handle --build2-metadata (see also buildfile).
    //
    // Note that the options::build2_metadata_specified () function is not
    // defined since using the --generate-specifier cli option would break
    // quite a bit of code (see buildfile for details). So here we rely on the
    // fact that build2 may not pass the zero (default) value for this option.
    //
    if (help_ops.build2_metadata () != 0)
    {
      wostream& o (wcout);

      // Note that the export.metadata variable should be the first non-
      // blank/comment line.
      //
      o << "# build2 buildfile xsde" << endl
        << "export.metadata = 1 xsde" << endl
        << "xsde.name = [string] xsde" << endl
        << "xsde.version = [string] '" << XSDE_VERSION_FULL << '\'' << endl
        << "xsde.checksum = [string] '" << XSDE_VERSION_FULL << '\'' << endl;

      return 0;
    }

    NarrowString cmd;
    if (args.more ())
      cmd = args.next ();

    if (help_ops.version () || cmd == "version")
    {
      wostream& o (wcout);

      o << "CodeSynthesis XSD/e XML Schema to C++ compiler " <<
        "for embedded systems " << XSDE_VERSION_STR << endl
        << "Copyright (c) " << XSDE_COPYRIGHT << "." << endl;

      if (!help_ops.proprietary_license () && cmd == "version")
      {
        // Parse the options after the command to detect trailing
        // --proprietary-license.
        //
        help_ops = help_options (args, cli::unknown_mode::stop);
      }

      if (help_ops.proprietary_license ())
      {
        o << "The compiler was invoked in the Proprietary License mode. You "
          << "should have\nreceived a proprietary license from Code Synthesis "
          << "Tools CC that entitles\nyou to use it in this mode." << endl;
      }
      else
      {
        o << "This is free software; see the source for copying conditions. "
          << "There is NO\nwarranty; not even for MERCHANTABILITY or FITNESS "
          << "FOR A PARTICULAR PURPOSE." << endl;
      }

      return 0;
    }

    if (help_ops.help () || cmd == "help")
    {
      wostream& o (wcout);

      if (cmd == "help" && args.more ())
      {
        NarrowString arg (args.next ());

        if (arg == "cxx-parser")
        {
          o << "Usage: " << argv[0] << " cxx-parser [options] file [file ...]"
            << endl
            << "Options:" << endl;

          CXX::Parser::Generator::usage ();
        }
        else if (arg == "cxx-serializer")
        {
          o << "Usage: " << argv[0] << " cxx-serializer [options] file " <<
            "[file ...]" << endl
            << "Options:" << endl;

          CXX::Serializer::Generator::usage ();
        }
        else if (arg == "cxx-hybrid")
        {
          o << "Usage: " << argv[0] << " cxx-hybrid [options] file " <<
            "[file ...]" << endl
            << "Options:" << endl;

          CXX::Hybrid::Generator::usage ();
        }
        else
        {
          e << "error: unknown command '" << arg.c_str () << "'" << endl
            << "info: try '" << argv[0] << " help' for the list of commands"
            << endl;

          return 1;
        }

        // Add frontend options at the end.
        //
        options::print_usage (o);
      }
      else
      {
        o << "Usage: " << argv[0] << " <cmd> ..." << endl
          << "Commands:" << endl;

        o << "  help            Print usage information and exit. Use\n"
          << "                  'help <cmd>' for command-specific options."
          << endl;

        o << "  version         Print version and exit."
          << endl;

        o << "  cxx-parser      Generate Embedded C++/Parser mapping."
          << endl;

        o << "  cxx-serializer  Generate Embedded C++/Serializer mapping."
          << endl;

        o << "  cxx-hybrid      Generate Embedded C++/Hybrid mapping."
          << endl;
      }

      return 0;
    }

    if (cmd.empty ())
    {
      e << "error: no command specified" << endl
        << "info: try '" << argv[0] << " help' for usage information" << endl;

      return 1;
    }

    if (cmd != "cxx-parser" && cmd != "cxx-serializer" && cmd != "cxx-hybrid")
    {
      e << "error: unknown command '" << cmd.c_str () << "'" << endl
        << "info: try '" << argv[0] << " help' for the list of commands"
        << endl;

      return 1;
    }

    // We need to parse command line options before we can get to
    // the arguments.
    //
    unique_ptr<CXX::Parser::options> p_ops (
      cmd == "cxx-parser" ? new CXX::Parser::options (args) : 0);

    unique_ptr<CXX::Serializer::options> s_ops (
      cmd == "cxx-serializer" ? new CXX::Serializer::options (args) : 0);

    unique_ptr<CXX::Hybrid::options> h_ops (
      cmd == "cxx-hybrid" ? new CXX::Hybrid::options (args) : 0);

    CXX::options& common_ops (
      cmd == "cxx-parser"
      ? static_cast<CXX::options&> (*p_ops)
      : cmd == "cxx-serializer"
      ? static_cast<CXX::options&> (*s_ops)
      : static_cast<CXX::options&> (*h_ops));

    // Disabled warnings.
    //
    WarningSet disabled_w;
    {
      NarrowStrings const& w (common_ops.disable_warning ());

      for (NarrowStrings::const_iterator i (w.begin ()); i != w.end (); ++i)
        disabled_w.insert (*i);
    }

    bool disabled_w_all (disabled_w.find ("all") != disabled_w.end ());

    // Collect all the files to compile in a vector.
    //
    NarrowStrings files;

    while (args.more ())
      files.push_back (args.next ());

    if (files.empty ())
    {
      e << "error: no input file specified" << endl;
      return 1;
    }

    bool fpt (common_ops.file_per_type ());

    // Generate/extern XML Schema checks.
    //
    if (cmd == "cxx-parser" || cmd == "cxx-serializer" || cmd == "cxx-hybrid")
    {
      bool gen (common_ops.generate_xml_schema ());
      bool use (common_ops.extern_xml_schema ());

      // Things get complicated when we are compiling several schemas at
      // once (non-file-per-type mode) and use the --generate-xml-schema/
      // --extern-xml-schema options. The only way we can figure out which
      // file corresponds to XML Schema is if the --extern-xml-schema option
      // is also present. So we are going to require it for this case,
      // especially since it generally makes sense.
      //
      if (!fpt)
      {
        if (files.size () > 1 && gen && !use)
        {
          e << "error: --extern-xml-schema is required when compiling more "
            << "than one schema and --generate-xml-schema is specified"
            << endl;

          return 1;
        }

        if (files.size () == 1 && gen && use)
        {
          e << "error: --generate-xml-schema and --extern-xml-schema are "
            << "mutually exclusive when compiling a single schema" << endl;

          return 1;
        }
      }
      else
      {
        // The --file-per-type and --generate-xml-schema options are
        // incompatible. It also makes sense to use --file-per-type
        // and --extern-xml-schema.
        //
        if (gen)
        {
          e << "error: --file-per-type and --generate-xml-schema are "
            << "incompatible" << endl
            << "info: use --generate-xml-schema in a separate invocation "
            << "of the compiler" << endl;

          return 1;
        }

        if (!use &&
            !disabled_w_all && disabled_w.find ("D001") == disabled_w.end ())
        {
          e << "warning D001: --extern-xml-schema is recommended when "
            << "--file-per-type is specified to reduce generated code size"
            << endl;
        }
      }
    }

    //
    //
    FileList file_list;
    AutoUnlinks unlinks;
    size_t sloc (0);

    LocationTranslator loc_translator (
      common_ops.location_map (),
      common_ops.location_regex (),
      common_ops.location_regex_trace ());

    AnonymousNameTranslator anon_translator (
      common_ops.anonymous_regex (),
      common_ops.anonymous_regex_trace ());

    bool gen_hybrid (cmd == "cxx-hybrid");

    bool gen_parser (
      cmd == "cxx-parser" ||
      (gen_hybrid && h_ops->generate_parser ()));

    bool gen_serializer (
      cmd == "cxx-serializer" ||
      (gen_hybrid && h_ops->generate_serializer ()));

    bool poly_aggr (
      gen_hybrid &&
      h_ops->generate_polymorphic () &&
      h_ops->generate_aggregate ());

    if (!fpt)
    {
      // File-per-schema compilation mode.
      //
      for (size_t i (0); i < files.size (); ++i)
      {
        // Parse schema.
        //
        SemanticGraph::Path tu;
        SemanticGraph::Paths paths;

        XSDFrontend::Parser parser (
          true,
          !common_ops.disable_multi_import (),
          !common_ops.disable_full_check (),
          loc_translator,
          disabled_w);

        unique_ptr<SemanticGraph::Schema> schema;

        try
        {
          tu = SemanticGraph::Path (files[i]);
        }
        catch (SemanticGraph::InvalidPath const&)
        {
          e << "error: '" << files[i].c_str () << "' is not a valid "
            << "filesystem path" << endl;
          return 1;
        }

        // See if we are generating code for the XML Schema namespace.
        // We could be compiling several schemas at once in which case
        // handling of the --generate-xml-schema option gets tricky: we
        // will need to rely on the presence of the --extern-xml-schema
        // to tell us which (fake) schema file corresponds to XML Schema.
        //
        bool gen_xml_schema (common_ops.generate_xml_schema ());
        NarrowString xml_schema_file;

        if (gen_xml_schema)
        {
          if ((xml_schema_file = common_ops.extern_xml_schema ()))
          {
            if (tu.string () != xml_schema_file)
              gen_xml_schema = false;
          }
        }

        // If we are generating polymorphic aggregates then we need
        // to add all the schemas to the semantic graph in case they
        // define derived polymorphic types, except for the fake XML
        // Schema file.
        //
        bool multi (poly_aggr && !gen_xml_schema);

        if (multi)
        {
          paths.push_back (tu);

          size_t ai (0);
          try
          {
            for (; ai < files.size (); ++ai)
            {
              if (ai != i && files[ai] != xml_schema_file)
                paths.push_back (SemanticGraph::Path (files[ai]));
            }
          }
          catch (SemanticGraph::InvalidPath const&)
          {
            e << "error: '" << files[ai].c_str () << "' is not a valid "
              << "filesystem path" << endl;
            return 1;
          }

          // Also include additional schemas that may be specified with
          // the --polymorphic-schema option.
          //
          NarrowStrings const& extra_files (h_ops->polymorphic_schema ());
          NarrowStrings::const_iterator i (extra_files.begin ());

          try
          {
            for (; i != extra_files.end (); ++i)
            {
              paths.push_back (SemanticGraph::Path (*i));
            }
          }
          catch (SemanticGraph::InvalidPath const&)
          {
            e << "error: '" << i->c_str () << "' is not a valid "
              << "filesystem path" << endl;
            return 1;
          }

          if (paths.size () == 1)
            multi = false;
        }

        if (gen_xml_schema)
          schema = parser.xml_schema (tu);
        else
        {
          if (multi)
            schema = parser.parse (paths);
          else
            schema = parser.parse (tu);
        }

        // Morph anonymous types.
        //
        if (!common_ops.preserve_anonymous ())
        {
          try
          {
            Transformations::Anonymous trans (anon_translator);

            if (multi)
              trans.transform (*schema, SemanticGraph::Path (), true);
            else
              trans.transform (*schema, tu, true);
          }
          catch (Transformations::Anonymous::Failed const&)
          {
            return 1; // Diagnostic has already been issued.
          }
        }

        // Synthesize enumerations from unions.
        //
        if (gen_hybrid)
        {
          Transformations::EnumSynthesis trans;
          trans.transform (*schema, tu);
        }

        // Simplify the schema graph.
        //
        {
          Transformations::Simplifier trans;

          if (multi)
            trans.transform (*schema, SemanticGraph::Path ());
          else
            trans.transform (*schema, tu);
        }

        // Calculate type sizes.
        //
        if (gen_hybrid)
        {
          try
          {
            if (multi)
              CXX::Hybrid::Generator::calculate_size (
                *h_ops, *schema, SemanticGraph::Path (), disabled_w);
            else
              CXX::Hybrid::Generator::calculate_size (
                *h_ops, *schema, tu, disabled_w);
          }
          catch (CXX::Hybrid::Generator::Failed const&)
          {
            return 1; // Diagnostic has already been issued.
          }
        }

        // Try to rearrange definitions so that there is no forward
        // inheritance.
        //
        try
        {
          Processing::Inheritance::Processor proc;

          if (multi)
            proc.process (*schema,
                          SemanticGraph::Path (),
                          gen_hybrid ? "fixed" : 0);
          else
            proc.process (*schema, tu, gen_hybrid ? "fixed" : 0);
        }
        catch (Processing::Inheritance::Processor::Failed const&)
        {
          return 1; // Diagnostic has already been issued.
        }

        // Normalize and annotate complex content restrictions.
        //
        if (gen_parser || gen_serializer)
        {
          try
          {
            Transformations::Restriction trans;

            if (multi)
              trans.transform (*schema, SemanticGraph::Path ());
            else
              trans.transform (*schema, tu);
          }
          catch (Transformations::Restriction::Failed const&)
          {
            return 1; // Diagnostic has already been issued.
          }
        }

        // Get the first schema and assign names in additional
        // schemas.
        //
        SemanticGraph::Schema* root;

        if (multi)
        {
          using SemanticGraph::Schema;

          Schema::UsesIterator b (schema->uses_begin ());
          ++b; // Implied XML Schema namespace.

          // The first schema. Will be handled later.
          //
          root = &b->schema ();

          // Create parser/serializer options (we need a schema, any
          // schema to do this).
          //
          if (gen_parser && !p_ops.get ())
            p_ops = CXX::Hybrid::Generator::parser_options (
              *h_ops, *root, b->path ());

          if (gen_serializer && !s_ops.get ())
            s_ops = CXX::Hybrid::Generator::serializer_options (
              *h_ops, *root, b->path ());

          ++b;

          for (Schema::UsesIterator e (schema->uses_end ()); b != e; ++b)
          {
            SemanticGraph::Schema& s (b->schema ());
            SemanticGraph::Path f (b->path ());

            if (gen_hybrid)
              CXX::Hybrid::Generator::process_tree_names (*h_ops, s, f);

            if (gen_parser)
              CXX::Parser::Generator::process_names (*p_ops, s, f);

            if (gen_serializer)
              CXX::Serializer::Generator::process_names (*s_ops, s, f);

            try
            {
              if (gen_hybrid && gen_parser)
                CXX::Hybrid::Generator::process_parser_names (*h_ops, s, f);

              if (gen_hybrid && gen_serializer)
                CXX::Hybrid::Generator::process_serializer_names (*h_ops, s, f);
            }
            catch (CXX::Hybrid::Generator::Failed const&)
            {
              // Diagnostics has already been issued.
              //
              return 1;
            }
          }
        }
        else
        {
          root = schema.get ();

          // Create parser/serializer options (we need a schema, any
          // schema to do this).
          //
          if (gen_parser && !p_ops.get ())
            p_ops = CXX::Hybrid::Generator::parser_options (*h_ops, *root, tu);

          if (gen_serializer && !s_ops.get ())
            s_ops = CXX::Hybrid::Generator::serializer_options (
              *h_ops, *root, tu);
        }

        // Generate mapping.
        //
        TypeMap::Namespaces parser_type_map, serializer_type_map;

        if (gen_hybrid)
        {
          try
          {
            sloc += CXX::Hybrid::Generator::generate_tree (
              *h_ops,
              *root,
              tu,
              false,
              disabled_w,
              parser_type_map,
              serializer_type_map,
              file_list,
              unlinks);
          }
          catch (CXX::Hybrid::Generator::Failed const&)
          {
            // Diagnostics has already been issued.
            //
            return 1;
          }
        }

        if (gen_parser)
        {
          try
          {
            sloc += CXX::Parser::Generator::generate (
              *p_ops,
              *root,
              tu,
              false,
              parser_type_map,
              true,
              disabled_w,
              file_list,
              unlinks);
          }
          catch (CXX::Parser::Generator::Failed const&)
          {
            // Diagnostics has already been issued.
            //
            return 1;
          }
        }

        if (gen_serializer)
        {
          try
          {
            sloc += CXX::Serializer::Generator::generate (
              *s_ops,
              *root,
              tu,
              false,
              serializer_type_map,
              true,
              disabled_w,
              file_list,
              unlinks);
          }
          catch (CXX::Serializer::Generator::Failed const&)
          {
            // Diagnostics has already been issued.
            //
            return 1;
          }
        }

        if (gen_hybrid)
        {
          if (gen_parser)
          {
            try
            {
              sloc += CXX::Hybrid::Generator::generate_parser (
                *h_ops,
                *root,
                tu,
                false,
                disabled_w,
                file_list,
                unlinks);
            }
            catch (CXX::Hybrid::Generator::Failed const&)
            {
              // Diagnostics has already been issued.
              //
              return 1;
            }
          }

          if (gen_serializer)
          {
            try
            {
              sloc += CXX::Hybrid::Generator::generate_serializer (
                *h_ops,
                *root,
                tu,
                false,
                disabled_w,
                file_list,
                unlinks);
            }
            catch (CXX::Hybrid::Generator::Failed const&)
            {
              // Diagnostics has already been issued.
              //
              return 1;
            }
          }
        }
      }
    }
    else
    {
      // File-per-type compilation mode.
      //
      SemanticGraph::Paths paths;

      for (size_t i (0); i < files.size (); ++i)
      {
        try
        {
          paths.push_back (SemanticGraph::Path (files[i]));
        }
        catch (SemanticGraph::InvalidPath const&)
        {
          e << "error: '" << files[i].c_str () << "' is not a valid "
            << "filesystem path" << endl;

          return 1;
        }
      }

      if (paths.size () > 1 &&
          ((cmd == "cxx-parser" && p_ops->generate_test_driver ()) ||
           (cmd == "cxx-serializer" && s_ops->generate_test_driver ())))
      {
        e << "info: generating test driver for the first schema only: '" <<
          paths[0] << "'" << endl;
      }

      XSDFrontend::Parser parser (
        cmd != "cxx-hybrid",
        !common_ops.disable_multi_import (),
        !common_ops.disable_full_check (),
        loc_translator,
        disabled_w);

      unique_ptr<SemanticGraph::Schema> schema (parser.parse (paths));

      // Morph anonymous types.
      //
      if (!common_ops.preserve_anonymous ())
      {
        try
        {
          Transformations::Anonymous trans (anon_translator);
          trans.transform (*schema, SemanticGraph::Path (), false);
        }
        catch (Transformations::Anonymous::Failed const&)
        {
          return 1; // Diagnostic has already been issued.
        }
      }

      // Synthesize enumerations from unions.
      //
      if (gen_hybrid)
      {
        Transformations::EnumSynthesis trans;
        trans.transform (*schema, SemanticGraph::Path ());
      }

      // Simplify the schema graph.
      //
      {
        Transformations::Simplifier trans;
        trans.transform (*schema, SemanticGraph::Path ());
      }

      // Calculate type sizes.
      //
      if (gen_hybrid)
      {
        try
        {
          CXX::Hybrid::Generator::calculate_size (
            *h_ops, *schema, SemanticGraph::Path (), disabled_w);
        }
        catch (CXX::Hybrid::Generator::Failed const&)
        {
          return 1; // Diagnostic has already been issued.
        }
      }

      // Normalize and annotate complex content restrictions.
      //
      if (gen_parser || gen_serializer)
      {
        try
        {
          Transformations::Restriction trans;
          trans.transform (*schema, SemanticGraph::Path ());
        }
        catch (Transformations::Restriction::Failed const&)
        {
          return 1; // Diagnostic has already been issued.
        }
      }

      // Rearrange the graph so that each type is in a seperate
      // schema file.
      //
      typedef vector<SemanticGraph::Schema*> Schemas;

      SchemaPerTypeTranslator type_translator (
        common_ops.type_file_regex (),
        common_ops.type_file_regex_trace (),
        common_ops.schema_file_regex (),
        common_ops.schema_file_regex_trace ());

      Transformations::SchemaPerType trans (
        type_translator,
        common_ops.fat_type_file (),
        gen_hybrid ? "fixed" : 0);

      Schemas schemas (trans.transform (*schema));

      // Generate code.
      //
      for (Schemas::iterator b (schemas.begin ()), i (b), e (schemas.end ());
           i != e; ++i)
      {
        SemanticGraph::Schema& s (**i);
        SemanticGraph::Path path (
          s.context ().count ("renamed")
          ? s.context ().get<SemanticGraph::Path> ("renamed")
          : s.used_begin ()->path ());

        // Create parser/serializer options (we need a schema, any
        // schema to do this).
        //
        if (gen_parser && !p_ops.get ())
          p_ops = CXX::Hybrid::Generator::parser_options (*h_ops, s, path);

        if (gen_serializer && !s_ops.get ())
          s_ops = CXX::Hybrid::Generator::serializer_options (*h_ops, s, path);

        TypeMap::Namespaces parser_type_map, serializer_type_map;

        if (gen_hybrid)
        {
          try
          {
            sloc += CXX::Hybrid::Generator::generate_tree (
              *h_ops,
              s,
              path,
              true,
              disabled_w,
              parser_type_map,
              serializer_type_map,
              file_list,
              unlinks);
          }
          catch (CXX::Hybrid::Generator::Failed const&)
          {
            // Diagnostic has already been issued.
            //
            return 1;
          }
        }

        if (gen_parser)
        {
          try
          {
            // Only generate driver for the first schema.
            //
            sloc += CXX::Parser::Generator::generate (
              *p_ops,
              s,
              path,
              true,
              parser_type_map,
              i == b,
              disabled_w,
              file_list,
              unlinks);
          }
          catch (CXX::Parser::Generator::Failed const&)
          {
            // Diagnostic has already been issued.
            //
            return 1;
          }
        }

        if (gen_serializer)
        {
          try
          {
            // Only generate driver for the first schema.
            //
            sloc += CXX::Serializer::Generator::generate (
              *s_ops,
              s,
              path,
              true,
              serializer_type_map,
              i == b,
              disabled_w,
              file_list,
              unlinks);
          }
          catch (CXX::Serializer::Generator::Failed const&)
          {
            // Diagnostic has already been issued.
            //
            return 1;
          }
        }

        if (gen_hybrid)
        {
          if (gen_parser)
          {
            try
            {
              sloc += CXX::Hybrid::Generator::generate_parser (
                *h_ops,
                s,
                path,
                true,
                disabled_w,
                file_list,
                unlinks);
            }
            catch (CXX::Hybrid::Generator::Failed const&)
            {
              // Diagnostics has already been issued.
              //
              return 1;
            }
          }

          if (gen_serializer)
          {
            try
            {
              sloc += CXX::Hybrid::Generator::generate_serializer (
                *h_ops,
                s,
                path,
                true,
                disabled_w,
                file_list,
                unlinks);
            }
            catch (CXX::Hybrid::Generator::Failed const&)
            {
              // Diagnostics has already been issued.
              //
              return 1;
            }
          }
        }
      }
    }

    // See if we need to produce the file list.
    //
    if (NarrowString fl = common_ops.file_list ())
    {
      typedef std::ofstream OutputFileStream;

      try
      {
        OutputFileStream ofs;
        SemanticGraph::Path path (fl);

        ofs.open (path.string ().c_str (), ios_base::out);

        if (!ofs.is_open ())
        {
          wcerr << path << ": error: unable to open in write mode" << endl;
          return 1;
        }

        NarrowString d (common_ops.file_list_delim ());
        expand_nl (d);

        if (NarrowString p = common_ops.file_list_prologue ())
        {
          expand_nl (p);
          ofs << p;
        }

        for (FileList::iterator i (file_list.begin ()), e (file_list.end ());
             i != e;)
        {
          ofs << *i;

          if (++i != e)
            ofs << d;
        }

        if (NarrowString e = common_ops.file_list_epilogue ())
        {
          expand_nl (e);
          ofs << e;
        }
      }
      catch (SemanticGraph::InvalidPath const&)
      {
        wcerr << "error: '" << fl.c_str () << "' is not a valid "
              << "filesystem path" << endl;
        return 1;
      }
    }

    if (common_ops.show_sloc ())
      e << "total: " << sloc << endl;

    if (size_t sloc_limit = common_ops.sloc_limit ())
    {
      if (sloc_limit < sloc)
      {
        e << "error: SLOC limit of " << sloc_limit
          << " lines has been exceeded" << endl;

        return 1;
      }
    }

    unlinks.cancel ();

    return 0;
  }
  catch (LocationTranslator::Failed const&)
  {
    // Diagnostic has already been issued.
  }
  catch (AnonymousNameTranslator::Failed const&)
  {
    // Diagnostic has already been issued.
  }
  catch (SchemaPerTypeTranslator::Failed const&)
  {
    // Diagnostic has already been issued.
  }
  catch (Transformations::SchemaPerType::Failed const&)
  {
    // Diagnostic has already been issued.
  }
  catch (XSDFrontend::InvalidSchema const&)
  {
    // Diagnostic has already been issued.
  }
  catch (cli::exception const& ex)
  {
    wcerr << ex << endl;
    wcerr << "try '" << argv[0] << " help' for usage information" << endl;
  }

  return 1;
}

// LocationTranslator
//

LocationTranslator::
LocationTranslator (NarrowStrings const& map,
                    NarrowStrings const& regex,
                    bool trace)
    : trace_ (trace)
{
  // Map.
  //
  for (NarrowStrings::const_iterator i (map.begin ()); i != map.end (); ++i)
  {
    // Split the string in two parts at the last '='.
    //
    size_t pos (i->rfind ('='));

    if (pos == NarrowString::npos)
    {
      wcerr << "error: invalid location map: '" << i->c_str () <<
        "': delimiter ('=') not found" << endl;

      throw Failed ();
    }

    map_[NarrowString (*i, 0, pos)] = NarrowString (*i, pos + 1);
  }

  // Regex.
  //
  for (NarrowStrings::const_iterator i (regex.begin ()); i != regex.end (); ++i)
  {
    try
    {
      regex_.push_back (Regex (*i));
    }
    catch (RegexFormat const& e)
    {
      wcerr << "error: invalid location regex: '" <<
        e.regex ().c_str () << "': " <<
        e.description ().c_str () << endl;

      throw Failed ();
    }
  }
}

NarrowString LocationTranslator::
translate (NarrowString const& l)
{
  // First check the cache.
  //
  Cache::const_iterator ci (cache_.find (l));

  if (ci != cache_.end ())
    return ci->second;

  // Then check the direct map.
  //
  Map::const_iterator mi (map_.find (l));

  if (mi != map_.end ())
  {
    cache_[l] = mi->second;
    return mi->second;
  }

  // Finally try regex.
  //
  if (trace_)
    wcerr << "location: '" << l.c_str () << "'" << endl;

  for (RegexVector::reverse_iterator i (regex_.rbegin ());
       i != regex_.rend (); ++i)
  {
    if (trace_)
      wcerr << "try: '" << i->regex ().str ().c_str () << "' : ";

    if (i->match (l))
    {
      NarrowString r (i->replace (l));

      if (trace_)
        wcerr << "'" << r.c_str () << "' : +" << endl;

      cache_[l] = r;
      return r;
    }

    if (trace_)
      wcerr << '-' << endl;
  }

  // No match - return the original location.
  //
  cache_[l] = l;
  return l;
}

// AnonymousNameTranslator
//

AnonymousNameTranslator::
AnonymousNameTranslator (NarrowStrings const& regex, bool trace)
    : trace_ (trace)
{
  for (NarrowStrings::const_iterator i (regex.begin ()); i != regex.end (); ++i)
  {
    try
    {
      regex_.push_back (Regex (String (*i)));
    }
    catch (RegexFormat const& e)
    {
      wcerr << "error: invalid anonymous type regex: '" <<
        e.regex () << "': " << e.description ().c_str () << endl;

      throw Failed ();
    }
  }
}

String AnonymousNameTranslator::
translate (String const& file,
           String const& ns,
           String const& name,
           String const& xpath)
{
  String s (file + L' ' + ns + L' ' + xpath);

  if (trace_)
    wcerr << "anonymous type: '" << s << "'" << endl;

  for (RegexVector::reverse_iterator i (regex_.rbegin ());
       i != regex_.rend (); ++i)
  {
    if (trace_)
      wcerr << "try: '" << i->regex () << "' : ";

    if (i->match (s))
    {
      String r (i->replace (s));

      if (trace_)
        wcerr << "'" << r << "' : +" << endl;

      return r;
    }

    if (trace_)
      wcerr << '-' << endl;
  }

  // No match - return the name.
  //
  return name;
}

// SchemaPerTypeTranslator
//

SchemaPerTypeTranslator::
SchemaPerTypeTranslator (NarrowStrings const& type_regex,
                         bool type_trace,
                         NarrowStrings const& schema_regex,
                         bool schema_trace)
    : type_trace_ (type_trace), schema_trace_ (schema_trace)
{
  for (NarrowStrings::const_iterator i (type_regex.begin ());
       i != type_regex.end (); ++i)
  {
    try
    {
      type_regex_.push_back (TypeRegex (String (*i)));
    }
    catch (TypeRegexFormat const& e)
    {
      wcerr << "error: invalid type file regex: '" <<
        e.regex () << "': " << e.description ().c_str () << endl;

      throw Failed ();
    }
  }

  for (NarrowStrings::const_iterator i (schema_regex.begin ());
       i != schema_regex.end (); ++i)
  {
    try
    {
      schema_regex_.push_back (SchemaRegex (*i));
    }
    catch (SchemaRegexFormat const& e)
    {
      wcerr << "error: invalid type file regex: '" <<
        e.regex ().c_str () << "': " << e.description ().c_str () << endl;

      throw Failed ();
    }
  }
}

String SchemaPerTypeTranslator::
translate_type (String const& ns, String const& name)
{
  String s (ns + L' ' + name);

  if (type_trace_)
    wcerr << "type: '" << s << "'" << endl;

  for (TypeRegexVector::reverse_iterator i (type_regex_.rbegin ());
       i != type_regex_.rend (); ++i)
  {
    if (type_trace_)
      wcerr << "try: '" << i->regex () << "' : ";

    if (i->match (s))
    {
      String r (i->replace (s));

      if (type_trace_)
        wcerr << "'" << r << "' : +" << endl;

      return r;
    }

    if (type_trace_)
      wcerr << '-' << endl;
  }

  // No match - return empty string.
  //
  return L"";
}

NarrowString SchemaPerTypeTranslator::
translate_schema (NarrowString const& file)
{
  if (schema_trace_)
    wcerr << "schema: '" << file.c_str () << "'" << endl;

  for (SchemaRegexVector::reverse_iterator i (schema_regex_.rbegin ());
       i != schema_regex_.rend (); ++i)
  {
    if (schema_trace_)
      wcerr << "try: '" << i->regex ().str ().c_str () << "' : ";

    if (i->match (file))
    {
      NarrowString r (i->replace (file));

      if (schema_trace_)
        wcerr << "'" << r.c_str () << "' : +" << endl;

      return r;
    }

    if (schema_trace_)
      wcerr << '-' << endl;
  }

  // No match - return empty string.
  //
  return "";
}

//
//
void
expand_nl (NarrowString& s)
{
  for (size_t i (0); i < s.size ();)
  {
    if (s[i] == '\\' && (i + 1) < s.size () && s[i + 1] == 'n')
    {
      NarrowString tmp (s, 0, i);
      tmp += '\n';
      tmp.append (s.c_str () + i + 2);
      s = tmp;
    }
    else
      ++i;
  }
}
