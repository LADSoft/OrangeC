/*
 *  Classified Tydeman Consulting Proprietary
 *  Copyright (C) 2002, 2017 Tydeman Consulting.  All rights reserved.
 *
 *  Fred J. Tydeman	tydeman@tybor.com
 *
 * Any person is hereby authorized to copy, use, and distribute, this
 * one sample test, subject to the following conditions:
 *
 *      1.  the software may not be redistributed for a fee except as
 *          reasonable to cover media costs;
 *      2.  any copy of the software must include this notice, as well as
 *          any other embedded copyright notices; and
 *      3.  any distribution of this software or derivative works thereof
 *          must comply with all applicable U.S. export control laws.
 *
 * Floating-Point C Extensions (FPCE) have been added to C99 (the
 * revision of the C language finished in late 1999).  If you are
 * interested in other sample tests of the FPCE Test Suite, which
 * tests the floating-point and numerics capabilities of C/C++
 * compilers and libraries, please see the FTP site:
 *      ftp://ftp.tybor.com
 *     user ID: anonymous
 *    password: your email address
 *
 ***********************************************************************
 *  FILE: tint2flt.c
 *
 *  Assertion being tested:
 *
 * C11/C99:
 * 6.3.1.4 Real floating and integer
 *    When a value of integer type is converted to a real floating type,
 *    if the value being converted can be represented exactly in the new
 *    type, it is unchanged.
 *
 *  This test is for C11/C99 with IEEE-754 floating-point support.
 *  Test conversions from integer types to floating-point types.
 *  For all integer types, test with powers of 2.
 *  For signed integer types, also test with negative of powers of 2.
 *  Checks for same value and no exceptions.
 *
 * If you need to disable trapping of floating-point exceptions,
 * you might try one of the next two ways:
 *  #define FPU_DISABLE_TRAPS (void)feholdexcept( &current_fenv );
 *  #define FPU_DISABLE_TRAPS (void)fedisabletrap(FE_TRAP_ALL);
 * in tflt2int.h.  You will also need to define NEED_TRAPS_DISABLED.
 *
 * Results of running this test on various compilers:
 * (as reported by several users of this test; your results may vary)
 *
 * Failures    Hardware      Operating system Compiler, version, and options
 *      0 Apple Mac PPC G4   Mac OS X 10.3.7  Gnu gcc 3.3 (build 1495)
 *      0 Apple Mac PPC G4   Mac OS X 10.3.7  IBM XL C 6.0 + Gnu gcc 3.3
 *      0 Apple PowerPC G3   Linux 2.4.19     Gnu gcc 3.2.2 w/ glibc ?; no long long
 *      0 Apple PowerPC G4 7450 MacOS 10.2.5  Gnu gcc 3.3.2 w/ glibc ?
 *      0 DEC Alpha          Linux 2.4.18     Gnu gcc 3.3.1; no long long
 *      0 DEC Alpha 21064    Linux 2.2.22     Gnu gcc 3.3
 *      0 HP Integrity       HP-UX 11i V3     HP c99, A.06.23, +decfp
 *      0 HP PA RISC rp5470  Linux 3.0        cc 3.0.4; no long long
 *      0 HP ia64 Itanium II HP UX 11         HP cc; no long long
 *      0 IBM Power 6        AIX 6.1.7.17     IBM XL C/C++ for AIX V11.1
 *      0 IBM Power 6        AIX 6.1.7.17     gcc 4.5.1; -maix64 -lm
 *      0 Intel Core i5-64   Linux Fedora 14  pcc 0.9.9 (2010/11/19) + glibc-2.13-1 (x86_64)
 *      0 Intel Core i5-64   Linux Fedora 14  pcc 0.9.9 (2011/02/03) + glibc-2.13-1 (x86_64)
 *      0 Intel Core i5-64   Linux Fedora 14  pcc 1.0.0 (2011/04/01) + glibc-2.13-1 (x86_64)
 *      0 Intel Core i5-64   Linux Fedora 15  clang 2.8; -std=c99 -lm
 *      0 Intel Core i5-64   Linux Fedora 15  gcc 4.6.0-9 (2011/05/30) + glibc-2.14-3 (x86_64)
 *      0 Intel Core i5-64   Linux Fedora 16  clang 2.9-10; -std=c99 -lm
 *      0 Intel Core i5-64   Linux Fedora 16  gcc 4.6.3-2 + glibc-2.14.90-24 (x86_64)
 *      0 Intel Core i5-64   Linux Fedora 16  pcc 1.0.0-2 (2011/12/16) + glibc-2.14.90-24 (x86_64)
 *      0 Intel Core i5-64   Linux Fedora 18  gcc 4.7.2-8 + glibc-2.16-24 (x86_64) -std=gnu11 -mfpmath=387
 *      0 Intel Core i5-64   Linux Fedora 18  gcc 4.7.2-8 + glibc-2.16-24 (x86_64) -std=gnu11 -mfpmath=sse
 *      0 Intel Core i5-64   Solaris 11.1     Solaris Studio 12.3
 *      0 Intel Core i7-64   Linux debian6    gcc 4.4.5-8 + glibc (x86_64)
 *      0 Intel Core i7-64   Ubuntu 3.2.0-40  gcc 4.6.3 + eglibc 2.15-0
 *      0 Intel Core2 Duo    Linux Fedora 14  Gnu gcc 4.5.1-4 + glibc 2.12.90-15; -std=gnu99 -frounding-math
 *      0 Intel Core2 Duo    Linux Fedora 15  Gnu gcc 4.6.1-9 + glibc 2.14-5; -std=gnu99 -frounding-math
 *      0 Intel Core2 Duo    Linux Fedora 16  Gnu gcc 4.6.2-1 + glibc 2.14.90-21; -std=gnu99 -frounding-math
 *      0 Intel Core2 Duo    Linux Fedora 16  clang 2.9-6; -std=c99 -lm
 *      0 Intel Core2 Duo    Linux Fedora 16  pcc 1.0.0-2 (2011/12/16) + glibc-2.14.90-24
 *      0 Intel Core2 Duo    Linux Fedora 17  Intel C/C++ icc 13.1 -std=c99 -strict-ansi -fp-model strict -O0
 *      0 Intel Core2 Duo    Linux Fedora 17  clang 3.0-10; -std=c1x -pedantic -lm
 *      0 Intel Core2 Duo    Linux Fedora 17  gcc 4.7.0-2 + glibc 2.15-32; -std=gnu11 -mieee-fp
 *      0 Intel Core2 Duo    OS/2 eCS 2.0     Open Watcom C/C++ v1.8
 *      0 Intel Core2 Duo    OS/2 eCS 2.0     Open Watcom C/C++ v1.9
 *      0 Intel Core2 Duo    Windows Vista    LCC 2012/10/22
 *      0 Intel Pentium 4    Linux Fedora  6  Gnu gcc 4.1.2-13.fc6 + glibc2.5-18.fc6
 *      0 Intel Pentium 4    Linux Fedora 10  Gnu gcc 4.3.2-7 + glibc 2.9-3
 *      0 Intel Pentium 4    Linux Fedora 10  Intel C/C++ icc 11.1 -std=c99 -strict-ansi -fp-model strict -O0
 *      0 Intel Pentium 4    Linux Fedora 11  Gnu gcc 4.4.0-4 + glibc 2.10.1-2
 *      0 Intel Pentium 4    Linux Fedora 11  Gnu gcc 4.4.1-2 + glibc 2.10.1-5
 *      0 Intel Pentium 4    Linux Fedora 14  Gnu gcc 4.5.1-4 + glibc 2.12.90-15; -std=gnu99 -frounding-math
 *      0 Intel Pentium 4    Linux Fedora 15  clang 2.8; -std=c99 -lm32
 *      0 Intel Pentium 4    OS/2 eCS 1.1     Open Watcom C/C++ v1.3
 *      0 Intel Pentium 4    OS/2 eCS 1.1     Open Watcom C/C++ v1.4
 *      0 Intel Pentium 4    OS/2 eCS 1.1     Open Watcom C/C++ v1.5
 *      0 Intel Pentium 4    SuSE Linux 9.1   Gnu gcc 3.3.3-41 + glibc 2.3.3-97
 *      0 Intel Pentium 4    Windows XP       Digital Mars 8.39
 *      0 Intel Pentium 4    Windows XP       Digital Mars 8.47
 *      0 Intel Pentium 4    Windows XP       Digital Mars 8.48
 *      0 Intel Pentium 4    Windows XP       Digital Mars 8.50
 *      0 Intel Pentium 4    Windows XP       Digital Mars 8.51
 *      0 Intel Pentium 4    Windows XP       LCC 2003/01/12
 *      0 Intel Pentium 4    Windows XP       LCC 2004/07/26
 *      0 Intel Pentium 4    Windows XP       LCC 2006/04/10
 *      0 Intel Pentium 4    Windows XP       LCC 2008/07/24 -ansic
 *      0 Intel Pentium 4    Windows XP       Metrowerks CodeWarrior Pro 8.3
 *      0 Intel Pentium 4    Windows XP       Metrowerks CodeWarrior Pro 9.2
 *      0 Intel Pentium III  OS/2 v4          Gnu gcc 2.5.7 + emx 0.8h; no long long
 *      0 Intel Pentium III  OS/2 v4          Gnu gcc 2.7.2 + emx 0.9b + fix05
 *      0 Intel Pentium III  OS/2 v4          Gnu gcc 2.7.2.1 + emx 0.9c + fix02
 *      0 Intel Pentium III  OS/2 v4          Gnu gcc 2.8.1 + emx 0.9d
 *      0 Intel Pentium III  OS/2 v4+DOS      Borland C/C++ v4
 *      0 Intel Pentium III  RedHat Linux 7   Gnu gcc 2.96 + glibc 2.1.92
 *      0 Intel Pentium III  RedHat Linux 7   Intel C/C++ icc 7 w/ Gnu gcc 2.96 + glibc 2.1.92
 *      0 Intel Pentium III  RedHat Linux 8   Comeau 4.3.0.1 w/ Gnu gcc 3.2 + glibc 2.2.93
 *      0 Intel Pentium III  RedHat Linux 8   Gnu gcc 3.2 + glibc 2.2.93
 *      0 Intel Pentium III  RedHat Linux 8   Gnu gcc 3.3.1 + glibc 2.3.2-4.80.6
 *      0 Intel Pentium III  RedHat Linux 8   Intel C/C++ icc 7 w/ Gnu gcc 3.2 + glibc 2.2.93
 *      0 Intel Pentium III  RedHat Linux 8   Intel C/C++ icc 7.1 w/ Gnu gcc 3.3 + glibc 2.3.2-4.80.6
 *      0 Intel Pentium III  Windows 98       Digital Mars 8.35.6
 *      0 Intel Pentium III  Windows 98       LCC 2002/02/10; no ULLONG
 *      0 Intel Pentium III  Windows 98       LCC 2002/06/06; no ULLONG
 *      0 Intel Pentium Pro  RedHat Linux 4   Gnu gcc 2.7.2.1 + libc 5.3.12 + libg++ 2.7.1.4
 *      0 Intel Pentium Pro  Windows NT       IBM VisualAge for C/C++ 3.5.7
 *      0 Intel Pentium Pro  Windows NT       Intel C/C++ 2.4
 *      0 Intel x86          Solaris 9        Sun ONE Studio 8, Sun C 5.5
 *      0 Intel Xeon E7-4850-64 Linux 2.6.32  gcc 4.7.1 + glibc 2.12
 *      0 Intel Xeon E7-4850-64 Linux 2.6.32  icc 13.0.1
 *      0 Intel Xeon X7550-64 Linux RedHat 2.6.18 gcc 4.4.6-3 + glibc 2.5
 *      0 Intel Xeon X7550-64 Linux RedHat 2.6.18 icc 12.1.0
 *      0 Intel Xeon X7560-64 Linux Bull 2.6.32 gcc 4.4.5-6 + glibc 2.12
 *      0 Intel Xeon X7560-64 Linux Bull 2.6.32 icc 12.1.0
 *      0 Intel/ARM XScale   RISC OS 5        Norcroft C 5.53
 *      0 Sun SPARC          Solaris 9        Sun ONE Studio 8, Sun C 5.5
 *      1 Intel Core2 Duo    Windows Vista    Borland C/C++ 5.5 w/ -DFENV_H="fenvx87.h" -DNEED_TRAPS_DISABLED -DFPU_DISABLE_TRAPS="(void)_control87( 0x003fu, 0x003fu );"
 *      1 Intel Core2 Duo    Windows 7        Microsoft C/C++ VS 2013; /DFIX_SEMIBROKEN_PP
 *      1 Intel Core2 Duo    Windows 7        Microsoft C/C++ VS 2015; /DFIX_SEMIBROKEN_PP
 *      1 Intel Core2 Duo    Windows Vista    Microsoft C/C++ VS 2010; /DFIX_SEMIBROKEN_PP /DFENV_H=\"fenvx87.h\"
 *      1 Intel Pentium 4    Windows XP       Microsoft C/C++ V 12.00 aka Visual Studio 6 [semi-broken preprocessor]
 *      1 Intel Pentium 4    Windows XP       Microsoft C/C++ V 14.00 aka Visual Studio 8 [semi-broken preprocessor]
 *      2 Intel Pentium III  Windows 98       LCC 2003/01/12
 *      3 Intel Pentium III  Windows 98       LCC 2002/06/09
 *      3 Intel Pentium III  Windows 98       LCC 2002/11/30
 *      6 Intel Pentium 4    Linux Fedora 14  Portable C Compiler pcc 0.9.9 2010/11/19
 *     54 Intel Pentium Pro  Windows NT       Metrowerks CodeWarrior Pro 4
 *    108 Intel Pentium Pro  Windows NT       Metrowerks CodeWarrior Pro 5
 *    108 Intel Pentium Pro  Windows NT       Metrowerks CodeWarrior Pro 6
 * ?????? DEC/Compaq/HP      OSF/1 4.0        Missing <fenv.h>
 * ?????? DEC/Compaq/HP      OSF/1 5.1 (aka Tru64 Unix)        Missing <fenv.h>
 * ?????? DEC/Compaq/HP Alpha OpenVMS 7.3     Missing <fenv.h>
 * ?????? IBM RS/6000        AIX 4.2          Missing <fenv.h>
 * ?????? Intel Pentium III  OS/2 v4+DOS      Borland Turbo C v2 [dies in preprocessor]
 * ?????? Intel Pentium III  OS/2 v4+DOS      Symantec C/C++ v7.01 [dies in preprocessor]
 * ?????? Intel Pentium III  Windows 98       Digital Mars 8.16 [dies in preprocessor]
 * ?????? Intel Pentium III  Windows 98       Digital Mars 8.19 [dies in preprocessor]
 * ?????? Intel Pentium III  Windows 98       Digital Mars 8.20 [dies in preprocessor]
 * ?????? Intel Pentium III  Windows 98       Microsoft C/C++ V 12.00 aka Visual Studio 6 [dies in preprocessor]
 * ?????? Intel Pentium III  Windows NT       Borland C/C++ v5 [dies in preprocessor]
 * ?????? Intel Pentium Pro  Windows NT       Microsoft C/C++ V 10.10.6038 [dies in preprocessor]
 * ?????? Intel Pentium Pro  Windows NT       Watcom C/C++ 10.5 [dies in preprocessor]
 * ?????? Intel x86          FreeBSD 4.4      Missing <fenv.h>
 * ?????? Intel x86          FreeBSD 5.0      Missing <fenv.h>
 * ?????? Intel x86          NetBSD 1.6       Missing <fenv.h>
 * ?????? Intel x86          OpenBSD 3.2      Missing <fenv.h>
 * ?????? SGI                IRIX 6.5         Missing <fenv.h>
 *
 * I wish to acknowledge the following people for taking the time and effort
 * to run this test and sending me their results:
 *      Nelson H. F. Beebe, Center for Scientific Computing, University of Utah
 *      Zhishun Alex Liu, Sun
 *
 *  MODIFICATION HISTORY:
 *
 * 2002/06/27 - Create as an easier to use version of t0120400.
 * 2002/07/04 - Use SCHAR_MAX in place of CHAR_MAX.
 * 2003/02/07 - Add ability to tailor via external file (TAILOR).
 * 2003/02/22 - Add FIX_IMPLICIT_FLUSH.
 * 2003/07/10 - Add FIX_INVALID_OCTAL.
 * 2003/07/16 - Add FIX_SEMIBROKEN_PP.
 * 2003/08/21 - Add FENV_ACCESS pragma.
 * 2003/08/21 - Add NEED_TRAPS_DISABLED and FPU_DISABLE_TRAPS.
 * 2004/04/23 - Add uses of CLEAR_ALL and RAISE_ALL.
 * 2004/07/30 - Update DEBUG section.
 * 2005/02/05 - Add FIX_SYM_FE_*_ENUM.
 * 2006/07/23 - Mods due to Lint 8.
 * 2006/07/23 - Update DEBUG section.
 * 2006/07/23 - Indent #if's.
 * 2007/11/21 - Add const as per PC-Lint 8.
 * 2008/07/14 - Shorten marco symbols to less than 32 chars.
 * 2008/11/29 - Suppress info messages if RETVAL defined (part of test suite).
 * 2009/07/01 - Update DEBUG section.
 * 2012/01/23 - Add C11 (which replaced C99 in December 2011).
 * 2012/06/06 - Include myfloat.h (so user can do overrides).
 * 2014/05/25 - Add FIX_NAMESPACE_STD.
 * 2017/03/21 - Prevent <<=1 going negative.
 * 2017/03/22 - Update DEBUG section.
 * 2017/03/22 - Update PRTDEBUG.
 *
 ***********************************************************************/

