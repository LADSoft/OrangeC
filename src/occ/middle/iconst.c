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
 * iconst.c
 *
 * Constant folding/ dead block removal
 *
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "compiler.h"
 
extern COMPILER_PARAMS cparams;
extern ARCH_ASM *chosenAssembler;
extern TEMP_INFO **tempInfo;
extern BLOCK **blockArray;
extern int tempCount;
extern int blockCount;

static BRIGGS_SET *visited ;
static INSTRUCTIONLIST *insWorkHead, *insWorkTail, *listHolder;
static BLOCKLIST *blockWorkHead, *blockWorkTail;

#ifdef XXXXX
IMODE *intconst(LLONG_TYPE val)
{
    return make_immed(ISZ_UINT,val);
}
#endif

/*-------------------------------------------------------------------------*/

void badconst(void)
{
    diag("ICODE- Bad constant node");
}

static void ReassignMulDiv(QUAD *d, enum i_ops op, LLONG_TYPE val, int fromleft)
{
    IMODE *ip = make_immed(ISZ_UINT,val);
    if (fromleft) {
        d->dc.left = d->dc.right;
        d->temps |= (d->temps & TEMP_RIGHT) ? TEMP_LEFT : 0;
    }
    d->dc.right = ip;	
    d->dc.opcode = op;
    d->temps &= ~TEMP_RIGHT;
    if (d->temps & TEMP_ANS)
        tempInfo[d->ans->offset->v.sp->value.i]->preSSATemp = -1;
}
static void ReassignInt(QUAD *d, LLONG_TYPE val)
{
    IMODE *ip = make_immed(ISZ_UINT,val);
    d->dc.left = ip ;
    d->dc.right = 0;
    d->dc.opcode = i_assn;
    d->temps &= ~(TEMP_LEFT | TEMP_RIGHT);
    if (d->temps & TEMP_ANS)
    {
        tempInfo[d->ans->offset->v.sp->value.i]->preSSATemp = -1;
        d->dc.left->size = d->ans->size;
    }
}
static void ReassignFloat(QUAD *d, FPF val)
{
    IMODE *ip = make_fimmed(ISZ_LDOUBLE, val);
    d->dc.left = ip ;
    d->dc.right = 0;
    d->dc.opcode = i_assn;
    d->temps &= ~(TEMP_LEFT | TEMP_RIGHT);
    if (d->temps & TEMP_ANS)
        tempInfo[d->ans->offset->v.sp->value.i]->preSSATemp = -1;
}
static void setFloatZero(QUAD *d)
{
    IMODE *ip ;
    FPF val;
    memset(&val, 0, sizeof(val));
    val.type = IFPF_IS_ZERO;
    ip = make_fimmed(ISZ_LDOUBLE, val);
    d->dc.left = ip ;
    d->dc.right = 0;
    d->dc.opcode = i_assn;
    d->temps &= ~(TEMP_LEFT | TEMP_RIGHT);
    if (d->temps & TEMP_ANS)
        tempInfo[d->ans->offset->v.sp->value.i]->preSSATemp = -1;
}
static void ReassignCompare(QUAD *d, int yes, BOOLEAN reflow)
{
    if (d->block->dead)
    {
        d->temps = 0;
        d->dc.left = d->dc.right = NULL;
        return;
    }
    if (d->dc.opcode >= i_jne) 
    {
        BLOCKLIST *b, *n;
        if (yes)
        {
            d->dc.opcode = i_goto;
            b = d->block->succ->next;
        }
        else 
        {
            d->dc.opcode = i_nop;
            b = d->block->succ;
        }
        d->temps = 0;
        d->dc.left = d->dc.right = NULL;
        if (reflow)
            reflowConditional(d->block, b->block);
#ifdef XXXXX
        l1 = & n->block->pred->next;
        while (*l1)
        {
            if (d->block == (*l1)->block)
            {
                *l1 = (*l1)->next;
                break;
            }
            l1 = & (*l1)->next;
        }
        b->next = NULL;
        d->block->succ = b;
#endif
    } else {
        ReassignInt(d,yes);
    }
}
static void ASNFromLeft(QUAD *d)
{
    d->dc.right = 0;
    d->dc.opcode = i_assn;
    d->temps &= ~TEMP_RIGHT;
}
static void ASNFromRight(QUAD *d)
{
    d->dc.left = d->dc.right;
    d->dc.right = 0;
    d->dc.opcode = i_assn;
    d->temps |= (d->temps & TEMP_RIGHT) ? TEMP_LEFT : 0;
    d->temps &= ~TEMP_RIGHT;
}
static int xgetmode(QUAD *d, EXPRESSION **left, EXPRESSION **right)
{
    int mode = icnone;
    *left = d->dc.left->offset;
    if (d->dc.right)
        *right = d->dc.right->offset;
    else
        *right = NULL;
    if (*left) {
        if (*right) {
            if (isintconst((*left)))
            {
                if (isintconst((*right)))
                    mode = icll;
                else if (isconstaddress((*right)))
                    mode = icla;
                else if (!((*right)->pragmas & STD_PRAGMA_FENV))
                {
                    if (isfloatconst((*right)))
                        mode = iclr;
                    else if (isimaginaryconst((*right)))
                        mode = icli;
                    else if (iscomplexconst((*right)))
                        mode = iclc;
                }
            }
            else if (isconstaddress((*left)))
            {
                if (isintconst((*right)))
                    mode = ical;
                else if (isconstaddress((*right)))
                    mode = icaa;
                    
            }
            else if (!((*left)->pragmas & STD_PRAGMA_FENV))
            {
                if (isfloatconst((*left)))
                {
                    if (isintconst((*right)))
                        mode = icrl;
                    else if (!((*right)->pragmas & STD_PRAGMA_FENV))
                    {
                        if (isfloatconst((*right)))
                            mode = icrr;
                        else if (isimaginaryconst((*right)))
                            mode = icri;
                        else if (iscomplexconst((*right)))
                            mode = icrc;
                    }
                }
                else if (isimaginaryconst((*left)))
                {
                    if (isintconst((*right)))
                        mode = icil;
                    else if (!((*right)->pragmas & STD_PRAGMA_FENV))
                    {
                        if (isfloatconst((*right)))
                            mode = icir;
                        else if (isimaginaryconst((*right)))
                            mode = icii;
                        else if (iscomplexconst((*right)))
                            mode = icic;
                    }
                }
                else if (iscomplexconst((*left)))
                {
                    if (isintconst((*right)))
                        mode = iccl;
                    else if (!((*right)->pragmas & STD_PRAGMA_FENV))
                    {
                        if (isfloatconst((*right)))
                            mode = iccr;
                        else if (isimaginaryconst((*right)))
                            mode = icci;
                        else if (iscomplexconst((*right)))
                            mode = iccc;
                    }
                }
            }
        } else {
            if (isintconst((*left)))
                mode = icln;
            else if (isconstaddress((*left)))
            {
                if (isintconst((*right)))
                    mode = ical;
                else if (isconstaddress((*right)))
                    mode = icaa;
            }
            else if (!((*left)->pragmas & STD_PRAGMA_FENV))
            {
                if (isfloatconst((*left)))
                    mode = icrn;
                else if (isimaginaryconst((*left)))
                    mode = icin;
                else if (iscomplexconst((*left)))
                    mode = iccn;
            }
        }
    } 
    if (mode == icnone && *right) 
    {
        if (isintconst((*right)))
            mode = icnl;
        else if (!((*right)->pragmas & STD_PRAGMA_FENV))
        {
            if (isfloatconst((*right)))
                mode = icnr;
            else if (isimaginaryconst((*right)))
                mode = icni;
            else if (iscomplexconst((*right)))
                mode = icnc;
        }
    }
    return (mode);
}
/*-------------------------------------------------------------------------*/
LLONG_TYPE calmask(int i)
{
    if (pwrof2(i) != -1)
        return i - 1;
    return (LLONG_TYPE)-1;
}
QUAD *ReCast(int size, QUAD *in, QUAD *newMode)
{
    if (size) {
        if (size <= ISZ_ULONGLONG || size == ISZ_ADDR) {
            LLONG_TYPE i;
            if (in->dc.opcode == i_icon)
                i = CastToInt(size, 	in->dc.v.i);
            else
                i = CastToInt(size, 	FPFToLongLong(&in->dc.v.f));
            if (!newMode) {
                in->dc.v.i = i;
                return in;
            } else {
                memset(&newMode->dc,0,sizeof(newMode->dc));
                newMode->dc.opcode = i_icon;
                newMode->dc.v.i = i;
                newMode->ans = make_immed(ISZ_UINT,i);
                return newMode;
            }
        } else if (size >= ISZ_FLOAT) {
            FPF f, temp;
            if (in->dc.opcode == i_icon)
                f = CastToFloat(size, 	IntToFloat(&temp, ISZ_ULONGLONG, in->dc.v.i));
            else
                f = CastToFloat(size, 	&in->dc.v.f);
            if (!newMode) {
                in->dc.v.f = f;
                return in;
            } else {
                memset(&newMode->dc,0,sizeof(newMode->dc));
                newMode->dc.opcode = i_fcon;
                newMode->dc.v.f = f;
                newMode->ans = make_fimmed(ISZ_LDOUBLE, f);
                return newMode;
            }
        } else
            return in;
    }
    return in;
}
/*-------------------------------------------------------------------------*/
void ConstantFold(QUAD *d, BOOLEAN reflow)
{
    int index; /*, shift; */
    int shift;
    FPF temp;
    EXPRESSION *left = 0,  *right = 0;
    switch (d->dc.opcode)
    {
        case i_setne:
        case i_jne:
            index = xgetmode(d, &left, &right);
            switch(index)
            {
            case icll:
                ReassignCompare(d,left->v.i != right->v.i, reflow);
                break;
            case iclr:
                LongLongToFPF(&temp,left->v.i);
                ReassignCompare(d,!FPFEQ(&temp, &right->v.f), reflow);
                break;
            case icrl:
                LongLongToFPF(&temp,right->v.i);
                ReassignCompare(d,!FPFEQ(&left->v.f, &temp), reflow);
                break;
            case icrr:
                ReassignCompare(d,!FPFEQ(&left->v.f, &right->v.f), reflow);
                break;
            case ical:
            case icla:
                ReassignCompare(d, TRUE, reflow);
                break;
            case icaa:
                ReassignCompare(d, !equalnode(left, right), reflow);
                break;			
            }		
            break;
        case i_sete:
        case i_je:
            index = xgetmode(d, &left, &right);
            switch(index)
            {
            case icll:
                ReassignCompare(d,left->v.i == right->v.i, reflow);
                break;
            case iclr:
                LongLongToFPF(&temp,left->v.i);
                ReassignCompare(d,FPFEQ(&temp, &right->v.f), reflow);
                break;
            case icrl:
                LongLongToFPF(&temp,right->v.i);
                ReassignCompare(d,FPFEQ(&left->v.f, &temp), reflow);
                break;
            case icrr:
                ReassignCompare(d,FPFEQ(&left->v.f, &right->v.f), reflow);
                break;
            case ical:
            case icla:
                ReassignCompare(d, FALSE, reflow);
                break;
            case icaa:
                ReassignCompare(d, equalnode(left, right), reflow);
                break;
            }		
            break;
        case i_setc:
        case i_jc:
            index = xgetmode(d, &left, &right);
            switch(index)
            {
            case icll:
                if (right->v.i == 0)
                    ReassignCompare(d,0, reflow);
                else
                    ReassignCompare(d,(unsigned LLONG_TYPE)left->v.i < (unsigned LLONG_TYPE)right->v.i, reflow);
                break;
            case iclr:
                if (right->v.f.type == IFPF_IS_ZERO)
                    ReassignCompare(d,0, reflow);
                else {
                    UnsignedLongLongToFPF(&temp, left->v.i);
                    ReassignCompare(d,!FPFGTE(&temp, &right->v.f), reflow);
                }
                break;
            case icrl:
                if (right->v.i == 0)
                    ReassignCompare(d,0, reflow);
                else {
                    UnsignedLongLongToFPF(&temp, right->v.i);
                    ReassignCompare(d,!FPFGTE(&left->v.f, &temp), reflow);
                }
                break;
            case icrr:
                if (right->v.f.type == IFPF_IS_ZERO)
                    ReassignCompare(d,0, reflow);
                else
                    ReassignCompare(d,!FPFGTE(&left->v.f, &right->v.f), reflow);
                break;
            }		
            break;
        case i_setbe:
        case i_jbe:
            index = xgetmode(d, &left, &right);
            switch(index)
            {
            case icll:
                ReassignCompare(d,(unsigned LLONG_TYPE)left->v.i <= (unsigned LLONG_TYPE)right->v.i, reflow);
                break;
            case iclr:
                UnsignedLongLongToFPF(&temp, left->v.i);
                ReassignCompare(d,!FPFGT(&temp, &right->v.f), reflow);
                break;
            case icrl:
                UnsignedLongLongToFPF(&temp, right->v.i);
                ReassignCompare(d,!FPFGT(&left->v.f, &temp), reflow);
                break;
            case icrr:
                ReassignCompare(d,!FPFGT(&left->v.f, &right->v.f), reflow);
                break;
            }		
            break;
        case i_seta:
        case i_ja:
            index = xgetmode(d, &left, &right);
            switch(index)
            {
            case icll:
                ReassignCompare(d,(unsigned LLONG_TYPE)left->v.i > (unsigned LLONG_TYPE)right->v.i, reflow);
                break;
            case iclr:
                UnsignedLongLongToFPF(&temp, left->v.i);
                ReassignCompare(d,FPFGT(&temp, &right->v.f), reflow);
                break;
            case icrl:
                UnsignedLongLongToFPF(&temp, right->v.i);
                ReassignCompare(d,FPFGT(&left->v.f, &temp), reflow);
                break;
            case icrr:
                ReassignCompare(d,FPFGT(&left->v.f, &right->v.f), reflow);
                break;
            }		
            break;
        case i_setnc:
        case i_jnc:
            index = xgetmode(d, &left, &right);
            switch(index)
            {
            case icll:
                ReassignCompare(d,(unsigned LLONG_TYPE)left->v.i >= (unsigned LLONG_TYPE)right->v.i, reflow);
                break;
            case iclr:
                UnsignedLongLongToFPF(&temp, left->v.i);
                ReassignCompare(d,FPFGTE(&temp, &right->v.f), reflow);
                break;
            case icrl:
                UnsignedLongLongToFPF(&temp, right->v.i);
                ReassignCompare(d,FPFGTE(&left->v.f, &temp), reflow);
                break;
            case icrr:
                ReassignCompare(d,FPFGTE(&left->v.f, &right->v.f), reflow);
                break;
            }		
            break;
        case i_setl:
        case i_jl:
            index = xgetmode(d, &left, &right);
            switch(index)
            {
            case icll:
                ReassignCompare(d,left->v.i < right->v.i, reflow);
                break;
            case iclr:
                LongLongToFPF(&temp, left->v.i);
                ReassignCompare(d,!FPFGTE(&temp, &right->v.f), reflow);
                break;
            case icrl:
                LongLongToFPF(&temp, right->v.i);
                ReassignCompare(d,!FPFGTE(&left->v.f, &temp), reflow);
                break;
            case icrr:
                ReassignCompare(d,!FPFGTE(&left->v.f, &right->v.f), reflow);
                break;
            }		
            break;
        case i_setle:
        case i_jle:
            index = xgetmode(d, &left, &right);
            switch(index)
            {
            case icll:
                ReassignCompare(d,left->v.i <= right->v.i, reflow);
                break;
            case iclr:
                LongLongToFPF(&temp, left->v.i);
                ReassignCompare(d,!FPFGT(&temp, &right->v.f), reflow);
                break;
            case icrl:
                LongLongToFPF(&temp, right->v.i);
                ReassignCompare(d,!FPFGT(&left->v.f, &temp), reflow);
                break;
            case icrr:
                ReassignCompare(d,!FPFGT(&left->v.f, &right->v.f), reflow);
                break;
            }		
            break;
        case i_setg:
        case i_jg:
            index = xgetmode(d, &left, &right);
            switch(index)
            {
            case icll:
                ReassignCompare(d,left->v.i > right->v.i, reflow);
                break;
            case iclr:
                LongLongToFPF(&temp, left->v.i);
                ReassignCompare(d,FPFGT(&temp, &right->v.f), reflow);
                break;
            case icrl:
                LongLongToFPF(&temp, right->v.i);
                ReassignCompare(d,FPFGT(&left->v.f, &temp), reflow);
                break;
            case icrr:
                ReassignCompare(d,FPFGT(&left->v.f, &right->v.f), reflow);
                break;
            }		
            break;
        case i_setge:
        case i_jge:
            index = xgetmode(d, &left, &right);
            switch(index)
            {
            case icll:
                ReassignCompare(d,left->v.i >= right->v.i, reflow);
                break;
            case iclr:
                LongLongToFPF(&temp, left->v.i);
                ReassignCompare(d,FPFGTE(&temp, &right->v.f), reflow);
                break;
            case icrl:
                LongLongToFPF(&temp, right->v.i);
                ReassignCompare(d,FPFGTE(&left->v.f, &temp), reflow);
                break;
            case icrr:
                ReassignCompare(d,FPFGTE(&left->v.f, &right->v.f), reflow);
                break;
            }		
            break;
        case i_add:
        case i_array:
            if (d->dc.right->offset->type == en_structelem)
                break;
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icll:
                ReassignInt(d,left->v.i + right->v.i);
                break;
            case iclr:
                temp = refloat(left);
                AddSubFPF(FALSE, &temp, &right->v.f, &temp);
                ReassignFloat(d, temp);
                break;
            case icrl:
                temp = refloat(right);
                AddSubFPF(FALSE, &left->v.f, &temp, &temp);
                ReassignFloat(d, temp);
                break;
            case icrr:
                AddSubFPF(FALSE, &left->v.f, &right->v.f, &temp);
                ReassignFloat(d, temp);
                break;
            case icln:
                if (left->v.i == 0)
                {
                    ASNFromRight(d);
                }
                break;
            case icrn:
                if (left->v.f.type == IFPF_IS_ZERO)
                {
                    ASNFromRight(d);
                }
                break;
            case icnl:
                if (right->v.i == 0)
                {
                    ASNFromLeft(d);
                }
                break;
            case icnr:
                if (right->v.f.type == IFPF_IS_ZERO)
                {
                    ASNFromLeft(d);
                }
                break;
            }
            break;
        case i_sub:
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icnone:
                if (d->dc.left == d->dc.right)
                {
                    ReassignInt(d,0);
                }
                break;
            case icll:
                ReassignInt(d,left->v.i - right->v.i);
                break;
            case iclr:
                temp = refloat(left);
                AddSubFPF(TRUE, &temp, &right->v.f, &temp);
                ReassignFloat(d, temp);
                break;
            case icrl:
                temp = refloat(right);
                AddSubFPF(TRUE, &left->v.f, &temp, &temp);
                ReassignFloat(d, temp);
                break;
            case icrr:
                AddSubFPF(TRUE, &left->v.f, &right->v.f, &temp);
                ReassignFloat(d, temp);
                break;
            case icln:
                if (left->v.i == 0)
                {
                    ASNFromLeft(d);
                }
                break;
            case icrn:
                if (left->v.f.type == IFPF_IS_ZERO)
                {
                }
                break;
            case icnl:
                if (right->v.i == 0)
                {
                }
                break;
            case icnr:
                if (right->v.f.type == IFPF_IS_ZERO)
                {
                }
                break;
            }
            break;
        case i_udiv:
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icll:
                if ((unsigned LLONG_TYPE)right->v.i != 0)
                    ReassignInt(d,(unsigned LLONG_TYPE)left->v.i / (unsigned LLONG_TYPE)right->v.i);
                break;
            case iclr:
                if (right->v.f.type != IFPF_IS_ZERO)
                {
                    temp = refloat(left);
                    DivideFPF(&temp, &right->v.f, &temp);
                    ReassignFloat(d, temp);
                }
                break;
            case icrl:
                if ((unsigned LLONG_TYPE)right->v.i != 0)
                {
                    temp = refloat(right);
                    DivideFPF(&left->v.f, &temp, &temp);
                    ReassignFloat(d, temp);
                }
                break;
            case icrr:
                if (right->v.f.type != IFPF_IS_ZERO)
                {
                    DivideFPF(&left->v.f, &right->v.f, &temp);
                    ReassignFloat(d, temp);
                }
                break;
            case icln:
                if ((unsigned LLONG_TYPE)left->v.i == 0)
                {
                    ReassignInt(d,0);
                }
                break;
            case icrn:
                if (left->v.f.type == IFPF_IS_ZERO)
                {
                    setFloatZero(d);
                }
                break;
            case icnl:
                if ((unsigned LLONG_TYPE)right->v.i == 1)
                {
                    ASNFromLeft(d);
                }
                else if ((shift = pwrof2((unsigned LLONG_TYPE)right->v.i)) !=  - 1)
                {
                    ReassignMulDiv(d,i_lsr,shift,FALSE);
                }
                break;
            case icnr:
                if (ValueIsOne(&right->v.f))
                {
                    ASNFromLeft(d);
                }
                break;
            }
            break;
        case i_umod:
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icnone:
                break;
            case icll:
                if ((unsigned LLONG_TYPE)right->v.i != 0)
                    ReassignInt(d,(unsigned LLONG_TYPE)left->v.i % (unsigned LLONG_TYPE)right->v.i);
                break;
            case icnl:
                if ((shift = calmask((unsigned LLONG_TYPE)right->v.i)) != -1)
                {
                    ReassignMulDiv(d,i_and,shift,FALSE);
                }
                break;
            default:
                badconst();
                break;
            }
            break;
        case i_sdiv:
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icll:
                if (right->v.i != 0)
                    ReassignInt(d,left->v.i / right->v.i);
                break;
            case iclr:
                if (right->v.f.type != IFPF_IS_ZERO)
                {
                    temp = refloat(left);
                    DivideFPF(&temp, &right->v.f, &temp);
                    ReassignFloat(d, temp);
                }
                break;
            case icrl:
                if (right->v.i != 0)
                {
                    temp = refloat(right);
                    DivideFPF(&left->v.f, &temp, &temp);
                    ReassignFloat(d, temp);
                }
                break;
            case icrr:
                if (right->v.f.type != IFPF_IS_ZERO)
                {
                    DivideFPF(&left->v.f, &right->v.f, &temp);
                    ReassignFloat(d, temp);
                }
                break;
            case icln:
                if (left->v.i == 0)
                {
                    ReassignInt(d,0);
                }
                break;
            case icrn:
                if (left->v.f.type == IFPF_IS_ZERO)
                {
                    setFloatZero(d);
                }
                break;
            case icnl:
                if (right->v.i == 1)
                {
                    ASNFromLeft(d);
                }
                else if ((shift = pwrof2(right->v.i)) !=  - 1)
                {
                    ReassignMulDiv(d,i_lsr,shift,FALSE);
                }
                break;
            case icnr:
                if (ValueIsOne(&right->v.f))
                {
                    ASNFromLeft(d);
                }
                break;
            }
            break;
        case i_smod:
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icnone:
                break;
            case icll:
                if (right->v.i != 0)
                    ReassignInt(d,left->v.i % right->v.i);
                break;
            case icln:
                if (left->v.i == 0)
                {
                    ReassignInt(d,0);
                }
                break;
            case icnl:
                if ((shift = calmask(right->v.i)) != -1)
                {
                    ReassignMulDiv(d,i_and,shift,FALSE);
                }
                break;
            default:
                badconst();
                break;
            }
            break;
