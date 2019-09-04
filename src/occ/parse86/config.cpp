/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

/*
 * C configuration when we are using ICODE code generator output
 */
#include <stdio.h>
#include <string.h>
#include "compiler.h"
#include "winmode.h"
#include "Utils.h"

extern int dbgblocknum;
#ifndef WIN32
int prm_targettype = DOS32A;
#else
int prm_targettype = CONSOLE;
#endif
int prm_crtdll = false;
int prm_lscrtdll = false;
int prm_msvcrt = false;
int prm_assembler = false;
int prm_flat = false;
int prm_nodos = false;
static char usage_text[] =
    "[options] [@response file] files\n"
    "\n"
    "/1        - C1x mode                  /9        - C99 mode\n"
    "/c        - compile only              +e        - dump errors to file\n"
    "+i        - dump preprocessed file    +l        - dump listing file\n"
    "/oname    - specify output file name  /snn      - align stack\n"
    "+v        - enable debug symbols      /w-       - disable warnings\n"
    "+A        - disable extensions        /Dxxx     - define something\n"
    "/E[+]nn   - max number of errors      /Ipath    - specify include path\n"
    "/O-       - disable optimizations     +Q        - quiet mode\n"
    "/Sasm;dbg - make ASM source file      /T        - translate trigraphs\n"
    "/V        - show version and date     /Wxx      - set executable type\n"
    "/!        - No logo\n"
    "Codegen parameters: (/C[+][-][params])\n"
    "  +d   - display diagnostics          -b        - no BSS\n"
    "  +F   - flat model                   -l        - no C source in ASM file\n"
    "  -m   - no leading underscores       +r        - reverse order of bit ops\n"
    "  +R   - honor FAR keyword            +u        - 'char' type is unsigned\n"
    "Time: " __TIME__ "  Date: " __DATE__;

static int parse_param(char mode, const char* string);
static KEYWORD prockeywords[] = {
    /*
        {"_CR0", 4, kw_cr0, KW_NONANSI, 0 },
        {"_CR1", 4, kw_cr1, KW_NONANSI, 0 },
        {"_CR2", 4, kw_cr2, KW_NONANSI, 0 },
        {"_CR3", 4, kw_cr3, KW_NONANSI, 0 },
        {"_CR4", 4, kw_cr4, KW_NONANSI, 0 },
        {"_CR5", 4, kw_cr5, KW_NONANSI, 0 },
        {"_CR6", 4, kw_cr6, KW_NONANSI, 0 },
        {"_CR7", 4, kw_cr7, KW_NONANSI, 0 },
        {"_CS", 3, kw_A8, KW_NONANSI, 0 },
        {"_DR0", 4, kw_dr0, KW_NONANSI, 0 },
        {"_DR1", 4, kw_dr1, KW_NONANSI, 0 },
        {"_DR2", 4, kw_dr2, KW_NONANSI, 0 },
        {"_DR3", 4, kw_dr3, KW_NONANSI, 0 },
        {"_DR4", 4, kw_dr4, KW_NONANSI, 0 },
        {"_DR5", 4, kw_dr5, KW_NONANSI, 0 },
        {"_DR6", 4, kw_dr6, KW_NONANSI, 0 },
        {"_DR7", 4, kw_dr7, KW_NONANSI, 0 },
        {"_DS", 3, kw_A9, KW_NONANSI, 0 },
        {"_EAX", 4, kw_D0, KW_NONANSI, 0 },
        {"_EBP", 4, kw_D5, KW_NONANSI, 0 },
        {"_EBX", 4, kw_D3, KW_NONANSI, 0 },
        {"_ECX", 4, kw_D1, KW_NONANSI, 0 },
        {"_EDI", 4, kw_D7, KW_NONANSI, 0 },
        {"_EDX", 4, kw_D2, KW_NONANSI, 0 },
        {"_ES", 3, kw_AA, KW_NONANSI, 0 },
        {"_ESI", 4, kw_D6, KW_NONANSI, 0 },
        {"_ESP", 4, kw_D4, KW_NONANSI, 0 },
        {"_FS", 3, kw_AB, KW_NONANSI, 0 },
        {"_GS", 3, kw_AC, KW_NONANSI, 0 },
        {"_SS", 3, kw_AD, KW_NONANSI, 0 },
        {"_TR0", 4, kw_tr0, KW_NONANSI, 0 },
        {"_TR1", 4, kw_tr1, KW_NONANSI, 0 },
        {"_TR2", 4, kw_tr2, KW_NONANSI, 0 },
        {"_TR3", 4, kw_tr3, KW_NONANSI, 0 },
        {"_TR4", 4, kw_tr4, KW_NONANSI, 0 },
        {"_TR5", 4, kw_tr5, KW_NONANSI, 0 },
        {"_TR6", 4, kw_tr6, KW_NONANSI, 0 },
        {"_TR7", 4, kw_tr7, KW_NONANSI, 0 },
        {"__cdecl", 7, kw__cdecl, 0, 0 },
        {"__export", 8, kw__export, KW_NONANSI, 0 },
        {"__import", 8, kw__import, KW_NONANSI, 0 },
        {"__stdcall", 9, kw__stdcall, KW_NONANSI, 0 },
        {"_absolute", 9, kw__absolute, KW_NONANSI, 0 },
        {"_cdecl", 6, kw__cdecl, 0, 0 },
        {"_export", 7, kw__export, KW_NONANSI, 0 },
        {"_far", 4, kw__far, KW_NONANSI, 0 },
        {"_fault", 6, kw__fault, KW_NONANSI, 0 },
        {"_genbyte", 8, kw__genword, KW_NONANSI, 0 },
        {"_import", 7, kw__import, KW_NONANSI, 0 },
        {"_indirect", 9, kw__indirect, KW_NONANSI, 0 },
        {"_interrupt", 10, kw__interrupt, KW_NONANSI, 0 },
        {"_intrinsic", 10, kw__intrinsic, KW_NONANSI, 0 },
        {"_loadds", 7, kw__loadds, KW_NONANSI, 0 },
        {"_near", 5, kw__near, KW_NONANSI, 0 },
        {"_pascal", 7, kw__pascal, KW_NONANSI, 0 },
        {"_seg", 4, kw__seg, KW_NONANSI, 0 },
        {"_stdcall", 8, kw__stdcall, KW_NONANSI, 0 },
        {"_trap", 5, kw__trap, KW_NONANSI, 0 },
        {"alloca", 6, kw_alloca, KW_NONANSI, 0 },
        {"far", 3, kw__far, KW_NONANSI, 0 },
        {"near", 4, kw__near, KW_NONANSI, 0 },
        */
    {0, 0, kw_none}};
