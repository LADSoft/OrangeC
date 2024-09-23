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

/*
 * istmt.c
 *
 * change the statement list to icode
 */
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <climits>
#include <cstdlib>
#include "compiler.h"
#include "ccerr.h"
#include "config.h"
#include "rtti.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "iblock.h"
#include "initbackend.h"
#include "declcpp.h"
#include "ildata.h"
#include "OptUtils.h"
#include "iblock.h"
#include "stmt.h"
#include "iinline.h"
#include "occparse.h"
#include "istmt.h"
#include "iexpr.h"
#include "expr.h"
#include "memory.h"
#include "declare.h"
#include "init.h"
#include "lex.h"
#include "help.h"
#include "beinterf.h"
#include "inasm.h"
#include "optmodules.h"
#include "namespace.h"
#include "symtab.h"
#include "constopt.h"
#include "types.h"

Optimizer::SimpleSymbol* currentFunction;

namespace Parser
{

Optimizer::IMODE* returnImode;
int retcount;
int consIndex;
EXPRESSION* xcexp;
int catchLevel;
int codeLabelOffset;

static Optimizer::LIST* mpthunklist;
static std::map<int, int> retLabs;

static int breaklab;
static int contlab;
static int tryStart, tryEnd;
static int plabel;
static Optimizer::IMODE* returnSym;

void genstmtini(void)
{
    mpthunklist = nullptr;
    returnImode = nullptr;
}

Optimizer::QUAD* gen_xcexp_expression(int n)
{
    Optimizer::IMODE* ap = Allocate<Optimizer::IMODE>();
    ap->mode = Optimizer::i_direct;
    ap->offset = Optimizer::SymbolManager::Get(xcexp);
    ap->size = ISZ_UINT;

    Optimizer::IMODE* ap2 = Optimizer::make_immed(ISZ_UINT, n);
    return gen_icode(Optimizer::i_assn, ap, ap2, nullptr);
}
/*-------------------------------------------------------------------------*/

Optimizer::IMODE* imake_label(int label)
/*
 *      make a node to reference an immediate value i.
 */

{
    Optimizer::IMODE* ap = Allocate<Optimizer::IMODE>();
    ap->mode = Optimizer::i_immed;
    ap->offset = Allocate<Optimizer::SimpleExpression>();
    ap->offset->type = Optimizer::se_labcon;
    ap->offset->i = label;
    ap->size = ISZ_ADDR;

    return ap;
}
/*-------------------------------------------------------------------------*/

Optimizer::IMODE* make_direct(int i)
/*
 *      make a direct reference to an immediate value.
 */
{
    return make_ioffset(MakeIntExpression(ExpressionNode::c_i_, i));
}

/*-------------------------------------------------------------------------*/

void gen_genword(Statement* stmt, SYMBOL* funcsp)
/*
 * generate data in the code seg
 */
{
    (void)funcsp;
    Optimizer::gen_icode(Optimizer::i_genword, 0, Optimizer::make_immed(ISZ_UINT, (int)stmt->select->v.i), 0);
}

/*-------------------------------------------------------------------------*/
Optimizer::IMODE* set_symbol(const char* name, int isproc)
/*
 *      generate a call to a library routine.
 */
{
    SYMBOL* sym;
    Optimizer::IMODE* result;
    (void)isproc;
    sym = gsearch(name);
    if (sym == 0)
    {
        Optimizer::LIST* l1;
        sym = SymAlloc();
        sym->sb->storage_class = StorageClass::external_;
        sym->name = sym->sb->decoratedName = litlate(name);
        sym->tp = Type::MakeType(isproc ? BasicType::func_ : BasicType::int_);
        sym->sb->safefunc = true;
        globalNameSpace->front()->syms->Add(sym);
        auto osym = Optimizer::SymbolManager::Get(sym);
        Optimizer::externalSet.insert(osym);
        Optimizer::externals.push_back(osym);
    }
    else
    {
        if (sym->sb->storage_class == StorageClass::overloads_)
            sym = sym->tp->syms->front();
    }
    result = Allocate<Optimizer::IMODE>();
    result->offset = Allocate<Optimizer::SimpleExpression>();
    result->offset->type = Optimizer::se_global;
    result->offset->sp = Optimizer::SymbolManager::Get(sym);
    result->mode = Optimizer::i_direct;
    if (isproc)
    {
        result->offset->type = Optimizer::se_pc;
        if (Optimizer::chosenAssembler->arch->libsasimports)
            result->mode = Optimizer::i_direct;
        else
            result->mode = Optimizer::i_immed;
    }
    return result;
}

/*-------------------------------------------------------------------------*/

Optimizer::IMODE* call_library(const char* lib_name, int size)
/*
 *      generate a call to a library routine.
 */
{
    Optimizer::IMODE* result;
    result = set_symbol(lib_name, 1);
    Optimizer::gen_icode(Optimizer::i_gosub, 0, result, 0);
    Optimizer::gen_icode(Optimizer::i_parmadj, 0, Optimizer::make_parmadj(size < 0 ? 0 : size), Optimizer::make_parmadj(abs(size)));
    result = Optimizer::tempreg(ISZ_UINT, 0);
    result->retval = true;
    return result;
}
static void AddProfilerData(SYMBOL* funcsp)
{
    LCHAR* pname;
    if (Optimizer::cparams.prm_profiler)
    {
        StringData* string;
        int i;
        int l = strlen(funcsp->sb->decoratedName);
        pname = Allocate<LCHAR>(l + 1);
        for (i = 0; i < l + 1; i++)
            pname[i] = funcsp->sb->decoratedName[i];
        string = Allocate<StringData>();
        string->strtype = LexType::l_astr_;
        string->size = 1;
        string->pointers = Allocate<Optimizer::SLCHAR*>();
        string->pointers[0] = Allocate<Optimizer::SLCHAR>();
        string->pointers[0]->str = pname;
        string->pointers[0]->count = l;
        string->suffix = nullptr;
        stringlit(string);
        plabel = string->label;
        Optimizer::gen_icode(Optimizer::i_parm, 0, imake_label(plabel), 0);
        call_library("__profile_in", getSize(BasicType::pointer_));
    }
}

//-------------------------------------------------------------------------

void SubProfilerData(void)
{
    if (Optimizer::cparams.prm_profiler)
    {
        Optimizer::gen_icode(Optimizer::i_parm, 0, imake_label(plabel), 0);
        call_library("__profile_out", getSize(BasicType::pointer_));
    }
}


EXPRESSION* tempVar(Type* tp, bool global) 
{
    if (global)
        anonymousNotAlloc++;
    auto val = AnonymousVar(global ? StorageClass::global_ : StorageClass::auto_, tp);
    if (global)
    {
        insertInitSym(val->v.sp);
        anonymousNotAlloc--;
    }
    auto expx = Optimizer::SymbolManager::Get(val);
    Optimizer::cacheTempSymbol(expx->sp);
    expx->sp->anonymous = false;
    expx->sp->allocate = false;
    Dereference(tp, &val);
    return val;
}
EXPRESSION *makeParamSubs(EXPRESSION* left, Optimizer::IMODE* im)
{
    auto val = MakeExpression(ExpressionNode::paramsubstitute_, left);
    val->v.imode = im;
    return val;
}

