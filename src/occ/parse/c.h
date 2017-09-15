/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
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
/*      compiler header file    */


#define CI_CONSTRUCTOR 0
#define CI_DESTRUCTOR 1
#define CI_CAST 2
#define CI_NEW 3
#define CI_DELETE 4
#define CI_FUNC (openpa+3)
#define CI_NEWA (compl+1+3)
#define CI_DELETEA (compl+2+3)
#define CI_LIT (compl + 3+3)

#define issymchar(x) (((x) >= 0) && (isalnum(x) || (x) == '_'))
#define isstartchar(x) (((x) >= 0) && (isalpha(x) || (x) == '_'))

#define GENREF(sym) { sym->genreffed = TRUE; if (sym->mainsym) sym->mainsym->genreffed = TRUE; }

#define STD_PRAGMA_FENV 1
#define STD_PRAGMA_FCONTRACT 2
#define STD_PRAGMA_CXLIMITED 4

#define MACRO_REPLACE_SIZE (128 * 1024)

#define basetype(x) ((x) && (x)->rootType ? (x)->rootType : (x))


#define __isref(x) ((x)->type == bt_lref || (x)->type == bt_rref)
#define isref(x) (__isref(basetype(x)) || \
                 (x)->type == bt_templateparam && \
                 (x)->templateParam->p->type == kw_int && \
                 __isref((x)->templateParam->p->byNonType.tp))

#define __ispointer(x) ((x)->type == bt_pointer || (x)->type == bt_seg)
#define ispointer(x) (__ispointer(basetype(x)) || \
                 (x)->type == bt_templateparam && \
                 (x)->templateParam->p->type == kw_int && \
                 __ispointer((x)->templateParam->p->byNonType.tp))

#define __isfunction(x) ((x)->type == bt_func || (x)->type == bt_ifunc)
#define isfunction(x) (__isfunction(basetype(x)))

#define isfuncptr(x) (ispointer(x) && basetype(x)->btp && isfunction(basetype(x)->btp))
#define __isstructured(x) ((x)->type == bt_class || (x)->type == bt_struct || (x)->type == bt_union)
#define isstructured(x) (__isstructured(basetype(x)))

typedef struct
{
    LCHAR *str;
    int count;
} SLCHAR;
/* keywords and symbols */
enum e_kw
{
    /* first comes all the C++ overloadable operators */
    /* be sure to change expectlist if you insert things */
    kw_new, kw_delete, plus, minus, star, divide, leftshift, rightshift, mod, eq,
        neq, lt, leq, gt, geq, assign, asplus, asminus, astimes, asdivide,
        asmod, asleftshift, asrightshift, asand, asor, asxor, autoinc, autodec,
        openbr, openpa, pointstar, pointsto, comma, lor, land, not, or, and, uparrow,
        compl, kw_newa, kw_dela, quot,
        
    /* then generic stuff that isn't overloadable or is internal */
    id, hook, colon, begin, end, dot,
        closebr, closepa, ellipse, semicolon, hash, classsel,
        dotstar, lassign, 
    /* Finally tokens for keywords */
    kw_int, kw_void, kw_char, kw_float, kw_double, kw_struct, kw_union, kw_long,
        kw_short, kw_unsigned, kw_signed, kw_auto, kw_extern, kw_register,
        kw_typedef, kw_static, kw_goto, kw_return, kw_sizeof, kw_break,
        kw_continue, kw_if, kw_else, kw_for, kw_do, kw_while, kw_switch,
        kw_case, kw_default, kw_enum, kw_volatile, kw_const,
        kw__trap, kw__interrupt, kw__fault, kw__absolute, kw__genword,
        kw__Complex, kw_restrict, kw__Imaginary, kw___I, kw__INF, kw__NAN, 
        kw_typeof, kw__Pragma, kw_atomic, kw_noreturn, kw_generic,
    /* C++ */
    kw_public, kw_private, kw_protected, kw_class, kw_friend, kw_namespace,
        kw_using, kw_this, kw_operator, kw_inline, kw_virtual, kw_try, kw_throw,
        kw_catch, kw_template, kw_bool, kw_true, kw_false, kw_wchar_t,
        kw_static_cast, kw_dynamic_cast, kw_reinterpret_cast, kw_const_cast,
        kw_typeid, kw_typename, kw_explicit, kw_decltype,
        kw_export, kw_static_assert, kw_alignas, kw_alignof, kw_char16_t,
        kw_char32_t, kw_mutable, kw_nullptr, kw_noexcept, kw_thread_local, kw_constexpr,
        kw_rangefor, 
    /* Extended */
    kw_atomic_flag_test_set, kw_atomic_flag_clear, kw_atomic_fence, kw_atomic_kill_dependency,
        kw_atomic_load, kw_atomic_store, kw_atomic_modify, kw_atomic_cmpswp, kw_atomic_var_init,
        kw__pascal, kw__stdcall, kw__cdecl, kw__intrinsic, kw_asm, kw__loadds,
        kw__far, kw_asmreg, kw_asminst, kw__indirect, kw__export, kw__import, kw___func__,
        kw__near, kw__seg, kw___typeid, kw___int64, kw_alloca, kw__msil_rtl,
        kw___va_list__,  kw___va_typeof__, kw__unmanaged,  kw__uuid, kw__uuidof,
        kw___string, kw___object,  kw_native, kw__cpblk, kw__initblk, kw__property,  kw__entrypoint,
    /* These next are generic register names */
    kw_D0, kw_D1, kw_D2, kw_D3, kw_D4, kw_D5, kw_D6, kw_D7, kw_D8, kw_D9, kw_DA,
        kw_DB, kw_DC, kw_DD, kw_DE, kw_DF, kw_A0, kw_A1, kw_A2, kw_A3, kw_A4,
        kw_A5, kw_A6, kw_A7, kw_A8, kw_A9, kw_AA, kw_AB, kw_AC, kw_AD, kw_AE,
        kw_AF, kw_F0, kw_F1, kw_F2, kw_F3, kw_F4, kw_F5, kw_F6, kw_F7, kw_F8,
        kw_F9, kw_FA, kw_FB, kw_FC, kw_FD, kw_FE, kw_FF, kw_cr0, kw_cr1, kw_cr2,
        kw_cr3, kw_cr4, kw_cr5, kw_cr6, kw_cr7, kw_dr0, kw_dr1, kw_dr2, kw_dr3,
        kw_dr4, kw_dr5, kw_dr6, kw_dr7, kw_tr0, kw_tr1, kw_tr2, kw_tr3, kw_tr4,
        kw_tr5, kw_tr6, kw_tr7, eol, kw_none
};

