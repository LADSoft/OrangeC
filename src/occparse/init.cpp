/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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
 * 
 */

#include "compiler.h"
#include <cassert>
#include <climits>
#include "PreProcessor.h"
#include "init.h"
#include "ccerr.h"
#include "config.h"
#include "initbackend.h"
#include "stmt.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "declare.h"
#include "mangle.h"
#include "ildata.h"
#include "occparse.h"
#include "expr.h"
#include "lex.h"
#include "help.h"
#include "OptUtils.h"
#include "exprcpp.h"
#include "constopt.h"
#include "declcons.h"
#include "declcpp.h"
#include "beinterf.h"
#include "memory.h"
#include "inline.h"
#include "Property.h"
#include "istmt.h"
#include "types.h"
#include "browse.h"
#include "constexpr.h"
#include "namespace.h"
#include "symtab.h"
#include "inline.h"
#include "ListFactory.h"
#include "overload.h"
#include "class.h"
#include "exprpacked.h"

 /* initializers, local... can do w/out c99 */
#define CPP_BASE_PRIO 99 // this is low prio, high prio is 98 

namespace Parser
{
bool initializingGlobalVar;
int ignore_global_init;

static std::list<DynamicInitializer>dynamicInitializers, TLSInitializers;
static std::list<DynamicInitializer>dynamicDestructors, TLSDestructors;

static Optimizer::LIST *symListHead, *symListTail;
static int inittag = 0;
static std::list<SYMBOL*> file_level_constructors;
void init_init(void)
{
    symListHead = nullptr;
    dynamicInitializers.clear();
    TLSInitializers.clear();
    dynamicDestructors.clear();
    TLSDestructors.clear();
    initializingGlobalVar = false;
    file_level_constructors.clear();
}


int genstring(StringData* str)
/*
 * Generate a string literal
 */
{
    if (str->refCount > 0)
    {
        int size = 1;
        int i;
        bool instring = false;
        char buf[10000], *dest = buf;
        for (i = 0; i < str->size; i++)
        {
            LCHAR* p = str->pointers[i]->str;
            int n = str->pointers[i]->count;
            size += n;
            // if (str->strtype == LexType::l_astr_ && Optimizer::chosenAssembler->gen->gen_string)
            // Optimizer::chosenAssembler->gen->gen_string(p, n);
            while (n--)
            {
                switch (str->strtype)
                {
                    case LexType::l_wstr_:
                        Optimizer::genwchar_t(*p++);
                        break;
                    case LexType::l_ustr_:
                        Optimizer::genuint16(*p++);
                        break;
                    case LexType::l_Ustr_:
                        Optimizer::genuint32(*p++);
                        break;
                    default:
                        *dest++ = *p++;
                        if (dest - buf >= sizeof(buf) - 1)
                        {
                            Optimizer::putstring(buf, dest - buf);
                            dest = buf;
                        }

                        break;
                }
            }
        }
        if (dest != buf)
            Optimizer::putstring(buf, dest - buf);

        switch (str->strtype)
        {
            case LexType::l_wstr_:
                Optimizer::genwchar_t(0);
                size *= getSize(BasicType::wchar_t_);
                break;
            case LexType::l_ustr_:
                Optimizer::genuint16(0);
                size *= 2;
                break;
            case LexType::l_Ustr_:
                Optimizer::genuint32(0);
                size *= 4;
                break;
            default:
                Optimizer::genbyte(0);
                break;
        }
        return size;
    }
    return 0;
}

/*-------------------------------------------------------------------------*/

void dumpLits(void)
/*
 *      dump the string literal pool.
 */
{
    int n = 0;
    for (auto s : strtab)
    {
        Optimizer::xstringseg();
        switch (s->strtype)
        {
            case LexType::l_wstr_:
            case LexType::l_ustr_:
                if (n % 2)
                    Optimizer::genbyte(0);
                break;
            case LexType::l_Ustr_: {
                int remainder = 4 - n % 4;
                if (remainder != 4)
                    for (int i = 0; i < remainder; i++)
                        Optimizer::genbyte(0);
            }
            break;
        }
        Optimizer::put_string_label(s->label, (int)s->strtype);
        n += genstring(s);
    }
}

void dumpStartups(void)
{
    if (IsCompiler())
    {
        SYMBOL* s;
        bool started = false;
        for (auto&& starts : preProcessor->GetStartups())
        {
            if (starts.second->startup)
            {
                if (!started)
                {
                    started = true;
                    Optimizer::startupseg();
                }
                s = search(globalNameSpace->front()->syms, starts.first.c_str());
                if (!s || s->sb->storage_class != StorageClass::overloads_)
                    errorstr(ERR_UNDEFINED_IDENTIFIER, starts.first.c_str());
                else
                {
                    s = search(s->tp->syms, starts.first.c_str());
                    Optimizer::gensrref(Optimizer::SymbolManager::Get(s), starts.second->prio, STARTUP_TYPE_STARTUP);
                    s->sb->attribs.inheritable.used = true;
                }
            }
        }
        for (auto s : file_level_constructors)
        {
            if (s->sb->attribs.uninheritable.constructorPriority)
            {
                if (!started)
                {
                    started = true;
                    Optimizer::startupseg();
                }
                Optimizer::gensrref(Optimizer::SymbolManager::Get(s), s->sb->attribs.uninheritable.constructorPriority < 256 ? s->sb->attribs.uninheritable.constructorPriority : 255, STARTUP_TYPE_STARTUP);
                s->sb->attribs.inheritable.used = true;
            }
        }
        started = false;
        for (auto&& starts : preProcessor->GetStartups())
        {
            if (!starts.second->startup)
            {
                if (!started)
                {
                    started = true;
                    Optimizer::rundownseg();
                }
                s = search(globalNameSpace->front()->syms, starts.first.c_str());
                if (!s || s->sb->storage_class != StorageClass::overloads_)
                    errorstr(ERR_UNDEFINED_IDENTIFIER, starts.first.c_str());
                else
                {
                    s = search(s->tp->syms, starts.first.c_str());
                    Optimizer::gensrref(Optimizer::SymbolManager::Get(s), starts.second->prio, STARTUP_TYPE_RUNDOWN);
                    s->sb->attribs.inheritable.used = true;
                }
            }
        }
        for (auto s : file_level_constructors)
        {
            if (s->sb->attribs.uninheritable.destructorPriority)
            {
                if (!started)
                {
                    started = true;
                    Optimizer::rundownseg();
                }
                Optimizer::gensrref(Optimizer::SymbolManager::Get(s), s->sb->attribs.uninheritable.destructorPriority < 256 ?s->sb->attribs.uninheritable.destructorPriority : 255, STARTUP_TYPE_RUNDOWN);
                s->sb->attribs.inheritable.used = true;
            }
        }
    }
}
static int dumpBits(std::list<Initializer*>::iterator &it)
{
    if (IsCompiler())
    {
        int offset = (*it)->offset;
        long long resolver = 0;
        Type* base = (*it)->basetp->BaseType();
        do
        {
            long long i = 0;
            Type* tp = (*it)->basetp->BaseType();
            if (tp->anonymousbits)
            {
                ++it;
                continue;
            }
            if (isfloatconst((*it)->exp))
            {
                i = (long long)((*it)->exp->v.f);
            }
            else if (isintconst((*it)->exp))
            {
                i = (*it)->exp->v.i;
            }
            else if (isimaginaryconst((*it)->exp))
            {
                i = 0;
            }
            else if (iscomplexconst((*it)->exp))
            {
                i = (long long)((*it)->exp->v.c->r);
            }
            else
                diag("dump-bits: non-constant");
#ifdef ERROR
#    error REVERSE BITS
#endif
            i &= Optimizer::mod_mask(tp->bits);
            resolver |= i << tp->startbit;
            ++it;
        } while ((*it) && (*it)->offset == offset);
        switch (base->type)
        {
            case BasicType::char_:
            case BasicType::unsigned_char_:
            case BasicType::signed_char_:
            case BasicType::char8_t_:
                Optimizer::genbyte(resolver);
                break;
            case BasicType::short_:
            case BasicType::unsigned_short_:
                Optimizer::genshort(resolver);
                break;
            case BasicType::int_:
            case BasicType::unsigned_:
            case BasicType::inative_:
            case BasicType::unative_:
                Optimizer::genint(resolver);
                break;
            case BasicType::char16_t_:
            case BasicType::wchar_t_:
                Optimizer::genuint16(resolver);
                break;
            case BasicType::char32_t_:
                Optimizer::genuint32(resolver);
                break;
            case BasicType::long_:
            case BasicType::unsigned_long_:
                Optimizer::genlong(resolver);
                break;
            case BasicType::long_long_:
            case BasicType::unsigned_long_long_:
                Optimizer::genlonglong(resolver);
                break;
            default:
                diag("dumpBits: unknown bit size");
                break;
        }
        if ((*it)->basetp->IsAtomic() && needsAtomicLockFromType((*it)->basetp))
            Optimizer::genstorage(ATOMIC_FLAG_SPACE);
        return base->size;
    }
    return 4;
}
void insert_file_constructor(SYMBOL* sym)
{
    if (sym->sb->attribs.uninheritable.constructorPriority || sym->sb->attribs.uninheritable.destructorPriority)
    {
        file_level_constructors.push_back(sym);
    }
} 
void insertDynamicInitializer(SYMBOL* sym, std::list<Initializer*>* init, bool front)
{
    if (!ignore_global_init && !definingTemplate)
    {
        if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
        {
            if (front)
                TLSInitializers.push_front(DynamicInitializer{ sym, init });
            else
                TLSInitializers.push_back(DynamicInitializer{ sym, init });

        }
        else
        {
            if (front)
                dynamicInitializers.push_front(DynamicInitializer{ sym, init });
            else
                dynamicInitializers.push_back(DynamicInitializer{ sym, init });
        }
    }
}
static void insertTLSInitializer(SYMBOL* sym, std::list<Initializer*>* init)
{
    TLSInitializers.push_front(DynamicInitializer{ sym, init });
}
void insertDynamicDestructor(SYMBOL* sym, std::list<Initializer*>* init)
{
    if (!ignore_global_init && !definingTemplate)
    {
        if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
        {
            TLSDestructors.push_front(DynamicInitializer{ sym, init });
        }
        else
        {
            dynamicDestructors.push_front(DynamicInitializer{ sym, init });
        }
    }
}
static void callDynamic(const char* name, int startupType, int index, std::list<Statement*>* st, bool virt = false)
{
    if (IsCompiler())
    {
        if (st->size())
        {
            Statement* stbegin = Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::dbgblock_);
            stbegin->label = 1;
            Statement* stend = Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::dbgblock_);
            stend->label = 0;
            st->push_front(stbegin);
            st->push_back(stend);
            char fullName[512];
            if (index != -1)
                Optimizer::my_sprintf(fullName, "%s_%d", name, index);
            else
                Utils::StrCpy(fullName, name);
            SYMBOL* funcsp;
            Type* tp = Type::MakeType(BasicType::ifunc_, Type::MakeType(BasicType::void_));
            tp->syms = symbols->CreateSymbolTable();
            if (index == -1)
                funcsp =
                    makeID((Optimizer::architecture == ARCHITECTURE_MSIL) ? StorageClass::global_ : StorageClass::static_, tp,
                                 nullptr, litlate(fullName));
            else
                funcsp = makeUniqueID((Optimizer::architecture == ARCHITECTURE_MSIL) ? StorageClass::global_ : StorageClass::static_, tp, nullptr, fullName);
            funcsp->sb->inlineFunc.stmt = stmtListFactory.CreateList();
            funcsp->sb->inlineFunc.stmt->push_back(Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::block_));
            funcsp->sb->inlineFunc.stmt->front()->lower = st;
            if (virt)
                funcsp->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
            tp->sp = funcsp;
            SetLinkerNames(funcsp, Linkage::none_);
            startlab = Optimizer::nextLabel++;
            retlab = Optimizer::nextLabel++;
            genfunc(funcsp, !(Optimizer::architecture == ARCHITECTURE_MSIL));
            startlab = retlab = 0;

