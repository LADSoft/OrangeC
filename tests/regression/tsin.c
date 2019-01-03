/*
 *  Classified Tydeman Consulting Proprietary
 *  Copyright (C) 1996, 2014 Tydeman Consulting.  All rights reserved.
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
 *****************************************************************
 *
 * How accurate is sin(355.0)?
 *
 * The program computes the approximate number of bits in error in the
 * answer returned from the sin() math function for one particular
 * argument. The number of ULPs (Units in Last Place) of error would be
 * about 2 ** BitsWrong.  So, the entries with 7.09 bits wrong have about
 * 135 ULPs error.
 *
 * The old C standard, C89 as amended, had no requirements on the
 * accuracy of the math library; it is a quality of implementation issue.
 * C99, the revision of C89 finished in late 1999, has added the requirement
 * that implementations document the accuracy of their math library.
 * C11 (the current C standard) has retained that requirement.
 *
 * While C99 has no requirement on the accuracy of the math library that
 * all implementations must follow (such as 5 ULP accuracy), there are
 * other "standards" that do have accuracy requirements on the math library.
 *
 * One "standard" is Draft international standard ISO/IEC 10967-2,
 * Language Independent Arithmetic (LIA) Part 2: Elementary Numerical
 * Functions.  LIA-2 should be added to ISO/ANSI C sometime in the future
 * (it may be several years). LIA-2 says the error limit on sin() is
 * 1.5 * rounding_error as long as the magnitude of the argument is
 * less than max_arg_sin, an implementation defined value.
 *
 * Proposed Standard for a Generic Package of Elementary Functions for Ada,
 * ISO-IEC/JTC1/SC22/WG9 (Ada) Numerics Rapporteur Group, 1990 is another
 * "standard".  It says that the maximum relative error allowed is
 * 2.0 * epsilon as long as the magnitude of the argument is less than
 * an implementation-dependent threshold, which must not be less than
 * radix ** (mantissa digits/2).
 *
 * Both "standards" have similar requirements of about 2 ULPs error. In
 * terms of this program, for IEEE-754 DBL which has 53 mantissa bits,
 * an error of 2 bits or less for arguments smaller than 94,906,265 would
 * meet Ada.  So, testing with an argument of 355 is well within the domain
 * where sin() must be accurate.  The integer value 355 was chosen for a
 * reason; for the details, see the file v1150001.h on the above FTP site.
 *
 * Depending upon how floating-point numbers are represented internally,
 * the number of bits wrong can vary by as much as one.  For machines
 * that use the same representation (such as all the IEEE-754 ones), that
 * is not a problem for using these numbers for comparison.
 *
 * For example, consider the 15-bit binary numbers (one near the top and
 * the other near the bottom of the same binade):
 *
 * 111111111000001      100000000001001         F = computed value
 * 111111111000000      100000000001000         f = correct value
 *               1                    1         absolute error
 * 1/111111111000000    1/100000000001000       relative error
 * (3.06e-5)            (6.10e-5)               relative error in decimal
 * -10.395              -9.704                  log(relative error)
 * 14.9972              14.0007                 bits "correct"
 *  0.0028               0.9993                 bits "wrong"
 *
 * Results of running this test on various compilers:
 * (as reported by several users of this test; your mileage may vary)
 *
 * BitsWrong   Hardware       Operating system  Compiler, version, and options
 *
 *  0.00 ARM + FPU              RiscOS             Norcroft ARM v4.69 C [no options]
 *  0.00 Apple Mac PPC 603e     Tenon MachTen      gcc 2.7.2.1 + Tim Prince sin()
 *  0.00 Apple Mac PPC G4       Mac OS X 10.3.7    Gnu gcc 3.3 (build 1495)
 *  0.00 Apple Mac PPC G4       Mac OS X 10.3.7    IBM XL C 6.0 + Gnu gcc 3.3
 *  0.00 DEC Alpha              OSF/1 3.2          DEC cc
 *  0.00 DEC Alpha              OSF/1 3.2          Gnu gcc 2.7.2.1
 *  0.00 DEC Alpha              OpenVMS 7.1        DEC C V5.7 (G FLT format)
 *  0.00 DEC Alpha              Windows NT 4.0 SP4 Visual C++ 5.0
 *  0.00 DEC Alpha 2100-5/250   OSF/1 3.2          /bin/c89
 *  0.00 DEC Alpha 2100-5/250   OSF/1 3.2          /bin/cc
 *  0.00 DEC Alpha 2100-5/250   OSF/1 3.2          /bin/cxx -x cxx
 *  0.00 DEC Alpha 2100-5/250   OSF/1 3.2          /usr/bin/c89
 *  0.00 DEC Alpha 2100-5/250   OSF/1 3.2          /usr/bin/cc
 *  0.00 DEC Alpha 2100-5/250   OSF/1 3.2          /usr/ccs/bin/c89
 *  0.00 DEC Alpha 2100-5/250   OSF/1 3.2          /usr/ccs/bin/cc
 *  0.00 DEC Alpha 2100-5/250   OSF/1 3.2          /usr/local/bin/g++
 *  0.00 DEC Alpha 2100-5/250   OSF/1 3.2          /usr/local/bin/gcc
 *  0.00 DEC Alpha 2100-5/250   OSF/1 3.2          /usr/local/bin/lcc -A -A
 *  0.00 DEC Alpha 2100-5/250   OSF/1 3.2          /usr/ucb/cc
 *  0.00 DEC Alpha 250 4/266    OSF/1 V4.0         cc (DEC C V5.2-023)
 *  0.00 DEC Alpha 4000/610     OSF/1 V3.2C        cc
 *  0.00 DEC Alpha 4100 5/600   Digital Unix V4.0D cc (DEC C V5.6-079)
 *  0.00 HP 9000 750            HP-UX B.10.20      c89 (A.10.32.03)
 *  0.00 HP 9000/735            HP-UX 10.01        /bin/CC
 *  0.00 HP 9000/735            HP-UX 10.01        /bin/c89
 *  0.00 HP 9000/735            HP-UX 10.01        /usr/bin/CC
 *  0.00 HP 9000/735            HP-UX 10.01        /usr/bin/c89
 *  0.00 HP 9000/735            HP-UX 10.01        /usr/local/bin/g++
 *  0.00 HP 9000/735            HP-UX 10.01        /usr/local/bin/gcc
 *  0.00 HP 9000/770            HP-UX B.10.01      Gnu gcc 2.7.0
 *  0.00 HP 9000/770            HP-UX B.10.01      HP cc A.10.11 + libc 76.3 -Ae
 *  0.00 HP Integrity           HP-UX 11i V3       HP c99, A.06.23, +decfp
 *  0.00 Hitachi 3027-G32/3050  HI-UX 04.00        c89 (1.2)
 *  0.00 IBM Power 6            AIX 6.1.7.17       IBM XL C/C++ for AIX V11.1
 *  0.00 IBM Power 6            AIX 6.1.7.17       gcc 4.5.1; -maix64 -lm
 *  0.00 IBM PowerPC 43P        AIX 4.1            /bin/c89
 *  0.00 IBM PowerPC 43P        AIX 4.1            /bin/cc
 *  0.00 IBM PowerPC 43P        AIX 4.1            /bin/xlC
 *  0.00 IBM PowerPC 43P        AIX 4.1            /usr/bin/c89
 *  0.00 IBM PowerPC 43P        AIX 4.1            /usr/bin/cc
 *  0.00 IBM PowerPC 43P        AIX 4.1            /usr/local/bin/g++
 *  0.00 IBM PowerPC 43P        AIX 4.1            /usr/local/bin/gcc
 *  0.00 IBM PowerPC 43P        AIX 4.2            /bin/c89
 *  0.00 IBM PowerPC 43P        AIX 4.2            /bin/cc
 *  0.00 IBM PowerPC 43P        AIX 4.2            /bin/xlC
 *  0.00 IBM PowerPC 43P        AIX 4.2            /usr/bin/c89
 *  0.00 IBM PowerPC 43P        AIX 4.2            /usr/bin/cc
 *  0.00 IBM PowerPC 43P        AIX 4.2            /usr/local/bin/g++
 *  0.00 IBM PowerPC 43P        AIX 4.2            /usr/local/bin/gcc
 *  0.00 IBM RS6000 6015/DD1    AIX 4.1.2.0        c89 (xlC.C 3.1.1.0)
 *  0.00 IBM RS6000 7006/42T    AIX 4.2.0.0        c89 (xlC.C 3.1.3.5)
 *  0.00 IBM RS6000 7010/250    AIX 4.1.4.0        c89 (xlC.C 3.1.3.5)
 *  0.00 IBM RS6000 7011/230    AIX 3.2.5.0        c89 (xlCcmp.* 2.1.3.0)
 *  0.00 IBM RS6000 7030/3AT    AIX 4.1.5.0        c89 (xlC.C 3.1.4.0)
 *  0.00 IBM S/390              OS/390 V2R5        IBM C/C++ V2R4
 *  0.00 Intel Core i5-64       Linux Fedora 15    gcc 4.6.0-9 (2011/05/30) + glibc-2.14-3 (x86_64)
 *  0.00 Intel Core i5-64       Linux Fedora 16    clang 2.9-10 + glibc 2.14.90-24; -std=c99 -lm
 *  0.00 Intel Core i5-64       Linux Fedora 16    gcc 4.6.3-2 + glibc-2.14.90-24 (x86_64)
 *  0.00 Intel Core i5-64       Linux Fedora 16    pcc 1.0.0-2 (2011/12/16) + glibc-2.14.90-24 (x86_64)
 *  0.00 Intel Core i5-64       Linux Fedora 18    gcc 4.7.2-8 + glibc-2.16-24 (x86_64) -std=gnu11 -mfpmath=387
 *  0.00 Intel Core i5-64       Linux Fedora 18    gcc 4.7.2-8 + glibc-2.16-24 (x86_64) -std=gnu11 -mfpmath=sse
 *  0.00 Intel Core i5-64       Solaris 11.1       Solaris Studio 12.3
 *  0.00 Intel Core i7-64       Ubuntu 3.2.0-40    gcc 4.6.3 + eglibc 2.15-0
 *  0.00 Intel Core2 Duo        Linux Fedora 17    Intel C/C++ icc 13.1 -std=c99 -strict-ansi -fp-model strict -O0
 *  0.00 Intel Core2 Duo        Windows Vista      Microsoft C/C++ VS 2010
 *  0.00 Intel Core2 Duo        Windows 7          Microsoft C/C++ VS 2013
 *  0.00 Intel PPro 150MHz      Solaris 2.5.1      gcc v2.7.2, -O -ffloat-store
 *  0.00 Intel PPro 150MHz      Solaris 2.5.1      sun cc PC3.0.1 21 Jan 1995, -O -fsimple
 *  0.00 Intel Pentium          OS/2 Warp          IBM Visual Age C++ 3.0
 *  0.00 Intel Pentium 4        Windows XP         LCC 2002/06/17
 *  0.00 Intel Pentium 4        Windows XP         Microsoft C/C++ V 14.00 aka Visual Studio 8
 *  0.00 Intel Pentium 4 IA-32  Linux Fedora 10    Intel C/C++ icc 11.1 -std=c99 -strict-ansi -fp-model strict -O0
 *  0.00 Intel Pentium/90       Windows NT 4.0     IBM Visual Age C/C++ V3.5
 *  0.00 Intel x86              Solaris 2.6        cc (Workshop 4.2)
 *  0.00 Intel x86              Solaris 7          cc (Workshop 5.0)
 *  0.00 Intel x86?             SCO UnixWare 7.1   cc 3.2
 *  0.00 Intel Xeon E7-4850-64  Linux 2.6.32       gcc 4.7.1 + glibc 2.12
 *  0.00 Intel Xeon E7-4850-64  Linux 2.6.32       icc 13.0.1
 *  0.00 Intel Xeon X7550-64    Linux RedHat 2.6.18 gcc 4.4.6-3 + glibc 2.5
 *  0.00 Intel Xeon X7550-64    Linux RedHat 2.6.18 icc 12.1.0
 *  0.00 Intel Xeon X7560-64    Linux Bull 2.6.32  gcc 4.4.5-6 + glibc 2.12
 *  0.00 Intel Xeon X7560-64    Linux Bull 2.6.32  icc 12.1.0
 *  0.00 Mips                   SGI Irix           cc -n32 -mips3 -O -lm
 *  0.00 SGI O2 R10000-SC       IRIX 6.3           /usr/local/bin/g++
 *  0.00 SGI O2 R10000-SC       IRIX 6.3           /usr/local/bin/gcc
 *  0.00 SGI Origin/200-4       IRIX 6.4           /bin/CC
 *  0.00 SGI Origin/200-4       IRIX 6.4           /bin/c89
 *  0.00 SGI Origin/200-4       IRIX 6.4           /bin/cc
 *  0.00 SGI Origin/200-4       IRIX 6.4           /usr/bin/CC
 *  0.00 SGI Origin/200-4       IRIX 6.4           /usr/bin/c89
 *  0.00 SGI Origin/200-4       IRIX 6.4           /usr/bin/cc
 *  0.00 SGI Origin/200-4       IRIX 6.4           /usr/local/bin/g++
 *  0.00 SGI Origin/200-4       IRIX 6.4           /usr/local/bin/gcc
 *  0.00 StrongARM+FPE 1.07M    RiscBSD 1.2        gcc 2.7.2.1
 *  0.00 Sun 4m sparc           Solaris 2.3        Gnu gcc 2.7.0
 *  0.00 Sun 4m sparc           Solaris 2.3        Sun cc
 *  0.00 Sun 4m sparc           Sun OS 4.1.4       Sun acc 3.0.1
 *  0.00 Sun 4m sparc           Sun OS 5.4         Gnu gcc 2.7.0
 *  0.00 Sun 4m sparc           Sun OS 5.4         Sun C 4.0
 *  0.00 Sun 4m sparc           Sun OS 5.4         cc (C 4.0)
 *  0.00 Sun SPARC 20/512       Solaris 2.6        /opt/SUNWspro/bin/CC
 *  0.00 Sun SPARC 20/512       Solaris 2.6        /opt/SUNWspro/bin/c89
 *  0.00 Sun SPARC 20/512       Solaris 2.6        /opt/SUNWspro/bin/cc
 *  0.00 Sun SPARC 20/512       Solaris 2.6        /usr/local/bin/g++
 *  0.00 Sun SPARC 20/512       Solaris 2.6        /usr/local/bin/gcc
 *  0.00 Sun SPARC 20/512       Solaris 2.6        /usr/local/bin/lcc -A -A
 *  0.00 Sun SPARC 20/512       Solaris 2.6        /usr/ucb/cc
 *  0.00 Sun SparcStation4      Solaris 2.5.1      gcc v2.7.2, -O -ffloat-store
 *  0.00 Sun SparcStation4      Solaris 2.5.1      sun cc SC4.0 18 Oct 1995 C 4.0, -O -fsimple=0
 *  0.00 Sun sparc              Sun OS 5.5.1       ACE CoSy cc
 *  0.00 Ultra Sparc            UXP/DS V20L10      fcc (Fujitsu C V12L10)
 *  0.02 Amiga1200 68030/882    AmigaOS v39 (3.0)  SAS/C 6.58 math=ffp [DBL is 24 bits]
 *  0.02 DECstation 5000/200    ULTRIX 4.3         /bin/cc
 *  0.02 DECstation 5000/200    ULTRIX 4.3         /usr/bin/cc
 *  0.02 DECstation 5000/200    ULTRIX 4.3         /usr/local/bin/g++
 *  0.02 DECstation 5000/200    ULTRIX 4.3         /usr/local/bin/gcc
 *  0.02 DECstation 5000/200    ULTRIX 4.3         /usr/local/bin/lcc -A -A
 *  0.02 IBM AS/400             IBM OS/400         IBM C/C++ ?
 *  0.02 Intel Pentium 4	RedHat Linux 8	   edg 3.2 + gcc 3.2 + Dinkumware 4.2 + glibc 2.2.93-5
 *  0.02 Motorola M-CORE        SDS simulator      dcc (Diab Data 4.3a)
 *  0.02 Motorola M-CORE        SDS simulator      dcc (Diab Data 4.3b)
 *  0.02 Sun UltraSPARC         Sun Solaris 2.5    Apogee-C SPARC rel 4.0 -fast
 *  0.02 Unisys A18             MCP (SSR 44.1)     A Series C++
 *  1.60 HP 9000 715            HP-UX A.09.05      c89 (A.09.61)
 *  1.60 HP 9000 715            HP-UX A.09.05      c89 (A.09.77)
 *  1.60 HP 9000 735            HP-UX A.09.07      c89 (A.09.73)
 *  1.60 HP 9000 777/C100       HP-UX A.09.07      c89 (A.09.77)
 *  1.60 HP 9000/735            HP-UX A.09.07      Gnu gcc 2.7.0
 *  1.60 HP 9000/735            HP-UX A.09.07      HP cc 9.73 -Ae
 *  3.34 MIPS R10000            UX/4800 R13.5      NEC R13.5
 *  3.34 MIPS R4400             UX/4800 R11.5      NEC R11.5
 *  4.48 Intel x86              DG/UX 5.4?         cc (Data General 4.20MU04?)
 *  7.09 Intel 80486            ISC 4.0.1          cc
 *  7.09 Intel 80486DX          OS/2 Warp          Gnu gcc 2.5.7 + emx 0.8h
 *  7.09 Intel 80486DX          OS/2 Warp          Gnu gcc 2.7.2 + emx 0.9b + fix05
 *  7.09 Intel 80486DX          OS/2 Warp          Gnu gcc 2.7.2.1 + emx 0.9c + fix02
 *  7.09 Intel 80486DX          PC DOS             Borland C/C++ 4.00 in C mode
 *  7.09 Intel 80486DX          PC DOS             Borland Turbo C 2.01
 *  7.09 Intel 80486DX          PC DOS             Symantec C/C++ 7.0r1x in C mode
 *  7.09 Intel 80486DX          PC DOS             Symantec C/C++ 7.0r1x in C++ mode
 *  7.09 Intel 80486DX          Windows 3.1        Borland C/C++ 4.00 in C++ mode
 *  7.09 Intel Core 2 Duo       Linux Fedora 14    Gnu gcc 4.5.1-4 + glibc 2.12.90-15; -std=gnu99 -frounding-math
 *  7.09 Intel Core 2 Duo       Linux Fedora 15    Gnu gcc 4.6.1-9 + glibc 2.14-5; -std=gnu99 -frounding-math
 *  7.09 Intel Core 2 Duo       Linux Fedora 16    Gnu gcc 4.6.2-1 + glibc 2.14.90-21; -std=gnu99 -frounding-math
 *  7.09 Intel Core 2 Duo       Linux Fedora 16    clang 2.9-6 + glibc-2.14.90-24
 *  7.09 Intel Core 2 Duo       Linux Fedora 16    pcc 1.0.0-2 (2011/12/16) + glibc-2.14.90-24
 *  7.09 Intel Core 2 Duo       Linux Fedora 17    clang 3.0-10 + glibc 2.15-32; -std=c1x -pedantic -lm
 *  7.09 Intel Core 2 Duo       Linux Fedora 17    gcc 4.7.0-2 + glibc 2.15-32; -std=gnu11 -mieee-fp
 *  7.09 Intel Core 2 Duo       OS/2 eCS 2.0       Open Watcom C/C++ v1.8
 *  7.09 Intel Core 2 Duo       OS/2 eCS 2.0       Open Watcom C/C++ v1.9
 *  7.09 Intel Core 2 Duo       Windows Vista      Borland C/C++ 5.5 in C mode w/ -DFENV_H="fenvx87.h"
 *  7.09 Intel Core 2 Duo       Windows Vista      LCC 2012/10/22
 *  7.09 Intel Pentium 4	Linux Fedora  6    gcc 4.1.2-13.fc6 + glibc2.5-18.fc6
 *  7.09 Intel Pentium 4	RedHat Linux 8	   gcc 3.2 + glibc 2.2.93-5
 *  7.09 Intel Pentium 4	SuSE Linux 9.1	   gcc 3.3.3-41 + glibc 2.3.3-97
 *  7.09 Intel Pentium 4	Windows XP	   Metrowerks CodeWarrior Pro 8.3
 *  7.09 Intel Pentium 4	Windows XP	   Metrowerks CodeWarrior Pro 9.2
 *  7.09 Intel Pentium 4        Linux Fedora 10    gcc 4.3.2-7, glibc 2.9-3; -std=gnu99 -frounding-math
 *  7.09 Intel Pentium 4        Linux Fedora 11    gcc 4.4.0-4, glibc 2.10.1-2; -std=gnu99 -frounding-math
 *  7.09 Intel Pentium 4        Linux Fedora 11    gcc 4.4.1-2, glibc 2.10.1-5; -std=gnu99 -frounding-math
 *  7.09 Intel Pentium 4        Linux Fedora 14    gcc 4.5.1-4 + glibc 2.12.90-15; -std=gnu99 -frounding-math
 *  7.09 Intel Pentium 4        OS/2 eCS 1.1       Open Watcom C/C++ v1.3
 *  7.09 Intel Pentium 4        OS/2 eCS 1.1       Open Watcom C/C++ v1.4
 *  7.09 Intel Pentium 4        OS/2 eCS 1.1       Open Watcom C/C++ v1.5
 *  7.09 Intel Pentium 4        Windows XP         Digital Mars C/C++ V8.16
 *  7.09 Intel Pentium 4        Windows XP         Digital Mars C/C++ V8.19
 *  7.09 Intel Pentium 4        Windows XP         Digital Mars C/C++ V8.20
 *  7.09 Intel Pentium 4        Windows XP         Digital Mars C/C++ V8.35.6
 *  7.09 Intel Pentium 4        Windows XP         Digital Mars C/C++ V8.38
 *  7.09 Intel Pentium 4        Windows XP         Digital Mars C/C++ V8.39
 *  7.09 Intel Pentium 4        Windows XP         Digital Mars C/C++ V8.47
 *  7.09 Intel Pentium 4        Windows XP         Digital Mars C/C++ V8.48
 *  7.09 Intel Pentium 4        Windows XP         Digital Mars C/C++ V8.50
 *  7.09 Intel Pentium 4        Windows XP         Digital Mars C/C++ V8.51
 *  7.09 Intel Pentium 4        Windows XP         LCC 2002/02/10
 *  7.09 Intel Pentium 4        Windows XP         LCC 2004/07/26
 *  7.09 Intel Pentium 4        Windows XP         LCC 2006/04/10
 *  7.09 Intel Pentium 4        Windows XP         LCC 2008/07/24 -ansic
 *  7.09 Intel Pentium 4        Windows XP         Microsoft VC++ V 12.00 aka Visual Studio 6.0
 *  7.09 Intel Pentium II MMX   Linux 2.0.33       /usr/bin/cc
 *  7.09 Intel Pentium II MMX   Linux 2.0.33       /usr/bin/g++
 *  7.09 Intel Pentium II MMX   Linux 2.0.33       /usr/bin/gcc
 *  7.09 Intel Pentium MMX 200  Linux 2.0.30       /usr/bin/cc
 *  7.09 Intel Pentium MMX 200  Linux 2.0.30       /usr/bin/g++
 *  7.09 Intel Pentium MMX 200  Linux 2.0.30       /usr/bin/gcc
 *  7.09 Intel Pentium/90       Windows NT 4.0     Microsoft C/C++ Visual 4.2
 *  7.09 Intel Pentium/90       Windows NT 4.0     Symantec C/C++ V7.01
 *  7.09 Intel PentiumPro/200   OS/2 4 Merlin      Gnu gcc 2.8.1 + emx 0.9d
 *  7.09 Intel PentiumPro/200   OS/2 4 Merlin      Metrowerks CodeWarrior Pro 4
 *  7.09 Intel PentiumPro/200   OS/2 4 Merlin      Metrowerks CodeWarrior Pro 5
 *  7.09 Intel PentiumPro/200   Windows NT 4.0     Intel C/C++ V2.4 -Op -Za -Od
 *  7.09 Intel PentiumPro/200   Windows NT 4.0 SP3 Borland C/C++ V5.01 (16-bit C mode)
 *  7.09 Intel PentiumPro/350   Windows NT 4.0 SP3 Microsoft VC++ V 12.00 aka Visual Studio 6.0
 *  7.09 Intel x86              DYNIX/ptx V4.4.4   cc (Sequent V4.4.4)
 *  7.09 Intel x86              NCR 3.01.01        NCR cc 3.02
 *  7.09 Intel x86              NCR 3.02           NCR cc 3.03
 *  7.09 Intel x86              Windows NT 4.0 SP4 Visual C++ 5.0
 *  7.09 Intel x86              Windows NT 4.0 SP4 Visual C++ 6.0 SP2
 *  7.09 NeXT Turbostation      Mach 3.3           /bin/cc
 *  7.09 NeXT Turbostation      Mach 3.3           /usr/local/bin/g++
 *  7.09 NeXT Turbostation      Mach 3.3           /usr/local/bin/gcc
 *  7.09 Sun 4m sparc           Sun OS 4.1.4       Gnu gcc 2.7.0
 *  7.09 Sun SPARC 4/380        SunOS 4.1.3        /usr/lang/CC
 *  7.09 Sun SPARC 4/380        SunOS 4.1.3        /usr/lang/acc
 *  7.09 Sun SPARC 4/380        SunOS 4.1.3        /usr/local/bin/g++
 *  7.09 Sun SPARC 4/380        SunOS 4.1.3        /usr/local/bin/gcc
 *  7.09 Sun SPARC 4/380        SunOS 4.1.3        /usr/local/bin/lcc -A -A
 *  7.11 Intel 80486DX          ISC 4.0.1          gcc 2.7.2
 *  7.13 Intel Pentium/90       Windows NT 4.0     Watcom v10.5
 * 10.73 Intel Pentium/90       Windows NT 4.0     Borland C/C++ V5.01
 * 10.73 MC68030/MC68882        System V.3 TAU     ACE Expert C alfa-8.21
 * 11.75 ARM + SWFP             RiscOS             Norcroft ARM v4.69 C [no options]
 * 21.98 Apple Mac PPC 603e     Tenon MachTen      gcc 2.7.2.1
 * 21.98 Mips IP22              SGI Irix   5.2     Gnu gcc 2.7.2.f.1
 * 21.98 Mips IP22              SGI Irix   5.2     cc
 * 21.98 Mips R4400+R4000       SGI Irix64 6.2     cc (C 6.2)
 * 21.98 Mips R4400+R4010       SGI Irix   5.3     cc (C 3.19)
 * 21.98 SGI Challenge L        IRIX 5.3           /bin/CC
 * 21.98 SGI Challenge L        IRIX 5.3           /bin/cc
 * 21.98 SGI Challenge L        IRIX 5.3           /usr/bin/CC
 * 21.98 SGI Challenge L        IRIX 5.3           /usr/bin/cc
 * 21.98 SGI Challenge L        IRIX 5.3           /usr/local/bin/g++
 * 21.98 SGI Challenge L        IRIX 5.3           /usr/local/bin/gcc
 * 21.98 SGI Challenge L        IRIX 5.3           /usr/local/bin/lcc -A -A
 * 21.98 SGI O2 R10000-SC       IRIX 6.3           /bin/CC
 * 21.98 SGI O2 R10000-SC       IRIX 6.3           /bin/c89
 * 21.98 SGI O2 R10000-SC       IRIX 6.3           /bin/cc
 * 21.98 SGI O2 R10000-SC       IRIX 6.3           /usr/bin/CC
 * 21.98 SGI O2 R10000-SC       IRIX 6.3           /usr/bin/c89
 * 21.98 SGI O2 R10000-SC       IRIX 6.3           /usr/bin/cc
 * 21.98 Sony NWS-5000 VP       NEWS-OS 6.1.1      cc (ANSI C 3.18)
 * 23.40 Motorola M-CORE        SDS simulator      dcc (Diab Data 4.1a)
 * 23.40 Motorola M-CORE        SDS simulator      dcc (Diab Data 4.2a)
 * ??.?? HP 9000/735            HP-UX 10.01        /bin/cc [would not compile]
 * ??.?? HP 9000/735            HP-UX 10.01        /usr/bin/cc [would not compile]
 * ??.?? HP 9000/735            HP-UX 10.01        /usr/ccs/bin/cc [would not compile]
 * ??.?? SGI O2 R10000-SC       IRIX 6.3           /usr/local/bin/lcc -A -A [would not link]
 * ??.?? SGI Origin/200-4       IRIX 6.4           /usr/local/bin/lcc -A -A [DBL_MANT_DIG is 0]
 * ??.?? Sun SPARC 4/380        SunOS 4.1.3        /bin/cc [no float.h]
 * ??.?? Sun SPARC 4/380        SunOS 4.1.3        /usr/bin/cc [no float.h]
 * ??.?? Sun SPARC 4/380        SunOS 4.1.3        /usr/ucb/cc [no float.h]
 * NaN   NeXT Turbostation      Mach 3.3           /usr/local/bin/lcc -A -A [FLT_RADIX is -1]
 *
 * I wish to acknowledge the following people for taking the time and effort
 * to run this test and sending me their results:
 *      Nelson H. F. Beebe, Center for Scientific Computing, University of Utah
 *      L. Busby
 *      Rex Jaeschke, consultant
 *      Larry Jones
 *      David S. Schwab
 *      Jonathan Ziebell
 *
 * MODIFICATION HISTORY:
 *
 * 1996/12/02 - Create; calculate approximate number of bits in error.
 * 1997/01/22 - Add in ULPs error calculation.
 * 1998/07/07 - Add many more system's results.
 * 1998/07/08 - Detect FLT_RADIX < 2.  Detect small precision for DBL.
 * 1999/08/11 - Add many more system's results.
 * 2000/03/01 - Make classification explicit in each file.
 * 2000/03/01 - Remove confidential so can post to public ftp site.
 * 2000/07/23 - Add "using namespace std;" for C++ usage.
 * 2003/02/17 - Add Pass/Fail messages.
 * 2003/02/20 - Add ability to tailor via external file (TAILOR).
 * 2003/02/22 - Add FIX_IMPLICIT_FLUSH.
 * 2004/07/30 - Update DEBUG section.
 * 2006/07/25 - Indent #if's.
 * 2007/11/21 - Add const as per PC-Lint 8.
 * 2008/07/14 - Shorten marco symbols to less than 32 chars.
 * 2008/11/29 - Suppress info messages if RETVAL defined (part of test suite).
 * 2009/04/28 - Another suppress info messages if RETVAL defined.
 * 2009/07/01 - Update DEBUG section.
 * 2012/01/23 - Add C11 (which replaced C99 in December 2011).
 * 2012/04/11 - C type names to FLT, DBL, LDBL.
 * 2012/06/06 - Print # of bits bad (even if 0) if not TAILOR.
 * 2012/06/06 - Include myfloat.h (so user can do overrides).
 * 2014/05/25 - Add FIX_NAMESPACE_STD.
 *
 ***********************************************************************/

