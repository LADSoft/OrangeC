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

namespace Parser
{
/*      compiler header file    */
#define CopySymbol(x) (CopySymbol)(x, true)
#define CopySymbolfalse(x) (CopySymbol)(x, false)
#define CI_CONSTRUCTOR 0
#define CI_DESTRUCTOR 1
#define CI_CAST 2
#define CI_NEW 3
#define CI_DELETE 4
#define CI_ASSIGN (assign + 3)
#define CI_FUNC (openpa + 3)
#define CI_POINTSTO (pointsto + 3)
#define CI_NEWA (complx + 1 + 3)
#define CI_DELETEA (complx + 2 + 3)
#define CI_LIT (complx + 3 + 3)

#ifndef imax
#    define imax(x, y) ((x) > (y) ? (x) : (y))
#    define imin(x, y) ((x) < (y) ? (x) : (y))
#endif

#define issymchar(x) (((x) >= 0) && (isalnum(x) || (x) == '_') || (x) == '$')
#define isstartchar(x) (((x) >= 0) && (isalpha(x) || (x) == '_') || (x) == '$')

bool IsCompiler();

class StringHash
{
public:
    unsigned operator() (const std::string& aa) const
    {
        unsigned rv = 0;
        const unsigned char *x = (const unsigned char *)aa.c_str();
        for (;*x; ++x)
            rv = (rv << 8) + (rv << 1) + rv + *x;
        return rv;
    }
};
template <class T>
class SymbolTable
{
public:
    typedef typename std::list<T*>::iterator iterator;
    iterator begin() { return inOrder_.begin(); }
    iterator end() { return inOrder_.end(); }
    size_t size() const { return inOrder_.size(); }
    T* back() const { return inOrder_.back(); }
    T* front() const { return inOrder_.front(); }
    inline void remove(iterator it);
    inline iterator insert(iterator it, struct sym* sym);
    inline T* Lookup(const std::string& name) const;
    void Next(SymbolTable<T>* next) { next_ = next; }
    SymbolTable<T>* Next() const { return next_; }
    void Chain(SymbolTable<T>* chain) { chain_ = chain; }
    SymbolTable<T>* Chain() const { return chain_; }
    auto ReleaseNext() { auto rv = next_; if (next_) next_ = next_->next_; return rv; }
    int Block() const { return blockLevel_; }
    void Block(int level) { blockLevel_ = level; }

    inline void Add(T* sym);
    inline T* AddOverloadName(T* sym);
    inline void insertOverload(T* in);
    inline void baseInsert(T* sym);

    inline void AddName(T* sym);
private:
    std::list<T*> inOrder_;
    std::unordered_map<std::string, T*, StringHash> lookupTable_;
    int blockLevel_;
    SymbolTable<T>* next_ = nullptr;
    SymbolTable<T>* chain_ = nullptr;
};

template <class T>
class SymbolTableFactory
{
public:
    SymbolTableFactory() = default;
    void Reset()
    {
        tables.clear();
    }
    SymbolTable<T>* CreateSymbolTable()
    {
        tables.push_back(SymbolTable<T>());
        return &tables.back();
    }
private:
    std::list<SymbolTable<T>> tables;
};


/* keywords and symbols */
// clang-format off
    enum e_kw
    {
        /* first comes all the C++ overloadable operators */
        /* be sure to change expectlist if you insert things */
        kw_new, kw_delete, plus, minus, star, divide, leftshift, rightshift, mod, eq,
        neq, lt, leq, gt, geq, assign, asplus, asminus, astimes, asdivide,
        asmod, asleftshift, asrightshift, asand, asor, asxor, autoinc, autodec,
        openbr, openpa, pointstar, pointsto, comma, lor, land, notx, orx, andx, uparrow,
        complx, kw_newa, kw_dela, quot,
        plus_unary, minus_unary, star_unary, and_unary,
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
        kw_atomic_flag_test_set, kw_atomic_flag_clear, kw_atomic_kill_dependency,
        /* Clang compatibility for atomics */
        kw_c11_atomic_init, kw_c11_atomic_thread_fence, kw_c11_atomic_signal_fence,
        kw_c11_atomic_is_lock_free, kw_c11_atomic_store, kw_c11_atomic_load, // NOTE: clang's atomic_is_lock_free runs on the atomic's size, not the address
        kw_c11_atomic_xchg, kw_c11_atomic_cmpxchg_strong, kw_c11_atomic_cmpxchg_weak,
        kw_c11_atomic_ftchadd, kw_c11_atomic_ftchsub, kw_c11_atomic_ftchand,
        kw_c11_atomic_ftchor, kw_c11_atomic_ftchxor,
        /* GNU atomic support, incomplete but works */
        /* NOTE: __atomic_*_n are able to be mapped to our existing structure without issue except compare_exchange*/
        /* NOTE: __atomic_fetch_* are supported by the same mapping structure __atomic_*_n has */
        kw_atomic_addftch, kw_atomic_subftch, kw_atomic_andftch, kw_atomic_xorftch,
        kw_atomic_orftch, kw_atomic_cmpxchg_n,
        /* Extended */
        kw__pascal, kw__stdcall, kw__fastcall, kw__cdecl, kw__intrinsic, kw_asm, kw__loadds,
        kw__far, kw_asmreg, kw_asminst, kw__indirect, kw__export, kw__import, kw___func__,
        kw__near, kw__seg, kw___typeid, kw___int64, kw_alloca, kw__msil_rtl,
        kw___va_list__, kw___va_typeof__, kw__unmanaged, kw__uuid, kw__uuidof,
        kw___string, kw___object, kw_native, kw__cpblk, kw__initblk, kw__property, kw__entrypoint,
        kw___try, kw___catch, kw___finally, kw___fault, kw__declspec, kw__rtllinkage, kw__attribute,
        kw___offsetof, kw___underlying_type, kw__volatile,
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
// clang-format on

// clang-format off
    enum ovcl
    {
        ovcl_unary_numeric, ovcl_unary_numericptr, ovcl_unary_int,
        ovcl_unary_prefix, ovcl_unary_postfix,
        ovcl_unary_pointer, ovcl_unary_any, ovcl_binary_any,
        ovcl_binary_numericptr, ovcl_binary_numeric, ovcl_binary_int,
        ovcl_assign_any, ovcl_assign_numericptr, ovcl_assign_numeric, ovcl_assign_int,
        ovcl_pointsto, ovcl_openbr, ovcl_openpa, ovcl_comma
    };
// clang-format on

typedef struct
{
    FPF r;
    FPF i;
} _COMPLEX_S;

// clang-format off
    enum e_node
    {

