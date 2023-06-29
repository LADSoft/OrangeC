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
#include "Utils.h"
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "../occ/x86regs.h"
#include "../occ/be.h"
#include "../occparse/winmode.h"
#include "config.h"
#include "irc.h"
#include "optmodules.h"

extern int usingEsp;
namespace Optimizer
{

bool doBackendInit = false;

static char BackendIntrinsicPrototypes[] =
#include "../occ/beIntrinsicProtos.h"
    ;
static char help_text[] =
    "[options] [@response file] files\n"
    "\n"
    "/1        - C1x mode                  /8        - c89 mode\n"
    "/9        - C99 mode                  /#        - compile then assemble then link\n"
    "/axxx     - set assembler extension   /c        - compile only\n"
    "+e        - dump errors to file       /f{flags} - set flags\n"
    "/g        - enable debug symbols      +i        - dump preprocessed file\n"
    "/lxxx     - include library           /oxxx     - specify output file name\n"
    "/pxxx     - pass command to tool      /snn      - align stack\n"
    "/t        - display timing info       /wxxx     - warning control\n"
    "/x xxx    - specify language          /y[...]   - verbosity\n"
    "/z        - set SYSTEM include path\n"
    "+A        - disable extensions        /Dxxx     - define something\n"
    "/E[+]nn   - max number of errors      /Ipath    - specify include path\n"
    "/Lxxx     - set library path          /M        - generate make stubs\n"
    "/Ox        - optimization control     /P        - set pipe for retrieving input files\n"
    "+Q        - quiet mode                /Sasm;dbg - make ASM source file\n"
    "/T        - translate trigraphs       /Uxxx     - undefine something\n"
    "/V,/v,--version - show version/date   /Wxx      - set executable type\n"
    "/X-       - don't include RTTI info   /Y        - output intermediate code\n"
    "/!,--nologo - No logo\n"
    "/?, --help  - This text\n"
    "\nCodegen parameters: (/C[+][-][params])\n"
    "  +d   - display diagnostics          -b        - no BSS\n"
    "  +F   - flat model                   -l        - no C source in ASM file\n"
    "  -m   - no leading underscores       +r        - reverse order of bit ops\n"
    "  +R   - honor FAR keyword            +u        - 'char' type is unsigned\n"
    "\nPassthrough behaviors:\n"
    "  filename extensions are used to determine what tool a file needs to be processed\n"
    "     supported tools:  compiler, assembler, resource compiler, linker\n"
    "  /paxxx    pass switch to assembler\n"
    "  /plxxx    pass switch to linker\n"
    "  /prxxx    pass switch to resource compiler\n"
    "   note that /lxxx passes the library on to the linker.\n"
    "\nWarning Control:\n"
    " /w      - display no warnings         /wx            - display warnings as errors\n"
    " /w+     - display all warnings\n"
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
    "\nExecutable Mode:\n"
    "  /Wxy\n"
    "  x is executable type               y is library type\n"
    "    c - console                         empty = static lib   l = lscrtl.dll\n"
    "    d - dll                             c = crtdll.dll\n"
    "    w - windowing                       m = msvcrtdll.dll\n"
    "\n"
    "\nOptimization control:\n" OPTIMIZATION_DESCRIPTION "\nFlags:\n" OPTMODULES_DESCRIPTION
    "  -fsyntax-only                  compile only, don't produce an output file\n"
    "  -funsigned-char                'char' type is unsigned\n"
    " -std=xxxx                       specify the language standard to use\n"
    "    note for C++, this only changes the definition of __cplusplus\n"
    "    it doesn't check for conformity to the standard or deprecate things properly\n"
    "    valid standards are:\n"
    "       c89   1989 version of ansi c\n"
    "       c99   1999 version of ansi c\n"
    "       c11   2011 version of ansi c\n"
    "       c++11 2011 version of C++\n"
    "       c++14 2014 version of C++\n"
    " -nostdinc, nostdinc++           disable system include file path\n"
    " --output-def-file filename      output a .def file instead of a .lib file for DLLs\n"
    " --export-all-symbols            reserved\n"
    " -link                           reserved\n"
    " -dll or -shared                 compile as DLL (same as /Wd)\n"
    " -static                         build a static library\n"
    " -dumpversion                    print(to stdout) the version of the compiler\n"
    " -dumpmachine                    print(to stdout) a representation of the target architecture\n"
    " -print-file-name=xxx            print(to stdout) the runtime library file path\n"
    " -print-prog-name=xxx            print(to stdout) the executable path for the xxx executable\n"
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
static char usage_text[] = "[options] [@response file] files";

void WinmodeSetup(const char* string);
static void chkdos(void)
{
    if (doBackendInit)
    {
        char* a = getenv("ORANGECDOS");
        if (a && *a)
        {
            WinmodeSetup("a");  // DOS32A
            cparams.prm_useesp = false;
            cparams.compile_under_dos = true;
        }
        else
        {
            WinmodeSetup("c");  // Windows console
        }
    }
}
static int initoasm(COMPILER_PARAMS* parms, ARCH_ASM* data, ARCH_DEBUG* debug)
{
    (void)parms;
    (void)data;
    (void)debug;
    cparams.prm_assembler = pa_oasm;
    chkdos();
    return 1;
}
static int initnasm(COMPILER_PARAMS* parms, ARCH_ASM* data, ARCH_DEBUG* debug)
{
    (void)parms;
    (void)data;
    (void)debug;
    cparams.prm_assembler = pa_nasm;
    chkdos();
    return 1;
}
static int initfasm(COMPILER_PARAMS* parms, ARCH_ASM* data, ARCH_DEBUG* debug)
{
    (void)parms;
    (void)data;
    (void)debug;
    cparams.prm_assembler = pa_fasm;
    chkdos();
    return 1;
}
static int initgeneric(COMPILER_PARAMS* parms, ARCH_ASM* data, ARCH_DEBUG* debug)
{
    (void)parms;
    (void)data;
    (void)debug;
    initnasm(parms, data, debug);
    cparams.prm_nodos = true;
    chkdos();
    return 1;
}
static int inittasm(COMPILER_PARAMS* parms, ARCH_ASM* data, ARCH_DEBUG* debug)
{
    (void)parms;
    (void)data;
    (void)debug;
    cparams.prm_assembler = pa_tasm;
    chkdos();
    return 1;
}
static int initmasm(COMPILER_PARAMS* parms, ARCH_ASM* data, ARCH_DEBUG* debug)
{
    (void)parms;
    (void)data;
    (void)debug;
    cparams.prm_assembler = pa_masm;
    chkdos();
    return 1;
}

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
    {"__SEH__", "1", true, true},
    {"__386__", "1", true, true},
    {"__i386__", "1", true, true},
    {"_i386_", "1", true, true},
    {"__i386", "1", true, true},
    {"_X86_", "1", true, true},
    {"_M_IX86", "1", true, true},
    {"_INTEGRAL_MAX_BITS", "64", true, true},
    { 0 }
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
    0, /* char a_struct;  */ /* alignment only */
    4,                    /*char a_float;*/
    8,                    /*char a_double;*/
    8,                    /*char a_longdouble;*/
    0,                    /*char a_fcomplexpad;*/
    0,                    /*char a_rcomplexpad;*/
    0,                    /*char a_lrcomplexpad;*/
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
    0,                    /*char a_fcomplexpad;*/
    0,                    /*char a_rcomplexpad;*/
    0,                    /*char a_lrcomplexpad;*/
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
    0, /*char a_longlong; */
    0, /*char a_char16_t; */
    0, /*char a_char32_t; */
    0, /*char a_addr; */
    1, /*char a_farptr; */
    1, /*char a_farseg; */
    1, /*char a_memberptr; */
    1, /*char a_struct; */
    /* alignment only */
    0,  /*char a_float; */
    0,  /*char a_double; */
    0,  /*char a_longdouble; */
    1,  /*char a_fcomplexpad; */
    1,  /*char a_rcomplexpad; */
    1,  /*char a_lrcomplexpad; */
    0,  // char a_alignedstruct; // __attribute((__aligned__))
};
static ARCH_FLOAT aflt = {-126, 126, 128, 24};
static ARCH_FLOAT adbl = {-1022, 1022, 1024, 53};
static ARCH_FLOAT aldbl = {-1022, 1022, 1024, 53};
// static ARCH_FLOAT aldbl = {-16382, 16382, 16384, 64};
static ARCH_PEEP peeps[] = {0};
static char fastcallRegs[3] = {R_ECX, R_EDX, R_EAX};
static ARCH_CHARACTERISTICS architecture_characteristics = {
    &alignments, /* alignments */
    0,           /* custom alignment routine */
    &sizes,      /* sizes */
    &locks,      /* atomic locks */
    8,           /* maximum value for __c11_atomic_is_lock_free */
    0,           /* routine is called in case parameters less than paramwidth need offsets */
    ISZ_ULONG,   /* size compatible to an integer */
    ISZ_ULONG,   /* size compatible to an address */
    ISZ_UINT,    /* size compatible with the register width */
    8,           /* default packing level */
    8,           /* alignment of malloc() out of RTL */
    /* floating point characteristics not fully implemented */
    &aflt,                                                                       /* characteristics of 'float' keyword */
    &adbl,                                                                       /* characteristics of 'double' keyword */
    &aldbl,                                                                      /* characteristics of 'long double' keyword */
    0,                                                                           /* future floating type */
    REG_MAX,                                                                     /* register count */
    &regNames[0],                                                                /* defines registers */
    1,                                                                           /* register trees count */
    &regRoot, (ARCH_REGCLASS**)regClasses, &regCosts, allocOrder, regmap, peeps, /* defines peephole information */
    sizeof(fastcallRegs) / sizeof(fastcallRegs[0]),                              /* Max number of regs considered for fastcall */
    fastcallRegs,                                                                /* register list for regs used in fastcall */
    OPT_BYTECOMPARE,                                                             /* preferred optimizations */
    0,                                                                           /* optimizations we don't want */
    0,                                                                           /* error options */
    true,                                                                        /* true if has floating point regs */
    0,                                                                           /* floating point modes, not honored currently */
    ABM_USESIZE,                                                                 /* bool is determined by sizing above */
    ARM_FUNCTIONSCOPE,                                                           /* register allocation across entire function */
    8,     /* number of bits in a MAU.  values other than 8 not supported */
    4,                   /* natural size for this architecture, e.g. 4 for 32 bit arches and 8 for 64 bits */
    false, /* little endian */
    false, /* normal bit allocation*/
    false, /* locals in stack memory*/
    false, /* stack pointer grows down */
    false, /* preallocate locals */
    4,     /* size of a return block on stack (e.g. function ret addr & frame ptr) */
    4,     /* minimium width/ padding of passed parameters in maus */
    4,     /* minimum stack alignment */
    false, /* library functions should bes genned as import calls */
    4,     /* ret block param adjust for RTTI */
};
static ARCH_DEBUG dbgStruct[1];
#if 0
    = { {
                                  "LS",                /* name of debug format */
                                  0,                   /* backend specific data, compiler ignores */
                                  &dbgblocknum,        /* pointer to variable which holds block number, or zero for no blocking */
                                  dbginit,             /* per file initialization */
                                  0,                   /* per file rundown */
                                  debug_outputtypedef, /* output a (global) typedef */
                                  /* browser funcs */
                                  0,               /* per file initialization */
                                  dump_browsedata, /* put browse info somewhere */
                                  dump_browsefile, /* put browse file list somewhere */
                                  0,               /* per file rundown */
                              },
                              {0}
    };