/*lint -e122 // Digit too large for radix */
/*lint -e506 // constant value Boolean */
/*lint -e537 // repeated include */
/*lint -e737 // loss of sign on promotion */
/*lint -e766 // header not needed */
/*lint -e774 // constant value in if */
/*lint -e792 // void cast of void expression */
/*lint -e801 // use of goto is deprecated */
/*lint -e909 // implicit conversion: if(i) */
/*lint -e912 // implicit conversion */

#ifdef   TAILOR			/* A way to tailor via external file */
  #include "tint2flt.h"		/* A place to define FIX_BROKEN_PP, ... */
#endif

#ifdef DEBUG	/* local override of FIXes to force errors to show */
  #ifdef DEBUG2	/* Kills some compilers */
    #undef  FENV_H
    #undef  FIX_BROKEN_PP
    #undef  FIX_FP_FROM_ULLI
    #undef  FIX_INVALID_OCTAL
    #undef  FIX_NAMESPACE_STD
    #undef  FIX_SEMIBROKEN_PP
    #undef  FIX_SYM_FE_DIVBYZERO_ENUM
    #undef  FIX_SYM_FE_INEXACT_ENUM
    #undef  FIX_SYM_FE_INVALID_ENUM
    #undef  FIX_SYM_FE_OVERFLOW_ENUM
    #undef  FIX_SYM_FE_UNDERFLOW_ENUM
  #endif
