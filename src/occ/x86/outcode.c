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
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "be.h"
 
#define FULLVERSION

extern ARCH_ASM *chosenAssembler;
extern MULDIV *muldivlink;
extern ASMNAME oplst[];
extern enum e_sg oa_currentSeg;
extern DBGBLOCK *DbgBlocks[];
extern SYMBOL *theCurrentFunc;

int outcode_base_address;

EMIT_TAB segs[MAX_SEGS];
LIST *includedFiles ;
VIRTUAL_LIST *virtualFirst, *virtualLast;

static int virtualSegmentNumber;
static LABEL **labelbuf;
static int lastIncludeNum;
/*char segregs[] = "CSDSESFSGSSS";*/
static UBYTE segoverxlattab[] = 
{
    0x3e, 0x2e, 0x3e, 0x26, 0x64, 0x65, 0x36
};
static UBYTE segtab[] = 
{
    0x18, 0x8, 0x18, 0x0, 0x20, 0x28, 0x10
};

OPCODE popn_aaa[] = 
{
    {
        OP_CODEONLY, 0x0037, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_aad[] = 
{
    {
        OP_CODEONLY, 0x0ad5, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_aam[] = 
{
    {
        OP_CODEONLY, 0x0ad4, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_aas[] = 
{
    {
        OP_CODEONLY, 0x003f, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_adc[] = 
{
    {
        OP_RMIMMSIGNED, 0x1080, 0x0200
    }
    , 
    {
        OP_ACCIMM, 0x0014, 0x0000
    }
    , 
    {
        OP_REGRMREG, 0x0010, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_add[] = 
{
    {
        OP_RMIMMSIGNED, 0x0080, 0x0200
    }
    , 
    {
        OP_ACCIMM, 0x0004, 0x0000
    }
    , 
    {
        OP_REGRMREG, 0x0000, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_and[] = 
{
    {
        OP_RMIMMSIGNED, 0x2080, 0x0200
    }
    , 
    {
        OP_ACCIMM, 0x0024, 0x0000
    }
    , 
    {
        OP_REGRMREG, 0x0020, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_arpl[] = 
{
    {
        OP_WORDRMREG, 0x0063, 0x2302
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_bound[] = 
{
    {
        OP_WORDREGRM, 0x0062, 0x0201
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_bsf[] = 
{
    {
        OP_WORDREGRM, 0x00bc, 0x0a03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_bsr[] = 
{
    {
        OP_WORDREGRM, 0x00bd, 0x0a04
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_bswap[] = 
{
    {
        OP_BSWAP, 0x00c8, 0x0804
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_bt[] = 
{
    {
        OP_BITNUM, 0x20ba, 0x0a03
    }
    , 
    {
        OP_WORDRMREG, 0x00a3, 0x0a03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_btc[] = 
{
    {
        OP_BITNUM, 0x38ba, 0x0a03
    }
    , 
    {
        OP_WORDRMREG, 0x00bb, 0x0a03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_btr[] = 
{
    {
        OP_BITNUM, 0x30ba, 0x0a03
    }
    , 
    {
        OP_WORDRMREG, 0x00b3, 0x0a03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_bts[] = 
{
    {
        OP_BITNUM, 0x28ba, 0x0a03
    }
    , 
    {
        OP_WORDRMREG, 0x00ab, 0x0a03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_call[] = 
{
    {
        OP_BRANCH, 0x00e8, 0x0000
    }
    , 
    {
        OP_RM, 0x10ff, 0x0200
    }
    , 
    {
        OP_SEGBRANCH, 0x009a, 0x0000
    }
    , 
    {
        OP_FARRM, 0x18ff, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cbw[] = 
{
    {
        OP_CBW, 0x0098, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cdq[] = 
{
    {
        OP_INSDWORDSIZE, 0x0099, 0x0003
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_clc[] = 
{
    {
        OP_CODEONLY, 0x00f8, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cld[] = 
{
    {
        OP_CODEONLY, 0x00fc, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cli[] = 
{
    {
        OP_CODEONLY, 0x00fa, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_clts[] = 
{
    {
        OP_CODEONLY, 0x0006, 0x0803
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmc[] = 
{
    {
        OP_CODEONLY, 0x00f5, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmova[] = 
{
    {
        OP_WORDREGRM, 0x0047, 0x0a06
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmovb[] = 
{
    {
        OP_WORDREGRM, 0x0042, 0x0a06
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmovbe[] = 
{
    {
        OP_WORDREGRM, 0x0046, 0x0a06
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmovg[] = 
{
    {
        OP_WORDREGRM, 0x004f, 0x0a06
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmovge[] = 
{
    {
        OP_WORDREGRM, 0x004d, 0x0a06
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmovl[] = 
{
    {
        OP_WORDREGRM, 0x004c, 0x0a06
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmovle[] = 
{
    {
        OP_WORDREGRM, 0x004e, 0x0a06
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmovnb[] = 
{
    {
        OP_WORDREGRM, 0x0043, 0x0a06
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmovno[] = 
{
    {
        OP_WORDREGRM, 0x0041, 0x0a06
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmovnp[] = 
{
    {
        OP_WORDREGRM, 0x004b, 0x0a06
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmovns[] = 
{
    {
        OP_WORDREGRM, 0x0049, 0x0a06
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmovnz[] = 
{
    {
        OP_WORDREGRM, 0x0045, 0x0a06
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmovo[] = 
{
    {
        OP_WORDREGRM, 0x0040, 0x0a06
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmovp[] = 
{
    {
        OP_WORDREGRM, 0x004a, 0x0a06
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmovs[] = 
{
    {
        OP_WORDREGRM, 0x0048, 0x0a06
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmovz[] = 
{
    {
        OP_WORDREGRM, 0x0044, 0x0a06
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmp[] = 
{
    {
        OP_RMIMMSIGNED, 0x3880, 0x0200
    }
    , 
    {
        OP_ACCIMM, 0x003c, 0x0000
    }
    , 
    {
        OP_REGRMREG, 0x0038, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmps[] = 
{
    {
        OP_CMPS, 0x00a7, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmpsb[] = 
{
    {
        OP_CODEONLY, 0x00a6, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmpsd[] = 
{
    {
        OP_INSDWORDSIZE, 0x00a7, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmpsw[] = 
{
    {
        OP_INSWORDSIZE, 0x00a7, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmpxchg[] = 
{
    {
        OP_BYTERMREG, 0x00b0, 0x0a04
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cmpxchg8b[] = 
{
    {
        OP_NOSTRICTRM, 0x08c7, 0x0a05
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cpuid[] = 
{
    {
        OP_CODEONLY, 0x00a2, 0x0805
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cwd[] = 
{
    {
        OP_CBW, 0x0099, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_cwde[] = 
{
    {
        OP_INSDWORDSIZE, 0x0098, 0x0003
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_daa[] = 
{
    {
        OP_CODEONLY, 0x0027, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_das[] = 
{
    {
        OP_CODEONLY, 0x002f, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_dec[] = 
{
    {
        OP_WREG02, 0x0048, 0x0000
    }
    , 
    {
        OP_RM, 0x08fe, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_div[] = 
{
    {
        OP_RM, 0x30f6, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_enter[] = 
{
    {
        OP_ENTER, 0x00c8, 0x0002
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_esc[] = 
{
    {
        OP_ESC, 0x00d8, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_f2xm1[] = 
{
    {
        OP_CODEONLY, 0xf0d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fabs[] = 
{
    {
        OP_CODEONLY, 0xe1d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fadd[] = 
{
    {
        OP_FLOATMATH, 0x00d8, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_faddp[] = 
{
    {
        OP_FLOATMATHP, 0xc0de, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fbld[] = 
{
    {
        OP_FBLD, 0x20df, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fbstp[] = 
{
    {
        OP_FBLD, 0x30df, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fchs[] = 
{
    {
        OP_CODEONLY, 0xe0d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fnclex[] = 
{
    {
        OP_CODEONLY, 0xe2db, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fclex[] = 
{
    {
        OP_CODEONLY, 0xe2db, 0x1200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fcom[] = 
{
    {
        OP_CODEONLY, 0xd1d8, 0x0200
    }
    , 
    {
        OP_FLOATRM, 0x10d8, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fcomp[] = 
{
    /*		{OP_CODEONLY,0xd9d8,0x0200},*/
    {
        OP_FREG, 0xd8d8, 0x0200
    }
    , 
    {
        OP_FLOATRM, 0x18d8, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fcompp[] = 
{
    {
        OP_CODEONLY, 0xd9de, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fcos[] = 
{
    {
        OP_CODEONLY, 0xffd9, 0x0230
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fdecstp[] = 
{
    {
        OP_CODEONLY, 0xf6d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fdisi[] = 
{
    {
        OP_CODEONLY, 0xe1db, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fdiv[] = 
{
    {
        OP_FLOATMATH, 0xf0dc, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fdivp[] = 
{
    {
        OP_FLOATMATHP, 0xf8de, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fdivr[] = 
{
    {
        OP_FLOATMATH, 0xf8dc, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fdivrp[] = 
{
    {
        OP_FLOATMATHP, 0xf0de, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_feni[] = 
{
    {
        OP_CODEONLY, 0xe0db, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_ffree[] = 
{
    {
        OP_FREG, 0xc0dd, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fiadd[] = 
{
    {
        OP_FLOATIMATH, 0x00da, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_ficom[] = 
{
    {
        OP_FLOATIMATH, 0x10da, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_ficomp[] = 
{
    {
        OP_FLOATIMATH, 0x18da, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fidiv[] = 
{
    {
        OP_FLOATIMATH, 0x30da, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fidivr[] = 
{
    {
        OP_FLOATIMATH, 0x38da, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fild[] = 
{
    {
        OP_FILD, 0x00db, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fimul[] = 
{
    {
        OP_FLOATIMATH, 0x08da, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fincstp[] = 
{
    {
        OP_CODEONLY, 0xf7d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_finit[] = 
{
    {
        OP_CODEONLY, 0xe3db, 0x1200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fninit[] = 
{
    {
        OP_CODEONLY, 0xe3db, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fist[] = 
{
    {
        OP_FIST, 0x10db, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fistp[] = 
{
    {
        OP_FILD, 0x18db, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fisub[] = 
{
    {
        OP_FLOATIMATH, 0x20da, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fisubr[] = 
{
    {
        OP_FLOATIMATH, 0x28da, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fld[] = 
{
    {
        OP_FLD, 0xC0d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fld1[] = 
{
    {
        OP_CODEONLY, 0xe8d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fldcw[] = 
{
    {
        OP_FLOATNOPTR, 0x28d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fldenv[] = 
{
    {
        OP_FLOATNOPTR, 0x20d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fldl2e[] = 
{
    {
        OP_CODEONLY, 0xead9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fldl2t[] = 
{
    {
        OP_CODEONLY, 0xe9d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fldlg2[] = 
{
    {
        OP_CODEONLY, 0xecd9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fldln2[] = 
{
    {
        OP_CODEONLY, 0xedd9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fldpi[] = 
{
    {
        OP_CODEONLY, 0xebd9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fldz[] = 
{
    {
        OP_CODEONLY, 0xeed9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fmul[] = 
{
    {
        OP_FLOATMATH, 0x08d8, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fmulp[] = 
{
    {
        OP_FLOATMATHP, 0xc8de, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fnop[] = 
{
    {
        OP_CODEONLY, 0xd0d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fnsave[] = 
{
    {
        OP_FLOATNOPTR, 0x30dd, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fsave[] = 
{
    {
        OP_FLOATNOPTR, 0x30dd, 0x1200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fnstcw[] = 
{
    {
        OP_FLOATNOPTR, 0x38d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fstcw[] = 
{
    {
        OP_FLOATNOPTR, 0x38d9, 0x1200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fnstenv[] = 
{
    {
        OP_FLOATNOPTR, 0x30d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fstenv[] = 
{
    {
        OP_FLOATNOPTR, 0x30d9, 0x1200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fnstsw[] = 
{
    {
        OP_AX, 0xe0df, 0x0200
    }
    , 
    {
        OP_FLOATNOPTR, 0x38dd, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fstsw[] = 
{
    {
        OP_AX, 0xe0df, 0x1200
    }
    , 
    {
        OP_FLOATNOPTR, 0x38dd, 0x1200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fpatan[] = 
{
    {
        OP_CODEONLY, 0xf3d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fprem[] = 
{
    {
        OP_CODEONLY, 0xf8d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fprem1[] = 
{
    {
        OP_CODEONLY, 0xf5d9, 0x0230
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fptan[] = 
{
    {
        OP_CODEONLY, 0xf2d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_frndint[] = 
{
    {
        OP_CODEONLY, 0xfcd9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_frstor[] = 
{
    {
        OP_FLOATNOPTR, 0x20dd, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fscale[] = 
{
    {
        OP_CODEONLY, 0xfdd9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fsetpm[] = 
{
    {
        OP_CODEONLY, 0xe4db, 0x0220
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fsin[] = 
{
    {
        OP_CODEONLY, 0xfed9, 0x0230
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fsincos[] = 
{
    {
        OP_CODEONLY, 0xfbd9, 0x0230
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fsqrt[] = 
{
    {
        OP_CODEONLY, 0xfad9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fst[] = 
{
    {
        OP_FST, 0x10d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fstp[] = 
{
    {
        OP_FLD, 0x18d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fsub[] = 
{
    {
        OP_FLOATMATH, 0x20d8, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fsubp[] = 
{
    {
        OP_FLOATMATHP, 0xe8de, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fsubr[] = 
{
    {
        OP_FLOATMATH, 0x28d8, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fsubrp[] = 
{
    {
        OP_FLOATMATHP, 0xe0de, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_ftst[] = 
{
    {
        OP_CODEONLY, 0xe4d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fucom[] = 
{
    {
        OP_CODEONLY, 0xe1dd, 0x0230
    }
    , 
    {
        OP_FREGMAYBE, 0xe0dd, 0x0230
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fucomp[] = 
{
    {
        OP_CODEONLY, 0xe9dd, 0x0200
    }
    , 
    {
        OP_FREGMAYBE, 0xe8dd, 0x0230
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fucompp[] = 
{
    {
        OP_CODEONLY, 0xe9da, 0x0230
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fwait[] = 
{
    {
        OP_CODEONLY, 0x009b, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fxam[] = 
{
    {
        OP_CODEONLY, 0xe5d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fxch[] = 
{
    {
        OP_CODEONLY, 0xc9d9, 0x0200
    }
    , 
    {
        OP_FREGMAYBE, 0xc8d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fxtract[] = 
{
    {
        OP_CODEONLY, 0xf4d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fyl2x[] = 
{
    {
        OP_CODEONLY, 0xf1d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_fyl2xp1[] = 
{
    {
        OP_CODEONLY, 0xf9d9, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_hlt[] = 
{
    {
        OP_CODEONLY, 0x00f4, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_ibts[] = 
{
    {
        OP_CODEONLY, 0x00a7, 0x0803
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_idiv[] = 
{
    {
        OP_RM, 0x38f6, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_imul[] = 
{
    {
        OP_IMUL, 0x0069, 0x0003
    }
    , 
    {
        OP_WORDREGRM, 0x00af, 0x0a03
    }
    , 
    {
        OP_RM, 0x28f6, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_in[] = 
{
    {
        OP_PORTACCPORT, 0x00e4, 0x0000
    }
    , 
    {
        OP_ACCDX, 0x00ec, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_inc[] = 
{
    {
        OP_WREG02, 0x0040, 0x0200
    }
    , 
    {
        OP_RM, 0x00fe, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_ins[] = 
{
    {
        OP_INS, 0x006d, 0x0802
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_insb[] = 
{
    {
        OP_CODEONLY, 0x006c, 0x0003
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_insd[] = 
{
    {
        OP_INSDWORDSIZE, 0x006d, 0x0003
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_insw[] = 
{
    {
        OP_INSWORDSIZE, 0x006d, 0x0003
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_int[] = 
{
    {
        OP_INTR, 0x00cc, 0x0000
    }
    , 
    {
        OP_INTR, 0x00cd, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_int3[] = 
{
    {
        OP_CODEONLY, 0x00cc, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_into[] = 
{
    {
        OP_CODEONLY, 0x00ce, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_invd[] = 
{
    {
        OP_CODEONLY, 0x0008, 0x0904
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_invlpg[] = 
{
    {
        OP_CODEONLY, 0x0008, 0x0904
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_iret[] = 
{
    {
        OP_INSDWORDSIZE, 0x00cf, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_iretd[] = 
{
    {
        OP_INSDWORDSIZE, 0x00cf, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_ja[] = 
{
    {
        OP_BRANCH, 0x0087, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0077, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jae[] = 
{
    {
        OP_BRANCH, 0x0083, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0073, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jb[] = 
{
    {
        OP_BRANCH, 0x0082, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0072, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jbe[] = 
{
    {
        OP_BRANCH, 0x0086, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0076, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jc[] = 
{
    {
        OP_BRANCH, 0x0082, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0072, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jcxz[] = 
{
    {
        OP_SHORTBRANCH, 0x00e3, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_je[] = 
{
    {
        OP_BRANCH, 0x0084, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0074, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jg[] = 
{
    {
        OP_BRANCH, 0x008f, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x007f, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jge[] = 
{
    {
        OP_BRANCH, 0x008d, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x007d, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jl[] = 
{
    {
        OP_BRANCH, 0x008c, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x007c, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jle[] = 
{
    {
        OP_BRANCH, 0x008e, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x007e, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jmp[] = 
{
    {
        OP_RM, 0x20ff, 0x0000
    }
    , 
    {
        OP_BRANCH, 0x00e9, 0x0000
    }
    , 
    {
        OP_SHORTBRANCH, 0x00eb, 0x0000
    }
    , 
    {
        OP_SEGBRANCH, 0x00ea, 0x0000
    }
    , 
    {
        OP_FARRM, 0x28ff, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jna[] = 
{
    {
        OP_BRANCH, 0x0086, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0076, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jnae[] = 
{
    {
        OP_BRANCH, 0x0082, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0072, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jnb[] = 
{
    {
        OP_BRANCH, 0x0083, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0073, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jnbe[] = 
{
    {
        OP_BRANCH, 0x0087, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0077, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jnc[] = 
{
    {
        OP_BRANCH, 0x0083, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0073, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jne[] = 
{
    {
        OP_BRANCH, 0x0085, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0075, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jng[] = 
{
    {
        OP_BRANCH, 0x008e, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x007e, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jnge[] = 
{
    {
        OP_BRANCH, 0x008c, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x007c, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jnl[] = 
{
    {
        OP_BRANCH, 0x008d, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x007d, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jnle[] = 
{
    {
        OP_BRANCH, 0x008f, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x007f, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jno[] = 
{
    {
        OP_BRANCH, 0x0081, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0071, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jnp[] = 
{
    {
        OP_BRANCH, 0x008b, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x007b, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jns[] = 
{
    {
        OP_BRANCH, 0x0089, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0079, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jnz[] = 
{
    {
        OP_BRANCH, 0x0085, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0075, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jo[] = 
{
    {
        OP_BRANCH, 0x0080, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0070, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jp[] = 
{
    {
        OP_BRANCH, 0x008a, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x007a, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jpe[] = 
{
    {
        OP_BRANCH, 0x008a, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x007a, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jpo[] = 
{
    {
        OP_BRANCH, 0x008b, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x007b, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_js[] = 
{
    {
        OP_BRANCH, 0x0088, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0078, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_jz[] = 
{
    {
        OP_BRANCH, 0x0084, 0x0803
    }
    , 
    {
        OP_SHORTBRANCH, 0x0074, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lahf[] = 
{
    {
        OP_CODEONLY, 0x009f, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lar[] = 
{
    {
        OP_WORDREGRM, 0x0002, 0x0a02
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lds[] = 
{
    {
        OP_WORDREGRM, 0x00c5, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lea[] = 
{
    {
        OP_WORDREGRM, 0x008d, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_leave[] = 
{
    {
        OP_CODEONLY, 0x00c9, 0x0002
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lfence[] =
{
    {
        OP_CODEONLY, 0xe8ae, 0x0a06
    }
    ,
    {
        0, 0, 0
    }
    , 
} ;
OPCODE popn_les[] = 
{
    {
        OP_WORDREGRM, 0x00c4, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lfs[] = 
{
    {
        OP_WORDREGRM, 0x00b4, 0x0a03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lgdt[] = 
{
    {
        OP_NOSTRICTRM, 0x1001, 0x0b02
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lgs[] = 
{
    {
        OP_WORDREGRM, 0x00b5, 0x0a03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lidt[] = 
{
    {
        OP_NOSTRICTRM, 0x1801, 0x0b02
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lldt[] = 
{
    {
        OP_NOSTRICTRM, 0x1000, 0x0b02
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lmsw[] = 
{
    {
        OP_NOSTRICTRM, 0x3001, 0x0b03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lock[] = 
{
    {
        OP_CODEONLY, 0x00f0, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lods[] = 
{
    {
        OP_LODS, 0x00ad, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lodsb[] = 
{
    {
        OP_CODEONLY, 0x00ac, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lodsd[] = 
{
    {
        OP_INSDWORDSIZE, 0x00ad, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lodsw[] = 
{
    {
        OP_INSWORDSIZE, 0x00ad, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_loop[] = 
{
    {
        OP_SHORTBRANCH, 0x00e2, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_loope[] = 
{
    {
        OP_SHORTBRANCH, 0x00e1, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_loopne[] = 
{
    {
        OP_SHORTBRANCH, 0x00e0, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_loopnz[] = 
{
    {
        OP_SHORTBRANCH, 0x00e0, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_loopz[] = 
{
    {
        OP_SHORTBRANCH, 0x00e1, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lsl[] = 
{
    {
        OP_WORDREGRM, 0x0003, 0x0a02
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_lss[] = 
{
    {
        OP_WORDREGRM, 0x00b2, 0x0a03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_ltr[] = 
{
    {
        OP_NOSTRICTRM, 0x1800, 0x0b03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_mfence[] =
{
    {
        OP_CODEONLY, 0xf0ae, 0x0a06
    }
    ,
    {
        0, 0, 0
    }
    , 
} ;
OPCODE popn_mov[] = 
{
    {
        OP_REG03, 0x00b8, 0x0000
    }
    , 
    {
        OP_ABSACC, 0x00a2, 0x0000
    }
    , 
    {
        OP_ACCABS, 0x00a0, 0x0000
    }
    , 
    {
        OP_ACCIMMB3, 0x00b0, 0x0000
    }
    , 
    {
        OP_REGRMREG, 0x0088, 0x0200
    }
    , 
    {
        OP_RMIMM, 0x00c6, 0x0200
    }
    , 
    {
        OP_386REG, 0xc020, 0x0b03
    }
    , 
    {
        OP_SEGRMSEG, 0x008c, 0x0200
    }
    ,
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_movs[] = 
{
    {
        OP_MOVS, 0x00a5, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_movsb[] = 
{
    {
        OP_CODEONLY, 0x00a4, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_movsd[] = 
{
    {
        OP_INSDWORDSIZE, 0x00a5, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_movsw[] = 
{
    {
        OP_INSWORDSIZE, 0x00a5, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_movsx[] = 
{
    {
        OP_MIXEDREGRM, 0x00be, 0x0a03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_movzx[] = 
{
    {
        OP_MIXEDREGRM, 0x00b6, 0x0a03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_mul[] = 
{
    {
        OP_RM, 0x20f6, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_neg[] = 
{
    {
        OP_RM, 0x18f6, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_nop[] = 
{
    {
        OP_CODEONLY, 0x0090, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_not[] = 
{
    {
        OP_RM, 0x10f6, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_or[] = 
{
    {
        OP_RMIMMSIGNED, 0x0880, 0x0200
    }
    , 
    {
        OP_ACCIMM, 0x000c, 0x0000
    }
    , 
    {
        OP_REGRMREG, 0x0008, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_out[] = 
{
    {
        OP_DXACC, 0x00ee, 0x0000
    }
    , 
    {
        OP_PORTACCPORT, 0x00e6, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_outs[] = 
{
    {
        OP_OUTS, 0x006f, 0x0803
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_outsb[] = 
{
    {
        OP_CODEONLY, 0x006e, 0x0003
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_outsd[] = 
{
    {
        OP_INSDWORDSIZE, 0x006f, 0x0003
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_outsw[] = 
{
    {
        OP_INSWORDSIZE, 0x006f, 0x0003
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_pop[] = 
{
    {
        OP_WREG02, 0x0058, 0x0000
    }
    , 
    {
        OP_PUSHW, 0x008f, 0x0000
    }
    , 
    {
        OP_SEG35, 0x0007, 0x0000
    }
    , 
    {
        OP_SEG35, 0x00a1, 0x0803
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_popa[] = 
{
    {
        OP_INSWORDSIZE, 0x0061, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_popad[] = 
{
    {
        OP_INSDWORDSIZE, 0x0061, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_popf[] = 
{
    {
        OP_INSWORDSIZE, 0x009d, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_popfd[] = 
{
    {
        OP_INSDWORDSIZE, 0x009d, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_push[] = 
{
    {
        OP_WREG02, 0x0050, 0x0000
    }
    , 
    {
        OP_IMM, 0x0068, 0x0002
    }
    , 
    {
        OP_PUSHW, 0x30ff, 0x0000
    }
    , 
    {
        OP_SEG35, 0x0006, 0x0000
    }
    , 
    {
        OP_SEG35, 0x00a0, 0x0803
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_pusha[] = 
{
    {
        OP_INSWORDSIZE, 0x0060, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_pushad[] = 
{
    {
        OP_INSDWORDSIZE, 0x0060, 0x0003
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_pushf[] = 
{
    {
        OP_INSWORDSIZE, 0x009c, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_pushfd[] = 
{
    {
        OP_INSDWORDSIZE, 0x009c, 0x0003
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_rcl[] = 
{
    {
        OP_RMSHIFT, 0x10c0, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_rcr[] = 
{
    {
        OP_RMSHIFT, 0x18c0, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_rdmsr[] = 
{
    {
        OP_CODEONLY, 0x0032, 0x0905
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_rdpmc[] = 
{
    {
        OP_CODEONLY, 0x0033, 0x0905
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_rdtsc[] = 
{
    {
        OP_CODEONLY, 0x0031, 0x0905
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_repnz[] = 
{
    {
        OP_CODEONLY, 0x00f2, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_repz[] = 
{
    {
        OP_CODEONLY, 0x00f3, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_ret[] = 
{
    {
        OP_CODEONLY, 0x00c3, 0x0000
    }
    , 
    {
        OP_RET, 0x00c2, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_retf[] = 
{
    {
        OP_CODEONLY, 0x00cb, 0x0000
    }
    , 
    {
        OP_RET, 0x00ca, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_rol[] = 
{
    {
        OP_RMSHIFT, 0x00c0, 0x0202
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_ror[] = 
{
    {
        OP_RMSHIFT, 0x08c0, 0x0202
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_rsm[] = 
{
    {
        OP_CODEONLY, 0x00aa, 0x0803
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_sahf[] = 
{
    {
        OP_CODEONLY, 0x009e, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_sal[] = 
{
    {
        OP_RMSHIFT, 0x20c0, 0x0202
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_sar[] = 
{
    {
        OP_RMSHIFT, 0x38c0, 0x0202
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_sbb[] = 
{
    {
        OP_RMIMMSIGNED, 0x1880, 0x0200
    }
    , 
    {
        OP_ACCIMM, 0x001c, 0x0000
    }
    , 
    {
        OP_REGRMREG, 0x0018, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_scas[] = 
{
    {
        OP_SCAS, 0x00af, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_scasb[] = 
{
    {
        OP_CODEONLY, 0x00ae, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_scasd[] = 
{
    {
        OP_INSDWORDSIZE, 0x00af, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_scasw[] = 
{
    {
        OP_INSWORDSIZE, 0x00af, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_seta[] = 
{
    {
        OP_NOSTRICTRM, 0x0097, 0x0a03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setae[] = 
{
    {
        OP_NOSTRICTRM, 0x0093, 0x0a03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setb[] = 
{
    {
        OP_NOSTRICTRM, 0x0092, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setbe[] = 
{
    {
        OP_NOSTRICTRM, 0x0096, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setc[] = 
{
    {
        OP_NOSTRICTRM, 0x0092, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_sete[] = 
{
    {
        OP_NOSTRICTRM, 0x0094, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setg[] = 
{
    {
        OP_NOSTRICTRM, 0x009f, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setge[] = 
{
    {
        OP_NOSTRICTRM, 0x009d, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setl[] = 
{
    {
        OP_NOSTRICTRM, 0x009c, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setle[] = 
{
    {
        OP_NOSTRICTRM, 0x009e, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setna[] = 
{
    {
        OP_NOSTRICTRM, 0x0096, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setnae[] = 
{
    {
        OP_NOSTRICTRM, 0x0092, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setnb[] = 
{
    {
        OP_NOSTRICTRM, 0x0093, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setnbe[] = 
{
    {
        OP_NOSTRICTRM, 0x0097, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setnc[] = 
{
    {
        OP_NOSTRICTRM, 0x0093, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setne[] = 
{
    {
        OP_NOSTRICTRM, 0x0095, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setng[] = 
{
    {
        OP_NOSTRICTRM, 0x009e, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setnge[] = 
{
    {
        OP_NOSTRICTRM, 0x009c, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setnl[] = 
{
    {
        OP_NOSTRICTRM, 0x009d, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setnle[] = 
{
    {
        OP_NOSTRICTRM, 0x009f, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setno[] = 
{
    {
        OP_NOSTRICTRM, 0x0091, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setnp[] = 
{
    {
        OP_NOSTRICTRM, 0x009b, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setns[] = 
{
    {
        OP_NOSTRICTRM, 0x0099, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setnz[] = 
{
    {
        OP_NOSTRICTRM, 0x0095, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_seto[] = 
{
    {
        OP_NOSTRICTRM, 0x0090, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setp[] = 
{
    {
        OP_NOSTRICTRM, 0x009a, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setpe[] = 
{
    {
        OP_NOSTRICTRM, 0x009a, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setpo[] = 
{
    {
        OP_NOSTRICTRM, 0x009b, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_sets[] = 
{
    {
        OP_NOSTRICTRM, 0x0098, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_setz[] = 
{
    {
        OP_NOSTRICTRM, 0x0094, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_sfence[] =
{
    {
        OP_CODEONLY, 0xf8ae, 0x0a06
    }
    ,
    {
        0, 0, 0
    }
    , 
} ;
OPCODE popn_sgdt[] = 
{
    {
        OP_NOSTRICTRM, 0x0001, 0x0A03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_shl[] = 
{
    {
        OP_RMSHIFT, 0x20c0, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_shld[] = 
{
    {
        OP_REGRMSHIFT, 0x00a4, 0x0a03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_shr[] = 
{
    {
        OP_RMSHIFT, 0x28c0, 0x0202
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_shrd[] = 
{
    {
        OP_REGRMSHIFT, 0x00ac, 0x0a03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_sidt[] = 
{
    {
        OP_NOSTRICTRM, 0x0801, 0x0a02
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_sldt[] = 
{
    {
        OP_NOSTRICTRM, 0x0000, 0x0a02
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_smsw[] = 
{
    {
        OP_NOSTRICTRM, 0x2001, 0x0a02
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_stc[] = 
{
    {
        OP_CODEONLY, 0x00f9, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_std[] = 
{
    {
        OP_CODEONLY, 0x00fd, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_sti[] = 
{
    {
        OP_CODEONLY, 0x00fb, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_stos[] = 
{
    {
        OP_STOS, 0x00ab, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_stosb[] = 
{
    {
        OP_CODEONLY, 0x00aa, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_stosd[] = 
{
    {
        OP_INSDWORDSIZE, 0x00ab, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_stosw[] = 
{
    {
        OP_INSWORDSIZE, 0x00ab, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_str[] = 
{
    {
        OP_NOSTRICTRM, 0x0800, 0x0a03
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_sub[] = 
{
    {
        OP_RMIMMSIGNED, 0x2880, 0x0200
    }
    , 
    {
        OP_ACCIMM, 0x002c, 0x0000
    }
    , 
    {
        OP_REGRMREG, 0x0028, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_test[] = 
{
    {
        OP_ACCIMM, 0x00a8, 0x0000
    }
    , 
    {
        OP_REGMOD, 0x0084, 0x0200
    }
    , 
    {
        OP_RMIMM, 0x00f6, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_ud2[] = 
{
    {
        OP_CODEONLY, 0x000b, 0x0806
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_verr[] = 
{
    {
        OP_NOSTRICTRM, 0x2000, 0x0a02
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_verw[] = 
{
    {
        OP_NOSTRICTRM, 0x2800, 0x0a02
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_wait[] = 
{
    {
        OP_CODEONLY, 0x009b, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_wbinvd[] = 
{
    {
        OP_CODEONLY, 0x0009, 0x0904
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_wrmsr[] = 
{
    {
        OP_CODEONLY, 0x0030, 0x0905
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_xadd[] = 
{
    {
        OP_BYTERMREG, 0x00c0, 0x0a04
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_xbts[] = 
{
    {
        OP_CODEONLY, 0x00a6, 0x0803
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_xchg[] = 
{
    {
        OP_ACCREG02, 0x0090, 0x0000
    }
    , 
    {
        OP_REGRM, 0x0086, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_xlat[] = 
{
    {
        OP_CODEONLY, 0x00d7, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_xlatb[] = 
{
    {
        OP_CODEONLY, 0x00d7, 0x0000
    }
    , 
    {
        0, 0, 0
    }
    , 
};
OPCODE popn_xor[] = 
{
    {
        OP_RMIMMSIGNED, 0x3080, 0x0200
    }
    , 
    {
        OP_ACCIMM, 0x0034, 0x0000
    }
    , 
    {
        OP_REGRMREG, 0x0030, 0x0200
    }
    , 
    {
        0, 0, 0
    }
    , 
};
void adjustUsesESP();
void outcode_file_init(void)
{
    int i;
    adjustUsesESP(1);
    lastIncludeNum = 0;
    labelbuf = beGlobalAlloc(10000 *sizeof(LABEL*));

    for (i = 0; i < MAX_SEGS; i++)
        memset(&segs[i], 0, sizeof(segs[i]));
    virtualFirst = virtualLast = 0;
    virtualSegmentNumber = MAX_SEGS;
}

/*-------------------------------------------------------------------------*/

void outcode_func_init(void)
{
    int i;
    /*   memset(labelbuf,0,400 * sizeof(LABEL *)) ;*/
#define NOP 0x90

/*    
    if (cparams.prm_optimize_for_speed)
    {
        while (segs[codeseg].curlast % 16)
        {
            oa_genint(chargen, NOP);
        }
    }
    */
    for (i = 0; i < MAX_SEGS; i++)
        segs[i].curbase = segs[i].curlast;

    if (beGetCurrentFunc)
        if (oa_currentSeg == virtseg)
            beGetCurrentFunc->offset = outcode_base_address = 0;
        else
            beGetCurrentFunc->offset = outcode_base_address =
                segs[codeseg].curbase;
    else
        outcode_base_address = 0;
}

void compile_start(char *name)
{
    LIST *new = beGlobalAlloc(sizeof(LIST));
    new->data = beGlobalAlloc(strlen(name) + 1);
    strcpy(new->data, name);

    includedFiles = new ;
    oa_ini();
    o_peepini();
    omfInit();
}
void include_start(char *name, int num)
{
    if (num > lastIncludeNum)
    {
        LIST *new = beGlobalAlloc(sizeof(LIST));
        new->data = beGlobalAlloc(strlen(name) + 1);
        strcpy(new->data, name);

        if (!includedFiles)
            includedFiles = new ;
        else
        {
            LIST *l = includedFiles;
            while (l->next)
                l = l->next ;
            l->next = new;
        }
        lastIncludeNum = num;
    }
    

}
/*-------------------------------------------------------------------------*/

void InsertLabel(int lbl, int address, int seg)
{
    int x = lbl / 512;
    if (!labelbuf[x])
    {
        labelbuf[x] = beGlobalAlloc(512 *sizeof(LABEL));
    }
    labelbuf[x][lbl % 512].address = address;
    labelbuf[x][lbl % 512].seg = seg;
}

/*-------------------------------------------------------------------------*/

int LabelAddress(int lbl)
{
    if (labelbuf)
    {
        LABEL *p = labelbuf[lbl / 512];
        if (p)
        {
            /*	  	 printf("addr %x\n",p[lbl%412].address) ;*/
            return p[lbl % 512].address;
        }
    }
    diag("LabelAddress - missing label");
    return 0;
    /*   return labelbuf[lbl/512][lbl%512].address ;*/
}

/*-------------------------------------------------------------------------*/

int LabelSeg(int lbl)
{
    return labelbuf[lbl / 512][lbl % 512].seg;
}
/*-------------------------------------------------------------------------*/
void InsertAttrib(int address, enum e_adtype type, void *data, int start)
{
    EMIT_TAB *t = gettab(oa_currentSeg);
    ATTRIBDATA *ad ;
    ad= (ATTRIBDATA *)Alloc(sizeof(ATTRIBDATA));
    ad->address = address;
    ad->type = type;
    ad->start = start;
    ad->v.sp = data;
    if (t->attriblist)
        t->attribtail = t->attribtail->next = ad;
    else
        t->attriblist = t->attribtail = ad;
}
void InsertLine(int address, LINEDATA *linedata)
{
    InsertAttrib(address, e_ad_linedata, linedata, 0);
}
void InsertVarStart(int address, SYMBOL *sp)
{
    InsertAttrib(address, e_ad_vardata, sp, 0);
}
void InsertFunc(int address, SYMBOL *sp, int start)
{
    InsertAttrib(address, e_ad_funcdata, sp, start);
}
void InsertBlock(int address, int start)
{
    InsertAttrib(address, e_ad_blockdata, NULL, start);
}

/*-------------------------------------------------------------------------*/

EMIT_TAB *gettab(int seg)
{
    if (seg == virtseg)
        return virtualLast->seg;
    else
    {
        EMIT_TAB *rv = &segs[seg];
        if (!rv->first)
        {
            rv->first = rv->last = beGlobalAlloc(sizeof(EMIT_LIST));
        }
        return rv;
    }
}

/*-------------------------------------------------------------------------*/

void emit(int seg, void *data, int len)
{
    EMIT_TAB *tab = gettab(seg);
    int ofs = 0;
    if (!tab->first)
    {
        tab->first = tab->last = beGlobalAlloc(sizeof(EMIT_LIST));
    }
    tab->last->lastfilled = tab->last->filled;
    tab->curlast += len;
    while (len)
    {
        int size = len >= 65536 ? 65536 : len;
        if (tab->last->filled + len > 65536)
        {
            int address = tab->last->address + tab->last->filled;
            tab->last = tab->last->next = beGlobalAlloc(sizeof(EMIT_LIST));
            tab->last->address = address;
        }
        memcpy(tab->last->data + tab->last->filled, (UBYTE *)data + ofs, size);
        tab->last->filled += size;
        len -= size;
        ofs += size;
    }
}

/*-------------------------------------------------------------------------*/

void write_to_seg(int seg, int offset, void *value, int len)
{
    EMIT_TAB *tab = gettab(seg);
    EMIT_LIST *lst = tab->first;
    while (lst->address + lst->filled < offset)
        lst = lst->next;
    memcpy(lst->data + offset - lst->address, value, len);
}

/*-------------------------------------------------------------------------*/

void gen_symbol_fixup(enum mode xmode, int seg, int address, SYMBOL *pub)
{
    FIXUP *fixup;
    EMIT_TAB *tab = gettab(seg);
//	printf("%p:%s\n",pub, pub->decoratedName);
    fixup = beGlobalAlloc(sizeof(FIXUP));
    fixup->fmode = xmode;
    fixup->address = address;
    fixup->sym = pub;
    if (tab->last->fixups)
        tab->last->lastfixup = tab->last->lastfixup->next = fixup;
    else
        tab->last->fixups = tab->last->lastfixup = fixup;

} 
void gen_label_fixup(enum mode xmode, int seg, int address, int lab)
{
    FIXUP *fixup;
    EMIT_TAB *tab = gettab(seg);
    fixup = beGlobalAlloc(sizeof(FIXUP));
    fixup->fmode = xmode;
    fixup->address = address;
    fixup->label = lab;
    if (tab->last->fixups)
        tab->last->lastfixup = tab->last->lastfixup->next = fixup;
    else
        tab->last->fixups = tab->last->lastfixup = fixup;

} 
void gen_threadlocal_fixup(int seg, int address, SYMBOL *tls, SYMBOL *base)
{
    FIXUP *fixup;
    EMIT_TAB *tab = gettab(seg);
    fixup = beGlobalAlloc(sizeof(FIXUP));
    fixup->fmode = fm_threadlocal;
    fixup->address = address;
    fixup->sym = tls;
    fixup->base = base;
    if (tab->last->fixups)
        tab->last->lastfixup = tab->last->lastfixup->next = fixup;
    else
        tab->last->fixups = tab->last->lastfixup = fixup;

} 
void outcode_dump_muldivval(void)
{
    MULDIV *v = muldivlink;
    UBYTE buf[10];
    while (v)
    {
        oa_align(8);
        InsertLabel(v->label, segs[oa_currentSeg].curlast, oa_currentSeg);
        if (v->size == 0)
        {
            *(int*)buf = v->value;
            emit(oa_currentSeg, buf, 4);
        }
        else if (v->size == ISZ_FLOAT || v->size == ISZ_IFLOAT)
        {
            FPFToFloat(buf, &v->floatvalue);
            emit(oa_currentSeg, buf, 4);
        }
        else if (v->size == ISZ_DOUBLE || v->size == ISZ_IDOUBLE)
        {
            FPFToDouble(buf, &v->floatvalue);
            emit(oa_currentSeg, buf, 8);
        }
        else
        {
            FPFToLongDouble(buf, &v->floatvalue);
            emit(oa_currentSeg, buf, 10);
        }
        v = v->next;
    }
}


/*-------------------------------------------------------------------------*/

void outcode_genref(SYMBOL *sp, int offset)
{
    EMIT_TAB *seg = gettab(oa_currentSeg);
    emit(oa_currentSeg, &offset, 4);

    if (sp->storage_class == sc_localstatic)
    {
        gen_label_fixup(fm_label, oa_currentSeg, seg->last->address + seg->last
            ->lastfilled, sp->label);
    }
    else
    {
        gen_symbol_fixup(fm_symbol, oa_currentSeg, seg->last->address + seg->last
            ->lastfilled, sp);
    }
}

/*-------------------------------------------------------------------------*/

void outcode_gen_labref(int n)
{
    int i = 0;
    EMIT_TAB *seg = gettab(oa_currentSeg);
    emit(oa_currentSeg, &i, 4);
    gen_label_fixup(fm_label, oa_currentSeg, seg->last->address + seg->last
        ->lastfilled, n);
}

/* the labels will already be resolved well enough by this point */
void outcode_gen_labdifref(int n1, int n2)
{
    int i = LabelAddress(n1) - LabelAddress(n2);
    emit(oa_currentSeg, &i, 4);
}

/*-------------------------------------------------------------------------*/

void outcode_gensrref(SYMBOL *sp, int val)
{
    char buf[8];
    EMIT_TAB *seg = gettab(oa_currentSeg);
    memset(buf, 0, 8);
    buf[1] = val;
    emit(oa_currentSeg, buf, 2);
    emit(oa_currentSeg, buf+2, 4);
    gen_symbol_fixup(fm_symbol, oa_currentSeg, seg->last->address + seg->last
        ->lastfilled, sp);
}

/*-------------------------------------------------------------------------*/

void outcode_genstorage(int len)
{
    char buf[256];
    memset(buf, 0, 256);
    while (len >= 256)
    {
        emit(oa_currentSeg, buf, 256);
        len -= 256;
    }
    if (len)
        emit(oa_currentSeg, buf, len);
}

/*-------------------------------------------------------------------------*/

void outcode_genfloat(FPF *val)
{
    UBYTE buf[4];
    FPFToFloat(buf, val);
    emit(oa_currentSeg, buf, 4);
}

/*-------------------------------------------------------------------------*/

void outcode_gendouble(FPF *val)
{
    UBYTE buf[8];
    FPFToDouble(buf, val);
    emit(oa_currentSeg, buf, 8);
}

/*-------------------------------------------------------------------------*/

void outcode_genlongdouble(FPF *val)
{
    UBYTE buf[10];
    FPFToLongDouble(buf, val);
    emit(oa_currentSeg, buf, 10);
}

/*-------------------------------------------------------------------------*/

void outcode_genstring(LCHAR *string, int len)
{
    int i;
    for (i=0; i < len; i++)
        outcode_genbyte(string[i]);
}

/*-------------------------------------------------------------------------*/

void outcode_genbyte(int val)
{
    char v = (char)val;
    emit(oa_currentSeg, &v, 1);
}

/*-------------------------------------------------------------------------*/

void outcode_genword(int val)
{
    short v = (short)val;
    emit(oa_currentSeg, &v, 2);
}

/*-------------------------------------------------------------------------*/

void outcode_genlong(int val)
{
    emit(oa_currentSeg, &val, 4);
}

/*-------------------------------------------------------------------------*/

void outcode_genlonglong(LLONG_TYPE val)
{
    emit(oa_currentSeg, &val, 4);
    #ifdef BCC32
        val = val < 0 ?  - 1: 0;
    #else 
        val = val >> 32;
    #endif 
    emit(oa_currentSeg, &val, 4);
}

/*-------------------------------------------------------------------------*/

void outcode_align(int size)
{
    EMIT_TAB *seg = gettab(oa_currentSeg);
    if (seg->last)
    {
        int adr = seg->last->address + seg->last->filled;
        adr = size - adr % size;
        if (size != adr)
            outcode_genstorage(adr);
    }
}

/*-------------------------------------------------------------------------*/

void outcode_put_label(int lab)
{
    EMIT_TAB *seg = gettab(oa_currentSeg);
    InsertLabel(lab, seg->last->address + seg->last->filled, oa_currentSeg);
}

/*-------------------------------------------------------------------------*/

void outcode_start_virtual_seg(SYMBOL *sp, int data)
{
    VIRTUAL_LIST *x;
    x = beGlobalAlloc(sizeof(VIRTUAL_LIST));
    x->sp = sp;
    x->seg = beGlobalAlloc(sizeof(EMIT_TAB));
    x->data = data;
    sp->value.i = virtualSegmentNumber++;
    if (virtualFirst)
        virtualLast = virtualLast->next = x;
    else
        virtualFirst = virtualLast = x;
}

/*-------------------------------------------------------------------------*/

void outcode_end_virtual_seg(SYMBOL *sp)
{
    (void)sp;
}


/*-------------------------------------------------------------------------*/

int resolveoffset(OCODE *ins, EXPRESSION *n, int *resolved)
{
    int rv = 0;
    if (n)
    {
        switch (n->type)
        {
            case en_sub:
                rv += resolveoffset(ins, n->left, resolved);
                rv -= resolveoffset(ins, n->right, resolved);
                break;
            case en_add:
            //case en_addstruc:
                rv += resolveoffset(ins, n->left, resolved);
                rv += resolveoffset(ins, n->right, resolved);
                break;
            case en_c_ll:
            case en_c_ull:
            case en_c_i:
            case en_c_c:
            case en_c_uc:
            case en_c_u16:
            case en_c_u32:
            case en_c_l:
            case en_c_ul:
            case en_c_ui:
            case en_c_wc:
            case en_c_s:
            case en_c_us:
            case en_absolute:
            case en_c_bool:
                rv += n->v.i;
                break;
            case en_auto:
                rv += n->v.sp->offset;
                break;
            case en_labcon:
            case en_global:
            case en_label:
            case en_pc:
            case en_threadlocal:
                *resolved = 0;
                break;
            default:
                diag("Unexpected node type in resolveoffset");
                break;
        }
    }
    return rv;
}

/*-------------------------------------------------------------------------*/

int asmrm(int reg, OCODE *ins, AMODE *data, UBYTE **p)
{
    int resolved = 1, val;
    UBYTE *rm;
    if (data->mode == am_dreg)
    {
        reg |= 0xc0 + data->preg;
        *(*p)++ = reg;
        return 1;
    }
    rm =  *p;
    if (data == ins->oper2)
    {
        ins->resobyte = 1;
    }
    switch (data->mode)
    {
        case am_indisp:
            indisp: if (data->preg == EBP)
            {
                *(*p)++ = 5 | reg;
                ins->addroffset =  *p - ins->outbuf;
                val = resolveoffset(ins, data->offset, &resolved);
                ins->resolved = resolved;
            }
            else if (data->preg == ESP)
            {
                *(*p)++ = 4+reg;
                *(*p)++ = 0x24;
                ins->addroffset =  *p - ins->outbuf;
                val = resolveoffset(ins, data->offset, &resolved);
                ins->resolved = resolved;
                #ifdef FULLVERSION
                    if (resolved &&!val)
                        return 1;
                #endif 
            }
            else
            {
                *(*p)++ = reg + data->preg;
                ins->addroffset =  *p - ins->outbuf;
                val = resolveoffset(ins, data->offset, &resolved);
                ins->resolved = resolved;
                #ifdef FULLVERSION
                    if (resolved && !val)
                        return 1;
                #endif 
            }
            break;
        case am_indispscale:
            if (data->sreg ==  - 1)
                goto indisp;
            if (data->preg ==  - 1)
            {
                *(*p)++ = 0x04 + reg;
                *(*p)++ = 5+(data->sreg << 3) + (data->scale << 6);
                ins->addroffset =  *p - ins->outbuf;
                *(int*)(*p) = resolveoffset(ins, data->offset, &resolved);
                (*p) += sizeof(int);
                ins->resolved = resolved;
                return 1;
            }
            else
            {
                if (data->sreg == ESP || (data->sreg == data->preg && data->sreg
                    == EBP))
                    return 0;
                if (data->preg != EBP)
                {
                    *(*p)++ = 4+reg;
                    *(*p)++ = data->preg + (data->sreg << 3) + (data->scale <<
                        6);
                    ins->addroffset =  *p - ins->outbuf;
                    val = resolveoffset(ins, data->offset, &resolved);
                    #ifdef FULLVERSION
                        if (!val && resolved)
                            return 1;
                    #endif 
                }
                else
                {
                    *(*p)++ = 4+reg;
                    *(*p)++ = 5+(data->sreg << 3) + (data->scale << 6);
                    ins->addroffset =  *p - ins->outbuf;
                    val = resolveoffset(ins, data->offset, &resolved);
                    ins->resolved = resolved;
                }
            }
            break;
        case am_direct:
            *(*p)++ = 5 | reg;
            ins->addroffset =  *p - ins->outbuf;
            *(int*)(*p) = resolveoffset(ins, data->offset, &resolved);
            (*p) += sizeof(int);
            ins->resolved = resolved;
            return 1;

        default:
            return 0;
    }
    if (resolved)
    {
        #ifdef FULLVERSION
            if (val >=  - 128 && val < 128)
            {

                *rm |= 0x40;
                *(*p)++ = val;
            }
            else
        #endif 
        {
            *rm |= 0x80;
            *(int*)(*p) = val;
            (*p) += sizeof(int);
        }
    }
    else
    {
        *rm |= 0x80;
        *(int*)(*p) = val;
        (*p) += sizeof(int);
    }
    ins->resolved = resolved;
    return 1;
}

/*-------------------------------------------------------------------------*/

int asmfrm(int reg, OCODE *ins, AMODE *data, UBYTE **p)
{
    reg &= 0x38;
    if (data->mode == am_freg)
    {
        *(*p)++ = 0xc0 + reg + data->preg;
        return 1;
    }
    return asmrm(reg, ins, data, p);
}

/*
 * no operands. In this case, the opcode length is 1, so we go through this
 *	weird locution to rep mov a single byte out of the structure and
 *	into [di], wherever that points. Apparently it points into an opcode
 *	buffer somewhere. If the opcode length is greater than one, what
 *	happens?
 */
int AOP0(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper1)
        return 0;
    if (descript->ocflags &OCtwobyte)
    {
        *(short*)(*p) = descript->ocvalue;
        (*p) += sizeof(short);
    }
    else
        *(*p)++ = descript->ocvalue;
    return 1;
}

/* 
 *  word reg, bits 0-2 of opcode = reg num
 */
int AOP1(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper2)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->length == ISZ_UCHAR || data->oper1->length ==  - ISZ_UCHAR || data->oper1->length == ISZ_BOOLEAN)
        return 0;
    if (data->oper1->mode != am_dreg)
        return 0;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    *(*p)++ = descript->ocvalue | data->oper1->preg;
    return 1;

}

/* 
 *  word acc,reg... reg = bits 0-2 of opcode
 */
int AOP2(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper3 || !data->oper2)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->mode != am_dreg || data->oper2->mode != am_dreg)
        return 0;
    if (data->oper1->length == ISZ_UCHAR || data->oper1->length ==  - ISZ_UCHAR || data->oper1->length == ISZ_BOOLEAN)
        return 0;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    if (data->oper1->preg == 0)
        *(*p)++ = descript->ocvalue | data->oper2->preg;
    else if (data->oper2->preg == 0)
        *(*p)++ = descript->ocvalue | data->oper1->preg;
    else
        return 0;
    return 1;
}

/* 
 *  one arg, seg goes in b3-4 of opcode
 */
int AOP3(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper2)
        return 0;
    if (data->oper1->mode != am_seg)
        return 0;
    if (!(descript->ocflags &OCprefix0F))
    {
        if (data->oper1->seg == 4 || data->oper1->seg == 5)
            return 0;
        *(*p)++ = descript->ocvalue | (segtab[data->oper1->seg]);
    }
    else
    {
        if (data->oper1->seg == 4 || data->oper1->seg == 5)
            *(*p)++ = descript->ocvalue | ((data->oper1->seg - 4) << 3);
        else
            return 0;
    }
    return 1;
}

/* 
 *  either combo of a reg & rm... bit 1 of opcode set if reg is dest
 *  bit 0 set if size = word
 */
int AOP4(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int temp = 0, reg;
    AMODE *rm;
    if (data->oper3 || !data->oper2)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->length != ISZ_UCHAR && data->oper1->length !=  - ISZ_UCHAR && data->oper1->length != ISZ_BOOLEAN)
        temp |= 1;
    if (data->oper2->mode == am_dreg)
    {
        rm = data->oper1;
        reg = data->oper2->preg;
    }
    else if (data->oper1->mode == am_dreg)
    {
        temp |= 2;
        rm = data->oper2;
        reg = data->oper1->preg;
    }
    else
        return 0;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    *(*p)++ = temp | (descript->ocvalue);
    return asmrm(reg << 3, data, rm, p);
}

/* 
 *  use only rm, bit 0 = size (exception : jmp/call)
 */
int AOP5(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val = descript->ocvalue;
    if (data->oper2)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->length != ISZ_UCHAR && data->oper1->length !=  - ISZ_UCHAR && data->oper1->length != ISZ_BOOLEAN)
        val |= 1;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    *(*p)++ = val;
    return asmrm((val >> 8) &0x38, data, data->oper1, p);
}

/* 
 *  rm,count or rm,cl (shifts) bit 0 = size
 *  bit 1 set if size = 1, bit 4 set if size = cl, otherwise follow rm with
 *  a count byte
 */
int AOP6(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper3)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    val = (descript->ocvalue &~1);
    if (data->oper1->length != ISZ_UCHAR && data->oper1->length !=  - ISZ_UCHAR && data->oper1->length != ISZ_BOOLEAN)
        val |= 1;
    if (data->oper2->mode == am_immed)
    {
        if (data->oper2->offset->v.i == 1)
        {
            val |= 0x10;
        }
        else
        {
            if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
                *(*p)++ = 0x66;
            *(*p)++ = val;
            if (!asmrm(val >> 8, data, data->oper1, p))
                return 0;
            *(*p)++ = data->oper2->offset->v.i;
            return 1;
        }
    }
    else if (data->oper2->mode == am_dreg && (data->oper2->length == ISZ_UCHAR || data
        ->oper2->length ==  - ISZ_UCHAR || data->oper2->length == ISZ_BOOLEAN)
              && data->oper2->preg == ECX)
    {
        val |= 0x12;
    }
    else
        return 0;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    *(*p)++ = val;
    return asmrm(val >> 8, data, data->oper1, p);
}

/* 
 *  unordered version of AOP4.  (any combo of reg and RM)
 *  This is XCHG instructions,
 *  this is unordered, so, it doesn't
 *  need to know which comes first
 * 
 *  bit 0 = size
 */
int AOP7(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    AMODE *rm;
    int reg, len;
    if (data->oper3)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    val = descript->ocvalue;
    if (data->oper1->mode == am_dreg)
    {
        reg = data->oper1->preg;
        rm = data->oper2;
        len = data->oper1->length;
    }
    else if (data->oper2->mode == am_dreg)
    {
        reg = data->oper2->preg;
        rm = data->oper1;
        len = data->oper2->length;
    }
    else
        return 0;
    if (len != ISZ_UCHAR && len !=  - ISZ_UCHAR && len != ISZ_BOOLEAN)
        val |= 1;

    if (len == ISZ_USHORT || len ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    *(*p)++ = val;
    return asmrm(reg << 3, data, rm, p);
}

/* 
 *  word regrm, reg = dest.
 */
int AOP8(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper3 || !data->oper2)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->length == ISZ_UCHAR || data->oper1->length ==  - ISZ_UCHAR || data->oper1->length == ISZ_BOOLEAN)
        return 0;
    if (data->oper1->mode != am_dreg)
        return 0;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    *(*p)++ = descript->ocvalue;
    return asmrm(data->oper1->preg << 3, data, data->oper2, p);

}

/* 
 *  interrupts (imm byte)
 */
int AOP9(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper2)
        return 0;
    if (data->oper1->mode != am_immed)
        return 0;
    if (data->oper1->offset->v.i == 3)
    {
        *(*p)++ = 0xcc;
    }
    else
    {
        *(*p)++ = descript->ocvalue | 1;
        *(*p)++ = data->oper1->offset->v.i;
    }
    return 1;
}

/* 
 *  short relative branches
 */
int AOP10(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper2)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->mode != am_immed || data->oper1->offset->type !=
        en_labcon)
        return 0;
    *(*p)++ = descript->ocvalue;
    data->addroffset =  *p - data->outbuf;
    *(*p)++ = 0;
    data->resolved = 0;
    data->branched |= BR_SHORT;
    return 1;
}

/*                                          
 *  RM, IMMEDIATE
 *  bit 0 = size
 */
int AOP11(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val, word = FALSE, resolved = 1;
    if (data->oper3)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper2->mode != am_immed)
        return 0;
    val = descript->ocvalue;
    if (data->oper1->length != ISZ_UCHAR && data->oper1->length !=  - ISZ_UCHAR && data->oper1->length != ISZ_BOOLEAN)
        val |= 1;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
    {
        *(*p)++ = 0x66;
        word = TRUE;
    }
    *(*p)++ = val;
    if (!asmrm(val >> 8, data, data->oper1, p))
        return 0;
    if (val &1)
    {
        val = resolveoffset(data, data->oper2->offset, &resolved);
        if (!data->resolved && !resolved)
            data->resobyte = 3;
        else if (!resolved)
            data->resobyte = 1;
        data->resolved &= resolved;
        if (data->resobyte == 1)
            data->addroffset =  *p - data->outbuf;
        if (word)
        {
            *(short*)(*p) = val;
            (*p) += sizeof(short);
        }
        else
        {
            *(int*)(*p) = val;
            (*p) += sizeof(int);
        }
    }
    else
        *(*p)++ = data->oper2->offset->v.i;
    return 1;
}

/* 
 *  ACC,immediate
 *  bit 0 = size
 */
int AOP12(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val, word = FALSE, resolved = 1;
    if (data->oper3)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->mode != am_dreg || data->oper1->preg != EAX)
        return 0;
    if (data->oper2->mode != am_immed)
        return 0;
    val = descript->ocvalue;
    if (data->oper1->length !=  - ISZ_UCHAR && data->oper1->length != ISZ_UCHAR && data->oper1->length != ISZ_BOOLEAN)
        val |= 1;

    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
    {
        *(*p)++ = 0x66;
        word = TRUE;
    }
    *(*p)++ = val;
    if (val &1)
        if (word)
        {
            *(short*)(*p) = data->oper2->offset->v.i;
            (*p) += sizeof(short);
        }
        else
        {
            val = resolveoffset(data, data->oper2->offset, &resolved);
            data->resobyte = 1;
            data->resolved = resolved;
            data->addroffset =  *p - data->outbuf;
            *(int*)(*p) = val;
            (*p) += sizeof(int);
        }
    else
        *(*p)++ = data->oper2->offset->v.i;
    return 1;
}

/* 
 *  mem,acc
 *  bit 0 = size
 */
int AOP13(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val, resolved = 1;
    if (data->oper3)
        return 0;
    if (data->oper2->length > ISZ_ADDR || data->oper2->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper2->mode != am_dreg || data->oper2->preg != EAX)
        return 0;
    if (data->oper1->mode != am_direct)
        return 0;
    val = descript->ocvalue;
    if (data->oper1->length != ISZ_UCHAR && data->oper1->length !=  - ISZ_UCHAR && data->oper1->length != ISZ_BOOLEAN)
        val |= 1;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    *(*p)++ = val;
    val = resolveoffset(data, data->oper1->offset, &resolved);
    data->resolved = resolved;
    data->addroffset =  *p - data->outbuf;
    *(int*)(*p) = val;
    (*p) += sizeof(int);
    return 1;

}

/* 
 *  sign-extended RM/IMM
 *  b1 = 0, treat as normal RM/IMM (aop11)
 *  else b01=11 means sign-extend byte to word
 */
int AOP14(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper3 || !data->oper2)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper2->mode != am_immed)
        return 0;
    if (!isintconst(data->oper2->offset))
        return AOP11(descript, data, p);
    /* want to use AOP12 for embedding some values but we can't call it directly */
    if (data->oper1->mode == am_dreg && data->oper1->preg == EAX && data->oper2
        ->mode == am_immed && (data->oper1->length == ISZ_UCHAR || data->oper1->length 
        ==  - 1  || data->oper1->length == ISZ_BOOLEAN
        || data->oper2->length == ISZ_USHORT || data->oper2->length ==  - ISZ_USHORT ||
        data->oper2->length == ISZ_U16 ||
         data->oper2->offset->v.i >= 128 || data->oper2->offset->v.i <  - 128))
        return 0;
    if (data->oper1->length == ISZ_UCHAR || data->oper1->length ==  - ISZ_UCHAR || data->oper1->length == ISZ_BOOLEAN)
        return AOP11(descript, data, p);
    if (data->oper2->offset->v.i <  - 128 || data->oper2->offset->v.i > 127)
        return AOP11(descript, data, p);
    data->oper2->length = ISZ_UCHAR;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    val = descript->ocvalue;
    *(*p)++ = val | 3;
    if (!asmrm(val >> 8, data, data->oper1, p))
        return 0;
    *(*p)++ = data->oper2->offset->v.i;
    return 1;
}

/* 
 *  acc,imm
 *  b3 of opcode = size
 */
int AOP15(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val, word = FALSE;
    if (data->oper3)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->mode != am_dreg || data->oper1->preg != EAX)
        return 0;
    if (data->oper2->mode != am_immed)
        return 0;
    val = descript->ocvalue;
    if (data->oper1->length != ISZ_UCHAR && data->oper1->length !=  - ISZ_UCHAR && data->oper1->length != ISZ_BOOLEAN)
        val |= 8;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
    {
        *(*p)++ = 0x66;
        word = TRUE;
    }
    *(*p)++ = val;
    if (val &8)
        if (word)
        {
            *(short*)(*p) = data->oper2->offset->v.i;
            (*p) += sizeof(short);
        }
        else
        {
            *(int*)(*p) = data->oper2->offset->v.i;
            (*p) += sizeof(int);
        }
        else
            *(*p)++ = data->oper2->offset->v.i;
    return 1;
}

/* 
 *  seg,regrm or regrm,seg
 *  b1 set if seg is dest
 */
int AOP16(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    int reg;
    AMODE *rm;
    if (data->oper3)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    val = descript->ocvalue;
    if (data->oper1->mode == am_seg)
    {
        val |= 2;
        reg = segtab[data->oper1->seg];
        rm = data->oper2;
    }
    else if (data->oper2->mode == am_seg)
    {
        reg = segtab[data->oper2->seg];
        rm = data->oper1;
    }
    else
        return 0;
    *(*p)++ = val;
    return asmrm(reg, data, rm, p);
}

/* 
 *  returns which pop the stack
 */
int AOP17(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper2)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->mode != am_immed)
        return 0;
    *(*p)++ = descript->ocvalue;
    *(short*)(*p) = data->oper1->offset->v.i;
    (*p) += sizeof(short);
    return 1;
}

/* 
 *  far branch or call
 */
int AOP18(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper1 && data->oper2)
    {
        if (data->oper1->mode == am_immed && data->oper2->mode == am_immed)
        {
            *(*p)++ = descript->ocvalue;
            *(int*)(*p) = data->oper2->offset->v.i;
            (*p) += sizeof(int);
            *(short*)(*p) = data->oper1->offset->v.i;
            (*p) += sizeof(short);
            return 1;
        }
    }
    return 0;
}

/* 
 *  ESC instruction
 *  imm,rm... imm is six bits and fills the low three bits of the
 *  opcode and the reg field
 */
int AOP19(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper3)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->mode != am_immed)
        return 0;
    *(*p)++ = descript->ocvalue + ((data->oper1->offset->v.i >> 3) &7);
    return asmrm(((data->oper1->offset->v.i &7) << 3), data, data->oper2, p);
}

/* 
 *  long relative branch
 *   (the parser fills in the 0f starter)
 */
int AOP20(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper2)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->mode != am_immed)
        return 0;
    data->branched = BR_LONG;
    *(*p)++ = descript->ocvalue;
    data->addroffset =  *p - data->outbuf;
    *(int*)(*p) = 0;
    (*p) += sizeof(int);
    data->resolved = 0;
    return 1;
}

/* 
 * 	acc,dx (in instructions)
 * 	bit 0 = size
 */
int AOP21(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper3)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper2->mode != am_dreg || data->oper2->preg != EDX)
        return 0;
    if (data->oper1->mode != am_dreg || data->oper1->preg != EAX)
        return 0;
    val = descript->ocvalue;
    if (data->oper1->length != ISZ_UCHAR && data->oper1->length !=  - ISZ_UCHAR && data->oper1->length != ISZ_BOOLEAN)
        val |= 1;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    *(*p)++ = val;
    return 1;
}

/* 
 *  dx,acc (out_)
 *  bit 0 = size
 */
int AOP22(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper3)
        return 0;
    if (data->oper2->length > ISZ_ADDR || data->oper2->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->mode != am_dreg || data->oper1->preg != EDX)
        return 0;
    if (data->oper2->mode != am_dreg || data->oper2->preg != EAX)
        return 0;
    val = descript->ocvalue;
    if (data->oper2->length != ISZ_UCHAR && data->oper2->length !=  - ISZ_UCHAR && data->oper2->length != ISZ_BOOLEAN)
        val |= 1;
    if (data->oper2->length == ISZ_USHORT || data->oper2->length ==  - ISZ_USHORT || data->oper2->length == ISZ_U16)
        *(*p)++ = 0x66;
    *(*p)++ = val;
    return 1;
}

/* 
 *  port,acc or acc,port
 *  b0 =size, b1 = 1 if port is dest
 */
int AOP23(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val, val1, len;
    if (data->oper3)
        return 0;
    if (data->oper2->length > ISZ_ADDR || data->oper2->length == ISZ_ULONGLONG)
        return 0;
    val = descript->ocvalue;
    if (descript->ocvalue &2)
    {
        if (data->oper2->mode != am_dreg || data->oper2->preg != EAX)
            return 0;
        if (data->oper1->mode != am_immed)
            return 0;
        val1 = data->oper1->offset->v.i;
        len = data->oper2->length;
    }
    else
    {
        if (data->oper1->mode != am_dreg || data->oper1->preg != EAX)
            return 0;
        if (data->oper2->mode != am_immed)
            return 0;
        val1 = data->oper2->offset->v.i;
        len = data->oper1->length;
    }
    if (len != ISZ_UCHAR && len !=  - ISZ_UCHAR && len != ISZ_BOOLEAN)
        val |= 1;
    if (len == ISZ_USHORT || len ==  - ISZ_USHORT || len == ISZ_U16)
        *(*p)++ = 0x66;
    *(*p)++ = val;
    *(*p)++ = val1;
    return 1;
}

/* 
 *  acc,mem
 *  bit 0 = size
 */
int AOP24(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val, resolved = 1;
    if (data->oper3)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->mode != am_dreg || data->oper1->preg != EAX)
        return 0;
    if (data->oper2->mode != am_direct)
        return 0;
    val = descript->ocvalue;
    if (data->oper2->length != ISZ_UCHAR && data->oper2->length !=  - ISZ_UCHAR && data->oper2->length != ISZ_BOOLEAN)
        val |= 1;
    if (data->oper2->length == ISZ_USHORT || data->oper2->length ==  - ISZ_USHORT || data->oper2->length == ISZ_U16)
        *(*p)++ = 0x66;
    *(*p)++ = val;
    val = resolveoffset(data, data->oper2->offset, &resolved);
    data->resobyte = 1;
    data->resolved = resolved;
    data->addroffset =  *p - data->outbuf;
    *(int*)(*p) = val;
    (*p) += sizeof(int);
    
    return 1;
}

/* 
 *  immediate byte or word
 *  this is push imm, bit 1 set for byte
 *  we were about due for a departure from the standard...
 *  anyway the op is sign-extended if it is byte size
 */
int AOP25(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val, resolved = TRUE, xval;
    if (data->oper2)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->mode != am_immed)
        return 0;
    xval = resolveoffset(data, data->oper1->offset, &resolved);
    data->resolved = resolved;
    val = descript->ocvalue;
    /*	if (resolved)*/
    /*		printf("r:%x\n",xval) ;*/
    /*	else*/
    /*		printf("s:%x\n",xval) ;*/
    if (resolved)
    {
        if (xval >=  - 128 && xval < 128 && data->oper1->length != ISZ_USHORT && data->oper1->length != ISZ_U16)
        {
            val |= 2;
            *(*p)++ = val;
            *(*p)++ = xval;
            data->oper1->length = ISZ_UCHAR;
        }
        else
        {
            if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
            {
                *(*p)++ = 0x66;
                *(*p)++ = val;
                *(short*)(*p) = xval;
                (*p) += sizeof(short);
            }
            else
            {
                *(*p)++ = val;
                *(int*)(*p) = xval;
                (*p) += sizeof(int);
            }
        }
    }
    else
    {
        *(*p)++ = val;
        data->addroffset =  *p - data->outbuf;
        *(int*)(*p) = xval;
        (*p) += sizeof(int);
    }
    return 1;
}

/* 
 *  enter command, we have a word then a byte
 */
int AOP26(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper3)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->mode != am_immed || data->oper2->mode != am_immed)
        return 0;
    *(*p)++ = descript->ocvalue;
    *(short*)(*p) = data->oper1->offset->v.i;
    (*p) += sizeof(short);
    *(*p)++ = data->oper2->offset->v.i;
    return 1;
}

/* 
 *  stringu/w/d, pushaw,pushfw, etc
 *  explicit byte sizing handled elsewhere (aop0)
 */
int AOP27(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper1)
        return 0;
    *(*p)++ = 0x66;
    *(*p)++ = descript->ocvalue;
    return 1;
}

/* 
 *  rm,reg (test instruction)
 *  bit 0 = size
 */
int AOP28(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int temp = 0, reg;
    AMODE *rm;
    if (data->oper3)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->length != ISZ_UCHAR && data->oper1->length !=  - ISZ_UCHAR && data->oper1->length != ISZ_BOOLEAN)
        temp |= 1;
    if (data->oper2->mode == am_dreg)
    {
        rm = data->oper1;
        reg = data->oper2->preg;
    }
    else if (data->oper1->mode == am_dreg)
    {
        rm = data->oper2;
        reg = data->oper1->preg;
    }
    else
        return 0;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    *(*p)++ = temp | (descript->ocvalue);
    return asmrm(reg << 3, data, rm, p);
}

/* 
 *  rm, size don't care
 */
int AOP29(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper2)
        return 0;
    val = descript->ocvalue;
    *(*p)++ = val;
    return asmrm(val >> 8, data, data->oper1, p);
}

/* 
 *  RM, shift
 *  bit 0 & 1 of opcode set if uses CL
 *  bit 0 & 4 set if uses uses 1
 *  else nothing set
 */
int AOP30(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper3)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    val = descript->ocvalue &0xffec;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    if (data->oper1->length != ISZ_UCHAR && data->oper1->length !=  - ISZ_UCHAR && data->oper1->length != ISZ_BOOLEAN)
        val |= 1;
    if (data->oper2->mode == am_dreg && data->oper2->preg == ECX && (data->oper2
        ->length == ISZ_UCHAR || data->oper2->length ==  - ISZ_UCHAR || data->oper2->length == ISZ_BOOLEAN))
    {

        *(*p)++ = val;
        return asmrm(val >> 8, data, data->oper1, p);
    }
    else if (data->oper2->mode == am_immed)
    {
        if (data->oper2->offset->v.i == 1)
        {
            *(*p)++ = val | 0x10;
            return asmrm(val >> 8, data, data->oper1, p);
        }
        else
        {
            *(*p)++ = val | 0x2;
            if (!asmrm(val >> 8, data, data->oper1, p))
                return 0;
            *(*p)++ = data->oper2->offset->v.i;
        }
    }
    else
        return 0;
    return 1;
}

/* 
 *  reg,rm,imm or reg,imm (imul)
 *  bit 1 = set if immed = signed byte
 */
int AOP31(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val, xval, byte = FALSE;
    if (data->oper1->mode != am_dreg)
        return 0;
    val = descript->ocvalue;
    if (data->oper3)
    {
        if (data->oper3->mode != am_immed)
            return 0;
        xval = data->oper3->offset->v.i;
        if (xval < 128 && xval >=  - 128)
        {
            byte = TRUE;
            data->oper3->length = ISZ_UCHAR;
            val |= 2;
        }
        if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
            *(*p)++ = 0x66;
        *(*p)++ = val;
        if (!asmrm(data->oper1->preg << 3, data, data->oper2, p))
            return 0;
    }
    else
    {
        if (!data->oper2 || data->oper2->mode != am_immed)
            return 0;
        xval = data->oper2->offset->v.i;
        if (xval < 128 && xval >=  - 128)
        {
            byte = TRUE;
            data->oper2->length = ISZ_UCHAR;
            val |= 2;
        }
        val |= (data->oper1->preg << 11) | (data->oper1->preg << 8) | 0xc000;
        if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
            *(*p)++ = 0x66;
        *(short*) (*p) = val;
        (*p) += sizeof(short);
    }
    if (!byte)
        if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        {
            *(short*)(*p) = xval;
            (*p) += sizeof(short);
        }
        else 
        {
            *(int*)(*p) = xval;
            (*p) += sizeof(int);
        }
        else
            *(*p)++ = xval;
    return 1;

}

/* 
 *  move to/from a special register
 *  bit 1 = set if spc reg is the dest
 *  bit 0 & 2 define the special reg
 */
int AOP32(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val, reg;
    AMODE *rm;
    if (data->oper3)
        return 0;
    if (data->oper1->length != ISZ_UINT && data->oper1->length !=  - ISZ_UINT && data->oper1->length != ISZ_U32)
        return 0;
    val = descript->ocvalue;
    if (data->oper1->mode != am_dreg)
    {
        val |= 2;
        if (data->oper2->mode != am_dreg)
            return 0;
        reg = data->oper2->preg;
        rm = data->oper1;
    }
    else
    {
        if (data->oper1->mode != am_dreg)
            return 0;
        reg = data->oper1->preg;
        rm = data->oper2;
    }
    if (rm->mode == am_sdreg)
        val |= 1;
    else if (rm->mode == am_streg)
        val |= 4;
    else if (rm->mode != am_screg)
        return 0;
    *(*p)++ = val;
    val = (val >> 8) | (rm->preg << 3) | reg;
    *(*p)++ = val;
    return 1;
}

/* 
 *  rm,reg,count (shld/shrd)
 *  bit 0 = set if using CL for count
 * 
 */
int AOP33(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper2->mode != am_dreg)
        return 0;
    if (data->oper2->length == ISZ_UCHAR || data->oper2->length ==  - ISZ_UCHAR || data->oper2->length == ISZ_BOOLEAN)
        return 0;
    val = descript->ocvalue;
    if (data->oper2->length == ISZ_USHORT || data->oper2->length ==  - ISZ_USHORT || data->oper2->length == ISZ_U16)
        *(*p)++ = 0x66;
    if (data->oper3->mode == am_dreg && data->oper3->preg == ECX && (data
        ->oper3->length == ISZ_UCHAR || data->oper3->length ==  - ISZ_UCHAR || data->oper3->length == ISZ_BOOLEAN))
    {
        *(*p)++ = val | 1;
        return asmrm(data->oper2->preg << 3, data, data->oper1, p);
    }
    else if (data->oper3->mode == am_immed)
    {
        *(*p)++ = val;
        if (!asmrm(data->oper2->preg << 3, data, data->oper1, p))
            return 0;
        *(*p)++ = data->oper3->offset->v.i;
    }
    else
        return 0;
    return 1;
}

/* 
 *  push & pop rm
 */
int AOP34(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper2)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->length == ISZ_UCHAR || data->oper1->length ==  - ISZ_UCHAR || data->oper1->length == ISZ_BOOLEAN)
        return 0;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    val = descript->ocvalue;
    *(*p)++ = val;
    return asmrm(val >> 8, data, data->oper1, p);
}

/* 
 *  floating R/M
 *  bit two of opcode set if size is qword
 *  bit 3 of mod/rm set if last ch = 'p'
 */
int AOP35(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper2)
        return 0;
    val = descript->ocvalue;
    if (data->oper1->preg != am_freg && (data->oper1->length == ISZ_DOUBLE || data->oper1->length == ISZ_IDOUBLE))
        val |= 4;
    *(*p)++ = val;
    return asmfrm(val >> 8, data, data->oper1, p);
}

/* 
 *  fmathp
 *  sti),st(0) or nothing
 */
int AOP36(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper3)
        return 0;
    val = 0;
    if (data->oper2)
    {
        if (data->oper1->mode != am_freg || data->oper2->mode != am_freg ||
            data->oper2->preg != 0)
            return 0;
    }
    else
        if (!data->oper1)
            val = 0x100;
    val |= data->oper1->preg << 8;
    *(short*)(*p) = descript->ocvalue | val;
    (*p) += sizeof(short);
    return 1;
}

/* 
 *  fmath
 *  st(i),st(0)  *  st(0), st(i), mem
 *  bit two of opcode set for i dest or qword mem
 *  bit 3 of mod/rm gets flipped if reg & al &6 & bit 5 of mod/rm set
 *  
 */
int AOP37(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    int reg;
    if (data->oper3)
        return 0;
    val = descript->ocvalue &~4;
    if (!data->oper2)
    {
        if (data->oper1->length == ISZ_DOUBLE || data->oper1->length == ISZ_IDOUBLE)
            val |= 4;
        *(*p)++ = val;
        return asmfrm(val >> 8, data, data->oper1, p);
    }
    else if (data->oper1->mode == am_freg && data->oper2->mode == am_freg)
    {
        if (data->oper1->preg == 0)
            reg = data->oper2->preg;
        else if (data->oper2->preg == 0)
        {
            reg = data->oper1->preg;
            val |= 4;
        }
        if ((val &6) && (val &0x2000))
            val ^= 0x800;
        val |= 0xc000 | (reg << 8);
        *(short*)(*p) = val;
        (*p) += sizeof(short);
    }
    else
        return 0;
    return 1;
}

/* 
 *  far RM
 */
int AOP38(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper2)
        return 0;
    if (data->oper1->length != ISZ_FLOAT && data->oper1->length != ISZ_IFLOAT)
        return 0;
    if (data->oper1->mode == am_dreg)
        return 0;
    val = descript->ocvalue;
    *(*p)++ = val;
    return asmrm(val >> 8, data, data->oper1, p);
}

/* 
 *  regrm with reg source
 *  bit 0 = size
 */
int AOP40(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper3)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper2->mode != am_dreg)
        return 0;
    val = descript->ocvalue;
    if (data->oper2->length != ISZ_UCHAR && data->oper2->length !=  - ISZ_UCHAR && data->oper2->length != ISZ_BOOLEAN)
        val |= 1;
    if ((data->oper2->length == ISZ_USHORT || data->oper2->length ==  - ISZ_USHORT || data->oper2->length == ISZ_U16) && !(descript
        ->ocflags &OCAlwaysword))
        *(*p)++ = 0x66;
    *(*p)++ = val;
    return asmrm(data->oper2->preg << 3, data, data->oper1, p);
}

/* 
 *  word regrm with reg  source
 *  bug: lets arpl [bx],eax through
 */
int AOP39(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper2->length == ISZ_UCHAR || data->oper2->length ==  - ISZ_UCHAR || data->oper2->length == ISZ_BOOLEAN)
        return 0;
    return AOP40(descript, data, p);
}

/* 
 *  rm,immediate
 */
int AOP41(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper3)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper2->mode != am_immed)
        return 0;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    *(*p)++ = val = descript->ocvalue;
    if (!asmrm(val >> 8, data, data->oper1, p))
        return 0;
    *(*p)++ = data->oper2->offset->v.i;
    return 1;
}

/* 
 *  regrm with reg dest & forced strictness (MOVZX & MOVSX)
 *  bit 0 of opcode set if size is word
 */
int AOP42(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper3)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->mode != am_dreg)
        return 0;
    if (data->oper1->length == ISZ_UCHAR || data->oper1->length ==  - ISZ_UCHAR || data->oper1->length == ISZ_BOOLEAN)
        return 0;
    val = descript->ocvalue;
    if (data->oper2->length != ISZ_UCHAR && data->oper2->length !=  - ISZ_UCHAR && data->oper2->length != ISZ_BOOLEAN)
        val |= 1;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    *(*p)++ = val;
    return asmrm(data->oper1->preg << 3, data, data->oper2, p);
}

/* 
 *  forced opsize prefix
 */
int AOP43(OPCODE *descript, OCODE *data, UBYTE **p)
{
    *(*p)++ = 0x66;
    return AOP0(descript, data, p);
}

/* 
 *  unused
 */
int AOP44(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper2)
        return 0;
    if (data->oper1->length > ISZ_ADDR || data->oper1->length == ISZ_ULONGLONG)
        return 0;
    if (data->oper1->mode != am_dreg)
        return 0;
    if (data->oper1->length != ISZ_UINT && data->oper1->length !=  - ISZ_UINT && data->oper1->length != ISZ_U32)
        return 0;
    *(*p)++ = data->oper1->preg + descript->ocvalue;
    return 1;
}

/* 
 *  any regrm with reg source
 *  same as aop40
 */
int AOP45(OPCODE *descript, OCODE *data, UBYTE **p)
{
    return AOP40(descript, data, p);
}

/*-------------------------------------------------------------------------*/

int AOP46(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper1->mode == am_dreg || data->oper1->mode == am_freg)
        return 0;
    return AOP29(descript, data, p);
}

/* 
 *  ax reg only
 */
int AOP47(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper2)
        return 0;

    if (data->oper1->mode == am_dreg && data->oper1->preg == EAX)
    {
        if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        {
            *(short*)(*p) = descript->ocvalue;
            (*p) += sizeof(short);
            return 1;
        }
    }
    return 0;
}

/* 
 *  bswap, 32-bit reg to bits 0-3 of opcode
 */
int AOP48(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper2)
        return 0;
    if (data->oper1->mode != am_dreg)
        return 0;
    if (data->oper1->length != ISZ_UINT && data->oper1->length !=  - ISZ_UINT && data->oper1->length != ISZ_U32)
        return 0;
    *(*p)++ = descript->ocvalue | data->oper1->preg;
    return 1;
}

/* 
 *  fld/fstp
 *  freg
 *  dword,qword,tybe
 *  bit 1 of opcode set for tbyte
 *  bit 2 of opcode set for qword or store
 *  bit 5 of modrm set if tbyte
 *  bit 3 of modrm gets cleared if not tbyte && bit 4 set
 */
int AOP50(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper2)
        return 0;
    val = descript->ocvalue &~0xe006;
    if (data->oper1->mode == am_freg)
    {
        if (val &0x1000)
            val |= 4;
        else
            val &= ~0x800;
        val |= 0xc000;
        val |= data->oper1->preg << 8;
        *(short*)(*p) = val;
        (*p) += sizeof(short);
    }
    else if (data->oper1->length < ISZ_FLOAT)
        return 0;
    else
    {
        if (data->oper1->length == ISZ_LDOUBLE || data->oper1->length == ISZ_ILDOUBLE)
            val |= 0x2802;
        else
        {
            if (data->oper1->length == ISZ_DOUBLE || data->oper1->length == ISZ_IDOUBLE)
            {
                val |= 4;
                if (!(val &0x1000))
                    val &= ~0x800;
            }
        }
        *(*p)++ = val;
        return asmfrm(val >> 8, data, data->oper1, p);
    }
    return 1;
}

/* 
 *  fst
 *  same as next but no tbyte
 */
int AOP49(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper1->mode != am_freg && (data->oper1->length == ISZ_LDOUBLE || data->oper1->length == ISZ_ILDOUBLE))
        return 0;
    return AOP50(descript, data, p);
}

/* 
 * 
 *  fbld/fbstp
 *  tbyte ptr mem
 */
int AOP51(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper2)
        return 0;
    if (data->oper1->mode == am_freg)
        return 0;
    if (data->oper1->length != ISZ_LDOUBLE && data->oper1->length != ISZ_ILDOUBLE)
        return 0;
    val = descript->ocvalue;
    *(*p)++ = val;
    return asmfrm(val >> 8, data, data->oper1, p);
}

/* 
 *  fild/fistp
 *  word,dword,qword mem
 *  bit 2 of opcode set if word or qword
 *  bit 3 of modrm gets cleared if not qword & bit 4 is set
 *  bit 5 or modrm set if qword
 */
int AOP52(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper2)
        return 0;
    val = descript->ocvalue &~0x2004;
    if (data->oper1->mode == am_freg)
        return 0;
    if (data->oper1->length == ISZ_LDOUBLE || data->oper1->length == ISZ_ILDOUBLE)
        return 0;
    if (data->oper1->length != ISZ_UINT && data->oper1->length !=  - ISZ_UINT 
        && data->oper1->length != ISZ_U32 && data->oper1->length != ISZ_ULONGLONG 
        && data->oper1->length !=  - ISZ_ULONGLONG)
        val |= 4;
    if (data->oper1->length == ISZ_DOUBLE || data->oper1->length == ISZ_IDOUBLE)
    {
        val |= 0x2000;
        if (!(val &0x1000))
            val |= 0x800;
    }
    *(*p)++ = val;
    return asmfrm(val >> 8, data, data->oper1, p);
}

/* 
 *  fist
 *  same as above but no qword mode
 */
int AOP53(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper1->length == ISZ_DOUBLE || data->oper1->length == ISZ_IDOUBLE)
        return 0;
    return AOP52(descript, data, p);
}

/* 
 *  freg
 *  reg put in mod/rm byte
 */
int AOP54(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper2)
        return 0;
    if (data->oper1->mode != am_freg)
        return 0;
    *(short*)(*p) = descript->ocvalue | (data->oper1->preg << 8);
    (*p) += sizeof(short);
    return 1;
}

/* 
 *  same as above, deault to reg 1 if no args
 */
int AOP55(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (!data->oper1)
    {
        *(short*)(*p) = descript->ocvalue;
        (*p) += sizeof(short);
    }
    else
        return AOP54(descript, data, p);
    return 1;
}

/* 
 *  fimath
 *  word or dword arg
 *  bit two gets set if word
 */
int AOP56(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper2)
        return 0;
    if (data->oper1->length == ISZ_UCHAR || data->oper1->length ==  - ISZ_UCHAR || data->oper1->length == ISZ_BOOLEAN)
        return 0;
    if (data->oper1->mode == am_freg)
        return 0;
    val = descript->ocvalue;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        val |= 4;
    *(*p)++ = val;
    return asmfrm(val >> 8, data, data->oper1, p);
}

/*-------------------------------------------------------------------------*/

int AOP57(OPCODE *descript, OCODE *data, UBYTE **p)
{
    if (data->oper1)
        return 0;
    *(*p)++ = descript->ocvalue;
    return 1;
}

/*
 * cmps	byte ptr [edi],byte ptr [esi]
 */
int AOP58(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper1->mode != am_indisp || data->oper2->mode != am_indisp)
        return 0;
    if (data->oper1->preg != EDI || data->oper2->preg != ESI)
        return 0;
    val = descript->ocvalue;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    if (data->oper1->length != ISZ_UCHAR && data->oper1->length !=  - ISZ_UCHAR && data->oper1->length != ISZ_BOOLEAN)
        val |= 1;
    *(*p)++ = val;
    return 1;
}

/*
 * ins	byte ptr [edi],dx
 */
int AOP59(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper1->mode != am_indisp || data->oper1->preg != ESI)
        return 0;
    if (data->oper2->mode != am_dreg || data->oper2->preg != EDX)
        return 0;
    val = descript->ocvalue;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    if (data->oper1->length != ISZ_UCHAR && data->oper1->length !=  - ISZ_UCHAR && data->oper1->length != ISZ_BOOLEAN)
        val |= 1;
    *(*p)++ = val;
    return 1;
}

/*
 * lods	byte ptr [esi]
 */
int AOP60(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper1->mode != am_indisp || data->oper1->preg != ESI)
        return 0;
    val = descript->ocvalue;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    if (data->oper1->length != ISZ_UCHAR && data->oper1->length !=  - ISZ_UCHAR && data->oper1->length != ISZ_BOOLEAN)
        val |= 1;
    *(*p)++ = val;
    return 1;
}

/*
 * movs	byte ptr [edi],byte ptr [esi]
 */
int AOP61(OPCODE *descript, OCODE *data, UBYTE **p)
{
    return AOP58(descript, data, p);
}

/*
 * outs	dx,byte ptr [esi]
 */
int AOP62(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper2->mode != am_indisp || data->oper2->preg != EDI)
        return 0;
    if (data->oper1->mode != am_dreg || data->oper1->preg != EDX)
        return 0;
    val = descript->ocvalue;
    if (data->oper2->length == ISZ_USHORT  || data->oper2->length ==  - ISZ_USHORT || data->oper2->length == ISZ_U16)
        *(*p)++ = 0x66;
    if (data->oper2->length != ISZ_UCHAR && data->oper2->length !=  - ISZ_UCHAR && data->oper2->length != ISZ_BOOLEAN)
        val |= 1;
    *(*p)++ = val;
    return 1;
}

/*
 * scas	byte ptr [edi]
 */
int AOP63(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    if (data->oper1->mode != am_indisp || data->oper1->preg != EDI)
        return 0;
    val = descript->ocvalue;
    if (data->oper1->length == ISZ_USHORT || data->oper1->length ==  - ISZ_USHORT || data->oper1->length == ISZ_U16)
        *(*p)++ = 0x66;
    if (data->oper1->length != ISZ_UCHAR && data->oper1->length !=  - ISZ_UCHAR && data->oper1->length != ISZ_BOOLEAN)
        val |= 1;
    *(*p)++ = val;
    return 1;
}

/*
 * stos	byte ptr [edi] 
 */
int AOP64(OPCODE *descript, OCODE *data, UBYTE **p)
{
    return AOP63(descript, data, p);
}

/*
 * word reg, lower 3 bits
 */
int AOP65(OPCODE *descript, OCODE *data, UBYTE **p)
{
    int val;
    int resolved = 1;
    (void)descript;
    if (data->oper3)
        return 0;
    if (data->oper2->mode != am_immed)
        return 0;
    if (data->oper1->mode != am_dreg)
        return 0;
    val = resolveoffset(data, data->oper2->offset, &resolved);
    if (resolved && (data->oper1->length == ISZ_UCHAR || data->oper1->length ==  - ISZ_UCHAR || data->oper1->length == ISZ_BOOLEAN))
    {
        *(*p)++ = 0xb0 + data->oper1->preg;
        *(*p)++ = val;
        return 1;
    }
    else if (data->oper1->length == ISZ_USHORT || data->oper2->length ==  - ISZ_USHORT || data->oper2->length == ISZ_U16)
        *(*p)++ = 0x66;
    *(*p)++ = 0xb8 + data->oper1->preg;
    if (data->oper1->length == ISZ_USHORT || data->oper2->length ==  - ISZ_USHORT || data->oper2->length == ISZ_U16)
    {
        *(short*)(*p) = val;
        (*p) += sizeof(short);
    }
    else
    {
        if (!resolved)
            data->addroffset =  *p - data->outbuf;
        *(int*)(*p) = val;
        (*p) += sizeof(int);
    }
    data->resobyte = 1;
    data->resolved = resolved;
    return 1;
}
/*-------------------------------------------------------------------------*/

int(*genfunctab[])(OPCODE *descript, OCODE *data, UBYTE **p) = 
{
    AOP0, AOP1, AOP2, AOP3, AOP4, AOP5, AOP6, AOP7, AOP8, AOP9, AOP10, AOP11,
        AOP12, AOP13, AOP14, AOP15, AOP16, AOP17, AOP18, AOP19, AOP20, AOP21,
        AOP22, AOP23, AOP24, AOP25, AOP26, AOP27, AOP28, AOP29, AOP30, AOP31,
        AOP32, AOP33, AOP34, AOP35, AOP36, AOP37, AOP38, AOP39, AOP40, AOP41,
        AOP42, AOP43, AOP44, AOP45, AOP46, AOP47, AOP48, AOP49, AOP50, AOP51,
        AOP52, AOP53, AOP54, AOP55, AOP56, AOP57, AOP58, AOP59, AOP60, AOP61,
        AOP62, AOP63, AOP64, AOP65,
};
int outcode_AssembleIns(OCODE *ins, int address)
{
    ins->resolved = TRUE;
    if (ins->opcode >= op_aaa)
    {
        UBYTE *p;
        OPCODE *descript = oplst[ins->opcode].data;
        int found = FALSE;
        p = ins->outbuf;
        while (descript->ocvalue || descript->ocoperands)
        {
            p = ins->outbuf;
            if (ins->oper1 && ins->oper1->seg && (ins->oper1->mode == am_direct
                || ins->oper1->mode == am_indisp || ins->oper1->mode ==
                am_indispscale))
                *p++ = segoverxlattab[ins->oper1->seg];
            else if (ins->oper2 && ins->oper2->seg && (ins->oper2->mode ==
                am_direct || ins->oper2->mode == am_indisp || ins->oper2->mode 
                == am_indispscale))
                *p++ = segoverxlattab[ins->oper2->seg];
            else if (ins->oper3 && ins->oper3->seg && (ins->oper3->mode ==
                am_direct || ins->oper3->mode == am_indisp || ins->oper3->mode 
                == am_indispscale))
                *p++ = segoverxlattab[ins->oper3->seg];
            if (descript->ocflags &OCprefix0F)
                *p++ = 0x0f;
            else if (descript->ocflags &OCprefixfwait)
                *p++ = 0x9b;
            if ((*genfunctab[descript->ocoperands])(descript, ins, &p))
            {
                found = TRUE;
                break;
            }
            descript++;
        }
        if (found)
        {
            ins->address = address;
            ins->outlen = p - ins->outbuf;
            if (ins->outbuf[0] == 0x0f && ins->outbuf[1] == 0x66)
                ins->outbuf[0] = 0x66, ins->outbuf[1] = 0x0f;
            if (ins->outbuf[0] == 0x9b && ins->outbuf[1] == 0x66)
                ins->outbuf[0] = 0x66, ins->outbuf[1] = 0x9b;
            return ins->outlen;
        }
        else
        {
            ins->outlen = 0;
            diag("Unassembled instruction in AssembleIns");
            return 0;
        }
    }
    else
    switch (ins->opcode)
    {
        case op_label:
            ins->address = address;
            InsertLabel((int)ins->oper1, address, oa_currentSeg == virtseg ? theCurrentFunc->value.i : codeseg);
            return 0;
        case op_line:
        case op_blockstart:
        case op_blockend:
        case op_varstart:
            ins->address = address;
            return ins->outlen = 0;
        case op_genword:
            ins->address = address;
            *((char*)(ins->outbuf)) = ins->oper1->offset->v.i;
            return ins->outlen = 1;
        case op_dd:
            ins->address = address;
            *(int*)(ins->outbuf) = 0;
            ins->resolved = FALSE;
            return ins->outlen = 4;
        default:
            return ins->outlen = 0;
    }
}

/*-------------------------------------------------------------------------*/

void outcode_optimize(OCODE *peeplist)
{
    int done = FALSE;
    while (!done)
    {
        OCODE *head = peeplist;
        int offset = 0;
        done = TRUE;
        while (head)
        {
            head->address += offset;
            if ((head->branched &BR_LONG) && head->oper1->offset->type ==
                en_labcon)
            {
                int adr = LabelAddress((int)head->oper1->offset->v.i);
                if (adr > head->address)
                    adr += offset;
                adr = adr - (head->address + 2);
                if (adr < 128 && adr >=  - 128)
                {
                    if (head->outbuf[0] == 0x0f)
                    {
                        offset -= 4;
                        outcode_base_address -= 4;
                        head->outbuf[0] = head->outbuf[1] - 0x10;
                    }
                    else
                    {
                        offset -= 3;
                        outcode_base_address -= 3;
                        head->outbuf[0] = 0xeb;
                    }
                    head->oper1->length =  - ISZ_UCHAR;
                    done = FALSE;
                    head->branched = BR_SHORT;
                    head->outlen = 2;
                    head->outbuf[1] = adr;
                    head->resolved = 1;
                }
                else
                {
                    adr += head->outlen - 2;
                    *((int*)(head->outbuf + head->outlen)) = adr;
                    head->resolved = 1;
                }
            }
            else if (head->branched &BR_SHORT)
            {
                int adr = LabelAddress((int)head->oper1->offset->v.i);
                if (adr > head->address)
                    adr += offset;
                adr = adr - (head->address + 2);
                head->outbuf[1] = adr;
            }
            if (head->opcode == op_label && offset)
                InsertLabel((int)head->oper1, head->address, oa_currentSeg == virtseg ? theCurrentFunc->value.i : codeseg);
            head = head->fwd;
        }
    }

}

/*-------------------------------------------------------------------------*/

void outcode_dumpIns(OCODE *peeplist)
{
    while (peeplist)
    {
        int resolved = FALSE;
        if (peeplist->branched)
        {
            if (peeplist->oper1->offset->type == en_labcon)
            {
                int adr = LabelAddress((int)peeplist->oper1->offset->v.i);
                adr = adr - peeplist->address - peeplist->outlen;
                if (peeplist->branched &BR_SHORT)
                {
                    if (adr <  - 128 || adr >= 127)
                    {
                        diag("Short branch out of range in outcode_dumpIns");
                    }
                    peeplist->outbuf[peeplist->addroffset] = adr;
                }
                else
                {
                    *(int*) &peeplist->outbuf[peeplist->addroffset] = adr;
                }
                resolved = TRUE;
            }
        }
        /*      if (peeplist->opcode == op_dd)*/
        /*         *(int *)(peeplist->outbuf) = LabelAddress((int)peeplist->oper1->offset->v.i) ;*/
        emit(oa_currentSeg, peeplist->outbuf, peeplist->outlen);
        if (!peeplist->resolved && !resolved)
        {
            AMODE *oper;
            EXPRESSION *node, *node1;
            if (peeplist->resobyte == 1)
            {
                oper = peeplist->oper2;
            }
            else
            {
                oper = peeplist->oper1;
            }
            if (oper->offset->type == en_sub && oper->offset->left->type == en_threadlocal)
            {
                node = GetSymRef(oper->offset->left);
                node1 = GetSymRef(oper->offset->right);
                gen_threadlocal_fixup(oa_currentSeg, 
                                      peeplist->address + peeplist->addroffset,
                                      node->v.sp, node1->v.sp);
            }
            else
            {
                node = GetSymRef(oper->offset);
                if (!node)
                    diag("Unlocated fixup in outcode_dumpIns");
                else if (node->type == en_labcon)
                    gen_label_fixup(peeplist->branched ? fm_rellabel : fm_label,
                        oa_currentSeg, peeplist->address + peeplist->addroffset, node->v.i)
                        ;
                else if (node->type == en_label)
                    gen_label_fixup(peeplist->branched ? fm_rellabel : fm_label,
                        oa_currentSeg, peeplist->address + peeplist->addroffset, node
                        ->v.sp->label);
                else 
                    gen_symbol_fixup(peeplist->branched ? fm_relsymbol : fm_symbol,
                        oa_currentSeg, peeplist->address + peeplist->addroffset, node
                        ->v.sp);
            }
            /* Special case the RM/IMM mode*/
            if (peeplist->resobyte == 3)
            {
                oper = peeplist->oper2;
                node = GetSymRef(oper->offset);
                if (!node)
                    diag("Unlocated fixup in outcode_dumpIns");
                else if (node->type == en_labcon)
                    gen_label_fixup(peeplist->branched ? fm_rellabel : fm_label,
                        oa_currentSeg, peeplist->address + peeplist->addroffset + 4,
                        node->v.i);
                else if (node->type == en_label)
                    gen_label_fixup(peeplist->branched ? fm_rellabel : fm_label,
                        oa_currentSeg, peeplist->address + peeplist->addroffset + 4, node
                        ->v.sp->label);
                else
                    gen_symbol_fixup(peeplist->branched ? fm_relsymbol :
                        fm_symbol, oa_currentSeg, peeplist->address + peeplist
                        ->addroffset + 4, node->v.sp);
            }
        }
        switch (peeplist->opcode)
        {
            case op_line:
                {
                    LINEDATA *ld = (LINEDATA *)peeplist->oper1;
                    while (ld->next)
                        ld = ld->next;
                    InsertLine(peeplist->address, ld);
                }
                break;
            case op_varstart:
                InsertVarStart(peeplist->address, (SYMBOL *)peeplist->oper1);
                break;
            case op_blockstart:
                InsertBlock(peeplist->address, 1);
                break;
            case op_blockend:
                InsertBlock(peeplist->address, 0);
                break;
            case op_funcstart:
                InsertFunc(peeplist->address, (SYMBOL *)peeplist->oper1, 1);
                break;
            case op_funcend:
                InsertFunc(peeplist->address, (SYMBOL *)peeplist->oper1, 0);
                break;
            default:
                break; 
        }
        peeplist = peeplist->fwd;
    }
}

/*-------------------------------------------------------------------------*/

void outcode_gen(OCODE *peeplist)
{
    OCODE *head = peeplist;
    outcode_func_init();
    while (head)
    {
        outcode_base_address += outcode_AssembleIns(head, outcode_base_address);
        head = head->fwd;
    }
    outcode_optimize(peeplist);
    if (!cparams.prm_asmfile)
        outcode_dumpIns(peeplist);
}
