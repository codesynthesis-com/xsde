// -*- C++ -*-
//
// This file was generated by CLI, a command line interface
// compiler for C++.
//

// Begin prologue.
//
#include <xsde/options-parser.hxx>
//
// End prologue.

#include <xsde/cxx/serializer/options.hxx>

#include <map>
#include <set>
#include <string>
#include <vector>
#include <utility>
#include <ostream>
#include <sstream>
#include <cstring>

namespace cli
{
  template <typename X>
  struct parser
  {
    static void
    parse (X& x, scanner& s)
    {
      using namespace std;

      const char* o (s.next ());
      if (s.more ())
      {
        string v (s.next ());
        istringstream is (v);
        if (!(is >> x && is.peek () == istringstream::traits_type::eof ()))
          throw invalid_value (o, v);
      }
      else
        throw missing_value (o);
    }
  };

  template <>
  struct parser<bool>
  {
    static void
    parse (bool& x, scanner& s)
    {
      const char* o (s.next ());

      if (s.more ())
      {
        const char* v (s.next ());

        if (std::strcmp (v, "1")    == 0 ||
            std::strcmp (v, "true") == 0 ||
            std::strcmp (v, "TRUE") == 0 ||
            std::strcmp (v, "True") == 0)
          x = true;
        else if (std::strcmp (v, "0")     == 0 ||
                 std::strcmp (v, "false") == 0 ||
                 std::strcmp (v, "FALSE") == 0 ||
                 std::strcmp (v, "False") == 0)
          x = false;
        else
          throw invalid_value (o, v);
      }
      else
        throw missing_value (o);
    }
  };

  template <>
  struct parser<std::string>
  {
    static void
    parse (std::string& x, scanner& s)
    {
      const char* o (s.next ());

      if (s.more ())
        x = s.next ();
      else
        throw missing_value (o);
    }
  };

  template <typename X>
  struct parser<std::pair<X, std::size_t> >
  {
    static void
    parse (std::pair<X, std::size_t>& x, scanner& s)
    {
      x.second = s.position ();
      parser<X>::parse (x.first, s);
    }
  };

  template <typename X>
  struct parser<std::vector<X> >
  {
    static void
    parse (std::vector<X>& c, scanner& s)
    {
      X x;
      parser<X>::parse (x, s);
      c.push_back (x);
    }
  };

  template <typename X, typename C>
  struct parser<std::set<X, C> >
  {
    static void
    parse (std::set<X, C>& c, scanner& s)
    {
      X x;
      parser<X>::parse (x, s);
      c.insert (x);
    }
  };

  template <typename K, typename V, typename C>
  struct parser<std::map<K, V, C> >
  {
    static void
    parse (std::map<K, V, C>& m, scanner& s)
    {
      const char* o (s.next ());

      if (s.more ())
      {
        std::size_t pos (s.position ());
        std::string ov (s.next ());
        std::string::size_type p = ov.find ('=');

        K k = K ();
        V v = V ();
        std::string kstr (ov, 0, p);
        std::string vstr (ov, (p != std::string::npos ? p + 1 : ov.size ()));

        int ac (2);
        char* av[] =
        {
          const_cast<char*> (o),
          0
        };

        if (!kstr.empty ())
        {
          av[1] = const_cast<char*> (kstr.c_str ());
          argv_scanner s (0, ac, av, false, pos);
          parser<K>::parse (k, s);
        }

        if (!vstr.empty ())
        {
          av[1] = const_cast<char*> (vstr.c_str ());
          argv_scanner s (0, ac, av, false, pos);
          parser<V>::parse (v, s);
        }

        m[k] = v;
      }
      else
        throw missing_value (o);
    }
  };

  template <typename K, typename V, typename C>
  struct parser<std::multimap<K, V, C> >
  {
    static void
    parse (std::multimap<K, V, C>& m, scanner& s)
    {
      const char* o (s.next ());

      if (s.more ())
      {
        std::size_t pos (s.position ());
        std::string ov (s.next ());
        std::string::size_type p = ov.find ('=');

        K k = K ();
        V v = V ();
        std::string kstr (ov, 0, p);
        std::string vstr (ov, (p != std::string::npos ? p + 1 : ov.size ()));

        int ac (2);
        char* av[] =
        {
          const_cast<char*> (o),
          0
        };

        if (!kstr.empty ())
        {
          av[1] = const_cast<char*> (kstr.c_str ());
          argv_scanner s (0, ac, av, false, pos);
          parser<K>::parse (k, s);
        }

        if (!vstr.empty ())
        {
          av[1] = const_cast<char*> (vstr.c_str ());
          argv_scanner s (0, ac, av, false, pos);
          parser<V>::parse (v, s);
        }

        m.insert (typename std::multimap<K, V, C>::value_type (k, v));
      }
      else
        throw missing_value (o);
    }
  };

