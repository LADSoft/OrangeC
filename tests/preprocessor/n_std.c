/*
 *      n_std.c
 *
 * 1998/08      made public                                     kmatsui
 * 2002/08      revised not to conflict with C99 Standard       kmatsui
 * 2004/10      added a few testcases for macro expansion       kmatsui
 *
 *   Samples to test Standard C preprocessing.
 *   This is a strictly-comforming program.
 *   Any Standard-comforming translator must translate successfully this
 * program.  The generated execution program must be executed with the message
 * <End of "n_std.c"> on stdout and no other messages on stderr.
 *   A translator must process also #error directive properly, which is not
 * included here because the directive might cause translator to terminate.
 */


#include    "defs.h"

#define ZERO_TOKEN
#define TWO_ARGS        a,b
#define MACRO_0         0
#define MACRO_1         1
#define sub( x, y)      (x - y)
#define str( a)         # a
#define xstr( a)        str( a)
#define glue( a, b)     a ## b
#define xglue( a, b)    glue( a, b)

void    n_1( void);
void    n_2( void);
void    n_3( void);
void    n_4( void);
void    n_5( void);
void    n_6( void);
void    n_7( void);
void    n_9( void);
void    n_10( void);
void    n_11( void);
void    n_12( void);
void    n_13( void);
void    n_13_5( void);
void    n_13_7( void);
void    n_13_8( void);
void    n_13_13( void);
void    n_15( void);
void    n_18( void);
void    n_19( void);
void    n_20( void);
void    n_21( void);
void    n_22( void);
void    n_23( void);
void    n_24( void);
void    n_25( void);
void    n_26( void);
void    n_27( void);
void    n_28( void);
void    n_29( void);
void    n_30( void);
void    n_32( void);
void    n_37( void);

int main( void)
{
    n_1();
    n_2();
    n_3();
    n_4();
    n_5();
    n_6();
    n_7();
    n_9();
    n_10();
    n_11();
    n_12();
    n_13();
    n_13_5();
    n_13_7();
    n_13_8();
    n_13_13();
    n_15();
    n_18();
    n_19();
    n_20();
    n_21();
    n_22();
    n_23();
    n_24();
    n_25();
    n_26();
    n_27();
    n_28();
    n_29();
    n_30();
    n_32();
    n_37();
    puts( "<End of \"n_std.c\">");
    return  0;
}

char    quasi_trigraph[] = { '?', '?', ' ', '?', '?', '?', ' '
            , '?', '?', '%', ' ', '?', '?', '^', ' ', '?', '#', '\0' };

void    n_1( void)
/*      Conversion of trigraph sequences.   */
{
    int     ab = 1, cd = 2;

/* 1.1: The following 9 sequences are valid trigraph sequences. */
    assert( strcmp( "??( ??) ??/??/ ??' ??< ??> ??! ??- ??="
            ,"[ ] \\ ^ { } | ~ #") == 0);

/* 1.2: In directive line.  */
??= define  OR( a, b)   a ??! b
    assert( OR( ab, cd) == 3);

/* 1.3: Any sequence other than above 9 is not a trigraph sequence. */
    assert( strcmp( "?? ??? ??% ??^ ???=", quasi_trigraph) == 0);
}

void    n_2( void)
/*      Line splicing by <backslash><newline> sequence. */
{
    int     ab = 1, cd = 2, ef = 3, abcde = 5;

/* 2.1: In a #define directive line, between the parameter list and the
        replacement text.   */
#define FUNC( a, b, c)  \
        a + b + c
    assert( FUNC( ab, cd, ef) == 6);

/* 2.2: In a #define directive line, among the parameter list and among the
        replacement text.   */
#undef  FUNC
#define FUNC( a, b  \
    , c)            \
    a + b           \
    + c
    assert (FUNC( ab, cd, ef) == 6);

/* 2.3: In a string literal.    */
    assert (strcmp( "abc\
de", "abcde") == 0);

/* 2.4: <backslash><newline> in midst of an identifier. */
    assert( abc\
de == 5);

/* 2.5: <backslash><newline> by trigraph.   */
    assert( abc??/
de == 5);
}

