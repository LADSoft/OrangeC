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

#include "common.h"
#include "rtti.h"

extern COMPILER_PARAMS cparams;
extern NAMESPACEVALUES *localNameSpace;
extern char *overloadNameTab[];
extern TYPE stdpointer, stdvoid;
extern int mangledNamesCount ;

HASHTABLE *rttiSyms;

// in enum e_bt order
static char *typeNames[] = { "bit", "bool", "signed char", "char", "unsigned char", "short", "char16_t", "unsigned short",
    "wchar_t",  NULL, "int", "inative", "char32_t", "unsigned", "unative", "long", "unsigned long", "long long",
    "unsigned long long", "float", "double", "long double", "float imaginary", "double imaginary",
    "long double imaginary", "float complex", "double complex", "long double complex", "void",
    "__object", "__string" };

void rtti_init(void)
{
    rttiSyms = CreateHashTable(32);
}
#ifndef PARSER_ONLY
static char *addNameSpace(char *buf, SYMBOL *sp)
{
    if (!sp)
        return buf;
    buf = addNameSpace(buf, sp->parentNameSpace);
    my_sprintf(buf, "%s::", sp->name);
    return buf + strlen(buf);
}
static char *addParent(char *buf, SYMBOL *sp)
{
    if (!sp)
        return buf;
    if (sp->parentClass)
        buf = addParent(buf, sp->parentClass);
    else
        buf = addNameSpace(buf, sp->parentNameSpace);
    my_sprintf(buf, "%s", sp->name);
    return buf + strlen(buf);
}
static char *RTTIGetDisplayName(char *buf, TYPE *tp)
{
    if (tp->type == bt_templateparam)
    {
        if (tp->templateParam && tp->templateParam->p->type == kw_typename && tp->templateParam->p->byClass.val)
            tp = tp->templateParam->p->byClass.val;
    }
    if (isconst(tp))
    {
        my_sprintf(buf, "%s ", "const");
        buf += strlen(buf);
    }
    if (isvolatile(tp))
    {
        my_sprintf(buf, "%s ", "volatile");
        buf += strlen(buf);
    }
    tp = basetype(tp);
    if (isstructured(tp) || tp->type == bt_enum)
    {
        buf = addParent(buf, tp->sp);
    }
    else if (ispointer(tp))
    {
        buf = RTTIGetDisplayName(buf, tp->btp);
        *buf++ = '*';
        *buf++ = ' ';
        *buf = 0;
    }
    else if (isref(tp))
    {
        buf = RTTIGetDisplayName(buf, tp->btp);
        *buf++ = '&';
        *buf = 0;
    }
    else if (tp->type == bt_templateparam)
    {
        strcpy(buf, "*templateParam");
        buf += strlen(buf);
    }
    else if (tp->type == bt_any)
    {
        strcpy(buf, "any");
        buf += strlen(buf);
    }
    else if (isfunction(tp))
    {
        buf = RTTIGetDisplayName(buf, tp->btp);
        *buf++ = '(';
        *buf++ = '*';
        *buf++ = ') ';
        *buf++ = '( ';
        NITERSYMTAB(hr, basetype(tp))
        {
            buf = RTTIGetDisplayName(buf, ((SYMBOL *)hr->p)->tp);
            if (hr->next)
            {
                *buf++ = ',';
                *buf++ = ' ';
            }
        }
        *buf++ = ') ';
        *buf = 0;
    }
    else
    {
        strcpy(buf, typeNames[tp->type]);
        buf += strlen(buf);
    }
    return buf;
}
static char *RTTIGetName(char *buf, TYPE *tp)
{
    if (tp->type == bt_templateparam)
    {
        if (tp->templateParam && tp->templateParam->p->type == kw_typename && tp->templateParam->p->byClass.val)
            tp = tp->templateParam->p->byClass.val;
    }
    mangledNamesCount = 0;
    strcpy(buf, "@$xt@");
    buf += strlen(buf);
    buf = mangleType(buf, tp, TRUE);
    return buf;
}
static void RTTIDumpHeader(SYMBOL *xtSym, TYPE *tp, int flags)
{
    char name[4096], *p;
    SYMBOL *sp = NULL;
    if (ispointer(tp))
    {
        sp = RTTIDumpType(basetype(tp)->btp);
        flags = isarray(tp) ? XD_ARRAY : XD_POINTER;
    }
    else if (isref(tp))
    {
        sp = RTTIDumpType(basetype(tp)->btp);
        flags = XD_REF;
    }
    else if (isstructured(tp) && !basetype(tp)->sp->trivialCons)
    {
        sp = search(overloadNameTab[CI_DESTRUCTOR], basetype(tp)->syms);
        // at this point if the class was never instantiated the destructor
        // may not have been created...
        if (sp)
        {
            if (!sp->inlineFunc.stmt && !sp->deferredCompile)
            {
                EXPRESSION *exp = intNode(en_c_i, 0);
                callDestructor(basetype(tp)->sp, NULL, &exp, NULL, TRUE, FALSE, TRUE);
                if (exp && exp->left)
                    sp = exp->left->v.func->sp;
                
            }
            else
            {
                sp = (SYMBOL *)SYMTABBEGIN(basetype(sp->tp))->p;
            }
            GENREF(sp);
        }
    }

    cseg();
    gen_virtual(xtSym, FALSE);
    if (sp)
    {
        genref(sp, 0);            
    }
    else
    {
        genaddress(0);
    }
    genint(tp->size);
    genint(flags);
    RTTIGetDisplayName(name, tp);
    for (p = name; *p; p++)
        genbyte(*p);
    genbyte(0);
}
static void DumpEnclosedStructs(TYPE *tp, BOOLEAN genXT)
{
    SYMBOL *sym = basetype(tp)->sp;
    tp = PerformDeferredInitialization(tp,  NULL);
    NITERVBASEENT(entries, sym)
    {
        if (entries->alloc)
        {
            if (genXT)
            {
                RTTIDumpType(entries->cls->tp);
            }
            else
            {
                SYMBOL *xtSym;
                char name[4096];
                RTTIGetName(name, entries->cls->tp);
                xtSym = search(name, rttiSyms);
                if (!xtSym)
                {
                    RTTIDumpType(entries->cls->tp);
                    xtSym = search(name, rttiSyms);
                }
                genint(XD_CL_VIRTUAL);
                genref(xtSym, 0);
                genint(entries->structOffset);
            }
        }
    }
    NITERBASECLASS(bc, sym)
    {
        if (!bc->isvirtual)
        {
            if (genXT)
            {
                RTTIDumpType(bc->cls->tp);
            }
            else
            {
                SYMBOL *xtSym;
                char name[4096];
                RTTIGetName(name, bc->cls->tp);
                xtSym = search(name, rttiSyms);
                genint(XD_CL_BASE);
                genref(xtSym, 0);
                genint(bc->offset);
            }
        }
    }
    if (sym->tp->syms)
    {
        NITERSYMTAB(hr, sym->tp)
        {
            SYMBOL *member = (SYMBOL *)hr->p;
            TYPE *tp = member->tp;
            int flags = XD_CL_ENCLOSED;
            if (isref(tp))
            {
                tp = basetype(tp)->btp;
                flags |= XD_CL_BYREF;
            }
            if (isconst(tp))
                flags |= XD_CL_CONST;
            tp = basetype(tp);
            if (isstructured(tp))
            {
                tp = PerformDeferredInitialization(tp, NULL);
                if (genXT)
                {
                    RTTIDumpType(tp);
                }
                /*
                else
                {
                    SYMBOL *xtSym;
                    char name[4096];
                    RTTIGetName(name, tp);
                    xtSym = search(name, rttiSyms);
                    genint(flags);
                    genref(xtSym , 0);
                    genint(member->offset);
                }
                */
            }
        }
    }
}
static void RTTIDumpStruct(SYMBOL *xtSym, TYPE *tp)
{
    DumpEnclosedStructs(tp, TRUE);
    RTTIDumpHeader(xtSym, tp, XD_CL_PRIMARY);
    DumpEnclosedStructs(tp, FALSE);
    genint(0);
    gen_endvirtual(xtSym);
}
static void RTTIDumpArray(SYMBOL *xtSym, TYPE *tp)
{
    RTTIDumpHeader(xtSym, tp, XD_ARRAY | getSize(bt_int));
    genint(tp->size/(tp->btp->size));
    gen_endvirtual(xtSym);
}
static void RTTIDumpPointer(SYMBOL *xtSym, TYPE *tp)
{
    RTTIDumpHeader(xtSym, tp, XD_POINTER);
    gen_endvirtual(xtSym);
}
static void RTTIDumpRef(SYMBOL *xtSym, TYPE *tp)
{
    RTTIDumpHeader(xtSym, tp, XD_REF);
    gen_endvirtual(xtSym);
}
static void RTTIDumpArithmetic(SYMBOL *xtSym, TYPE *tp)
{
    RTTIDumpHeader(xtSym, tp, 0);
    gen_endvirtual(xtSym);
}
#endif
SYMBOL *RTTIDumpType(TYPE *tp)
{
    SYMBOL *xtSym = NULL;
#ifndef PARSER_ONLY
    if (cparams.prm_xcept)
    {
        char name[4096];
        RTTIGetName(name, tp);
        xtSym = search(name, rttiSyms);
        if (!xtSym)
        {
            xtSym = makeID(sc_global, tp, NULL, litlate(name));
            xtSym->linkage = lk_virtual;
            xtSym->decoratedName = xtSym->errname = xtSym->name;
            xtSym->xtEntry = TRUE;
            insert(xtSym, rttiSyms);
            if (isstructured(tp) && basetype(tp)->sp->dontinstantiate)
            {
                InsertExtern(xtSym);
                GENREF(xtSym);
                xtSym->dontinstantiate = TRUE;
            }
            else
            {
                switch (basetype(tp)->type)
                {
                    case bt_lref:
                    case bt_rref:
                        RTTIDumpRef(xtSym, tp);
                        break;
                    case bt_pointer:
                        if (isarray(tp))
                            RTTIDumpArray(xtSym, tp);
                        else
                            RTTIDumpPointer(xtSym, tp);
                        break;
                    case bt_struct:
                    case bt_class:
                        RTTIDumpStruct(xtSym, tp);
                        break;
                    default:
                        RTTIDumpArithmetic(xtSym, tp);
                        break;
                }
            }
        }
        else {
            while (ispointer(tp) || isref(tp))
                tp = basetype(tp)->btp;
            if (isstructured(tp) && !basetype(tp)->sp->dontinstantiate)
            {
                SYMBOL *xtSym2;
                // xtSym *should* be there.
                RTTIGetName(name, basetype(tp));
                xtSym2 = search(name, rttiSyms);
                if (xtSym2 && xtSym2->dontinstantiate)
                {
                    xtSym2->dontinstantiate = FALSE;
                    RTTIDumpStruct(xtSym2, tp);
                }
            }
        }
            
    }
#endif
    return xtSym;
}
#ifndef PARSER_ONLY
typedef struct _xclist
{
    struct _xclist *next;
    union {
        EXPRESSION *exp;
        STATEMENT *stmt;
    } ;
    SYMBOL *xtSym;
    char byStmt:1;
    char used:1;
} XCLIST;
static void XCStmt(STATEMENT *block, XCLIST ***listPtr);
static void XCExpression(EXPRESSION *node, XCLIST ***listPtr)
{
    FUNCTIONCALL *fp;
    if (node == 0)
        return;
    switch (node->type)
    {
        case en_auto:
            break;
        case en_const:
            break;
        case en_c_ll:
        case en_c_ull:
        case en_c_d:
        case en_c_ld:
        case en_c_f:
        case en_c_dc:
        case en_c_ldc:
        case en_c_fc:
        case en_c_di:
        case en_c_ldi:
        case en_c_fi:
        case en_c_i:
        case en_c_l:
        case en_c_ui:
        case en_c_ul:
        case en_c_c:
        case en_c_bool:
        case en_c_uc:
        case en_c_wc:
        case en_c_u16:
        case en_c_u32:
        case en_c_string:
        case en_nullptr:
        case en_memberptr:
        case en_structelem:
            break;
        case en_global:
        case en_pc:
        case en_labcon:
        case en_absolute:
        case en_threadlocal:
            break;
        case en_l_sp:
        case en_l_fp:
        case en_bits:
        case en_l_f:
        case en_l_d:
        case en_l_ld:
        case en_l_fi:
        case en_l_di:
        case en_l_ldi:
        case en_l_fc:
        case en_l_dc:
        case en_l_ldc:
        case en_l_c:
        case en_l_u16:
        case en_l_u32:
        case en_l_s:
        case en_l_ul:
        case en_l_l:
        case en_l_p:
        case en_l_ref:
        case en_l_i:
        case en_l_ui:
        case en_l_inative:
        case en_l_unative:
        case en_l_uc:
        case en_l_us:
        case en_l_bool:
        case en_l_bit:
        case en_l_ll:
        case en_l_ull:
        case en_l_string:
        case en_l_object:
        case en_literalclass:
        case en_l_wc:
            XCExpression(node->left, listPtr);
            break;
        case en_uminus:
        case en_compl:
        case en_not:
        case en_x_f:
        case en_x_d:
        case en_x_ld:
        case en_x_fi:
        case en_x_di:
        case en_x_ldi:
        case en_x_fc:
        case en_x_dc:
        case en_x_ldc:
        case en_x_ll:
        case en_x_ull:
        case en_x_i:
        case en_x_ui:
        case en_x_inative:
        case en_x_unative:
        case en_x_c:
        case en_x_u16:
        case en_x_u32:
        case en_x_wc:
        case en_x_uc:
        case en_x_bool:
        case en_x_bit:
        case en_x_s:
        case en_x_us:
        case en_x_l:
        case en_x_ul:
        case en_x_p:
        case en_x_fp:
        case en_x_sp:
        case en_x_string:
        case en_x_object:
        case en_trapcall:
        case en_shiftby:
/*        case en_movebyref: */
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
            XCExpression(node->left, listPtr);
            break;
        case en_assign:
        case en__initblk:
        case en__cpblk:
            XCExpression(node->right, listPtr);
            XCExpression(node->left, listPtr);
            break;
        case en_autoinc:
        case en_autodec:
            XCExpression(node->left, listPtr);
            break; 
        case en_add:
        case en_sub:
/*        case en_addcast: */
        case en_lsh:
        case en_arraylsh:
        case en_rsh:
        case en_rshd:
        case en_void:
        case en_voidnz:
/*        case en_dvoid: */
        case en_arraymul:
        case en_arrayadd:
        case en_arraydiv:
        case en_structadd:
        case en_mul:
        case en_div:
        case en_umul:
        case en_udiv:
        case en_umod:
        case en_ursh:
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
        case en_stackblock:
        case en_blockassign:
        case en_mp_compare:
/*		case en_array: */
            XCExpression(node->right, listPtr);
        case en_mp_as_bool:
        case en_blockclear:
        case en_argnopush:
        case en_not_lvalue:
        case en_lvalue:
        case en_funcret:
            XCExpression(node->left, listPtr);
            break;
        case en_thisref:
            **listPtr = Alloc(sizeof(XCLIST));
            (**listPtr)->exp = node;
            (*listPtr) = &(**listPtr)->next;                
            XCExpression(node->left, listPtr);
            break;
        case en_atomic:
            break;
        case en_func:
            fp = node->v.func;
            {
                for(INITLIST* args = fp->arguments; args; args = args->next)
                {
                    XCExpression(args->exp, listPtr);
                }
                if (fp->thisptr)
                    XCExpression(fp->thisptr, listPtr);
                if (fp->returnEXP)
                    XCExpression(fp->returnEXP, listPtr);
            }
            break;
        case en_stmt:
            XCStmt(node->v.stmt, listPtr);
            break;
        default:
            diag("XCExpression");
            break;
    }
}
static void XCStmt(STATEMENT *block, XCLIST ***listPtr)
{
    while (block != NULL)
    {
        switch (block->type)
        {
            case st__genword:
                break;
            case st_catch:
            case st___catch:
            case st___finally:
            case st___fault:
                **listPtr = Alloc(sizeof(XCLIST));
                (**listPtr)->stmt = block;
                (**listPtr)->byStmt = TRUE;
                (*listPtr) = &(**listPtr)->next;
            case st_try:
            case st___try:
                XCStmt(block->lower, listPtr);
                break;
            case st_return:
            case st_expr:
            case st_declare:
                XCExpression(block->select, listPtr);
                break;
            case st_goto:
            case st_label:
                break;
            case st_select:
            case st_notselect:
                XCExpression(block->select, listPtr);
                break;
            case st_switch:
                XCExpression(block->select, listPtr);
                XCStmt(block->lower, listPtr);
                break;
            case st_block:
                XCStmt(block->lower, listPtr);
                XCStmt(block->blockTail, listPtr);
                break;
            case st_passthrough:
            case st_nop:
                break;
            case st_datapassthrough:
                break;
            case st_line:
            case st_varstart:
            case st_dbgblock:
                break;
            default:
                diag("Invalid block type in XCStmt");
                break;
        }
        block = block->next;
    }
}
static SYMBOL *DumpXCSpecifiers(SYMBOL *funcsp)
{
    SYMBOL *xcSym = FALSE;
    if (funcsp->xcMode != xc_unspecified)
    {
        char name[4096];
        SYMBOL *list[1000];
        int count = 0, i;
        if (funcsp->xcMode == xc_dynamic)
        {
            for(LIST* p = funcsp->xc->xcDynamic; p; p = p->next)
            {
                TYPE *tp = (TYPE *)p->data;
                if (tp->type == bt_templateparam && tp->templateParam->p->packed)
                {
                    if (tp->templateParam->p->type == kw_typename)
                    {
                        for(TEMPLATEPARAMLIST *pack = tp->templateParam->p->byPack.pack; pack; pack = pack->next)
                        {
                            list[count++] = RTTIDumpType((TYPE *)pack->p->byClass.val);
                        }
                    }
                }
                else
                {
                    list[count++] = RTTIDumpType((TYPE *)p->data);
                }
            }
        }
        my_sprintf(name, "@$xct%s", funcsp->decoratedName);
        xcSym = makeID(sc_global, &stdpointer, NULL, litlate(name));
        xcSym->linkage = lk_virtual;
        xcSym->decoratedName = xcSym->errname = xcSym->name;
        cseg();
        gen_virtual(xcSym, FALSE);
        switch(funcsp->xcMode)
        {
            case xc_none:
                genint(XD_NOXC);
                genint(0);
                break;
            case xc_all:
                genint(XD_ALLXC);
                genint(0);
                break;
            case xc_dynamic:
                genint(XD_DYNAMICXC);
                for (i=0; i < count; i++)
                {
                    genref(list[i] ,0);
                }
                genint(0);
                break;
            case xc_unspecified:
                break;
        }
        gen_endvirtual(xcSym);
       
    }
    return xcSym;
}
static BOOLEAN allocatedXC(EXPRESSION *exp)
{
    switch (exp->type)
    {
        case en_add:
            return allocatedXC(exp->left) || allocatedXC(exp->right);
        case en_auto:
            return exp->v.sp->allocate;
        default:
            return FALSE;
    }
}
static int evalofs(EXPRESSION *exp)
{
    switch (exp->type)
    {
        case en_add:
            return evalofs(exp->left) + evalofs(exp->right);
        case en_c_i:
        case en_c_ui:
        case en_c_l:
        case en_c_ul:
            return exp->v.i;
        case en_auto:
        case en_structelem:
            return exp->v.sp->offset;
        default:
            return 0;
    }
}
static BOOLEAN throughThis(EXPRESSION *exp)
{
    switch(exp->type)
    {
        case en_add:
            return throughThis(exp->left) | throughThis(exp->right);
        case en_l_p:
            return (exp->left->type == en_auto && exp->left->v.sp->thisPtr);
        default:
            return FALSE;
    }
}
void XTDumpTab(SYMBOL *funcsp)
{
    if (funcsp->xc && funcsp->xc->xctab && cparams.prm_xcept)
    {
        XCLIST *list = NULL, **listPtr = &list;
        SYMBOL *throwSym;
        XCStmt(funcsp->inlineFunc.stmt, &listPtr);
        for(XCLIST* p = list; p; p = p->next)
        {
            if (p->byStmt)
            {
                if (p->stmt->tp)
                    p->xtSym = RTTIDumpType(basetype(p->stmt->tp));
            }
            else
            {
                // en_thisref
                if (basetype(p->exp->v.t.tp)->sp->hasDest)
                    p->xtSym = RTTIDumpType(basetype(p->exp->v.t.tp));
                
            }
        }
        throwSym = DumpXCSpecifiers(funcsp);
        gen_virtual(funcsp->xc->xclab, FALSE);
        if (throwSym)
        {
            genref(throwSym , 0);
        }
        else
        {
            genaddress(0);
        }
        genint(funcsp->xc->xctab->offset);
        for(XCLIST* p = list; p; p = p->next)
        {
            if (p->byStmt)
            {
                genint(XD_CL_TRYBLOCK);
                if (p->xtSym)
                {
                    genref(p->xtSym, 0);
                }
                else
                {
                    genaddress(0);
                }
                // this was normalized in the back end...  depends on the RTTI information
                // being generated AFTER the function is generated, however...
                gen_labref(p->stmt->altlabel);
                genint(p->stmt->tryStart);
                genint(p->stmt->tryEnd);
            }
            else
            {
                if (p->xtSym && !p->exp->dest && allocatedXC(p->exp->v.t.thisptr))
                {
                    XCLIST *q;
                    for(q = p; q; q = q->next)
                    {
                        if (!q->byStmt && q->exp->dest)
                        {
                            if (equalnode(p->exp->v.t.thisptr, q->exp->v.t.thisptr))
                                break;
                        }
                    }
                    if (q)
                        q->used = TRUE;
                    genint(XD_CL_PRIMARY | (throughThis(p->exp) ? XD_THIS  : 0));
                    genref(p->xtSym, 0);
                    genint(evalofs(p->exp->v.t.thisptr));
                    genint(p->exp->v.t.thisptr->xcInit);
                    genint(p->exp->v.t.thisptr->xcDest);
                }
            }
        }
        // for arguments which are destructed
        for(XCLIST* p = list; p; p = p->next)
        {
            if (!p->byStmt && p->xtSym && p->exp->dest && !p->used)
            {
                p->used = TRUE;
                genint(XD_CL_PRIMARY | (throughThis(p->exp) ? XD_THIS  : 0));
                genref(p->xtSym, 0);
                genint(evalofs(p->exp->v.t.thisptr));
                genint(0);
                genint(p->exp->v.t.thisptr->xcDest);
            }
        }
        genint(0);
        gen_endvirtual(funcsp->xc->xclab);
    }
}
#endif