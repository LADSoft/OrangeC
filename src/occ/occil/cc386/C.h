/* 
CC386 C Compiler
Copyright 1994-2011 David Lindauer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

This program is derived from the cc68k complier by 
Matthew Brandt (mailto::mattb@walkingdog.net) 

You may contact the author of this derivative at:

mailto::camille@bluegrass.net
 */
/*      compiler header file    */

#ifndef C_H
#define C_H
#define TRUE 1
#define FALSE 0

#define issymchar(x) (isalnum(x) || (x) == '_')
#define isstartchar(x) (isalpha(x) || (x) == '_')

/* declaration flags */
#define DF_INT 1
#define DF_ABS 2
#define DF_CONST 4
#define DF_VOL 8
#define DF_FUNCPARMS 16
#define DF_GLOBAL 32
#define DF_AUTOREG 64
#define DF_PRIVATE 128
#define DF_PROTECTED 256
#define DF_PUBLIC 512
#define DF_INTRINS 1024
#define DF_LOADDS 2048
#define DF_FORCEREG 4096
#define DF_RESTRICT 8192
#define DF_FAR 16384
#define DF_FAULT 32768
#define DF_SEG 0x10000
#define DF_CALLRUNHEDGE 0x20000
#define DF_ACTUALEXPR 0x40000
#define DF_NOFCONTRACT 0x80000
#define DF_HASOVERRIDE 0x100000
#define DF_CXLIMITED   0x200000
#define DF_FENV_ACCESS 0x400000
#define DF_FLEXARRAY    0x800000
#define DF_FUNCPARM     0x1000000
#define UF_DEFINED 1
#define UF_USED	2
#define UF_ASSIGNED 4
#define UF_CANASSIGN 8
#define UF_ALTERED 16

#define GF_ASSIGN 1
#define GF_CONTINUABLE 2
#define GF_CONTINUE 4
#define GF_GOTO 8
#define GF_DEF 16
#define GF_BREAK 32
#define GF_RETURN 64
#define GF_UNREACH 128
#define GF_NOPROTO 256
#define GF_AND 512
#define GF_SUPERAND 1024
#define GF_INLOOP 2048
#define GF_INFUNCPARMS 4096
#define GF_BREAKABLE 8192
#define GF_INIF 16384
#define GF_THROW 32768

    #define BM_PRIVATE 0
    #define BM_PROTECTED 1
    #define BM_PUBLIC 2

    #define PF_PRIVATE 0x20000
    #define PF_PROTECTED 1
    #define PF_PUBLIC 2
    #define PF_VIRTUAL 4
    #define PF_PURE 8
    #define PF_INLINE 16
    #define PF_STATIC 32
    #define PF_CONSTRUCTOR 64
    #define PF_DESTRUCTOR 128
    #define PF_PARENTPRIVATE 256
    #define PF_PARENTPROTECTED 512
    #define PF_PARENTPUBLIC 1024
    #define PF_FRIEND 2048
    #define PF_NODEF 4096
    #define PF_HASCONS 8192
    #define PF_HASDEST 16384
    #define PF_UNREACHABLE 0x8000
    #define PF_MEMBER 0x10000
    #define PF_HEADERFUNC 0x40000
    #define PF_OPERATOR   0x80000
    #define PF_INSTANTIATED 0x100000
    #define PF_INHERITED 0x200000
    #define PF_DUMDEST 0x400000
    #define PF_TEMPCONSALREADY 0x800000
    #define PF_TEMPCONSREFALREADY 0x1000000
    #define PF_CPPSTRUCT 0x2000000
    #define PF_EXPLICIT 0x4000000
    #define PF_NOTFULLYDEFINED 0x8000000

    #define CI_CONSTRUCTOR 0
    #define CI_DESTRUCTOR 1
    #define CI_NEW 2
    #define CI_DELETE 3
    #define CI_NEWA 41
    #define CI_DELETEA 42

#define STD_PRAGMA_FENV 1
#define STD_PRAGMA_FCONTRACT 2
#define STD_PRAGMA_CXLIMITED 4

#define MACRO_REPLACE_SIZE 65536
#define INPUT_BUFFER_SIZE 32768