void    n_3( void)
/*      Handling of comment.    */
{
    int     abcd = 4;

/* 3.1: A comment is converted to one space.    */
    assert( strcmp( str( abc/* comment */de), "abc de") == 0);

/* 3.2: // is not a comment of C.   */
/*    assert( strcmp( str( //), "//") == 0);    */

/* 3.3: Comment is parsed prior to the parsing of preprocessing directive.  */
#if     0
    "nonsence"; /*
#else
    still in
    comment     */
#else
#define MACRO_abcd  /*
    in comment
    */  abcd
#endif
    assert( MACRO_abcd == 4);
}

void    n_4( void)
/* Special tokens.  */
{
/* 4.1: Digraph spellings in directive line.    */
%: define  stringize( a)    %: a

    assert( strcmp( stringize( abc), "abc") == 0);

/* 4.2: Digraph spellings are retained in stringization.    */
    assert( strcmp( stringize( <:), "<" ":") == 0);
}

void    n_5( void)
/*      Spaces or tabs are allowed at any place in pp-directive line,
        including between the top of a pp-directive line and '#', and between
        the '#' and the directive. */
{
    int     abcde = 5;
/*  |**|[TAB]# |**|[TAB]define |**| MACRO_abcde[TAB]|**| abcde |**| */
/**/    # /**/  define /**/ MACRO_abcde /**/ abcde /**/
    assert( MACRO_abcde == 5);
}

/* 6.1: Header-name quoted by " and " as well as by < and > can include
        standard headers.   */
/* Note: Standard headers can be included any times.    */
#include    "ctype.h"
#include    <ctype.h>

void    n_6( void)
/*      #include directive. */
{
    int     abc = 3;

    assert( isalpha( 'a'));

/* 6.2: Macro is allowed in #include line.  */
#define HEADER  "header.h"
#include    HEADER
    assert( MACRO_abc == 3);

/* 6.3: With macro nonsence but legal.  */
#undef  MACRO_abc
#include    ZERO_TOKEN HEADER ZERO_TOKEN
    assert( MACRO_abc == 3);
}

void    n_7( void)
/*      #line directive.    */
{
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
}

/* Restore to correct line number and filename. */
#line   248 "n_std.c"

void    n_9( void)
/*      #pragma directive.  */
{
/* 9.1: Any #pragma directive should be processed or ignored, should not
        be diagnosed as an error.   */
#pragma once
#pragma who knows ?
}

void    n_10( void)
/*      #if, #elif, #else and #endif pp-directive.  */
{
/* 10.1:    */
/* Note: an undefined identifier in #if expression is replaced to 0.    */
#if     a
    assert( a);
#elif   MACRO_0
    assert( MACRO_0);
#elif   MACRO_1         /* Valid block  */
    assert( MACRO_1);
#else
    assert( 0);
#endif

/* 10.2:    Comments must be processed even if in skipped #if block.    */
/* At least tokenization of string literal and character constant is necessary
        to process comments, e.g. /* is not a comment mark in string literal.
 */
#ifdef  UNDEFINED
    /* Comment  */
    "in literal /* is not a comment"
#endif
}

void    n_11( void)
/*      Operator "defined" in #if or #elif directive.   */
{
    int     abc = 1, a = 0;

/* 11.1:    */
#undef  MACRO_abc
#define MACRO_abc   abc
#if     defined a
    assert( a);
#else
    assert( MACRO_abc);
#endif
#if     defined (MACRO_abc)
    assert( MACRO_abc);
#else
    assert( a);
#endif

/* 11.2:    "defined" is an unary operator whose result is 1 or 0.  */
#if     defined MACRO_0 * 3 != 3
    fputs( "Bad handling of defined operator.\n", stderr);
#endif
#if     (!defined ZERO_TOKEN != 0) || (-defined ZERO_TOKEN != -1)
    fputs( "Bad grouping of defined, -, ! in #if expression.\n", stderr);
#endif
}

#include    <limits.h>