  template <typename X, typename T, T X::*M>
  void
  thunk (X& x, scanner& s)
  {
    parser<T>::parse (x.*M, s);
  }

  template <typename X, bool X::*M>
  void
  thunk (X& x, scanner& s)
  {
    s.next ();
    x.*M = true;
  }
}

#include <map>

namespace CXX
{
  namespace Serializer
  {
    // options
    //

    options::
    options ()
    : type_map_ (),
      suppress_validation_ (),
      reuse_style_mixin_ (),
      reuse_style_none_ (),
      generate_polymorphic_ (),
      runtime_polymorphic_ (),
      generate_empty_impl_ (),
      generate_test_driver_ (),
      force_overwrite_ (),
      root_element_first_ (),
      root_element_last_ (),
      root_element_ (),
      skel_type_suffix_ ("_sskel"),
      skel_file_suffix_ ("-sskel"),
      impl_type_suffix_ ("_simpl"),
      impl_file_suffix_ ("-simpl")
    {
    }

    options::
    options (int& argc,
             char** argv,
             bool erase,
             ::cli::unknown_mode opt,
             ::cli::unknown_mode arg)
    : type_map_ (),
      suppress_validation_ (),
      reuse_style_mixin_ (),
      reuse_style_none_ (),
      generate_polymorphic_ (),
      runtime_polymorphic_ (),
      generate_empty_impl_ (),
      generate_test_driver_ (),
      force_overwrite_ (),
      root_element_first_ (),
      root_element_last_ (),
      root_element_ (),
      skel_type_suffix_ ("_sskel"),
      skel_file_suffix_ ("-sskel"),
      impl_type_suffix_ ("_simpl"),
      impl_file_suffix_ ("-simpl")
    {
      ::cli::argv_scanner s (argc, argv, erase);
      _parse (s, opt, arg);
    }

    options::
    options (int start,
             int& argc,
             char** argv,
             bool erase,
             ::cli::unknown_mode opt,
             ::cli::unknown_mode arg)
    : type_map_ (),
      suppress_validation_ (),
      reuse_style_mixin_ (),
      reuse_style_none_ (),
      generate_polymorphic_ (),
      runtime_polymorphic_ (),
      generate_empty_impl_ (),
      generate_test_driver_ (),
      force_overwrite_ (),
      root_element_first_ (),
      root_element_last_ (),
      root_element_ (),
      skel_type_suffix_ ("_sskel"),
      skel_file_suffix_ ("-sskel"),
      impl_type_suffix_ ("_simpl"),
      impl_file_suffix_ ("-simpl")
    {
      ::cli::argv_scanner s (start, argc, argv, erase);
      _parse (s, opt, arg);
    }

    options::
    options (int& argc,
             char** argv,
             int& end,
             bool erase,
             ::cli::unknown_mode opt,
             ::cli::unknown_mode arg)
    : type_map_ (),
      suppress_validation_ (),
      reuse_style_mixin_ (),
      reuse_style_none_ (),
      generate_polymorphic_ (),
      runtime_polymorphic_ (),
      generate_empty_impl_ (),
      generate_test_driver_ (),
      force_overwrite_ (),
      root_element_first_ (),
      root_element_last_ (),
      root_element_ (),
      skel_type_suffix_ ("_sskel"),
      skel_file_suffix_ ("-sskel"),
      impl_type_suffix_ ("_simpl"),
      impl_file_suffix_ ("-simpl")
    {
      ::cli::argv_scanner s (argc, argv, erase);
      _parse (s, opt, arg);
      end = s.end ();
    }

    options::
    options (int start,
             int& argc,
             char** argv,
             int& end,
             bool erase,
             ::cli::unknown_mode opt,
             ::cli::unknown_mode arg)
    : type_map_ (),
      suppress_validation_ (),
      reuse_style_mixin_ (),
      reuse_style_none_ (),
      generate_polymorphic_ (),
      runtime_polymorphic_ (),
      generate_empty_impl_ (),
      generate_test_driver_ (),
      force_overwrite_ (),
      root_element_first_ (),
      root_element_last_ (),
      root_element_ (),
      skel_type_suffix_ ("_sskel"),
      skel_file_suffix_ ("-sskel"),
      impl_type_suffix_ ("_simpl"),
      impl_file_suffix_ ("-simpl")
    {
      ::cli::argv_scanner s (start, argc, argv, erase);
      _parse (s, opt, arg);
      end = s.end ();
    }