/* keywords and symbols */
enum e_sym
{
    /* first comes all the C++ overloadable operators */
    /* be sure to change expectlist if you insert things */
    kw_new, kw_delete, plus, minus, star, divide, lshift, rshift, modop, eq,
        neq, lt, leq, gt, geq, assign, asplus, asminus, astimes, asdivide,
        asmodop, aslshift, asrshift, asand, asor, asxor, autoinc, autodec,
        openbr, openpa, pointstar, pointsto, lor, land, not, or, and, uparrow,
        compl, 
    /* then generic stuff that isn't overloadable or is internal */
    id, cconst, iconst, lconst, iuconst, luconst, llconst, lluconst, sconst,
        lsconst, fconst, lrconst, rconst, hook, colon, begin, end, dot, dostar,
        closebr, closepa, ellipse, comma, semicolon, hash, classsel,
        classselstar, dotstar, lassign, 
    /* Finally tokens for keywords */
    kw_int, kw_void, kw_char, kw_float, kw_double, kw_struct, kw_union, kw_long,
        kw_short, kw_unsigned, kw_signed, kw_auto, kw_extern, kw_register,
        kw_typedef, kw_static, kw_goto, kw_return, kw_sizeof, kw_break,
        kw_continue, kw_if, kw_else, kw_for, kw_do, kw_while, kw_switch,
        kw_case, kw_default, kw_enum, kw_volatile, kw_const, kw_phitext,
        kw__trap, kw__interrupt, kw__fault, kw__abs, kw__genword, kw___func__,
        kw__Complex, kw_restrict, kw__Imaginary, kw___I, kw__NAN,
		kw_typeof, kw__Pragma, 
    /* C++ */
    kw_public, kw_private, kw_protected, kw_class, kw_friend, kw_namespace,
        kw_using, kw_this, kw_operator, kw_inline, kw_virtual, kw_try, kw_throw,
        kw_catch, kw_template, kw_bool, kw_true, kw_false, kw_wchar_t,
        kw_static_cast, kw_dynamic_cast, kw_typeid, kw_typename, kw_explicit,
        kw_export, 
    /* extended */
    kw__pascal, kw__stdcall, kw__cdecl, kw__intrinsic, kw_asm, kw__loadds,
        kw__far, kw_asmreg, kw_asminst, kw__indirect, kw__export, kw__import,
        kw__near, kw__seg, kw___typeid, kw___int64, kw_alloca, 
    /* These next are generic register names */
    kw_D0, kw_D1, kw_D2, kw_D3, kw_D4, kw_D5, kw_D6, kw_D7, kw_D8, kw_D9, kw_DA,
        kw_DB, kw_DC, kw_DD, kw_DE, kw_DF, kw_A0, kw_A1, kw_A2, kw_A3, kw_A4,
        kw_A5, kw_A6, kw_A7, kw_A8, kw_A9, kw_AA, kw_AB, kw_AC, kw_AD, kw_AE,
        kw_AF, kw_F0, kw_F1, kw_F2, kw_F3, kw_F4, kw_F5, kw_F6, kw_F7, kw_F8,
        kw_F9, kw_FA, kw_FB, kw_FC, kw_FD, kw_FE, kw_FF, kw_cr0, kw_cr1, kw_cr2,
        kw_cr3, kw_cr4, kw_cr5, kw_cr6, kw_cr7, kw_dr0, kw_dr1, kw_dr2, kw_dr3,
        kw_dr4, kw_dr5, kw_dr6, kw_dr7, kw_tr0, kw_tr1, kw_tr2, kw_tr3, kw_tr4,
        kw_tr5, kw_tr6, kw_tr7, eol, eof
};

    /* Definitions for C++ overloaded operators */
    #define IT_THRESHOLD 2
    #define IT_OV_THRESHOLD 2
    enum overloadop
    {
        ov_ctr, ov_dtr, ov_new, ov_delete, ov_add, ov_sub, ov_mul, ov_div,
            ov_shl, ov_shr, ov_mod, ov_equ, ov_neq, ov_lt, ov_le, ov_gt, ov_ge,
            ov_asn, ov_aasadd, ov_assub, ov_asmul, ov_asdiv, ov_asmod, ov_asshl,
            ov_asshr, ov_asand, ov_asor, ov_asxor, ov_autoinc, ov_autodec,
            ov_ind, ov_arg, ov_pstar, ov_arr, ov_lor, ov_land, ov_not, ov_or,
            ov_and, ov_xor, ov_cpl, 
        // must come after everything else
        ov_newa = CI_NEWA, ov_deletea = CI_DELETEA, 
    };

/* storage classes */
enum e_sc
{
    sc_static, sc_auto, sc_global, sc_external, sc_type, sc_defunc,
        sc_friendlist, sc_const, sc_tconst, sc_member, sc_classmember, sc_label,
        sc_ulabel, sc_argument, sc_memberreg, sc_autoreg, sc_externalfunc,
        sc_abs, sc_namespace, sc_temp
};

