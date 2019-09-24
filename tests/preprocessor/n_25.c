/* n_25.c:  Macro arguments are pre-expanded (unless the argument is an
        operand of # or ## operator) separately, that is, are macro-replaced
        completely prior to rescanning. */

#include    "defs.h"

#define ZERO_TOKEN
#define MACRO_0         0
#define MACRO_1         1
#define TWO_ARGS        a,b
#define sub( x, y)      (x - y)
#define glue( a, b)     a ## b
#define xglue( a, b)    glue( a, b)
#define str( a)         # a

main( void)
{
    int     a = 1, b = 2, abc = 3, MACRO_0MACRO_1 = 2;

    fputs( "started\n", stderr);

/* 25.1:    "TWO_ARGS" is read as one argument to "sub", then expanded to
        "a,b", then "x" is substituted by "a,b".    */
    assert( sub( TWO_ARGS, 1) == 1);

/* 25.2:    An argument pre-expanded to 0-token.    */
/*  ( - 1); */
    assert( sub( ZERO_TOKEN, a) == -1);

/* 25.3:    "glue( a, b)" is pre-expanded.  */
    assert( xglue( glue( a, b), c) == 3);

/* 25.4:    Operands of ## operator are not pre-expanded.   */
    assert( glue( MACRO_0, MACRO_1) == 2);

/* 25.5:    Operand of # operator is not pre-expanded.  */
    assert( strcmp( str( ZERO_TOKEN), "ZERO_TOKEN") == 0);

    fputs( "success\n", stderr);
    return  0;
}

