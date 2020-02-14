// file      : xsde/cxx/parser/generator.cxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <algorithm>
#include <iostream>
#include <fstream>

#include <cutl/re.hxx>

#include <cutl/compiler/code-stream.hxx>
#include <cutl/compiler/cxx-indenter.hxx>
#include <cutl/compiler/sloc-counter.hxx>

#include <xsd-frontend/semantic-graph.hxx>

#include <type-map/lexer.hxx>
#include <type-map/parser.hxx>

#include <cxx/parser/elements.hxx>
#include <cxx/parser/generator.hxx>

#include <cxx/parser/validator.hxx>
#include <cxx/parser/name-processor.hxx>
#include <cxx/parser/state-processor.hxx>
#include <cxx/parser/type-processor.hxx>

#include <cxx/parser/parser-forward.hxx>
#include <cxx/parser/parser-header.hxx>
#include <cxx/parser/parser-inline.hxx>
#include <cxx/parser/parser-source.hxx>

#include <cxx/parser/impl-header.hxx>
#include <cxx/parser/impl-source.hxx>
#include <cxx/parser/driver-source.hxx>

#include <cxx/parser/element-validation-source.hxx>
#include <cxx/parser/attribute-validation-source.hxx>
#include <cxx/parser/characters-validation-source.hxx>

#include <cxx/parser/options.hxx>

#include "../../../libxsde/xsde/cxx/version.hxx"

using std::endl;
using std::wcerr;
using std::wcout;

using namespace XSDFrontend::SemanticGraph;

//
//
typedef std::wifstream WideInputFileStream;
typedef std::wofstream WideOutputFileStream;
typedef std::ifstream NarrowInputFileStream;

namespace CXX
{
  namespace
  {
    char const copyright_gpl[] =
    "// Copyright (c) 2005-2017 Code Synthesis Tools CC\n"
    "//\n"
    "// This program was generated by CodeSynthesis XSD/e, an XML Schema\n"
    "// to C++ data binding compiler for embedded systems.\n"
    "//\n"
    "// This program is free software; you can redistribute it and/or modify\n"
    "// it under the terms of the GNU General Public License version 2 as\n"
    "// published by the Free Software Foundation.\n"
    "//\n"
    "// This program is distributed in the hope that it will be useful,\n"
    "// but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "// GNU General Public License for more details.\n"
    "//\n"
    "// You should have received a copy of the GNU General Public License\n"
    "// along with this program; if not, write to the Free Software\n"
    "// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA\n"
    "//\n"
    "//\n\n";

    char const copyright_proprietary[] =
    "// Copyright (c) 2005-2017 Code Synthesis Tools CC\n"
    "//\n"
    "// This program was generated by CodeSynthesis XSD/e, an XML Schema to\n"
    "// C++ data binding compiler for embedded systems, in the Proprietary\n"
    "// License mode. You should have received a proprietary license from\n"
    "// Code Synthesis Tools CC prior to generating this code. See the\n"
    "// license text for conditions.\n"
    "//\n\n";

    char const copyright_impl[] =
    "// Not copyrighted - public domain.\n"
    "//\n"
    "// This sample parser implementation was generated by CodeSynthesis XSD/e,\n"
    "// an XML Schema to C++ data binding compiler for embedded systems. You\n"
    "// may use it in your programs without any restrictions.\n"
    "//\n\n";
  }

  void Parser::Generator::
  usage ()
  {
    CXX::Parser::options::print_usage (wcout);
    CXX::options::print_usage (wcout);
  }

  void Parser::Generator::
  process_names (options const& ops,
                 XSDFrontend::SemanticGraph::Schema& schema,
                 XSDFrontend::SemanticGraph::Path const& file)
  {
    NameProcessor proc;
    proc.process (ops, schema, file, false);
  }

  namespace
  {
    template <typename S>
    void
    open (S& ifs, NarrowString const& path)
    {
      try
      {
        Path fs_path (path);
        ifs.open (fs_path.string ().c_str (),
                  std::ios_base::in | std::ios_base::binary);

        if (!ifs.is_open ())
        {
          wcerr << path.c_str () << ": error: unable to open in read mode"
                << endl;

          throw Parser::Generator::Failed ();
        }
      }
      catch (InvalidPath const&)
      {
        wcerr << "error: '" << path.c_str () << "' is not a valid "
              << "filesystem path" << endl;

        throw Parser::Generator::Failed ();
      }
    }

    void
    append (WideOutputFileStream& os,
            NarrowString const& path,
            WideInputFileStream& default_is)
    {
      using std::ios_base;

      if (path)
      {
        WideInputFileStream is;
        open (is, path);
        os << is.rdbuf ();
      }
      else if (default_is.is_open ())
      {
        os << default_is.rdbuf ();
        default_is.seekg (0, ios_base::beg);
      }
    }

