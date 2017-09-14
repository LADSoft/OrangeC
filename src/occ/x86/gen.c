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
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "be.h"

extern OCODE *peep_tail ;
extern int startlab, retlab;
extern int usingEsp;

#define MAX_ALIGNS 50
int pushlevel;
int funcstackheight;

static int floatpop;
static int fstackid;
static int inframe;
static int switch_deflab;
static LLONG_TYPE switch_range, switch_case_count, switch_case_max;
static IMODE *switch_ip;
static enum {swm_enumerate, swm_compactstart, swm_compact, swm_tree} switch_mode;
static int switch_lastcase;
static AMODE *switch_apl, *switch_aph;
static int switch_live;
static int *switchTreeLabels, *switchTreeBranchLabels ;
static LLONG_TYPE *switchTreeCases;
static int switchTreeLabelCount;
static int switchTreePos;
/*static int floatArea; */

/* map the icode version of the regs to the processor version */
extern int regmap[REG_MAX][2];

//-------------------------------------------------------------------------

AMODE *make_muldivval(AMODE *ap)
{
    AMODE *ap1 = make_label(queue_muldivval(ap->offset->v.i));
    ap1->mode = am_direct;
    return (ap1);
}


/*-------------------------------------------------------------------------*/

AMODE *make_label(int lab)
/*
 *      construct a reference node for an internal label number.
 */
{
    EXPRESSION *lnode;
    AMODE *ap;
    lnode = beLocalAlloc(sizeof(EXPRESSION));
    lnode->type = en_labcon;
    lnode->v.i = lab;
    ap = beLocalAlloc(sizeof(AMODE));
    ap->mode = am_immed;
    ap->offset = lnode;
    ap->length = ISZ_UINT;
    return ap;
}

/*-------------------------------------------------------------------------*/

AMODE *makesegreg(int seg)
{
    AMODE *ap = beLocalAlloc(sizeof(AMODE));
    ap->mode = am_seg;
    ap->seg = seg;
    ap->length = ISZ_SEG;
    return ap;
}
AMODE *makefloat(FPF *f, int size)
{
        AMODE *ap = make_label(queue_floatval(f, size));
        ap->mode = am_direct;
        ap->length = size;
        return ap;
}

/*-------------------------------------------------------------------------*/

AMODE *aimmed(ULLONG_TYPE i)
/*
 *      make a node to reference an immediate value i.
 */
{
    AMODE *ap;
    EXPRESSION *ep;
    i &= 0xffffffffU;
    ep = beLocalAlloc(sizeof(EXPRESSION));
    ep->type = en_c_i;
    ep->v.i = i;
    ap = beLocalAlloc(sizeof(AMODE));
    ap->mode = am_immed;
    ap->offset = ep;
    ap->length = ISZ_ADDR;
    return ap;
}

/*-------------------------------------------------------------------------*/

AMODE *aimmedt(long i, int size)
/*
 *      make a node to reference an immediate value i.
 */
{
    AMODE *ap;
    switch (size)
    {
        case ISZ_UCHAR:
            case  - ISZ_UCHAR: i &= 0xff;
            break;
        case ISZ_U16:
        case ISZ_USHORT:
            case  - ISZ_USHORT: i &= 0xffff;
            break;
            case ISZ_ADDR:
        case ISZ_UINT :
        case ISZ_ULONG :
        case ISZ_U32:
            case - ISZ_ADDR:
            case  - ISZ_UINT :
            case - ISZ_ULONG:  i &= 0xffffffffU;
            break;
    }
    ap = aimmed(i);
    ap->length = size ;
    return ap;
}

/*-------------------------------------------------------------------------*/
BOOLEAN isauto(EXPRESSION *ep)
{
    if (ep->type == en_auto)
        return TRUE;
    if (ep->type == en_add || ep->type == en_structadd)
        return isauto(ep->left) || isauto(ep->right);
    if (ep->type == en_sub)
        return isauto(ep->left);
    return FALSE;
}
AMODE *make_offset(EXPRESSION *node)
/*
 *      make a direct reference to a node.
 */
{
    AMODE *ap;
    ap = beLocalAlloc(sizeof(AMODE));
    if (node->type == en_tempref)
    {
        diag("make_offset: orignode");
    }
    if (isauto(node) )
    {
        if (usingEsp)
        {
            ap->preg = ESP;
        }
        else
        {
            ap->preg = EBP;
        }
        ap->mode = am_indisp;
    }
    else
        ap->mode = am_direct;
    ap->offset = node;
    ap->seg = 0;
    return ap;
}
/*-------------------------------------------------------------------------*/

AMODE *make_stack(int number)
{
    AMODE *ap = beLocalAlloc(sizeof(AMODE));
    EXPRESSION *ep = beLocalAlloc(sizeof(EXPRESSION));
    ep->type = en_c_i;
    ep->v.i =  - number;
    ap->mode = am_indisp;
    ap->preg = ESP;
    ap->offset = ep;
    ap->length = ISZ_UINT;
    ap->keepesp = TRUE;
    return (ap);
}
AMODE *fstack(void)
{
    AMODE *ap = beLocalAlloc(sizeof(AMODE));
    ap->mode = am_freg;
    ap->length = ISZ_LDOUBLE;
    ap->preg = 0;
    ap->sreg = 0;
    ap->offset = (void *)fstackid;
    ap->tempflag = TRUE;
    return (ap);
}
static void callLibrary(char *name, int size)
{
    /* so it will get in the external list */
    IMODE *ip = beSetProcSymbol(name);
    AMODE *an = (AMODE *)beLocalAlloc(sizeof(AMODE));
    an->offset = ip->offset ;
    an->mode = am_immed;
    gen_code(op_call, an, 0);
    if (size)
        gen_code(op_add, makedreg(ESP), aimmed(size));
}
void oa_gen_vtt(VTABENTRY *vt, SYMBOL *func)
{
    AMODE *ofs = make_offset(varNode(en_pc, func));
    ofs->mode = am_immed;
    gen_code(op_add, make_stack(-4), aimmedt(-vt->dataOffset, ISZ_ADDR));
    gen_code(op_jmp, ofs , NULL);
    flush_peep(NULL, NULL);    
}
void oa_gen_vc1(SYMBOL *func)
{
    AMODE *ofs = makedreg(EAX);
    ofs->offset = intNode(en_c_i, 0);
    ofs->mode = am_indisp;
    gen_code(op_mov, makedreg(EAX), make_stack(-4));
    gen_code(op_mov, makedreg(EAX), ofs);
    ofs->offset = intNode(en_c_i, func->offset);
    gen_code(op_jmp, ofs, NULL);
    flush_peep(NULL, NULL);
}
void oa_gen_importThunk(SYMBOL *func)
{
    AMODE *ofs = (AMODE *)Alloc(sizeof(AMODE));
    ofs->mode = am_direct;
    ofs->offset = varNode(en_pc, func->mainsym);
    gen_code(op_jmp, ofs, NULL);
    flush_peep(NULL, NULL);
}
void make_floatconst(AMODE *ap)
{
    int size = ap->length;
    if (isintconst(ap->offset))
    {
        LongLongToFPF(&ap->offset->v.f, ap->offset->v.i);
        ap->offset->type = en_c_d;
        size = ISZ_DOUBLE;
    }
    else
    {
        switch (ap->offset->type)
        {
            case en_c_f:
                size = ISZ_FLOAT;
                break;
            case en_c_d:
                size = ISZ_DOUBLE;
                break;
            case en_c_ld:
                size = ISZ_LDOUBLE;
                break;
            case en_c_fi:
                size = ISZ_IFLOAT;
                break;
            case en_c_di:
                size = ISZ_IDOUBLE;
                break;
            case en_c_ldi:
                size = ISZ_ILDOUBLE;
                break;
            case en_c_fc:
                size = ISZ_CFLOAT;
                break;
            case en_c_dc:
                size = ISZ_CDOUBLE;
                break;
            case en_c_ldc:
                size = ISZ_CLDOUBLE;
                break;
            default:
                break;
        }
    }
    if (ValueIsOne(&ap->offset->v.f))
    {
        ap->mode = am_fconst;
        if (ap->offset->v.f.sign)
            ap->preg = fcmone;
        else
            ap->preg = fcone;
            
    }
    else if (ap->offset->v.f.type == IFPF_IS_ZERO)
    {
        ap->mode = am_fconst;
        ap->preg = fczero;
    }
    else
    {
        AMODE *ap1 = make_label(queue_floatval(&ap->offset->v.f, size));
        ap->mode = am_direct;
        ap->length = size;
        ap->offset = ap1->offset;
    }
}

void make_complexconst(AMODE *ap, AMODE *api)
{
    AMODE *apt ;
    if (isintconst(ap->offset))
    {
        api->offset = exprNode(en_c_d, 0, 0); /* defaults to zero. 0 */
    }
    else
    {
        switch (ap->offset->type)
        {
            case en_c_f:
                api->offset = exprNode(en_c_f, 0, 0); /* defaults to zero. 0 */
                break;
            case en_c_d:
                api->offset = exprNode(en_c_d, 0, 0); /* defaults to zero. 0 */
                break;
            case en_c_ld:
                api->offset = exprNode(en_c_ld, 0, 0); /* defaults to zero. 0 */
                break;
            case en_c_fi:
                api->offset = exprNode(en_c_fi, 0, 0); /* defaults to zero. 0 */
                apt = api;
                api = ap;
                ap = apt;
                break;
            case en_c_di:
                api->offset = exprNode(en_c_di, 0, 0); /* defaults to zero. 0 */
                apt = api;
                api = ap;
                ap = apt;
                break;
            case en_c_ldi:
                api->offset = exprNode(en_c_ldi, 0, 0); /* defaults to zero. 0 */
                apt = api;
                api = ap;
                ap = apt;
                break;
            case en_c_fc:
                api->offset = exprNode(en_c_fc, 0, 0); /* defaults to zero. 0 */
                api->offset->v.f = ap->offset->v.c.i;
                ap->offset->type = en_c_f;
                ap->offset->v.f = ap->offset->v.c.r;
                break;
            case en_c_dc:
                api->offset = exprNode(en_c_dc, 0, 0); /* defaults to zero. 0 */
                api->offset->v.f = ap->offset->v.c.i;
                ap->offset->type = en_c_f;
                ap->offset->v.f = ap->offset->v.c.r;
                break;
            case en_c_ldc:
                api->offset = exprNode(en_c_ldc, 0, 0); /* defaults to zero. 0 */
                api->offset->v.f = ap->offset->v.c.i;
                ap->offset->type = en_c_f;
                ap->offset->v.f = ap->offset->v.c.r;
                break;
            default:
                break;
        }
    }
    make_floatconst(ap);
    make_floatconst(api);

}
void floatStore(AMODE *ap, int sz)
{
    if (ap->mode != am_freg)
        gen_codes(op_fstp, sz, ap, 0);
}
void complexStore(AMODE *ap, AMODE *api, int sz)
{
    floatStore(ap, sz - ISZ_CFLOAT + ISZ_FLOAT);
    floatStore(api, sz - ISZ_CFLOAT + ISZ_FLOAT);
}
void floatLoad(AMODE *ap, int sz, int okind)
{
    if (ap->mode != am_freg)
    {
        if (ap->mode == am_fconst)
        {
            if (ap->preg == fczero)
            {
                gen_code(op_fldz, 0, 0);
                if (ap->offset->v.f.sign)
                    gen_code(op_fchs, 0, 0);
            }
            else
            {
                gen_code(op_fld1, 0, 0);
                if (ap->preg == fcmone)
                    gen_code(op_fchs, 0, 0);
            }
        }
        else if (!okind || sz == ISZ_LDOUBLE)
        {
            gen_codes(op_fld, sz, ap, 0);
        }
    }
}
BOOLEAN sameTemp(QUAD *head);
void float_gen(QUAD *q, AMODE *apll, AMODE *aprl, int op, int rop, int pop, int prop)
{
    if (sameTemp(q))
    {
        if (apll->mode != am_freg)
            floatLoad(apll, q->dc.left->size, FALSE);
        gen_code(op_fld, makefreg(0), NULL);
        gen_codes(pop, ISZ_LDOUBLE, makefreg(1), 0);
    }
    else
    {
        BOOLEAN doleft = FALSE;
        if (apll->mode != am_freg && aprl->mode != am_freg)
        {
            floatLoad(apll, q->dc.left->size, FALSE);
            doleft = TRUE;
        }
        if (doleft || apll->mode == am_freg)
        {
            switch (aprl->mode)
            {
                default:
                    if (q->dc.right->size != ISZ_LDOUBLE)
                    {
                        gen_codes(op, q->dc.right->size, aprl, 0);
                        break;
                    }
                    // fall through
                case am_fconst:
                    floatLoad(aprl, q->dc.left->size, FALSE);
                    /* fall through */
                case am_freg:
                    gen_codes(pop, ISZ_LDOUBLE, makefreg(1), 0);
                    break;
            }
        }
        else
        {
            switch (apll->mode)
            {
                default:
                    if (q->dc.left->size != ISZ_LDOUBLE)
                    {
                        gen_codes(rop, q->dc.left->size, apll, 0);
                        break;
                    }
                    // fall through
                case am_fconst:
                    floatLoad(apll, q->dc.left->size, FALSE);
                    gen_codes(prop, ISZ_LDOUBLE, makefreg(1), 0);
                    break;
                case am_freg:
                    /* can't get here */
                    break;
            }
        }
    }
}

