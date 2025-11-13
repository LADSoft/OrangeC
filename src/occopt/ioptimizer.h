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
/*
 * iexpr.h
 *
 * ICODE structures
 */
#include <unordered_map>
#include <list>
#include <vector>
#include "ctypes.h"
#include <Floating.h>

#define imax(x, y) ((x) > (y) ? (x) : (y))
#define imin(x, y) ((x) < (y) ? (x) : (y))

#define F_VOL 1
#define F_NOVALUE 2
#define F_ADDR 4
#define F_COMPARE 8
#define F_STORE 16
#define F_SWITCHVALUE 32
#define F_OBJECT 64
#define F_INRETURN 128
#define F_NEEDEPILOG 256
#define F_RETURNREFBYVALUE 512
#define F_RETURNSTRUCTBYVALUE 1024
#define F_RETURNSTRUCTBYADDRESS 2048
#define F_RETURNSTRUCTNOADJUST 4096
#define F_NORETURNVALUE 8192

namespace Parser
{
struct sym;
struct Type;
struct expr;
enum class BasicType : int;
enum class StorageClass : int;
}  // namespace Parser

namespace Optimizer
{

struct BaseList
{
    BaseList* next;
    struct SimpleSymbol* sym;
    int offset;
};
enum st_type
{
    st_none,
    st_i,
    st_ui,
    st_f,
    st_fi,
    st_fc,
    st_pointer,
    st_void,
    st___string,
    st___object,
    st_func,
    st_lref,
    st_rref,
    st_struct,
    st_union,
    st_class,
    st_enum,
    st_memberptr,
    st_aggregate,
    st_ellipse,
    st_any
};
struct SimpleType
{
    SimpleType* btp;
    SimpleType* structuredAlias;
    struct SimpleSymbol* sp;

    st_type type;
    int size;
    int sizeFromType;
    int bits;
    int startbit;
    union
    {
        struct
        {
            unsigned isarray : 1;
            unsigned isvla : 1;
            unsigned isatomic : 1;
            unsigned msil : 1;
            unsigned va_list : 1;
            unsigned scoped : 1;
            unsigned isconst : 1;
            unsigned isvolatile : 1;
            unsigned isrestrict : 1;
            unsigned istypedef : 1;
            unsigned pinned : 1;
        };
        unsigned flags;
    };
};

enum e_scc_type
{
    scc_none,
    scc_static,
    scc_localstatic,
    scc_auto,
    scc_register,
    scc_global,
    scc_external,
    scc_templateparam,
    scc_parameter,
    scc_catchvar,
    scc_type,
    scc_typedef,
    scc_member,
    scc_mutable,
    scc_cast,
    scc_defunc,
    scc_label,
    scc_ulabel,
    scc_overloads,
    scc_constant,
    scc_enumconstant,
    scc_absolute,
    scc_friendlist,
    scc_const,
    scc_tconst,
    scc_classmember,
    scc_constexpr,
    scc_memberreg,
    scc_namespace,
    scc_namespacealias,
    scc_temp,
    scc_virtual
};

struct SimpleSymbol
{
    const char* name;
    const char* outputName;
    const char* importfile;
    const char* namespaceName;
    int fileIndex;
    int typeIndex;

