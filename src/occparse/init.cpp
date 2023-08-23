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
#include <cassert>
#include <climits>
#include "PreProcessor.h"
#include "init.h"
#include "ccerr.h"
#include "config.h"
#include "initbackend.h"
#include "stmt.h"
#include "template.h"
#include "declare.h"
#include "mangle.h"
#include "ildata.h"
#include "occparse.h"
#include "expr.h"
#include "help.h"
#include "OptUtils.h"
#include "cpplookup.h"
#include "exprcpp.h"
#include "constopt.h"
#include "declcons.h"
#include "lex.h"
#include "declcpp.h"
#include "beinterf.h"
#include "memory.h"
#include "inline.h"
#include "Property.h"
#include "istmt.h"
#include "types.h"
#include "browse.h"
#include "constexpr.h"
#include "symtab.h"
#include "inline.h"
#include "ListFactory.h"

 /* initializers, local... can do w/out c99 */
#define CPP_BASE_PRIO 99 // this is low prio, high prio is 98 

namespace Parser
{
bool initializingGlobalVar;
int ignore_global_init;

static DYNAMIC_INITIALIZER *dynamicInitializers, *TLSInitializers;
static DYNAMIC_INITIALIZER *dynamicDestructors, *TLSDestructors;
static Optimizer::LIST *symListHead, *symListTail;
static int inittag = 0;
static std::list<STRING*> strtab;
static SYMBOL* msilToString;
static std::list<SYMBOL*> file_level_constructors;
LEXLIST* initType(LEXLIST* lex, SYMBOL* funcsp, int offset, StorageClass sc, std::list<INITIALIZER*>** init, std::list<INITIALIZER*>** dest, TYPE* itype,
                  SYMBOL* sym, bool arrayMember, int flags);

void init_init(void)
{
    strtab.clear();
    symListHead = nullptr;
    dynamicInitializers = TLSInitializers = nullptr;
    dynamicDestructors = TLSDestructors = nullptr;
    initializingGlobalVar = false;
    strtab.clear();
    file_level_constructors.clear();
}

static SYMBOL* LookupMsilToString()
{
    if (!msilToString)
    {
        SYMBOL* sym = namespacesearch("lsmsilcrtl", globalNameSpace, false, false);
        if (sym && sym->sb->storage_class == StorageClass::namespace_)
        {
            sym = namespacesearch("CString", sym->sb->nameSpaceValues, true, false);
            if (sym && isstructured(sym->tp))
            {
                sym = search(basetype(sym->tp)->syms, "ToPointer");
                if (sym)
                {
                    for (auto sp : *sym->tp->syms)
                    {
                        if (sp->sb->storage_class == StorageClass::static_)
                        {
                            msilToString = sp;
                            break;
                        }
                    }
                }
            }
        }
        if (!msilToString)
        {
            Utils::Fatal("internal error");
        }
    }
    return msilToString;
}
EXPRESSION* ConvertToMSILString(EXPRESSION* val)
{
    val->type = ExpressionNode::c_string_;
    SYMBOL* var = LookupMsilToString();

    FUNCTIONCALL* fp = Allocate<FUNCTIONCALL>();
    fp->functp = var->tp;
    fp->sp = var;
    fp->fcall = varNode(ExpressionNode::global_, var);
    fp->arguments = initListListFactory.CreateList();
    auto arg = Allocate<INITLIST>();
    arg->exp = val;
    arg->tp = &std__string;
    fp->arguments->push_back(arg);
    fp->ascall = true;
    EXPRESSION* rv = exprNode(ExpressionNode::func_, nullptr, nullptr);
    rv->v.func = fp;
    return rv;
}
EXPRESSION* stringlit(STRING* s)
/*
 *      make s a string literal and return it's label number.
 */
{
    EXPRESSION* rv;
    if (Optimizer::cparams.prm_mergestrings)
    {
        for (auto lp : strtab)
        {
            int i;
            if (s->size == lp->size && s->strtype == lp->strtype)
            {
                /* but it won't get in here if s and lp are the same, but
                 * resulted from different concatenation sequences
                 * this whole behavior of using strings over is undefined
                 * in the standard...
                 */
                for (i = 0; i < s->size && i < lp->size; i++)
                {
                    if (lp->pointers[i]->count != s->pointers[i]->count ||
                        Optimizer::wchart_cmp(lp->pointers[i]->str, s->pointers[i]->str, s->pointers[i]->count))
                        break;
                }
                if (i >= s->size)
                {
                    rv = intNode(ExpressionNode::labcon_, lp->label);
                    rv->string = s;
                    rv->size = MakeType(BasicType::struct_);
                    rv->altdata = intNode(ExpressionNode::c_i_, s->strtype);
                    lp->refCount++;
                    if (Optimizer::msilstrings)
                    {
                        rv = ConvertToMSILString(rv);
                    }
                    return rv;
                }
            }
        }
    }
    s->label = Optimizer::nextLabel++;
    strtab.push_back(s);
    rv = intNode(ExpressionNode::labcon_, s->label);
    rv->string = s;
    rv->size = MakeType(BasicType::struct_);
    rv->altdata = intNode(ExpressionNode::c_i_, s->strtype);
    s->refCount++;
    if (Optimizer::msilstrings)
    {
        rv = ConvertToMSILString(rv);
    }
    return rv;
}

int genstring(STRING* str)
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
            // if (str->strtype == l_astr && Optimizer::chosenAssembler->gen->gen_string)
            // Optimizer::chosenAssembler->gen->gen_string(p, n);
            while (n--)
            {
                switch (str->strtype)
                {
                    case l_wstr:
                        Optimizer::genwchar_t(*p++);
                        break;
                    case l_ustr:
                        Optimizer::genuint16(*p++);
                        break;
                    case l_Ustr:
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
            case l_wstr:
                Optimizer::genwchar_t(0);
                size *= getSize(BasicType::wchar_t_);
                break;
            case l_ustr:
                Optimizer::genuint16(0);
                size *= 2;
                break;
            case l_Ustr:
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
            case l_wstr:
            case l_ustr:
                if (n % 2)
                    Optimizer::genbyte(0);
                break;
            case l_Ustr: {
                int remainder = 4 - n % 4;
                if (remainder != 4)
                    for (int i = 0; i < remainder; i++)
                        Optimizer::genbyte(0);
            }
            break;
        }
        Optimizer::put_string_label(s->label, s->strtype);
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
static int dumpBits(std::list<INITIALIZER*>::iterator &it)
{
    if (IsCompiler())
    {
        int offset = (*it)->offset;
        long long resolver = 0;
        TYPE* base = basetype((*it)->basetp);
        do
        {
            long long i = 0;
            TYPE* tp = basetype((*it)->basetp);
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
        if (isatomic((*it)->basetp) && needsAtomicLockFromType((*it)->basetp))
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
void insertDynamicInitializer(SYMBOL* sym, std::list<INITIALIZER*>* init)
{
    if (!ignore_global_init && !templateNestingCount)
    {
        DYNAMIC_INITIALIZER* di = Allocate<DYNAMIC_INITIALIZER>();
        di->sp = sym;
        di->init = init;
        if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
        {
            di->next = TLSInitializers;
            TLSInitializers = di;
        }
        else
        {
            di->next = dynamicInitializers;
            dynamicInitializers = di;
        }
    }
}
static void insertTLSInitializer(SYMBOL* sym, std::list<INITIALIZER*>* init)
{
    DYNAMIC_INITIALIZER* di = Allocate<DYNAMIC_INITIALIZER>();
    di->sp = sym;
    di->init = init;
    di->next = TLSInitializers;
    TLSInitializers = di;
    //	genstorage(sym->tp->size);
}
void insertDynamicDestructor(SYMBOL* sym, std::list<INITIALIZER*>* init)
{
    if (!ignore_global_init && !templateNestingCount)
    {
        DYNAMIC_INITIALIZER* di = Allocate<DYNAMIC_INITIALIZER>();
        di->sp = sym;
        di->init = init;
        if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
        {
            di->next = TLSDestructors;
            TLSDestructors = di;
        }
        else
        {
            di->next = dynamicDestructors;
            dynamicDestructors = di;
        }
    }
}
static void callDynamic(const char* name, int startupType, int index, std::list<STATEMENT*>* st)
{
    if (IsCompiler())
    {
        if (st->size())
        {
            STATEMENT* stbegin = stmtNode(nullptr, emptyBlockdata, StatementNode::dbgblock_);
            stbegin->label = 1;
            STATEMENT* stend = stmtNode(nullptr, emptyBlockdata, StatementNode::dbgblock_);
            stend->label = 0;
            st->push_front(stbegin);
            st->push_back(stend);
            char fullName[512];
            Optimizer::my_sprintf(fullName, "%s_%d", name, index);
            SYMBOL* funcsp;
            TYPE* tp = MakeType(BasicType::ifunc_, MakeType(BasicType::void_));
            tp->syms = symbols.CreateSymbolTable();
            funcsp = makeUniqueID((Optimizer::architecture == ARCHITECTURE_MSIL) ? StorageClass::global_ : StorageClass::static_, tp, nullptr, fullName);
            funcsp->sb->inlineFunc.stmt = stmtListFactory.CreateList();
            funcsp->sb->inlineFunc.stmt->push_back(stmtNode(nullptr, emptyBlockdata, StatementNode::block_));
            funcsp->sb->inlineFunc.stmt->front()->lower = st;
            tp->sp = funcsp;
            SetLinkerNames(funcsp, Linkage::none_);
            startlab = Optimizer::nextLabel++;
            retlab = Optimizer::nextLabel++;
            genfunc(funcsp, !(Optimizer::architecture == ARCHITECTURE_MSIL));
            startlab = retlab = 0;

            if (!(Optimizer::chosenAssembler->arch->denyopts & DO_NOADDRESSINIT))
            {
                if (startupType == STARTUP_TYPE_STARTUP)
                    Optimizer::startupseg();
                else
                    Optimizer::rundownseg();
                Optimizer::gensrref(Optimizer::SymbolManager::Get(funcsp), CPP_BASE_PRIO + preProcessor->GetCppPrio(), startupType);
            }
        }
    }
}
static void dumpDynamicInitializers(void)
{
    if (IsCompiler() && dynamicInitializers)
    {
        AllocateLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
        int index = 0;
        int counter = 0;
        std::list<STATEMENT*> st;
        codeLabel = INT_MIN;
        while (dynamicInitializers)
        {
            if (!isstructured(dynamicInitializers->sp->tp) || basetype(dynamicInitializers->sp->tp)->sp->sb->trivialCons ||
                !isintconst(dynamicInitializers->init->front()->exp))
            {
                EXPRESSION *exp = nullptr, **next = &exp, *exp1;
                std::list<STATEMENT*> stmt;
                int i = 0;
                exp = convertInitToExpression(dynamicInitializers->init ? dynamicInitializers->init->front()->basetp
                                                                        : dynamicInitializers->sp->tp,
                                              dynamicInitializers->sp, nullptr, nullptr, dynamicInitializers->init, nullptr, false);

                while (*next && (*next)->type == ExpressionNode::void_)
                {
                    counter++;
                    if (++i == 10)
                    {
                        exp1 = *next;
                        *next = intNode(ExpressionNode::c_i_, 0);  // fill in the final right with a value
                        stmt.push_back(stmtNode(nullptr, emptyBlockdata, StatementNode::expr_));
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
                    stmt.push_back(stmtNode(nullptr, emptyBlockdata, StatementNode::expr_));
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
            dynamicInitializers = dynamicInitializers->next;
        }
        callDynamic("__DYNAMIC_STARTUP__", STARTUP_TYPE_STARTUP, index++, &st);
        FreeLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
    }
}
static void dumpTLSInitializers(void)
{
    if (IsCompiler())
    {
        if (TLSInitializers)
        {
            AllocateLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);            
            std::list<STATEMENT*> st;
            SYMBOL* funcsp;
            TYPE* tp = MakeType(BasicType::ifunc_, MakeType(BasicType::void_));
            tp->syms = symbols.CreateSymbolTable();
            funcsp = makeUniqueID((Optimizer::architecture == ARCHITECTURE_MSIL) ? StorageClass::global_ : StorageClass::static_, tp, nullptr,
                                  "__TLS_DYNAMIC_STARTUP__");
            tp->sp = funcsp;
            SetLinkerNames(funcsp, Linkage::none_);
            codeLabel = INT_MIN;
            while (TLSInitializers)
            {
                EXPRESSION* exp = varNode(ExpressionNode::threadlocal_, TLSInitializers->sp);
                STATEMENT* stmt = stmtNode(nullptr, emptyBlockdata, StatementNode::expr_);
                exp = convertInitToExpression(TLSInitializers->init->front()->basetp, TLSInitializers->sp, nullptr, nullptr,
                                              TLSInitializers->init, exp, false);
                optimize_for_constants(&exp);
                stmt->select = exp;
                st.push_back(stmt);
                TLSInitializers = TLSInitializers->next;
            }
            funcsp->sb->inlineFunc.stmt = stmtListFactory.CreateList();
            funcsp->sb->inlineFunc.stmt->push_front(stmtNode(nullptr, emptyBlockdata, StatementNode::block_));
            funcsp->sb->inlineFunc.stmt->front()->lower = &st;
            startlab = Optimizer::nextLabel++;
            retlab = Optimizer::nextLabel++;
            genfunc(funcsp, true);
            startlab = retlab = 0;
            Optimizer::tlsstartupseg();
            Optimizer::gensrref(Optimizer::SymbolManager::Get(funcsp), 32, STARTUP_TYPE_TLS_STARTUP);
            FreeLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
        }
    }
}
static void dumpDynamicDestructors(void)
{
    if (IsCompiler() && dynamicDestructors)
    {
        AllocateLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
        int index = 0;
        int counter = 0;
        std::list<STATEMENT*> st;
        codeLabel = INT_MIN;
        while (dynamicDestructors)
        {
            EXPRESSION* exp = convertInitToExpression(dynamicDestructors->init->front()->basetp, dynamicDestructors->sp, nullptr, nullptr,
                                                      dynamicDestructors->init, nullptr, true);
            auto stmt = stmtNode(nullptr, emptyBlockdata, StatementNode::expr_);
            optimize_for_constants(&exp);
            stmt->select = exp;
            st.push_back(stmt);
            dynamicDestructors = dynamicDestructors->next;
            if (++counter % 1500 == 0)
            {
                callDynamic("__DYNAMIC_RUNDOWN__", STARTUP_TYPE_RUNDOWN, index++, &st);
                st.clear();
            }
        }
        callDynamic("__DYNAMIC_RUNDOWN__", STARTUP_TYPE_RUNDOWN, index++, &st);
        FreeLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
    }
}
static void dumpTLSDestructors(void)
{
    if (IsCompiler())
    {
        if (TLSDestructors)
        {
            AllocateLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
            std::list<STATEMENT*> st;
            SYMBOL* funcsp;
            TYPE* tp = MakeType(BasicType::ifunc_, MakeType(BasicType::void_));
            tp->syms = symbols.CreateSymbolTable();
            funcsp = makeUniqueID((Optimizer::architecture == ARCHITECTURE_MSIL) ? StorageClass::global_ : StorageClass::static_, tp, nullptr,
                                  "__TLS_DYNAMIC_RUNDOWN__");
            tp->sp = funcsp;
            SetLinkerNames(funcsp, Linkage::none_);
            codeLabel = INT_MIN;
            while (TLSDestructors)
            {
                EXPRESSION* exp = varNode(ExpressionNode::threadlocal_, TLSDestructors->sp);
                exp = convertInitToExpression(TLSDestructors->init->front()->basetp, TLSDestructors->sp, nullptr, nullptr,
                                              TLSDestructors->init, exp, true);
                auto stmt = stmtNode(nullptr, emptyBlockdata, StatementNode::expr_);
                optimize_for_constants(&exp);
                st.push_back(stmt);
                TLSDestructors = TLSDestructors->next;
            }

            funcsp->sb->inlineFunc.stmt = stmtListFactory.CreateList();
            funcsp->sb->inlineFunc.stmt->push_back(stmtNode(nullptr, emptyBlockdata, StatementNode::block_));
            funcsp->sb->inlineFunc.stmt->front()->lower = &st;
            startlab = Optimizer::nextLabel++;
            retlab = Optimizer::nextLabel++;
            genfunc(funcsp, true);
            startlab = retlab = 0;
            Optimizer::tlsrundownseg();
            Optimizer::gensrref(Optimizer::SymbolManager::Get(funcsp), 32, STARTUP_TYPE_TLS_RUNDOWN);
            FreeLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
        }
    }
}
int dumpMemberPtr(SYMBOL* sym, TYPE* membertp, bool make_label)
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
            if (isfunction(membertp->btp))
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
            exp = baseClassOffset(sym->sb->parentClass, basetype(membertp)->sp, &expx);
            optimize_for_constants(&exp);
            if (isfunction(sym->tp))
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
                while (castvalue(in))
                    in = in->left;
                if (*exp)
                    diag("GetStructData - multiple sym");
                *exp = in;
            }
            break;
    }
}
int dumpInit(SYMBOL* sym, INITIALIZER* init)
{
    if (IsCompiler())
    {
        TYPE* tp = basetype(init->basetp);
        int rv;
        long long i;
        FPF f, im;
        if (tp->type == BasicType::templateparam_)
            tp = tp->templateParam->second->byClass.val;
        if (isstructured(tp))
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
        else
        {
            EXPRESSION* exp = init->exp;
            while (castvalue(exp))
                exp = exp->left;
            while (exp->type == ExpressionNode::void_ && exp->right)
                exp = exp->right;
            if (exp->type == ExpressionNode::func_ && !exp->v.func->ascall)
                exp = exp->v.func->fcall;
            if (!IsConstantExpression(exp, false, false))
            {
                if (Optimizer::cparams.prm_cplusplus)
                {
                    if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
                    {
                        std::list<INITIALIZER*>* temp = initListFactory.CreateList();
                        temp->push_back(init);
                        insertTLSInitializer(sym, temp);
                    }
                    else if (sym->sb->storage_class != StorageClass::localstatic_)
                    {
                        std::list<INITIALIZER*>* temp = initListFactory.CreateList();
                        temp->push_back(init);
                        insertDynamicInitializer(sym, temp);
                    }
                    return 0;
                }
                else
                    diag("dumpsym: unknown constant type");
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
                            std::list<INITIALIZER*>* temp = initListFactory.CreateList();
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
                                std::list<INITIALIZER*>* temp = initListFactory.CreateList();
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
                                std::list<INITIALIZER*>* temp = initListFactory.CreateList();
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
bool IsConstWithArr(TYPE* tp)
{
    tp = basetype(tp);
    while (tp->array)
    {
        tp = tp->btp;
        if (!ispointer(tp) || !basetype(tp)->array)
            break;
        else
            tp = basetype(tp);
    }
    return isconst(tp);
}
void dumpInitGroup(SYMBOL* sym, TYPE* tp)
{
    if (IsCompiler())
    {

        if (sym->sb->init || (isarray(sym->tp) && sym->tp->msil))
        {
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
            {
                insertDynamicInitializer(sym, sym->sb->init);
            }
            else
            {
                if (basetype(sym->tp)->array || isstructured(tp))
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
                        if ((*it)->basetp && basetype((*it)->basetp)->hasbits)
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
            Optimizer::genstorage(basetype(tp)->size);
        if (isatomic(tp) && needsAtomicLockFromType(tp))
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
                TYPE* tp = sym->tp;
                TYPE* stp = tp;
                int al;
                while (isarray(stp))
                    stp = basetype(stp)->btp;
                if ((IsConstWithArr(sym->tp) && !isvolatile(sym->tp)) || sym->sb->storage_class == StorageClass::constant_)
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
                    al = getAlign(StorageClass::global_, basetype(tp));

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
                    if (isarray(tp))
                    {
                        TYPE* tp1 = tp;
                        while (isarray(tp1))
                            tp1 = basetype(tp1)->btp;
                        tp1 = basetype(tp1);
                        if (isstructured(tp1) && !tp1->sp->sb->trivialCons)
                        {
                            Optimizer::genint(basetype(tp)->size);
                            *sizep += getSize(BasicType::int_);
                        }
                    }
                }
                sym->sb->offset = *sizep;
                *sizep += basetype(tp)->size;
                Optimizer::gen_strlab(Optimizer::SymbolManager::Get(sym));
                if (sym->sb->storage_class == StorageClass::constant_)
                    Optimizer::put_label(sym->sb->label);
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
INITIALIZER* initInsert(std::list<INITIALIZER*>** pos, std::list<INITIALIZER*>::iterator it, TYPE* tp, EXPRESSION* exp, int offset, bool noassign)
{
    if (!*pos)
        *pos = initListFactory.CreateList();
    INITIALIZER* pos1 = Allocate<INITIALIZER>();

    pos1->basetp = tp;
    pos1->exp = exp;
    pos1->offset = offset;
    pos1->tag = inittag++;
    pos1->noassign = noassign;
    (*pos)->insert(it, pos1);
    return pos1;
}
INITIALIZER* initInsert(std::list<INITIALIZER*>** pos, TYPE* tp, EXPRESSION* exp, int offset, bool noassign)
{
    if (!*pos)
        *pos = initListFactory.CreateList();
    return initInsert(pos, (**pos).end(), tp, exp, offset, noassign);
}
static LEXLIST* init_expression(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** expr, bool commaallowed,
                                std::function<EXPRESSION*(EXPRESSION*, TYPE*)> modify)
{
    LEXLIST* start = lex;
    if (commaallowed)
        lex = expression(lex, funcsp, atp, tp, expr, 0);
    else
        lex = expression_no_comma(lex, funcsp, atp, tp, expr, nullptr, 0);
    *expr = modify(*expr, *tp);
    if (*tp && (isvoid(*tp) || ismsil(*tp)))
        error(ERR_NOT_AN_ALLOWED_TYPE);
    optimize_for_constants(expr);
    ConstExprPatch(expr);
    if (*tp)
    {
        if (*expr && (*expr)->type == ExpressionNode::func_ && (*expr)->v.func->sp->sb->parentClass && !(*expr)->v.func->ascall &&
            !(*expr)->v.func->asaddress)
        {
            for (auto sym : *basetype((*expr)->v.func->functp)->syms)
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
                std::list<INITLIST*>* temp = nullptr;
                checkPackedExpression(*expr);
                // this is going to presume that the expression involved
                // is not too long to be cached by the LEXLIST mechanism.
                expandPackedInitList(&temp, funcsp, start, *expr);
                if (!temp || !temp->size())
                {
                    *expr = intNode(ExpressionNode::c_i_, 0);
                    *tp = &stdint;
                }
                else
                {
                    if (temp->size() > 1)
                    {
                        error(ERR_TOO_MANY_INITIALIZERS);
                    }
                    *expr = temp->front()->exp;
                    *tp = temp->front()->tp;
                }
            }
        }
        else
        {
            checkUnpackedExpression(*expr);
        }
    }
    return lex;
}
static LEXLIST* init_expression(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** expr, bool commaallowed)
{
    return init_expression(lex, funcsp, atp, tp, expr, commaallowed, [](EXPRESSION* exp, TYPE* tp) { return exp; });
}
static LEXLIST* initialize_bool_type(LEXLIST* lex, SYMBOL* funcsp, int offset, StorageClass sc, TYPE* itype, std::list<INITIALIZER*>** init)
{
    TYPE* tp;
    EXPRESSION* exp;
    bool needend = false;
    if (MATCHKW(lex, Keyword::begin_))
    {
        needend = true;
        lex = getsym();
    }
    if (Optimizer::cparams.prm_cplusplus && needend && MATCHKW(lex, Keyword::end_))
    {
        exp = intNode(ExpressionNode::c_bool_, 0);
    }
    else
    {
        lex = init_expression(lex, funcsp, nullptr, &tp, &exp, false);
        if (!tp)
        {
            error(ERR_EXPRESSION_SYNTAX);
        }
        else if (itype->type != BasicType::templateparam_ && !templateNestingCount)
        {
            ResolveTemplateVariable(&tp, &exp, itype, nullptr);
            castToArithmetic(false, &tp, &exp, (Keyword) - 1, itype, true);
            if (isstructured(tp))
                error(ERR_ILL_STRUCTURE_ASSIGNMENT);
            else if ((!isarithmetic(tp) && basetype(tp)->type != BasicType::enum_ && basetype(tp)->type != BasicType::pointer_) ||
                     (sc != StorageClass::auto_ && sc != StorageClass::register_ && !isarithmeticconst(exp) && !msilConstant(exp) &&
                      !Optimizer::cparams.prm_cplusplus))
                error(ERR_CONSTANT_VALUE_EXPECTED);

            if (!comparetypes(itype, tp, true))
            {
                cast(itype, &exp);
                optimize_for_constants(&exp);
            }
        }
    }
    initInsert(init, itype, exp, offset, false);
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
static LEXLIST* initialize_arithmetic_type(LEXLIST* lex, SYMBOL* funcsp, int offset, StorageClass sc, TYPE* itype, std::list<INITIALIZER*>** init,
                                           int flags)
{

    TYPE* tp = nullptr;
    EXPRESSION* exp = nullptr;
    bool needend = false;
    if (MATCHKW(lex, Keyword::begin_))
    {
        needend = true;
        lex = getsym();
    }
    if (Optimizer::cparams.prm_cplusplus && needend && MATCHKW(lex, Keyword::end_))
    {
        exp = intNode(ExpressionNode::c_i_, 0);
        cast(itype, &exp);
    }
    else
    {
        lex = init_expression(lex, funcsp, nullptr, &tp, &exp, false);
        if (!tp || !exp)
        {
            error(ERR_EXPRESSION_SYNTAX);
        }
        else
        {
            ResolveTemplateVariable(&tp, &exp, itype, nullptr);
            if (itype->type != BasicType::templateparam_ && tp->type != BasicType::templateselector_ && !templateNestingCount)
            {
                EXPRESSION** exp2;
                exp2 = &exp;
                while (castvalue(*exp2))
                    exp2 = &(*exp2)->left;
                if ((*exp2)->type == ExpressionNode::func_ && (*exp2)->v.func->sp->sb->storage_class == StorageClass::overloads_ &&
                    (*exp2)->v.func->sp->tp->syms->size() > 0)
                {
                    SYMBOL* sp2;
                    TYPE* tp1 = nullptr;
                    sp2 = MatchOverloadedFunction((*exp2)->v.func->sp->tp, &tp1, (*exp2)->v.func->sp, exp2, 0);
                }
                if (Optimizer::cparams.prm_cplusplus && (isarithmetic(itype) || basetype(itype)->type == BasicType::enum_) &&
                    isstructured(tp))
                {
                    castToArithmetic(false, &tp, &exp, (Keyword) - 1, itype, true);
                }
                if (isfunction(tp))
                    tp = basetype(tp)->btp;
                if (isstructured(tp))
                {
                    error(ERR_ILL_STRUCTURE_ASSIGNMENT);
                }
                else if (ispointer(tp))
                    error(ERR_NONPORTABLE_POINTER_CONVERSION);
                else if ((!isarithmetic(tp) && basetype(tp)->type != BasicType::enum_) ||
                         (sc != StorageClass::auto_ && sc != StorageClass::register_ && !isarithmeticconst(exp) && !msilConstant(exp) &&
                          !Optimizer::cparams.prm_cplusplus))
                    error(ERR_CONSTANT_VALUE_EXPECTED);
                else
                    checkscope(tp, itype);
                if (!comparetypes(itype, tp, true))
                {
                    if (Optimizer::cparams.prm_cplusplus && needend)
                        if (basetype(itype)->type < basetype(tp)->type)
                        {
                            if (isintconst(exp))
                            {

                                int val = exp->v.i;
                                switch (basetype(itype)->type)
                                {
                                    case BasicType::char_:
                                        if (val < CHAR_MIN || val > CHAR_MAX)
                                            error(ERR_INIT_NARROWING);
                                        break;
                                    case BasicType::unsigned_char_:
                                        if (val < 0 || val > UCHAR_MAX)
                                            error(ERR_INIT_NARROWING);
                                        break;
                                    case BasicType::short_:
                                        if (val < SHRT_MIN || val > SHRT_MAX)
                                            error(ERR_INIT_NARROWING);
                                        break;
                                    case BasicType::unsigned_short_:
                                        if (val < 0 || val > USHRT_MAX)
                                            error(ERR_INIT_NARROWING);
                                        break;
                                    default:
                                        error(ERR_INIT_NARROWING);
                                        break;
                                }
                            }
                            else
                            {
                                error(ERR_INIT_NARROWING);
                            }
                        }
                    cast(itype, &exp);
                    optimize_for_constants(&exp);
                }
            }
        }
    }
    initInsert(init, itype, exp, offset, false);
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
static LEXLIST* initialize_string(LEXLIST* lex, SYMBOL* funcsp, TYPE** rtype, EXPRESSION** exp)
{
    enum e_lexType tp;
    (void)funcsp;
    lex = concatStrings(lex, exp, &tp, 0);
    switch (tp)
    {
        default:
        case l_astr:
            *rtype = &stdstring;
            break;
        case l_wstr:
            *rtype = &stdwcharptr;
            break;
        case l_msilstr:
            *rtype = &std__string;
        case l_ustr:
            *rtype = &stdchar16tptr;
            break;
        case l_Ustr:
            *rtype = &stdchar32tptr;
            break;
    }
    return lex;
}
static LEXLIST* initialize_pointer_type(LEXLIST* lex, SYMBOL* funcsp, int offset, StorageClass sc, TYPE* itype, std::list<INITIALIZER*>** init)
{
    TYPE* tp = nullptr;
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
        exp = intNode(ExpressionNode::c_i_, 0);
    }
    else
    {
        if (!lex || (lex->data->type != l_astr && lex->data->type != l_wstr && lex->data->type != l_ustr &&
                     lex->data->type != l_Ustr && lex->data->type != l_msilstr))
        {
            lex = init_expression(lex, funcsp, itype, &tp, &exp, false);
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
        DeduceAuto(&itype, tp, exp);
        if (sc != StorageClass::auto_ && sc != StorageClass::register_)
        {
            EXPRESSION* exp2 = exp;
            while (exp2->type == ExpressionNode::void_ && exp2->right)
                exp2 = exp2->right;
            if (!isarithmeticconst(exp2) && !isconstaddress(exp2) && !msilConstant(exp2) && !Optimizer::cparams.prm_cplusplus)
                error(ERR_NEED_CONSTANT_OR_ADDRESS);
        }
        if (tp)
        {
            EXPRESSION** exp2;
            if (Optimizer::cparams.prm_cplusplus && isstructured(tp))
            {
                castToPointer(&tp, &exp, (Keyword) - 1, itype);
            }
            exp2 = &exp;
            while (castvalue(*exp2))
                exp2 = &(*exp2)->left;
            if ((*exp2)->type == ExpressionNode::func_ && (*exp2)->v.func->sp->sb->storage_class == StorageClass::overloads_)
            {
                TYPE* tp1 = nullptr;
                SYMBOL* sp2;
                sp2 = MatchOverloadedFunction(itype, ispointer(itype) ? &tp : &tp1, (*exp2)->v.func->sp, exp2, 0);
                if (sp2)
                {
                    if ((*exp2)->type == ExpressionNode::pc_ || ((*exp2)->type == ExpressionNode::func_ && !(*exp2)->v.func->ascall))
                    {
                        thunkForImportTable(exp2);
                    }
                }
            }
            if ((*exp2)->type == ExpressionNode::func_ && !(*exp2)->v.func->ascall)
            {
                InsertInline((*exp2)->v.func->sp);
            }
            if (tp->type == BasicType::memberptr_)
            {
                errorConversionOrCast(true, tp, itype);
            }
            if (Optimizer::cparams.prm_cplusplus && string && !isconst(basetype(itype)->btp))
                error(ERR_INVALID_CHARACTER_STRING_CONVERSION);

            if (isarray(tp) && (tp)->msil)
                error(ERR_MANAGED_OBJECT_NO_ADDRESS);
            else if (isstructured(tp))
                error(ERR_ILL_STRUCTURE_ASSIGNMENT);
            else if (!ispointer(tp) && (tp->btp && !ispointer(tp->btp)) && !isfunction(tp) && !isint(tp) &&
                     tp->type != BasicType::aggregate_)
                error(ERR_INVALID_POINTER_CONVERSION);
            else if (isfunction(tp) || tp->type == BasicType::aggregate_)
            {
                if (!isfuncptr(itype) || !comparetypes(basetype(itype)->btp, tp, true))
                {
                    if (Optimizer::cparams.prm_cplusplus)
                    {
                        if (!isvoidptr(itype) && !tp->nullptrType)
                            if (tp->type == BasicType::aggregate_)
                                errorConversionOrCast(true, tp, itype);
                    }
                    else if (!isvoidptr(tp) && !isvoidptr(itype))
                        error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                }
            }
            else if (!comparetypes(itype, tp, true))
            {
                if (ispointer(tp))
                {
                    if (Optimizer::cparams.prm_cplusplus)
                    {
                        if (!isvoidptr(itype) && !tp->nullptrType)
                            if (!ispointer(itype) || tp->type == BasicType::aggregate_ || !isstructured(basetype(tp)->btp) ||
                                !isstructured(basetype(itype)->btp) ||
                                classRefCount(basetype(basetype(itype)->btp)->sp, basetype(basetype(tp)->btp)->sp) != 1)
                                errorConversionOrCast(true, tp, itype);
                    }
                    else if (!isvoidptr(tp) && !isvoidptr(itype))
                        if (!matchingCharTypes(tp, itype))
                            error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                }
                else if (isint(tp) && isintconst(exp))
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
            if (!comparetypes(itype, tp, true) && !isint(tp))
                cast(tp, &exp);
        }
    }
    exp->structByAddress = true;
    initInsert(init, itype, exp, offset, false);
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
static LEXLIST* initialize_memberptr(LEXLIST* lex, SYMBOL* funcsp, int offset, StorageClass sc, TYPE* itype, std::list<INITIALIZER*>** init)
{
    TYPE* tp = nullptr;
    EXPRESSION* exp = nullptr;
    bool needend = false;
    if (MATCHKW(lex, Keyword::begin_))
    {
        needend = true;
        lex = getsym();
    }
    if (Optimizer::cparams.prm_cplusplus && needend && MATCHKW(lex, Keyword::end_))
    {
        exp = intNode(ExpressionNode::memberptr_, 0);  // no SP means fill it with zeros...
    }
    else
    {
        lex = init_expression(lex, funcsp, nullptr, &tp, &exp, false);
        ResolveTemplateVariable(&tp, &exp, itype, nullptr);
        if (!isconstzero(tp, exp) && exp->type != ExpressionNode::nullptr_)
        {
            EXPRESSION** exp2;
            if (Optimizer::cparams.prm_cplusplus && isstructured(tp))
            {
                castToPointer(&tp, &exp, (Keyword) - 1, itype);
            }
            exp2 = &exp;
            while (castvalue(*exp2))
                exp2 = &(*exp2)->left;
            if ((*exp2)->type == ExpressionNode::func_ && (*exp2)->v.func->sp->sb->storage_class == StorageClass::overloads_)
            {
                TYPE* tp1 = nullptr;
                if ((*exp2)->v.func->sp->sb->parentClass && !(*exp2)->v.func->asaddress)
                    error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
                funcsp = MatchOverloadedFunction(itype, &tp1, (*exp2)->v.func->sp, exp2, 0);
                if (funcsp)
                {
                    exp = varNode(ExpressionNode::memberptr_, funcsp);
                }
                if (tp1 && !comparetypes(itype->btp, tp1, true))
                {
                    errorConversionOrCast(true, tp1, itype);
                }
            }
            else if (exp->type == ExpressionNode::memberptr_)
            {
                if ((exp->v.sp->sb->parentClass != basetype(itype)->sp &&
                     exp->v.sp->sb->parentClass != basetype(itype)->sp->sb->mainsym &&
                     !sameTemplate(itype, exp->v.sp->sb->parentClass->tp)) ||
                    !comparetypes(basetype(itype)->btp, basetype(exp->v.sp->tp), true))

                    errorConversionOrCast(true, tp, itype);
            }
            else
            {
                TYPE tp4;
                if (Optimizer::cparams.prm_cplusplus && isstructured(tp))
                {
                    castToPointer(&tp, &exp, (Keyword) - 1, itype);
                }
                if (isfunction(tp)&& basetype(tp)->syms->size())
                {
                    auto sp = basetype(tp)->syms->front();
                    if (sp->sb->thisPtr)
                    {
                        tp4 = {};
                        MakeType(tp4, BasicType::memberptr_, tp);
                        tp4.sp = basetype(basetype(sp->tp)->btp)->sp;
                        tp = &tp4;
                    }
                }
                if (!comparetypes(itype, tp, true))
                {
                    errorConversionOrCast(true, tp, itype);
                }
            }
        }
        else
        {
            exp = intNode(ExpressionNode::memberptr_, 0);  // no SP means fill it with zeros...
        }
    }
    initInsert(init, itype, exp, offset, false);
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
enum ExpressionNode referenceTypeError(TYPE* tp, EXPRESSION* exp)
{
    enum ExpressionNode en = ExpressionNode::void_;
    tp = basetype(basetype(tp)->btp);
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
                return ExpressionNode::void_;
            en = ExpressionNode::l_p_;
            break;
        case BasicType::class_:
        case BasicType::struct_:
        case BasicType::union_:
        case BasicType::func_:
        case BasicType::ifunc_:
            if (exp->type == ExpressionNode::l_ref_)
                return exp->type;
            return ExpressionNode::l_p_;
        default:
            diag("ref reference error");
            break;
    }
    return en;
}
EXPRESSION* createTemporary(TYPE* tp, EXPRESSION* val)
{
    EXPRESSION* rv;
    tp = basetype(tp)->btp;
    if (tp->type == BasicType::pointer_) // to get around arrays not doing a deref...
        tp = &stdpointer;
    rv = anonymousVar(StorageClass::auto_, tp);
    if (val)
    {
        if (IsConstantExpression(val, true, true))
            rv->v.sp->sb->constexpression = true;
        EXPRESSION* rv1 = copy_expression(rv);
        deref(tp, &rv);
        cast(tp, &val);
        rv = exprNode(ExpressionNode::void_, exprNode(ExpressionNode::assign_, rv, val), rv1);
    }
    errortype(ERR_CREATE_TEMPORARY, tp, tp);
    return rv;
}
EXPRESSION* msilCreateTemporary(TYPE* tp, EXPRESSION* val)
{
    EXPRESSION* rv = anonymousVar(StorageClass::auto_, tp);
    if (val)
    {
        EXPRESSION* rv1 = copy_expression(rv);
        deref(tp, &rv);
        cast(tp, &val);
        rv = exprNode(ExpressionNode::void_, exprNode(ExpressionNode::assign_, rv, val), rv1);
    }
    errortype(ERR_CREATE_TEMPORARY, tp, tp);
    return rv;
}
static EXPRESSION* ConvertInitToRef(EXPRESSION* exp, TYPE* tp, TYPE* boundTP, StorageClass sc)
{
    if (exp->type == ExpressionNode::cond_)
    {
        exp->right->left = ConvertInitToRef(exp->right->left, tp, boundTP, sc);
        exp->right->right = ConvertInitToRef(exp->right->right, tp, boundTP, sc);
    }
    else
    {
        EXPRESSION* exp1 = exp;
        if (!templateNestingCount && (referenceTypeError(tp, exp) != exp->type || (tp->type == BasicType::rref_ && lvalue(exp))) &&
            (!isstructured(basetype(tp)->btp) || exp->type != ExpressionNode::lvalue_) && (!ispointer(basetype(tp)->btp) || exp->type != ExpressionNode::l_p_))
        {
            if (!isarithmeticconst(exp) && exp->type != ExpressionNode::thisref_ && exp->type != ExpressionNode::func_ &&
                basetype(basetype(tp)->btp)->type != BasicType::memberptr_ && !boundTP->rref && !boundTP->lref)
                errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, tp, boundTP);
            if (sc != StorageClass::parameter_ && !boundTP->rref && !boundTP->lref)
                exp = createTemporary(tp, exp);
        }
        while (castvalue(exp))
            exp = exp->left;
        if (lvalue(exp))
            exp = exp->left;
    }
    return exp;
}
static LEXLIST* initialize_reference_type(LEXLIST* lex, SYMBOL* funcsp, int offset, StorageClass sc, TYPE* itype, std::list<INITIALIZER*>** init,
                                          int flags, SYMBOL* sym)
{
    TYPE* tp;
    EXPRESSION* exp;
    bool needend = false;
    TYPE* tpi = itype;
    STRUCTSYM s;
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
        ResolveTemplateVariable(&tp, &exp, basetype(itype)->btp, nullptr);
        DeduceAuto(&itype, tp, exp);
        DeduceAuto(&sym->tp, tp, exp);
        UpdateRootTypes(itype);
        UpdateRootTypes(sym->tp);
        ConstExprPromote(exp, isconst(basetype(itype)->btp));
        if (!isref(tp) &&
            ((isconst(tp) && !isconst(basetype(itype)->btp)) || (isvolatile(tp) && !isvolatile(basetype(itype)->btp))))
            error(ERR_REF_INITIALIZATION_DISCARDS_QUALIFIERS);
        else if (basetype(basetype(itype)->btp)->type == BasicType::memberptr_)
        {
            bool ref = false;
            EXPRESSION* exp1 = exp;
            TYPE* itype1 = basetype(basetype(itype)->btp);
            if (tp->lref || tp->rref)
            {
                tp = basetype(tp)->btp;
                if (!comparetypes(itype1, tp, true))
                    errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, itype1, tp);
            }
            else
            {
                if (basetype(tp)->nullptrType || (isint(tp) && isconstzero(tp, exp)))
                {
                    int lbl = dumpMemberPtr(nullptr, itype1, true);
                    exp = intNode(ExpressionNode::labcon_, lbl);
                }
                else if (tp->type == BasicType::aggregate_)
                {
                    if (isfunction(itype1->btp))
                    {
                        SYMBOL* funcsp;
                        funcsp = MatchOverloadedFunction(itype1, &tp, tp->sp, &exp, 0);
                        if (funcsp)
                        {
                            int lbl = dumpMemberPtr(funcsp, tp, true);
                            exp = intNode(ExpressionNode::labcon_, lbl);
                        }
                        else
                        {
                            exp = intNode(ExpressionNode::c_i_, 0);
                        }
                    }
                    else
                    {
                        errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, itype1, tp);
                    }
                }
                else if (isfunction(tp))
                {
                    if (!comparetypes(itype1, tp, true))
                        errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, itype1, tp);
                    else
                    {
                        funcsp = basetype(tp->btp)->sp;
                        if (funcsp)
                        {
                            int lbl = dumpMemberPtr(funcsp, tp, true);
                            exp = intNode(ExpressionNode::labcon_, lbl);
                        }
                        else
                        {
                            exp = intNode(ExpressionNode::c_i_, 0);
                        }
                    }
                }
                else
                {
                    if (!comparetypes(itype1, tp, true))
                        errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, itype, tp);
                }
                if (exp->type == ExpressionNode::labcon_)
                {
                    if (sc != StorageClass::parameter_)
                        exp1 = createTemporary(itype1, nullptr);
                    exp = exprNode(ExpressionNode::blockassign_, exp1, exp);
                    exp->size = MakeType(BasicType::memberptr_);
                    exp->altdata = (void*)(&stdpointer);
                    exp = exprNode(ExpressionNode::void_, exp, exp1);
                }
            }
        }
        else if (((!isarithmetic(basetype(itype)->btp) && basetype(itype)->type != BasicType::enum_) ||
                  (!isarithmetic(tp) && basetype(tp)->type == BasicType::enum_)) &&
                 !comparetypes(itype, tp, true) &&
                 (!isstructured(itype->btp) || !isstructured(tp) || classRefCount(basetype(itype->btp)->sp, basetype(tp)->sp) != 1))
        {
            if (isstructured(itype->btp))
            {
                EXPRESSION* ths = anonymousVar(StorageClass::auto_, tp);
                EXPRESSION* paramexp = exp;
                TYPE* ctype = basetype(itype->btp);
                exp = ths;
                callConstructorParam(&ctype, &exp, tp, paramexp, true, false, false, false, true);
            }
            else
            {
                errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, itype->btp, tp);
            }
        }
        else if (!isref(tp) && !isstructured(tp) && !isfunction(tp) && (!ispointer(tp) || !tp->array))
        {
            exp = ConvertInitToRef(exp, itype, tp, sc);
        }
        if (itype->type == BasicType::rref_ && isstructured(itype->btp) && exp->type != ExpressionNode::lvalue_ && exp->type != ExpressionNode::l_ref_)
        {
            EXPRESSION* expx = exp;
            bool lref = false;
            if (expx->type == ExpressionNode::thisref_)
                expx = expx->left;
            if (expx->type == ExpressionNode::func_)
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
    initInsert(init, itype, exp, offset, false);
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
    TYPE* tp;
    
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
    if (isstructured(desc->tp))
        return desc->it == desc->ite;
    else
        return desc->tp->size && desc->reloffset >= desc->tp->size;
}
static void unwrap_desc(AGGREGATE_DESCRIPTOR** descin, AGGREGATE_DESCRIPTOR** cache, std::list<INITIALIZER*>** dest)
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
                            initInsert(dest, list->basetp, list->exp, list->offset + (*descin)->offset + (*descin)->reloffset,
                                       false);
                            if (ismember(sym))
                            {
                                (*dest)->back()->fieldsp = sym;
                                (*dest)->back()->fieldoffs = intNode(ExpressionNode::c_i_, (*descin)->offset);
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
static void allocate_desc(TYPE* tp, int offset, AGGREGATE_DESCRIPTOR** descin, AGGREGATE_DESCRIPTOR** cache)
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
    if (isstructured(tp))
    {
        desc->it = basetype(tp)->syms->begin();
        desc->ite = basetype(tp)->syms->end();
        if (basetype(tp)->sp->sb->baseClasses)
        {
            auto bc = basetype(tp)->sp->sb->baseClasses;
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
static int str_candidate(LEXLIST* lex, TYPE* tp)
{
    TYPE* bt;
    bt = basetype(tp);
    if (bt->type == BasicType::string_)
        return true;
    if (bt->type == BasicType::pointer_)
        if (lex->data->type == l_astr || lex->data->type == l_wstr || lex->data->type == l_ustr || lex->data->type == l_Ustr)
        {
            bt = basetype(bt->btp);
            if (bt->type == BasicType::short_ || bt->type == BasicType::unsigned_short_ || bt->type == BasicType::wchar_t_ || bt->type == BasicType::char_ ||
                bt->type == BasicType::unsigned_char_ || bt->type == BasicType::signed_char_ || bt->type == BasicType::char16_t_ || bt->type == BasicType::char32_t_)
                return true;
        }
    return false;
}
static bool designator(LEXLIST** lex, SYMBOL* funcsp, AGGREGATE_DESCRIPTOR** desc, AGGREGATE_DESCRIPTOR** cache)
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
                TYPE* tp = nullptr;
                EXPRESSION* enode = nullptr;
                int index;
                *lex = getsym();
                *lex = init_expression(*lex, funcsp, nullptr, &tp, &enode, false);
                needkw(lex, Keyword::closebr_);
                if (!tp)
                    error(ERR_EXPRESSION_SYNTAX);
                else if (!isint(tp))
                    error(ERR_NEED_INTEGER_TYPE);
                else if (!isarithmeticconst(enode) && !msilConstant(enode) && !Optimizer::cparams.prm_cplusplus)
                    error(ERR_CONSTANT_VALUE_EXPECTED);
                else if (isstructured((*desc)->tp) || !basetype((*desc)->tp)->array)
                    error(ERR_ARRAY_EXPECTED);
                else
                {
                    index = enode->v.i;
                    tp = basetype((*desc)->tp);
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
                    if ((isarray(tp) && MATCHKW(*lex, Keyword::openbr_)) || (isstructured(tp) && MATCHKW(*lex, Keyword::dot_)))
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
                    if (isstructured((*desc)->tp))
                    {
                        SymbolTable<SYMBOL>::iterator it;
                        for (it = basetype((*desc)->tp)->syms->begin(); it != basetype((*desc)->tp)->syms->end() && strcmp((*it)->name, (*lex)->data->value.s.a) != 0; ++it);
                        *lex = getsym();
                        if (it != basetype((*desc)->tp)->syms->end())
                        {
                            SYMBOL* sym = *it;
                            TYPE* tp = sym->tp;
                            (*desc)->reloffset = sym->sb->offset;
                            (*desc)->it = it;
                            (*desc)->ite = basetype((*desc)->tp)->syms->end();
                            if ((isarray(tp) && MATCHKW(*lex, Keyword::openbr_)) || (isstructured(tp) && MATCHKW(*lex, Keyword::dot_)))
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
                        errorNotMember(basetype((*desc)->tp)->sp, nullptr, (*lex)->data->value.s.a);
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
        if (isstructured((*desc)->tp))
        {
            int offset = (*desc)->reloffset + (*desc)->offset;
            if (isunion((*desc)->tp))
                (*desc)->it = (*desc)->ite;
            else if (!basetype((*desc)->tp)->sp->sb->hasUserCons)
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
            TYPE* tp = (*desc)->tp;
            (*desc)->reloffset += basetype(tp)->btp->size;
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
static bool ascomp(const INITIALIZER* left, const INITIALIZER* right)
{
    int lofs = (left->offset << 7) + basetype(left->basetp)->startbit;
    int rofs = (right->offset << 7) + basetype(right->basetp)->startbit;
    if (lofs < rofs)
        return true;
    return false;
}
static void sort_aggregate_initializers(std::list<INITIALIZER*>* data)
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
    std::list<INITIALIZER*>::iterator itlast;
    /* trim duplicates - highest tag value indicated for a given offset/startbit pair wins */
    for (auto it = data->begin(); it != data->end();)
    {
        int lofs = ((*it)->offset << 7) + basetype((*it)->basetp)->startbit;
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
        if (!isstructured(cache->tp))
        {
            if (cache->tp->size == 0)
            {
                int size = cache->max;
                int base = basetype(cache->tp)->btp->size;
                TYPE* temp = cache->tp;
                if (base == 0)
                    base = 1;
                size = size + base - 1;
                size = (size / base) * base;
                while (temp && temp->size == 0)
                {
                    temp->size = size;
                    temp->esize = intNode(ExpressionNode::c_i_, size / base);
                    temp = temp->btp;
                }
            }
        }
        cache = cache->next;
    }
}
static LEXLIST* read_strings(LEXLIST* lex, std::list<INITIALIZER*>** next, AGGREGATE_DESCRIPTOR** desc)
{
    bool nothingWritten = true;
    TYPE* tp = basetype((*desc)->tp);
    TYPE* btp = basetype(tp->btp);
    int max = tp->size / btp->size;
    int j;
    EXPRESSION* expr;
    STRING* string = nullptr;
    int index = 0;
    int i;
    /* this assumes the sizeof(short) & sizeof(wchar_t) < 16 */
    if (max == 0)
        max = INT_MAX / 16;
    lex = concatStringsInternal(lex, &string, 0);
    switch (string->strtype)
    {
        case l_astr:
            if (btp->type != BasicType::char_ && btp->type != BasicType::unsigned_char_ && btp->type != BasicType::signed_char_)
                error(ERR_STRING_TYPE_MISMATCH_IN_INITIALIZATION);
            break;
        case l_wstr:
            if (btp->type != BasicType::wchar_t_ && btp->type != BasicType::short_ && btp->type != BasicType::unsigned_short_)
                error(ERR_STRING_TYPE_MISMATCH_IN_INITIALIZATION);
            break;
        case l_msilstr:
            if (tp->type != BasicType::string_ && btp->type != BasicType::wchar_t_ && btp->type != BasicType::short_ && btp->type != BasicType::unsigned_short_)
                error(ERR_STRING_TYPE_MISMATCH_IN_INITIALIZATION);
            break;
        case l_ustr:
            if (btp->type != BasicType::char16_t_)
                error(ERR_STRING_TYPE_MISMATCH_IN_INITIALIZATION);
            break;
        case l_Ustr:
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
            error(ERR_TOO_MANY_INITIALIZERS);
        }
        else
        {
            int i;
            for (i = 0; i < len; i++)
            {
                EXPRESSION* exp = intNode(ExpressionNode::c_i_, string->pointers[j]->str[i]);
                initInsert(next, btp, exp, (*desc)->offset + (*desc)->reloffset, false); /* nullptr=no initializer */
                (*desc)->reloffset += btp->size;
                index++;
                nothingWritten = false;
            }
        }
    }
    if (max == INT_MAX / 16)
    {
        EXPRESSION* exp = intNode(ExpressionNode::c_i_, 0);

        initInsert(next, btp, exp, (*desc)->offset + (*desc)->reloffset, false); /* nullptr=no initializer */
        max = (*desc)->reloffset / btp->size;
        nothingWritten = false;
    }
    if (nothingWritten)
    {
        EXPRESSION* exp = intNode(ExpressionNode::c_i_, 0);

        initInsert(next, btp, exp, (*desc)->offset + (*desc)->reloffset, false); /* nullptr=no initializer */
    }
    for (i = (*desc)->reloffset / btp->size; i < max; i++)
    {
        (*desc)->reloffset += btp->size;
    }
    if ((*desc)->reloffset < max * btp->size)
    {
        EXPRESSION* exp = intNode(ExpressionNode::c_i_, 0);

        initInsert(next, btp, exp, (*desc)->offset + (*desc)->reloffset, false); /* nullptr=no initializer */
        (*desc)->reloffset += btp->size;
    }
    return lex;
}
static TYPE* nexttp(AGGREGATE_DESCRIPTOR* desc)
{
    TYPE* rv;
    if (isstructured(desc->tp))
    {
        if (!Optimizer::cparams.prm_cplusplus || !basetype(desc->tp)->sp->sb->hasUserCons)
        {
            for (; desc->it != desc->ite && (istype(*desc->it) || (*desc->it)->tp->type == BasicType::aggregate_); ++desc->it)
                ;
            if (desc->it == desc->ite)
                return nullptr;
            rv = (*desc->it)->tp;
        }
        else
        {
            rv = basetype(desc->tp);
        }
    }
    else
        rv = basetype(desc->tp)->btp;
    return rv;
}
static LEXLIST* initialize___object(LEXLIST* lex, SYMBOL* funcsp, int offset, TYPE* itype, std::list<INITIALIZER*>** init)
{
    EXPRESSION* expr = nullptr;
    TYPE* tp = nullptr;
    lex = expression_assign(lex, funcsp, nullptr, &tp, &expr, nullptr, 0);
    ConstExprPatch(&expr);
    if (!tp || !lex)
    {
        error(ERR_EXPRESSION_SYNTAX);
    }
    else if (isarithmetic(tp) || (ispointer(tp) && (!isarray(tp) || !tp->msil)))
    {
        cast(tp, &expr);
    }
    initInsert(init, itype, expr, offset, false);
    return lex;
}
static LEXLIST* initialize___string(LEXLIST* lex, SYMBOL* funcsp, int offset, TYPE* itype, std::list<INITIALIZER*>** init)
{
    EXPRESSION* expr = nullptr;
    TYPE* tp = nullptr;
    lex = expression_assign(lex, funcsp, itype, &tp, &expr, nullptr, 0);
    ConstExprPatch(&expr);
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
    initInsert(init, itype, expr, offset, false);
    return lex;
}
static LEXLIST* initialize_auto_struct(LEXLIST* lex, SYMBOL* funcsp, int offset, TYPE* itype, std::list<INITIALIZER*>** init)
{
    EXPRESSION* expr = nullptr;
    TYPE* tp = nullptr;
    lex = expression_assign(lex, funcsp, nullptr, &tp, &expr, nullptr, 0);
    ConstExprPatch(&expr);
    if (!tp || !lex)
    {
        error(ERR_EXPRESSION_SYNTAX);
    }
    else if (!comparetypes(itype, tp, true))
    {
        if ((Optimizer::architecture == ARCHITECTURE_MSIL) && isstructured(itype) && basetype(itype)->sp->sb->msil &&
            (isconstzero(tp, expr) || basetype(tp)->nullptrType))
            initInsert(init, itype, expr, offset, false);
        else
            error(ERR_ILL_STRUCTURE_ASSIGNMENT);
    }
    else
    {
        initInsert(init, itype, expr, offset, false);
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
            exp = exprNode(ExpressionNode::add_, varNode(ExpressionNode::thisshim_, nullptr), intNode(ExpressionNode::c_i_, sym->sb->offset));
            break;
        case StorageClass::auto_:
        case StorageClass::parameter_:
        case StorageClass::register_: /* register variables are treated as
                           * auto variables in this compiler
                           * of course the usage restraints of the
                           * register keyword are enforced elsewhere
                           */
            exp = varNode(ExpressionNode::auto_, sym);
            break;

        case StorageClass::localstatic_:
            if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
                exp = varNode(ExpressionNode::threadlocal_, sym);
            else
                exp = varNode(ExpressionNode::global_, sym);
            break;
        case StorageClass::absolute_:
            exp = varNode(ExpressionNode::absolute_, sym);
            break;
        case StorageClass::static_:
        case StorageClass::global_:
        case StorageClass::external_:
            if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
                exp = varNode(ExpressionNode::threadlocal_, sym);
            else
                exp = varNode(ExpressionNode::global_, sym);
            break;
        default:
            diag("getThisNode: unknown storage class");
            exp = intNode(ExpressionNode::c_i_, 0);
            break;
    }
    return exp;
}

auto InitializeSimpleAggregate(LEXLIST*& lex, TYPE* itype, bool needend, int offset, SYMBOL* funcsp, StorageClass sc, SYMBOL* base,
                               std::list<INITIALIZER*>** dest, int flags)
{
    std::list<INITIALIZER*>* data = nullptr;
    AGGREGATE_DESCRIPTOR *desc = nullptr, *cache = nullptr;
    bool toomany = false;
    bool c99 = false;
    allocate_desc(itype, offset, &desc, &cache);
    desc->stopgap = true;
    while (lex)
    {
        bool gotcomma = false;
        TYPE* tp2;
        c99 |= designator(&lex, funcsp, &desc, &cache);
        tp2 = nexttp(desc);
        bool hasSome = false;
        while (tp2 && (tp2->type == BasicType::aggregate_ || isarray(tp2) ||
                       (isstructured(tp2) && (!Optimizer::cparams.prm_cplusplus || !basetype(tp2)->sp->sb->hasUserCons))))
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
                        c99 |= designator(&lex, funcsp, &desc, &cache);
                    }
                }
                else
                {
                    if (isstructured(tp2))
                    {
                        auto placeholder = lex;
                        EXPRESSION* exp = nullptr;
                        TYPE* tp = nullptr;
                        lex = expression_no_comma(lex, funcsp, nullptr, &tp, &exp, nullptr, 0);
                        lex = prevsym(placeholder);
                        if (tp && comparetypes(tp2, tp, true))
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
            lex = initType(lex, funcsp, desc->offset + desc->reloffset, sc, &data, dest, nexttp(desc), base, isarray(itype),
                           flags | _F_NESTEDINIT);
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
                    if (isarray(itype) && Optimizer::architecture == ARCHITECTURE_MSIL)
                    {
                        TYPE* btp = itype;
                        while (isarray(btp))
                            btp = btp->btp;
                        int n = desc->offset / btp->size;
                        (*it)->fieldoffs = exprNode(ExpressionNode::umul_, intNode(ExpressionNode::c_i_, n), exprNode(ExpressionNode::sizeof_, typeNode(btp), nullptr));
                    }
                    else
                    {
                        (*it)->fieldoffs = intNode(ExpressionNode::c_i_, desc->offset);
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
                if (desc->it != desc->ite && Optimizer::cparams.prm_cplusplus && isstructured(itype) &&
                    !basetype(itype)->sp->sb->trivialCons)
                {
                    while (desc->it != desc->ite)
                    {
                        if (isstructured((*desc->it)->tp) && !basetype((*desc->it)->tp)->sp->sb->trivialCons)
                        {
                            SYMBOL* fieldsp;
                            int size = data->size();
                            lex = initType(lex, funcsp, desc->offset + desc->reloffset, sc, &data, dest, nexttp(desc), base,
                                           isarray(itype), flags);
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
                                    (*it)->fieldoffs = intNode(ExpressionNode::c_i_, desc->offset);
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
    if (c99 && Optimizer::cparams.c_dialect < Dialect::c99)
        error(ERR_C99_STYLE_INITIALIZATION_USED);
    if (toomany)
        error(ERR_TOO_MANY_INITIALIZERS);
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
static LEXLIST* initialize_aggregate_type(LEXLIST * lex, SYMBOL * funcsp, SYMBOL * base, int offset, StorageClass sc, TYPE* itype,
                                          std::list<INITIALIZER*>** init, std::list<INITIALIZER*>** dest, bool arrayMember, int flags)
{
    std::list<INITIALIZER*>* data = nullptr;
    AGGREGATE_DESCRIPTOR *desc = nullptr, *cache = nullptr;
    bool needend = false;
    bool assn = false;
    bool implicit = false;
    EXPRESSION* baseexp = nullptr;
    if ((Optimizer::cparams.prm_cplusplus || (Optimizer::architecture == ARCHITECTURE_MSIL)) && isstructured(itype))
        baseexp = exprNode(ExpressionNode::add_, getThisNode(base), intNode(ExpressionNode::c_i_, offset));

    if (MATCHKW(lex, Keyword::assign_))
    {
        assn = true;
        lex = getsym();
    }
    if ((Optimizer::cparams.prm_cplusplus || ((Optimizer::architecture == ARCHITECTURE_MSIL) && !assn)) && isstructured(itype) &&
        (basetype(itype)->sp->sb->hasUserCons || (!basetype(itype)->sp->sb->trivialCons && !MATCHKW(lex, Keyword::begin_)) || arrayMember))
    {
        if ((base->sb->storage_class != StorageClass::member_ && base->sb->storage_class != StorageClass::mutable_) || MATCHKW(lex, Keyword::openpa_) || assn ||
            MATCHKW(lex, Keyword::begin_))
        {
            // initialization via constructor
            FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
            EXPRESSION* exp;
            TYPE* ctype = itype;
            std::list<INITIALIZER*>* it;
            bool maybeConversion = true;
            bool isconversion;
            bool isList = MATCHKW(lex, Keyword::begin_);
            bool constructed = false;
            exp = baseexp;
            if (assn || arrayMember)
            {
                // assignments or array members come here
                if (startOfType(lex, nullptr, false))
                {
                    TYPE* tp1 = nullptr;
                    EXPRESSION* exp1;
                    lex =
                        init_expression(lex, funcsp, nullptr, &tp1, &exp1, false, [&exp, &constructed](EXPRESSION* exp1, TYPE* tp) {
                            EXPRESSION* oldthis;
                            for (oldthis = exp1; oldthis->right && oldthis->type == ExpressionNode::void_; oldthis = oldthis->right)
                                ;
                            if (oldthis->type == ExpressionNode::thisref_)
                            {
                                constructed = true;
                                if (oldthis->left->v.func->thisptr || !oldthis->left->v.func->returnEXP)
                                {
                                    if (!lvalue(oldthis->left->v.func->thisptr))
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
                                    if (!lvalue(oldthis->left->v.func->returnEXP))
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
                            if (exp1->type == ExpressionNode::void_)
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
                        });
                    if (isautotype(tp1))
                        tp1 = itype;
                    if (!tp1 || !comparetypes(basetype(tp1), basetype(itype), true))
                    {
                        SYMBOL* sym = nullptr;
                        if (isstructured(itype))
                            sym = getUserConversion(F_CONVERSION | F_WITHCONS, itype, tp1, nullptr, nullptr, nullptr, nullptr, nullptr, false);
                        if (sym && !sym->sb->isExplicit)
                        {
                            funcparams->arguments = initListListFactory.CreateList();
                            auto arg = Allocate<INITLIST>();
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
                                auto p = Allocate<INITLIST>();
                                p->nested = funcparams->arguments;
                                funcparams->arguments = initListListFactory.CreateList();
                                funcparams->arguments->push_back(p);
                            }
                        }
                        maybeConversion = false;
                    }
                    else
                    {
                        // shortcut for conversion from single expression
                        EXPRESSION* exp1 = nullptr;
                        TYPE* tp1 = nullptr;
                        lex = init_expression(lex, funcsp, itype, &tp1, &exp1, false,
                                              [&exp, itype, &constructed](EXPRESSION* exp1, TYPE* tp1) {
                                                  if (exp1->type == ExpressionNode::thisref_ && exp1->left->type == ExpressionNode::func_)
                                                  {
                                                      if (exp1->left->v.func->returnEXP)
                                                      {
                                                          if (isstructured(tp1) && comparetypes(itype, tp1, 0))
                                                          {
                                                              if (!basetype(tp1)->sp->sb->templateLevel || sameTemplate(itype, tp1))
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
                                              });
                        funcparams->arguments = initListListFactory.CreateList();
                        auto arg = Allocate<INITLIST>();
                        funcparams->arguments->push_back(arg);
                        arg->tp = tp1;
                        arg->exp = exp1;
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
            }
            else if (flags & _F_NESTEDINIT)
            {
                // shortcut for conversion from single expression
                EXPRESSION* exp1 = nullptr;
                TYPE* tp1 = nullptr;
                lex = init_expression(lex, funcsp, nullptr, &tp1, &exp1, false);
                funcparams->arguments = initListListFactory.CreateList();
                auto arg = Allocate<INITLIST>();
                funcparams->arguments->push_back(arg);
                arg->tp = tp1;
                arg->exp = exp1;
            }
            else
            {
                // default constructor without param list
            }
            if (!constructed)
            {
                bool toContinue = true;
                if (toContinue && (flags & _F_ASSIGNINIT))
                {
                    toContinue = !callConstructor(&ctype, &exp, funcparams, false, nullptr, false, maybeConversion, implicit, false,
                                                  isList ? _F_INITLIST : 0, true, true);
                }
                if (toContinue)
                    callConstructor(&ctype, &exp, funcparams, false, nullptr, true, maybeConversion, implicit, false,
                                    isList ? _F_INITLIST : 0, false, true);
                if (funcparams->sp)  // may be an error
                    PromoteConstructorArgs(funcparams->sp, funcparams);
            }
            if (sc != StorageClass::auto_ && sc != StorageClass::localstatic_ && sc != StorageClass::parameter_ && sc != StorageClass::member_ && sc != StorageClass::mutable_ && !arrayMember)
            {
                it = nullptr;
                initInsert(&it, itype, exp, offset, true);
                insertDynamicInitializer(base, it);
            }
            else
            {
                it = *init;
                initInsert(&it, itype, exp, offset, true);
                *init = it;
            }
            exp = baseexp;
            if (sc != StorageClass::auto_ && sc != StorageClass::parameter_ && sc != StorageClass::member_ && sc != StorageClass::mutable_ && sc != StorageClass::localstatic_ && !arrayMember)
            {
                it = nullptr;
                callDestructor(basetype(itype)->sp, nullptr, &exp, nullptr, true, false, false, true);
                initInsert(&it, itype, exp, offset, true);
                insertDynamicDestructor(base, it);
            }
            else if (dest)
            {
                it = *dest;
                callDestructor(basetype(itype)->sp, nullptr, &exp, nullptr, true, false, false, true);
                initInsert(&it, itype, exp, offset, true);
                *dest = it;
            }
        }
        return lex;
    }
    else if (!Optimizer::cparams.prm_cplusplus && !MATCHKW(lex, Keyword::begin_) && !itype->msil && !itype->array)
    {
        EXPRESSION* exp = base ? getThisNode(base) : nullptr;
        TYPE* tp = nullptr;
        lex = expression_no_comma(lex, funcsp, nullptr, &tp, &exp, nullptr, 0);
        ConstExprPatch(&exp);
        if (!tp)
        {
            error(ERR_EXPRESSION_SYNTAX);
        }
        else
        {
            if (!comparetypes(itype, tp, true))
            {
                error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
            }
            else
            {
                std::list<INITIALIZER*>* it = nullptr;
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
                                        while (xx->type == ExpressionNode::void_ && xx->left->type == ExpressionNode::assign_)
                                            xx = xx->right;
                                    initInsert(&it, it2->basetp, xx, it2->offset + offset, it2->noassign);
                                }
                            }
                        }
                        break;
                    default: 
                    {
                        initInsert(&it, itype, exp, offset, false);
                        break;
                    }
                }
                if (it)
                {
                    if (sc != StorageClass::auto_ && sc != StorageClass::localstatic_ && sc != StorageClass::parameter_ && sc != StorageClass::member_ && sc != StorageClass::mutable_ && !arrayMember)
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
    if (isarray(itype) && itype->msil && lex && !MATCHKW(lex, Keyword::begin_))
    {
        EXPRESSION* exp = nullptr;
        TYPE* tp = nullptr;
        lex = expression(lex, funcsp, nullptr, &tp, &exp, 0);
        ConstExprPatch(&exp);
        if (!tp)
        {
            error(ERR_EXPRESSION_SYNTAX);
        }
        else
        {
            std::list<INITIALIZER*>* it = nullptr;
            if (!comparetypes(itype, tp, true))
            {
                error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
            }
            exp = exprNode(ExpressionNode::assign_, exprNode(ExpressionNode::l_object_, getThisNode(base), nullptr), exp);
            exp->left->v.tp = itype;
            initInsert(&it, itype, exp, offset, true);
            if (sc != StorageClass::auto_ && sc != StorageClass::localstatic_ && sc != StorageClass::parameter_ && sc != StorageClass::member_ && sc != StorageClass::mutable_ && !arrayMember)
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
            TYPE* tp1 = nullptr;
            EXPRESSION* exp1 = nullptr;
            if (Optimizer::cparams.prm_cplusplus && !assn && isstructured(itype) && MATCHKW(lex, Keyword::openpa_))
            {
                bool doTrivial = false;
                std::list<INITIALIZER*>* it = nullptr;
                if (basetype(itype)->sp->sb->trivialCons)
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
                    it = InitializeSimpleAggregate(lex, itype, needend, offset, funcsp, sc, base, dest, flags);
                    if (!needkw(&lex, Keyword::closepa_))
                        errskim(&lex, skim_closepa);
                }
                else
                {
                    // conversion constructor params
                    FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
                    lex = getArgs(lex, funcsp, funcparams, Keyword::closepa_, true, 0);
                    if (funcparams->arguments && funcparams->arguments->size() > 1)
                        error(ERR_EXPRESSION_SYNTAX);
                    else if (funcparams->arguments && funcparams->arguments->size() &&
                             !comparetypes(itype, funcparams->arguments->front()->tp, true))
                        error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                    else
                    {
                        TYPE* ttp = itype;
                        if (isconst(itype))
                        {
                            ttp = MakeType(BasicType::const_, ttp);
                        }
                        if (isvolatile(itype))
                        {
                            ttp = MakeType(BasicType::volatile_, ttp);
                        }
                        UpdateRootTypes(ttp);
                        tp1 = itype;
                        if (funcparams->arguments && funcparams->arguments->size() && !isref(funcparams->arguments->front()->tp))
                        {
                            funcparams->arguments->front()->tp = CopyType(funcparams->arguments->front()->tp);
                            funcparams->arguments->front()->tp->lref = true;
                            funcparams->arguments->front()->tp->rref = false;
                        }
                        funcparams->thistp = ttp;
                        exp1 = baseexp;
                        callConstructor(&tp1, &exp1, funcparams, false, nullptr, true, false, false, false, 0, false, true);
                        PromoteConstructorArgs(funcparams->sp, funcparams);
                        initInsert(&it, itype, exp1, offset, true);
                    }
                }
                if (sc != StorageClass::auto_ && sc != StorageClass::localstatic_ && sc != StorageClass::parameter_ && sc != StorageClass::member_ && sc != StorageClass::mutable_ &&
                    !arrayMember)
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
                if (!tp1)
                    error(ERR_EXPRESSION_SYNTAX);
                else if (!comparetypes(itype, tp1, true))
                {
                    bool toErr = true;
                    if (isstructured(tp1))
                    {
                        auto sym = lookupSpecificCast(basetype(tp1)->sp, itype);
                        if (sym)
                        {
                            toErr = false;
                            auto exp2 = exprNode(ExpressionNode::func_, nullptr, nullptr);
                            exp2->v.func = Allocate<FUNCTIONCALL>();
                            exp2->v.func->sp = sym;
                            exp2->v.func->functp = sym->tp;
                            exp2->v.func->fcall = varNode(ExpressionNode::pc_, sym);
                            exp2->v.func->ascall = true;
                            exp2->v.func->thisptr = exp1;
                            exp2->v.func->thistp = MakeType(BasicType::pointer_, tp1);
                            if (isstructured(basetype(sym->tp)->btp))
                            {
                                if (basetype(basetype(sym->tp)->btp)->sp->sb->structuredAliasType)
                                {
                                    auto bn = baseexp;
                                    deref(basetype(basetype(sym->tp)->btp)->sp->sb->structuredAliasType, &bn);
                                    exp2 = exprNode(ExpressionNode::assign_, bn, exp2);
                                }
                                else
                                {
                                    exp2->v.func->returnSP = basetype(basetype(sym->tp)->btp)->sp;
                                    exp2->v.func->returnEXP = baseexp;
                                }
                            }
                            exp1 = exp2;
                        }
                        else if (isstructured(itype) && classRefCount(basetype(itype)->sp, basetype(tp1)->sp) == 1)
                        {
                            toErr = false;
                            EXPRESSION* v = Allocate<EXPRESSION>();
                            v->type = ExpressionNode::c_i_;
                            v = baseClassOffset(basetype(tp1)->sp, basetype(itype)->sp, v);
                            exp1 = exprNode(ExpressionNode::add_, exp1, v);
                            TYPE* ctype = itype; 
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
                    std::list<INITIALIZER*>* it = nullptr;
                    initInsert(&it, itype, exp1, offset, false);
                    if (sc != StorageClass::auto_ && sc != StorageClass::localstatic_ && sc != StorageClass::parameter_ && sc != StorageClass::member_ && sc != StorageClass::mutable_ &&
                        !arrayMember)
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
        *init = InitializeSimpleAggregate(lex, itype, needend, offset, funcsp, sc, base, dest, flags);
    }
    // have to fill in unused array elements with C++ constructors
    // this doesn't play well with the designator stuff but doesn't matter in C++
    if (Optimizer::cparams.prm_cplusplus && isarray(itype) && !arrayMember)
    {
        TYPE* btp = itype;
        while (isarray(btp))
            btp = basetype(btp)->btp;
        btp = basetype(btp);
        if (isstructured(btp) && !btp->sp->sb->trivialCons)
        {
            int s = (btp->size);
            std::list<INITIALIZER*>::iterator test = (*init)->begin();
            std::list<INITIALIZER*>* first = nullptr;
            int last = 0, i;
            for (; test!= (*init)->end()  || last < itype->size;)
            {
                if ((test != (*init)->end() && last < (*test)->offset) || (test == (*init)->end() && last < itype->size))
                {
                    int n = (test != (*init)->end() ? (*test)->offset - last : itype->size - last) / s;
                    EXPRESSION* sz = nullptr;
                    TYPE *ctype = btp, *tn = btp;
                    EXPRESSION* exp;
                    if (last)
                        exp = exprNode(ExpressionNode::add_, getThisNode(base), intNode(ExpressionNode::c_i_, last));
                    else
                        exp = getThisNode(base);
                    if (n > 1)
                    {
                        sz = intNode(ExpressionNode::c_i_, n);
                        tn = MakeType(BasicType::pointer_, btp);
                        tn->array = true;
                        tn->size = n * s;
                        tn->esize = sz;
                    }
                    callConstructor(&ctype, &exp, nullptr, true, sz, true, false, false, false, false, false, true);
                    initInsert(&first, tn, exp, last, true);
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
            if (sc != StorageClass::auto_ && sc != StorageClass::localstatic_ && sc != StorageClass::parameter_ && sc != StorageClass::member_ && sc != StorageClass::mutable_)
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
            TYPE* tn = btp;
            exp = getThisNode(base);
            if (n > 1)
            {
                sz = intNode(ExpressionNode::c_i_, n);
                tn = MakeType(BasicType::pointer_, btp);
                tn->array = true;
                tn->size = n * s;
                tn->esize = sz;
            }
            if (sc != StorageClass::auto_ && sc != StorageClass::parameter_ && sc != StorageClass::member_ && sc != StorageClass::mutable_)
            {
                callDestructor(btp->sp, nullptr, &exp, sz, true, false, false, true);
                initInsert(&first, tn, exp, last, false);
                insertDynamicDestructor(base, first);
                *dest = nullptr;
            }
            else if (dest)
            {
                callDestructor(btp->sp, nullptr, &exp, sz, true, false, false, true);
                initInsert(&first, tn, exp, last, false);
                *dest = first;
            }
        }
    }
    return lex;
}
static LEXLIST* initialize_bit(LEXLIST* lex, SYMBOL* funcsp, int offset, StorageClass sc, TYPE* itype, std::list<INITIALIZER*>** init)
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
static LEXLIST* initialize_auto(LEXLIST* lex, SYMBOL* funcsp, int offset, StorageClass sc, TYPE* itype, std::list<INITIALIZER*>** init,
                                std::list<INITIALIZER*>** dest, SYMBOL* sym)
{
    TYPE* tp;
    EXPRESSION* exp;
    bool needend = false;
    if (MATCHKW(lex, Keyword::begin_))
    {
        needend = true;
        lex = getsym();
    }
    if (Optimizer::cparams.prm_cplusplus && needend && MATCHKW(lex, Keyword::end_))
    {
        exp = intNode(ExpressionNode::c_i_, 0);
        sym->tp = &stdint;  // sets type for variable
        initInsert(init, sym->tp, exp, offset, false);
    }
    else
    {
        TYPE* tp = nullptr;
        EXPRESSION* exp;
        lex = init_expression(lex, funcsp, nullptr, &tp, &exp, false);
        if (!tp)
            error(ERR_EXPRESSION_SYNTAX);
        else
        {
            if (isarray(tp))
            {
                tp = MakeType(BasicType::pointer_, basetype(tp)->btp);
            }
            if (isconst(sym->tp) && !isconst(tp))
            {
                tp = MakeType(BasicType::const_, tp);
            }
            if (isvolatile(sym->tp) && !isvolatile(tp))
            {
                tp = MakeType(BasicType::volatile_, tp);
            }
            sym->tp = tp;  // sets type for variable
        }
        if (Optimizer::cparams.prm_cplusplus && isstructured(sym->tp))
        {

            std::list<INITIALIZER *>*dest = nullptr, *it;
            EXPRESSION* expl = getThisNode(sym);
            initInsert(init, sym->tp, exp, offset, false);
            if (sym->sb->storage_class != StorageClass::auto_ && sym->sb->storage_class != StorageClass::parameter_ &&
                sym->sb->storage_class != StorageClass::member_ && sym->sb->storage_class != StorageClass::mutable_)
            {
                callDestructor(basetype(sym->tp)->sp, nullptr, &expl, nullptr, true, false, false, true);
                initInsert(&dest, sym->tp, expl, offset, true);
                insertDynamicDestructor(sym, dest);
            }
            else if (dest)
            {
                callDestructor(basetype(sym->tp)->sp, nullptr, &expl, nullptr, true, false, false, true);
                initInsert(&dest, sym->tp, expl, offset, true);
                sym->sb->dest = dest;
            }
        }
        else
        {
            initInsert(init, sym->tp, exp, offset, false);
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
LEXLIST* initType(LEXLIST* lex, SYMBOL* funcsp, int offset, StorageClass sc, std::list<INITIALIZER*>** init, std::list<INITIALIZER*>** dest, TYPE* itype,
                  SYMBOL* sym, bool arrayMember, int flags)
{
    TYPE* tp;
    tp = basetype(itype);
    if (tp->type == BasicType::templateselector_)
    {
        SYMBOL* ts = (*tp->sp->sb->templateSelector)[1].sp;
        auto find = (*tp->sp->sb->templateSelector).begin();
        auto finde = (*tp->sp->sb->templateSelector).end();
        SYMBOL* sym = nullptr;
        if ((*tp->sp->sb->templateSelector)[1].isDeclType)
        {
            TYPE* tp1 = TemplateLookupTypeFromDeclType((*tp->sp->sb->templateSelector)[1].tp);
            if (tp1 && isstructured(tp1))
                sym = basetype(tp1)->sp;
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
        else if (basetype(ts->tp)->templateParam->second->type == Keyword::typename_)
        {
            tp = basetype(ts->tp)->templateParam->second->byClass.val;
            if (!tp)
            {
                if (templateNestingCount)
                {
                    lex = getsym();
                    errskim(&lex, skim_closepa);
                }
                return lex;
            }
            sym = tp->sp;
        }
        else if (basetype(ts->tp)->templateParam->second->type == Keyword::delete_)
        {
            std::list<TEMPLATEPARAMPAIR>* args = basetype(ts->tp)->templateParam->second->byDeferred.args;
            sym = tp->templateParam->first;
            sym = TemplateClassInstantiateInternal(sym, args, true);
        }
        if (sym)
        {
            sym = basetype(PerformDeferredInitialization(sym->tp, nullptr))->sp;
            for (++find, ++find ; find != finde && sym; ++ find)
            {
                SYMBOL* spo = sym;
                if (!isstructured(spo->tp))
                    break;

                sym = search(spo->tp->syms, find->name);
                if (!sym)
                {
                    sym = classdata(find->name, spo, nullptr, false, false);
                    if (sym == (SYMBOL*)-1)
                        sym = nullptr;
                    if (sym && find->isTemplate)
                    {
                        sym = GetClassTemplate(sym, find->templateParams, false);
                        if (sym)
                            sym->tp = PerformDeferredInitialization(sym->tp, funcsp);
                    }
                }
            }
            if (find == finde && sym && istype(sym))
                tp = basetype(sym->tp);
            else
                tp = nullptr;
        }
        else
            tp = nullptr;
    }
    if (!tp)
    {
        if (!templateNestingCount && !(flags & _F_TEMPLATEARGEXPANSION))
        {
            errortype(ERR_CANNOT_INITIALIZE, basetype(itype), nullptr);
        }
        return lex;
    }
    switch (tp->type)
    {
        case BasicType::aggregate_:
            return lex;
        case BasicType::bool_:
            return initialize_bool_type(lex, funcsp, offset, sc, tp, init);
        case BasicType::inative_:
        case BasicType::unative_:
        case BasicType::char_:
        case BasicType::unsigned_char_:
        case BasicType::signed_char_:
        case BasicType::short_:
        case BasicType::unsigned_short_:
        case BasicType::int_:
        case BasicType::unsigned_:
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
            return initialize_arithmetic_type(lex, funcsp, offset, sc, tp, init, flags);
        case BasicType::lref_:
        case BasicType::rref_:
            return initialize_reference_type(lex, funcsp, offset, sc, tp, init, flags, sym);
        case BasicType::pointer_:
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
                    return initialize_aggregate_type(lex, funcsp, sym, offset, sc, tp, init, dest, arrayMember, flags);
                }
            }
            else
            {
                return initialize_pointer_type(lex, funcsp, offset, sc, tp, init);
            }
        case BasicType::memberptr_:
            return initialize_memberptr(lex, funcsp, offset, sc, tp, init);
        case BasicType::bit_:
            return initialize_bit(lex, funcsp, offset, sc, tp, init);
        case BasicType::auto_:
            return initialize_auto(lex, funcsp, offset, sc, tp, init, dest, sym);
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
                        return initialize_aggregate_type(lex, funcsp, sym, offset, sc, tp, init, dest, arrayMember, flags);
                    }
                    else
                    {
                        return initialize_auto_struct(lex, funcsp, offset, tp, init);
                    }
                }
                else
                {
                    return initialize_aggregate_type(lex, funcsp, sym, offset, sc, tp, init, dest, arrayMember, flags);
                }
            }
            /* fallthrough */
        default:
            if (!templateNestingCount)
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
static bool hasData(TYPE* tp)
{
    for (auto sp : *basetype(tp)->syms )
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
void RecalculateVariableTemplateInitializers(std::list<INITIALIZER*>::iterator& ilbegin, std::list<INITIALIZER*>::iterator& ilend, std::list<INITIALIZER*>** out, TYPE* tp, int offset)
{
    if (isstructured(tp))
    {
        int bcoffset = offset;
        SYMBOL* sym;
        AGGREGATE_DESCRIPTOR *desc = nullptr, *cache = nullptr;
        allocate_desc(tp, offset, &desc, &cache);

        if (basetype(tp)->sp->sb->baseClasses)
            for (auto base : *basetype(tp)->sp->sb->baseClasses)
                while (base)
                {
                    RecalculateVariableTemplateInitializers(ilbegin, ilend, out, base->cls->tp, bcoffset);
                    bcoffset += base->cls->tp->size;
                }
        while (desc)
        {
            sym = *desc->it;
            if (ismember(sym))
            {
                if (InitVariableMatches(sym, (*ilbegin)->fieldsp))
                    RecalculateVariableTemplateInitializers(ilbegin, ilend, out, sym->tp, offset + sym->sb->offset);
            }
            increment_desc(&desc, &cache);
        }
        if ((*ilbegin)->basetp == 0)
        {
            auto init = Allocate<INITIALIZER>();
            init->offset = tp->size;
            (*out)->push_back(init);
        }
    }
    else if (isarray(tp))
    {
        if (tp->size)
        {
            int i;
            int n = tp->size / tp->btp->size;
            TYPE* base = basetype(basetype(tp)->btp);
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
        if ((*ilbegin)->basetp == 0)
        {
            auto init = Allocate<INITIALIZER>();
            init->offset = tp->size;
            (*out)->push_back(init);
        }
    }
    else
    {
        auto init = Allocate<INITIALIZER>();
        init->offset = tp->size;
        init->basetp = tp;
        (*out)->push_back(init);
        ++ilbegin;
    }
}
LEXLIST* initialize(LEXLIST* lex, SYMBOL* funcsp, SYMBOL* sym, StorageClass storage_class_in, bool asExpression, bool inTemplate,
                    int flags)
{
    auto sp = basetype(sym->tp)->sp;
    if (sp && isstructured(sp->tp) && sp->sb && sp->sb->attribs.uninheritable.deprecationText)
        deprecateMessage(basetype(sym->tp)->sp);
    TYPE* tp;
    bool initialized = MATCHKW(lex, Keyword::assign_) || MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::openpa_);
    inittag = 0;
    browse_variable(sym);
    // MSIL property
    if (IsCompiler())
    {
        if (sym->sb->attribs.inheritable.linkage2 == Linkage::property_)
            return initialize_property(lex, funcsp, sym, storage_class_in, asExpression, flags);
    }
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
    tp = basetype(sym->tp);
    if ((ispointer(tp) && tp->array) || isref(tp))
        tp = basetype(basetype(tp)->btp);
    if (sym->sb->storage_class != StorageClass::typedef_ && sym->sb->storage_class != StorageClass::external_ && isstructured(tp) && !isref(sym->tp) &&
        !tp->syms)
    {
        tp = PerformDeferredInitialization(tp, funcsp);
        sym->tp = tp = tp->sp->tp;
    }
    if (sym->sb->storage_class != StorageClass::typedef_ && sym->sb->storage_class != StorageClass::external_ && isstructured(tp) && !isref(sym->tp) &&
        !tp->syms)
    {
        if (MATCHKW(lex, Keyword::assign_))
            errskim(&lex, skim_semi);
        if (!templateNestingCount)
            errorsym(ERR_STRUCT_NOT_DEFINED, tp->sp);
    }
    // if not in a constructor, any Keyword::openpa_() will be eaten by an expression parser
    else if (MATCHKW(lex, Keyword::assign_) || (Optimizer::cparams.prm_cplusplus && (MATCHKW(lex, Keyword::openpa_) || MATCHKW(lex, Keyword::begin_))) ||
             ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions && MATCHKW(lex, Keyword::openpa_)))
    {
        std::list<INITIALIZER*>** init;
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
            if (isautotype(sym->tp) && MATCHKW(lex, Keyword::assign_))
            {
                LEXLIST* placeholder = lex;
                TYPE* tp1 = nullptr;
                EXPRESSION* exp1;
                lex = getsym();
                if (!MATCHKW(lex, Keyword::begin_) && !MATCHKW(lex, Keyword::openbr_))
                {
                    lex = expression_no_check(lex, funcsp, nullptr, &tp1, &exp1, _F_TYPETEST);
                    if (tp1 && tp1->stringconst)
                    {
                        tp1 = MakeType(BasicType::pointer_, MakeType(BasicType::const_, tp1->btp));
                        UpdateRootTypes(tp1);
                        sym->tp = tp1;
                    }
                    else if (tp1)
                    {
                        if (isarray(tp1))
                        {
                            tp1 = MakeType(BasicType::pointer_, basetype(tp1)->btp);
                        }
                        DeduceAuto(&sym->tp, tp1, exp1);
                        TYPE** tp2 = &sym->tp;
                        while (ispointer(*tp2) || isref(*tp2))
                            tp2 = &basetype(*tp2)->btp;
                        //                        *tp2 = tp1;
                        if (isstructured(*tp2))
                            *tp2 = basetype(*tp2)->sp->tp;

                        if (sym->sb->storage_class != StorageClass::typedef_ && sym->sb->storage_class != StorageClass::external_ && isstructured(*tp2) &&
                            !isref(sym->tp) && !(*tp2)->syms)
                        {
                            *tp2 = PerformDeferredInitialization((*tp2), funcsp);
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
                TYPE* t = !isassign && (Optimizer::architecture == ARCHITECTURE_MSIL) ? find_boxed_type(sym->tp) : 0;
                if (!t || !search(basetype(t)->syms, overloadNameTab[CI_CONSTRUCTOR]))
                    t = sym->tp;
                if (MATCHKW(lex, Keyword::assign_))
                {
                    if (!isstructured(t) && !isarray(t))
                        lex = getsym(); /* past = */
                    else
                        assigned = true;
                }
                lex = initType(lex, funcsp, 0, sym->sb->storage_class, &sym->sb->init, &sym->sb->dest, t, sym, false, flags | _F_EXPLICIT);
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
                        initInsert(&sym->sb->init, nullptr, nullptr, isautotype(t) ? sym->tp->size : t->size, false);
                }
            }
        }
    }
    else if ((Optimizer::cparams.prm_cplusplus || ((Optimizer::architecture == ARCHITECTURE_MSIL) && isstructured(sym->tp) &&
                                                   !basetype(sym->tp)->sp->sb->trivialCons)) &&
             sym->sb->storage_class != StorageClass::typedef_ && sym->sb->storage_class != StorageClass::external_ && !asExpression)
    {
        TYPE* t = ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions)
                      ? find_boxed_type(sym->tp)
                      : 0;
        if (!t || !search(basetype(t)->syms, overloadNameTab[CI_CONSTRUCTOR]))
            t = sym->tp;
        if (isstructured(t))
        {
            if (!basetype(tp)->sp->sb->trivialCons)
            {
                // default constructor without (), or array of structures without an initialization list
                lex = initType(lex, funcsp, 0, sym->sb->storage_class, &sym->sb->init, &sym->sb->dest, t, sym, false, flags | _F_EXPLICIT);
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
                        initInsert(&sym->sb->init, nullptr, nullptr, t->size, false);
                }
            }
            else if (Optimizer::cparams.prm_cplusplus)
            {
                AGGREGATE_DESCRIPTOR *desc = nullptr, *cache = nullptr;
                std::list<INITIALIZER *>*data = nullptr;
                allocate_desc(basetype(tp), 0, &desc, &cache);
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
                        initInsert(&sym->sb->init, nullptr, nullptr, t->size, false);
                }
            }
        }
        else if (isarray(sym->tp))
        {
            // constructors for uninitialized array
            TYPE* z = sym->tp;
            if (!z)
                z = sym->tp;
            while (isarray(z))
                z = basetype(z)->btp;
            z = basetype(z);
            if (isstructured(z) && !z->sp->sb->trivialCons)  // && !sym->sb->parentClass)
            {
                std::list<INITIALIZER *>* init = nullptr;
                int n = sym->tp->size / (z->size);
                TYPE* ctype = z;
                EXPRESSION* sz = n > 1 ? intNode(ExpressionNode::c_i_, n) : nullptr;
                EXPRESSION* baseexp = getThisNode(sym);
                EXPRESSION* exp = baseexp;
                errors[ERR_NO_OVERLOAD_MATCH_FOUND].level = 0;
                bool test = callConstructor(&ctype, &exp, nullptr, true, sz, true, false, false, false, false, false, true);
                errors[ERR_NO_OVERLOAD_MATCH_FOUND].level = CE_ERROR;
                if (test)
                {
                    initInsert(&init, z, exp, 0, true);
                    if (storage_class_in != StorageClass::auto_ && storage_class_in != StorageClass::localstatic_ && storage_class_in != StorageClass::parameter_ &&
                        storage_class_in != StorageClass::member_ && storage_class_in != StorageClass::mutable_)
                    {
                        insertDynamicInitializer(sym, init);
                    }
                    else
                    {
                        sym->sb->init = init;
                    }
                    init = nullptr;
                    exp = baseexp;
                    callDestructor(z->sp, nullptr, &exp, sz, true, false, false, true);
                    initInsert(&init, z, exp, 0, true);
                    if (storage_class_in != StorageClass::auto_ && storage_class_in != StorageClass::parameter_ && storage_class_in != StorageClass::member_ &&
                        storage_class_in != StorageClass::mutable_)
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
                            initInsert(&sym->sb->init, nullptr, nullptr, sym->tp->size, false);
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
        (isarithmetic(sym->tp) || (ispointer(sym->tp) && !isarray(sym->tp))) && sym->sb->storage_class == StorageClass::auto_)
    {
        EXPRESSION* exp = intNode(ExpressionNode::c_i_, 0);
        cast(sym->tp, &exp);
        optimize_for_constants(&exp);
        initInsert(&sym->sb->init, sym->tp, exp, 0, false);
        sym->sb->assigned = true;
    }
    if (isautotype(sym->tp) && !MATCHKW(lex, Keyword::colon_))
    {
        errorsym(ERR_AUTO_NEEDS_INITIALIZATION, sym);
        sym->tp = &stdint;
    }
    tp = sym->tp;
    while (ispointer(tp))
    {
        tp = basetype(tp);
        if (!tp->array)
        {
            tp = sym->tp;
            break;
        }
        tp = tp->btp;
    }
    if (sym->sb->constexpression && !templateNestingCount)
    {
        if (!ispointer(tp) && !isarithmetic(tp) && basetype(tp)->type != BasicType::enum_ &&
            (!isstructured(tp) /*|| !basetype(tp)->sp->sb->trivialCons*/))
        {
            error(ERR_CONSTEXPR_SIMPLE_TYPE);
        }
        else if (!sym->sb->init)
        {
            if (sym->sb->storage_class != StorageClass::external_ && !initialized && (!sp || !isstructured(sp->tp)))
                error(ERR_CONSTEXPR_REQUIRES_INITIALIZER);
        }
        else if (isstructured(tp))
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
        else if (!isfunction(tp))
        {
            tp = MakeType(BasicType::const_, tp);
            UpdateRootTypes(tp);
            if (!IsConstantExpression(sym->sb->init->front()->exp, false, true))
            {
                sym->sb->init->front()->exp = intNode(ExpressionNode::c_i_, 0);
                error(ERR_CONSTANT_EXPRESSION_EXPECTED);
            }
        }
        else
        {
            if (!IsConstantExpression(sym->sb->init->front()->exp, false, true))
            {
                sym->sb->init->front()->exp = intNode(ExpressionNode::c_i_, 0);
                error(ERR_CONSTANT_EXPRESSION_EXPECTED);
            }
        }
    }
    if (isconst(tp))
    {
        if (!templateNestingCount)
        {
            if (!sym->sb->init)
            {
                if (!asExpression)
                {
                    if (sym->sb->storage_class != StorageClass::external_ && sym->sb->storage_class != StorageClass::typedef_ &&
                        sym->sb->storage_class != StorageClass::member_ && sym->sb->storage_class != StorageClass::mutable_)
                    {
                        if (!sym->sb->assigned && (!isstructured(tp) || !Optimizer::cparams.prm_cplusplus ||
                                                   (basetype(tp)->sp->sb->trivialCons && hasData(tp))))
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
                     (isint(sym->tp) || basetype(sym->tp)->type == BasicType::enum_)))
                {
                    if (sym->sb->storage_class != StorageClass::static_ && !Optimizer::cparams.prm_cplusplus && !funcsp)
                        insertInitSym(sym);
                    sym->sb->value.i = sym->sb->init->front()->exp->v.i;
                    sym->sb->storage_class = StorageClass::constant_;
                    Optimizer::SymbolManager::Get(sym)->i = sym->sb->value.i;
                    Optimizer::SymbolManager::Get(sym)->storage_class = Optimizer::scc_constant;
                }
            }
        }
        else if (sym->sb->init && !inTemplate && sym->sb->init->front()->exp &&
                 (sym->sb->constexpression || isint(sym->tp) || basetype(sym->tp)->type == BasicType::enum_))
        {
            if (sym->sb->storage_class != StorageClass::static_ && !Optimizer::cparams.prm_cplusplus && !funcsp)
                insertInitSym(sym);
            sym->sb->storage_class = StorageClass::constant_;
            Optimizer::SymbolManager::Get(sym)->i = sym->sb->value.i;
            Optimizer::SymbolManager::Get(sym)->storage_class = Optimizer::scc_constant;
        }
    }
    if (isatomic(sym->tp) && (sym->sb->storage_class == StorageClass::auto_))
    {
        if (sym->sb->init == nullptr && needsAtomicLockFromType(sym->tp))
        {
            // sets the atomic_flag portion of a locked type to zero
            sym->sb->init = initListFactory.CreateList();
            sym->sb->init->push_back(Allocate<INITIALIZER>());
            sym->sb->init->front()->offset = sym->tp->size - ATOMIC_FLAG_SPACE;
            sym->sb->init->front()->basetp = &stdint;
            sym->sb->init->front()->exp = intNode(ExpressionNode::c_i_, 0);
        }
    }
    if (isref(sym->tp) && sym->sb->storage_class != StorageClass::typedef_)
    {
        if (!sym->sb->init && sym->sb->storage_class != StorageClass::external_ && sym->sb->storage_class != StorageClass::member_ &&
            sym->sb->storage_class != StorageClass::mutable_ && !asExpression)
        {
            errorsym(ERR_REF_MUST_INITIALIZE, sym);
        }
    }
    if (sym->sb->storage_class == StorageClass::static_ || sym->sb->storage_class == StorageClass::global_ || sym->sb->storage_class == StorageClass::localstatic_)
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
        else
        {
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
    return lex;
}
}  // namespace Parser