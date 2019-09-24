/* n_7.c:   #line directive.    */

#include    "defs.h"

main( void)
{
    fputs( "started\n", stderr);

/* 7.1: Line number and filename.   */
#line   1234    "cpp"
    assert( __LINE__ == 1234);
    assert( strcmp( __FILE__, "cpp") == 0);

/* 7.2: Filename argument is optional.  */
#line   2345
    assert( __LINE__ == 2345);
    assert( strcmp( __FILE__, "cpp") == 0);

/* 7.3: Argument with macro.    */
#define LINE_AND_FILENAME   1234 "n_7.c"
#line   LINE_AND_FILENAME
    assert( __LINE__ == 1234);
    assert( strcmp( __FILE__, "n_7.c") == 0);

    fputs( "success\n", stderr);
    return  0;
}

