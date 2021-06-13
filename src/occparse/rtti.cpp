/* Software License Agreement
 * 
 *     Copyright(C) 1994-2021 David Lindauer, (LADSoft)
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
#include "rtti.h"
#include "config.h"
#include "symtab.h"
#include "mangle.h"
#include "initbackend.h"
#include "expr.h"
#include "declcpp.h"
#include "iexpr.h"
#include "OptUtils.h"
#include "declare.h"
#include "memory.h"
#include "help.h"
#include "beinterf.h"
#include "ildata.h"
#include "ccerr.h"
#include "declcons.h"
#include "cpplookup.h"
#include "inline.h"

namespace Parser
{
HASHTABLE* rttiSyms;

static std::set<SYMBOL*> defaultRecursionMap;

std::map<int, std::map<int, __xclist*>> rttiStatements;

// in enum e_bt order
static const char* typeNames[] = {"bit",
                                  "bool",
                                  "signed char",
                                  "char",
                                  "unsigned char",
                                  "short",
                                  "char16_t",
                                  "unsigned short",
                                  "wchar_t",
                                  nullptr,
                                  "int",
                                  "inative",
                                  "char32_t",
                                  "unsigned",
                                  "unative",
                                  "long",
                                  "unsigned long",
                                  "long long",
                                  "unsigned long long",
                                  "float",
                                  "double",
                                  "long double",
                                  "float imaginary",
                                  "double imaginary",
                                  "long double imaginary",
                                  "float complex",
                                  "double complex",
                                  "long double complex",
                                  "void",
                                  "__object",
                                  "__string"};

void rtti_init(void) { rttiSyms = CreateHashTable(32); }
bool equalnode(EXPRESSION* node1, EXPRESSION* node2)
/*
 *      equalnode will return 1 if the expressions pointed to by
 *      node1 and node2 are equivalent.
 */
{
    if (node1 == 0 || node2 == 0)
        return 0;
    if (node1->type != node2->type)
        return 0;
    if (natural_size(node1) != natural_size(node2))
        return 0;
    switch (node1->type)
    {
        case en_const:
        case en_pc:
        case en_global:
        case en_auto:
        case en_absolute:
        case en_threadlocal:
        case en_structelem:
            return node1->v.sp == node2->v.sp;
        case en_labcon:
            return node1->v.i == node2->v.i;
        default:
            return (!node1->left || equalnode(node1->left, node2->left)) &&
                   (!node1->right || equalnode(node1->right, node2->right));
        case en_c_i:
        case en_c_l:
        case en_c_ul:
        case en_c_ui:
        case en_c_c:
        case en_c_u16:
        case en_c_u32:
        case en_c_bool:
        case en_c_uc:
        case en_c_ll:
        case en_c_ull:
        case en_c_wc:
        case en_nullptr:
            return node1->v.i == node2->v.i;
        case en_c_d:
        case en_c_f:
        case en_c_ld:
        case en_c_di:
        case en_c_fi:
        case en_c_ldi:
            return (*node1->v.f == *node2->v.f);
        case en_c_dc:
        case en_c_fc:
        case en_c_ldc:
            return (node1->v.c->r == node2->v.c->r) && (node1->v.c->i == node2->v.c->i);
        case en_tempref:
            return node1->v.sp == node2->v.sp;
    }
}