#endif
static ARCH_GEN outputfunctions = {
#if 0
        oa_header,            /* generate assembly language header */
        oa_trailer,           /* generate assembly language trailer */
        oa_adjust_codelab,    /* adjust an assembly language statement for the relative code labels */
        nullptr,              /* allow access to the quad list prior to GCSE */
        examine_icode,        /* allow access to the quad list after GCSE */
        flush_peep,           /* called after function body is generated */
        oa_end_generation,    /* end of code generation */
        cg_internal_conflict, /* internal conflict */
        preRegAlloc,          /* rewrites to improve register allocation */
        precolor,             /* precolor routine */
        oa_gen_strlab,        /* generate a named label */
        oa_put_label,         /* generate a numbered label */
        oa_put_string_label,  /* generate a numbered label */
        nullptr,              /* reserve space for a bit */
        oa_genint,            /* initialize an int */
        oa_genfloat,          /* initialize a float */
        oa_genaddress,        /* initializae a pointer */
        oa_genstring,         /* initialize a string */
        oa_genref,            /* put a reference to a variable */
        oa_genpcref,          /* put a reference to something in the code segment */
        oa_gensrref,          /* put a reference to the startup.rundown */
        oa_gen_labref,        /* put a reference to a label */
        oa_gen_labdifref,     /* put the difference of two labels */
        oa_gen_virtual,       /* start a virtual segment */
        oa_gen_endvirtual,    /* end a virtual segment */
        oa_gen_vtt,           /* do a VT thunk entry */
        oa_gen_vc1,           /* do a vc1 thunk entry */
        oa_gen_importThunk,   /* do an import thunk entry */
        oa_genstorage,        /* generate uninitialized storage */
        oa_align,             /* put an alignment command */
        oa_setalign,          /* put alignment sizes */
        oa_enterseg,          /* switch to new seg */
        oa_exitseg,           /* exit current segment */
        oa_globaldef,         /* put a global definition */
        oa_localdef,          /* put a local definition */
        oa_localstaticdef,    /* put a local static definition */
        oa_put_extern,        /* put an external definition */
        oa_put_impfunc,       /* put an import definition */
        oa_put_expfunc,       /* put an export definition */
        oa_output_alias,      /* put an alias */
        oa_output_includelib, /* put an included library name */
        0,                    /* backend handle intrinsic */
        0,                    /* expression tag */
        0,                    /* tag */
        asm_line,             /* line number information and text */
        asm_blockstart,       /* block start */
        asm_blockend,         /* block end */
        asm_varstart,         /* var start */
        asm_func,             /* func start or end */
        asm_passthrough,      /* reserved */
        asm_datapassthrough,  /* reserved */
        asm_label,            /* put a label in the code stream */
        asm_atomic,           /* atomic primitives */
        asm_goto,             /* unconditional branch */
        asm_gosub,            /* normal gosub to an immediate label or through a var */
        asm_fargosub,         /* far version of gosub */
        asm_trap,             /* 'trap' instruction - the arg will be an immediate # */
        asm_int,              /* 'int' instruction, calls a labeled function which is an interrupt */
        asm_ret,              /* return from subroutine */
        asm_fret,             /* far return from subroutine */
        asm_rett,             /* return from trap or int */
        asm_add,              /* evaluate an addition */
        asm_sub,              /* evaluate a subtraction */
        asm_udiv,             /* unsigned division */
        asm_umod,             /* unsigned modulous */
        asm_sdiv,             /* signed division */
        asm_smod,             /* signed modulous */
        asm_muluh,            /* multiply highword unsigned*/
        asm_mulsh,            /* multiply highword signed*/
        asm_mul,              /* multiply */
        asm_lsl,              /* unsigned shift left */
        asm_lsr,              /* unsigned shift right */
        asm_asr,              /* signed shift right */
        asm_neg,              /* negation */
        asm_not,              /* complement */
        asm_and,              /* binary and */
        asm_or,               /* binary or */
        asm_eor,              /* binary exclusive or */
        asm_setne,            /* evaluate a = b != c */
        asm_sete,             /* evaluate a = b == c */
        asm_setc,             /* evaluate a = b U< c */
        asm_seta,             /* evaluate a = b U> c */
        asm_setnc,            /* evaluate a = b U>= c */
        asm_setbe,            /* evaluate a = b U<= c */
        asm_setl,             /* evaluate a = b S< c */
        asm_setg,             /* evaluate a = b s> c */
        asm_setle,            /* evaluate a = b S<= c */
        asm_setge,            /* evaluate a = b S>= c */
        asm_assn,             /* assignment */
        asm_genword,          /* put a byte or word into the code stream */
        asm_coswitch,         /* switch characteristics */
        asm_swbranch,         /* case characteristics */
        asm_dc,               /* unused */
        asm_assnblock,        /* copy block of memory*/
        asm_clrblock,         /* clear block of memory */
        asm_cmpblock,         /* branch if two blocks of memory aren't equal */
        asm_jc,               /* branch if a U< b */
        asm_ja,               /* branch if a U> b */
        asm_je,               /* branch if a == b */
        asm_jnc,              /* branch if a U>= b */
        asm_jbe,              /* branch if a U<= b */
        asm_jne,              /* branch if a != b */
        asm_jl,               /* branch if a S< b */
        asm_jg,               /* branch if a S> b */
        asm_jle,              /* branch if a S<= b */
        asm_jge,              /* branch if a S>= b */
        asm_parm,             /* push a parameter*/
        asm_parmadj,          /* adjust stack after function call */
        asm_parmblock,        /* push a block of memory */
        asm_cppini,           /* cplusplus initialization (historic)*/
        asm_prologue,         /* function prologue */
        asm_epilogue,         /* function epilogue */
        asm_pushcontext,      /* push register context */
        asm_popcontext,       /* pop register context */
        asm_loadcontext,      /* load register context (e.g. at interrupt level ) */
        asm_unloadcontext,    /* unload register context (e.g. at interrupt level ) */
        asm_tryblock,         /* try, catch, end block */
        asm_stackalloc,       /* allocate stack space - positive value = allocate, negative value deallocate */
        asm_loadstack,        /* load the stack pointer from a var */
        asm_savestack,        /* save the stack pointer to a var */
        nullptr,              /* seh */
        asm_functail,         /* function tail (e.g. destructor) start/end */
#endif
};
ARCH_ASM x86AssemblerInterface[] = {
    {
        "oasm",                                     /* assembler name */
        0,                                          /* backend data (compiler ignores) */
        "1",                                        /* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
        ".s",                                       /* extension for assembly files */
        ".o",                                       /* extension for object files, nullptr = has no object mode */
        ".l;.lib;.a;.rc;.res;.o;.asm;.nas;.s;.dll", /* extensions for files that should be passed to the backend*/
        "occ",                                      /* name of an environment variable to parse, or 0 */
        "occ",                                      /* name of the program, for usage */
        "occ",                                      /* name of a config file if you want to use one, or nullptr (sans extension) */
        usage_text,                                 /* pointer to usage text */
        help_text,                                  /* pointer to help text */
        "clwin.l",                                  /* lib file */
        "i686-w32-occ",                             /* machine string */
        nullptr,                                    // args,                           /* extra args */
        0,                                          // sizeof(args) / sizeof(args[0]), /* number of args */
        nullptr,                                    /* specific keywords, e.g. allow a 'bit' keyword and so forth */
        defines,                                    /* defines list to create at compile time, or null */
        &dbgStruct[0],                              /* debug structure, or nullptr */
        &architecture_characteristics,              /* architecture characteristics */
        &outputfunctions,                           /* pointer to backend function linkages */
#if 0
            nullptr,                        /* pointer to MSIL-specific data and functions */
#endif
        BackendIntrinsicPrototypes, /* pointer to extra builtin data */
        initoasm,                   /* return 1 to proceed */
#if 0
            0,                              /* precompile function, or nullptr */
            0,                              /* postcompile function, or nullptr */
            RunExternalFiles,               /* postprocess function, or nullptr */
            0,                              /* compiler rundown */
            InsertOutputFileName,           /* insert the output (executable name) into the backend */
            InsertExternalFile,             /* insert a non-compilable file in the backend list, e.g. for post processing, or nullptr */
            parOptimizer::se_param,                    /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
            parOptimizer::se_codegen,                  /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
            0,                              /* parse a pragma directive, or null */
            compile_start,                  /* signal start of compile on a per file basis */
            include_start,                  /* signal switching to a new source file */
            output_obj_file,                /* write the object file (for native object formats) */
            outcode_file_init,              /* initialize the object file */
            inasmini,                       /* initialize inline assembler, per file, or nullptr */
            inasm_statement,                /* parse an assembly statement, or nullptr */
            inlineAsmStmt,                  /* translate an assembly instruction which was inlined */
            0,                              /* initialize intrinsic mechanism, compiler startup */
            0,                              /* search for an intrinsic */
            0,                              /* enter a type in the BE */
            get_dll_linkage,                /* get dll linkage corresponding to command line switches */
#endif
    },
    {
        "nasm",                                /* assembler name */
        0,                                     /* backend data (compiler ignores) */
        "1",                                   /* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
        ".asm",                                /* extension for assembly files */
        ".o",                                  /* extension for object files, nullptr = has no object mode */
        ".l;.lib;.a;.rc;.res;.o;.asm;.nas;.s", /* extensions for files that should be passed to the backend*/
        "occ",                                 /* name of an environment variable to parse, or 0 */
        "occ",                                 /* name of the program, for usage */
        "occ",                                 /* name of a config file if you want to use one, or nullptr (sans extension) */
        usage_text,                            /* pointer to usage text */
        help_text,                                  /* pointer to help text */
        "clwin.l",                             /* lib file */
        "i686-w32-occ",                        /* machine string */
        nullptr,                               // args,                           /* extra args */
        0,                                     // sizeof(args) / sizeof(args[0]), /* number of args */
        nullptr,                               /* specific keywords, e.g. allow a 'bit' keyword and so forth */
        defines,                               /* defines list to create at compile time, or null */
        &dbgStruct[0],                         /* debug structure, or nullptr */
        &architecture_characteristics,         /* architecture characteristics */
        &outputfunctions,                      /* pointer to backend function linkages */
#if 0
            nullptr,                        /* pointer to MSIL-specific data and functions */
#endif
        BackendIntrinsicPrototypes, /* pointer to extra builtin data */
        initnasm,                   /* return 1 to proceed */
#if 0
            0,                              /* precompile function, or nullptr */
            0,                              /* postcompile function, or nullptr */
            RunExternalFiles,               /* postprocess function, or nullptr */
            0,                              /* compiler rundown */
            InsertOutputFileName,           /* insert the output (executable name) into the backend */
            InsertExternalFile,             /* insert a non-compilable file in the backend list, e.g. for post processing, or nullptr */
            parOptimizer::se_param,                    /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
            parOptimizer::se_codegen,                  /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
            0,                              /* parse a pragma directive, or null */
            compile_start,                  /* signal start of compile on a per file basis */
            include_start,                  /* signal switching to a new source file */
            output_obj_file,                /* write the object file (for native object formats) */
            outcode_file_init,              /* initialize the object file */
            inasmini,                       /* initialize inline assembler, per file, or nullptr */
            inasm_statement,                /* parse an assembly statement, or nullptr */
            inlineAsmStmt,                  /* translate an assembly instruction which was inlined */
            0,                              /* initialize intrinsic mechanism, compiler startup */
            0,                              /* search for an intrinsic */
            0,                              /* enter a type in the BE */
            get_dll_linkage,                /* get dll linkage corresponding to command line switches */
#endif
    },
    {
        "fasm",                                /* assembler name */
        0,                                     /* backend data (compiler ignores) */
        "1",                                   /* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
        ".asm",                                /* extension for assembly files */
        ".o",                                  /* extension for object files, nullptr = has no object mode */
        ".l;.lib;.a;.rc;.res;.o;.asm;.nas;.s", /* extensions for files that should be passed to the backend*/
        "occ",                                 /* name of an environment variable to parse, or 0 */
        "occ",                                 /* name of the program, for usage */
        "occ",                                 /* name of a config file if you want to use one, or nullptr (sans extension) */
        usage_text,                            /* pointer to usage text */
        help_text,                                  /* pointer to help text */
        "clwin.l",                             /* lib file */
        "i686-w32-occ",                        /* machine string */
        nullptr,                               // args,                           /* extra args */
        0,                                     // sizeof(args) / sizeof(args[0]), /* number of args */
        nullptr,                               /* specific keywords, e.g. allow a 'bit' keyword and so forth */
        defines,                               /* defines list to create at compile time, or null */
        &dbgStruct[0],                         /* debug structure, or nullptr */
        &architecture_characteristics,         /* architecture characteristics */
        &outputfunctions,                      /* pointer to backend function linkages */
#if 0
            nullptr,                        /* pointer to MSIL-specific data and functions */
#endif
        BackendIntrinsicPrototypes, /* pointer to extra builtin data */
        initfasm,                   /* return 1 to proceed */
#if 0
            0,                              /* precompile function, or nullptr */
            0,                              /* postcompile function, or nullptr */
            RunExternalFiles,               /* postprocess function, or nullptr */
            0,                              /* compiler rundown */
            InsertOutputFileName,           /* insert the output (executable name) into the backend */
            InsertExternalFile,             /* insert a non-compilable file in the backend list, e.g. for post processing, or nullptr */
            parOptimizer::se_param,                    /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
            parOptimizer::se_codegen,                  /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
            0,                              /* parse a pragma directive, or null */
            compile_start,                  /* signal start of compile on a per file basis */
            include_start,                  /* signal switching to a new source file */
            output_obj_file,                /* write the object file (for native object formats) */
            outcode_file_init,              /* initialize the object file */
            inasmini,                       /* initialize inline assembler, per file, or nullptr */
            inasm_statement,                /* parse an assembly statement, or nullptr */
            inlineAsmStmt,                  /* translate an assembly instruction which was inlined */
            0,                              /* initialize intrinsic mechanism, compiler startup */
            0,                              /* search for an intrinsic */
            0,                              /* __using__ declaration */
            get_dll_linkage,                /* get dll linkage corresponding to command line switches */
#endif
    },
    {
        "tasm",                                /* assembler name */
        0,                                     /* backend data (compiler ignores) */
        "1",                                   /* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
        ".asm",                                /* extension for assembly files */
        ".o",                                  /* extension for object files, nullptr = has no object mode */
        ".l;.lib;.a;.rc;.res;.o;.asm;.nas;.s", /* extensions for files that should be passed to the backend*/
        "occ",                                 /* name of an environment variable to parse, or 0 */
        "occ",                                 /* name of the program, for usage */
        "occ",                                 /* name of a config file if you want to use one, or nullptr (sans extension) */
        usage_text,                            /* pointer to usage text */
        help_text,                                  /* pointer to help text */
        "clwin.l",                             /* lib file */
        "i686-w32-occ",                        /* machine string */
        nullptr,                               // args,                           /* extra args */
        0,                                     // sizeof(args) / sizeof(args[0]), /* number of args */
        nullptr,                               /* specific keywords, e.g. allow a 'bit' keyword and so forth */
        defines,                               /* defines list to create at compile time, or null */
        &dbgStruct[0],                         /* debug structure, or nullptr */
        &architecture_characteristics,         /* architecture characteristics */
        &outputfunctions,                      /* pointer to backend function linkages */
#if 0
            nullptr,                        /* pointer to MSIL-specific data and functions */
#endif
        BackendIntrinsicPrototypes, /* pointer to extra builtin data */
        inittasm,                   /* return 1 to proceed */
#if 0
            0,                              /* precompile function, or nullptr */
            0,                              /* postcompile function, or nullptr */
            RunExternalFiles,               /* postprocess function, or nullptr */
            0,                              /* compiler rundown */
            InsertOutputFileName,           /* insert the output (executable name) into the backend */
            InsertExternalFile,             /* insert a non-compilable file in the backend list, e.g. for post processing, or nullptr */
            parOptimizer::se_param,                    /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
            parOptimizer::se_codegen,                  /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
            0,                              /* parse a pragma directive, or null */
            compile_start,                  /* signal start of compile on a per file basis */
            include_start,                  /* signal switching to a new source file */
            output_obj_file,                /* write the object file (for native object formats) */
            outcode_file_init,              /* initialize the object file */
            inasmini,                       /* initialize inline assembler, per file, or nullptr */
            inasm_statement,                /* parse an assembly statement, or nullptr */
            inlineAsmStmt,                  /* translate an assembly instruction which was inlined */
            0,                              /* initialize intrinsic mechanism, compiler startup */
            0,                              /* search for an intrinsic */
            0,                              /* __using__ declaration */
            get_dll_linkage,                /* get dll linkage corresponding to command line switches */
#endif
    },
    {
        "masm",                                /* assembler name */
        0,                                     /* backend data (compiler ignores) */
        "1",                                   /* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
        ".asm",                                /* extension for assembly files */
        ".o",                                  /* extension for object files, nullptr = has no object mode */
        ".l;.lib;.a;.rc;.res;.o;.asm;.nas;.s", /* extensions for files that should be passed to the backend*/
        "occ",                                 /* name of an environment variable to parse, or 0 */
        "occ",                                 /* name of the program, for usage */
        "occ",                                 /* name of a config file if you want to use one, or nullptr (sans extension) */
        usage_text,                            /* pointer to usage text */
        help_text,                                  /* pointer to help text */
        "clwin.l",                             /* lib file */
        "i686-w32-occ",                        /* machine string */
        nullptr,                               // args,                           /* extra args */
        0,                                     // sizeof(args) / sizeof(args[0]), /* number of args */
        nullptr,                               /* specific keywords, e.g. allow a 'bit' keyword and so forth */
        defines,                               /* defines list to create at compile time, or null */
        &dbgStruct[0],                         /* debug structure, or nullptr */
        &architecture_characteristics,         /* architecture characteristics */
        &outputfunctions,                      /* pointer to backend function linkages */
#if 0
            nullptr,                        /* pointer to MSIL-specific data and functions */
#endif
        BackendIntrinsicPrototypes, /* pointer to extra builtin data */
        initmasm,                   /* return 1 to proceed */
#if 0
            0,                              /* precompile function, or nullptr */
            0,                              /* postcompile function, or nullptr */
            RunExternalFiles,               /* postprocess function, or nullptr */
            0,                              /* compiler rundown */
            InsertOutputFileName,           /* insert the output (executable name) into the backend */
            InsertExternalFile,             /* insert a non-compilable file in the backend list, e.g. for post processing, or nullptr */
            parOptimizer::se_param,                    /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
            parOptimizer::se_codegen,                  /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
            0,                              /* parse a pragma directive, or null */
            compile_start,                  /* signal start of compile on a per file basis */
            include_start,                  /* signal switching to a new source file */
            output_obj_file,                /* write the object file (for native object formats) */
            outcode_file_init,              /* initialize the object file */
            inasmini,                       /* initialize inline assembler, per file, or nullptr */
            inasm_statement,                /* parse an assembly statement, or nullptr */
            inlineAsmStmt,                  /* translate an assembly instruction which was inlined */
            0,                              /* initialize intrinsic mechanism, compiler startup */
            0,                              /* search for an intrinsic */
            0,                              /* __using__ declaration */
            get_dll_linkage,                /* get dll linkage corresponding to command line switches */
#endif
    },
    {
        "generic",                             /* assembler name */
        0,                                     /* backend data (compiler ignores) */
        "1",                                   /* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
        ".asm",                                /* extension for assembly files */
        ".o",                                  /* extension for object files, nullptr = has no object mode */
        ".l;.lib;.a;.rc;.res;.o;.asm;.nas;.s", /* extensions for files that should be passed to the backend*/
        "occ",                                 /* name of an environment variable to parse, or 0 */
        "occ",                                 /* name of the program, for usage */
        "occ",                                 /* name of a config file if you want to use one, or nullptr (sans extension) */
        usage_text,                            /* pointer to usage text */
        help_text,                                  /* pointer to help text */
        "clwin.l",                             /* lib file */
        "i686-w32-occ",                        /* machine string */
        nullptr,                               // args,                           /* extra args */
        0,                                     // sizeof(args) / sizeof(args[0]), /* number of args */
        nullptr,                               /* specific keywords, e.g. allow a 'bit' keyword and so forth */
        defines,                               /* defines list to create at compile time, or null */
        &dbgStruct[0],                         /* debug structure, or nullptr */
        &architecture_characteristics,         /* architecture characteristics */
        &outputfunctions,                      /* pointer to backend function linkages */
#if 0
            nullptr,                        /* pointer to MSIL-specific data and functions */
#endif
        BackendIntrinsicPrototypes, /* pointer to extra builtin data */
        initgeneric,                /* return 1 to proceed */
#if 0
            0,                              /* precompile function, or nullptr */
            0,                              /* postcompile function, or nullptr */
            RunExternalFiles,               /* postprocess function, or nullptr */
            0,                              /* compiler rundown */
            InsertOutputFileName,           /* insert the output (executable name) into the backend */
            InsertExternalFile,             /* insert a non-compilable file in the backend list, e.g. for post processing, or nullptr */
            parOptimizer::se_param,                    /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
            parOptimizer::se_codegen,                  /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
            0,                              /* parse a pragma directive, or null */
            compile_start,                  /* signal start of compile on a per file basis */
            include_start,                  /* signal switching to a new source file */
            output_obj_file,                /* write the object file (for native object formats) */
            outcode_file_init,              /* initialize the object file */
            inasmini,                       /* initialize inline assembler, per file, or nullptr */
            inasm_statement,                /* parse an assembly statement, or nullptr */
            inlineAsmStmt,                  /* translate an assembly instruction which was inlined */
            0,                              /* initialize intrinsic mechanism, compiler startup */
            0,                              /* search for an intrinsic */
            0,                              /* __using__ declaration */
            get_dll_linkage,                /* get dll linkage corresponding to command line switches */
#endif
    },
    {0}};

