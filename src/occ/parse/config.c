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
/*char/*
 * C configuration when we are using ICODE code generator output
 */
#include <stdio.h>
#include <string.h>
#include "compiler.h"
#include "winmode.h"
 
extern TEMP_INFO **tempInfo;
extern COMPILER_PARAMS cparams;
extern ARCH_ASM *chosenAssembler;

    #ifndef WIN32
        int prm_winmode = DOS32A;
    #else 
        int prm_winmode = CONSOLE;
    #endif 
    int prm_crtdll = FALSE;
    int prm_lscrtdll = FALSE;
    enum { pa_nasm,pa_masm, pa_tasm } prm_assembler;
    int prm_flat = FALSE ;
    int prm_nodos = FALSE ;
static    char usage_text[] = "[options] files\n"
    "\n""/9     - C99 mode                     +A        - disable extensions\n"
        "/Dxxx  - define something             /E[+]nn   - max number of errors\n"
        "/Ipath - specify include path         -O        - disable optimizations\n"
        "/Q     - quiet                        /S        - make assembly source file\n"
        "/T     - translate trigraphs          /Uxxx     - undefine something\n"
        "/Wxx   - set executable type          +e        - send errors to error file\n"
        "/fname - specify parameter file       +l        - create list file\n"
        "/oname - specify output file name     +v        - enable debug symbols\n"
        "Codegen parameters: (/C[+][-][params])\n"
        "  +d   - display diagnostics          -b     - no BSS\n"
        "  +F   - flat model                   -l     - no C source in ASM file\n"
        "  -m   - no leading underscores       +r     - reverse order of bit ops\n"
        "  +R   - honor FAR keyword            +u     - 'char' type is unsigned\n"
        "\nTime: " __TIME__ "  Date: " __DATE__;

static KEYWORD prockeywords[] = {
    {
        0,
    }
};
static ARCH_DEFINES defines[] = { 
    {"__ccidc__","",TRUE,TRUE },
    {0 },
};
static ARCH_SIZING sizes = {
    1, /*char a_bool; */
    1, /*char a_char; */
    2, /*char a_short; */
    2, /*char a_wchar_t; */
    4, /*char a_enum; */
    4, /*char a_int; */
    4, /*char a_long; */
    8, /*char a_longlong; */
    4, /*char a_addr; */
    8, /*char a_farptr; */
    2, /*char a_farseg; */
    8, /*char a_memberptr; */    
    0, /*char a_struct; */  /* alignment only */
    4, /*char a_float; */
    8, /*char a_double; */
    8, /*char a_longdouble; */
    0,/*char a_fcomplexpad; */
    0,/*char a_rcomplexpad; */
    0,/*char a_lrcomplexpad; */
} ;
static ARCH_SIZING alignments = {
    1,/*char a_bool; */
    1,/*char a_char; */
    2,/*char a_short; */
    2,/*char a_wchar_t; */
    4,/*char a_enum; */
    4,/*char a_int; */
    4,/*char a_long; */
    8,/*char a_longlong; */
    4,/*char a_addr; */
    4,/*char a_farptr; */
    2,/*char a_farseg; */
    4,/*char a_memberptr; */    
    0,/*char a_struct; */  /* alignment only */
    /* imaginary same as real */
    4,/*char a_float; */
    8,/*char a_double; */
    8,/*char a_longdouble; */
} ;
static ARCH_SIZING locks = {
    0, /*char a_bool; */
    0, /*char a_char; */
    0, /*char a_short; */
    0, /*char a_wchar_t; */
    0, /*char a_enum; */
    0, /*char a_int; */
    0, /*char a_long; */
    1, /*char a_longlong; */
    0, /*char a_addr; */
    1, /*char a_farptr; */
    1, /*char a_farseg; */
    1, /*char a_memberptr; */    
    1, /*char a_struct; */  /* alignment only */
    1, /*char a_float; */
    1, /*char a_double; */
    1, /*char a_longdouble; */
    1,/*char a_fcomplexpad; */
    1,/*char a_rcomplexpad; */
    1,/*char a_lrcomplexpad; */
} ;
static ARCH_FLOAT aflt = {
    -126 , 126, 128, 24
} ;
static ARCH_FLOAT adbl = {
    -1022, 1022, 1024, 53
    } ;
