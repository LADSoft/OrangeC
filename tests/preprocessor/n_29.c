/* n_29.c:  #undef directive.   */

#include    "defs.h"

main( void)
{
    int     DEFINED = 1;

    fputs( "started\n", stderr);

/* 29.1:    Undefined macro is not a macro. */
#define DEFINED
#undef  DEFINED
    assert( DEFINED == 1);

/* 29.2:    Undefining undefined name is not an error.  */
#undef  UNDEFINED

    fputs( "success\n", stderr);
    return  0;
}

