/* n_11.c:  Operator "defined" in #if or #elif directive.   */

#include    "defs.h"

#define MACRO_abc   abc
#define MACRO_0     0
#define ZERO_TOKEN

main( void)
{
    int     abc = 1, a = 0;

    fputs( "started\n", stderr);

/* 11.1:    */
#if     defined a
    assert( a);
#else
    assert( MACRO_abc);
#endif
#if     defined (MACRO_abc)
    assert( MACRO_abc);
#else
    assert( a);
#endif

/* 11.2:    "defined" is an unary operator whose result is 1 or 0.  */
#if     defined MACRO_0 * 3 != 3
    fputs( "Bad handling of defined operator.\n", stderr);
    exit( 1);
#endif
#if     (!defined ZERO_TOKEN != 0) || (-defined ZERO_TOKEN != -1)
    fputs( "Bad grouping of defined, -, ! in #if expression.\n", stderr);
    exit( 1);
#endif

    fputs( "success\n", stderr);
    return  0;
}