enum ovcl 
{
    ovcl_unary_numeric, ovcl_unary_numericptr, ovcl_unary_int,
    ovcl_unary_prefix, ovcl_unary_postfix, 
    ovcl_unary_pointer, ovcl_unary_any, ovcl_binary_any,
    ovcl_binary_numericptr, ovcl_binary_numeric, ovcl_binary_int, 
    ovcl_assign_any, ovcl_assign_numericptr, ovcl_assign_numeric, ovcl_assign_int,
    ovcl_pointsto, ovcl_openbr, ovcl_openpa, ovcl_comma
};
typedef struct
{ 
    FPF r;
    FPF i;
} _COMPLEX_S;

enum e_node
{

    en_void, en_not_lvalue, en_lvalue, en_argnopush, en_voidnz, en_shiftby, 
    en_global, en_auto, en_labcon, en_absolute, en_pc, en_label, en_const, en_threadlocal,
    en_c_bit, en_c_bool, en_c_c, en_c_uc, en_c_wc, en_c_s, en_c_u16, en_c_us, en_c_i, en_c_ui, 
    en_c_u32, en_c_l, en_c_ul, en_c_ll, en_c_ull, en_c_f, en_c_d, en_c_ld,
        en_c_p, en_c_sp, en_c_fp, en_c_fc, en_c_dc, en_c_ldc,
        en_c_fi, en_c_di, en_c_ldi, en_x_bool, en_x_bit, 
        en_c_string,
        en_x_i, en_x_ui, en_x_l, en_x_ul, en_x_inative, en_x_unative,
        en_x_ll, en_x_ull, en_x_f, en_x_d, en_x_ld, en_x_fi, en_x_di, en_x_ldi, en_x_fp, en_x_sp,
        en_x_fc, en_x_dc, en_x_ldc,en_x_c, en_x_uc, en_x_wc, en_x_u16, en_x_u32, en_x_s, en_x_us, en_x_label, 
        en_x_string, en_x_object,
        en_l_bool, en_l_c, en_l_uc, en_l_u16, en_l_u32, en_l_wc, en_l_s, en_l_us, en_l_i, en_l_ui,
        en_l_inative, en_l_unative,
        en_l_l, en_l_ul, en_l_ll, en_l_ull, en_l_f, en_l_d, en_l_ld,  en_l_p, en_l_ref,
        en_l_fi, en_l_di, en_l_ldi, en_l_fc, en_l_dc, en_l_ldc, en_l_fp, en_l_sp, en_l_bit,
        en_l_string, en_l_object, en_msil_array_access, en_msil_array_init,
        en_nullptr, en_memberptr, en_mp_as_bool, en_mp_compare,
        en_trapcall, en_func, en_funcret, en_intcall, en_tempref, 
        en_arraymul, en_arraylsh, en_arraydiv, en_arrayadd, en_structadd, en_structelem,
        en_add, en_sub, en_mul, en_mod, en_div, en_lsh, en_rsh, en_ursh,
        en_cond, en_assign, en_eq, en_ne, 
        en_uminus, en_not, en_compl, en_ascompl, en_lt, en_le, en_gt, en_ge,
        en_and, en_or, en_land, en_lor, en_xor, en_umul, en_autoinc, en_autodec,
        en_udiv, en_umod, en_ugt, en_uge, en_ule, en_ult, en_blockclear, en_stackblock, 
        en_blockassign, en_rshd, en_bits,
        en_imode, en_x_p, en_substack, en_alloca, en__cpblk, en__initblk,
        en_loadstack, en_savestack, en_stmt, en_atomic, en_placeholder, en_thisshim, en_thisref,
        en_literalclass, en_templateparam, en_templateselector, en_packedempty, en_sizeofellipse,
        en_type
};
/*      statement node descriptions     */

