// file      : xsde/xsde.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <iostream>
#include <boost/filesystem/fstream.hpp>

#include <cult/types.hxx>

#include <cult/trace/log.hxx>

#include <cult/containers/map.hxx>
#include <cult/containers/vector.hxx>

#include <cult/cli/exceptions.hxx>
#include <cult/cli/file-arguments.hxx>
#include <cult/cli/options.hxx>
#include <cult/cli/options-spec.hxx>
#include <cult/cli/options-parser.hxx>

#include <xsd-frontend/parser.hxx>
#include <xsd-frontend/transformations/anonymous.hxx>
#include <xsd-frontend/transformations/restriction.hxx>
#include <xsd-frontend/transformations/schema-per-type.hxx>
#include <xsd-frontend/transformations/simplifier.hxx>

#include <xsde.hxx>
#include <usage.hxx>
#include <type-map/type-map.hxx>

#include <backend-elements/regex.hxx>
#include <backend-elements/indentation/clip.hxx>

#include <cxx/parser/generator.hxx>
#include <cxx/serializer/generator.hxx>
#include <cxx/hybrid/generator.hxx>

#include <processing/inheritance/processor.hxx>

#include "../libxsde/xsde/cxx/version.hxx"

using namespace Cult::Types;

typedef Cult::Containers::Vector<NarrowString> NarrowStrings;

namespace SemanticGraph = XSDFrontend::SemanticGraph;
namespace Transformations = XSDFrontend::Transformations;

using std::wcerr;
using std::endl;

namespace CLI
{
  using namespace Cult::CLI;

  typedef Char const Key[];

  extern Key help                = "help";
  extern Key version             = "version";
  extern Key proprietary_license = "proprietary-license";

  typedef Cult::CLI::Options
  <
    help,                Boolean,
    version,             Boolean,
    proprietary_license, Boolean
  >
  HelpOptions;

  struct HelpOptionsSpec: Cult::CLI::OptionsSpec<HelpOptions> {};


  extern Key disable_warning       = "disable-warning";
  extern Key sloc_limit            = "sloc-limit";
  extern Key preserve_anonymous    = "preserve-anonymous";
  extern Key anonymous_regex       = "anonymous-regex";
  extern Key anonymous_regex_trace = "anonymous-regex-trace";
  extern Key location_map          = "location-map";
  extern Key location_regex        = "location-regex";
  extern Key location_regex_trace  = "location-regex-trace";
  extern Key file_per_type         = "file-per-type";
  extern Key type_file_regex       = "type-file-regex";
  extern Key type_file_regex_trace = "type-file-regex-trace";
  extern Key file_list             = "file-list";
  extern Key file_list_prologue    = "file-list-prologue";
  extern Key file_list_epilogue    = "file-list-epilogue";
  extern Key file_list_delim       = "file-list-delim";
  extern Key disable_full_check    = "disable-full-check"; // Undocumented.

  typedef Cult::CLI::Options
  <
    disable_warning,       Cult::Containers::Vector<NarrowString>,
    sloc_limit,            UnsignedLong,
    preserve_anonymous,    Boolean,
    anonymous_regex,       NarrowStrings,
    anonymous_regex_trace, Boolean,
    location_map,          NarrowStrings,
    location_regex,        NarrowStrings,
    location_regex_trace,  Boolean,
    file_per_type,         Boolean,
    type_file_regex,       NarrowStrings,
    type_file_regex_trace, Boolean,
    file_list,             NarrowString,
    file_list_prologue,    NarrowString,
    file_list_epilogue,    NarrowString,
    file_list_delim,       NarrowString,
    disable_full_check,    Boolean
  >
  CommonOptions;

  struct CommonOptionsSpec: Cult::CLI::OptionsSpec<CommonOptions> {};
}

//
//
struct LocationTranslator: XSDFrontend::LocationTranslator
{
  struct Failed {};

