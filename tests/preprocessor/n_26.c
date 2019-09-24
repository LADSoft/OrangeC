/* n_26.c:  The name once replaced is not furthur replaced. */

#include    "defs.h"

int     f( a)
    int     a;
{
    return  a;
}

int     g( a)
    int     a;
{
    return  a * 2;
}

main( void)
{
    int     x = 1;
    int     AB = 1;
    int     Z[1];

    fputs( "started\n", stderr);

    Z[0] = 1;

/* 26.1:    Directly recursive object-like macro definition.    */
/*  Z[0];   */
#define Z   Z[0]
    assert( Z == 1);

/* 26.2:    Intermediately recursive object-like macro definition.  */
/*  AB; */
#define AB  BA
#define BA  AB
    assert( AB == 1);

/* 26.3:    Directly recursive function-like macro definition.  */
/*  x + f(x);   */
#define f(a)    a + f(a)
    assert( f( x) == 2);

/* 26.4:    Intermediately recursive function-like macro definition.    */
/*  x + x + g( x);  */
#define g(a)    a + h( a)
#define h(a)    a + g( a)
    assert( g( x) == 4);

/* 26.5:    Rescanning encounters the non-replaced macro name.  */
/*  Z[0] + f( Z[0]);    */
    assert( f( Z) == 2);

    fputs( "success\n", stderr);
    return  0;
}

