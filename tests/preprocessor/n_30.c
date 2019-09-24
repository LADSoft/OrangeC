/* n_30.c:  Macro calls.    */
/*  Note:   Comma separate the arguments of function-like macro call,
        but comma between matching inner parenthesis doesn't.  This feature
        is tested on so many places in this suite especially on *.c samples
        which use assert() macro, that no separete item to test this feature
        is provided.    */

#include    "defs.h"

#define FUNC( a, b, c)      a + b + c

main( void)
{
    int     a = 1, b = 2, c = 3;

    fputs( "started\n", stderr);

/* 30.1:    A macro call crossing lines.    */
    assert
    (
        FUNC
        (
            a,
            b,
            c
        )
        == 6
    );

    fputs( "success\n", stderr);
    return  0;
}

