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
 *  FILE: tflt2int.c
 *
 *  Assertion being tested:
 *
 * C11:
 * F.4 Floating to integer conversion
 *    If the integer type is _Bool, 6.3.1.2 applies ....
 *    Otherwise, if the floating value is infinite or NaN or if the
 *    integral part of the floating value exceeds the range of the
 *    integer type, then the ''invalid'' floating-point exception is
 *    raised and the resulting value is unspecified. ....  Conversion
 *    of an integral floating value that does not exceed the range of
 *    the integer type raises no floating-point exceptions;
 *
 * C99:
 * F.4 Floating to integer conversion
 *    If the floating value is infinite or NaN or if the integral part
 *    of the floating value exceeds the range of the integer type,
 *    then the ''invalid'' floating-point exception is raised and the
 *    resulting value is unspecified.
 *
 *  This test is for C11/C99 with IEEE-754 floating-point support.
 *  Test conversions from floating-point types to integer types.
 *  Test with powers of 2.
 *  For numbers in range, checks for correct value and no exceptions.
 *  For numbers too big, checks for FE_INVALID exception.
 *  Also, check that no exceptions are cleared with a 2nd conversion.
 *
 * If you need to disable trapping of floating-point exceptions,
 * you might try one of the next two ways:
 *  #define FPU_DISABLE_TRAPS (void)feholdexcept( &current_fenv );
 *  #define FPU_DISABLE_TRAPS (void)fedisabletrap(FE_TRAP_ALL);
 * in tdfp2int.h.  You will also need to define NEED_TRAPS_DISABLED.
 *
 * Results of running this test on various compilers:
 * (as reported by several users of this test; your results may vary)
 *
 * Failures    Hardware      Operating system   Compiler, version, and options
 *      0 HP Integrity       HP-UX 11i V3      HP c99, A.06.23, +decfp
 *      0 HP ia64 Itanium II HP UX 11          HP cc; no long long
 *      0 Intel Pentium 4    Windows XP        LCC 2003/01/12; no unsigned long long
 *      0 Intel Pentium 4    Windows XP        LCC 2004/07/30
 *      0 Intel Pentium 4    Windows XP        LCC 2008/07/24 -ansic
 * >=  18 Intel Pentium 4    Windows XP        LCC 2002/06/17
 * >=  18 Intel Pentium 4    Windows XP        LCC 2003/07/11
 * >=  19 Intel Core2 Duo    Linux Fedora 17   Intel C/C++ icc 13.1 -std=c99   -mia32 -O0 -fp-model strict -fp-model extended
 * >=  19 Intel Core2 Duo    Linux Fedora 17   Intel C/C++ icc 13.1 -std=gnu99 -mia32 -O0 -fp-model strict -fp-model extended
 * >=  29 Intel Core2 Duo    Linux Fedora 17   Intel C/C++ icc 13.1 -std=c99   -xSSSE3 -O0 -fp-model strict -fp-model source
 * >=  29 Intel Core2 Duo    Linux Fedora 17   Intel C/C++ icc 13.1 -std=gnu99 -xSSSE3 -O0 -fp-model strict -fp-model source
 * >=  54 Intel Pentium 4    Windows XP        LCC 2002/06/09
 * >=  65 Intel Core2 Duo    Linux Fedora 17   Intel C/C++ icc 13.1 -std=c99 -O0 -fp-model strict -fp-model double
 * >=  72 Intel Core2 Duo    Windows Vista     LCC 2012/10/22
 * >=  75 Intel Pentium 4    Windows XP        LCC 2002/06/06
 * >= 108 IBM Power 6        AIX 6.1.7.17      IBM XL C/C++ for AIX V11.1
 * >= 127 Intel Pent 4 IA-32 Linux Fedora 10   Intel C/C++ icc 11.1 -std=c99 -strict-ansi -mia32 -fp-model strict -O0
 * >= 127 Intel Pent 4 IA-32 Linux Fedora 10   Intel C/C++ icc 11.1 -std=c99 -strict-ansi -msse -fp-model strict -O0
 * >= 131 Intel Pentium 4    Windows XP        Digital Mars 8.38
 * >= 131 Intel Pentium 4    Windows XP        Digital Mars 8.39
 * >= 131 Intel Pentium 4    Windows XP        Digital Mars 8.47
 * >= 131 Intel Pentium 4    Windows XP        Digital Mars 8.48
 * >= 131 Intel Pentium 4    Windows XP        Digital Mars 8.50
 * >= 131 Intel Pentium 4    Windows XP        Digital Mars 8.51
 * >= 131 Intel Pentium 4    Windows XP        Digital Mars 8.52c
 * >= 131 Intel Pentium III  Windows 98        Digital Mars 8.35.6
 * >= 138 Intel Pentium III  RedHat Linux 8    Gnu gcc 3.3.1 + glibc 2.3.2-4.80.6 (no long long)
 * >= 165 Apple PowerPC G3   Linux 2.4.19      Gnu gcc 3.2.2 w/ glibc ?; no long long
 * >= 165 DEC Alpha          Linux 2.4.18      Gnu gcc 3.3.1; no long long
 * >= 165 HP PA RISC rp5470  Linux 3.0         cc 3.0.4; no long long
 * >= 165 Intel Core i7-64   Ubuntu 3.2.0-40   gcc 4.6.3 + eglibc 2.15-0
 * >= 165 Intel Core2 Duo    Linux Fedora 17   gcc 4.7.0-2 + glibc 2.15-32; -std=gnu11 -mieee-fp
 * >= 165 Intel Pentium Pro  OS/2 v4           Gnu gcc 2.8.1 + emx 0.9d; no long long
 * >= 165 Intel Pentium Pro  Windows NT        Watcom C/C++ 10.5; no long long
 * >= 165 Intel Xeon E7-4850-64 Linux 2.6.32   gcc 4.7.1 + glibc 2.12
 * >= 165 Intel Xeon E7-4850-64 Linux 2.6.32   icc 13.0.1
 * >= 165 Intel Xeon X7550-64 Linux RedHat 2.6.18 gcc 4.4.6-3 + glibc 2.5
 * >= 165 Intel Xeon X7550-64 Linux RedHat 2.6.18 icc 12.1.0
 * >= 165 Intel Xeon X7560-64 Linux Bull 2.6.32 gcc 4.4.5-6 + glibc 2.12
 * >= 165 Intel Xeon X7560-64 Linux Bull 2.6.32 icc 12.1.0
 * >= 166 Intel Core2 Duo    Linux Fedora 14   Gnu gcc 4.5.1-4 + glibc 2.12.90-15; -std=gnu99 -frounding-math
 * >= 166 Intel Core2 Duo    Linux Fedora 15   Gnu gcc 4.6.1-9 + glibc 2.14-5; -std=gnu99 -frounding-math
 * >= 166 Intel Core2 Duo    Linux Fedora 16   Gnu gcc 4.6.2-1 + glibc 2.14.90-21; -std=gnu99 -frounding-math
 * >= 166 Intel Core2 Duo-64 Linux Fedora 10   Intel C/C++ icc 11.1 -std=c99 -fp-model strict (x86_64)
 * >= 166 Intel Pentium 4    Linux Fedora  6   Gnu gcc 4.1.2-13.fc6 + glibc 2.5-18.fc6
 * >= 166 Intel Pentium 4    Linux Fedora 10   Gnu gcc 4.3.2-7 + glibc 2.9-3
 * >= 166 Intel Pentium 4    Linux Fedora 11   Gnu gcc 4.4.0-4 + glibc 2.10.1-2
 * >= 166 Intel Pentium 4    Linux Fedora 11   Gnu gcc 4.4.1-2 + glibc 2.10.1-5
 * >= 166 Intel Pentium 4    Linux Fedora 14   Gnu gcc 4.5.1-4 + glibc 2.12.90-15; -std=gnu99 -frounding-math
 * >= 168 Intel Pentium 4    RedHat Linux 8    EDG 3.4 w/ Gnu gcc 3.2 + glibc 2.2.93
 * >= 168 Intel Pentium 4    RedHat Linux 8    gcc 3.2 + glibc 2.2.93-5
 * >= 168 Intel Pentium 4    SuSE Linux 9.1    gcc 3.3.3-41 + glibc 2.3.3-97
 * >= 168 Intel Pentium III  RedHat Linux 7    Gnu gcc 2.96 + glibc 2.1.92
 * >= 168 Intel Pentium III  RedHat Linux 8    Comeau 4.3.0.1 w/ Gnu gcc 3.2 + glibc 2.2.93
 * >= 168 Intel Pentium III  RedHat Linux 8    EDG 3.2 w/ Gnu gcc 3.2 + glibc 2.2.93
 * >= 168 Intel Pentium III  RedHat Linux 8    EDG 3.3 w/ Gnu gcc 3.2 + glibc 2.2.93
 * >= 168 Intel Pentium III  RedHat Linux 8    Gnu gcc 3.2 + glibc 2.2.93
 * >= 173 Intel Pent 4 IA-32 Linux Fedora 10   Intel C/C++ icc 11.1 -std=c99 -strict-ansi -fp-model strict -O0
 * >= 173 Intel Pent 4 IA-32 Linux Fedora 10   Intel C/C++ icc 11.1 -std=c99 -strict-ansi -msse2 -fp-model strict -O0
 * >= 183 Intel Core i5-64   Linux Fedora 14   pcc 0.9.9 (2010/11/19) + glibc-2.13-1 (x86_64)
 * >= 183 Intel Core i5-64   Linux Fedora 15   gcc 4.6.0-9 (2011/05/30) + glibc-2.14-3 (x86_64)
 * >= 183 Intel Core i5-64   Linux Fedora 16   gcc 4.6.3-2 + glibc-2.14.90-24 (x86_64)
 * >= 183 Intel Core i5-64   Linux Fedora 18   gcc 4.7.2-8 + glibc-2.16-24 (x86_64) -std=gnu11 -mfpmath=387
 * >= 183 Intel Core i5-64   Linux Fedora 18   gcc 4.7.2-8 + glibc-2.16-24 (x86_64) -std=gnu11 -mfpmath=sse
 * >= 183 Intel Core i7-64   Linux debian6     gcc 4.4.5-8 + glibc (x86_64)
 * >= 183 Intel Core2 Duo    Linux Fedora 17   gcc 4.7.0-2 + glibc 2.15-32; -std=gnu11 -mfpmath=sse -mieee-fp
 * >= 183 Intel Core2 Duo-64 Linux Fedora 10   gcc 4.3.2-7 + glibc-2.9-3 (x86_64)
 * >= 184 Intel Core2 Duo    Linux Fedora 16   pcc 1.0.0-2 (2011/12/16) + glibc-2.14.90-24
 * >= 192 Intel Core i5-64   Linux Fedora 14   pcc 1.0.0 (2011/04/01) + glibc-2.13-1 (x86_64)
 * >= 192 Intel Core i5-64   Solaris 11.1      Solaris Studio 12.3
 * >= 192 Intel Core2 Duo    OS/2 eCS 2.0      Open Watcom C/C++ 1.8
 * >= 192 Intel Core2 Duo    OS/2 eCS 2.0      Open Watcom C/C++ 1.9
 * >= 192 Intel Core2 Duo-64 OpenSolaris 2009.6 Sun Studio 12.1: c99
 * >= 192 Intel Pentium      UnixWare 7.1.3    OUDK C Compilation System 8.0.1c
 * >= 192 Intel Pentium 4    Linux Fedora 14   Portable C Compiler pcc 0.9.9 2010/11/19
 * >= 192 Intel Pentium 4    OS/2 eCS 1.1      Open Watcom C/C++ 1.3
 * >= 192 Intel Pentium 4    OS/2 eCS 1.1      Open Watcom C/C++ 1.4
 * >= 192 Intel Pentium 4    OS/2 eCS 1.1      Open Watcom C/C++ 1.5
 * >= 192 Intel Pentium 4    Solaris 9         Sun ONE Studio 8: cc
 * >= 192 Intel Pentium 4    Solaris 9         Sun ONE Studio 8: cc -O
 * >= 192 Intel Pentium 4    Windows XP        Metrowerks CodeWarrior Pro 8.3
 * >= 192 Intel Pentium 4    Windows XP        Metrowerks CodeWarrior Pro 9.2
 * >= 192 Intel Pentium III  OS/2 v4           Gnu gcc 2.7.2 + emx 0.9b + fix05
 * >= 192 Intel Pentium III  OS/2 v4           Gnu gcc 2.7.2.1 + emx 0.9c + fix02
 * >= 192 Intel Pentium III  OS/2 v4           Gnu gcc 2.8.1 + emx 0.9d
 * >= 192 Intel Pentium III  OS/2 v4           Open Watcom C/C++ 1.0
 * >= 192 Intel Pentium III  RedHat Linux 7    Intel C/C++ icc 7 w/ Gnu gcc 2.96 + glibc 2.1.92
 * >= 192 Intel Pentium III  RedHat Linux 8    Intel C/C++ icc 7 w/ Gnu gcc 3.2 + glibc 2.2.93
 * >= 192 Intel Pentium III  RedHat Linux 8    Intel C/C++ icc 7.1 w/ Gnu gcc 3.3.1 + glibc 2.3.2-4.80.6
 * >= 192 Intel Pentium Pro  Windows NT        Metrowerks CodeWarrior Pro 6
 * >= 192 Intel x86          Solaris 9         Sun ONE Studio 8, Sun C 5.5
 * >= 192 Sun Blade 150      Solaris 9         Sun ONE Studio 8: cc
 * >= 192 Sun Blade 150      Solaris 9         Sun ONE Studio 8: cc -O
 * >= 192 Sun Blade 150      Solaris 9         Sun ONE Studio 8: cc -xarch=v9
 * >= 192 Sun Blade 150      Solaris 9         Sun ONE Studio 8: cc -xarch=v9 -O
 * >= 192 Sun SPARC          Solaris 9         Sun ONE Studio 8, Sun C 5.5
 * >= 194 Intel Core i5-64   Linux Fedora 15   clang 2.8; -std=c99 -lm
 * >= 194 Intel Core i5-64   Linux Fedora 16   clang 2.9-10; -std=c99 -lm
 * >= 194 Intel Core i5-64   Linux Fedora 17   clang 3.0-12; -std=c1x -fmath-errno -O0 -lm
 * >= 197 Intel Core2 Duo    Linux Fedora 16   clang 2.9-6; -std=c99 -lm
 * >= 197 Intel Core2 Duo    Linux Fedora 17   clang 3.0-10; -std=c1x -pedantic -lm
 * >= 197 Intel Pentium 4    Linux Fedora 15   clang 2.8; -std=c99 -lm
 * >= 198 Intel Core i5-64   Linux Fedora 14   pcc 0.9.9 (2011/02/03) + glibc-2.13-1 (x86_64)
 * >= 201 Intel Core i5-64   Linux Fedora 16   pcc 1.0.0-2 (2011/12/16) + glibc-2.14.90-24 (x86_64)
 * >= 213 Intel Core 2 Duo   Windows 7         Microsoft C/C++ VS 2013; /DFIX_SEMIBROKEN_PP
 * >= 213 Intel Core 2 Duo   Windows 7         Microsoft C/C++ VS 2015; /DFIX_SEMIBROKEN_PP
 * >= 213 Intel Core 2 Duo   Windows Vista     Microsoft C/C++ VS 2010; /DFIX_SEMIBROKEN_PP /DFENV_H=\"fenvx87.h\"
 * >= 213 Intel Pentium 4    Windows XP        Microsoft C/C++ V 14.00 aka Visual Studio 8; semi-broken preprocessor
 * >= 214 IBM Power 6        AIX 6.1.7.17      gcc 4.5.1; -maix64 -lm
 * >= 219 DEC Alpha 21064    Linux 2.2.22      Gnu gcc 3.3
 * >= 219 Intel Pentium 4    Windows XP        LCC 2002/05/01 no long long
 * >= 219 Intel Pentium III  OS/2 v4           Gnu gcc 2.5.7 + emx 0.8h; no long long
 * >= 219 Intel Pentium Pro  Windows NT        Intel C/C++ 2.4; no long long
 * >= 219 Intel Pentium Pro  Windows NT        Metrowerks CodeWarrior Pro 4
 * >= 219 Intel Pentium Pro  Windows NT        Metrowerks CodeWarrior Pro 5
 * >= 220 Intel Pentium 4    Windows XP        Microsoft C/C++ V 12.00 aka Visual 6; semi-broken preprocessor
 * >= 220 Intel Pentium III  Windows 98        Microsoft C/C++ V 12.00 aka Visual 6; semi-broken preprocessor
 * >= 225 Apple Mac PPC G4   Mac OS X 10.3.7   Gnu gcc 3.3 (build 1495)
 * >= 225 Apple PowerPC G4 7450 MacOS 10.2.5   Gnu gcc 3.3.2 w/ glibc ?
 * >= 273 Intel Core2 Duo    Windows Vista     Borland C/C++ 5.5 w/ -DFENV_H="fenvx87.h" -DNEED_TRAPS_DISABLED -DFPU_DISABLE_TRAPS="(void)_control87( 0x003fu, 0x003fu );"
 * >= 274 Intel Pentium Pro  Windows NT        Borland C/C++ v5
 * >= 284 Apple Mac PPC G4   Mac OS X 10.3.7   IBM XL C 6.0 + Gnu gcc 3.3
 * >= 285 Intel Pentium III  Windows 98        LCC 2002/02/10 no long long
 * >= 285 Intel Pentium Pro  RedHat Linux 4    Gnu gcc 2.7.2.1 + libc 5.3.12 + libg++ 2.7.1.4
 * ?????? DEC/Compaq/HP      OSF/1 4.0         Missing <fenv.h>
 * ?????? DEC/Compaq/HP      OSF/1 5.1 (aka Tru64 Unix) Missing <fenv.h>
 * ?????? DEC/Compaq/HP Alpha OpenVMS 7.3      Missing <fenv.h>
 * ?????? IBM RS/6000        AIX 4.2           Missing <fenv.h>
 * ?????? Intel Pentium 4    Windows XP        LCC 2004/07/26 [dies at linktime]
 * ?????? Intel Pentium III  OS/2 v4+DOS       Borland Turbo C v2 [dies in preprocessor]
 * ?????? Intel Pentium III  OS/2 v4+DOS       Symantec C/C++ v7.01 [dies in preprocessor]
 * ?????? Intel Pentium III  Windows 98        Digital Mars 8.16 [dies in preprocessor]
 * ?????? Intel Pentium III  Windows 98        Digital Mars 8.19 [dies in preprocessor]
 * ?????? Intel Pentium III  Windows 98        Digital Mars 8.20 [dies in preprocessor]
 * ?????? Intel Pentium Pro  OS/2 v4+DOS       Symantec C/C++ v7.01 [dies in preprocessor]
 * ?????? Intel Pentium Pro  Windows NT        Microsoft C/C++ 10.10.6038 [dies in preprocessor]
 * ?????? Intel x86          FreeBSD 4.4       Missing <fenv.h>
 * ?????? Intel x86          FreeBSD 5.0       Missing <fenv.h>
 * ?????? Intel x86          NetBSD 1.6        Missing <fenv.h>
 * ?????? Intel x86          OpenBSD 3.2       Missing <fenv.h>
 * ?????? SGI                IRIX 6.5          Missing <fenv.h>
 * abort  Intel Pent 4 IA-32 Linux Fedora 10   Intel C/C++ icc 11.1 -std=c99 -strict-ansi -msse3 -fp-model strict -O0
 * abort  Intel Pentium 4    Windows XP        LCC 2006/04/10; [dies at runtime]
 * abort  Intel Pentium III  OS/2 v4+DOS       Borland C/C++ v4 [dies at runtime]
 * abort  Intel Pentium III  Windows 98        LCC 2002/06/06 no long long; [dies at runtime]
 * abort  Intel Pentium III  Windows 98        LCC 2002/06/09 [dies at runtime]
 * abort  Intel Pentium III  Windows 98        LCC 2002/06/17 [dies at runtime]
 * abort  Intel Pentium III  Windows 98        LCC 2002/10/03 [dies at runtime]
 * abort  Intel Pentium III  Windows 98        LCC 2002/11/30 [dies at runtime]
 * abort  Intel Pentium III  Windows 98        LCC 2003/01/12 [dies at runtime]
 * abort  Intel Pentium Pro  Windows NT        IBM VisualAge for C/C++ 3.5.7 [dies at run time]
 *
 * The following were tests of just i = +f;  that is, no tests of i = -f;  Need to be rerun.
 * >= 144 UltraSparc III (v8+)  ?              Forte 7: cc -lm9x -xc99 -xtarget=native -xarch=native
 * >= 144 UltraSparc III (v8+b) ?              Forte 7: cc -lm9x -xc99 -xtarget=native -xarch=native
 * >= 144 UltraSparc III (v9)   ?              Forte 7: cc -lm9x -xc99 -xtarget=native -xarch=native
 * >= 162 Intel/ARM XScale   RISC OS 5         Norcroft C 5.53
 * >= 273 UltraSparc IIi     SunPro 5          cc -lm9x -xtarget=native
 *
 * I wish to acknowledge the following people for taking the time and effort
 * to run this test and sending me their results:
 *      Nelson H. F. Beebe, Center for Scientific Computing, University of Utah
 *      Zhishun Alex Liu, Sun
 *      Dave Prosser, The SCO Group
 *
 * MODIFICATION HISTORY:
 *
 * 2002/06/04 - Create as an easier to use version of t0120200.
 * 2002/06/05 - Add tests to make sure flags are not cleared by conversion.
 * 2002/07/04 - Use SCHAR_MAX in place of CHAR_MAX.
 * 2003/02/07 - Add ability to tailor via external file (TAILOR).
 * 2003/02/22 - Add FIX_IMPLICIT_FLUSH.
 * 2003/07/10 - Add FIX_INVALID_OCTAL.
 * 2003/07/16 - Add FIX_SEMIBROKEN_PP.
 * 2003/08/21 - Add FENV_ACCESS pragma.
 * 2003/08/21 - Add NEED_TRAPS_DISABLED and FPU_DISABLE_TRAPS.
 * 2003/08/21 - Add i = -f tests.
 * 2003/11/17 - Allow for FE_* to be union of two or more different FPUs.
 * 2004/04/23 - Add uses of CLEAR_ALL and RAISE_ALL.
 * 2004/07/30 - Update DEBUG section.
 * 2005/02/05 - Add FIX_SYM_FE_*_ENUM.
 * 2006/07/23 - Mods due to Lint 8.
 * 2006/07/23 - Update DEBUG section.
 * 2006/07/23 - Indent #if's.
 * 2006/09/18 - Make sure RAISE_ALL raises some flags.
 * 2007/11/21 - Add const as per PC-Lint 8.
 * 2008/06/25 - Add more lint disables.
 * 2008/07/14 - Shorten marco symbols to less than 32 chars.
 * 2008/11/29 - Suppress info messages if RETVAL defined (part of test suite).
 * 2009/01/31 - Add uses of FIX_*_RANDOM_INVALID.
 * 2009/07/01 - Update DEBUG section.
 * 2009/09/15 - Reorganize code to "match" tdfp2int.
 * 2012/01/23 - Add C11 (which replaced C99 in December 2011).
 * 2012/03/29 - Add QUAD_LDBL.
 * 2012/06/06 - Include myfloat.h (so user can do overrides).
 * 2014/05/25 - Add FIX_NAMESPACE_STD.
 * 2017/03/21 - Redo when <<= 1 to avoid undefined behavior.
 * 2017/03/22 - Update PRTDEBUG.
 *
 ***********************************************************************/

