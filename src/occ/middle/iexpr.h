/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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
 * iexpr.h
 *
 * ICODE structures
 */

#define F_VOL 1
#define F_NOVALUE 2
#define F_ADDR 4
#define F_COMPARE 8
#define F_STORE 16
#define F_SWITCHVALUE 32
#define F_OBJECT 64
#define F_INRETURN 128

/* icode innstruction opcodes */
// clang-format off
enum i_ops
{
        /* note, the branches MUST be higher in the ordering than the sets */
        i_nop, i_phi, i_line, i_passthrough, i_datapassthrough, i_skipcompare,
        i_label, i_asmgoto, i_goto, i_directbranch, 
        i_gosub, i_fargosub, i_trap, i_int, i_ret,
        i_fret, i_rett, i_add, i_sub, i_udiv, i_umod, i_sdiv, i_smod, i_muluh, i_mulsh, i_mul,
        i_lsl, i_lsr, i_asr, i_neg, i_not, i_and, i_or, i_eor,
        i_setne, i_sete, i_setc, i_seta, i_setnc, i_setbe, i_setl, i_setg, i_setle, i_setge,
        i_asmcond, i_jne, i_je, i_jc, i_ja, i_jnc, i_jbe, i_jl, i_jg, i_jle, i_jge,  
        i_assn, i_genword, i_coswitch, i_swbranch, i_assnblock, i_clrblock, i_parmadj, i_parmblock, i_parm,
        i_array, i_arrayindex, i_arraylsh, i_struct, i_cppini, i_block, i_blockend, 
        i_dbgblock, i_dbgblockend, i_varstart, i_func, i_livein, i_icon, i_fcon, i_imcon, i_cxcon, 
        i_atomic_flag_test_and_set, i_atomic_flag_clear,
        i_atomic_fence, i_atomic_flag_fence, i_cmpswp, i_xchg,
        i_prologue, i_epilogue, i_pushcontext, i_popcontext, i_loadcontext, i_unloadcontext,
        i_tryblock, i_substack, i_parmstack, i_loadstack, i_savestack, i_functailstart, i_functailend,
        i_gcsestub, i_expressiontag, i_tag, i_seh,
        /* msil */
        i__initblk, i__cpblk,
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
    enum i_adr mode;      /* mode */
    struct expr* offset;  /* offset */
    struct expr* offset2; /* a second temp reg */
    struct expr* offset3; /* an address */
    struct expr *vararg;
//    struct exprlist
//    {
//        struct exprlist* next;
//        struct expr* offset;
//    } * vararg;
    int scale; /* scale factor on the second temp reg */
    char useindx;
    char size;                    /* size */
    char ptrsize;                 /* ptr indirection size, either ISZ_ADDR or ISZ_FARPTR */
    char startbit, bits;          /* bit width  for i_Bf*/
    char seg;                     /* seg reg  for segmented architectures */
    unsigned char vol : 1;        /* true if is a node for a volatile var */
    unsigned char restricted : 1; /* true if pointer type is set to restricted */
    unsigned char retval : 1;     /* true if this is the value returned by a function */
    unsigned char altretval : 1;  /* true if an alternate return value should be used (e.g. SSE instead of FP)*/
    unsigned char fieldname : 1;  /* vararg is an en_structelem field name */
    unsigned char msilObject : 1; /* true if this is an MSIL object that shouldn't be loaded by address */
} IMODE;

/*-------------------------------------------------------------------------*/

#define IM_LIVELEFT 1
#define IM_LIVERIGHT 2
#define IM_LIVEANS 4
struct _phiblock
{
    struct _phiblock* next;
    int Tn;
    struct _block* block;
};

typedef struct _phidata
{
    int nblocks;
    int T0;
    struct _phiblock *temps;
} PHIDATA;

/*
 * icode node
 * this is also used for dag nodes
 */
struct _basic_dag
{
    enum i_ops opcode; /* opcode */
    IMODE* left;       /* ans = left opcode right */
    IMODE* right;
    union ival
    {
        /* values for constant nodes */
        LLONG_TYPE i;
        FPFC f;
        struct
        {
            FPFC r;
            FPFC i;
        } c;
        void* data; /* generic data, won't be filled in until after LCSE */
        PHIDATA* phi;
        long label;  // branches
    } v;
};

typedef struct quad
{
    struct _basic_dag dc;
    IMODE* ans;
    ULLONG_TYPE liveRegs;
    struct quad *fwd, *back;
    struct _block* block;
    void* altdata;
    BITINT* uses;
    BITINT* transparent;
    BITINT* dsafe;
    BITINT* earliest;
    BITINT* delay;
    BITINT* latest;
    BITINT* isolated;
    BITINT* OCP;
    BITINT* RO;
    struct expr* valist; /* argument is a valist that needs translation */
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
    char sehMode;
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
    int vararg : 1;      /* a param passed as a vararg */
    int varargPrev : 1;  /* right before the vararg is genned */
    int beforeGosub : 1;
    int nullvararg : 1;
    int blockassign : 1;
    int atomic : 1; /* atomic instruction */
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

#define DAGCOMPARE sizeof(struct _basic_dag)
#define DAGSIZE 251

/* constant node combinattions:
 * ic = prefix
 * l = int
 * r = real
 * i = imaginary
 * c = complex
 */
// clang-format off

enum e_icmode {
    icnone,icnl,icnr,icni,icnc, 
    icln,icll,iclr,icli,iclc, 
    icrn,icrl,icrr,icri,icrc,
    icin,icil,icir,icii,icic,
    iccn,iccl,iccr,icci,iccc,
    ical,icla,icaa
};
// clang-format on
struct caseptrs
{
    int label;
    LLONG_TYPE id;
};
struct cases
{
    LLONG_TYPE bottom;
    LLONG_TYPE top;
    int count;
    struct caseptrs *ptrs;
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

typedef struct _casttemphash
{
    struct _casttemphash* next;
    struct
    {
        IMODE* im;
        int size;
    } sf;
    IMODE* rv;
} CASTTEMPHASH;