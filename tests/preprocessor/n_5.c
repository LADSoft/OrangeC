/* n_5.c:   Spaces or tabs are allowed at any place in pp-directive line,
        including between the top of a pp-directive line and '#', and between
        the '#' and the directive. */

#include    "defs.h"

main( void)
{
    int     abcde = 5;
/*  |**|[TAB]# |**|[TAB]define |**| MACRO_abcde[TAB]|**| abcde |**| */
/**/	# /**/	define /**/ MACRO_abcde	/**/ abcde /**/

    fputs( "started\n", stderr);

    assert( MACRO_abcde == 5);

    fputs( "success\n", stderr);
    return  0;
}

