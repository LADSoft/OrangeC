 /* 
CC386 C Compiler
Copyright 1994-2011 David Lindauer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

This program is derived from the cc68k complier by 
Matthew Brandt (mailto::mattb@walkingdog.net) 

You may contact the author of this derivative at:

mailto::camille@bluegrass.net
 */
#include <stdio.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"
#include "diag.h"
extern int block_nesting;
extern SYM *currentfunc,  *declclass;
extern TABLE lsyms;
extern int stdretblocksize, stdaddrsize, stdintsize;
extern int global_flag;
extern TYP stdint;
extern LIST *instantiated_inlines;

static TABLE xsyms;
static SNODE *argstmt,  **argnext;
static ENODE *thisn;
static int namenumber;
static SYM *inlinesp;

void inlineinit(void)
{
    namenumber = 0;
}

//-------------------------------------------------------------------------


//-------------------------------------------------------------------------

static SYM *inlineSearch(SYM *name)
{
    SYM *sp = xsyms.head;
    while (sp)
    {
        if (sp->mainsym == name->mainsym)
            return sp;
        sp = sp->next;
    }
    return 0;
}

//-------------------------------------------------------------------------

static void inlineinsert(SYM *sp)
{
    if (xsyms.head)
        xsyms.tail = xsyms.tail->next = sp;
    else
        xsyms.head = xsyms.tail = sp;
}

static LIST *symsToBlock(SYM *sp)
{
    LIST *p = NULL, **hold = &p;
    while (sp)
    {
        LIST *n = xalloc(sizeof(LIST));
        n->data = sp;
        *hold = n;
        hold = &(*hold)->link;
        sp = sp->next;
    }
    return p;
}
static void insertInlineSyms(SYM *sp)
{
    LIST *l;
    global_flag++;
    l = sp->value.classdata.inlinefunc->syms;
    while (l)
    {
        SYM *head = l->data;
        if (head != (SYM*) - 1)
        {
            while (head)
            {
                if (!inlineSearch(head))
                {
                    SYM *v = copysym(head);
                    if (!v->funcparm)
                    {
                        head->mainsym->value.classdata.inlinefunc = makenode(en_autocon, v, NULL);
                        v->value.i = block_nesting + 1;
                    }
                    v->funcparm = FALSE;
                    inlineinsert(v);
                }
                head = head->next;
            }
        }
        l = l->link;
    }
    global_flag--;
}
static SYM *copyInlineFunc(SYM *sp)
{
    INLINEFUNC *old = sp->value.classdata.inlinefunc;
    sp = copysym(sp);
    insertInlineSyms(sp);
    sp->value.classdata.inlinefunc = xalloc(sizeof(INLINEFUNC));
    sp->value.classdata.inlinefunc->stmt = inlinestmt(old->stmt)
        ;
    sp->value.classdata.inlinefunc->syms = old->syms;
    return sp;
}
//-------------------------------------------------------------------------

