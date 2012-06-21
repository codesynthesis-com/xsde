// file      : xsde/xsde.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_HXX
#define XSDE_HXX

#include <cstdio> // std::remove

#include <cutl/shared-ptr.hxx>

#include <xsd-frontend/semantic-graph/elements.hxx> // Path

#include <cult/types.hxx>
#include <cult/containers/set.hxx>
#include <cult/containers/vector.hxx>

using namespace Cult::Types;

//
//
typedef Cult::Containers::Set<NarrowString> WarningSet;

//
//
typedef Cult::Containers::Vector<NarrowString> FileList;

//
//
struct AutoUnlink
{
  AutoUnlink (XSDFrontend::SemanticGraph::Path const& file)
      : file_ (file), canceled_ (false)
  {
  }

  ~AutoUnlink ()
  {
    if (!canceled_)
      std::remove (file_.native_file_string ().c_str ());
  }

  void
  cancel ()
  {
    canceled_ = true;
  }

private:
  XSDFrontend::SemanticGraph::Path file_;
  Boolean canceled_;
};

//
//
struct AutoUnlinks
{
  Void
  add (XSDFrontend::SemanticGraph::Path const& file)
  {
    unlinks_.push_back(
      cutl::shared_ptr<AutoUnlink> (
        new (shared) AutoUnlink (file)));
  }

  Void
  cancel ()
  {
    for (Unlinks::Iterator i (unlinks_.begin ()); i != unlinks_.end (); ++i)
      (*i)->cancel ();
  }

private:
  typedef Cult::Containers::Vector<cutl::shared_ptr<AutoUnlink> > Unlinks;
  Unlinks unlinks_;
};

#endif // XSDE_HXX
