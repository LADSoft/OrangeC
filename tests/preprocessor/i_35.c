/* i_35.c:  Multi-character character constant. */

#include    "defs.h"

main( void)
{
    fputs( "started\n", stderr);

/* In ASCII character set.  */
/* 35.1:    */
#if     ('ab' != '\x61\x62') || ('\aa' != '\7\x61')
    fputs( "Bad handling of multi-character character constant.\n", stderr);
    exit( 1);
#endif

    fputs( "success\n", stderr);
    return  0;
}