    // copied from main symbol
    int i;
    enum e_scc_type storage_class;
    int sizeFromType;
    int offset;
    int align;
    int size;
    int label;
    BaseList* baseClasses;
    SimpleType* tp;
    struct Parser::expr* paramSubstitute;
    SimpleSymbol* parentClass;
    LIST* syms;
    const char* msil;
    int templateLevel;
    int ellipsePos;
    union
    {
        struct
        {
            unsigned hasParams : 1;  // from paramsize
            unsigned isstructured : 1;
            unsigned anonymous : 1; /* if it is a generated variable */
            unsigned allocate : 1;  /* variable is used, allocate space for it */
            unsigned thisPtr : 1;
            unsigned stackblock : 1;
            unsigned inasm : 1;
            unsigned isimport : 1;
            unsigned isexport : 1;
            unsigned isvirtual : 1;
            unsigned isinternal : 1;
            unsigned msil_rtl : 1;
            unsigned isproperty : 1;
            unsigned unmanaged : 1;
            unsigned isstdcall : 1;
            unsigned iscdecl : 1;
            unsigned ispascal : 1;
            unsigned isfastcall : 1;
            unsigned entrypoint : 1;
            unsigned temp : 1;  // temporary
            unsigned vbase : 1;
            unsigned anyTry : 1;
            unsigned xc : 1;
            unsigned canThrow : 1;
            unsigned usesEsp : 1;
            unsigned addressTaken : 1;
            unsigned generated : 1;
            unsigned msilObjectArray : 1;
            unsigned stackProtectBasic : 1;
            unsigned stackProtectStrong : 1;
            unsigned stackProtectExplicit : 1;
            unsigned allocaUsed : 1;
        };
        unsigned long long flags;
    };
    // not used past frontend
    unsigned inFunc : 1;
    unsigned hasInlineFunc : 1;
    unsigned ispure : 1;
    unsigned dontinstantiate : 1;
    unsigned initialized : 1;
    // new for intermediate
    unsigned inCatch : 1;          // used inside a catch block
    unsigned usedasbit : 1;        /* used in a bit field op */
    unsigned spillVar : 1;         /* backend allocator spill variable */
    unsigned noGlobal : 1;         /* no global opts on this temp var */
    unsigned storeTemp : 1;        /* is a storetemp */
    unsigned loadTemp : 1;         /* is a loadtemp */
    unsigned visited : 1;          /* temproary which means it is visited */
    unsigned pushedtotemp : 1;     /* if a local variable has been transformed to a temp */
    unsigned noCoalesceImmed : 1;  // set to true if temp or memory address which references an immediate is used
                                   // other than as the immediate reference
    unsigned regmode : 2;          /* 0 = pure var, 1 = addr in reg, 2 = value in reg*/
    unsigned retemp : 1;           // retemp has already been performed on this SP
    unsigned inAllocTable : 1;     /* auto temp var is in the allocation table already */
    unsigned importThunk : 1;
    struct _imode_* imaddress;
    struct _imode_* imvalue;
    struct _im_list* imind;
    struct _imode_* imstore;
    struct Parser::sym* inlineSym;
};
enum se_type
{
    se_none,
    se_add,
    se_sub,
    se_uminus,
    se_i,
    se_ui,
    se_f,
    se_fi,
    se_fc,
    se_const,
    se_absolute,
    se_auto,
    se_global,
    se_threadlocal,
    se_pc,
    se_labcon,
    se_structelem,
    se_func,
    se_tempref,
    se_msil_array_init,
    se_msil_array_access,
    se_string,
    se_typeref
};
struct SimpleString
{
    const unsigned char* value;
    int byteCount;
    int sizeFromISZ;
};
struct SimpleExpression
{
    SimpleExpression()  {}  // don't care about init, we just want the memory functions to compile
    se_type type;
    union
    {
        struct
        {
            char* str;
            int len;
        } astring;
        SimpleSymbol* sp;
        long long i;
        FPF f;
        struct
        {
            FPF r;
            FPF i;
        } c;
        SimpleType* tp;
        SimpleType* msilArrayTP;
        //        SimpleString *string;
    };
    union
    {
        struct
        {
            int isvolatile : 1;
            int isrestrict : 1;
            int ascall : 1;
            int pragmas;
            int unionoffset;
            int sizeFromType;
        };
        unsigned flags;
    };