  LocationTranslator (NarrowStrings const& map,
                      NarrowStrings const& regex,
                      Boolean trace);

  virtual NarrowString
  translate (NarrowString const&);

private:
  typedef Cult::Containers::Map<NarrowString, NarrowString> Map;

  typedef BackendElements::Regex::Expression<Char> Regex;
  typedef BackendElements::Regex::Format<Char> RegexFormat;
  typedef Cult::Containers::Vector<Regex> RegexVector;

  typedef Cult::Containers::Map<NarrowString, NarrowString> Cache;

  Map map_;
  RegexVector regex_;
  Cache cache_;
  Boolean trace_;
};

//
//
struct AnonymousNameTranslator: Transformations::AnonymousNameTranslator
{
  struct Failed {};

  AnonymousNameTranslator (NarrowStrings const& regex, Boolean trace);

  virtual WideString
  translate (WideString const& file,
             WideString const& ns,
             WideString const& name,
             WideString const& xpath);

private:
  typedef BackendElements::Regex::Expression<WideChar> Regex;
  typedef BackendElements::Regex::Format<WideChar> RegexFormat;
  typedef Cult::Containers::Vector<Regex> RegexVector;

  RegexVector regex_;
  Boolean trace_;

};

//
//
struct TypeSchemaTranslator: Transformations::TypeSchemaTranslator
{
  struct Failed {};

  TypeSchemaTranslator (NarrowStrings const& regex, Boolean trace);

  virtual WideString
  translate (WideString const& ns, WideString const& name);

private:
  typedef BackendElements::Regex::Expression<WideChar> Regex;
  typedef BackendElements::Regex::Format<WideChar> RegexFormat;
  typedef Cult::Containers::Vector<Regex> RegexVector;

  RegexVector regex_;
  Boolean trace_;
};

// Expand the \n escape sequence.
//
Void
expand_nl (NarrowString& s);