#endif

#ifdef __cplusplus		/* For C++ to call C functions		*/
  #ifndef FIX_NAMESPACE_STD
using namespace std;		/* Add implicit std:: to each function call */
  #endif
#endif

#ifdef     FENV_H
  #include FENV_H	/* fe clear/raise/test except(), FE_ALL_EXCEPT */
#else
  #include <fenv.h>	/* fe clear/raise/test except(), FE_ALL_EXCEPT */
#endif
#include <float.h>	/* *_MAX_EXP */
#include <limits.h>	/* *_MAX */
#include <stdio.h>	/* printf() */

/*
 * Clear all FP exception flags.
 * Must be before myfloat.h
 */
#ifndef   CLEAR_ALL
  #define CLEAR_ALL (void)feclearexcept( FE_ALL_EXCEPT )
#endif

#ifdef   TAILOR			/* A way to tailor via external file	*/
  #include "myfloat.h"		/* Will include your *.flt file		*/
#endif

#ifdef DEBUG	/* local override of FIXes to force errors to show */
  #ifdef DEBUG3
    #undef  LONGLONG
    #undef  NEED_TRAPS_DISABLED
    #undef  ULONGLONG
  #endif
  #undef  FIX_IMPLICIT_FLUSH
  #undef  FIX_PRAGMAS
  #undef  FIX_PRAGMA_FENV_ACCESS