    void
    append (WideOutputFileStream& os,
            NarrowStrings const& primary,
            NarrowStrings const& def)
    {
      NarrowStrings const& v (primary.empty () ? def : primary);

      for (NarrowStrings::const_iterator i (v.begin ()), e (v.end ());
           i != e; ++i)
      {
        os << i->c_str () << endl;
      }
    }
  }


  size_t Parser::Generator::
  generate (Parser::options const& ops,
            Schema& schema,
            Path const& file_path,
            bool fpt,
            TypeMap::Namespaces& type_map,
            bool gen_driver,
            const WarningSet& disabled_warnings,
            FileList& file_list,
            AutoUnlinks& unlinks)
  {
    using std::ios_base;

    typedef cutl::re::regexsub Regex;

    try
    {
      bool generate_xml_schema (ops.generate_xml_schema ());

      // We could be compiling several schemas at once in which case
      // handling of the --generate-xml-schema option gets tricky: we
      // will need to rely on the presence of the --extern-xml-schema
      // to tell us which (fake) schema file corresponds to XML Schema.
      //
      if (generate_xml_schema)
      {
        if (NarrowString name = ops.extern_xml_schema ())
        {
          if (file_path.string () != name)
            generate_xml_schema = false;
        }
      }

      bool impl (!generate_xml_schema &&
                    (ops.generate_noop_impl () ||
                     ops.generate_print_impl ()));

      bool driver (gen_driver && !generate_xml_schema &&
                      ops.generate_test_driver ());


      // Evaluate the graph for possibility of generating something useful.
      //
      {
        Validator validator;
        if (!validator.validate (
              ops, schema, file_path, driver, disabled_warnings))
          throw Failed ();
      }

      // Process names.
      //
      {
        NameProcessor proc;
        proc.process (ops, schema, file_path, true);
      }

      bool validation (!ops.suppress_validation ());

      // Compute state machine info.
      //
      if (validation)
      {
        StateProcessor proc;
        proc.process (schema, file_path);
      }

      // Read-in type maps.
      //
      {
        using namespace TypeMap;

        NarrowStrings const& files (ops.type_map ());

        for (NarrowStrings::const_iterator f (files.begin ());
             f != files.end (); ++f )
        {
          NarrowInputFileStream ifs;
          open (ifs, *f);

          Lexer l (ifs, *f);
          TypeMap::Parser p (l, *f);

          if (!p.parse (type_map))
            throw Failed ();
        }

        // Add the built-in mappings at the end.
        //
        String xns;
        {
          Context ctx (std::wcerr, schema, file_path, ops, 0, 0, 0);
          xns = ctx.xs_ns_name ();
        }

        if (ops.no_stl ())
        {
          TypeMap::Namespace xsd_std ("http://www\\.w3\\.org/2001/XMLSchema");

          String qname (xns + L"::qname*");
          String string_seq (xns + L"::string_sequence*");

          xsd_std.types_push_back ("anySimpleType", "char*", "char*");

          xsd_std.types_push_back ("string", "char*", "char*");
          xsd_std.types_push_back ("normalizedString", "char*", "char*");
          xsd_std.types_push_back ("token", "char*", "char*");
          xsd_std.types_push_back ("Name", "char*", "char*");
          xsd_std.types_push_back ("NMTOKEN", "char*", "char*");
          xsd_std.types_push_back ("NMTOKENS", string_seq, string_seq);
          xsd_std.types_push_back ("NCName", "char*", "char*");

          xsd_std.types_push_back ("ID", "char*", "char*");
          xsd_std.types_push_back ("IDREF", "char*", "char*");
          xsd_std.types_push_back ("IDREFS", string_seq, string_seq);

          xsd_std.types_push_back ("language", "char*", "char*");
          xsd_std.types_push_back ("anyURI", "char*", "char*");
          xsd_std.types_push_back ("QName", qname, qname);

          type_map.push_back (xsd_std);
        }
        else
        {
          TypeMap::Namespace xsd_std ("http://www\\.w3\\.org/2001/XMLSchema");

          String qname (xns + L"::qname");
          String string_seq (xns + L"::string_sequence*");

          xsd_std.types_push_back ("anySimpleType", "::std::string");

          xsd_std.types_push_back ("string", "::std::string");
          xsd_std.types_push_back ("normalizedString", "::std::string");
          xsd_std.types_push_back ("token", "::std::string");
          xsd_std.types_push_back ("Name", "::std::string");
          xsd_std.types_push_back ("NMTOKEN", "::std::string");
          xsd_std.types_push_back ("NMTOKENS", string_seq, string_seq);
          xsd_std.types_push_back ("NCName", "::std::string");

          xsd_std.types_push_back ("ID", "::std::string");
          xsd_std.types_push_back ("IDREF", "::std::string");
          xsd_std.types_push_back ("IDREFS", string_seq, string_seq);

          xsd_std.types_push_back ("language", "::std::string");
          xsd_std.types_push_back ("anyURI", "::std::string");
          xsd_std.types_push_back ("QName", qname);

          type_map.push_back (xsd_std);
        }

        String buffer (xns + L"::buffer*");

        TypeMap::Namespace xsd ("http://www\\.w3\\.org/2001/XMLSchema");

        xsd.types_push_back ("boolean", "bool", "bool");

        xsd.types_push_back ("byte", "signed char", "signed char");
        xsd.types_push_back ("unsignedByte", "unsigned char", "unsigned char");

        xsd.types_push_back ("short", "short", "short");
        xsd.types_push_back ("unsignedShort", "unsigned short", "unsigned short");

        xsd.types_push_back ("int", "int", "int");
        xsd.types_push_back ("unsignedInt", "unsigned int", "unsigned int");

        if (ops.no_long_long ())
        {
          xsd.types_push_back ("long", "long", "long");
          xsd.types_push_back ("unsignedLong", "unsigned long", "unsigned long");
        }
        else
        {
          xsd.types_push_back ("long", "long long", "long long");
          xsd.types_push_back ("unsignedLong", "unsigned long long", "unsigned long long");
        }

        xsd.types_push_back ("integer", "long", "long");

        xsd.types_push_back ("negativeInteger", "long", "long");
        xsd.types_push_back ("nonPositiveInteger", "long", "long");

        xsd.types_push_back ("positiveInteger", "unsigned long", "unsigned long");
        xsd.types_push_back ("nonNegativeInteger", "unsigned long", "unsigned long");

        xsd.types_push_back ("float", "float", "float");
        xsd.types_push_back ("double", "double", "double");
        xsd.types_push_back ("decimal", "double", "double");

        xsd.types_push_back ("base64Binary", buffer, buffer);
        xsd.types_push_back ("hexBinary", buffer, buffer);

        xsd.types_push_back ("gDay", xns + L"::gday");
        xsd.types_push_back ("gMonth", xns + L"::gmonth");
        xsd.types_push_back ("gYear", xns + L"::gyear");
        xsd.types_push_back ("gMonthDay", xns + L"::gmonth_day");
        xsd.types_push_back ("gYearMonth", xns + L"::gyear_month");
        xsd.types_push_back ("date", xns + L"::date");
        xsd.types_push_back ("time", xns + L"::time");
        xsd.types_push_back ("dateTime", xns + L"::date_time");
        xsd.types_push_back ("duration", xns + L"::duration");

        type_map.push_back (xsd);

        // Everyhting else maps to void.
        //
        TypeMap::Namespace rest (".*");
        rest.types_push_back (".*", "void", "void");
        type_map.push_back (rest);
      }

      // Process types.
      //
      {
        TypeProcessor proc;
        proc.process (ops, schema, type_map);
      }

      // Generate code.
      //
      bool inline_ (ops.generate_inline () && !generate_xml_schema);
      bool source (!generate_xml_schema);

      NarrowString name (file_path.leaf ().string ());
      NarrowString skel_suffix (ops.skel_file_suffix ());
      NarrowString impl_suffix (ops.impl_file_suffix ());

      NarrowString hxx_suffix (ops.hxx_suffix ());
      NarrowString ixx_suffix (ops.ixx_suffix ());
      NarrowString cxx_suffix (ops.cxx_suffix ());

      Regex hxx_expr (
        ops.hxx_regex ().empty ()
        ? "#^(.+?)(\\.[^./\\\\]+)?$#$1" + skel_suffix + hxx_suffix + "#"
        : ops.hxx_regex ());

      Regex ixx_expr (
        ops.ixx_regex ().empty ()
        ? "#^(.+?)(\\.[^./\\\\]+)?$#$1" + skel_suffix + ixx_suffix + "#"
        : ops.ixx_regex ());

      Regex cxx_expr (
        ops.cxx_regex ().empty ()
        ? "#^(.+?)(\\.[^./\\\\]+)?$#$1" + skel_suffix + cxx_suffix + "#"
        : ops.cxx_regex ());

      Regex hxx_impl_expr;
      Regex cxx_impl_expr;
      Regex cxx_driver_expr;

      if (impl || driver)
      {
        hxx_impl_expr =
          "#^(.+?)(\\.[^./\\\\]+)?$#$1" + impl_suffix + hxx_suffix + "#";

        cxx_impl_expr =
          "#^(.+?)(\\.[^./\\\\]+)?$#$1" + impl_suffix + cxx_suffix + "#";

        cxx_driver_expr =
          "#^(.+?)(\\.[^./\\\\]+)?$#$1-pdriver" + cxx_suffix + "#";
      }

      if (!hxx_expr.match (name))
      {
        wcerr << "error: header expression '" <<
          hxx_expr.regex ().str ().c_str () << "' does not match '" <<
          name.c_str () << "'" << endl;
        throw Failed ();
      }

      if (inline_ && !ixx_expr.match (name))
      {
        wcerr << "error: inline expression '" <<
          ixx_expr.regex ().str ().c_str () << "' does not match '" <<
          name.c_str () << "'" << endl;
        throw Failed ();
      }

      if (source && !cxx_expr.match (name))
      {
        wcerr << "error: source expression '" <<
          cxx_expr.regex ().str ().c_str () << "' does not match '" <<
          name.c_str () << "'" << endl;
        throw Failed ();
      }

      if (impl || driver)
      {
        if (!hxx_impl_expr.match (name))
        {
          wcerr << "error: implementation header expression '" <<
            hxx_impl_expr.regex ().str ().c_str () << "' does not match '" <<
            name.c_str () << "'" << endl;
          throw Failed ();
        }

        if (!cxx_impl_expr.match (name))
        {
          wcerr << "error: implementation source expression '" <<
            cxx_impl_expr.regex ().str ().c_str () << "' does not match '" <<
            name.c_str () << "'" << endl;
          throw Failed ();
        }

        if (!cxx_driver_expr.match (name))
        {
          wcerr << "error: driver source expression '" <<
            cxx_driver_expr.regex ().str ().c_str () << "' does not match '" <<
            name.c_str () << "'" << endl;
          throw Failed ();
        }
      }

      NarrowString hxx_name (hxx_expr.replace (name));
      NarrowString ixx_name (inline_ ? ixx_expr.replace (name) : NarrowString ());
      NarrowString cxx_name (source ? cxx_expr.replace (name) : NarrowString ());

      NarrowString hxx_impl_name;
      NarrowString cxx_impl_name;
      NarrowString cxx_driver_name;

      if (impl || driver)
      {
        hxx_impl_name = hxx_impl_expr.replace (name);
        cxx_impl_name = cxx_impl_expr.replace (name);
        cxx_driver_name = cxx_driver_expr.replace (name);
      }

      Path hxx_path (hxx_name);
      Path ixx_path (ixx_name);
      Path cxx_path (cxx_name);

      Path hxx_impl_path;
      Path cxx_impl_path;
      Path cxx_driver_path;

      if (impl || driver)
      {
        hxx_impl_path = Path (hxx_impl_name);
        cxx_impl_path = Path (cxx_impl_name);
        cxx_driver_path = Path (cxx_driver_name);
      }

      Path out_dir;

      if (NarrowString dir = ops.output_dir ())
      {
        try
        {
          out_dir = Path (dir);
        }
        catch (InvalidPath const&)
        {
          wcerr << dir.c_str () << ": error: invalid path" << endl;
          throw Failed ();
        }
      }

      if (fpt && !generate_xml_schema)
      {
        // In the file-per-type mode the schema files are always local
        // unless the user added the directory so that we propagate this
        // to the output files.
        //
        Path fpt_dir (file_path.directory ());

        if (!fpt_dir.empty ())
          out_dir /= fpt_dir;
      }

      if (!out_dir.empty ())
      {
        hxx_path = out_dir / hxx_path;
        ixx_path = out_dir / ixx_path;
        cxx_path = out_dir / cxx_path;

        if (impl || driver)
        {
          hxx_impl_path = out_dir / hxx_impl_path;
          cxx_impl_path = out_dir / cxx_impl_path;
          cxx_driver_path = out_dir /cxx_driver_path;
        }
      }

      // Open the impl files first so that if open fails, the skel files
      // are not deleted.
      //
      WideOutputFileStream hxx_impl;
      WideOutputFileStream cxx_impl;
      WideOutputFileStream cxx_driver;

      if (impl)
      {
        if (!ops.force_overwrite ())
        {
          WideInputFileStream tmp (
            hxx_impl_path.string ().c_str (), ios_base::in);

          if (tmp.is_open ())
          {
            wcerr << hxx_impl_path << ": error: cowardly refusing to " <<
              "overwrite an existing file" << endl;
            throw Failed ();
          }

          tmp.close ();
        }

        hxx_impl.open (hxx_impl_path.string ().c_str (), ios_base::out);

        if (!hxx_impl.is_open ())
        {
          wcerr << hxx_impl_path << ": error: unable to open in write mode"
                << endl;
          throw Failed ();
        }

        unlinks.add (hxx_impl_path);
        file_list.push_back (hxx_impl_path.string ());

        if (!ops.force_overwrite ())
        {
          WideInputFileStream tmp (
            cxx_impl_path.string ().c_str (), ios_base::in);

          if (tmp.is_open ())
          {
            wcerr << cxx_impl_path << ": error: cowardly refusing to " <<
              "overwrite an existing file" << endl;
            throw Failed ();
          }

          tmp.close ();
        }

        cxx_impl.open (cxx_impl_path.string ().c_str (), ios_base::out);

        if (!cxx_impl.is_open ())
        {
          wcerr << cxx_impl_path << ": error: unable to open in write mode"
                << endl;
          throw Failed ();
        }

        unlinks.add (cxx_impl_path);
        file_list.push_back (cxx_impl_path.string ());
      }

      if (driver)
      {
        if (!ops.force_overwrite ())
        {
          WideInputFileStream tmp (
            cxx_driver_path.string ().c_str (), ios_base::in);

          if (tmp.is_open ())
          {
            wcerr << cxx_driver_path << ": error: cowardly refusing to " <<
              "overwrite an existing file" << endl;
            throw Failed ();
          }

          tmp.close ();
        }

        cxx_driver.open (cxx_driver_path.string ().c_str (), ios_base::out);

        if (!cxx_driver.is_open ())
        {
          wcerr << cxx_driver_path << ": error: unable to open in write " <<
            "mode" << endl;
          throw Failed ();
        }

        unlinks.add (cxx_driver_path);
        file_list.push_back (cxx_driver_path.string ());
      }

      // Open the skel files.
      //
      WideOutputFileStream hxx (hxx_path.string ().c_str (), ios_base::out);
      WideOutputFileStream ixx;
      WideOutputFileStream cxx;

      if (!hxx.is_open ())
      {
        wcerr << hxx_path << ": error: unable to open in write mode" << endl;
        throw Failed ();
      }

      unlinks.add (hxx_path);
      file_list.push_back (hxx_path.string ());

      if (inline_)
      {
        ixx.open (ixx_path.string ().c_str (), ios_base::out);

        if (!ixx.is_open ())
        {
          wcerr << ixx_path << ": error: unable to open in write mode" << endl;
          throw Failed ();
        }

        unlinks.add (ixx_path);
        file_list.push_back (ixx_path.string ());
      }

      if (source)
      {
        cxx.open (cxx_path.string ().c_str (), ios_base::out);

        if (!cxx.is_open ())
        {
          wcerr << cxx_path << ": error: unable to open in write mode" << endl;
          throw Failed ();
        }

        unlinks.add (cxx_path);
        file_list.push_back (cxx_path.string ());
      }

      // Print copyright and license.
      //
      char const* copyright (
        ops.proprietary_license () ? copyright_proprietary : copyright_gpl);

      hxx << copyright;

      if (inline_)
        ixx << copyright;

      if (source)
        cxx << copyright;

      if (impl)
      {
        hxx_impl << copyright_impl;
        cxx_impl << copyright_impl;
      }

      if (driver)
        cxx_driver << copyright_impl;

      // Prologue.
      //
      WideInputFileStream prologue;
      {
        NarrowString name (ops.prologue_file ());

        if (name)
          open (prologue, name);
      }

      // Epilogue.
      //
      WideInputFileStream epilogue;
      {
        NarrowString name (ops.epilogue_file ());

        if (name)
          open (epilogue, name);
      }

      // SLOC counter.
      //
      size_t sloc_total (0);
      bool show_sloc (ops.show_sloc ());

      typedef
      compiler::ostream_filter<compiler::cxx_indenter, wchar_t>
      ind_filter;

      typedef
      compiler::ostream_filter<compiler::sloc_counter, wchar_t>
      sloc_filter;

      //
      //
      Regex guard_expr ("/([a-z])([A-Z])/$1_$2/"); // Split words.

      NarrowString guard_prefix (ops.guard_prefix ());

      if (!guard_prefix)
        guard_prefix = file_path.directory ().string ();

      if (guard_prefix)
        guard_prefix += '_';

      // HXX
      //
      {
        Context ctx (
          hxx, schema, file_path, ops, &hxx_expr, &ixx_expr, &hxx_impl_expr);

        sloc_filter sloc (hxx);

        String guard (guard_expr.replace (guard_prefix + hxx_name));
        guard = ctx.escape (guard); // Make it a C++ id.
        std::transform (guard.begin (), guard.end(), guard.begin (), upcase);

        hxx << "#ifndef " << guard << endl
            << "#define " << guard << endl
            << endl;

        // Copy prologue.
        //
        hxx << "// Begin prologue." << endl
            << "//" << endl;

        append (hxx, ops.hxx_prologue (), ops.prologue ());
        append (hxx, ops.hxx_prologue_file (), prologue);

        hxx << "//" << endl
            << "// End prologue." << endl
            << endl;

        // Version check.
        //
        hxx << "#include <xsde/cxx/version.hxx>" << endl
            << endl
            << "#if (XSDE_INT_VERSION != " << XSDE_INT_VERSION << "L)" << endl
            << "#error XSD/e runtime version mismatch" << endl
            << "#endif" << endl
            << endl;

        // Runtime/generated code compatibility checks.
        //

        hxx << "#include <xsde/cxx/config.hxx>" << endl
            << endl;

        if (ops.char_encoding () == "iso8859-1")
        {
          hxx << "#ifndef XSDE_ENCODING_ISO8859_1" << endl
              << "#error the generated code uses the ISO-8859-1 encoding " <<
            "while the XSD/e runtime does not (reconfigure the runtime " <<
            "or change the --char-encoding value)" << endl
              << "#endif" << endl
              << endl;
        }
        else
        {
          hxx << "#ifndef XSDE_ENCODING_UTF8" << endl
              << "#error the generated code uses the UTF-8 encoding " <<
            "while the XSD/e runtime does not (reconfigure the runtime " <<
            "or change the --char-encoding value)" << endl
              << "#endif" << endl
              << endl;
        }

        if (ops.no_stl ())
        {
          hxx << "#ifdef XSDE_STL" << endl
              << "#error the XSD/e runtime uses STL while the " <<
            "generated code does not (reconfigure the runtime or " <<
            "remove --no-stl)" << endl
              << "#endif" << endl
              << endl;
        }
        else
        {
          hxx << "#ifndef XSDE_STL" << endl
              << "#error the generated code uses STL while the " <<
            "XSD/e runtime does not (reconfigure the runtime or " <<
            "add --no-stl)" << endl
              << "#endif" << endl
              << endl;
        }

        if (ops.no_iostream ())
        {
          hxx << "#ifdef XSDE_IOSTREAM" << endl
              << "#error the XSD/e runtime uses iostream while the " <<
            "generated code does not (reconfigure the runtime or " <<
            "remove --no-iostream)" << endl
              << "#endif" << endl
              << endl;
        }
        else
        {
          hxx << "#ifndef XSDE_IOSTREAM" << endl
              << "#error the generated code uses iostream while the " <<
            "XSD/e runtime does not (reconfigure the runtime or " <<
            "add --no-iostream)" << endl
              << "#endif" << endl
              << endl;
        }

        if (ops.no_exceptions ())
        {
          hxx << "#ifdef XSDE_EXCEPTIONS" << endl
              << "#error the XSD/e runtime uses exceptions while the " <<
            "generated code does not (reconfigure the runtime or " <<
            "remove --no-exceptions)" << endl
              << "#endif" << endl
              << endl;
        }
        else
        {
          hxx << "#ifndef XSDE_EXCEPTIONS" << endl
              << "#error the generated code uses exceptions while the " <<
            "XSD/e runtime does not (reconfigure the runtime or " <<
            "add --no-exceptions)" << endl
              << "#endif" << endl
              << endl;
        }

        if (ops.no_long_long ())
        {
          hxx << "#ifdef XSDE_LONGLONG" << endl
              << "#error the XSD/e runtime uses long long while the " <<
            "generated code does not (reconfigure the runtime or " <<
            "remove --no-long-long)" << endl
              << "#endif" << endl
              << endl;
        }
        else
        {
          hxx << "#ifndef XSDE_LONGLONG" << endl
              << "#error the generated code uses long long while the " <<
            "XSD/e runtime does not (reconfigure the runtime or " <<
            "add --no-long-long)" << endl
              << "#endif" << endl
              << endl;
        }

        if (ops.suppress_validation ())
        {
          hxx << "#ifdef XSDE_PARSER_VALIDATION" << endl
              << "#error the XSD/e runtime uses validation while the " <<
            "generated code does not (reconfigure the runtime or " <<
            "remove --suppress-validation)" << endl
              << "#endif" << endl
              << endl;
        }
        else
        {
          hxx << "#ifndef XSDE_PARSER_VALIDATION" << endl
              << "#error the generated code uses validation while the " <<
            "XSD/e runtime does not (reconfigure the runtime or " <<
            "add --suppress-validation)" << endl
              << "#endif" << endl
              << endl;
        }

        if (ops.generate_polymorphic () || ops.runtime_polymorphic ())
        {
          hxx << "#ifndef XSDE_POLYMORPHIC" << endl
              << "#error the generated code expects XSD/e runtime with " <<
            "polymorphism support (reconfigure the runtime or remove " <<
            "--generate-polymorphic/--runtime-polymorphic)" << endl
              << "#endif" << endl
              << endl;
        }
        else
        {
          hxx << "#ifdef XSDE_POLYMORPHIC" << endl
              << "#error the generated code expects XSD/e runtime " <<
            "without polymorphism support (reconfigure the runtime or " <<
            "add --generate-polymorphic/--runtime-polymorphic)" << endl
              << "#endif" << endl
              << endl;
        }

        if (ops.reuse_style_mixin ())
        {
          hxx << "#ifndef XSDE_REUSE_STYLE_MIXIN" << endl
              << "#error the generated code uses the mixin reuse style " <<
            "while the XSD/e runtime does not (reconfigure the runtime " <<
            "or remove --reuse-style-mixin)" << endl
              << "#endif" << endl
              << endl;
        }
        else if (ops.reuse_style_none ())
        {
          hxx << "#ifndef XSDE_REUSE_STYLE_NONE" << endl
              << "#error the generated code does not provide support " <<
            "for parser reuse while the XSD/e runtime does (reconfigure " <<
            "the runtime or remove --reuse-style-none)" << endl
              << "#endif" << endl
              << endl;
        }
        else
        {
          hxx << "#ifndef XSDE_REUSE_STYLE_TIEIN" << endl
              << "#error the generated code uses the tiein reuse style " <<
            "while the XSD/e runtime does not (reconfigure the runtime " <<
            "or add --reuse-style-mixin or --reuse-style-none)" << endl
              << "#endif" << endl
              << endl;
        }

        if (ops.custom_allocator ())
        {
          hxx << "#ifndef XSDE_CUSTOM_ALLOCATOR" << endl
              << "#error the generated code uses custom allocator while " <<
            "the XSD/e runtime does not (reconfigure the runtime or " <<
            "remove --custom-allocator)" << endl
              << "#endif" << endl
              << endl;
        }
        else
        {
          hxx << "#ifdef XSDE_CUSTOM_ALLOCATOR" << endl
              << "#error the XSD/e runtime uses custom allocator while " <<
            "the generated code does not (reconfigure the runtime or " <<
            "add --custom-allocator)" << endl
              << "#endif" << endl
              << endl;
        }

        hxx << "#include <xsde/cxx/pre.hxx>" << endl
            << endl;

        // Generate.
        //
        {
          ind_filter ind (hxx); // We don't want to indent prologues/epilogues.

          if (!generate_xml_schema)
            generate_parser_forward (ctx);

          generate_parser_header (ctx, generate_xml_schema);
        }

        if (inline_)
          hxx << "#include " << ctx.process_include_path (ixx_name) << endl
              << endl;

        hxx << "#include <xsde/cxx/post.hxx>" << endl
            << endl;

        // Copy epilogue.
        //
        hxx << "// Begin epilogue." << endl
            << "//" << endl;

        append (hxx, ops.hxx_epilogue_file (), epilogue);
        append (hxx, ops.hxx_epilogue (), ops.epilogue ());

        hxx << "//" << endl
            << "// End epilogue." << endl
            << endl;

        hxx << "#endif // " << guard << endl;

        if (show_sloc)
          wcerr << hxx_path << ": " << sloc.stream ().count () << endl;

        sloc_total += sloc.stream ().count ();
      }

      // IXX
      //
      if (inline_)
      {
        Context ctx (
          ixx, schema, file_path, ops, &hxx_expr, &ixx_expr, &hxx_impl_expr);

        sloc_filter sloc (ixx);

        // Copy prologue.
        //
        ixx << "// Begin prologue." << endl
            << "//" << endl;

        append (ixx, ops.ixx_prologue (), ops.prologue ());
        append (ixx, ops.ixx_prologue_file (), prologue);

        ixx << "//" << endl
            << "// End prologue." << endl
            << endl;

        // Generate.
        //
        {
          ind_filter ind (ixx); // We don't want to indent prologues/epilogues.
          generate_parser_inline (ctx);
        }

        // Copy epilogue.
        //
        ixx << "// Begin epilogue." << endl
            << "//" << endl;

        append (ixx, ops.ixx_epilogue_file (), epilogue);
        append (ixx, ops.ixx_epilogue (), ops.epilogue ());

        ixx << "//" << endl
            << "// End epilogue." << endl
            << endl;

        if (show_sloc)
          wcerr << ixx_path << ": " << sloc.stream ().count () << endl;

        sloc_total += sloc.stream ().count ();
      }


      // CXX
      //
      if (source)
      {
        Context ctx (
          cxx, schema, file_path, ops, &hxx_expr, &ixx_expr, &hxx_impl_expr);

        sloc_filter sloc (cxx);

        // Copy prologue.
        //
        cxx << "// Begin prologue." << endl
            << "//" << endl;

        append (cxx, ops.cxx_prologue (), ops.prologue ());
        append (cxx, ops.cxx_prologue_file (), prologue);

        cxx << "//" << endl
            << "// End prologue." << endl
            << endl;

        cxx << "#include <xsde/cxx/pre.hxx>" << endl
            << endl;

        cxx << "#include " << ctx.process_include_path (hxx_name) << endl
              << endl;

        // Generate.
        //
        {
          ind_filter ind (cxx); // We don't want to indent prologues/epilogues.

          if (!inline_)
            generate_parser_inline (ctx);

          generate_parser_source (ctx);

          if (validation)
          {
            generate_element_validation_source (ctx);
            generate_attribute_validation_source (ctx);
            generate_characters_validation_source (ctx);
          }
        }

        cxx << "#include <xsde/cxx/post.hxx>" << endl
            << endl;

        // Copy epilogue.
        //
        cxx << "// Begin epilogue." << endl
            << "//" << endl;

        append (cxx, ops.cxx_epilogue_file (), epilogue);
        append (cxx, ops.cxx_epilogue (), ops.epilogue ());

        cxx << "//" << endl
            << "// End epilogue." << endl
            << endl;

        if (show_sloc)
          wcerr << cxx_path << ": " << sloc.stream ().count () << endl;

        sloc_total += sloc.stream ().count ();
      }

      // HXX impl
      //
      if (impl)
      {
        Context ctx (hxx_impl, schema, file_path, ops,
                     &hxx_expr, &ixx_expr, &hxx_impl_expr);

        String guard (guard_expr.replace (guard_prefix + hxx_impl_name));
        guard = ctx.escape (guard); // Make it a C++ id.
        std::transform (guard.begin (), guard.end(), guard.begin (), upcase);

        hxx_impl << "#ifndef " << guard << endl
                 << "#define " << guard << endl
                 << endl;

        hxx_impl << "#include " << ctx.process_include_path (hxx_name)
                 << endl << endl;

        {
          ind_filter ind (hxx_impl);
          generate_impl_header (ctx);
        }

        hxx_impl << "#endif // " << guard << endl;
      }

      // CXX impl
      //
      if (impl)
      {
        Context ctx (cxx_impl, schema, file_path, ops,
                     &hxx_expr, &ixx_expr, &hxx_impl_expr);

        cxx_impl << "#include " << ctx.process_include_path (hxx_impl_name)
                 << endl << endl;

        {
          ind_filter ind (cxx_impl);
          generate_impl_source (ctx);
        }
      }

      // CXX driver
      //
      if (driver)
      {
        Context ctx (cxx_driver, schema, file_path, ops,
                     &hxx_expr, &ixx_expr, &hxx_impl_expr);

        cxx_driver << "#include " << ctx.process_include_path (hxx_impl_name)
                   << endl << endl;

        {
          ind_filter ind (cxx_driver);
          generate_driver_source (ctx);
        }
      }

      return sloc_total;
    }
    catch (UnrepresentableCharacter const& e)
    {
      wcerr << "error: character at position " << e.position () << " "
            << "in string '" << e.string () << "' is unrepresentable in "
            << "the target encoding" << endl;

      throw Failed ();
    }
    catch (NoNamespaceMapping const& e)
    {
      wcerr << e.file () << ":" << e.line () << ":" << e.column ()
            << ": error: unable to map XML Schema namespace '" << e.ns ()
            << "' to C++ namespace" << endl;

      wcerr << e.file () << ":" << e.line () << ":" << e.column ()
            << ": info: use the --namespace-map or --namespace-regex option "
            << "to provide custom mapping" << endl;

      throw Failed ();
    }
    catch (InvalidNamespaceMapping const& e)
    {
      wcerr << "error: invalid XML to C++ namespace mapping specified: "
            << "'" << e.mapping () << "': " << e.reason () << endl;

      throw Failed ();
    }
    catch (cutl::re::format const& e)
    {
      wcerr << "error: invalid regex: '" <<
        e.regex ().c_str () << "': " <<
        e.description ().c_str () << endl;

      throw Failed ();
    }
    catch (cutl::re::wformat const& e)
    {
      wcerr << "error: invalid regex: '" <<
        e.regex () << "': " << e.description ().c_str () << endl;

      throw Failed ();
    }
  }
}