            if (!(Optimizer::chosenAssembler->arch->denyopts & DO_NOADDRESSINIT))
            {
                SYMBOL* name = nullptr;
                if (startupType == STARTUP_TYPE_STARTUP)
                    Optimizer::startupseg();
                else
                    Optimizer::rundownseg();
                if (virt)
                {
                    Utils::StrCat(fullName, startupType == STARTUP_TYPE_STARTUP? "_startup" : "_rundown");
                    name = makeID(StorageClass::global_, &stdint, nullptr, litlate(fullName));
                    SetLinkerNames(name, Linkage::cpp_, false);
                    Optimizer::gen_virtual(Optimizer::SymbolManager::Get(name), startupType == STARTUP_TYPE_STARTUP?Optimizer::vt_startup : Optimizer::vt_rundown);
                }
                Optimizer::gensrref(Optimizer::SymbolManager::Get(funcsp), CPP_BASE_PRIO + preProcessor->GetCppPrio(), startupType);
                if (virt)
                {
                    Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(name));

                }
            }
        }
    }
}
static void dumpDynamicInitializers(void)
{
    if (IsCompiler() && dynamicInitializers.size())
    {
        StatementGenerator::AllocateLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
        int index = 0;
        int counter = 0;
        std::list<Statement*> st;
        codeLabel = INT_MIN;
        for (auto&& init : dynamicInitializers)
        {
            if (!init.sp->tp->IsStructured() || init.sp->tp->BaseType()->sp->sb->trivialCons ||
                !isintconst(init.init->front()->exp))
            {
                EXPRESSION *exp = nullptr, **next = &exp, *exp1;
                std::list<Statement*> stmt;
                int i = 0;
                exp = ConverInitializersToExpression(init.init ? init.init->front()->basetp
                                                                        : init.sp->tp,
                                              init.sp, nullptr, nullptr, init.init, nullptr, false);

                while (*next && (*next)->type == ExpressionNode::comma_)
                {
                    counter++;
                    if (++i == 10)
                    {
                        exp1 = *next;
                        *next = MakeIntExpression(ExpressionNode::c_i_, 0);  // fill in the final right with a value
                        stmt.push_back(Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::expr_));
                        stmt.back()->select = exp;

                        next = &exp1;
                        exp = exp1;
                        i = 0;
                    }
                    else
                    {
                        next = &(*next)->right;
                    }
                }
                if (exp)
                {
                    stmt.push_back(Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::expr_));
                    stmt.back()->select = exp;
                }
                if (stmt.size())
                {
                    st.insert(st.begin(), stmt.begin(), stmt.end());
                }
                if (++counter >= 1500)
                {
                    counter = 0;
                    callDynamic("__DYNAMIC_STARTUP__", STARTUP_TYPE_STARTUP, index++, &st);
                    st.clear();
                }
            }
        }
        callDynamic("__DYNAMIC_STARTUP__", STARTUP_TYPE_STARTUP, index++, &st);
        StatementGenerator::FreeLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
    }
}
static void dumpTLSInitializers(void)
{
    if (IsCompiler())
    {
        if (TLSInitializers.size())
        {
            StatementGenerator::AllocateLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
            std::list<Statement*> st;
            SYMBOL* funcsp;
            Type* tp = Type::MakeType(BasicType::ifunc_, Type::MakeType(BasicType::void_));
            tp->syms = symbols->CreateSymbolTable();
            funcsp = makeUniqueID((Optimizer::architecture == ARCHITECTURE_MSIL) ? StorageClass::global_ : StorageClass::static_, tp, nullptr,
                                  "__TLS_DYNAMIC_STARTUP__");
            tp->sp = funcsp;
            SetLinkerNames(funcsp, Linkage::none_);
            codeLabel = INT_MIN;
            for (auto&&init : TLSInitializers)
            {
                EXPRESSION* exp = MakeExpression(ExpressionNode::threadlocal_, init.sp);
                Statement* stmt = Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::expr_);
                exp = ConverInitializersToExpression(init.init->front()->basetp, init.sp, nullptr, nullptr,
                                              init.init, exp, false);
                optimize_for_constants(&exp);
                stmt->select = exp;
                st.push_back(stmt);
            }
            funcsp->sb->inlineFunc.stmt = stmtListFactory.CreateList();
            funcsp->sb->inlineFunc.stmt->push_front(Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::block_));
            funcsp->sb->inlineFunc.stmt->front()->lower = &st;
            startlab = Optimizer::nextLabel++;
            retlab = Optimizer::nextLabel++;
            genfunc(funcsp, true);
            startlab = retlab = 0;
            Optimizer::tlsstartupseg();
            Optimizer::gensrref(Optimizer::SymbolManager::Get(funcsp), 32, STARTUP_TYPE_TLS_STARTUP);
            StatementGenerator::FreeLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
        }
    }
}
static void dumpDynamicDestructors(void)
{
    if (IsCompiler() && dynamicDestructors.size())
    {
        StatementGenerator::AllocateLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
        int index = 0;
        int counter = 0;
        std::list<Statement*> st;
        codeLabel = INT_MIN;
        for (auto && dest : dynamicDestructors)
        {
            EXPRESSION* exp = ConverInitializersToExpression(dest.init->front()->basetp, dest.sp, nullptr, nullptr,
                                                      dest.init, nullptr, true);
            auto stmt = Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::expr_);
            optimize_for_constants(&exp);
            stmt->select = exp;
            st.push_back(stmt);
            if (++counter % 1500 == 0)
            {
                callDynamic("__DYNAMIC_RUNDOWN__", STARTUP_TYPE_RUNDOWN, index++, &st);
                st.clear();
            }
        }
        callDynamic("__DYNAMIC_RUNDOWN__", STARTUP_TYPE_RUNDOWN, index++, &st);
        StatementGenerator::FreeLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
    }
}
static void dumpTLSDestructors(void)
{
    if (IsCompiler())
    {
        if (TLSDestructors.size())
        {
            StatementGenerator::AllocateLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
            std::list<Statement*> st;
            SYMBOL* funcsp;
            Type* tp = Type::MakeType(BasicType::ifunc_, Type::MakeType(BasicType::void_));
            tp->syms = symbols->CreateSymbolTable();
            funcsp = makeUniqueID((Optimizer::architecture == ARCHITECTURE_MSIL) ? StorageClass::global_ : StorageClass::static_, tp, nullptr,
                                  "__TLS_DYNAMIC_RUNDOWN__");
            tp->sp = funcsp;
            SetLinkerNames(funcsp, Linkage::none_);
            codeLabel = INT_MIN;
            for (auto&& dest : TLSDestructors)
            {
                EXPRESSION* exp = MakeExpression(ExpressionNode::threadlocal_, dest.sp);
                exp = ConverInitializersToExpression(dest.init->front()->basetp, dest.sp, nullptr, nullptr,
                                              dest.init, exp, true);
                auto stmt = Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::expr_);
                optimize_for_constants(&exp);
                st.push_back(stmt);
            }

            funcsp->sb->inlineFunc.stmt = stmtListFactory.CreateList();
            funcsp->sb->inlineFunc.stmt->push_back(Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::block_));
            funcsp->sb->inlineFunc.stmt->front()->lower = &st;
            startlab = Optimizer::nextLabel++;
            retlab = Optimizer::nextLabel++;
            genfunc(funcsp, true);
            startlab = retlab = 0;
            Optimizer::tlsrundownseg();
            Optimizer::gensrref(Optimizer::SymbolManager::Get(funcsp), 32, STARTUP_TYPE_TLS_RUNDOWN);
            StatementGenerator::FreeLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
        }
    }
}
int dumpMemberPtr(SYMBOL* sym, Type* membertp, bool make_label)
{
    int lbl = -1;
    if (IsCompiler())
    {
        int vbase = 0, ofs;
        EXPRESSION expx, *exp = &expx;
        if (make_label)
        {
            // well if we wanted we could reuse existing structures, but,
            // it doesn't seem like the amount of duplicates there might be is
            // really worth the work.  Borland didn't think so anyway...
            lbl = Optimizer::nextLabel++;
            if (sym)
                sym->sb->label = lbl;
        }
        if (!sym)
        {
            // null...
            if (membertp->btp->IsFunction())
            {
                InsertMemberPointer(lbl, (Optimizer::SimpleSymbol*)-1, 0, 0);
            }
            else
            {
                InsertMemberPointer(lbl, nullptr, 0, 0);
            }
        }
        else
        {
            if (sym->sb->storage_class != StorageClass::member_ && sym->sb->storage_class != StorageClass::mutable_ && sym->sb->storage_class != StorageClass::virtual_)
                errorConversionOrCast(true, sym->tp, membertp);
            memset(&expx, 0, sizeof(expx));
            expx.type = ExpressionNode::c_i_;
            exp = baseClassOffset(sym->sb->parentClass, membertp->BaseType()->sp, &expx);
            optimize_for_constants(&exp);
            if (sym->tp->IsFunction())
            {
                Optimizer::SimpleSymbol* genned;
                SYMBOL* genned1;
                if (sym->sb->storage_class == StorageClass::virtual_)
                    genned = Optimizer::SymbolManager::Get(genned1 = getvc1Thunk(sym->sb->vtaboffset));
                else
                    genned = Optimizer::SymbolManager::Get(genned1 = sym);

                int offset1 = 0;
                int offset2 = 0;
                if (exp->type == ExpressionNode::add_)
                {
                    if (exp->left->type == ExpressionNode::l_p_)
                    {
                        offset1 = exp->right->v.i + 1;
                        offset2 = exp->left->left->v.i + 1;
                    }
                    else
                    {
                        offset1 = exp->left->v.i;
                        offset2 = exp->right->left->v.i + 1;
                    }
                }
                else if (exp->type == ExpressionNode::l_p_)
                {
                    offset1 = 0;
                    offset2 = exp->left->v.i + 1;
                }
                else
                {
                    offset1 = exp->v.i;
                    offset2 = 0;
                }
                InsertMemberPointer(lbl, genned, offset1, offset2);
                InsertInline(genned1);
            }
            else
            {
                int offset1 = 0;
                int offset2 = 0;
                if (exp->type == ExpressionNode::add_)
                {
                    if (exp->left->type == ExpressionNode::l_p_)
                    {
                        offset1 = exp->right->v.i + sym->sb->offset + 1;
                        offset2 = exp->left->left->v.i + 1;
                    }
                    else
                    {
                        offset1 = exp->left->v.i + sym->sb->offset + 1;
                        offset2 = exp->right->left->v.i + 1;
                    }
                }
                else if (exp->type == ExpressionNode::l_p_)
                {
                    offset1 = 1 + sym->sb->offset;
                    offset2 = exp->left->v.i + 1;
                }
                else
                {
                    offset1 = exp->v.i + sym->sb->offset + 1;
                    offset2 = 0;
                }
                InsertMemberPointer(lbl, nullptr, offset1, offset2);
            }
        }
    }
    return lbl;
}
static void GetStructData(EXPRESSION* in, EXPRESSION** exp, int* ofs)
{
    switch (in->type)
    {
        case ExpressionNode::add_:
        case ExpressionNode::arrayadd_:
        case ExpressionNode::structadd_:
            GetStructData(in->left, exp, ofs);
            GetStructData(in->right, exp, ofs);
            break;
        default:
            if (isintconst(in))
            {
                *ofs += in->v.i;
            }
            else
            {
                while (IsCastValue(in))
                    in = in->left;
                if (*exp)
                    diag("GetStructData - multiple sym");
                *exp = in;
            }
            break;
    }
}
void CreateInlineConstructor(SYMBOL* sym) 
{
    if (IsCompiler())
    {
        StatementGenerator::AllocateLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
        EXPRESSION *exp = nullptr;
        std::list<Statement*> st;
        exp = ConverInitializersToExpression(sym->sb->init->front()->basetp,
                                        sym, nullptr, nullptr, sym->sb->init, nullptr, false);
        st.push_back(Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::expr_));
        optimize_for_constants(&exp);
        st.back()->select = exp;
        char buf[4000];
        const char* q = sym->sb->decoratedName + strlen(sym->sb->decoratedName);
        while (isdigit(q[-1])) q--;
        sprintf(buf, "initializer@%s_%s", sym->name, q);
        callDynamic(buf, STARTUP_TYPE_STARTUP, -1, &st, true);
        StatementGenerator::FreeLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
    }
}
void CreateInlineDestructor(SYMBOL* sym)
{
    if (IsCompiler())
    {
        StatementGenerator::AllocateLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
        std::list<Statement*> st;
        EXPRESSION* exp = ConverInitializersToExpression(sym->sb->dest->front()->basetp, sym, nullptr,
                                                    nullptr, sym->sb->dest, nullptr, true);
        auto stmt = Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::expr_);
        optimize_for_constants(&exp);
        stmt->select = exp;
        st.push_back(stmt);
        char buf[4000];
        const char* q = sym->sb->decoratedName + strlen(sym->sb->decoratedName);
        while (isdigit(q[-1])) q--;
        sprintf(buf, "destructor@%s_%s", sym->name, sym->sb->decoratedName);
        callDynamic(buf, STARTUP_TYPE_RUNDOWN, -1, &st, true);
        StatementGenerator::FreeLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
    }
}
int dumpInit(SYMBOL* sym, Initializer* init)
{
    if (IsCompiler())
    {
        Type* tp = init->basetp->BaseType();
        int rv;
        long long i;
        FPF f, im;
        if (tp->type == BasicType::templateparam_)
            tp = tp->templateParam->second->byClass.val;
        if (tp->IsStructured())
        {
            rv = tp->size;  // +tp->sp->sb->attribs.inheritable.structAlign;
        }
        else
        {
            rv = getSize(tp->type);
        }
        if (isfloatconst(init->exp))
        {
            f = *init->exp->v.f;
            i = (long long)f;
            im.SetZero(0);
        }
        else if (isintconst(init->exp))
        {
            i = init->exp->v.i;
            f = (long long)i;
            im.SetZero(0);
        }
        else if (isimaginaryconst(init->exp))
        {
            i = 0;
            f.SetZero(0);
            im = *init->exp->v.f;
        }
        else if (iscomplexconst(init->exp))
        {
            f = init->exp->v.c->r;
            im = init->exp->v.c->i;
            i = (long long)(f);
        }
        else if (init->exp->type != ExpressionNode::c_bitint_ && init->exp->type != ExpressionNode::c_ubitint_)
        {
            EXPRESSION* exp = init->exp;
            while (IsCastValue(exp))
                exp = exp->left;
            while (exp->type == ExpressionNode::comma_ && exp->right)
                exp = exp->right;
            if (exp->type == ExpressionNode::callsite_ && !exp->v.func->ascall)
                exp = exp->v.func->fcall;
            int offset = 0;
            auto exp2 = relptr(exp, offset);
            if (Optimizer::cparams.prm_cplusplus && !IsConstantExpression(exp, false, false) && (!exp2 || exp2->type != ExpressionNode::global_))
            {
                if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
                {
                    std::list<Initializer*>* temp = initListFactory.CreateList();
                    temp->push_back(init);
                    insertTLSInitializer(sym, temp);
                }
                else if (sym->sb->storage_class != StorageClass::localstatic_ && !sym->sb->attribs.inheritable.isInline)
                {
                    std::list<Initializer*>* temp = initListFactory.CreateList();
                    temp->push_back(init);
                    insertDynamicInitializer(sym, temp, false);
                }
                return 0;
            }
            else if (Optimizer::chosenAssembler->arch->denyopts & DO_NOADDRESSINIT)
            {
                switch (exp->type)
                {
                    case ExpressionNode::memberptr_:
                        dumpMemberPtr(nullptr, tp, false);
                        // fall through
                    case ExpressionNode::pc_:
                    case ExpressionNode::global_:
                    case ExpressionNode::labcon_:
                    case ExpressionNode::add_:
                    case ExpressionNode::arrayadd_:
                    case ExpressionNode::structadd_:
                        if (exp->type != ExpressionNode::memberptr_)
                            Optimizer::genaddress(0);
                        if (!Optimizer::cparams.prm_cplusplus || sym->sb->storage_class != StorageClass::localstatic_)
                        {
                            std::list<Initializer*>* temp = initListFactory.CreateList();
                            temp->push_back(init);
                            insertDynamicInitializer(sym, temp);
                        }
                        break;
                        /* fall through */
                    default:
                        if (isintconst(exp))
                        {
                            Optimizer::genint(exp->v.i);
                        }
                        else if (Optimizer::cparams.prm_cplusplus)
                        {
                            if (sym->sb->storage_class != StorageClass::localstatic_)
                            {
                                std::list<Initializer*>* temp = initListFactory.CreateList();
                                temp->push_back(init);
                                insertDynamicInitializer(sym, temp);
                            }
                            return 0;
                        }
                        else
                        {
                            diag("unknown pointer type in initSym");
                            Optimizer::genaddress(0);
                        }
                        break;
                }
                return rv;
            }
            else
            {
                switch (exp->type)
                {
                    case ExpressionNode::pc_:
                        Optimizer::genpcref(Optimizer::SymbolManager::Get(exp->v.sp), 0);
                        break;
                    case ExpressionNode::global_:
                        Optimizer::genref(Optimizer::SymbolManager::Get(exp->v.sp), 0);
                        break;
                    case ExpressionNode::labcon_:
                        Optimizer::gen_labref(exp->v.i);
                        break;
                    case ExpressionNode::memberptr_:
                        dumpMemberPtr(exp->v.sp, tp, false);
                        break;
                    case ExpressionNode::add_:
                    case ExpressionNode::arrayadd_:
                    case ExpressionNode::structadd_: {
                        EXPRESSION* ep1 = nullptr;
                        int offs = 0;
                        GetStructData(exp, &ep1, &offs);
                        if (ep1)
                        {
                            if (ep1->type == ExpressionNode::pc_)
                            {
                                Optimizer::genpcref(Optimizer::SymbolManager::Get(ep1->v.sp), offs);
                            }
                            else if (ep1->type == ExpressionNode::global_)
                            {
                                Optimizer::genref(Optimizer::SymbolManager::Get(ep1->v.sp), offs);
                                break;
                            }
                        }
                    }
                    /* fall through */
                    default:
                        if (isintconst(exp))
                        {
                            Optimizer::genint(exp->v.i);
                        }
                        else if (Optimizer::cparams.prm_cplusplus)
                        {
                            if (sym->sb->storage_class != StorageClass::localstatic_)
                            {
                                std::list<Initializer*>* temp = initListFactory.CreateList();
                                temp->push_back(init);
                                insertDynamicInitializer(sym, temp);
                            }
                            return 0;
                        }
                        else
                        {
                            diag("unknown pointer type in initSym");
                            Optimizer::genaddress(0);
                        }
                        break;
                }
            }
            /* we are reserving enough space for the entire pointer
             * even if the type has less space.  On an x86 the resulting linker
             * conversion will work, except for example in the case of
             * arrays of characters initialized to pointers.  On a big endian processor
             * the conversion will not leave the expected results...
             */
            if (rv < getSize(BasicType::pointer_))
                return getSize(BasicType::pointer_);
            else
                return rv;
        }

        switch (tp->type == BasicType::enum_ ? tp->btp->type : tp->type)
        {
            case BasicType::bool_:
                Optimizer::genbool(i);
                break;
            case BasicType::char_:
            case BasicType::unsigned_char_:
            case BasicType::signed_char_:
            case BasicType::char8_t_:
                Optimizer::genbyte(i);
                break;
            case BasicType::short_:
            case BasicType::unsigned_short_:
                Optimizer::genshort(i);
                break;
            case BasicType::wchar_t_:
                Optimizer::genwchar_t(i);
                break;
            case BasicType::int_:
            case BasicType::unsigned_:
            case BasicType::inative_:
            case BasicType::unative_:
                Optimizer::genint(i);
                break;
            case BasicType::char16_t_:
                Optimizer::genuint16(i);
                break;
            case BasicType::char32_t_:
                Optimizer::genuint32(i);
                break;
            case BasicType::long_:
            case BasicType::unsigned_long_:
                Optimizer::genlong(i);
                break;
            case BasicType::long_long_:
            case BasicType::unsigned_long_long_:
                Optimizer::genlonglong(i);
                break;
            case BasicType::float_:
                Optimizer::genfloat(&f);
                break;
            case BasicType::double_:
                Optimizer::gendouble(&f);
                break;
            case BasicType::long_double_:
                Optimizer::genlongdouble(&f);
                break;
            case BasicType::float__imaginary_:
                Optimizer::genfloat(&im);
                break;
            case BasicType::double__imaginary_:
                Optimizer::gendouble(&im);
                break;
            case BasicType::long_double_imaginary_:
                Optimizer::genlongdouble(&im);
                break;
            case BasicType::float__complex_:
                Optimizer::genfloat(&f);
                Optimizer::genfloat(&im);
                break;
            case BasicType::double__complex_:
                Optimizer::gendouble(&f);
                Optimizer::gendouble(&im);
                break;
            case BasicType::long_double_complex_:
                Optimizer::genlongdouble(&f);
                Optimizer::genlongdouble(&im);
                break;

            case BasicType::pointer_:
                Optimizer::genaddress(i);
                break;
            case BasicType::bitint_:
            case BasicType::unsigned_bitint_: {
                // there are a couple of problems with this, first there is the problem if we change the underlying type
                // second some architectures might need this to be big endian
                // fixme: endianness
                assert(Optimizer::chosenAssembler->arch->bitintunderlying == 32);
                int words = (tp->bitintbits + Optimizer::chosenAssembler->arch->bitintunderlying - 1);
                words /= Optimizer::chosenAssembler->arch->bitintunderlying;
                words *= Optimizer::chosenAssembler->arch->bitintunderlying;
                words /= CHAR_BIT;
                int max = 0;
                unsigned char* src;
                if (init->exp->type == ExpressionNode::c_bitint_ || init->exp->type == ExpressionNode::c_ubitint_)
                {
                    max = (init->exp->v.b.bits + Optimizer::chosenAssembler->arch->bitintunderlying - 1);
                    max /= Optimizer::chosenAssembler->arch->bitintunderlying;
                    max *= Optimizer::chosenAssembler->arch->bitintunderlying;
                    max /= CHAR_BIT;
                    src = init->exp->v.b.value;
                }
                else
                {
                    max = sizeof(long long);
                    src = (unsigned char *) & i;
                }
                for (i = 0; i < words && i < max; i++)
                {
                    Optimizer::genbyte(*src++);
                }
                if (i < words)
                {
                    auto fill = tp->type == BasicType::bitint_ && (src[-1] & 0x80000000) ? 0xff : 0;
                    for (; i < words; i++)
                    {
                        Optimizer::genbyte(fill);
                    }
                }
                rv = words;
                break;
            }
            case BasicType::far_:
            case BasicType::seg_:
            case BasicType::bit_:
            default:
                diag("dumpInit: unknown type");
                break;
        }
        return rv;
    }
    return 4;
}
void dumpInitGroup(SYMBOL* sym, Type* tp)
{
    if (IsCompiler())
    {

        if (sym->sb->init || (sym->tp->IsArray() && sym->tp->msil))
        {
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
            {
                insertDynamicInitializer(sym, sym->sb->init);
            }
            else
            {
                if (sym->tp->BaseType()->array || tp->IsStructured())
                {
                    auto it = sym->sb->init->begin();
                    auto ite = sym->sb->init->end();
                    int pos = 0;
                    while (it != ite)
                    {
                        if (pos != (*it)->offset)
                        {
                            if (pos > (*it)->offset)
                                diag("position error in dumpInitializers");
                            else
                                Optimizer::genstorage((*it)->offset - pos);
                            pos = (*it)->offset;
                        }
                        if ((*it)->basetp && (*it)->basetp->BaseType()->hasbits)
                        {
                            pos += dumpBits(it);
                        }
                        else
                        {
                            if ((*it)->basetp && (*it)->exp)
                            {
                                int s;
                                s = dumpInit(sym, *it);
                                if (s < (*it)->basetp->size)
                                {

                                    Optimizer::genstorage((*it)->basetp->size - s);
                                    s = (*it)->basetp->size;
                                }
                                pos += s;
                            }
                            ++it;
                        }
                    }
                }
                else
                {
                    int s = dumpInit(sym, sym->sb->init->front());
                    if (s < sym->sb->init->front()->basetp->size)
                    {

                        Optimizer::genstorage(sym->sb->init->front()->basetp->size - s);
                    }
                }
            }
        }
        else
            Optimizer::genstorage(tp->BaseType()->size);
        if (tp->IsAtomic() && needsAtomicLockFromType(tp))
        {
            Optimizer::genstorage(ATOMIC_FLAG_SPACE);
        }
    }
}
static void dumpStaticInitializers(void)
{
    if (IsCompiler())
    {
        int abss = 0;
        int adata = 0;
        int aconst = 0;
        int anull = 0;
        int sconst = 0;
        int bss = 0;
        int data = 0;
        int thread = 0;
        int *sizep, *alignp;
        symListTail = symListHead;
        while (symListTail)
        {
            SYMBOL* sym = (SYMBOL*)symListTail->data;
            if (sym->sb->storage_class == StorageClass::global_ || sym->sb->storage_class == StorageClass::static_ ||
                sym->sb->storage_class == StorageClass::localstatic_ || sym->sb->storage_class == StorageClass::constant_)
            {
                Type* tp = sym->tp;
                Type* stp = tp;
                int al;
                while (stp->IsArray())
                    stp = stp->BaseType()->btp;
                if ((sym->tp->IsConstWithArr() && !sym->tp->IsVolatile()) || sym->sb->storage_class == StorageClass::constant_)
                {
                    Optimizer::xconstseg();
                    sizep = &sconst;
                    alignp = &aconst;
                }
                else if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
                {
                    Optimizer::tseg();
                    sizep = &thread;
                    alignp = &anull;
                }
                else if (sym->sb->init || !Optimizer::cparams.prm_bss)
                {
                    Optimizer::dseg();
                    sizep = &data;
                    alignp = &adata;
                }
                else
                {
                    Optimizer::bssseg();
                    sizep = &bss;
                    alignp = &abss;
                }
                if (sym->sb->attribs.inheritable.structAlign)
                    al = sym->sb->attribs.inheritable.structAlign;
                else
                    al = getAlign(StorageClass::global_, tp->BaseType());

                if (*alignp < al)
                    *alignp = al;
                if (*sizep % al)
                {
                    int n = al - *sizep % al;
                    if (n != al)
                    {
                        if (!(Optimizer::architecture == ARCHITECTURE_MSIL))
                        {
                            Optimizer::genstorage(n);
                        }
                        *sizep += n;
                    }
                }
                //  have to thunk in a size for __arrCall
                if (Optimizer::cparams.prm_cplusplus)
                {
                    if (tp->IsArray())
                    {
                        Type* tp1 = tp;
                        while (tp1->IsArray())
                            tp1 = tp1->BaseType()->btp;
                        tp1 = tp1->BaseType();
                        if (tp1->IsStructured() && !tp1->sp->sb->trivialCons)
                        {
                            Optimizer::genint(tp->BaseType()->size);
                            *sizep += getSize(BasicType::int_);
                        }
                    }
                }
                sym->sb->offset = *sizep;
                *sizep += tp->BaseType()->size;
                if (sym->sb->anonymousGlobalUnion)
                {
                    Optimizer::put_label(sym->sb->label);
                }
                else
                {
                    Optimizer::gen_strlab(Optimizer::SymbolManager::Get(sym));
                }
                dumpInitGroup(sym, tp);
            }
            symListTail = symListTail->next;
        }
        symListHead = nullptr;
        Optimizer::dataAlign = adata;
        Optimizer::bssAlign = abss;
        Optimizer::constAlign = aconst;
    }
}
void dumpInitializers(void)
{
    if (IsCompiler())
    {
        if (!TotalErrors())
        {
            dumpStaticInitializers();
            dumpDynamicInitializers();
            dumpTLSInitializers();
            dumpDynamicDestructors();
            dumpTLSDestructors();
            dumpvc1Thunks();
            dumpMemberPointers();
            dumpStaticInitializers();
        }
    }
}
void insertInitSym(SYMBOL* sym)
{
    if (!sym->sb->indecltable)
    {
        Optimizer::LIST* lst = Allocate<Optimizer::LIST>();
        lst->data = sym;
        if (symListHead)
            symListTail = symListTail->next = lst;
        else
            symListHead = symListTail = lst;
        sym->sb->indecltable = true;
    }
}
Initializer* InsertInitializer(std::list<Initializer*>** pos, std::list<Initializer*>::iterator it, Type* tp, EXPRESSION* exp, int offset, bool noassign)
{
    if (!*pos)
        *pos = initListFactory.CreateList();
    Initializer* pos1 = Allocate<Initializer>();

    pos1->basetp = pos1->realtp = tp;
    pos1->exp = exp;
    pos1->offset = offset;
    pos1->tag = inittag++;
    pos1->noassign = noassign;
    (*pos)->insert(it, pos1);
    return pos1;
}
Initializer* InsertInitializer(std::list<Initializer*>** pos, Type* tp, EXPRESSION* exp, int offset, bool noassign)
{
    if (!*pos)
        *pos = initListFactory.CreateList();
    return InsertInitializer(pos, (**pos).end(), tp, exp, offset, noassign);
}
static LexList* init_expression(LexList* lex, SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** expr, Type* itype, bool commaallowed,
    std::function<EXPRESSION* (EXPRESSION*, Type*)> modify, bool arrayElem, SYMBOL* sym)
{
    LexList* start = lex;
    int noeval = 0;
    if (Optimizer::cparams.prm_cplusplus && definingTemplate && !instantiatingTemplate && sym && sym->sb->templateLevel)
    {
        noeval = _F_NOEVAL;
    }
    else if (Optimizer::cparams.prm_cplusplus && arrayElem)
    {
        auto start = lex;
        int pa = 0, lt = 0, beg = 0; 
        do
        {
            if (MATCHKW(lex, Keyword::openpa_))
                pa++;
            if (MATCHKW(lex, Keyword::lt_))
                lt++;
            if (MATCHKW(lex, Keyword::begin_))
                beg++;
            if (MATCHKW(lex, Keyword::closepa_))
                if (pa)
                    pa--;
                else
                    break;
            if (MATCHKW(lex, Keyword::gt_))
                if (lt )
                    lt--;
                else
                    break;
            if (MATCHKW(lex, Keyword::end_))
                if (beg)
                    beg--;
                else
                    break;
            if (!pa && !lt && !beg)
            {
                if (MATCHKW(lex, Keyword::ellipse_) || MATCHKW(lex, Keyword::comma_))
                    break;
            }
            if (MATCHKW(lex, Keyword::semicolon_))
                break;
            lex = getsym();
        } while (lex);
        if (lex && MATCHKW(lex, Keyword::ellipse_))
            noeval = _F_NOEVAL;
        lex = prevsym(start);
    }
    EnterPackedSequence();
    if (commaallowed)
        lex = expression(lex, funcsp, atp, tp, expr, noeval);
    else
        lex = expression_no_comma(lex, funcsp, atp, tp, expr, nullptr, noeval);
    *expr = modify(*expr, *tp);
    if (*tp && ((*tp)->IsVoid() || (*tp)->IsMsil()))
        error(ERR_NOT_AN_ALLOWED_TYPE);
    if (Optimizer::architecture == ARCHITECTURE_MSIL)
        RemoveSizeofOperators(*expr);
    if (!noeval)
        optimize_for_constants(expr);
    if (*tp)
    {
        if (*expr && (*expr)->type == ExpressionNode::callsite_ && (*expr)->v.func->sp->sb->parentClass && !(*expr)->v.func->ascall &&
            !(*expr)->v.func->asaddress)
        {
            for (auto sym : *(*expr)->v.func->functp->BaseType()->syms)
            {
                if (sym->sb->storage_class == StorageClass::member_ || sym->sb->storage_class == StorageClass::mutable_)
                {
                    error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
                    break;
                }
            }
        }
        if (MATCHKW(lex, Keyword::ellipse_))
        {
            // lose p
            lex = getsym();
            if (*expr && (*expr)->type != ExpressionNode::packedempty_)
            {
                std::list<Argument*>* temp = nullptr;
                checkPackedExpression(*expr);
                // this is going to presume that the expression involved
                // is not too long to be cached by the LexList mechanism.
                expandPackedInitList(&temp, funcsp, start, *expr);
                if (!temp || !temp->size())
                {
                    *expr = MakeIntExpression(ExpressionNode::c_i_, 0);
                    *tp = itype;
                }
                else
                {
                    *tp = temp->front()->tp;
                    auto** last = expr;
                    *expr = nullptr;
                    for (auto t : *temp)
                    {
                        if (!*last)
                        {
                            *last = t->exp;
                        }
                        else
                        {
                            *last = MakeExpression(ExpressionNode::comma_, *last, t->exp);
                            last = &(*last)->right;
                        }
                    }
                    if (*last != *expr)
                    {
                        (*expr)->packedArray = true;
                    }
                }
            }
            ClearPackedSequence();
        }
    }
    LeavePackedSequence();
    return lex;
}
static LexList* init_expression(LexList* lex, SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** expr, Type* itype, bool commaallowed, bool arrayElem, SYMBOL* sym)
{
    return init_expression(lex, funcsp, atp, tp, expr, itype, commaallowed, [](EXPRESSION* exp, Type* tp) { return exp; }, arrayElem, sym);
}
static LexList* initialize_bool_type(LexList* lex, SYMBOL* funcsp, int offset, StorageClass sc, Type* itype, std::list<Initializer*>** init, bool arrayElem, SYMBOL* sym)
{
    Type* tp;
    EXPRESSION* exp;
    bool needend = false;
    if (MATCHKW(lex, Keyword::begin_))
    {
        needend = true;
        lex = getsym();
    }
    if (Optimizer::cparams.prm_cplusplus && needend && MATCHKW(lex, Keyword::end_))
    {
        exp = MakeIntExpression(ExpressionNode::c_bool_, 0);
    }
    else
    {
        lex = init_expression(lex, funcsp, nullptr, &tp, &exp, itype, false, arrayElem && !needend, sym);
        if (!tp)
        {
            error(ERR_EXPRESSION_SYNTAX);
        }
        else if (itype->type != BasicType::templateparam_ && !definingTemplate)
        {
            ResolveTemplateVariable(&tp, &exp, itype, nullptr);
            castToArithmetic(false, &tp, &exp, (Keyword) - 1, itype, true);
            if (tp->IsStructured())
                error(ERR_ILL_STRUCTURE_ASSIGNMENT);
            else if ((!tp->IsArithmetic() && tp->BaseType()->type != BasicType::enum_ && tp->BaseType()->type != BasicType::pointer_) ||
                     (sc != StorageClass::auto_ && sc != StorageClass::register_ && !isarithmeticconst(exp) && !msilConstant(exp) &&
                      !Optimizer::cparams.prm_cplusplus))
                error(ERR_CONSTANT_VALUE_EXPECTED);

            if (!itype->CompatibleType(tp))
            {
                cast(itype, &exp);
                optimize_for_constants(&exp);
            }
        }
    }
    InsertInitializer(init, itype, exp, offset, false);
    if (needend)
    {
        if (!needkw(&lex, Keyword::end_))
        {
            errskim(&lex, skim_end);
            skip(&lex, Keyword::end_);
        }
    }
    return lex;
}
void CheckNarrowing(Type* dest, Type* source, EXPRESSION* exp)
{
    
    if (Optimizer::cparams.prm_cplusplus && dest->IsArithmetic() && source->IsArithmetic() && dest->BaseType()->type != source->BaseType()->type)
    {
        bool err = false;
        if (isintconst(exp) || isfloatconst(exp))
        {
            long long val;
            if (isfloatconst(exp))
            {
                if (dest->IsInt())
                {
                    val = (long long)exp->v.f;
                    if (dest->IsUnsigned())
                    {
                        unsigned long long val2 = (unsigned long long)val;
                        err = val2 < 0 || val2 > ULLONG_MAX;
                    }
                    else
                    {
                        err = val < LLONG_MIN || val > LLONG_MAX;
                    }

                }
                else
                {
                    double aa;
                    exp->v.f->ToDouble((uchar*)&aa);
                    val = aa;
                    switch (dest->type)
                    {
                    case BasicType::float_:
                        err = aa < FLT_MIN || aa > FLT_MAX;
                        break;
                    case BasicType::double_:
                        err = aa < DBL_MIN || aa > DBL_MAX;
                        break;
                    default:
                        if (dest->IsUnsigned())
                        {
                            err = aa < 0 || aa > ULLONG_MAX;
                        }
                        else
                        {
                            err = aa < LLONG_MIN || aa > LLONG_MAX;
                        }
                        break;
                    }
                }
            }
            else
            {
                val = exp->v.i;
            }
            if (dest->IsInt())
            {

                switch (dest->BaseType()->type)
                {
                case BasicType::char_:
                    err = val < CHAR_MIN || val > CHAR_MAX;
                    break;
                case BasicType::unsigned_char_:
                    err = val < 0 || val > UCHAR_MAX;
                    break;
                case BasicType::short_:
                    err = val < SHRT_MIN || val > SHRT_MAX;
                    break;
                case BasicType::unsigned_short_:
                    err = val < 0 || val > USHRT_MAX;
                    break;
                case BasicType::int_:
                case BasicType::long_:
                    err = val < INT_MIN || val > INT_MAX;
                    break;
                case BasicType::unsigned_:
                case BasicType::unsigned_long_:
                    err = val < 0 || val > UINT_MAX;
                    break;
                case BasicType::long_long_:
                    err = val < LLONG_MIN || val > LLONG_MAX;
                    break;
                case BasicType::unsigned_long_long_:
                    err = val < 0 || val > ULLONG_MAX;
                    break;

                }
            }
        }
        else if (dest->IsFloat())
        {
            err = source->IsInt() || source->type > dest->type;
        }
        else
        {
            err = source->IsFloat() || source->type > dest->type || source->IsUnsigned() != dest->IsUnsigned();
        }
        if (err)
            errortype(ERR_INIT_NARROWING, source, dest);
    }
}
static LexList* initialize_arithmetic_type(LexList* lex, SYMBOL* funcsp, int offset, StorageClass sc, Type* itype, std::list<Initializer*>** init,
    bool arrayElem, SYMBOL* sym)
{

    Type* tp = nullptr;
    EXPRESSION* exp = nullptr;
    bool needend = false;
    if (MATCHKW(lex, Keyword::begin_))
    {
        needend = true;
        lex = getsym();
    }
    if (Optimizer::cparams.prm_cplusplus && needend && MATCHKW(lex, Keyword::end_))
    {
        exp = MakeIntExpression(ExpressionNode::c_i_, 0);
        cast(itype, &exp);
    }
    else
    {
        lex = init_expression(lex, funcsp, nullptr, &tp, &exp, itype, false, arrayElem && !needend, sym);
        if (!tp || !exp)
        {
            error(ERR_EXPRESSION_SYNTAX);
        }
        else
        {
            ResolveTemplateVariable(&tp, &exp, itype, nullptr);
            if (itype->type != BasicType::templateparam_ && tp->type != BasicType::templateselector_ && !definingTemplate)
            {
                EXPRESSION** exp2;
                exp2 = &exp;
                while (IsCastValue(*exp2))
                    exp2 = &(*exp2)->left;
                if ((*exp2)->type == ExpressionNode::callsite_ && (*exp2)->v.func->sp->sb->storage_class == StorageClass::overloads_ &&
                    (*exp2)->v.func->sp->tp->syms->size() > 0)
                {
                    SYMBOL* sp2;
                    Type* tp1 = nullptr;
                    sp2 = MatchOverloadedFunction((*exp2)->v.func->sp->tp, &tp1, (*exp2)->v.func->sp, exp2, 0);
                }
                if (Optimizer::cparams.prm_cplusplus && (itype->IsArithmetic() || itype->BaseType()->type == BasicType::enum_) &&
                    tp->IsStructured())
                {
                    castToArithmetic(false, &tp, &exp, (Keyword) - 1, itype, true);
                }
                if (tp->IsFunction())
                    tp = tp->BaseType()->btp;
                if (tp->IsStructured())
                {
                    error(ERR_ILL_STRUCTURE_ASSIGNMENT);
                }
                else if (tp->IsPtr())
                    error(ERR_NONPORTABLE_POINTER_CONVERSION);
                else if ((!tp->IsArithmetic() && tp->BaseType()->type != BasicType::enum_) ||
                         (sc != StorageClass::auto_ && sc != StorageClass::register_ && !isarithmeticconst(exp) && !msilConstant(exp) &&
                          !Optimizer::cparams.prm_cplusplus))
                    if (!tp->IsBitInt() || (exp->type != ExpressionNode::c_bitint_ && exp->type != ExpressionNode::c_ubitint_))
                        error(ERR_CONSTANT_VALUE_EXPECTED);
                    else
                        checkscope(tp, itype);
                else
                    checkscope(tp, itype);
                if (tp->BaseType()->type != itype->BaseType()->type);
                {
                    if (needend)
                        CheckNarrowing(itype, tp, exp);
                    cast(itype, &exp);
                    optimize_for_constants(&exp);
                }
            }
        }
    }
    InsertInitializer(init, itype, exp, offset, false);
    (*init)->back()->realtp = tp;
    if (needend)
    {
        if (!needkw(&lex, Keyword::end_))
        {
            errskim(&lex, skim_end);
            skip(&lex, Keyword::closebr_);
        }
    }
    return lex;
}
static LexList* initialize_string(LexList* lex, SYMBOL* funcsp, Type** rtype, EXPRESSION** exp)
{
    LexType tp;
    (void)funcsp;
    lex = concatStrings(lex, exp, &tp, 0);
    switch (tp)
    {
        default:
        case LexType::l_u8str_:
            *rtype = &stdchar8_tptr;
            break;
        case LexType::l_astr_:
            *rtype = &stdstring;
            break;
        case LexType::l_wstr_:
            *rtype = &stdwcharptr;
            break;
        case LexType::l_msilstr_:
            *rtype = &std__string;
        case LexType::l_ustr_:
            *rtype = &stdchar16tptr;
            break;
        case LexType::l_Ustr_:
            *rtype = &stdchar32tptr;
            break;
    }
    return lex;
}
static LexList* initialize_pointer_type(LexList* lex, SYMBOL* funcsp, int offset, StorageClass sc, Type* itype, std::list<Initializer*>** init, bool arrayElem, SYMBOL* sym)
{
    Type* tp = nullptr;
    EXPRESSION* exp = nullptr;
    bool string = false;
    bool needend = false;
    if (MATCHKW(lex, Keyword::begin_))
    {
        needend = true;
        lex = getsym();
    }
    if (Optimizer::cparams.prm_cplusplus && needend && MATCHKW(lex, Keyword::end_))
    {
        exp = MakeIntExpression(ExpressionNode::c_i_, 0);
    }
    else
    {
        bool hasOpenPa = !needend && lex && MATCHKW(lex, Keyword::openpa_);
        if (!lex || (lex->data->type != LexType::l_astr_ && lex->data->type != LexType::l_wstr_ && lex->data->type != LexType::l_ustr_ &&
                     lex->data->type != LexType::l_Ustr_ && lex->data->type != LexType::l_msilstr_ && lex->data->type != LexType::l_u8str_))
        {
            lex = init_expression(lex, funcsp, itype, &tp, &exp, itype, false, arrayElem && !needend, sym);
            if (!tp)
            {
                error(ERR_EXPRESSION_SYNTAX);
                tp = &stdint;
            }
            else
            {
                ResolveTemplateVariable(&tp, &exp, itype, nullptr);
            }
        }
        else
        {
            lex = initialize_string(lex, funcsp, &tp, &exp);
            string = true;
        }
        castToPointer(&tp, &exp, (Keyword) - 1, itype);
        DeduceAuto(&itype, tp, exp, hasOpenPa);
        if (sc != StorageClass::auto_ && sc != StorageClass::register_)
        {
            EXPRESSION* exp2 = exp;
            while (exp2->type == ExpressionNode::comma_ && exp2->right)
                exp2 = exp2->right;
            if (!isarithmeticconst(exp2) && !isconstaddress(exp2) && !msilConstant(exp2) && !Optimizer::cparams.prm_cplusplus)
                error(ERR_NEED_CONSTANT_OR_ADDRESS);
        }
        if (tp)
        {
            EXPRESSION** exp2;
            if (Optimizer::cparams.prm_cplusplus && tp->IsStructured())
            {
                castToPointer(&tp, &exp, (Keyword) - 1, itype);
            }
            exp2 = &exp;
            while (IsCastValue(*exp2))
                exp2 = &(*exp2)->left;
            if ((*exp2)->type == ExpressionNode::callsite_ && (*exp2)->v.func->sp->sb->storage_class == StorageClass::overloads_)
            {
                Type* tp1 = nullptr;
                SYMBOL* sp2;
                sp2 = MatchOverloadedFunction(itype, itype->IsPtr() ? &tp : &tp1, (*exp2)->v.func->sp, exp2, 0);
                if (sp2)
                {
                    if ((*exp2)->type == ExpressionNode::pc_ || ((*exp2)->type == ExpressionNode::callsite_ && !(*exp2)->v.func->ascall))
                    {
                        thunkForImportTable(exp2);
                    }
                }
            }
            if ((*exp2)->type == ExpressionNode::callsite_ && !(*exp2)->v.func->ascall)
            {
                InsertInline((*exp2)->v.func->sp);
            }
            if (tp->type == BasicType::memberptr_)
            {
                errorConversionOrCast(true, tp, itype);
            }
            if (Optimizer::cparams.prm_cplusplus && string && !itype->BaseType()->btp->IsConst())
                error(ERR_INVALID_CHARACTER_STRING_CONVERSION);

            if (tp->IsArray() && (tp)->msil)
                error(ERR_MANAGED_OBJECT_NO_ADDRESS);
            else if (tp->IsStructured())
                error(ERR_ILL_STRUCTURE_ASSIGNMENT);
            else if (!tp->IsPtr() && (tp->btp && !tp->btp->IsPtr()) && !tp->IsFunction() && !tp->IsInt() &&
                     tp->type != BasicType::aggregate_)
                error(ERR_INVALID_POINTER_CONVERSION);
            else if (!itype->SameExceptionType(tp))
            {
                errorConversionOrCast(true, tp, itype);
            }
            else if (tp->IsFunction() || tp->type == BasicType::aggregate_)
            {
                if (!itype->IsFunctionPtr() || !itype->BaseType()->btp->CompatibleType(tp))
                {
                    if (Optimizer::cparams.prm_cplusplus)
                    {
                        if (!itype->IsVoidPtr() && !tp->nullptrType)
                            if (tp->type == BasicType::aggregate_)
                                errorConversionOrCast(true, tp, itype);
                    }
                    else if (!tp->IsVoidPtr() && !itype->IsVoidPtr())
                        error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                }
            }
            else if (!itype->CompatibleType(tp))
            {
                if (tp->IsPtr())
                {
                    if (Optimizer::cparams.prm_cplusplus)
                    {
                        if (!itype->IsVoidPtr() && !tp->nullptrType)
                            if (!itype->IsPtr() || tp->type == BasicType::aggregate_ || !tp->BaseType()->btp->IsStructured() ||
                                !itype->BaseType()->btp->IsStructured() ||
                                classRefCount(itype->BaseType()->btp->BaseType()->sp, tp->BaseType()->btp->BaseType()->sp) != 1)
                                errorConversionOrCast(true, tp, itype);
                    }
                    else if (!tp->IsVoidPtr() && !itype->IsVoidPtr())
                        if (!tp->SameCharType(itype))
                            error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                }
                else if (tp->IsInt() && isintconst(exp))
                {
                    if (!isconstzero(tp, exp))
                        error(ERR_NONPORTABLE_POINTER_CONVERSION);
                }
                else if (Optimizer::cparams.prm_cplusplus)
                {
                    errorConversionOrCast(true, tp, itype);
                }
            }
            /* might want other types of conversion checks */
            if (!itype->CompatibleType(tp) && !tp->IsInt())
                cast(tp, &exp);
        }
    }
    exp->structByAddress = true;
    InsertInitializer(init, itype, exp, offset, false);
    if (needend)
    {
        if (!needkw(&lex, Keyword::end_))
        {
            errskim(&lex, skim_end);
            skip(&lex, Keyword::closebr_);
        }
    }
    return lex;
}
static LexList* initialize_memberptr(LexList* lex, SYMBOL* funcsp, int offset, StorageClass sc, Type* itype, std::list<Initializer*>** init, bool arrayElem, SYMBOL* sym)
{
    Type* tp = nullptr;
    EXPRESSION* exp = nullptr;
    bool needend = false;
    if (MATCHKW(lex, Keyword::begin_))
    {
        needend = true;
        lex = getsym();
    }
    if (Optimizer::cparams.prm_cplusplus && needend && MATCHKW(lex, Keyword::end_))
    {
        exp = MakeIntExpression(ExpressionNode::memberptr_, 0);  // no SP means fill it with zeros...
    }
    else
    {
        lex = init_expression(lex, funcsp, nullptr, &tp, &exp, itype, false, arrayElem && !needend, sym);
        ResolveTemplateVariable(&tp, &exp, itype, nullptr);
        if (!isconstzero(tp, exp) && exp->type != ExpressionNode::nullptr_)
        {
            EXPRESSION** exp2;
            if (Optimizer::cparams.prm_cplusplus && tp->IsStructured())
            {
                castToPointer(&tp, &exp, (Keyword) - 1, itype);
            }
            exp2 = &exp;
            while (IsCastValue(*exp2))
                exp2 = &(*exp2)->left;
            if ((*exp2)->type == ExpressionNode::callsite_ && (*exp2)->v.func->sp->sb->storage_class == StorageClass::overloads_)
            {
                Type* tp1 = nullptr;
                if ((*exp2)->v.func->sp->sb->parentClass && !(*exp2)->v.func->asaddress)
                    error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
                funcsp = MatchOverloadedFunction(itype, &tp1, (*exp2)->v.func->sp, exp2, 0);
                if (funcsp)
                {
                    exp = MakeExpression(ExpressionNode::memberptr_, funcsp);
                }
                if (tp1 && !itype->btp->CompatibleType(tp1))
                {
                    errorConversionOrCast(true, tp1, itype);
                }
            }
            else if (exp->type == ExpressionNode::memberptr_)
            {
                if ((exp->v.sp->sb->parentClass != itype->BaseType()->sp &&
                     exp->v.sp->sb->parentClass != itype->BaseType()->sp->sb->mainsym &&
                     !SameTemplate(itype, exp->v.sp->sb->parentClass->tp)) ||
                    !itype->BaseType()->btp->CompatibleType(exp->v.sp->tp->BaseType()))

                    errorConversionOrCast(true, tp, itype);
            }
            else
            {
                Type tp4;
                if (Optimizer::cparams.prm_cplusplus && tp->IsStructured())
                {
                    castToPointer(&tp, &exp, (Keyword) - 1, itype);
                }
                if (tp->IsFunction()&& tp->BaseType()->syms->size())
                {
                    auto sp = tp->BaseType()->syms->front();
                    if (sp->sb->thisPtr)
                    {
                        tp4 = {};
                        Type::MakeType(tp4, BasicType::memberptr_, tp);
                        tp4.sp = sp->tp->BaseType()->btp->BaseType()->sp;
                        tp = &tp4;
                    }
                }
                if (!itype->CompatibleType(tp))
                {
                    errorConversionOrCast(true, tp, itype);
                }
            }
        }
        else
        {
            exp = MakeIntExpression(ExpressionNode::memberptr_, 0);  // no SP means fill it with zeros...
        }
    }
    InsertInitializer(init, itype, exp, offset, false);
    if (needend)
    {
        if (!needkw(&lex, Keyword::end_))
        {
            errskim(&lex, skim_end);
            skip(&lex, Keyword::closebr_);
        }
    }
    return lex;
}
ExpressionNode referenceTypeError(Type* tp, EXPRESSION* exp)
{
    ExpressionNode en = ExpressionNode::comma_;
    tp = tp->BaseType()->btp->BaseType();
    switch (tp->type == BasicType::enum_ ? tp->btp->type : tp->type)
    {
        case BasicType::lref_: /* only used during initialization */
        case BasicType::rref_: /* only used during initialization */
            en = ExpressionNode::l_ref_;
            break;
        case BasicType::bit_:
            en = (ExpressionNode)-1;
            break;
        case BasicType::bool_:
            en = ExpressionNode::l_bool_;
            break;
        case BasicType::char_:
            if (Optimizer::cparams.prm_charisunsigned)
                en = ExpressionNode::l_uc_;
            else
                en = ExpressionNode::l_c_;
            break;
        case BasicType::signed_char_:
            en = ExpressionNode::l_c_;
            break;
        case BasicType::char8_t_:
        case BasicType::unsigned_char_:
            en = ExpressionNode::l_uc_;
            break;
        case BasicType::short_:
            en = ExpressionNode::l_s_;
            break;
        case BasicType::unsigned_short_:
            en = ExpressionNode::l_us_;
            break;
        case BasicType::wchar_t_:
            en = ExpressionNode::l_wc_;
            break;
        case BasicType::int_:
            en = ExpressionNode::l_i_;
            break;
        case BasicType::inative_:
            en = ExpressionNode::l_inative_;
            break;
        case BasicType::unsigned_:
            en = ExpressionNode::l_ui_;
            break;
        case BasicType::unative_:
            en = ExpressionNode::l_unative_;
            break;
        case BasicType::char16_t_:
            en = ExpressionNode::l_u16_;
            break;
        case BasicType::char32_t_:
            en = ExpressionNode::l_u32_;
            break;
        case BasicType::long_:
            en = ExpressionNode::l_l_;
            break;
        case BasicType::unsigned_long_:
            en = ExpressionNode::l_ul_;
            break;
        case BasicType::long_long_:
            en = ExpressionNode::l_ll_;
            break;
        case BasicType::unsigned_long_long_:
            en = ExpressionNode::l_ull_;
            break;
        case BasicType::bitint_:
            en = ExpressionNode::l_bitint_;
            break;
        case BasicType::unsigned_bitint_:
            en = ExpressionNode::l_ubitint_;
            break;
        case BasicType::float_:
            en = ExpressionNode::l_f_;
            break;
        case BasicType::double_:
            en = ExpressionNode::l_d_;
            break;
        case BasicType::long_double_:
            en = ExpressionNode::l_ld_;
            break;
        case BasicType::float__complex_:
            en = ExpressionNode::l_fc_;
            break;
        case BasicType::double__complex_:
            en = ExpressionNode::l_dc_;
            break;
        case BasicType::long_double_complex_:
            en = ExpressionNode::l_ldc_;
            break;
        case BasicType::float__imaginary_:
            en = ExpressionNode::l_fi_;
            break;
        case BasicType::double__imaginary_:
            en = ExpressionNode::l_di_;
            break;
        case BasicType::long_double_imaginary_:
            en = ExpressionNode::l_ldi_;
            break;
        case BasicType::string_:
            en = ExpressionNode::l_string_;
            break;
        case BasicType::object_:
            en = ExpressionNode::l_object_;
            break;
        case BasicType::pointer_:
            if (tp->array || tp->vla)
                return ExpressionNode::comma_;
            if (tp->IsFunctionPtr() && (exp->type == ExpressionNode::callsite_ || exp->type == ExpressionNode::pc_))
                return exp->type;
            en = ExpressionNode::l_p_;
            break;
        case BasicType::class_:
        case BasicType::struct_:
        case BasicType::union_:
            if (exp->type == ExpressionNode::l_ref_)
                return exp->type;
            return ExpressionNode::l_p_;
        case BasicType::func_:
        case BasicType::ifunc_:
            if (exp->type == ExpressionNode::l_ref_ || exp->type == ExpressionNode::callsite_ || exp->type == ExpressionNode::pc_)
                return exp->type;
            return ExpressionNode::l_p_;
        default:
            diag("ref reference error");
            break;
    }
    return en;
}
static EXPRESSION* ConvertInitToRef(EXPRESSION* exp, Type* tp, Type* boundTP, StorageClass sc)
{
    if (exp->type == ExpressionNode::hook_)
    {
        exp->right->left = ConvertInitToRef(exp->right->left, tp, boundTP, sc);
        exp->right->right = ConvertInitToRef(exp->right->right, tp, boundTP, sc);
    }
    else
    {
        auto exp2 = exp;
        while (IsCastValue(exp2)) exp2 = exp2->left;
        while (exp2->type == ExpressionNode::comma_ && exp2->right) exp2 = exp2->right;
        if (exp2->type == ExpressionNode::assign_)
        {
            exp2 = exp2->left;
            while (IsCastValue(exp2)) exp2 = exp2->left;
        }
        if (!definingTemplate && (referenceTypeError(tp, exp2) != exp2->type || (tp->type == BasicType::rref_ && IsLValue(exp))) &&
            (!tp->BaseType()->btp->IsStructured() || exp->type != ExpressionNode::lvalue_) && (!tp->BaseType()->btp->IsPtr() || exp->type != ExpressionNode::l_p_))
        {
            if (!isarithmeticconst(exp) && exp->type != ExpressionNode::thisref_ && exp->type != ExpressionNode::callsite_ &&
                tp->BaseType()->btp->BaseType()->type != BasicType::memberptr_ && !boundTP->rref && !boundTP->lref)
                errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, tp, boundTP);
            if (sc != StorageClass::parameter_ && !boundTP->rref && !boundTP->lref)
                exp = createTemporary(tp, exp);
        }
        TakeAddress(&exp, tp);
    }
    return exp;
}
static LexList* initialize_reference_type(LexList* lex, SYMBOL* funcsp, int offset, StorageClass sc, Type* itype, std::list<Initializer*>** init,
                                          int flags, SYMBOL* sym)
{
    Type* tp;
    EXPRESSION* exp;
    bool needend = false;
    Type* tpi = itype;
    (void)sc;
    if (MATCHKW(lex, Keyword::begin_))
    {
        needend = true;
        lex = getsym();
    }
    lex = expression_no_comma(lex, funcsp, nullptr, &tp, &exp, nullptr, flags);
    if (tp)
    {
        optimize_for_constants(&exp);
    }
    if (!tp)
        error(ERR_EXPRESSION_SYNTAX);
    if (tp)
    {
        ResolveTemplateVariable(&tp, &exp, itype->BaseType()->btp, nullptr);
        DeduceAuto(&itype, tp, exp,  true);
        DeduceAuto(&sym->tp, tp, exp, true);
        itype->InstantiateDeferred();
        itype->UpdateRootTypes();
        sym->tp->UpdateRootTypes();
        CheckThroughConstObject(itype, exp);
        if (!tp->IsRef() &&
            ((tp->IsConst() && !itype->BaseType()->btp->IsConst()) || (tp->IsVolatile() && !itype->BaseType()->btp->IsVolatile())))
            error(ERR_REF_INITIALIZATION_DISCARDS_QUALIFIERS);
        else if (itype->BaseType()->btp->BaseType()->type == BasicType::memberptr_)
        {
            bool ref = false;
            EXPRESSION* exp1 = exp;
            Type* itype1 = itype->BaseType()->btp->BaseType();
            if (tp->lref || tp->rref)
            {
                tp = tp->BaseType()->btp;
                if (!itype1->CompatibleType(tp))
                    errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, itype1, tp);
            }
            else
            {
                if (tp->BaseType()->nullptrType || (tp->IsInt() && isconstzero(tp, exp)))
                {
                    int lbl = dumpMemberPtr(nullptr, itype1, true);
                    exp = MakeIntExpression(ExpressionNode::labcon_, lbl);
                }
                else if (tp->type == BasicType::aggregate_)
                {
                    if (itype1->btp->IsFunction())
                    {
                        SYMBOL* funcsp;
                        funcsp = MatchOverloadedFunction(itype1, &tp, tp->sp, &exp, 0);
                        if (funcsp)
                        {
                            int lbl = dumpMemberPtr(funcsp, tp, true);
                            exp = MakeIntExpression(ExpressionNode::labcon_, lbl);
                        }
                        else
                        {
                            exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                        }
                    }
                    else
                    {
                        errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, itype1, tp);
                    }
                }
                else if (tp->IsFunction())
                {
                    if (!itype1->CompatibleType(tp))
                        errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, itype1, tp);
                    else
                    {
                        funcsp = tp->btp->BaseType()->sp;
                        if (funcsp)
                        {
                            int lbl = dumpMemberPtr(funcsp, tp, true);
                            exp = MakeIntExpression(ExpressionNode::labcon_, lbl);
                        }
                        else
                        {
                            exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                        }
                    }
                }
                else
                {
                    if (!itype1->CompatibleType(tp))
                        errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, itype, tp);
                }
                if (exp->type == ExpressionNode::labcon_)
                {
                    if (sc != StorageClass::parameter_)
                        exp1 = createTemporary(itype1, nullptr);
                    exp = MakeExpression(ExpressionNode::blockassign_, exp1, exp);
                    exp->size = Type::MakeType(BasicType::memberptr_);
                    exp->altdata = (void*)(&stdpointer);
                    exp = MakeExpression(ExpressionNode::comma_, exp, exp1);
                }
            }
        }
        else if (((!itype->BaseType()->btp->IsArithmetic() && itype->BaseType()->type != BasicType::enum_) ||
                  (!tp->IsArithmetic() && tp->BaseType()->type == BasicType::enum_)) &&
                 !itype->CompatibleType(tp) &&
                 (!itype->btp->IsStructured() || !tp->IsStructured() || classRefCount(itype->btp->BaseType()->sp, tp->BaseType()->sp) != 1))
        {
            if (itype->btp->IsStructured())
            {
                EXPRESSION* ths = AnonymousVar(StorageClass::auto_, tp);
                EXPRESSION* paramexp = exp;
                Type* ctype = itype->btp->BaseType();
                exp = ths;
                callConstructorParam(&ctype, &exp, tp, paramexp, true, true, false, false, true);
            }
            else
            {
                errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, itype->btp, tp);
            }
        }
        else if (!tp->IsRef() && !tp->IsStructured() && !tp->IsFunction() && (!tp->IsPtr() || !tp->array))
        {
            exp = ConvertInitToRef(exp, itype, tp, sc);
        }
        if (itype->type == BasicType::rref_ && itype->btp->IsStructured() && exp->type != ExpressionNode::lvalue_ && exp->type != ExpressionNode::l_ref_)
        {
            EXPRESSION* expx = exp;
            bool lref = false;
            if (expx->type == ExpressionNode::thisref_)
                expx = expx->left;
            if (expx->type == ExpressionNode::callsite_)
            {
                if (expx->v.func->returnSP)
                {
                    if (!expx->v.func->returnSP->sb->anonymous)
                        lref = true;
                }
            }
            else
            {
                lref = true;
            }
            if (lref)
                errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, itype, tp);
        }
    }
    exp->structByAddress = true;
    InsertInitializer(init, itype, exp, offset, false);
    if (needend)
    {
        if (!needkw(&lex, Keyword::end_))
        {
            errskim(&lex, skim_end);
            skip(&lex, Keyword::closebr_);
        }
    }
    return lex;
}
typedef struct _aggregate_descriptor
{
    struct _aggregate_descriptor* next;
    int stopgap : 1;
    int inbase : 1;
    int offset;
    int reloffset;
    int max;
    SymbolTable<SYMBOL>::iterator it, ite;
    Type* tp;
    
    std::list<BASECLASS*>::iterator currentBase;
    std::list<BASECLASS*>::iterator currentBaseEnd;
} AGGREGATE_DESCRIPTOR;
static void increment_desc(AGGREGATE_DESCRIPTOR** desc, AGGREGATE_DESCRIPTOR** cache);

