/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

#include "compiler.h"
#include <unordered_set>
#include "config.h"
#include "initbackend.h"
#include "symtab.h"
#include "stmt.h"
#include "expr.h"
#include "ccerr.h"
#include "template.h"
#include "declare.h"
#include "lex.h"
#include "constopt.h"
#include "OptUtils.h"
#include "help.h"
#include "memory.h"
#include "inasm.h"
#include "init.h"
#include "ildata.h"
#include "declcons.h"
#include "istmt.h"
#include "declcpp.h"
#include "inline.h"

namespace Parser
{

static Optimizer::LIST *inlineHead, *inlineTail, *inlineVTabHead, *inlineVTabTail;
static Optimizer::LIST *inlineDataHead, *inlineDataTail;

static SYMBOL* inlinesp_list[MAX_INLINE_NESTING];

static int inlinesp_count;
static HASHTABLE* vc1Thunks;
static std::unordered_set<std::string> didInlines;

static FUNCTIONCALL* function_list[MAX_INLINE_NESTING];
static int function_list_count;
static int namenumber;

void inlineinit(void)
{
    namenumber = 0;
    inlineHead = nullptr;
    inlineVTabHead = nullptr;
    inlineDataHead = nullptr;
    vc1Thunks = CreateHashTable(1);
    didInlines.clear();  // = CreateHashTable(32);
}
static bool inSearch(SYMBOL* sp)
{
    return didInlines.find(sp->sb->decoratedName) != didInlines.end();
    /*
    SYMLIST** hr = GetHashLink(didInlines, sp->sb->decoratedName);
    while (*hr)
    {
        SYMBOL* sym = (SYMBOL*)(*hr)->p;
        if (!strcmp(sym->sb->decoratedName, sp->sb->decoratedName))
            return sym;
        hr = &(*hr)->next;
    }
    return nullptr;
    */
}
static void inInsert(SYMBOL* sym)
{
    didInlines.insert(sym->sb->decoratedName);
    /*
// assumes the symbol isn't already there...
SYMLIST** hr = GetHashLink(didInlines, sym->sb->decoratedName);
SYMLIST* added = (SYMLIST*)Alloc(sizeof(SYMLIST));
//    sym->sb->mainsym = nullptr;
added->p = (SYMBOL*)sym;
added->next = *hr;
*hr = added;
*/
}
static void UndoPreviousCodegen(SYMBOL* sym) {}
void dumpInlines(void)
{
#ifndef PARSER_ONLY
    if (!TotalErrors())
    {
        bool done;
        Optimizer::LIST* vtabList;
        Optimizer::LIST* dataList;
        Optimizer::cseg();
        do
        {
            Optimizer::LIST* funcList = inlineHead;
            done = true;
            while (funcList)
            {
                SYMBOL* sym = (SYMBOL*)funcList->data;
                if (((sym->sb->attribs.inheritable.isInline && sym->sb->dumpInlineToFile) ||
                     (Optimizer::SymbolManager::Test(sym) && Optimizer::SymbolManager::Test(sym)->genreffed)))
                {
                    if ((sym->sb->parentClass && sym->sb->parentClass->sb->dontinstantiate && !sym->sb->templateLevel) ||
                        sym->sb->attribs.inheritable.linkage2 == lk_import)
                    {
                        sym->sb->dontinstantiate = true;
                    }
                    if (!sym->sb->didinline && !sym->sb->dontinstantiate)
                    {
                        if (inSearch(sym))
                        {
                            sym->sb->didinline = true;
                        }
                        else
                        {
                            if (isfunction(sym->tp) && !sym->sb->inlineFunc.stmt && Optimizer::cparams.prm_cplusplus)
                            {
                                propagateTemplateDefinition(sym);
                            }
                            if ((sym->sb->attribs.inheritable.isInline || sym->sb->attribs.inheritable.linkage == lk_virtual) &&
                                sym->sb->inlineFunc.stmt)
                            {
                                inInsert(sym);
                                sym->sb->noextern = true;
                                UndoPreviousCodegen(sym);
                                startlab = Optimizer::nextLabel++;
                                retlab = Optimizer::nextLabel++;
                                genfunc(sym, true);
                                done = false;
                                sym->sb->didinline = true;
                            }
                        }
                    }
                }
                funcList = funcList->next;
            }
            startlab = retlab = 0;
            vtabList = inlineVTabHead;
            while (vtabList)
            {
                SYMBOL* sym = (SYMBOL*)vtabList->data;
                if (Optimizer::SymbolManager::Test(sym->sb->vtabsp) && hasVTab(sym) && !sym->sb->vtabsp->sb->didinline)
                {
                    if (sym->sb->dontinstantiate || sym->sb->vtabsp->sb->dontinstantiate)
                    {
                        Optimizer::SymbolManager::Get(sym->sb->vtabsp)->dontinstantiate = true;
                        sym->sb->vtabsp->sb->storage_class = sc_external;
                        sym->sb->vtabsp->sb->attribs.inheritable.linkage = lk_c;
                    }
                    else
                    {
                        sym->sb->vtabsp->sb->didinline = true;
                        sym->sb->vtabsp->sb->noextern = true;
                        dumpVTab(sym);
                        done = false;
                    }
                }
                vtabList = vtabList->next;
            }
        } while (!done);
        dataList = inlineDataHead;
        Optimizer::dseg();
        while (dataList)
        {
            SYMBOL* sym = (SYMBOL*)dataList->data;
            if (sym->sb->attribs.inheritable.linkage2 != lk_import && sym->sb->storage_class != sc_constant)
            {
                if (sym->sb->parentClass && sym->sb->parentClass->sb->parentTemplate)
                {
                    SYMBOL* parentTemplate = sym->sb->parentClass->sb->parentTemplate;
                    SYMBOL* origsym;
                    SYMLIST* instants = parentTemplate->sb->instantiations;
                    while (instants)
                    {
                        if (TemplateInstantiationMatch(instants->p, sym->sb->parentClass))
                        {
                            parentTemplate = instants->p;
                            break;
                        }
                        instants = instants->next;
                    }
                    origsym = search(sym->name, parentTemplate->tp->syms);
                    //            printf("%s\n", origsym->sb->decoratedName);

                    if (!origsym || origsym->sb->storage_class != sc_global)
                    {
                        parentTemplate = sym->sb->parentClass->sb->parentTemplate;
                        origsym = search(sym->name, parentTemplate->tp->syms);
                    }

                    if (sym->sb->parentClass && sym->sb->parentClass->sb->dontinstantiate)
                    {
                        sym->sb->dontinstantiate = true;
                    }
                    if (origsym && origsym->sb->storage_class == sc_global && !sym->sb->didinline && !sym->sb->dontinstantiate)
                    {
                        sym->sb->didinline = true;
                        sym->sb->noextern = true;
                        sym->sb->storage_class = sc_global;
                        sym->sb->attribs.inheritable.linkage = lk_virtual;
                        if (origsym->sb->deferredCompile)
                        {
                            STRUCTSYM s1, s;
                            LEXEME* lex;
                            s1.str = sym->sb->parentClass;
                            addStructureDeclaration(&s1);
                            s.tmpl = sym->templateParams;
                            addTemplateDeclaration(&s);
                            lex = SetAlternateLex(origsym->sb->deferredCompile);
                            sym->sb->init = nullptr;
                            lex = initialize(lex, nullptr, sym, sc_global, true, 0);
                            SetAlternateLex(nullptr);
                            dropStructureDeclaration();
                            dropStructureDeclaration();
                        }
                        Optimizer::gen_virtual(Optimizer::SymbolManager::Get(sym), true);
                        if (sym->sb->init)
                        {
                            if (isstructured(sym->tp) || isarray(sym->tp))
                            {
                                dumpInitGroup(sym, sym->tp);
                            }
                            else
                            {
                                int s = dumpInit(sym, sym->sb->init);
                                if (s < sym->tp->size)
                                    Optimizer::genstorage(sym->tp->size - s);
                            }
                        }
                        else
                        {
                            Optimizer::genstorage(basetype(sym->tp)->size);
                        }
                        Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(sym));
                    }
                }
                else
                {
                    if (!sym->sb->didinline)
                    {
                        if (inSearch(sym))
                        {
                            sym->sb->didinline = true;
                        }
                        else
                        {
                            inInsert(sym);
                            Optimizer::gen_virtual(Optimizer::SymbolManager::Get(sym), true);
                            if (sym->sb->init)
                            {
                                if (isstructured(sym->tp) || isarray(sym->tp))
                                {
                                    dumpInitGroup(sym, sym->tp);
                                }
                                else
                                {
                                    int s = dumpInit(sym, sym->sb->init);
                                    if (s < sym->tp->size)
                                        Optimizer::genstorage(sym->tp->size - s);
                                }
                            }
                            else
                                Optimizer::genstorage(basetype(sym->tp)->size);
                            Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(sym));
                        }
                    }
                }
            }
            dataList = dataList->next;
        }
    }