/*lint -e122 // Digit too large for radix */
/*lint -e537 // repeated include */
/*lint -e701 // Shift left of signed quantity */
/*lint -e703 // Shift left of signed quantity */
/*lint -e737 // loss of sign on promotion */
/*lint -e766 // header not needed */
/*lint -e792 // void cast of void expression */
/*lint -e801 // use of goto is deprecated */
/*lint -e909 // implicit conversion: if(i) */
/*lint -e912 // implicit conversion */
/*lint -e915 // implicit conversion */

#ifdef   TAILOR			/* A way to tailor via external file */
  #include "tflt2int.h"		/* A place to define FIX_BROKEN_PP, ... */
#endif

#ifdef DEBUG	/* local override of FIXes to force errors to show */
  #ifdef DEBUG2	/* Kills some compilers */
    #undef  FENV_H
    #undef  FIX_BROKEN_PP
    #undef  FIX_FE_FLAGS
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

/*
 * Raise all FP exception flags.
 * Must be before myfloat.h
 */
#ifndef   RAISE_ALL
  #define RAISE_ALL (void)feraiseexcept( FE_ALL_EXCEPT )
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
  #undef  FIX_SCHAR_RANDOM_INVALID
  #undef  FIX_SINT_RANDOM_INVALID
  #undef  FIX_SLLONG_RANDOM_INVALID
  #undef  FIX_SLONG_RANDOM_INVALID
  #undef  FIX_SSHRT_RANDOM_INVALID
  #undef  FIX_UCHAR_RANDOM_INVALID
  #undef  FIX_UINT_RANDOM_INVALID
  #undef  FIX_ULLONG_RANDOM_INVALID
  #undef  FIX_ULONG_RANDOM_INVALID
  #undef  FIX_USHRT_RANDOM_INVALID
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