static void free_desc(AGGREGATE_DESCRIPTOR** descin, AGGREGATE_DESCRIPTOR** cache)
{
    if (*descin)
    {
        AGGREGATE_DESCRIPTOR* temp = *descin;
        int max;
        max = (*descin)->reloffset + (*descin)->offset;
        if (max > (*descin)->max)
            (*descin)->max = max;
        max = (*descin)->max;
        *descin = (*descin)->next;
        if (*descin && max > (*descin)->max)
            (*descin)->max = max;
        temp->next = *cache;
        *cache = temp;
    }
}
static bool atend(AGGREGATE_DESCRIPTOR* desc)
{
    if (desc->tp->IsStructured())
        return desc->it == desc->ite;
    else
        return desc->tp->size && desc->reloffset >= desc->tp->size;
}
static void unwrap_desc(AGGREGATE_DESCRIPTOR** descin, AGGREGATE_DESCRIPTOR** cache, std::list<Initializer*>** dest)
{
    if (!Optimizer::cparams.prm_cplusplus)
        dest = nullptr;
    while (*descin && (!(*descin)->stopgap || (dest && ((*descin)->it != (*descin)->ite))))
    {
        // this won't work with the declarator syntax in C++20
        if (dest)
        {
            if ((*descin)->it != (*descin)->ite)
            {
                SYMBOL* sym = *(*descin)->it;
                if (sym->sb->init)
                {
                    for (auto list : *sym->sb->init)
                    {
                        if (list->basetp && list->exp)
                        {
                            SYMBOL* fieldsp;
                            InsertInitializer(dest, list->basetp, list->exp, list->offset + (*descin)->offset + (*descin)->reloffset,
                                       false);
                            if (ismember(sym))
                            {
                                (*dest)->back()->fieldsp = sym;
                                (*dest)->back()->fieldoffs = MakeIntExpression(ExpressionNode::c_i_, (*descin)->offset);
                            }
                        }
                    }
                }
            }
            increment_desc(descin, cache);
        }
        else
        {
            free_desc(descin, cache);
        }
    }
}
static void allocate_desc(Type* tp, int offset, AGGREGATE_DESCRIPTOR** descin, AGGREGATE_DESCRIPTOR** cache)
{
    AGGREGATE_DESCRIPTOR **temp = cache, *desc = nullptr;

    while (*temp)
    {
        if ((*temp)->offset == offset)
        {
            desc = *temp;
            *temp = desc->next;
            break;
        }
        temp = &(*temp)->next;
    }
    if (!desc)
    {
        desc = new AGGREGATE_DESCRIPTOR;
        desc->tp = tp;
        desc->offset = offset;
    }
    desc->reloffset = 0;
    desc->next = *descin;
    desc->stopgap = false;
    desc->max = 0;
    desc->inbase = false;
    *descin = desc;
    if (tp->IsStructured())
    {
        desc->it = tp->BaseType()->syms->begin();
        desc->ite = tp->BaseType()->syms->end();
        if (tp->BaseType()->sp->sb->baseClasses)
        {
            auto bc = tp->BaseType()->sp->sb->baseClasses;
            if (bc && bc->size())
            {
                desc->currentBase =bc->begin();
                desc->currentBaseEnd = bc->end();
                desc->inbase = true;
                tp = bc->front()->cls->tp;
                allocate_desc(tp, offset + bc->front()->offset, descin, cache);
            }
        }
    }
}
static int str_candidate(LexList* lex, Type* tp)
{
    LexList *old = lex;
    if (MATCHKW(lex, Keyword::openpa_))
    {
        while (lex && MATCHKW(lex, Keyword::openpa_))
            lex = getsym();
        prevsym(old);
    }
    if (!lex)
        return false;
    Type* bt;
    bt = tp->BaseType();
    if (bt->type == BasicType::string_)
        return true;
    if (bt->type == BasicType::pointer_)
        if (lex->data->type == LexType::l_astr_ || lex->data->type == LexType::l_wstr_ || lex->data->type == LexType::l_ustr_ || lex->data->type == LexType::l_Ustr_ || lex->data->type == LexType::l_u8str_)
        {
            bt = bt->btp->BaseType();
            if (bt->type == BasicType::char8_t_ || bt->type == BasicType::short_ || bt->type == BasicType::unsigned_short_ || bt->type == BasicType::wchar_t_ || bt->type == BasicType::char_ ||
                bt->type == BasicType::unsigned_char_ || bt->type == BasicType::signed_char_ || bt->type == BasicType::char16_t_ || bt->type == BasicType::char32_t_)
                return true;
        }
    return false;
}
static bool designator(LexList** lex, SYMBOL* funcsp, AGGREGATE_DESCRIPTOR** desc, AGGREGATE_DESCRIPTOR** cache, SYMBOL* sym)
{

    if (MATCHKW(*lex, Keyword::openbr_) || MATCHKW(*lex, Keyword::dot_))
    {
        bool done = false;
        unwrap_desc(desc, cache, nullptr);
        (*desc)->reloffset = 0;
        while (!done && (MATCHKW(*lex, Keyword::openbr_) || MATCHKW(*lex, Keyword::dot_)))
        {
            if (MATCHKW(*lex, Keyword::openbr_))
            {
                Type* tp = nullptr;
                EXPRESSION* enode = nullptr;
                int index;
                *lex = getsym();
                *lex = init_expression(*lex, funcsp, nullptr, &tp, &enode, (*desc)->tp, false, false, sym);
                needkw(lex, Keyword::closebr_);
                if (!tp)
                    error(ERR_EXPRESSION_SYNTAX);
                else if (!tp->IsInt())
                    error(ERR_NEED_INTEGER_TYPE);
                else if (!isarithmeticconst(enode) && !msilConstant(enode) && !Optimizer::cparams.prm_cplusplus)
                    error(ERR_CONSTANT_VALUE_EXPECTED);
                else if ((*desc)->tp->IsStructured() || !(*desc)->tp->BaseType()->array)
                    error(ERR_ARRAY_EXPECTED);
                else
                {
                    index = enode->v.i;
                    tp = (*desc)->tp->BaseType();
                    if (tp->size)
                    {
                        if (index >= tp->size / tp->btp->size)
                        {
                            error(ERR_ARRAY_INDEX_OUT_OF_RANGE);
                            index = 0;
                        }
                    }
                    tp = tp->btp;
                    (*desc)->reloffset = index * tp->size;
                    if ((tp->IsArray() && MATCHKW(*lex, Keyword::openbr_)) || (tp->IsStructured() && MATCHKW(*lex, Keyword::dot_)))
                        allocate_desc(tp, (*desc)->reloffset + (*desc)->offset, desc, cache);
                    else
                        done = true;
                }
            }
            else
            {
                *lex = getsym();
                if (ISID(*lex))
                {
                    if ((*desc)->tp->IsStructured())
                    {
                        SymbolTable<SYMBOL>::iterator it;
                        for (it = (*desc)->tp->BaseType()->syms->begin(); it != (*desc)->tp->BaseType()->syms->end() && strcmp((*it)->name, (*lex)->data->value.s.a) != 0; ++it);
                        *lex = getsym();
                        if (it != (*desc)->tp->BaseType()->syms->end())
                        {
                            SYMBOL* sym = *it;
                            Type* tp = sym->tp;
                            (*desc)->reloffset = sym->sb->offset;
                            (*desc)->it = it;
                            (*desc)->ite = (*desc)->tp->BaseType()->syms->end();
                            if ((tp->IsArray() && MATCHKW(*lex, Keyword::openbr_)) || (tp->IsStructured() && MATCHKW(*lex, Keyword::dot_)))
                                allocate_desc(tp, (*desc)->reloffset + (*desc)->offset, desc, cache);
                            else
                                done = true;
                        }
                        else
                            error(ERR_CLASS_TYPE_EXPECTED);
                    }
                    else
                    {
                        *lex = getsym();
                        errorNotMember((*desc)->tp->BaseType()->sp, nullptr, (*lex)->data->value.s.a);
                    }
                }
                else
                {
                    error(ERR_IDENTIFIER_EXPECTED);
                }
            }
        }
        needkw(lex, Keyword::assign_);
        return true;
    }
    return false;
}
static void increment_desc(AGGREGATE_DESCRIPTOR** desc, AGGREGATE_DESCRIPTOR** cache)
{
    while (*desc)
    {
        if ((*desc)->tp->IsStructured())
        {
            int offset = (*desc)->reloffset + (*desc)->offset;
            if ((*desc)->tp->IsUnion())
                (*desc)->it = (*desc)->ite;
            else if (!(*desc)->tp->BaseType()->sp->sb->hasUserCons)
                while (true)
                {
                    ++(*desc)->it;
                    if ((*desc)->it == (*desc)->ite)
                    {
                        if ((*desc)->next && (*desc)->next->inbase)
                        {
                            free_desc(desc, cache);

                            ++(*desc)->currentBase;
                            if ((*desc)->currentBase != (*desc)->currentBaseEnd)
                            {
                                allocate_desc((*(*desc)->currentBase)->cls->tp, (*desc)->offset + (*(*desc)->currentBase)->offset,
                                              desc,
                                    cache);
                            }
                            offset = -1;
                        }
                        else
                        {
                            break;
                        }
                    }
                    if ((*(*desc)->it)->sb->storage_class != StorageClass::overloads_)
                    {
                        if ((*(*desc)->it)->tp->hasbits)
                        {
                            if (!(*(*desc)->it)->sb->anonymous)
                                break;
                        }
                        else if ((*(*desc)->it)->sb->offset + (*desc)->offset != offset)
                        {
                            break;
                        }
                    }
                }
            else
                (*desc)->it = (*desc)->ite;
            if ((*desc)->it != (*desc)->ite)
                (*desc)->reloffset = (*(*desc)->it)->sb->offset;
            else
                (*desc)->reloffset = (*desc)->tp->size;
            if (atend(*desc) && !(*desc)->stopgap)
            {
                free_desc(desc, cache);
            }
            else
                break;
        }
        else  // array element
        {
            Type* tp = (*desc)->tp;
            (*desc)->reloffset += tp->BaseType()->btp->size;
            if (atend(*desc) && !(*desc)->stopgap)
            {
                free_desc(desc, cache);
            }
            else
            {
                break;
            }
        }
    }
}
static bool ascomp(const Initializer* left, const Initializer* right)
{
    int lofs = (left->offset << 7) + left->basetp->BaseType()->startbit;
    int rofs = (right->offset << 7) + right->basetp->BaseType()->startbit;
    if (lofs < rofs)
        return true;
    return false;
}
static void sort_aggregate_initializers(std::list<Initializer*>* data)
{
    if (!data)
        return;
    bool inorder = true;
    int last = 0;
    for (auto d : *data)
        if (d->offset < last)
        {
            inorder = false;
            break;
        }
        else
        {
            last = d->offset;
        }
    if (!inorder)
        data->sort(ascomp);
    int offset = -1;
    std::list<Initializer*>::iterator itlast;
    /* trim duplicates - highest tag value indicated for a given offset/startbit pair wins */
    for (auto it = data->begin(); it != data->end();)
    {
        int lofs = ((*it)->offset << 7) + (*it)->basetp->BaseType()->startbit;
        if (lofs == offset)
        {
            if ((*itlast)->tag > (*it)->tag)
            {
                it = data->erase(it);
            }
            else
            {
                data->erase(itlast);
                itlast = it;
                ++it;
            }
        }
        else
        {
            offset = lofs;
            itlast = it;
            ++it;
        }
    }
}
static void set_array_sizes(AGGREGATE_DESCRIPTOR* cache)
{
    while (cache)
    {
        if (!cache->tp->IsStructured())
        {
            if (cache->tp->size == 0)
            {
                int size = cache->max;
                int base = cache->tp->BaseType()->btp->size;
                Type* temp = cache->tp;
                if (base == 0)
                    base = 1;
                size = size + base - 1;
                size = (size / base) * base;
                while (temp && temp->size == 0)
                {
                    temp->size = size;
                    temp->esize = MakeIntExpression(ExpressionNode::c_i_, size / base);
                    temp = temp->btp;
                }
            }
        }
        cache = cache->next;
    }
}
static LexList* read_strings(LexList* lex, std::list<Initializer*>** next, AGGREGATE_DESCRIPTOR** desc)
{
    bool nothingWritten = true;
    Type* tp = (*desc)->tp->BaseType();
    Type* btp = tp->btp->BaseType();
    int max = tp->size / btp->size;
    int j;
    EXPRESSION* expr;
    StringData* string = nullptr;
    int index = 0;
    int i;
    /* this assumes the sizeof(short) & sizeof(wchar_t) < 16 */
    if (max == 0)
        max = INT_MAX / 16;
    int opencount = 0;
    while (lex && MATCHKW(lex, Keyword::openpa_))
    {
         lex = getsym();
         opencount++;
    }
    lex = concatStringsInternal(lex, &string, 0);
    while (opencount--)
    {
         if (!needkw(&lex, Keyword::closepa_))
             break;
    }
    switch (string->strtype)
    {
        case LexType::l_u8str_:
            if (btp->type != BasicType::char8_t_ && btp->type != BasicType::char_ && btp->type != BasicType::unsigned_char_ && btp->type != BasicType::signed_char_)
                error(ERR_STRING_TYPE_MISMATCH_IN_INITIALIZATION);
            break;
        case LexType::l_astr_:
            if (btp->type != BasicType::char_ && btp->type != BasicType::unsigned_char_ && btp->type != BasicType::signed_char_)
                error(ERR_STRING_TYPE_MISMATCH_IN_INITIALIZATION);
            break;
        case LexType::l_wstr_:
            if (btp->type != BasicType::wchar_t_ && btp->type != BasicType::short_ && btp->type != BasicType::unsigned_short_)
                error(ERR_STRING_TYPE_MISMATCH_IN_INITIALIZATION);
            break;
        case LexType::l_msilstr_:
            if (tp->type != BasicType::string_ && btp->type != BasicType::wchar_t_ && btp->type != BasicType::short_ && btp->type != BasicType::unsigned_short_)
                error(ERR_STRING_TYPE_MISMATCH_IN_INITIALIZATION);
            break;
        case LexType::l_ustr_:
            if (btp->type != BasicType::char16_t_)
                error(ERR_STRING_TYPE_MISMATCH_IN_INITIALIZATION);
            break;
        case LexType::l_Ustr_:
            if (btp->type != BasicType::char32_t_)
                error(ERR_STRING_TYPE_MISMATCH_IN_INITIALIZATION);
            break;
        default:
            break;
    }
    for (j = 0; j < string->size; j++)
    {
        int len = string->pointers[j]->count;
        if (len + index > max)
        {
            error(ERR_TOO_MANY_InitializerS);
        }
        else
        {
            int i;
            for (i = 0; i < len; i++)
            {
                EXPRESSION* exp = MakeIntExpression(ExpressionNode::c_i_, string->pointers[j]->str[i]);
                InsertInitializer(next, btp, exp, (*desc)->offset + (*desc)->reloffset, false); /* nullptr=no initializer */
                (*desc)->reloffset += btp->size;
                index++;
                nothingWritten = false;
            }
        }
    }
    if (max == INT_MAX / 16)
    {
        EXPRESSION* exp = MakeIntExpression(ExpressionNode::c_i_, 0);

        InsertInitializer(next, btp, exp, (*desc)->offset + (*desc)->reloffset, false); /* nullptr=no initializer */
        max = (*desc)->reloffset / btp->size;
        nothingWritten = false;
    }
    if (nothingWritten)
    {
        EXPRESSION* exp = MakeIntExpression(ExpressionNode::c_i_, 0);

        InsertInitializer(next, btp, exp, (*desc)->offset + (*desc)->reloffset, false); /* nullptr=no initializer */
    }
    for (i = (*desc)->reloffset / btp->size; i < max; i++)
    {
        (*desc)->reloffset += btp->size;
    }
    if ((*desc)->reloffset < max * btp->size)
    {
        EXPRESSION* exp = MakeIntExpression(ExpressionNode::c_i_, 0);

        InsertInitializer(next, btp, exp, (*desc)->offset + (*desc)->reloffset, false); /* nullptr=no initializer */
        (*desc)->reloffset += btp->size;
    }
    return lex;
}
static Type* nexttp(AGGREGATE_DESCRIPTOR* desc)
{
    Type* rv;
    if (desc->tp->IsStructured())
    {
        if (!Optimizer::cparams.prm_cplusplus || !desc->tp->BaseType()->sp->sb->hasUserCons)
        {
            for (; desc->it != desc->ite && (istype(*desc->it) || (*desc->it)->tp->type == BasicType::aggregate_); ++desc->it)
                ;
            if (desc->it == desc->ite)
                return nullptr;
            rv = (*desc->it)->tp;
        }
        else
        {
            rv = desc->tp->BaseType();
        }
    }
    else
        rv = desc->tp->BaseType()->btp;
    return rv;
}
static LexList* initialize___object(LexList* lex, SYMBOL* funcsp, int offset, Type* itype, std::list<Initializer*>** init)
{
    EXPRESSION* expr = nullptr;
    Type* tp = nullptr;
    lex = expression_assign(lex, funcsp, nullptr, &tp, &expr, nullptr, 0);
    if (!tp || !lex)
    {
        error(ERR_EXPRESSION_SYNTAX);
    }
    else if (tp->IsArithmetic() || (tp->IsPtr() && (!tp->IsArray() || !tp->msil)))
    {
        cast(tp, &expr);
    }
    InsertInitializer(init, itype, expr, offset, false);
    return lex;
}
static LexList* initialize___string(LexList* lex, SYMBOL* funcsp, int offset, Type* itype, std::list<Initializer*>** init)
{
    EXPRESSION* expr = nullptr;
    Type* tp = nullptr;
    lex = expression_assign(lex, funcsp, itype, &tp, &expr, nullptr, 0);
    if (!tp || !lex)
    {
        error(ERR_EXPRESSION_SYNTAX);
    }
    else if (tp->type != BasicType::string_)
    {
        if (expr && expr->type == ExpressionNode::labcon_ && expr->string)
            expr->type = ExpressionNode::c_string_;
        else
            errorConversionOrCast(true, tp, itype);
    }
    InsertInitializer(init, itype, expr, offset, false);
    return lex;
}
static LexList* initialize_auto_struct(LexList* lex, SYMBOL* funcsp, int offset, Type* itype, std::list<Initializer*>** init)
{
    EXPRESSION* expr = nullptr;
    Type* tp = nullptr;
    lex = expression_assign(lex, funcsp, nullptr, &tp, &expr, nullptr, 0);
    if (!tp || !lex)
    {
        error(ERR_EXPRESSION_SYNTAX);
    }
    else if (!itype->CompatibleType(tp))
    {
        if ((Optimizer::architecture == ARCHITECTURE_MSIL) && itype->IsStructured() && itype->BaseType()->sp->sb->msil &&
            (isconstzero(tp, expr) || tp->BaseType()->nullptrType))
            InsertInitializer(init, itype, expr, offset, false);
        else
            error(ERR_ILL_STRUCTURE_ASSIGNMENT);
    }
    else
    {
        InsertInitializer(init, itype, expr, offset, false);
    }
    return lex;
}
EXPRESSION* getThisNode(SYMBOL* sym)
{
    EXPRESSION* exp;
    switch (sym->sb->storage_class)
    {
        case StorageClass::member_:
        case StorageClass::mutable_:
            exp = MakeExpression(ExpressionNode::add_, MakeExpression(ExpressionNode::thisshim_), MakeIntExpression(ExpressionNode::c_i_, sym->sb->offset));
            break;
        case StorageClass::auto_:
        case StorageClass::parameter_:
        case StorageClass::register_: /* register variables are treated as
                           * auto variables in this compiler
                           * of course the usage restraints of the
                           * register keyword are enforced elsewhere
                           */
            exp = MakeExpression(ExpressionNode::auto_, sym);
            break;

        case StorageClass::localstatic_:
            if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
                exp = MakeExpression(ExpressionNode::threadlocal_, sym);
            else
                exp = MakeExpression(ExpressionNode::global_, sym);
            break;
        case StorageClass::absolute_:
            exp = MakeExpression(ExpressionNode::absolute_, sym);
            break;
        case StorageClass::static_:
        case StorageClass::global_:
        case StorageClass::external_:
            if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
                exp = MakeExpression(ExpressionNode::threadlocal_, sym);
            else
                exp = MakeExpression(ExpressionNode::global_, sym);
            break;
        default:
            diag("getThisNode: unknown storage class");
            exp = MakeIntExpression(ExpressionNode::c_i_, 0);
            break;
    }
    return exp;
}