void    n_12( void)
/*      Integer preprocessing number token and type of #if expression.  */
{
/* 12.1:    */
#if     LONG_MAX <= LONG_MIN
    fputs( "Bad evaluation of long.\n", stderr);
#endif
#if     LONG_MAX <= 1073741823  /* 0x3FFFFFFF   */
    fputs( "Bad evaluation of long.\n", stderr);
#endif

/* 12.2:    */
#if     ULONG_MAX / 2 < LONG_MAX
    fputs( "Bad evaluation of unsigned long.\n", stderr);
#endif

/* 12.3:    Octal number.   */
#if     0177777 != 65535
    fputs( "Bad evaluation of octal number.\n", stderr);
#endif

/* 12.4:    Hexadecimal number. */
#if     0Xffff != 65535 || 0XFfFf != 65535
    fputs( "Bad evaluation of hexadecimal number.\n", stderr);
#endif

/* 12.5:    Suffix 'L' or 'l'.  */
#if     0L != 0 || 0l != 0
    fputs( "Bad evaluation of 'L' suffix.\n", stderr);
#endif

/* 12.6:    Suffix 'U' or 'u'.  */
#if     1U != 1 || 1u != 1
    fputs( "Bad evaluation of 'U' suffix.\n", stderr);
#endif

/* 12.7:    Negative integer.   */
#if     0 <= -1
    fputs( "Bad evaluation of negative number.\n", stderr);
#endif
}

void    n_13( void)
/*      evaluation of #if expressions.  */
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
{
/* 13.1:    Bit shift.  */
#if     1 << 2 != 4 || 8 >> 1 != 4
    fputs( "Bad arithmetic of <<, >> operators.\n", stderr);
#endif

/* 13.2:    Bitwise operators.  */
#if     (3 ^ 5) != 6 || (3 | 5) != 7 || (3 & 5) != 1
    fputs( "Bad arithmetic of ^, |, & operators.\n", stderr);
#endif

/* 13.3:    Result of ||, && operators is either of 1 or 0. */
#if     (2 || 3) != 1 || (2 && 3) != 1 || (0 || 4) != 1 || (0 && 5) != 0
    fputs( "Bad arithmetic of ||, && operators.\n", stderr);
#endif

/* 13.4:    ?, : operator.  */
#if     (0 ? 1 : 2) != 2
    fputs( "Bad arithmetic of ?: operator.\n", stderr);
#endif
}

void    n_13_5( void)
/*      Arithmetic conversion in #if expressions.   */
{
/* 13.5:    The usual arithmetic conversion is not performed on bit shift.  */
#if     -1 << 3U > 0
    fputs( "Bad conversion of bit shift operands.\n", stderr);
#endif

/* 13.6:    Usual arithmetic conversions.   */
#if     -1 <= 0U        /* -1 is converted to unsigned long.    */
    fputs( "Bad arithmetic conversion.\n", stderr);
#endif

#if     -1 * 1U <= 0
    fputs( "Bad arithmetic conversion.\n", stderr);
#endif

/* Second and third operands of conditional operator are converted to the
        same type, thus -1 is converted to unsigned long.    */
#if     (1 ? -1 : 0U) <= 0
    fputs( "Bad arithmetic conversion.\n", stderr);
#endif
}

void    n_13_7( void)
/*      Short-circuit evaluation of #if expression. */
{
/* 13.7:    10/0 or 10/MACRO_0 are never evaluated, "divide by zero" error
        cannot occur.   */

#if     0 && 10 / 0
#endif
#if     not_defined && 10 / not_defined
#endif
#if     MACRO_0 && 10 / MACRO_0 > 1
#endif
#if     MACRO_0 ? 10 / MACRO_0 : 0
#endif
#if     MACRO_0 == 0 || 10 / MACRO_0 > 1        /* Valid block  */
#else
#endif
}

