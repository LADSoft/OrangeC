/* n_10.c:  #if, #elif, #else and #endif pp-directive.  */

#include    "defs.h"

#define MACRO_0     0
#define MACRO_1     1

main( void)
{
    fputs( "started\n", stderr);

/* 10.1:    */
/* Note: an undefined identifier in #if expression is replaced to 0.    */
#if     a
    assert( a);
#elif   MACRO_0
    assert( MACRO_0);
#elif   MACRO_1         /* Valid block  */
    assert( MACRO_1);
#else
    assert( 0);
#endif

/* 10.2: Comments must be processed even if in skipped #if block.   */
/* At least tokenization of string literal and character constant is necessary
        to process comments, e.g. /* is not a comment mark in string literal.
 */
#ifdef  UNDEFINED
    /* Comment  */
    "in literal /* is not a comment"
#endif

    fputs( "success\n", stderr);
    return  0;
}

