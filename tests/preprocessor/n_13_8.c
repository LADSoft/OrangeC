/* n_13_8.c:    Grouping of sub-expressions in #if expression.  */

#include    "defs.h"

main( void)
{
    fputs( "started\n", stderr);

/* 13.8:    Unary operators are grouped from right to left. */
#if (- -1 != 1) || (!!9 != 1) || (-!+!9 != -1) || (~~1 != 1)
    fputs( "Bad grouping of -, +, !, ~ in #if expression.\n", stderr);
    exit( 1);
#endif

/* 13.9:    ?: operators are grouped from right to left.    */
#if (1 ? 2 ? 3 ? 3 : 2 : 1 : 0) != 3
    fputs( "Bad grouping of ? : in #if expression.\n", stderr);
    exit( 1);
#endif

/* 13.10:   Other operators are grouped from left to right. */
#if (15 >> 2 >> 1 != 1) || (3 << 2 << 1 != 24)
    fputs( "Bad grouping of >>, << in #if expression.\n", stderr);
    exit( 1);
#endif

/* 13.11:   Test of precedence. */
#if 3*10/2 >> !0*2 >> !+!-9 != 1
    fputs( "Bad grouping of -, +, !, *, /, >> in #if expression.\n", stderr);
    exit( 1);
#endif

/* 13.12:   Overall test.  Grouped as:
        ((((((+1 - -1 - ~~1 - -!0) & 6) | ((8 % 9) ^ (-2 * -2))) >> 1) == 7)
        ? 7 : 0) != 7
    evaluated to FALSE.
 */
#if (((+1- -1-~~1- -!0&6|8%9^-2*-2)>>1)==7?7:0)!=7
    fputs( "Bad arithmetic of #if expression.\n", stderr);
    exit( 1);
#endif

    fputs( "success\n", stderr);
    return  0;
}