    options::
    options (::cli::scanner& s,
             ::cli::unknown_mode opt,
             ::cli::unknown_mode arg)
    : type_map_ (),
      suppress_validation_ (),
      reuse_style_mixin_ (),
      reuse_style_none_ (),
      generate_polymorphic_ (),
      runtime_polymorphic_ (),
      generate_empty_impl_ (),
      generate_test_driver_ (),
      force_overwrite_ (),
      root_element_first_ (),
      root_element_last_ (),
      root_element_ (),
      skel_type_suffix_ ("_sskel"),
      skel_file_suffix_ ("-sskel"),
      impl_type_suffix_ ("_simpl"),
      impl_file_suffix_ ("-simpl")
    {
      _parse (s, opt, arg);
    }

    ::cli::usage_para options::
    print_usage (::std::wostream& os, ::cli::usage_para p)
    {
      CLI_POTENTIALLY_UNUSED (os);

      if (p == ::cli::usage_para::text)
        os << ::std::endl;

      os << "--type-map <mapfile>         Read XML Schema to C++ type mapping information" << ::std::endl
         << "                             from <mapfile>." << ::std::endl;

      os << "--suppress-validation        Suppress the generation of validation code." << ::std::endl;

      os << "--reuse-style-mixin          Generate code that supports the mixin base" << ::std::endl
         << "                             serializer implementation reuse style." << ::std::endl;

      os << "--reuse-style-none           Do not generate any support for base serializer" << ::std::endl
         << "                             implementation reuse." << ::std::endl;

      os << "--generate-polymorphic       Generate polymorphism-aware code." << ::std::endl;

      os << "--runtime-polymorphic        Generate non-polymorphic code that uses the" << ::std::endl
         << "                             runtime library configured with polymorphism" << ::std::endl
         << "                             support." << ::std::endl;

      os << "--generate-empty-impl        Generate a sample serializer implementation with" << ::std::endl
         << "                             empty function bodies which can then be filled" << ::std::endl
         << "                             with the application-specific code." << ::std::endl;

      os << "--generate-test-driver       Generate a test driver for the sample serializer" << ::std::endl
         << "                             implementation." << ::std::endl;

      os << "--force-overwrite            Force overwriting of the existing implementation" << ::std::endl
         << "                             and test driver files." << ::std::endl;

      os << "--root-element-first         Indicate that the first global element is the" << ::std::endl
         << "                             document root." << ::std::endl;

      os << "--root-element-last          Indicate that the last global element is the" << ::std::endl
         << "                             document root." << ::std::endl;

      os << "--root-element <element>     Indicate that <element> is the document root." << ::std::endl;

      os << "--skel-type-suffix <suffix>  Use the provided <suffix> instead of the default" << ::std::endl
         << "                             _sskel to construct the names of generated" << ::std::endl
         << "                             serializer skeletons." << ::std::endl;

      os << "--skel-file-suffix <suffix>  Use the provided <suffix> instead of the default" << ::std::endl
         << "                             -sskel to construct the names of generated" << ::std::endl
         << "                             serializer skeleton files." << ::std::endl;

      os << "--impl-type-suffix <suffix>  Use the provided <suffix> instead of the default" << ::std::endl
         << "                             _simpl to construct the names of serializer" << ::std::endl
         << "                             implementations for the built-in XML Schema types" << ::std::endl
         << "                             and sample serializer implementations." << ::std::endl;

      os << "--impl-file-suffix <suffix>  Use the provided <suffix> instead of the default" << ::std::endl
         << "                             -simpl to construct the names of generated sample" << ::std::endl
         << "                             serializer implementation files." << ::std::endl;

      p = ::cli::usage_para::option;

      return p;
    }

    typedef
    std::map<std::string, void (*) (options&, ::cli::scanner&)>
    _cli_options_map;

    static _cli_options_map _cli_options_map_;

