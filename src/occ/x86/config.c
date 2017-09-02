/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and se of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
/*
 * C configuration when we are using ICODE code generator output
 */
#include <stdio.h>
#include <string.h>
#include "be.h"
#include "winmode.h"
#include "x86regs.h"
  
extern int dbgblocknum;
extern ASMNAME oplst[] ;
extern ASMREG reglst[] ;
    #ifndef WIN32
        int prm_targettype = DOS32A;
    #else 
        int prm_targettype = CONSOLE;
    #endif 
    int prm_bepeep = TRUE;
    int prm_crtdll = FALSE;
    int prm_lscrtdll = FALSE;
    int prm_msvcrt = FALSE;
    enum asmTypes prm_assembler;
    int prm_flat = FALSE ;
    int prm_nodos = FALSE ;
    int prm_useesp = TRUE;
    int usingEsp = FALSE;
static    char usage_text[] = "[options] [@response file] files\n"
    "\n""/1        - C1x mode                  /9        - C99 mode\n"
        "/c        - compile only              +e        - dump errors to file\n"
        "+i        - dump preprocessed file    +l        - dump listing file\n"
        "/oname    - specify output file name  /snn      - align stack\n"
        "+v        - enable debug symbols\n"
        "+A        - disable extensions        /Dxxx     - define something\n"
        "/E[+]nn   - max number of errors      /Ipath    - specify include path\n"
        "/M        - generate make stubs       /O-       - disable optimizations\n"
        "+Q        - quiet mode                /Sasm;dbg - make ASM source file\n"
        "/T        - translate trigraphs       /Wxx      - set executable type\n"
        "Codegen parameters: (/C[+][-][params])\n"
        "  +d   - display diagnostics          -b        - no BSS\n"
        "  +F   - flat model                   -l        - no C source in ASM file\n"
        "  -m   - no leading underscores       +r        - reverse order of bit ops\n"
        "  +R   - honor FAR keyword            +u        - 'char' type is unsigned\n"
        "Time: " __TIME__ "  Date: " __DATE__;

static int parse_param(char mode, char *string);
static CMDLIST args[] = 
{
    {
        'W', ARG_CONCATSTRING, (void (*)(char, char *))parse_param
    }
    ,
    {
        'P', ARG_CONCATSTRING, (void (*)(char, char *))parse_param
    }
    
} ;
static KEYWORD prockeywords[] = 
{
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
    {
        0, 0, 0
    }
};
static ARCH_DEFINES defines[] = { 
    /* must come first in this order */
    {"__WIN32__","1",FALSE, TRUE },
    {"__LSCRTL_DLL","1",FALSE, TRUE },
    {"__DOS__","1",FALSE, TRUE },
    {"_WIN32","1",FALSE, TRUE },
    {"__MSVCRT_DLL","1",FALSE, TRUE },
    {"__CRTDLL_DLL","1",FALSE, TRUE },
    {"__RAW_IMAGE__","1",FALSE, TRUE },
    /* end ordered */
    {"__386__","1",TRUE,TRUE },
    {"__i386__","1",TRUE,TRUE },
    {"_i386_","1",TRUE,TRUE },
    {"__i386","1",TRUE,TRUE },
    {0 },
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
    12, /*char a_memberptr;    */
    0, /* char a_struct;  */ /* alignment only */
    4, /*char a_float;*/
    8, /*char a_double;*/
    10, /*char a_longdouble;*/
    0,/*char a_fcomplexpad;*/
    0,/*char a_rcomplexpad;*/
    2,/*char a_lrcomplexpad;*/
} ;
static ARCH_SIZING alignments = {
    1,/*char a_bool;*/
    1,/*char a_char;*/
    2,/*char a_short;*/
    2,/*char a_wchar_t;*/
    4,/*char a_enum;*/
    4,/*char a_int;*/
    4,/*char a_long;*/
    8,/*char a_longlong;*/
    4,/*char a_addr;*/
    4,/*char a_farptr;*/
    2,/*char a_farseg;*/
    4,/*char a_memberptr;    */
    0,/*char a_struct;  alignment only */
    /* imaginary same as real */
    4,/*char a_float;*/
    8,/*char a_double;*/
    8,/*char a_longdouble;*/
} ;
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
    1, /*char a_struct; */  /* alignment only */
    1, /*char a_float; */
    1, /*char a_double; */
    1, /*char a_longdouble; */
    1,/*char a_fcomplexpad; */
    1,/*char a_rcomplexpad; */
    1,/*char a_lrcomplexpad; */
} ;
static ARCH_FLOAT aflt = {
    -126 , 126, 128, 24
} ;
static ARCH_FLOAT adbl = {
    -1022, 1022, 1024, 53
    } ;