static char* addNameSpace(char* buf, SYMBOL* sym)
{
    if (!sym)
        return buf;
    buf = addNameSpace(buf, sym->sb->parentNameSpace);
    Optimizer::my_sprintf(buf, "%s::", sym->name);
    return buf + strlen(buf);
}
static char* addParent(char* buf, SYMBOL* sym)
{
    if (!sym)
        return buf;
    if (sym->sb->parentClass)
        buf = addParent(buf, sym->sb->parentClass);
    else
        buf = addNameSpace(buf, sym->sb->parentNameSpace);
    Optimizer::my_sprintf(buf, "%s", sym->name);
    return buf + strlen(buf);
}
static char* RTTIGetDisplayName(char* buf, TYPE* tp)
{
    if (tp->type == bt_templateparam)
    {
        if (tp->templateParam && tp->templateParam->p->type == kw_typename && tp->templateParam->p->byClass.val)
            tp = tp->templateParam->p->byClass.val;
    }
    if (isconst(tp))
    {
        Optimizer::my_sprintf(buf, "%s ", "const");
        buf += strlen(buf);
    }
    if (isvolatile(tp))
    {
        Optimizer::my_sprintf(buf, "%s ", "volatile");
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
        SYMLIST* hr = basetype(tp)->syms->table[0];
        buf = RTTIGetDisplayName(buf, tp->btp);
        *buf++ = '(';
        *buf++ = '*';
        *buf++ = ')';
        *buf++ = '(';
        while (hr)
        {
            buf = RTTIGetDisplayName(buf, hr->p->tp);
            if (hr->next)
            {
                *buf++ = ',';
                *buf++ = ' ';
            }
            hr = hr->next;
        }
        *buf++ = ')';
        *buf = 0;
    }
    else
    {
        strcpy(buf, typeNames[tp->type]);
        buf += strlen(buf);
    }
    return buf;
}
static char* RTTIGetName(char* buf, TYPE* tp)
{
    if (tp->type == bt_templateparam)
    {
        if (tp->templateParam && tp->templateParam->p->type == kw_typename && tp->templateParam->p->byClass.val)
            tp = tp->templateParam->p->byClass.val;
    }
    mangledNamesCount = 0;
    strcpy(buf, "@$xt@");
    buf += strlen(buf);
    buf = mangleType(buf, tp, true);
    return buf;
}
static void RTTIDumpHeader(SYMBOL* xtSym, TYPE* tp, int flags)
{
    char name[4096], *p;
    SYMBOL* sym = nullptr;
    if (ispointer(tp))
    {
        sym = RTTIDumpType(basetype(tp)->btp);
        flags = isarray(tp) ? XD_ARRAY : XD_POINTER;
    }
    else if (isref(tp))
    {
        sym = RTTIDumpType(basetype(tp)->btp);
        flags = XD_REF;
    }
    else if (isstructured(tp) && !basetype(tp)->sp->sb->trivialCons)
    {
        sym = search(overloadNameTab[CI_DESTRUCTOR], basetype(tp)->syms);
        // at this point if the class was never instantiated the destructor
        // may not have been created...
        if (sym)
        {
            if (!sym->sb->inlineFunc.stmt && !sym->sb->deferredCompile)
            {
                // if it is deleted we just won't call it...
                // still need the xt table entry though...
                if (basetype(sym->tp)->syms->table[0]->p && basetype(sym->tp)->syms->table[0]->p->sb->deleted)
                {
                    sym = nullptr;
                }
                else
                {
                    EXPRESSION* exp = intNode(en_c_i, 0);
                    callDestructor(basetype(tp)->sp, nullptr, &exp, nullptr, true, false, true, true);
                    if (exp && exp->left)
                        sym = exp->left->v.func->sp;
                }
            }
            else
            {
                sym = (SYMBOL*)basetype(sym->tp)->syms->table[0]->p;
            }
            Optimizer::SymbolManager::Get(sym);
            if (sym && sym->sb->attribs.inheritable.linkage2 == lk_import)
            {
                EXPRESSION* exp = varNode(en_pc, sym);
                thunkForImportTable(&exp);
                sym = exp->v.sp;
            }
        }
    }

    Optimizer::cseg();
    Optimizer::gen_virtual(Optimizer::SymbolManager::Get(xtSym), false);
    if (sym)
    {
        Optimizer::genref(Optimizer::SymbolManager::Get(sym), 0);
    }
    else
    {
        Optimizer::genaddress(0);
    }
    Optimizer::genint(tp->size);
    Optimizer::genint(flags);
    RTTIGetDisplayName(name, tp);
    for (p = name; *p; p++)
        Optimizer::genbyte(*p);
    Optimizer::genbyte(0);
}
static void DumpEnclosedStructs(TYPE* tp, bool genXT)
{
    SYMBOL* sym = basetype(tp)->sp;
    SYMLIST* hr;
    tp = PerformDeferredInitialization(tp, nullptr);
    if (sym->sb->vbaseEntries)
    {
        VBASEENTRY* entries = sym->sb->vbaseEntries;
        while (entries)
        {
            if (entries->alloc)
            {
                if (genXT)
                {
                    RTTIDumpType(entries->cls->tp);
                }
                else
                {
                    SYMBOL* xtSym;
                    char name[4096];
                    RTTIGetName(name, entries->cls->tp);
                    xtSym = search(name, rttiSyms);
                    if (!xtSym)
                    {
                        RTTIDumpType(entries->cls->tp);
                        xtSym = search(name, rttiSyms);
                    }
                    Optimizer::genint(XD_CL_VIRTUAL);
                    Optimizer::genref(Optimizer::SymbolManager::Get(xtSym), 0);
                    Optimizer::genint(entries->structOffset);
                }
            }
            entries = entries->next;
        }
    }
    if (sym->sb->baseClasses)
    {
        BASECLASS* bc = sym->sb->baseClasses;
        while (bc)
        {
            if (!bc->isvirtual)
            {
                if (genXT)
                {
                    RTTIDumpType(bc->cls->tp);
                }
                else
                {
                    SYMBOL* xtSym;
                    char name[4096];
                    RTTIGetName(name, bc->cls->tp);
                    xtSym = search(name, rttiSyms);
                    Optimizer::genint(XD_CL_BASE);
                    Optimizer::genref(Optimizer::SymbolManager::Get(xtSym), 0);
                    Optimizer::genint(bc->offset);
                }
            }
            bc = bc->next;
        }
    }
    if (sym->tp->syms)
    {
        hr = sym->tp->syms->table[0];
        while (hr)
        {
            SYMBOL* member = hr->p;
            if (member->sb->storage_class == sc_member || member->sb->storage_class == sc_mutable)
            {
                TYPE* tp = member->tp;
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
                    tp = PerformDeferredInitialization(tp, nullptr);
                    if (genXT)
                    {
                        RTTIDumpType(tp);
                    }
                    /*
                    else
                    {
                        SYMBOL*xtSym;
                        char name[4096];
                        RTTIGetName(name, tp);
                        xtSym = search(name, rttiSyms);
                        Optimizer::genint(flags);
                        Optimizer::genref(xtSym , 0);
                        genint(member->sb->offset);
                    }
                    */
                }
            }
            hr = hr->next;
        }
    }
}
static void RTTIDumpStruct(SYMBOL* xtSym, TYPE* tp)
{
    Optimizer::SymbolManager::Get(xtSym)->noextern = true;
    DumpEnclosedStructs(tp, true);
    RTTIDumpHeader(xtSym, tp, XD_CL_PRIMARY);
    DumpEnclosedStructs(tp, false);
    Optimizer::genint(0);
    Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(xtSym));
}
static void RTTIDumpArray(SYMBOL* xtSym, TYPE* tp)
{
    RTTIDumpHeader(xtSym, tp, XD_ARRAY | getSize(bt_int));
    Optimizer::genint(tp->size / (tp->btp->size));
    Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(xtSym));
}
static void RTTIDumpPointer(SYMBOL* xtSym, TYPE* tp)
{
    RTTIDumpHeader(xtSym, tp, XD_POINTER);
    Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(xtSym));
}
static void RTTIDumpRef(SYMBOL* xtSym, TYPE* tp)
{
    RTTIDumpHeader(xtSym, tp, XD_REF);
    Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(xtSym));
}
static void RTTIDumpArithmetic(SYMBOL* xtSym, TYPE* tp)
{
    RTTIDumpHeader(xtSym, tp, 0);
    Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(xtSym));
}
SYMBOL* RTTIDumpType(TYPE* tp)
{
    SYMBOL* xtSym = nullptr;
    if (IsCompiler())
    {
        if (Optimizer::cparams.prm_xcept)
        {
            char name[4096];
            RTTIGetName(name, tp);
            xtSym = search(name, rttiSyms);
            if (!xtSym)
            {
                xtSym = makeID(sc_global, tp, nullptr, litlate(name));
                xtSym->sb->attribs.inheritable.linkage = lk_virtual;
                if (isstructured(tp))
                    xtSym->sb->attribs.inheritable.linkage2 = basetype(tp)->sp->sb->attribs.inheritable.linkage2;
                xtSym->sb->decoratedName = xtSym->name;
                xtSym->sb->xtEntry = true;
                insert(xtSym, rttiSyms);
                if (isstructured(tp) && basetype(tp)->sp->sb->dontinstantiate &&
                    basetype(tp)->sp->sb->attribs.inheritable.linkage2 != lk_import)
                {
                    xtSym->sb->dontinstantiate = true;
                    Optimizer::SymbolManager::Get(xtSym);
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
            else
            {
                while (ispointer(tp) || isref(tp))
                    tp = basetype(tp)->btp;
                if (isstructured(tp) && !basetype(tp)->sp->sb->dontinstantiate)
                {
                    SYMBOL* xtSym2;
                    // xtSym *should* be there.
                    RTTIGetName(name, basetype(tp));
                    xtSym2 = search(name, rttiSyms);
                    if (xtSym2 && xtSym2->sb->dontinstantiate)
                    {
                        xtSym2->sb->dontinstantiate = false;
                        RTTIDumpStruct(xtSym2, tp);
                    }
                }
            }
        }
    }
    return xtSym;
}
static void XCStmt(STATEMENT* block, std::map<int, std::map<int, __xclist*>> & lst);
static void XCExpression(EXPRESSION* node, std::map<int, std::map<int, __xclist*>> & lst)
{
    FUNCTIONCALL* fp;
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
            XCExpression(node->left, lst);
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
        case en__initobj:
        case en__sizeof:
            XCExpression(node->left, lst);
            break;
        case en_assign:
        case en__initblk:
        case en__cpblk:
            XCExpression(node->right, lst);
            XCExpression(node->left, lst);
            break;
        case en_autoinc:
        case en_autodec:
            XCExpression(node->left, lst);
            break;
        case en_add:
        case en_sub:
            /*        case en_addcast: */
        case en_lsh:
        case en_arraylsh:
        case en_rsh:
        case en_rshd:
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
        case en_void:
        case en_voidnz:
            /*		case en_array: */
            XCExpression(node->right, lst);
        case en_mp_as_bool:
        case en_blockclear:
        case en_argnopush:
        case en_not_lvalue:
        case en_lvalue:
        case en_funcret:
            XCExpression(node->left, lst);
            break;
        case en_thisref:
            XCExpression(node->left, lst);
            break;
        case en_atomic:
            break;
        case en_func:
            fp = node->v.func;
            {
                INITLIST* args = fp->arguments;
                while (args)
                {
                    XCExpression(args->exp, lst);
                    args = args->next;
                }
                if (fp->thisptr)
                    XCExpression(fp->thisptr, lst);
                if (fp->returnEXP)
                    XCExpression(fp->returnEXP, lst);
            }
            break;
        case en_stmt:
            XCStmt(node->v.stmt, lst);
            break;
        default:
            diag("XCExpression");
            break;
    }
}
static void XCStmt(STATEMENT* block, std::map<int, std::map<int, __xclist*>> & lst)
{
    while (block != nullptr)
    {
        switch (block->type)
        {
            case st__genword:
                break;
            case st_catch:
            case st___catch:
            case st___finally:
            case st___fault:
            {
                __xclist* temp = Allocate<__xclist>();
                temp->stmt = block;
                temp->byStmt = true;
                lst[block->tryStart][block->tryEnd] = temp;
                XCStmt(block->lower, lst);
                break;
            }
            case st_try:
            case st___try:
                XCStmt(block->lower, lst);
                break;
            case st_return:
            case st_expr:
            case st_declare:
                XCExpression(block->select, lst);
                break;
            case st_goto:
            case st_label:
                break;
            case st_select:
            case st_notselect:
                XCExpression(block->select, lst);
                break;
            case st_switch:
                XCExpression(block->select, lst);
                XCStmt(block->lower, lst);
                break;
            case st_block:
                XCStmt(block->lower, lst);
                XCStmt(block->blockTail, lst);
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
static SYMBOL* DumpXCSpecifiers(SYMBOL* funcsp)
{
    SYMBOL* xcSym = nullptr;
    if (funcsp->sb->xcMode != xc_unspecified)
    {
        char name[4096];
        SYMBOL* list[1000];
        int count = 0, i;
        if (funcsp->sb->xcMode == xc_dynamic)
        {
            Optimizer::LIST* p = funcsp->sb->xc->xcDynamic;
            while (p)
            {
                TYPE* tp = (TYPE*)p->data;
                if (tp->type == bt_templateparam && tp->templateParam->p->packed)
                {
                    if (tp->templateParam->p->type == kw_typename)
                    {
                        TEMPLATEPARAMLIST* pack = tp->templateParam->p->byPack.pack;
                        while (pack)
                        {
                            list[count++] = RTTIDumpType((TYPE*)pack->p->byClass.val);
                            pack = pack->next;
                        }
                    }
                }
                else
                {
                    list[count++] = RTTIDumpType((TYPE*)p->data);
                }
                p = p->next;
            }
        }
        Optimizer::my_sprintf(name, "@$xct%s", funcsp->sb->decoratedName);
        xcSym = makeID(sc_global, &stdpointer, nullptr, litlate(name));
        xcSym->sb->attribs.inheritable.linkage = lk_virtual;
        xcSym->sb->decoratedName = xcSym->name;
        Optimizer::cseg();
        Optimizer::gen_virtual(Optimizer::SymbolManager::Get(xcSym), false);
        switch (funcsp->sb->xcMode)
        {
            case xc_none:
                Optimizer::genint(XD_NOXC);
                Optimizer::genint(0);
                break;
            case xc_all:
                Optimizer::genint(XD_ALLXC);
                Optimizer::genint(0);
                break;
            case xc_dynamic:
                Optimizer::genint(XD_DYNAMICXC);
                for (i = 0; i < count; i++)
                {
                    Optimizer::genref(Optimizer::SymbolManager::Get(list[i]), 0);
                }
                Optimizer::genint(0);
                break;
            case xc_unspecified:
                break;
        }
        Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(xcSym));
    }
    return xcSym;
}
static bool allocatedXC(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case en_add:
            return allocatedXC(exp->left) || allocatedXC(exp->right);
        case en_auto:
            return exp->v.sp->sb->allocate;
        default:
            return false;
    }
}
Optimizer::SimpleSymbol* evalsp(EXPRESSION* exp)
{
    switch (exp->type)
    {
        Optimizer::SimpleSymbol* rv;
        case en_l_p:
            return evalsp(exp->left);
        case en_add:
            rv = evalsp(exp->left);
            if (rv)
                return rv;
            return evalsp(exp->right);
        case en_auto:
            rv = Optimizer::SymbolManager::Get(exp->v.sp);
            return rv;
        default:
            return nullptr;
    }
}
static int evalofs(EXPRESSION* exp, SYMBOL* funcsp)
{
    switch (exp->type)
    {
        case en_l_p:
            return evalofs(exp->left, funcsp);
        case en_add:
            return evalofs(exp->left, funcsp) + evalofs(exp->right, funcsp);
        case en_c_i:
        case en_c_ui:
        case en_c_l:
        case en_c_ul:
            return exp->v.i;
        case en_auto:
            return 0;
            //            return exp->v.sp->sb->offset + (exp->v.sp->sb->offset > 0 ? funcsp->sb->retblockparamadjust : 0);
        case en_structelem:
            return exp->v.sp->sb->offset;
        default:
            return 0;
    }
}
static bool throughThis(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case en_add:
            return throughThis(exp->left) | throughThis(exp->right);
        case en_l_p:
            return (exp->left->type == en_auto && exp->left->v.sp->sb->thisPtr);
        default:
            return false;
    }
}
void XTDumpTab(SYMBOL* funcsp)
{
    if (funcsp->sb->xc && funcsp->sb->xc->xctab && Optimizer::cparams.prm_xcept)
    {
        XCLIST* list = nullptr, * p, *last = nullptr;
        SYMBOL* throwSym;
        XCStmt(funcsp->sb->inlineFunc.stmt, rttiStatements);
        // this is done this way because the nested maps form a natural sorting mechanism...
        for (auto& s : rttiStatements)
            for (auto& e : s.second)
            {
                p = e.second;
                if (last)
                    last->next = p;
                else 
                    list = p;
                last = p;
            }
        p = list;
        while (p)
        {
            if (p->byStmt)
            {
                if (p->stmt->tp)
                    p->xtSym = RTTIDumpType(basetype(p->stmt->tp));
            }
            else
            {
                // en_thisref
                if (basetype(p->exp->v.t.tp)->sp->sb->hasDest)
                    p->xtSym = RTTIDumpType(basetype(p->exp->v.t.tp));
            }
            p = p->next;
        }
        throwSym = DumpXCSpecifiers(funcsp);
        Optimizer::gen_virtual(Optimizer::SymbolManager::Get(funcsp->sb->xc->xclab), false);
        if (throwSym)
        {
            Optimizer::genref(Optimizer::SymbolManager::Get(throwSym), 0);
        }
        else
        {
            Optimizer::genaddress(0);
        }
        Optimizer::gen_autoref(Optimizer::SymbolManager::Get(funcsp->sb->xc->xctab), 0);
        //        genint(funcsp->sb->xc->xctab->sb->offset);
        p = list;
        while (p)
        {
            if (p->byStmt)
            {
                Optimizer::genint(XD_CL_TRYBLOCK);
                if (p->xtSym)
                {
                    Optimizer::genref(Optimizer::SymbolManager::Get(p->xtSym), 0);
                }
                else
                {
                    Optimizer::genaddress(0);
                }
                // this was normalized in the back end...  depends on the RTTI information
                // being generated AFTER the function is generated, however...
                Optimizer::gen_labref(p->stmt->altlabel);
                Optimizer::genint(p->stmt->tryStart);
                Optimizer::genint(p->stmt->tryEnd);
            }
            else
            {
                if (p->xtSym && !p->exp->dest && allocatedXC(p->exp->v.t.thisptr))
                {        
                    XCLIST* q = p;
                    while (q)
                    {
                        if (!q->byStmt && q->exp->dest)
                        {
                            if (equalnode(p->exp->v.t.thisptr, q->exp->v.t.thisptr))
                                break;
                        }
                        q = q->next;
                    }
                    if (q)
                        q->used = true;
                    auto thsym = evalsp(p->exp->v.t.thisptr);
                    if (thsym && thsym->genreffed)
                    {
                        Optimizer::genint(XD_CL_PRIMARY | (throughThis(p->exp) ? XD_THIS : 0));
                        Optimizer::genref(Optimizer::SymbolManager::Get(p->xtSym), 0);
                        Optimizer::gen_autoref(thsym, evalofs(p->exp->v.t.thisptr, funcsp));
                        Optimizer::genint(p->exp->v.t.thisptr->xcInit);
                        Optimizer::genint(p->exp->v.t.thisptr->xcDest);
                    }
                }
            }
            p = p->next;
        }
        // for arguments which are destructed
        p = list;
        while (p)
        {
            if (!p->byStmt && p->xtSym && p->exp->dest && !p->used)
            {
                p->used = true;
                auto thsym = evalsp(p->exp->v.t.thisptr);
                if (thsym && thsym->genreffed)
                {
                    Optimizer::genint(XD_CL_PRIMARY | (throughThis(p->exp) ? XD_THIS : 0));
                    Optimizer::genref(Optimizer::SymbolManager::Get(p->xtSym), 0);
                    Optimizer::gen_autoref(thsym, evalofs(p->exp->v.t.thisptr, funcsp));
                    Optimizer::genint(0);
                    Optimizer::genint(p->exp->v.t.thisptr->xcDest);
                }
            }
            p = p->next;
        }
        Optimizer::genint(0);
        Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(funcsp->sb->xc->xclab));
    }
}
}  // namespace Parser