/* n_15.c:  #ifdef, #ifndef directives. */

#include    "defs.h"

#define MACRO_0     0
#define MACRO_1     1

main( void)
{
    fputs( "started\n", stderr);

/* 15.1:    #ifdef directive.   */
#ifdef  MACRO_1     /* Valid block  */
    assert( MACRO_1);
#else
    assert( MACRO_0);
#endif

/* 15.2:    #ifndef directive.  */
#ifndef MACRO_1
    assert( MACRO_0);
#else               /* Valid block  */
    assert( MACRO_1);
#endif

    fputs( "success\n", stderr);
    return  0;
}