static ARCH_FLOAT aldbl = {
    -16382, 16382, 16384, 64
} ;
static ARCH_PEEP peeps[] = { 0 } ;
static ARCH_CHARACTERISTICS architecture = {
    &alignments, /* alignments */
    0,  /* custom alignment routine */
    &sizes,    /* sizes */
    &locks,     /* atomic locks */
    0, /* routine is called in case parameters less than paramwidth need offsets */
    ISZ_ULONG,	/* size compatible to an integer */    
    ISZ_ULONG,	/* size compatible to an address */    
    8,				/* default packing level */
    8,              /* alignment of malloc() out of RTL */
    /* floating point characteristics not fully implemented */
    &aflt,      /* characteristics of 'float' keyword */
    &adbl,        /* characteristics of 'double' keyword */
    &aldbl,       /* characteristics of 'long double' keyword */
    0,   /* future floating type */
    REG_MAX,		/* register count */
    &regNames[0],  /* defines registers */
    1,              /* register trees count */
    &regRoot,
    (ARCH_REGCLASS**)regClasses,
    &regCosts,
    allocOrder,
    peeps,   /* defines peephole information */
    OPT_BYTECOMPARE, /* preferred optimizations */
    0,             /* optimizations we don't want */
    0,                /* error options */
    FALSE,			/* true if has floating point regs */
    0,            /* floating point modes, not honored currently */
    ABM_USESIZE,  /* BOOLEAN is determined by sizing above */
    ARM_FUNCTIONSCOPE, /* register allocation across entire function */
    8,            /* number of bits in a MAU.  values other than 8 not supported */
    FALSE,        /* little endian */
    FALSE,        /* normal bit allocation*/
    FALSE,        /* locals in stack memory*/
    FALSE,        /* stack pointer grows down */
    FALSE,        /* preallocate locals */
    8,            /* size of a return block on stack (e.g. function ret addr & frame ptr) */
    4,            /* minimium width/ padding of passed parameters in maus */
    4,            /* minimum stack alignment */
    FALSE,		  /* library functions should bes genned as import calls */
} ;
extern ARCH_GEN outputfunctions ;
ARCH_GEN outputfunctions ;