    SimpleExpression* left;
    SimpleExpression* right;
    SimpleExpression* altData;
};

struct SymbolManager
{
    static SimpleSymbol* Get(struct Parser::sym* sym, bool definingFunction = false);
    static SimpleSymbol* Test(struct Parser::sym* sym);
    static SimpleExpression* Get(struct Parser::expr* e);
    static e_scc_type Get(Parser::StorageClass storage_class);
    static SimpleType* Get(struct Parser::Type* tp);
    static st_type Get(Parser::BasicType type);
    static void clear();
    static SimpleSymbol* Get(const char* name);
    static void Put(SimpleSymbol* sym);
    static SimpleSymbol* Lookup(struct Parser::sym* old);
    static void Add(struct Parser::sym* old, SimpleSymbol* sym);

  private:
    static unsigned long long Key(struct Parser::sym* old);
    static SimpleSymbol* Make(struct Parser::sym* sym);
    static std::unordered_map<unsigned long long, SimpleSymbol*> symbols;
    static std::unordered_map<std::string, SimpleSymbol*> globalSymbols;
};

bool equalnode(SimpleExpression* left, SimpleExpression* right);
inline bool isarithmeticconst(SimpleExpression* e)
{
    switch (e->type)
    case se_i:
    case se_ui:
    case se_f:
    case se_fc:
    case se_fi:
        return true;
    return false;
}
inline bool isintconst(SimpleExpression* e)
{
    switch (e->type)
    case se_i:
    case se_ui:
        return true;
    return false;
}
inline bool isfloatconst(SimpleExpression* e)
{
    switch (e->type)
    case se_f:
        return true;
    return false;
}
inline bool iscomplexconst(SimpleExpression* e)
{
    switch (e->type)
    case se_fc:
        return true;
    return false;
}
inline bool isimaginaryconst(SimpleExpression* e)
{
    switch (e->type)
    case se_fi:
        return true;
    return false;
}
inline bool isconstaddress(SimpleExpression* exp)
{
    switch (exp->type)
    {
        case se_add:
            return (isconstaddress(exp->left) || isintconst(exp->left)) && (isconstaddress(exp->right) || isintconst(exp->right));
        case se_global:
            return exp->sp->storage_class != scc_external;
        case se_pc:
        case se_labcon:
            return true;
        case se_func:
            return !exp->ascall;
        case se_threadlocal:
        default:
            return false;
    }
}

struct ArgList
{
    ArgList* next;
    SimpleType* tp;
    SimpleExpression* exp;
};
/* icode innstruction opcodes */
// clang-format off
    enum i_ops
    {
        /* note, the branches MUST be higher in the ordering than the sets */
        i_nop, i_phi, i_line, i_passthrough, i_datapassthrough, i_skipcompare,
        i_label, i_asmgoto, i_goto, i_computedgoto,
        i_gosub, i_fargosub, i_trap, i_int, i_ret,
        i_fret, i_rett, i_add, i_sub, i_udiv, i_umod, i_sdiv, i_smod, i_muluh, i_mulsh, i_mul,
        i_lsl, i_lsr, i_asr, i_neg, i_not, i_and, i_or, i_eor,
        i_setne, i_sete, i_setc, i_seta, i_setnc, i_setbe, i_setl, i_setg, i_setle, i_setge,
        i_asmcond, i_jne, i_je, i_jc, i_ja, i_jnc, i_jbe, i_jl, i_jg, i_jle, i_jge,
        i_assn, i_genword, i_coswitch, i_swbranch, i_assnblock, i_clrblock, i_cmpblock, i_parmadj, i_parmblock, i_parm,
        i_array, i_arrayindex, i_arraylsh, i_struct, i_cppini, i_block, i_blockend,
        i_dbgblock, i_dbgblockend, i_varstart, i_func, i_livein, i_icon, i_fcon, i_imcon, i_cxcon,
        i_atomic_flag_test_and_set, i_atomic_flag_clear,
        i_atomic_thread_fence, i_atomic_signal_fence, i_atomic_flag_fence, i_cmpxchgweak, i_cmpxchgstrong, 
        i_kill_dependency, i_xchg,
        i_prologue, i_epilogue, i_beginexcept, i_endexcept, i_pushcontext, i_popcontext, i_loadcontext, i_unloadcontext,
        i_tryblock, i_substack, i_parmstack, i_loadstack, i_savestack, i_functailstart, i_functailend,
        i_gcsestub, i_expressiontag, i_tag, i_seh,
        /* msil */
        i__initblk, i__cpblk, i__initobj, i__sizeof,
        /* Dag- specific stuff */
        i_var, i_const, i_ptr, i_labcon,
        /* end marker */
        i_endoflist
    };
// clang-format on

/* icode address modes annd special regs */
// clang-format off
    enum i_adr
    {
        i_none, i_immed, i_direct, i_ind, i_rret
    };
// clang-format on

/*
 * address mode
 */
typedef struct _imode_
{
    enum i_adr mode;           /* mode */
    SimpleExpression* offset;  /* offset */
    SimpleExpression* offset2; /* a second temp reg */
    SimpleExpression* offset3; /* an address */
    SimpleExpression* vararg;
    RUNTIMEDATA* runtimeData;  // this is a temporary, will be moved to the quad when it gets generated
    int scale;                 /* scale factor on the second temp reg */
    char useindx;
    char size;           /* size */
    char ptrsize;        /* ptr indirection size, either ISZ_ADDR or ISZ_FARPTR */
    char startbit, bits; /* bit width  for i_Bf*/
    char seg;            /* seg reg  for segmented architectures */
    union
    {
        struct
        {
            unsigned char vol : 1;            /* true if is a node for a volatile var */
            unsigned char restricted : 1;     /* true if pointer type is set to restricted */
            unsigned char retval : 1;         /* true if this is the value returned by a function */
            unsigned char altretval : 1;      /* true if an alternate return value should be used (e.g. SSE instead of FP)*/
            unsigned char fieldname : 1;      /* vararg is an en_structelem field name */
            unsigned char msilObject : 1;     /* true if this is an MSIL object that shouldn't be loaded by address */
            unsigned char returnRefByVal : 1; /* true if this return value is a by value reference value */
            unsigned char wasinlined : 1;     /* true if this is the result of an inlining operation */
        };
        unsigned flags;
    };
} IMODE;

/*-------------------------------------------------------------------------*/

#define IM_LIVELEFT 1
#define IM_LIVERIGHT 2
#define IM_LIVEANS 4
struct _phiblock
{
    struct _phiblock* next;
    int Tn;
    struct Block* block;
};

typedef struct _phidata
{
    int nblocks;
    int T0;
    struct _phiblock* temps;
} PHIDATA;

/*
 * icode node
 * this is also used for dag nodes
 */
// we are doing comparisons on this...   the packing on MSVC caused problems...
struct _basic_dag
{
    enum i_ops opcode; /* opcode */
    IMODE* left;       /* ans = left opcode right */
    IMODE* right;
    struct ival
    {
        union
        {
            /* values for constant nodes */
            unsigned i; // should be unsigned long long but that broke the builds for the 32 bit compiler...   will have to revist when working on 64 bit compilers.
                        // as more on this story, later coverity pointed out that using dc.v.i in iconst.cpp was broken,
                        // because all the conversions were from long long...  but just adding a new long long member
                        // to this union so i could attempt to fix it, again broke the builds...
            void* data; /* generic data, won't be filled in until after LCSE */
            PHIDATA* phi;
            long label;  // branches
        };
        FPF f;
        struct
        {
            FPF r;
            FPF i;
        } c;
    } v;
};

typedef struct quad
{
    struct _basic_dag dc;
    IMODE* ans;
    unsigned long long liveRegs;
    struct quad *fwd, *back;
    struct Block* block;
    SimpleSymbol* altsp;
    SimpleType* alttp;
    RUNTIMEDATA* runtimeData;
    ArgList* altargs;
    BITINT* uses;
    BITINT* transparent;
    BITINT* dsafe;
    BITINT* earliest;
    BITINT* delay;
    BITINT* latest;
    BITINT* isolated;
    BITINT* OCP;
    BITINT* RO;
    unsigned char *assemblyRegs;
    //	unsigned short *modifiesTnum;
    int index;
    int ansColor;
    int leftColor;
    int rightColor;
    int scaleColor;
    int definition;
    int available;
    int sourceindx;
    int copy;
    int retcount;
    union
    {
        struct
        {
            int valist : 1; /* argument is a valist that needs translation */
            int denormal : 1;
            int isvolatile : 1;
            int isrestrict : 1;
            int cxlimited : 1;
            int dead : 1;
            int loopKeep : 1;
            int live : 1;
            int alwayslive : 1;
            int OCPInserted : 1;
            int invarInserted : 1;
            int invarKeep : 1;
            int needsOCP : 1;  /* special case instruction needs to be OCP in lazy opts */
            int OCPUsed : 1;   /* answer was used in an OCP sort */
            int OCPGenned : 1; /* node was generated already in an OCP sort */
            int spill : 1;
            int ignoreMe : 1;
            int genConflict : 1; /* assignment node the ans conflicts with left */
            int hook : 1;        /* one of the two assigns for a hook, used in diagnostic generation */
            int beforeGosub : 1;
            int nullvararg : 1;
            int blockassign : 1;
            int atomic : 1;          /* atomic instruction */
            int atomicpostfetch : 1; /* fetch has result after operation... */
            int vararg : 1;          // msil
            int ptrbox : 1;          // msil - box this pointer
            int runtimeIsStore : 1;
            int moveBarrier : 1; /* can't move instructions past this point, e.g. for computed goto/label */
            int blockInit : 1;
        };
        unsigned flags;
    };
    short OCPTerms;
    char sehMode;
    unsigned char assemblyRegCount;
    unsigned assemblyTempRegStart;
    char fastcall; /* index for fastcall-related arg, positive for call sites and negative as callee */
    char oldmode;
    char novalue;
    char temps;
    char precolored;
    char moved;
    char livein;
} QUAD;

#define TEMP_ANS 1
#define TEMP_LEFT 2
#define TEMP_RIGHT 4
/*-------------------------------------------------------------------------*/

#define DAGCOMPARE sizeof(struct Optimizer::_basic_dag)

/* constant node combinattions:
 * ic = prefix
 * l = int
 * r = real
 * i = imaginary
 * c = complex
 */
// clang-format off

