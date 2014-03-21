// file      : xsde/xsde.hxx
// copyright : Copyright (c) 2005-2014 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_HXX
#define XSDE_HXX

#include <set>
#include <vector>
#include <cstdio> // std::remove

#include <cutl/shared-ptr.hxx>
#include <cutl/fs/auto-remove.hxx>

#include <xsd-frontend/semantic-graph/elements.hxx> // Path

#include <types.hxx>

typedef std::set<NarrowString> WarningSet;
typedef std::vector<NarrowString> FileList;

typedef cutl::fs::auto_remove AutoUnlink;
typedef cutl::fs::auto_removes AutoUnlinks;

#endif // XSDE_HXX
