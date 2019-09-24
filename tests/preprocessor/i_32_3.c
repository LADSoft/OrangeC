/* i_32_3.c:    Character constant in #if expression.   */

#include    "defs.h"

main( void)
{
    fputs( "started\n", stderr);

/* 32.3:    */
#if     'a' != 0x61
    fputs( "Not ASCII character set, or bad evaluation of character constant."
            , stderr);
    exit( 1);
#endif

/* 32.4:    '\a' and '\v'   */
#if     '\a' != 7 || '\v' != 11
    fputs( "Not ASCII character set, or bad evaluation of escape sequences."
            , stderr);
    exit( 1);
#endif

    fputs( "success\n", stderr);
    return  0;
}