enum e_stmt
{
    st_line, st_nop, st_expr, st_declare, st_goto, st_asmgoto, st_asmcond, 
    st_loopgoto, st_select, st_notselect, st_varstart, st_dbgblock,
    st_switch, st_return, st_block, st_throw, st_try, st_catch,
    st__genword, st_passthrough, st_datapassthrough, st_abs, st_label
};

/* storage classes */
enum e_sc
{
        sc_none, sc_static, sc_localstatic, sc_auto, sc_register, sc_global, sc_external, sc_templateparam,
        sc_parameter, sc_catchvar, sc_type, sc_typedef, sc_member, sc_mutable, sc_cast, sc_defunc, sc_label, sc_ulabel,
        sc_overloads, sc_constant, sc_enumconstant, sc_absolute,
        sc_friendlist, sc_const, sc_tconst, sc_classmember, sc_constexpr,
           sc_memberreg, sc_namespace, sc_namespacealias, sc_temp, sc_virtual
};

/* basic types */
enum e_bt
{
    /* keep this ordering and dont insert anything before the end of the
     * basic types, type comparisons (LOSTCONV) depends on the ordering,
     * and the debug info has a table indexed by type
     */
    bt_bit, bt_bool, bt_signed_char, bt_char, bt_unsigned_char, bt_short, bt_char16_t, bt_unsigned_short, 
    bt_wchar_t, bt_enum, bt_int, bt_inative, bt_char32_t, bt_unsigned, bt_unative, bt_long, bt_unsigned_long, bt_long_long,
        bt_unsigned_long_long, bt_float, bt_double, bt_long_double, bt_float_imaginary,
        bt_double_imaginary, bt_long_double_imaginary, bt_float_complex, 
        bt_double_complex, bt_long_double_complex,
    /* end of basic types */
    bt_void, bt___object, bt___string, 
    /* end of debug needs */
    bt_signed, bt_static, bt_atomic, bt_const, bt_volatile, bt_restrict, bt_far, bt_near, bt_seg,
    bt_aggregate, bt_untyped, bt_typedef, bt_pointer, bt_lref, bt_rref, bt_lrqual, bt_rrqual, bt_struct,
        bt_union, bt_func, bt_class, bt_ifunc, bt_any, bt_auto,
        bt_match_none, bt_ellipse, bt_memberptr, bt_cond, bt_va_list, bt_objectArray,
        bt_consplaceholder, bt_templateparam, bt_templateselector, bt_templatedecltype, bt_derivedfromtemplate, bt_string, 
        bt_templateholder,
        /* last */
        bt_none
};
enum e_lk { lk_none, lk_cdecl, lk_pascal, lk_stdcall, lk_c, lk_cpp,
    lk_interrupt, lk_fault, lk_inline, lk_virtual, lk_noreturn, lk_threadlocal, 
    lk_import, lk_export, lk_auto, lk_msil_rtl, lk_unmanaged, lk_property, lk_entrypoint };
    
enum e_ac { ac_private, ac_protected, ac_public, ac_none };

enum e_cvsrn
{
    // tier 1
    CV_IDENTITY,
    CV_LVALUETORVALUE,
    CV_ARRAYTOPOINTER,
    CV_FUNCTIONTOPOINTER,
    CV_QUALS,
    CV_INTEGRALPROMOTION,
    CV_FLOATINGPROMOTION,
    CV_INTEGRALCONVERSION,
    CV_FLOATINGCONVERSION,
    CV_FLOATINGINTEGRALCONVERSION,
    CV_POINTERCONVERSION,
    CV_POINTERTOMEMBERCONVERSION,
    CV_BOOLCONVERSION, 
    CV_ENUMINTEGRALCONVERSION,
    CV_DERIVEDFROMBASE,
    // tier 2
    CV_USER,
    // tier 3
    CV_ELLIPSIS,
    // other
    CV_PAD,
    CV_AMBIGUOUS,
    CV_NONE,
} ;

#define _F_AMPERSAND 1
#define _F_PACKABLE 2
#define _F_SELECTOR 4
#define _F_INTEMPLATEPARAMS 8
#define _F_INARGS 16
#define _F_SIZEOF 32
#define _F_INITLIST 64
#define _F_TEMPLATEARGEXPANSION 128
#define _F_TYPETEST 256

#define _F_NOVIRTUALBASE 1
#define _F_VALIDPOINTER 2

#define _F_NOCHECKAUTO 0x80

typedef struct expr
{
    struct expr *left, *right;
    enum e_node type;
    int pragmas;
    long size; /* For block moves */
    long altdata;
    union
    {
        LLONG_TYPE i;
        FPF f;
        _COMPLEX_S c;
        struct sym *sp; /* sym will be defined later */
        char *name; /* name during base class processing */
        struct functioncall *func;
        struct _atomicData *ad;
        struct stmt *stmt;
        struct _imode_ *imode;
        struct _templateSelector *templateSelector;
        struct _templateParamList *templateParam;
        struct _msilarray *msilArray;
        HASHTABLE *syms;
        struct typ *tp;
        struct {
            struct expr *thisptr;
            struct typ *tp;
        } t;
    } v;
    struct _string *string;
    LIST *destructors; // for &&  and ||
    int xcInit, xcDest;
    int lockOffset;
    char bits;
    char startbit;
    int isvolatile:1;
    int isrestrict:1;
    int isatomic:1;
    int atomicinit:1;
    int unionoffset:1;
    int isfunc:1;
    int dest:1; // for thisref
    int noexprerr: 1;
} EXPRESSION;

