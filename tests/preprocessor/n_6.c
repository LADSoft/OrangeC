/* n_6.c:   #include directive. */

#include    "defs.h"
/* 6.1: Header-name quoted by " and " as well as by < and > can include
        standard headers.   */
/* Note: Standard headers can be included any times.    */
#include    "ctype.h"
#include    <ctype.h>

main( void)
{
    int     abc = 3;

    fputs( "started\n", stderr);

    assert( isalpha( 'a'));

/* 6.2: Macro is allowed in #include line.  */
#define HEADER  "header.h"
#include    HEADER
    assert( MACRO_abc == 3);

/* 6.3: With macro nonsence but legal.  */
#undef  MACRO_abc
#define ZERO_TOKEN
#include    ZERO_TOKEN HEADER ZERO_TOKEN
    assert( MACRO_abc == 3);

    fputs( "success\n", stderr);
    return  0;
}