static ARCH_DEFINES defines[] = {
    /* must come first in this order */
    {"__WIN32__", "1", false, true},
    {"__LSCRTL_DLL", "1", false, true},
    {"__DOS__", "1", false, true},
    {"_WIN32", "1", false, true},
    {"__MSVCRT_DLL", "1", false, true},
    {"__CRTDLL_DLL", "1", false, true},
    {"__RAW_IMAGE__", "1", false, true},
    /* end ordered */
    {"__386__", "1", true, true},
    {"__i386__", "1", true, true},
    {"_i386_", "1", true, true},
    {"__i386", "1", true, true},
    {0},
};
static ARCH_SIZING sizes = {
    1, /*char a_bool;*/
    1, /*char a_char;*/
    2, /*char a_short;*/
    2, /*char a_wchar_t;*/
    4, /*char a_enum;*/
    4, /*char a_int;*/
    4, /*char a_long;*/
    8, /*char a_longlong;*/
    4, /*char a_addr;*/
    8, /*char a_farptr;*/
    2, /*char a_farseg;*/
    8, /*char a_memberptr;    */
    0,
    /* char a_struct; */ /* alignment only */
    4,                   /*char a_float;*/
    8,                   /*char a_double;*/
    10,                  /*char a_longdouble;*/
    0,                   /*char a_fcomplexpad;*/
    0,                   /*char a_rcomplexpad;*/
    0,                   /*char a_lrcomplexpad;*/
};
static ARCH_SIZING alignments = {
    1, /*char a_bool;*/
    1, /*char a_char;*/
    2, /*char a_short;*/
    2, /*char a_wchar_t;*/
    4, /*char a_enum;*/
    4, /*char a_int;*/
    4, /*char a_long;*/
    8, /*char a_longlong;*/
    4, /*char a_addr;*/
    4, /*char a_farptr;*/
    2, /*char a_farseg;*/
    4, /*char a_memberptr;    */
    0, /*char a_struct;  alignment only */
    /* imaginary same as real */
    4, /*char a_float;*/
    8, /*char a_double;*/
    8, /*char a_longdouble;*/
};
static ARCH_SIZING locks = {
    0, /*char a_bool; */
    0, /*char a_char; */
    0, /*char a_short; */
    0, /*char a_wchar_t; */
    0, /*char a_enum; */
    0, /*char a_int; */
    0, /*char a_long; */
    1, /*char a_longlong; */
    0, /*char a_addr; */
    1, /*char a_farptr; */
    1, /*char a_farseg; */
    1, /*char a_memberptr; */
    1,
    /*char a_struct; */ /* alignment only */
    1,                  /*char a_float; */
    1,                  /*char a_double; */
    1,                  /*char a_longdouble; */
    1,                  /*char a_fcomplexpad; */
    1,                  /*char a_rcomplexpad; */
    1,                  /*char a_lrcomplexpad; */
};
static ARCH_FLOAT aflt = {-126, 126, 128, 24};
static ARCH_FLOAT adbl = {-1022, 1022, 1024, 54};
static ARCH_FLOAT aldbl = {-16382, 16382, 16384, 64};
static ARCH_PEEP peeps[] = {0};
static ARCH_CHARACTERISTICS architecture = {
    &alignments, /* alignments */
    0,           /* custom alignment routine */
    &sizes,      /* sizes */
    &locks,      /* atomic locks */
    0,           /* routine is called in case parameters less than paramwidth need offsets */
    ISZ_ULONG,   /* size compatible to an integer */
    ISZ_ULONG,   /* size compatible to an address */
    8,           /* default packing level */
    4,           /* alignment for memory operations, as provided by RTL */
    /* floating point characteristics not fully implemented */
    &aflt,             /* characteristics of 'float' keyword */
    &adbl,             /* characteristics of 'double' keyword */
    &aldbl,            /* characteristics of 'long double' keyword */
    0,                 /* future floating type */
    0,                 /* register count */
    0,                 /* defines registers */
    1,                 /* register trees count */
    0, 0, 0, 0, 0,     /* defines peephole information */
    0,                 /* Max number of regs considered for fastcall */
    NULL,              /* register list for regs used in fastcall */
    0,                 /* preferred optimizations */
    0,                 /* optimizations we don't want */
    0,                 /* error options */
    false,             /* true if has floating point regs */
    0,                 /* floating point modes, not honored currently */
    ABM_USESIZE,       /* bool is determined by sizing above */
    ARM_FUNCTIONSCOPE, /* register allocation across entire function */
    8,                 /* number of bits in a MAU.  values other than 8 not supported */
    false,             /* little endian */
    false,             /* normal bit allocation*/
    false,             /* locals in stack memory*/
    false,             /* stack pointer grows down */
    false,             /* preallocate locals */
    8,                 /* size of a return block on stack (e.g. function ret addr & frame ptr) */
    4,                 /* minimium width/ padding of passed parameters in maus */
    4,                 /* minimum stack alignment */
    false,             /* library functions should bes genned as import calls */
};
extern ARCH_GEN outputfunctions;
ARCH_GEN outputfunctions;

