#ifndef _MALLOC_H
#define _MALLOC_H

/* malloc.h - private header for memory allocation definitions */

#ifndef RC_INVOKED

#if __POCC__ < 900
#error <malloc.h> need POCC version 9.0 or higher
#endif /* __POCC__ < 900 */

#pragma once

#pragma message("Use <stdlib.h> instead of non-standard <malloc.h>")

#include <crtdef.h>

/* type definitions */
#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef __SIZE_TYPE__ size_t;
#endif /* _SIZE_T_DEFINED */

/* declarations */
extern _CRTUSE _CRTMEM void * __cdecl _alloca(size_t);
extern _CRTIMP _CRTUSE _CRTMEM void * __cdecl calloc(size_t, size_t);
extern _CRTIMP _CRTFRE(1) void __cdecl free(void *);
extern _CRTIMP _CRTUSE _CRTMEM void * __cdecl malloc(size_t);
extern _CRTIMP _CRTUSE _CRTMEM void * __cdecl realloc(void *, size_t);

/* private extensions */
extern _CRTIMP int __cdecl _heap_validate(void *);
extern _CRTIMP size_t __cdecl _msize(void *);
extern _CRTIMP _CRTUSE _CRTMEM void * __cdecl _mm_malloc(size_t, size_t);
extern _CRTIMP _CRTFRE(1) void __cdecl _mm_free(void *);
#ifdef _MSC_EXTENSIONS
#define _aligned_malloc(n,m)  _mm_malloc(n,m)
#define _aligned_free(p)  _mm_free(p)
#endif /* _MSC_EXTENSIONS */

#ifdef __POCC__OLDNAMES
#undef alloca
#define alloca  _alloca
#endif /* __POCC__OLDNAMES */

#endif /* !RC_INVOKED */

#endif /* _MALLOC_H */
