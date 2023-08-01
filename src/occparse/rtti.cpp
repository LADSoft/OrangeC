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

#include "compiler.h"
#include "rtti.h"
#include "config.h"
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
#include "template.h"
#include "declcons.h"
#include "cpplookup.h"
#include "inline.h"
#include "symtab.h"

namespace Parser
{
SymbolTable<SYMBOL>* rttiSyms;

static std::set<SYMBOL*> defaultRecursionMap;

std::map<int, std::map<int, __xcentry*>> rttiStatements;

// in enum BasicType order
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

void rtti_init(void) 
{
    rttiSyms = symbols.CreateSymbolTable(); 
}
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
        case ExpressionNode::const_:
        case ExpressionNode::pc:
        case ExpressionNode::global:
        case ExpressionNode::auto_:
        case ExpressionNode::absolute:
        case ExpressionNode::threadlocal:
        case ExpressionNode::structelem:
            return node1->v.sp == node2->v.sp;
        case ExpressionNode::labcon:
            return node1->v.i == node2->v.i;
        default:
            return (!node1->left || equalnode(node1->left, node2->left)) &&
                   (!node1->right || equalnode(node1->right, node2->right));
        case ExpressionNode::c_i:
        case ExpressionNode::c_l:
        case ExpressionNode::c_ul:
        case ExpressionNode::c_ui:
        case ExpressionNode::c_c:
        case ExpressionNode::c_u16:
        case ExpressionNode::c_u32:
        case ExpressionNode::c_bool:
        case ExpressionNode::c_uc:
        case ExpressionNode::c_ll:
        case ExpressionNode::c_ull:
        case ExpressionNode::c_wc:
        case ExpressionNode::nullptr_:
            return node1->v.i == node2->v.i;
        case ExpressionNode::c_d:
        case ExpressionNode::c_f:
        case ExpressionNode::c_ld:
        case ExpressionNode::c_di:
        case ExpressionNode::c_fi:
        case ExpressionNode::c_ldi:
            return (*node1->v.f == *node2->v.f);
        case ExpressionNode::c_dc:
        case ExpressionNode::c_fc:
        case ExpressionNode::c_ldc:
            return (node1->v.c->r == node2->v.c->r) && (node1->v.c->i == node2->v.c->i);
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
    if (tp->type == BasicType::templateparam)
    {
        if (tp->templateParam && tp->templateParam->second->type == Keyword::_typename && tp->templateParam->second->byClass.val)
            tp = tp->templateParam->second->byClass.val;
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
    if (isstructured(tp) || tp->type == BasicType::enum_)
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
    else if (tp->type == BasicType::templateparam)
    {
        strcpy(buf, "*templateParam");
        buf += strlen(buf);
    }
    else if (tp->type == BasicType::any)
    {
        strcpy(buf, "any");
        buf += strlen(buf);
    }
    else if (isfunction(tp))
    {
        buf = RTTIGetDisplayName(buf, tp->btp);
        *buf++ = '(';
        *buf++ = '*';
        *buf++ = ')';
        *buf++ = '(';
        for (auto sym : *basetype(tp)->syms)
        {
            buf = RTTIGetDisplayName(buf, sym->tp);
            *buf++ = ',';
            *buf++ = ' ';
        }
        if (basetype(tp)->syms->size())
            buf -= 2;
        *buf++ = ')';
        *buf = 0;
    }
    else
    {
        strcpy(buf, typeNames[(int)tp->type]);
        buf += strlen(buf);
    }
    return buf;
}
static char* RTTIGetName(char* buf, TYPE* tp)
{
    if (tp->type == BasicType::templateparam)
    {
        if (tp->templateParam && tp->templateParam->second->type == Keyword::_typename && tp->templateParam->second->byClass.val)
            tp = tp->templateParam->second->byClass.val;
    }
    mangledNamesCount = 0;
    strcpy(buf, "@.xt@");
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
        sym = search(basetype(tp)->syms, overloadNameTab[CI_DESTRUCTOR]);
        // at this point if the class was never instantiated the destructor
        // may not have been created...
        if (sym)
        {
            if (!sym->sb->inlineFunc.stmt && !sym->sb->deferredCompile)
            {
                // if it is deleted we just won't call it...
                // still need the xt table entry though...
                if (basetype(sym->tp)->syms->size() && basetype(sym->tp)->syms->front()->sb->deleted)
                {
                    sym = nullptr;
                }
                else
                {
                    EXPRESSION* exp = intNode(ExpressionNode::c_i, 0);
                    callDestructor(basetype(tp)->sp, nullptr, &exp, nullptr, true, false, true, true);
                    if (exp && exp->left)
                    {
                        sym = exp->left->v.func->sp;
                        InsertInline(sym);
                    }
                }
            }
            else
            {
                sym = (SYMBOL*)basetype(sym->tp)->syms->front();
            }
            Optimizer::SymbolManager::Get(sym);
            if (sym && sym->sb->attribs.inheritable.linkage2 == Linkage::import_)
            {
                EXPRESSION* exp = varNode(ExpressionNode::pc, sym);
                thunkForImportTable(&exp);
                sym = exp->v.sp;
            }
        }
    }

