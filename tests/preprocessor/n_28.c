/* n_28.c:  __FILE__, __LINE__, __DATE__, __TIME__, __STDC__ and
            __STDC_VERSION__ are predefined.    */

#include    "defs.h"

main( void)
{
    char *  date = __DATE__;
    char *  fname = __FILE__;

    fputs( "started\n", stderr);

/* 28.1:    */
/* Remove directory part (if any).  */
    fname += strlen( fname) - 6;
    assert( strcmp( fname, "n_28.c") == 0);

/* 28.2:    */
    assert( __LINE__ == 19);

/* 28.3:    */
    assert( strlen( __DATE__) == 11);
    assert( date[ 4] != '0');

/* 28.4:    */
    assert( strlen( __TIME__) == 8);

/* 28.5:    */
    assert( __STDC__);

/* 28.6:    */
    assert( __STDC_VERSION__);

/* 28.7:    */
#include    "line.h"

    fputs( "success\n", stderr);
    return  0;
}