#endif
#ifdef DEBUG4
  #undef  RETVAL
#endif

#ifdef ULLI_PREC
  #if 0 == ULLI_PREC
    #undef ULLONG_MAX
  #endif
#endif

#ifdef SLLI_PREC
  #if 0 == SLLI_PREC
    #undef  LLONG_MAX
  #endif
#endif

/*
 * Tell implementation that we will be checking the floating-point status flags.
 */
#if (!defined FIX_PRAGMAS) && (!defined FIX_PRAGMA_FENV_ACCESS)
  #pragma STDC FENV_ACCESS ON
#endif

#define LD long double, LDBL_MAX_EXP, "%#.0Lf"
#define  D      double,  DBL_MAX_EXP, "%#.0f"
#define  F       float,  FLT_MAX_EXP, "%#.0f"

#ifdef ULLONG_MAX
  #ifdef ULONGLONG
    #define ULL          ULONGLONG, "%#llx", ULLONG_MAX
  #else
    #define ULL unsigned long long, "%#llx", ULLONG_MAX
  #endif
#endif
#ifdef LLONG_MAX
  #ifdef LONGLONG
    #define SLL           LONGLONG, "%#llx",  LLONG_MAX
  #else
    #define SLL   signed long long, "%#llx",  LLONG_MAX
  #endif