static ENODE *inlineexpr(ENODE *node)
{
    /*
     * routine takes an enode tree and replaces it with a copy of itself.
     * Used because we have to munge the block_nesting field (value.i) of each
     * sp in an inline function to force allocation of the variables
     */
    ENODE *temp,  *temp1;
    if (node == 0)
        return 0;
    temp = xalloc(sizeof(ENODE));
    memcpy(temp, node, sizeof(ENODE));
    switch (temp->nodetype)
    {
        case en_structret:
            break;
        case en_autoreg:
        case en_autocon:
            if (!strcmp(temp->v.sp->name, "**THIS**"))
            {
                temp = copy_enode(thisn);
            }
            else
            {
                /* this should never fail */
                temp = node->v.sp->mainsym->value.classdata.inlinefunc;
                if (!temp)
                {
                    DIAG("Unfound SP in inlineexpr");
                    temp = dummyvar(4, &stdint, 0);
                }
            }
            break;
        case en_llcon:
        case en_llucon:
        case en_rcon:
        case en_lrcon:
        case en_fcon:
        case en_rcomplexcon:
        case en_lrcomplexcon:
        case en_fcomplexcon:
        case en_rimaginarycon:
        case en_lrimaginarycon:
        case en_fimaginarycon:
        case en_icon:
        case en_lcon:
        case en_iucon:
        case en_lucon:
        case en_ccon:
        case en_boolcon:
        case en_cucon:
            break;
        case en_nacon:
        case en_absacon:
        case en_nalabcon:
        case en_napccon:
            break;
        case en_fp_ref:
        case en_bits:
        case en_floatref:
        case en_doubleref:
        case en_longdoubleref:
        case en_fimaginaryref:
        case en_rimaginaryref:
        case en_lrimaginaryref:
        case en_fcomplexref:
        case en_rcomplexref:
        case en_lrcomplexref:
        case en_b_ref:
        case en_w_ref:
        case en_ul_ref:
        case en_l_ref:
		case en_a_ref:
		case en_ua_ref:
		case en_i_ref:
		case en_ui_ref:
        case en_ub_ref:
        case en_uw_ref:
        case en_bool_ref:
        case en_ll_ref:
        case en_ull_ref:
            temp1 = temp;
            while (temp1->nodetype == en_a_ref) // explicitly ignoring the UAREF nodes here to keep from collapsing them
                temp1 = temp1->v.p[0];
            if (temp1->nodetype == en_autocon && !strcmp(temp1->v.sp->name, 
                "**THIS**"))
            {
                temp = copy_enode(thisn);
                break;
            }
			if (temp->v.p[0]->nodetype == en_autocon || temp->v.p[0]->nodetype == en_autoreg)
			{
				SYM *sp1, *sp2;
				sp1 = temp->v.p[0]->v.sp;
	            temp1 = temp->v.p[0] = inlineexpr(node->v.p[0]);
				sp2 = temp->v.p[0]->v.sp;
				deref(&temp1, sp1->tp);
				promote_type(sp2->tp, &temp1);
				temp = temp1;
			}
			else
	            temp1 = temp->v.p[0] = inlineexpr(node->v.p[0]);
            break;
        case en_uminus:
        case en_asuminus:
        case en_ascompl:
        case en_compl:
        case en_not:
        case en_cf:
        case en_cd:
        case en_cld:
        case en_cfi:
        case en_cri:
        case en_clri:
        case en_cfc:
        case en_crc:
        case en_clrc:
        case en_cll:
        case en_cull:
		case en_ci:
		case en_cui:
        case en_cb:
        case en_cub:
        case en_cbool:
        case en_cw:
        case en_cuw:
        case en_cl:
        case en_cul:
        case en_cp:
        case en_cfp:
        case en_csp:
        case en_trapcall:
        case en_cl_reg:
        case en_movebyref:
        case en_substack:
		case en_loadstack:
		case en_savestack:
            temp->v.p[0] = inlineexpr(node->v.p[0]);
            break;
        case en_asadd:
        case en_assub:
        case en_ainc:
        case en_adec:
        case en_add:
        case en_sub:
        case en_addstruc:
        case en_addcast: 
		case en_asalsh:
        case en_asarsh:
        case en_alsh:
        case en_arsh:
        case en_arshd:
        case en_asarshd:
        case en_asmul:
        case en_asdiv:
        case en_asmod:
        case en_aslsh:
        case en_asumod:
        case en_asudiv:
        case en_asumul:
        case en_asrsh:
        case en_asand:
        case en_assign:
        case en_refassign:
        case en_lassign:
        case en_asor:
        case en_asxor:
        case en_void:
        case en_dvoid:
        case en_pmul:
		case en_arrayindex:
        case en_pdiv:
        case en_mul:
        case en_div:
        case en_umul:
        case en_udiv:
        case en_umod:
        case en_lsh:
        case en_rsh:
        case en_mod:
        case en_and:
        case en_or:
        case en_xor:
        case en_lor:
        case en_land:
        case en_eq:
        case en_ne:
        case en_gt:
        case en_ge:
        case en_lt:
        case en_le:
        case en_ugt:
        case en_uge:
        case en_ult:
        case en_ule:
        case en_cond:
        case en_intcall:
        case en_moveblock:
        case en_stackblock:
        case en_thiscall:
        case en_repcons:
        case en_callblock:
        case en_pcallblock:
        case en_scallblock:
		case en_array:
            temp->v.p[1] = inlineexpr(node->v.p[1]);
        case en_clearblock:
            temp->v.p[0] = inlineexpr(node->v.p[0]);
            break;
        case en_fcall:
        case en_fcallb:
        case en_pfcall:
        case en_pfcallb:
        case en_sfcall:
        case en_sfcallb:
            temp->v.p[0] = inlineexpr(node->v.p[0]);
            temp->v.p[1] = inlineexpr(node->v.p[1]);
            if (temp->v.p[1]->v.p[0]->v.p[0]->nodetype == en_napccon)
            {
                SYM *sp = temp->v.p[1]->v.p[0]->v.p[0]->v.sp;
                if (sp->value.classdata.inlinefunc && (sp
                    ->value.classdata.cppflags &PF_INLINE) && !(sp
                    ->value.classdata.cppflags &PF_INSTANTIATED))
                {
                    sp = copyInlineFunc(sp);
                    temp->v.p[1]->v.p[0]->v.p[0]->v.sp = sp;
                }
            }
            break;
        case en_conslabel:
        case en_destlabel:
            temp->v.p[0] = inlineexpr(node->v.p[0]);
            temp->v.p[1] = node->v.p[1];
            break;
        case en_labcon:
		case en_regref:
            break;
        default:
            DIAG("Invalid expr type in inlineexpr");
    }
    return temp;
}