typedef struct _msilarray
{
    int count;
    int max;
    struct typ *tp;
    EXPRESSION *base;
    EXPRESSION *indices[1]; // expands
} MSIL_ARRAY;

typedef struct
{
    char *name; // must be first as it will go in a hashtable
    EXPRESSION *exp;
    struct sym *sym;
} CONSTEXPRSYM;

typedef struct casedata
{
    struct casedata *next;
    LLONG_TYPE val;
    int label;
    char *file;
    int line;
} CASEDATA;

union u_val    {
        LLONG_TYPE i; /* int val */
        ULLONG_TYPE u; /* nsigned val */
        FPF f; /* float val */
        _COMPLEX_S c;
        union
        {
            char *a; /* string val */
            LCHAR *w;
        } s;
        struct _defstruct *defs ; /* macro definition */
    } ;
typedef    struct typ
    {
        enum e_bt type; /* the type */
        long size; /* total size of type */
        struct typ *btp; /* pointer to next type (pointers & arrays */
        struct typ *rootType; /* pointer to base type of sequence */
        int used:1; /* type has actually been used in a declaration or cast or expression */
        int array:1; /* not a dereferenceable pointer */
        int msil:1; /* allocate as an MSIL array */
        int vla:1;   /* varriable length array */
        int unsized:1; /* type doesn't need a size */
        int hasbits:1; /* type is a bit type */
        int anonymousbits:1; /* type is a bit type without a name */
        int scoped:1; /* c++ scoped enumeration */
        int fixed:1; /* c++ fixed enumeration */
        int nullptrType:1; /* c++: std::nullptr */
        int templateTop : 1;
        int templateConst : 1;
        int templateVol : 1;
        int enumConst:1; /* is an enumeration constant */
        int lref:1;
        int rref:1;
        int decltypeauto : 1;
        int decltypeautoextended : 1;
        char bits; /* -1 for not a bit val, else bit field len */
        char startbit; /* start of bit field */
        struct sym *sp; /* pointer to a symbol which describes the type */
        /* local symbol tables */
        HASHTABLE *syms; /* Symbol table for structs & functions */
        HASHTABLE *tags; /* Symbol table for nested types*/
        struct _templateParamList *templateParam;
        int dbgindex; /* type index for debugger */
        int alignment; /* alignment pref for this structure/class/union   */
        EXPRESSION *esize; /* enode version of size */
        struct typ *etype; /* type of size field  when size isn't constant */
        int vlaindex; /* index into the vararray */
        EXPRESSION *templateDeclType; /* for bt_templatedecltype, used in templates */
    } TYPE;
typedef struct _linedata
{
    struct _linedata *next, *stmtNext;
    char *line;
    char *file;
    int lineno;
    int fileindex;
} LINEDATA;

typedef struct stmt
{
    struct stmt *next;
    struct stmt *lower;
    struct stmt *blockTail;
    enum e_stmt type;
    EXPRESSION *select;
    EXPRESSION *destexp;
    LINEDATA *lineData;
    union {
        TYPE *tp;
        CASEDATA *cases;
        struct blockdata *parent;
    };
    struct sym *sym;
    int blocknum;
    int charpos;
    int line;
    char *file;
    int label;
    int endlabel;
    int breaklabel; // also the label at the end of the try block
    int altlabel;
    int tryStart;
    int tryEnd;
    int hasvla: 1;
    int hasdeclare: 1;
    int purelabel: 1;
} STATEMENT;

typedef struct blockdata
{
    struct blockdata *next;
    struct blockdata *caseDestruct;
    enum e_kw type;
    CASEDATA *cases;
    STATEMENT *head, *tail;
    STATEMENT *blockTail;
    HASHTABLE *table;
    int breaklabel;
    int continuelabel;
    int defaultlabel;
    int needlabel:1;
    int hasbreak:1;
    int hassemi:1;
    int nosemi:1; /* ok to skip semi */
    int lastcaseordefault:1;
} BLOCKDATA;

typedef struct init
{
    struct init *next;
    int offset;
    TYPE *basetp;
    struct sym *fieldsp;
    int fieldoffs;
    EXPRESSION *exp;
    int tag; /* sequence number */
    int noassign : 1;    
} INITIALIZER;
    
typedef struct ifunc
{
    STATEMENT *stmt;
    HASHTABLE *syms;
    HASHTABLE *tags;
} INLINEFUNC;

typedef struct __nsv
{
    struct __nsv *next;
    HASHTABLE *syms;
    HASHTABLE *tags;
    LIST *usingDirectives;    
    LIST *inlineDirectives;    
    struct sym *origname;
    struct sym *name;
} NAMESPACEVALUES;

