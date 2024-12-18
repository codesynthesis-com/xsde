// file      : xsde/types.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_TYPES_HXX
#define XSDE_TYPES_HXX

#include <vector>
#include <cstddef> // std::size_t

#include <libxsd-frontend/types.hxx>

using std::size_t;

using XSDFrontend::String;
using XSDFrontend::NarrowString;

typedef std::vector<NarrowString> NarrowStrings;

#endif // XSDE_TYPES_HXX