    /*-------------------------------------------------------------------------*/
void count_cases(std::list<CASEDATA*>* casedata, struct Optimizer::cases* cs)
{
    for (auto cd : *casedata)
    {
        cs->count++;
        if (cd->val < cs->bottom)
            cs->bottom = cd->val;
        if (cd->val > cs->top)
            cs->top = cd->val;
    }
}
/*-------------------------------------------------------------------------*/
void gather_cases(std::list<CASEDATA*>* cds, struct Optimizer::cases* cs)
{
    int pos = 0;
    if (!cs->ptrs)
    {
        cs->ptrs = Allocate<Optimizer::caseptrs>(cs->count);
    }
    if (cds)
        for (auto cd : *cds)
        {
            cs->ptrs[pos].label = cd->label + codeLabelOffset;
            cs->ptrs[pos++].id = cd->val;
        }
}

/*-------------------------------------------------------------------------*/

int gcs_compare(void const* left, void const* right)
{
    struct Optimizer::caseptrs const* lleft = (struct Optimizer::caseptrs const*)left;
    struct Optimizer::caseptrs const* lright = (struct Optimizer::caseptrs const*)right;
    if (lleft->id < lright->id)
        return -1;
    return lleft->id > lright->id;
}
/*-------------------------------------------------------------------------*/

void genxswitch(Statement* stmt, SYMBOL* funcsp)
/*
 *      analyze and generate best switch statement.
 */
{
    int oldbreak, i;
    struct Optimizer::cases cs;
    Optimizer::IMODE *ap, *ap3;

#ifdef USE_LONGLONG
    unsigned long long a = 1;
#endif
    oldbreak = breaklab;
    breaklab = stmt->breaklabel + codeLabelOffset;
    memset(&cs, 0, sizeof(cs));
#ifndef USE_LONGLONG
    cs.top = INT_MIN;
    cs.bottom = INT_MAX;
#else
    cs.top = (a << 63);     /* LLONG_MIN*/
    cs.bottom = cs.top - 1; /* LLONG_MAX*/
#endif
    count_cases(stmt->cases, &cs);
    cs.top++;
    ap3 = gen_expr(funcsp, stmt->select, F_VOL | F_SWITCHVALUE, ISZ_UINT);
    ap = Optimizer::LookupLoadTemp(nullptr, ap3);
    if (ap != ap3)
    {
        Optimizer::IMODE* barrier;
        //        if (stmt->select->isatomic)
        //        {
        //            barrier = doatomicFence(funcsp, nullptr, stmt->select, nullptr);
        //        }
        Optimizer::gen_icode(Optimizer::i_assn, ap, ap3, nullptr);
        //        if (stmt->select->isatomic)
        //        {
        //            doatomicFence(funcsp, nullptr, stmt->select, barrier);
        //        }
    }
    if (Optimizer::chosenAssembler->arch->preferopts & OPT_EXPANDSWITCH)
    {
        EXPRESSION* en = AnonymousVar(StorageClass::auto_, &stdint);
        en->v.sp->sb->anonymous = false;
        Optimizer::cacheTempSymbol(Optimizer::SymbolManager::Get(en->v.sp));
        if (ap->size != -ISZ_UINT)
        {
            ap3 = Optimizer::tempreg(-ISZ_UINT, 0);
            Optimizer::gen_icode(Optimizer::i_assn, ap3, ap, nullptr);
            ap = ap3;
        }
        ap3 = Allocate<Optimizer::IMODE>();
        ap3->mode = Optimizer::i_direct;
        ap3->offset = Optimizer::SymbolManager::Get(en);
        ap3->size = -ISZ_UINT;
        Optimizer::gen_icode(Optimizer::i_assn, ap3, ap, nullptr);
        ap = ap3;
    }
    Optimizer::gen_icode2(Optimizer::i_coswitch, Optimizer::make_immed(ISZ_UINT, cs.count), ap,
                          Optimizer::make_immed(ISZ_UINT, cs.top - cs.bottom), stmt->label + codeLabelOffset);
    gather_cases(stmt->cases, &cs);
    qsort(cs.ptrs, cs.count, sizeof(cs.ptrs[0]), gcs_compare);
    for (i = 0; i < cs.count; i++)
    {
        Optimizer::gen_icode2(Optimizer::i_swbranch, 0, Optimizer::make_immed(ISZ_UINT, cs.ptrs[i].id), 0, cs.ptrs[i].label);
    }
    breaklab = oldbreak;
}

void genselect(Statement* stmt, SYMBOL* funcsp, bool jmptrue)
{
    if (stmt->altlabel + codeLabelOffset)
    {
        //		Optimizer::gen_label(stmt->altlabel + codeLabelOffset);
        //		Optimizer::intermed_tail->dc.opcode = Optimizer::i_skipcompare;
    }
    if (jmptrue)
        truejmp(stmt->select, funcsp, stmt->label + codeLabelOffset);
    else
        falsejmp(stmt->select, funcsp, stmt->label + codeLabelOffset);
    if (stmt->altlabel + codeLabelOffset)
    {
        //		Optimizer::gen_label(stmt->altlabel + codeLabelOffset);
        //		Optimizer::intermed_tail->dc.opcode = Optimizer::i_skipcompare;
    }
}
/*-------------------------------------------------------------------------*/
static void gen_try(SYMBOL* funcsp, Statement* stmt, int startLab, int endLab, int transferLab, std::list<Statement*>* lower)
{
    Optimizer::gen_label(startLab);
    stmt->tryStart = ++consIndex;
    gen_xcexp_expression(consIndex);
    genstmt(lower, funcsp, 0);
    stmt->tryEnd = ++consIndex;
    gen_xcexp_expression(consIndex);
    Optimizer::gen_label(endLab);
    /* not using gen_igoto because it will make a new block */
    Optimizer::gen_icode(Optimizer::i_goto, nullptr, nullptr, nullptr);
    Optimizer::intermed_tail->dc.v.label = transferLab;
    tryStart = stmt->tryStart;
    tryEnd = stmt->tryEnd;
}
static void gen_catch(SYMBOL* funcsp, Statement* stmt, int startLab, int transferLab, std::list<Statement*>* lower)
{
    int oldtryStart = tryStart;
    int oldtryEnd = tryEnd;
    Optimizer::gen_label(startLab);
    Optimizer::currentBlock->alwayslive = true;
    Optimizer::intermed_tail->alwayslive = true;
    catchLevel++;
    genstmt(lower, funcsp, 0);
    catchLevel--;
    /* not using gen_igoto because it will make a new block */
    Optimizer::gen_icode(Optimizer::i_goto, nullptr, nullptr, nullptr);
    Optimizer::intermed_tail->dc.v.label = transferLab;
    tryStart = oldtryStart;
    tryEnd = oldtryEnd;
    stmt->tryStart = tryStart;
    stmt->tryEnd = tryEnd;
}
static void gen___try(SYMBOL* funcsp, std::list<Statement*> stmts)
{
    int label = Optimizer::nextLabel++;
    for (auto stmt : stmts)
    {
        int mode = 0;
        Optimizer::IMODE* left = nullptr;
        switch (stmt->type)
        {
        case StatementNode::seh_try_:
            mode = 1;
            break;
        case StatementNode::seh_catch_:
            mode = 2;
            if (stmt->sp)
            {
                left = Allocate<Optimizer::IMODE>();
                left->mode = Optimizer::i_direct;
                left->size = ISZ_OBJECT;
                left->offset = Allocate<Optimizer::SimpleExpression>();
                left->offset->type = Optimizer::se_auto;
                left->offset->sp = Optimizer::SymbolManager::Get(stmt->sp);
            }
            break;
        case StatementNode::seh_fault_:
            mode = 3;
            break;
        case StatementNode::seh_finally_:
            mode = 4;
            break;
        default:
            Optimizer::gen_label(label);
            return;
        }
        Optimizer::gen_icode(Optimizer::i_seh, nullptr, left, nullptr);
        Optimizer::intermed_tail->alwayslive = true;
        Optimizer::intermed_tail->sehMode = mode | 0x80;
        Optimizer::intermed_tail->dc.v.label = label;
        genstmt(stmt->lower, funcsp, 0);
        Optimizer::gen_icode(Optimizer::i_seh, nullptr, left, nullptr);
        Optimizer::intermed_tail->sehMode = mode;
        Optimizer::intermed_tail->dc.v.label = label;
        //        if (stmt && stmt->type == StatementNode::seh_try_)
        //            break;
    }
    Optimizer::gen_label(label);
}
void gen_except(bool begin, xcept* xc)
{
    auto tab = Allocate<Optimizer::IMODE>();
    tab->mode = Optimizer::i_immed;
    tab->size = ISZ_ADDR;
    tab->offset = Optimizer::SymbolManager::Get(MakeExpression(ExpressionNode::auto_, xc->xctab));
    if (begin)
    {
        auto lab = Allocate<Optimizer::IMODE>();
        lab->mode = Optimizer::i_immed;
        lab->size = ISZ_ADDR;
        lab->offset = Optimizer::SymbolManager::Get(MakeExpression(ExpressionNode::pc_, xc->xclab));
        gen_icode(Optimizer::i_beginexcept, nullptr, tab, lab);
    }
    else
    {
        gen_icode(Optimizer::i_endexcept, nullptr, tab, nullptr);
    }
}
/*
 *      generate a return statement.
 */
void genreturn(Statement* stmt, SYMBOL* funcsp, int flags, Optimizer::IMODE* allocaAP)
{
    bool refbyval = false;
    Optimizer::IMODE* ap = nullptr, * ap1 = nullptr, * ap3;
    EXPRESSION ep;
    int size;
    /* returns a value? */
    if (stmt != 0 && stmt->select != 0 && (!(flags & F_NORETURNVALUE)  || expressionHasSideEffects(stmt->select) || HasIncDec()))
    {
        // the return type should NOT be an array at this point unless it is a managed one...
        if (funcsp->tp->BaseType()->btp &&
            (funcsp->tp->BaseType()->btp->IsStructured() || funcsp->tp->BaseType()->btp->IsBitInt() ||
                                          (funcsp->tp->BaseType()->btp->IsArray() && (Optimizer::architecture == ARCHITECTURE_MSIL)) ||
                                          funcsp->tp->BaseType()->btp->BaseType()->type == BasicType::memberptr_))
        {
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
            {
                EXPRESSION* exp = stmt->select;
                while (IsCastValue(exp))
                    exp = exp->left;
                if (isconstzero(&stdint, exp))
                    ap = Optimizer::make_immed(ISZ_OBJECT, 0);  // LDNULL
                else
                    ap = gen_expr(funcsp, stmt->select, F_OBJECT | F_INRETURN, ISZ_ADDR);
                DumpIncDec(funcsp);
                size = ISZ_ADDR;
            }
            else
            {
                if (funcsp->tp->BaseType()->btp->IsStructured() && funcsp->tp->BaseType()->btp->BaseType()->sp->sb->structuredAliasType)
                {
                    EXPRESSION* exp = stmt->select;
                    size = natural_size(exp);

                    if (!(flags & F_RETURNSTRUCTBYVALUE) && inlineSymStructPtr.size() &&
                        funcsp->tp->BaseType()->btp->BaseType()->type != BasicType::memberptr_)
                    {
                        if (TakeAddress(&exp))
                        {
                            auto exp1 = exp;
                            while (IsCastValue(exp1))
                                exp1 = exp1->left;
                            if (exp1->type == ExpressionNode::thisref_)
                                exp1 = exp1->left;
                            if (exp1->type == ExpressionNode::callsite_)
                            {
                                auto tpx = exp1->v.sp->tp->BaseType()->btp->BaseType();
                                if (!tpx->IsStructured() || !tpx->sp->sb->structuredAliasType)
                                {
                                    exp = stmt->select; 
                                }
                            }
                        }
                    }

                    ap1 = gen_expr(funcsp, exp, flags & F_RETURNSTRUCTBYVALUE, size);

                    ap = Optimizer::LookupLoadTemp(nullptr, ap1);
                    if (ap != ap1)
                    {
                        Optimizer::gen_icode(Optimizer::i_assn, ap, ap1, nullptr);
                    }

                    DumpIncDec(funcsp);
                    if (abs(size) < ISZ_UINT)
                        size = -ISZ_UINT;
                }
                else if (inlineSymStructPtr.size())
                {
                    ap = gen_expr(funcsp, stmt->select, 0, ISZ_ADDR);
                    DumpIncDec(funcsp);
                    ap = gen_expr(funcsp, inlineSymStructPtr.back(), 0, ISZ_ADDR);
                    size = ISZ_ADDR;
                }
                else
                {
                    EXPRESSION* en = AnonymousVar(StorageClass::parameter_, &stdpointer);
                    SYMBOL* sym = en->v.sp;
                    ap = gen_expr(funcsp, stmt->select, 0, ISZ_ADDR);
                    DumpIncDec(funcsp);
                    sym->sb->offset = Optimizer::chosenAssembler->arch->retblocksize;
                    sym->name = "__retblock";
                    sym->sb->retblk = true;
                    sym->sb->allocate = false;
                    // instead of 'front()' the next line did table[0] without the ->p
                    if ((funcsp->sb->attribs.inheritable.linkage == Linkage::pascal_) && funcsp->tp->BaseType()->syms->size() &&
                        ((SYMBOL*)funcsp->tp->BaseType()->syms->front())->tp->type != BasicType::void_)
                    {
                        sym->sb->offset = funcsp->sb->paramsize;
                    }
                    Optimizer::SimpleSymbol* ssym = Optimizer::SymbolManager::Get(sym);
                    ssym->offset = sym->sb->offset;
                    ssym->allocate = false;
                    Dereference(&stdpointer, &en);
                    ap = gen_expr(funcsp, en, 0, ISZ_ADDR);
                    size = ISZ_ADDR;
                }
            }
        }
        else
        {
            auto tpr = (Type*)nullptr;
            if ((flags & F_RETURNREFBYVALUE) && funcsp->sb->retcount == 1 && funcsp->tp->BaseType()->btp->IsRef())
            {
                 tpr = funcsp->tp->BaseType()->btp->BaseType()->btp;
                 if (tpr->IsStructured())
                 {
                    tpr = tpr->BaseType()->sp->sb->structuredAliasType;
                 }
                 else
                 {
                    tpr = nullptr;
                 }
            }
            if (tpr && tpr->IsInt() && tpr->size <= Optimizer::chosenAssembler->arch->word_size)
            {
                size = SizeFromType(tpr);
                EXPRESSION* exp = stmt->select;
                ap3 = gen_expr(funcsp, exp, F_RETURNREFBYVALUE, size);
                DumpIncDec(funcsp);
                ap = Optimizer::LookupLoadTemp(nullptr, ap3);
                if (ap != ap3)
                {
                    Optimizer::gen_icode(Optimizer::i_assn, ap, ap3, nullptr);
                }
                if (!ap3->returnRefByVal)
                {
                    ap = indnode(ap, SizeFromType(tpr));
                }
                refbyval = true;
                if (abs(size) < ISZ_UINT)
                    size = -ISZ_UINT;
            }
            else
            {
                EXPRESSION* exp = stmt->select;
                size = natural_size(exp);
                if (size == ISZ_OBJECT && isconstzero(&stdint, exp))
                {
                    ap3 = Optimizer::make_immed(ISZ_OBJECT, 0);  // LDNULL
                }
                else
                {
                    if ((flags & F_RETURNSTRUCTBYVALUE) && inlineSymThisPtr.size() && funcsp->sb->isConstructor && funcsp->sb->parentClass->sb->structuredAliasType)
                    {
                        exp = MakeExpression(ExpressionNode::structadd_, exp, MakeIntExpression(ExpressionNode::c_i_, 0));
                        Dereference(funcsp->sb->parentClass->sb->structuredAliasType, &exp);
                    }
                    ap3 = gen_expr(funcsp, exp, flags & F_RETURNSTRUCTBYVALUE, size);
                }
                DumpIncDec(funcsp);
                ap = Optimizer::LookupLoadTemp(nullptr, ap3);
                if (ap != ap3)
                {
                    Optimizer::gen_icode(Optimizer::i_assn, ap, ap3, nullptr);
                }
                if (abs(size) < ISZ_UINT)
                    size = -ISZ_UINT;
            }
        }
    }
    else
    {
        DumpIncDec(funcsp);
    }
    if (stmt && stmt->destexp)
    {
        gen_expr(funcsp, stmt->destexp, F_NOVALUE, ISZ_ADDR);
    }
    if (ap && (inlineSymThisPtr.size() || !funcsp->tp->BaseType()->btp->IsVoid() || funcsp->sb->isConstructor))
    {
        bool needsOCP = funcsp->sb->retcount <= 1;
        if (returnImode)
        {
            ap1 = returnImode;
        }
        else
        {
            ap1 = Optimizer::tempreg(ap->size, 0);
            ap1->returnRefByVal = refbyval;
            if (!inlineSymThisPtr.size())
            {
                ap1->retval = true;
                needsOCP = false;
            }
            else
            {
                returnImode = ap1;
                returnImode->offset->sp->pushedtotemp = !needsOCP;
            }
        }
        Optimizer::gen_icode(Optimizer::i_assn, ap1, ap, 0);
        Optimizer::intermed_tail->needsOCP = needsOCP;
    }
    /* create the return or a branch to the return
     * return is put at Keyword::end_ of function...
     */
    if (flags & F_NEEDEPILOG)
    {
        int retsize = 0;
        if (funcsp->sb->attribs.inheritable.linkage == Linkage::pascal_ || funcsp->sb->attribs.inheritable.linkage == Linkage::stdcall_)
        {
            retsize = funcsp->sb->paramsize;
        }
        if (!inlineSymThisPtr.size() || retLabs[retlab] >= 2)
        {
            // main function or multiple returns, label is needed
            Optimizer::gen_label(retlab);
        }
        else if (retLabs[retlab] == 1)
        {
            // only one return statement, label isn't needed
            // and get rid of the goto as well...
            Optimizer::QUAD* find = Optimizer::intermed_tail;
            Optimizer::Block* b = find->block;
            while (b == find->block && (find->dc.opcode == Optimizer::i_line || find->ignoreMe))
                find = find->back;
            if (b == find->block && find->dc.opcode == Optimizer::i_goto && find->dc.v.label == retlab)
            {
                find->dc.opcode = Optimizer::i_nop;
                find->dc.v.label = 0;
            }
            else
            {
                // might fall off the Keyword::end_ without a return statement...
                Optimizer::gen_label(retlab);
            }
        }
        else
        {
            Optimizer::gen_label(retlab);
        }
        Optimizer::intermed_tail->retcount = retcount;
        if (!inlineSymThisPtr.size())
        {
            if (allocaAP && (Optimizer::architecture != ARCHITECTURE_MSIL))
            {
                Optimizer::gen_icode(Optimizer::i_loadstack, 0, allocaAP, 0);
            }
            if (Optimizer::cparams.prm_xcept && Optimizer::SymbolManager::Get(funcsp)->xc && funcsp->sb->xc && funcsp->sb->xc->xclab)
                gen_except(false, funcsp->sb->xc);
            SubProfilerData();
            if (returnSym && !funcsp->tp->BaseType()->btp->IsVoid())
            {
                ap1 = Optimizer::tempreg(returnSym->size, 0);
                ap1->retval = true;
                Optimizer::gen_icode(Optimizer::i_assn, ap1, returnSym, nullptr);
            }
            Optimizer::gen_icode(Optimizer::i_epilogue, 0, 0, 0);
            if (funcsp->sb->attribs.inheritable.linkage == Linkage::interrupt_ || funcsp->sb->attribs.inheritable.linkage == Linkage::fault_)
            {
                Optimizer::gen_icode(Optimizer::i_popcontext, 0, 0, 0);
                Optimizer::gen_icode(Optimizer::i_rett, 0,
                                     Optimizer::make_immed(ISZ_UINT, funcsp->sb->attribs.inheritable.linkage == Linkage::interrupt_),
                                     0);
            }
            else
            {
                Optimizer::gen_icode(Optimizer::i_ret, 0, Optimizer::make_immed(ISZ_UINT, retsize), nullptr);
            }
        }
    }
    else
    {
        if (returnSym)
        {
            if (!ap1)
            {
                ap1 = Optimizer::tempreg(returnSym->size, 0);
                ap1->retval = true;
            }
            Optimizer::gen_icode(Optimizer::i_assn, returnSym, ap1, nullptr);
        }
        /* not using gen_igoto because it will make a new block */
        Optimizer::gen_icode(Optimizer::i_goto, nullptr, nullptr, nullptr);
        Optimizer::intermed_tail->dc.v.label = retlab;
        retLabs[retlab]++;
        retcount++;
    }
}

void gen_varstart(void* exp)
{
    if (Optimizer::cparams.prm_debug)
    {
        Optimizer::IMODE* ap = Allocate<Optimizer::IMODE>();
        ap->mode = Optimizer::i_immed;
        ap->offset = Optimizer::SymbolManager::Get((expr*)exp);
        ap->size = ISZ_ADDR;
        Optimizer::gen_icode(Optimizer::i_varstart, 0, ap, 0);
    }
}
void gen_func(void* exp, int start)
{
    Optimizer::IMODE* ap = Allocate<Optimizer::IMODE>();
    ap->mode = Optimizer::i_immed;
    ap->offset = Optimizer::SymbolManager::Get((expr*)exp);
    ap->size = ISZ_ADDR;
    Optimizer::gen_icode(Optimizer::i_func, 0, ap, 0)->dc.v.label = start;
}
void gen_dbgblock(int start) { Optimizer::gen_icode(start ? Optimizer::i_dbgblock : Optimizer::i_dbgblockend, 0, 0, 0); }

void gen_asm(Statement* stmt)
/*
 * generate an ASM statement
 */
{
#ifndef ORANGE_NO_INASM
    Optimizer::QUAD* newQuad;
    newQuad = Allocate<Optimizer::QUAD>();
    newQuad->dc.opcode = Optimizer::i_passthrough;
    newQuad->dc.left = (Optimizer::IMODE*)stmt->select; /* actually is defined by the INASM module*/
    // if (Optimizer::chosenAssembler->gen->adjustcodelab)
    //    Optimizer::chosenAssembler->gen->adjustcodelab(newQuad->dc.left, codeLabelOffset);
    adjustcodelab(newQuad->dc.left, codeLabelOffset);
    Optimizer::flush_dag();
    add_intermed(newQuad);
#endif
}
void gen_asmdata(Statement* stmt)
{
    Optimizer::QUAD* newQuad;
    newQuad = Allocate<Optimizer::QUAD>();
    newQuad->dc.opcode = Optimizer::i_datapassthrough;
    newQuad->dc.left = (Optimizer::IMODE*)stmt->select; /* actually is defined by the INASM module*/
    Optimizer::flush_dag();
    add_intermed(newQuad);
}

/*-------------------------------------------------------------------------*/

Optimizer::IMODE* genstmt(std::list<Statement*>* stmts, SYMBOL* funcsp, int flags)
/*
 *      genstmt will generate a statement and follow the next pointer
 *      until the block is generated.
 */
{
    Optimizer::IMODE* rv = nullptr;
    if (stmts)
    {
        auto il = stmts->begin();
        auto ile = stmts->end();

        while (il != ile)
        {
            auto stmt = *il;
            switch (stmt->type)
            {
                case StatementNode::nop_:
                    break;
                case StatementNode::varstart_:
                    gen_varstart(stmt->select);
                    break;
                case StatementNode::dbgblock_:
                    gen_dbgblock(stmt->label);
                    break;
                    break;
                case StatementNode::block_:
                    rv = genstmt(stmt->lower, funcsp, flags);
                    genstmt(stmt->blockTail, funcsp, flags);
                    break;
                case StatementNode::label_:
                    Optimizer::gen_label((int)stmt->label + codeLabelOffset);
                    if (stmt->purelabel)
                       Optimizer::intermed_tail->alwayslive = true;
                    break;
                case StatementNode::goto_:
                    if (stmt->destexp)
                        gen_expr(funcsp, stmt->destexp, F_NOVALUE, ISZ_ADDR);
                    if (stmt->indirectGoto)
                    {
                        auto ap1 = gen_expr(funcsp, stmt->select, F_NOVALUE, natural_size(stmt->select));
                        Optimizer::gen_icode(Optimizer::i_computedgoto, nullptr, ap1, nullptr);
                    }
                    else
                    {
                        Optimizer::gen_igoto(Optimizer::i_goto, (int)stmt->label + codeLabelOffset);
                    }
                    break;
                case StatementNode::asmgoto_:
                    Optimizer::gen_igoto(Optimizer::i_asmgoto, (int)stmt->label + codeLabelOffset);
                    break;
                case StatementNode::asmcond_:
                    Optimizer::gen_igoto(Optimizer::i_asmcond, (int)stmt->label + codeLabelOffset);
                    break;
                case StatementNode::try_:
                    gen_try(funcsp, stmt, stmt->label + codeLabelOffset, stmt->endlabel + codeLabelOffset,
                            stmt->breaklabel + codeLabelOffset, stmt->lower);
                    break;
                case StatementNode::catch_:
                {
                    int breaklab = 0;
                    while (stmt->type == StatementNode::catch_)
                    {
                        // the following adjustment to altlabel is required to get the XT info proper
                        gen_catch(funcsp, stmt, stmt->altlabel += codeLabelOffset, stmt->breaklabel + codeLabelOffset, stmt->lower);
                        breaklab = stmt->breaklabel + codeLabelOffset;
                        ++il;
                        if (il == ile)
                            break;
                        stmt = *il;
                    }
                    Optimizer::gen_label(breaklab);
                    continue;
                }
                case StatementNode::seh_try_:
                case StatementNode::seh_catch_:
                case StatementNode::seh_finally_:
                case StatementNode::seh_fault_: {
                    auto ilx = il;
                    while (ilx != ile && (*ilx)->type != StatementNode::seh_try_)
                        ilx++;
                    std::list<Statement*> stmts(il, ilx);
                    gen___try(funcsp, stmts);
                    if ((*il)->destexp)
                    {
                        gen_expr(funcsp, (*il)->destexp, F_NOVALUE, ISZ_ADDR);
                    }
                    il = ilx;
                    continue;
                }
                case StatementNode::expr_:
                case StatementNode::declare_:
                    if (stmt->select)
                        rv = gen_expr(funcsp, stmt->select, F_NOVALUE | F_RETURNSTRUCTBYVALUE, natural_size(stmt->select));
                    break;
                case StatementNode::return_:
                    genreturn(stmt, funcsp, flags, nullptr);
                    break;
                case StatementNode::line_:
                    Optimizer::gen_line(stmt->lineData);
                    break;
                case StatementNode::select_:
                    genselect(stmt, funcsp, true);
                    break;
                case StatementNode::notselect_:
                    genselect(stmt, funcsp, false);
                    break;
                case StatementNode::switch_:
                    genxswitch(stmt, funcsp);
                    break;
                case StatementNode::genword_:
                    gen_genword(stmt, funcsp);
                    break;
                case StatementNode::passthrough_:
                    gen_asm(stmt);
                    break;
                case StatementNode::datapassthrough_:
                    gen_asmdata(stmt);
                    break;
                default:
                    diag("unknown statement.");
                    break;
            }
            if ((*il)->type != StatementNode::return_ && (*il)->type != StatementNode::goto_ && (*il)->destexp)
            {
                gen_expr(funcsp, (*il)->destexp, F_NOVALUE, ISZ_ADDR);
            }
            ++il;
        }
    }
    return rv;
}
// simplify addresses to make the aliasing work faster...
// this is necessary because the inlining replaces what were
// references to 'this' with addresses
//
// it also improves teh code generation when inlining functions
// with parameters passed by reference...
typedef struct _data
{
    struct _data* next;
    Optimizer::IMODE* mem;
    Optimizer::IMODE* addr;
} DATA;
static DATA* buckets[32];
static void StoreInBucket(Optimizer::IMODE* mem, Optimizer::IMODE* addr)
{
    DATA* lst;
    int bucket = (int)(intptr_t)mem;
    bucket = (bucket >> 16) ^ (bucket >> 8) ^ (bucket >> 2);
    bucket %= 32;
    lst = buckets[bucket];
    while (lst)
    {
        if (lst->mem == mem)
        {
            if (lst->addr != addr)
                lst->mem->offset->sp->noCoalesceImmed = true;
            return;
        }
        lst = lst->next;
    }
    lst = Allocate<DATA>();
    lst->mem = mem;
    lst->addr = addr;
    lst->next = buckets[bucket];
    buckets[bucket] = lst;
}
static Optimizer::IMODE* GetBucket(Optimizer::IMODE* mem)
{
    DATA* lst;
    int bucket = (int)(intptr_t)mem;
    bucket = (bucket >> 16) ^ (bucket >> 8) ^ (bucket >> 2);
    bucket %= 32;
    lst = buckets[bucket];
    while (lst)
    {
        if (lst->mem == mem)
            return lst->addr;
        lst = lst->next;
    }
    return nullptr;
}
/*-------------------------------------------------------------------------*/
static void InsertParameterThunks(SYMBOL* funcsp, Optimizer::Block* b)
{
    Optimizer::QUAD *old, *oldit;
    Optimizer::Block* ocb = Optimizer::currentBlock;
    old = b->head->fwd;
    while (old != b->tail && old->dc.opcode != Optimizer::i_label)
        old = old->fwd;
    old = old->fwd;
    oldit = Optimizer::intermed_tail;
    Optimizer::intermed_tail = old->back;
    Optimizer::intermed_tail->fwd = nullptr;
    Optimizer::currentBlock = b;
    for (auto sym : *funcsp->tp->BaseType()->syms)
    {
        Optimizer::SimpleSymbol* simpleSym = Optimizer::SymbolManager::Get(sym);
        if (sym->tp->type == BasicType::void_ || sym->tp->type == BasicType::ellipse_ || sym->tp->IsStructured())
        {
            continue;
        }
        if (!simpleSym->imvalue || simpleSym->imaddress)
        {
            continue;
        }
        if (funcsp->sb->oldstyle && sym->tp->type == BasicType::float_)
        {
            Optimizer::IMODE* right = Allocate<Optimizer::IMODE>();
            *right = *simpleSym->imvalue;
            right->size = ISZ_DOUBLE;
            if (!Optimizer::chosenAssembler->arch->hasFloatRegs)
            {
                Optimizer::IMODE* dp = Optimizer::tempreg(ISZ_DOUBLE, 0);
                Optimizer::IMODE* fp = Optimizer::tempreg(ISZ_FLOAT, 0);
                /* oldstyle float gets promoted from double */
                Optimizer::gen_icode(Optimizer::i_assn, dp, right, 0);
                Optimizer::gen_icode(Optimizer::i_assn, fp, dp, 0);
                Optimizer::gen_icode(Optimizer::i_assn, simpleSym->imvalue, fp, 0);
            }
        }
    }
    Optimizer::currentBlock = ocb;
    if (old->back == b->tail)
    {
        b->tail = Optimizer::intermed_tail;
    }
    old->back = Optimizer::intermed_tail;
    Optimizer::intermed_tail->fwd = old;
    Optimizer::intermed_tail = oldit;
}
void CopyVariables(SYMBOL* funcsp)
{
    Optimizer::functionVariables.clear();
    for (SymbolTable<SYMBOL>* syms = funcsp->sb->inlineFunc.syms; syms; syms = syms->Next())
    {
        for (auto sym1 : *syms)
        {
            if (sym1->sb->storage_class == StorageClass::auto_ || sym1->sb->storage_class == StorageClass::parameter_)
            {
                Optimizer::SimpleSymbol* sym = Optimizer::SymbolManager::Get(sym1);
                sym->i = syms->Block();
                Optimizer::functionVariables.push_back(sym);
            }
        }
    }
}
static void SetReturnSym(SYMBOL* funcsp)
{
    if (Optimizer::architecture == ARCHITECTURE_MSIL && !funcsp->tp->BaseType()->btp->IsVoid())
    {
        auto exp = AnonymousVar(StorageClass::auto_, funcsp->tp->BaseType()->btp);
        auto sym = exp->v.sp;
        sym->sb->anonymous = false;
        Optimizer::IMODE* ap = Allocate<Optimizer::IMODE>();
        auto sym2 = Optimizer::SymbolManager::Get(sym);
        sym2->imvalue = ap;
        ap->offset = Optimizer::SymbolManager::Get(exp);
        ap->offset->sizeFromType = sym2->tp->sizeFromType;
        ap->mode = Optimizer::i_direct;
        ap->size = sym2->tp->sizeFromType;
        returnSym = ap;
    }
    else
    {
        returnSym = nullptr;
    }
}
void genfunc(SYMBOL* funcsp, bool doOptimize)
/*
 *      generate a function body and dump the icode
 */
{
    int flags = 0;
    retLabs.clear();        
    rttiStatements.clear();
    Optimizer::IMODE* oldReturnImode = returnImode;
    Optimizer::IMODE* allocaAP = nullptr;
    SYMBOL* oldCurrentFunc;
    Optimizer::SimpleSymbol* oldCurrentFunction;
    EXPRESSION* funcexp = MakeExpression(ExpressionNode::global_, funcsp);
    SYMBOL* tmpl = funcsp;
    if (TotalErrors())
        return;

    insert_file_constructor(funcsp);

    Optimizer::SymbolManager::Get(funcsp, true)->generated = true;
    // if returning struct by val set up an expression for the return value
    if (funcsp->tp->BaseType()->btp->IsStructured())
    {
        auto spr = funcsp->tp->BaseType()->btp->BaseType()->sp->sb->structuredAliasType;
        if (spr)
        {
            flags |= F_RETURNSTRUCTBYVALUE;
            auto val = tempVar(spr);
            auto src = gen_expr(funcsp, val, F_STORE, natural_size(val));
            val = makeParamSubs(val->left, src);
            Dereference(&stdpointer, &val);
            inlineSymStructPtr.push_back(val);
        }
    }

    SetReturnSym(funcsp);
    oldCurrentFunction = currentFunction;
    currentFunction = Optimizer::SymbolManager::Get(funcsp);
    currentFunction->initialized = true;
    CopyVariables(funcsp);
    returnImode = nullptr;
    while (tmpl)
        if (tmpl->sb->templateLevel)
            break;
        else
            tmpl = tmpl->sb->parentClass;
    /* for inlines */
    codeLabelOffset = Optimizer::nextLabel - INT_MIN;
    Optimizer::nextLabel += funcsp->sb->labelCount;

    Optimizer::temporarySymbols.clear();
    contlab = breaklab = -1;
    structret_imode = 0;
    Optimizer::tempCount = 0;
    Optimizer::blockCount = 0;
    Optimizer::exitBlock = 0;
    consIndex = 0;
    retcount = 0;
    oldCurrentFunc = theCurrentFunc;
    theCurrentFunc = funcsp;
    iexpr_func_init();

    if (funcsp->sb->xc && funcsp->sb->xc->xctab)
    {
        EXPRESSION* exp;
        Optimizer::temporarySymbols.push_back(Optimizer::SymbolManager::Get(funcsp->sb->xc->xctab));
        xcexp = MakeExpression(ExpressionNode::auto_, funcsp->sb->xc->xctab);
        xcexp = MakeExpression(ExpressionNode::add_, xcexp, MakeIntExpression(ExpressionNode::c_i_, XCTAB_INDEX_OFS));
    }
    else
    {
        xcexp = nullptr;
    }
    Optimizer::cseg();
    Optimizer::gen_line(funcsp->sb->linedata);
    gen_func(funcexp, 1);

    /* in C99 inlines can clash if declared 'extern' in multiple modules */
    /* in C++ we introduce virtual functions that get coalesced at link time */
    if (!currentFunction->isinternal && (funcsp->sb->attribs.inheritable.linkage4 == Linkage::virtual_ || tmpl))
    {
        funcsp->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
        Optimizer::gen_virtual(Optimizer::SymbolManager::Get(funcsp), Optimizer::vt_code);
    }
    else
    {
        Optimizer::gen_funcref(Optimizer::SymbolManager::Get(funcsp));
        Optimizer::gen_strlab(Optimizer::SymbolManager::Get(funcsp)); /* name of function */
    }
    Optimizer::addblock(-1);
    if (funcsp->sb->attribs.inheritable.linkage == Linkage::interrupt_ || funcsp->sb->attribs.inheritable.linkage == Linkage::fault_)
    {
        Optimizer::gen_icode(Optimizer::i_pushcontext, 0, 0, 0);
        /*		if (funcsp->sb->loadds) */
        /*	        Optimizer::gen_icode(Optimizer::i_loadcontext, 0,0,0); */
    }
    Optimizer::gen_icode(Optimizer::i_prologue, 0, 0, 0);
    if (Optimizer::cparams.prm_debug)
    {
        if (funcsp->tp->BaseType()->syms->size() && ((SYMBOL*)funcsp->tp->BaseType()->syms->front())->sb->thisPtr)
        {
            EXPRESSION* exp = MakeExpression(ExpressionNode::auto_, ((SYMBOL*)funcsp->tp->BaseType()->syms->front()));
            exp->v.sp->tp->used = true;
            gen_varstart(exp);
        }
    }
    else
    {
        if (funcsp->tp->BaseType()->syms->size() && ((SYMBOL*)funcsp->tp->BaseType()->syms->front())->sb->thisPtr)
        {
            baseThisPtr = Optimizer::SymbolManager::Get((SYMBOL*)funcsp->tp->BaseType()->syms->front());
        }
    }
    Optimizer::gen_label(startlab);
    AddProfilerData(funcsp);
    if (Optimizer::cparams.prm_xcept && Optimizer::SymbolManager::Get(funcsp)->xc && funcsp->sb->xc && funcsp->sb->xc->xclab)
    {
         gen_except(true, funcsp->sb->xc);
    }
    /*    if (funcsp->sb->loadds && funcsp->sb->farproc) */
    /*	        Optimizer::gen_icode(Optimizer::i_loadcontext, 0,0,0); */
    StatementGenerator::AllocateLocalContext(emptyBlockdata, funcsp, Optimizer::nextLabel++);
    if (funcsp->sb->allocaUsed && (Optimizer::architecture != ARCHITECTURE_MSIL))
    {
        EXPRESSION* allocaExp = AnonymousVar(StorageClass::auto_, &stdpointer);
        allocaAP = gen_expr(funcsp, allocaExp, 0, ISZ_ADDR);
        Optimizer::gen_icode(Optimizer::i_savestack, 0, allocaAP, 0);
    }
    /* Generate the icode */
    /* LCSE is done while code is generated */
    genstmt(funcsp->sb->inlineFunc.stmt->front()->lower, funcsp, flags);
    if (funcsp->sb->inlineFunc.stmt->front()->blockTail)
    {
        Optimizer::gen_icode(Optimizer::i_functailstart, 0, 0, 0);
        genstmt(funcsp->sb->inlineFunc.stmt->front()->blockTail, funcsp, 0);
        Optimizer::gen_icode(Optimizer::i_functailend, 0, 0, 0);
    }
    genreturn(0, funcsp, F_NEEDEPILOG, allocaAP);
    gen_func(funcexp, 0);
    tFree();

    InsertParameterThunks(funcsp, Optimizer::blockArray[1]);
    StatementGenerator::FreeLocalContext(emptyBlockdata, funcsp, Optimizer::nextLabel++);

    /* Code gen from icode */
    Optimizer::SymbolManager::Get(funcsp)->allocaUsed = funcsp->sb->allocaUsed;
    Optimizer::AddFunction();

    if (!currentFunction->isinternal && (funcsp->sb->attribs.inheritable.linkage4 == Linkage::virtual_ || tmpl))
        Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(funcsp));

