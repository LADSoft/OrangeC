/* i_35_3.c:    Multi-character wide character constant.    */

#include    "defs.h"

main( void)
{
    fputs( "started\n", stderr);

/* In ASCII character set.  */
/* 35.3:    */
#if     (L'ab' != L'\x61\x62') || (L'ab' == 'ab')
    fputs( "Bad handling of multi-character wide character constant.\n",
            stderr);
    exit( 1);
#endif

    fputs( "success\n", stderr);
    return  0;
}
