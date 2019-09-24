/* n_13_5.c:    Arithmetic conversion in #if expressions.   */

#include    "defs.h"

main( void)
{
    fputs( "started\n", stderr);

/* 13.5:    The usual arithmetic conversion is not performed on bit shift.  */
#if     -1 << 3U > 0
    fputs( "Bad conversion of bit shift operands.\n", stderr);
    exit( 1);
#endif

/* 13.6:    Usual arithmetic conversions.   */
#if     -1 <= 0U        /* -1 is converted to unsigned long.    */
    fputs( "Bad arithmetic conversion 1.\n", stderr);
    exit( 1);
#endif

#if     -1 * 1U <= 0
    fputs( "Bad arithmetic conversion 2.\n", stderr);
    exit( 1);
#endif

/* Second and third operands of conditional operator are converted to the
        same type, thus -1 is converted to unsigned long.    */
#if     (1 ? -1 : 0U) <= 0
    fputs( "Bad arithmetic conversion 3.\n", stderr);
    exit( 1);
#endif

    fputs( "success\n", stderr);
    return  0;
}