#endif
#define  UL unsigned      long,  "%#lx",  ULONG_MAX
#define  SL   signed      long,  "%#lx",   LONG_MAX
#define  UI unsigned       int,   "%#x",   UINT_MAX
#define  SI   signed       int,   "%#x",    INT_MAX
#define  US unsigned     short,  "%#hx",  USHRT_MAX
#define  SS   signed     short,  "%#hx",   SHRT_MAX
#define  UC unsigned      char,   "%#x",  UCHAR_MAX
#define  SC   signed      char,   "%#x",  SCHAR_MAX

#define STR(s) # s
#define STN(n) tst_ ## n

/*
 * 'Type generic' function done via macro.
 * i = 2**j and is the value to convert.
 * First, test for expected value and no flags being raised.
 * Second, for signed types, test from negative.
 */
#define DEF_TSTS(N,PRT,I_TYPE,I_FMT,I_MAX,F_TYPE,F_MAX_EXP,F_FMT)\
static void STN(N)(void){\
  F_TYPE f_exp = (F_TYPE)1.0;\
  F_TYPE f;\
  I_TYPE i = (I_TYPE)1;\
  I_TYPE imax = (I_TYPE)I_MAX;\
  unsigned long j;\
  unsigned long const max_exp = log2radix * (unsigned long)F_MAX_EXP;\
  int flags;\
  unsigned int lcl_failed=0u;\
  if(PRT)(void)printf("Testing conversion from " STR(I_TYPE) " to " STR(F_TYPE) "\n");\
  for(j=0uL; j<max_exp; j++){\
    CLEAR_ALL;\
		/*lint -e524 -e732 -e915 -e919 */\
    f = i;	/* conversion we are testing */\
		/*lint +e524 +e732 +e915 +e919  */\
    flags = fetestexcept( FE_ALL_EXCEPT );\
    if( flags ){\
      lcl_failed++;\
        (void)printf("j=%lu, i=" I_FMT ", expected no flags, got: %#x\n", j, i, (unsigned)flags);\
    }\
    if( f != f_exp ){\
      lcl_failed++;\
      (void)printf("j=%lu, expected power of 2, got: " F_FMT "\n", j, f);\
    }\
    CLEAR_ALL;\
		/*lint -e501 -e524 -e732 -e915 -e919 */\
    f = -i;	/* conversion we are testing */\
		/*lint +e501 +e524 +e732 +e915 +e919  */\
    flags = fetestexcept( FE_ALL_EXCEPT );\
    if( flags ){\
      lcl_failed++;\
      (void)printf("j=%lu, i=" I_FMT ", expected no flags, got: %#x\n", j, (I_TYPE)-i, (unsigned)flags);\
    }\
    if( f != -f_exp ){\
      lcl_failed++;\
      (void)printf("j=%lu, expected negative of power of 2, got: " F_FMT "\n", j, f);\
    }\
    f_exp += f_exp;	/* next larger power of 2 */\
			/*lint -e915 -e704 -e703 -e702 -e701 */\
    imax >>= 1;		/* next smaller power of 2 */\
    if(0==imax) break;	/* prevent undefined behavior on next line */\
    i <<= 1;		/* next larger power of 2 */\
			/*lint +e915 +e704 +e703 +e702 +e701 */\
    if(9<=lcl_failed){\
      if(!PRT)(void)printf("Testing conversion from " STR(I_TYPE) " to " STR(F_TYPE) "\n");\
      (void)printf("Too many failures\n");\
      break;\
    }\
  }\
  failed += lcl_failed;\
}