#ifdef QUAD_LDBL
  #define LD        LDBL, LDBL_MAX_EXP
#else
  #define LD long double, LDBL_MAX_EXP
#endif
#define  D      double,  DBL_MAX_EXP
#define  F       float,  FLT_MAX_EXP

#ifdef ULLONG_MAX
  #ifdef ULONGLONG
    #define ULL          ULONGLONG, "%#llx", ULLONG_MAX, ULLONG_MIN
  #else
    #define ULL unsigned long long, "%#llx", ULLONG_MAX, ULLONG_MIN
  #endif
#endif
#ifdef LLONG_MAX
  #ifdef LONGLONG
    #define SLL           LONGLONG, "%#llx",  LLONG_MAX,  LLONG_MIN
  #else
    #define SLL   signed long long, "%#llx",  LLONG_MAX,  LLONG_MIN
  #endif
#endif
#define  UL unsigned      long,  "%#lx",  ULONG_MAX,  ULONG_MIN
#define  SL   signed      long,  "%#lx",   LONG_MAX,   LONG_MIN
#define  UI unsigned       int,   "%#x",   UINT_MAX,   UINT_MIN
#define  SI   signed       int,   "%#x",    INT_MAX,    INT_MIN
#define  US unsigned     short,  "%#hx",  USHRT_MAX,  USHRT_MIN
#define  SS   signed     short,  "%#hx",   SHRT_MAX,   SHRT_MIN
#define  UC unsigned      char,   "%#x",  UCHAR_MAX,  UCHAR_MIN
#define  SC   signed      char,   "%#x",  SCHAR_MAX,  SCHAR_MIN