/* basic types */
enum e_bt
{
    /* keep this ordering and dont insert anything before the end of the
     * basic types, type comparisons (LOSTCONV) depends on the ordering,
     * and the debug info has a table indexed by type
     */
    bt_bit, bt_bool, bt_char, bt_unsignedchar, bt_short, bt_unsignedshort, bt_wchar_t, bt_enum,
        bt_int, bt_unsigned, bt_long, bt_unsignedlong, bt_longlong,
        bt_unsignedlonglong, bt_float, bt_double, bt_longdouble, bt_fimaginary,
        bt_rimaginary, bt_lrimaginary, bt_fcomplex, bt_rcomplex, bt_lrcomplex, 
    /* end of basic types */
    bt_void, 
    /* end of debug needs */
    bt_untyped, bt_typedef, bt_pointer, bt_ref, bt_farpointer, bt_struct,
        bt_union, bt_func, bt_class, bt_iclass, bt_ifunc, bt_matchall,
        bt_matchnone, bt_ellipse, bt_bitfield, bt_memberptr, bt_defunc, bt_cond,
        bt_consplaceholder, bt_templateplaceholder, bt_segpointer, bt_string
};

struct slit
{
    struct slit *next;
    int label; /* label to use */
    int len;
    char *str; /* string to dump */
    char type; /* ascii/phitext or wide */
};
struct stab
{
    struct sym *head,  *tail;
};
typedef struct _vlist
{
    struct _vlist *next;
    struct sym *entry;
    int negofs;
} VLIST;

struct _classlist;

/* symbols */
struct sym
{
    struct sym *next; /* next symbol (local tabs only) */
    char *name; /* symbol name */
    enum e_sc storage_class; /* storage class */
    #ifdef ICODE
        struct _imode_ *imaddress;
        struct _imode_ *imvalue;
        struct _imode_ *imind;
    #endif 
    int extflag: 1; /* if external, was it used */
    int absflag: 1; /* the _abs keyword was used */
    int intflag: 1; /* the _interrupt keyword was used */
    int indecltable: 1; /* global already in dump table */
    int pascaldefn: 1; /* function: pascal type */
    int isstdcall: 1; /* function: stdcall type */
    int inreg: 1; /* function var is in reg, no stack space needed */
    int funcparm: 1; /* function var is a param and has space allocated */
    int staticlabel: 1; /* Var is a static declared in a function */
    int loadds: 1; /* to load data seg (limited) */
    int farproc: 1; /* this procedure should be terminated with retf */
    int indirect: 1; /* This procedure is to be called indirectly. */
    int exportable: 1; /* Export the symbol (for win32 dll) */
    int importable: 1; /* Import the symbol (for win32 dll) */
    int calleenearret: 1; /* true if callee provided a place for the return
        value */
    int hasproto: 1; /* C language prototype was encountered */
    int hasunnamed: 1; /* structure has unnamed substructs */
    int isunnamed: 1; /* substructure is unnamed */
    int istemplate: 1; /* type is a template placeholder */
    int istemplatevar: 1; /* type is a template variable */
    int isinsttemplate: 1; /* function is an instantiated template */
    int faultflag: 1; /* interrupt must pop the fault value */
    int gennedvirtfunc: 1; /* got generated as a virtual function */
    int recalculateParameters: 1; /* inline func needs its parameters
        recalculated */
    int istypedef: 1; /* is a typedef */
    int iscatchvar: 1; /* catch var, no cleanup */
    int nullsym: 1; /* if was a callblock return which isn't used */
    int mangled: 1; /* if we applied C++ name mangling, e.g. it is ok to expand
        namespaces at back end */
    int intagtable: 1; /* it is in a tag table */
    int dontlist: 1; /* it is a system include, don't put in list file */
    int constructed: 1; /* a constructor was run on it */
	int preprocessing: 1; /* true if is currently not a candidate for preprocessing (macros only( */
    #ifdef ICODE
        int iglobal: 1; /* if a temp variable has gone global */
        int ipointerindx; /* pointer index for pointer opts */
    #endif 
        int structAlign; /* alignment pref for this structure/class/union   */
    char **oldstyle; /* pointer to a names list if an old style function arg */
    int linkid; /* ID to use for linkage purposes (binary output) */
    int offset; /* address offset of data in the given seg */
    int paramsize; /* Size of parameter list for stdcall functions */
    ENODE *defalt; /* Default for function params */
    /* Also name for CPP overload lists */
    /* also default for template parameters, is a TYP */
    char *alias; /* alias.  Filled in right before end */
    char *importfile; /* import name */
    char *memberptrname; /* name to use for address of member pointer, or 0 if
        none */
    struct sym *parentclass; /* the sym of the class which holds this member
        item */
    struct sym *originalparentclass; /* sym of its original parent (used for
        friend matching in inheritance)*/
    struct sym *mainsym; /* pointer to the global version of a copied symbol */
    struct sym *enumlist; /* pointer to a list of enumeration values, or link
        in chain */
    /* these fields depend on storage_class */
    union
    {
        long i; /* int val */
        unsigned long u; /* nsigned val */
        FPF f; /* float val */
        char *s; /* string val */

