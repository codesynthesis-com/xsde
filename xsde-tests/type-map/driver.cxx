      std::ifstream is (argv[1]);
      TypeMap::Lexer l (is, argv[1]);

      /*
        for (TypeMap::Lexer::Token t (l.next ());
             t.type () != TypeMap::Lexer::Token::eos;
             t = l.next ())
        {
          e << t.line () << ": " << t.lexeme () << endl;
        }
      */

      TypeMap::Parser p (l, argv[1]);

      TypeMap::Namespaces ns;

      if (!p.parse (ns))
      {
        e << "failed" << endl;
        return 1;
      }

      for (TypeMap::Namespaces::ConstIterator n (type_map.begin ());
           n != type_map.end (); ++n)
      {
        wcerr << "namespace " << n->xsd_name () << " " << n->cxx_name () << endl
              << "{" << endl;

        for (TypeMap::Namespace::IncludesIterator i (n->includes_begin ());
             i != n->includes_end (); ++i)
        {
          wcerr << "include " << *i << ";" << endl;
        }

        for (TypeMap::Namespace::TypesIterator t (n->types_begin ());
             t != n->types_end (); ++t)
        {
          wcerr << "type " << t->xsd_name () << " " << t->cxx_ret_name ();

          if (t->cxx_arg_name ())
            wcerr << " " << t->cxx_arg_name ();

          wcerr << ";" << endl;
        }

        wcerr << "}" << endl;
      }