static void WinmodeSetup(char select, const char* string)
{
    (void)select;
    switch (string[0])
    {
        case 'r':
            prm_targettype = RAW;
            defines[3].respect = false;
            defines[2].respect = false;
            defines[0].respect = false;
            defines[6].respect = true;
            break;
        case 'a':
            prm_targettype = DOS32A;
            defines[3].respect = false;
            defines[2].respect = true;
            defines[0].respect = false;
            defines[6].respect = false;
            break;
        case 'd':
            prm_targettype = DLL;
            defines[0].respect = true;
            defines[2].respect = false;
            defines[3].respect = true;
            defines[6].respect = false;
            break;
        case 'c':
            prm_targettype = CONSOLE;
            defines[0].respect = true;
            defines[2].respect = false;
            defines[3].respect = true;
            defines[6].respect = false;
            break;
        case 'g':
            prm_targettype = GUI;
            defines[0].respect = true;
            defines[2].respect = false;
            defines[3].respect = true;
            defines[6].respect = false;
            break;
        case 'e':
            prm_targettype = DOS;
            defines[3].respect = false;
            defines[2].respect = true;
            defines[0].respect = false;
            defines[6].respect = false;
            break;
        case 'h':
            prm_targettype = HXDOS;
            defines[3].respect = true;
            defines[2].respect = true;
            defines[0].respect = true;
            defines[6].respect = false;
            break;
        case 'x':
            prm_targettype = WHXDOS;
            defines[3].respect = false;
            defines[2].respect = true;
            defines[0].respect = false;
            defines[6].respect = false;
            break;
        default:
            Utils::fatal("Invalid executable type");
    }
    if (string[1] == 'm')
    {
        if (!defines[0].respect)
            Utils::fatal("Invalid use of LSCRTDLL");
        prm_msvcrt = true;
        defines[4].respect = true;
    }
    if (string[1] == 'c')
    {
        if (!defines[0].respect)
            Utils::fatal("Invalid use of CRTDLL");
        prm_crtdll = true;
        defines[5].respect = true;
    }
    else if (string[1] == 'l')
    {
        if (!defines[0].respect)
            Utils::fatal("Invalid use of LSCRTDLL");
        prm_lscrtdll = true;
        defines[1].respect = true;
        architecture.libsasimports = true;
    }
}
static int init(COMPILER_PARAMS* parms, ARCH_ASM* data, ARCH_DEBUG* debug)
{
    (void)parms;
    (void)data;
    (void)debug;
    WinmodeSetup(0, "c");
    return 1;
}
static int parse_param(char select, const char* string)
{
    if (select == 'W')
    {
        WinmodeSetup(select, string);
    }
    return 0;
}
static int parse_codegen(char mode, const char* string)
{
    switch (string[0])
    {
        case 'X':
            prm_nodos = mode;
            break;
        case 'F':
            prm_flat = mode;
            break;
        case 'R':
            cparams.prm_farkeyword = mode;
            break;
        case '?':
            cparams.prm_browse = mode;
            break;
        default:
            return 0; /* illegal */
    }
    return 1; /* eat one char */
}
ARCH_ASM assemblerInterface[] = {{
                                     "nasm",     /* assembler name */
                                     0,          /* backend data (compiler ignores) */
                                     "1",        /* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
                                     ".asm",     /* extension for assembly files */
                                     ".o",       /* extension for object files, NULL = has no object mode */
                                     "occ",      /* name of an environment variable to parse, or 0 */
                                     "occ",      /* name of the program, for usage */
                                     "occ",      /* name of a config file if you want to use one, or NULL (sans extension) */
                                     usage_text, /* pointer to usage text */
                                     nullptr,//args,       /* extra args */
                                     0, //sizeof(args) / sizeof(args[0]), /* number of args */
                                     prockeywords,     /* specific keywords, e.g. allow a 'bit' keyword and so forth */
                                     defines,          /* defines list to create at compile time, or null */
                                     NULL,             /* debug structure, or NULL */
                                     &architecture,    /* architecture characteristics */
                                     &outputfunctions, /* pointer to backend function linkages */
                                     NULL,             /* pointer to MSIL-specific data and functions */
                                     NULL,             /* pointer to extra builtin data */
                                     init,             /* return 1 to proceed */
                                     0,                /* precompile function, or NULL */
                                     0,                /* postcompile function, or NULL */
                                     NULL,             /* postprocess function, or NULL */
                                     0,                /* compiler rundown */
                                     NULL,             /* insert the output (executable name) into the backend */
                                     NULL, /* insert a non-compilable file in the backend list, e.g. for post processing, or NULL */
                                     parse_param,   /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
                                     parse_codegen, /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
                                     0,             /* parse a pragma directive, or null */
                                     NULL,          /* signal start of compile on a per file basis */
                                     NULL,          /* signal switching to a new source file */
                                     NULL,          /* write the object file (for native object formats) */
                                     NULL,          /* initialize the object file */
                                     NULL,          /* initialize inline assembler, per file, or NULL */
                                     NULL,          /* parse an assembly statement, or NULL */
                                     NULL,          /* translate an assembly instruction which was inlined */
                                     0,             /* initialize intrinsic mechanism, compiler startup */
                                     0,             /* search for an intrinsic */
                                     0,             /* enter a type in the BE */
                                 },
                                 {
                                     "fasm",     /* assembler name */
                                     0,          /* backend data (compiler ignores) */
                                     "1",        /* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
                                     ".asm",     /* extension for assembly files */
                                     ".o",       /* extension for object files, NULL = has no object mode */
                                     "occ",      /* name of an environment variable to parse, or 0 */
                                     "occ",      /* name of the program, for usage */
                                     "occ",      /* name of a config file if you want to use one, or NULL (sans extension) */
                                     usage_text, /* pointer to usage text */
                                     nullptr,//args,       /* extra args */
                                     0, //sizeof(args) / sizeof(args[0]), /* number of args */
                                     prockeywords,     /* specific keywords, e.g. allow a 'bit' keyword and so forth */
                                     defines,          /* defines list to create at compile time, or null */
                                     NULL,             /* debug structure, or NULL */
                                     &architecture,    /* architecture characteristics */
                                     &outputfunctions, /* pointer to backend function linkages */
                                     NULL,             /* pointer to MSIL-specific data and functions */
                                     NULL,             /* pointer to extra builtin data */
                                     init,             /* return 1 to proceed */
                                     0,                /* precompile function, or NULL */
                                     0,                /* postcompile function, or NULL */
                                     NULL,             /* postprocess function, or NULL */
                                     0,                /* compiler rundown */
                                     NULL,             /* insert the output (executable name) into the backend */
                                     NULL, /* insert a non-compilable file in the backend list, e.g. for post processing, or NULL */
                                     parse_param,   /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
                                     parse_codegen, /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
                                     0,             /* parse a pragma directive, or null */
                                     NULL,          /* signal start of compile on a per file basis */
                                     NULL,          /* signal switching to a new source file */
                                     NULL,          /* write the object file (for native object formats) */
                                     NULL,          /* initialize the object file */
                                     NULL,          /* initialize inline assembler, per file, or NULL */
                                     NULL,          /* parse an assembly statement, or NULL */
                                     NULL,          /* translate an assembly instruction which was inlined */
                                     0,             /* initialize intrinsic mechanism, compiler startup */
                                     0,             /* search for an intrinsic */
                                     0,             /* enter a type in the BE */
                                 },
                                 {
                                     "masm",     /* assembler name */
                                     0,          /* backend data (compiler ignores) */
                                     "1",        /* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
                                     ".asm",     /* extension for assembly files */
                                     ".o",       /* extension for object files, NULL = has no object mode */
                                     "occ",      /* name of an environment variable to parse, or 0 */
                                     "occ",      /* name of the program, for usage */
                                     "occ",      /* name of a config file if you want to use one, or NULL (sans extension) */
                                     usage_text, /* pointer to usage text */
                                     nullptr,//args,       /* extra args */
                                     0, //sizeof(args) / sizeof(args[0]), /* number of args */
                                     prockeywords,     /* specific keywords, e.g. allow a 'bit' keyword and so forth */
                                     defines,          /* defines list to create at compile time, or null */
                                     NULL,             /* debug structure, or NULL */
                                     &architecture,    /* architecture characteristics */
                                     &outputfunctions, /* pointer to backend function linkages */
                                     NULL,             /* pointer to MSIL-specific data and functions */
                                     NULL,             /* pointer to extra builtin data */
                                     init,             /* return 1 to proceed */
                                     0,                /* precompile function, or NULL */
                                     0,                /* postcompile function, or NULL */
                                     NULL,             /* postprocess function, or NULL */
                                     0,                /* compiler rundown */
                                     NULL,             /* insert the output (executable name) into the backend */
                                     NULL, /* insert a non-compilable file in the backend list, e.g. for post processing, or NULL */
                                     parse_param,   /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
                                     parse_codegen, /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
                                     0,             /* parse a pragma directive, or null */
                                     NULL,          /* signal start of compile on a per file basis */
                                     NULL,          /* signal switching to a new source file */
                                     NULL,          /* write the object file (for native object formats) */
                                     NULL,          /* initialize the object file */
                                     NULL,          /* initialize inline assembler, per file, or NULL */
                                     NULL,          /* parse an assembly statement, or NULL */
                                     NULL,          /* translate an assembly instruction which was inlined */
                                     0,             /* initialize intrinsic mechanism, compiler startup */
                                     0,             /* search for an intrinsic */
                                     0,             /* enter a type in the BE */
                                 },
                                 {0}};

int natural_size(EXPRESSION* exp)
{
    (void)exp;
    return ISZ_UINT;
}

EXPRESSION* stringlit(STRING* s)
{
    (void)s;
    return intNode(en_c_i, 0);
}