#define STR(s) # s
#define STN(n) tst_ ## n

/*
 * 'Type generic' function done via macro.
 * f = 2**j and is the value to convert.
 * First, test for expected flags being raised; for both plus and minus.
 * Second, test for any flags being cleared; for both plus and minus.
 * Note:  For sign-magnitude and 1s complement, type_MIN == -type_MAX,
 * and is 1 less than a power of 2.
 * While for 2s complement, type_MIN == -type_MAX-1, and is a power of 2.
 * minint is used to detect this one special case.
 */
#define DEF_TSTS(N,PRT,I_TYPE,FMT,I_MAX,I_MIN,F_TYPE,F_MAX_EXP)\
static void STN(N)(void){\
  F_TYPE f = (F_TYPE)1.0;\
  I_TYPE i1,i2,i3,i4,want;\
  I_TYPE imax = (I_TYPE)(I_MAX);\
  unsigned long j;\
  unsigned long const max_exp = log2radix * (unsigned long)F_MAX_EXP;\
  int flags;\
  unsigned int lcl_failed=0u;\
  int minint = (int)((I_MIN) != -(I_MAX));\
  if(PRT)(void)printf("Testing conversion from " STR(F_TYPE) " to " STR(I_TYPE) "\n");\
  want = (I_TYPE)1;\
  for(j=0uL; (j<max_exp); j++){\
    CLEAR_ALL;\
		/*lint -e524 -e732 -e919 */\
    i1 = f;	/* conversion we are testing */\
		/*lint +e524 +e732 +e919  */\
    flags = fetestexcept( FE_ALL_EXCEPT );\
    if( imax ){\
      if( flags ){\
	lcl_failed++;\
        (void)printf("+f: j=%lu, expected no flags, got: %#x\n", j, (unsigned)flags);\
      }\
      if( i1 != want ){\
	lcl_failed++;\
        (void)printf("+f: j=%lu, expected power of 2, got: " FMT "\n", j, i1);\
      }\
    }else{\
      if( !((FE_INVALID) & flags) ){\
	lcl_failed++;\
	(void)printf("+f: j=%lu, expected FE_INVALID, got: %#x\n", j, (unsigned)flags);\
      }\
      if( (~(FE_INVALID)) & flags ){\
	lcl_failed++;\
	(void)printf("+f: j=%lu, expected just FE_INVALID, got: %#x\n", j, (unsigned)flags);\
      }\
    }\
    CLEAR_ALL;\
		/*lint -e524 -e732 -e919 */\
    i2 = -f;	/* conversion we are testing */\
		/*lint +e524 +e732 +e919  */\
    flags = fetestexcept( FE_ALL_EXCEPT );\
    if( imax || minint ){\
      if(!imax){\
        minint = 0;\
        want = (I_TYPE)(I_MIN);\
      }else{\
        want = (I_TYPE)-want;\
      }\
      if( flags ){\
	lcl_failed++;\
        (void)printf("-f: j=%lu, expected no flags, got: %#x\n", j, (unsigned)flags);\
      }\
      if( i2 != want ){\
	lcl_failed++;\
        (void)printf("-f: j=%lu, expected power of 2, got: " FMT "\n", j, i2);\
      }\
    }else{\
      if( !((FE_INVALID) & flags) ){\
	lcl_failed++;\
	(void)printf("-f: j=%lu, expected FE_INVALID, got: %#x\n", j, (unsigned)flags);\
      }\
      if( (~(FE_INVALID)) & flags ){\
	lcl_failed++;\
	(void)printf("-f: j=%lu, expected just FE_INVALID, got: %#x\n", j, (unsigned)flags);\
      }\
    }\
    RAISE_ALL;\
		/*lint -e524 -e732 -e919 */\
    i3 = f;	/* conversion we are testing */\
		/*lint +e524 +e732 +e919  */\
    flags = fetestexcept( FE_ALL_EXCEPT );\
    if( allflags != flags ){\
      lcl_failed++;\
      (void)printf("+f: j=%lu, expected all flags, got: %#x\n", j, (unsigned)flags);\
    }\
    RAISE_ALL;\
		/*lint -e524 -e732 -e919 */\
    i4 = -f;	/* conversion we are testing */\
		/*lint +e524 +e732 +e919  */\
    flags = fetestexcept( FE_ALL_EXCEPT );\
    if( allflags != flags ){\
      lcl_failed++;\
      (void)printf("-f: j=%lu, expected all flags, got: %#x\n", j, (unsigned)flags);\
    }\
    f += f;		/* next larger power of 2 */\
			/*lint -e704 -e702 */\
    imax >>= 1;		/* next smaller power of 2 */\
			/*lint +e704 +e702 */\
    if(imax){\
      want = (I_TYPE)-want;\
      want <<= 1;\
    }\
    if(9<=lcl_failed){\
      if(!PRT)(void)printf("Testing conversion from " STR(F_TYPE) " to " STR(I_TYPE) "\n");\
      (void)printf("Too many failures\n");\
      break;\
    }\
  }\
  failed += lcl_failed;\
}

