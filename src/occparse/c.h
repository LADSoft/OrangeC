/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, 
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
#include "ctypes.h"

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
#define CI_ASSIGN ((int)Keyword::assign_ + 3)
#define CI_FUNC ((int)Keyword::openpa_ + 3)
#define CI_POINTSTO ((int)Keyword::pointsto_ + 3)
#define CI_NEWA ((int)Keyword::complx_ + 1 + 3)
#define CI_DELETEA ((int)Keyword::complx_ + 2 + 3)
#define CI_LIT ((int)Keyword::complx_ + 3 + 3)

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
  typedef typename std::list<T*>::reverse_iterator reverse_iterator;
  iterator begin() { return inOrder_.begin(); }
    iterator end() { return inOrder_.end(); }
    reverse_iterator rbegin() { return inOrder_.rbegin(); }
    reverse_iterator rend() { return inOrder_.rend(); }
    size_t size() const { return inOrder_.size(); }
    T* back() const { return inOrder_.back(); }
    T* front() const { return inOrder_.front(); }
    inline void remove(iterator it);
    inline void remove(T* sym);
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
    enum class Keyword
    {
        /* first comes all the C++ overloadable operators */
        /* be sure to change expectlist if you insert things */
        new_, delete_, plus_, minus_, star_, divide_, leftshift_, rightshift_, mod_, eq_,
        neq_, lt_, leq_, gt_, geq_, assign_, asplus_, asminus_, astimes_, asdivide_,
        asmod_, asleftshift_, asrightshift_, asand_, asor_, asxor_, autoinc_, autodec_,
        openbr_, openpa_, pointstar_, pointsto_, comma_, lor_, land_, not_, or_, and_, uparrow_,
        complx_, newa_, dela_, quot_,
        unary_plus_, unary_minus_, unary_star_, unary_and_,
        /* then generic stuff that isn't overloadable or is internal */
        id_, hook_, colon_, begin_, end_, dot_,
        closebr_, closepa_, ellipse_, semicolon_, hash_, classsel_,
        dotstar_, lassign,
        /* Finally tokens for keywords */
        int_, void_, char_, float_, double_, struct_, union_, long_,
        short_, unsigned_, signed_, auto_, extern_, register_,
        typedef_, static_, goto_, return_, sizeof_, break_,
        continue_, if_, else_, for_, do_, while_, switch_,
        case_, default_, enum_, volatile_, const_,
        trap_, interrupt_, fault_, absolute_, genword_,
        Complex_, restrict_, Imaginary_, I_, INF_, NAN_,
        typeof_, typeof_unqual_, Pragma_, atomic_, noreturn_, generic_, bitint_,
        /* C++ */
        public_, private_, protected_, class_, friend_, namespace_,
        using_, this_, operator_, inline_, virtual_, try_, throw_,
        catch_, template_, bool_, true_, false_, wchar_t_,
        static_cast_, dynamic_cast_, reinterpret_cast_, const_cast_,
        typeid_, typename_, explicit_, decltype_,
        export_, static_assert_, alignas_, alignof_, char16_t_,
        char32_t_, mutable_, nullptr_, noexcept_, thread_local_, constexpr_,
        rangefor_,
        /* Extended */
        char8_t_, has_c_attribute_, ckdadd_, ckdsub_, ckdmul_,
        atomic_flag_test_set_, atomic_flag_clear_, atomic_kill_dependency_,
        /* Clang compatibility for atomics */
        c11_atomic_init_, c11_atomic_thread_fence_, c11_atomic_signal_fence_,
        c11_atomic_is_lock_free_, c11_atomic_store_, c11_atomic_load_, // NOTE: clang's atomic_is_lock_free runs on the atomic's size, not the address
        c11_atomic_xchg_, c11_atomic_cmpxchg_strong_, c11_atomic_cmpxchg_weak_,
        c11_atomic_ftchadd_, c11_atomic_ftchsub_, c11_atomic_ftchand_,
        c11_atomic_ftchor_, c11_atomic_ftchxor_,
        /* GNU atomic support, incomplete but works */
        /* NOTE: atomic_*_n are able to be mapped to our existing structure without issue except compare_exchange*/
        /* NOTE: atomic_fetch_* are supported by the same mapping structure atomic_*_n has */
        atomic_addftch_, atomic_subftch_, atomic_andftch_, atomic_xorftch_,
        atomic_orftch_, atomic_cmpxchg_n_,
        /* Extended */              
        pascal_, stdcall_, fastcall_, cdecl_, intrinsic_, asm_, loadds_,
        far_, asmreg_, asminst_, indirect_, dllexport_, dllimport_, func_,
        near_, seg_, typeid__, int64_, alloca_, msil_rtl_,
        va_list_, va_typeof_, unmanaged_, uuid_, uuidof_,
        string_, object_, native_, cpblk_, initblk_, property_, entrypoint_,
        seh_try_, seh_catch_, seh_finally_, seh_fault_, declspec_, rtllinkage_, attribute_,
        offsetof_, underlying_type_, volatile__,
        /* These next are generic register names */
        D0_, D1_, D2_, D3_, D4_, D5_, D6_, D7_, D8_, D9_, DA_,
        DB_, DC_, DD_, DE_, DF_, A0_, A1_, A2_, A3_, A4_,
        A5_, A6_, A7_, A8_, A9_, AA_, AB_, AC_, AD_, AE_,
        AF_, F0_, F1_, F2_, F3_, F4_, F5_, F6_, F7_, F8_,
        F9_, FA_, FB_, FC_, FD_, FE_, FF_, CR0_, CR1_, CR2_,
        CR3_, CR4_, CR5_, CR6_, CR7_, DR0_, DR1_, DR2_, DR3_,
        DR4_, DR5_, DR6_, DR7_, TR0_, TR1_, TR2_, TR3_, TR4_,
        TR5_, TR6_, TR7_, eol, none_
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
    enum class ExpressionNode
    {
        void_, not__lvalue_, lvalue_, argnopush_, void_nz_, shiftby_,
        global_, auto_, labcon_, absolute_, pc_, const_, threadlocal_,
        c_bit_, c_bool_, c_c_, c_uc_, c_wc_, c_s_, c_u16_, c_us_, c_i_, c_ui_,
        c_u32_, c_l_, c_ul_, c_ll_, c_ull_, c_f_, c_d_, c_ld_,
        c_p_, c_sp_, c_fp_, c_fc_, c_dc_, c_ldc_,
        c_fi_, c_di_, c_ldi_, x_bool_, x_bit_,
        c_string_, c_bitint_, c_ubitint_,
        x_i_, x_ui_, x_l_, x_ul_, x_inative_, x_unative_,
        x_ll_, x_ull_, x_f_, x_d_, x_ld_, x_fi_, x_di_, x_ldi_, x_fp_, x_sp_,
        x_fc_, x_dc_, x_ldc_, x_c_, x_uc_, x_wc_, x_u16_, x_u32_, x_s_, x_us_, x_label_,
        x_string_, x_object_, x_bitint_, x_ubitint_,
        l_bool_, l_c_, l_uc_, l_u16_, l_u32_, l_wc_, l_s_, l_us_, l_i_, l_ui_,
        l_inative_, l_unative_,
        l_l_, l_ul_, l_ll_, l_ull_, l_f_, l_d_, l_ld_, l_p_, l_ref_,
        l_fi_, l_di_, l_ldi_, l_fc_, l_dc_, l_ldc_, l_fp_, l_sp_, l_bit_,
        l_string_, l_object_, l_bitint_, l_ubitint_, msil_array_access_, msil_array_init_,
        nullptr_, memberptr_, mp_as_bool_, mp_compare_,
        trapcall_, func_, funcret_, intcall_,
        arraymul_, arraylsh_, arraydiv_, arrayadd_, structadd_, structelem_,
        add_, sub_, mul_, mod_, div_, lsh_, rsh_, ursh_,
        cond_, assign_, eq_, ne_,
        uminus_, not_, compl_, lt_, le_, gt_, ge_,
        and_, or_, land_, lor_, xor_, umul_, auto_inc_, auto_dec_,
        udiv_, umod_, ugt_, uge_, ule_, ult_, blockclear_, stackblock_,
        blockassign_, bits_,
        imode_, x_p_, substack_, alloca_, cpblk_, initblk_, initobj_, sizeof_,
        loadstack_, savestack_, stmt_, atomic_, placeholder_, thisshim_, thisref_,
        construct_, literalclass_, templateparam_, templateselector_, packedempty_, sizeofellipse_,
        type_, pointsto_, dot_, select_,
        // stuff that can only appear temporarily in constexpr expressions
        cvarpointer_, paramsubstitute_
    };
// clang-format on

/*      statement node descriptions     */
// clang-format off
    enum class StatementNode : int
    {
        line_, nop_, expr_, declare_, goto_, indgoto_, asmgoto_, asmcond_,
        loopgoto_, select_, notselect_, varstart_, dbgblock_,
        switch_, return_, block_, throw_, try_, catch_,
        genword_, passthrough_, datapassthrough_, abs_, label_,
        seh_try_, seh_catch_, seh_finally_, seh_fault_,
    };
// clang-format on
                
/* storage classes */
// clang-format off
    enum class StorageClass : int
    {
        none_, static_, localstatic_, auto_, register_, global_, external_, templateparam_,
        parameter_, catchvar_, type_, typedef_, member_, mutable_, cast_, defunc_, label_, ulabel_,
        overloads_, constant_, enumconstant_, absolute_, alias_,
        friendlist_, const_, tconst_, classmember_, constexpr_,
        memberreg_, namespace_, namespace_alias_, temp_, virtual_
    };
// clang-format on

/* basic types */
// clang-format off
    enum class BasicType : int
    {
        /* keep this ordering and dont insert anything before the end of the
         * basic types, type comparisons (LOSTCONV) depends on the ordering,
         * and the debug info has a table indexed by type
         */
        bit_, bool_, signed_char_, char_, unsigned_char_, char8_t_, short_, char16_t_, unsigned_short_,
        wchar_t_, enum_, int_, inative_, char32_t_, unsigned_, unative_, long_, unsigned_long_, long_long_,
        unsigned_long_long_, bitint_,  unsigned_bitint_, float_, double_, long_double_, float__imaginary_,
        double__imaginary_, long_double_imaginary_, float__complex_,
        double__complex_, long_double_complex_,
        /* end of basic types */
        void_, object_, string_,
        /* end of debug needs */
        signed_, static_, atomic_, const_, volatile_, restrict_, far_, near_, seg_,
        aggregate_, untyped_, typedef_, pointer_, lref_, rref_, lrqual_, rrqual_, struct_,
        union_, func_, class_, ifunc_, any_, auto_,
        match_none_, ellipse_, memberptr_, cond_, va_list_, objectArray_,
        consplaceholder_, templateparam_, templateselector_, templatedecltype_, derivedfromtemplate_,
        templateholder_,
        /* last */
        none_
    };
// clang-format on

// clang-format off
    enum class Linkage : int {
        none_, cdecl_, pascal_, stdcall_, fastcall_, c_, cpp_,
        interrupt_, fault_, inline_, virtual_, noreturn_, threadlocal_,
        import_, export_, internal_, auto_, msil_rtl_, unmanaged_, property_, entrypoint_
    };
    // clang-format on

    // clang-format off
    enum class AccessLevel : int
    {
        private_,
        protected_,
        public_,
        none_
    };

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
#define _F_NOCONSTGEN 0x100000
#define _F_NOVARIADICFOLD 0x200000
#define _F_EXPRESSIONINPAREN 0x400000

#define _F_NOVIRTUALBASE 1
#define _F_VALIDPOINTER 2

#define _F_NOCHECKAUTO 0x80

typedef std::pair<struct sym*, struct _templateParam*> TEMPLATEPARAMPAIR;

struct ConstExprArgArray
{
    unsigned short size;
    unsigned short multiplier;
    struct expr** data;
};

typedef struct expr
{
    struct expr *left, *right;
    enum ExpressionNode type;
    int pragmas;
    struct typ* size; /* For block moves */
    void* altdata;
    Optimizer::RUNTIMEDATA* runtimeData;
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
        struct
        {
            int bits;
            unsigned char* value;
        } b;
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
    int computedLabel : 1;
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
        struct
        {
            int bits;
            unsigned char* value;
        } b;
        struct _defstruct* defs; /* macro definition */
        FPF* f;                  /* float val */
        _COMPLEX_S* c;
    };
};
typedef struct typ
{
    enum BasicType type;        /* the type */
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
    int bitintbits;
    struct sym* sp; /* pointer to a symbol which describes the type */
    /* local symbol tables */
    SymbolTable<struct sym>* syms; /* Symbol table for structs & functions */
    SymbolTable<struct sym>* tags; /* Symbol table for nested types*/
    TEMPLATEPARAMPAIR* templateParam;
    int alignment;                /* alignment pref for this structure/class/union   */
    EXPRESSION* esize;            /* enode version of size */
    struct typ* etype;            /* type of size field  when size isn't constant */
    int vlaindex;                 /* index into the vararray */
    EXPRESSION* templateDeclType; /* for BasicType::templatedecltype_, used in templates */
    struct typ* typedefType;      /* The typedef which describes this type */
} TYPE;