void WinmodeSetup(const char* string)
{
    switch (string[0])
    {
        case 'r':
            cparams.prm_targettype = RAW;
            defines[3].respect = false;
            defines[2].respect = false;
            defines[0].respect = false;
            defines[6].respect = true;
            break;
        case 'a':
            cparams.prm_targettype = DOS32A;
            defines[3].respect = false;
            defines[2].respect = true;
            defines[0].respect = false;
            defines[6].respect = false;
            break;
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
        case 'e':
            cparams.prm_targettype = DOS;
            defines[3].respect = false;
            defines[2].respect = true;
            defines[0].respect = false;
            defines[6].respect = false;
            break;
        case 'h':
            cparams.prm_targettype = HXDOS;
            defines[3].respect = true;
            defines[2].respect = true;
            defines[0].respect = true;
            defines[6].respect = false;
            break;
        case 'x':
            cparams.prm_targettype = WHXDOS;
            defines[3].respect = false;
            defines[2].respect = true;
            defines[0].respect = false;
            defines[6].respect = false;
            break;
        default:
            Utils::fatal("Invalid executable type");
            break;
    }
    if (string[1] == 'm')
    {
        if (!defines[0].respect)
            Utils::fatal("Invalid use of LSCRTDLL");
        cparams.prm_msvcrt = true;
        defines[4].respect = true;
    }
    if (string[1] == 'c')
    {
        if (!defines[0].respect)
            Utils::fatal("Invalid use of CRTDLL");
        cparams.prm_crtdll = true;
        defines[5].respect = true;
    }
    else if (string[1] == 'l')
    {
        if (!defines[0].respect)
            Utils::fatal("Invalid use of LSCRTDLL");
        cparams.prm_lscrtdll = true;
        defines[1].respect = true;
        Optimizer::chosenAssembler->arch->libsasimports = true;
    }
}
int parse_codegen(char mode, const char* string)
{
    switch (string[0])
    {
        case 'X':
            cparams.prm_nodos = mode;
            break;
        case 'F':
            cparams.prm_flat = mode;
            break;
        case 'R':
            cparams.prm_farkeyword = mode;
            break;
        case 'E':
            cparams.prm_useesp = mode;
            break;
        case '?':
            cparams.prm_browse = mode;
            break;
        default:
            return 0; /* illegal */
    }
    return 1; /* eat one char */
}

void SetUsesESP(bool yes)
{
    int whichway = 0;
    if (yes && !usingEsp)
    {
        whichway = 1;
        usingEsp = true;
    }
    else if (!yes && usingEsp)
    {
        whichway = -1;
        usingEsp = false;
    }
    if (whichway)
    {
        pushedDoubleRegs.regCount += 2 * whichway;
        pushedDwordRegs.regCount += whichway;
        pushedWordRegs.regCount += whichway;
        allDoubleRegs.regCount += 2 * whichway;
        allDwordRegs.regCount += whichway;
        allWordRegs.regCount += whichway;
        regInit();
    }
}
}  // namespace Optimizer