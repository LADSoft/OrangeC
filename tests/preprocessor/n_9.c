/* n_9.c:   #pragma directive.  */

#include    "stdio.h"

/* 9.1: Any #pragma directive should be processed or ignored, should not
        be diagnosed as an error.   */
#pragma once
#pragma who knows ?

main( void)
{
    fputs( "started\n", stderr);
    fputs( "success\n", stderr);
    return  0;
}