Int
main (Int argc, Char* argv[])
{
  std::wostream& e (wcerr);

  Cult::Trace::Log::instance ().level (0);

  try
  {
    CLI::FileArguments args (argc, argv, "--options-file");

    CLI::HelpOptions help_ops (
      CLI::parse (CLI::HelpOptionsSpec (), args, CLI::UnknownMode::stop));

    NarrowString cmd;

    if (args.size () > 1)
    {
      cmd = args[1];
      args.erase (1);
    }

    if (help_ops.value<CLI::version> () || cmd == "version")
    {
      e << "CodeSynthesis XSD/e XML Schema to C++ compiler " <<
        "for embedded systems " << XSDE_STR_VERSION << endl
        << "Copyright (C) 2005-2009 Code Synthesis Tools CC" << endl;

      if (!help_ops.value<CLI::proprietary_license> () &&
          cmd == "version")
      {
        // Parse the options after the command to detect trailing
        // --proprietary-license.
        //
        help_ops = CLI::parse (
          CLI::HelpOptionsSpec (), args, CLI::UnknownMode::stop);
      }

      if (help_ops.value<CLI::proprietary_license> ())
      {
        e << "The compiler was invoked in the Proprietary License mode. You "
          << "should have\nreceived a proprietary license from Code Synthesis "
          << "Tools CC that entitles\nyou to use it in this mode." << endl;
      }
      else
      {
        e << "This is free software; see the source for copying conditions. "
          << "There is NO\nwarranty; not even for MERCHANTABILITY or FITNESS "
          << "FOR A PARTICULAR PURPOSE." << endl;
      }

      return 0;
    }

    if (help_ops.value<CLI::help> () || cmd == "help")
    {
      if (cmd == "help" && args.size () > 1)
      {
        NarrowString arg (args[1]);

        if (arg == "cxx-parser")
        {
          e << "Usage: " << args[0] << " cxx-parser [options] file [file ...]"
            << endl
            << "Options:" << endl;

          CXX::Parser::Generator::usage ();
        }
        else if (arg == "cxx-serializer")
        {
          e << "Usage: " << args[0] << " cxx-serializer [options] file " <<
            "[file ...]" << endl
            << "Options:" << endl;

          CXX::Serializer::Generator::usage ();
        }
        else if (arg == "cxx-hybrid")
        {
          e << "Usage: " << args[0] << " cxx-hybrid [options] file " <<
            "[file ...]" << endl
            << "Options:" << endl;

          CXX::Hybrid::Generator::usage ();
        }
        else
        {
          e << "error: unknown command '" << arg.c_str () << "'" << endl
            << "info: try '" << args[0] << " help' for the list of commands"
            << endl;

          return 1;
        }

        ::CLI::Indent::Clip< ::CLI::OptionsUsage, WideChar> clip (e);

        // Disable warning option.
        //
        e << "--disable-warning <warn>" << endl
          << " Disable printing warning with id <warn>. If 'all'\n"
          << " is specified for the warning id then all warnings\n"
          << " are disabled."
          << endl;

        // Anonymous morphing options.
        //
        e << "--preserve-anonymous" << endl
          << " Preserve anonymous types. By default anonymous\n"
          << " types are automatically named with names derived\n"
          << " from the enclosing elements/attributes."
          << endl;

        e << "--anonymous-regex <regex>" << endl
          << " Add the provided regular expression to the list of\n"
          << " regular expressions used to derive names for\n"
          << " anonymous types from the names of the enclosing\n"
          << " attributes/elements."
          << endl;

        e << "--anonymous-regex-trace" << endl
          << " Trace the process of applying regular expressions\n"
          << " specified with the --anonymous-regex option."
          << endl;

        // Location mapping options.
        //
        e << "--location-map <ol>=<nl>" << endl
          << " Map the original schema location <ol> that is\n"
          << " specified in the XML Schema include or import\n"
          << " elements to new schema location <nl>. Repeat\n"
          << " this option to map more than one schema location."
          << endl;

        e << "--location-regex <regex>" << endl
          << " Add <regex> to the list of regular expressions\n"
          << " used to map schema locations that are specified\n"
          << " in the XML Schema include or import elements."
          << endl;

        e << "--location-regex-trace" << endl
          << " Trace the process of applying regular expressions\n"
          << " specified with the --location-regex option."
          << endl;

        // File-per-type compilation mode options.
        //
        e << "--file-per-type" << endl
          << " Generate a separate set of C++ files for each\n"
          << " type defined in XML Schema."
          << endl;

        e << "--type-file-regex <regex>" << endl
          << " Add the provided regular expression to the list of\n"
          << " regular expressions used to translate type names\n"
          << " to file names when the --type-per-file option is\n"
          << " specified."
          << endl;

        e << "--type-file-regex-trace" << endl
          << " Trace the process of applying regular expressions\n"
          << " specified with the --type-file-regex option."
          << endl;

        // File list options.
        //
        e << "--file-list <file>" << endl
          << " Write a list of generated C++ files to <file>."
          << endl;

        e << "--file-list-prologue <p>" << endl
          << " Insert <p> at the beginning of the file list. All\n"
          << " occurrences of the \\n character sequence in <p>\n"
          << " are replaced with new lines."
          << endl;

        e << "--file-list-prologue <e>" << endl
          << " Insert <e> at the end of the file list. All\n"
          << " occurrences of the \\n character sequence in <e>\n"
          << " are replaced with new lines."
          << endl;

        e << "--file-list-delim <d>" << endl
          << " Delimit file names written to the file list with\n"
          << " <d> instead of new lines. All occurrences of the\n"
          << " \\n character sequence in <d> are replaced with\n"
          << " new lines."
          << endl;
      }
      else
      {
        e << "Usage: " << args[0] << " <cmd> ..." << endl
          << "Commands:" << endl;

        e << "  help            Print usage information and exit. Use\n"
          << "                  'help <cmd>' for command-specific options."
          << endl;

        e << "  version         Print version and exit."
          << endl;

        e << "  cxx-parser      Generate Embedded C++/Parser mapping."
          << endl;

        e << "  cxx-serializer  Generate Embedded C++/Serializer mapping."
          << endl;

        e << "  cxx-hybrid      Generate Embedded C++/Hybrid mapping."
          << endl;
      }

      return 0;
    }

    if (cmd.empty ())
    {
      e << "error: no command specified" << endl
        << "info: try '" << args[0] << " help' for usage information" << endl;

      return 1;
    }

    if (cmd != "cxx-parser" && cmd != "cxx-serializer" && cmd != "cxx-hybrid")
    {
      e << "error: unknown command '" << cmd.c_str () << "'" << endl
        << "info: try '" << args[0] << " help' for the list of commands"
        << endl;

      return 1;
    }

    // We need to parse command line options before we can get to
    // the arguments.
    //
    CLI::CommonOptionsSpec common_spec;
    common_spec.option<CLI::file_list_delim> ().default_value ("\n");

    CLI::CommonOptions common_ops (
      CLI::parse (
        common_spec,
        args,
        CLI::UnknownMode::skip,
        CLI::UnknownMode::skip));

    WarningSet disabled_w;
    {
      typedef Cult::Containers::Vector<NarrowString> Warnings;
      Warnings const& w (common_ops.value<CLI::disable_warning> ());

      for (Warnings::ConstIterator i (w.begin ()); i != w.end (); ++i)
        disabled_w.insert (*i);
    }

    Boolean disabled_w_all (disabled_w.find ("all") != disabled_w.end ());

    Evptr<CXX::Parser::CLI::Options> p_ops;
    Evptr<CXX::Serializer::CLI::Options> s_ops;
    Evptr<CXX::Hybrid::CLI::Options> h_ops;

    Boolean show_sloc (false);

    if (cmd == "cxx-parser")
    {
      p_ops = new CXX::Parser::CLI::Options (
        CLI::parse (CXX::Parser::Generator::options_spec (), args));

      show_sloc = p_ops->value<CXX::Parser::CLI::show_sloc> ();
    }
    else if (cmd == "cxx-serializer")
    {
      s_ops = new CXX::Serializer::CLI::Options (
        CLI::parse (CXX::Serializer::Generator::options_spec (), args));

      show_sloc = s_ops->value<CXX::Serializer::CLI::show_sloc> ();
    }
    else if (cmd == "cxx-hybrid")
    {
      h_ops = new CXX::Hybrid::CLI::Options (
        CLI::parse (CXX::Hybrid::Generator::options_spec (), args));

      if (h_ops->value<CXX::Hybrid::CLI::generate_parser> ())
        p_ops = CXX::Hybrid::Generator::parser_options (*h_ops);

      if (h_ops->value<CXX::Hybrid::CLI::generate_serializer> ())
        s_ops = CXX::Hybrid::Generator::serializer_options (*h_ops);

      show_sloc = h_ops->value<CXX::Hybrid::CLI::show_sloc> ();
    }

    if (args.size () < 2)
    {
      e << "error: no input file specified" << endl;
      return 1;
    }

    Boolean fpt (common_ops.value<CLI::file_per_type> ());

    // Generate/extern XML Schema checks.
    //
    if (cmd == "cxx-parser" || cmd == "cxx-serializer" || cmd == "cxx-hybrid")
    {
      Boolean gen (false), use (false);

      if (cmd == "cxx-parser")
      {
        gen = p_ops->value<CXX::Parser::CLI::generate_xml_schema> ();
        use = p_ops->value<CXX::Parser::CLI::extern_xml_schema> ();
      }
      else if (cmd == "cxx-serializer")
      {
        gen = s_ops->value<CXX::Serializer::CLI::generate_xml_schema> ();
        use = s_ops->value<CXX::Serializer::CLI::extern_xml_schema> ();
      }
      else if (cmd == "cxx-hybrid")
      {
        gen = h_ops->value<CXX::Hybrid::CLI::generate_xml_schema> ();
        use = h_ops->value<CXX::Hybrid::CLI::extern_xml_schema> ();
      }

      // Things get complicated when we are compiling several schemas at
      // once (non-file-per-type mode) and use the --generate-xml-schema/
      // --extern-xml-schema options. The only way we can figure out which
      // file corresponds to XML Schema is if the --extern-xml-schema option
      // is also present. So we are going to require it for this case,
      // especially since it generally makes sense.
      //
      if (!fpt)
      {
        if (args.size () > 2 && gen && !use)
        {
          e << "error: --extern-xml-schema is required when compiling more "
            << "than one schema and --generate-xml-schema is specified"
            << endl;

          return 1;
        }

        if (args.size () == 2 && gen && use)
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
    UnsignedLong sloc (0);

    LocationTranslator loc_translator (
      common_ops.value<CLI::location_map> (),
      common_ops.value<CLI::location_regex> (),
      common_ops.value<CLI::location_regex_trace> ());

    AnonymousNameTranslator anon_translator (
      common_ops.value<CLI::anonymous_regex> (),
      common_ops.value<CLI::anonymous_regex_trace> ());

    Boolean gen_hybrid (cmd == "cxx-hybrid");

    Boolean gen_parser (
      cmd == "cxx-parser" ||
      (gen_hybrid && h_ops->value<CXX::Hybrid::CLI::generate_parser> ()));

    Boolean gen_serializer (
      cmd == "cxx-serializer" ||
      (gen_hybrid && h_ops->value<CXX::Hybrid::CLI::generate_serializer> ()));

    Boolean poly_aggr (
      gen_hybrid &&
      h_ops->value<CXX::Hybrid::CLI::generate_polymorphic> () &&
      h_ops->value<CXX::Hybrid::CLI::generate_aggregate> ());

    if (!fpt)
    {
      // File-per-schema compilation mode.
      //
      for (Size i (1); i < args.size (); ++i)
      {
        // Parse schema.
        //
        SemanticGraph::Path tu;
        SemanticGraph::Paths paths;

        XSDFrontend::Parser parser (
          true,
          !common_ops.value<CLI::disable_full_check> (),
          loc_translator,
          disabled_w);

        Evptr<SemanticGraph::Schema> schema;

        try
        {
          tu = SemanticGraph::Path (args[i], boost::filesystem::native);
        }
        catch (SemanticGraph::InvalidPath const&)
        {
          e << "error: '" << args[i] << "' is not a valid "
            << "filesystem path" << endl;
          return 1;
        }

        // See if we are generating code for the XML Schema namespace.
        // We could be compiling several schemas at once in which case
        // handling of the --generate-xml-schema option gets tricky: we
        // will need to rely on the presence of the --extern-xml-schema
        // to tell us which (fake) schema file corresponds to XML Schema.
        //
        Boolean gen_xml_schema (false);
        NarrowString xml_schema_file;

        if (cmd == "cxx-parser" ||
            cmd == "cxx-serializer" ||
            cmd == "cxx-hybrid")
        {
          if (cmd == "cxx-parser")
          {
            gen_xml_schema =
              p_ops->value<CXX::Parser::CLI::generate_xml_schema> ();

            if (gen_xml_schema)
            {
              if (xml_schema_file =
                  p_ops->value<CXX::Parser::CLI::extern_xml_schema> ())
              {
                if (tu.native_file_string () != xml_schema_file)
                  gen_xml_schema = false;
              }
            }
          }
          else if (cmd == "cxx-serializer")
          {
            gen_xml_schema =
              s_ops->value<CXX::Serializer::CLI::generate_xml_schema> ();

            if (gen_xml_schema)
            {
              if (xml_schema_file =
                  s_ops->value<CXX::Serializer::CLI::extern_xml_schema> ())
              {
                if (tu.native_file_string () != xml_schema_file)
                  gen_xml_schema = false;
              }
            }
          }
          else if (cmd == "cxx-hybrid")
          {
            gen_xml_schema =
              h_ops->value<CXX::Hybrid::CLI::generate_xml_schema> ();

            if (gen_xml_schema)
            {
              if (xml_schema_file =
                  h_ops->value<CXX::Hybrid::CLI::extern_xml_schema> ())
              {
                if (tu.native_file_string () != xml_schema_file)
                  gen_xml_schema = false;
              }
            }
          }
        }

        // If we are generating polymorphic aggregates then we need
        // to add all the schemas to the semantic graph in case they
        // define derived polymorphic types, except for the fake XML
        // Schema file.
        //
        Boolean multi (poly_aggr && !gen_xml_schema);

        if (multi)
        {
          Size ai (1);
          paths.push_back (tu);

          try
          {
            for (; ai < args.size (); ++ai)
            {
              if (ai != i && args[ai] != xml_schema_file)
                paths.push_back (
                  SemanticGraph::Path (args[ai], boost::filesystem::native));
            }
          }
          catch (SemanticGraph::InvalidPath const&)
          {
            e << "error: '" << args[ai] << "' is not a valid "
              << "filesystem path" << endl;
            return 1;
          }

          // Also include additional schemas that may be specified with
          // the --polymorphic-schema option.
          //
          NarrowStrings const& extra_files (
            h_ops->value<CXX::Hybrid::CLI::polymorphic_schema> ());

          NarrowStrings::ConstIterator i (extra_files.begin ());

          try
          {
            for (; i != extra_files.end (); ++i)
            {
              paths.push_back (
                SemanticGraph::Path (*i, boost::filesystem::native));
            }
          }
          catch (SemanticGraph::InvalidPath const&)
          {
            e << "error: '" << i->c_str () << "' is not a valid "
              << "filesystem path" << endl;
            return 1;
          }


          if (args.size () <= 1)
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
        if (!common_ops.value<CLI::preserve_anonymous> ())
        {
          try
          {
            Transformations::Anonymous trans (anon_translator);

            if (multi)
              trans.transform (*schema, "", true);
            else
              trans.transform (*schema, tu, true);
          }
          catch (Transformations::Anonymous::Failed const&)
          {
            return 1; // Diagnostic has already been issued.
          }
        }

        // Simplify the schema graph.
        //
        {
          Transformations::Simplifier trans;

          if (multi)
            trans.transform (*schema, "");
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
                *h_ops, *schema, "", disabled_w);
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
            proc.process (*schema, "", gen_hybrid ? "fixed" : 0);
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
              trans.transform (*schema, "");
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

            if (gen_hybrid && gen_parser)
              CXX::Hybrid::Generator::process_parser_names (*h_ops, s, f);

            if (gen_hybrid && gen_serializer)
              CXX::Hybrid::Generator::process_serializer_names (*h_ops, s, f);
          }
        }
        else
          root = schema.get ();

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

      for (Size i (1); i < args.size (); ++i)
      {
        try
        {
          paths.push_back (
            SemanticGraph::Path (args[i], boost::filesystem::native));
        }
        catch (SemanticGraph::InvalidPath const&)
        {
          e << "error: '" << args[i] << "' is not a valid "
            << "filesystem path" << endl;

          return 1;
        }
      }

      if (paths.size () > 1 &&
          ((cmd == "cxx-parser" &&
            p_ops->value<CXX::Parser::CLI::generate_test_driver> ()) ||
           (cmd == "cxx-serializer" &&
            s_ops->value<CXX::Serializer::CLI::generate_test_driver> ())))
      {
        e << "info: generating test driver for the first schema only: '" <<
          paths[0] << "'" << endl;
      }

      XSDFrontend::Parser parser (
        cmd != "cxx-hybrid",
        !common_ops.value<CLI::disable_full_check> (),
        loc_translator,
        disabled_w);

      Evptr<SemanticGraph::Schema> schema (parser.parse (paths));

      // Morph anonymous types.
      //
      if (!common_ops.value<CLI::preserve_anonymous> ())
      {
        try
        {
          Transformations::Anonymous trans (anon_translator);
          trans.transform (*schema, "", false);
        }
        catch (Transformations::Anonymous::Failed const&)
        {
          return 1; // Diagnostic has already been issued.
        }
      }

      // Simplify the schema graph.
      //
      {
        Transformations::Simplifier trans;
        trans.transform (*schema, "");
      }

      // Calculate type sizes.
      //
      if (gen_hybrid)
      {
        try
        {
          CXX::Hybrid::Generator::calculate_size (
            *h_ops, *schema, "", disabled_w);
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
          trans.transform (*schema, "");
        }
        catch (Transformations::Restriction::Failed const&)
        {
          return 1; // Diagnostic has already been issued.
        }
      }

      // Rearrange the graph so that each type is in a seperate
      // schema file.
      //
      typedef Cult::Containers::Vector<SemanticGraph::Schema*> Schemas;

      TypeSchemaTranslator type_translator (
        common_ops.value<CLI::type_file_regex> (),
        common_ops.value<CLI::type_file_regex_trace> ());

      Transformations::SchemaPerType trans (
        type_translator, gen_hybrid ? "fixed" : 0);
      Schemas schemas (trans.transform (*schema));

      // Generate code.
      //
      for (Schemas::Iterator b (schemas.begin ()), i (b), e (schemas.end ());
           i != e; ++i)
      {
        SemanticGraph::Schema& s (**i);
        SemanticGraph::Path path (s.used_begin ()->path ());

        TypeMap::Namespaces parser_type_map, serializer_type_map;

        if (gen_hybrid)
        {
          try
          {
            sloc += CXX::Hybrid::Generator::generate_tree (
              *h_ops,
              s,
              path,
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
    if (NarrowString fl = common_ops.value<CLI::file_list> ())
    {
      typedef boost::filesystem::ofstream OutputFileStream;

      try
      {
        OutputFileStream ofs;
        SemanticGraph::Path path (fl);

        ofs.open (fl, std::ios_base::out);

        if (!ofs.is_open ())
        {
          wcerr << path << ": error: unable to open in write mode" << endl;
          return 1;
        }

        NarrowString d (common_ops.value<CLI::file_list_delim> ());
        expand_nl (d);

        if (NarrowString p = common_ops.value<CLI::file_list_prologue> ())
        {
          expand_nl (p);
          ofs << p;
        }

        for (FileList::Iterator i (file_list.begin ()), e (file_list.end ());
             i != e;)
        {
          ofs << *i;

          if (++i != e)
            ofs << d;
        }

        if (NarrowString e = common_ops.value<CLI::file_list_epilogue> ())
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

    if (show_sloc)
      e << "total: " << sloc << endl;

    if (UnsignedLong sloc_limit = common_ops.value<CLI::sloc_limit> ())
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
  catch (TypeSchemaTranslator::Failed const&)
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
  catch (CLI::UnexpectedOption const& e)
  {
    wcerr << "error: unknown option '" << e.option ().c_str () << "'" << endl
          << "info: try '" << argv[0] << " help' for usage information"
          << endl;
  }
  catch (CLI::OptionFormat const& e)
  {
    wcerr << "error: value for option '" << e.option ().c_str ()
          << "' is invalid or missing" << endl
          << "info: try '" << argv[0] << " help' for usage information"
          << endl;
  }
  catch (CLI::OptionFile const& e)
  {
    if (e.value ())
      wcerr << "error: " << e.value ().c_str () << ": "
            << e.description ().c_str () << endl;
    else
      wcerr << "error: missing --options-file argument" << endl;
  }

  return 1;
}

// LocationTranslator
//

LocationTranslator::
LocationTranslator (NarrowStrings const& map,
                    NarrowStrings const& regex,
                    Boolean trace)
    : trace_ (trace)
{
  // Map.
  //
  for (NarrowStrings::ConstIterator i (map.begin ()); i != map.end (); ++i)
  {
    // Split the string in two parts at the last '='.
    //
    Size pos (i->rfind ('='));

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
  for (NarrowStrings::ConstIterator i (regex.begin ()); i != regex.end (); ++i)
  {
    try
    {
      regex_.push_back (Regex (*i));
    }
    catch (RegexFormat const& e)
    {
      wcerr << "error: invalid location regex: '" <<
        e.expression ().c_str () << "': " <<
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
  Cache::ConstIterator ci (cache_.find (l));

  if (ci != cache_.end ())
    return ci->second;

  // Then check the direct map.
  //
  Map::ConstIterator mi (map_.find (l));

  if (mi != map_.end ())
  {
    cache_[l] = mi->second;
    return mi->second;
  }

  // Finally try regex.
  //
  if (trace_)
    wcerr << "location: '" << l.c_str () << "'" << endl;

  for (RegexVector::ReverseIterator i (regex_.rbegin ());
       i != regex_.rend (); ++i)
  {
    if (trace_)
      wcerr << "try: '" << i->pattern () << "' : ";

    if (i->match (l))
    {
      NarrowString r (i->merge (l));

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
AnonymousNameTranslator (NarrowStrings const& regex, Boolean trace)
    : trace_ (trace)
{
  for (NarrowStrings::ConstIterator i (regex.begin ()); i != regex.end (); ++i)
  {
    try
    {
      regex_.push_back (Regex (*i));
    }
    catch (RegexFormat const& e)
    {
      wcerr << "error: invalid anonymous type regex: '" <<
        e.expression () << "': " << e.description () << endl;

      throw Failed ();
    }
  }
}

WideString AnonymousNameTranslator::
translate (WideString const& file,
           WideString const& ns,
           WideString const& name,
           WideString const& xpath)
{
  if (regex_.empty ())
    return name;

  WideString s (file + L' ' + ns + L' ' + xpath);

  if (trace_)
    wcerr << "anonymous type: '" << s << "'" << endl;

  for (RegexVector::ReverseIterator i (regex_.rbegin ());
       i != regex_.rend (); ++i)
  {
    if (trace_)
      wcerr << "try: '" << i->pattern () << "' : ";

    if (i->match (s))
    {
      WideString r (i->merge (s));

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

// TypeSchemaTranslator
//

TypeSchemaTranslator::
TypeSchemaTranslator (NarrowStrings const& regex, Boolean trace)
    : trace_ (trace)
{
  for (NarrowStrings::ConstIterator i (regex.begin ()); i != regex.end (); ++i)
  {
    try
    {
      regex_.push_back (Regex (*i));
    }
    catch (RegexFormat const& e)
    {
      wcerr << "error: invalid type file regex: '" <<
        e.expression () << "': " << e.description () << endl;

      throw Failed ();
    }
  }
}

WideString TypeSchemaTranslator::
translate (WideString const& ns, WideString const& name)
{
  if (regex_.empty ())
    return name;

  WideString s (ns + L' ' + name);

  if (trace_)
    wcerr << "type: '" << s << "'" << endl;

  for (RegexVector::ReverseIterator i (regex_.rbegin ());
       i != regex_.rend (); ++i)
  {
    if (trace_)
      wcerr << "try: '" << i->pattern () << "' : ";

    if (i->match (s))
    {
      WideString r (i->merge (s));

      if (trace_)
        wcerr << "'" << r << "' : +" << endl;

      return r;
    }

    if (trace_)
      wcerr << '-' << endl;
  }

  // No match - return the type name.
  //
  return name;
}

//
//
Void
expand_nl (NarrowString& s)
{
  for (Size i (0); i < s.size ();)
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