    struct _cli_options_map_init
    {
      _cli_options_map_init ()
      {
        _cli_options_map_["--type-map"] =
        &::cli::thunk< options, NarrowStrings, &options::type_map_ >;
        _cli_options_map_["--suppress-validation"] =
        &::cli::thunk< options, &options::suppress_validation_ >;
        _cli_options_map_["--reuse-style-mixin"] =
        &::cli::thunk< options, &options::reuse_style_mixin_ >;
        _cli_options_map_["--reuse-style-none"] =
        &::cli::thunk< options, &options::reuse_style_none_ >;
        _cli_options_map_["--generate-polymorphic"] =
        &::cli::thunk< options, &options::generate_polymorphic_ >;
        _cli_options_map_["--runtime-polymorphic"] =
        &::cli::thunk< options, &options::runtime_polymorphic_ >;
        _cli_options_map_["--generate-empty-impl"] =
        &::cli::thunk< options, &options::generate_empty_impl_ >;
        _cli_options_map_["--generate-test-driver"] =
        &::cli::thunk< options, &options::generate_test_driver_ >;
        _cli_options_map_["--force-overwrite"] =
        &::cli::thunk< options, &options::force_overwrite_ >;
        _cli_options_map_["--root-element-first"] =
        &::cli::thunk< options, &options::root_element_first_ >;
        _cli_options_map_["--root-element-last"] =
        &::cli::thunk< options, &options::root_element_last_ >;
        _cli_options_map_["--root-element"] =
        &::cli::thunk< options, NarrowString, &options::root_element_ >;
        _cli_options_map_["--skel-type-suffix"] =
        &::cli::thunk< options, NarrowString, &options::skel_type_suffix_ >;
        _cli_options_map_["--skel-file-suffix"] =
        &::cli::thunk< options, NarrowString, &options::skel_file_suffix_ >;
        _cli_options_map_["--impl-type-suffix"] =
        &::cli::thunk< options, NarrowString, &options::impl_type_suffix_ >;
        _cli_options_map_["--impl-file-suffix"] =
        &::cli::thunk< options, NarrowString, &options::impl_file_suffix_ >;
      }
    };

    static _cli_options_map_init _cli_options_map_init_;

    bool options::
    _parse (const char* o, ::cli::scanner& s)
    {
      _cli_options_map::const_iterator i (_cli_options_map_.find (o));

      if (i != _cli_options_map_.end ())
      {
        (*(i->second)) (*this, s);
        return true;
      }

      // options base
      //
      if (::CXX::options::_parse (o, s))
        return true;

      return false;
    }

    bool options::
    _parse (::cli::scanner& s,
            ::cli::unknown_mode opt_mode,
            ::cli::unknown_mode arg_mode)
    {
      // Can't skip combined flags (--no-combined-flags).
      //
      assert (opt_mode != ::cli::unknown_mode::skip);

      bool r = false;
      bool opt = true;

      while (s.more ())
      {
        const char* o = s.peek ();

        if (std::strcmp (o, "--") == 0)
        {
          opt = false;
          s.skip ();
          r = true;
          continue;
        }

        if (opt)
        {
          if (_parse (o, s))
          {
            r = true;
            continue;
          }

          if (std::strncmp (o, "-", 1) == 0 && o[1] != '\0')
          {
            // Handle combined option values.
            //
            std::string co;
            if (const char* v = std::strchr (o, '='))
            {
              co.assign (o, 0, v - o);
              ++v;

              int ac (2);
              char* av[] =
              {
                const_cast<char*> (co.c_str ()),
                const_cast<char*> (v)
              };

              ::cli::argv_scanner ns (0, ac, av);

              if (_parse (co.c_str (), ns))
              {
                // Parsed the option but not its value?
                //
                if (ns.end () != 2)
                  throw ::cli::invalid_value (co, v);

                s.next ();
                r = true;
                continue;
              }
              else
              {
                // Set the unknown option and fall through.
                //
                o = co.c_str ();
              }
            }

            // Handle combined flags.
            //
            char cf[3];
            {
              const char* p = o + 1;
              for (; *p != '\0'; ++p)
              {
                if (!((*p >= 'a' && *p <= 'z') ||
                      (*p >= 'A' && *p <= 'Z') ||
                      (*p >= '0' && *p <= '9')))
                  break;
              }

              if (*p == '\0')
              {
                for (p = o + 1; *p != '\0'; ++p)
                {
                  std::strcpy (cf, "-");
                  cf[1] = *p;
                  cf[2] = '\0';

                  int ac (1);
                  char* av[] =
                  {
                    cf
                  };

                  ::cli::argv_scanner ns (0, ac, av);

                  if (!_parse (cf, ns))
                    break;
                }

                if (*p == '\0')
                {
                  // All handled.
                  //
                  s.next ();
                  r = true;
                  continue;
                }
                else
                {
                  // Set the unknown option and fall through.
                  //
                  o = cf;
                }
              }
            }

            switch (opt_mode)
            {
              case ::cli::unknown_mode::skip:
              {
                s.skip ();
                r = true;
                continue;
              }
              case ::cli::unknown_mode::stop:
              {
                break;
              }
              case ::cli::unknown_mode::fail:
              {
                throw ::cli::unknown_option (o);
              }
            }

            break;
          }
        }

        switch (arg_mode)
        {
          case ::cli::unknown_mode::skip:
          {
            s.skip ();
            r = true;
            continue;
          }
          case ::cli::unknown_mode::stop:
          {
            break;
          }
          case ::cli::unknown_mode::fail:
          {
            throw ::cli::unknown_argument (o);
          }
        }

        break;
      }

      return r;
    }
  }
}

// Begin epilogue.
//
//
// End epilogue.