/*lint -e766 // Header might not be used */

#ifdef   TAILOR			/* A way to tailor via external file	*/
  #include "tsin.h"		/* A place to define overrides		*/
#endif

#ifdef DEBUG	/* local override of FIXes to force errors to show */
  #ifdef DEBUG2	/* Kills some compilers */
    #undef  FIX_NAMESPACE_STD
  #endif
#endif

#ifdef __cplusplus		/* For C++ to call C functions		*/
  #ifndef FIX_NAMESPACE_STD
using namespace std;		/* Add implicit std:: to each function call */
  #endif
#endif

#include <float.h>		/* FLT_RADIX, DBL_MANT_DIG, DBL_EPSILON */
#include <math.h>		/* sin(), log()                         */
#include <stdio.h>		/* printf()                             */

#ifdef   TAILOR			/* A way to tailor via external file	*/
  #include "myfloat.h"		/* Will include your *.flt file		*/
#endif

#ifdef DEBUG	/* local override of FIXes to force errors to show */
  #ifdef DEBUG3
  #endif
  #undef  FIX_IMPLICIT_FLUSH
#endif
#ifdef DEBUG4
  #undef  RETVAL
#endif

#ifndef   DBL
  #define DBL double
#endif

int
main(void)
{
  DBL const     correct = -3.014435335948844921433028000865009959025e-5;
  DBL const     base = (DBL) FLT_RADIX;
  DBL const     prec = (DBL) DBL_MANT_DIG;
  DBL           calculate;	/* sin(355.0)                           */
  DBL           abs_error;	/* absolute error                       */
  DBL           rel_error;	/* relative error                       */
  DBL           ulp_error;	/* unit last place error                */
  DBL           eps;		/* epsilon in terms of bits             */
  DBL           bits_cor;	/* number of bits correct in answer     */
  DBL           bits_bad;	/* number of bits bad in answer         */
  DBL           bits_tot;	/* total number of bits in DBL          */
  DBL           log_2;		/* log(2)                               */

#ifndef TAILOR
  (void)printf("Info:  'TAILOR' not defined => NOT using any bug workarounds.\n");
#endif

  calculate = sin(355.0);

#ifndef RETVAL
  (void) printf("sin(355) = %.17e from math library.\n", calculate);
#endif

#if (FLT_RADIX < 2)
  (void) printf("Failed: FLT_RADIX has illegal value: %i", (int) FLT_RADIX);
  goto death;
#endif

  eps = DBL_EPSILON / base;	/* base ** -precision                   */
  log_2 = log(2.0);

  switch (FLT_RADIX) {
  case 2:
    bits_tot = prec;
    break;
  case 4:
    bits_tot = 2.0 * prec;
    break;
  case 8:
    bits_tot = 3.0 * prec;
    break;
  case 16:
    bits_tot = 4.0 * prec;
    break;
  default:
    bits_tot = (log(base) / log_2) * prec;	/* base-2 log( radix ) */
    break;
  }

  /*
   * Absolute error
   */
  abs_error = (calculate - correct);
  if (abs_error < 0.0) {
    abs_error = -abs_error;
  };

  /*
   * Relative error
   */
  rel_error = (abs_error) / correct;
  if (rel_error < 0.0) {
    rel_error = -rel_error;
  }
  if (0.0 == rel_error) {
    bits_bad = 0.0;
  } else {
    bits_cor = -log(rel_error) / log_2;
    bits_bad = bits_tot - bits_cor;
  }

  /*
   * 2**-16 < |sin(355)| < 2**-15
   * |sin(355)| * 32768 = 0.98777, eg, in binade just below 1.0
   * eps = base ** -prec = ULP spacing in binade just below 1.0
   * eps / 32768 = ULP spacing of sin(355) in terms of base-2
   * Absolute error / spacing of correct answer = ULPs error w.r.t. correct
   */
  ulp_error = abs_error * 32768.0 / eps;
  if( (2.0 < ulp_error)
#ifndef TAILOR
    || 1
#endif
  ){
    (void) printf("sin(355) = %.17e is correct answer.\n", correct);
    (void) printf("About %.2f bits are wrong out of %.2f total bits.\n",
		  bits_bad, bits_tot);
    (void) printf("That is about %.2f ULPs (Units Last Place) bits error.\n",
		  ulp_error);
  }

  if (bits_tot < 33.22) {	/* base-2 log of 10**10                 */
    (void) printf("ERROR: DBL has too small precision: %.2f", bits_tot);
  }
#ifdef DEBUG
  (void) printf("eps =%.25g\n", eps);
  (void) printf("abs =%.25g\n", abs_error);
  (void) printf("ulp =%.25g\n", ulp_error);
  (void) printf("rad =%.25g\n", base);
  (void) printf("prec=%.25g\n", prec);
#endif
  if( ulp_error <= 2.0 ){
    (void)printf("Test ( Pass ) {} \n");
  }else{
    (void)printf("Test Failed due to ULPs error being too large.\n");
  }

#if (FLT_RADIX < 2)
death:;
#endif

#ifdef FIX_IMPLICIT_FLUSH
  (void)fflush(stdout);		/* need explicit flush */
#endif
#if (defined RETVAL) && (defined STANDALONE_TEST)
  return RETVAL;		/* Being run as part of test suite */
#else
  return 0;
#endif
}
