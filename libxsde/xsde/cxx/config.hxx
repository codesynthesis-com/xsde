// file      : xsde/cxx/config.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_CONFIG_HXX
#define XSDE_CXX_CONFIG_HXX

#include <xsde/config.h>

// Macro to suppress unused variable warning.
//
#define XSDE_UNUSED(x) (void)x

// Using strtof appears to be highly non-portable.
//
#undef XSDE_STRTOF

// The snprintf() function on Win32 and WinCE is called _snprintf.
//
// Note that VC 14 introduces the snprintf() function.
//
#ifdef XSDE_SNPRINTF
#  if defined(XSDE_PLATFORM_WIN32) || defined(XSDE_PLATFORM_WINCE)
#    if !defined(_MSC_VER) || _MSC_VER < 1900
#      define snprintf _snprintf
#    endif
#  endif
#endif

#endif // XSDE_CXX_CONFIG_HXX
