/* n_13.c:  Valid operators in #if expression.  */

/* Valid operators are (precedence in this order) :
    defined, (unary)+, (unary)-, ~, !,
    *, /, %,
    +, -,
    <<, >>,
    <, >, <=, >=,
    ==, !=,
    &,
    ^,
    |,
    &&,
    ||,
    ? :
 */

#include    "defs.h"

main( void)
{
    fputs( "started\n", stderr);

/* 13.1:    Bit shift.  */
#if     1 << 2 != 4 || 8 >> 1 != 4
    fputs( "Bad arithmetic of <<, >> operators.\n", stderr);
    exit( 1);
#endif

/* 13.2:    Bitwise operators.  */
#if     (3 ^ 5) != 6 || (3 | 5) != 7 || (3 & 5) != 1
    fputs( "Bad arithmetic of ^, |, & operators.\n", stderr);
    exit( 1);
#endif

/* 13.3:    Result of ||, && operators is either of 1 or 0. */
#if     (2 || 3) != 1 || (2 && 3) != 1 || (0 || 4) != 1 || (0 && 5) != 0
    fputs( "Bad arithmetic of ||, && operators.\n", stderr);
    exit( 1);
#endif

/* 13.4:    ?, : operator.  */
#if     (0 ? 1 : 2) != 2
    fputs( "Bad arithmetic of ?: operator.\n", stderr);
    exit( 1);
#endif

    fputs( "success\n", stderr);
    return  0;
}