void    n_13_8( void)
/*      Grouping of sub-expressions in #if expression.  */
{
/* 13.8:    Unary operators are grouped from right to left. */
#if     (- -1 != 1) || (!!9 != 1) || (-!+!9 != -1) || (~~1 != 1)
    fputs( "Bad grouping of -, +, !, ~ in #if expression.\n", stderr);
#endif

/* 13.9:    ?: operators are grouped from right to left.    */
#if     (1 ? 2 ? 3 ? 3 : 2 : 1 : 0) != 3
    fputs( "Bad grouping of ? : in #if expression.\n", stderr);
#endif

/* 13.10:   Other operators are grouped from left to right. */
#if     (15 >> 2 >> 1 != 1) || (3 << 2 << 1 != 24)
    fputs( "Bad grouping of >>, << in #if expression.\n", stderr);
#endif

/* 13.11:   Test of precedence. */
#if     3*10/2 >> !0*2 >> !+!-9 != 1
    fputs( "Bad grouping of -, +, !, *, /, >> in #if expression.\n", stderr);
#endif

/* 13.12:   Overall test.  Grouped as:
        ((((((+1 - -1 - ~~1 - -!0) & 6) | ((8 % 9) ^ (-2 * -2))) >> 1) == 7)
        ? 7 : 0) != 7
    evaluated to FALSE.
 */
#if     (((+1- -1-~~1- -!0&6|8%9^-2*-2)>>1)==7?7:0)!=7
    fputs( "Bad arithmetic of #if expression.\n", stderr);
#endif
}

void    n_13_13( void)
/*      #if expression with macros. */
{
#define and             &&
#define or              ||
#define not_eq          !=
#define bitor           |

/* 13.13:   With macros expanding to operators. */
#if     (1 bitor 2) == 3 and 4 not_eq 5 or 0
    /* #if (1 | 2) == 3 && 4 != 5 || 0  */
#else
    fputs(
    "Bad evaluation of macros expanding to operators in #if expression.\n"
        , stderr);
#endif

/* 13.14:   With macros expanding to 0 token, nonsence but legal.   */
#if     ZERO_TOKEN MACRO_1 ZERO_TOKEN > ZERO_TOKEN MACRO_0 ZERO_TOKEN
    /* #if 1 > 0    */
#else
    fputs(
    "Bad evaluation of macros expanding to 0 token in #if expression.\n"
        , stderr);
#endif
}

void    n_15( void)
/*      #ifdef, #ifndef directives. */
{

/* 15.1:    #ifdef directive.   */
#ifdef  MACRO_1         /* Valid block  */
    assert( MACRO_1);
#else
    assert( MACRO_0);
#endif

/* 15.2:    #ifndef directive.  */
#ifndef MACRO_1
    assert( MACRO_0);
#else               /* Valid block  */
    assert( MACRO_1);
#endif
}

void    n_18( void)
/*      #define directive.  */
/* Excerpts from ISO C 6.8.3 "Examples".    */
#define OBJ_LIKE        (1-1)
#define FTN_LIKE(a)     ( a )
{
    int     c = 3;

/* 18.1:    Definition of an object-like macro. */
    assert( OBJ_LIKE == 0);
#ifndef ZERO_TOKEN
    fputs( "Can't define macro to 0-token.\n", stderr);
#endif

/* 18.2:    Definition of a function-like macro.    */
    assert( FTN_LIKE( c) == 3);

/* 18.3:    Spelling in string identical to parameter is not a parameter.   */
#define STR( n1, n2)    "n1:n2"
    assert( strcmp( STR( 1, 2), "n1:n2") == 0);
}

void    n_19( void)
/*      Valid re-definitions of macros. */
{
    int     c = 1;

/* 19.1:    */
#define OBJ_LIKE    /* white space */  (1-1) /* other */

/* 19.2:    */
#define FTN_LIKE( a     )(  /* note the white space */  \
                        a  /* other stuff on this line
                           */ )
    assert( FTN_LIKE( c) == 1);
}

void    n_20( void)
/*      Macro lexically identical to keyword. */
{
/* 20.1:    */
#define float   double
    float   fl;
    assert( sizeof fl == sizeof (double));
}

void    n_21( void)
/*      Tokenization (No preprocessing tokens are merged implicitly).   */
{
    int     a = 1, x = 2, y = -3;

/* 21.1:    */
#define MINUS   -
    assert( -MINUS-a == -1);

/* 21.2:    */
#undef  sub
#define sub( a, b)  a-b     /* '(a)-(b)' is better  */
#define Y   -y              /* '(-y)' is better     */
/*  x- -y   */
    assert( sub( x, Y) == -1);
}