//-------------------------------------------------------------------------
#ifdef i386
#include "gen386.h"
#endif
static ENODE *asm_inlinestmt(ENODE *e)
{
#ifdef i386
	OCODE *rv = xalloc(sizeof(OCODE));
	memcpy(rv, e, sizeof(*rv));
	if (rv->opcode != op_label && rv->opcode != op_line)
	{
		AMODE *ap = rv->oper1;
		if (ap && ap->offset)
			ap->offset = inlineexpr(ap->offset);
		ap = rv->oper2;
		if (ap && ap->offset)
			ap->offset = inlineexpr(ap->offset);
		ap = rv->oper3;
		if (ap && ap->offset)
			ap->offset = inlineexpr(ap->offset);
	}
	return rv;
#else
    return e;
#endif
}

//-------------------------------------------------------------------------

static SNODE *inlinestmt(SNODE *block)
/*
 *      scan will gather all optimizable expressions into the expression
 *      list for a block of statements.
 */
{
    SNODE *out = 0,  **outptr = &out;
    while (block != 0)
    {
        *outptr = xalloc(sizeof(SNODE));
        memcpy(*outptr, block, sizeof(SNODE));
        (*outptr)->next = NULL;
        switch (block->stype)
        {
			case st__genword:
				break;
            case st_tryblock:
                break;
            case st_throw:
                if (block->exp)
                {
                    (*outptr)->exp = inlineexpr(block->exp);
                    opt4(&(*outptr)->exp, FALSE);
                }
                break;
            case st_return:
            case st_expr:
                (*outptr)->exp = inlineexpr(block->exp);
                opt4(&(*outptr)->exp, FALSE);
                break;
            case st_while:
            case st_do:
                (*outptr)->exp = inlineexpr(block->exp);
                (*outptr)->s1 = inlinestmt(block->s1);
                opt4(&(*outptr)->exp, FALSE);
                break;
            case st_for:
                (*outptr)->label = inlineexpr(block->label);
                (*outptr)->exp = inlineexpr(block->exp);
                (*outptr)->s1 = inlinestmt(block->s1);
                (*outptr)->s2 = inlineexpr(block->s2);
                opt4(&(*outptr)->exp, FALSE);
                opt4(&(*outptr)->label, FALSE);
                break;
            case st_if:
                (*outptr)->exp = inlineexpr(block->exp);
                (*outptr)->s1 = inlinestmt(block->s1);
                (*outptr)->s2 = inlinestmt(block->s2);
                opt4(&(*outptr)->exp, FALSE);
                break;
            case st_switch:
                (*outptr)->exp = inlineexpr(block->exp);
                (*outptr)->s1 = inlinestmt(block->s1);
                opt4(&(*outptr)->exp, FALSE);
                break;
            case st_case:
                (*outptr)->s1 = inlinestmt(block->s1);
                break;
            case st_block:
                (*outptr)->exp = inlinestmt(block->exp);
                break;
            case st_asm:
                if ((*outptr)->exp)
                    (*outptr)->exp = asm_inlinestmt((*outptr)->exp);
                break;
            case st_line:
            case st_break:
                break;
            default:
                DIAG("Invalid block type in inlinestmt");
                break;
        }
        block = block->next;
        outptr = &(*outptr)->next;
    }
    return out;
}

