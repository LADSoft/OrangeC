/* n_37.c:  Translation limits. */

#include    "defs.h"

/* 37.1:    Number of parameters in macro: at least 31. */
#define glue31(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E)   \
    a##b##c##d##e##f##g##h##i##j##k##l##m##n##o##p##q##r##s##t##u##v##w##x##y##z##A##B##C##D##E

main( void)
{
    int     ABCDEFGHIJKLMNOPQRSTUVWXYZabcde = 31;
    int     ABCDEFGHIJKLMNOPQRSTUVWXYZabcd_ = 30;
    int     nest = 0;

    fputs( "started\n", stderr);

/* 37.2:    Number of arguments in macro call: at least 31. */
    assert(
        glue31( A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R
            , S, T, U, V, W, X, Y, Z, a, b, c, d, e)
        == 31);

/* 37.3:    Significant initial characters in an internal identifier or a
        macro name: at least 31.  */
    assert( ABCDEFGHIJKLMNOPQRSTUVWXYZabcd_ == 30);

/* 37.4:    Nested conditional inclusion: at least 8 levels.    */
    nest = 0;
#ifdef  A
#else
#   ifdef   B
#   else
#       ifdef   C
#       else
#           ifdef   D
#           else
#               ifdef   E
#               else
#                   ifdef   F
#                   else
#                       ifdef   G
#                       else
#                           ifdef   H
#                           else
                                nest = 8;
#                           endif
#                       endif
#                   endif
#               endif
#           endif
#       endif
#   endif
#endif
    assert( nest == 8);

/* 37.5:    Nested source file inclusion: at least 8 levels.    */
    nest = 0;
#include    "nest1.h"
    assert( nest == 8);

/* 37.6:    Parenthesized expression: at least 32 levels.   */
#if     0 + (1 - (2 + (3 - (4 + (5 - (6 + (7 - (8 + (9 - (10 + (11 - (12 +  \
        (13 - (14 + (15 - (16 + (17 - (18 + (19 - (20 + (21 - (22 + (23 -   \
        (24 + (25 - (26 + (27 - (28 + (29 - (30 + (31 - (32 + 0))))))))))   \
        )))))))))))))))))))))) == 0
    nest = 32;
#endif
    assert( nest == 32);

/* 37.7:    Characters in a string (after concatenation): at least 509. */
    {
        char *  extremely_long_string1 =
"123456789012345678901234567890123456789012345678901234567890123456789\
0123456789012345678901234567890123456789012345678901234567890123456789\
0123456789012345678901234567890123456789012345678901234567890123456789\
0123456789012345678901234567890123456789012345678901234567890123456789\
0123456789012345678901234567890123456789012345678901234567890123456789\
0123456789012345678901234567890123456789012345678901234567890123456789\
0123456789012345678901234567890123456789012345678901234567890123456789\
012345678901234567"
        ;
        assert( strlen( extremely_long_string1) == 507);
    }

/* 37.8:    Characters in a logical source line: at least 509.  */
    {
    int a123456789012345678901234567890 = 123450;   \
    int b123456789012345678901234567890 = 123451;   \
    int c123456789012345678901234567890 = 123452;   \
    int d123456789012345678901234567890 = 123453;   \
    int e123456789012345678901234567890 = 123454;   \
    int f123456789012345678901234567890 = 123455;   \
    int A123456789012345678901234567890 = 123456;   \
    int B123456789012345678901234567890 = 123457;   \
    int C123456789012345678901234567890 = 123458;   \
    int D1234567890123456789012 = 123459;
        assert( a123456789012345678901234567890 == 123450
            && D1234567890123456789012 == 123459);
    }

/* 37.9:    Macro definitions: at least 1024.   */
#include    "m1024.h"
    assert( ZX);

    fputs( "success\n", stderr);
    return  0;
}