        en_void, en_not_lvalue, en_lvalue, en_argnopush, en_voidnz, en_shiftby,
        en_global, en_auto, en_labcon, en_absolute, en_pc, en_const, en_threadlocal,
        en_c_bit, en_c_bool, en_c_c, en_c_uc, en_c_wc, en_c_s, en_c_u16, en_c_us, en_c_i, en_c_ui,
        en_c_u32, en_c_l, en_c_ul, en_c_ll, en_c_ull, en_c_f, en_c_d, en_c_ld,
        en_c_p, en_c_sp, en_c_fp, en_c_fc, en_c_dc, en_c_ldc,
        en_c_fi, en_c_di, en_c_ldi, en_x_bool, en_x_bit,
        en_c_string,
        en_x_i, en_x_ui, en_x_l, en_x_ul, en_x_inative, en_x_unative,
        en_x_ll, en_x_ull, en_x_f, en_x_d, en_x_ld, en_x_fi, en_x_di, en_x_ldi, en_x_fp, en_x_sp,
        en_x_fc, en_x_dc, en_x_ldc, en_x_c, en_x_uc, en_x_wc, en_x_u16, en_x_u32, en_x_s, en_x_us, en_x_label,
        en_x_string, en_x_object,
        en_l_bool, en_l_c, en_l_uc, en_l_u16, en_l_u32, en_l_wc, en_l_s, en_l_us, en_l_i, en_l_ui,
        en_l_inative, en_l_unative,
        en_l_l, en_l_ul, en_l_ll, en_l_ull, en_l_f, en_l_d, en_l_ld, en_l_p, en_l_ref,
        en_l_fi, en_l_di, en_l_ldi, en_l_fc, en_l_dc, en_l_ldc, en_l_fp, en_l_sp, en_l_bit,
        en_l_string, en_l_object, en_msil_array_access, en_msil_array_init,
        en_nullptr, en_memberptr, en_mp_as_bool, en_mp_compare,
        en_trapcall, en_func, en_funcret, en_intcall,
        en_arraymul, en_arraylsh, en_arraydiv, en_arrayadd, en_structadd, en_structelem,
        en_add, en_sub, en_mul, en_mod, en_div, en_lsh, en_rsh, en_ursh,
        en_cond, en_assign, en_eq, en_ne,
        en_uminus, en_not, en_compl, en_lt, en_le, en_gt, en_ge,
        en_and, en_or, en_land, en_lor, en_xor, en_umul, en_autoinc, en_autodec,
        en_udiv, en_umod, en_ugt, en_uge, en_ule, en_ult, en_blockclear, en_stackblock,
        en_blockassign, en_bits,
        en_imode, en_x_p, en_substack, en_alloca, en__cpblk, en__initblk, en__initobj,  en__sizeof,
        en_loadstack, en_savestack, en_stmt, en_atomic, en_placeholder, en_thisshim, en_thisref,
        en_construct, en_literalclass, en_templateparam, en_templateselector, en_packedempty, en_sizeofellipse,
        en_type, en_pointsto, en_dot, en_select,
        // stuff that can only appear temporarily in constexpr expressions
        en_cshimref, en_cshimthis, en_paramsubstitute
    };
// clang-format on

/*      statement node descriptions     */
// clang-format off
    enum e_stmt
    {
        st_line, st_nop, st_expr, st_declare, st_goto, st_indgoto, st_asmgoto, st_asmcond,
        st_loopgoto, st_select, st_notselect, st_varstart, st_dbgblock,
        st_switch, st_return, st_block, st_throw, st_try, st_catch,
        st__genword, st_passthrough, st_datapassthrough, st_abs, st_label,
        st___try, st___catch, st___finally, st___fault,
    };
// clang-format on

/* storage classes */
// clang-format off
    enum e_sc : int
    {
        sc_none, sc_static, sc_localstatic, sc_auto, sc_register, sc_global, sc_external, sc_templateparam,
        sc_parameter, sc_catchvar, sc_type, sc_typedef, sc_member, sc_mutable, sc_cast, sc_defunc, sc_label, sc_ulabel,
        sc_overloads, sc_constant, sc_enumconstant, sc_absolute,
        sc_friendlist, sc_const, sc_tconst, sc_classmember, sc_constexpr,
        sc_memberreg, sc_namespace, sc_namespacealias, sc_temp, sc_virtual
    };
// clang-format on

/* basic types */
// clang-format off
    enum e_bt : int
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
// clang-format on

// clang-format off
    enum e_lk : int {
        lk_none, lk_cdecl, lk_pascal, lk_stdcall, lk_fastcall, lk_c, lk_cpp,
        lk_interrupt, lk_fault, lk_inline, lk_virtual, lk_noreturn, lk_threadlocal,
        lk_import, lk_export, lk_internal, lk_auto, lk_msil_rtl, lk_unmanaged, lk_property, lk_entrypoint
    };
// clang-format on

// clang-format off
    enum e_ac
    {
        ac_private,
        ac_protected,
        ac_public,
        ac_none
    };
// clang-format on

// clang-format off
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
        CV_INTEGRALCONVERSIONWEAK,
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
    };
// clang-format on

#define _F_AMPERSAND 1
#define _F_PACKABLE 2
#define _F_SELECTOR 4
#define _F_INTEMPLATEPARAMS 8
#define _F_INARGS 16
#define _F_SIZEOF 32
#define _F_INITLIST 64
#define _F_TEMPLATEARGEXPANSION 128
#define _F_TYPETEST 256
#define _F_NESTEDINIT 512
#define _F_ASSIGNINIT 1024
#define _F_NOEVAL 2048
#define _F_SCOPEDENUM 4096
#define _F_MEMBER 8192
#define _F_IS_NOTHROW 16384
#define _F_RETURN_DELETED 32768
#define _F_NOGEN 0x10000
#define _F_INDECLTYPE 0x20000
#define _F_INCONSTRUCTOR 0x40000
#define _F_EXPLICIT 0x80000
#define _F_NOVIRTUALBASE 1
#define _F_VALIDPOINTER 2

#define _F_NOCHECKAUTO 0x80

typedef std::pair<struct sym*, struct _templateParam*> TEMPLATEPARAMPAIR;

struct ConstExprArgArray
{
    int size;
    struct expr** data;
};

typedef struct expr
{
    struct expr *left, *right;
    enum e_node type;
    int pragmas;
    struct typ* size; /* For block moves */
    void* altdata;
    struct
    {
        union
        {
            long long i;
            FPF* f;
            _COMPLEX_S* c;
            struct
            {
                struct sym* sp; /* sym will be defined later */
                struct ConstExprArgArray constexprData;
            };
            const char* name; /* name during base class processing */
            struct functioncall* func;
            struct _atomicData* ad;
            std::list<struct stmt*>* stmt;
            struct Optimizer::_imode_* imode;
            struct _msilarray* msilArray;
            SymbolTable<struct sym>* syms;
            struct typ* tp;
            struct expr* exp;
            struct
            {
                struct expr* thisptr;
                struct typ* tp;
            } t;
            struct
            {
                struct typ* tp;
                struct lexlist* deferred;
            } construct;
            struct
            {
                std::list<struct expr*>* left;
                std::list<struct expr*>* right;
            } logicaldestructors;
        };
        TEMPLATEPARAMPAIR* templateParam;
        std::vector<struct _templateSelector>* templateSelector;
    } v;
    struct _string* string;
    int xcInit, xcDest;
    int lockOffset;
    char bits;
    char startbit;
    short treesize;
    int isvolatile : 1;
    int isrestrict : 1;
    int isatomic : 1;
    int atomicinit : 1;
    int unionoffset : 1;
    int isfunc : 1;
    int dest : 1;  // for thisref
    int noexprerr : 1;
    int init : 1;       // for no replacement by a constexpr array
    int preincdec : 1;  //  an assignment which is the 'pre' form of autoinc
    int keepZero : 1;
    int paramArray : 1;
    int adjustLabel : 1;
    int structByAddress : 1;
    int isStructAddress : 1;
} EXPRESSION;

typedef struct _msilarray
{
    int count;
    int max;
    struct typ* tp;
    EXPRESSION* base;
    EXPRESSION* indices[1];  // expands
} MSIL_ARRAY;

typedef struct
{
    const char* name;  // must be first as it will go in a hashtable
    EXPRESSION* exp;
    struct sym* sp;
} CONSTEXPRSYM;

typedef struct casedata
{
    long long val;
    int label;
    const char* file;
    int line;
} CASEDATA;

struct u_val
{
    union
    {
        long long i; /* int val */
        union
        {
            const char* a; /* string val */
            const LCHAR* w;
        } s;
        struct _defstruct* defs; /* macro definition */
        FPF* f;                  /* float val */
        _COMPLEX_S* c;
    };
};
typedef struct typ
{
    enum e_bt type;        /* the type */
    long size;             /* total size of type */
    struct typ* btp;       /* pointer to next type (pointers & arrays */
    struct typ* rootType;  /* pointer to base type of sequence */
    int used : 1;          /* type has actually been used in a declaration or cast or expression */
    int array : 1;         /* not a dereferenceable pointer */
    int msil : 1;          /* allocate as an MSIL array */
    int byRefArray : 1;    /* array base address is a reference type */
    int vla : 1;           /* varriable length array */
    int unsized : 1;       /* type doesn't need a size */
    int hasbits : 1;       /* type is a bit type */
    int anonymousbits : 1; /* type is a bit type without a name */
    int scoped : 1;        /* c++ scoped enumeration */
    int fixed : 1;         /* c++ fixed enumeration */
    int nullptrType : 1;   /* c++: std::nullptr */
    int templateTop : 1;
    int enumConst : 1; /* is an enumeration constant */
    int lref : 1;
    int rref : 1;
    int decltypeauto : 1;
    int decltypeautoextended : 1;
    int stringconst : 1;
    char bits;      /* -1 for not a bit val, else bit field len */
    char startbit;  /* start of bit field */
    struct sym* sp; /* pointer to a symbol which describes the type */
    /* local symbol tables */
    SymbolTable<struct sym>* syms; /* Symbol table for structs & functions */
    SymbolTable<struct sym>* tags; /* Symbol table for nested types*/
    TEMPLATEPARAMPAIR* templateParam;
    int alignment;                /* alignment pref for this structure/class/union   */
    EXPRESSION* esize;            /* enode version of size */
    struct typ* etype;            /* type of size field  when size isn't constant */
    int vlaindex;                 /* index into the vararray */
    EXPRESSION* templateDeclType; /* for bt_templatedecltype, used in templates */
    struct typ* typedefType;      /* The typedef which describes this type */
} TYPE;

typedef struct stmt
{
    std::list<struct stmt*>* lower;
    std::list<struct stmt*>* blockTail;
    enum e_stmt type;
    EXPRESSION* select;
    EXPRESSION* destexp;
    Optimizer::LINEDATA* lineData;
    union
    {
        TYPE* tp;
        std::list<CASEDATA*>* cases;
        struct blockdata* parent;
    };
    struct sym* sp;
    int charpos;
    int line;
    const char* file;
    int label;
    int endlabel;
    int breaklabel;  // also the label at the end of the try block
    int altlabel;
    int tryStart;
    int tryEnd;
    int hasvla : 1;
    int hasdeclare : 1;
    int purelabel : 1;
    int explicitGoto : 1;
    int indirectGoto : 1;
} STATEMENT;

typedef struct blockdata
{
    struct blockdata* orig;
    struct blockdata* caseDestruct;
    enum e_kw type;
    std::list<CASEDATA*>* cases;
    std::list<STATEMENT*>* statements;
    std::list<STATEMENT*>* blockTail;
    SymbolTable<struct sym>* table;
    int breaklabel;
    int continuelabel;
    int defaultlabel;
    int needlabel : 1;
    int hasbreak : 1;
    int hassemi : 1;
    int nosemi : 1; /* ok to skip semi */
} BLOCKDATA;

typedef struct init
{
    int offset;
    TYPE* basetp;
    struct sym* fieldsp;
    EXPRESSION* fieldoffs;
    EXPRESSION* exp;
    int tag; /* sequence number */
    int noassign : 1;
} INITIALIZER;

typedef struct ifunc
{
    std::list<STATEMENT*>* stmt;
    SymbolTable<struct sym>* syms;
    SymbolTable<struct sym>* tags;
} INLINEFUNC;

typedef struct _nameSpaceValueData
{
    SymbolTable<struct sym>* syms;
    SymbolTable<struct sym>* tags;
    std::list<struct sym*>* usingDirectives;
    std::list<struct sym*>* inlineDirectives;
    struct sym* origname;
    struct sym* name;

} NAMESPACEVALUEDATA;

struct _ccNamespaceData
{
    const char* declfile;
    int startline;
    int endline;
};
// clang-format off
    enum e_cm { cmNone, cmValue, cmRef, cmThis, cmExplicitValue };
// clang-format on

// order is important for this next, a comparison is done based on this ordering
// clang-format off
    enum e_xc { xc_unspecified, xc_all, xc_dynamic, xc_none };
// clang-format on
struct xcept
{
    Optimizer::LIST* xcDynamic;  // list of types, the exception specification when dynamic
    int xcInitLab, xcDestLab;    // for auto vars
    struct sym* xctab;
    struct sym* xclab;
};

struct attributes
{
    struct
    {
        int structAlign;                                /* alignment of structures/ unions */
        int warnAlign;                                  /* if nz, warn if not aligned */
        unsigned vectorSize;                            /* total size of a vectored attribute */
        enum e_lk linkage;                              /* stdcall */
        enum e_lk linkage2;                             /* export, import, msil */
        enum e_lk linkage3;                             /* used here for noreturn */
        enum e_lk linkage4;                             /* lk_virtual */
        struct sym* cleanup;                            /* cleanup function */
        bool packed;                                    /* True if to reset alignment to 1 */
        bool alignedAttribute;                          /* True if alignment came from gcc aligned attribute */
        unsigned used : 1;                              /* value has been fetched */
        unsigned nonstring : 1;                         /* value is not a zero terminated string */
        unsigned zstring : 1;                           // (argument) was tagged as a zero terminated string
        unsigned isInline : 1;                          /* function is inlined */
        unsigned excludeFromExplicitInstantiation : 1;  // template member should be instantiated implicitly
    } inheritable;
    struct
    {
        const char* deprecationText;  // C++ declaration was deprecated
        const char* alias;            // link-time alias
        struct sym* copyFrom;         // symbol to copy attributes from
        int constructorPriority;      // is a file-scope constructor, set the priority
        int destructorPriority;       // is a file-scope destructor, set the priority
    } uninheritable;
};
/* symbols */
typedef struct sym
{
    const char* name;
    TYPE* tp;
    std::list<TEMPLATEPARAMPAIR>* templateParams;
    unsigned packed : 1;       // packed template param instance
    unsigned synthesized : 1;  // packed template param was synthesized during parsing
    int parserSet : 1;         /* sent to parser already*/
    struct _symbody
    {
        const char* decoratedName; /* symbol name with decorations, as used in output format */
        struct Optimizer::SimpleSymbol* symRef;
        const char *declfile, *origdeclfile;      /* file symbol was declared in */
        int declline, origdeclline, realdeclline; /* line number symbol was declared at */
        short declcharpos;                        /* character position symbol was declared at */
        short declfilenum;                        /* the file number */
        int sizeNoVirtual;                        /* size without virtual classes and thunks */
        struct sym* parent;
        struct sym* parentClass;
        struct sym* friendContext;
        struct sym* parentNameSpace;
        std::list<NAMESPACEVALUEDATA*>* nameSpaceValues; /* for a namespace SP */
        struct sym* vtabsp;
        Optimizer::LINEDATA* linedata;
        enum e_sc storage_class; /* storage class */
        enum e_ac access;        /* c++ access rights for members */
        int operatorId;          /* operator id, CI + kw for an operator function */
        enum e_cm lambdaMode;
        INLINEFUNC inlineFunc;
        int overlayIndex;              /* differentiating index when function differs only in return type from similar functions */
        int ccEndLine;                 /* end line for code completion */
        unsigned long long ccStructId; /* code completion struct id */
        std::list<struct _ccNamespaceData*>* ccNamespaceData; /* namespace data for code completion */
        unsigned declaring : 1;                   /* currently being declared */
        unsigned declaringRecursive : 1;          /* structure is recursively declared */
        unsigned compilerDeclared : 1;            /* compiler declared this */
        unsigned hasproto : 1;                    /* C/90 language prototype was encountered */
        unsigned intagtable : 1;                  /* it is in a tag table */
        unsigned dontlist : 1;                    /* it is a system include, don't put in list file */
        unsigned allocate : 1;                    /* variable is used, allocate space for it */
        unsigned indecltable : 1;                 /* global already in dump table */
        unsigned spaceallocated : 1;              /* space has been allocated */
        unsigned loadds : 1;                      /* to load data seg (limited) */
        unsigned farproc : 1;                     /* this procedure should be terminated with retf */
        unsigned calleenearret : 1;               /* true if callee provided a place for the return
                           value */
        unsigned hasunnamed : 1;                  /* structure has unnamed substructs */
        unsigned isunnamed : 1;                   /* substructure is unnamed */
        unsigned recalculateParameters : 1;       /* inline func needs its parameters
                   recalculated */
        unsigned nullsym : 1;                     /* if was a callblock return which isn't used */
        unsigned anonymous : 1;                   /* if it is a generated variable */
        unsigned inasm : 1;                       /* a way to force the local optimizer to leave autos on the stack */
        unsigned assigned : 1;                    /* value has been assigned */
        unsigned altered : 1;
        unsigned wasExternal : 1;  /* was originally declared as external */
        unsigned gentemplate : 1; /* template instantiation or reference generated */
        unsigned allocaUsed : 1;
        unsigned oldstyle : 1;         /* pointer to a names list if an old style function arg */
        unsigned constexpression : 1;  /* declared with constexpression */
        unsigned addressTaken : 1;     /* address taken */
        unsigned wasUsing : 1;         /* came to this symbol table as a result of 'using' */
        unsigned redeclared : 1;       /* symbol was declared more than once */
        unsigned thisPtr : 1;          /*is a this pointer*/
        unsigned structuredReturn : 1; /* is a pointer to a structure's structure pointer address for returning a value */
        unsigned constop : 1;          /* a constructor 'top' parameter */
        unsigned castoperator : 1;     /* a cast operator */
        unsigned deleted : 1;          /* function was deleted */
        unsigned defaulted : 1;        /* function was defaulted */
        unsigned defaultarg : 1;       /* function argument was defaulted */
        unsigned isfinal : 1;          /* class or virtual function is final */
        unsigned isoverride : 1;       /* virtual function marked override */
        unsigned ispure : 1;           /* pure virtual function */
        unsigned hasvtab : 1;          /* class has a vtab */
        unsigned isabstract : 1;       /* class is abstract */
        unsigned accessspecified : 1;  /* class has access specifiers */
        unsigned safefunc : 1;         /* RTL helper function with no side effects */
        unsigned throughClass : 1;     /* last search was found through a class */
        unsigned hasUserCons : 1;      /* has user-defined constructors */
        unsigned trivialCons : 1;      /* constructor is trivial */
        unsigned trivialDest : 1;      /* destructor is trivial */
        unsigned internallyGenned : 1; /* constructor declaration was made by the compiler */
        unsigned stackblock : 1;       // stacked structure in C++ mode
        unsigned islambda : 1;         // lambda closure struct
        unsigned noinline : 1;         // don't inline an inline qualified function
        unsigned didinline : 1;        // already genned an inline func for this symbol
        unsigned simpleFunc : 1;       // simple enough to override the max_nesting inline requirement
        unsigned hasTry : 1;           // function surrounded by try statement
        unsigned anyTry : 1;           // function has either external or internal try statement
                                       // or variable is used within a try statement
        unsigned canThrow : 1;         // function throws directly
        unsigned hasDest : 1;          // class has a destructor that is called
        unsigned pureDest : 1;         // destructor is pure
        unsigned isConstructor : 1;    // is a constructor
        unsigned isDestructor : 1;     // is  adestructor
        unsigned isAssign : 1;         // is an assignment operator
        unsigned literalClass : 1;     // is a literal class
        unsigned xtEntry : 1;          // is an exception table label
        unsigned isExplicit : 1;       // explicit constructor or conversion function
        unsigned specialized : 1;      // is a template specialization
        unsigned specialized2 : 1;     // specialization of a template class nontemplate func
        unsigned instantiated : 1;     // instantiated template
        unsigned dontinstantiate : 1;  // don't instantiate this template (is extern)
        unsigned instantiating : 1;    // template is currently being instantiated
        unsigned copiedTemplateFunction : 1;
        unsigned instantiatedInlineInClass : 1;  // function instantiated inside a class body
        unsigned promotedToInline : 1;           /* function wasn't declare inline but was promoted to it */
        unsigned temp : 1;                       // temporary boolean...
        unsigned
            pushedTemplateSpecializationDefinition : 1;  // set to true if the current body for the template
                                                         // specialization was pushed from the generalized version of the template
        unsigned destructed : 1;                         // the c++ class instance has had a destructor generated
        unsigned va_typeof : 1;                          // MSIL: a va_typeof symbol
        unsigned has_property_setter : 1;                // a property has a setter
        unsigned nonConstVariableUsed : 1;               // a non-const variable was used or assigned to in this function's body
        unsigned importThunk : 1;                        // an import thunk
        unsigned visited : 1;                            // symbol has been visited in a traversal
        unsigned postExpansion : 1;                      // template was post-expanded
        unsigned performedStructInitialization : 1;      // structure declaration was completed
        unsigned delegated : 1;                          // construct is delegated to another constructor
        unsigned hasBody : 1;                            // function has a body
        unsigned forcedefault : 1;                       // defaulted internal function needs instantiation
        unsigned noExcept : 1;                           // tagged as noexcept or default determined as noexcept
        unsigned explicitlyInstantiated : 1;             // explicitly instantiated
        unsigned retblk : 1;                             // is a return block address
        unsigned initializer_list : 1;                   // struct is initializer_list
        unsigned externShim : 1;                         // symbol is an automatically generated external
        unsigned deleteCopyCons : 1;                     // check if copy constructor needs deletion
        unsigned deleteCopyAssign : 1;                   // check if copy assignment operator needs deleteion
        unsigned deleteMove : 1;                         // check if move constructor/assignment ops need deleteion
        int __func__label;                               /* label number for the __func__ keyword */
        int labelCount;                                  /* number of code labels within a function body */
        int offset;                                      /* address offset of data in the given seg, or optimize register */
        int vtaboffset;                                  /* vtab offset for virtual functions */
        int label;                                       /* label number for statics */
        int uniqueID;                                    /* unique index for local statics */
        int startLine, endLine;                          /* line numbers spanning the function */
        short paramsize;                                 /* Size of parameter list for stdcall functions */
        short accessibleTemplateArgument;                /* something used as a template argument was validated for
                                                          * accessibility before instantiating the template */
        short retcount;                                  /* number of return statements in a function */
        /* Also name for CPP overload lists */
        /* also default for template parameters, is a TYP */
        char* importfile;    /* import name */
        unsigned char* uuid; /* Microsoft: GUID */
        int uuidLabel;       /* Microsoft: Label for a GUID which has been instantiated */
        struct sym* overloadName;
        struct sym* typedefSym;
        struct sym* mainsym;                            /* pointer to the global version of a copied symbol */
        struct sym* maintemplate;                       /* pointer to the global version of a copied symbol */
        std::list<struct _memberInitializers*>* memberInitializers; /* initializers for constructor */
        std::list<STATEMENT*>* gotoTable;                           /* pointer to hashtable associated with goto or label */
        std::list<BLOCKDATA*>* gotoBlockTable;
        /* these fields depend on storage_class */
        struct u_val value;
        std::list<struct _baseClass*>* baseClasses;
        std::list<struct _vbaseEntry*>* vbaseEntries;
        std::list<struct _vtabEntry*>* vtabEntries;
        struct lexlist* deferredCompile;
        struct lexlist* deferredNoexcept;
        std::list<struct sym*>* templateNameSpace;
        short templateLevel;
        std::list<TEMPLATEPARAMPAIR>* typeAlias;
        std::list<struct sym*>* specializations;
        std::list<struct sym*>* instantiations;
        const char* msil;                            // MSIL path
        std::vector<struct _templateSelector>* templateSelector;  // first element is the last valid sym found, second element is the template
                                                     // parameter sym following elements are the list of pointers to names
        struct sym* parentTemplate;                  // could be the parent of a specialization or an instantiation
        std::list<struct init *>* init, *lastInit, *dest;
        // order is important for this next, a comparison is done based on this ordering
        // clang-format off
            enum e_xc xcMode;
        // clang-format on
        struct xcept* xc;
        std::list<struct sym*>* friends;
        TYPE* structuredAliasType;
        attributes attribs;
    } * sb;
} SYMBOL;

typedef struct __lambda
{
    enum e_cm captureMode;
    SymbolTable<struct __lambdasp>* captured;
    SYMBOL* cls;
    SYMBOL* func;
    SYMBOL* lthis;
    TYPE* functp;
    SYMBOL* enclosingFunc;
    SymbolTable<SYMBOL>* oldSyms;
    SymbolTable<SYMBOL>* oldTags;
    TYPE* rv;
    int index;
    int isMutable : 1;
    int captureThis : 1;
    int templateFunctions : 1;
} LAMBDA;

typedef struct __lambdasp
{
    const char* name;
    SYMBOL* sp;
    SYMBOL* parent;
    LAMBDA* enclosing;
} LAMBDASP;

typedef struct _memberInitializers
{
    const char* name;
    SYMBOL* sp;
    SYMBOL* basesym;
    std::list<INITIALIZER*>* init;
    int line;
    const char* file;
    struct lexlist* initData;
    int packed : 1;
    int delegating : 1;
    int valueInit : 1;
} MEMBERINITIALIZERS;

typedef struct _baseClass
{
    SYMBOL* cls;
    enum e_ac accessLevel;
    unsigned offset;
    int isvirtual : 1;
    int top : 1;
} BASECLASS;
typedef struct _vtabEntry
{
    std::list<struct _vtabEntry*>* children;
    std::list<struct sym*>* virtuals;
    SYMBOL* cls;
    unsigned dataOffset;
    unsigned vtabOffset;
    bool isvirtual;
    bool isdead;
} VTABENTRY;
typedef struct _vbaseEntry
{
    SYMBOL* cls;
    bool alloc;
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
    int index : 8;
    int packed : 1;
    int usedAsUnpacked : 1;
    int initialized : 1;
    int lref : 1;
    int rref : 1;
    int resolved : 1;  // packed template has already been resolved.
    int ellipsis : 1;  // ellipsis found after this item...
    int replaced : 1;  // replaced during type alias substitution
    int deduced : 1;   // filled in during deduction
    int specializationParam : 1; // specialization paramneter
    SYMBOL* packsym;
    void* hold; /* value held during partial template ordering */
    Optimizer::LIST* stack;
    union
    {
        // the dflt & val fields must be in the same place for each item
        struct
        {
            SYMBOL* dflt;
            SYMBOL* val;
            struct lexlist* txtdflt;
            std::list<SYMBOL*>* txtargs;
            SYMBOL* temp;
            std::list<TEMPLATEPARAMPAIR>* args;
            TEMPLATEPARAMPAIR* orig;
        } byTemplate;
        struct
        {
            TYPE* dflt;
            TYPE* val;
            struct lexlist* txtdflt;
            std::list<SYMBOL*>* txtargs;
            TYPE* temp;
        } byClass;
        struct
        {
            EXPRESSION* dflt;
            EXPRESSION* val;
            struct lexlist* txtdflt;
            std::list<SYMBOL*>* txtargs;
            EXPRESSION* temp;
            struct lexlist* txttype;
            TYPE* tp;
        } byNonType;
        struct
        {
            std::list<TEMPLATEPARAMPAIR>* types;
            std::list<TEMPLATEPARAMPAIR>* next;
        } bySpecialization;
        struct
        {
            std::list<TEMPLATEPARAMPAIR>* pack;
        } byPack;
        struct
        {
            std::list<TEMPLATEPARAMPAIR>* args;
        } byDeferred;
    };
} TEMPLATEPARAM;

typedef struct _templateSelector
{
    union
    {
        SYMBOL* sp;
        const char* name;
        TYPE* tp;
    };
    std::list<TEMPLATEPARAMPAIR>* templateParams;
    std::list<struct initlist*>* arguments;
    int isTemplate : 1;
    int isDeclType : 1;
    int asCall : 1;
} TEMPLATESELECTOR;

typedef struct _structSym
{
    SYMBOL* str;
    std::list<TEMPLATEPARAMPAIR>* tmpl;
} STRUCTSYM;
typedef struct initlist
{
    TYPE* tp;
    EXPRESSION* exp;
    std::list<struct expr*>* destructors;
    std::list<struct initlist*>* nested;
    int byRef : 1;
    int packed : 1;
    int vararg : 1;
    int valist : 1;
    int initializer_list : 1;
} INITLIST;

typedef struct functioncall
{
    SYMBOL* sp;
    TYPE* functp;
    EXPRESSION* fcall;
    std::list<INITLIST*>* arguments;
    std::list<struct expr*>* destructors;
    SYMBOL* returnSP;
    EXPRESSION* returnEXP;
    EXPRESSION* thisptr;
    TYPE* thistp;
    std::list<TEMPLATEPARAMPAIR>* templateParams;
    NAMESPACEVALUEDATA* nameSpace;
    SYMBOL* rttiType;
    SYMBOL* rttiType2;
    int callLab;
    int novtab : 1;
    int ascall : 1;
    int astemplate : 1;
    int noobject : 1;
    int asaddress : 1;
    int vararg : 1;
    int resolvedCall : 1;
} FUNCTIONCALL;

#define MAX_STRLEN 16384
#define MAX_STLP1 (MAX_STRLEN + 1)

/* error list */
struct errl
{
    int errornumber;
    void* data;
};

/* used for error skimming */
#define BALANCE struct balance
#define BAL_PAREN 0
#define BAL_BRACKET 1
#define BAL_BEGIN 2
#define BAL_LT 3
#define ERRORS struct errl

struct balance
{
    struct balance* back;
    short type;
    short count;
};

// clang-format off
    enum _matchFlags
    {
        KW_NONE = 0, KW_CPLUSPLUS = 1, KW_INLINEASM = 2, KW_NONANSI = 4, KW_C99 = 8,
        KW_C1X = 16, KW_ASSEMBLER = 32, KW_MSIL = 64,
        KW_386 = 128, KW_68K = 256, KW_C2X = 512, KW_ALL = 0x40000000
    };
// clang-format on
// clang-format off