//-------------------------------------------------------------------------

static ENODE *inlinefuncargs(ENODE *node, SYM *sp)
{
    ENODE *xnode,  *tnode;
    if (!node)
        return node;
    xnode = node->v.p[0];
    if (node->v.p[1])
        inlinefuncargs(node->v.p[1], sp->next);

    if (sp->tp->uflags &UF_ALTERED)
        goto dodef;
    tnode = xnode;
    while (castvalue(tnode))
        tnode = tnode->v.p[0];
    if (lvalue(tnode))
    {
        switch (tnode->v.p[0]->nodetype)
        {
            case en_napccon:
            case en_autocon:
            case en_autoreg:
            case en_labcon:
                xnode = tnode->v.p[0];
                break;
            default:
                dodef: xnode = makenode(en_autocon, sp, 0);
                sp->funcparm = FALSE;
                sp->value.i = block_nesting + 1;
                tnode = xnode;
                deref(&tnode, sp->tp);
                tnode = makenode(en_assign, tnode, node->v.p[0]);
                opt4(&tnode, FALSE);
                *argnext = xalloc(sizeof(SNODE));
                (*argnext)->stype = st_expr;
                (*argnext)->exp = tnode;
                argnext = &(*argnext)->next;
                break;
        }
    }
    else
    {
        goto dodef;
        //		if (isintconst(tnode->nodetype))
        //			xnode = node->v.p[0] ;
        //		else
        //			goto dodef;
    }
    sp->mainsym->value.classdata.inlinefunc = xnode;
    return node;
}

//-------------------------------------------------------------------------

static void inlinereblock(SYM *sp)
/* Copy all the func args into the xsyms table.
 * This copies the function parameters twice...
 */
{
    SYM *head = sp->tp->lst.head;
    LIST *l;
    xsyms.head = xsyms.tail = 0;
    if (head != (SYM*) - 1)
    while (head)
    {
        SYM *v = copysym(head);
        if (!v->funcparm)
        {
            v->value.i = block_nesting + 1;
        }
        inlineinsert(v);
        head = head->next;
    }
    insertInlineSyms(sp);
}

//-------------------------------------------------------------------------

static void redoParameters(SYM *sp)
{
    SYM *sp1 = sp->tp->lst.head;
    int poffset = 0;
    poffset = stdretblocksize; /* size of return block */
    if (!sp->pascaldefn && isstructured(sp->tp->btp))
    {
        poffset += stdaddrsize;
    }
    if ((sp->value.classdata.cppflags &PF_MEMBER) && !(sp
        ->value.classdata.cppflags &PF_STATIC))
        poffset += stdaddrsize;
    if (sp->farproc)
        poffset += stdintsize;
    if (!sp->pascaldefn)
    while (sp1)
    {
        if (sp1->tp->type != bt_pointer && sp1->tp->type != bt_farpointer &&
            sp1->tp->type != bt_segpointer && sp1->tp->size < stdintsize)
        {
            sp1->value.i = poffset + funcvaluesize(sp1->tp->size);
            poffset += stdintsize;
        }
        else
        {
            sp1->value.i = poffset;
            if (sp1->tp->type == bt_pointer)
                poffset += stdaddrsize;
            else
            {
                poffset += sp1->tp->size;
                if (poffset % stdintsize)
                    poffset += stdintsize - poffset % stdintsize;
            }
        }
        sp1 = sp1->next;
    }

}

//-------------------------------------------------------------------------

