/* n_22.c:  Tokenization of preprocessing number.   */

#include    "defs.h"

#define str( a)     # a
#define xstr( a)    str( a)
#define EXP         1

main( void)
{
    fputs( "started\n", stderr);

/* 22.1:    12E+EXP is a preprocessing number, EXP is not expanded. */
    assert( strcmp( xstr( 12E+EXP), "12E+EXP") == 0);

/* 22.2:    .2e-EXP is also a preprocessing number. */
    assert( strcmp( xstr( .2e-EXP), ".2e-EXP") == 0);

/* 22.3:    + or - is allowed only following E or e, 12+EXP is not a
        preprocessing number.   */
    assert( strcmp( xstr( 12+EXP), "12+1") == 0);

    fputs( "success\n", stderr);
    return  0;
}

