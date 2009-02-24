// file      : xsde/cxx/hybrid/cli.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef CXX_HYBRID_CLI_HXX
#define CXX_HYBRID_CLI_HXX

#include <cult/types.hxx>

#include <cult/containers/vector.hxx>

#include <cult/cli/options.hxx>
#include <cult/cli/options-spec.hxx>

namespace CXX
{
  namespace Hybrid
  {
    namespace CLI
    {
      using namespace Cult::Types;

      typedef Char const Key[];

      extern Key no_stl;
      extern Key no_iostream;
      extern Key no_exceptions;
      extern Key no_long_long;
      extern Key generate_parser;
      extern Key generate_serializer;
      extern Key generate_aggregate;
      extern Key suppress_validation;
      extern Key suppress_parser_val;
      extern Key suppress_serializer_val;
      extern Key generate_inline;
      extern Key generate_forward;
      extern Key generate_xml_schema;
      extern Key extern_xml_schema;
      extern Key suppress_reset;
      extern Key reuse_style_mixin;
      extern Key custom_data;
      extern Key custom_parser;
      extern Key custom_serializer;
      extern Key root_element_first;
      extern Key root_element_last;
      extern Key root_element_all;
      extern Key root_element_none;
      extern Key root_element;
      extern Key root_type;
      extern Key output_dir;
      extern Key pskel_type_suffix;
      extern Key sskel_type_suffix;
      extern Key pskel_file_suffix;
      extern Key sskel_file_suffix;
      extern Key pimpl_type_suffix;
      extern Key simpl_type_suffix;
      extern Key pimpl_file_suffix;
      extern Key simpl_file_suffix;
      extern Key paggr_type_suffix;
      extern Key saggr_type_suffix;
      extern Key namespace_map;
      extern Key namespace_regex;
      extern Key namespace_regex_trace;
      extern Key reserved_name;
      extern Key include_with_brackets;
      extern Key include_prefix;
      extern Key include_regex;
      extern Key include_regex_trace;
      extern Key guard_prefix;
      extern Key hxx_suffix;
      extern Key ixx_suffix;
      extern Key cxx_suffix;
      extern Key fwd_suffix;
      extern Key hxx_regex;
      extern Key ixx_regex;
      extern Key cxx_regex;
      extern Key fwd_regex;
      extern Key hxx_prologue;
      extern Key ixx_prologue;
      extern Key cxx_prologue;
      extern Key fwd_prologue;
      extern Key prologue;
      extern Key hxx_epilogue;
      extern Key ixx_epilogue;
      extern Key cxx_epilogue;
      extern Key fwd_epilogue;
      extern Key epilogue;
      extern Key hxx_prologue_file;
      extern Key ixx_prologue_file;
      extern Key cxx_prologue_file;
      extern Key fwd_prologue_file;
      extern Key prologue_file;
      extern Key hxx_epilogue_file;
      extern Key ixx_epilogue_file;
      extern Key cxx_epilogue_file;
      extern Key fwd_epilogue_file;
      extern Key epilogue_file;
      extern Key show_anonymous;
      extern Key show_sloc;
      extern Key proprietary_license;

      typedef Cult::CLI::Options<
        no_stl,                   Boolean,
        no_iostream,              Boolean,
        no_exceptions,            Boolean,
        no_long_long,             Boolean,
        generate_parser,          Boolean,
        generate_serializer,      Boolean,
        generate_aggregate,       Boolean,
        suppress_validation,      Boolean,
        suppress_parser_val,      Boolean,
        suppress_serializer_val,  Boolean,
        generate_inline,          Boolean,
        generate_forward,         Boolean,
        generate_xml_schema,      Boolean,
        extern_xml_schema,        NarrowString,
        suppress_reset,           Boolean,
        reuse_style_mixin,        Boolean,
        custom_data,              Cult::Containers::Vector<NarrowString>,
        custom_parser,            Cult::Containers::Vector<NarrowString>,
        custom_serializer,        Cult::Containers::Vector<NarrowString>,
        root_element_first,       Boolean,
        root_element_last,        Boolean,
        root_element_all,         Boolean,
        root_element_none,        Boolean,
        root_element,             Cult::Containers::Vector<NarrowString>,
        root_type,                Cult::Containers::Vector<NarrowString>,
        output_dir,               NarrowString,
        pskel_type_suffix,        NarrowString,
        sskel_type_suffix,        NarrowString,
        pskel_file_suffix,        NarrowString,
        sskel_file_suffix,        NarrowString,
        pimpl_type_suffix,        NarrowString,
        simpl_type_suffix,        NarrowString,
        pimpl_file_suffix,        NarrowString,
        simpl_file_suffix,        NarrowString,
        paggr_type_suffix,        NarrowString,
        saggr_type_suffix,        NarrowString,
        namespace_map,            Cult::Containers::Vector<NarrowString>,
        namespace_regex,          Cult::Containers::Vector<NarrowString>,
        namespace_regex_trace,    Boolean,
        reserved_name,            Cult::Containers::Vector<NarrowString>,
        include_with_brackets,    Boolean,
        include_prefix,           NarrowString,
        include_regex,            Cult::Containers::Vector<NarrowString>,
        include_regex_trace,      Boolean,
        guard_prefix,             NarrowString,
        hxx_suffix,               NarrowString,
        ixx_suffix,               NarrowString,
        cxx_suffix,               NarrowString,
        fwd_suffix,               NarrowString,
        hxx_regex,                Cult::Containers::Vector<NarrowString>,
        ixx_regex,                Cult::Containers::Vector<NarrowString>,
        cxx_regex,                Cult::Containers::Vector<NarrowString>,
        fwd_regex,                NarrowString,
        hxx_prologue,             Cult::Containers::Vector<NarrowString>,
        ixx_prologue,             Cult::Containers::Vector<NarrowString>,
        cxx_prologue,             Cult::Containers::Vector<NarrowString>,
        fwd_prologue,             Cult::Containers::Vector<NarrowString>,
        prologue,                 Cult::Containers::Vector<NarrowString>,
        hxx_epilogue,             Cult::Containers::Vector<NarrowString>,
        ixx_epilogue,             Cult::Containers::Vector<NarrowString>,
        cxx_epilogue,             Cult::Containers::Vector<NarrowString>,
        fwd_epilogue,             Cult::Containers::Vector<NarrowString>,
        epilogue,                 Cult::Containers::Vector<NarrowString>,
        hxx_prologue_file,        Cult::Containers::Vector<NarrowString>,
        ixx_prologue_file,        Cult::Containers::Vector<NarrowString>,
        cxx_prologue_file,        Cult::Containers::Vector<NarrowString>,
        fwd_prologue_file,        NarrowString,
        prologue_file,            Cult::Containers::Vector<NarrowString>,
        hxx_epilogue_file,        Cult::Containers::Vector<NarrowString>,
        ixx_epilogue_file,        Cult::Containers::Vector<NarrowString>,
        cxx_epilogue_file,        Cult::Containers::Vector<NarrowString>,
        fwd_epilogue_file,        NarrowString,
        epilogue_file,            Cult::Containers::Vector<NarrowString>,
        show_anonymous,           Boolean,
        show_sloc,                Boolean,
        proprietary_license,      Boolean

        > Options;

      struct OptionsSpec: Cult::CLI::OptionsSpec<Options> {};
    }
  }
}

#endif // CXX_HYBRID_CLI_HXX