void    n_22( void)
/*      Tokenization of preprocessing number.   */
{
#define EXP         1

/* 22.1:    12E+EXP is a preprocessing number, EXP is not expanded. */
    assert( strcmp( xstr( 12E+EXP), "12E+EXP") == 0);

/* 22.2:    .2e-EXP is also a preprocessing number. */
    assert( strcmp( xstr( .2e-EXP), ".2e-EXP") == 0);

/* 22.3:    + or - is allowed only following E or e, 12+EXP is not a
        preprocessing number.   */
    assert( strcmp( xstr( 12+EXP), "12+1") == 0);
}

void    n_23( void)
/*      ## operator in macro definition.    */
{
    int     xy = 1;

/* 23.1:    */
    assert( glue( x, y) == 1);

/* 23.2:    Generate a preprocessing number.    */
#undef  EXP
#define EXP     2
    assert( xglue( .12e+, EXP) == 12.0);
}

void    n_24( void)
/*      # operator in macro definition. */
{
/* 24.1:    */
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
#define f(a)        a
    assert( strcmp( xstr( x-f(y)), "x-y") == 0);
}

void    n_25( void)
/*      Macro arguments are pre-expanded (unless the argument is an
        operand of # or ## operator) separately, that is, are macro-replaced
        completely prior to rescanning. */
{
    int     a = 1, b = 2, abc = 3, MACRO_0MACRO_1 = 2;

#undef sub
#define sub( x, y)      (x - y)

/* 25.1:    "TWO_ARGS" is read as one argument to "sub", then expanded to
        "a,b", then "x" is substituted by "a,b".    */
    assert( sub( TWO_ARGS, 1) == 1);

/* 25.2:    An argument pre-expanded to 0-token.    */
    assert( sub( ZERO_TOKEN, a) == -1);

/* 25.3:    "glue( a, b)" is pre-expanded.  */
    assert( xglue( glue( a, b), c) == 3);

/* 25.4:    Operands of ## operator are not pre-expanded.   */
    assert( glue( MACRO_0, MACRO_1) == 2);

/* 25.5:    Operand of # operator is not pre-expanded.  */
    assert( strcmp( str( ZERO_TOKEN), "ZERO_TOKEN") == 0);
}

#undef  f

#ifdef  void
int     f( a)
    int     a;
{
    return  a;
}

int     g( a)
    int     a;
{
    return  a * 2;
}
#else
int     f( int a)
{
    return  a;
}

int     g( int a)
{
    return  a * 2;
}
#endif

void    n_26( void)
/*      The name once replaced is not furthur replaced. */
{
    int     x = 1;
    int     AB = 1;
    int     Z[1];
    Z[0] = 1;

/* 26.1:    Directly recursive macro definition.    */
/*  Z[0];   */
#define Z   Z[0]
    assert( Z == 1);

/* 26.2:    Intermediately recursive macro definition.  */
/*  AB; */
#define AB  BA
#define BA  AB
    assert( AB == 1);

/* 26.3:    Directly recursive function-like macro definition.  */
/*  x + f(x);   */
#define f(a)    a + f(a)
    assert( f( x) == 2);

/* 26.4:    Intermediately recursive function-like macro definition.    */
/*  x + x + g( x);  */
#define g(a)    a + h( a)
#define h(a)    a + g( a)
    assert( g( x) == 4);

/* 26.5:    Rescanning encounters the non-replaced macro name.  */
/*  Z[0] + f( Z[0]);    */
    assert( f( Z) == 2);
}

void    n_27( void)
/*      Rescanning of a macro raplace any macro call in the replacement
        text after substitution of parameters by pre-expanded-arguments.  This
        re-examination may involve the succeding sequences from the source
        file (what a queer thing!). */
{
    int     a = 1, b = 2, c, m = 1, n = 2;

/* 27.1:    Cascaded use of object-like macros. */
#define NEST8   NEST7 + 8
#define NEST7   NEST6 + 7
#define NEST6   NEST5 + 6
#define NEST5   NEST4 + 5
#define NEST4   NEST3 + 4
#define NEST3   NEST2 + 3
#define NEST2   NEST1 + 2
#define NEST1   1
    assert( NEST8 == 36);

/* 27.2:    Cascaded use of function-like macros.   */
#define FUNC4( a, b)    FUNC3( a, b) + NEST4
#define FUNC3( a, b)    FUNC2( a, b) + NEST3
#define FUNC2( a, b)    FUNC1( a, b) + NEST2
#define FUNC1( a, b)    (a) + (b)
    assert( FUNC4( NEST1, NEST2) == 23);

/* 27.3:    An identifier generated by ## operator is subject to expansion. */
    assert( glue( MACRO_, 1) == 1);

#define head            sub(
#define math( op, a, b) op( (a), (b))

/* 27.4:    'sub' as an argument of math() is not pre-expanded, since '(' is
        missing.    */
    assert( math( sub, a, b) == -1);

/* 27.5:    Queer thing.    */
    c = head a,b );
    assert( c == -1);