/*
 * Same tests as above, but for unsigned types.
 */
#define DEF_TSTU(N,PRT,I_TYPE,FMT,I_MAX,I_MIN,F_TYPE,F_MAX_EXP)\
static void STN(N)(void){\
  F_TYPE f = (F_TYPE)1.0;\
  I_TYPE i1,i2,i3,i4,want;\
  I_TYPE imax = (I_TYPE)(I_MAX);\
  unsigned long j;\
  unsigned long const max_exp = log2radix * (unsigned long)F_MAX_EXP;\
  int flags;\
  unsigned int lcl_failed=0u;\
  if(PRT)(void)printf("Testing conversion from " STR(F_TYPE) " to " STR(I_TYPE) "\n");\
  want = (I_TYPE)1;\
  for(j=0uL; (j<max_exp); j++){\
    CLEAR_ALL;\
		/*lint -e524 -e732 -e919 */\
    i1 = f;	/* conversion we are testing */\
		/*lint +e524 +e732 +e919  */\
    flags = fetestexcept( FE_ALL_EXCEPT );\
    if( imax ){\
      if( flags ){\
	lcl_failed++;\
        (void)printf("+f: j=%lu, expected no flags, got: %#x\n", j, (unsigned)flags);\
      }\
      if( i1 != want ){\
	lcl_failed++;\
        (void)printf("+f: j=%lu, expected power of 2, got: " FMT "\n", j, i1);\
      }\
    }else{\
      if( !((FE_INVALID) & flags) ){\
	lcl_failed++;\
	(void)printf("+f: j=%lu, expected FE_INVALID, got: %#x\n", j, (unsigned)flags);\
      }\
      if( (~(FE_INVALID)) & flags ){\
	lcl_failed++;\
	(void)printf("+f: j=%lu, expected just FE_INVALID, got: %#x\n", j, (unsigned)flags);\
      }\
    }\
    CLEAR_ALL;\
		/*lint -e524 -e732 -e919 */\
    i2 = -f;	/* conversion we are testing */\
		/*lint +e524 +e732 +e919  */\
    flags = fetestexcept( FE_ALL_EXCEPT );\
    if( !((FE_INVALID) & flags) ){\
      lcl_failed++;\
      (void)printf("-f: j=%lu, expected FE_INVALID, got: %#x; i2 is " FMT "\n", j, (unsigned)flags, i2);\
    }\
    if( (~(FE_INVALID)) & flags ){\
      lcl_failed++;\
      (void)printf("-f: j=%lu, expected just FE_INVALID, got: %#x; i2 is " FMT "\n", j, (unsigned)flags, i2);\
    }\
    RAISE_ALL;\
		/*lint -e524 -e732 -e919 */\
    i3 = f;	/* conversion we are testing */\
		/*lint +e524 +e732 +e919  */\
    flags = fetestexcept( FE_ALL_EXCEPT );\
    if( allflags != flags ){\
      lcl_failed++;\
      (void)printf("+f: j=%lu, expected all flags, got: %#x\n", j, (unsigned)flags);\
    }\
    RAISE_ALL;\
		/*lint -e524 -e732 -e919 */\
    i4 = -f;	/* conversion we are testing */\
		/*lint +e524 +e732 +e919  */\
    flags = fetestexcept( FE_ALL_EXCEPT );\
    if( allflags != flags ){\
      lcl_failed++;\
      (void)printf("-f: j=%lu, expected all flags, got: %#x\n", j, (unsigned)flags);\
    }\
    f += f;		/* next larger power of 2 */\
			/*lint -e704 -e702 */\
    imax >>= 1;		/* next smaller power of 2 */\
			/*lint +e704 +e702 */\
    if(imax){\
      want <<= 1;\
    }\
    if(9<=lcl_failed){\
      if(!PRT)(void)printf("Testing conversion from " STR(F_TYPE) " to " STR(I_TYPE) "\n");\
      (void)printf("Too many failures\n");\
      break;\
    }\
  }\
  failed += lcl_failed;\
}