/*
 * Same tests as above, but for unsigned types (no test of -i)
 */
#define DEF_TSTU(N,PRT,I_TYPE,I_FMT,I_MAX,F_TYPE,F_MAX_EXP,F_FMT)\
static void STN(N)(void){\
  F_TYPE f_exp = (F_TYPE)1.0;\
  F_TYPE f;\
  I_TYPE i = (I_TYPE)1;\
  I_TYPE imax = (I_TYPE)I_MAX;\
  unsigned long j;\
  unsigned long const max_exp = log2radix * (unsigned long)F_MAX_EXP;\
  int flags;\
  unsigned int lcl_failed=0u;\
  if(PRT)(void)printf("Testing conversion from " STR(I_TYPE) " to " STR(F_TYPE) "\n");\
  for(j=0uL; imax && j<max_exp; j++){\
    CLEAR_ALL;\
		/*lint -e524 -e732 -e915 -e919 */\
    f = i;	/* conversion we are testing */\
		/*lint +e524 +e732 +e915 +e919  */\
    flags = fetestexcept( FE_ALL_EXCEPT );\
    if( flags ){\
      lcl_failed++;\
        (void)printf("j=%lu, i=" I_FMT ", expected no flags, got: %#x\n", j, i, (unsigned)flags);\
    }\
    if( f != f_exp ){\
      lcl_failed++;\
      (void)printf("j=%lu, expected power of 2, got: " F_FMT "\n", j, f);\
    }\
    f_exp += f_exp;	/* next larger power of 2 */\
			/*lint -e915 -e704 -e702 */\
    imax >>= 1;		/* next smaller power of 2 */\
    i <<= 1;		/* next larger power of 2 */\
			/*lint +e915 +e704 +e702 */\
    if(9<=lcl_failed){\
      if(!PRT)(void)printf("Testing conversion from " STR(I_TYPE) " to " STR(F_TYPE) "\n");\
      (void)printf("Too many failures\n");\
      break;\
    }\
  }\
  failed += lcl_failed;\
}

static unsigned long log2radix;
static unsigned int failed = 0u;

#if 1	/* Dummy to match tflt2int.fjt */

  #if (!defined FIX_BROKEN_PP)	/* many pre-processors die on these macros */
/*
 * Used to convert 4 arguments (which contain embedded ',' chars)
 * into 8 arguments.
 */
    #ifdef RETVAL			/* Running as part of test suite */
      #define P 0
    #else				/* Running standalone */
      #define P 1
    #endif
    #if (!defined FIX_SEMIBROKEN_PP)	/* many pre-processors die on these macros */
      #define DEF_TESTU(N,I,F) DEF_TSTU(N,P,I,F)
      #define DEF_TESTS(N,I,F) DEF_TSTS(N,P,I,F)
    #else					/* a workaround for some compilers */
      #define DEF_TESTU(N,I,F) DEF_TEST_UI((N,P,I,F))
      #define DEF_TEST_UI(args) DEF_TSTU args
      #define DEF_TESTS(N,I,F) DEF_TEST_SI((N,P,I,F))
      #define DEF_TEST_SI(args) DEF_TSTS args
    #endif

    #if (defined ULLONG_MAX) && (!defined FIX_FP_FROM_ULLI)
