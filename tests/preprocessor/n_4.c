/* n_4.c:   Special tokens. */

#include    "defs.h"

main( void)
{
/* 4.1: Digraph spellings in directive line.    */
%: define  stringize( a)    %: a

    fputs( "started\n", stderr);

    assert( strcmp( stringize( abc), "abc") == 0);

/* 4.2: Digraph spellings are retained in stringization.    */
    assert( strcmp( stringize( <:), "<" ":") == 0);

    fputs( "success\n", stderr);
    return 0;
}

