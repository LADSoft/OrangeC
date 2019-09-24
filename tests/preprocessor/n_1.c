/* n_1.c:   Conversion of trigraph sequences.   */

#include    "defs.h"

char    quasi_trigraph[] = { '?', '?', ' ', '?', '?', '?', ' ', '?', '?', '%',
            ' ', '?', '?', '^', ' ', '?', '#', '\0' };

main( void)
{
    int     ab = 1, cd = 2;

    fputs( "started\n", stderr);

/* 1.1: The following 9 sequences are valid trigraph sequences. */
    assert( strcmp( "??( ??) ??/??/ ??' ??< ??> ??! ??- ??="
            ,"[ ] \\ ^ { } | ~ #") == 0);

/* 1.2: In directive line.  */
??= define  OR( a, b)   a ??! b
    assert( OR( ab, cd) == 3);

/* 1.3: Any sequence other than above 9 is not a trigraph sequence. */
    assert( strcmp( "?? ??? ??% ??^ ???=", quasi_trigraph) == 0);

    fputs( "success\n", stderr);
    return  0;
}

