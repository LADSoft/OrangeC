/* n_13_13.c:   #if expression with macros. */

#include    "defs.h"
#define ZERO_TOKEN
#define MACRO_0         0
#define MACRO_1         1
#define and             &&
#define or              ||
#define not_eq          !=
#define bitor           |

main( void)
{
    fputs( "started\n", stderr);

/* 13.13:   With macros expanding to operators. */
#if     (1 bitor 2) == 3 and 4 not_eq 5 or 0
    /* #if (1 | 2) == 3 && 4 != 5 || 0  */
#else
    fputs(
    "Bad evaluation of macros expanding to operators in #if expression.\n"
            , stderr);
    exit( 1);
#endif

/* 13.14:   With macros expanding to 0 token, nonsence but legal.   */
#if     ZERO_TOKEN MACRO_1 ZERO_TOKEN > ZERO_TOKEN MACRO_0 ZERO_TOKEN
    /* #if 1 > 0    */
#else
    fputs(
    "Bad evaluation of macros expanding to 0 token in #if expression.\n"
            , stderr);
    exit( 1);
#endif

    fputs( "success\n", stderr);
    return  0;
}

