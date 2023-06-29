/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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
#include <cstdio>
#include <cstring>
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "Utils.h"
#include "../occ/x86regs.h"
#include "../occ/be.h"
#include "../occparse/winmode.h"
#include "config.h"
#include "ildata.h"
#include "optmodules.h"

namespace Optimizer
{
static char help_text[] =
    "[options] [@response file] files\n"
    "\n"
    "/1        - C1x mode                  /8        - c89 mode\n"
    "/9        - C99 mode                  /c        - compile only\n"
    "-d        - don't allow extensions    +e        - dump errors to file\n"
    "+i        - dump preprocessed file    +l        - dump listing file\n"
    "/oxxx     - specify output file name\n"
    "+A        - disable extensions        /Dxxx     - define something\n"
    "/E[+]nn   - max number of errors      /Ipath    - specify include path\n"
    "/Kfile    - set strong name key       /Lxxx     - set dlls to import from\n"
    "/M        - generate make stubs       /Nns.cls  - set namespace and class\n"
    "/O-       - disable optimizations     /P        - replace PInvokes\n"
    "+Q        - quiet mode                /T        - translate trigraphs\n"
    "/Vx.x.x.x - set assembly version      /!        - No logo\n"
    "--version - show version info\n"
    "/?, --help  - This text\n"
    "\nCodegen parameters: (/C[+][-][params])\n"
    "  +a   - use delegate for func ptr    +d   - display diagnostics\n"
    "  -b   - no BSS                       +f   - generated pinned addresses\n"
    "  +I   - initialize scalars           -l   - no C source in ASM file\n"
    "  -m   - no leading underscores       +s   - use native string literals\n"
    "  +u   - 'char' type is unsigned\n"
    "\nWarning Control:\n"
    " /w      - display no warnings         /wx            - display warnings as errors\n"
    " /woxxx  - only display warning once   /wexxx         - display warning xxx as error\n"
    " /wdxxx  - disable warning xxx         /wxxx          - enable warning xxx\n"
    " /werror=xxx[,yyy] display specific warning as error by name\n"
    "    valid warnings are:\n"
    "        missing-prototypes\n"
    "        unused-but-set-variables\n"
    "        unused-parameter\n"
    "        unused-variable\n"
    "        shadow\n"
    "        sign-compare\n"
    "        return-local-addr\n"
    "        jump-misses-init\n"
    "        write-strings\n"
    "        unused-function\n"
    "\n--architecture <architecture>\n"
    "    x86 - x86 code       msil - managed code\n"
    "\nDependency generation:\n"
    "  /M             - basic generation\n"
    "  /MM            - basic generation, user files only\n"
    "  /MF file       - specify output file\n"
    "  /MG            - missing headers as dependencies\n"
    "  /MP            - add phony targets\n"
    "  /MT target     - add target\n"
    "  /MQ target     - add target, quote special characters\n"
    "  /MD            - basic generation and continue\n"
    "  /MMD           - basic generation and continue, user files only\n"
    "\nCommand line behavior has changed.  Use environment var OCC_LEGACY_OPTIONS for old behavior\n\n"
    "Time: " __TIME__ "  Date: " __DATE__;
static char usage_text[] =
    "[options] [@response file] files";

char msil_bltins[] =
    " void exit(int); "
    "void __getmainargs(void *,void *,void*,int, void *); "
    "void *__iob_func(); "
    "unsigned short *__pctype_func(); "
    "int *_errno(); "
    "void *__OCCMSIL_GetProcThunkToManaged(void *proc, void *pdata); "
    "void *__OCCMSIL_GetProcThunkToUnmanaged(void *proc); "
    "void *__va_start__(); "
    "void *__va_arg__(void *, ...); ";

static int initnasm(COMPILER_PARAMS* parms, ARCH_ASM* data, ARCH_DEBUG* debug)
{
    (void)parms;
    (void)data;
    (void)debug;
    cparams.prm_assembler = pa_nasm;
    return 1;
}

static ARCH_DEFINES defines[] = {
    /* must come first in this order */
    {"__WIN32__", "1", true, true},
    {"__LSCRTL_DLL", "1", false, true},
    {"__DOS__", "1", false, true},
    {"_WIN32", "1", true, true},
    {"__MSVCRT_DLL", "1", false, true},
    {"__CRTDLL_DLL", "1", false, true},
    {"__RAW_IMAGE__", "1", false, true},
    {"__MANAGED__", "1", false, true},
    /* end ordered */
    {"__MSIL__", "1", true, true},
    {"__386__", "1", true, true},
    {"__i386__", "1", true, true},
    {"_i386_", "1", true, true},
    {"__i386", "1", true, true},
    {"_X86_", "1", true, true},
    {"_M_IX86", "1", true, true},
    {"_INTEGRAL_MAX_BITS", "64", true, true},
    {0},
};
static ARCH_SIZING sizes = {
    1,  /*char a_bool;*/
    1,  /*char a_char;*/
    2,  /*char a_short;*/
    2,  /*char a_wchar_t;*/
    4,  /*char a_enum;*/
    4,  /*char a_int;*/
    4,  /*char a_long;*/
    8,  /*char a_longlong;*/
    2,  /*char a_char16_t;*/
    4,  /*char a_char32_t;*/
    4,  /*char a_addr;*/
    8,  /*char a_farptr;*/
    2,  /*char a_farseg;*/
    12, /*char a_memberptr;    */
    0,
    /* char a_struct;  */ /* alignment only */
    4,                    /*char a_float;*/
    8,                    /*char a_double;*/
    8,                    /*char a_longdouble;*/
    0,                    /*char a_fcomplexpad;*/
    0,                    /*char a_rcomplexpad;*/
    2,                    /*char a_lrcomplexpad;*/
    0,                    // char a_alignedstruct; // __attribute((__aligned__))
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
    2, /*char a_char16_t;*/
    4, /*char a_char32_t;*/
    4, /*char a_addr;*/
    4, /*char a_farptr;*/
    2, /*char a_farseg;*/
    4, /*char a_memberptr;    */
    0, /*char a_struct;  alignment only */
    /* imaginary same as real */
    4,  /*char a_float;*/
    8,  /*char a_double;*/
    8,  /*char a_longdouble;*/
    8,  // char a_alignedstruct; // __attribute((__aligned__))
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
    0, /*char a_char16_t;*/
    0, /*char a_char32_t;*/
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
    0,                  // char a_alignedstruct; // __attribute((__aligned__))
};
static ARCH_FLOAT aflt = {-126, 126, 128, 24};
static ARCH_FLOAT adbl = {-1022, 1022, 1024, 53};
static ARCH_FLOAT aldbl = {-1022, 1022, 1024, 53};
static ARCH_PEEP peeps[] = {0};
static ARCH_CHARACTERISTICS architecture_characteristics = {
    &alignments,   /* alignments */
    0,             /* custom alignment routine */
    &sizes,        /* sizes */
    &locks,        /* atomic locks */
    4,             /* maximum value for __c11_atomic_is_lock_free */
    0,             /* routine is called in case parameters less than paramwidth need offsets */
    ISZ_ULONG,     /* size compatible to an integer */
    ISZ_ULONG,     /* size compatible to an address */
    ISZ_ULONGLONG, /* size compatible with the register width */
    8,             /* default packing level */
    8,             /* alignment of malloc() out of RTL */
    /* floating point characteristics not fully implemented */
    &aflt,                                                                        /* characteristics of 'float' keyword */
    &adbl,                                                                        /* characteristics of 'double' keyword */
    &aldbl,                                                                       /* characteristics of 'long double' keyword */
    0,                                                                            /* future floating type */
    REG_MAX,                                                                      /* register count */
    &regNames[0],                                                                 /* defines registers */
    1,                                                                            /* register trees count */
    &regRoot, (ARCH_REGCLASS**)regClasses, &regCosts, allocOrder, nullptr, peeps, /* defines peephole information */
    0,                                                                            /* Max number of regs considered for fastcall */
    0,                                                                            /* register list for regs used in fastcall */

    OPT_REVERSESTORE | OPT_REVERSEPARAM | OPT_ARGSTRUCTREF | OPT_EXPANDSWITCH | OPT_THUNKRETVAL, /* preferred optimizations */
    DO_NOGCSE | DO_NOLCSE | DO_NOREGALLOC | DO_NOADDRESSINIT | DO_NOPARMADJSIZE | DO_NOLOADSTACK | DO_NOENTRYIF | DO_NOALIAS |
        DO_NOCONST | DO_NOOPTCONVERSION | DO_NOINLINE | DO_UNIQUEIND | DO_NOFASTDIV | DO_NODEADPUSHTOTEMP | DO_MIDDLEBITS |
        DO_NOBRANCHTOBRANCH | DO_NOMULTOSHIFT,
    /* optimizations we don't want */
    EO_RETURNASERR,    /* error options */
    false,             /* true if has floating point regs */
    0,                 /* floating point modes, not honored currently */
    ABM_USESIZE,       /* bool is determined by sizing above */
    ARM_FUNCTIONSCOPE, /* register allocation across entire function */
    8,                 /* number of bits in a MAU.  values other than 8 not supported */
    4,                 /* natural size for this architecture, e.g. 4 for 32 bit arches and 8 for 64 bits */
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

static ARCH_DEBUG dbgStruct[1];
#if 0
    = { {
                                  "LS",         /* name of debug format */
                                  0,            /* backend specific data, compiler ignores */
                                  &dbgblocknum, /* pointer to variable which holds block number, or zero for no blocking */
                                  NULL,         // dbginit,                    /* per file initialization */
                                  0,            /* per file rundown */
                                  NULL,         // debug_outputtypedef,        /* output a (global) typedef */
                                  /* browser funcs */
                                  0, /* per file initialization */
                                  0, /* put browse info somewhere */
                                  0, /* put browse file list somewhere */
                                  0, /* per file rundown */
                              },
                              {0}

    };
#endif
static ARCH_GEN outputfunctions = {
#if 0
        NULL,                /* generate assembly language header */
        NULL,                /* generate assembly language trailer */
        NULL,                /* adjust an assembly language statement for the relative code labels */
        NULL,                /* allow access to the quad list prior to GCSE */
        examine_icode,       /* allow access to the quad list after GCSE */
        flush_peep,          /* called after function body is generated */
        oa_end_generation,   /* end of code generation */
        NULL,                /* internal conflict */
        NULL,                /* rewrites to improve register allocation */
        NULL,                /* precolor routine */
        oa_gen_strlab,       /* generate a named label */
        NULL,                /* generate a numbered label */
        oa_put_string_label, /* generate a numbered label */
        NULL,                /* reserve space for a bit */
        NULL,                /* initialize an int */
        NULL,                /* initialize a float */
        NULL,                /* initializae a pointer */
        oa_genstring,        /* initialize a string */
        NULL,                /* put a reference to a variable */
        NULL,                /* put a reference to something in the code segment */
        oa_gensrref,         /* put a reference to the startup.rundown */
        NULL,                /* put a reference to a label */
        NULL,                /* put the difference of two labels */
        NULL,                /* start a virtual segment */
        NULL,                /* end a virtual segment */
        oa_gen_vtt,          /* do a VT thunk entry */
        oa_gen_vc1,          /* do a vc1 thunk entry */
        oa_gen_importThunk,  /* do an import thunk entry */
        NULL,                /* generate uninitialized storage */
        NULL,                /* put an alignment command */
        NULL,                /* segment alignments */
        oa_enterseg,         /* switch to new seg */
        NULL,                /* exit current segment */
        NULL,                /* put a global definition */
        NULL,                /* put a local definition */
        NULL,                /* put a local static definition */
        oa_put_extern,       /* put an external definition */
        NULL,                /* put an import definition */
        NULL,                /* put an export definition */
        NULL,                /* put an alias */
        NULL,                /* put an included library name */
        0,                   /* backend handle intrinsic */
        asm_expressiontag,   /* expression tag */
        asm_tag,             /* tag */
        asm_line,            /* line number information and text */
        asm_blockstart,      /* block start */
        asm_blockend,        /* block end */
        asm_varstart,        /* var start */
        asm_func,            /* func start or end */
        asm_passthrough,     /* reserved */
        asm_datapassthrough, /* reserved */
        asm_label,           /* put a label in the code stream */
        asm_atomic,          /* atomic primitives */
        asm_goto,            /* unconditional branch */
        asm_gosub,           /* normal gosub to an immediate label or through a var */
        asm_fargosub,        /* far assemblyVersion of gosub */
        asm_trap,            /* 'trap' instruction - the arg will be an immediate # */
        asm_int,             /* 'int' instruction, calls a labeled function which is an interrupt */
        asm_ret,             /* return from subroutine */
        asm_fret,            /* far return from subroutine */
        asm_rett,            /* return from trap or int */
        asm_add,             /* evaluate an addition */
        asm_sub,             /* evaluate a subtraction */
        asm_udiv,            /* unsigned division */
        asm_umod,            /* unsigned modulous */
        asm_sdiv,            /* signed division */
        asm_smod,            /* signed modulous */
        asm_muluh,           /* multiply highword unsigned*/
        asm_mulsh,           /* multiply highword signed*/
        asm_mul,             /* multiply */
        asm_lsl,             /* unsigned shift left */
        asm_lsr,             /* unsigned shift right */
        asm_asr,             /* signed shift right */
        asm_neg,             /* negation */
        asm_not,             /* complement */
        asm_and,             /* binary and */
        asm_or,              /* binary or */
        asm_eor,             /* binary exclusive or */
        asm_setne,           /* evaluate a = b != c */
        asm_sete,            /* evaluate a = b == c */
        asm_setc,            /* evaluate a = b U< c */
        asm_seta,            /* evaluate a = b U> c */
        asm_setnc,           /* evaluate a = b U>= c */
        asm_setbe,           /* evaluate a = b U<= c */
        asm_setl,            /* evaluate a = b S< c */
        asm_setg,            /* evaluate a = b s> c */
        asm_setle,           /* evaluate a = b S<= c */
        asm_setge,           /* evaluate a = b S>= c */
        asm_assn,            /* assignment */
        asm_genword,         /* put a byte or word into the code stream */
        asm_coswitch,        /* switch characteristics */
        asm_swbranch,        /* case characteristics */
        asm_dc,              /* unused */
        asm_assnblock,       /* copy block of memory*/
        asm_clrblock,        /* clear block of memory */
        asm_cmpblock,        /* branch if two blocks of memory aren't equal */
        asm_jc,              /* branch if a U< b */
        asm_ja,              /* branch if a U> b */
        asm_je,              /* branch if a == b */
        asm_jnc,             /* branch if a U>= b */
        asm_jbe,             /* branch if a U<= b */
        asm_jne,             /* branch if a != b */
        asm_jl,              /* branch if a S< b */
        asm_jg,              /* branch if a S> b */
        asm_jle,             /* branch if a S<= b */
        asm_jge,             /* branch if a S>= b */
        asm_parm,            /* push a parameter*/
        asm_parmadj,         /* adjust stack after function call */
        asm_parmblock,       /* push a block of memory */
        asm_cppini,          /* cplusplus initialization (historic)*/
        asm_prologue,        /* function prologue */
        asm_epilogue,        /* function epilogue */
        asm_pushcontext,     /* push register context */
        asm_popcontext,      /* pop register context */
        asm_loadcontext,     /* load register context (e.g. at interrupt level ) */
        asm_unloadcontext,   /* unload register context (e.g. at interrupt level ) */
        asm_tryblock,        /* try, catch, end block */
        asm_stackalloc,      /* allocate stack space - positive value = allocate, negative value deallocate */
        asm_loadstack,       /* load the stack pointer from a var */
        asm_savestack,       /* save the stack pointer to a var */
        asm_seh,             /* seh */
        asm_functail,        /* function tail (e.g. destructor) start/end */
#endif
};

ARCH_ASM msilAssemblerInterface[] = {
    {
        "ilasm",          /* assembler name */
        0,                /* backend data (compiler ignores) */
        "1",              /* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
        ".il",            /* extension for assembly files */
        ".ilo",           /* extension for object files, NULL = has no object mode */
        ".rc;.res;.ilo",  /* extensions for files that should be passed to the backend*/
        "occil",          /* name of an environment variable to parse, or 0 */
        "occil",          /* name of the program, for usage */
        "occil",          /* name of a config file if you want to use one, or NULL (sans extension) */
        usage_text,       /* pointer to usage text */
        help_text,        /* pointer to help text */
        "",               /* lib file */
        "msil-w32-occil", /* machine string */
        nullptr,          // args,                           /* extra args */
        0,                // sizeof(args) / sizeof(args[0]), /* number of args */
        0,                // prockeywords,                   /* specific keywords, e.g. allow a 'bit' keyword and so forth */
        defines,          /* defines list to create at compile time, or null */
        &dbgStruct[0],    /* debug structure, or NULL */
        &architecture_characteristics, /* architecture characteristics */
        &outputfunctions,              /* pointer to backend function linkages */
#if 0
            & msilData,                      /* pointer to MSIL-specific data and functions */
#endif
        msil_bltins, /* pointer to extra builtin data */
        initnasm,    /* return 1 to proceed */
#if 0
            oa_main_preprocess,             /* precompile function, or NULL */
            oa_main_postprocess,            /* postcompile function, or NULL */
            RunExternalFiles,               /* postprocess function, or NULL */
            0,                              /* compiler rundown */
            InsertOutputFileName,           /* insert the output (executable name) into the backend */
            InsertExternalFile,             /* insert a non-compilable file in the backend list, e.g. for post processing, or NULL */
            parOptimizer::se_param,                    /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
            parOptimizer::se_codegen,                  /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
            parOptimizer::se_pragma,                   /* parse a pragma directive, or null */
            compile_start,                  /* signal start of compile on a per file basis */
            include_start,                  /* signal switching to a new source file */
            NULL,                           /* write the object file (for native object formats) */
            NULL,                           /* initialize the object file */
            NULL,                           /* initialize inline assembler, per file, or NULL */
            NULL,                           /* parse an assembly statement, or NULL */
            NULL,                           /* translate an assembly instruction which was inlined */
            0,                              /* initialize intrinsic mechanism, compiler startup */
            0,                              /* search for an intrinsic */
            oa_enter_type,                  /* enter a type in the BE */
#endif
    },
    {0}};

int parse_msil_codegen(bool v, const char* string)
{
    if (string[0] == ';')
        return 1;
    if (string[0] == 'f')
        pinning = v;
    else if (string[0] == 's')
        msilstrings = v;
    else if (string[0] == 'a')
        delegateforfuncptr = v;
    else if (string[0] == 'I')
        initializeScalars = v;
    else
        return 0;
    return 1;
}
void msilWinmodeSetup(const char* string)
{
    switch (string[0])
    {
        case 'd':
            cparams.prm_targettype = DLL;
            defines[0].respect = true;
            defines[2].respect = false;
            defines[3].respect = true;
            defines[6].respect = false;
            break;
        case 'c':
            cparams.prm_targettype = CONSOLE;
            defines[0].respect = true;
            defines[2].respect = false;
            defines[3].respect = true;
            defines[6].respect = false;
            break;
        case 'g':
            cparams.prm_targettype = GUI;
            defines[0].respect = true;
            defines[2].respect = false;
            defines[3].respect = true;
            defines[6].respect = false;
            break;
        default:
            Utils::fatal("Invalid executable type");
            break;
    }
    if (string[1] == 'l')
    {
        defines[1].respect = true;
        defines[4].respect = false;
        pinvoke_dll = "lscrtl.dll";
    }
    else if (string[1] == 'm')
    {
        defines[1].respect = false;
        defines[4].respect = true;
        pinvoke_dll = "msvcrt.dll";
    }
    else if (string[1] == 'M')
    {
        cparams.managed_library = true;
        defines[4].respect = false;
        defines[7].respect = true;
    }
    if (string[2] == 'n')
        cparams.no_default_libs = true;
}

}  // namespace Optimizer