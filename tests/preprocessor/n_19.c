/* n_19.c:  Valid re-definitions of macros. */

#include    "defs.h"

main( void)
{
    int     c = 1;

    fputs( "started\n", stderr);

/* Excerpts from ISO C 6.8.3 "Examples".    */
#define OBJ_LIKE        (1-1)
#define FTN_LIKE(a)     ( a )

/* 19.1:    */
#define OBJ_LIKE    /* white space */  (1-1) /* other */

/* 19.2:    */
#define FTN_LIKE( a     )(  /* note the white space */  \
                        a  /* other stuff on this line
                           */ )
    assert( FTN_LIKE( c) == 1);

    fputs( "success\n", stderr);
    return  0;
}