ENODE *doinline(ENODE *node)
{
    SYM *lastinlinesp;
    ENODE *out,  *xnode,  *revarg = 0,  *arg;
    SYM *sp,  *newsp,  *dc;
    argstmt = 0;
    argnext = &argstmt;
    thisn = 0;
    if (node->nodetype == en_thiscall)
    {
        thisn = node->v.p[0];
        xnode = node->v.p[1];
        if (!xnode)
        {
            return node;
        }
    }
    else
        xnode = node;
    if (xnode->nodetype != en_pfcall && xnode->nodetype != en_fcall && xnode
        ->nodetype != en_sfcall)
    {
        return node;
    }
    if (xnode->v.p[1]->v.p[0]->v.p[0]->nodetype != en_nacon && xnode->v.p[1]
        ->v.p[0]->v.p[0]->nodetype != en_napccon)
    {
        return node;
    }
    sp = xnode->v.p[1]->v.p[0]->v.p[0]->v.sp;
    if (!(sp->value.classdata.cppflags &PF_INLINE) || (sp
        ->value.classdata.cppflags &PF_INSTANTIATED))
    {
        return node;
    }
    if (!(sp->value.classdata.inlinefunc))
    {
//        gensymerror(ERR_UNDEFINED, sp->name);
        LIST *l;
        sp->value.classdata.cppflags &= ~PF_INLINE;
        sp->value.classdata.cppflags |= PF_INSTANTIATED;
        global_flag++;
        l = xalloc(sizeof(LIST));
        global_flag--;
        l->data = sp;
        l->link = instantiated_inlines;
        instantiated_inlines = l;
        return node;
    }
    if (thisn)
    if (thisn->nodetype == en_conslabel || thisn->nodetype == en_thiscall ||
        thisn->nodetype == en_callblock || thisn->nodetype == en_scallblock ||
        thisn->nodetype == en_pcallblock)
    {
        LIST *l;
        sp->value.classdata.cppflags &= ~PF_INLINE;
        sp->value.classdata.cppflags |= PF_INSTANTIATED;
        global_flag++;
        l = xalloc(sizeof(LIST));
        global_flag--;
        l->data = sp;
        l->link = instantiated_inlines;
        instantiated_inlines = l;
        return node;
    }
    lastinlinesp = inlinesp;
    inlinesp = sp;
    dc = declclass;
    declclass = sp->parentclass;
    if (sp->recalculateParameters)
        redoParameters(sp);
    sp->recalculateParameters = FALSE;
    inlinereblock(sp);
    newsp = copysym(sp);
    arg = xnode->v.p[1]->v.p[1]->v.p[0];
    while (arg)
    {
        ENODE *temp = arg->v.p[1];
        arg->v.p[1] = revarg;
        revarg = arg;
        arg = temp;
    }
    xnode->v.p[1]->v.p[1]->v.p[0] = 0;
    out = inlinefuncargs(revarg, xsyms.head);
    newsp->value.classdata.inlinefunc = xalloc(sizeof(INLINEFUNC));
    newsp->value.classdata.inlinefunc->stmt = inlinestmt(sp
        ->value.classdata.inlinefunc->stmt);
    newsp->value.classdata.inlinefunc->syms = symsToBlock(xsyms.head);
    
    out = makenode(en_void, copy_enode(xnode->v.p[1]->v.p[0]), 0);
    out = makenode(xnode->nodetype, inlineexpr(xnode->v.p[0]), out);
    out->v.p[1]->v.p[0]->v.p[0]->v.sp = newsp;
    if (argstmt)
    {
        *argnext = newsp->value.classdata.inlinefunc->stmt;
        newsp->value.classdata.inlinefunc->stmt = argstmt;
    }
    if (thisn)
        out = makenode(en_thiscall, thisn, out);
    //   inlinerename(&xsyms);
    addblocklist(xsyms.head, 0);
    //   /* quick check against nested inlines */
    //   if (currentfunc)
    //      currentfunc->value.classdata.cppflags &= ~PF_INLINE;
    declclass = dc;
    inlinesp = lastinlinesp;
    return out;
}