static unsigned long log2radix;
static unsigned int failed = 0u;

/*
 * Suppose a CPU has two or more FPUs (such as a x86 with a x87 and MMX/sse2),
 * each with its own set of floating-point exception flags.  Then, it is
 * possible that FE_INVALID might have a bit for each FPU.  If, invalid is
 * raised on this FPU and, flags = fetestexcept(FE_ALL_EXCEPT);
 * returns the exceptions for just the FPU where this program is running,
 * then flags == FE_INVALID will be false, even though just invalid is raised.
 * So, need to form a mask of all the flags for just this FPU.
 */
static int allflags;	/* flags raised after RAISE_ALL; */

#if 1	/* Dummy to match tdfp2int.fjt */

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
    #else				/* a workaround for some compilers */
      #define DEF_TESTU(N,I,F) DEF_TEST_UI((N,P,I,F))
      #define DEF_TEST_UI(args) DEF_TSTU args
      #define DEF_TESTS(N,I,F) DEF_TEST_SI((N,P,I,F))
      #define DEF_TEST_SI(args) DEF_TSTS args
    #endif

    #if (defined ULLONG_MAX) && (!defined FIX_FP_FROM_ULLI) && (!defined FIX_ULLONG_RANDOM_INVALID)
DEF_TESTU(00,ULL,LD)
    #endif
    #if (defined LLONG_MAX) && (!defined FIX_SLLONG_RANDOM_INVALID)
DEF_TESTS(01,SLL,LD)
    #endif
    #if (!defined FIX_ULONG_RANDOM_INVALID)
DEF_TESTU(02, UL,LD)
    #endif
    #if (!defined FIX_SLONG_RANDOM_INVALID)
DEF_TESTS(03, SL,LD)
    #endif
    #if (!defined FIX_UINT_RANDOM_INVALID)
DEF_TESTU(04, UI,LD)
    #endif
    #if (!defined FIX_SINT_RANDOM_INVALID)
