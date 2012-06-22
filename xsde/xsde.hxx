// file      : xsde/xsde.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_HXX
#define XSDE_HXX

#include <set>
#include <vector>
#include <cstdio> // std::remove

#include <cutl/shared-ptr.hxx>

#include <xsd-frontend/semantic-graph/elements.hxx> // Path

#include <types.hxx>

typedef std::set<NarrowString> WarningSet;
typedef std::vector<NarrowString> FileList;

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
  bool canceled_;
};

//
//
struct AutoUnlinks
{
  void
  add (XSDFrontend::SemanticGraph::Path const& file)
  {
    unlinks_.push_back(
      cutl::shared_ptr<AutoUnlink> (
        new (shared) AutoUnlink (file)));
  }

  void
  cancel ()
  {
    for (Unlinks::iterator i (unlinks_.begin ()); i != unlinks_.end (); ++i)
      (*i)->cancel ();
  }

private:
  typedef std::vector<cutl::shared_ptr<AutoUnlink> > Unlinks;
  Unlinks unlinks_;
};

#endif // XSDE_HXX
