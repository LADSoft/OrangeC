/* n_24.c:  # operator in macro definition. */

#include    "defs.h"

main( void)
{
    fputs( "started\n", stderr);

/* 24.1:    */
#define str( a)     # a
    assert( strcmp( str( a+b), "a+b") == 0);

/* 24.2:    White spaces between tokens of operand are converted to one space.
 */
    assert( strcmp( str(    ab  /* comment */   +
        cd  ), "ab + cd") == 0);

/* 24.3:    \ is inserted before \ and " in or surrounding literals and no
        other character is inserted to anywhere.    */
    assert( strcmp( str( '"' + "' \""), "'\"' + \"' \\\"\"") == 0);

/* 24.4:    Line splicing by <backslash><newline> is done prior to token
        parsing.   */
    assert( strcmp( str( "ab\
c"), "\"abc\"") == 0);

/* 24.5:    Token separator inserted by macro expansion should be removed.
        (Meanwhile, tokens should not be merged.  See 21.2.)    */
#define xstr( a)    str( a)
#define f(a)        a
    assert( strcmp( xstr( x-f(y)), "x-y") == 0);

    fputs( "success\n", stderr);
    return  0;
}