#ifdef PARSER_ONLY
struct _ccNamespaceData
{
    struct _ccNamespaceData *next;
    char * declfile;
    int startline;
    int endline;
};
#endif
enum e_cm { cmNone, cmValue, cmRef, cmThis, cmExplicitValue };
/* symbols */
typedef struct sym
{
    char *name;
    char *decoratedName; /* symbol name with decorations, as used in output format */
    char *errname; /* name to be used in errors */
    char *declfile, *origdeclfile ; /* file symbol was declared in */
    int  declline, origdeclline ; /* line number symbol was declared at */
    short declcharpos ; /* character position symbol was declared at */
    short declfilenum; /* the file number */
    int sizeNoVirtual; /* size without virtual classes and thunks */
    struct sym *parent;
    struct sym *parentClass;
    struct sym *parentNameSpace;
    struct sym *vtabsp;
    EXPRESSION *localInitGuard;
    NAMESPACEVALUES *nameSpaceValues; /* for a namespace SP */
    LINEDATA *linedata;
    enum e_sc storage_class; /* storage class */
    enum e_lk linkage; /* cdecl, pascal, stdcall, inline */
    enum e_lk linkage2; /* export, import */
    enum e_lk linkage3; /* noreturn */
    enum e_ac access; /* c++ access rights for members */
    int operatorId; /* operator id, CI + kw for an operator function */
    struct _imode_ *imaddress;
    struct _imode_ *imvalue;
    struct _im_list *imind;
    struct _imode_ *imstore;
    enum e_cm lambdaMode;
    INLINEFUNC inlineFunc;
    int overlayIndex;        /* differentiating index when function differs only in return type from similar functions */
#ifdef PARSER_ONLY
    int      ccEndLine;      /* end line for code completion */
    ULLONG_TYPE   ccStructId;     /* code completion struct id */
    struct  _ccNamespaceData *ccNamespaceData; /* namespace data for code completion */
    int     parserSet: 1;      /* sent to parser already*/
#endif
    unsigned declaring: 1; /* currently being declared */
    unsigned compilerDeclared: 1; /* compiler declared this */
    unsigned hasproto: 1; /* C/90 language prototype was encountered */
    unsigned intagtable: 1; /* it is in a tag table */
    unsigned dontlist: 1; /* it is a system include, don't put in list file */
    unsigned allocate: 1; /* variable is used, allocate space for it */
    unsigned inAllocTable: 1; /* auto temp var is in the allocation table already */
    unsigned indecltable: 1; /* global already in dump table */
    unsigned spaceallocated: 1; /* space has been allocated */
    unsigned regmode: 2; /* 0 = pure var, 1 = addr in reg, 2 = value in reg*/
    unsigned loadds: 1; /* to load data seg (limited) */
    unsigned farproc: 1; /* this procedure should be terminated with retf */
    unsigned calleenearret: 1; /* true if callee provided a place for the return
        value */
    unsigned hasunnamed: 1; /* structure has unnamed substructs */
    unsigned isunnamed: 1; /* substructure is unnamed */
    unsigned recalculateParameters: 1; /* inline func needs its parameters
        recalculated */
    unsigned nullsym: 1; /* if was a callblock return which isn't used */
        unsigned pushedtotemp: 1; /* if a local variable has been transformed to a temp */
        unsigned anonymous: 1; /* if it is a generated variable */
        unsigned usedasbit: 1; /* used in a bit field op */
        unsigned inasm: 1; /* a way to force the local optimizer to leave autos on the stack */
        unsigned assigned: 1; /* value has been assigned */
        unsigned altered: 1;
        unsigned used: 1; /* value has been fetched */
        unsigned genreffed: 1; /* reffed in codegen */
        unsigned noextern:1; /* no external reference needed, it was inlined */
        unsigned gentemplate: 1; /* template instantiation or reference generated */
        unsigned allocaUsed: 1;
        unsigned oldstyle : 1; /* pointer to a names list if an old style function arg */
        unsigned spillVar : 1; /* backend allocator spill variable */
        unsigned noGlobal : 1; /* no global opts on this temp var */
        unsigned storeTemp : 1; /* is a storetemp */
        unsigned loadTemp : 1; /* is a loadtemp */
        unsigned visited : 1; /* temproary which means it is visited */
        unsigned constexpression : 1; /* declared with constexpression */
        unsigned addressTaken : 1; /* address taken */
        unsigned wasUsing : 1; /* came to this symbol table as a result of 'using' */
        unsigned redeclared : 1; /* symbol was declared more than once */
        unsigned thisPtr: 1; /*is a this pointer*/
        unsigned structuredReturn: 1; /* is a pointer to a structure's structure pointer address for returning a value */
        unsigned constop:1; /* a constructor 'top' parameter */
        unsigned castoperator:1; /* a cast operator */
        unsigned deleted : 1; /* function was deleted */
        unsigned defaulted : 1; /* function was defaulted */
        unsigned isfinal :1 ; /* class or virtual function is final */
        unsigned isoverride : 1; /* virtual function marked override */
        unsigned ispure : 1; /* pure virtual function */
        unsigned hasvtab : 1; /* class has a vtab */
        unsigned isabstract:1; /* class is abstract */
        unsigned accessspecified : 1; /* class has access specifiers */
        unsigned safefunc : 1; /* RTL helper function with no side effects */
        unsigned throughClass : 1; /* last search was found through a class */
        unsigned hasUserCons : 1; /* has user-defined constructors */
        unsigned trivialCons : 1; /* constructor is trivial */
        unsigned internallyGenned : 1; /* constructor declaration was made by the compiler */
        unsigned stackblock : 1; // stacked structure in C++ mode
        unsigned islambda : 1; // lambda closure struct
        unsigned noinline :1; // don't inline an inline qualified function
        unsigned didinline :1; // already genned an inline func for this symbol
        unsigned hasTry : 1; // function surrounded by try statement
        unsigned hasDest: 1; // class has a destructor that is called
        unsigned pureDest: 1; // destructor is pure
        unsigned inCatch:1; // used inside a catch block
        unsigned isConstructor:1; // is a constructor
        unsigned isDestructor:1; // is  adestructor
        unsigned xtEntry:1; // is an exception table label
        unsigned isExplicit:1; // explicit constructor or conversion function
        unsigned specialized:1; // is a template specialization
        unsigned packed:1; // packed template param instance
        unsigned instantiated:1; // instantiated template
        unsigned instantiated2:1; // instantiated template
        unsigned dontinstantiate:1; // don't instantiate this template (is extern)
        unsigned copiedTemplateFunction:1;
        unsigned performedStructInitialization:1; // function performed structured initialization
        unsigned instantiatedInlineInClass :1; // function instantiated inside a class body
        unsigned isInline;// : 1; /* function is a candidate for inline functionality */
        unsigned dumpInlineToFile : 1; /* inline function needs to be placed in the output file */
        unsigned promotedToInline : 1; /* function wasn't declare inline but was promoted to it */
        unsigned temp : 1; // temporary boolean...  
        unsigned noCoalesceImmed : 1; // set to true if temp or memory address which references an immediate is used
                                     // other than as the immediate reference      
        unsigned pushedTemplateSpecializationDefinition: 1;  // set to true if the current body for the template
                                     // specialization was pushed from the generalized version of the template
        unsigned destructed:1;  // the c++ class instance has had a destructor generated
        unsigned initializer_list:1; // constructor with initializer_list parameter
        unsigned va_typeof:1; // MSIL: a va_typeof symbol
        unsigned retemp:1; // retemp has already been performed on this SP
        unsigned has_property_setter : 1; // a property has a setter
        unsigned nonConstVariableUsed; // a non-const variable was used or assigned to in this function's body
        char *deprecationText; // C++ declaration was deprecated
        int __func__label; /* label number for the __func__ keyword */
        int ipointerindx; /* pointer index for pointer opts */
    int labelCount; /* number of code labels within a function body */ 
    int nextid; /* ID to use for nextage purposes (binary output) */
    int offset; /* address offset of data in the given seg, or optimize register */
    int vtaboffset; /* vtab offset for virtual functions */
    int label; /* label number for statics */
    int startLine, endLine; /* line numbers spanning the function */
    short paramsize; /* Size of parameter list for stdcall functions */
    short structAlign; /* alignment of structures/ unions */
    short accessibleTemplateArgument; /* something used as a template argument was validated for
                                        * accessibility before instantiating the template */
    int retcount; /* number of return statements in a function */
    /* Also name for CPP overload lists */
    /* also default for template parameters, is a TYP */
    char *importfile; /* import name */
    unsigned char *uuid; /* Microsoft: GUID */
    int uuidLabel; /* Microsoft: Label for a GUID which has been instantiated */
    struct sym *overloadName;
    struct sym *typedefSym;
    struct sym *mainsym; /* pointer to the global version of a copied symbol */
    struct sym *maintemplate; /* pointer to the global version of a copied symbol */
    struct _memberInitializers *memberInitializers; /* initializers for constructor */
    STATEMENT *gotoTable; /* pointer to hashtable associated with goto or label */
    /* these fields depend on storage_class */
    union u_val value;
    struct _baseClass *baseClasses;
    struct _vbaseEntry *vbaseEntries;
    struct _vtabEntry *vtabEntries;
    struct lexeme *deferredTemplateHeader ;
    struct lexeme *deferredCompile ;
    struct _templateParamList *templateParams;
    LIST *templateNameSpace;
    LIST *staticAsserts;
    short templateLevel;
    LIST *specializations;
    LIST *instantiations;
    void *msil; // MSIL data
    struct _templateSelector *templateSelector; // first element is the last valid sym found, second element is the template parameter sym
                                // following elements are the list of pointers to names
    struct sym *parentTemplate; // could be the parent of a specialization or an instantiation
    struct init * init, *lastInit, *dest;
    // order is important for this next, a comparison is done based on this ordering
    enum e_xc { xc_unspecified, xc_all, xc_dynamic, xc_none } xcMode;
    struct xcept
    {
        LIST *xcDynamic; // list of types, the exception specification when dynamic
        int xcInitLab, xcDestLab; // for auto vars
        struct sym *xctab;
        struct sym *xclab;
        EXPRESSION *xcInitializeFunc;
        EXPRESSION *xcRundownFunc;
    } *xc;
    LIST *friends;
    /* Type declarations */
    struct typ *tp;
} SYMBOL;

