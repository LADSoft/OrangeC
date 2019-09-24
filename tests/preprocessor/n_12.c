/* n_12.c:  Integer preprocessing number token and type of #if expression.  */

#include    "defs.h"
#include    <limits.h>

main( void)
{
    fputs( "started\n", stderr);

/* 12.1:    */
#if     LONG_MAX <= LONG_MIN
    fputs( "Bad evaluation of long.\n", stderr);
    exit( 1);
#endif
#if     LONG_MAX <= 1073741823  /* 0x3FFFFFFF   */
    fputs( "Bad evaluation of long.\n", stderr);
    exit( 1);
#endif

/* 12.2:    */
#if     ULONG_MAX / 2 < LONG_MAX
    fputs( "Bad evaluation of unsigned long.\n", stderr);
    exit( 1);
#endif

/* 12.3:    Octal number.   */
#if     0177777 != 65535
    fputs( "Bad evaluation of octal number.\n", stderr);
    exit( 1);
#endif

/* 12.4:    Hexadecimal number. */
#if     0Xffff != 65535 || 0xFfFf != 65535
    fputs( "Bad evaluation of hexadecimal number.\n", stderr);
    exit( 1);
#endif

/* 12.5:    Suffix 'L' or 'l'.  */
#if     0L != 0 || 0l != 0
    fputs( "Bad evaluation of 'L' suffix.\n", stderr);
    exit( 1);
#endif

/* 12.6:    Suffix 'U' or 'u'.  */
#if     1U != 1 || 1u != 1
    fputs( "Bad evaluation of 'U' suffix.\n", stderr);
    exit( 1);
#endif

/* 12.7:    Negative integer.   */
#if     0 <= -1
    fputs( "Bad evaluation of negative number.\n", stderr);
    exit( 1);
#endif

    fputs( "success\n", stderr);
    return  0;
}

