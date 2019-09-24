/* n_32.c:  Escape sequence in character constant in #if expression.    */

#include    "defs.h"

main( void)
{
    fputs( "started\n", stderr);

/* 32.1:    Character octal escape sequence.    */
#if     '\123' != 83
    fputs( "Bad evaluation of octal escape sequence.\n", stderr);
    exit( 1);
#endif

/* 32.2:    Character hexadecimal escape sequence.  */
#if     '\x1b' != '\033'
    fputs( "Bad evaluation of hexadecimal escape sequence.\n", stderr);
    exit( 1);
#endif

    fputs( "success\n", stderr);
    return  0;
}

