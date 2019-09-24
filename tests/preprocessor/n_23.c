/* n_23.c:  ## operator in macro definition.    */

#include    "defs.h"

#define glue( a, b)     a ## b
#define xglue( a, b)    glue( a, b)
#define MACRO_0     0
#define MACRO_1     1

main( void)
{
    int     xy = 1;

    fputs( "started\n", stderr);

/* 23.1:    */
    assert( glue( x, y) == 1);

/* 23.2:    Generate a preprocessing number.    */
#define EXP     2
    assert( xglue( .12e+, EXP) == 12.0);

    fputs( "success\n", stderr);
    return  0;
}