static ARCH_FLOAT aldbl = {
    -16382, 16382, 16384, 64
} ;
static ARCH_PEEP peeps[] = { 0 } ;
#include "regs.h"
static ARCH_CHARACTERISTICS architecture = {
    &alignments, /* alignments */
    0,  /* custom alignment routine */
    &sizes,    /* sizes */
    &locks,     /* atomic locks */
    0, /* routine is called in case parameters less than paramwidth need offsets */
    ISZ_ULONG,	/* size compatible to an integer */    
    ISZ_ULONG,	/* size compatible to an address */    
    8,				/* default packing level */
    /* floating point characteristics not fully implemented */
    &aflt,      /* characteristics of 'float' keyword */
    &adbl,        /* characteristics of 'double' keyword */
    &aldbl,       /* characteristics of 'long double' keyword */
    0,   /* future floating type */
    REG_MAX,		/* register count */
    &regNames[0],  /* defines registers */
    1,              /* register trees count */
    &regRoot,
    regClasses,
    &regCosts,
    allocOrder,
    peeps,   /* defines peephole information */
    0,             /* preferred optimizations */
    0,             /* optimizations we don't want */
    FALSE,			/* true if has floating point regs */
    0,            /* floating point modes, not honored currently */
    ABM_USESIZE,  /* BOOLEAN is determined by sizing above */
    ARM_FUNCTIONSCOPE, /* register allocation across entire function */
    8,            /* number of bits in a MAU.  values other than 8 not supported */
    FALSE,        /* little endian */
    FALSE,        /* normal bit allocation*/
    FALSE,        /* locals in stack memory*/
    FALSE,        /* stack pointer grows down */
    FALSE,        /* preallocate locals */
    8,            /* size of a return block on stack (e.g. function ret addr & frame ptr) */
    4,            /* minimium width/ padding of passed parameters in maus */
    4,            /* minimum stack alignment */
    FALSE,		  /* library functions should bes genned as import calls */
} ;
static void rvColor(IMODE *ip)
{
    int n = ip->offset->v.sp->value.i;
    tempInfo[n]->precolored = TRUE;
    if (ip->size >= ISZ_FLOAT)
    {
        tempInfo[n]->color = R_EAX;
    }
    else if (ip->size == ISZ_ULONGLONG || ip->size == -ISZ_ULONGLONG)
    {
        tempInfo[n]->color = R_EAXEDX;
    }
    else if (ip->size == ISZ_USHORT || ip->size == -ISZ_USHORT || ip->size == ISZ_U16)
        tempInfo[n]->color = R_AX;
    else if (ip->size == ISZ_UCHAR || ip->size == -ISZ_UCHAR || ip->size == ISZ_BOOLEAN)
        tempInfo[n]->color = R_AL;
    else		
        tempInfo[n]->color = R_EAX;
    tempInfo[n]->enode->v.sp->regmode = 2;
    tempInfo[n]->precolored = TRUE;
}
void PreColor(QUAD *head)			/* precolor an instruction */
{
    if (head->dc.opcode == i_sdiv || head->dc.opcode == i_udiv)
    {
        if (head->temps & TEMP_ANS)
        {
            int ta = head->ans->offset->v.sp->value.i;
            if (tempInfo[ta]->size < ISZ_FLOAT)
            {
                tempInfo[ta]->precolored = TRUE;
                tempInfo[ta]->enode->v.sp->regmode = 2;
                tempInfo[ta]->color = R_EAX;
                head->precolored |= TEMP_ANS;
            }
        }
        if (head->temps & TEMP_LEFT)
        {
            int tl = head->dc.left->offset->v.sp->value.i;
            if (tempInfo[tl]->size < ISZ_FLOAT)
            {
                tempInfo[tl]->precolored = TRUE;
                tempInfo[tl]->enode->v.sp->regmode = 2;
                tempInfo[tl]->color = R_EAXEDX;
                head->precolored |= TEMP_LEFT;
            }
        }
    }
    else if (head->dc.opcode == i_smod || head->dc.opcode == i_umod)
    {
        if (head->temps & TEMP_ANS)
        {
            int ta = head->ans->offset->v.sp->value.i;
            if (tempInfo[ta]->size < ISZ_FLOAT)
            {
                tempInfo[ta]->precolored = TRUE;
                tempInfo[ta]->enode->v.sp->regmode = 2;
                tempInfo[ta]->color = R_EDX;
                head->precolored |= TEMP_ANS;
            }
        }
        if (head->temps & TEMP_LEFT)
        {
            int tl = head->dc.left->offset->v.sp->value.i;
            if (tempInfo[tl]->size < ISZ_FLOAT)
            {
                tempInfo[tl]->precolored = TRUE;
                tempInfo[tl]->enode->v.sp->regmode = 2;
                tempInfo[tl]->color = R_EAXEDX;
                head->precolored |= TEMP_LEFT;
            }
        }
    }
    else if (head->dc.opcode == i_lsr || head->dc.opcode == i_asr || head->dc.opcode == i_lsl)
    {
        if (head->temps & TEMP_RIGHT)
        {
            int tr = head->dc.right->offset->v.sp->value.i;
            tempInfo[tr]->precolored = TRUE;
            head->precolored |= TEMP_RIGHT;
            switch (tempInfo[tr]->size)
            {
                case ISZ_BOOLEAN:
                case ISZ_UCHAR:
                case -ISZ_UCHAR:
                    tempInfo[tr]->color = R_CL;
                    break;
                case ISZ_U16:
                case ISZ_USHORT:
                case -ISZ_USHORT:
                    tempInfo[tr]->color = R_CX;
                    break;
                case ISZ_ULONGLONG:
                case -ISZ_ULONGLONG:
                    tempInfo[tr]->color = R_EDXECX;
                    break;
                default:
                    tempInfo[tr]->color = R_ECX;
                    break;
            }
            tempInfo[tr]->enode->v.sp->regmode = 2;
        }
    }
    if (head->ans && head->ans->retval)
        rvColor(head->ans);				
    if (head->dc.left && head->dc.left->retval)
        rvColor(head->dc.left);				
    if (head->dc.right && head->dc.right->retval)
        rvColor(head->dc.right);				
}
int PostGCSE(QUAD *head)
{
    while (head)
    {
        if (head->dc.opcode == i_sdiv || head->dc.opcode == i_udiv ||
            head->dc.opcode == i_smod || head->dc.opcode == i_umod)
        {
            if (head->dc.left->size < ISZ_ULONGLONG && head->dc.left->size > -ISZ_ULONGLONG)
            {
                int size;
                IMODE *im;
                QUAD *q;
                if (head->dc.opcode == i_sdiv || head->dc.opcode == i_smod)
                    size = -ISZ_ULONGLONG;
                else
                    size = ISZ_ULONGLONG;
                im = InitTempOpt(size, size);
                q = Alloc(sizeof(QUAD));
                q->dc.opcode = i_assn;
                q->ans = im;
                q->dc.left = head->dc.left;
                head->dc.left = im;
                if (head->temps & TEMP_LEFT)
                    q->temps = TEMP_LEFT;
                head->temps |= TEMP_LEFT;
                q->temps |= TEMP_ANS;
                InsertInstruction(head->back, q);
            }
        }
        head = head->fwd;
    }
}
static BOOLEAN hasbp(EXPRESSION *expr)
{
    if (!expr)
        return FALSE;
    if (expr->type == en_add)
        return (hasbp(expr->left) || hasbp(expr->right));
    return expr->type == en_auto;
}
static IMODE *FindConst(QUAD *ins)
{
    while (1)
    {
        if (ins->dc.left->mode == i_immed)
            return ins->dc.left;
        if ((ins->temps & TEMP_LEFT) && ins->dc.left->mode == i_direct)
        {
            ins = tempInfo[ins->dc.left->offset->v.sp->value.i]->instructionDefines;
            if (ins->dc.opcode != i_assn)
                break;
        }
        else
            break;
    }
    return NULL;
}
int ProcessOneInd(EXPRESSION *match,
                  EXPRESSION **ofs1, EXPRESSION **ofs2, EXPRESSION **ofs3, int *scale)
{
    if (match && match->type == en_tempref)
    {
        int n = match->v.sp->value.i;
        QUAD *ins = tempInfo[n]->instructionDefines;
        if (ins)
        {
            switch(ins->dc.opcode)
            {
                case i_add:
                    if (!tempInfo[ins->ans->offset->v.sp->value.i]->inductionLoop)
                    {
                        if (*ofs1 && ins->ans->offset->v.sp->value.i == (*ofs1)->v.sp->value.i)
                        {
                            EXPRESSION *offset1 = *ofs1, *offset2 = *ofs2, *offset3 = NULL;
                            if (ins->temps & TEMP_LEFT)
                            {
                                offset1 = ins->dc.left->offset;
                                if (ins->temps & TEMP_RIGHT)
                                {
                                    if (offset2)
                                        break;
                                    offset2 = ins->dc.right->offset;
                                }
                                else if (ins->dc.right->mode == i_immed)
                                {
                                    offset3 = ins->dc.right->offset;
                                }
                                else
                                    break;
                                    
                            }
                            else if (ins->temps & TEMP_RIGHT)
                            {
                                offset1 = ins->dc.right->offset;
                                if (ins->dc.left->mode == i_immed)
                                {
                                    offset3 = ins->dc.left->offset;
                                }
                                else
                                    break;
                            }
                            else if (ins->dc.left->mode == i_immed && ins->dc.right->mode == i_immed)
                            {
                                offset1 = NULL;
                                offset3 = exprNode(en_add, ins->dc.left->offset, ins->dc.right->offset);
                            }
                            else
                                break;
                            if ((hasbp(offset3) || hasbp(*ofs3)) && offset1 && offset2)
                                break;
                            *ofs1 = offset1;
                            *ofs2 = offset2;
                            if (offset3)
                                if (*ofs3)
                                    *ofs3 = exprNode(en_add, *ofs3, offset3);
                                else
                                    *ofs3 = offset3;
                        }
                        else if (*ofs2 && ins->ans->offset->v.sp->value.i == (*ofs2)->v.sp->value.i)
                        {
                            EXPRESSION *offset1 = *ofs1, *offset2 = *ofs2, *offset3 = NULL;
                            if (ins->temps & TEMP_LEFT)
                            {
                                offset2 = ins->dc.left->offset;
                                if (ins->temps & TEMP_RIGHT)
                                {
                                    if (offset1 || *scale)
                                        break;
                                    offset1 = ins->dc.right->offset;
                                }
                                else if (ins->dc.right->mode == i_immed)
                                {
                                    offset3 = ins->dc.right->offset;
                                }
                                else
                                    break;
                            }
                            else if (ins->temps & TEMP_RIGHT)
                            {
                                offset2 = ins->dc.right->offset;
                                if (ins->dc.left->mode == i_immed)
                                {
                                    offset3 = ins->dc.left->offset;
                                }
                                else
                                    break;
                            }
                            else if (ins->dc.left->mode == i_immed && ins->dc.right->mode == i_immed)
                            {
                                if (*scale && (ins->dc.left->size == ISZ_ADDR || ins->dc.right->size == ISZ_ADDR))
                                    break;
                                offset2 = NULL;
                                offset3 = exprNode(en_add, ins->dc.left->offset, ins->dc.right->offset);
                            }
                            else
                                break;
                            if ((hasbp(offset3) || hasbp(*ofs3)) && offset1 && offset2)
                                break;
                            *ofs1 = offset1;
                            *ofs2 = offset2;
                            if (offset3 && *scale)
                                offset3 = intNode(offset3->type, offset3->v.i << *scale);
                            if (offset3)
                                if (*ofs3)
                                    *ofs3 = exprNode(en_add, *ofs3, offset3);
                                else
                                    *ofs3 = offset3;
                        }
                    }
                    break;
                case i_sub:
                    if (!tempInfo[ins->ans->offset->v.sp->value.i]->inductionLoop)
                    {
                        if (*ofs1 && ins->ans->offset->v.sp->value.i == (*ofs1)->v.sp->value.i)
                        {
                            EXPRESSION *offset1 = *ofs1, *offset3 = NULL;
                            if (ins->temps & TEMP_LEFT)
                            {
                                offset1 = ins->dc.left->offset;
                                if (ins->dc.right->mode != i_immed || !isintconst(ins->dc.right->offset))
                                    break;
                                offset3 = ins->dc.right->offset;
                                *ofs1 = offset1;
                                if (*ofs3)
                                    *ofs3 = exprNode(en_sub, *ofs3, offset3);
                                else
                                    *ofs3 = intNode(offset3->type, -offset3->v.i);
                            }
                        }
                        else if (*ofs2 && ins->ans->offset->v.sp->value.i == (*ofs2)->v.sp->value.i)
                        {
                            EXPRESSION *offset2 = *ofs2, *offset3 = NULL;
                            if (ins->temps & TEMP_LEFT)
                            {
                                offset2 = ins->dc.left->offset;
                                if (ins->dc.right->mode != i_immed || !isintconst(ins->dc.right->offset))
                                    break;
                                offset3 = ins->dc.right->offset;
                                *ofs2 = offset2;
                                if (offset3 && *scale)
                                    offset3 = intNode(offset3->type, offset3->v.i << *scale);
                                if (*ofs3)
                                    *ofs3 = exprNode(en_sub, *ofs3, offset3);
                                else
                                    *ofs3 = intNode(offset3->type, -offset3->v.i);
                            }
                        }
                    }
                    break;
                case i_lsl:
                    if (!*scale && (ins->temps & TEMP_LEFT))
                    {
                        if (ins->dc.right->mode == i_immed && isintconst(ins->dc.right->offset))
                        {
                            EXPRESSION *offset1 = *ofs1, *offset2 = *ofs2;
                            int scl = ins->dc.right->offset->v.i;
                            if (scl >= 4)
                                break;
                            if (*ofs1 && ins->ans->offset->v.sp->value.i == (*ofs1)->v.sp->value.i)
                            {
                                offset1 = offset2;
                                offset2 = ins->dc.left->offset;
                            }
                            else if (*ofs2 && ins->ans->offset->v.sp->value.i == (*ofs2)->v.sp->value.i)
                            {
                                offset2 = ins->dc.left->offset;
                            }
                            else
                                break;
                            *ofs1 = offset1;
                            *ofs2 = offset2;
                            *scale = scl;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
}
void ProcessInd(EXPRESSION **ofs1, EXPRESSION **ofs2, EXPRESSION **ofs3, int *scale)
{
    EXPRESSION *ofs4, *ofs5;
    do
    {
        ofs4 = *ofs1, ofs5 = *ofs2;
        ProcessOneInd(*ofs1, ofs1, ofs2, ofs3, scale);
        ProcessOneInd(*ofs2, ofs1, ofs2, ofs3, scale);
    } while ((ofs4 != *ofs1 || ofs5 != *ofs2) && !*ofs2 && !*ofs3);
}
static BOOLEAN LookupMem(QUAD *q, IMODE **im)
{
    BOOLEAN rv = FALSE;
    if ((*im)->mode == i_direct && (*im)->offset->type == en_tempref)
    {
        QUAD *tail = q->back;
        while (tail && tail->dc.opcode != i_block)
        {
            if (tail->dc.opcode == i_assn && tail->ans == *im)
            {
                if (tail->dc.left->mode== i_direct && !(tail->temps & TEMP_LEFT))
                {
                    if ((*im) == q->dc.left)
                    {
                        q->temps &= ~TEMP_LEFT;
                    }
                    if ((*im) == q->dc.right)
                    {
                        q->temps &= ~TEMP_RIGHT;
                    }
                    *im = tail->dc.left;
                    rv = TRUE;
                }
                break;
            }
            tail = tail->back;			
        }
    }
    return rv;
}
// relies on constant offsets being calculated the same way every time
static BOOLEAN MatchesConst(EXPRESSION *one, EXPRESSION *two)
{
    if (one == two)
        return TRUE;
    if (!one || !two)
        return FALSE;
    if (one->type != two->type)
        return FALSE;
    if (!MatchesConst(one->left, two->left) || !MatchesConst(one->right, two->right))
        return FALSE;
    if (isintconst(one))
        return one->v.i == two->v.i;
    switch(one->type)
    {
        case en_global:
        case en_auto:
        case en_pc:
        case en_labcon:
        case en_threadlocal:
            return one->v.sp == two->v.sp;
        case en_label:
            return (one->v.i == two->v.i);
        default:
            return FALSE;		
    }
}
static BOOLEAN MatchesMem(IMODE *one, IMODE *two)
{
    if (one == two)
        return TRUE;
    if (one->mode == i_ind && two->mode == i_ind)
    {
        if (one->offset && !two->offset || one->offset2 && !two->offset2 || one->offset3 && !two->offset3)
            return FALSE;
        if (!one->offset && two->offset || !one->offset2 && two->offset2 || !one->offset3 && two->offset3)
            return FALSE;
        if (one->offset)
            if (one->offset->v.sp->value.i != two->offset->v.sp->value.i)
                return FALSE;
        if (one->offset2)
            if (one->offset2->v.sp->value.i != two->offset2->v.sp->value.i)
                return FALSE;
        if (one->offset3)
            if (MatchesConst(one->offset3, two->offset3))
                return FALSE;
        return TRUE;
    }
    return FALSE;
}
static BOOLEAN HandleAssn(QUAD *ins, BRIGGS_SET *globalVars)
{
    if (ins->temps & (TEMP_LEFT|TEMP_RIGHT))
    {
        if (ins->ans->mode == i_ind || !(ins->temps & TEMP_ANS))
        {
            if (!briggsTest(globalVars, ins->dc.left->offset->v.sp->value.i))
            {
                if (ins->back->ans == ins->dc.left)
                {
                    switch(ins->back->dc.opcode)
                    {
                        case i_neg:
                        case i_not:
                        case i_sub:
                        case i_lsl:
                        case i_lsr:
                        case i_asr:
                            if (ins->back->dc.right && ins->back->dc.right->mode == i_immed || ((ins->temps & TEMP_RIGHT) && ins->back->dc.right->mode == i_direct))
                                if ((ins->back->temps & TEMP_LEFT) && ins->back->dc.left->mode == i_direct)
                                    if (ins->back->back->dc.opcode == i_assn)
                                        if (ins->back->dc.left == ins->back->back->ans)
                                            if (MatchesMem(ins->ans, ins->back->back->dc.left))
                                            {
                                                if (!briggsTest(globalVars, ins->back->ans->offset->v.sp->value.i) 
                                                    && !briggsTest(globalVars, ins->back->back->ans->offset->v.sp->value.i))
                                                {
                                                    ins->dc.left = ins->ans;
                                                    ins->dc.right = ins->back->dc.right;
                                                    if (ins->dc.left->offset)
                                                        if (ins->dc.left->offset->type == en_tempref)
                                                            ins->temps = TEMP_LEFT | TEMP_ANS;
                                                        else
                                                            ins->temps = 0;
                                                    else
                                                        ins->temps = TEMP_LEFT | TEMP_ANS;
                                                    if (ins->dc.right && ins->dc.right->offset && ins->dc.right->offset == en_tempref)
                                                        ins->temps |= TEMP_RIGHT;
                                                    ins->dc.opcode = ins->back->dc.opcode;
                                                }
                                            }									
                            break;
                        case i_add:
                        case i_or:
                        case i_eor:
                        case i_and:
                            if (ins->back->dc.right && ins->back->dc.right->mode == i_immed || ((ins->temps & TEMP_RIGHT) && ins->back->dc.right->mode == i_direct))
                                if ((ins->back->temps & TEMP_LEFT) && ins->back->dc.left->mode == i_direct)
                                    if (ins->back->back->dc.opcode == i_assn)
                                        if (ins->back->dc.left == ins->back->back->ans)
                                            if (MatchesMem(ins->ans, ins->back->back->dc.left))
                                            {
                                                if (!briggsTest(globalVars, ins->back->ans->offset->v.sp->value.i) 
                                                    && !briggsTest(globalVars, ins->back->back->ans->offset->v.sp->value.i))
                                                {
                                                    ins->dc.left = ins->ans;
                                                    ins->dc.right = ins->back->dc.right;
                                                    if (ins->dc.left->offset)
                                                        if (ins->dc.left->offset->type == en_tempref)
                                                            ins->temps = TEMP_LEFT | TEMP_ANS;
                                                        else
                                                            ins->temps = 0;
                                                    else
                                                        ins->temps = TEMP_LEFT | TEMP_ANS;
                                                    if (ins->dc.right && ins->dc.right->offset && ins->dc.right->offset == en_tempref)
                                                        ins->temps |= TEMP_RIGHT;
                                                    ins->dc.opcode = ins->back->dc.opcode;
                                                    break;
                                                }
                                            }
                            if (ins->back->dc.left && ins->back->dc.left->mode == i_immed || ((ins->temps & TEMP_LEFT) && ins->back->dc.left->mode == i_direct))
                                if ((ins->back->temps & TEMP_RIGHT) && ins->back->dc.right->mode == i_direct)
                                    if (ins->back->back->dc.opcode == i_assn)
                                        if (ins->back->dc.left == ins->back->back->ans)
                                            if (MatchesMem(ins->ans, ins->back->back->dc.left))
                                            {
                                                if (!briggsTest(globalVars, ins->back->ans->offset->v.sp->value.i) 
                                                    && !briggsTest(globalVars, ins->back->back->ans->offset->v.sp->value.i))
                                                {
                                                    ins->dc.right = ins->dc.left;
                                                    ins->dc.left = ins->ans;
                                                    if (ins->dc.left->offset)
                                                        if (ins->dc.left->offset->type == en_tempref)
                                                            ins->temps = TEMP_LEFT | TEMP_ANS;
                                                        else
                                                            ins->temps = 0;
                                                    else
                                                        ins->temps = TEMP_LEFT | TEMP_ANS;
                                                    if (ins->dc.right && ins->dc.right->offset && ins->dc.right->offset == en_tempref)
                                                        ins->temps |= TEMP_RIGHT;
                                                    ins->dc.opcode = ins->back->dc.opcode;
                                                }
                                            }									

                            break;				
                        default:
                            break;			
                    }
                }
            }
        }
    }
    else if (ins->dc.left->mode == i_immed && ((!ins->temps & TEMP_ANS) || (ins->ans->mode == i_ind)))
    {
        if (isintconst(ins->dc.left->offset) && ins->dc.left->offset->v.i== 0)
        {
            IMODE *imn = InitTempOpt(ins->ans->size, ins->ans->size);
            QUAD *newIns = Alloc(sizeof(QUAD));
            newIns->ans = imn;
            newIns->dc.left = ins->dc.left;
            newIns->dc.opcode = i_assn;
            newIns->temps = TEMP_ANS;
            ins->dc.left = imn;
            ins->temps |= TEMP_LEFT;
            InsertInstruction(ins->back, newIns);
        }
    }
}
int PreRegAlloc(QUAD *ins, BRIGGS_SET *globalVars, BRIGGS_SET *eobGlobals, int pass)
{
    IMODE *ind = NULL;
    if (pass == 1)
    {
        if (ins->ans && ins->ans->mode == i_ind)
        {
            ind = ins->ans;
        }
        else if (ins->dc.left && ins->dc.left->mode == i_ind)
        {
            ind = ins->dc.left;
        }
        if (ind)
        {
            EXPRESSION *offset1 = ind->offset, *offset2 = NULL, *offset3 = NULL;
            int scale = 0;
            ProcessInd(&offset1, &offset2, &offset3, &scale);
            if (offset2 || offset3 || ind->offset != offset1)
            {
                IMODE *ind1 = Alloc(sizeof(IMODE));
                *ind1 = *ind;
                if (ins->ans == ind)
                    ins->ans = ind1;
                else
                    ins->dc.left = ind1;
                ind = ind1;
                ind->offset = offset1;
                ind->offset2 = offset2;
                ind->offset3 = offset3;
                ind->scale = scale;
            }
        }
        return 0;
    }
    else if (pass == 2)
    {
        switch(ins->dc.opcode)
        {
            case i_assn:
                HandleAssn(ins, globalVars);
                break;			
            case i_parm:
                LookupMem(ins, &ins->dc.left);
                break;
            case i_sub:
            case i_add:
            case i_or:
            case i_eor:
            case i_and:
            case i_mul:
                if ((ins->temps & TEMP_ANS) && ins->ans->mode == i_direct)
                {
                    LookupMem(ins, &ins->dc.left);
                    LookupMem(ins, &ins->dc.right);
                }
                break;				
            case i_lsl:
            case i_lsr:
            case i_asr:
                if ((ins->temps & TEMP_ANS) && ins->ans->mode == i_direct)
                    LookupMem(ins, &ins->dc.left);
                break;				
            case i_sdiv:
            case i_udiv:
            case i_smod:
            case i_umod:
                LookupMem(ins, &ins->dc.right);
                break;
            case i_je:
            case i_jne:
            case i_jl:
            case i_jc:
            case i_jg:
            case i_ja:
            case i_jle:
            case i_jbe:
            case i_jge:
            case i_jnc:
                if (ins->dc.right->mode == i_immed || (ins->temps & TEMP_RIGHT) && ins->dc.right->mode == i_direct)
                    if (!(ins->temps & TEMP_LEFT) || !briggsTest(eobGlobals, ins->dc.left->offset->v.sp->value.i))
                        LookupMem(ins, &ins->dc.left);
                if ((ins->temps & TEMP_LEFT) && ins->dc.left->mode == i_direct)
                    if (!(ins->temps & TEMP_RIGHT) || !briggsTest(eobGlobals, ins->dc.right->offset->v.sp->value.i))
                        LookupMem(ins, &ins->dc.right);
                break;
            default:
                break;			
        }
        return 1;
    }
    return 1;
}
ARCH_GEN outputfunctions =
{
    NULL, NULL,
    NULL, PostGCSE,
    NULL,
    NULL,
    NULL,
    PreRegAlloc,
    PreColor,
};

static int initasm(COMPILER_PARAMS *parms, ARCH_ASM *data, ARCH_DEBUG *debug)
{
    (void)parms;
    (void)data;
    (void)debug;
    prm_assembler = pa_nasm;
    cparams.prm_icdfile = TRUE;
    return 1;
}
    static void WinmodeSetup(char select, char *string)
    {
        (void)select;
        switch (string[0])
        {
            case 'r':
                prm_winmode = RAW;
                break;
            case 'a':
                prm_winmode = DOS32A;
                break;
            case 'd':
                prm_winmode = DLL;
                defines[0].respect = TRUE;
                break;
            case 'c':
                prm_winmode = CONSOLE;
                defines[0].respect = TRUE;
                break;
            case 'g':
                prm_winmode = GUI;
                defines[0].respect = TRUE;
                break;
            case 'e':
                prm_winmode = DOS;
                break;
            default:
                fatal("Invalid executable type");
        }
        if (string[1] == 'c')
        {
            if (prm_winmode == DOS)
                fatal("Invalid use of CRTDLL");
            prm_crtdll = TRUE;
        } else if (string[1] == 'l') {
            if (prm_winmode == DOS)
                fatal("Invalid use of LSCRTDLL");
            prm_lscrtdll = TRUE;
            defines[1].respect = TRUE;
        }
    }
static int parse_param(int mode, char *string)
{
    if (*string == 'W') {
        WinmodeSetup(mode,string+1);
        return 2 ; /* eat string */
    }
    return 0;
}
static int parse_codegen(int mode, char *string)
{
    switch (string[0])
    {
        case 'X':
            prm_nodos = mode;
            break;
        case 'F':
            prm_flat = mode;
            break;
        case 'R':
            cparams.prm_farkeyword = mode;
            break;
        case '?':
            cparams.prm_browse = mode;
            break;
        default:
            return 0; /* illegal */
    }
    return 1; /* eat one char */
}
ARCH_ASM assemblerInterface[] = {
    {
    "asm",                                  /* assembler name */
    0,                                      /* backend data (compiler ignores) */
    "1",								/* __STDC__HOSTED__ value "0" = embedded, "1" = hosted */
    ".asm",                                  /* extension for assembly files */
    "",                               /* extension for object files, NULL = has no object mode */
    "CC",                               /* name of an environment variable to parse, or 0 */
    "CC",                             /* name of the program, for usage */
    "CC",                              /* name of a config file if you want to use one, or NULL (sans extension) */
    usage_text,                           /* pointer to usage text */
    NULL,									/* no arguments */
    0,										/* no arguments */
    0,                             /* inline assembler opcode list, or null */
    0,                             /* inline assembler register list, or null */
    prockeywords,                         /* specific keywords, e.g. allow a 'bit' keyword and so forth */
    defines,                     /* defines list to create at compile time, or null */
    0,                         /* debug structure, or NULL */
    &architecture,                /* architecture characteristics */
    &outputfunctions,                              /* pointer to backend function linkages */
    initasm,  /* return 1 to proceed */
    0,     /* postprocess function, or NULL */
    0,     /* compiler rundown */
    0,          /* insert the output (executable name) into the backend */
    0,      /* insert a non-compilable file in the backend list, e.g. for post processing, or NULL */
    parse_param,     /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    parse_codegen,   /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    0,         /* parse a pragma directive, or null */
    0,             /* signal start of compile on a per file basis */
    0,		/* signal switching to a new source file */
    0,              /* write the object file (for native object formats) */
    0,                 /* initialize the object file */
    0,                    /* initialize inline assembler, per file, or NULL */
    0,                   /* parse an assembly statement, or NULL */
    0,                   /* initialize intrinsic mechanism, compiler startup */
    0,                   /* search for an intrinsic */
    },
    { 0 }
} ;