//        case i_umul:
        case i_arrayindex:
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icll:
                ReassignInt(d,(unsigned LLONG_TYPE)left->v.i *(unsigned LLONG_TYPE)right->v.i);
                break;
            case iclr:
                temp = refloat(left);
                MultiplyFPF(&temp, &right->v.f, &temp);
                ReassignFloat(d, temp);
                break;
            case icrl:
                temp = refloat(right);
                MultiplyFPF(&left->v.f, &temp, &temp);
                ReassignFloat(d, temp);
                break;
            case icrr:
                MultiplyFPF(&left->v.f, &right->v.f, &temp);
                ReassignFloat(d, temp);
                break;
            case icln:
                if ((unsigned LLONG_TYPE)left->v.i == 0)
                {
                    ReassignInt(d,0);
                }
                else if ((unsigned LLONG_TYPE)left->v.i == 1)
                {
                    ASNFromRight(d);
                }
                else if ((shift = pwrof2((unsigned LLONG_TYPE)left->v.i)) !=  - 1)
                {
                    ReassignMulDiv(d,i_lsl,shift,TRUE);                }

                break;
            case icrn:
                if (left->v.f.type == IFPF_IS_ZERO)
                {
                    setFloatZero(d);
                }
                else if (ValueIsOne(&left->v.f))
                {
                    ASNFromRight(d);
                }
                break;
            case icnl:
                if ((unsigned LLONG_TYPE)right->v.i == 0)
                {
                    ReassignInt(d,0);
                }
                else if ((unsigned LLONG_TYPE)right->v.i == 1)
                {
                    ASNFromLeft(d);
                }
                else if ((shift = pwrof2((unsigned LLONG_TYPE)right->v.i)) !=  - 1)
                {
                    ReassignMulDiv(d,i_lsl,shift,FALSE);
                }
                break;
            case icnr:
                if (right->v.f.type == IFPF_IS_ZERO)
                {
                    setFloatZero(d);
                }
                else if (ValueIsOne(&right->v.f))
                {
                    ASNFromLeft(d);
                }
                break;
            }
            break;
        case i_mul:
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icll:
                ReassignInt(d,left->v.i * right->v.i);
                break;
            case iclr:
                temp = refloat(left);
                MultiplyFPF(&temp, &right->v.f, &temp);
                ReassignFloat(d, temp);
                break;
            case icrl:
                temp = refloat(right);
                MultiplyFPF(&left->v.f, &temp, &temp);
                ReassignFloat(d, temp);
                break;
            case icrr:
                MultiplyFPF(&left->v.f, &right->v.f, &temp);
                ReassignFloat(d, temp);
                break;
            case icln:
                if (left->v.i == 0)
                {
                    ReassignInt(d,0);
                }
                else if (left->v.i == 1)
                {
                    ASNFromRight(d);
                }
                else if ((shift = pwrof2(left->v.i)) !=  - 1)
                {
                    ReassignMulDiv(d,i_lsl,shift,TRUE);
                }

                break;
            case icrn:
                if (left->v.f.type == IFPF_IS_ZERO)
                {
                    setFloatZero(d);
                }
                else if (ValueIsOne(&left->v.f))
                {
                    ASNFromRight(d);
                }
                break;
            case icnl:
                if (right->v.i == 0)
                {
                    ReassignInt(d,0);
                }
                else if (right->v.i == 1)
                {
                    ASNFromLeft(d);
                }
                else if ((shift = pwrof2(right->v.i)) !=  - 1)
                {
                    ReassignMulDiv(d,i_lsl,shift,FALSE);
                }
                break;
            case icnr:
                if (right->v.f.type == IFPF_IS_ZERO)
                {
                    setFloatZero(d);
                }
                else if (ValueIsOne(&right->v.f))
                {
                    ASNFromLeft(d);
                }
                break;
            }
            break;
        case i_lsl:
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icnone:
                break;
            case icll:
                ReassignInt(d,(unsigned LLONG_TYPE)left->v.i << (unsigned LLONG_TYPE)right->v.i);
                break;
            case icln:
                if (left->v.i == 0)
                {
                    ReassignInt(d,0);
                }
                break;
            case icnl:
                if (right->v.i == 0)
                {
                    ASNFromLeft(d);
                }
                break;
            default:
                badconst();
                break;
            }
            break;
        case i_lsr:
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icnone:
                break;
            case icll:
                ReassignInt(d, (unsigned LLONG_TYPE)left->v.i >> (unsigned LLONG_TYPE)right->v.i);
                break;
            case icln:
                if (left->v.i == 0)
                {
                    ReassignInt(d,0);
                }
                break;
            case icnl:
                if (right->v.i == 0)
                {
                    ASNFromLeft(d);
                }
                break;
            default:
                badconst();
                break;
            }
            break;