auto InitializeSimpleAggregate(LexList*& lex, Type* itype, bool needend, int offset, SYMBOL* funcsp, StorageClass sc, SYMBOL* base,
                               std::list<Initializer*>** dest, bool templateLevel, bool deduceTemplate, int flags)
{
    std::list<Initializer*>* data = nullptr;
    AGGREGATE_DESCRIPTOR *desc = nullptr, *cache = nullptr;
    bool toomany = false;
    bool c99 = false;
    allocate_desc(itype, offset, &desc, &cache);
    desc->stopgap = true;
    while (lex)
    {
        bool gotcomma = false;
        Type* tp2;
        c99 |= designator(&lex, funcsp, &desc, &cache, base);
        tp2 = nexttp(desc);
        bool hasSome = false;
        while (tp2 && (tp2->type == BasicType::aggregate_ || tp2->IsArray() ||
                       (tp2->IsStructured() && (!Optimizer::cparams.prm_cplusplus || !tp2->BaseType()->sp->sb->hasUserCons))))
        {
            if (tp2->type == BasicType::aggregate_)
            {
                increment_desc(&desc, &cache);
            }
            else
            {
                if (MATCHKW(lex, Keyword::begin_))
                {
                    lex = getsym();
                    if (MATCHKW(lex, Keyword::end_))
                    {
                        lex = getsym();
                        increment_desc(&desc, &cache);
                    }
                    else
                    {
                        hasSome = true;
                        allocate_desc(tp2, desc->offset + desc->reloffset, &desc, &cache);
                        desc->stopgap = true;
                        c99 |= designator(&lex, funcsp, &desc, &cache, base);
                    }
                }
                else
                {
                    if (tp2->IsStructured())
                    {
                        auto placeholder = lex;
                        EXPRESSION* exp = nullptr;
                        Type* tp = nullptr;
                        lex = expression_no_comma(lex, funcsp, nullptr, &tp, &exp, nullptr, 0);
                        lex = prevsym(placeholder);
                        if (tp && tp2->CompatibleType(tp))
                        {
                            break;
                        }
                    }
                    if (!atend(desc))
                    {
                        hasSome = true;
                        allocate_desc(tp2, desc->offset + desc->reloffset, &desc, &cache);
                    }
                    else
                    {
                        break;
                    }
                }
            }
            tp2 = nexttp(desc);
        }
        if (atend(desc))
        {
            toomany = hasSome;
            if (!toomany)
            {
                unwrap_desc(&desc, &cache, &data);
                free_desc(&desc, &cache);
                while (MATCHKW(lex, Keyword::end_))
                {
                    lex = getsym();
                    unwrap_desc(&desc, &cache, &data);
                    free_desc(&desc, &cache);
                }
            }
            break;
        }
        /* when we get here, DESC has an aggregate with an element that isn't
         * an aggregate
         */
        if (str_candidate(lex, desc->tp) != 0)
        {
            lex = read_strings(lex, &data, &desc);
        }
        else
        {
            SYMBOL* fieldsp;
            int size = data ? data->size() : 0;
            auto tp1 = nexttp(desc);
            lex = initType(lex, funcsp, desc->offset + desc->reloffset, sc, &data, dest, tp1, base, true,
                templateLevel, flags | _F_NESTEDINIT);
            if (!data->empty() && data->back()->exp->packedArray)
            {
                auto cm = data->back()->exp;
                while (cm->type == ExpressionNode::comma_)
                {
                    auto&& t = data->back();
                    data->back()->exp = cm->left;
                    data->back()->offset = desc->offset + desc->reloffset;
                    data->push_back(Allocate<Initializer>());
                    *(data->back()) = *t;
                    data->back()->offset = desc->offset + desc->reloffset;
                    CheckNarrowing(tp1, data->back()->realtp, data->back()->exp);
                    increment_desc(&desc, &cache);
                    cm = cm->right;
                }
                data->back()->exp = cm;
                data->back()->offset = desc->offset + desc->reloffset;
            }
            CheckNarrowing(tp1, data->back()->realtp, data->back()->exp);
            int size1 = data->size();
            if (desc->it != desc->ite && size != size1)
            {
                fieldsp = *desc->it;
                if (ismember(fieldsp))
                {
                    auto it = data->begin();
                    for (int i = 0; i < size; i++, ++it)
                        ;
                    (*it)->fieldsp = fieldsp;
                    if (itype->IsArray() && Optimizer::architecture == ARCHITECTURE_MSIL)
                    {
                        Type* btp = itype;
                        while (btp->IsArray())
                            btp = btp->btp;
                        int n = desc->offset / btp->size;
                        (*it)->fieldoffs = MakeExpression(ExpressionNode::umul_, MakeIntExpression(ExpressionNode::c_i_, n), MakeExpression(ExpressionNode::sizeof_, MakeExpression(btp)));
                    }
                    else
                    {
                        (*it)->fieldoffs = MakeIntExpression(ExpressionNode::c_i_, desc->offset);
                    }
                }
            }
        }
        increment_desc(&desc, &cache);
        if ((((sc != StorageClass::auto_ && sc != StorageClass::register_) || needend) && MATCHKW(lex, Keyword::comma_)) || MATCHKW(lex, Keyword::end_))
        {
            gotcomma = MATCHKW(lex, Keyword::comma_);
            if (gotcomma && needend)
                lex = getsym();
            while (MATCHKW(lex, Keyword::end_))
            {
                if (desc->it != desc->ite && Optimizer::cparams.prm_cplusplus && itype->IsStructured() &&
                    !itype->BaseType()->sp->sb->trivialCons)
                {
                    while (desc->it != desc->ite)
                    {
                        if ((*desc->it)->tp->IsStructured() && !(*desc->it)->tp->BaseType()->sp->sb->trivialCons)
                        {
                            SYMBOL* fieldsp;
                            int size = data->size();
                            auto tp1 = nexttp(desc);
                            lex = initType(lex, funcsp, desc->offset + desc->reloffset, sc, &data, dest, tp1, base,
                                           itype->IsArray(), deduceTemplate, flags);
                            if (itype->IsArray() && !data->empty() && data->back()->exp->packedArray)
                            {
                                auto cm = data->back()->exp;
                                while (cm->type == ExpressionNode::comma_)
                                {
                                    auto&& t = data->back();
                                    data->back()->exp = cm->left;
                                    data->back()->offset = desc->offset + desc->reloffset;
                                    data->push_back(Allocate<Initializer>());
                                    *(data->back()) = *t;
                                    data->back()->offset = desc->offset + desc->reloffset;
                                    CheckNarrowing(tp1, data->back()->realtp, data->back()->exp);
                                    increment_desc(&desc, &cache);
                                    cm = cm->right;
                                }
                                data->back()->exp = cm;
                                data->back()->offset = desc->offset + desc->reloffset;
                            }
                            int size1 = data->size();
                            if (desc->it != desc->ite && size != size1)
                            {
                                fieldsp = *desc->it;
                                if (ismember(fieldsp))
                                {
                                    auto it = data->begin();
                                    for (int i = 0; i < size; i++, ++it)
                                        ;
                                    (*it)->fieldsp = fieldsp;
                                    (*it)->fieldoffs = MakeIntExpression(ExpressionNode::c_i_, desc->offset);
                                }
                            }
                        }
                        increment_desc(&desc, &cache);
                    }
                }
                gotcomma = false;
                lex = getsym();
                unwrap_desc(&desc, &cache, &data);
                free_desc(&desc, &cache);

                if (!desc)
                {
                    if (!needend)
                    {
                        error(ERR_DECLARE_SYNTAX); /* extra Keyword::end_ */
                    }
                    break;
                }
                increment_desc(&desc, &cache);
                if (MATCHKW(lex, Keyword::comma_))
                {
                    gotcomma = true;
                    lex = getsym();
                }
            }
            if (!desc)
                break;
        }
        if (!desc || !gotcomma || !needend)
            break;
    }
    if (c99)
        RequiresDialect::Feature(Dialect::c99, "Structure Designators");
    if (toomany)
        error(ERR_TOO_MANY_InitializerS);
    if (desc)
    {
        unwrap_desc(&desc, &cache, &data);
        if (needend || desc->next)
        {
            error(ERR_DECLARE_SYNTAX);
            errskim(&lex, skim_semi);
        }
    }
    /* theoretically desc will be nullptr if there are no errors */
    while (desc)
    {
        unwrap_desc(&desc, &cache, &data);
        free_desc(&desc, &cache);
    }
    set_array_sizes(cache);
    sort_aggregate_initializers(data);

    return data;
}
static LexList* initialize_aggregate_type(LexList * lex, SYMBOL * funcsp, SYMBOL * base, int offset, StorageClass sc, Type* itype,
                                          std::list<Initializer*>** init, std::list<Initializer*>** dest, bool arrayMember, bool deduceTemplate, int flags)
{
    std::list<Initializer*>* data = nullptr;
    AGGREGATE_DESCRIPTOR *desc = nullptr, *cache = nullptr;
    bool needend = false;
    bool assn = false;
    bool implicit = false;
    EXPRESSION* baseexp = nullptr;
    if ((Optimizer::cparams.prm_cplusplus || (Optimizer::architecture == ARCHITECTURE_MSIL)) && itype->IsStructured())
        baseexp = MakeExpression(ExpressionNode::add_, getThisNode(base), MakeIntExpression(ExpressionNode::c_i_, offset));

    if (MATCHKW(lex, Keyword::assign_))
    {
        assn = true;
        lex = getsym();
    }
    if ((Optimizer::cparams.prm_cplusplus || ((Optimizer::architecture == ARCHITECTURE_MSIL) && !assn)) && itype->IsStructured() &&
        (!itype->BaseType()->sp->sb->trivialCons && (itype->BaseType()->sp->sb->hasUserCons || !MATCHKW(lex, Keyword::begin_)) || arrayMember))
    {
        if ((base->sb->storage_class != StorageClass::member_ && base->sb->storage_class != StorageClass::mutable_) || MATCHKW(lex, Keyword::openpa_) || assn ||
            MATCHKW(lex, Keyword::begin_))
        {
            // initialization via constructor
            CallSite* funcparams = Allocate<CallSite>();
            EXPRESSION* exp;
            Type* ctype = itype;
            std::list<Initializer*>* it;
            bool maybeConversion = true;
            bool isconversion;
            bool isList = MATCHKW(lex, Keyword::begin_);
            bool constructed = false;
            bool tryelide = false;
            exp = baseexp;
            if (assn || arrayMember)
            {
                // assignments or array members come here
                if (TypeGenerator::StartOfType(lex, nullptr, false))
                {
                    Type* tp1 = nullptr;
                    EXPRESSION* exp1;
                    lex =
                        init_expression(lex, funcsp, nullptr, &tp1, &exp1, itype, false, [&exp, &constructed](EXPRESSION* exp1, Type* tp) {
                            EXPRESSION* oldthis;
                            for (oldthis = exp1; oldthis->right && oldthis->type == ExpressionNode::comma_; oldthis = oldthis->right)
                                ;
                            if (oldthis->type == ExpressionNode::thisref_)
                            {
                                constructed = true;
                                if (oldthis->left->v.func->thisptr || !oldthis->left->v.func->returnEXP)
                                {
                                    if (!IsLValue(oldthis->left->v.func->thisptr))
                                    {
                                        EXPRESSION* exp2 = oldthis->left->v.func->thisptr;
                                        while (exp2->left)
                                            exp2 = exp2->left;
                                        if (exp2->type == ExpressionNode::auto_)
                                        {
                                            exp2->v.sp->sb->dest = nullptr;
                                        }
                                    }

                                    auto ths = oldthis->left->v.func->thisptr;
                                    oldthis->left->v.func->thisptr = exp;
                                    oldthis = ths;
                                }
                                else
                                {
                                    if (!IsLValue(oldthis->left->v.func->returnEXP))
                                    {
                                        EXPRESSION* exp2 = oldthis->left->v.func->returnEXP;
                                        while (exp2->left)
                                            exp2 = exp2->left;
                                        if (exp2->type == ExpressionNode::auto_)
                                        {
                                            exp2->v.sp->sb->dest = nullptr;
                                        }
                                    }
                                    auto ths = oldthis->left->v.func->returnEXP;
                                    oldthis->left->v.func->returnEXP = exp;
                                    oldthis->v.t.thisptr = exp;
                                    oldthis = ths;
                                }
                            }
                            if (exp1->type == ExpressionNode::comma_)
                            {
                                // from constexpr
                                constructed = true;
                                std::stack<EXPRESSION*> stk;
                                stk.push(exp1);
                                while (!stk.empty())
                                {
                                    auto top = stk.top();
                                    stk.pop();
                                    if (top->left)
                                    {
                                        if (top->left == oldthis)
                                            top->left = exp;
                                        else
                                            stk.push(top->left);
                                    }
                                    if (top->right)
                                    {
                                        if (top->right == oldthis)
                                            top->right = exp;
                                        else
                                            stk.push(top->right);
                                    }
                                }
                            }
                            return exp1;
                        }, false, base);
                    if (tp1->IsAutoType())
                        tp1 = itype;
                    if (!tp1 || !tp1->BaseType()->CompatibleType(itype->BaseType()))
                    {
                        if (deduceTemplate)
                        {
                                auto spx = tp1->BaseType()->sp, spy = itype->BaseType()->sp;
                            if (spx->sb->maintemplate)
                                spx = spx->tp->sp->sb->maintemplate;
                            if (spy->sb->maintemplate)
                                spy = spy->tp->sp->sb->maintemplate;
                            if (spx != spy)
                            {
                                errorstr(ERR_CANNOT_DEDUCE_TEMPLATE, spy->name);
                            }
                            exp = exp1;
                            itype = base->tp = tp1;
                        }
                        else
                        {
                            SYMBOL* sym = nullptr;
                            if (itype->IsStructured())
                                sym = getUserConversion(F_CONVERSION | F_WITHCONS, itype, tp1, nullptr, nullptr, nullptr, nullptr, nullptr, false);
                            if (sym && !sym->sb->isExplicit)
                            {
                                funcparams->arguments = argumentListFactory.CreateList();
                                auto arg = Allocate<Argument>();
                                funcparams->arguments->push_back(arg);
                                arg->tp = tp1;
                                arg->exp = exp1;

                            }
                            else
                            {
                                error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                                errskim(&lex, skim_semi);
                                return lex;
                            }
                        }
                    }
                    else
                    {
                        exp = exp1;
                        itype = tp1;
                    }
                }
                else
                {
                    implicit = true;
                    if (MATCHKW(lex, Keyword::begin_))
                    {
                        lex = getArgs(lex, funcsp, funcparams, Keyword::end_, true, 0);
                        if (funcparams->arguments && funcparams->arguments->size())
                        {
                            if (funcparams->arguments->front()->nested)
                            {
                                for (auto a : *funcparams->arguments)
                                    a->initializer_list = true;
                            }
                            else
                            {
                                auto p = Allocate<Argument>();
                                p->nested = funcparams->arguments;
                                funcparams->arguments = argumentListFactory.CreateList();
                                funcparams->arguments->push_back(p);
                            }
                        }
                        else
                        {
                            auto p = Allocate<Argument>();
                            p->nested = argumentListFactory.CreateList();
                            funcparams->arguments = argumentListFactory.CreateList();
                            funcparams->arguments->push_back(p);
                        }
                        maybeConversion = false;
                    }
                    else
                    {
                        // shortcut for conversion from single expression
                        EXPRESSION* exp1 = nullptr;
                        Type* tp1 = nullptr;
                        lex = init_expression(lex, funcsp, itype, &tp1, &exp1, itype, false,
                                              [&exp, itype, &constructed](EXPRESSION* exp1, Type* tp1) {
                                                  if (exp1->type == ExpressionNode::thisref_ && exp1->left->type == ExpressionNode::callsite_)
                                                  {
                                                      if (exp1->left->v.func->returnEXP)
                                                      {
                                                          if (tp1->IsStructured() && itype->SameType(tp1))
                                                          {
                                                              if (!tp1->BaseType()->sp->sb->templateLevel || SameTemplate(itype, tp1))
                                                              {
                                                                  exp1->left->v.func->returnSP->sb->destructed = true;
                                                                  exp1->left->v.func->returnEXP = exp;
                                                                  exp1->v.t.thisptr = exp;
                                                                  int offs = 0;
                                                                  if (exp1->left->v.func->sp->sb->constexpression)
                                                                  {
                                                                      auto xx = relptr(exp1->left->v.func->returnEXP, offs);
                                                                      if (xx->type == ExpressionNode::auto_ && xx->v.sp->sb->anonymous)
                                                                          xx->v.sp->sb->constexpression = true;
                                                                  }
                                                                  constructed = true;
                                                                  exp = exp1;
                                                              }
                                                          }
                                                      }
                                                  }
                                                  return exp1;
                                              }, false, base);
                        funcparams->arguments = argumentListFactory.CreateList();
                        auto arg = Allocate<Argument>();
                        funcparams->arguments->push_back(arg);
                        arg->tp = tp1;
                        arg->exp = exp1;
                        int offset = 0;
                        auto exp2 = relptr(exp1, offset);
                        if (exp1->type == ExpressionNode::thisref_ && itype->SameType(tp1) && !exp1->left->v.func->returnEXP)
                            tryelide = true;
                    }
                }
            }
            else if (MATCHKW(lex, Keyword::openpa_) || MATCHKW(lex, Keyword::begin_))
            {
                if (!(flags & _F_EXPLICIT) && MATCHKW(lex, Keyword::begin_))
                    implicit = true;
                bool isbegin = MATCHKW(lex, Keyword::begin_);
                // conversion constructor params
                lex = getArgs(lex, funcsp, funcparams, MATCHKW(lex, Keyword::openpa_) ? Keyword::closepa_ : Keyword::end_, true, 0);
                if (isbegin)
                {
                    if (funcparams->arguments)
                        for (auto a : *funcparams->arguments)
                            a->initializer_list = true;
                }
                if (deduceTemplate)
                {
                    CTADLookup(funcsp, &exp, &itype, funcparams, 0);
                    base->tp = itype;
                    constructed = true;
                }
            }
            else {
                if (deduceTemplate)
                {
                    SpecializationError(itype->BaseType()->sp);
                }

                if (flags & _F_NESTEDINIT)
                {
                    // shortcut for conversion from single expression
                    EXPRESSION* exp1 = nullptr;
                    Type* tp1 = nullptr;
                    lex = init_expression(lex, funcsp, nullptr, &tp1, &exp1, itype, false, false, base);
                    funcparams->arguments = argumentListFactory.CreateList();
                    auto arg = Allocate<Argument>();
                    funcparams->arguments->push_back(arg);
                    arg->tp = tp1;
                    arg->exp = exp1;
                }
                else
                {
                    // default constructor without param list
                }
            }
            
            if (!constructed)
            {
                bool toContinue = true;
                if (toContinue && (flags & _F_ASSIGNINIT))
                {
                    toContinue = !CallConstructor(&ctype, &exp, funcparams, false, nullptr, false, maybeConversion, implicit, false,
                                                  isList ? _F_INITLIST : 0, true, true);
                }
                if (toContinue)
                    CallConstructor(&ctype, &exp, funcparams, false, nullptr, true, maybeConversion, implicit, false,
                                    isList ? _F_INITLIST : 0, false, true);
                // copy constructor elision
                if (tryelide && exp->type == ExpressionNode::thisref_ && exp->left->v.func->thistp && itype->SameType(exp->left->v.func->thistp) && exp->left->v.func->sp->sb->isConstructor && matchesCopy(exp->left->v.func->sp, false))
                {
                     exp = funcparams->arguments->back()->exp;
                }
                else if (funcparams->sp)  // may be an error
                    PromoteConstructorArgs(funcparams->sp, funcparams);
            }
            if (sc != StorageClass::auto_ && sc != StorageClass::localstatic_ && sc != StorageClass::parameter_ &&
                sc != StorageClass::member_ && sc != StorageClass::mutable_ && !arrayMember && !base->sb->attribs.inheritable.isInline)
            {
                it = nullptr;
                InsertInitializer(&it, itype, exp, offset, true);
                insertDynamicInitializer(base, it);
            }
            else
            {
                it = *init;
                if (exp->type == ExpressionNode::comma_)
                {
                    // constexpr support...
                    auto exp1 = exp;
                    auto its = itype->BaseType()->syms->begin();
                    bool written = false;
                    while (exp1->type == ExpressionNode::comma_)
                    {
                        if (exp1->left->type == ExpressionNode::assign_ && exp1->left->left->left->type == ExpressionNode::structadd_)
                        {
                            while (its != itype->BaseType()->syms->end() && !ismemberdata(*its)) ++its;
                            if (its != itype->BaseType()->syms->end())
                            {
                                int ofs = exp1->left->left->left->right->v.i;
                                auto exp2 = exp1->left->right;
                                InsertInitializer(&it, (*its)->tp, exp2, ofs, false);
                                written = true;
                            }
                        }
                        exp1 = exp1->right;
                    }
                    if (!written)
                    {
                        InsertInitializer(&it, itype, exp, offset, true);
                    }
                }
                else if (exp->type == ExpressionNode::cvarpointer_)
                {
                    for (auto its : *itype->syms)
                    {
                        if (ismemberdata(its))
                        {
                            InsertInitializer(&it, its->tp, exp->v.constexprData.data[its->sb->offset], its->sb->offset, false);
                        }
                    }
                }
                else
                {
                    InsertInitializer(&it, itype, exp, offset, true);
                }
                *init = it;
            }
            exp = baseexp;
            if (sc != StorageClass::auto_ && sc != StorageClass::parameter_ && sc != StorageClass::member_ &&
                sc != StorageClass::mutable_ && sc != StorageClass::localstatic_ && !arrayMember &&
                !base->sb->attribs.inheritable.isInline)
            {
                it = nullptr;
                CallDestructor(itype->BaseType()->sp, nullptr, &exp, nullptr, true, false, false, true);
                InsertInitializer(&it, itype, exp, offset, true);
                insertDynamicDestructor(base, it);
            }
            else if (dest)
            {
                it = *dest;
                CallDestructor(itype->BaseType()->sp, nullptr, &exp, nullptr, true, false, false, true);
                InsertInitializer(&it, itype, exp, offset, true);
                *dest = it;
            }
        }
        return lex;
    }
    else if (!Optimizer::cparams.prm_cplusplus && !MATCHKW(lex, Keyword::begin_) && !itype->msil && !itype->array)
    {
        EXPRESSION* exp = base ? getThisNode(base) : nullptr;
        Type* tp = nullptr;
        lex = expression_no_comma(lex, funcsp, nullptr, &tp, &exp, nullptr, 0);
        if (!tp)
        {
            error(ERR_EXPRESSION_SYNTAX);
        }
        else
        {
            if (!itype->CompatibleType(tp))
            {
                error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
            }
            else
            {
                std::list<Initializer*>* it = nullptr;
                switch (exp->type)
                {
                    case ExpressionNode::global_:
                    case ExpressionNode::auto_:
                    case ExpressionNode::threadlocal_:
                    case ExpressionNode::absolute_:
                        if (exp->v.sp->sb->init)
                        {
                            for (auto it2 : *exp->v.sp->sb->init)
                            {
                                if (it2->exp)
                                {
                                    auto xx = it2->exp;
                                    if (exp->type != ExpressionNode::auto_ || !exp->v.sp->sb->anonymous)
                                        while (xx->type == ExpressionNode::comma_ && xx->left->type == ExpressionNode::assign_)
                                            xx = xx->right;
                                    InsertInitializer(&it, it2->basetp, xx, it2->offset + offset, it2->noassign);
                                }
                            }
                        }
                        break;
                    default: 
                    {
                        InsertInitializer(&it, itype, exp, offset, false);
                        break;
                    }
                }
                if (it)
                {
                    if (sc != StorageClass::auto_ && sc != StorageClass::localstatic_ && sc != StorageClass::parameter_ &&
                        sc != StorageClass::member_ && sc != StorageClass::mutable_ && !arrayMember &&
                        !base->sb->attribs.inheritable.isInline)
                    {
                        insertDynamicInitializer(base, it);
                    }
                    else if (!*init)
                    {
                        *init = it;
                    }
                    else
                    {
                        (*init)->insert((*init)->begin(), it->begin(), it->end());
                    }
                }
            }
        }
        return lex;
    }
    if (itype->IsArray() && itype->msil && lex && !MATCHKW(lex, Keyword::begin_))
    {
        EXPRESSION* exp = nullptr;
        Type* tp = nullptr;
        lex = expression(lex, funcsp, nullptr, &tp, &exp, 0);
        if (!tp)
        {
            error(ERR_EXPRESSION_SYNTAX);
        }
        else
        {
            std::list<Initializer*>* it = nullptr;
            if (!itype->CompatibleType(tp))
            {
                error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
            }
            exp = MakeExpression(ExpressionNode::assign_, MakeExpression(ExpressionNode::l_object_, getThisNode(base)), exp);
            exp->left->v.tp = itype;
            InsertInitializer(&it, itype, exp, offset, true);
            if (sc != StorageClass::auto_ && sc != StorageClass::localstatic_ && sc != StorageClass::parameter_ &&
                sc != StorageClass::member_ && sc != StorageClass::mutable_ && !arrayMember &&
                !base->sb->attribs.inheritable.isInline)
            {
                insertDynamicInitializer(base, it);
            }
            else
            {
                *init = it;
            }
        }
        return lex;
    }
    // if we get here it is an array or a trivial structure
    else if (!lex || MATCHKW(lex, Keyword::begin_) || !str_candidate(lex, itype))
    {
        if (Optimizer::cparams.prm_cplusplus && !MATCHKW(lex, Keyword::begin_))
        {
            Type* tp1 = nullptr;
            EXPRESSION* exp1 = nullptr;
            if (Optimizer::cparams.prm_cplusplus && !assn && itype->IsStructured() && MATCHKW(lex, Keyword::openpa_))
            {
                bool doTrivial = false;
                std::list<Initializer*>* it = nullptr;
                if (itype->BaseType()->sp->sb->trivialCons)
                {
                    lex = getsym();
                    doTrivial = MATCHKW(lex, Keyword::begin_);
                    if (!doTrivial)
                        lex = backupsym();
                    else
                        lex = getsym();
                }
                if (doTrivial)
                {
                    // construction of trivial structure via initializer-list...
                    needend = true;
                    it = InitializeSimpleAggregate(lex, itype, needend, offset, funcsp, sc, base, dest, false, deduceTemplate, flags);
                    if (!needkw(&lex, Keyword::closepa_))
                        errskim(&lex, skim_closepa);
                }
                else
                {
                    // conversion constructor params
                    CallSite* funcparams = Allocate<CallSite>();
                    lex = getArgs(lex, funcsp, funcparams, Keyword::closepa_, true, 0);
                    if (funcparams->arguments && funcparams->arguments->size() > 1)
                        error(ERR_EXPRESSION_SYNTAX);
                    else if (funcparams->arguments && funcparams->arguments->size() &&
                             !itype->CompatibleType(funcparams->arguments->front()->tp))
                        error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                    else
                    {
                        Type* ttp = itype;
                        if (itype->IsConst())
                        {
                            ttp = Type::MakeType(BasicType::const_, ttp);
                        }
                        if (itype->IsVolatile())
                        {
                            ttp = Type::MakeType(BasicType::volatile_, ttp);
                        }
                        ttp->UpdateRootTypes();
                        tp1 = itype;
                        if (funcparams->arguments && funcparams->arguments->size() && !funcparams->arguments->front()->tp->IsRef())
                        {
                            funcparams->arguments->front()->tp = funcparams->arguments->front()->tp->CopyType();
                            funcparams->arguments->front()->tp->lref = true;
                            funcparams->arguments->front()->tp->rref = false;
                        }
                        funcparams->thistp = ttp;
                        exp1 = baseexp;
                        CallConstructor(&tp1, &exp1, funcparams, false, nullptr, true, false, false, false, 0, false, true);
                        PromoteConstructorArgs(funcparams->sp, funcparams);
                        InsertInitializer(&it, itype, exp1, offset, true);
                    }
                }
                if (sc != StorageClass::auto_ && sc != StorageClass::localstatic_ && sc != StorageClass::parameter_ && sc != StorageClass::member_ && sc != StorageClass::mutable_ && !arrayMember &&
                    !base->sb->attribs.inheritable.isInline)
                {
                    insertDynamicInitializer(base, it);
                }
                else
                {
                    *init = it;
                }
            }
            else
            {
                lex = expression_no_comma(lex, funcsp, nullptr, &tp1, &exp1, nullptr, 0);
                tp1->InstantiateDeferred();
                if (!tp1)
                    error(ERR_EXPRESSION_SYNTAX);
                else if (!itype->CompatibleType(tp1) && !SameTemplate(itype, tp1))
                {
                    bool toErr = true;
                    if (tp1->IsStructured())
                    {
                        auto sym = lookupSpecificCast(tp1->BaseType()->sp, itype);
                        if (sym)
                        {
                            toErr = false;
                            auto exp2 = MakeExpression(Allocate<CallSite>());
                            exp2->v.func->sp = sym;
                            exp2->v.func->functp = sym->tp;
                            exp2->v.func->fcall = MakeExpression(ExpressionNode::pc_, sym);
                            exp2->v.func->ascall = true;
                            exp2->v.func->thisptr = exp1;
                            exp2->v.func->thistp = Type::MakeType(BasicType::pointer_, tp1);
                            if (sym->tp->BaseType()->btp->IsStructured() || sym->tp->BaseType()->btp->IsBitInt())
                            {
                                if (sym->tp->BaseType()->btp->BaseType()->sp->sb->structuredAliasType)
                                {
                                    auto bn = baseexp;
                                    Dereference(sym->tp->BaseType()->btp->BaseType()->sp->sb->structuredAliasType, &bn);
                                    exp2 = MakeExpression(ExpressionNode::assign_, bn, exp2);
                                }
                                else
                                {
                                    exp2->v.func->returnSP = sym->tp->BaseType()->btp->BaseType()->sp;
                                    exp2->v.func->returnEXP = baseexp;
                                }
                            }
                            exp1 = exp2;
                        }
                        else if (itype->IsStructured() && classRefCount(itype->BaseType()->sp, tp1->BaseType()->sp) == 1)
                        {
                            toErr = false;
                            EXPRESSION* v = Allocate<EXPRESSION>();
                            v->type = ExpressionNode::c_i_;
                            v = baseClassOffset(tp1->BaseType()->sp, itype->BaseType()->sp, v);
                            exp1 = MakeExpression(ExpressionNode::add_, exp1, v);
                            Type* ctype = itype; 
                            auto exp2 = baseexp;
                            callConstructorParam(&ctype, &exp2, ctype, exp1, true, false, false, false, false);
                            exp1 = exp2;
                        }
                    }
                    if (toErr)
                    {
                        errorConversionOrCast(true, tp1, itype);                        
                    }
                }
                if (exp1)
                {
                    std::list<Initializer*>* it = nullptr;
                    InsertInitializer(&it, itype, exp1, offset, false);
                    if (sc != StorageClass::auto_ && sc != StorageClass::localstatic_ && sc != StorageClass::parameter_ && sc != StorageClass::member_ && sc != StorageClass::mutable_ && !arrayMember &&
                        !base->sb->attribs.inheritable.isInline)
                    {
                        insertDynamicInitializer(base, it);
                    }
                    else
                    {
                        *init = it;
                    }
                }
            }
            return lex;
        }
        else if (needkw(&lex, Keyword::begin_))
        {
            needend = true;
        }
    }
    if (needend && MATCHKW(lex, Keyword::end_))
    {
        // empty braces
        lex = getsym();
        allocate_desc(itype, offset, &desc, &cache);
        desc->stopgap = true;
        unwrap_desc(&desc, &cache, &data);
        free_desc(&desc, &cache);
        sort_aggregate_initializers(data);
        *init = data;
    }
    else
    {
        // if we get here, initialize a simple aggregate...
        *init = InitializeSimpleAggregate(lex, itype, needend, offset, funcsp, sc, base, dest, false, deduceTemplate, flags);
    }
    // have to fill in unused array elements with C++ constructors
    // this doesn't play well with the designator stuff but doesn't matter in C++
    if (Optimizer::cparams.prm_cplusplus && itype->IsArray() && !arrayMember)
    {
        Type* btp = itype;
        while (btp->IsArray())
            btp = btp->BaseType()->btp;
        btp = btp->BaseType();
        if (btp->IsStructured() && !btp->sp->sb->trivialCons)
        {
            int s = (btp->size);
            std::list<Initializer*>::iterator test = (*init)->begin();
            std::list<Initializer*>* first = nullptr;
            int last = 0, i;
            for (; test!= (*init)->end()  || last < itype->size;)
            {
                if ((test != (*init)->end() && last < (*test)->offset) || (test == (*init)->end() && last < itype->size))
                {
                    int n = (test != (*init)->end() ? (*test)->offset - last : itype->size - last) / s;
                    EXPRESSION* sz = nullptr;
                    Type *ctype = btp, *tn = btp;
                    EXPRESSION* exp;
                    if (last)
                        exp = MakeExpression(ExpressionNode::add_, getThisNode(base), MakeIntExpression(ExpressionNode::c_i_, last));
                    else
                        exp = getThisNode(base);
                    if (n > 1)
                    {
                        sz = MakeIntExpression(ExpressionNode::c_i_, n);
                        tn = Type::MakeType(BasicType::pointer_, btp);
                        tn->array = true;
                        tn->size = n * s;
                        tn->esize = sz;
                    }
                    CallConstructor(&ctype, &exp, nullptr, true, sz, true, false, false, false, false, false, true);
                    InsertInitializer(&first, tn, exp, last, true);
                    last += n * s;
                }
                if (test != (*init)->end())
                {
                    auto il1 = test;
                    ++il1;
                    if (!first)
                        first = initListFactory.CreateList();
                    first->insert(first->end(), test, il1);
                    ++test;
                    last += s;
                }
            }
            if (sc != StorageClass::auto_ && sc != StorageClass::localstatic_ && sc != StorageClass::parameter_ &&
                sc != StorageClass::member_ && sc != StorageClass::mutable_ && !base->sb->attribs.inheritable.isInline &&
                !base->sb->attribs.inheritable.isInline)
            {
                *init = nullptr;
                insertDynamicInitializer(base, first);
            }
            else
            {
                *init = first;
            }
            first = nullptr;
            int n = itype->size / s;
            EXPRESSION* exp;
            EXPRESSION* sz = nullptr;
            Type* tn = btp;
            exp = getThisNode(base);
            if (n > 1)
            {
                sz = MakeIntExpression(ExpressionNode::c_i_, n);
                tn = Type::MakeType(BasicType::pointer_, btp);
                tn->array = true;
                tn->size = n * s;
                tn->esize = sz;
            }
            if (sc != StorageClass::auto_ && sc != StorageClass::parameter_ && sc != StorageClass::member_ &&
                sc != StorageClass::mutable_ && !base->sb->attribs.inheritable.isInline)
            {
                CallDestructor(btp->sp, nullptr, &exp, sz, true, false, false, true);
                InsertInitializer(&first, tn, exp, last, false);
                insertDynamicDestructor(base, first);
                *dest = nullptr;
            }
            else if (dest)
            {
                CallDestructor(btp->sp, nullptr, &exp, sz, true, false, false, true);
                InsertInitializer(&first, tn, exp, last, false);
                *dest = first;
            }
        }
    }
    return lex;
}
static LexList* initialize_bit(LexList* lex, SYMBOL* funcsp, int offset, StorageClass sc, Type* itype, std::list<Initializer*>** init)
{
    (void)funcsp;
    (void)offset;
    (void)sc;
    (void)init;
    (void)itype;
    error(ERR_BIT_NO_INITIAL_VALUE);
    errskim(&lex, skim_comma);
    return lex;
}
void ReplaceVarRef(EXPRESSION** exp, SYMBOL* name, SYMBOL* newName)
{
    std::stack<EXPRESSION**> stk;
    stk.push(exp);
    while (!stk.empty())
    {
        exp = stk.top();
        stk.pop();
        if ((*exp)->left)
            stk.push(&(*exp)->left);
        if ((*exp)->right)
            stk.push(&(*exp)->right);
        switch ((*exp)->type)
        {
            case ExpressionNode::threadlocal_:
            case ExpressionNode::auto_:
            case ExpressionNode::global_: {
                if (name == (*exp)->v.sp)
                {
                    (*exp)->v.sp = newName;
                }
            }
                break;
            case ExpressionNode::thisref_:
            {
                stk.push(&(*exp)->left->v.func->thisptr);
            }
            break;
        }
    }
}
void ReplaceVarRef(EXPRESSION** exp, SYMBOL* name, EXPRESSION* newName)
{
    std::stack<EXPRESSION**> stk;
    stk.push(exp);
    while (!stk.empty())
    {
        exp = stk.top();
        stk.pop();
        if ((*exp)->left)
            stk.push(&(*exp)->left);
        if ((*exp)->right)
            stk.push(&(*exp)->right);
        switch ((*exp)->type)
        {
            case ExpressionNode::threadlocal_:
            case ExpressionNode::auto_:
            case ExpressionNode::global_: {
                if (name == (*exp)->v.sp)
                {
                    (*exp) = newName;
                }
            }
                break;
            case ExpressionNode::thisref_:
            {
                stk.push(&(*exp)->left->v.func->thisptr);
            }
                break;
            case ExpressionNode::callsite_:
            {
                if ((*exp)->v.func->returnEXP)
                    stk.push(&(*exp)->v.func->returnEXP);
            }
                break;
        }
    }
}
static void ReplaceLambdaInit(Type** tp, EXPRESSION** exp, SYMBOL* newName)
{  
    // get the temporary address
    auto name = *exp;
    if (name->type == ExpressionNode::comma_)
    {
        while (name->type == ExpressionNode::comma_)
            name = name->right;
        name->v.sp->sb->allocate = false;
        ReplaceVarRef(exp, name->v.sp, newName);
    }
}
static LexList* initialize_auto(LexList * lex, SYMBOL * funcsp, int offset, StorageClass sc, Type* itype,
                                std::list<Initializer*>** init,
                                std::list<Initializer*>** dest, bool arrayElem, SYMBOL* sym)
{
    Type* tp;
    EXPRESSION* exp;
    bool needend = false;
    if (MATCHKW(lex, Keyword::begin_))
    {
        needend = true;
        lex = getsym();
    }
    if (Optimizer::cparams.prm_cplusplus && needend && MATCHKW(lex, Keyword::end_))
    {
        exp = MakeIntExpression(ExpressionNode::c_i_, 0);
        sym->tp = &stdint;  // sets type for variable
        InsertInitializer(init, sym->tp, exp, offset, false);
    }
    else
    {
        Type* tp = nullptr;
        EXPRESSION* exp;
        lex = init_expression(lex, funcsp, nullptr, &tp, &exp, itype, false, arrayElem && !needend, sym);
        if (tp->IsStructured() && tp->BaseType()->sp->sb->islambda)
        {
            // this is part of copy elision for lambda declarations
            ReplaceLambdaInit(&tp, &exp, sym);
        }
        if (!tp)
            error(ERR_EXPRESSION_SYNTAX);
        else
        {
            if (tp->IsArray())
            {
                tp = Type::MakeType(BasicType::pointer_, tp->BaseType()->btp);
            }
            if (sym->tp->IsConst() && !tp->IsConst())
            {
                tp = Type::MakeType(BasicType::const_, tp);
            }
            if (sym->tp->IsVolatile() && !tp->IsVolatile())
            {
                tp = Type::MakeType(BasicType::volatile_, tp);
            }
            sym->tp = tp;  // sets type for variable
        }
        if (Optimizer::cparams.prm_cplusplus && sym->tp->IsStructured())
        {

            std::list<Initializer *>*dest = nullptr, *it;
            EXPRESSION* expl = getThisNode(sym);
            InsertInitializer(init, sym->tp, exp, offset, false);
            if (sym->sb->storage_class != StorageClass::auto_ && sym->sb->storage_class != StorageClass::parameter_ &&
                sym->sb->storage_class != StorageClass::member_ && sym->sb->storage_class != StorageClass::mutable_ && !sym->sb->attribs.inheritable.isInline)
            {
                CallDestructor(sym->tp->BaseType()->sp, nullptr, &expl, nullptr, true, false, false, true);
                InsertInitializer(&dest, sym->tp, expl, offset, true);
                insertDynamicDestructor(sym, dest);
            }
            else if (dest)
            {
                CallDestructor(sym->tp->BaseType()->sp, nullptr, &expl, nullptr, true, false, false, true);
                InsertInitializer(&dest, sym->tp, expl, offset, true);
                sym->sb->dest = dest;
            }
        }
        else
        {
            InsertInitializer(init, sym->tp, exp, offset, false);
        }
    }
    if (needend)
    {
        if (!needkw(&lex, Keyword::end_))
        {
            errskim(&lex, skim_end);
            skip(&lex, Keyword::end_);
        }
    }
    return lex;
}
/* for structured types, if the outer level is a structured type we get into
 *initialize_aggregate.  otherwise there can't be any structued types.
 * initialize_aggregate only recurses here if a non-aggregate type needs
 * initialization...  for aggregate types it completely handles all initialization
 * for the aggregate and any sub-aggregates with a single call of the function
 */
