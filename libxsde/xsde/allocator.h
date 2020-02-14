// file      : xsde/allocator.h
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_ALLOCATOR_HXX
#define XSDE_ALLOCATOR_HXX

#include <stddef.h> // size_t

#ifdef __cplusplus
extern "C"
{
#endif

// Custom allocator functions. Your implementation should always return
// 0 on failure, regardless of whether exceptions are enabled or not.
//
void* xsde_alloc (size_t);
void* xsde_realloc (void*, size_t);
void xsde_free (void*);

#ifdef __cplusplus
}
#endif

#endif // XSDE_ALLOCATOR_HXX