static void WinmodeSetup(char select, char *string);
static void chkdos(void)
{
    char *a = getenv("ORANGECDOS");
    if (a && *a)
    {
        WinmodeSetup(0, "a"); // DOS32A
        prm_useesp = FALSE;
    }
    else
    {
        WinmodeSetup(0, "c"); // Windows console
    }
}
static int initnasm(COMPILER_PARAMS *parms, ARCH_ASM *data, ARCH_DEBUG *debug)
{
    (void)parms;
    (void)data;
    (void)debug;
    prm_assembler = pa_nasm;
    chkdos();
    return 1;
}
static int initfasm(COMPILER_PARAMS *parms, ARCH_ASM *data, ARCH_DEBUG *debug)
{
    (void)parms;
    (void)data;
    (void)debug;
    prm_assembler = pa_fasm;
    chkdos();
    return 1;
}
static int initgeneric(COMPILER_PARAMS *parms, ARCH_ASM *data, ARCH_DEBUG *debug)
{
    (void)parms;
    (void)data;
    (void)debug;
    initnasm(parms, data, debug);
    prm_nodos = TRUE;
    chkdos();
    return 1;
}
static int inittasm(COMPILER_PARAMS *parms, ARCH_ASM *data, ARCH_DEBUG *debug)
{
    (void)parms;
    (void)data;
    (void)debug;
    prm_assembler = pa_tasm;
    chkdos();
    return 1;
}
static int initmasm(COMPILER_PARAMS *parms, ARCH_ASM *data, ARCH_DEBUG *debug)
{
    (void)parms;
    (void)data;
    (void)debug;
    prm_assembler = pa_masm;
    chkdos();
    return 1;
}
    static void WinmodeSetup(char select, char *string)
    {
        (void)select;
        switch (string[0])
        {
            case 'r':
                prm_targettype = RAW;
                defines[3].respect = FALSE;
                defines[2].respect = FALSE;
                defines[0].respect = FALSE;
                defines[6].respect = TRUE;
                break;
            case 'a':
                prm_targettype = DOS32A;
                defines[3].respect = FALSE;
                defines[2].respect = TRUE;
                defines[0].respect = FALSE;
                defines[6].respect = FALSE;
                break;
            case 'd':
                prm_targettype = DLL;
                defines[0].respect = TRUE;
                defines[2].respect = FALSE;
                defines[3].respect = TRUE;
                defines[6].respect = FALSE;
                break;
            case 'c':
                prm_targettype = CONSOLE;
                defines[0].respect = TRUE;
                defines[2].respect = FALSE;
                defines[3].respect = TRUE;
                defines[6].respect = FALSE;
                break;
            case 'g':
                prm_targettype = GUI;
                defines[0].respect = TRUE;
                defines[2].respect = FALSE;
                defines[3].respect = TRUE;
                defines[6].respect = FALSE;
                break;
            case 'e':
                prm_targettype = DOS;
                defines[3].respect = FALSE;
                defines[2].respect = TRUE;
                defines[0].respect = FALSE;
                defines[6].respect = FALSE;
                break;
            case 'h':
                prm_targettype = HXDOS;
                defines[3].respect = TRUE;
                defines[2].respect = TRUE;
                defines[0].respect = TRUE;
                defines[6].respect = FALSE;
                break;
            case 'x':
                prm_targettype = WHXDOS;
                defines[3].respect = FALSE;
                defines[2].respect = TRUE;
                defines[0].respect = FALSE;
                defines[6].respect = FALSE;
                break;
            default:
                fatal("Invalid executable type");
        }
        if (string[1] == 'm')
        {
            if (!defines[0].respect)
                fatal("Invalid use of LSCRTDLL");
            prm_msvcrt = TRUE;
            defines[4].respect = TRUE;
        }
        if (string[1] == 'c')
        {
            if (!defines[0].respect)
                fatal("Invalid use of CRTDLL");
            prm_crtdll = TRUE;
            defines[5].respect = TRUE;
        } else if (string[1] == 'l') {
            if (!defines[0].respect)
                fatal("Invalid use of LSCRTDLL");
            prm_lscrtdll = TRUE;
            defines[1].respect = TRUE;
            architecture.libsasimports = TRUE;
        }
    }