//        case i_lsl:
        case i_arraylsh:
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icnone:
                break;
            case icll:
                ReassignInt(d, left->v.i << right->v.i);
                break;
            case icln:
                if (left->v.i == 0)
                {
                    ReassignInt(d,0);
                }
                break;
            case icnl:
                if (right->v.i == 0)
                {
                    ASNFromLeft(d);
                }
                break;
            default:
                badconst();
                break;
            }
            break;
        case i_asr:
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icnone:
                break;
            case icll:
                ReassignInt(d, left->v.i >> right->v.i);
                break;
            case icln:
                if (left->v.i == 0)
                {
                    ReassignInt(d,0);
                }
                break;
            case icnl:
                if (right->v.i == 0)
                {
                    ASNFromLeft(d);
                }
                break;
            default:
                badconst();
                break;
            }
            break;
        case i_and:
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icnone:
                break;
            case icll:
                ReassignInt(d,left->v.i & right->v.i);
                break;
            case icln:
                if (left->v.i == 0)
                {
                    ReassignInt(d,0);
                }
                break;
            case icnl:
                if (right->v.i == 0)
                {
                    ReassignInt(d,0);
                }
                break;
            default:
                badconst();
                break;
            }
            break;
        case i_or:
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icnone:
                break;
            case icll:
                ReassignInt(d, left->v.i | right->v.i);
                break;
            case icln:
                if (left->v.i == 0)
                {
                    ASNFromRight(d);
                }
                break;
            case icnl:
                if (right->v.i == 0)
                {
                    ASNFromLeft(d);
                }
                break;
            default:
                badconst();
                break;
            }
            break;
        case i_eor:
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icnone:
                break;
            case icll:
                ReassignInt(d, left->v.i ^ right->v.i);
                break;
            case icln:
                if (left->v.i == 0)
                {
                    ASNFromRight(d);
                }
                break;
            case icnl:
                if (right->v.i == 0)
                {
                    ASNFromLeft(d);
                }
                break;
            default:
                badconst();
                break;
            }
            break;
        case i_neg:
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icnone:
                break;
            case icln:
                ReassignInt(d,-left->v.i);
                break;
            case icrn:
                left->v.f.sign ^= 1;
                ReassignFloat(d,left->v.f);
                break;
            }
            break;
        case i_not:
            index = xgetmode(d, &left, &right);
            switch (index)
            {
            case icnone:
                break;
            case icln:
                ReassignInt(d,~left->v.i);
                break;
            }
            break;
        default:
            break;
    }
}
static BOOLEAN eval(QUAD *q)
{
    int tnum;
    int rv = FALSE;
    IMODE *val;
    enum vop set;
    if (q->dc.opcode == i_phi)
    {
        tnum = q->dc.v.phi->T0;
    }
    else
    {
        if (q->ans->mode != i_direct || q->ans->retval)
            return FALSE;
        tnum = q->ans->offset->v.sp->value.i;
    }
    set = tempInfo[tnum]->value.type;
    val = tempInfo[tnum]->value.imode;
    if (set == vo_bottom)
        return FALSE;
        
    switch(q->dc.opcode)
    {
        QUAD qn;
        case i_assn:
            if (q->dc.left->mode == i_immed)
            {
                if (isintconst(q->dc.left->offset))
                {
                    set = vo_constant;
                    if (isintconst(q->dc.left->offset))
                    {
                        val = Alloc(sizeof(IMODE));
                        *val = *q->dc.left;
                    }				
                    else
                    {
                        val = q->dc.left;
                    }
                    switch (q->dc.left->offset->type)
                    {
                        case en_label:
                        case en_pc:
                        case en_global:
                        case en_auto:
                        case en_absolute:
                        case en_threadlocal:
                            break;
                        default:
                            val->size = q->ans->size;
                            break;
                    }
                    break;
                }
                else
                    set = vo_bottom;
            }
            else if ((q->temps & TEMP_LEFT) && q->dc.left->mode == i_direct && (!q->ans || q->ans->size == q->dc.left->size))
            {
                int t = q->dc.left->offset->v.sp->value.i;
                if (tempInfo[t]->value.type == vo_constant)
                {
                    set = tempInfo[t]->value.type;
                    val = tempInfo[t]->value.imode;
                }
                else
                    set = tempInfo[t]->value.type;
            }
            else
                set = vo_bottom;
            break;
        case i_phi:
            {
                PHIDATA *pd = q->dc.v.phi;
                struct _phiblock *pb = pd->temps;
                if (tempInfo[pd->T0]->size != tempInfo[tnum]->size)
                    set = vo_bottom;
                else while (pb && set != vo_bottom)
                {
                    switch (tempInfo[pb->Tn]->value.type)
                    {
                        case vo_top:
                        case vo_bottom:
                            set = vo_bottom;
                            break;
                        case vo_constant:
                            if (set != vo_constant)
                            {
                                set = vo_constant;
                                val = tempInfo[pb->Tn]->value.imode;
                            }
                            else
                            {
                                if (tempInfo[pb->Tn]->value.imode->offset->v.i !=
                                    val->offset->v.i)
                                    set = vo_bottom;
                            }
                            break;
                    }				
                    pb = pb->next;
                }
            }
            break ;
        case i_add:
        case i_sub:
        case i_mul:
        case i_sdiv:
        case i_udiv:
        case i_neg:
        case i_not:
            qn = *q;
            if ((qn.temps & TEMP_LEFT) && qn.dc.left->mode == i_direct)
            {
                int t = q->dc.left->offset->v.sp->value.i;
                if (tempInfo[t]->value.type == vo_constant)
                {
                    if (tempInfo[t]->size == tempInfo[tnum]->size)
                    {
                        qn.dc.left = tempInfo[t]->value.imode;
                        qn.temps &= ~TEMP_LEFT;
                    }
                    else
                        set = vo_bottom;
                }
                else if (tempInfo[t]->value.type == vo_bottom)
                    set = vo_bottom;
            }
            else if (qn.dc.left->mode != i_immed || !isintconst(qn.dc.left->offset))
                set = vo_bottom;
            if ((qn.temps & TEMP_RIGHT) && qn.dc.right->mode == i_direct)
            {
                int t = q->dc.right->offset->v.sp->value.i;
                if (tempInfo[t]->value.type == vo_constant)
                {
                    if (tempInfo[t]->size == tempInfo[tnum]->size)
                    {
                        qn.dc.right = tempInfo[t]->value.imode;
                        qn.temps &= ~TEMP_RIGHT;
                    }
                    else
                        set = vo_bottom;
                }
                else if (tempInfo[t]->value.type == vo_bottom)
                    set = vo_bottom;
            }
            else if (qn.dc.right && (qn.dc.right->mode != i_immed || !isintconst(qn.dc.right->offset)))
                set = vo_bottom;
            if (set != vo_bottom)
            {
                ConstantFold(&qn, FALSE);
                if (qn.dc.opcode == i_assn)
                {
                    set = vo_constant;
                    val = qn.dc.left;
                }
            }
            break;
        default:
            break;
    }
    if (tempInfo[tnum]->value.type != set || set == vo_constant)
    {
        if (set == tempInfo[tnum]->value.type)
        {
            if (tempInfo[tnum]->value.imode->offset->v.i != val->offset->v.i)
            {
                /* evaluated to a different constant */
                tempInfo[tnum]->value.type = vo_bottom;
                rv = TRUE;
            }
        }
        else
        {
            tempInfo[tnum]->value.type = set;
            tempInfo[tnum]->value.imode = val;
            rv = TRUE;
        }
    }
    return rv;
}
static void pushBlock(BLOCK *block, BLOCK *source)
{
    BLOCKLIST *l1 ;
    BLOCKLIST **edgereached = &block->edgereached ,*bl;
    while (*edgereached && source->blocknum > (*edgereached)->block->blocknum)
    {
        edgereached = &(*edgereached)->next;
    }
    if (*edgereached && source == (*edgereached)->block)
        return;
    bl = tAlloc(sizeof(BLOCKLIST));
    bl->block = source;
    bl->next = *edgereached;
    *edgereached = bl;
    
    l1 = blockWorkHead;
    while (l1)
    {
        if (block == l1->block)
            break;
        l1 = l1->next;
    }
    if (!l1)
    {
        l1 = (BLOCKLIST*)listHolder;
        if (l1)
        {
            listHolder = listHolder->next;
        }
        else
        {
            l1 = tAlloc(sizeof(BLOCKLIST));
        }
        l1->block = block;
        l1->next = NULL;
        if (blockWorkHead)
            blockWorkTail = blockWorkTail->next = l1;
        else
            blockWorkHead = blockWorkTail = l1;
    }
}
static BOOLEAN evalBranch(QUAD *I, BLOCK *b)
{
    BOOLEAN found = FALSE;
    if (I->dc.left && I->dc.right)
    {
        QUAD qn ;
        BLOCKLIST *bl = b->succ;
        found = TRUE;
        switch(I->dc.opcode)
        {
            int mode;
            case i_jc:
            case i_ja:
            case i_jnc:
            case i_jbe:
            case i_jne:
            case i_je:
            case i_jl:
            case i_jg:
            case i_jle:
            case i_jge:
                bl = NULL;
                qn = *I;
                if ((qn.temps & TEMP_LEFT) && qn.dc.left->mode == i_direct)
                {
                    int t = qn.dc.left->offset->v.sp->value.i;
                    if (tempInfo[t]->value.type == vo_constant)
                    {
                        qn.dc.left = tempInfo[t]->value.imode;
                        qn.temps &= ~TEMP_LEFT;
                    }
                    else if (tempInfo[t]->value.type == vo_bottom)
                        break;
                }
                else if (qn.dc.left->mode != i_immed || !isintconst(qn.dc.left->offset))
                    break;
                if ((qn.temps & TEMP_RIGHT) && qn.dc.right->mode == i_direct)
                {
                    int t = qn.dc.right->offset->v.sp->value.i;
                    if (tempInfo[t]->value.type == vo_constant)
                    {
                        qn.dc.right = tempInfo[t]->value.imode;
                        qn.temps &= ~TEMP_RIGHT;
                    }
                    else if (tempInfo[t]->value.type == vo_bottom)
                        break;
                }
                else if (qn.dc.right->mode != i_immed || !isintconst(qn.dc.right->offset))
                    break;
                ConstantFold(&qn, FALSE);
                if (qn.dc.opcode == i_nop)
                {
                    bl = b->succ;
                }
                else if (qn.dc.opcode == i_goto)
                {
                    bl = b->succ->next;
                }
                break ;
            case i_goto:
                bl = b->succ;
                if (bl->next)
                    bl = bl->next;
                break;
            case i_coswitch:
                if (I->dc.left->mode == i_direct && (I->temps & TEMP_LEFT))
                {
                    int t = I->dc.left->offset->v.sp->value.i;
                    if (tempInfo[t]->value.type == vo_constant)
                    {
                        BLOCKLIST *ff = b->succ->next;
                        I = I->fwd;
                        while (I->dc.opcode != i_swbranch && I != b->tail)
                            I = I->fwd;
                        if (I == b->tail)
                            return FALSE;
                        while (I && ff && I->dc.left->offset->v.i 
                               != tempInfo[t]->value.imode->offset->v.i)
                        {
                            I = I->fwd;
                            ff = ff->next;
                        }
                        if (ff)
                        {
                            pushBlock(ff->block, b);
                        }
                        else
                        {
                            /* the default path */
                            pushBlock(b->succ->block, b);
                        }
                        return TRUE;
                    }
                }
                bl = NULL;
                break;
            default:
                found = FALSE;
                break;
        }
        if (found)
        {
            if (bl)
            {
                pushBlock(bl->block, b);
            }
            else
            {
                bl = b->succ;
                while (bl)
                {
                    pushBlock(bl->block, b);
                    bl = bl->next;
                }
            }
        }
    }
    return found;
}
static BOOLEAN emulInstruction(QUAD *head, BLOCK *b)
{
    if (((head->temps & TEMP_ANS) || head->dc.opcode == i_phi)
         && head->dc.opcode != i_coswitch)
    {
        if (eval(head))
        {
            int tnum ;
            INSTRUCTIONLIST *uses ;
            if (head->dc.opcode == i_phi)
                tnum = head->dc.v.phi->T0;
            else
                tnum = head->ans->offset->v.sp->value.i;
            uses = tempInfo[tnum]->instructionUses;
            while (uses)
            {
                INSTRUCTIONLIST *l1 = insWorkHead;
                while (l1)
                {
                    if (l1->ins == uses->ins)
                        break;
                    l1 = l1->next;
                }
                if (!l1)
                {
                    l1 = listHolder;
                    if (l1)
                    {
                        listHolder = listHolder->next;
                    }
                    else
                    {
                        l1 = tAlloc(sizeof(INSTRUCTIONLIST));
                    }
                    l1->ins = uses->ins;
                    l1->next = NULL;
                    if (insWorkHead)
                        insWorkTail = insWorkTail->next = l1;
                    else
                        insWorkHead = insWorkTail = l1;
                }
                uses = uses->next;
            }
        }
        return FALSE;
    }
    else 
        return evalBranch(head, b);
}
static void emulBlock(BLOCK *b)
{
    QUAD *head = b->head->fwd;
    BOOLEAN br = FALSE;
    while (head && (head->ignoreMe || head->dc.opcode == i_label))
        head = head->fwd;
    while (head->dc.opcode == i_phi)
    {
        emulInstruction(head, b);
        head = head->fwd;
    }
    if (!briggsTest(visited, b->blocknum))
    {
        briggsSet(visited, b->blocknum);
        while (head != b->tail->fwd)
        {
            br |= emulInstruction(head, b);
            head = head->fwd;
        }
        if (!br)
        {
            if (b->succ)
            {
                pushBlock(b->succ->block, b);
            }
        }
    }
}
static void iterateMark(int n)
{
    INSTRUCTIONLIST *uses = tempInfo[n]->instructionUses;
    while (uses)
    {
        if ((uses->ins->temps & TEMP_ANS) && uses->ins->ans->mode == i_direct)
        {
            if (!uses->ins->ans->offset->v.sp->pushedtotemp)
            {
                int t = uses->ins->ans->offset->v.sp->value.i;
                tempInfo[t]->preSSATemp = -1;
                iterateMark(t);
            }
        }
        uses = uses->next;
    }
}
static void iterateConstants(void)
{
    int i;
    for (i=0; i < tempCount; i++)
    {
        if (tempInfo[i]->value.type == vo_constant)
        {
            INSTRUCTIONLIST *uses = tempInfo[i]->instructionUses;
//			QUAD *defines = tempInfo[i]->instructionDefines;
            while (uses)
            {
                
                if (uses->ins->dc.opcode != i_assn && 
                    (uses->ins->temps & TEMP_LEFT) && 
                    uses->ins->dc.left->mode == i_direct)
                {
                    int t = uses->ins->dc.left->offset->v.sp->value.i;
                    if (t == i)
                    {
                        uses->ins->dc.left =tempInfo[i]->value.imode;
                        uses->ins->temps &= ~TEMP_LEFT;
                        iterateMark(t);
                        ConstantFold(uses->ins, TRUE);
                    }
                }
                if ((uses->ins->temps & TEMP_RIGHT) && 
                    uses->ins->dc.left->mode == i_direct)
                {
                    int t = uses->ins->dc.right->offset->v.sp->value.i;
                    if (t == i)
                    {
                        uses->ins->dc.right =tempInfo[i]->value.imode;
                        uses->ins->temps &= ~TEMP_RIGHT;
                        iterateMark(t);
                        ConstantFold(uses->ins, TRUE);
                    }
                }
                uses = uses->next;
            }
//			tempInfo[i]->instructionUses = NULL;
//			if (defines == tempInfo[i]->blockDefines->tail)
//			{
//				tempInfo[i]->blockDefines->tail = defines->back;
//			}
            /* we don't remove the definition because it may be used
            * if it isnt the dead code analysis will get it later
            */
       }
    }
}
static void removePhiEntry(BLOCK *b, int n)
{
    QUAD *q = b->head;
    while ((q->dc.opcode == i_block || q->ignoreMe || q->dc.opcode == i_label) && q->back != b->tail)
        q = q->fwd;
    while (q->dc.opcode == i_phi && q->back != b->tail)
    {
        PHIDATA *pd = q->dc.v.phi;
        struct _phiblock **pb = &pd->temps;
        if (*pb)
        {
            int j = n;
            while (j)
            {
                j--, pb = &(*pb)->next;
            }
            *pb = (*pb)->next;
        }
        pd->nblocks--;
        q = q->fwd;
    }
}
static void removeForward(BLOCK *start)
{
    QUAD *tail = start->tail;
        
    while (tail)
    {
        switch(tail->dc.opcode)
        {
            BLOCKLIST **erasel;
            int con;
            case i_jc:
            case i_ja:
            case i_jnc:
            case i_jbe:
            case i_jne:
            case i_je:
            case i_jl:
            case i_jg:
            case i_jle:
            case i_jge:
                if ((tail->temps & TEMP_LEFT) && tail->dc.left->mode == i_direct)
                {
                    int t = tail->dc.left->offset->v.sp->value.i;
                    if (tempInfo[t]->value.type == vo_constant)
                    {
                        tail->dc.left = tempInfo[t]->value.imode;
                        tail->temps &= ~TEMP_LEFT;
                    }
                }
                if ((tail->temps & TEMP_RIGHT) && tail->dc.right->mode == i_direct)
                {
                    int t = tail->dc.right->offset->v.sp->value.i;
                    if (tempInfo[t]->value.type == vo_constant)
                    {
                        tail->dc.right = tempInfo[t]->value.imode;
                        tail->temps &= ~TEMP_RIGHT;
                    }
                }
                ConstantFold(tail, TRUE);
                if (tail->dc.opcode == i_nop)
                {
                    RemoveInstruction(tail);
                }
                break;
            case i_goto:
                return;
            case i_coswitch:
                if (tail->dc.left->mode == i_immed)
                {
                    con = tail->dc.left->offset->v.i;
                }
                else if ((tail->temps & TEMP_LEFT) && tail->dc.left->mode == i_direct)
                {
                    int t = tail->dc.left->offset->v.sp->value.i;
                    if (tempInfo[t]->value.type == vo_constant)
                    {
                        con = tail->dc.left->offset->v.i;
                    }
                    else
                        break;
                }
                else
                    break;
                {
                    BLOCK *b = NULL;
                    BLOCKLIST **succ = &tail->block->succ->next;
                    if (*succ)
                    {
                        QUAD *find = tail->fwd;
                        while (find && find->dc.opcode == i_swbranch)
                        {
                            if (find->dc.left->offset->v.i == con)
                            {
                                tail->dc.v.label = find->dc.v.label;
                                succ = &(*succ)->next;
                                b = tail->block;
                            }
                            else
                            {
                                BLOCKLIST *bl1 = (*succ)->block->succ;
                                while (bl1)
                                {
                                    BLOCKLIST **bl = &bl1->block->pred;
                                    int n = 0;
                                    while (*bl)
                                    {
                                        if ((*bl)->block == (*succ)->block)
                                        {
                                            (*bl) = (*bl)->next;
                                            removePhiEntry(bl1->block, n);
                                            break;
                                        }
                                        n++;
                                        bl = &(*bl)->next;
                                    }
                                    bl1 = bl1->next;
                                }
                                (*succ)->block->pred = NULL;
                                (*succ)= (*succ)->next;
                            }
                            find = find->fwd;
                        }
                    }
                    tail->dc.opcode = i_goto; /* normally this branch will be
                                               * to next and will itself
                                               * be optimized away later
                                               * we also will let the dead code elimination
                                               * clear out the garbage...
                                               */
                                
                    tail->ans = tail->dc.left = tail->dc.right = NULL;
                    tail->temps = 0;
                }
                return;
            default:
                break;	
        }
        if (tail == start->head)
            tail = NULL;
        else
            tail = tail->back;
    }
/*	diag("can't find branch");	 */
}
static void removeBlock(BLOCK *block)
{
    QUAD *head = block->head;
    while (head)
    {
        switch(head->dc.opcode)
        {
            case i_label:
            case i_line:
            case i_block:
                break;
            default:
                RemoveInstruction(head);
                head = head->back;
                break;
        }
        if (head == block->tail)
            head = NULL;
        else
            head = head->fwd;
    }
}
/* propagate constants as far as possible, and remove dead blocks */
extern QUAD *intermed_head;
void ConstantFlow(void)
{
    int i;
    QUAD *head;
    visited = briggsAlloc(blockCount);
    insWorkHead = insWorkTail = NULL;
    listHolder = NULL;
    blockWorkHead = blockWorkTail = tAlloc(sizeof(BLOCKLIST));

    /* value defaults to top */
    /* now reassign any temp which is a parameter to bottom */
    for (i=0; i < tempCount; i++)
    {
        if (tempInfo[i]->enode)
        {
            SYMBOL *orig = (SYMBOL *)tempInfo[i]->enode->right;
            if (orig && orig->storage_class == sc_parameter)
            {
                tempInfo[i]->value.type = vo_bottom;
            }
        }
    }
    /* the edge transitions are false by default */
    /* look for volatile values */
    head = blockArray[0]->head;
    while (head)
    {
    
        if (head->temps & TEMP_ANS)
        {
            if (head->ans->mode == i_direct)
            {
                if (head->temps & TEMP_LEFT)
                {
                    if (head->dc.left->vol)
                    {
                        tempInfo[head->ans->offset->v.sp->value.i]->value.type = vo_bottom;
                    }
                }
                if (head->temps & TEMP_RIGHT)
                {
                    if (head->dc.right->vol)
                    {
                        tempInfo[head->ans->offset->v.sp->value.i]->value.type = vo_bottom;
                    }
                }
            }
        }
        head = head->fwd;
    }
    blockWorkHead->block = blockArray[0]; /* first block */
    
    /* just keep evaluating instructions and blocks
     * until there is nothing left to evaluate
     */
    while (insWorkHead || blockWorkHead)
    {
        while (insWorkHead)
        {
            INSTRUCTIONLIST *l = insWorkHead ;
            insWorkHead = insWorkHead->next;
            l->next = listHolder;
            listHolder = l;
            emulInstruction(l->ins, l->ins->block);
        }
        while (blockWorkHead)
        {
            BLOCKLIST *l = blockWorkHead ;
            blockWorkHead = blockWorkHead->next;
            l->next = (BLOCKLIST *)listHolder;
            listHolder = (INSTRUCTIONLIST *)l;
            emulBlock(l->block);
        }
    }
    iterateConstants();
    for (i=0; i < blockCount; i++)
        if (blockArray[i])
            removeForward(blockArray[i]);
    tFree();
}