            struct cdat
            {
                int i; /* int val = member offset */
                int size; /* duplicates the type entry */
                int vtabindex; /* which vtab entry this symbol is */
                int vtaboffs; /* offset to the vtab for this symbol */
                long cppflags; /* flags */
                struct sym *memptrclass; /* member pointer referenece class */
                struct _classlist *baseclass; /* Parent class list */
                struct _classlist *enclosedclass; /* list of enclosed classes */
                /*         struct stab    overlist; overload table */
                struct stab memberptrs; /* pointers to members */
                struct ifunc *inlinefunc; /* inline functions-> to statement */
                ENODE *consbuild; /* constructor build tree */
                struct defvalue *defalt; /* Default for function params */
                /* Also name for CPP overload lists */
                ENODE *eofdest; /* destructor tree for end of function */
                struct sym *gdeclare; /* pointer to the global version of
                    static class members */
                struct _namespace *parentns; /* pointer to enclosing namespace
                    */
                struct template  *templatedata; /* data for template types */
                struct sym *templateparent; /* uninstantiated version of the
                    template class */
                LIST *templateargs; /* arguments used when instantiating a
                    template class */
                int conslabel; /* constructor label #, or xcpt tab label # for
                    functions */
                int destlabel; /* destructor label # or label following
                    _InitExceptBlock for functions */
                struct sym *xtsym; /* xception tags symbol */
                LIST *throwlist; /* list of throwable types */
				int virtualstructdatasize; /* size of appended virtual structures */
            } classdata;
    }
    value;
    /* Type declarations */
    struct typ
    {
        enum e_bt type; /* the type */
        #define NORMARRAY 1
        #define VARARRAY 2				
        char val_flag; /* set if is an array */
        char uflags; /* Track usage */
        char bits; /* -1 for not a bit val, else bit field len */
        char startbit; /* start of bit field */
        long size; /* total size of type */
        int cflags; /* C & C++ flags */
        struct typ *btp; /* pointer to next type (pointers & arrays */
        struct sym *sp; /* pointer to a symbol which describes the type */
        /* local symbol tables */
        struct stab lst; /* Symbol table for structs & functions, also tail =
            type table for template placeholders */
        int *typeindex; /* type index for debugger */
        struct sym *enumlist; /* enumeration list for enums */
        struct typ *reclone; /* clone list for things that have been cloned
            before the full declaration came into effect (circular) */
        ENODE *esize; /* enode version of size */
        int esizeindex; /* index into the vararray */
    }
    *tp;
    struct decldata
    {
        struct decldata *link;
        enum
        {
            dd_bool, dd_byte, dd_word, dd_enum, dd_int, dd_long, dd_longlong, dd_float, dd_double,
                dd_ldouble, dd_fcomplex, dd_rcomplex, dd_lrcomplex, dd_pcname,
                dd_dataname, dd_storage, dd_label, dd_farpointer
        } mode;
        union
        {
            LLONG_TYPE i;
            FPF f[2];
            struct sym *sp;
        }
        val;
        long offset;
    }
     *init;
};

#define SYM     struct sym
#define TYP     struct typ
#define TABLE   struct stab

#define MAX_STRLEN      16384
#define MAX_STLP1       (MAX_STRLEN + 1)

/* struct for preprocessor if tracking */
typedef struct ifstruct
{
    struct ifstruct *link; /* link */
    short iflevel;
    short elsetaken;
} IFSTRUCT;

/* #define tracking */
typedef struct
{
    char *string;
    short argcount;
    char **args;
    int flags ;
#define DS_VARARGS 1
#define DS_PERMANENT 2
} DEFSTRUCT;

/* used for #pragma aux */
typedef struct _auxlist
{
    struct _auxlist *link;
    char *name;
    char *alias;
} AUXLIST;
/* error list */
struct errl
{
    struct errl *link;
    short errornumber;
    void *data;
};

/* used for error skimming */
#define BALANCE struct balance
#define BAL_PAREN   0
#define BAL_BRACKET 1
#define BAL_BEGIN 2
#define ERRORS struct errl