    StatementGenerator::AllocateLocalContext(emptyBlockdata, funcsp, Optimizer::nextLabel);
    XTDumpTab(funcsp);
    StatementGenerator::FreeLocalContext(emptyBlockdata, funcsp, Optimizer::nextLabel);

    Optimizer::intermed_head = nullptr;
    theCurrentFunc = oldCurrentFunc;
    currentFunction = oldCurrentFunction;
    returnImode = oldReturnImode;
    if (Optimizer::blockCount > maxBlocks)
        maxBlocks = Optimizer::blockCount;
    if (Optimizer::tempCount > maxTemps)
        maxTemps = Optimizer::tempCount;

    // this is explicitly to clean up the this pointer
    for (auto sym : *funcsp->tp->BaseType()->syms)
    {
        if (sym->sb->storage_class == StorageClass::parameter_)
        {
            Optimizer::SimpleSymbol* simpleSym = Optimizer::SymbolManager::Get(sym);
            simpleSym->imind = nullptr;
            simpleSym->imvalue = nullptr;
            simpleSym->imaddress = nullptr;
        }
    }
    baseThisPtr = nullptr;
    Optimizer::nextLabel += 2;  // temporary
    inlineSymStructPtr.clear();
}
void genASM(std::list<Statement*>* st)
{
    Optimizer::cseg();
    contlab = breaklab = -1;
    structret_imode = 0;
    Optimizer::tempCount = 0;
    Optimizer::blockCount = 0;
    Optimizer::exitBlock = 0;
    genstmt(st, nullptr, 0);
}
}  // namespace Parser
