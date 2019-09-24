/* n_2.c:   Line splicing by <backslash><newline> sequence. */

#include    "defs.h"

main( void)
{
    int     ab = 1, cd = 2, ef = 3, abcde = 5;

    fputs( "started\n", stderr);

/* 2.1: In a #define directive line, between the parameter list and the
        replacement text.   */
#define FUNC( a, b, c)  \
        a + b + c
    assert( FUNC( ab, cd, ef) == 6);

/* 2.2: In a #define directive line, among the parameter list and among the
        replacement text.   */
#undef  FUNC
#define FUNC( a, b  \
    , c)            \
    a + b           \
    + c
    assert (FUNC( ab, cd, ef) == 6);

/* 2.3: In a string literal.    */
    assert (strcmp( "abc\
de", "abcde") == 0);

/* 2.4: <backslash><newline> in midst of an identifier. */
    assert( abc\
de == 5);

/* 2.5: <backslash><newline> by trigraph.   */
    assert( abc??/
de == 5);

    fputs( "success\n", stderr);
    return  0;
}