DEF_TESTS(05, SI,LD)
    #endif
    #if (!defined FIX_USHRT_RANDOM_INVALID)
DEF_TESTU(06, US,LD)
    #endif
    #if (!defined FIX_SSHRT_RANDOM_INVALID)
DEF_TESTS(07, SS,LD)
    #endif
    #ifdef FIX_INVALID_OCTAL
      #if (!defined FIX_UCHAR_RANDOM_INVALID)
DEF_TESTU(108, UC,LD)
      #endif
      #if (!defined FIX_SCHAR_RANDOM_INVALID)
DEF_TESTS(109, SC,LD)
      #endif
    #else
      #if (!defined FIX_UCHAR_RANDOM_INVALID)
DEF_TESTU(08, UC,LD)
      #endif
      #if (!defined FIX_SCHAR_RANDOM_INVALID)
DEF_TESTS(09, SC,LD)
      #endif
    #endif

    #if (defined ULLONG_MAX) && (!defined FIX_FP_FROM_ULLI) && (!defined FIX_ULLONG_RANDOM_INVALID)
DEF_TESTU(10,ULL, D)
    #endif
    #if (defined LLONG_MAX) && (!defined FIX_SLLONG_RANDOM_INVALID)
DEF_TESTS(11,SLL, D)
    #endif
    #if (!defined FIX_ULONG_RANDOM_INVALID)
DEF_TESTU(12, UL, D)
    #endif
    #if (!defined FIX_SLONG_RANDOM_INVALID)
DEF_TESTS(13, SL, D)
    #endif
    #if (!defined FIX_UINT_RANDOM_INVALID)
DEF_TESTU(14, UI, D)
    #endif
    #if (!defined FIX_SINT_RANDOM_INVALID)
DEF_TESTS(15, SI, D)
    #endif
    #if (!defined FIX_USHRT_RANDOM_INVALID)
DEF_TESTU(16, US, D)
    #endif
    #if (!defined FIX_SSHRT_RANDOM_INVALID)
DEF_TESTS(17, SS, D)
    #endif
    #if (!defined FIX_UCHAR_RANDOM_INVALID)
DEF_TESTU(18, UC, D)
    #endif
    #if (!defined FIX_SCHAR_RANDOM_INVALID)
DEF_TESTS(19, SC, D)
    #endif

    #if (defined ULLONG_MAX) && (!defined FIX_FP_FROM_ULLI) && (!defined FIX_ULLONG_RANDOM_INVALID)
DEF_TESTU(20,ULL, F)
    #endif
    #if (defined LLONG_MAX) && (!defined FIX_SLLONG_RANDOM_INVALID)
DEF_TESTS(21,SLL, F)
    #endif
    #if (!defined FIX_ULONG_RANDOM_INVALID)
DEF_TESTU(22, UL, F)
    #endif
    #if (!defined FIX_SLONG_RANDOM_INVALID)
DEF_TESTS(23, SL, F)
    #endif
    #if (!defined FIX_UINT_RANDOM_INVALID)
DEF_TESTU(24, UI, F)
    #endif
    #if (!defined FIX_SINT_RANDOM_INVALID)
DEF_TESTS(25, SI, F)
    #endif
    #if (!defined FIX_USHRT_RANDOM_INVALID)
DEF_TESTU(26, US, F)
    #endif
    #if (!defined FIX_SSHRT_RANDOM_INVALID)
DEF_TESTS(27, SS, F)
    #endif
    #if (!defined FIX_UCHAR_RANDOM_INVALID)
DEF_TESTU(28, UC, F)
    #endif
    #if (!defined FIX_SCHAR_RANDOM_INVALID)
DEF_TESTS(29, SC, F)
    #endif

  #endif /* FIX_BROKEN_PP */

#endif	/* Dummy to match tdfp2int.fjt */

/*================================================================*/