/* 27.6:    Recursive macro (the 2nd 'm' is expanded to 'n' since it is in
        source file).   */
#define m       n
#define n( a)   a 
    assert( m( m) == 2);
}

void    n_28( void)
/*      __FILE__, __LINE__, __DATE__, __TIME__, __STDC__ and
        __STDC_VERSION are predefined.  */
{
    char *  date = __DATE__;

/* 28.1:    */
    assert( strcmp( __FILE__, "n_std.c") == 0);

/* 28.2:    */
    assert( __LINE__ == 779);

/* 28.3:    */
    assert( strlen( __DATE__) == 11);
    assert( date[ 4] != '0');

/* 28.4:    */
    assert( strlen( __TIME__) == 8);

/* 28.5:    */
    assert( __STDC__);

/* 28.6:    */
    assert( __STDC_VERSION__ >= 199409L);

/* 28.7:    */
#include    "line.h"
}

void    n_29( void)
/*      #undef directive.   */
{
    int     DEFINED = 1;

/* 29.1:    Undefined macro is not a macro. */
#define DEFINED
#undef  DEFINED
    assert( DEFINED == 1);

/* 29.2:    Undefining undefined name is not an error.  */
#undef  UNDEFINED
}

void    n_30( void)
/*      Macro calls.    */
/*  Note:   Comma separate the arguments of function-like macro call,
        but comma between matching inner parenthesis doesn't.  This feature
        is tested on so many places in this suite especially on *.c samples
        which use assert() macro, that no separete item to test this feature
        is provided.    */
{
#undef  FUNC
#define FUNC( a, b, c)      a + b + c

    int     a = 1, b = 2, c = 3;

/* 30.1:    A macro may cross lines.    */
    assert
    (
        FUNC
        (
            a,
            b,
            c
        )
        == 6
    );
}

void    n_32( void)
/*      Escape sequence in character constant in #if expression.    */
{
/* 32.1:    Character octal escape sequence.    */
#if     '\123' != 83
    fputs( "Bad evaluation of octal escape sequence.\n", stderr);
#endif

/* 32.2:    Character hexadecimal escape sequence.  */
#if     '\x1b' != '\033'
    fputs( "Bad evaluation of hexadecimal escape sequence.\n", stderr);
#endif
}

void    n_37( void)
/* Translation limits.  */
{
#define     MACRO_8     8

/* 37.1:    Number of parameters in macro: at least 31. */
#define glue31(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E)   \
    a##b##c##d##e##f##g##h##i##j##k##l##m##n##o##p##q##r##s##t##u##v##w##x##y##z##A##B##C##D##E

    int     ABCDEFGHIJKLMNOPQRSTUVWXYZabcde = 31;
    int     ABCDEFGHIJKLMNOPQRSTUVWXYZabcd_ = 30;
    int     nest = 0;

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
        char *  extremely_long_string =
"123456789012345678901234567890123456789012345678901234567890123456789\
0123456789012345678901234567890123456789012345678901234567890123456789\
0123456789012345678901234567890123456789012345678901234567890123456789\
0123456789012345678901234567890123456789012345678901234567890123456789\
0123456789012345678901234567890123456789012345678901234567890123456789\
0123456789012345678901234567890123456789012345678901234567890123456789\
0123456789012345678901234567890123456789012345678901234567890123456789\
012345678901234567"
        ;
        assert( strlen( extremely_long_string) == 507);
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
#undef  AB
#undef  BA
#undef  OR

#include    "m1024.h"
    assert( ZX);
}

