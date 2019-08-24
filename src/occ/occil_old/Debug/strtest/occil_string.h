#ifndef _occil_string_h_
#define _occil_string_h_

#include <stdlib.h>						/* size_t */

#ifdef __OCCIL_DONE__
#	include <string.h>
#endif


/** The number of bytes per machine word. */
#define OCCIL_BYTES_PER_WORD (sizeof(void *))

/** The bit mask to AND with to see if a given pointer is aligned or not. */
#define OCCIL_ALIGNMENT_MASK (OCCIL_BYTES_PER_WORD - 1)

/* Check if a given pointer is aligned or not.  It is safe to cast to an unsigned byte is safe as we only need a few bits. */
#define occil_unaligned(ptr)  (((unsigned char) (ptr)) & OCCIL_ALIGNMENT_MASK)

/* Temporarily prefix all OCCIL RTL names with "occil_" to avoid having them resolved to MSVCRT.dll. */
/** \todo Eliminate OCCIL_PROTO macro once the dust settles and we know how to deal with standard C library names. */
#ifdef __OCCIL_DONE__
#  define OCCIL_PROTO(name)  name
#else
#  define OCCIL_PROTO(name)  occil_##name
#endif


/* Function prototypes. */
/** \todo Eliminate all occil_xxx prototypes as these should be pulled in from string.h. */
extern void *OCCIL_PROTO(memchr)(const void *buf, int c, size_t cnt);
extern int   OCCIL_PROTO(memcmp)(const void *first, const void *other, size_t cnt);
extern void *OCCIL_PROTO(memcpy)(void *dst, const void *src, size_t cnt);
extern void *OCCIL_PROTO(memmove)(void *dst, const void *src, size_t cnt);
extern void *OCCIL_PROTO(mempcpy)(void *dst, const void *src, size_t cnt);
extern void *OCCIL_PROTO(memset)(void *dst, int val, size_t cnt);


#endif /* _occil_string_h_ */