    enum e_icmode {
        icnone, icnl, icnr, icni, icnc,
        icln, icll, iclr, icli, iclc,
        icrn, icrl, icrr, icri, icrc,
        icin, icil, icir, icii, icic,
        iccn, iccl, iccr, icci, iccc,
        ical, icla, icaa
    };
// clang-format on
struct caseptrs
{
    int label;
    long long id;
};
struct cases
{
    long long bottom;
    long long top;
    int count;
    struct caseptrs* ptrs;
};
// clang-format off
    enum e_gt
    {
        nogen, chargen, shortgen, wchar_tgen, intgen, u16gen, u32gen, enumgen, longgen, longlonggen,
        floatgen, doublegen, longdoublegen, srrefgen
    };
// clang-format on

/* backend may have tables based on this ordering */
// clang-format off
    enum e_sg
    {
        noseg, codeseg, dataseg, bssxseg, stringseg, constseg, tlsseg, startupxseg,
        rundownxseg, tlssuseg, tlsrdseg, typeseg, symseg, browseseg,
        fixcseg, fixdseg, virtseg = 100
    };
// clang-format on

typedef struct _storetemphash
{
    struct _storetemphash* next;
    IMODE* mem;
    IMODE* temp;
} STORETEMPHASH;

typedef struct
{
    IMODE* im;
    int size;
} CASTTEMP;
}  // namespace Optimizer
#include "iopt.h"