typedef struct __lambda
{
    struct __lambda *prev, *next;
    enum e_cm captureMode;
    HASHTABLE *captured;
    SYMBOL *cls;
    SYMBOL *func;
    SYMBOL *lthis;
    TYPE *functp;
    HASHREC *funcargs;
    SYMBOL *enclosingFunc;
    HASHTABLE *oldSyms;
    HASHTABLE *oldTags;
    TYPE *rv;
    int index;
    int isMutable : 1;
    int captureThis : 1;
    int templateFunctions : 1;
} LAMBDA;

typedef struct __lambdasp
{
    char *name;
    SYMBOL *sym;
    SYMBOL *parent;
    LAMBDA *enclosing;
} LAMBDASP;

typedef struct _memberInitializers
{
    struct _memberInitializers *next;
    char *name;
    SYMBOL *sp;
    INITIALIZER *init;
    int line;
    char *file;
    struct lexeme *initData;
    int packed : 1;
    int delegating : 1;
} MEMBERINITIALIZERS;

typedef struct _baseClass
{
    struct _baseClass *next;
    SYMBOL *cls;
    enum e_ac accessLevel;
    unsigned offset;
    int isvirtual:1;
    int top:1;
} BASECLASS;
typedef struct _virtualFunc
{
    struct _virtualFunc *next;
    SYMBOL *func;
} VIRTUALFUNC;
typedef struct _vtabEntry
{
    struct _vtabEntry *next;
    struct _vtabEntry *children;
    struct _virtualFunc *virtuals;
    SYMBOL *cls;
    unsigned dataOffset;
    unsigned vtabOffset;
    BOOLEAN isvirtual;
    BOOLEAN isdead;
} VTABENTRY;
typedef struct _vbaseEntry
{
    struct _vbaseEntry *next;
    SYMBOL *cls;
    BOOLEAN alloc;
    unsigned pointerOffset;
    unsigned structOffset;
} VBASEENTRY;


