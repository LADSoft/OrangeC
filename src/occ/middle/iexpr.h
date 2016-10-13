/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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
/* icode innstruction opcodes */
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
        i_atomic_fence, i_atomic_flag_fence, i_cmpswp,
        i_prologue, i_epilogue, i_pushcontext, i_popcontext, i_loadcontext, i_unloadcontext,
        i_tryblock, i_substack, i_parmstack, i_loadstack, i_savestack, i_functailstart, i_functailend,
        i_gcsestub, i_expressiontag, i_tag,
     /* Dag- specific stuff */
        i_var, i_const, i_ptr, i_labcon,
        /* end marker */
        i_endoflist
};

/* icode address modes annd special regs */
enum i_adr
{
    i_none, i_immed, i_direct, i_ind, i_rret
};

/*
 * address mode
 */
typedef struct _imode_
{
    enum i_adr mode; /* mode */
    struct expr *offset; /* offset */
    struct expr *offset2; /* a second temp reg */
    struct expr *offset3; /* an address */
    struct exprlist {
        struct exprlist *next;
        struct expr *offset;
    } *vararg;
    int scale;				/* scale factor on the second temp reg */
    char useindx;
    char size; /* size */
    char ptrsize; /* ptr indirection size, either ISZ_ADDR or ISZ_FARPTR */
    char startbit, bits; /* bit width  for i_Bf*/
    char seg;	/* seg reg  for segmented architectures */
    char vol; /* TRUE if is a node for a volatile var */
    char restricted; /* TRUE if pointer type is set to restricted */
    char retval;	/* TRUE if this is the value returned by a function */
    char fieldname; /* vararg is an en_structelem field name */
} IMODE;

/*-------------------------------------------------------------------------*/


#define IM_LIVELEFT 1
#define IM_LIVERIGHT 2
#define IM_LIVEANS 4
typedef struct _phidata
{
    int nblocks;
    int T0;
    struct _phiblock
    {
        struct _phiblock *next;
        int Tn;
        struct _block *block;
    } *temps;
} PHIDATA;

/*
 * icode node
 * this is also used for dag nodes
 */

typedef struct quad
{
    struct _basic_dag
    {
        enum i_ops opcode; /* opcode */
        IMODE *left; /* ans = left opcode right */
        IMODE *right;
        union ival
        {
             /* values for constant nodes */
            LLONG_TYPE i;
            FPF f;
            struct {
                FPF r;
                FPF i;
            } c;
            void *data ; /* generic data, won't be filled in until after LCSE */
            PHIDATA *phi;
            long label; // branches
        } v;
    }
    dc;
    IMODE *ans;
    ULLONG_TYPE liveRegs;
    struct quad *fwd, *back;
    struct _block *block;
    void *altdata;
    BITINT *uses;
    BITINT *transparent;
    BITINT *dsafe;
    BITINT *earliest;
    BITINT *delay;
    BITINT *latest;
    BITINT *isolated;
    BITINT *OCP;
    BITINT *RO;
    struct expr * valist; /* argument is a valist that needs translation */
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
    int denormal:1;
    int isvolatile:1;
    int isrestrict:1;
    int cxlimited:1;
    int dead:1;
    int loopKeep:1;
    int live:1;
    int alwayslive:1;
    int OCPInserted:1;
    int invarInserted:1;
    int invarKeep:1;
    int needsOCP:1;  /* special case instruction needs to be OCP in lazy opts */
    int OCPUsed:1; /* answer was used in an OCP sort */
    int OCPGenned:1; /* node was generated already in an OCP sort */
    int spill:1;
    int ignoreMe:1;
    int genConflict:1; /* assignment node the ans conflicts with left */
    int hook:1; /* one of the two assigns for a hook, used in diagnostic generation */
    int vararg:1; /* a param passed as a vararg */
    int varargPrev:1; /* right before the vararg is genned */
    int beforeGosub:1;
    int nullvararg:1;
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
enum e_icmode {
    icnone,icnl,icnr,icni,icnc, 
    icln,icll,iclr,icli,iclc, 
    icrn,icrl,icrr,icri,icrc,
    icin,icil,icir,icii,icic,
    iccn,iccl,iccr,icci,iccc,
    ical,icla,icaa
};
struct cases {
    LLONG_TYPE bottom;
    LLONG_TYPE top;
    int count;
    struct caseptrs {
        int label;
        LLONG_TYPE id;
    } *ptrs;
} ;

enum e_gt
{
    nogen, chargen, shortgen, wchar_tgen, intgen, u16gen, u32gen, enumgen, longgen, longlonggen,
    floatgen, doublegen, longdoublegen, srrefgen
};
/* backend may have tables based on this ordering */
enum e_sg
{
    noseg, codeseg, dataseg, bssxseg, stringseg, constseg, tlsseg, startupxseg,
        rundownxseg, tlssuseg, tlsrdseg, typeseg, symseg, browseseg, 
        fixcseg, fixdseg, virtseg = 100
};
typedef struct _storetemphash
{
    struct _storetemphash *next;
    IMODE *mem;
    IMODE *temp;
} STORETEMPHASH ;

typedef struct _casttemphash
{
    struct _casttemphash *next;
    struct {
        IMODE *im;
        int size;
    } sf ;
    IMODE *rv;
} CASTTEMPHASH;