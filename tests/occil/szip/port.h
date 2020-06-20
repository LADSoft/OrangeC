#if !defined port_h
#define port_h

#if defined GCC
#define Inline inline
#else
#define Inline __inline
#endif

/* change to 1 if types.h exists */
#if 1
#include <sys/types.h>
#define uint2 u_int16_t
#define uint4 u_int32_t
/* uint is alredy defined in types.h */

#else
#include <limits.h>
#if INT_MAX > 0x7FFF
typedef unsigned short uint2;  /* two-byte integer (large arrays)      */
typedef unsigned int   uint4;  /* four-byte integers (range needed)    */
#else
typedef unsigned int   uint2;
typedef unsigned long  uint4;
#endif /* INT_MAX */

typedef unsigned int uint;     /* fast unsigned integer, 2 or 4 bytes  */

#endif


#endif