DEF_TESTU(00,ULL,LD)
    #endif
    #ifdef LLONG_MAX
DEF_TESTS(01,SLL,LD)
    #endif
DEF_TESTU(02, UL,LD)
DEF_TESTS(03, SL,LD)
DEF_TESTU(04, UI,LD)
DEF_TESTS(05, SI,LD)
DEF_TESTU(06, US,LD)
DEF_TESTS(07, SS,LD)
    #ifdef FIX_INVALID_OCTAL
DEF_TESTU(108, UC,LD)
DEF_TESTS(109, SC,LD)
    #else
DEF_TESTU(08, UC,LD)
DEF_TESTS(09, SC,LD)
    #endif

    #if (defined ULLONG_MAX) && (!defined FIX_FP_FROM_ULLI)
DEF_TESTU(10,ULL, D)
    #endif
    #ifdef LLONG_MAX
DEF_TESTS(11,SLL, D)
    #endif
DEF_TESTU(12, UL, D)
DEF_TESTS(13, SL, D)
DEF_TESTU(14, UI, D)
DEF_TESTS(15, SI, D)
DEF_TESTU(16, US, D)
DEF_TESTS(17, SS, D)
DEF_TESTU(18, UC, D)
DEF_TESTS(19, SC, D)

    #if (defined ULLONG_MAX) && (!defined FIX_FP_FROM_ULLI)
DEF_TESTU(20,ULL, F)
    #endif
    #ifdef LLONG_MAX
DEF_TESTS(21,SLL, F)
    #endif
DEF_TESTU(22, UL, F)
DEF_TESTS(23, SL, F)
DEF_TESTU(24, UI, F)
DEF_TESTS(25, SI, F)
DEF_TESTU(26, US, F)
DEF_TESTS(27, SS, F)
DEF_TESTU(28, UC, F)
DEF_TESTS(29, SC, F)

  #endif /* FIX_BROKEN_PP */

#endif	/* Dummy to match tflt2inf.fjt */

/*================================================================*/