int main(void){

#ifndef TAILOR
  (void)printf("Info:  'TAILOR' not defined => NOT using any bug workarounds.\n");
#endif

#ifndef RETVAL
  (void)printf("This program tests conversions from floating-point types to integer types.\n");
  (void)printf("It tests positive powers of 2.0, e.g., f = 2.0**j\n");
  (void)printf("It tests both i = +f; and i = -f;\n");
  (void)printf("For numbers that are representable, it checks expected value and no exceptions.\n");
  (void)printf("For numbers that are NOT representable, it checks for FE_INVALID exception.\n");
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

#if (!defined FIX_FE_FLAGS)
  RAISE_ALL;
  allflags = fetestexcept( FE_ALL_EXCEPT );	/* get flags for this FPU */
  if( 0 == allflags ){
    failed++;
    (void)printf("Failed due to RAISE_ALL or fetestexcept doing no flags.\n");
  }else if( 0 == (allflags & (FE_INVALID)) ){
    failed++;
    (void)printf("Failed due to RAISE_ALL or fetestexcept not doing FE_INVALID.\n");
  }
#endif

#if (defined FIX_BROKEN_PP)
  #ifndef RETVAL
  failed++;
  (void)printf("Failed due to broken PreProcessor.\n");
  #endif

#elif (defined FIX_FE_FLAGS)
  #ifndef RETVAL
  failed++;
  (void)printf("Failed due to broken FP flags.\n");
  #endif

#else

  #if (defined FIX_SEMIBROKEN_PP)
    #ifndef RETVAL
  failed++;
  (void)printf("Failed due to semibroken PreProcessor.\n");
    #endif
  #endif

  /*
   * Tests from long double
   */
  #if (defined ULLONG_MAX) && (!defined FIX_FP_FROM_ULLI) && (!defined FIX_ULLONG_RANDOM_INVALID)
  tst_00();
  #endif
  #if (defined LLONG_MAX) && (!defined FIX_SLLONG_RANDOM_INVALID)
  tst_01();
  #endif
  #if (!defined FIX_ULONG_RANDOM_INVALID)
  tst_02();
  #endif
  #if (!defined FIX_SLONG_RANDOM_INVALID)
  tst_03();
  #endif
  #if (!defined FIX_UINT_RANDOM_INVALID)
  tst_04();
  #endif
  #if (!defined FIX_SINT_RANDOM_INVALID)
  tst_05();
  #endif
  #if (!defined FIX_USHRT_RANDOM_INVALID)
  tst_06();
  #endif
  #if (!defined FIX_SSHRT_RANDOM_INVALID)
  tst_07();
  #endif
  #ifdef FIX_INVALID_OCTAL
    #if (!defined FIX_UCHAR_RANDOM_INVALID)
  tst_108();
    #endif
    #if (!defined FIX_SCHAR_RANDOM_INVALID)
  tst_109();
    #endif
  #else
    #if (!defined FIX_UCHAR_RANDOM_INVALID)
  tst_08();
    #endif
    #if (!defined FIX_SCHAR_RANDOM_INVALID)
  tst_09();
    #endif
  #endif

  /*
   * Tests from double
   */
  #if (defined ULLONG_MAX) && (!defined FIX_FP_FROM_ULLI) && (!defined FIX_ULLONG_RANDOM_INVALID)
  tst_10();
  #endif
  #if (defined LLONG_MAX) && (!defined FIX_SLLONG_RANDOM_INVALID)
  tst_11();
  #endif
  #if (!defined FIX_ULONG_RANDOM_INVALID)
  tst_12();
  #endif
  #if (!defined FIX_SLONG_RANDOM_INVALID)
  tst_13();
  #endif
  #if (!defined FIX_UINT_RANDOM_INVALID)
  tst_14();
  #endif
  #if (!defined FIX_SINT_RANDOM_INVALID)
  tst_15();
  #endif
  #if (!defined FIX_USHRT_RANDOM_INVALID)
  tst_16();
  #endif
  #if (!defined FIX_SSHRT_RANDOM_INVALID)
  tst_17();
  #endif
  #if (!defined FIX_UCHAR_RANDOM_INVALID)
  tst_18();
  #endif
  #if (!defined FIX_SCHAR_RANDOM_INVALID)
  tst_19();
  #endif

  /*
   * Tests from float
   */
  #if (defined ULLONG_MAX) && (!defined FIX_FP_FROM_ULLI) && (!defined FIX_ULLONG_RANDOM_INVALID)
  tst_20();
  #endif
  #if (defined LLONG_MAX) && (!defined FIX_SLLONG_RANDOM_INVALID)
  tst_21();
  #endif
  #if (!defined FIX_ULONG_RANDOM_INVALID)
  tst_22();
  #endif
  #if (!defined FIX_SLONG_RANDOM_INVALID)
  tst_23();
  #endif
  #if (!defined FIX_UINT_RANDOM_INVALID)
  tst_24();
  #endif
  #if (!defined FIX_SINT_RANDOM_INVALID)
  tst_25();
  #endif
  #if (!defined FIX_USHRT_RANDOM_INVALID)
  tst_26();
  #endif
  #if (!defined FIX_SSHRT_RANDOM_INVALID)
  tst_27();
  #endif
  #if (!defined FIX_UCHAR_RANDOM_INVALID)
  tst_28();
  #endif
  #if (!defined FIX_SCHAR_RANDOM_INVALID)
  tst_29();
  #endif

  if(failed){
    (void)printf("To help you figure out which flags were raised:\n");
  #if (defined FE_INVALID) || (defined FIX_SYM_FE_INVALID_ENUM)
    (void)printf(" FE_INVALID   = %#x", (unsigned)FE_INVALID );
    if( allflags != FE_ALL_EXCEPT )
      (void)printf(",\tmasked = %#x", (unsigned)FE_INVALID & allflags);
    (void)printf("\n");
  #endif
  #if (defined FE_DIVBYZERO) || (defined FIX_SYM_FE_DIVBYZERO_ENUM)
    (void)printf(" FE_DIVBYZERO = %#x", (unsigned)FE_DIVBYZERO);
    if( allflags != FE_ALL_EXCEPT )
      (void)printf(",\tmasked = %#x", (unsigned)FE_DIVBYZERO & allflags);
    (void)printf("\n");
  #endif
  #if (defined FE_OVERFLOW) || (defined FIX_SYM_FE_OVERFLOW_ENUM)
    (void)printf(" FE_OVERFLOW  = %#x", (unsigned)FE_OVERFLOW );
    if( allflags != FE_ALL_EXCEPT )
      (void)printf(",\tmasked = %#x", (unsigned)FE_OVERFLOW & allflags);
    (void)printf("\n");
  #endif
  #if (defined FE_UNDERFLOW) || (defined FIX_SYM_FE_UNDERFLOW_ENUM)
    (void)printf(" FE_UNDERFLOW = %#x", (unsigned)FE_UNDERFLOW);
    if( allflags != FE_ALL_EXCEPT )
      (void)printf(",\tmasked = %#x", (unsigned)FE_UNDERFLOW & allflags);
    (void)printf("\n");
  #endif
  #if (defined FE_INEXACT) || (defined FIX_SYM_FE_INEXACT_ENUM)
    (void)printf(" FE_INEXACT   = %#x", (unsigned)FE_INEXACT  );
    if( allflags != FE_ALL_EXCEPT )
      (void)printf(",\tmasked = %#x", (unsigned)FE_INEXACT & allflags);
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
#ifdef FIX_FE_FLAGS
    (void)printf("FIX_FE_FLAGS\n");
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
#ifdef FIX_SCHAR_RANDOM_INVALID
    (void)printf("FIX_SCHAR_RANDOM_INVALID\n");
#endif
#ifdef FIX_SINT_RANDOM_INVALID
    (void)printf("FIX_SINT_RANDOM_INVALID\n");
#endif
#ifdef FIX_SLLONG_RANDOM_INVALID
    (void)printf("FIX_SLLONG_RANDOM_INVALID\n");
#endif
#ifdef FIX_SLONG_RANDOM_INVALID
    (void)printf("FIX_SLONG_RANDOM_INVALID\n");
#endif
#ifdef FIX_SSHRT_RANDOM_INVALID
    (void)printf("FIX_SSHRT_RANDOM_INVALID\n");
#endif
#ifdef FIX_UCHAR_RANDOM_INVALID
    (void)printf("FIX_UCHAR_RANDOM_INVALID\n");
#endif
#ifdef FIX_UINT_RANDOM_INVALID
    (void)printf("FIX_UINT_RANDOM_INVALID\n");
#endif
#ifdef FIX_ULLONG_RANDOM_INVALID
    (void)printf("FIX_ULLONG_RANDOM_INVALID\n");
#endif
#ifdef FIX_ULONG_RANDOM_INVALID
    (void)printf("FIX_ULONG_RANDOM_INVALID\n");
#endif
#ifdef FIX_USHRT_RANDOM_INVALID
    (void)printf("FIX_USHRT_RANDOM_INVALID\n");
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
