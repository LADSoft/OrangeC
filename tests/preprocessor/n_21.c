/* n_21.c:  Tokenization (No preprocessing tokens are merged implicitly).   */

#include    "defs.h"

main( void)
{
    int     a = 1, x = 2, y = -3;

    fputs( "started\n", stderr);

/* 21.1:    */
#define MINUS   -
    assert( -MINUS-a == -1);

/* 21.2:    */
#define sub( a, b)  a-b     /* '(a)-(b)' is better  */
#define Y   -y              /* '(-y)' is better     */
/*  x- -y   */
    assert( sub( x, Y) == -1);

    fputs( "success\n", stderr);
    return  0;
}