    Optimizer::cseg();
    Optimizer::SymbolManager::Get(xtSym)->generated = true;
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
    tp = PerformDeferredInitialization(tp, nullptr);
    if (sym->sb->vbaseEntries)
    {
        for (auto vbase : *sym->sb->vbaseEntries)
        {
            if (vbase->alloc)
            {
                if (genXT)
                {
                    RTTIDumpType(vbase->cls->tp);
                }
                else
                {
                    SYMBOL* xtSym;
                    char name[4096];
                    RTTIGetName(name, vbase->cls->tp);
                    xtSym = search(rttiSyms, name);
                    if (!xtSym)
                    {
                        RTTIDumpType(vbase->cls->tp);
                        xtSym = search(rttiSyms, name);
                    }
                    Optimizer::genint(XD_CL_VIRTUAL);
                    Optimizer::genref(Optimizer::SymbolManager::Get(xtSym), 0);
                    Optimizer::genint(vbase->structOffset);
                }
            }
        }
    }
    if (sym->sb->baseClasses)
    {
        for (auto bc : *sym->sb->baseClasses)
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
                    xtSym = search(rttiSyms, name);
                    Optimizer::genint(XD_CL_BASE);
                    Optimizer::genref(Optimizer::SymbolManager::Get(xtSym), 0);
                    Optimizer::genint(bc->offset);
                }
            }
        }
    }
    if (sym->tp->syms)
    {
        for (auto member : *sym->tp->syms)
        {
            if (member->sb->storage_class == StorageClass::member || member->sb->storage_class == StorageClass::mutable_)
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
                }
            }
        }
    }
}
static void RTTIDumpStruct(SYMBOL* xtSym, TYPE* tp)
{
    Optimizer::SymbolManager::Get(xtSym)->generated = true;
    DumpEnclosedStructs(tp, true);
    RTTIDumpHeader(xtSym, tp, XD_CL_PRIMARY);
    DumpEnclosedStructs(tp, false);
    Optimizer::genint(0);
    Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(xtSym));
}
static void RTTIDumpArray(SYMBOL* xtSym, TYPE* tp)
{
    RTTIDumpHeader(xtSym, tp, XD_ARRAY | getSize(BasicType::int_));
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
SYMBOL* RTTIDumpType(TYPE*tp, bool symOnly)
{
    SYMBOL* xtSym = nullptr;
    if (IsCompiler())
    {
        if (Optimizer::cparams.prm_xcept)
        {
            char name[4096];
            RTTIGetName(name, tp);
            xtSym = search(rttiSyms, name);
            if (!xtSym || !Optimizer::SymbolManager::Get(xtSym)->generated)
            {
                if (!xtSym)
                {
                    xtSym = makeID(StorageClass::global, tp, nullptr, litlate(name));
                    xtSym->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                    if (isstructured(tp))
                        xtSym->sb->attribs.inheritable.linkage2 = basetype(tp)->sp->sb->attribs.inheritable.linkage2;
                    xtSym->sb->decoratedName = xtSym->name;
                    xtSym->sb->xtEntry = true;
                    rttiSyms->Add(xtSym);
                }
                if (!symOnly)
                {
                    Optimizer::SymbolManager::Get(xtSym)->generated = true;
                    switch (basetype(tp)->type)
                    {
                        case BasicType::lref:
                        case BasicType::rref:
                            RTTIDumpRef(xtSym, tp);
                            break;
                        case BasicType::pointer:
                            if (isarray(tp))
                                RTTIDumpArray(xtSym, tp);
                            else
                                RTTIDumpPointer(xtSym, tp);
                            break;
                        case BasicType::struct_:
                        case BasicType::class_:
                            RTTIDumpStruct(xtSym, tp);
                            break;
                        default:
                            RTTIDumpArithmetic(xtSym, tp);
                            break;
                    }
                }
            }
            else if (!symOnly)
            {
                while (ispointer(tp) || isref(tp))
                    tp = basetype(tp)->btp;
                if (isstructured(tp) && !basetype(tp)->sp->sb->dontinstantiate)
                {
                    SYMBOL* xtSym2;
                    // xtSym *should* be there.
                    RTTIGetName(name, basetype(tp));
                    xtSym2 = search(rttiSyms, name);
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
static void XCStmt(std::list<STATEMENT*>* block, std::list<XCENTRY*>& lst);
static void XCExpression(EXPRESSION* node, std::list<XCENTRY*>& lst)
{
    FUNCTIONCALL* fp;
    if (node == 0)
        return;
    switch (node->type)
    {
        case ExpressionNode::auto_:
            break;
        case ExpressionNode::const_:
            break;
        case ExpressionNode::c_ll:
        case ExpressionNode::c_ull:
        case ExpressionNode::c_d:
        case ExpressionNode::c_ld:
        case ExpressionNode::c_f:
        case ExpressionNode::c_dc:
        case ExpressionNode::c_ldc:
        case ExpressionNode::c_fc:
        case ExpressionNode::c_di:
        case ExpressionNode::c_ldi:
        case ExpressionNode::c_fi:
        case ExpressionNode::c_i:
        case ExpressionNode::c_l:
        case ExpressionNode::c_ui:
        case ExpressionNode::c_ul:
        case ExpressionNode::c_c:
        case ExpressionNode::c_bool:
        case ExpressionNode::c_uc:
        case ExpressionNode::c_wc:
        case ExpressionNode::c_u16:
        case ExpressionNode::c_u32:
        case ExpressionNode::c_string:
        case ExpressionNode::nullptr_:
        case ExpressionNode::memberptr:
        case ExpressionNode::structelem:
            break;
        case ExpressionNode::global:
        case ExpressionNode::pc:
        case ExpressionNode::labcon:
        case ExpressionNode::absolute:
        case ExpressionNode::threadlocal:
            break;
        case ExpressionNode::l_sp:
        case ExpressionNode::l_fp:
        case ExpressionNode::bits:
        case ExpressionNode::l_f:
        case ExpressionNode::l_d:
        case ExpressionNode::l_ld:
        case ExpressionNode::l_fi:
        case ExpressionNode::l_di:
        case ExpressionNode::l_ldi:
        case ExpressionNode::l_fc:
        case ExpressionNode::l_dc:
        case ExpressionNode::l_ldc:
        case ExpressionNode::l_c:
        case ExpressionNode::l_u16:
        case ExpressionNode::l_u32:
        case ExpressionNode::l_s:
        case ExpressionNode::l_ul:
        case ExpressionNode::l_l:
        case ExpressionNode::l_p:
        case ExpressionNode::l_ref:
        case ExpressionNode::l_i:
        case ExpressionNode::l_ui:
        case ExpressionNode::l_inative:
        case ExpressionNode::l_unative:
        case ExpressionNode::l_uc:
        case ExpressionNode::l_us:
        case ExpressionNode::l_bool:
        case ExpressionNode::l_bit:
        case ExpressionNode::l_ll:
        case ExpressionNode::l_ull:
        case ExpressionNode::l_string:
        case ExpressionNode::l_object:
        case ExpressionNode::literalclass:
        case ExpressionNode::l_wc:
            XCExpression(node->left, lst);
            break;
        case ExpressionNode::uminus:
        case ExpressionNode::compl_:
        case ExpressionNode::not_:
        case ExpressionNode::x_f:
        case ExpressionNode::x_d:
        case ExpressionNode::x_ld:
        case ExpressionNode::x_fi:
        case ExpressionNode::x_di:
        case ExpressionNode::x_ldi:
        case ExpressionNode::x_fc:
        case ExpressionNode::x_dc:
        case ExpressionNode::x_ldc:
        case ExpressionNode::x_ll:
        case ExpressionNode::x_ull:
        case ExpressionNode::x_i:
        case ExpressionNode::x_ui:
        case ExpressionNode::x_inative:
        case ExpressionNode::x_unative:
        case ExpressionNode::x_c:
        case ExpressionNode::x_u16:
        case ExpressionNode::x_u32:
        case ExpressionNode::x_wc:
        case ExpressionNode::x_uc:
        case ExpressionNode::x_bool:
        case ExpressionNode::x_bit:
        case ExpressionNode::x_s:
        case ExpressionNode::x_us:
        case ExpressionNode::x_l:
        case ExpressionNode::x_ul:
        case ExpressionNode::x_p:
        case ExpressionNode::x_fp:
        case ExpressionNode::x_sp:
        case ExpressionNode::x_string:
        case ExpressionNode::x_object:
        case ExpressionNode::trapcall:
        case ExpressionNode::shiftby:
            /*        case ExpressionNode::movebyref: */
        case ExpressionNode::substack:
        case ExpressionNode::alloca_:
        case ExpressionNode::loadstack:
        case ExpressionNode::savestack:
        case ExpressionNode::_initobj:
        case ExpressionNode::_sizeof:
            XCExpression(node->left, lst);
            break;
        case ExpressionNode::assign:
        case ExpressionNode::_initblk:
        case ExpressionNode::_cpblk:
            XCExpression(node->right, lst);
            XCExpression(node->left, lst);
            break;
        case ExpressionNode::auto_inc:
        case ExpressionNode::auto_dec:
            XCExpression(node->left, lst);
            break;
        case ExpressionNode::add:
        case ExpressionNode::sub:
            /*        case ExpressionNode::addcast: */
        case ExpressionNode::lsh:
        case ExpressionNode::arraylsh:
        case ExpressionNode::rsh:
            /*        case ExpressionNode::dvoid: */
        case ExpressionNode::arraymul:
        case ExpressionNode::arrayadd:
        case ExpressionNode::arraydiv:
        case ExpressionNode::structadd:
        case ExpressionNode::mul:
        case ExpressionNode::div:
        case ExpressionNode::umul:
        case ExpressionNode::udiv:
        case ExpressionNode::umod:
        case ExpressionNode::ursh:
        case ExpressionNode::mod:
        case ExpressionNode::and_:
        case ExpressionNode::or_:
        case ExpressionNode::xor_:
        case ExpressionNode::lor:
        case ExpressionNode::land:
        case ExpressionNode::eq:
        case ExpressionNode::ne:
        case ExpressionNode::gt:
        case ExpressionNode::ge:
        case ExpressionNode::lt:
        case ExpressionNode::le:
        case ExpressionNode::ugt:
        case ExpressionNode::uge:
        case ExpressionNode::ult:
        case ExpressionNode::ule:
        case ExpressionNode::cond:
        case ExpressionNode::intcall:
        case ExpressionNode::stackblock:
        case ExpressionNode::blockassign:
        case ExpressionNode::mp_compare:
        case ExpressionNode::dot:
        case ExpressionNode::pointsto:
            break;
        case ExpressionNode::void_:
        case ExpressionNode::void_nz:
            /*		case ExpressionNode::array: */
            XCExpression(node->right, lst);
        case ExpressionNode::mp_as_bool:
        case ExpressionNode::blockclear:
        case ExpressionNode::argnopush:
        case ExpressionNode::not__lvalue:
        case ExpressionNode::lvalue:
        case ExpressionNode::funcret:
        case ExpressionNode::select:
            XCExpression(node->left, lst);
            break;
        case ExpressionNode::thisref:
            XCExpression(node->left, lst);
            break;
        case ExpressionNode::atomic:
            break;
        case ExpressionNode::func:
            fp = node->v.func;
            {
                if (fp->arguments)
                    for (auto arg : *fp->arguments)
                        XCExpression(arg->exp, lst);
                if (fp->thisptr)
                    XCExpression(fp->thisptr, lst);
                if (fp->returnEXP)
                    XCExpression(fp->returnEXP, lst);
            }
            break;
        case ExpressionNode::stmt:
            XCStmt(node->v.stmt, lst);
            break;
        default:
            diag("XCExpression");
            break;
    }
}
static void XCStmt(std::list<STATEMENT*>* block, std::list<XCENTRY*>& lst)
{
    if (block)
    {
        for (auto stmt : *block)
        {
            switch (stmt->type)
            {
                case StatementNode::genword:
                    break;
                case StatementNode::catch_:
                case StatementNode::seh_catch_:
                case StatementNode::seh_finally_:
                case StatementNode::seh_fault_: {
                    __xcentry* temp = Allocate<__xcentry>();
                    temp->stmt = stmt;
                    temp->byStmt = true;
                    lst.push_back(temp);
                    XCStmt(stmt->lower, lst);
                    break;
                }
                case StatementNode::try_:
                case StatementNode::seh_try_:
                    XCStmt(stmt->lower, lst);
                    break;
                case StatementNode::return_:
                case StatementNode::expr:
                case StatementNode::declare:
                    XCExpression(stmt->select, lst);
                    break;
                case StatementNode::goto_:
                case StatementNode::label:
                    break;
                case StatementNode::select:
                case StatementNode::notselect:
                    XCExpression(stmt->select, lst);
                    break;
                case StatementNode::switch_:
                    XCExpression(stmt->select, lst);
                    XCStmt(stmt->lower, lst);
                    break;
                case StatementNode::block:
                    XCStmt(stmt->lower, lst);
                    XCStmt(stmt->blockTail, lst);
                    break;
                case StatementNode::passthrough:
                case StatementNode::nop:
                    break;
                case StatementNode::datapassthrough:
                    break;
                case StatementNode::line:
                case StatementNode::varstart:
                case StatementNode::dbgblock:
                    break;
                default:
                    diag("Invalid block type in XCStmt");
                    break;
            }
        }
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
                if (tp->type == BasicType::templateparam && tp->templateParam->second->packed)
                {
                    if (tp->templateParam->second->type == Keyword::_typename)
                    {
                        if (tp->templateParam->second->byPack.pack)
                        {
                            std::list<TEMPLATEPARAMPAIR>* pack = tp->templateParam->second->byPack.pack;
                            for (auto&& pack : *tp->templateParam->second->byPack.pack)
                            {
                                list[count++] = RTTIDumpType(pack.second->byClass.val);
                            }
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
        Optimizer::my_sprintf(name, "@.xct%s", funcsp->sb->decoratedName);
        xcSym = makeID(StorageClass::global, &stdpointer, nullptr, litlate(name));
        xcSym->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
        xcSym->sb->decoratedName = xcSym->name;
        Optimizer::cseg();
        Optimizer::SymbolManager::Get(xcSym)->generated = true;
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
        case ExpressionNode::add:
            return allocatedXC(exp->left) || allocatedXC(exp->right);
        case ExpressionNode::auto_:
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
        case ExpressionNode::l_p:
            return evalsp(exp->left);
        case ExpressionNode::add:
            rv = evalsp(exp->left);
            if (rv)
                return rv;
            return evalsp(exp->right);
        case ExpressionNode::auto_:
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
        case ExpressionNode::l_p:
            return evalofs(exp->left, funcsp);
        case ExpressionNode::add:
            return evalofs(exp->left, funcsp) + evalofs(exp->right, funcsp);
        case ExpressionNode::c_i:
        case ExpressionNode::c_ui:
        case ExpressionNode::c_l:
        case ExpressionNode::c_ul:
            return exp->v.i;
        case ExpressionNode::auto_:
            return exp->v.sp->sb->offset > 0 ? Optimizer::chosenAssembler->arch->retblockparamadjust : 0;
        case ExpressionNode::structelem:
            return exp->v.sp->sb->offset;
        default:
            return 0;
    }
}
static bool throughThis(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case ExpressionNode::add:
            return throughThis(exp->left) | throughThis(exp->right);
        case ExpressionNode::l_p:
            return (exp->left->type == ExpressionNode::auto_ && exp->left->v.sp->sb->thisPtr);
        default:
            return false;
    }
}
void XTDumpTab(SYMBOL* funcsp)
{
    if (funcsp->sb->xc && funcsp->sb->xc->xctab && Optimizer::cparams.prm_xcept)
    {
        std::list<XCENTRY*> list;
        SYMBOL* throwSym;
        XCStmt(funcsp->sb->inlineFunc.stmt, list);
        // this is done this way because the nested maps form a natural sorting mechanism...
        for (auto& s : rttiStatements)
            for (auto& e : s.second)
            {
                list.push_back(e.second);
            }
        for (auto p : list)
        {
            if (p->byStmt)
            {
                if (p->stmt->tp)
                    p->xtSym = RTTIDumpType(basetype(p->stmt->tp));
            }
            else
            {
                // ExpressionNode::thisref
                if (basetype(p->exp->v.t.tp)->sp->sb->hasDest)
                    p->xtSym = RTTIDumpType(basetype(p->exp->v.t.tp));
            }
        }
        throwSym = DumpXCSpecifiers(funcsp);
        Optimizer::SymbolManager::Get(funcsp->sb->xc->xclab)->generated = true;
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
        for (auto it = list.begin(); it != list.end(); ++it)
        {
            auto p = *it;
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
                // this was normalized in the back Keyword::_end...  depends on the RTTI information
                // being generated AFTER the function is generated, however...
                Optimizer::gen_labref(p->stmt->altlabel);
                Optimizer::genint(p->stmt->tryStart);
                Optimizer::genint(p->stmt->tryEnd);
            }
            else
            {
                if (p->xtSym && !p->exp->dest && allocatedXC(p->exp->v.t.thisptr))
                {
                    auto it1 = it;
                    for (; it1 != list.end(); ++it1)
                    {
                        auto q = *it1;
                        if (!q->byStmt && q->exp->dest)
                        {
                            if (equalnode(p->exp->v.t.thisptr, q->exp->v.t.thisptr))
                                q->used = true;
                        }
                    }
                    auto thsym = evalsp(p->exp->v.t.thisptr);
                    if (thsym && thsym->generated)
                    {
                        Optimizer::genint(XD_CL_PRIMARY | (throughThis(p->exp) ? XD_THIS : 0));
                        Optimizer::genref(Optimizer::SymbolManager::Get(p->xtSym), 0);
                        Optimizer::gen_autoref(thsym, evalofs(p->exp->v.t.thisptr, funcsp));
                        Optimizer::genint(p->exp->v.t.thisptr->xcInit);
                        Optimizer::genint(p->exp->v.t.thisptr->xcDest);
                    }
                }
            }
        }
        // for arguments which are destructed
        for (auto p : list)
        {
            if (!p->byStmt && p->xtSym && p->exp->dest && !p->used)
            {
                p->used = true;
                auto thsym = evalsp(p->exp->v.t.thisptr);
                if (thsym && thsym->generated)
                {
                    Optimizer::genint(XD_CL_PRIMARY | (throughThis(p->exp) ? XD_THIS : 0));
                    Optimizer::genref(Optimizer::SymbolManager::Get(p->xtSym), 0);
                    Optimizer::gen_autoref(thsym, evalofs(p->exp->v.t.thisptr, funcsp));
                    Optimizer::genint(0);
                    Optimizer::genint(p->exp->v.t.thisptr->xcDest);
                }
            }
        }
        Optimizer::genint(0);
        Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(funcsp->sb->xc->xclab));
    }
}
}  // namespace Parser