typedef struct stmt
{
    std::list<struct stmt*>* lower;
    std::list<struct stmt*>* blockTail;
    enum StatementNode type;
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
    Keyword type;
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
        enum Linkage linkage;                              /* stdcall */
        enum Linkage linkage2;                             /* export, import, msil */
        enum Linkage linkage3;                             /* used here for noreturn */
        enum Linkage linkage4;                             /* lvirtual */
        struct sym* cleanup;                            /* cleanup function */
        bool packed;                                    /* True if to reset alignment to 1 */
        bool alignedAttribute;                          /* True if alignment came from gcc aligned attribute */
        unsigned used : 1;                              /* value has been fetched */
        unsigned nonstring : 1;                         /* value is not a zero terminated string */
        unsigned zstring : 1;                           // (argument) was tagged as a zero terminated string
        unsigned isInline : 1;                          /* function or data is inlined */
        unsigned isInlineData : 1;                      /* data is inlined */
        unsigned excludeFromExplicitInstantiation : 1;  // template member should be instantiated implicitly
    } inheritable;
    struct
    {
        const char* deprecationText;  // C++ declaration was deprecated
        const char* alias;            // link-time alias
        struct sym* copyFrom;         // symbol to copy attributes from
        int constructorPriority;      // is a file-scope constructor, set the priority
        int destructorPriority;       // is a file-scope destructor, set the priority
        unsigned stackProtect : 1;    // explicit stackprotect
        unsigned fallthrough : 1;     // fallthrough case
        unsigned maybe_unused : 1;    // variable might be unused
        unsigned nodiscard : 1;       // function return value should not be discarded
    } uninheritable;
};
/* symbols */
typedef struct sym
{
    const char* name;
    TYPE* tp;
    std::list<TEMPLATEPARAMPAIR>* templateParams;
    unsigned short utilityIndex;
    unsigned short packed : 1;       // packed template param instance
    unsigned short synthesized : 1;  // packed template param was synthesized during parsing
    unsigned short parserSet : 1;    /* sent to parser already*/
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
        enum StorageClass storage_class; /* storage class */
        enum AccessLevel access;        /* c++ access rights for members */
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
        unsigned usingTypedef : 1;     /* typedef defined as a 'using' statement */
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
        struct sym* runtimeSym;                                /* Simple symbol for runtime assignment tracking */
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
        // clang-format off
            enum e_xc xcMode, xcModeSpecified;
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
    int thisByVal : 1;
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
    enum AccessLevel accessLevel;
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
enum class TplType
{
    new_,
    delete_,
    int_,
    typename_,
    template_
};

typedef struct _templateParam
{
    // class = class or namespace
    // int = nontype
    // template = template parameter
    // new = specialization
    // first in the list is always the specialization specifier
    enum class TplType type;
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
    int flag : 1; // utility flag
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
    enum _matchFlags : int
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
    Keyword key;
    unsigned matchFlags;
    unsigned tokenTypes;
} KEYWORD;


enum e_lexType
{
    l_none,
    l_i,
    l_ui,
    l_l,
    l_ul,
    l_ll,
    l_ull,
    l_bitint,
    l_ubitint,
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
    l_u8chr,
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

#define MATCHTYPE(lex, tp) (lex && (lex)->data->type == (tp))
#define ISID(lex) (lex && (lex)->data->type == l_id)
#define ISKW(lex) (lex && (lex)->data->type == l_kw)
#define MATCHKW(lex, keyWord) (ISKW(lex) && ((lex)->data->kw->key == keyWord))
bool KWTYPE(LEXLIST* lex, unsigned types);
#define KW(lex) (ISKW(lex) ? (lex)->data->kw->key : Keyword::none_)

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

constexpr inline bool __isref(TYPE* x) { return (x)->type == BasicType::lref_ || (x)->type == BasicType::rref_; }
constexpr inline bool isref(TYPE* x)
{
    return (__isref(basetype(x)) ||
            (x)->type == BasicType::templateparam_ && (x)->templateParam->second->type == TplType::int_ && __isref((x)->templateParam->second->byNonType.tp));
}
constexpr inline bool __ispointer(TYPE* x) { return ((x)->type == BasicType::pointer_ || (x)->type == BasicType::seg_); }
constexpr inline bool ispointer(TYPE* x)
{
    return (__ispointer(basetype(x)) || (x)->type == BasicType::templateparam_ && (x)->templateParam->second->type == TplType::int_ &&
                                            __ispointer((x)->templateParam->second->byNonType.tp));
}

constexpr inline bool __isfunction(TYPE* x) { return ((x)->type == BasicType::func_ || (x)->type == BasicType::ifunc_); }
constexpr inline bool isfunction(TYPE* x) { return (__isfunction(basetype(x))); }

constexpr inline bool isfuncptr(TYPE* x) { return (ispointer(x) && basetype(x)->btp && isfunction(basetype(x)->btp)); }
constexpr inline bool __isstructured(TYPE* x) { return ((x)->type == BasicType::class_ || (x)->type == BasicType::struct_ || (x)->type == BasicType::union_); }

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
