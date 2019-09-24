/* n_20.c:  Macro lexically identical to keyword. */

#include    "defs.h"

main( void)
{

/* 20.1:    */
#define float   double
    float   fl;

    fputs( "started\n", stderr);

    assert( sizeof fl == sizeof (double));

    fputs( "success\n", stderr);
    return  0;
}