    enum _tokenTypes
    {
        TT_BASE = 1,
        TT_BOOL = 2,
        TT_INT = 4,
        TT_FLOAT = 8,
        TT_COMPLEX = 16,
        TT_TYPEQUAL = 32,
        TT_POINTERQUAL = 64,
        TT_UNARY = 128,
        TT_BINARY = 0x100,
        TT_OPERATOR = 0x200,
        TT_ASSIGN = 0x400,
        TT_RELATION = 0x800,
        TT_EQUALITY = 0x1000,
        TT_INEQUALITY = 0x2000,
        TT_POINTER = 0x4000,
        TT_STORAGE_CLASS = 0x8000,
        TT_CONTROL = 0x10000,
        TT_BLOCK = 0x20000,
        TT_PRIMARY = 0x40000,
        TT_SELECTOR = 0x80000,
        TT_VAR = 0x100000,
        TT_BASETYPE = 0x200000,
        TT_INCREMENT = 0x400000,
        TT_SWITCH = 0x800000,
        TT_ENUM = 0x1000000,
        TT_STRUCT = 0x2000000,
        TT_TYPENAME = 0x4000000,
        TT_TYPEDEF = 0x8000000,
        TT_VOID = 0x10000000,
        TT_CLASS = 0x20000000,
        TT_LINKAGE = 0x40000000,
        TT_DECLARE = 0x80000000UL,
        TT_UNKNOWN = 0
    };
// clang-format on

typedef struct kwblk
{
    const char* name;
    int len;
    enum e_kw key;
    unsigned matchFlags;
    unsigned tokenTypes;
    /*    ASMNAME *data; */
} KEYWORD;

#define MATCHTYPE(lex, tp) (lex && (lex)->data->type == (tp))
#define ISID(lex) (lex && (lex)->data->type == l_id)
#define ISKW(lex) (lex && (lex)->data->type == l_kw)
#define MATCHKW(lex, keyWord) (ISKW(lex) && ((lex)->data->kw->key == keyWord))
#define KWTYPE(lex, types)                                                                                                \
    (ISKW(lex) && (((lex)->data->kw->key == kw_auto ? (Optimizer::cparams.prm_cplusplus ? TT_BASETYPE : TT_STORAGE_CLASS) \
                                                    : (lex)->data->kw->tokenTypes) &                                      \
                   (types)))
#define KW(lex) (ISKW(lex) ? (lex)->data->kw->key : kw_none)

enum e_lexType
{
    l_none,
    l_i,
    l_ui,
    l_l,
    l_ul,
    l_ll,
    l_ull,
    l_f,
    l_d,
    l_ld,
    l_I,
    l_id,
    l_kw,
    l_astr,
    l_wstr,
    l_ustr,
    l_Ustr,
    l_u8str,
    l_msilstr,
    l_achr,
    l_wchr,
    l_uchr,
    l_Uchr,
    l_qualifiedname,
    l_asminst,
    l_asmreg
};

typedef struct _string
{
    enum e_lexType strtype;
    int size;
    int label;
    int refCount;
    char* suffix;
    Optimizer::SLCHAR** pointers;
} STRING;

typedef struct lexeme
{
    // clang-format off
        enum e_lexType type;
    // clang-format on
    struct u_val value;
    char* litaslit;
    char* suffix;
    Optimizer::LINEDATA* linedata;
    int errline;
    const char* errfile;
    int charindex;
    int charindexend;
    int filenum;
    KEYWORD* kw;
    SYMBOL* typequal;
    int registered : 1;
} LEXEME;

typedef struct lexlist
{
    struct lexlist *next, *prev;
    struct lexeme* data;
} LEXLIST;
typedef struct lexContext
{
    struct lexContext* next;
    LEXLIST* cur;
    LEXLIST* last;
} LEXCONTEXT;

struct templateListData
{
    std::list<TEMPLATEPARAMPAIR>* args;  // list of templateparam lists
    std::list<TEMPLATEPARAMPAIR>**ptail, **plast, **phold;
    LEXLIST *head, *tail;
    LEXLIST *bodyHead, *bodyTail;
    SYMBOL* sp;
};

#define INCL_LEVEL_MAX 16

#define MAX_INLINE_NESTING 3

// must match stdatomic.h
typedef struct _atomicData
{
    enum Optimizer::e_ao atomicOp;
    EXPRESSION* memoryOrder1;
    EXPRESSION* memoryOrder2;
    EXPRESSION* flg;
    EXPRESSION* address;
    EXPRESSION* value;
    EXPRESSION* third;
    TYPE* tp;
} ATOMICDATA;

constexpr inline TYPE* basetype(TYPE* a)
{
    if (a)
        a = a->rootType;
    return a;
}

constexpr inline bool __isref(TYPE* x) { return (x)->type == bt_lref || (x)->type == bt_rref; }
constexpr inline bool isref(TYPE* x)
{
    return (__isref(basetype(x)) ||
            (x)->type == bt_templateparam && (x)->templateParam->second->type == kw_int && __isref((x)->templateParam->second->byNonType.tp));
}
constexpr inline bool __ispointer(TYPE* x) { return ((x)->type == bt_pointer || (x)->type == bt_seg); }
constexpr inline bool ispointer(TYPE* x)
{
    return (__ispointer(basetype(x)) || (x)->type == bt_templateparam && (x)->templateParam->second->type == kw_int &&
                                            __ispointer((x)->templateParam->second->byNonType.tp));
}

constexpr inline bool __isfunction(TYPE* x) { return ((x)->type == bt_func || (x)->type == bt_ifunc); }
constexpr inline bool isfunction(TYPE* x) { return (__isfunction(basetype(x))); }

constexpr inline bool isfuncptr(TYPE* x) { return (ispointer(x) && basetype(x)->btp && isfunction(basetype(x)->btp)); }
constexpr inline bool __isstructured(TYPE* x) { return ((x)->type == bt_class || (x)->type == bt_struct || (x)->type == bt_union); }

constexpr inline bool isstructured(TYPE* x) { return (__isstructured(basetype(x))); }

}  // namespace Parser

#define SET_GLOBAL(val, index) \
    bool oldgf##index;         \
    SetGlobalFlag(val, oldgf##index);
#define RELEASE_GLOBAL(index) ReleaseGlobalFlag(oldgf##index);
#define GET_GLOBAL GetGlobalFlag();
void SetGlobalFlag(bool flag, bool& old);
void ReleaseGlobalFlag(bool old);
bool GetGlobalFlag();