static int parse_param(char select, char *string)
{
    if (select == 'W') {
        WinmodeSetup(select, string);
    }
    else if (select == 'P')
        prm_bepeep = FALSE;
    return 0;
}
static int parse_codegen(char mode, char *string)
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
        case 'E':
            prm_useesp = mode;
            break;
        case '?':
            cparams.prm_browse = mode;
            break;
        default:
            return 0; /* illegal */
    }
    return 1; /* eat one char */
}
void SetUsesESP(BOOLEAN yes)
{
    int whichway = 0;
    if (yes && !usingEsp)
    {
        whichway = 1 ;
        usingEsp = TRUE;
    }
    else if (!yes && usingEsp)
    {
       whichway = -1;
       usingEsp = FALSE;
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
void adjustUsesESP()
{
    if (cparams.prm_debug || cparams.prm_stackalign || !cparams.prm_optimize_for_speed)
    {
        prm_useesp = FALSE;
    }
    else
    {
        if (prm_useesp)
            architecture.retblocksize = 4;
    }
    SetUsesESP(prm_useesp);
}
ARCH_DEBUG dbgStruct [] = {
    {
    "LS",                       /* name of debug format */
    0,                          /* backend specific data, compiler ignores */
    &dbgblocknum,               /* pointer to variable which holds block number, or zero for no blocking */
    dbginit,                    /* per file initialization */
    0,                          /* per file rundown */
    debug_outputtypedef,        /* output a (global) typedef */
    /* browser funcs */
    0,                          /* per file initialization */
    dump_browsedata,    /* put browse info somewhere */
    dump_browsefile,    /* put browse file list somewhere */
    0,                          /* per file rundown */
    },
    { 0 }

} ;
ARCH_GEN outputfunctions = {
    oa_header,             /* generate assembly language header */
    oa_trailer,            /* generate assembly language trailer */
    oa_adjust_codelab,   /* adjust an assembly language statement for the relative code labels */
    NULL, 			         /* allow access to the quad list prior to GCSE */
    examine_icode,                   /* allow access to the quad list after GCSE */
    flush_peep,             /* called after function body is generated */
    oa_end_generation,		/* end of code generation */
    cg_internal_conflict,   /* internal conflict */
    preRegAlloc,			/* rewrites to improve register allocation */
    precolor,				/* precolor routine */
    oa_gen_strlab,             /* generate a named label */
    oa_put_label,              /* generate a numbered label */
    oa_put_string_label,    /* generate a numbered label */
    NULL,                   /* reserve space for a bit */
    oa_genint,                 /* initialize an int */
    oa_genfloat,               /* initialize a float */
    oa_genaddress,             /* initializae a pointer */
    oa_genstring,              /* initialize a string */
    oa_genref,                 /* put a reference to a variable */
    oa_genpcref,               /* put a reference to something in the code segment */
    oa_gensrref,               /* put a reference to the startup.rundown */
    oa_gen_labref,             /* put a reference to a label */
    oa_gen_labdifref,          /* put the difference of two labels */
    oa_gen_virtual,        /* start a virtual segment */
    oa_gen_endvirtual,     /* end a virtual segment */
    oa_gen_vtt,                  /* do a VT thunk entry */
    oa_gen_vc1,                 /* do a vc1 thunk entry */
    oa_gen_importThunk,                 /* do an import thunk entry */
    oa_genstorage,             /* generate uninitialized storage */
    oa_align,                  /* put an alignment command */
    oa_enterseg,               /* switch to new seg */
    oa_exitseg,                /* exit current segment */
    oa_globaldef,          /* put a global definition */
    oa_localdef,          /* put a local definition */
    oa_localstaticdef,          /* put a local static definition */
    oa_put_extern,         /* put an external definition */
    oa_put_impfunc,        /* put an import definition */
    oa_put_expfunc,        /* put an export definition */
    oa_output_alias,       /* put an alias */
    oa_output_includelib,  /* put an included library name */
    0,                      /* backend handle intrinsic */
    0,                      /* expression tag */
    0,                      /* tag */
    asm_line,               /* line number information and text */
    asm_blockstart,         /* block start */
    asm_blockend,           /* block end */
    asm_varstart,           /* var start */
    asm_func,               /* func start or end */
    asm_passthrough,        /* reserved */
    asm_datapassthrough,	/* reserved */
    asm_label,              /* put a label in the code stream */
    asm_atomic,             /* atomic primitives */
    asm_goto,               /* unconditional branch */
    asm_gosub,              /* normal gosub to an immediate label or through a var */
    asm_fargosub,           /* far version of gosub */
    asm_trap,               /* 'trap' instruction - the arg will be an immediate # */
    asm_int,                /* 'int' instruction, calls a labeled function which is an interrupt */
    asm_ret,                /* return from subroutine */
    asm_fret,                /* far return from subroutine */
    asm_rett,               /* return from trap or int */
    asm_add,                /* evaluate an addition */
    asm_sub,                /* evaluate a subtraction */
    asm_udiv,               /* unsigned division */
    asm_umod,               /* unsigned modulous */
    asm_sdiv,               /* signed division */
    asm_smod,               /* signed modulous */
    asm_muluh,               /* multiply highword unsigned*/
    asm_mulsh,               /* multiply highword signed*/
    asm_mul,               /* multiply */
    asm_lsl,                /* unsigned shift left */
    asm_lsr,                /* unsigned shift right */
    asm_asr,                /* signed shift right */
    asm_neg,                /* negation */
    asm_not,                /* complement */
    asm_and,                /* binary and */
    asm_or,                 /* binary or */
    asm_eor,                /* binary exclusive or */
    asm_setne,              /* evaluate a = b != c */
    asm_sete,               /* evaluate a = b == c */
    asm_setc,               /* evaluate a = b U< c */
    asm_seta,               /* evaluate a = b U> c */
    asm_setnc,              /* evaluate a = b U>= c */
    asm_setbe,              /* evaluate a = b U<= c */
    asm_setl,               /* evaluate a = b S< c */
    asm_setg,               /* evaluate a = b s> c */
    asm_setle,              /* evaluate a = b S<= c */
    asm_setge,              /* evaluate a = b S>= c */
    asm_assn,               /* assignment */
    asm_genword,            /* put a byte or word into the code stream */
    asm_coswitch,           /* switch characteristics */
    asm_swbranch,           /* case characteristics */
    asm_dc,                 /* unused */
    asm_assnblock,          /* copy block of memory*/
    asm_clrblock,           /* clear block of memory */
    asm_jc,                 /* branch if a U< b */
    asm_ja,                 /* branch if a U> b */
    asm_je,                 /* branch if a == b */
    asm_jnc,                /* branch if a U>= b */
    asm_jbe,                /* branch if a U<= b */
    asm_jne,                /* branch if a != b */
    asm_jl,                 /* branch if a S< b */
    asm_jg,                 /* branch if a S> b */
    asm_jle,                /* branch if a S<= b */
    asm_jge,                /* branch if a S>= b */
    asm_parm,               /* push a parameter*/
    asm_parmadj,            /* adjust stack after function call */
    asm_parmblock,          /* push a block of memory */
    asm_cppini,             /* cplusplus initialization (historic)*/
    asm_prologue,           /* function prologue */
    asm_epilogue,           /* function epilogue */
    asm_pushcontext,        /* push register context */
    asm_popcontext,         /* pop register context */
    asm_loadcontext,        /* load register context (e.g. at interrupt level ) */
    asm_unloadcontext,        /* unload register context (e.g. at interrupt level ) */
    asm_tryblock,			/* try, catch, end block */
    asm_stackalloc,         /* allocate stack space - positive value = allocate, negative value deallocate */
    asm_loadstack,			/* load the stack pointer from a var */
    asm_savestack,			/* save the stack pointer to a var */
    asm_functail,			/* function tail (e.g. destructor) start/end */
} ;       
ARCH_ASM assemblerInterface[] = {
    {
    "nasm",                                 /* assembler name */
    0,                                      /* backend data (compiler ignores) */
     "1",								/* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
   ".asm",                                  /* extension for assembly files */
    ".o",                               /* extension for object files, NULL = has no object mode */
    "occ",                               /* name of an environment variable to parse, or 0 */
    "occ",                             /* name of the program, for usage */
    "occ",                              /* name of a config file if you want to use one, or NULL (sans extension) */
    usage_text,                           /* pointer to usage text */
    args,									/* extra args */
    sizeof(args)/sizeof(args[0]),			/* number of args */
    oplst,                             /* inline assembler opcode list, or null */
    reglst,                             /* inline assembler register list, or null */
    prockeywords,                         /* specific keywords, e.g. allow a 'bit' keyword and so forth */
    defines,                     /* defines list to create at compile time, or null */
    &dbgStruct[0],                         /* debug structure, or NULL */
    &architecture,                /* architecture characteristics */
    &outputfunctions,                              /* pointer to backend function linkages */
    NULL,                               /* pointer to MSIL-specific data and functions */
    NULL,                               /* pointer to extra builtin data */
    initnasm,  /* return 1 to proceed */
    0,              /* precompile function, or NULL */
    0,              /* postcompile function, or NULL */
    RunExternalFiles,     /* postprocess function, or NULL */
    0,     /* compiler rundown */
    InsertOutputFileName,          /* insert the output (executable name) into the backend */
    InsertExternalFile,      /* insert a non-compilable file in the backend list, e.g. for post processing, or NULL */
    parse_param,     /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    parse_codegen,   /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    0,         /* parse a pragma directive, or null */
    compile_start,    /* signal start of compile on a per file basis */
    include_start,		/* signal switching to a new source file */
    output_obj_file,      /* write the object file (for native object formats) */
    outcode_file_init,    /* initialize the object file */
    inasmini,             /* initialize inline assembler, per file, or NULL */
    inasm_statement,        /* parse an assembly statement, or NULL */
    inlineAsmStmt,		/* translate an assembly instruction which was inlined */
    0,                   /* initialize intrinsic mechanism, compiler startup */
    0,                   /* search for an intrinsic */
    0,                     /* enter a type in the BE */
    0,                   /* msil - get a boxed version of type*/
    0,                   /* msil - get an unboxed version of type*/
    
    },
    {
    "fasm",                                 /* assembler name */
    0,                                      /* backend data (compiler ignores) */
     "1",								/* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
    ".asm",                                  /* extension for assembly files */
    ".o",                               /* extension for object files, NULL = has no object mode */
    "occ",                               /* name of an environment variable to parse, or 0 */
    "occ",                             /* name of the program, for usage */
    "occ",                              /* name of a config file if you want to use one, or NULL (sans extension) */
    usage_text,                           /* pointer to usage text */
    args,									/* extra args */
    sizeof(args)/sizeof(args[0]),			/* number of args */
    oplst,                             /* inline assembler opcode list, or null */
    reglst,                             /* inline assembler register list, or null */
    prockeywords,                         /* specific keywords, e.g. allow a 'bit' keyword and so forth */
    defines,                     /* defines list to create at compile time, or null */
    &dbgStruct[0],                         /* debug structure, or NULL */
    &architecture,                /* architecture characteristics */
    &outputfunctions,                              /* pointer to backend function linkages */
    initfasm,  /* return 1 to proceed */
    RunExternalFiles,     /* postprocess function, or NULL */
    0,     /* compiler rundown */
    InsertOutputFileName,          /* insert the output (executable name) into the backend */
    InsertExternalFile,      /* insert a non-compilable file in the backend list, e.g. for post processing, or NULL */
    parse_param,     /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    parse_codegen,   /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    0,         /* parse a pragma directive, or null */
    compile_start,    /* signal start of compile on a per file basis */
    include_start,		/* signal switching to a new source file */
    output_obj_file,      /* write the object file (for native object formats) */
    outcode_file_init,    /* initialize the object file */
    inasmini,             /* initialize inline assembler, per file, or NULL */
    inasm_statement,        /* parse an assembly statement, or NULL */
    inlineAsmStmt,		/* translate an assembly instruction which was inlined */
    0,                   /* initialize intrinsic mechanism, compiler startup */
    0,                   /* search for an intrinsic */
    0,                       /* __using__ declaration */    
    },
    {
    "tasm",                                 /* assembler name */
    0,                                      /* backend data (compiler ignores) */
     "1",								/* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
    ".asm",                                  /* extension for assembly files */
    ".o",                               /* extension for object files, NULL = has no object mode */
    "occ",                               /* name of an environment variable to parse, or 0 */
    "occ",                             /* name of the program, for usage */
    "occ",                              /* name of a config file if you want to use one, or NULL (sans extension) */
    usage_text,                           /* pointer to usage text */
    args,									/* extra args */
    sizeof(args)/sizeof(args[0]),			/* number of args */
    oplst,                             /* inline assembler opcode list, or null */
    reglst,                             /* inline assembler register list, or null */
    prockeywords,                         /* specific keywords, e.g. allow a 'bit' keyword and so forth */
    defines,                     /* defines list to create at compile time, or null */
    &dbgStruct[0],                         /* debug structure, or NULL */
    &architecture,                /* architecture characteristics */
    &outputfunctions,                              /* pointer to backend function linkages */
    inittasm,  /* return 1 to proceed */
    RunExternalFiles,     /* postprocess function, or NULL */
    0,     /* compiler rundown */
    InsertOutputFileName,          /* insert the output (executable name) into the backend */
    InsertExternalFile,      /* insert a non-compilable file in the backend list, e.g. for post processing, or NULL */
    parse_param,     /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    parse_codegen,   /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    0,         /* parse a pragma directive, or null */
    compile_start,    /* signal start of compile on a per file basis */
    include_start,		/* signal switching to a new source file */
    output_obj_file,      /* write the object file (for native object formats) */
    outcode_file_init,    /* initialize the object file */
    inasmini,             /* initialize inline assembler, per file, or NULL */
    inasm_statement,        /* parse an assembly statement, or NULL */
    inlineAsmStmt,		/* translate an assembly instruction which was inlined */
    0,                   /* initialize intrinsic mechanism, compiler startup */
    0,                   /* search for an intrinsic */
    0,                       /* __using__ declaration */    
    },
    {
    "masm",                                 /* assembler name */
    0,                                      /* backend data (compiler ignores) */
     "1",								/* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
    ".asm",                                  /* extension for assembly files */
    ".o",                               /* extension for object files, NULL = has no object mode */
    "occ",                               /* name of an environment variable to parse, or 0 */
    "occ",                             /* name of the program, for usage */
    "occ",                              /* name of a config file if you want to use one, or NULL (sans extension) */
    usage_text,                           /* pointer to usage text */
    args,									/* extra args */
    sizeof(args)/sizeof(args[0]),			/* number of args */
    oplst,                             /* inline assembler opcode list, or null */
    reglst,                             /* inline assembler register list, or null */
    prockeywords,                         /* specific keywords, e.g. allow a 'bit' keyword and so forth */
    defines,                     /* defines list to create at compile time, or null */
    &dbgStruct[0],                         /* debug structure, or NULL */
    &architecture,                /* architecture characteristics */
    &outputfunctions,                              /* pointer to backend function linkages */
    initmasm,  /* return 1 to proceed */
    RunExternalFiles,     /* postprocess function, or NULL */
    0,     /* compiler rundown */
    InsertOutputFileName,          /* insert the output (executable name) into the backend */
    InsertExternalFile,      /* insert a non-compilable file in the backend list, e.g. for post processing, or NULL */
    parse_param,     /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    parse_codegen,   /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    0,         /* parse a pragma directive, or null */
    compile_start,    /* signal start of compile on a per file basis */
    include_start,		/* signal switching to a new source file */
    output_obj_file,      /* write the object file (for native object formats) */
    outcode_file_init,    /* initialize the object file */
    inasmini,             /* initialize inline assembler, per file, or NULL */
    inasm_statement,        /* parse an assembly statement, or NULL */
    inlineAsmStmt,		/* translate an assembly instruction which was inlined */
    0,                   /* initialize intrinsic mechanism, compiler startup */
    0,                   /* search for an intrinsic */
    0,                       /* __using__ declaration */    
    },
    {
    "generic",                                 /* assembler name */
    0,                                      /* backend data (compiler ignores) */
     "1",								/* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
    ".asm",                                  /* extension for assembly files */
    ".o",                               /* extension for object files, NULL = has no object mode */
    "occ",                               /* name of an environment variable to parse, or 0 */
    "occ",                             /* name of the program, for usage */
    "occ",                              /* name of a config file if you want to use one, or NULL (sans extension) */
    usage_text,                           /* pointer to usage text */
    args,									/* extra args */
    sizeof(args)/sizeof(args[0]),			/* number of args */
    oplst,                             /* inline assembler opcode list, or null */
    reglst,                             /* inline assembler register list, or null */
    prockeywords,                         /* specific keywords, e.g. allow a 'bit' keyword and so forth */
    defines,                     /* defines list to create at compile time, or null */
    &dbgStruct[0],                         /* debug structure, or NULL */
    &architecture,                /* architecture characteristics */
    &outputfunctions,                              /* pointer to backend function linkages */
    initgeneric,  /* return 1 to proceed */
    RunExternalFiles,     /* postprocess function, or NULL */
    0,     /* compiler rundown */
    InsertOutputFileName,          /* insert the output (executable name) into the backend */
    InsertExternalFile,      /* insert a non-compilable file in the backend list, e.g. for post processing, or NULL */
    parse_param,     /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    parse_codegen,   /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    0,         /* parse a pragma directive, or null */
    compile_start,    /* signal start of compile on a per file basis */
    include_start,		/* signal switching to a new source file */
    output_obj_file,      /* write the object file (for native object formats) */
    outcode_file_init,    /* initialize the object file */
    inasmini,             /* initialize inline assembler, per file, or NULL */
    inasm_statement,        /* parse an assembly statement, or NULL */
    inlineAsmStmt,		/* translate an assembly instruction which was inlined */
    0,                   /* initialize intrinsic mechanism, compiler startup */
    0,                   /* search for an intrinsic */
    0,                       /* __using__ declaration */    
    },
    { 0 }
} ;

