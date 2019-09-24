/* n_3.c:   Handling of comment.    */

#include    "defs.h"

#define str( a)     # a

main( void)
{
    int     abcd = 4;

    fputs( "started\n", stderr);

/* 3.1: A comment is converted to one space.    */
    assert( strcmp( str( abc/* comment */de), "abc de") == 0);

/* 3.2: // is not a comment of C.   */
/*  assert( strcmp( str( //), "//") == 0);  */

/* 3.3: Comment is parsed prior to the parsing of preprocessing directive.  */
#if     0
    "nonsence"; /*
#else
    still in
    comment     */
#else
#define MACRO_abcd  /*
    in comment
    */  abcd
#endif
    assert( MACRO_abcd == 4);

    fputs( "success\n", stderr);
    return  0;
}