typedef struct _templateParam
{
        // kw_class = class or namespace
        // kw_int = nontype
        // kw_template = template parameter
        // kw_new = specialization
        // first in the list is always the specialization specifier
    enum e_kw type;
    int index:8;
    int packed:1;
    int usedAsUnpacked : 1;
    int initialized:1;
    int lref:1;
    int rref:1;
    SYMBOL *packsym;
    void *hold; /* value held during partial template ordering */
    union {
        // the dflt & val fields must be in the same place for each item
        struct {
            SYMBOL *dflt;
            SYMBOL *val;
            struct lexeme *txtdflt;
            LIST *txtargs;
            SYMBOL *temp;
            struct _templateParamList *args;
            struct _templateParamList *orig;
        } byTemplate;
        struct {
            TYPE *dflt;
            TYPE *val;
            struct lexeme *txtdflt;
            LIST *txtargs;
            TYPE *temp;
        } byClass;
        struct {
            EXPRESSION *dflt;
            EXPRESSION *val;
            struct lexeme *txtdflt;
            LIST *txtargs;
            EXPRESSION *temp;
            TYPE *tp;
        }byNonType;  
        struct {
            struct _templateParamList *types;
            struct _templateParamList *next;
        }bySpecialization;
        struct {
            struct _templateParamList *pack;
        }byPack;
        struct {
            struct _templateParamList *args;
        }byDeferred;
    };
} TEMPLATEPARAM;

typedef struct _templateParamList
{
    struct _templateParamList *next;
    SYMBOL *argsym;
    TEMPLATEPARAM *p;
} TEMPLATEPARAMLIST;

typedef struct _templateSelector
{
    struct _templateSelector *next;
    union
    {
        SYMBOL *sym;
        char *name;
    } ;
    TEMPLATEPARAMLIST *templateParams;
    int isTemplate : 1;
} TEMPLATESELECTOR ;



typedef struct _structSym
{
    struct _structSym *next;
    SYMBOL *str;
    TEMPLATEPARAMLIST *tmpl;
} STRUCTSYM;
typedef struct initlist
{
    struct initlist *next;
    TYPE *tp;
    EXPRESSION *exp;
    EXPRESSION *dest;
    struct initlist *nested;
    int byRef : 1;
    int packed: 1;
    int vararg: 1;
    int valist: 1;
} INITLIST;

typedef struct functioncall
{
    SYMBOL *sp;
    TYPE *functp;
    EXPRESSION *fcall;
    INITLIST *arguments;
    SYMBOL *returnSP;
    EXPRESSION *returnEXP;
    EXPRESSION *thisptr;
    TYPE *thistp;
    TEMPLATEPARAMLIST *templateParams;
    int callLab;
    int novtab : 1;
    int ascall:1;
    int astemplate:1;
    int noobject:1;
    int asaddress:1;
    int vararg:1;
} FUNCTIONCALL;

#define MAX_STRLEN      16384
#define MAX_STLP1       (MAX_STRLEN + 1)