#endif
}
void dumpImportThunks(void)
{
#ifndef PARSER_ONLY
    Optimizer::LIST* l = importThunks;
    while (l)
    {
        Optimizer::gen_virtual(Optimizer::SymbolManager::Get((SYMBOL*)l->data), false);
        Optimizer::gen_importThunk(Optimizer::SymbolManager::Get(((SYMBOL*)l->data)->sb->mainsym));
        Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get((SYMBOL*)l->data));
        l = l->next;
    }
#endif
}
void dumpvc1Thunks(void)
{
#ifndef PARSER_ONLY
    SYMLIST* hr;
    Optimizer::cseg();
    hr = vc1Thunks->table[0];
    while (hr)
    {
        Optimizer::gen_virtual(Optimizer::SymbolManager::Get(hr->p), false);
        Optimizer::gen_vc1(Optimizer::SymbolManager::Get(hr->p));
        Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(hr->p));
        hr = hr->next;
    }
#endif
}
SYMBOL* getvc1Thunk(int offset)
{
    char name[256];
    SYMBOL* rv;
    Optimizer::my_sprintf(name, "@$vc1$B0$%d$0", offset + 1);
    rv = search(name, vc1Thunks);
    if (!rv)
    {
        rv = SymAlloc();
        rv->name = rv->sb->decoratedName = litlate(name);
        rv->sb->storage_class = sc_static;
        rv->sb->attribs.inheritable.linkage = lk_virtual;
        rv->sb->offset = offset;
        rv->tp = &stdvoid;
        insert(rv, vc1Thunks);
    }
    return rv;
}
void InsertInline(SYMBOL* sym)
{
    SYMBOL* sp = sym;
    Optimizer::LIST* temp = (Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
    temp->data = sym;
    if (isfunction(sym->tp))
    {
        if (inlineHead)
            inlineTail = inlineTail->next = temp;
        else
            inlineHead = inlineTail = temp;
    }
    else
    {
        if (inlineVTabHead)
            inlineVTabTail = inlineVTabTail->next = temp;
        else
            inlineVTabHead = inlineVTabTail = temp;
    }
}
void InsertInlineData(SYMBOL* sym)
{
    Optimizer::LIST* temp = (Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
    temp->data = sym;
    if (inlineDataHead)
        inlineDataTail = inlineDataTail->next = temp;
    else
        inlineDataHead = inlineDataTail = temp;
}
/*-------------------------------------------------------------------------*/

EXPRESSION* inlineexpr(EXPRESSION* node, bool* fromlval)
{
    /*
     * routine takes an enode tree and replaces it with a copy of itself.
     * Used because we have to munge the block_nesting field (value.i) of each
     * sym in an inline function to force allocation of the variables
     */
    EXPRESSION *temp, *temp1;
    FUNCTIONCALL* fp;
    int i;
    (void)fromlval;
    if (node == 0)
        return 0;
    temp = (EXPRESSION*)(EXPRESSION*)Alloc(sizeof(EXPRESSION));
    memcpy(temp, node, sizeof(EXPRESSION));
    switch (temp->type)
    {
        case en_thisshim:
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
        case en_structelem:
            break;
        case en_global:
        case en_pc:
        case en_labcon:
        case en_const:
        case en_threadlocal:
            break;
        case en_auto:
            if (temp->v.sp->sb->inlineFunc.stmt)
            {
                // guaranteed to be an lvalue at this point
                temp = ((EXPRESSION*)(temp->v.sp->sb->inlineFunc.stmt));
                temp = inlineexpr(temp, fromlval);
                if (fromlval)
                    *fromlval = true;
                else if (lvalue(temp))
                    temp = temp->left;
            }
            else if (temp->v.sp->sb->structuredReturn)
            {
                SYMBOL* sym = temp->v.sp;
                int n = function_list_count;
                while (sym && sym->sb->structuredReturn && --n >= 0)
                {
                    sym = function_list[n]->returnSP;
                }
                if (n >= 0 && sym)
                {
                    temp = function_list[n]->returnEXP;
                    temp = inlineexpr(temp, fromlval);
                    if (fromlval)
                        *fromlval = true;
                }
            }
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
        case en_l_wc:
        case en_l_c:
        case en_l_s:
        case en_l_u16:
        case en_l_u32:
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
            /*
            if (node->left->type == en_auto)
            {
                memcpy(temp, (EXPRESSION *)(node->left->v.sp->sb->inlineFunc.stmt), sizeof(EXPRESSION));
//                temp->left = (EXPRESSION *)(node->left->v.sp->sb->inlineFunc.stmt);
            }
            else
            */
            {
                bool lval = false;
                temp->left = inlineexpr(temp->left, &lval);
                if (lval)
                    temp = temp->left;
            }
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
        case en_x_uc:
        case en_x_u16:
        case en_x_u32:
        case en_x_wc:
        case en_x_bool:
        case en_x_bit:
        case en_x_s:
        case en_x_us:
        case en_x_l:
        case en_x_ul:
        case en_x_p:
        case en_x_fp:
        case en_x_sp:
        case en_trapcall:
        case en_shiftby:
            /*        case en_movebyref: */
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
        case en_not_lvalue:
        case en_lvalue:
        case en_literalclass:
        case en_x_string:
        case en_x_object:
            temp->left = inlineexpr(node->left, nullptr);
            break;
        case en_autoinc:
        case en_autodec:
        case en_add:
        case en_structadd:
        case en_sub:
            /*        case en_addcast: */
        case en_lsh:
        case en_arraylsh:
        case en_rsh:
        case en_rshd:
        case en_assign:
        case en_void:
        case en_voidnz:
            /*        case en_dvoid: */
        case en_arraymul:
        case en_arrayadd:
        case en_arraydiv:
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
        case en__initblk:
        case en__cpblk:
        case en_dot:
        case en_pointsto:
                break;
            /*		case en_array: */
            temp->right = inlineexpr(node->right, nullptr);
        case en_mp_as_bool:
        case en_blockclear:
        case en_argnopush:
        case en_thisref:
        case en_funcret:
        case en__initobj:
        case en__sizeof:
            temp->left = inlineexpr(node->left, nullptr);
            break;
        case en_atomic:
            temp->v.ad->flg = inlineexpr(node->v.ad->flg, nullptr);
            temp->v.ad->memoryOrder1 = inlineexpr(node->v.ad->memoryOrder1, nullptr);
            temp->v.ad->memoryOrder2 = inlineexpr(node->v.ad->memoryOrder2, nullptr);
            temp->v.ad->address = inlineexpr(node->v.ad->address, nullptr);
            temp->v.ad->value = inlineexpr(node->v.ad->value, nullptr);
            temp->v.ad->third = inlineexpr(node->v.ad->third, nullptr);
            break;
        case en_func:
            temp->v.func = nullptr;
            fp = node->v.func;
            if (fp->sp->sb->attribs.inheritable.linkage == lk_virtual)
            {
                // check for recursion
                for (i = 0; i < inlinesp_count; i++)
                {
                    if (inlinesp_list[i] == fp->sp)
                    {
                        break;
                    }
                }
            }
            if (fp->sp->sb->attribs.inheritable.isInline && !fp->sp->sb->noinline && i >= inlinesp_count)
            {
                if (inlinesp_count >= MAX_INLINE_NESTING)
                {
                    diag("inline sym queue too deep");
                }
                else
                {
                    EXPRESSION* temp1;
                    inlinesp_list[inlinesp_count++] = fp->sp;
                    temp1 = doinline(fp, nullptr); /* discarding our allocation */
                    inlinesp_count--;
                    if (temp1)
                        temp = temp1;
                }
            }
            if (temp->v.func == nullptr)
            {
                INITLIST* args = fp->arguments;
                INITLIST** p;
                temp->v.func = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
                *temp->v.func = *fp;
                p = &temp->v.func->arguments;
                *p = nullptr;
                while (args)
                {
                    *p = (INITLIST*)Alloc(sizeof(INITLIST));
                    **p = *args;
                    (*p)->exp = inlineexpr((*p)->exp, nullptr);
                    args = args->next;
                    p = &(*p)->next;
                }
                if (temp->v.func->thisptr)
                    temp->v.func->thisptr = inlineexpr(temp->v.func->thisptr, nullptr);
            }
            break;
        case en_stmt:
            temp->v.stmt = inlinestmt(temp->v.stmt);
            temp->left = inlineexpr(temp->left, nullptr);
            break;
        default:
            diag("Invalid expr type in inlineexpr");
            break;
    }
    return temp;
}

/*-------------------------------------------------------------------------*/

STATEMENT* inlinestmt(STATEMENT* block)
{
    STATEMENT *out = nullptr, **outptr = &out;
    while (block != nullptr)
    {
        *outptr = (STATEMENT*)(STATEMENT*)Alloc(sizeof(STATEMENT));
        memcpy(*outptr, block, sizeof(STATEMENT));
        (*outptr)->next = nullptr;
        switch (block->type)
        {
            case st__genword:
                break;
            case st_try:
            case st_catch:
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
                (*outptr)->lower = inlinestmt(block->lower);
                (*outptr)->blockTail = inlinestmt(block->blockTail);
                break;
            case st_return:
            case st_expr:
            case st_declare:
                (*outptr)->select = inlineexpr(block->select, nullptr);
                break;
            case st_goto:
            case st_label:
                break;
            case st_select:
            case st_notselect:
                (*outptr)->select = inlineexpr(block->select, nullptr);
                break;
            case st_switch:
                (*outptr)->select = inlineexpr(block->select, nullptr);
                (*outptr)->lower = inlinestmt(block->lower);
                break;
            case st_block:
                (*outptr)->lower = inlinestmt(block->lower);
                (*outptr)->blockTail = inlinestmt(block->blockTail);
                break;
            case st_passthrough:
                if (block->lower)
                    inlineAsmStmt(block->lower);
                break;
            case st_nop:
                break;
            case st_datapassthrough:
                break;
            case st_line:
            case st_varstart:
            case st_dbgblock:
                break;
            default:
                diag("Invalid block type in inlinestmt");
                break;
        }
        outptr = &(*outptr)->next;
        block = block->next;
    }
    return out;
}
static void inlineResetReturn(STATEMENT* block, TYPE* rettp, EXPRESSION* retnode)
{
    EXPRESSION* exp;
    if (isstructured(rettp))
    {
        diag("structure in inlineResetReturn");
        exp = intNode(en_c_i, 0);
    }
    else
    {
        exp = block->select;
        cast(rettp, &exp);
        exp = exprNode(en_assign, retnode, exp);
    }
    block->type = st_expr;
    block->select = exp;
}
static EXPRESSION* newReturn(TYPE* tp)
{
    EXPRESSION* exp;
    if (!isstructured(tp) && !isvoid(tp))
    {
        exp = anonymousVar(sc_auto, tp);
        deref(tp, &exp);
    }
    else
        exp = intNode(en_c_i, 0);
    return exp;
}
static void reduceReturns(STATEMENT* block, TYPE* rettp, EXPRESSION* retnode)
{
    while (block != nullptr)
    {
        switch (block->type)
        {
            case st__genword:
                break;
            case st_try:
            case st_catch:
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
                reduceReturns(block->lower, rettp, retnode);
                break;
            case st_return:
                inlineResetReturn(block, rettp, retnode);
                break;
            case st_goto:
            case st_label:
                break;
            case st_expr:
                /*			case st_functailexpr: */
            case st_declare:
            case st_select:
            case st_notselect:
                break;
            case st_switch:
                reduceReturns(block->lower, rettp, retnode);
                break;
            case st_block:
                reduceReturns(block->lower, rettp, retnode);
                /* skipping block tail as it will have no returns  */
                break;
            case st_passthrough:
            case st_datapassthrough:
                break;
            case st_nop:
                break;
            case st_line:
            case st_varstart:
            case st_dbgblock:
                break;
            default:
                diag("Invalid block type in reduceReturns");
                break;
        }
        block = block->next;
    }
}
static EXPRESSION* scanReturn(STATEMENT* block, TYPE* rettp)
{
    EXPRESSION* rv = nullptr;
    while (block != nullptr && !rv)
    {
        switch (block->type)
        {
            case st__genword:
                break;
            case st_try:
            case st_catch:
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
                rv = scanReturn(block->lower, rettp);
                break;
            case st_return:
                rv = block->select;
                if (!isstructured(rettp))
                {
                    if (isref(rettp))
                        deref(&stdpointer, &rv);
                    else
                        cast(rettp, &rv);
                }
                block->type = st_expr;
                block->select = rv;
                return rv;
            case st_goto:
            case st_label:
                break;
            case st_expr:
                /*			case st_functailexpr: */
            case st_declare:
            case st_select:
            case st_notselect:
                break;
            case st_switch:
                rv = scanReturn(block->lower, rettp);
                break;
            case st_block:
                rv = scanReturn(block->lower, rettp);
                /* skipping block tail as it will have no returns  */
                break;
            case st_passthrough:
            case st_datapassthrough:
                break;
            case st_nop:
                break;
            case st_line:
            case st_varstart:
            case st_dbgblock:
                break;
            default:
                diag("Invalid block type in scanReturn");
                break;
        }
        block = block->next;
    }
    return rv;
}

/*-------------------------------------------------------------------------*/
static bool sideEffects(EXPRESSION* node)
{
    bool rv = false;
    if (node == 0)
        return rv;
    switch (node->type)
    {
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
        case en_structelem:
            rv = false;
            break;
        case en_global:
        case en_pc:
        case en_threadlocal:
        case en_labcon:
        case en_const:
        case en_auto:
        case en__sizeof:
            rv = false;
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
        case en_l_wc:
        case en_l_c:
        case en_l_s:
        case en_l_u16:
        case en_l_u32:
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
            rv = sideEffects(node->left);
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
        case en_x_uc:
        case en_x_u16:
        case en_x_u32:
        case en_x_wc:
        case en_x_bool:
        case en_x_bit:
        case en_x_s:
        case en_x_us:
        case en_x_l:
        case en_x_ul:
        case en_x_p:
        case en_x_fp:
        case en_x_sp:
        case en_shiftby:
        case en_x_string:
        case en_x_object:
            /*        case en_movebyref: */
        case en_not_lvalue:
        case en_lvalue:
            rv = sideEffects(node->left);
            break;
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
        case en_assign:
        case en_autoinc:
        case en_autodec:
        case en_trapcall:
        case en__initblk:
        case en__cpblk:
        case en__initobj:
            rv = true;
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
        case en_dot:
        case en_pointsto:
                break;
            /*		case en_array: */
            rv = sideEffects(node->right);
        case en_mp_as_bool:
        case en_blockclear:
        case en_argnopush:
        case en_thisref:
        case en_funcret:
            rv |= sideEffects(node->left);
            break;
        case en_atomic:
            rv = sideEffects(node->v.ad->flg);
            rv |= sideEffects(node->v.ad->memoryOrder1);
            rv |= sideEffects(node->v.ad->memoryOrder2);
            rv |= sideEffects(node->v.ad->address);
            rv |= sideEffects(node->v.ad->value);
            rv |= sideEffects(node->v.ad->third);
            break;
        case en_func:
            rv = true;
            break;
        case en_stmt:
            rv = true;
            break;
        case en_thisshim:
            break;
        default:
            diag("sideEffects");
            break;
    }
    return rv;
}
static void setExp(SYMBOL* sx, EXPRESSION* exp, STATEMENT*** stp)
{
    if (!sx->sb->altered && !sx->sb->addressTaken && !sideEffects(exp))
    {
        // well if the expression is too complicated it gets evaluated over and over
        // but maybe the backend can clean it up again...
        sx->sb->inlineFunc.stmt = (STATEMENT*)exp;
    }
    else
    {
        EXPRESSION* tnode = anonymousVar(sc_auto, sx->tp);
        deref(sx->tp, &tnode);
        sx->sb->inlineFunc.stmt = (STATEMENT*)tnode;
        tnode = exprNode(en_assign, tnode, exp);
        **stp = (STATEMENT*)Alloc(sizeof(STATEMENT));
        (**stp)->type = st_expr;
        (**stp)->select = tnode;
        *stp = &(**stp)->next;
    }
}
static STATEMENT* SetupArguments(FUNCTIONCALL* params)
{

    STATEMENT *st = nullptr, **stp = &st;
    INITLIST* al = params->arguments;
    SYMLIST* hr = basetype(params->sp->tp)->syms->table[0];
    if (ismember(params->sp))
    {
        SYMBOL* sx = hr->p;
        setExp(sx, params->thisptr, &stp);
        hr = hr->next;
    }
    while (al && hr)
    {
        SYMBOL* sx = hr->p;
        setExp(sx, al->exp, &stp);
        al = al->next;
        hr = hr->next;
    }
    return st;
}
/*-------------------------------------------------------------------------*/

void SetupVariables(SYMBOL* sym)
/* Copy all the func args into the xsyms table.
 * This copies the function parameters twice...
 */
{
    HASHTABLE* syms = sym->sb->inlineFunc.syms;
    while (syms)
    {
        SYMLIST* hr = syms->table[0];
        while (hr)
        {
            SYMBOL* sx = hr->p;
            if (sx->sb->storage_class == sc_auto)
            {
                EXPRESSION* ev = anonymousVar(sc_auto, sx->tp);
                deref(sx->tp, &ev);
                sx->sb->inlineFunc.stmt = (STATEMENT*)ev;
            }
            hr = hr->next;
        }
        syms = syms->next;
    }
}
/*-------------------------------------------------------------------------*/

EXPRESSION* doinline(FUNCTIONCALL* params, SYMBOL* funcsp)
{
    bool found = false;
    STATEMENT *stmt = nullptr, **stp = &stmt, *stmt1;
    EXPRESSION* newExpression;
    bool allocated = false;

    if (function_list_count >= MAX_INLINE_NESTING)
    {
        params->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (!isfunction(params->functp))
    {
        params->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (!params->sp->sb->attribs.inheritable.isInline)
    {
        params->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (params->sp->templateParams)
    {
        params->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (params->sp->sb->noinline)
    {
        params->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (!params->sp->sb->inlineFunc.syms)
    {
        params->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (!params->sp->sb->inlineFunc.stmt)
    {
        // recursive
        params->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (!localNameSpace->valueData->syms)
    {
        allocated = true;
        AllocateLocalContext(nullptr, nullptr, Optimizer::nextLabel++);
    }
    stmt1 = SetupArguments(params);
    if (stmt1)
    {
        // this will kill the ret val but we don't care since we've modified params
        stmt = (STATEMENT*)Alloc(sizeof(STATEMENT));
        stmt->type = st_block;
        stmt->lower = stmt1;
    }
    SetupVariables(params->sp);
    function_list[function_list_count++] = params;

    while (*stp)
        stp = &(*stp)->next;
    *stp = inlinestmt(params->sp->sb->inlineFunc.stmt);
    newExpression = exprNode(en_stmt, nullptr, nullptr);
    newExpression->v.stmt = stmt;

    if (params->sp->sb->retcount == 1)
    {
        /* optimization for simple inline functions that only have
         * one return statement, don't save to an intermediate variable
         */
        scanReturn(stmt, basetype(params->sp->tp)->btp);
    }
    else if (params->sp->sb->retcount)
    {
        newExpression->left = newReturn(basetype(params->sp->tp)->btp);
        reduceReturns(stmt, params->sp->tp->btp, newExpression->left);
    }
    optimize_for_constants(&newExpression->left);
    if (allocated)
    {
        FreeLocalContext(nullptr, nullptr, Optimizer::nextLabel++);
    }
    function_list_count--;
    if (newExpression->type == en_stmt)
        if (newExpression->v.stmt->type == st_block)
            if (!newExpression->v.stmt->lower)
                newExpression = intNode(en_c_i, 0);  // noop if there is no body
    if (newExpression->type == en_stmt)
    {
        newExpression->left = intNode(en_c_i, 0);
        if (isstructured(basetype(params->sp->tp)->btp))
            cast(&stdpointer, &newExpression->left);
        else
            cast(basetype(basetype(params->sp->tp)->btp), &newExpression->left);
    }
    return newExpression;
}
static bool IsEmptyBlocks(STATEMENT* block)
{
    bool rv = true;
    while (block != nullptr && rv)
    {
        switch (block->type)
        {
            case st_line:
            case st_varstart:
            case st_dbgblock:
                break;
            case st__genword:
            case st_try:
            case st_catch:
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
            case st_return:
            case st_goto:
            case st_expr:
            case st_declare:
            case st_select:
            case st_notselect:
            case st_switch:
            case st_passthrough:
            case st_datapassthrough:
                rv = false;
                break;
            case st_nop:
                break;
            case st_label:
                break;
            case st_block:
                rv = IsEmptyBlocks(block->lower) && block->blockTail == nullptr;
                break;
            default:
                diag("Invalid block type in IsEmptyBlocks");
                break;
        }
        block = block->next;
    }
    return rv;
}
bool IsEmptyFunction(FUNCTIONCALL* params, SYMBOL* funcsp)
{
    STATEMENT* st;
    if (!isfunction(params->functp))
        return false;
    if (!params->sp->sb->inlineFunc.stmt)
        return false;
    st = params->sp->sb->inlineFunc.stmt;
    while (st && st->type == st_expr)
    {
        st = st->next;
    }
    if (!st)
        return true;
    return true || IsEmptyBlocks(st);
}
}  // namespace Parser