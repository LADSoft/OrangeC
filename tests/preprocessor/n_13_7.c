/* n_13_7.c:    Short-circuit evaluation of #if expression. */

#include    "defs.h"
#define MACRO_0     0

main( void)
{
    fputs( "started\n", stderr);

/* 13.7:    10/0 or 10/MACRO_0 are never evaluated, "divide by zero" error
        cannot occur.   */
#if     0 && 10 / 0
    exit( 1);
#endif
#if     not_defined && 10 / not_defined
    exit( 1);
#endif
#if     MACRO_0 && 10 / MACRO_0 > 1
    exit( 1);
#endif
#if     MACRO_0 ? 10 / MACRO_0 : 0
    exit( 1);
#endif
#if     MACRO_0 == 0 || 10 / MACRO_0 > 1        /* Valid block  */
    fputs( "success\n", stderr);
    return  0;
#else
    exit( 1);
#endif
}