/* error list */
struct errl
{
    struct errl *next;
    int errornumber;
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

typedef struct kwblk
{
    char *name;
    int len;
    enum e_kw key;
    enum
    {
        KW_NONE = 0, KW_CPLUSPLUS = 1, KW_INLINEASM = 2, KW_NONANSI = 4, KW_C99 = 8, 
        KW_C1X = 16, KW_ASSEMBLER = 32, KW_MSIL = 64,
        KW_386 = 128, KW_68K= 256, KW_ALL = 0x40000000
    } matchFlags;
    enum 
    {
        TT_BASE = 1, TT_BOOL=2, TT_INT = 4, TT_FLOAT= 8, TT_COMPLEX = 16, 
        TT_TYPEQUAL = 32, TT_POINTERQUAL = 64, TT_UNARY = 128, TT_BINARY = 0x100,
        TT_OPERATOR = 0x200, TT_ASSIGN = 0x400, TT_RELATION = 0x800, TT_EQUALITY = 0x1000,
        TT_INEQUALITY = 0x2000, TT_POINTER = 0x4000, TT_STORAGE_CLASS = 0x8000,
        TT_CONTROL = 0x10000, TT_BLOCK = 0x20000, TT_PRIMARY = 0x40000, TT_SELECTOR = 0x80000,
        TT_VAR = 0x100000, TT_BASETYPE = 0x200000, TT_INCREMENT = 0x400000,
        TT_SWITCH = 0x800000, TT_ENUM = 0x1000000, TT_STRUCT = 0x2000000, 
        TT_TYPENAME=0x4000000, TT_TYPEDEF = 0x8000000, TT_VOID = 0x10000000, TT_CLASS = 0x20000000,
        TT_LINKAGE = 0x40000000,TT_DECLARE = 0x80000000, TT_UNKNOWN = 0
    } tokenTypes;
/*    ASMNAME *data; */
}KEYWORD;


#define MATCHTYPE(lex, tp) (lex && (lex)->type == (tp))
#define ISID(lex) (lex && (lex)->type == l_id)
#define ISKW(lex) (lex && (lex)->type == l_kw)
#define MATCHKW(lex, keyWord) (ISKW(lex) && ((lex)->kw->key == keyWord))
#define KWTYPE(lex, types) (ISKW(lex) && (((lex)->kw->key == kw_auto ? (cparams.prm_cplusplus ? TT_BASETYPE : TT_STORAGE_CLASS ) :(lex)->kw->tokenTypes) & (types)))
#define KW(lex) (ISKW(lex) ? (lex)->kw->key : kw_none)

typedef struct lexeme
{
    struct lexeme *next, *prev;
    enum e_lexType { l_none, l_i, l_ui, l_l, l_ul, l_ll, l_ull, l_f, l_d, l_ld, l_I, 
            l_id, l_kw, 
            l_astr, l_wstr,  l_ustr, l_Ustr, l_u8str, 
            l_achr, l_wchr, l_uchr, l_Uchr, 
            l_qualifiedname, l_asminst, l_asmreg
         } type;
    union u_val value;
    char *litaslit;
    char *suffix;
    char *file;
    LINEDATA *linedata;
    int line;
    int charindex;
    int filenum;
    KEYWORD *kw;
    SYMBOL *typequal;
    int registered:1;
} LEXEME;

typedef struct lexContext {
    struct lexContext *next;
    LEXEME *cur;
    LEXEME *last;
} LEXCONTEXT ;

struct templateListData
{
    TEMPLATEPARAMLIST *args; // list of templateparam lists
    TEMPLATEPARAMLIST **ptail, **plast;
    LEXEME *head, *tail;
    LEXEME *bodyHead, *bodyTail;
    SYMBOL *sym;
} ;

typedef struct _string
{
    struct _string *next;
    enum e_lexType strtype;
    int size;
    int label;
    int refCount;
    char *suffix;
    SLCHAR **pointers;
} STRING;
#define INCL_LEVEL_MAX 16

#define MAX_INLINE_NESTING 100

#define OPT_RESHAPE 1
#define OPT_LSTRENGTH 2
#define OPT_GLOBAL 4
#define OPT_CONSTANT 8
#define OPT_INVARIANT 0x10


#define OPT_BYTECOMPARE 0x10000
#define OPT_REVERSESTORE 0x20000
#define OPT_REVERSEPARAM 0x40000
#define OPT_ARGSTRUCTREF 0x80000
#define OPT_EXPANDSWITCH 0x100000
#define OPT_THUNKRETVAL  0x200000
// must match stdatomic.h
enum e_mo {
    mo_relaxed=1,
    mo_acquire,
    mo_release,
    mo_acq_rel,
    mo_seq_cst   
} ;

enum e_ao {
    ao_init,
    ao_flag_set_test,
    ao_flag_clear,
    ao_fence,
    ao_load,
    ao_store,
    ao_modify,
    ao_cmpswp
} ;
typedef struct _atomicData
{
    enum e_ao atomicOp;
    EXPRESSION *memoryOrder1;
    EXPRESSION *memoryOrder2;
    EXPRESSION *flg;
    EXPRESSION *address;
    EXPRESSION *value;
    EXPRESSION *third;
    TYPE *tp;
} ATOMICDATA;


#define ATOMIC_FLAG_SPACE getSize(bt_int)
