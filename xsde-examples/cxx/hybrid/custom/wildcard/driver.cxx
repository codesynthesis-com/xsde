// file      : cxx/hybrid/custom/wildcard/driver.cxx
// copyright : not copyrighted - public domain

#include <string>   // memcpy
#include <memory>   // std::{unique,auto}_ptr
#include <iostream>

#include "email.hxx"
#include "email-pimpl.hxx"
#include "email-simpl.hxx"

using namespace std;

// Extended parser and serializer aggregates for the message
// element. They add parser/serializer aggregates for the
// wildcard elements.
//
namespace email
{
  class message_paggr_ex: public message_paggr
  {
  public:
    message_paggr_ex ()
    {
      envelope_p_.text_parser (text_p_.root_parser ());
      envelope_p_.binary_parser (binary_p_.root_parser ());
    }

  public:
    text_paggr text_p_;
    binary_paggr binary_p_;
  };

  class message_saggr_ex: public message_saggr
  {
  public:
    message_saggr_ex ()
    {
      envelope_s_.text_serializer (text_s_.root_serializer ());
      envelope_s_.binary_serializer (binary_s_.root_serializer ());
    }

  public:
    text_saggr text_s_;
    binary_saggr binary_s_;
  };
}

int
main (int argc, char* argv[])
{
  const char* input;

  if (argc < 2)
  {
    input = "STDIN";
    cerr << "XML file not specified, reading from STDIN" << endl;
  }
  else
    input = argv[1];

  try
  {
    using namespace email;

    // Parse.
    //
    message_paggr_ex message_p;

    xml_schema::document_pimpl doc_p (
      message_p.root_parser (),
      message_p.root_namespace (),
      message_p.root_name ());

    message_p.pre ();

    if (argc < 2)
      doc_p.parse (cin);
    else
      doc_p.parse (argv[1]);

#ifdef XSDE_CXX11
    unique_ptr<envelope> msg (message_p.post ());
#else
    auto_ptr<envelope> msg (message_p.post ());
#endif

    // Print what we've got.
    //
    cerr << "To:      " << msg->to () << endl
         << "From:    " << msg->from () << endl
         << "Subject: " << msg->subject () << endl;

    envelope::body_sequence& body_seq (msg->body ());

    for (envelope::body_iterator i = body_seq.begin ();
         i != body_seq.end ();
         ++i)
    {
      body& b = *i;

      switch (b.body_type ())
      {
      case body::type_text:
        {
          cerr << b.text () << endl
               << endl;
          break;
        }
      case body::type_binary:
        {
          const binary& bin = b.binary ();
          cerr << "binary: " << bin.name () << " "
               << "type: " << bin.mime () << endl
               << endl;
          break;
        }
      default:
        {
          cerr << "unexpected body type" << endl;
          break;
        }
      }
    }

    // Create a reply message.
    //
#ifdef XSDE_CXX11
    unique_ptr<envelope> reply (new envelope);
#else
    auto_ptr<envelope> reply (new envelope);
#endif

    reply->to (msg->from ());
    reply->from (msg->to ());
    reply->subject ("Re: " + msg->subject ());

    // Add a text body.
    //
#ifdef XSDE_CXX11
    unique_ptr<body> b (new body);
#else
    auto_ptr<body> b (new body);
#endif

    b->text ("Hi!\n\n"
             "Indeed nice pictures. Check out mine.\n\n"
             "Jane");
    reply->body ().push_back (b.release ());

    // Add a (fake) image.
    //
#ifdef XSDE_CXX11
    unique_ptr<binary> pic (new binary);
#else
    auto_ptr<binary> pic (new binary);
#endif

    pic->name ("pic.jpg");
    pic->mime ("image/jpeg");
    pic->size (3);
    memcpy (pic->data (), "123", 3);

#ifdef XSDE_CXX11
    b = unique_ptr<body> (new body);
#else
    b = auto_ptr<body> (new body);
#endif

    b->binary (pic.release ());
    reply->body ().push_back (b.release ());

    // Serialize.
    //
    message_saggr_ex message_s;

    xml_schema::document_simpl doc_s (
      message_s.root_serializer (),
      message_s.root_namespace (),
      message_s.root_name ());

    doc_s.add_prefix ("lib", "http://www.codesynthesis.com/email");
    doc_s.add_schema ("http://www.codesynthesis.com/email", "email.xsd");

    message_s.pre (*reply);
    doc_s.serialize (cout, xml_schema::document_simpl::pretty_print);
    message_s.post ();
  }
  catch (const xml_schema::parser_exception& e)
  {
    cerr << input << ":" << e.line () << ":" << e.column () << ": "
         << e.text () << endl;
    return 1;
  }
  catch (const xml_schema::serializer_exception& e)
  {
    cerr << "error: " << e.text () << endl;
    return 1;
  }
  catch (const std::ios_base::failure&)
  {
    cerr << input << ": unable to open or read/write failure" << endl;
    return 1;
  }

  return 0;
}
