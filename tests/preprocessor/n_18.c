/* n_18.c:  #define directive.  */

#include    "defs.h"

main( void)
{
    int     c = 3;

/* Excerpts from ISO C 6.8.3 "Examples".    */
#define OBJ_LIKE        (1-1)
#define FTN_LIKE(a)     ( a )

    fputs( "started\n", stderr);

/* 18.1:    Definition of an object-like macro. */
    assert( OBJ_LIKE == 0);
#define ZERO_TOKEN
#ifndef ZERO_TOKEN
    fputs( "Can't define macro to 0-token.\n", stderr);
    exit( 1);
#endif

/* 18.2:    Definition of a function-like macro.    */
    assert( FTN_LIKE( c) == 3);

/* 18.3:    Spelling in string identical to parameter is not a parameter.   */
#define STR( n1, n2)    "n1:n2"
    assert( strcmp( STR( 1, 2), "n1:n2") == 0);

    fputs( "success\n", stderr);
    return  0;
}

