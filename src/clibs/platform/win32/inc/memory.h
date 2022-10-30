#ifndef _MEMORY_H
#define _MEMORY_H

/* memory.h - private header for memory definitions */

#ifndef RC_INVOKED

#if __POCC__ < 900
#error <memory.h> need POCC version 9.0 or higher
#endif /* __POCC__ < 900 */

#pragma once

#pragma message("Use <string.h> instead of non-standard <memory.h>")

/* type definitions */
#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef __SIZE_TYPE__ size_t;
#endif /* _SIZE_T_DEFINED */

/* declarations */
extern void * __cdecl memchr(const void *, int, size_t);
extern int __cdecl memcmp(const void *, const void *, size_t);
extern void * __cdecl memcpy(void * restrict, const void * restrict, size_t);
extern void * __cdecl memmove(void *, const void *, size_t);
extern void * __cdecl memset(void *, int, size_t);

#endif /* !RC_INVOKED */

#endif /* _MEMORY_H */
