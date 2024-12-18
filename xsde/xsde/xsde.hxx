// file      : xsde/xsde.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_XSDE_HXX
#define XSDE_XSDE_HXX

#include <set>
#include <vector>
#include <cstdio> // std::remove

#include <libcutl/shared-ptr.hxx>
#include <libcutl/fs/auto-remove.hxx>

#include <libxsd-frontend/semantic-graph/elements.hxx> // Path

#include <xsde/types.hxx>

typedef std::set<NarrowString> WarningSet;
typedef std::vector<NarrowString> FileList;

typedef cutl::fs::auto_remove AutoUnlink;
typedef cutl::fs::auto_removes AutoUnlinks;

#endif // XSDE_XSDE_HXX