void complexLoad(AMODE *ap, AMODE *api, int sz)
{
    floatLoad(api, sz - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
    floatLoad(ap, sz - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
}
int beRegFromTempInd(QUAD *q, IMODE *im, int which)
{
    if (im == q->ans)
    {
        if (which)
        {
            if (q->scaleColor < 0)
                return 0;
            return q->scaleColor;
        }
        else
        {
            if (q->ansColor < 0)
                return 0;
            return q->ansColor;
        }
    }
    else if (im == q->dc.left)
    {
        if (which)
        {
            if (q->scaleColor < 0)
                return 0;
            return q->scaleColor;
        }
        else
        {
            if (q->leftColor < 0)
                return 0;
            return q->leftColor;
        }
    }
    else
    {
        if (which)
        {
            if (q->scaleColor < 0)
                return 0;
            return q->scaleColor;
        }
        else
        {
            if (q->rightColor < 0)
                return 0;
            return q->rightColor;
        }
    }
}
int beRegFromTemp(QUAD *q, IMODE *im)
{
    return beRegFromTempInd(q, im, 0);
}
BOOLEAN sameTemp(QUAD *head)
{
    if  ((head->temps & (TEMP_LEFT | TEMP_RIGHT)) == (TEMP_LEFT | TEMP_RIGHT))
    {
        if (head->dc.left->mode == i_direct && head->dc.right->mode == i_direct)
        {
            if (head->dc.left->offset->v.sp->value.i == head->dc.right->offset->v.sp->value.i)
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}
int imaginary_offset(int sz)
{
    int size;
    switch (sz)
    {
        case ISZ_CFLOAT:
            size = 4;
            break ;
        case ISZ_CDOUBLE:
            size = 8;
            break ;
        default:
        case ISZ_CLDOUBLE:
            size = 10;
            break ;
    }
    return size;
}

int samereg(AMODE *ap1, AMODE *ap2)
{
    switch(ap1->mode)
    {
        case am_indisp:
        case am_dreg:
            switch(ap2->mode)
            {
                case am_indisp:
                case am_dreg:
                    return ap1->preg == ap2->preg;
                case am_indispscale:
                    return ap1->preg == ap2->preg || ap1->preg == ap2->sreg;
                default:
                    break;
            }
            break;
        case am_indispscale:
            switch(ap2->mode)
            {
                case am_indispscale:
                    if (ap2->preg == ap1->preg || ap2->preg == ap1->sreg)
                        return TRUE;
                    /* fall through */
                case am_indisp:
                case am_dreg:
                    return ap1->preg == ap2->preg || ap1->preg == ap2->sreg;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return FALSE;
}
void getAmodes(QUAD *q, enum e_op *op, IMODE *im, AMODE **apl, AMODE **aph)
{
    *op = op_mov;
    *aph = 0;
    if (im->offset && im->offset->type == en_threadlocal)
    {
        AMODE *temp = (AMODE *)beLocalAlloc(sizeof(AMODE));
        IMODE *ip = beSetProcSymbol("__TLSINITSTART");
        EXPRESSION *exp ;
        exp = exprNode(en_sub, im->offset, ip->offset);
        temp->offset = exp;
        temp->mode = am_immed;
        gen_code(op_push, temp, 0);
        callLibrary("___tlsaddr", 0);
        *apl = (AMODE *)beLocalAlloc(sizeof(AMODE));
        (*apl)->preg = regmap[beRegFromTempInd(q, im, 0)][0];
        (*apl)->mode = am_dreg;
        gen_codes(op_pop, ISZ_ADDR, (*apl), 0);
    }
    else if (im->mode == i_ind)
    {
        enum e_am mode;
        if ((im->offset && im->offset2) || (im->offset2 && im->scale))  
            mode = am_indispscale;
        else if (im->offset || im->offset2)
            mode = am_indisp;
        else
            mode = am_direct;
        *apl = (AMODE *)beLocalAlloc(sizeof(AMODE));
        
        if (im->offset)
        {
            (*apl)->preg = regmap[beRegFromTempInd(q, im, 0)][0];
            if (im->offset2)
                (*apl)->sreg = regmap[beRegFromTempInd(q, im, 1)][0];
            else
                (*apl)->sreg = -1;
        }
        else if (mode == am_indisp && im->offset2)
        {
            if (im->offset2)
                (*apl)->preg = regmap[beRegFromTempInd(q, im, 1)][0];
            else
                (*apl)->preg = -1;
            (*apl)->sreg = -1;
        }
        else
        {
            (*apl)->preg = -1;
            if (im->offset2)
                (*apl)->sreg = regmap[beRegFromTempInd(q, im, 1)][0];
            else
                (*apl)->sreg = -1;
        }
        (*apl)->scale = im->scale;
        (*apl)->offset = im->offset3 ? im->offset3 : intNode(en_c_i, 0);
        (*apl)->length = im->size;
        if (im->offset3)
        {
            if (isauto(im->offset3))
            {
                if ((*apl)->preg == -1)
                {
                    if ((*apl)->sreg == -1)
                        mode = am_indisp;
                    else
                        mode = am_indispscale;
                    if (usingEsp)
                    {
                        (*apl)->preg = ESP;
                    }
                    else
                    {
                        (*apl)->preg = EBP;
                    }
                }
                else if ((*apl)->sreg == -1)
                {
                    mode = am_indispscale;
                    if (usingEsp)
                    {
                        (*apl)->sreg = (*apl)->preg;
                        (*apl)->preg = ESP;
                    }
                    else
                    {
                        (*apl)->sreg = EBP;
                    }
                }
                else
                    diag("getAmodes: no room for EBP in ind processing");
            }
        }
        (*apl)->mode = mode;
        if (im->size >= ISZ_CFLOAT)
        {
            *aph = (AMODE *)beLocalAlloc(sizeof(AMODE));
            **aph = **apl;
            (*aph)->offset = exprNode(en_add, (*apl)->offset, intNode(en_c_i,imaginary_offset(im->size)));
            if ((*apl)->preg >= 0)
                (*apl)->liveRegs |= 1 << (*apl)->preg;
            if ((*apl)->sreg >= 0)
                (*apl)->liveRegs |= 1 << (*apl)->sreg;
        }
        else if (im->size == ISZ_ULONGLONG || im->size == -ISZ_ULONGLONG)
        {
            *aph = (AMODE *)beLocalAlloc(sizeof(AMODE));
            **aph = **apl;
            (*aph)->offset = exprNode(en_add, (*apl)->offset, intNode(en_c_i,4));
            if ((*apl)->preg >= 0)
                (*apl)->liveRegs |= 1 << (*apl)->preg;
            if ((*apl)->sreg >= 0)
                (*apl)->liveRegs |= 1 << (*apl)->sreg;
        }
    }
    else if (im->mode == i_immed)
    {
        if (im->size >= ISZ_CFLOAT)
        {
            *apl = (AMODE *)beLocalAlloc(sizeof(AMODE));
            *aph = (AMODE *)beLocalAlloc(sizeof(AMODE));
            (*apl)->offset = im->offset;			
            make_complexconst(*apl, *aph);
            
        }
        else if (im->size >= ISZ_FLOAT)
        {
            *apl = (AMODE *)beLocalAlloc(sizeof(AMODE));
            (*apl)->offset = im->offset;			
            make_floatconst(*apl);
        }
        else if (im->size == ISZ_ULONGLONG || im->size == - ISZ_ULONGLONG)
        {
            *apl = aimmed(im->offset->v.i);
#ifdef USE_LONGLONG
            *aph = aimmed((im->offset->v.i >> 32));
#else
            if (im->size < 0 && im->offset->v.i < 0)
                *aph = aimmed(-1);
            else
                *aph = aimmed(0);
#endif
        }
        else
        {
            if (iscomplexconst(im->offset))
            {
                *apl = (AMODE *)beLocalAlloc(sizeof(AMODE));
                (*apl)->offset = im->offset;			
                *aph = (AMODE *)beLocalAlloc(sizeof(AMODE));
                make_complexconst(*apl, *aph);
            }
            else if (isfloatconst(im->offset) || isimaginaryconst(im->offset))
            {
                *apl = (AMODE *)beLocalAlloc(sizeof(AMODE));
                (*apl)->offset = im->offset;			
                make_floatconst(*apl);
            }
            else
            {
                if (!isintconst(im->offset))
                {
                    *apl = make_offset(im->offset);
                    if ((*apl)->mode == am_indisp)
                        *op = op_lea;
                    else
                        (*apl)->mode = am_immed;
                    (*apl)->length = im->size;
                }
                else if (im->offset->type == en_c_ll || im->offset->type == en_c_ull)
                {
                    *apl = aimmed(im->offset->v.i);
#ifdef USE_LONGLONG
                    *aph = aimmed((im->offset->v.i >> 32));
#else
                    if (im->size < 0 && im->offset->v.i < 0)
                        *aph = aimmed(-1);
                    else
                        *aph = aimmed(0);
#endif
                }
                else
                {
                    *apl = make_offset(im->offset);
                    if ((*apl)->mode == am_indisp)
                        *op = op_lea;
                    else
                    {
                        (*apl)->offset->v.i &= 0xffffffffU;
                        (*apl)->mode = am_immed;
                    }
                    // in case of a int const being used with a long long
#ifdef USE_LONGLONG
                    *aph = aimmed((im->offset->v.i >> 32));
#else
                    if (im->size < 0 && im->offset->v.i < 0)
                        *aph = aimmed(-1);
                    else
                        *aph = aimmed(0);
#endif
                        
                }
            }
        }
    }
    else if (im->mode == i_direct)
    {
    /*
        if (im->offset->type == en_reg)
        {
            *apl = makedreg(im->offset->v.i);
        }
        else
    */ 
         if (im->size >= ISZ_FLOAT)
        {
            if (im->size >= ISZ_CFLOAT)
            {
                if (im->offset->type == en_tempref && !im->offset->right)
                {
                    *aph = fstack();
                    *apl = fstack();
                }
                else
                {
                    *apl = make_offset(im->offset);
                    *aph = make_offset(im->offset);
                    (*aph)->offset = exprNode(en_add, (*aph)->offset, intNode(en_c_i, imaginary_offset(im->size)));
                    
                }
            }
            else if (im->offset->type == en_tempref && !im->offset->right)
            {
                *apl = fstack();
            }
            else
                *apl = make_offset(im->offset);
        }
        else if (im->size == ISZ_ULONGLONG || im->size == - ISZ_ULONGLONG)
        {
            int i;
            if (im->offset->type == en_tempref)
            {
                int clr = beRegFromTemp(q, im);
                int reg1, reg2;
                reg1 = regmap[clr & 0xff][1];
                reg2 = regmap[clr & 0xff][0];
                *apl = makedreg(reg2);
                *aph = makedreg(reg1);
                (*apl)->liveRegs = (*aph)->liveRegs = (1 << reg1) 
                        | (1 << reg2);
            }
            else
            {
                *apl = make_offset(im->offset);
                *aph = copy_addr(*apl);
                (*aph)->offset = exprNode(en_add, (*aph)->offset, intNode(en_c_i, 4));
            }
            
        }
        else if (im->offset->type == en_tempref)
        {
            int l = regmap[beRegFromTemp(q, im)][0];
            *apl = makedreg(l);
            (*apl)->length = im->size;
        }
        else
        {
            *apl = make_offset(im->offset);
            (*apl)->length = im->size;
        }
    }
    if (!(*aph))
        if ((*apl)->liveRegs == -1)
            (*apl)->liveRegs = 0;
    (*apl)->liveRegs |= q->liveRegs;
    if (*aph)
        (*aph)->liveRegs |= q->liveRegs;
}
void bit_store(AMODE *dest, AMODE *src, int size, int bits, int startbit)
{	
    if (src->mode == am_immed && isintconst(src->offset))
    {
        if (bits == 1)
        {
            if (src->offset->v.i & 1)
                gen_codes(op_bts, ISZ_UINT, dest, aimmed(startbit));
            else 
                gen_codes(op_btr, ISZ_UINT, dest, aimmed(startbit));
        }
        else
        {
            int l = dest->liveRegs;
            if (dest->mode == am_indisp && dest->preg != EBP)
                dest->liveRegs |= 1 << dest->preg;
            if (bits != 32)
            {
                gen_codes(op_and, size, dest, 
                          aimmed(~(((1 << bits) -1) << startbit)));
            }
            dest->liveRegs = l;
            gen_codes(op_or, size, dest, 
                      aimmed((src->offset->v.i & ((1 << bits) -1)) << startbit));
        }
    }
    else
    {
        if (bits != 32)
        {
            gen_codes(op_and, size, src, aimmed((1 << bits) -1));
            gen_codes(op_and, size, dest, aimmed(~(((1 << bits) -1) << startbit)));
            if (startbit)
                gen_codes(op_shl, size, src, aimmed(startbit));
            gen_codes(op_or, size, dest, src);
        }
        else
            gen_codes(op_mov, size, dest, src);
    }
}
void liveQualify(AMODE *reg, AMODE *left, AMODE *right)
{
    if (left)
    {
        if (left->mode == am_indisp)
        {
            reg->liveRegs |= 1 << left->preg;
        
        }
        else if (left->mode == am_indispscale)
        {
            reg->liveRegs |= 1 << left->preg;
            reg->liveRegs |= 1 << left->sreg;
        }
    }
    if (right)
    {
        if (right->mode == am_indisp)
        {
            reg->liveRegs |= 1 << right->preg;
        
        }
        else if (right->mode == am_indispscale)
        {
            reg->liveRegs |= 1 << right->preg;
            reg->liveRegs |= 1 << right->sreg;
        }
    }
}
BOOLEAN overlaps(AMODE *apal, AMODE *apah, AMODE* apll, AMODE *aplh)
{
    BOOLEAN overlap = FALSE;
    if (apll->mode == am_dreg || apll->mode == am_indisp || apll->mode == am_indispscale)
        overlap = overlap || apll->preg == apal->preg || apll->preg == apah->preg;
    if (apll->mode == am_dreg)
        overlap = overlap || aplh->preg == apal->preg || aplh->preg == apah->preg;
    if (apll->mode == am_indispscale)
        overlap = overlap || apll->sreg == apal->preg || apll->sreg == apah->preg;
    return overlap;
}
void func_axdx(enum e_op func, AMODE *apal, AMODE *apah, AMODE *apll, AMODE *aplh)
{
    enum e_op func1 = func;
    liveQualify(apal, apal, apah);
    liveQualify(apah, apal, apah);
    if (apll->mode == am_immed && !aplh)
    {
        if (isintconst(apll->offset))
        {
            aplh = aimmedt(apll->offset->v.i >> 32, ISZ_UINT);
            apll->offset->v.i &= 0xffffffff;
        }
        else
            aplh = aimmedt(0, ISZ_UINT);
    }
    if (func == op_add || func == op_sub)
    {
        func1 = func == op_add ? op_adc : op_sbb;
    }
    if (apal->mode == am_dreg)
    {
        BOOLEAN overlap = (apll->mode == am_indisp && apll->preg == apal->preg) ||
                (apll->mode == am_indispscale && (apll->preg == apal->preg || apll->sreg == apal->preg));
        if (overlap)
        {
            if (func == op_add || func == op_sub)
            {
                gen_codes(func, ISZ_UINT, apah, aplh);
                gen_codes(func, ISZ_UINT, apal, apll);
                gen_codes(func1, ISZ_UINT, apah, aimmed(0));
            }
            else
            {
                gen_codes(func1, ISZ_UINT, apah, aplh);
                gen_codes(func, ISZ_UINT, apal, apll);
            }
            return ;
        }
    }
    if (func == op_mov)
    {
        if (equal_address(apal, aplh))
        {
            if (equal_address(apll, apah))
            {
                /* just in case, reg allocator shouldn't do this though */
                gen_codes(op_xchg, ISZ_UINT, apal, apah);
            }
            else
            {
                gen_codes(func1, ISZ_UINT, apah, aplh);
                gen_codes(func, ISZ_UINT, apal, apll);
            }
        }
        else
        {
            gen_codes(func, ISZ_UINT, apal, apll);
            gen_codes(func1, ISZ_UINT, apah, aplh);
        }
    }
    else
    {
        gen_codes(func, ISZ_UINT, apal, apll);
        gen_codes(func1, ISZ_UINT, apah, aplh);
    }
}
void func_axdx_mov(enum e_op op, AMODE *apal, AMODE *apah, AMODE *apll, AMODE *aplh, AMODE *aprl, AMODE *aprh)
{
    if (overlaps(apal, apah, aprl, aprh))
    {
        func_axdx(op_mov, apal, apah, aprl, aprh);
        func_axdx(op, apal, apah, apll, aplh);
        if (op == op_sub)
        {
            gen_codes(op_neg, ISZ_UINT, apah, 0);
            gen_codes(op_neg, ISZ_UINT, apal, 0);
            gen_codes(op_sbb, ISZ_UINT, apah, aimmed(0));			
        }
    }
    else
    {
        func_axdx(op_mov, apal, apah, apll, aplh);
        func_axdx(op, apal, apah, aprl, aprh);
    }
}
void gen_lshift(enum e_op op, AMODE *aph, AMODE *apl, AMODE *n)
{
    if (apl->mode != am_dreg || aph->mode != am_dreg)
    {
        int reg = -1;
        BOOLEAN pushed = FALSE;
        if (!(apl->liveRegs & (1 << EAX)))
        {
            reg = EAX;
        }
        else if (!(apl->liveRegs & (1 << ECX)))
        {
            reg = ECX;
        }
        else if (!(apl->liveRegs & (1 << EDX)))
        {
            reg = EDX;
        }
        else
        {
            gen_code(op_push, makedreg(EAX), NULL);
            pushlevel += 4;
            pushed = TRUE;
            reg = EAX;
        }
        if (op == op_shrd)
        {
            gen_code(op_shr, apl, n);
            gen_code(op_mov, makedreg(reg), aph);
            gen_code(op_ror, makedreg(reg), n);
            gen_code(op_and, makedreg(reg), aimmed(-1 >> (n->offset->v.i)));
            gen_code(op_or, apl, makedreg(reg));
        }
        else
        {
            gen_code(op_shl, aph, n);
            gen_code(op_mov, makedreg(reg), apl);
            gen_code(op_rol, makedreg(reg), n);
            gen_code(op_and, makedreg(reg), aimmed((1 << (n->offset->v.i)) - 1));
            gen_code(op_or, aph, makedreg(reg));
        }
        if (pushed)
        {
            gen_code(op_pop, makedreg(EAX), NULL);
            pushlevel -= 4;
        }
    }
    else
    {
        gen_code3(op, aph, apl, n);
    }
}
void gen_xset(QUAD *q, enum e_op pos, enum e_op neg, enum e_op flt)
{
    enum e_op op = pos, opa;
    IMODE *left = q->dc.left;
    IMODE *right = q->dc.right;
    AMODE *apll, *aplh, *aprl, *aprh, *apal, *apah;
    BOOLEAN assign = FALSE;
    BOOLEAN stacked = FALSE;
    AMODE *altreg;
    if (left->mode == i_immed && left->size < ISZ_FLOAT)
    {
        IMODE *t = right ;
        right = left ;
        left = t ;
        op = neg;
    }
    if (left->bits && right->mode == i_immed && isintconst(right->offset))
    {
        if (left->bits != 1)
            diag("gen_xset: too many bits");
        getAmodes(q, &opa, left, &apll, &aplh);
        getAmodes(q, &opa, q->ans, &apal, &apah);
        gen_codes(op_bt, ISZ_UINT, apll, aimmed(q->dc.left->startbit));
        if (op == op_sete)
        {
            if (right->offset->v.i & 1)
            {
                op = op_setc;
            }
            else
            {
                op = op_setnc;
            }
        }
        else if (op == op_setne)
        {
            if (right->offset->v.i & 1)
            {
                op = op_setnc;
            }
            else
            {
                op = op_setc;
            }
        }
        else
            diag("gen_xset: Unknown bit type");
        if (apal->length != ISZ_UCHAR && apal->length != -ISZ_UCHAR)
            gen_codes(op_mov, ISZ_UINT, apal, aimmed(0));
        gen_codes(op, ISZ_UCHAR, apal, 0);
        return;
    }
    if (left->size >= ISZ_FLOAT)
    {
        int ulbl = beGetLabel;
        if (left->mode == i_direct && left->offset->type == en_tempref)
        {
            if (right->offset && right->offset->type != en_tempref)
            {
                switch(flt)
                {
                    case op_seta:
                        flt = op_setb;
                        break;
                    case op_setae:
                        flt = op_setbe;
                        break;
                    case op_setb:
                        flt = op_seta;
                        break;
                    case op_setbe:
                        flt = op_setae;
                        break;
                    default:
                        break;
                }
            }
        }
        getAmodes(q, &opa, right, &aprl, &aprh);
        floatLoad(aprl, left->size, FALSE);
        getAmodes(q, &opa, left, &apll, &aplh);
        if (sameTemp(q))
        {
            gen_code(op_fld, makefreg(0), NULL);
            gen_code(op_fucompp, 0, 0);
        }
        else
        {
            floatLoad(apll, left->size, FALSE);
//			if (apll->mode == am_freg)
            {
                gen_code(op_fucompp, 0, 0);
            }
//			else
//			{
//				gen_codes(op_fcomp, left->size, apll, 0);
//			}
        }
        if (apll->liveRegs & (1 << EAX))
            gen_code(op_push, makedreg(EAX), 0);
        gen_codes(op_fstsw, ISZ_USHORT, makedreg(EAX), 0);
        gen_codes(op_bt, ISZ_UINT, makedreg(EAX), aimmed(10));
        gen_code(op_jnc, make_label(ulbl), 0);
        gen_codes(op_btr, ISZ_UINT, makedreg(EAX), aimmed(14));
        if (flt == op_ja || flt == op_jae ||
            flt == op_seta || flt == op_setae)
            gen_codes(op_bts, ISZ_UINT, makedreg(EAX), aimmed(8));
        if (flt == op_jb || flt == op_jbe ||
            flt == op_setb || flt == op_setbe)
            gen_codes(op_btr, ISZ_UINT, makedreg(EAX), aimmed(8));
        oa_gen_label(ulbl);
        gen_code(op_sahf, 0, 0);
        if (apll->liveRegs & (1 << EAX))
            gen_code(op_pop, makedreg(EAX), 0);
        getAmodes(q, &opa, q->ans, &apal, &apah);
        gen_codes(flt, ISZ_UCHAR, apal, 0);
        gen_codes(op_and, ISZ_UINT, apal, aimmed(1));
        return;
    }
    getAmodes(q, &opa, left, &apll, &aplh);
    getAmodes(q, &opa, right, &aprl, &aprh);
    getAmodes(q, &opa, q->ans, &apal, &apah);
    if (apal->preg > EBX)
    {
        int i;
        for (i=0; i < 4; i++)
        {
            if (!live(apal->liveRegs, i))
            {
                altreg = makedreg(i);
                assign = TRUE;
                break;
            }
        }
        if (!assign)
        {
            altreg = make_stack(0);
            stacked = TRUE;
        }
    }
    if (left->size == ISZ_ULONGLONG || left->size == -ISZ_ULONGLONG)
    {
        int lab = beGetLabel;
        int sop = op;
        switch(op)
        {
            case op_setg:
                sop = op_seta;
                break;
            case op_setge:
                sop = op_setnc;
                break;
            case op_setl:
                sop = op_setc;
                break;
            case op_setle:
                sop = op_setbe;
                break;
            default:
                break;
        }
        if (op == sop)
            
        {
            gen_codes(op_cmp, ISZ_UINT, aplh, aprh);
            peep_tail->oper1->liveRegs = -1;
            gen_branch(op_jne, lab);
            gen_codes(op_cmp, ISZ_UINT, apll, aprl);
            peep_tail->oper1->liveRegs = -1;
            oa_gen_label(lab);		
            if (assign)
            {
                gen_codes(op, ISZ_UCHAR, altreg, 0);
                gen_codes(op_and, ISZ_UINT, altreg, aimmed(1));
                gen_codes(op_mov, ISZ_UINT, apal, altreg);
            }
            else if (stacked)
            {
                gen_code(op_push, aimmed(0), NULL);
                gen_codes(op, ISZ_UCHAR, altreg, 0);
                gen_code(op_pop, apal, NULL);
            }
            else
            {
                gen_codes(op, ISZ_UCHAR, apal, 0);
                gen_codes(op_and, ISZ_UINT, apal, aimmed(1));
            }
        }
        else
        {
            int lab2 = beGetLabel;
            gen_codes(op_cmp, ISZ_UINT, aplh, aprh);
            peep_tail->oper1->liveRegs = -1;
            gen_branch(op_jne, lab);
            gen_codes(op_cmp, ISZ_UINT, apll, aprl);
            peep_tail->oper1->liveRegs = -1;
            if (assign)
            {
                gen_codes(sop, ISZ_UCHAR, altreg, 0);
                if (apal->length != ISZ_UCHAR && apal->length != -ISZ_UCHAR)
                    gen_codes(op_and, ISZ_UINT, altreg, aimmed(1));
                gen_codes(op_mov, ISZ_UINT, apal, altreg);
            }
            else if (stacked)
            {
                gen_code(op_push, aimmed(0), NULL);
                gen_codes(sop, ISZ_UCHAR, altreg, 0);
                gen_code(op_pop, apal, NULL);
            }
            else
            {
                gen_codes(sop, ISZ_UCHAR, apal, 0);
                if (apal->length != ISZ_UCHAR && apal->length != -ISZ_UCHAR)
                    gen_codes(op_and, ISZ_UINT, apal, aimmed(1));
            }
            gen_branch(op_jmp, lab2);
            oa_gen_label(lab);		
            if (assign)
            {
                gen_codes(op, ISZ_UCHAR, altreg, 0);
                if (apal->length != ISZ_UCHAR && apal->length != -ISZ_UCHAR)
                    gen_codes(op_and, ISZ_UINT, altreg, aimmed(1));
                gen_codes(op_mov, ISZ_UINT, apal, altreg);
            }
            else if (stacked)
            {
                gen_code(op_push, aimmed(0), NULL);
                gen_codes(op, ISZ_UCHAR, altreg, 0);
                gen_code(op_pop, apal, NULL);
            }
            else
            {
                gen_codes(op, ISZ_UCHAR, apal, 0);
                if (apal->length != ISZ_UCHAR && apal->length != -ISZ_UCHAR)
                    gen_codes(op_and, ISZ_UINT, apal, aimmed(1));
            }
            oa_gen_label(lab2);
        }
        return;
    }
    else
    {
        gen_codes(op_cmp, left->size, apll, aprl);
        peep_tail->oper1->liveRegs = q->liveRegs;
    }
    if (assign)
    {
        gen_codes(op, ISZ_UCHAR, altreg, 0);
        if (apal->length != ISZ_UCHAR && apal->length != -ISZ_UCHAR)
            gen_codes(op_and, ISZ_UINT, altreg, aimmed(1));
        gen_codes(op_mov, ISZ_UINT, apal, altreg);
    }
    else if (stacked)
    {
        gen_code(op_push, aimmed(0), NULL);
        gen_codes(op, ISZ_UCHAR, altreg, 0);
        gen_code(op_pop, apal, NULL);
    }
    else
    {
        gen_codes(op, ISZ_UCHAR, apal, 0);
        if (apal->length != ISZ_UCHAR && apal->length != -ISZ_UCHAR)
            gen_codes(op_and, ISZ_UINT, apal, aimmed(1));
    }
}
void gen_goto(QUAD* q, enum e_op pos, enum e_op neg, enum e_op llpos, enum e_op llneg, enum e_op llinterm, enum e_op flt)
{
    enum e_op sop = pos, sop1 = llpos, top = llneg, opa;
    IMODE *left = q->dc.left;
    IMODE *right = q->dc.right;
    AMODE *apll, *aplh, *aprl, *aprh;
    if (left->mode == i_immed && left->size < ISZ_FLOAT)
    {
        IMODE *t = right ;
        right = left ;
        left = t ;
        if (pos != op_je && pos != op_jne)
        {
            sop = neg;
            sop1 = llneg;
            top = llpos;
        }
    }
    if (left->bits && right->mode == i_immed && isintconst(right->offset))
    {
        if (left->bits != 1)
            diag("gen_goto: too many bits");
        getAmodes(q, &opa, left, &apll, &aplh);
        gen_codes(op_bt, ISZ_UINT, apll, aimmed(q->dc.left->startbit));
        if (sop == op_je)
        {
            if (right->offset->v.i & 1)
            {
                sop = op_jc;
            }
            else
            {
                sop = op_jnc;
            }
        }
        else if (sop == op_jne)
        {
            if (right->offset->v.i & 1)
            {
                sop = op_jnc;
            }
            else
            {
                sop = op_jc;
            }
        }
        else
            diag("gen_goto: Unknown bit type");
        gen_branch(sop, q->dc.v.label);
        return;
    }
    if (left->size >= ISZ_FLOAT)
    {
        int ulbl = beGetLabel;
        if (left->mode == i_direct && left->offset->type == en_tempref)
        {
            if (right->offset && right->offset->type != en_tempref)
            {
                switch(flt)
                {
                    case op_ja:
                        flt = op_jb;
                        break;
                    case op_jae:
                        flt = op_jbe;
                        break;
                    case op_jb:
                        flt = op_ja;
                        break;
                    case op_jbe:
                        flt = op_jae;
                        break;
                    default:
                        break;
                }
            }
        }
        getAmodes(q, &opa, right, &aprl, &aprh);
        floatLoad(aprl, left->size, FALSE);
        getAmodes(q, &opa, left, &apll, &aplh);
        if (sameTemp(q))
        {
            gen_code(op_fld, makefreg(0), NULL);
            gen_code(op_fucompp, 0, 0);
        }
        else
        {
            floatLoad(apll, left->size, FALSE);
//			if (apll->mode == am_freg)
            {
                gen_code(op_fucompp, 0, 0);
            }
//			else
//			{
//				gen_codes(op_fcomp, left->size, apll, 0);
//			}
        }
        if (apll->liveRegs & (1 << EAX))
            gen_code(op_push, makedreg(EAX), 0);
        gen_codes(op_fstsw, ISZ_USHORT, makedreg(EAX), 0);
        gen_codes(op_bt, ISZ_UINT, makedreg(EAX), aimmed(10));
        gen_code(op_jnc, make_label(ulbl), 0);
        gen_codes(op_btr, ISZ_UINT, makedreg(EAX), aimmed(14));
        if (flt == op_ja || flt == op_jae)
            gen_codes(op_btr, ISZ_UINT, makedreg(EAX), aimmed(8));
        if (flt == op_jb || flt == op_jbe)
            gen_codes(op_bts, ISZ_UINT, makedreg(EAX), aimmed(8));		
        oa_gen_label(ulbl);
        gen_code(op_sahf, 0, 0);
        if (apll->liveRegs & (1 << EAX))
            gen_code(op_pop, makedreg(EAX), 0);
        gen_branch(flt, q->dc.v.label);
    }
    else 
    {
        getAmodes(q, &opa, left, &apll, &aplh);
        getAmodes(q, &opa, right, &aprl, &aprh);
        if (left->size == ISZ_ULONGLONG || left->size == -ISZ_ULONGLONG)
        {
            int lab = beGetLabel;
            sop = llinterm;
            gen_codes(op_cmp, ISZ_UINT, aplh, aprh);
            peep_tail->oper1->liveRegs = -1;
            if (top != op_jne)
                gen_branch(sop1, q->dc.v.label);
            if (top != op_je)
                gen_branch(top, lab);
            gen_codes(op_cmp, ISZ_UINT, apll, aprl);
            peep_tail->oper1->liveRegs = -1;
            gen_branch(sop, q->dc.v.label);
            oa_gen_label(lab);		
        }
        else
        {
            int size ;
//			if (isintconst(left->offset))
//				size = right->size;
//			else
                size = left->size;
            gen_codes(op_cmp, size, apll, aprl);
            peep_tail->oper1->liveRegs = q->liveRegs;       
            gen_branch(sop, q->dc.v.label);
        }
    }
}
static void gen_div(QUAD *q, enum e_op op)               /* unsigned division */
{
    enum e_op opa, opl, opr;
    AMODE *apal, *apah, *apll, *aplh, *aprl, *aprh;
    int mod = q->dc.opcode == i_umod || q->dc.opcode == i_smod;
    getAmodes(q, &opl, q->dc.left, &apll, &aplh);
    getAmodes(q, &opr, q->dc.right, &aprl, &aprh);
    getAmodes(q, &opa, q->ans, &apal, &apah);
    if (q->ans->size >= ISZ_FLOAT)
    {
        if (mod)
        {
            diag("gen_div: floating point in mod operation");
        }
        float_gen(q, apll, aprl, op_fdiv, op_fdivr, op_fdivp, op_fdivrp);
        floatStore(apal, q->ans->size);
    }
    else if (q->ans->size == ISZ_ULONGLONG || q->ans->size == -ISZ_ULONGLONG)
    {
        diag("gen_div: long long");
    }
    else
    {
        AMODE *divby = aprl;
        if (apal->mode != am_dreg)
            diag("asm_udiv: answer not a dreg");
        if (aprl->mode == am_immed)
        {
            divby = make_muldivval(aprl);
        }
        if (op == op_div)
        {
            gen_code(op_mov, makedreg(EDX), aimmed(0));
        }
        else
        {
            gen_code(op_cdq, 0, 0);
        }
        divby->liveRegs = q->liveRegs;
        gen_codes(op, q->ans->size, divby, 0);
    }
}
static void gen_mulxh(QUAD *q, enum e_op op)               /* unsigned division */
{
    enum e_op opa, opl, opr;
    AMODE *apal, *apah, *apll, *aplh, *aprl, *aprh;
    int mod = q->dc.opcode == i_umod || q->dc.opcode == i_smod;
    getAmodes(q, &opl, q->dc.left, &apll, &aplh);
    getAmodes(q, &opr, q->dc.right, &aprl, &aprh);
    getAmodes(q, &opa, q->ans, &apal, &apah);
    if (q->ans->size >= ISZ_FLOAT)
    {
        diag("gen_mulxh: floating point");
    }
    else if (q->ans->size == ISZ_ULONGLONG || q->ans->size == -ISZ_ULONGLONG)
    {
        diag("gen_mulxh: long long");
    }
    else
    {
        AMODE *mulby = aprl;
        if (apal->mode != am_dreg)
            diag("asm_mulxh: answer not a dreg");
        if (apll->mode == am_immed)
        {
            gen_codes(op_mov, q->ans->size, makedreg(EAX), apll);
        }
        if (aprl->mode == am_immed)
        {
            mulby = make_muldivval(aprl);
        }
        mulby->liveRegs = q->liveRegs;
        gen_codes(op, q->ans->size, mulby, 0);
    }
}
static void gen_shift(QUAD *q, enum e_op op, AMODE *apal, AMODE *apll, AMODE *aprl)
{
    AMODE *cx = makedreg(ECX);
    cx->liveRegs = q->liveRegs;
    cx->length = ISZ_UCHAR;
    if (aprl->mode == am_immed)
    {
        gen_code(op_mov, apal, apll);
        gen_code(op, apal, aprl);
    }
    else if (equal_address(apal, aprl))
    {
        if (aprl->mode == am_dreg && aprl->preg == ECX)
        {
            if (apll->mode == am_immed)
            {
                gen_codes(op_push, ISZ_UINT, apll, NULL);
                gen_code(op, make_stack(0), cx);
                gen_codes(op_pop, ISZ_UINT, cx, NULL);
            }
            else
            {
                BOOLEAN pushed = FALSE;
                if (apll->mode != am_dreg || live(apal->liveRegs, apll->preg))
                {
                    pushed = TRUE;
                    gen_codes(op_push, ISZ_UINT, apll, NULL);
                    pushlevel += 4;
                }
                gen_code(op, apll, cx);
                gen_codes(op_mov, ISZ_UINT, apal, apll);
                if (pushed)
                {
                    gen_codes(op_pop, ISZ_UINT, apll, NULL);
                    pushlevel -= 4;
                }
            }
        }
        else if (apll->mode == am_dreg && apll->preg == ECX)
        {
            gen_codes(op_xchg, ISZ_UINT, apll, aprl);
            gen_code(op, aprl, cx);
            gen_codes(op_xchg, ISZ_UINT, apll, aprl);
        }
        else
        {
            if (apll->mode == am_immed)
            {
                gen_codes(op_xchg, ISZ_UINT, aprl, cx);
                gen_codes(op_push, ISZ_UINT, apll, NULL);
                gen_code(op, make_stack(0), cx);
                gen_codes(op_pop, ISZ_UINT, cx, NULL);
                gen_codes(op_xchg, ISZ_UINT, aprl, cx);
            }
            else
            {
                BOOLEAN pushed = FALSE;
                if (apll->mode != am_dreg || live(apal->liveRegs, apll->preg))
                {
                    pushed = TRUE;
                    gen_codes(op_push, ISZ_UINT, apll, NULL);
                    pushlevel += 4;
                }
                gen_codes(op_xchg, ISZ_UINT, aprl, cx);
                gen_code(op, apll, cx);
                gen_codes(op_xchg, ISZ_UINT, aprl, cx);
                gen_codes(op_mov, ISZ_UINT, apal, apll);
                if (pushed)
                {
                    gen_codes(op_pop, ISZ_UINT, apll, NULL);
                    pushlevel -= 4;
                }
            }
        }
    }
    else
    {
        if (aprl->mode == am_dreg && aprl->preg == ECX)
        {
            if (apal->mode == am_dreg && apal->preg == ECX)
            {
                if (apll->mode != am_dreg || (apll->liveRegs & (1 << apll->preg)))
                {
                    gen_code(op_push, apll, NULL);
                    pushlevel += 4;
                    gen_code(op, apll, cx);
                    gen_codes(op_mov, ISZ_UINT, apal, apll);
                    gen_code(op_pop, apll, NULL);
                    pushlevel -= 4;
                }
                else
                {
                    gen_code(op, apll, cx);
                    gen_codes(op_mov, ISZ_UINT, apal, apll);
                }
            }
            else
            {
                gen_codes(op_mov, ISZ_UINT, apal, apll);
                gen_code(op, apal, cx);
            }
        }
        else if (apal->mode == am_dreg && apal->preg == ECX)
        {
            gen_codes(op_push, ISZ_UINT, apll, NULL);			
            pushlevel += 4;
            gen_codes(op_mov, ISZ_UINT, cx, aprl);
            gen_code(op, make_stack(0), cx);
            gen_codes(op_pop, ISZ_UINT, cx, NULL);
            pushlevel -= 4;
        }
        else 
        {
            gen_codes(op_xchg, ISZ_UINT, aprl, cx);
            gen_codes(op_mov, ISZ_UINT, apal, apll);
            gen_code(op, apal, cx);
            gen_codes(op_xchg, ISZ_UINT, aprl, cx);
        }
    }
}
static void	do_movzx(int dsize, int ssize, AMODE *dest, AMODE *src)
{
        gen_code2(op_movzx,dsize, ssize, dest, src);
    return;
    
    if (!equal_address(dest, src))
    {
        gen_code2(op_movzx,dsize, ssize, dest, src);
//		gen_code(op_xor, dest, dest);
//		gen_codes(op_mov, ssize, dest, src);
    }
    else
    {
        AMODE *am;
        if (ssize == ISZ_UCHAR)
            am = aimmed(0xff);
        else
            am = aimmed(0xffff);
        gen_codes(op_and, ISZ_UINT, dest, am);
    }
}
static void	do_movsx(int dsize, int ssize, AMODE *dest, AMODE *src)
{
    if (dest->preg == EAX)
    {
        if (!equal_address(dest, src))
        {
            gen_codes(op_mov, ssize, dest, src);
        }
        if (ssize == ISZ_UCHAR)
            gen_code(op_cbw, NULL, NULL);
        gen_code(op_cwde, NULL, NULL);
    }
    else
    {
        gen_code2(op_movsx, dsize, ssize, dest, src);
    }
}
static void compactSwitchHeader(LLONG_TYPE bottom)
{
    int tablab, size;
    AMODE *ap;
    EXPRESSION *lnode;
    tablab = beGetLabel;
    size = switch_ip->size;
    if (size == ISZ_ULONGLONG || size ==  - ISZ_ULONGLONG)
    {
        if (bottom)
        {
            int golab = beGetLabel;
#ifdef USE_LONGLONG
            gen_codes(op_cmp, ISZ_UINT, switch_aph, aimmed(bottom >>
                32));
#else 
            gen_codes(op_cmp, ISZ_UINT, switch_aph, aimmed((switch_ip->size < 9 && bottom < 0)
                ?  - 1: 0));
#endif 
            if (size < 0)
            {
                gen_branch(op_jl, switch_deflab);
                gen_branch(op_jg, golab);
            }
            else
            {
                peep_tail->noopt = TRUE;
                gen_branch(op_jb, switch_deflab);
                gen_branch(op_ja, golab);
            }
            gen_codes(op_cmp, ISZ_UINT, switch_apl, aimmed(bottom));
            if (size < 0)
                gen_branch(op_jl, switch_deflab);
            else
            {
                peep_tail->noopt = TRUE;
                gen_branch(op_jb, switch_deflab);
            }
            oa_gen_label(golab);
        }
        else
        if (size < 0)
        {
            if (switch_aph->mode == am_dreg)
                gen_codes(op_and, ISZ_UINT, switch_aph, switch_aph);
            else
                gen_codes(op_test, ISZ_UINT, switch_aph, aimmed(-1));
            gen_branch(op_jl, switch_deflab);
        }
        gen_codes(op_cmp, ISZ_UINT, switch_apl, aimmed(switch_range + bottom)
            );
    }
    else
    {
        if (bottom)
        {
            gen_codes(op_cmp, switch_ip->size, switch_apl, aimmed(bottom));
            if (size < 0)
                gen_branch(op_jl, switch_deflab);
            else
            {
                peep_tail->noopt = TRUE;
                gen_branch(op_jb, switch_deflab);
            }
        }
        else
        if (size < 0)
        {
            if (switch_apl->mode == am_dreg)
                gen_codes(op_and, switch_ip->size, switch_apl, switch_apl);
            else
                gen_codes(op_test, switch_ip->size, switch_apl, aimmed(-1));
            gen_branch(op_jl, switch_deflab);
        }
        gen_codes(op_cmp, switch_ip->size, switch_apl, aimmed(switch_range + bottom));
    }
    if (size < 0)
        gen_branch(op_jge, switch_deflab);
    else
        gen_branch(op_jnc, switch_deflab);
    gen_codes(op_push, ISZ_UINT, switch_apl, 0);
    pushlevel += 4;
    switch (switch_ip->size)
        {
            case ISZ_USHORT:
                do_movzx(ISZ_UINT, ISZ_USHORT, switch_apl, switch_apl);
                break;
            case -ISZ_USHORT:
                do_movsx(ISZ_UINT, ISZ_USHORT, switch_apl, switch_apl);
                break;
            case ISZ_UCHAR:
                do_movzx(ISZ_UINT, ISZ_UCHAR, switch_apl, switch_apl);
                break;
            case ISZ_BOOLEAN:
                gen_codes(op_and, ISZ_UINT, switch_apl, aimmed(1));
                break;
            case -ISZ_UCHAR:
                do_movsx(ISZ_UINT, ISZ_UCHAR, switch_apl, switch_apl);
                break;
            default:
                break;
        }

    peep_tail->noopt = TRUE;
    lnode = beLocalAlloc(sizeof(EXPRESSION));
    lnode->type = en_labcon;
    lnode->v.i = tablab;
    if (bottom)
    {
        lnode = exprNode(en_add, lnode, intNode(en_c_i, -bottom * 4));
    }
    ap = beLocalAlloc(sizeof(AMODE));
    ap->mode = am_indispscale;
    ap->preg = -1;
    ap->scale = 2;
    ap->sreg = switch_apl->preg;
    ap->offset = lnode;
    gen_codes(op_mov, ISZ_UINT, switch_apl, ap);
    gen_codes(op_xchg, ISZ_UINT, switch_apl, make_stack(0));
    gen_codes(op_ret,0,0,0);
    pushlevel -= 4;
    oa_align(4);
    oa_gen_label(tablab);
}
//-------------------------------------------------------------------------

void bingen(int lower, int avg, int higher)
{
    AMODE *ap2 = aimmed(switchTreeCases[avg]);
    if (switchTreeBranchLabels[avg] !=  0)
        oa_gen_label(switchTreeBranchLabels[avg]);
    gen_coden(op_cmp, switch_apl->length, switch_apl, ap2);
    gen_branch(op_je, switchTreeLabels[avg]);
    peep_tail->oper1->liveRegs = switch_live;
    if (avg == lower)
    {
        gen_branch(op_jmp, switch_deflab);
        peep_tail->oper1->liveRegs = switch_live;
    }
    else
    {
        int avg1 = (lower + avg) / 2;
        int avg2 = (higher + avg + 1) / 2;
        int lab;
        if (avg + 1 < higher)
            lab = switchTreeBranchLabels[avg2] = beGetLabel;
        else
            lab = switch_deflab;
        if (switch_apl->length < 0)
            gen_branch(op_jg, lab);
        else
            gen_branch(op_ja, lab);
        peep_tail->oper1->liveRegs = switch_live;
        bingen(lower, avg1, avg);
        if (avg + 1 < higher)
            bingen(avg + 1, avg2, higher);
    }
}
int getPushMask(int i)
{
    if (i & (1 << 8))
        i |= 3;
    if (i & (1 << 9))
        i |= 8;
    return i & 0x0b;
}
void asm_line(QUAD *q)               /* line number information and text */
{
    OCODE *new = beLocalAlloc(sizeof(OCODE));
    new->opcode = op_line;
    new->oper1 = (AMODE*)(q->dc.left); /* line data */
    add_peep(new);
}
void asm_blockstart(QUAD *q)               /* line number information and text */
{
    OCODE *new = beLocalAlloc(sizeof(OCODE));
    new->opcode = op_blockstart;
    add_peep(new);
}
void asm_blockend(QUAD *q)               /* line number information and text */
{
    OCODE *new = beLocalAlloc(sizeof(OCODE));
    new->opcode = op_blockend;
    add_peep(new);
}
void asm_varstart(QUAD *q)               /* line number information and text */
{
    OCODE *new = beLocalAlloc(sizeof(OCODE));
    new->opcode = op_varstart;
    new->oper1 = (AMODE*)(q->dc.left->offset->v.sp); /* line data */
    add_peep(new);
}
void asm_func(QUAD *q)               /* line number information and text */
{
    OCODE *new = beLocalAlloc(sizeof(OCODE));
    new->opcode = q->dc.v.label ? op_funcstart : op_funcend;
    new->oper1 = (AMODE*)(q->dc.left->offset->v.sp); /* line data */
    add_peep(new);
}
void asm_passthrough(QUAD *q)        /* reserved */
{
    OCODE *val = (OCODE *)q->dc.left;
    if (val->oper1 && val->oper1->mode == am_indisp && val->oper1->preg == EBP)
    {
        SYMBOL *sp = varsp(val->oper1->offset);
        if (usingEsp)
            val->oper1->preg = ESP;
        if (sp && sp->regmode == 2) 
        {
            int len = -(sp->offset >> 8);
            val->oper1 = makedreg(-sp->offset & 255);
            if (len == 2 || len == -2)
                len = ISZ_USHORT;
            else if (len == 1 || len == -1)
                len = ISZ_UCHAR;
            else
                len = ISZ_UINT;
            val->oper1->length = len;
        }
    }
    if (val->oper2 && val->oper2->mode == am_indisp && val->oper2->preg == EBP)
    {
        SYMBOL *sp = varsp(val->oper2->offset);
        if (usingEsp)
            val->oper2->preg = ESP;
        if (sp && sp->regmode == 2) 
        {
            int len = -(sp->offset >> 8);
            val->oper2 = makedreg(-sp->offset & 255);
            if (len == 2 || len == -2)
                len = ISZ_USHORT;
            else if (len == 1 || len == -1)
                len = ISZ_UCHAR;
            else
                len = ISZ_UINT;
            val->oper2->length = len;
        }
    }
    val = gen_code(val->opcode, val->oper1, val->oper2);
    val->noopt = TRUE;
}
void asm_datapassthrough(QUAD *q)        /* reserved */
{
    (void)q;
}
void asm_label(QUAD *q)              /* put a label in the code stream */
{
    OCODE *out = beLocalAlloc(sizeof(OCODE));
    out->opcode = op_label;
    out->oper1 = (AMODE *)q->dc.v.label;
    add_peep(out);
}
void asm_goto(QUAD *q)               /* unconditional branch */
{
    if (q->dc.opcode == i_goto)
        gen_branch(op_jmp, q->dc.v.label);
    else /* directbranch */
    {
        AMODE *ap;
        SYMBOL *sp = q->dc.left->offset->v.sp;
        if (sp->regmode)
        {
            ap = makedreg(regmap[q->leftColor & 0xff][0]);
            ap->liveRegs = q->liveRegs;
            if (q->dc.left->mode == i_ind)
            {
                ap->mode = am_indisp;
                ap->offset = intNode(en_c_i, 0);
            }
        }
        else
        {
            ap = make_offset(q->dc.left->offset);
            if (q->dc.left->offset->type == en_pc)
                ap->mode = am_immed;
            ap->liveRegs = q->liveRegs;
        }
        gen_code(op_jmp, ap, 0);
    }
}
void asm_parm(QUAD *q)               /* push a parameter*/
{
    enum e_op op;
    AMODE *apl, *aph;
    getAmodes(q, &op, q->dc.left, &apl, &aph);
    if (q->dc.left->mode == i_immed)
    {
        if (q->dc.left->size >= ISZ_CFLOAT)
        {
            int sz = q->dc.left->size, sz1;
            sz1 = sizeFromISZ(sz);
            gen_codes(op_sub, ISZ_UINT, makedreg(ESP), aimmed(sz1));
            pushlevel += sz1;
            floatLoad(aph, sz - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
            floatStore(make_stack(-imaginary_offset(sz)), sz - ISZ_CFLOAT + ISZ_FLOAT);
            pushlevel += sz1;
            floatLoad(apl, sz - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
            floatStore(make_stack(0), sz - ISZ_CFLOAT + ISZ_FLOAT);
        }
        else if (q->dc.left->size >= ISZ_FLOAT)
        {
            int sz = q->dc.left->size, sz1;
            if (sz >= ISZ_IFLOAT)
                sz = sz - ISZ_IFLOAT + ISZ_FLOAT;
            sz1 = sizeFromISZ(sz);
            floatLoad(apl, sz, FALSE);
            if (sz1 == 10)
                sz1 = 12;
            gen_codes(op_sub, ISZ_UINT, makedreg(ESP), aimmed(sz1));
            pushlevel += sz1;
            floatStore(make_stack(0), sz);
        }
        else {
            int sz = q->dc.left->size;
            if (!isintconst(apl->offset))
            {
                gen_codes(op_push, ISZ_UINT, apl, 0);
                pushlevel += 4;
            }				
            else if (sz == ISZ_ULONGLONG || sz == -ISZ_ULONGLONG)
            {
                gen_codes(op_push, ISZ_UINT, aph, 0);
                pushlevel +=4;
                gen_codes(op_push, ISZ_UINT, apl, 0);
                pushlevel +=4;
            }
            else
            {
                LLONG_TYPE i;
                i = apl->offset->v.i;
                switch(sz)
                {
                    case ISZ_USHORT:
                        i &= 0xffff;
                        break;
                    case -ISZ_USHORT:
                        i &= 0xffff;
                        i |= i & 0x8000 ? 0xffff0000U : 0;
                        break;
                    case ISZ_UCHAR:
                        i &= 0xff;
                        break;
                    case -ISZ_UCHAR:
                        i &= 0xff;
                        i |= i & 0x80 ? 0xffffff00U : 0;
                        break;
                    case ISZ_BOOLEAN:
                        i &= 1;
                        break ;
                }
                gen_codes(op_push, ISZ_UINT, aimmed(i), 0);
                pushlevel += 4;
            }
        }
    }
    else
    {
        if (q->dc.left->size >= ISZ_CFLOAT)
        {
            int sz = q->dc.left->size, sz1;
            sz1 = sizeFromISZ(sz);
            gen_codes(op_sub, ISZ_UINT, makedreg(ESP), aimmed(sz1));
            pushlevel += sz1;
            floatLoad(aph, sz - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
            floatStore(make_stack(-imaginary_offset(sz)), sz - ISZ_CFLOAT + ISZ_FLOAT);
            floatLoad(apl, sz - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
            floatStore(make_stack(0), sz - ISZ_CFLOAT + ISZ_FLOAT);
        }
        else if (q->dc.left->size >= ISZ_FLOAT)
        {
            int sz = q->dc.left->size, sz1;
            sz1 = sizeFromISZ(sz);
            if (sz1 == 10)
                sz1 = 12 ;
            if (sz >= ISZ_IFLOAT)
                sz = sz - ISZ_IFLOAT + ISZ_FLOAT;
            gen_codes(op_sub, ISZ_UINT, makedreg(ESP), aimmed(sz1));
            pushlevel += sz1;
            floatLoad(apl, sz, FALSE);
            floatStore(make_stack(0), sz);
        }
        else
        {
            if (q->dc.left->size == ISZ_ULONGLONG || q->dc.left->size == -ISZ_ULONGLONG)
            {
                if (q->dc.left->mode == i_direct && q->dc.left->offset->type == en_tempref)
                {
                    int clr = beRegFromTemp(q, q->dc.left);
                    int reg1, reg2;
                    reg1 = regmap[clr & 0xff][1];
                    reg2 = regmap[clr & 0xff][0];
                    gen_codes(op_push, ISZ_UINT, makedreg(reg1), 0);
                    pushlevel += 4;
                    gen_codes(op_push, ISZ_UINT, makedreg(reg2), 0);
                    pushlevel += 4;
                }
                else
                {
                    gen_codes(op_push, ISZ_UINT, aph, 0);
                    pushlevel += 4;
                    gen_codes(op_push, ISZ_UINT, apl, 0);
                    pushlevel += 4;
                }
            }
            else
            {
                if (q->dc.left->mode == i_direct && q->dc.left->offset->type == en_tempref)
                {
                    int l = beRegFromTemp(q, q->dc.left);
                    AMODE *pal;
                    l = regmap[l][0];
                    pal = makedreg(l);
                    pal->liveRegs = q->liveRegs;
                    switch (q->dc.left->size)
                    {
                        case ISZ_USHORT:
                            do_movzx(ISZ_UINT, ISZ_USHORT, pal, pal);
                            break;
                        case -ISZ_USHORT:
                            do_movsx(ISZ_UINT, ISZ_USHORT, pal, pal);
                            break;
                        case ISZ_UCHAR:
                            do_movzx(ISZ_UINT, ISZ_UCHAR, pal, pal);
                            break;
                        case ISZ_BOOLEAN:
                            gen_codes(op_and, ISZ_UINT, pal, aimmed(1));
                            break;
                        case -ISZ_UCHAR:
                            do_movsx(ISZ_UINT, ISZ_UCHAR, pal, pal);
                            break;
                        default:
                            break;
                    }
                    gen_codes(op_push, ISZ_UINT, pal, 0);
                }
                else
                {
                    gen_codes(op_push, ISZ_UINT, apl, 0);
                }
                pushlevel += 4;
            }
        }
    }
}
void asm_parmblock(QUAD *q)          /* push a block of memory */
{
    int n = q->dc.right->offset->v.i;
    AMODE *apl, *aph;
    enum e_op op;
    EXPRESSION *ofs;
    
    getAmodes(q, &op, q->dc.left, &apl, &aph);
    
    n += 3;
    n &= 0xfffffffcU;

        
    ofs = apl->offset ;
    if (q->dc.left->mode == i_immed)
    {
        op = op_lea;
        if (ofs->type == en_auto)
        {
            if (usingEsp)
            {
                apl->preg = ESP;
            }
            else
            {
                apl->preg = EBP;
            }
            apl->mode = am_indisp;
        }
        else
            apl->mode = am_direct;
    }

    if (n <= 24 && q->dc.left->mode == i_immed)
    {
        while (n > 0)
        {
            n -= 4;
            apl->offset = exprNode(en_add, ofs, intNode(en_c_i, n));
            gen_codes(op_push, ISZ_UINT, apl, 0);
            pushlevel += 4;
        }
    }
    else
    {
        AMODE *cx = makedreg(ECX);
        AMODE *di = makedreg(EDI);
        AMODE *si = makedreg(ESI);
        AMODE *sp = makedreg(ESP);
        cx->liveRegs = q->liveRegs;
        si->liveRegs = q->liveRegs;
        di->liveRegs = q->liveRegs;
        gen_codes(op_sub, ISZ_UINT, sp, aimmed(n));
        gen_codes(op_push, ISZ_UINT, di, 0);		
        gen_codes(op_push, ISZ_UINT, si, 0);
        gen_codes(op_push, ISZ_UINT, cx, 0);
        pushlevel += 12;
        gen_codes(op, ISZ_UINT, si, apl);
        gen_codes(op_mov, ISZ_UINT, di, sp);
        gen_codes(op_add, ISZ_UINT, di, aimmed(12));
        gen_codes(op_mov, ISZ_UINT, cx, aimmed(n/ 4));
        gen_code(op_cld, 0, 0);
        gen_code(op_rep, 0, 0);
        gen_code(op_movsd, 0, 0);
        gen_codes(op_pop, ISZ_UINT, cx, 0);
        gen_codes(op_pop, ISZ_UINT, si, 0);
        gen_codes(op_pop, ISZ_UINT, di, 0);
        pushlevel -= 12;
        pushlevel += n;

    }
}
void asm_parmadj(QUAD *q)            /* adjust stack after function call */
{
    int mask;
    int i = beGetIcon(q->dc.left);
    if (i)
    {
        if (i == 4)
        {
            gen_code(op_pop, makedreg(ECX), 0);
        }
        else if (i == 8)
        {
            gen_code(op_pop, makedreg(ECX), 0);
            gen_code(op_pop, makedreg(ECX), 0);
        }   
        else
        {
            gen_code(op_add, makedreg(ESP), aimmed(i));
        }
    }
    pushlevel -= beGetIcon(q->dc.right);
    mask = getPushMask(q->dc.v.i);
    for (i=3; i >= 0; i-- )
        if ((1 << i) & mask)
        {
            gen_code(op_pop, makedreg(regmap[i][0]), 0);
            pushlevel -= 4;
        }
}
void asm_gosub(QUAD *q)              /* normal gosub to an immediate label or through a var */
{
    TYPE *tp = NULL;
    EXPRESSION *en = NULL;
    enum e_op op;
    AMODE *apl, *aph;
    if (q->dc.left->offset)
        en = GetSymRef(q->dc.left->offset);
    if (!en && q->dc.left->offset2)
        en = GetSymRef(q->dc.left->offset2);
    if (!en && q->dc.left->offset3)
        en = GetSymRef(q->dc.left->offset3);
    getAmodes(q, &op, q->dc.left, &apl, &aph);
        
    if (en)
        tp = en->v.sp->tp;
        
    if (q->dc.left->mode == i_immed)
    {
        apl->length = 0;
        gen_code(op_call, apl, 0);
    }
    else
    {
        apl->liveRegs = q->liveRegs;
        gen_code(op_call, apl, 0);
    }
    
    // pop the stack if returning from a function returning floating point
    // whose return value lies unused.
    if (q->novalue >= 0)
    {
        switch (q->novalue)
        {
            case ISZ_CFLOAT:
            case ISZ_CDOUBLE:
            case ISZ_CLDOUBLE:
                gen_codes(op_fstp, ISZ_LDOUBLE, makefreg(0), 0);
                // fall through
            case ISZ_FLOAT:
            case ISZ_DOUBLE:
            case ISZ_LDOUBLE:
            case ISZ_IFLOAT:
            case ISZ_IDOUBLE:
            case ISZ_ILDOUBLE:
                gen_codes(op_fstp, ISZ_LDOUBLE, makefreg(0), 0);
                break;
            
        }
    }
}
void asm_fargosub(QUAD *q)           /* far version of gosub */
{
    gen_code(op_push, makesegreg(CS), 0);
    asm_gosub(q);
}
void asm_trap(QUAD *q)               /* 'trap' instruction - the arg will be an immediate # */
{
    if (q->dc.left->offset->v.i  == 3)
        gen_code(op_int3, 0, 0);
    else
        gen_code(op_int, aimmed(q->dc.left->offset->v.i), 0);
}
void asm_int(QUAD *q)                /* 'int' instruction(QUAD *q) calls a labeled function which is an interrupt */
{
    gen_code(op_pushf, 0, 0);
    gen_code(op_push, makesegreg(CS), 0);
    asm_gosub(q);    
}
/* left will be a constant holding the number of bytes to pop
 * e.g. the parameters will be popped in stdcall or pascal type functions
 */
void asm_ret(QUAD *q)                /* return from subroutine */
{
    if (beGetIcon(q->dc.left))
        gen_code(op_ret, aimmed(beGetIcon(q->dc.left)), 0);
    else
        gen_code(op_ret, 0, 0);    
    
}
/* left will be a constant holding the number of bytes to pop
 * e.g. the parameters will be popped in stdcall or pascal type functions
 */
void asm_fret(QUAD *q)                /* far return from subroutine */
{
    if (beGetIcon(q->dc.left))
        gen_code(op_retf, aimmed(beGetIcon(q->dc.left)), 0);
    else
        gen_code(op_retf, 0, 0);    
}
/*
 * this can be either a fault or iret return
 * for processors that char, the 'left' member will have an integer
 * value that is true for an iret or false or a fault ret
 */
void asm_rett(QUAD *q)               /* return from trap or int */
{
    (void)q;
    gen_code(op_iret, 0, 0);
}
void asm_add(QUAD *q)                /* evaluate an addition */
{
    enum e_op opa, opl, opr;
    AMODE *apal, *apah, *apll, *aplh, *aprl, *aprh;
    getAmodes(q, &opl, q->dc.left, &apll, &aplh);
    getAmodes(q, &opr, q->dc.right, &aprl, &aprh);
    getAmodes(q, &opa, q->ans, &apal, &apah);
    if (q->ans->size >= ISZ_CFLOAT)
    {
        floatLoad(aplh, q->dc.left->size - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
        floatLoad(apll, q->dc.left->size - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
        floatLoad(aprh, q->dc.right->size - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
        floatLoad(aprl, q->dc.right->size - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
        gen_codes(op_faddp, ISZ_LDOUBLE, makefreg(2), 0);
        gen_codes(op_faddp, ISZ_LDOUBLE, makefreg(2), 0);
        floatStore(apal, q->ans->size - ISZ_CFLOAT + ISZ_FLOAT);
        floatStore(apah, q->ans->size - ISZ_CFLOAT + ISZ_FLOAT);
    }
    else if (q->ans->size >= ISZ_FLOAT)
    {
        float_gen(q, apll, aprl, op_fadd, op_fadd, op_faddp, op_faddp);
        floatStore(apal, q->ans->size);
    }
    else if (q->ans->size == ISZ_ULONGLONG || q->ans->size == -ISZ_ULONGLONG)
    {
        if (equal_address(apal, apll))
        {
            func_axdx(op_add, apal, apah, aprl, aprh);
        }
        else if (equal_address(apal, aprl))
        {
            func_axdx(op_add, apal, apah, apll, aplh);
        }
        else
        {
            liveQualify(apal, apal, apah);
            liveQualify(apah, apal, apah);
            func_axdx_mov(op_add, apal, apah, apll, aplh, aprl, aprh);
        }
        
    }
    else
    {
        if (equal_address(apal, apll))
        {
            if (opr == op_lea)
            {
                if (apal->mode != am_dreg)
                    diag("asm_add: expected dreg in add from EBP");
                aprl->mode = am_indispscale;
                aprl->sreg = apal->preg;
                gen_codes(op_lea, q->ans->size, apal, aprl);
            }
            else
                gen_codes(op_add, q->ans->size, apal, aprl);
        }
        else if (equal_address(apal, aprl))
        {
            if (opl == op_lea)
            {
                if (apal->mode != am_dreg)
                    diag("asm_add: expected dreg in add from EBP");
                apll->mode = am_indispscale;
                apll->sreg = apal->preg;
                gen_codes(op_lea, q->ans->size, apal, apll);
            }
            else
                gen_codes(op_add, q->ans->size, apal, apll);
        }
        else
        {
            if (samereg(apal, aprl) || apll->mode == am_immed)
            {
                liveQualify(apal, apal, 0);
                gen_codes(opr, q->ans->size, apal, aprl);
                if (opl == op_lea)
                {
                    if (apal->mode != am_dreg)
                        diag("asm_add: expected dreg in add from EBP");
                    apll->mode = am_indispscale;
                    apll->sreg = apal->preg;
                    gen_codes(op_lea, q->ans->size, apal, apll);
                }
                else
                    gen_codes(op_add, q->ans->size, apal, apll);
            }
            else
            {
                liveQualify(apal, apal, 0);
                if (opl == op_lea && aprl->mode == am_dreg)
                {
                    if (apal->mode != am_dreg)
                        diag("asm_add: expected dreg in add from EBP");
                    apll->mode = am_indispscale;
                    apll->sreg = aprl->preg;
                    gen_codes(op_lea, q->ans->size, apal, apll);
                }
                else if (opr == op_lea && apll->mode == am_dreg)
                {
                    if (apal->mode != am_dreg)
                        diag("asm_add: expected dreg in add from EBP");
                    aprl->mode = am_indispscale;
                    aprl->sreg = apll->preg;
                    gen_codes(op_lea, q->ans->size, apal, aprl);
                }
                else
                {
                    if (equal_address(apll, aprl))
                        apal->liveRegs = -1;
                    if (opl == op_lea || opr != op_lea)
                    {
                        gen_codes(opl, q->ans->size, apal, apll);
                        gen_codes(op_add, q->ans->size, apal, aprl);
                    }
                    else
                    {
                        gen_codes(opr, q->ans->size, apal, aprl);
                        gen_codes(op_add, q->ans->size, apal, apll);
                    }
                }
            }
        }
    }
    
}
void asm_sub(QUAD *q)                /* evaluate a subtraction */
{
    enum e_op opa, opl, opr;
    AMODE *apal, *apah, *apll, *aplh, *aprl, *aprh;
    getAmodes(q, &opl, q->dc.left, &apll, &aplh);
    getAmodes(q, &opr, q->dc.right, &aprl, &aprh);
    getAmodes(q, &opa, q->ans, &apal, &apah);
    if (q->ans->size >= ISZ_CFLOAT)
    {
        floatLoad(aplh, q->dc.left->size - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
        floatLoad(apll, q->dc.left->size - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
        floatLoad(aprh, q->dc.right->size - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
        floatLoad(aprl, q->dc.right->size - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
        if (aprl->mode == am_freg && apll->mode != am_freg)
        {
            gen_codes(op_fsubrp, ISZ_LDOUBLE, makefreg(2), 0);
            gen_codes(op_fsubrp, ISZ_LDOUBLE, makefreg(2), 0);
        }
        else
        {
            gen_codes(op_fsubp, ISZ_LDOUBLE, makefreg(2), 0);
            gen_codes(op_fsubp, ISZ_LDOUBLE, makefreg(2), 0);
        }
        floatStore(apal, q->ans->size - ISZ_CFLOAT + ISZ_FLOAT);
        floatStore(apah, q->ans->size - ISZ_CFLOAT + ISZ_FLOAT);
    }
    else if (q->ans->size >= ISZ_FLOAT)
    {
        float_gen(q, apll, aprl, op_fsub, op_fsubr, op_fsubp, op_fsubrp);
        floatStore(apal, q->ans->size);
    }
    else if (q->ans->size == ISZ_ULONGLONG || q->ans->size == -ISZ_ULONGLONG)
    {
        if (equal_address(apal, apll))
        {
            func_axdx(op_sub, apal, apah, aprl, aprh);
        }
        else if (equal_address(apal, aprl))
        {
            func_axdx(op_sub, apal, apah, apll, aplh);
            gen_codes(op_neg, ISZ_UINT, apah, 0);
            gen_codes(op_neg, ISZ_UINT, apal, 0);
            gen_codes(op_sbb, ISZ_UINT, apah, aimmed(0));
        }
        else
        {
            liveQualify(apal, apal, apah);
            liveQualify(apah, apal, apah);
            func_axdx_mov(op_sub, apal, apah, apll, aplh, aprl, aprh);
        }
        
    }
    else
    {
        if (equal_address(apal, apll))
        {
            gen_codes(op_sub, q->ans->size, apal, aprl);
        }
        else if (equal_address(apal, aprl))
        {
            liveQualify(apal, apal, 0);
            gen_codes(op_sub, q->ans->size, apal, apll);
            gen_codes(op_neg, q->ans->size, apal, 0);
        }
        else
        {
            if (equal_address(apll, aprl))
                apal->liveRegs = -1;
            if (samereg(apal, aprl))
            {
                liveQualify(apal, apal, 0);
                gen_codes(opr, q->ans->size, apal, aprl);
                gen_codes(op_sub, q->ans->size, apal, apll);
                gen_codes(op_neg, q->ans->size, apal, 0);
            }
            else
            {
                liveQualify(apal, apal, 0);
                gen_codes(opl, q->ans->size, apal, apll);
                gen_codes(op_sub, q->ans->size, apal, aprl);
            }
        }
    }
    
}
void asm_udiv(QUAD *q)               /* unsigned division */
{
    gen_div(q, op_div);
}
void asm_umod(QUAD *q)               /* unsigned modulous */
{
    gen_div(q, op_div);
}
void asm_sdiv(QUAD *q)               /* signed division */
{
    gen_div(q, op_idiv);
}
void asm_smod(QUAD *q)               /* signed modulous */
{
    gen_div(q, op_idiv);
}
void asm_muluh(QUAD *q)
{
    gen_mulxh( q, op_mul);
}
void asm_mulsh(QUAD *q)
{
    gen_mulxh( q, op_imul);
}
void asm_mul(QUAD *q)               /* signed multiply */
{
    enum e_op opa, opl, opr;
    AMODE *apal, *apah, *apll, *aplh, *aprl, *aprh;
    getAmodes(q, &opl, q->dc.left, &apll, &aplh);
    getAmodes(q, &opr, q->dc.right, &aprl, &aprh);
    getAmodes(q, &opa, q->ans, &apal, &apah);
    if (q->ans->size >= ISZ_FLOAT)
    {
        float_gen(q, apll, aprl, op_fmul, op_fmul, op_fmulp, op_fmulp);
        floatStore(apal, q->ans->size);
    }
    else if (q->ans->size == ISZ_ULONGLONG || q->ans->size == -ISZ_ULONGLONG)
    {
        diag("asm_smul: long long");
    }
    else
    {
        if (equal_address(apal, apll))
        {
            gen_codes(op_imul, q->ans->size, apal, aprl);
        }
        else if (equal_address(apal, aprl))
            gen_codes(op_imul, q->ans->size, apal, apll);
        else if (apll->mode == am_immed)
        {
            if (aprl->mode == am_immed)
            {
                apll = aimmed(apll->offset->v.i * aprl->offset->v.i);
                gen_codes(op_mov, q->ans->size, apal, apll);
            }
            else
            {
                gen_code3(op_imul, apal, aprl, apll);
            }
        }
        else if (aprl->mode == am_immed)
        {
            gen_code3(op_imul, apal, apll, aprl);
        }
        else
        {
            if (samereg(apal, aprl) || apll->mode == am_immed)
            {
                liveQualify(apal, apal, 0);
                gen_codes(op_mov, q->ans->size, apal, aprl);
                gen_codes(op_imul, q->ans->size, apal, apll);
            }
            else
            {
                liveQualify(apal, apal, 0);
                gen_codes(op_mov, q->ans->size, apal, apll);
                gen_codes(op_imul, q->ans->size, apal, aprl);
            }
        }
    }
    
}
void asm_lsr(QUAD *q)                /* unsigned shift right */
{
    enum e_op opa, opl, opr;
    AMODE *apal, *apah, *apll, *aplh, *aprl, *aprh;
    getAmodes(q, &opl, q->dc.left, &apll, &aplh);
    getAmodes(q, &opr, q->dc.right, &aprl, &aprh);
    getAmodes(q, &opa, q->ans, &apal, &apah);
    if (q->ans->size == ISZ_ULONGLONG || q->ans->size == -ISZ_ULONGLONG)
    {
        if (aprl->mode == am_immed)
        {
            int n = aprl->offset->v.i;
            if (n >= 32)
            {
                n -= 32;
                liveQualify(apah, apal, apah);
                gen_codes(op_mov, ISZ_UINT, apal, aplh);
                gen_codes(op_mov, ISZ_UINT, apah, aimmed(0));
                if (n)
                    gen_codes(op_shr, ISZ_UINT, apal, aimmed(n));
            }
            else
            {
                if (!equal_address(apal, apll))
                {
                    func_axdx(op_mov, apal, apah, apll, aplh);
                }
                if (n)
                {
                    gen_lshift(op_shrd, apal, apah, aimmed(n));
                    gen_codes(op_shr, ISZ_UINT, apah, aimmed(n));
                }
            }
        }
        else
            diag("asm_lsr: long long shift by non-const");
    }
    else
    {
        gen_shift(q, op_shr, apal, apll, aprl);
    }    
}
void asm_lsl(QUAD *q)                /* signed shift left */
{
    enum e_op opa, opl, opr;
    AMODE *apal, *apah, *apll, *aplh, *aprl, *aprh;
    getAmodes(q, &opl, q->dc.left, &apll, &aplh);
    getAmodes(q, &opr, q->dc.right, &aprl, &aprh);
    getAmodes(q, &opa, q->ans, &apal, &apah);
    if (q->ans->size == ISZ_ULONGLONG || q->ans->size == -ISZ_ULONGLONG)
    {
        if (aprl->mode == am_immed)
        {
            int n = aprl->offset->v.i;
            if (n >= 32)
            {
                n -= 32;
                liveQualify(apal, apal, apah);
                liveQualify(apah, apal, apah);
                gen_codes(op_mov, ISZ_UINT, apah, apll);
                gen_codes(op_mov, ISZ_UINT, apal, aimmed(0));
                if (n)
                    gen_code(op_shl, apah, aimmed(n));
            }
            else
            {
                liveQualify(apal, apal, apah);
                liveQualify(apah, apal, apah);
                if (!equal_address(apal, apll))
                {
                    func_axdx(op_mov, apal, apah, apll, aplh);
                }
                gen_lshift(op_shld, apah, apal, aimmed(n));
                gen_codes(op_shl, ISZ_UINT, apal, aimmed(n));
            }
        }
        else
            diag("asm_lsr: long long shift by non-const");
    }
    else
    {
        gen_shift(q, op_shl, apal, apll, aprl);
    }
}
void asm_asr(QUAD *q)                /* signed shift right */
{
    enum e_op opa, opl, opr;
    AMODE *apal, *apah, *apll, *aplh, *aprl, *aprh;
    getAmodes(q, &opl, q->dc.left, &apll, &aplh);
    getAmodes(q, &opr, q->dc.right, &aprl, &aprh);
    getAmodes(q, &opa, q->ans, &apal, &apah);
    if (q->ans->size == ISZ_ULONGLONG || q->ans->size == -ISZ_ULONGLONG)
    {
        if (aprl->mode == am_immed)
        {
            int n = aprl->offset->v.i;
            if (n >= 32)
            {
                n -= 32;
                liveQualify(apal, apal, apah);
                liveQualify(apah, apal, apah);
                func_axdx(op_mov, apal, apah, aplh, aplh);
                gen_codes(op_sar, ISZ_UINT, apah, aimmed(31));
                if (n)
                    gen_code(op_sar, apal, aimmed(n));
            }
            else
            {
                if (!equal_address(apal, apll))
                {
                    liveQualify(apah, apal, apah);
                    func_axdx(op_mov, apal, apah, apll, aplh);
                }
                if (n)
                {
                    liveQualify(apah, apal, apah);
                    gen_lshift(op_shrd, apal, apah, aimmed(n));
                    gen_codes(op_sar, ISZ_UINT, apah, aimmed(n));
                }
            }
        }
        else
            diag("asm_asr: long long shift by non-const");
    }
    else
    {
        gen_shift(q, op_sar, apal, apll, aprl);
    }    
    
}
void asm_neg(QUAD *q)                /* negation */
{
    enum e_op opa, opl;
    AMODE *apal, *apah, *apll, *aplh;
    getAmodes(q, &opl, q->dc.left, &apll, &aplh);
    getAmodes(q, &opa, q->ans, &apal, &apah);
    if (q->ans->size >= ISZ_CFLOAT)
    {
        floatLoad(aplh, q->dc.left->size - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
        floatLoad(apll, q->dc.left->size - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
        gen_code(op_fchs, 0, 0);
        gen_code(op_fxch, 0, 0);
        gen_code(op_fchs, 0, 0);
        gen_code(op_fxch, 0, 0);
        floatStore(apal, q->ans->size - ISZ_CFLOAT + ISZ_FLOAT);
        floatStore(apah, q->ans->size - ISZ_CFLOAT + ISZ_FLOAT);
    }
    else if (q->ans->size >= ISZ_FLOAT)
    {
        floatLoad(apll, q->dc.left->size, FALSE);
        gen_code(op_fchs, 0, 0);
        floatStore(apal, q->ans->size);
    }
    else if (q->ans->size == ISZ_ULONGLONG || q->ans->size == -ISZ_ULONGLONG)
    {
        if (!equal_address(apal, apll))
        {
            func_axdx(op_mov, apal, apah, apll, aplh);
        }
        liveQualify(apah, apah, 0);
        liveQualify(apal, apah, 0);
        gen_codes(op_neg, ISZ_UINT, apah, 0);
        gen_codes(op_neg, ISZ_UINT, apal, 0);
        gen_codes(op_sbb, ISZ_UINT, apah, aimmed(0));		
    }
    else
    {
        if (!equal_address(apal, apll))
        {
            liveQualify(apal, apal, 0);
            gen_codes(op_mov, q->ans->size, apal, apll);
        }
        gen_codes(op_neg, q->ans->size, apal, 0);
    }
}
void asm_not(QUAD *q)                /* complement */
{
    enum e_op opa, opl;
    AMODE *apal, *apah, *apll, *aplh;
    getAmodes(q, &opl, q->dc.left, &apll, &aplh);
    getAmodes(q, &opa, q->ans, &apal, &apah);
    if (q->ans->size == ISZ_ULONGLONG || q->ans->size == -ISZ_ULONGLONG)
    {
        if (!equal_address(apal, apll))
        {
            func_axdx(op_mov, apal, apah, apll, aplh);
        }
        gen_codes(op_not, ISZ_UINT, apah, 0);
        gen_codes(op_not, ISZ_UINT, apal, 0);
        
    }
    else
    {
        if (!equal_address(apal, apll))
        {
            liveQualify(apal, apal, 0);
            gen_codes(op_mov, q->ans->size, apal, apll);
        }
        gen_codes(op_not, q->ans->size, apal, 0);
    }
    
}
void asm_and(QUAD *q)                /* binary and */
{
    enum e_op opa, opl, opr;
    AMODE *apal, *apah, *apll, *aplh, *aprl, *aprh;
    getAmodes(q, &opl, q->dc.left, &apll, &aplh);
    getAmodes(q, &opr, q->dc.right, &aprl, &aprh);
    getAmodes(q, &opa, q->ans, &apal, &apah);
    if (q->ans->size == ISZ_ULONGLONG || q->ans->size == -ISZ_ULONGLONG)
    {
        if (equal_address(apal, apll))
        {
            func_axdx(op_and, apal, apah, aprl, aprh);
        }
        else if (equal_address(apal, aprl))
        {
            func_axdx(op_and, apal, apah, apll, aplh);
        }
        else
        {
            liveQualify(apal, apal, apah);
            liveQualify(apah, apal, apah);
            func_axdx_mov(op_and, apal, apah, apll, aplh, aprl, aprh);
        }
        
    }
    else
    {
        if (equal_address(apal, apll))
        {
            gen_codes(op_and, q->ans->size, apal, aprl);
        }
        else if (equal_address(apal, aprl))
            gen_codes(op_and, q->ans->size, apal, apll);
        else
        {
            if (equal_address(apll, aprl))
                apal->liveRegs = -1;
            if (samereg(apal, aprl))
            {
                liveQualify(apal, apal, 0);
                gen_codes(op_mov, q->ans->size, apal, aprl);
                gen_codes(op_and, q->ans->size, apal, apll);
            }
            else
            {
                liveQualify(apal, apal, 0);
                gen_codes(op_mov, q->ans->size, apal, apll);
                gen_codes(op_and, q->ans->size, apal, aprl);
            }
        }
    }
}
void asm_or(QUAD *q)                 /* binary or */
{
    enum e_op opa, opl, opr;
    AMODE *apal, *apah, *apll, *aplh, *aprl, *aprh;
    getAmodes(q, &opl, q->dc.left, &apll, &aplh);
    getAmodes(q, &opr, q->dc.right, &aprl, &aprh);
    getAmodes(q, &opa, q->ans, &apal, &apah);
    if (q->ans->size == ISZ_ULONGLONG || q->ans->size == -ISZ_ULONGLONG)
    {
        if (equal_address(apal, apll))
        {
            func_axdx(op_or, apal, apah, aprl, aprh);
        }
        else if (equal_address(apal, aprl))
        {
            func_axdx(op_or, apal, apah, apll, aplh);
        }
        else
        {
            liveQualify(apal, apal, apah);
            liveQualify(apah, apal, apah);
            func_axdx_mov(op_or, apal, apah, apll, aplh, aprl, aprh);
        }
        
    }
    else
    {
        if (equal_address(apal, apll))
        {
            gen_codes(op_or, q->ans->size, apal, aprl);
        }
        else if (equal_address(apal, aprl))
            gen_codes(op_or, q->ans->size, apal, apll);
        else
        {
            if (equal_address(apll, aprl))
                apal->liveRegs = -1;
            if (samereg(apal, aprl))
            {
                liveQualify(apal, apal, 0);
                gen_codes(op_mov, q->ans->size, apal, aprl);
                gen_codes(op_or, q->ans->size, apal, apll);
            }
            else
            {
                liveQualify(apal, apal, 0);
                gen_codes(op_mov, q->ans->size, apal, apll);
                gen_codes(op_or, q->ans->size, apal, aprl);
            }
        }
    }
    
}
void asm_eor(QUAD *q)                /* binary exclusive or */
{
    enum e_op opa, opl, opr;
    AMODE *apal, *apah, *apll, *aplh, *aprl, *aprh;
    getAmodes(q, &opl, q->dc.left, &apll, &aplh);
    getAmodes(q, &opr, q->dc.right, &aprl, &aprh);
    getAmodes(q, &opa, q->ans, &apal, &apah);
    if (q->ans->size == ISZ_ULONGLONG || q->ans->size == -ISZ_ULONGLONG)
    {
        if (equal_address(apal, apll))
        {
            func_axdx(op_xor, apal, apah, aprl, aprh);
        }
        else if (equal_address(apal, aprl))
        {
            func_axdx(op_xor, apal, apah, apll, aplh);
        }
        else
        {
            liveQualify(apal, apal, apah);
            liveQualify(apah, apal, apah);
            func_axdx_mov(op_xor, apal, apah, apll, aplh, aprl, aprh);
        }
        
    }
    else
    {
        if (equal_address(apal, apll))
        {
            gen_codes(op_xor, q->ans->size, apal, aprl);
        }
        else if (equal_address(apal, aprl))
            gen_codes(op_xor, q->ans->size, apal, apll);
        else
        {
            if (equal_address(apll, aprl))
                apal->liveRegs = -1;
            if (samereg(apal, aprl))
            {
                liveQualify(apal, apal, 0);
                gen_codes(op_mov, q->ans->size, apal, aprl);
                gen_codes(op_xor, q->ans->size, apal, apll);
            }
            else
            {
                liveQualify(apal, apal, 0);
                gen_codes(op_mov, q->ans->size, apal, apll);
                gen_codes(op_xor, q->ans->size, apal, aprl);
            }
        }
    }
}
void asm_setne(QUAD *q)              /* evaluate a = b != c */
{
    gen_xset(q, op_setne, op_setne, op_setne);
}
void asm_sete(QUAD *q)               /* evaluate a = b == c */
{
    gen_xset(q, op_sete, op_sete, op_sete);
    
}
void asm_setc(QUAD *q)               /* evaluate a = b U< c */
{
    gen_xset(q, op_setc, op_seta, op_setc);
    
}
void asm_seta(QUAD *q)               /* evaluate a = b U> c */
{
    gen_xset(q, op_seta, op_setc, op_seta);
    
}
void asm_setnc(QUAD *q)              /* evaluate a = b U>= c */
{
    gen_xset(q, op_setnc, op_setbe, op_setae);
    
}
void asm_setbe(QUAD *q)              /* evaluate a = b U<= c */
{
    gen_xset(q, op_setbe, op_setnc, op_setbe);
    
}
void asm_setl(QUAD *q)               /* evaluate a = b S< c */
{
    gen_xset(q, op_setl, op_setg, op_setb);
    
}
void asm_setg(QUAD *q)               /* evaluate a = b s> c */
{
    gen_xset(q, op_setg, op_setl, op_seta);
    
}
void asm_setle(QUAD *q)              /* evaluate a = b S<= c */
{
    gen_xset(q, op_setle, op_setge,op_setbe);
    
}
void asm_setge(QUAD *q)              /* evaluate a = b S>= c */
{
    gen_xset(q, op_setge, op_setle, op_setae);
    
}
void asm_assn(QUAD *q)               /* assignment */
{
    AMODE *apa, *apa1, *apl, *apl1;
    enum e_op opa, opl;
    /* when we get here, one side or the other is in a register */
    int szl ;
    int sza = q->ans->size;
    int bits = q->ans->bits;
    if (sza < 0)
        sza = - sza ;
    szl = sza;
    q->ans->bits = 0;
    getAmodes(q, &opa, q->ans, &apa, &apa1);
    q->ans->bits = bits;
    if (q->dc.opcode == i_assn)
    {
        szl = q->dc.left->size;
        if (szl < 0)
            szl = - szl;
        getAmodes(q, &opl, q->dc.left, &apl, &apl1);
    }
    else if (q->dc.opcode == i_icon)
    {
        opl = op_mov;
        if (sza == ISZ_ULONGLONG)
        {
        
            apl = aimmed(q->dc.v.i);
#ifdef USE_LONGLONG
            apl1 = aimmed((q->dc.v.i >> 32));
#else
            if (q->dc.v.i < 0)
                apl1 = aimmed(-1);
            else
                apl1 = aimmed(0);
#endif			
        }
        else
        {
            apl = aimmed(q->dc.v.i);
        }
    }
    else if (q->dc.opcode == i_fcon)
    {
        EXPRESSION *node = exprNode(en_c_ld, 0, 0);
        node->v.f = q->dc.v.f;
        apl = (AMODE *)beLocalAlloc(sizeof(AMODE));
        apl->offset = node;
        make_floatconst(apl);
    }
    else
        diag("asm_assn: unknown opcode");
    if (sza == szl || (q->dc.left->mode == i_immed && szl < ISZ_FLOAT))
    {
        if (q->ans->size >= ISZ_CFLOAT)
        {
            if (apl1->offset && apl1->mode == am_immed && isintconst(apl1->offset))
                make_floatconst(apl1);
            if (apl1->offset && (isfloatconst(apl1->offset)))
                make_floatconst(apl1);
            if (apl->offset && apl->mode == am_immed && isintconst(apl->offset))
                make_floatconst(apl);
            if (apl->offset && (isfloatconst(apl->offset)))
                make_floatconst(apl);
            floatLoad(apl1, sza - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
            floatLoad(apl, sza - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
            floatStore(apa, sza - ISZ_CFLOAT + ISZ_FLOAT);
            floatStore(apa1, sza - ISZ_CFLOAT + ISZ_FLOAT);
        }
        else if (q->ans->size >= ISZ_FLOAT)
        {
            int sz = sza ;
            if (apl->offset && apl->mode == am_immed && isintconst(apl->offset))
                make_floatconst(apl);
            if (apl->offset && (isfloatconst(apl->offset)))
                make_floatconst(apl);
            if (sz >= ISZ_IFLOAT)
                sz = sz - ISZ_IFLOAT + ISZ_FLOAT;
            floatLoad(apl, sz, FALSE);
            floatStore(apa, sz);
        }
        else if (sza == ISZ_ULONGLONG)
        {
            func_axdx(op_mov, apa, apa1, apl, apl1);
        }
        else
        {
            if (q->ans->bits)
                bit_store(apa, apl, q->ans->size, q->ans->bits, q->ans->startbit);
            else 
            {
                gen_codes(opl, q->ans->size, apa, apl);
                if (q->dc.opcode == i_assn && q->dc.left->bits)
                {
                    int max;
                    switch(sza)
                    {
                        case ISZ_UCHAR:
                            max = 8;
                            break;
                        case ISZ_USHORT:
                        case ISZ_U16:
                            max = 16;
                            break;
                        case ISZ_UINT:
                        case ISZ_ULONG:
                        case ISZ_U32:
                        default:
                            max = 32;
                            break;
                                                            
                    }
                    /* should be in a register at this point */
                    if (apa->mode != am_dreg)
                        diag("asm_assn: Expected register in bit processing");
                    if (q->dc.left->size < 0)
                    {
                        gen_codes(op_shl, q->ans->size, apa, aimmed(max - q->dc.left->startbit - q->dc.left->bits));
                        gen_codes(op_sar, q->ans->size, apa, aimmed(max - q->dc.left->bits));
                    }
                    else
                    {
                        if (q->dc.left->startbit)
                            gen_codes(op_shr, q->ans->size, apa, aimmed(q->dc.left->startbit));
                        if (q->dc.left->bits < max)
                            gen_codes(op_and, q->ans->size, apa, aimmed(( 1 << q->dc.left->bits) -1));
                    }
                }
            }
        }
    }
    else if (sza < szl)
    {
        if (q->dc.left->size == ISZ_ADDR)
            if (q->ans->size == ISZ_ULONGLONG || q->ans->size == -ISZ_ULONGLONG)
                goto addrupjn;
        if (q->ans->size == ISZ_BOOLEAN)
            
        {
            int ulbl = beGetLabel;
            if (q->dc.left->size >= ISZ_CFLOAT)
            {
                int lab1 = beGetLabel;
                int lab2 = beGetLabel;
                if (apl->mode != am_freg)
                    floatLoad(apl, szl - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
                gen_code(op_fldz, 0, 0);
                gen_code(op_fucompp, 0, 0);
                if (apa->liveRegs & (1 << EAX))
                    gen_code(op_push, makedreg(EAX), 0);
                gen_codes(op_fstsw, ISZ_USHORT, makedreg(EAX), 0);
                gen_codes(op_bt, ISZ_UINT, makedreg(EAX), aimmed(10));
                gen_code(op_jnc, make_label(ulbl), 0);
                gen_codes(op_btr, ISZ_UINT, makedreg(EAX), aimmed(14));
                oa_gen_label(ulbl);
                gen_code(op_sahf, 0, 0);
                if (apa->liveRegs & (1 << EAX))
                    gen_code(op_pop, makedreg(EAX), 0);
                gen_branch(op_jne, lab1);
                
                if (apl->mode != am_freg)
                    floatLoad(apl1, szl - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
                gen_code(op_fldz, 0, 0);
                gen_code(op_fucompp, 0, 0);
                if (apa->liveRegs & (1 << EAX))
                    gen_code(op_push, makedreg(EAX), 0);
                gen_codes(op_fstsw, ISZ_USHORT, makedreg(EAX), 0);
                gen_codes(op_bt, ISZ_UINT, makedreg(EAX), aimmed(10));
                gen_code(op_jnc, make_label(ulbl), 0);
                gen_codes(op_btr, ISZ_UINT, makedreg(EAX), aimmed(14));
                oa_gen_label(ulbl);
                gen_code(op_sahf, 0, 0);
                if (apa->liveRegs & (1 << EAX))
                    gen_code(op_pop, makedreg(EAX), 0);
                gen_branch(op_jmp, lab2);
                gen_label(lab1);
                gen_code(op_fcomp, makefreg(0), 0);
                gen_label(lab2);
            }
            else if (q->dc.left->size >= ISZ_IFLOAT)
            {
                if (apl->mode != am_freg)
                    floatLoad(apl, szl - ISZ_IFLOAT + ISZ_FLOAT, FALSE);
                gen_code(op_fldz, 0, 0);
                gen_code(op_fucompp, 0, 0);
                if (apa->liveRegs & (1 << EAX))
                    gen_code(op_push, makedreg(EAX), 0);
                gen_codes(op_fstsw, ISZ_USHORT, makedreg(EAX), 0);
                gen_codes(op_bt, ISZ_UINT, makedreg(EAX), aimmed(10));
                gen_code(op_jnc, make_label(ulbl), 0);
                gen_codes(op_btr, ISZ_UINT, makedreg(EAX), aimmed(14));
                oa_gen_label(ulbl);
                gen_code(op_sahf, 0, 0);
                if (apa->liveRegs & (1 << EAX))
                    gen_code(op_pop, makedreg(EAX), 0);
            }
            else if (q->dc.left->size >= ISZ_FLOAT)
            {
                if (apl->mode != am_freg)
                    floatLoad(apl, szl, FALSE );
                gen_code(op_fldz, 0, 0);
                gen_code(op_fucompp, 0, 0);
                if (apa->liveRegs & (1 << EAX))
                    gen_code(op_push, makedreg(EAX), 0);
                gen_codes(op_fstsw, ISZ_USHORT, makedreg(EAX), 0);
                gen_codes(op_bt, ISZ_UINT, makedreg(EAX), aimmed(10));
                gen_code(op_jnc, make_label(ulbl), 0);
                gen_codes(op_btr, ISZ_UINT, makedreg(EAX), aimmed(14));
                oa_gen_label(ulbl);
                gen_code(op_sahf, 0, 0);
                if (apa->liveRegs & (1 << EAX))
                    gen_code(op_pop, makedreg(EAX), 0);
            }
            else
            {
                gen_code(op_cmp, apl, aimmed(0));
            }
            gen_codes(op_setne, ISZ_UCHAR, apa, 0);
        }
        else if (q->dc.left->size >= ISZ_CFLOAT)
        {
            if (q->ans->size >= ISZ_CFLOAT)
            {
                floatLoad(apl1, szl - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
                floatLoad(apl, szl - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
                floatStore(apa, sza - ISZ_CFLOAT + ISZ_FLOAT);
                floatStore(apa1, sza - ISZ_CFLOAT + ISZ_FLOAT);
            }
            else if (q->ans->size >= ISZ_IFLOAT)
            {
                if (apl->mode == am_freg)
                {
                    gen_codes(op_fstp, ISZ_LDOUBLE, makefreg(0), 0);
                }
                else
                {
                    floatLoad(apl1, szl - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
                }
                floatStore(apa, sza - ISZ_IFLOAT + ISZ_FLOAT);
            }
            else
            {
                if (apl->mode == am_freg)
                {
                    floatStore(apa, sza);
                    gen_codes(op_fstp, ISZ_LDOUBLE, makefreg(0), 0);
                }
                else
                {
                    floatLoad(apl, szl - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
                    floatStore(apa, sza);
                }
            }
        }
        else if (q->dc.left->size >= ISZ_IFLOAT)
        {
            if (q->ans->size < ISZ_IFLOAT)
            {
                if (apl->mode == am_freg)
                    gen_codes(op_fstp, ISZ_LDOUBLE, fstack(), 0);
                gen_code(op_fldz, 0, 0);
                floatStore(apa, q->ans->size);
            }
            else
            {
                floatLoad(apl, q->dc.left->size - ISZ_IFLOAT + ISZ_FLOAT, FALSE);
                floatStore(apa, q->ans->size - ISZ_IFLOAT + ISZ_FLOAT);
            }
        }
        else if (q->dc.left->size >= ISZ_FLOAT)
        {
            floatLoad(apl, q->dc.left->size, FALSE);
            if (q->ans->size >= ISZ_FLOAT)
            {
                floatStore(apa, q->ans->size);
            }
            else 
            {
                diag("asm_assn: float to int conversion");
            }
        }
        else 
        {
            int size;
            /* this next size optimization is good for code gen,
             * but it is required for example if we were to try to
             * gen mov cl,(lowbyte(edi))
             *
             * the ans will be one of the lower four registers
             * in this case because of the way the registers
             * are declared in config.c
             */
             
addrdnjn:
            size = q->ans->size;
            if (apa->mode == am_dreg && apl->mode == am_dreg)			
                size = ISZ_UINT;
            else if (size <= ISZ_UCHAR && apl->mode == am_dreg && apl->preg > EBX)
                diag("asm_assn: Conversion of invalid reg to byte");
            if (q->ans->bits)
                bit_store(apa, apl, q->ans->size, q->ans->bits, q->ans->startbit);
            else
            {
                if (q->dc.opcode == i_assn && q->dc.left->bits)
                {
                    int max;
                    switch(szl)
                    {
                        case ISZ_UCHAR:
                            max = 8;
                            break;
                        case ISZ_USHORT:
                        case ISZ_U16:
                            max = 16;
                            break;
                        case ISZ_UINT:
                        case ISZ_ULONG:
                        case ISZ_U32:
                        default:
                            max = 32;
                            break;
                                                            
                    }
                    liveQualify(apa, apa, 0);
                    gen_codes(opl, size, apa, apl);
                    /* should be in a register at this point */
                    if (apa->mode != am_dreg)
                        diag("asm_assn: Expected register in bit processing");
                    if (q->dc.left->startbit)
                        gen_codes(op_shr, q->ans->size, apa, aimmed(q->dc.left->startbit));
                    if (q->dc.left->bits < max)
                        gen_codes(op_and, q->ans->size, apa, aimmed(( 1 << q->dc.left->bits) -1));
                }
                else
                {
                    gen_codes(opl, size, apa, apl);
                }
            }
        }
    }
    else /* q->ans->size > q->dc.left->size */
    {
        if (q->ans->size == ISZ_ADDR)
            if (q->dc.left->size == ISZ_ULONGLONG || q->dc.left->size == -ISZ_ULONGLONG)
            {
                goto addrdnjn;
            }
        if (q->ans->size >= ISZ_FLOAT)
        {
            if (q->dc.left->size >= ISZ_CFLOAT)
            {
                floatLoad(apl1, szl - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
                floatLoad(apl, szl - ISZ_CFLOAT + ISZ_FLOAT, FALSE);
                floatStore(apa, sza - ISZ_CFLOAT + ISZ_FLOAT);
                floatStore(apa1, sza - ISZ_CFLOAT + ISZ_FLOAT);
            }
            else if (q->dc.left->size >= ISZ_IFLOAT)
            {
                if (q->ans->size >= ISZ_CFLOAT)
                {
                    floatLoad(apl, szl - ISZ_IFLOAT + ISZ_FLOAT, FALSE);
                    gen_code(op_fldz, 0, 0);
                    {
                        floatStore(apa, sza - ISZ_CFLOAT + ISZ_FLOAT);
                        floatStore(apa1, sza - ISZ_CFLOAT + ISZ_FLOAT);
                    }
                }
                else if (q->ans->size >= ISZ_IFLOAT)
                {
                    floatLoad(apl, szl - ISZ_IFLOAT + ISZ_FLOAT, FALSE);
                    floatStore(apa, sza - ISZ_IFLOAT + ISZ_FLOAT);
                }
                else
                {
                    gen_code(op_fldz, 0, 0);
                    floatStore(apa, sza);
                }
            }
            else if (q->dc.left->size >= ISZ_FLOAT)
            {
                if (q->ans->size >= ISZ_CFLOAT)
                {
                    floatLoad(apl, szl, FALSE);
                    gen_code(op_fldz, 0, 0);
                    if (apa->mode == am_freg)
                        gen_code(op_fxch, 0, 0);
                    else
                    {
                        floatStore(apa1, sza - ISZ_CFLOAT + ISZ_FLOAT);
                        floatStore(apa, sza - ISZ_CFLOAT + ISZ_FLOAT);
                    }
                }
                else if (q->ans->size >= ISZ_IFLOAT)
                {
                    if (apl->mode == am_freg)
                        gen_codes(op_fstp, ISZ_LDOUBLE, fstack(), 0);
                    gen_code(op_fldz, 0, 0);
                    floatStore(apa, sza- ISZ_IFLOAT + ISZ_FLOAT);
                }
                else
                {
                    floatLoad(apl, q->dc.left->size, FALSE);
                    floatStore(apa, q->ans->size);
                }
            }
            else if (q->dc.left->size == ISZ_ULONGLONG || q->dc.left->size == - ISZ_ULONGLONG)
            {
                if (q->ans->size >= ISZ_IFLOAT)
                {
                    gen_code(op_fldz, 0, 0);
                    floatStore(apa, q->ans->size - ISZ_IFLOAT + ISZ_FLOAT);
                }
                else
                {
                    liveQualify( apl, apl1, 0);
                    gen_codes(op_push, ISZ_UINT, apl1, 0);
                    pushlevel+= 4;
                    gen_codes(op_push, ISZ_UINT, apl, 0);
                    pushlevel += 4;
                    gen_codes(op_fild, ISZ_ULONGLONG, make_stack(0), 0);
                    gen_codes(op_add, ISZ_UINT, makedreg(ESP), aimmed(8));
                    pushlevel -= 8;
                    if (q->ans->size >= ISZ_CFLOAT)
                    {
                        floatStore(apa, q->ans->size - ISZ_CFLOAT + ISZ_FLOAT);
                        gen_code(op_fldz, 0, 0);
                        floatStore(apa1, sza - ISZ_CFLOAT + ISZ_FLOAT);
                        if (apa->mode == am_freg)
                            gen_code(op_fxch, 0, 0);
                    }
                    else
                        floatStore(apa, q->ans->size);
                }
                
            }
            else if (q->dc.left->size == ISZ_ULONG || q->dc.left->size == - ISZ_ULONG)
            {
                if (q->ans->size >= ISZ_IFLOAT)
                {
                    gen_code(op_fldz, 0, 0);
                    floatStore(apa, q->ans->size - ISZ_IFLOAT + ISZ_FLOAT);
                }
                else
                {
                    gen_codes(op_push, ISZ_UINT, apl, 0);
                    gen_codes(op_fild, ISZ_UINT, make_stack(0), 0);
                    gen_codes(op_add, ISZ_UINT, makedreg(ESP), aimmed(4));
                    if (q->ans->size >= ISZ_CFLOAT)
                    {
                        floatStore(apa, q->ans->size - ISZ_CFLOAT + ISZ_FLOAT);
                        gen_code(op_fldz, 0, 0);
                        floatStore(apa1, sza - ISZ_CFLOAT + ISZ_FLOAT);
                        if (apa->mode == am_freg)
                            gen_code(op_fxch, 0, 0);
                    }
                    else
                        floatStore(apa, q->ans->size);
                }				
            }
            else
            {
                if (q->ans->size >= ISZ_IFLOAT)
                {
                    gen_code(op_fldz, 0, 0);
                    floatStore(apa, q->ans->size - ISZ_IFLOAT + ISZ_FLOAT);
                }
                else
                {
                    if (q->dc.left->size != ISZ_UINT && q->dc.left->size != - ISZ_UINT)
                    {
                        AMODE *ap = makedreg(ECX);
                        ap->liveRegs = q->liveRegs;
                        gen_codes(op_push, ISZ_UINT, ap, 0);
                        pushlevel += 4;
                        switch(q->dc.left->size)
                        {
                            case ISZ_BOOLEAN:
                                gen_codes(op_mov, ISZ_UINT, ap, apl);
                                gen_codes(op_and, ISZ_UINT, ap, aimmed(1));
                                break;
                            case ISZ_UCHAR:
                                do_movzx(ISZ_UINT, ISZ_UCHAR, ap, apl);
                                break;
                            case -ISZ_UCHAR:
                                do_movsx(ISZ_UINT, ISZ_UCHAR, ap, apl);
                                break;
                            case ISZ_USHORT:
                                do_movzx(ISZ_UINT, ISZ_USHORT, ap, apl);
                                break;
                            case - ISZ_USHORT:
                                do_movsx(ISZ_UINT, ISZ_USHORT, ap, apl);
                                break;
                            default:
                                break;
                        }
                        gen_codes(op_xchg, ISZ_UINT, ap, make_stack(0));
                        gen_codes(op_fild, ISZ_UINT, make_stack(0), 0);
                        gen_codes(op_add, ISZ_UINT, makedreg(ESP), aimmed(4));
                        pushlevel -= 4;
                    }
                    else
                    {
                        if (apl->mode == am_dreg)
                        {
                            gen_codes(op_push, ISZ_UINT, apl, 0);
                            gen_codes(op_fild, ISZ_UINT, make_stack(0), 0);
                            gen_codes(op_add, ISZ_UINT, makedreg(ESP), aimmed(4));
                        }
                        else
                            gen_codes(op_fild, ISZ_UINT, apl, 0);
                    }
                    if (q->ans->size >= ISZ_CFLOAT)
                    {
                        floatStore(apa, q->ans->size - ISZ_CFLOAT + ISZ_FLOAT);
                        gen_code(op_fldz, 0, 0);
                        floatStore(apa1, sza - ISZ_CFLOAT + ISZ_FLOAT);
                        if (apa->mode == am_freg)
                            gen_code(op_fxch, 0, 0);
                    }
                    else
                        floatStore(apa, q->ans->size);
                }
            }
        }
        else {
            AMODE *ap;
addrupjn:
            ap = apa;
            if (apa1)
            {
                liveQualify(apa1, apa, apa1);
                liveQualify(apa, apa, apa1);
            }
            if (ap->mode != am_dreg)
            {
                ap = apl;
                if (apa1)
                    liveQualify(apl, apa, apa1);
            }
            if (szl <= ISZ_ULONG || szl == ISZ_ADDR)
            {
                switch(q->dc.left->size)
                {
                    case ISZ_BOOLEAN:
                        if (ap->preg <= EBX)
                        {
                            gen_codes(op_mov, ISZ_UCHAR, ap, apl);
                            gen_codes(op_and, ISZ_UINT, ap, aimmed(1));
                        }
                        else
                        {
                            gen_codes(op_mov, ISZ_USHORT, ap, apl);
                            gen_codes(op_and, ISZ_UINT, ap, aimmed(1));
                        }
                        break;
                    case ISZ_UCHAR:
                        do_movzx(ISZ_UINT, ISZ_UCHAR, ap, apl);
                        break;
                    case -ISZ_UCHAR:
                        do_movsx(ISZ_UINT, ISZ_UCHAR, ap, apl);
                        break;
                    case ISZ_USHORT:
                    case ISZ_U16:
                        do_movzx(ISZ_UINT, ISZ_USHORT, ap, apl);
                        break;
                    case - ISZ_USHORT:
                        do_movsx(ISZ_UINT, ISZ_USHORT, ap, apl);
                        break;
                    case ISZ_U32:
                    case ISZ_UINT:
                    case - ISZ_UINT:
                    case ISZ_ULONG: 
                    case -ISZ_ULONG:
                    case ISZ_ADDR:
                        if (ap == apa)
                        {
                            if (q->ans->bits)
                                bit_store(ap, apl, q->ans->size, q->ans->bits, q->ans->startbit);
                            else
                            {
                                gen_codes(op_mov, ISZ_UINT, ap, apl);
                                if (q->dc.opcode == i_assn && q->dc.left->bits)
                                {
                                    /* should be in a register at this point */
                                    if (apa->mode != am_dreg)
                                        diag("asm_assn: Expected register in bit processing");
                                    if (q->dc.left->startbit)
                                    {
                                        liveQualify(apa, apa, 0);
                                        gen_codes(op_shr, q->ans->size, apa, aimmed(q->dc.left->startbit));
                                    }
                                    if (q->dc.left->bits < 32)
                                        gen_codes(op_and, q->ans->size, apa, aimmed(( 1 << q->dc.left->bits) -1));
                                }
                            }
                        }			
                        break;
                }
            }
            if (ap != apa)
            {
                if (q->ans->bits)
                    bit_store(ap, apl, q->ans->size, q->ans->bits, q->ans->startbit);
                else
                {
                    gen_codes(op_mov, ISZ_UINT, apa, ap);
                    peep_tail->oper1->liveRegs = q->liveRegs;
                    if (q->dc.opcode == i_assn && q->dc.left->bits)
                    {
                    int max;
                        switch(szl)
                        {
                            case ISZ_UCHAR:
                                max = 8;
                                break;
                            case ISZ_USHORT:
                            case ISZ_U16:
                                max = 16;
                                break;
                            case ISZ_UINT:
                            case ISZ_ULONG:
                            case ISZ_U32:
                            case ISZ_ADDR:
                            default:
                                max = 32;
                                break;
                                                                
                        }
                        /* should be in a register at this point */
                        if (apa->mode != am_dreg)
                            diag("asm_assn: Expected register in bit processing");
                        if (q->dc.left->startbit)
                        {
                            liveQualify(apa, apa, 0);
                            gen_codes(op_shr, q->ans->size, apa, aimmed(q->dc.left->startbit));
                        }
                        if (q->dc.left->bits < max)
                            gen_codes(op_and, q->ans->size, apa, aimmed(( 1 << q->dc.left->bits) -1));
                    }
                }
            }
            if (sza == ISZ_ULONGLONG)
            {
                if (q->dc.left->size < 0)
                {
                    if (apa1->mode == am_dreg && apa->mode == am_dreg && apa1->preg == EDX && apa->preg == EAX)
                    {
                        gen_code(op_cdq, NULL, NULL);
                    }
                    else
                    {
                        gen_codes(op_mov, ISZ_UINT, apa1, apa);
                        gen_codes(op_sar, ISZ_UINT, apa1, aimmed(31));
                    }
                }
                else
                    if (apa->mode == am_dreg)
                        gen_codes(op_sub, ISZ_UINT, apa1, apa1);
                    else
                        gen_codes(op_mov, ISZ_UINT, apa1, aimmed(0));
            }
        }
    }
}
void asm_genword(QUAD *q)            /* put a byte or word into the code stream */
{
    gen_code(op_genword, aimmed(q->dc.left->offset->v.i), 0);
}

void asm_coswitch(QUAD *q)           /* switch characteristics */
{
    enum e_op op;
     switch_deflab = q->dc.v.label;
    switch_range = q->dc.right->offset->v.i;
    switch_case_max = switch_case_count = q->ans->offset->v.i;
    switch_ip = q->dc.left;
    getAmodes(q, &op, switch_ip, &switch_apl, &switch_aph);
    switch_live = 0;
    if (switch_apl->mode == am_dreg)
        switch_live |= 1 << switch_apl->preg;
    if (switch_aph && switch_aph->mode == am_dreg)
        switch_live |= 1 << switch_aph->preg;
    if (switch_ip->size == ISZ_ULONGLONG || switch_ip->size == - ISZ_ULONGLONG || switch_case_max <= 5)
    {
        switch_mode = swm_enumerate;
    }
    else if (switch_case_max * 10 / switch_range > 8)
    {
        switch_mode = swm_compactstart;
    }
    else
    {
        switch_mode = swm_tree;
        if (!switchTreeLabelCount || switchTreeLabelCount  < switch_case_max)
        {
            free(switchTreeCases);
            free(switchTreeLabels);
            free(switchTreeBranchLabels);
            switchTreeLabelCount = (switch_case_max + 1024) & ~1023;
            switchTreeCases = (LLONG_TYPE *)calloc(switchTreeLabelCount, sizeof (LLONG_TYPE));
            switchTreeLabels = (int *)calloc(switchTreeLabelCount, sizeof (int));
            switchTreeBranchLabels = (int *)calloc(switchTreeLabelCount, sizeof (int));
        }
        switchTreePos = 0;
        memset(switchTreeBranchLabels, 0, sizeof(int) * switch_case_max);
    }
}
void asm_swbranch(QUAD *q)           /* case characteristics */
{
    ULLONG_TYPE swcase = q->dc.left->offset->v.i;
    int lab = q->dc.v.label;
    if (switch_case_count == 0)
    {
/*		diag("asm_swbranch, count mismatch"); in case only a default */
        return;
    }

    if (switch_mode == swm_compactstart)
    {
        compactSwitchHeader(swcase);		
    }
    switch(switch_mode)
    {
        case swm_enumerate:
        default:
            if (switch_ip->size == ISZ_ULONGLONG || switch_ip->size == -ISZ_ULONGLONG)
            {
                int nxlab = beGetLabel;
                gen_codes(op_cmp, ISZ_UINT, switch_apl, aimmed(swcase));
                gen_branch(op_jne, nxlab);
                peep_tail->oper1->liveRegs = switch_live;
#ifdef USE_LONGLONG
                gen_codes(op_cmp, ISZ_UINT, switch_aph, aimmed(swcase >> 32));
#else
                if (switch_ip->size < 0 && swcase < 0)
                    gen_codes(op_cmp, ISZ_UINT, switch_aph, aimmed(-1));
                else
                    gen_codes(op_cmp, ISZ_UINT, switch_aph, aimmed(0));
#endif
                gen_branch(op_je, lab);
                peep_tail->oper1->liveRegs = switch_live;
                oa_gen_label(nxlab);		
            }
            else
            {
                gen_codes(op_cmp, switch_ip->size, switch_apl, aimmed(swcase));
                gen_branch(op_je, lab);
                peep_tail->oper1->liveRegs = switch_live;
            }
            if (-- switch_case_count == 0)
            {
                gen_branch(op_jmp, switch_deflab);
                peep_tail->oper1->liveRegs = switch_live;
            }
            break ;
        case swm_compact:
            while(switch_lastcase < swcase)
            {
                gen_codes(op_dd, 0, make_label(switch_deflab), 0);
                switch_lastcase++;
            }
        case swm_compactstart:
            gen_codes(op_dd, 0, make_label(lab), 0);
            switch_lastcase = swcase + 1;
            switch_mode = swm_compact;
            -- switch_case_count;
            break ;
        case swm_tree:
            liveQualify(switch_apl, switch_apl, switch_aph);
            switchTreeCases[switchTreePos] = swcase;
            switchTreeLabels[switchTreePos++] = lab;
            if (--switch_case_count == 0)
                bingen(0, switch_case_max / 2, switch_case_max);
                
            break ;
    }
    
}
void asm_dc(QUAD *q)                 /* unused */
{
    (void)q;
}
void asm_assnblock(QUAD *q)          /* copy block of memory*/
{
    int n = q->ans->offset->v.i;
    AMODE *apl, *aph, *apal, *apah;
    enum e_op op, opa;
    EXPRESSION *ofs, *ofsa;
    
    getAmodes(q, &op, q->dc.right, &apl, &aph);
    getAmodes(q, &opa, q->dc.left, &apal, &apah);
        
    ofs = apl->offset ;
    ofsa = apal->offset;
    if (q->dc.right->mode == i_immed)
    {
        op = op_lea;
        if (ofs->type == en_auto)
        {
            if (usingEsp)
            {
                apl->preg = ESP;
            }
            else
            {
                apl->preg = EBP;
            }
            apl->mode = am_indisp;
        }
        else
            apl->mode = am_direct;
    }
    if (q->dc.left->mode == i_immed)
    {
        opa = op_lea;
        if (ofsa->type == en_auto)
        {
            if (usingEsp)
            {
                apal->preg = ESP;
            }
            else
            {
                apal->preg = EBP;
            }
            apal->mode = am_indisp;
        }
        else
            apal->mode = am_direct;
    }

    if (n <= 24 
        && (q->dc.right->mode == i_immed || apl->mode == am_dreg)
        && (q->dc.left->mode == i_immed || apal->mode == am_dreg))
    {
        AMODE *ax ;
        int reg = -1;
        int i;
        int push = FALSE;
        if (apl->mode == am_dreg)
        {
            apl->mode = am_indisp;
            ofs = intNode(en_c_i, 0);
        }
        if (apal->mode == am_dreg)
        {
            apal->mode = am_indisp;
            ofsa = intNode(en_c_i, 0);
        }
        for (i=0; i < 4; i++)
        {
            if (regmap[i][0] < 3 && !(q->liveRegs & ((ULLONG_TYPE)1 << i)))
            {
                if ((apl->mode != am_indisp || apl->preg != regmap[i][0])
                     && (apl->mode != am_indispscale  || ( apl->preg != regmap[i][0] && apl->sreg != regmap[i][0]))
                    && (apal->mode != am_indisp || apal->preg != regmap[i][0])
                     && (apal->mode != am_indispscale  || ( apal->preg != regmap[i][0] && apal->sreg != regmap[i][0])))
                {
                    reg = regmap[i][0];
                    break;
                }
            }		
        }
        if (reg == -1)
        {
            for (i=0; i < 6; i++)
            {
                if ((apl->mode != am_indisp || apl->preg != regmap[i][0])
                     && (apal->mode != am_indispscale  || ( apl->preg != regmap[i][0] && apl->sreg != regmap[i][0]))
                    && (apal->mode != am_indisp || apal->preg != regmap[i][0])
                     && (apal->mode != am_indispscale  || ( apal->preg != regmap[i][0] && apal->sreg != regmap[i][0])))
                {
                    reg = regmap[i][0];
                    push = TRUE;
                    break ;
                }
            }
                                         
        }
        ax = makedreg(reg);
        ax->liveRegs = q->liveRegs;
        if (push)
        {
            gen_codes(op_push, ISZ_UINT, ax, 0);
            pushlevel += 4;
        }
        if (n & 1)
        {
            apl->offset = exprNode(en_add, ofs, intNode(en_c_i, n-1));
            apal->offset = exprNode(en_add, ofsa, intNode(en_c_i, n-1));
            gen_codes(op_mov, ISZ_UCHAR, ax, apl);
            gen_codes(op_mov, ISZ_UCHAR, apal, ax);
            n--;
        }
        if (n & 2)
        {
            apl->offset = exprNode(en_add, ofs, intNode(en_c_i, n-2));
            apal->offset = exprNode(en_add, ofsa, intNode(en_c_i, n-2));
            gen_codes(op_mov, ISZ_USHORT, ax, apl);
            gen_codes(op_mov, ISZ_USHORT, apal, ax);
            n-=2;
        }
        
        while (n > 0)
        {
            n -= 4;
            apl->offset = exprNode(en_add, ofs, intNode(en_c_i, n));
            apal->offset = exprNode(en_add, ofsa, intNode(en_c_i, n));
            gen_codes(op_mov, ISZ_UINT, ax, apl);
            gen_codes(op_mov, ISZ_UINT, apal, ax);
        }
        if (push)
        {
            gen_codes(op_pop, ISZ_UINT, ax, 0);
            pushlevel -= 4;
        }
    }
    else
    {
        AMODE *cx = makedreg(ECX);
        AMODE *di = makedreg(EDI);
        AMODE *si = makedreg(ESI);
        cx->liveRegs = q->liveRegs;
        di->liveRegs = q->liveRegs;
        si->liveRegs = q->liveRegs;
        gen_codes(op_push, ISZ_UINT, di, 0);		
        gen_codes(op_push, ISZ_UINT, si, 0);
        gen_codes(op_push, ISZ_UINT, cx, 0);
        pushlevel += 12;
        if (samereg(di,apl))
        {
            if (samereg(si, apal))
            {
                gen_codes(op, ISZ_UINT, cx, apl);
                gen_codes(opa, ISZ_UINT, di, apal);
                gen_codes(op_mov, ISZ_UINT, si, cx);
            }
            else
            {
                gen_codes(op, ISZ_UINT, si, apl);
                gen_codes(opa, ISZ_UINT, di, apal);
            }
        }
        else
        {
            gen_codes(opa, ISZ_UINT, di, apal);
            gen_codes(op, ISZ_UINT, si, apl);
        }
        gen_codes(op_mov, ISZ_UINT, cx, aimmed(n/ 4));
        gen_code(op_cld, 0, 0);
        gen_code(op_rep, 0, 0);
        gen_code(op_movsd, 0, 0);
        if (n & 2)
            gen_code(op_movsw, 0, 0);
        if (n & 1)
            gen_code(op_movsb, 0, 0);
        gen_codes(op_pop, ISZ_UINT, cx, 0);
        gen_codes(op_pop, ISZ_UINT, si, 0);
        gen_codes(op_pop, ISZ_UINT, di, 0);
        pushlevel -= 12;
    }
    
}
void asm_clrblock(QUAD *q)           /* clear block of memory */
{
    int n = q->dc.right->offset->v.i;
    AMODE *apl, *aph;
    AMODE *aprl, *aprh;
    enum e_op op, opr;
    EXPRESSION *ofs;

    getAmodes(q, &opr, q->dc.right, &aprl, &aprh);    
    getAmodes(q, &op, q->dc.left, &apl, &aph);
    if (q->dc.right->mode != i_immed)
        n = INT_MAX;
    
    ofs = apl->offset ;
    if (q->dc.left->mode == i_immed)
    {
        op = op_lea;
        if (ofs->type == en_auto)
        {
            if (usingEsp)
            {
                apl->preg = ESP;
            }
            else
            {
                apl->preg = EBP;
            }
            apl->mode = am_indisp;
        }
        else
            apl->mode = am_direct;
    }

    if (n <= 24 && (q->dc.left->mode == i_immed || apl->mode == am_dreg))
    {
        if (apl->mode == am_dreg)
        {
            apl->mode = am_indisp;
            ofs = intNode(en_c_i, 0);
        }			
        if (n & 1)
        {
            apl->offset = exprNode(en_add, ofs, intNode(en_c_i, n-1));
            gen_codes(op_mov, ISZ_UCHAR, apl, aimmed(0));
            n--;
        }
        if (n & 2)
        {
            apl->offset = exprNode(en_add, ofs, intNode(en_c_i, n-2));
            gen_codes(op_mov, ISZ_USHORT, apl, aimmed(0));
            n-=2;
        }
        while (n > 0)
        {
            n -= 4;
            apl->offset = exprNode(en_add, ofs, intNode(en_c_i, n));
            gen_codes(op_mov, ISZ_ULONG, apl, aimmed(0));
        }
    }
    else
    {
        AMODE *ax = makedreg(EAX);
        AMODE *cx = makedreg(ECX);
        AMODE *di = makedreg(EDI);
        cx->liveRegs = q->liveRegs;
        di->liveRegs = q->liveRegs;
        gen_codes(op_push, ISZ_UINT, di, 0);		
        gen_codes(op_push, ISZ_UINT, cx, 0);
        gen_codes(op_push, ISZ_UINT, ax, 0);
        pushlevel += 12;
        gen_codes(op_xor, ISZ_UINT, ax, ax);
        if (n == INT_MAX)
        {
            gen_codes(op_mov, ISZ_UINT, cx, aprl);
            gen_codes(op_shr, ISZ_UINT, cx, aimmed(2));
        }
        else
        {
            gen_codes(op_mov, ISZ_UINT, cx, aimmed(n/ 4));
        }
        gen_codes(op, ISZ_UINT, di, apl);
        gen_code(op_cld, 0, 0);
        gen_code(op_rep, 0, 0);
        gen_code(op_stosd, 0, 0);
        if (n & 2)
            gen_code(op_stosw, 0, 0);
        if (n & 1)
            gen_code(op_stosb, 0, 0);
        gen_codes(op_pop, ISZ_UINT, ax, 0);
        gen_codes(op_pop, ISZ_UINT, cx, 0);
        gen_codes(op_pop, ISZ_UINT, di, 0);
        pushlevel -= 12;
    }
}
void asm_jc(QUAD *q)                 /* branch if a U< b */
{
    gen_goto(q, op_jc, op_ja, op_jc, op_ja, op_jb, op_jb);
    
}
void asm_ja(QUAD *q)                 /* branch if a U> b */
{
    gen_goto(q, op_ja, op_jc, op_ja, op_jc, op_ja, op_ja);
    
}
void asm_je(QUAD *q)                 /* branch if a == b */
{
    gen_goto(q, op_je, op_jne, op_je, op_jne, op_je, op_je);
    
}
void asm_jnc(QUAD *q)                /* branch if a U>= b */
{
    gen_goto(q, op_jnc, op_jbe, op_ja, op_jc, op_jae, op_jae);
    
}
void asm_jbe(QUAD *q)                /* branch if a U<= b */
{
    gen_goto(q, op_jbe, op_jnc, op_jc, op_ja, op_jbe, op_jbe);
    
}
void asm_jne(QUAD *q)                /* branch if a != b */
{
    gen_goto(q, op_jne, op_je, op_jne, op_je, op_jne, op_jne);
    
}
void asm_jl(QUAD *q)                 /* branch if a S< b */
{
    gen_goto(q, op_jl, op_jg, op_jl, op_jg, op_jb, op_jb);

}
void asm_jg(QUAD *q)                 /* branch if a S> b */
{
       gen_goto(q, op_jg, op_jl, op_jg, op_jl, op_ja, op_ja);

}
void asm_jle(QUAD *q)                /* branch if a S<= b */
{
    gen_goto(q, op_jle, op_jge, op_jl, op_jg, op_jbe, op_jbe);
    
}
void asm_jge(QUAD *q)                /* branch if a S>= b */
{
    gen_goto(q, op_jge, op_jle, op_jg, op_jl, op_jae, op_jae);
    
}
void asm_cppini(QUAD *q)             /* cplusplus initialization (historic)*/
{
    (void)q;    
}
/*
 * function prologue.  left has a constant which is a bit mask
 * of registers to push.  It also has a flag indicating whether frames
 * are absolutely necessary
 *
 * right has the number of bytes to allocate on the stack
 */
void asm_prologue(QUAD *q)           /* function prologue */
{
    inframe = !!(beGetIcon(q->dc.left) & FRAME_FLAG_NEEDS_FRAME) 
            || cparams.prm_debug || cparams.prm_stackalign;
    if (inframe)
    {
        int n = beGetIcon(q->dc.right) ;
        
        if (cparams.prm_stackalign && beGetIcon(q->dc.left) != 0)
        {
            // adjust for pushed regs
            int cnt = 0;
            int mask=1, compare;
            compare = (unsigned)(beGetIcon(q->dc.left) & ~(FRAME_FLAG_NEEDS_FRAME));
            while (mask <= compare)
            {
                if (mask & compare)
                    cnt += 4;
                mask <<= 1;
            }
            if (cnt % cparams.prm_stackalign)
                cnt = cparams.prm_stackalign - cnt % cparams.prm_stackalign;
            n += cnt;
        }
        /* enter is *really* inefficient so we will not use it */
        if (!usingEsp)
        {
            gen_code(op_push, makedreg(EBP), 0);
            gen_code(op_mov, makedreg(EBP), makedreg(ESP));
        }
        pushlevel = 0;
        if (n)
        {
            if (n < 16)
            {
                int i;
                for (i=0; i < n; i+=4)
                    gen_code(op_push, makedreg(ECX), 0);
            }
            else if (n <= 4092)
            {
                gen_code(op_add, makedreg(ESP), aimmed(-n));
            }
            else
            {
                if (n <= 8188)
                {
                    gen_code(op_add, makedreg(ESP), aimmed(-4092));
                    gen_code(op_push, makedreg(EAX), 0);
                    gen_code(op_add, makedreg(ESP), aimmed(-(n-4096)));
                }
                else
                {
                    n += 15; // must match the RTL...
                    n &= -16; //
                    gen_code(op_push, aimmed(n), 0);
                    callLibrary("___substackp", 0);
                }
            }
          }
        funcstackheight = n;
    }
    else
    {
        funcstackheight = 0;
    }
    if (beGetIcon(q->dc.left) != 0)
    {
        int cnt = 0;
        int mask=1, compare;
        compare = (unsigned)(beGetIcon(q->dc.left) & ~(FRAME_FLAG_NEEDS_FRAME));
        while (mask <= compare)
        {
            if (mask & compare)
            {
                funcstackheight += 4;
                gen_code(op_push, makedreg(regmap[cnt][0]), 0);
            }
            cnt++, mask <<= 1;
        }
    }
}
/*
 * function epilogue, left holds the mask of which registers were pushed
 */
void asm_epilogue(QUAD *q)           /* function epilogue */
{
    if (pushlevel != 0 && usingEsp)
        diag("asm_epilogue: pushlevel not aligned");
    if (beGetIcon(q->dc.left) != 0)
    {
        int mask=0x80, compare;
        int cnt = 7;
        compare = (unsigned)(beGetIcon(q->dc.left) & ~(FRAME_FLAG_NEEDS_FRAME));
        while (mask)
        {
            if (mask & compare)
            {
                gen_code(op_pop, makedreg(regmap[cnt][0]), 0);
                funcstackheight -= 4;
            }
            cnt--, mask >>= 1;
        }
    }    
    if (inframe)
    {
        if (usingEsp)
        {
            if (funcstackheight <= 16)
            {
                int i;
                for (i=0; i < funcstackheight; i += 4)
                    gen_code(op_pop, makedreg(ECX), 0);
            }
            else
            {
                gen_code(op_add, makedreg(ESP), aimmed(funcstackheight));
            }
        }
        else
        {
            if (beGetIcon(q->dc.right))
                gen_code(op_mov, makedreg(ESP), makedreg(EBP));
            gen_code(op_pop, makedreg(EBP), 0);
        }
    }
}
/*
 * in an interrupt handler, push the current context
 */
void asm_pushcontext(QUAD *q)        /* push register context */
{
    (void)q;
        if (cparams.prm_farkeyword)
        {
            gen_code(op_push, makesegreg(ES),0);
            gen_code(op_push, makesegreg(FS),0);
            gen_code(op_push, makesegreg(GS),0);
        }
        gen_code(op_pushad, 0, 0);
    
}
/*
 * in an interrupt handler, pop the current context
 */
void asm_popcontext(QUAD *q)         /* pop register context */
{
    (void)q;
        gen_code(op_popad, 0, 0);
        if (cparams.prm_farkeyword)
        {
            gen_code(op_pop, makesegreg(GS),0);
            gen_code(op_pop, makesegreg(FS),0);
            gen_code(op_pop, makesegreg(ES),0);
        }    
}
/*
 * loads a context, e.g. for the loadds qualifier
 */
void asm_loadcontext(QUAD *q)        /* load register context (e.g. at interrupt level ) */
{
    (void)q;
    gen_code(op_push, makesegreg(DS), 0);
    gen_code(op_push, makesegreg(CS), 0);
    gen_codes(op_add, 2, make_stack(0), aimmed(8));
    gen_code(op_pop, makesegreg(DS), 0);
    
}
/*
 * unloads a context, e.g. for the loadds qualifier
 */
void asm_unloadcontext(QUAD *q)        /* load register context (e.g. at interrupt level ) */
{
    (void)q;
    gen_code(op_pop, makesegreg(DS), 0);
    
}
void asm_tryblock(QUAD *q)			 /* try/catch */
{
    AMODE *ap1 = beLocalAlloc(sizeof(IMODE));
    ap1->mode = am_indisp;
    if (usingEsp)
    {
        ap1->preg = ESP;
        ap1->offset = intNode(en_c_i,  q->dc.v.label + funcstackheight); // ESP
    }
    else
    {
        ap1->preg = EBP;
        ap1->offset = intNode(en_c_i,  q->dc.v.label); // ESP
    }

    switch((int)q->dc.left->offset->v.i)
    {
        case 0: /* try */
            gen_codes(op_push, ISZ_UINT , ap1, 0);
            gen_codes(op_mov, ISZ_UINT , ap1, makedreg(ESP));
            break;
        case 1: /* catch */
            gen_codes(op_mov, ISZ_UINT , makedreg(ESP), ap1);
            gen_codes(op_pop, ISZ_UINT , ap1, 0);
            break;
        case 2: /* end block */
            gen_codes(op_mov, ISZ_UINT , makedreg(ESP), ap1);
            gen_codes(op_pop, ISZ_UINT , ap1, 0);
            break;
        default:
            diag("asm_tryblock: Unknown tryblock type");
            break;
    }
}
void asm_stackalloc(QUAD *q)         /* allocate stack space - positive value = allocate(QUAD *q) negative value deallocate */
{
    enum e_op op;
    AMODE *apl, *aph;
    getAmodes(q, &op, q->dc.left, &apl, &aph);
    if (apl->mode == am_immed)
    {
        int n = apl->offset->v.i;
        if (n)
        {
            if (n < 0)
                n = (n-3) & ~3;
            else
                n = (n + 3) & ~3;
            
            if (n <= 4092) // also gets adds to the stack pointer
                gen_code(op_add, makedreg(ESP), aimmed(-n));
            else
            {
                if (n <= 8188)
                {
                    gen_code(op_add, makedreg(ESP), aimmed(-4092));
                    gen_code(op_push, makedreg(EAX), 0);
                    gen_code(op_add, makedreg(ESP), aimmed(-(n-4096)));
                }
                else
                {
                    if (n < 0) // must match rtl
                        n = (n-15) & ~15;
                    else
                        n = (n + 15) & ~15;
                    gen_code(op_push, aimmed(n), 0);
                    callLibrary("___substackp", 0);
                }
            }
            pushlevel += n;
            if (q->ans)
            {
                getAmodes(q, &op, q->ans, &apl, &aph);
                gen_codes(op_mov, ISZ_UINT, apl, makedreg(ESP));
            }
        }
    }
    else
    {
        gen_code(op_push, apl, 0);
        callLibrary("___substackp", 0); // substack does paragraph rounding already...
        if (q->ans)
        {
            getAmodes(q, &op, q->ans, &apl, &aph);
            gen_codes(op_mov, ISZ_UINT, apl, makedreg(ESP));
        }
    }
}
void asm_loadstack(QUAD *q)			/* load the stack pointer from a var */
{
    AMODE *apl, *aph;
    enum e_op op;
    
    getAmodes(q, &op, q->dc.left, &apl, &aph);
    apl->liveRegs = q->liveRegs;
    gen_codes(op_mov, ISZ_UINT, makedreg(ESP), apl);
}
void asm_savestack(QUAD *q)			/* save the stack pointer to a var */
{
    AMODE *apl, *aph;
    enum e_op op;
    
    getAmodes(q, &op, q->dc.left, &apl, &aph);
    apl->liveRegs = q->liveRegs;
    gen_codes(op_mov, ISZ_UINT, apl, makedreg(ESP));    
}
void asm_functail(QUAD *q, int begin, int size)	/* functail start or end */
{
    enum e_op op = op_push;
    (void)q;
    if (!begin)
        op = op_pop;
    if (size == ISZ_ULONGLONG)
    {
        if (begin)
            gen_code(op, makedreg(EDX),0);
        gen_code(op, makedreg(EAX),0);
        if (!begin)
            gen_code(op, makedreg(EDX),0);
    }
    else if (size < ISZ_FLOAT)
    {
        gen_code(op, makedreg(EAX),0);
    }
}
void asm_atomic(QUAD *q)
{
    int needsync = q->dc.left->offset->v.i;
    if (needsync <0)
        needsync = 0;
    // direct store has bit 7 set...
    // well some sources say aquire and release fences on the x86 are superfluous,
    // (although clearly the mutual fence isn't)
    // but, I'm going to take the tack that since they are there there must be
    // some use for them and emit them if specified...
    if (needsync == mo_acquire || needsync == mo_acq_rel)
        gen_code(op_lfence, NULL, NULL);
    switch(q->dc.opcode)
    {
        enum e_op opa;
        enum e_op opl;
        enum e_op opr;
        AMODE *apal, *apah, *apll, *aplh, *aprl, *aprh;
        int lbl1, lbl2;
        case i_atomic_fence:
            if (needsync == mo_seq_cst) // in this case the value may be mo_seq_cst + 0x80 for store,
                                        // for the x86 we are only genning these on loads though...
            {
                gen_code(op_mfence, NULL, NULL);
            }
            break;
        case i_atomic_flag_fence:
               getAmodes(q, &opl, q->dc.right, &apll, &aplh);
            apll->mode = am_indisp;
            apll->offset = intNode(en_c_i, 0);
            if (q->dc.left->offset->v.i > 0)
            {
                lbl1 = beGetLabel;
                lbl2 = beGetLabel;
                oa_gen_label(lbl1);
                gen_code(op_lock, NULL, NULL);
                gen_code(op_bts, apll, aimmed(0));
                gen_code(op_jnc, make_label(lbl2),0);
                callLibrary("___atomic_yield", 0);
                gen_code(op_jmp, make_label(lbl1), 0);
                oa_gen_label(lbl2);
            }
            else
            {
                gen_code(op_lock, NULL, NULL);
                gen_code(op_btc, apll, aimmed(0));
            }
            break;
        case i_atomic_flag_test_and_set:
            if (needsync == mo_seq_cst)
            {
                gen_code(op_mfence, NULL, NULL);
            }
            getAmodes(q, &opl, q->dc.right, &apll, &aplh);
            getAmodes(q, &opa, q->ans, &apal, &apah);
            gen_code(op_lock, NULL, NULL);
            gen_code(op_bts, apll, aimmed(0));
            gen_code(op_mov, apal, aimmed(0));
            gen_codes(op_setc, ISZ_UCHAR, apal,0);
            break;
        case i_atomic_flag_clear:
            getAmodes(q, &opl, q->dc.right, &apll, &aplh);
            gen_code(op_lock, NULL, NULL);
            gen_code(op_btc, apll, aimmed(0));
            break;
        case i_cmpswp:
            if (needsync == mo_seq_cst)
            {
                gen_code(op_mfence, NULL, NULL);
            }
        /*
            address == address2 ? address = value : address2 = address
            rv         right      rv        left    right      rv 
            dest       accum      dest     src      accum      dest
                                    zf              nz
                                    true            false
            rm32       eax        rm32      reg     eax        rm32
            
          */
          {
            int sz = q->ans->size;
            getAmodes(q, &opr, q->dc.right, &aprl, &aprh);            
            getAmodes(q, &opl, q->dc.left, &apll, &aplh);
            getAmodes(q, &opa, q->ans, &apal, &apah);
            if (sz == ISZ_ULONGLONG || sz == - ISZ_ULONGLONG)
            {
                if (apll->liveRegs & (1 << EBX))
                {
                    gen_code(op_push, makedreg(EBX), NULL);
                }
                if (apll->liveRegs & (1 << ECX))
                {
                    gen_code(op_push, makedreg(ECX), NULL);
                }
                if (apll->liveRegs & (1 << EDX))
                {
                    gen_code(op_push, makedreg(EDX), NULL);
                }
                gen_codes(op_push, ISZ_UINT, aplh, NULL);
                gen_codes(op_push, ISZ_UINT, apll, NULL);
                gen_codes(op_pop, ISZ_UINT, makedreg(EBX), NULL);
                gen_codes(op_pop, ISZ_UINT, makedreg(ECX), NULL);
                
                gen_codes(op_mov, ISZ_UINT, makedreg(EAX), aprl);
                gen_codes(op_mov, ISZ_UINT, makedreg(EDX), aprh);
                gen_code(op_lock, NULL, NULL);
                gen_codes(op_cmpxchg8b, ISZ_ULONGLONG, apal, NULL);
                gen_codes(op_mov, ISZ_UINT, aprl, makedreg(EAX));
                gen_codes(op_mov, ISZ_UINT, aprh, makedreg(EDX));

                if (apll->liveRegs & (1 << EDX))
                {
                    gen_code(op_pop, makedreg(EDX), NULL);
                }
                if (apll->liveRegs & (1 << ECX))
                {
                    gen_code(op_pop, makedreg(ECX), NULL);
                }
                if (apll->liveRegs & (1 << EBX))
                {
                    gen_code(op_pop, makedreg(EBX), NULL);
                }
            }
            else
            {
                int reg=-1, push = FALSE, push1 = FALSE, push2 = FALSE;
                if (apll->mode != am_dreg)
                {
                    int reg1=-1, reg2=-1;
                    if (apal->mode == am_indisp)
                    {
                        reg1 = apal->preg;
                    }
                    else if (apal->mode == am_indispscale)
                    {
                        reg1 = apal->preg;
                        reg2 = apal->sreg;
                    }
                    if (!(apll->liveRegs & (1 << ECX)) && reg1 != ECX && reg2 != ECX)
                    {
                        reg = ECX;
                    }
                    else if (!(apll->liveRegs & (1 << EDX)) && reg1 != EDX && reg2 != EDX)
                    {
                        reg = EDX;
                    }
                    else if (!(apll->liveRegs & (1 << EBX)) && reg1 != EBX && reg2 != EBX)
                    {
                        reg = EBX;
                    }
                    else if (!(apll->liveRegs & (1 << ESI)) && reg1 != ESI && reg2 != ESI)
                    {
                        reg = ESI;
                    }
                    else if (!(apll->liveRegs & (1 << EDI)) && reg1 != EDI && reg2 != EDI)
                    {
                        reg = EDI;
                    }
                    if (reg == -1)
                    {
                        reg = EDX;
                        push1 = TRUE;
                        gen_code(op_push, makedreg(EDX), NULL);
                        pushlevel += 4;
                    }
                    gen_codes(op_mov, sz, makedreg(reg), apll);
                }
                if ((aprl->mode == am_indisp && aprl->preg == EAX )
                    || (aprl->mode == am_indispscale && (aprl->preg == EAX || aprl->sreg == EAX)))
                {
                    push2 = TRUE;
                    gen_code(op_push, makedreg(EAX), NULL);
                    pushlevel += 4;
                }
                gen_codes(op_mov, sz, makedreg(EAX), aprl);
                gen_code(op_lock, NULL, NULL);
                gen_codes(op_cmpxchg, sz, apal, makedreg(reg));
                if (push2)
                {
                    gen_codes(op_mov, sz, makedreg(EDX), makedreg(EAX));
                    gen_code(op_pop, makedreg(EAX), NULL);
                    pushlevel -= 4;
                    gen_codes(op_mov, sz, aprl, makedreg(EDX));
                }
                else
                {
                    gen_codes(op_mov, sz, aprl, makedreg(EAX));
                }
                if (push)
                {
                    
                    gen_code(op_pop, apll, NULL);
                    pushlevel -= 4;
                }
                else if (push1)
                {
                    gen_code(op_pop, makedreg(EDX), NULL);
                    pushlevel -= 4;
                }
            }
            gen_code(op_mov, makedreg(EAX), aimmed(0));
            gen_codes(op_setz, ISZ_UCHAR, makedreg(EAX), NULL);
          }
            break;
        default:
            break;
    }
    if (needsync == mo_release || needsync == mo_acq_rel)
        gen_code(op_sfence, NULL, NULL);
}