LexList* initType(LexList* lex, SYMBOL* funcsp, int offset, StorageClass sc, std::list<Initializer*>** init, std::list<Initializer*>** dest, Type* itype,
                  SYMBOL* sym, bool arrayMember, bool deduceTemplate, int flags)
{
    Type* tp;
    tp = itype->BaseType();
    if (tp->type == BasicType::templateselector_)
    {
        SYMBOL* ts = (*tp->sp->sb->templateSelector)[1].sp;
        auto find = (*tp->sp->sb->templateSelector).begin();
        auto finde = (*tp->sp->sb->templateSelector).end();
        SYMBOL* sym = nullptr;
        if ((*tp->sp->sb->templateSelector)[1].isDeclType)
        {
            Type* tp1 = TemplateLookupTypeFromDeclType((*tp->sp->sb->templateSelector)[1].tp);
            if (tp1 && tp1->IsStructured())
                sym = tp1->BaseType()->sp;
        }
        else if ((*tp->sp->sb->templateSelector)[1].isTemplate)
        {
            if (ts->tp->type == BasicType::templateparam_ && ts->tp->templateParam->second->byTemplate.val == nullptr)
            {
                lex = getsym();
                errskim(&lex, skim_end);
                needkw(&lex, Keyword::end_);
                return lex;
            }
            if (!(*tp->sp->sb->templateSelector)[1].sp->sb->instantiated && !(*tp->sp->sb->templateSelector)[1].templateParams)
            {
                std::list<TEMPLATEPARAMPAIR>* current = (*tp->sp->sb->templateSelector)[1].templateParams;
                sym = GetClassTemplate(ts, current, false);
            }
            else
            {
                sym = (*tp->sp->sb->templateSelector)[1].sp;
            }
            tp = nullptr;
        }
        else if (ts->tp->BaseType()->templateParam->second->type == TplType::typename_)
        {
            tp = ts->tp->BaseType()->templateParam->second->byClass.val;
            if (!tp)
            {
                if (definingTemplate)
                {
                    lex = getsym();
                    errskim(&lex, skim_closepa);
                }
                return lex;
            }
            sym = tp->sp;
        }
        else if (ts->tp->BaseType()->templateParam->second->type == TplType::delete_)
        {
            std::list<TEMPLATEPARAMPAIR>* args = ts->tp->BaseType()->templateParam->second->byDeferred.args;
            sym = tp->templateParam->first;
            sym = TemplateClassInstantiateInternal(sym, args, true);
        }
        if (sym)
        {
            sym->tp->InstantiateDeferred();
            sym = sym->tp->BaseType()->sp;
            for (++find, ++find ; find != finde && sym; ++ find)
            {
                SYMBOL* spo = sym;
                if (!spo->tp->IsStructured())
                    break;

                sym = search(spo->tp->syms, find->name);
                if (!sym)
                {
                    sym = classdata(find->name, spo, nullptr, false, false);
                    if (sym == (SYMBOL*)-1)
                        sym = nullptr;
                    if (sym && find->isTemplate)
                    {
                        auto tn = Type::MakeType(sym, find->templateParams);
                        tn->InstantiateDeferred();
                        if (!tn->IsDeferred())
                            sym = tn->sp;
                        else
                            sym = nullptr;
                    }
                }
            }
            if (find == finde && sym && istype(sym))
                tp = sym->tp->BaseType();
            else
                tp = nullptr;
        }
        else
            tp = nullptr;
    }
    if (!tp)
    {
        if (MATCHKW(lex, Keyword::begin_))
        {
            lex = getsym();
            errskim(&lex, skim_end);
            skip(&lex, Keyword::end_);
        }
        if (!definingTemplate && !(flags & _F_TEMPLATEARGEXPANSION))
        {
            if (itype->BaseType()->type != BasicType::templateselector_)
            {
                errortype(ERR_CANNOT_INITIALIZE, itype->BaseType(), nullptr);
            }
            else
            {
                auto&& tpl = (*itype->BaseType()->sp->sb->templateSelector)[1];
                if (unpackingTemplate || tpl.sp->name[0] != '_'  || tpl.sp->name[2] != 'm' || strcmp(tpl.sp->name, "__make_integer_seq_cls") != 0)
                {
                    errortype(ERR_CANNOT_INITIALIZE, itype->BaseType(), nullptr);
                }
                else
                {
                    // make a dummy templateselector node so the make_integer_seq can be processed...
                    auto exp = MakeExpression(ExpressionNode::templateselector_);
                    exp->v.templateSelector = itype->BaseType()->sp->sb->templateSelector;
                    InsertInitializer(init, itype, exp, 0, true);
                }
            }
        }
        return lex;
    }
    if (deduceTemplate && tp->type == BasicType::templatedeferredtype_)
    {
        tp = tp->sp->tp;
    }
    switch (tp->type)
    {
        case BasicType::aggregate_:
            return lex;
        case BasicType::bool_:
            return initialize_bool_type(lex, funcsp, offset, sc, tp, init, arrayMember, sym);
        case BasicType::inative_:
        case BasicType::unative_:
        case BasicType::char_:
        case BasicType::unsigned_char_:
        case BasicType::signed_char_:
        case BasicType::short_:
        case BasicType::unsigned_short_:
        case BasicType::int_:
        case BasicType::unsigned_:
        case BasicType::char8_t_:
        case BasicType::char16_t_:
        case BasicType::char32_t_:
        case BasicType::long_:
        case BasicType::unsigned_long_:
        case BasicType::long_long_:
        case BasicType::unsigned_long_long_:
        case BasicType::float_:
        case BasicType::double_:
        case BasicType::long_double_:
        case BasicType::float__imaginary_:
        case BasicType::double__imaginary_:
        case BasicType::long_double_imaginary_:
        case BasicType::float__complex_:
        case BasicType::double__complex_:
        case BasicType::long_double_complex_:
        case BasicType::enum_:
        case BasicType::templateparam_:
        case BasicType::wchar_t_:
        case BasicType::bitint_:
        case BasicType::unsigned_bitint_:
            return initialize_arithmetic_type(lex, funcsp, offset, sc, tp, init, arrayMember, sym);
        case BasicType::lref_:
        case BasicType::rref_:
            return initialize_reference_type(lex, funcsp, offset, sc, tp, init, flags, sym);
        case BasicType::pointer_:
            // promote char * to char [] for initialization from string (when constexpr)
            if (!tp->array && sym && sym->sb->constexpression && tp->BaseType()->btp->type != BasicType::pointer_)
            {
                bool found = false;
                if (MATCHKW(lex, Keyword::begin_))
                {
                    lex = getsym();
                    found = lex->data->type == LexType::l_astr_;
                    lex = backupsym();
                }
                else
                {
                    found = lex->data->type == LexType::l_astr_;
                }
                if (found)
                {
                    tp->array = true;
                    tp->size = 0;
                }
            }
            if (tp->array)
            {
                if (tp->vla)
                {
                    error(ERR_VLA_NO_INIT);
                    errskim(&lex, skim_semi);
                    return lex;
                }
                else
                {
                    return initialize_aggregate_type(lex, funcsp, sym, offset, sc, tp, init, dest, arrayMember, deduceTemplate, flags);
                }
            }
            else
            {
                return initialize_pointer_type(lex, funcsp, offset, sc, tp, init, arrayMember, sym);
            }
        case BasicType::memberptr_:
            return initialize_memberptr(lex, funcsp, offset, sc, tp, init, arrayMember, sym);
        case BasicType::bit_:
            return initialize_bit(lex, funcsp, offset, sc, tp, init);
        case BasicType::auto_:
            return initialize_auto(lex, funcsp, offset, sc, tp, init, dest, arrayMember, sym);
        case BasicType::string_:
            return initialize___string(lex, funcsp, offset, tp, init);
        case BasicType::object_:
            return initialize___object(lex, funcsp, offset, tp, init);
        case BasicType::struct_:
        case BasicType::union_:
        case BasicType::class_:
            if (tp->syms)
            {
                if (!Optimizer::cparams.prm_cplusplus && MATCHKW(lex, Keyword::assign_) && (sc == StorageClass::auto_ || sc == StorageClass::register_))
                {
                    lex = getsym();
                    if (MATCHKW(lex, Keyword::begin_))
                    {
                        return initialize_aggregate_type(lex, funcsp, sym, offset, sc, tp, init, dest, arrayMember, deduceTemplate, flags);
                    }
                    else
                    {
                        return initialize_auto_struct(lex, funcsp, offset, tp, init);
                    }
                }
                else
                {
                    return initialize_aggregate_type(lex, funcsp, sym, offset, sc, tp, init, dest, arrayMember, deduceTemplate, flags);
                }
            }
            /* fallthrough */
        default:
            if (!definingTemplate)
                errortype(ERR_CANNOT_INITIALIZE, tp, nullptr);
            else if (MATCHKW(lex, Keyword::begin_))
            {
                lex = getsym();
                errskim(&lex, skim_end);
                skip(&lex, Keyword::end_);
            }
            else
            {
                // this assumes that any '<' is related to a template rather than an expression
                errskim(&lex, skim_comma, true);
            }
            break;
    }
    return lex;
}
static bool hasData(Type* tp)
{
    for (auto sp : *tp->BaseType()->syms )
    {
        if (sp->tp->size)
            return true;
    }
    return false;
}
bool InitVariableMatches(SYMBOL* left, SYMBOL* right)
{
    while (left && right)
    {
        if (strcmp(left->name, right->name))
            break;
        left = left->sb->parentClass;
        right = right->sb->parentClass;
    }

    return (!left && !right);
}
void RecalculateVariableTemplateInitializers(std::list<Initializer*>::iterator& ilbegin, std::list<Initializer*>::iterator& ilend, std::list<Initializer*>** out, Type* tp, int offset)
{
    if (tp->IsStructured())
    {
        int bcoffset = offset;
        SYMBOL* sym;
        AGGREGATE_DESCRIPTOR *desc = nullptr, *cache = nullptr;
        allocate_desc(tp, offset, &desc, &cache);

        if (tp->BaseType()->sp->sb->baseClasses)
            for (auto base : *tp->BaseType()->sp->sb->baseClasses)
                while (base)
                {
                    RecalculateVariableTemplateInitializers(ilbegin, ilend, out, base->cls->tp, bcoffset);
                    bcoffset += base->cls->tp->size;
                }
        offset = bcoffset;
        while (desc)
        {
            sym = *desc->it;
            if (ismember(sym))
            {
                if (InitVariableMatches(sym, (*ilbegin)->fieldsp))
                {
                    RecalculateVariableTemplateInitializers(ilbegin, ilend, out, sym->tp, offset + sym->sb->offset);
                    offset += sym->tp->size;
                }
            }
            increment_desc(&desc, &cache);
        }
    }
    else if (tp->IsArray())
    {
        if (tp->size)
        {
            int i;
            int n = tp->size / tp->btp->size;
            Type* base = tp->BaseType()->btp->BaseType();
            for (i = 0; i < n; i++)
                if (!(*ilbegin)->basetp)
                {
                    break;
                }
                else
                {
                    RecalculateVariableTemplateInitializers(ilbegin, ilend, out, base, offset);
                    offset += tp->btp->size;
                }
        }
    }
    else
    {
        auto init = Allocate<Initializer>();
        init->offset = 0;
        init->basetp = tp;
        TEMPLATEPARAM tpx = {};
        tpx.type = TplType::int_;
        tpx.byNonType.dflt = (*ilbegin)->exp;
        tpx.byNonType.tp = tp;
        std::list<TEMPLATEPARAMPAIR> aa = {{nullptr, &tpx}};
        auto bb = ResolveTemplateSelectors(nullptr, &aa, false);
        init->exp = bb->front().second->byNonType.dflt;
        (*out)->push_back(init);
        ++ilbegin;
    }
    if ((*ilbegin)->basetp == 0)
    {
        auto init = Allocate<Initializer>();
        init->offset = offset;
        (*out)->push_back(init);
    }
}
LexList* initialize(LexList* lex, SYMBOL* funcsp, SYMBOL* sym, StorageClass storage_class_in, bool asExpression, bool inTemplate,
                    bool deduceTemplate, int flags)
{
    auto sp = sym->tp->BaseType()->sp;
    if (sp && sym->sb && sym->sb->storage_class != StorageClass::typedef_ && (!definingTemplate || instantiatingTemplate))
    {
        if (sym->tp->IsDeferred())
        {
            sym->tp->InstantiateDeferred();
        }
        else
        {
            sp->tp = sp->tp->InitializeDeferred();
        }
    }
    if (sp && sp->tp->IsStructured() && sp->sb && sp->sb->attribs.uninheritable.deprecationText)
        deprecateMessage(sym->tp->BaseType()->sp);
    Type* tp;
    bool initialized = MATCHKW(lex, Keyword::assign_) || MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::openpa_);
    inittag = 0;
    browse_variable(sym);
    // MSIL property
    if (IsCompiler())
    {
        if (sym->sb->attribs.inheritable.linkage2 == Linkage::property_)
            return initialize_property(lex, funcsp, sym, storage_class_in, asExpression, flags);
    }
    inFunctionExpressionParsing = true;
    switch (sym->sb->storage_class)
    {
        case StorageClass::parameter_:
            sym->sb->assigned = true;
            break;
        case StorageClass::global_:
            initializingGlobalVar = true;
            sym->sb->assigned = true;
            sym->sb->attribs.inheritable.used = true;
            break;
        case StorageClass::external_:
            initializingGlobalVar = true;
            sym->sb->assigned = true;
            sym->sb->attribs.inheritable.used = true;
            break;
        case StorageClass::static_:
        case StorageClass::localstatic_:
            initializingGlobalVar = true;
            sym->sb->assigned = true;
            break;
        case StorageClass::auto_:
        case StorageClass::register_:
            sym->sb->allocate = true;
            break;
        case StorageClass::type_:
        case StorageClass::typedef_:
            break;
        default:
            break;
    }
    if (funcsp && funcsp->sb->attribs.inheritable.isInline && sym->sb->storage_class == StorageClass::static_)
    {
        if (Optimizer::cparams.prm_cplusplus)
        {
            sym->sb->attribs.inheritable.isInline = true;
        }
        else
        {
            errorsym(ERR_INLINE_NO_STATIC, sym);
        }
    }
    // check for attempt to use an undefined struct
    tp = sym->tp->BaseType();
    if ((tp->IsPtr() && tp->array) || tp->IsRef())
        tp = tp->BaseType()->btp->BaseType();
    if (sym->sb->storage_class != StorageClass::typedef_ && sym->sb->storage_class != StorageClass::external_ && !sym->tp->IsRef() && !sym->sb->attribs.inheritable.isInline && !tp->syms)
    {
        if (tp->IsStructured())
            tp = tp->BaseType()->sp->tp;
        int sz = sym->tp->IsArray() ? sym->tp->size / tp->size : -1;
        tp->InstantiateDeferred();
        if (tp->IsStructured())
        {
            auto tp1 = sym->tp->BaseType();
            if (tp1->IsPtr() || tp1->IsRef())
            {
                tp1->btp = tp;
                if (sz != -1)
                {
                    sz = sz * tp->size;
                    tp1 = sym->tp;
                    while (tp1 != tp1->BaseType())
                    {
                        tp1->size = sz;
                        tp1 = tp1->btp;
                    }
                    tp1->size = sz;
                }
            }
            else
            {
                sym->tp = tp = tp->sp->tp;
            }
        }
    }
    if (sym->sb->storage_class != StorageClass::typedef_ && sym->sb->storage_class != StorageClass::external_ && tp->IsStructured() && !sym->tp->IsRef() &&
        !tp->syms)
    {
        if (MATCHKW(lex, Keyword::assign_))
            errskim(&lex, skim_semi);
        if (!definingTemplate)
            errorsym(ERR_STRUCT_NOT_DEFINED, tp->sp);
    }
    // if not in a constructor, any Keyword::openpa_() will be eaten by an expression parser
    else if (MATCHKW(lex, Keyword::assign_) || (Optimizer::cparams.prm_cplusplus && (MATCHKW(lex, Keyword::openpa_) || MATCHKW(lex, Keyword::begin_))) ||
             ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions && MATCHKW(lex, Keyword::openpa_)))
    {
        std::list<Initializer*>** init;
        bool isassign = MATCHKW(lex, Keyword::assign_);
        sym->sb->assigned = true;
        if (sym->sb->init)
            errorsym(ERR_MULTIPLE_INITIALIZATION, sym);
        if (storage_class_in == StorageClass::auto_)
            if (sym->sb->storage_class == StorageClass::external_)
                error(ERR_EXTERN_NO_INIT);
        if (sym->sb->storage_class == StorageClass::typedef_)
        {
            error(ERR_TYPEDEF_NO_INIT);
        }
        else
        {
            if (sym->tp->IsAutoType() && MATCHKW(lex, Keyword::assign_))
            {
                LexList* placeholder = lex;
                Type* tp1 = nullptr;
                EXPRESSION* exp1;
                lex = getsym();
                if (!MATCHKW(lex, Keyword::begin_) && !MATCHKW(lex, Keyword::openbr_))
                {
                    bool hasOpenPa = lex && MATCHKW(lex, Keyword::openpa_);
                    lex = expression_no_check(lex, funcsp, nullptr, &tp1, &exp1, _F_TYPETEST);
                    if (tp1 && tp1->stringconst)
                    {
                        tp1 = Type::MakeType(BasicType::pointer_, Type::MakeType(BasicType::const_, tp1->btp));
                        tp1->UpdateRootTypes();
                        sym->tp = tp1;
                    }
                    else if (tp1)
                    {
                        if (tp1->IsArray())
                        {
                            tp1 = Type::MakeType(BasicType::pointer_, tp1->BaseType()->btp);
                        }
                        DeduceAuto(&sym->tp, tp1, exp1, hasOpenPa);
                        Type** tp2 = &sym->tp;
                        while ((*tp2)->IsPtr() || (*tp2)->IsRef())
                            tp2 = &(*tp2)->BaseType()->btp;
                        //                        *tp2 = tp1;
                        if ((*tp2)->IsStructured())
                        {
                            auto tp3 = tp2;
                            while (*tp3 != (*tp3)->BaseType())
                                tp3 = &(*tp3)->btp;
                            *tp3 = (*tp3)->sp->tp;
                        }

                        if (sym->sb->storage_class != StorageClass::typedef_ && sym->sb->storage_class != StorageClass::external_ &&
                            !sym->tp->IsRef() && !(*tp2)->syms)
                        {
                            (*tp2)->InstantiateDeferred();
                        }
                    }
                }
                lex = prevsym(placeholder);
            }
            if (sym->sb->storage_class == StorageClass::absolute_)
                error(ERR_ABSOLUTE_NOT_INITIALIZED);
            else if (sym->sb->storage_class == StorageClass::external_)
            {
                sym->sb->storage_class = StorageClass::global_;
                sym->sb->wasExternal = true;
            }
            {
                bool assigned = false;
                Type* t = !isassign && (Optimizer::architecture == ARCHITECTURE_MSIL) ? find_boxed_type(sym->tp) : 0;
                if (!t || !search(t->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]))
                    t = sym->tp;
                if (MATCHKW(lex, Keyword::assign_))
                {
                    if (!t->IsStructured() && !t->IsArray() && !t->IsDeferred())
                        lex = getsym(); /* past = */
                    else
                        assigned = true;
                }
                lex = initType(lex, funcsp, 0, sym->sb->storage_class, &sym->sb->init, &sym->sb->dest, t, sym, false, deduceTemplate, flags | _F_EXPLICIT);
                /* set up an Keyword::end_ tag */
                if (sym->sb->init || assigned)
                {
                    bool found = false;
                    if (sym->sb->init)
                    {
                        for (auto init : *sym->sb->init)
                        {
                            if (!init->basetp)
                            {
                                found = true;
                                break;
                            }
                        }
                    }
                    if (!found)
                        InsertInitializer(&sym->sb->init, nullptr, nullptr, t->IsAutoType() ? sym->tp->size : t->size, false);
                }
            }
        }
    }
    else if ((Optimizer::cparams.prm_cplusplus || ((Optimizer::architecture == ARCHITECTURE_MSIL) && sym->tp->IsStructured() &&
                                                   !sym->tp->BaseType()->sp->sb->trivialCons)) &&
             sym->sb->storage_class != StorageClass::typedef_ && sym->sb->storage_class != StorageClass::external_ && !asExpression)
    {
        Type* t = ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions)
                      ? find_boxed_type(sym->tp)
                      : 0;
        if (!t || !search(t->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]))
            t = sym->tp;
        if (t->IsStructured())
        {
            if (!tp->BaseType()->sp->sb->trivialCons)
            {
                // default constructor without (), or array of structures without an initialization list
                lex = initType(lex, funcsp, 0, sym->sb->storage_class, &sym->sb->init, &sym->sb->dest, t, sym, false, deduceTemplate, flags | _F_EXPLICIT);
                /* set up an Keyword::end_ tag */
                if (sym->sb->init)
                {
                    bool found = false;
                    for (auto init : *sym->sb->init)
                    {
                        if (!init->basetp)
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                        InsertInitializer(&sym->sb->init, nullptr, nullptr, t->size, false);
                }
            }
            else if (Optimizer::cparams.prm_cplusplus)
            {
                AGGREGATE_DESCRIPTOR *desc = nullptr, *cache = nullptr;
                std::list<Initializer *>*data = nullptr;
                allocate_desc(tp->BaseType(), 0, &desc, &cache);
                desc->stopgap = true;
                unwrap_desc(&desc, &cache, &data);
                free_desc(&desc, &cache);
                if (data)
                {
                    sort_aggregate_initializers(data);
                    sym->sb->init = data;
                    bool found = false;
                    for (auto init : *sym->sb->init)
                    {
                        if (!init->basetp)
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                        InsertInitializer(&sym->sb->init, nullptr, nullptr, t->size, false);
                }
            }
        }
        else if (sym->tp->IsArray())
        {
            // constructors for uninitialized array
            Type* z = sym->tp;
            if (!z)
                z = sym->tp;
            while (z->IsArray())
                z = z->BaseType()->btp;
            z = z->BaseType();
            if (z->IsStructured() && !z->sp->sb->trivialCons)  // && !sym->sb->parentClass)
            {
                std::list<Initializer *>* init = nullptr;
                int n = sym->tp->size / (z->size);
                Type* ctype = z;
                EXPRESSION* sz = n > 1 ? MakeIntExpression(ExpressionNode::c_i_, n) : nullptr;
                EXPRESSION* baseexp = getThisNode(sym);
                EXPRESSION* exp = baseexp;
                errors[ERR_NO_OVERLOAD_MATCH_FOUND].level = 0;
                bool test = CallConstructor(&ctype, &exp, nullptr, true, sz, true, false, false, false, false, false, true);
                errors[ERR_NO_OVERLOAD_MATCH_FOUND].level = CE_ERROR;
                if (test)
                {
                    InsertInitializer(&init, z, exp, 0, true);
                    if (storage_class_in != StorageClass::auto_ && storage_class_in != StorageClass::localstatic_ && storage_class_in != StorageClass::parameter_ && storage_class_in != StorageClass::member_ &&
                        storage_class_in != StorageClass::mutable_ && !sym->sb->attribs.inheritable.isInline)
                    {
                        insertDynamicInitializer(sym, init);
                    }
                    else
                    {
                        sym->sb->init = init;
                    }
                    init = nullptr;
                    exp = baseexp;
                    CallDestructor(z->sp, nullptr, &exp, sz, true, false, false, true);
                    InsertInitializer(&init, z, exp, 0, true);
                    if (storage_class_in != StorageClass::auto_ && storage_class_in != StorageClass::parameter_ && storage_class_in != StorageClass::member_ && storage_class_in != StorageClass::mutable_ &&
                        !sym->sb->attribs.inheritable.isInline)
                    {
                        insertDynamicDestructor(sym, init);
                    }
                    else
                    {
                        sym->sb->dest = init;
                    }
                    if (sym->sb->init)
                    {
                        bool found = false;
                        for (auto init : *sym->sb->init)
                        {
                            if (!init->basetp)
                            {
                                found = true;
                                break;
                            }
                        }
                        if (!found)
                            InsertInitializer(&sym->sb->init, nullptr, nullptr, sym->tp->size, false);
                    }
                }
            }
        }
    }
    switch (sym->sb->storage_class)
    {
        case StorageClass::parameter_:
            sym->sb->assigned = true;
            break;
        case StorageClass::global_:
            Optimizer::SymbolManager::Get(sym)->storage_class = Optimizer::SymbolManager::Get(sym->sb->storage_class);
            initializingGlobalVar = true;
            sym->sb->assigned = true;
            sym->sb->attribs.inheritable.used = true;
            break;
        case StorageClass::external_:
            initializingGlobalVar = true;
            sym->sb->assigned = true;
            sym->sb->attribs.inheritable.used = true;
            break;
        case StorageClass::static_:
        case StorageClass::localstatic_:
            Optimizer::SymbolManager::Get(sym)->storage_class = Optimizer::SymbolManager::Get(sym->sb->storage_class);
            initializingGlobalVar = true;
            sym->sb->assigned = true;
            break;
        case StorageClass::auto_:
        case StorageClass::register_:
            sym->sb->allocate = true;
            break;
        case StorageClass::type_:
        case StorageClass::typedef_:
            break;
        default:
            break;
    }
    switch (sym->sb->storage_class)
    {
        case StorageClass::parameter_:
            break;
        case StorageClass::global_:
            Optimizer::SymbolManager::Get(sym)->storage_class = Optimizer::SymbolManager::Get(sym->sb->storage_class);
            break;
        case StorageClass::external_:
            break;
        case StorageClass::static_:
        case StorageClass::localstatic_:
            Optimizer::SymbolManager::Get(sym)->storage_class = Optimizer::SymbolManager::Get(sym->sb->storage_class);
            break;
        case StorageClass::auto_:
        case StorageClass::register_:
            break;
        case StorageClass::type_:
        case StorageClass::typedef_:
            break;
        default:
            break;
    }

    if (Optimizer::initializeScalars && !sym->sb->anonymous && !sym->sb->init &&
        (sym->tp->IsArithmetic() || (sym->tp->IsPtr() && !sym->tp->IsArray())) && sym->sb->storage_class == StorageClass::auto_)
    {
        EXPRESSION* exp = MakeIntExpression(ExpressionNode::c_i_, 0);
        cast(sym->tp, &exp);
        optimize_for_constants(&exp);
        InsertInitializer(&sym->sb->init, sym->tp, exp, 0, false);
        sym->sb->assigned = true;
    }
    if (sym->tp->IsAutoType() && !MATCHKW(lex, Keyword::colon_))
    {
        errorsym(ERR_AUTO_NEEDS_INITIALIZATION, sym);
        sym->tp = &stdint;
    }
    tp = sym->tp;
    while (tp->IsPtr())
    {
        tp = tp->BaseType();
        if (!tp->array)
        {
            tp = sym->tp;
            break;
        }
        tp = tp->btp;
    }
    if (sym->sb->constexpression && !definingTemplate)
    {
        if (!tp->IsPtr() && !tp->IsArithmetic() && tp->BaseType()->type != BasicType::enum_ &&
            (!tp->IsStructured() /*|| !tp->BaseType()->sp->sb->trivialCons*/))
        {
            error(ERR_CONSTEXPR_SIMPLE_TYPE);
        }
        else if (!sym->sb->init)
        {
            if (sym->sb->storage_class != StorageClass::external_ && !initialized && (!sp || !sp->tp->IsStructured()))
                error(ERR_CONSTEXPR_REQUIRES_Initializer);
        }
        else if (tp->IsStructured())
        {
            for (auto it = sym->sb->init->begin(); it != sym->sb->init->end(); ++it)
            {
                auto it1 = it;
                ++it1;
                if (it1 != sym->sb->init->end() || (*it)->exp)
                    // this only checked the first item...
                    if (!IsConstantExpression((*it)->exp, false, true))
                    {
                        error(ERR_CONSTANT_EXPRESSION_EXPECTED);
                        break;
                    }
            }
        }
        else if (!tp->IsFunction())
        {
            tp = Type::MakeType(BasicType::const_, tp);
            tp->UpdateRootTypes();
            if (!IsConstantExpression(sym->sb->init->front()->exp, false, true))
            {
                sym->sb->init->front()->exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                error(ERR_CONSTANT_EXPRESSION_EXPECTED);
            }
            int oldLevel = structLevel;
            structLevel = 0;
            optimize_for_constants(&sym->sb->init->front()->exp);
            structLevel = oldLevel;
        }
        else
        {
            if (!IsConstantExpression(sym->sb->init->front()->exp, false, true))
            {
                sym->sb->init->front()->exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                error(ERR_CONSTANT_EXPRESSION_EXPECTED);
            }
        }
    }
    if (tp->IsConst())
    {
        if (!definingTemplate)
        {
            if (!sym->sb->init)
            {
                if (!asExpression)
                {
                    if (sym->sb->storage_class != StorageClass::external_ && sym->sb->storage_class != StorageClass::typedef_ &&
                        sym->sb->storage_class != StorageClass::member_ && sym->sb->storage_class != StorageClass::mutable_)
                    {
                        if (!sym->sb->assigned && (!tp->IsStructured() || !Optimizer::cparams.prm_cplusplus ||
                                                   (tp->BaseType()->sp->sb->trivialCons && hasData(tp))))
                        {
                            errorsym(ERR_CONSTANT_MUST_BE_INITIALIZED, sym);
                        }
                    }
                }
            }
            else
            {
                if (sym->sb->attribs.inheritable.linkage2 != Linkage::export_ && !sym->sb->wasExternal)
                    Optimizer::SymbolManager::Get(sym)->isinternal |= Optimizer::cparams.prm_cplusplus;
                if ((sym->sb->init->front()->exp && isintconst(sym->sb->init->front()->exp) &&
                     (sym->tp->IsInt() || sym->tp->BaseType()->type == BasicType::enum_)))
                {
                    if ((sym->sb->storage_class != StorageClass::static_ && !Optimizer::cparams.prm_cplusplus && !funcsp) ||
                        (storage_class_in == StorageClass::global_ && Optimizer::cparams.prm_cplusplus && !(flags & _F_NOCONSTGEN)))
                    {
                        insertInitSym(sym);
                    }
                    sym->sb->value.i = sym->sb->init->front()->exp->v.i;
                    sym->sb->storage_class = StorageClass::constant_;
                    Optimizer::SymbolManager::Get(sym)->i = sym->sb->value.i;
                    Optimizer::SymbolManager::Get(sym)->storage_class = Optimizer::scc_constant;
                }
            }
        }
        else if (sym->  sb->init && !inTemplate && sym->sb->init->front()->exp &&
                 (sym->sb->constexpression || sym->tp->IsInt() || sym->tp->BaseType()->type == BasicType::enum_))
        {
            if (sym->sb->storage_class != StorageClass::static_ && !Optimizer::cparams.prm_cplusplus && !funcsp)
                insertInitSym(sym);
            sym->sb->storage_class = StorageClass::constant_;
            Optimizer::SymbolManager::Get(sym)->i = sym->sb->value.i;
            Optimizer::SymbolManager::Get(sym)->storage_class = Optimizer::scc_constant;
        }
    }
    if (sym->tp->IsAtomic() && (sym->sb->storage_class == StorageClass::auto_))
    {
        if (sym->sb->init == nullptr && needsAtomicLockFromType(sym->tp))
        {
            // sets the atomic_flag portion of a locked type to zero
            sym->sb->init = initListFactory.CreateList();
            sym->sb->init->push_back(Allocate<Initializer>());
            sym->sb->init->front()->offset = sym->tp->size - ATOMIC_FLAG_SPACE;
            sym->sb->init->front()->basetp = &stdint;
            sym->sb->init->front()->exp = MakeIntExpression(ExpressionNode::c_i_, 0);
        }
    }
    if (sym->tp->IsRef() && sym->sb->storage_class != StorageClass::typedef_)
    {
        if (!sym->sb->init && sym->sb->storage_class != StorageClass::external_ && sym->sb->storage_class != StorageClass::member_ &&
            sym->sb->storage_class != StorageClass::mutable_ && !asExpression)
        {
            errorsym(ERR_REF_MUST_INITIALIZE, sym);
        }
    }
    if (Optimizer::architecture != ARCHITECTURE_MSIL && !instantiatingTemplate && !structLevel && sym->sb->storage_class == StorageClass::global_ && !sym->sb->parentClass && !sym->sb->parent && !sym->sb->attribs.inheritable.isInline && !sym->sb->init && !sym->sb->templateLevel &&
        (!Optimizer::cparams.prm_cplusplus || !sym->tp->IsStructured() || sym->sb->trivialCons) && !sym->tp->IsAtomic() && sym->sb->attribs.inheritable.linkage3 == Linkage::none_)
    {
            sym->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
            sym->sb->attribs.inheritable.isInlineData = true;
            Optimizer::SymbolManager::Get(sym)->generated = true;
            InsertInlineData(sym);
    }
    else if (sym->sb->storage_class == StorageClass::static_ || sym->sb->storage_class == StorageClass::global_ || sym->sb->storage_class == StorageClass::localstatic_)
    {
        if (instantiatingTemplate)
        {
            if (!sym->sb->parentClass ||
                (sym->sb->parentClass &&
                 allTemplateArgsSpecified(sym->sb->parentClass, sym->sb->parentClass->templateParams)))
            {
                sym->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                InsertInlineData(sym);
            }
        }
        else if (sym->sb->attribs.inheritable.isInline && !sym->tp->IsFunction() && !sym->sb->parentClass && !sym->sb->templateLevel)
        {
            // so it won't show up in the output file unless used...
            RequiresDialect::Feature(Dialect::cpp17, "Inline variables");
            sym->sb->attribs.inheritable.linkage4 = Linkage::none_;
            sym->sb->attribs.inheritable.isInlineData = true;
        }
        else
        {
            if (sym->sb->attribs.inheritable.isInlineData)
                sym->sb->didinline = true;
            SYMBOL* tmpl;
            tmpl = sym;
            while (tmpl)
            {
                if (tmpl->sb->templateLevel)
                    break;
                else
                    tmpl = tmpl->sb->parentClass;
            }
            if (!tmpl)
            {
                insertInitSym(sym);
            }
        }
    }
    if (sym->sb->init)
    {
        declareAndInitialize = true;
    }
    if (sym->tp->array && sym->tp->size)
        if (sym->sb->storage_class == StorageClass::global_ || sym->sb->storage_class == StorageClass::static_ || sym->sb->storage_class == StorageClass::localstatic_)
            Optimizer::SymbolManager::Get(sym)->tp->size = sym->tp->size;
    initializingGlobalVar = false;
    inFunctionExpressionParsing = false;
    return lex;
}
}  // namespace Parser