struct balance
{
    struct balance *back;
    short type;
    short count;
};

/* Global symbol table is a hash table */
#define HASHTABLESIZE 9973

typedef struct defvalue
{
    ENODE *ep;
    ENODE *cons;
    ENODE *dest;
} DEFVALUE;

typedef struct _hashrec_
{
    struct _hashrec_ *link; /* Link to next element in list */
    char *key; /* Full key */
} HASHREC;

typedef struct
{
    char *word;
    short atype;
    short amode;
    struct _opcode *data;
} ASMNAME;
typedef struct kwblk
{
    struct kwblk *next;
    char *word;
    short stype;
    enum
    {
        KW_CPLUSPLUS = 1, KW_INLINEASM = 2, KW_NONANSI = 4, KW_C99 = 8
    } flags;
    ASMNAME *data;
}KEYWORDS;

//-------------------------------------------------------------------------

typedef struct intrinsblk
{
    struct intrinsblk *next;
    char *word;
    short val;
} INTRINSICS;

typedef struct
{
    char *word;
    short regtype;
    char regnum;
    char size;
} ASMREG;

/* we use C++ name spaces for the gsyms, but it is transparent if C++
 * is not enabled
 */
typedef struct _namespace
{
    struct _namespace *next;
    short anonymous;
    short guardian;
    LIST **using_list,  **tag_using_list;
    SYM *sp;
    HASHREC **table;
    HASHREC **tagtable;
} NAMESPACE;

struct template 
{
    struct stab classes; /* the template's class declarations */
    int lineno; /* line number template starts at */
    char *filename; /* file template is in */
    short argcount;
    short tplargcount;
    short partials;
    short *lines;
    int linemax, linelen;
    char hasbody; /* has a body? */
};

typedef struct ifunc
{
    SNODE *stmt;
    LIST *syms;
} INLINEFUNC;

typedef struct _filelist
{
    struct _filelist *link;
    char *data;
    int hascode;
} FILELIST;

typedef struct _classlist
{
    struct _classlist *link;
    SYM *data; /* pointer to class def */
    int offset; /* offset to this subclass */
    int vtabsize; /* size of vtab */
    struct sym *vtabsp; /* vtab name for classes */
    LIST *vtablist; /* list of vtab entries for the subclass */
    int vtaboffs; /* offset into the actual vtab of this base */
    int flags;
    #define CL_PRIMARY 1          /* a base class */
    #define CL_SECONDARY 2        /* class directly included in this class */
    #define CL_VTAB 4             /* class has vtab */
    #define CL_TOPCLASS 8         /* a class at offset 0 */
    int mode; /* include mode for class */
} CLASSLIST;

typedef struct _templatelist
{
    struct _templatelist *next;
    char *name; /* we fake out as a SYM for hash tables, so first two fields
        have to match */
    char *prefixname;
    SYM *finalsp;
    SYM *sp;
    LIST *oldtypes;
    LIST *newtypes;
    int partialcount;
};
typedef struct ilist
{
    struct ilist *next;
    SYM *sp;
    SYM *dc;
} ILIST;

typedef struct lastdest
{
    struct lastdst *last;
    struct lastdst *chain;
    ENODE *block;
} LASTDEST;

typedef struct catchblock
{
    struct catchblock *next;
    int label; /* label to execute at */
    ENODE *dummy; /* dummy var to load data to */
    TYP *xtcatch; /* type descriptor */
} CATCHBLOCK;

typedef struct tryblock
{
    struct tryblock *next;
    struct catchblock *catchit;
    int startlab;
    int endlab;
    int catchendlab;
} TRYBLOCK;

typedef struct rtti_list
{
    struct rtti *next;
    SYM *label;
    TYP *tp;
} RTTI_LIST;

typedef struct _blk_
{
    struct _blk_ *next;
    long blksize;
    char m[1]; /* memory area */
} BLK;

#define REPLACED_TOKENIZING 0xb0
#define TOKENIZING_PLACEHOLDER 0xb1
#define STRINGIZING_PLACEHOLDER 0xb2
#define REPLACED_ALREADY 0xb3
#define MACRO_PLACEHOLDER 0xb4
#include <ctype.h>
#include "utype.h"
#include "gen386.h"
#include "cc.p"
#define strdup _strdup
#define spawnlp _spawnlp
#define open _open
#define close _close
#define unlink _unlink
#define getcwd _getcwd
#define read _read
#define stricmp _stricmp
#ifdef __MSIL__
#undef _setjmp
#define setjmp _setjmp
#endif

#endif