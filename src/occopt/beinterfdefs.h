/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#pragma once

#include "browsedefs.h"
#include "ppCommon.h"

enum asmTypes : int;

namespace Optimizer
{
#define ARCHITECTURE_X86 0
#define ARCHITECTURE_MSIL 1
extern int architecture;

#define FRAME_FLAG_NEEDS_FRAME (((~(unsigned long long)0) >> 1) + 1)

/* max registers we support */
#define MAX_REGISTERS 100

#define REG_DOUBLE_FLAG 0x10000

#define STACK_PROTECT_BASIC 1
#define STACK_PROTECT_ALL 2
#define STACK_PROTECT_STRONG 4
#define STACK_PROTECT_EXPLICIT 8
#define STACK_OBJECT_OVERFLOW 16
#define STACK_UNINIT_VARIABLE 32
#define HEAP_CHECK 64
#define STACK_PROTECT_WITH_CANARY (STACK_PROTECT_BASIC | STACK_PROTECT_ALL | STACK_PROTECT_STRONG | STACK_PROTECT_EXPLICIT)

#define STACK_PROTECT_MINIMUM_CONSIDERED 4

/* Common compiler parameters */
typedef struct
{
    int prm_maxerr;         /* number of errors at which the compiler stops*/
    int prm_stackalign;     /* stack alignment */
    int optimizer_modules;  // optimizer module selection
    int icd_flags;
    int verbosity;       // verbosity level
    Dialect c_dialect;   /* language dialect for C */
    Dialect cpp_dialect; /* lanaguage dialect for c++ */
    bool prm_optimize_for_speed;
    bool prm_optimize_for_size;
    bool prm_optimize_float_access;
    bool prm_quiet;              /* no warnings/errors on console */
    bool prm_warning;            /* display warnings */
    bool prm_extwarning;         /* display extended warnings */
    bool prm_diag;               /* display diagnostics/ memory usage */
    bool prm_ansi;               /* use ansi restrictions */
    bool prm_cmangle;            /* add underscore to names */
    bool prm_cplusplus;          /* C++ mode */
    bool prm_xcept;              /* generate RTTI in C++ mode */
    bool prm_icdfile;            /* dump intermediate code peep list to file */
    bool prm_asmfile;            /* assembly language output */
    bool prm_compileonly;        /* don't run postprocess routines */
    bool prm_debug;              /* generate debug info */
    bool prm_listfile;           /* generate list file (not currently used)*/
    bool prm_cppfile;            /* generate preprocessor file */
    bool prm_errfile;            /* generate error file */
    bool prm_browse;             /* generate browse information */
    bool prm_trigraph;           /* parse trigraphs */
    bool prm_oldfor;             /* old C++ for scoping */
    bool prm_stackcheck;         /* generate stack check code */
    bool prm_allowinline;        /* enable inlining of functions */
    bool prm_profiler;           /* generate profiler insets */
    bool prm_mergestrings;       /* merge strings and other constants */
    bool prm_revbits;            /* use reverse of the default bit ordering */
    bool prm_lines;              /* put line info into assembly files */
    bool prm_bss;                /* Use a BSS */
    bool prm_intrinsic;          /* look for intrinsics */
    bool prm_smartframes;        /* omit frame pointer initialization as possible */
    bool prm_farkeyword;         /* honor the 'far' and 'near' keywords */
    bool prm_linkreg;            /* use a frame pointer */
    bool prm_charisunsigned;     /* char type is unsigned */
    bool prm_assemble;           /* do assembler mode */
    bool prm_makestubs;          /* generate make stubs */
    int prm_targettype;          /* DOS, WIN32, etc*/
    bool compile_under_dos;      /* true if compiling under dos*/
    bool prm_bepeep;             /* true to do peephole optimization in backend*/
    bool prm_crtdll;             /* use crtdll.dll*/
    bool prm_lscrtdll;           /* use lscrtl.dll*/
    bool prm_msvcrt;             /* use msvcrt.dll*/
    enum asmTypes prm_assembler; /* via assembly*/
    bool prm_flat;               /* flat mode*/
    bool prm_nodos;              /* no dos support */
    bool prm_useesp;             /* use ESP global selection (may be overridden)*/
    bool managed_library;        /* occil: use managed library */
    bool no_default_libs;        /* occil: don't load default assemlies and libraries*/
    bool replacePInvoke;         /* occil */
    bool msilAllowExtensions;    /* occil: allow extensions*/
    bool prm_displaytiming;      /* display timing info */
    bool prm_makelib;            /* make library */
    int prm_stackprotect;        /* stack protection mode */
    int prm_netcore_version;     /* .net core version to compile against.   0 = none, NetCore::DummyNeedsLatest = latest*/
    bool prm_exportAll;              /* export all symbols */
} COMPILER_PARAMS;

/* Sizing and alignment info uses this structure */
/* each item is expressed in MAUs. */
/* structure alignment will be automatically calculated if you set the
 * struct field to zero when using this as an alignment struct
 */
typedef struct
{
    char a_bool;
    char a_char;
    char a_short;
    char a_wchar_t;
    char a_enum;
    char a_int;
    char a_long;
    char a_longlong;
    char a_char16_t;
    char a_char32_t;
    char a_addr;
    char a_farptr;
    char a_farseg;
    char a_memberptr;
    char a_struct; /* alignment only */
    /* imaginary same as real */
    char a_float;
    char a_double;
    char a_longdouble;
    /* for complex numbers, padding as below is added to the corresponding float size
     * to get the offset of the imaginary part.  That offset is multiplied by two to get
     * the type size
     */
    char a_fcomplexpad;
    char a_rcomplexpad;
    char a_lrcomplexpad;
    char a_alignedstruct;  // __attribute((__aligned__))
    char a_maxalign;       // maximum alignment native to the implementation
} ARCH_SIZING;

/* floating point characteristics */
typedef struct
{
    int exp_min;
    int exp_bias;
    int exp_max;
    int mantissa_bits;
} ARCH_FLOAT;

typedef struct _regdesc
{
    const char* name;
    unsigned pushMask;
    int reg1live;
    int reg2live;
    int aliasCount;
    unsigned short aliases[32];
    BITINT* aliasBits;
} ARCH_REGDESC;

typedef struct _regclass
{
    struct _regclass* next;
    int regCount;
    unsigned short* regs;
    int vertex;
    int index;
    short* saturationBound;
    BITINT* regBits;
    BITINT* aliasBits;
} ARCH_REGCLASS;

typedef struct _regvertex
{
    struct _regclass* cls;
    struct _regvertex *left, *right;
    int index;
    struct _regvertex* parent;
    BITINT* aliasBits;
} ARCH_REGVERTEX;

enum e_pm
{
    ap_datareg = 1,
    ap_addrreg = 2,
    ap_floatreg = 4,
    ap_volatile = 8,
    ap_const0 = 16,
    ap_const1 = 32
};
typedef struct
{
    char unused;
} ARCH_PEEP;

/* architecture characteristics */
typedef struct
{
    ARCH_SIZING* type_align;     /* alignments */
    int (*align)(int size);      /* custom alignment routine */
    ARCH_SIZING* type_sizes;     /* sizes */
    ARCH_SIZING* type_needsLock; /* needs atomic lock */
    int isLockFreeSize;          /* maximum value for __c11_atomic_is_lock_free */
    int (*param_offs)(int size); /* routine is called in case parameters less than paramwidth need offsets */
    int compatibleIntSize;       /* size compatible to an integer */
    int compatibleAddrSize;      /* size compatible to an address */
    int nativeRegisterWidth;     /* size compatible with the register width */
    int packSize;                /* default packing */
    int rtlAlign;                /* alignment for memory operations, as provided by RTL */
    /* floating point characteristics not fully implemented */
    ARCH_FLOAT* flt_float;      /* characteristics of 'float' keyword */
    ARCH_FLOAT* flt_dbl;        /* characteristics of 'double' keyword */
    ARCH_FLOAT* flt_ldbl;       /* characteristics of 'long double' keyword */
    ARCH_FLOAT* flt_reserved;   /* future floating type */
    int registerCount;          /* number of registers */
    ARCH_REGDESC* regNames;     /* defines registers names */
    int regTreeCount;           /* number of register trees */
    ARCH_REGVERTEX* regRoot;    /* defines the register tree */
    ARCH_REGCLASS** regClasses; /* defines register class, based on size */
    ARCH_SIZING* regCosts;      /* defines register costs */
    UBYTE* regOrder;            /* register allocation order */
    int (*regMap)[2];           /* register map */
    ARCH_PEEP* peephole_defs;   /* defines peephole information */
    int fastcallRegCount;       /* Max number of regs considered for fastcall */
    const char* fastcallRegs;   /* register list for regs used in fastcall */
    int preferopts;             /* preferred optimizations */
    int denyopts;               /* optimizations we don't want */
#define DO_NOGCSE 1
#define DO_NOLCSE 2
#define DO_NOREGALLOC 4
#define DO_NOADDRESSINIT 8
#define DO_NOPARMADJSIZE 16
#define DO_NOLOADSTACK 32
#define DO_NOENTRYIF 64
#define DO_NOOPTCONVERSION 128
#define DO_NOINLINE 256
#define DO_UNIQUEIND 512
#define DO_NOFASTDIV 1024
#define DO_NODEADPUSHTOTEMP 2048
#define DO_MIDDLEBITS 4096
#define DO_NOBRANCHTOBRANCH 8192
#define DO_NOMULTOSHIFT 16384
#define DO_NOALIAS 32768
#define DO_NOCONST 0x10000

    int erropts; /* error options */
#define EO_RETURNASERR 1

    char hasFloatRegs; /* true if has floating point registers */
#define AFM_SIGNEDZERO 1
    char floatmode;                   /* floating point modes, not honored currently */
#define ABM_USESIZE 0                 /* bool is determined by sizing above */
#define ABM_GLOBALBITS 1              /* global bools become global bits */
#define ABM_LOCALBITS 2               /* all bools outside a struct definition become global bits */
    char boolmode;                    /* boolean mode */
#define ARM_NONE 0                    /* local register allocation only */
#define ARM_FUNCTIONSCOPE 1           /* register allocation across entire function */
    char regallocmode;                /* register allocation mode */
    char bits_per_mau;                /* number of bits in a MAU.  values other than 8 not supported */
    char word_size;                   /* natural size for this architecture, e.g. 4 for 32 bit arches and 8 for 64 bits */
    char big_endian;                  /* true if the architecture uses big endian numbering scheme */
                                      /* used minimally in front end */
    char reversedefaultbitallocation; /* architecture requires default bit allocation to be reversed */
    char globallocals;                /* locals in global memory */
    char spgrowsup;                   /* stack pointer grows up */
    char dynamiclocals;               /* true if to allocate subblock data dynamically (not supported) */
    char retblocksize;                /* size of a return block on stack (e.g. function ret addr & frame ptr) */
    char parmwidth;                   /* minimium width/ padding of passed parameters in maus */
    char stackalign;                  /* minimum stack alignment */
    char libsasimports;               /* library functions should be genned as import calls */
    char retblockparamadjust;         /* Adjustment for retblock parameters */
    char eofScratchReg;               /* register that can be used as scratch at end of function */
    char bitintunderlying;            /* size of underlying type for bitint */
    int bitintmax;                    /* max number of bits for bitint */
} ARCH_CHARACTERISTICS;

/* debugger characteristics */
typedef struct _arch_dbg
{
#ifdef ISPARSER___
    const char* name;                               /* name of debug format */
    void* userdata;                                 /* backend specific data, compiler ignores */
    int* blocknum;                                  /* pointer to variable which holds block number, or zero for no blocking */
    void (*init)(void);                             /* per file initialization */
    void (*rundown)(void);                          /* per file rundown */
    void (*outputtypedef)(struct SimpleSymbol* sp); /* dump the typedef HASHTABLE */
    /* browser funcs */
    void (*init_browsedata)(char* name); /* per file initialization */
    void (*browsedata)(BROWSEINFO* bri); /* put browse info somewhere */
    void (*browsefile)(BROWSEFILE* fil); /* put the browse file list */
    void (*rundown_browsedata)(void);    /* per file rundown */
#endif
} ARCH_DEBUG;

/* things to #define before compiling */
typedef struct _arch_defines
{
    const char* define; /* symbol to define */
    const char* value;  /* value */
    char respect;       /* set to true to make it define it, false to ignore it */
    char permanent;     /* set to true if the symbol cannot be undefined */
} ARCH_DEFINES;

/* a function for code generation */
typedef void (*CGFUNC)(QUAD* q);

/* list of routines to call during code generation */
/* these are all optional, you can fill them in with zero although if the compiler generates
 * code and there is no function to handle it, nothing will happen.
 *
 */
typedef struct _arch_gen
{
#ifdef ISPARSER___                                                          // this is temporary
    void (*gen_header)(const char* filename, const char* compiler_version); /* generate assembly language header */
    void (*gen_trailer)(void);                                              /* generate assembly language trailer */
    void (*adjust_codelab)(void* select, int offset); /* adjust an assembly language statement for the relative code labels */
    int (*pre_gcse)(QUAD* list);                      /* allow access to the quad list prior to GCSE */
    int (*post_gcse)(QUAD* list);                     /* allow access to the quad list after GCSE */
    void (*post_function_gen)(struct SimpleSymbol* funcsp, QUAD* list); /* called after function body is generated */
    void (*finalGen)(void);                                             /* end of code generation, lits have been dumped */
    void (*internalConflict)(QUAD* q);                                  /* enter a conflict for this instruction if necessary */
    int (*preRegAlloc)(QUAD* ins, BriggsSet* globals, BriggsSet* eobGlobals,
                       int pass);                                     /* allow access to the quad list before register allocation */
    void (*preColor)(QUAD* q);                                        /* precolor an instruction */
    void (*gen_strlab)(struct SimpleSymbol* sp);                      /* generate a named label */
    void (*gen_label)(int labnum);                                    /* generate a numbered label */
    void (*gen_string_label)(int labnum, int type);                   /* generate a numbered label */
    void (*gen_bit)(struct SimpleSymbol* sp, long long val);          /* reserve space for a bit */
    void (*gen_int)(enum e_gt type, long long val);                   /* initialize one of the integer types */
    void (*gen_float)(enum e_gt type, FPF* val);                      /* initialize a float */
    void (*gen_address)(unsigned long long val);                      /* initializae a pointer */
    void (*gen_string)(LCHAR* string, int len);                       /* initialize a string */
    void (*gen_ref)(struct SimpleSymbol* sp, int offset);             /* put a reference to a variable */
    void (*gen_pcref)(struct SimpleSymbol* sp, int offset);           /* put a reference to something in the code segment */
    void (*gen_srref)(struct SimpleSymbol* sp, int val, int type);    /* put a reference to the startup.rundown */
    void (*gen_labref)(int label);                                    /* put a reference to a label */
    void (*gen_labdifref)(int left, int right);                       /* put the difference of two labels */
    void (*gen_virtual)(struct SimpleSymbol* sp, int data);           /* start a virtual segment */
    void (*gen_endvirtual)(struct SimpleSymbol* sp);                  /* end a virtual segment */
    void (*gen_vtt)(VTABENTRY* vt, struct SimpleSymbol* func);        /* vttthunk entry */
    void (*gen_vc1)(struct SimpleSymbol* func);                       /* vc1thunk entry */
    void (*gen_importThunk)(struct SimpleSymbol* func);               /* import thunk entry */
    void (*gen_storage)(int size);                                    /* generate uninitialized storage */
    void (*align)(int size);                                          /* put an alignment command */
    void (*setalign)(int code, int data, int bss, int constant);      /* put alignment sizes */
    void (*enterseg)(enum e_sg segnum);                               /* enter a new segment */
    void (*exitseg)(enum e_sg segnum);                                /* exit current segment */
    void (*global_define)(struct SimpleSymbol* sp);                   /* put a global definition */
    void (*local_define)(struct SimpleSymbol* sp);                    /* put a global definition */
    void (*local_static_define)(struct SimpleSymbol* sp);             /* put a function local definition */
    void (*extern_define)(struct SimpleSymbol* sp, int code);         /* put an external definition */
    void (*import_define)(struct SimpleSymbol* sp, const char* file); /* put an import definition */
    void (*export_define)(struct SimpleSymbol* sp);                   /* put an export definition */
    void (*output_alias)(char* sym, char* alias);                     /* put an alias */
    void (*output_includelib)(const char* name);                      /* put an included library name */
    IMODE* (*handleIntrins)(EXPRESSION* node, int novalue);           /* backend handle intrinsic */
    CGFUNC asm_exprtag;                                               /* expression tag */
    CGFUNC asm_tag;                                                   /* tag */
    CGFUNC asm_line;                                                  /* line number information and text */
    CGFUNC asm_blockstart;                                            /* block start */
    CGFUNC asm_blockend;                                              /* block end */
    CGFUNC asm_varstart;                                              /* local var start */
    CGFUNC asm_func;                                                  /* function start or end */
    CGFUNC asm_passthrough;                                           /* reserved */
    CGFUNC asm_datapassthrough;                                       /* reserved */
    CGFUNC asm_label;                                                 /* put a label in the code stream */
    CGFUNC asm_atomic;                                                /* atomic primitives */
    CGFUNC asm_goto;                                                  /* unconditional branch */
    CGFUNC asm_gosub;                                                 /* normal gosub to an immediate label or through a var */
    CGFUNC asm_fargosub;                                              /* far version of gosub */
    CGFUNC asm_trap;                                                  /* 'trap' instruction - the arg will be an immediate # */
    CGFUNC asm_int;           /* 'int' instruction, calls a labeled function which is an interrupt */
    CGFUNC asm_ret;           /* return from subroutine */
    CGFUNC asm_fret;          /* far return from subroutine */
    CGFUNC asm_rett;          /* return from trap or int */
    CGFUNC asm_add;           /* evaluate an addition */
    CGFUNC asm_sub;           /* evaluate a subtraction */
    CGFUNC asm_udiv;          /* unsigned division */
    CGFUNC asm_umod;          /* unsigned modulous */
    CGFUNC asm_sdiv;          /* signed division */
    CGFUNC asm_smod;          /* signed modulous */
    CGFUNC asm_muluh;         /* multiply highword unsigned*/
    CGFUNC asm_mulsh;         /* multiply highword signed*/
    CGFUNC asm_mul;           /* multiply */
    CGFUNC asm_lsl;           /* unsigned shift left */
    CGFUNC asm_lsr;           /* unsigned shift right */
    CGFUNC asm_asr;           /* signed shift right */
    CGFUNC asm_neg;           /* negation */
    CGFUNC asm_not;           /* complement */
    CGFUNC asm_and;           /* binary and */
    CGFUNC asm_or;            /* binary or */
    CGFUNC asm_eor;           /* binary exclusive or */
    CGFUNC asm_setne;         /* evaluate a = b != c */
    CGFUNC asm_sete;          /* evaluate a = b == c */
    CGFUNC asm_setc;          /* evaluate a = b U< c */
    CGFUNC asm_seta;          /* evaluate a = b U> c */
    CGFUNC asm_setnc;         /* evaluate a = b U>= c */
    CGFUNC asm_setbe;         /* evaluate a = b U<= c */
    CGFUNC asm_setl;          /* evaluate a = b S< c */
    CGFUNC asm_setg;          /* evaluate a = b s> c */
    CGFUNC asm_setle;         /* evaluate a = b S<= c */
    CGFUNC asm_setge;         /* evaluate a = b S>= c */
    CGFUNC asm_assn;          /* assignment */
    CGFUNC asm_genword;       /* put a byte or word into the code stream */
    CGFUNC asm_coswitch;      /* switch characteristics */
    CGFUNC asm_swbranch;      /* case characteristics */
    CGFUNC asm_dc;            /* unused */
    CGFUNC asm_assnblock;     /* copy block of memory*/
    CGFUNC asm_clrblock;      /* clear block of memory */
    CGFUNC asm_cmpblock;      /* branch if two blocks of memory aren't equal */
    CGFUNC asm_jc;            /* branch if a U< b */
    CGFUNC asm_ja;            /* branch if a U> b */
    CGFUNC asm_je;            /* branch if a == b */
    CGFUNC asm_jnc;           /* branch if a U>= b */
    CGFUNC asm_jbe;           /* branch if a U<= b */
    CGFUNC asm_jne;           /* branch if a != b */
    CGFUNC asm_jl;            /* branch if a S< b */
    CGFUNC;                   /* branch if a S> b */
    CGFUNC asm_jle;           /* branch if a S<= b */
    CGFUNC asm_jge;           /* branch if a S>= b */
    CGFUNC asm_parm;          /* push a parameter*/
    CGFUNC asm_parmadj;       /* adjust stack after function call */
    CGFUNC asm_parmblock;     /* push a block of memory */
    CGFUNC asm_cppini;        /* cplusplus initialization (historic)*/
    CGFUNC asm_prologue;      /* function prologue */
    CGFUNC asm_epilogue;      /* function epilogue */
    CGFUNC asm_pushcontext;   /* push register context */
    CGFUNC asm_popcontext;    /* pop register context */
    CGFUNC asm_loadcontext;   /* load register context (e.g. at interrupt level ) */
    CGFUNC asm_unloadcontext; /* unload register context (e.g. at interrupt level ) */
    CGFUNC asm_tryblock;      /* try/catch/ exit try block */
    CGFUNC asm_stackalloc;    /* allocate stack space - positive value = allocate, negative value deallocate */
    CGFUNC asm_loadstack;     /* load the stack pointer */
    CGFUNC asm_savestack;     /* save the stack pointer */
    CGFUNC asm_seh;           /* seh */
    void (*asm_functail)(QUAD* q, int begin, int size); /* functail start or end */
#endif
} ARCH_GEN;

typedef struct
{
    const char* name;
    short atype;
    short amode;
    void* data;
} ASMNAME;
typedef struct
{
    const char* name;
    short regtype;
    char regnum;
    char size;
} ASMREG;

typedef struct
{
    bool allowExtensions;                     /* True if allowing language extensions */
    bool (*managed)(struct SimpleSymbol* sp); /* return true if the function is a managed function, false otherwise */
    void (*create_property)(struct SimpleSymbol* property, struct SimpleSymbol* getter,
                            struct SimpleSymbol* setter);  // create a property instance
} ARCH_MSIL;
typedef struct _arch_asm
{
    const char* name;       /* assembler name */
    void* userdata;         /* backend data (compiler ignores) */
    const char* hosted;     /* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
    const char* asmext;     /* extension for assembly files */
    const char* objext;     /* extension for object files, nullptr = has no object mode */
    const char* beext;      /* extensions for files which should be passed to the back end */
    const char* envname;    /* name of an environment variable to parse, or 0 */
    const char* progname;   /* name of the program, for usage */
    const char* cfgname;    /* name of a config file if you want to use one, or nullptr (sans extension) */
    const char* usage_text; /* pointer to usage text */
    const char* help_text;  /* pointer to help text */
    const char* libfile;    /* name of library to be linked against */
    const char* machine;    /* machine string */
#ifdef ISPARSER___
    CMDLIST* Args; /* argument handling */
#else
    void* Args;
#endif
    int ArgCount; /* number of arguments */
#ifdef ISPARSER___
    KEYWORD* keywords; /* specific keywords, e.g. allow a 'bit' keyword and so forth */
#else
    void* keywords;
#endif
    ARCH_DEFINES* defines;      /* defines list to create at compile time, or null */
    ARCH_DEBUG* debug;          /* debug structure, or nullptr */
    ARCH_CHARACTERISTICS* arch; /* architecture characteristics */
    ARCH_GEN* gen;              /* pointer to backend function linkages */
#ifdef ISPARSER___
    ARCH_MSIL* msil; /* pointer to MSIL-specific data and functions */
#endif
    const char* bltins;                                                              /* pointer to extra builtin data */
    int (*init)(COMPILER_PARAMS* params, struct _arch_asm* data, ARCH_DEBUG* debug); /* return 1 to proceed */
#ifdef ISPARSER___
    int (*main_preprocess)();                     /* preprocess function, or nullptr */
    void (*main_postprocess)(bool hasErrors);     /* postprocess function, or nullptr */
    int (*compiler_postprocess)(char*);           /* postprocess function, or nullptr */
    int (*rundown)(void);                         /* compiler rundown */
    void (*insert_output_file)(const char* name); /* insert the output (executable name) into the backend */
    int (*insert_noncompile_file)(
        char* name, bool primary); /* insert a non-compilable file in the backend list, e.g. for post processing, or nullptr */
    int (*parse_param)(char mode, const char* string);   /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    int (*parse_codegen)(char mode, const char* string); /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    void (*doPragma)(const char* kw, const char* tag);   /* parse a pragma directive, or null */
    void (*enter_filename)(char* name);                  /* signal start of compile on a per file basis */
    void (*enter_includename)(char* name, int num);      /* switch to or from an include file */
    void (*output_obj_file)(void);                       /* write the object file (for native object formats) */
    void (*outcode_init)(void);                          /* initialize the object file */
    void (*inlineAsmInit)(void);                         /* initialize inline assembler, per file, or nullptr */
#    ifdef ISPARSER
    LEXEME* (*inlineAsm)(LEXEME*, BLOCKDATA*); /* parse an assembly statement */
#    else
    void* inlineAsm;
#    endif
    void* (*inlineAsmStmt)(void* stmt);             /* inlined asm stmt */
    void (*intrinsicInit)(void);                    /* initialize intrinsic mechanism, compiler startup */
    void (*SearchIntrins)(struct SimpleSymbol* sp); /* search for an intrinsic */
    void (*enter_type)(struct SimpleSymbol* sp);    /* enter a type in the BE */
    enum e_lk (*getDefaultLinkage)();               /* get dll linkage corresponding to command line switches */
#endif
} ARCH_ASM;

enum e_bet
{
    bee_unknown,
    bee_icon,
    bee_float,
    bee_imaginary,
    bee_complex,
    bee_temp,
    bee_global_pc,
    bee_abs,
    bee_global_data,
    bee_local,
    bee_label,
    bee_rv,
    bee_threadlocal
};

#define BIT_NO_BITS -1

typedef struct
{
    enum e_bet mode;
    IMODE* m;
    int size;
    int ind : 1;
    int immed : 1;
    union
    {
        long long i;
        struct
        {
            FPF* r;
            FPF* i;
        } f;
        struct
        {
            const char* name;
            struct SimpleSymbol* sp;
            int localOffset;
            int symOffset;
            int startBit;
            int bits;
        } sym;
        int tempRegNum;
        int labelNum;
    } u;
} BE_IMODEDATA;

#define STARTUP_TYPE_STARTUP 1
#define STARTUP_TYPE_RUNDOWN 2
#define STARTUP_TYPE_TLS_STARTUP 3
#define STARTUP_TYPE_TLS_RUNDOWN 4

#define beGetIcon(x) ((x)->offset->i)

#define beGetLabel Optimizer::nextLabel++
#define beSetProcSymbol(x) set_symbol(x, true)
#define beSetDataSymbol(x) set_symbol(x, false)
#define beLocalAlloc(x) Alloc(x)
#define beCompilerVersion version
#define beGetKeywordData identdata
#define beGlobalAlloc(x) globalAlloc(x)

extern COMPILER_PARAMS cparams;
extern int nextLabel;
extern char version[];
extern char* prm_searchpath;
extern ASMNAME* identdata;

void bePrintf(char* format, ...);
void beWrite(char* buf, size_t size);
void* globalAlloc(int siz);
BE_IMODEDATA* beArgType(IMODE* in);
int beVariableLive(IMODE* m);

struct MsilProperty
{
    SimpleSymbol* prop;
    SimpleSymbol* getter;
    SimpleSymbol* setter;
};
}  // namespace Optimizer