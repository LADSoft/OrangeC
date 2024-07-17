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

#include "ioptimizer.h"
#include "beinterfdefs.h"
#ifndef ORANGE_NO_INASM
#include "InstructionParser.h"
#include "x64Instructions.h"
#endif
#define live(mask, reg) (mask & (1 << reg))

#define REG_MAX 64
/*
 *      code generation structures and constants
 */

/* address mode specifications */
#define MAX_SEGS browseseg + 1

namespace occx86
{
enum e_lk : int;

enum fconst
{
    fczero,
    fcone,
    fcmone
};

/*      addressing mode structure       */

/*      output code structure   */

/* Used for fixup gen */
typedef struct dl
{
    struct dl* next;
    char* string;
    int offset;
    short type;
} DATALINK;

#define AMODE struct amode
#define OCODE struct ocode

#define FLOAT printf("codegen-Floating point not implemented in push/pop\n");
/* 386 register set */
#define EAX 0
#define ECX 1
#define EDX 2
#define EBX 3
#define ESP 4
#define EBP 5
#define ESI 6
#define EDI 7
#define EIP 8

#define AL 0
#define CL 1
#define DL 2
#define BL 3
#define AH 4
#define CH 5
#define DH 6
#define BH 7
#define CS 1
#define DS 2
#define ES 3
#define FS 4
#define GS 5
#define SS 6

#define OP_CODEONLY 0
#define OP_WREG02 1
#define OP_ACCREG02 2
#define OP_SEG35 3
#define OP_REGRMREG 4
#define OP_RM 5
#define OP_RMSHIFT 6
#define OP_REGRM 7
#define OP_WORDREGRM 8
#define OP_INTR 9
#define OP_SHORTBRANCH 10
#define OP_RMIMM 11
#define OP_ACCIMM 12
#define OP_ABSACC 13
#define OP_RMIMMSIGNED 14
#define OP_ACCIMMB3 15
#define OP_SEGRMSEG 16
#define OP_RET 17
#define OP_SEGBRANCH 18
#define OP_ESC 19
#define OP_BRANCH 20
#define OP_ACCDX 21
#define OP_DXACC 22
#define OP_PORTACCPORT 23
#define OP_ACCABS 24
#define OP_IMM 25
#define OP_ENTER 26
#define OP_INSWORDSIZE 27
#define OP_REGMOD 28
#define OP_NOSTRICTRM 29
#define OP_RMSHIFTB3 30
#define OP_IMUL 31
#define OP_386REG 32
#define OP_REGRMSHIFT 33
#define OP_PUSHW 34
#define OP_FLOATRM 35
#define OP_FLOATMATHP 36
#define OP_FLOATMATH 37
#define OP_FARRM 38
#define OP_WORDRMREG 39
#define OP_RMREG 40
#define OP_BITNUM 41
#define OP_MIXEDREGRM 42
#define OP_CBW 43
#define OP_REG02 44
#define OP_BYTERMREG 45
#define OP_FLOATNOPTR 46
#define OP_AX 47
#define OP_BSWAP 48
#define OP_FST 49
#define OP_FLD 50
#define OP_FBLD 51
#define OP_FILD 52
#define OP_FIST 53
#define OP_FREG 54
#define OP_FREGMAYBE 55
#define OP_FLOATIMATH 56
#define OP_INSDWORDSIZE 57
#define OP_CMPS 58
#define OP_INS 59
#define OP_LODS 60
#define OP_MOVS 61
#define OP_OUTS 62
#define OP_SCAS 63
#define OP_STOS 64
#define OP_REG03 65

typedef struct _opcode
{
    short ocoperands;
    short ocvalue;
    short ocflags;
} OPCODE;

#define OCAlwaysword 0x2000
#define OCprefixfwait 0x1000
#define OCprefix0F 0x800
#define OCtwobyte 0x200
#define OCsystem 0x100
#define OCfloatmask 0x0f0
#define OCfloatshift 4
#define OCprocmask 0x0f

typedef struct muldiv
{
    struct muldiv* next;
    long value;
    FPF floatvalue;
    int size;
    int label;
} MULDIV;

enum mode
{
    fm_label,
    fm_symbol,
    fm_rellabel,
    fm_relsymbol,
    fm_threadlocal
};

typedef struct _dbgblock
{
    struct _dbgblock* next;
    struct _dbgblock* parent;
    struct _dbgblock* child;
    Optimizer::SimpleSymbol* syms;
    int startlab;
    int endlab;
} DBGBLOCK;

#define R_EAX 0
#define R_ECX 1
#define R_EDX 2
#define R_EAXEDX 8
#define R_EDXECX 10
#define R_AL 16
#define R_CL 17
#define R_AX 24
#define R_CX 25
enum e_adtype
{
    e_ad_linedata,
    e_ad_blockdata,
    e_ad_funcdata,
    e_ad_vfuncdata,
    e_ad_vardata
};

typedef struct _attribdata
{
    enum e_adtype type;
    union
    {
        Optimizer::LINEDATA* ld;
        struct Optimizer::SimpleSymbol* sp;
#ifndef ORANGE_NO_INASM
        std::shared_ptr<Section>* section;
#endif
    } v;
    bool start;
} ATTRIBDATA;
}  // namespace occx86

enum asmTypes : int
{
    pa_oasm,
    pa_nasm,
    pa_fasm,
    pa_masm,
    pa_tasm
};
enum e_am
{
    am_none,
    am_axdx,
    am_dreg,
    am_freg,
    am_frfr,
    am_cfreg,
    am_screg,
    am_sdreg,
    am_streg,
    am_mmreg,
    am_xmmreg,
    am_seg,
    am_indisp,
    am_indispscale,
    am_fconst,
    am_direct,
    am_immed,
    am_ext,
    am_segoffs,
    /* these MUST be in this order, and last!!! */
    am_stackedtemp,
    am_stackedtempaddr
};
enum e_op
{
    op_reserved,
    op_line,
    op_blockstart,
    op_blockend,
    op_varstart,
    op_funcstart,
    op_funcend,
    op_void,
    op_comment,
    op_label,
    op_funclabel,
    op_seq,
    op_genword,
    op_dd,
    op_align
};
enum e_asmw
{
    akw_byte,
    akw_word,
    akw_dword,
    akw_fword,
    akw_qword,
    akw_tbyte,
    akw_offset,
    akw_ptr
};
struct amode
{
    enum e_am mode;
    char preg;
    char sreg;
    char tempflag;
    char scale;
    char length;
    char addrlen;
    char seg;
#define e_default 0
#define e_cs 1
#define e_ds 2
#define e_es 3
#define e_fs 4
#define e_gs 5
#define e_ss 6
    Optimizer::SimpleExpression* offset;
    int liveRegs;
    int keepesp : 1;
};
#ifdef __cplusplus
struct ocode
{
    struct ocode *fwd, *back;
#ifndef ORANGE_NO_INASM
    enum e_opcode opcode;
#endif
    struct amode *oper1, *oper2, *oper3;
    void* ins;
    int size;
    int blocknum;
    char diag;
    char noopt;
};
#endif