int main(void){

#ifndef TAILOR
  (void)printf("Info:  'TAILOR' not defined => NOT using any bug workarounds.\n");
#endif

#ifndef RETVAL
  (void)printf("This program tests conversions from integer types to floating-point types.\n");
  (void)printf("For all types, it test positive powers of 2, e.g., 2**j\n");
  (void)printf("For signed types, it also tests negative of positive powers of 2, e.g., -(2**j)\n");
  (void)printf("For numbers that are representable, it checks expected value and no exceptions.\n");
#endif

#ifndef __STDC_IEC_559__
  (void)printf("This implementation does not claim conformance to IEC-60559 or IEEE-754\n");
  (void)printf("Checks of the floating-point exception flags may be meaningless.\n");
#endif

  switch (FLT_RADIX){
  case 2:
    log2radix = 1uL;
    break;
  case 4:
    log2radix = 2uL;
    break;
  case 8:
    log2radix = 3uL;
    break;
  case 16:
    log2radix = 4uL;
    break;
  default:
    failed++;
    (void)printf("Failed.  Unsupported FLT_RADIX value: %i\n", (int)FLT_RADIX);
    goto death;
  }

#if (defined NEED_TRAPS_DISABLED)
  #ifndef RETVAL
  failed++;
  (void)printf("Failed due to traps enabled.\n");
  #endif
  { fenv_t current_fenv;
    FPU_DISABLE_TRAPS
  }
#endif

#if (defined FIX_BROKEN_PP)
  #ifndef RETVAL
  failed++;
  (void)printf("Failed due to broken PreProcessor.\n");
  #endif

#else

  #if (defined FIX_SEMIBROKEN_PP)
    #ifndef RETVAL
  failed++;
  (void)printf("Failed due to semibroken PreProcessor.\n");
    #endif
  #endif

  /*
   * Tests to long double
   */
  #if (defined ULLONG_MAX) && (!defined FIX_FP_FROM_ULLI)
  tst_00();
  #endif
  #ifdef LLONG_MAX
  tst_01();
  #endif
  tst_02();
  tst_03();
  tst_04();
  tst_05();
  tst_06();
  tst_07();
  #ifdef FIX_INVALID_OCTAL
  tst_108();
  tst_109();
  #else
  tst_08();
  tst_09();
  #endif

  /*
   * Tests to double
   */
  #if (defined ULLONG_MAX) && (!defined FIX_FP_FROM_ULLI)
  tst_10();
  #endif
  #ifdef LLONG_MAX
  tst_11();
  #endif
  tst_12();
  tst_13();
  tst_14();
  tst_15();
  tst_16();
  tst_17();
  tst_18();
  tst_19();

  /*
   * Tests to float
   */
  #if (defined ULLONG_MAX) && (!defined FIX_FP_FROM_ULLI)
  tst_20();
  #endif
  #ifdef LLONG_MAX
  tst_21();
  #endif
  tst_22();
  tst_23();
  tst_24();
  tst_25();
  tst_26();
  tst_27();
  tst_28();
  tst_29();

  if(failed){
    (void)printf("To help you figure out which flags were raised:\n");
  #if (defined FE_INVALID) || (defined FIX_SYM_FE_INVALID_ENUM)
    (void)printf(" FE_INVALID   = %#x", (unsigned)FE_INVALID );
    (void)printf("\n");
  #endif
  #if (defined FE_DIVBYZERO) || (defined FIX_SYM_FE_DIVBYZERO_ENUM)
    (void)printf(" FE_DIVBYZERO = %#x", (unsigned)FE_DIVBYZERO);
    (void)printf("\n");
  #endif
  #if (defined FE_OVERFLOW) || (defined FIX_SYM_FE_OVERFLOW_ENUM)
    (void)printf(" FE_OVERFLOW  = %#x", (unsigned)FE_OVERFLOW );
    (void)printf("\n");
  #endif
  #if (defined FE_UNDERFLOW) || (defined FIX_SYM_FE_UNDERFLOW_ENUM)
    (void)printf(" FE_UNDERFLOW = %#x", (unsigned)FE_UNDERFLOW);
    (void)printf("\n");
  #endif
  #if (defined FE_INEXACT) || (defined FIX_SYM_FE_INEXACT_ENUM)
    (void)printf(" FE_INEXACT   = %#x", (unsigned)FE_INEXACT  );
    (void)printf("\n");
  #endif
  }

#endif /* FIX_BROKEN_PP */

death:;

#ifndef PRTDEBUG
  #if (defined DEBUG)
    #define PRTDEBUG(x) (1)
  #else
    #define PRTDEBUG(x) (x)
  #endif
#endif

  if( PRTDEBUG(failed) ){
    (void)printf("\n... Symbols defined ...\n");
#ifdef FIX_BROKEN_PP
    (void)printf("FIX_BROKEN_PP\n");
#endif
#ifdef FIX_FP_FROM_ULLI
    (void)printf("FIX_FP_FROM_ULLI\n");
#endif
#ifdef FIX_INVALID_OCTAL
    (void)printf("FIX_INVALID_OCTAL\n");
#endif
#ifdef FIX_NAMESPACE_STD
    (void)printf("FIX_NAMESPACE_STD\n");
#endif
#ifdef FIX_SEMIBROKEN_PP
    (void)printf("FIX_SEMIBROKEN_PP\n");
#endif
#ifdef FIX_SYM_FE_DIVBYZERO_ENUM
    (void)printf("FIX_SYM_FE_DIVBYZERO_ENUM\n");
#endif
#ifdef FIX_SYM_FE_INEXACT_ENUM
    (void)printf("FIX_SYM_FE_INEXACT_ENUM\n");
#endif
#ifdef FIX_SYM_FE_INVALID_ENUM
    (void)printf("FIX_SYM_FE_INVALID_ENUM\n");
#endif
#ifdef FIX_SYM_FE_OVERFLOW_ENUM
    (void)printf("FIX_SYM_FE_OVERFLOW_ENUM\n");
#endif
#ifdef FIX_SYM_FE_UNDERFLOW_ENUM
    (void)printf("FIX_SYM_FE_UNDERFLOW_ENUM\n");
#endif
#ifdef NEED_TRAPS_DISABLED
    (void)printf("NEED_TRAPS_DISABLED\n");
#endif
#ifdef FIX_IMPLICIT_FLUSH
    (void)printf("FIX_IMPLICIT_FLUSH\n");
#endif
#ifdef FIX_PRAGMAS
    (void)printf("FIX_PRAGMAS\n");
#endif
#ifdef FIX_PRAGMA_FENV_ACCESS
    (void)printf("FIX_PRAGMA_FENV_ACCESS\n");
#endif
  }

  if(failed){
    (void)printf("Test Failed with at least %u failures\n", failed);
  }else{
    (void)printf("Test ( Pass ) {} \n");
  }

#ifdef FIX_IMPLICIT_FLUSH
  (void)fflush(stdout);		/* need explicit flush */
#endif
#if (defined RETVAL) && (defined STANDALONE_TEST)
  return RETVAL;		/* Being run as part of test suite */
#else
  return 0;
#endif
}
