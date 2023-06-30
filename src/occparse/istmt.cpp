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
#include "template.h"
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
#include "help.h"
#include "beinterf.h"
#include "inasm.h"
#include "optmodules.h"
#include "symtab.h"
#include "constopt.h"
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
    return make_ioffset(intNode(en_c_i, i));
}

/*-------------------------------------------------------------------------*/

void gen_genword(STATEMENT* stmt, SYMBOL* funcsp)
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
        sym->sb->storage_class = sc_external;
        sym->name = sym->sb->decoratedName = litlate(name);
        sym->tp = MakeType(isproc ? bt_func : bt_int);
        sym->sb->safefunc = true;
        globalNameSpace->front()->syms->Add(sym);
        auto osym = Optimizer::SymbolManager::Get(sym);
        Optimizer::externalSet.insert(osym);
        Optimizer::externals.push_back(osym);
    }
    else
    {
        if (sym->sb->storage_class == sc_overloads)
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
    Optimizer::gen_icode(Optimizer::i_parmadj, 0, Optimizer::make_parmadj(size), Optimizer::make_parmadj(size));
    result = Optimizer::tempreg(ISZ_UINT, 0);
    result->retval = true;
    return result;
}
static void AddProfilerData(SYMBOL* funcsp)
{
    LCHAR* pname;
    if (Optimizer::cparams.prm_profiler)
    {
        STRING* string;
        int i;
        int l = strlen(funcsp->sb->decoratedName);
        pname = Allocate<LCHAR>(l + 1);
        for (i = 0; i < l + 1; i++)
            pname[i] = funcsp->sb->decoratedName[i];
        string = Allocate<STRING>();
        string->strtype = l_astr;
        string->size = 1;
        string->pointers = Allocate<Optimizer::SLCHAR*>();
        string->pointers[0] = Allocate<Optimizer::SLCHAR>();
        string->pointers[0]->str = pname;
        string->pointers[0]->count = l;
        string->suffix = nullptr;
        stringlit(string);
        plabel = string->label;
        Optimizer::gen_icode(Optimizer::i_parm, 0, imake_label(plabel), 0);
        call_library("__profile_in", getSize(bt_pointer));
    }
}

//-------------------------------------------------------------------------

void SubProfilerData(void)
{
    if (Optimizer::cparams.prm_profiler)
    {
        Optimizer::gen_icode(Optimizer::i_parm, 0, imake_label(plabel), 0);
        call_library("__profile_out", getSize(bt_pointer));
    }
}


EXPRESSION* tempVar(TYPE* tp, bool global) 
{
    if (global)
        anonymousNotAlloc++;
    auto val = anonymousVar(global ? sc_global : sc_auto, tp);
    if (global)
    {
        insertInitSym(val->v.sp);
        anonymousNotAlloc--;
    }
    auto expx = Optimizer::SymbolManager::Get(val);
    Optimizer::cacheTempSymbol(expx->sp);
    expx->sp->anonymous = false;
    expx->sp->allocate = false;
    deref(tp, &val);
    return val;
}
EXPRESSION *makeParamSubs(EXPRESSION* left, Optimizer::IMODE* im)
{
    auto val = exprNode(en_paramsubstitute, left, 0);
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

void genxswitch(STATEMENT* stmt, SYMBOL* funcsp)
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
        EXPRESSION* en = anonymousVar(sc_auto, &stdint);
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

void genselect(STATEMENT* stmt, SYMBOL* funcsp, bool jmptrue)
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
static void gen_try(SYMBOL* funcsp, STATEMENT* stmt, int startLab, int endLab, int transferLab, std::list<STATEMENT*>* lower)
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
static void gen_catch(SYMBOL* funcsp, STATEMENT* stmt, int startLab, int transferLab, std::list<STATEMENT*>* lower)
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
static void gen___try(SYMBOL* funcsp, std::list<STATEMENT*> stmts)
{
    int label = Optimizer::nextLabel++;
    for (auto stmt : stmts)
    {
        int mode = 0;
        Optimizer::IMODE* left = nullptr;
        switch (stmt->type)
        {
        case st___try:
            mode = 1;
            break;
        case st___catch:
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
        case st___fault:
            mode = 3;
            break;
        case st___finally:
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
        //        if (stmt && stmt->type == st___try)
        //            break;
    }
    Optimizer::gen_label(label);
}
void gen_except(bool begin, xcept* xc)
{
    auto tab = Allocate<Optimizer::IMODE>();
    tab->mode = Optimizer::i_immed;
    tab->size = ISZ_ADDR;
    tab->offset = Optimizer::SymbolManager::Get(varNode(en_auto, xc->xctab));
    if (begin)
    {
        auto lab = Allocate<Optimizer::IMODE>();
        lab->mode = Optimizer::i_immed;
        lab->size = ISZ_ADDR;
        lab->offset = Optimizer::SymbolManager::Get(varNode(en_pc, xc->xclab));
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
void genreturn(STATEMENT* stmt, SYMBOL* funcsp, int flags, Optimizer::IMODE* allocaAP)
{
    bool refbyval = false;
    Optimizer::IMODE* ap = nullptr, * ap1 = nullptr, * ap3;
    EXPRESSION ep;
    int size;
    /* returns a value? */
    if (stmt != 0 && stmt->select != 0 && (!(flags & F_NORETURNVALUE)  || expressionHasSideEffects(stmt->select) || HasIncDec()))
    {
        // the return type should NOT be an array at this point unless it is a managed one...
        if (basetype(funcsp->tp)->btp && (isstructured(basetype(funcsp->tp)->btp) ||
                                          (isarray(basetype(funcsp->tp)->btp) && (Optimizer::architecture == ARCHITECTURE_MSIL)) ||
                                          basetype(basetype(funcsp->tp)->btp)->type == bt_memberptr))
        {
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
            {
                EXPRESSION* exp = stmt->select;
                while (castvalue(exp))
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
                if (isstructured(basetype(funcsp->tp)->btp) && basetype(basetype(funcsp->tp)->btp)->sp->sb->structuredAliasType)
                {
                    EXPRESSION* exp = stmt->select;
                    size = natural_size(exp);

                    if (!(flags & F_RETURNSTRUCTBYVALUE) && inlineSymStructPtr.size() &&
                        basetype(basetype(funcsp->tp)->btp)->type != bt_memberptr)
                    {
                        if (lvalue(exp))
                        {
                            exp = exp->left;
                            auto exp1 = exp;
                            while (castvalue(exp1))
                                exp1 = exp1->left;
                            if (exp1->type == en_thisref)
                                exp1 = exp1->left;
                            if (exp1->type == en_func)
                            {
                                auto tpx = basetype(basetype(exp1->v.sp->tp)->btp);
                                if (!isstructured(tpx) || !tpx->sp->sb->structuredAliasType)
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
                    EXPRESSION* en = anonymousVar(sc_parameter, &stdpointer);
                    SYMBOL* sym = en->v.sp;
                    ap = gen_expr(funcsp, stmt->select, 0, ISZ_ADDR);
                    DumpIncDec(funcsp);
                    sym->sb->offset = Optimizer::chosenAssembler->arch->retblocksize;
                    sym->name = "__retblock";
                    sym->sb->retblk = true;
                    sym->sb->allocate = false;
                    // instead of 'front()' the next line did table[0] without the ->p
                    if ((funcsp->sb->attribs.inheritable.linkage == lk_pascal) && basetype(funcsp->tp)->syms->size() &&
                        ((SYMBOL*)basetype(funcsp->tp)->syms->front())->tp->type != bt_void)
                    {
                        sym->sb->offset = funcsp->sb->paramsize;
                    }
                    Optimizer::SimpleSymbol* ssym = Optimizer::SymbolManager::Get(sym);
                    ssym->offset = sym->sb->offset;
                    ssym->allocate = false;
                    deref(&stdpointer, &en);
                    ap = gen_expr(funcsp, en, 0, ISZ_ADDR);
                    size = ISZ_ADDR;
                }
            }
        }
        else
        {
            auto tpr = (TYPE*)nullptr;
            if ((flags & F_RETURNREFBYVALUE) && funcsp->sb->retcount == 1 && isref(basetype(funcsp->tp)->btp))
            {
                 tpr = basetype(basetype(funcsp->tp)->btp)->btp;
                 if (isstructured(tpr))
                 {
                    tpr = basetype(tpr)->sp->sb->structuredAliasType;
                 }
                 else
                 {
                    tpr = nullptr;
                 }
            }
            if (tpr && isint(tpr) && tpr->size <= Optimizer::chosenAssembler->arch->word_size)
            {
                size = sizeFromType(tpr);
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
                    ap = indnode(ap, sizeFromType(tpr));
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
                        exp = exprNode(en_structadd, exp, intNode(en_c_i, 0));
                        deref(funcsp->sb->parentClass->sb->structuredAliasType, &exp);
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
    if (ap && (inlineSymThisPtr.size() || !isvoid(basetype(funcsp->tp)->btp) || funcsp->sb->isConstructor))
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
     * return is put at end of function...
     */
    if (flags & F_NEEDEPILOG)
    {
        int retsize = 0;
        if (funcsp->sb->attribs.inheritable.linkage == lk_pascal || funcsp->sb->attribs.inheritable.linkage == lk_stdcall)
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
            Optimizer::BLOCK* b = find->block;
            while (b == find->block && (find->dc.opcode == Optimizer::i_line || find->ignoreMe))
                find = find->back;
            if (b == find->block && find->dc.opcode == Optimizer::i_goto && find->dc.v.label == retlab)
            {
                find->dc.opcode = Optimizer::i_nop;
                find->dc.v.label = 0;
            }
            else
            {
                // might fall off the end without a return statement...
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
            if (returnSym && !isvoid(basetype(funcsp->tp)->btp))
            {
                ap1 = Optimizer::tempreg(returnSym->size, 0);
                ap1->retval = true;
                Optimizer::gen_icode(Optimizer::i_assn, ap1, returnSym, nullptr);
            }
            Optimizer::gen_icode(Optimizer::i_epilogue, 0, 0, 0);
            if (funcsp->sb->attribs.inheritable.linkage == lk_interrupt || funcsp->sb->attribs.inheritable.linkage == lk_fault)
            {
                Optimizer::gen_icode(Optimizer::i_popcontext, 0, 0, 0);
                Optimizer::gen_icode(Optimizer::i_rett, 0,
                                     Optimizer::make_immed(ISZ_UINT, funcsp->sb->attribs.inheritable.linkage == lk_interrupt),
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

void gen_asm(STATEMENT* stmt)
/*
 * generate an ASM statement
 */
{
    Optimizer::QUAD* newQuad;
    newQuad = Allocate<Optimizer::QUAD>();
    newQuad->dc.opcode = Optimizer::i_passthrough;
    newQuad->dc.left = (Optimizer::IMODE*)stmt->select; /* actually is defined by the INASM module*/
    // if (Optimizer::chosenAssembler->gen->adjust_codelab)
    //    Optimizer::chosenAssembler->gen->adjust_codelab(newQuad->dc.left, codeLabelOffset);
    adjust_codelab(newQuad->dc.left, codeLabelOffset);
    Optimizer::flush_dag();
    add_intermed(newQuad);
}
void gen_asmdata(STATEMENT* stmt)
{
    Optimizer::QUAD* newQuad;
    newQuad = Allocate<Optimizer::QUAD>();
    newQuad->dc.opcode = Optimizer::i_datapassthrough;
    newQuad->dc.left = (Optimizer::IMODE*)stmt->select; /* actually is defined by the INASM module*/
    Optimizer::flush_dag();
    add_intermed(newQuad);
}

/*-------------------------------------------------------------------------*/

Optimizer::IMODE* genstmt(std::list<STATEMENT*>* stmts, SYMBOL* funcsp, int flags)
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
                case st_nop:
                    break;
                case st_varstart:
                    gen_varstart(stmt->select);
                    break;
                case st_dbgblock:
                    gen_dbgblock(stmt->label);
                    break;
                    break;
                case st_block:
                    rv = genstmt(stmt->lower, funcsp, flags);
                    genstmt(stmt->blockTail, funcsp, flags);
                    break;
                case st_label:
                    Optimizer::gen_label((int)stmt->label + codeLabelOffset);
                    if (stmt->purelabel)
                       Optimizer::intermed_tail->alwayslive = true;
                    break;
                case st_goto:
                    if (stmt->destexp)
                        gen_expr(funcsp, stmt->destexp, F_NOVALUE, ISZ_ADDR);
                    if (stmt->indirectGoto)
                    {
                        auto ap1 = gen_expr(funcsp, stmt->select, F_NOVALUE, natural_size(stmt->select));
                        Optimizer::gen_icode(Optimizer::i_directbranch, nullptr, ap1, nullptr);
                    }
                    else
                    {
                        Optimizer::gen_igoto(Optimizer::i_goto, (int)stmt->label + codeLabelOffset);
                    }
                    break;
                case st_asmgoto:
                    Optimizer::gen_igoto(Optimizer::i_asmgoto, (int)stmt->label + codeLabelOffset);
                    break;
                case st_asmcond:
                    Optimizer::gen_igoto(Optimizer::i_asmcond, (int)stmt->label + codeLabelOffset);
                    break;
                case st_try:
                    gen_try(funcsp, stmt, stmt->label + codeLabelOffset, stmt->endlabel + codeLabelOffset,
                            stmt->breaklabel + codeLabelOffset, stmt->lower);
                    break;
                case st_catch:
                {
                    int breaklab = 0;
                    while (stmt->type == st_catch)
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
                case st___try:
                case st___catch:
                case st___finally:
                case st___fault: {
                    auto ilx = il;
                    while (ilx != ile && (*ilx)->type != st___try)
                        ilx++;
                    std::list<STATEMENT*> stmts(il, ilx);
                    gen___try(funcsp, stmts);
                    if ((*il)->destexp)
                    {
                        gen_expr(funcsp, (*il)->destexp, F_NOVALUE, ISZ_ADDR);
                    }
                    il = ilx;
                    continue;
                }
                case st_expr:
                case st_declare:
                    if (stmt->select)
                        rv = gen_expr(funcsp, stmt->select, F_NOVALUE | F_RETURNSTRUCTBYVALUE, natural_size(stmt->select));
                    break;
                case st_return:
                    genreturn(stmt, funcsp, flags, nullptr);
                    break;
                case st_line:
                    Optimizer::gen_line(stmt->lineData);
                    break;
                case st_select:
                    genselect(stmt, funcsp, true);
                    break;
                case st_notselect:
                    genselect(stmt, funcsp, false);
                    break;
                case st_switch:
                    genxswitch(stmt, funcsp);
                    break;
                case st__genword:
                    gen_genword(stmt, funcsp);
                    break;
                case st_passthrough:
                    gen_asm(stmt);
                    break;
                case st_datapassthrough:
                    gen_asmdata(stmt);
                    break;
                default:
                    diag("unknown statement.");
                    break;
            }
            if ((*il)->type != st_return && (*il)->type != st_goto && (*il)->destexp)
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
static void InsertParameterThunks(SYMBOL* funcsp, Optimizer::BLOCK* b)
{
    Optimizer::QUAD *old, *oldit;
    Optimizer::BLOCK* ocb = Optimizer::currentBlock;
    old = b->head->fwd;
    while (old != b->tail && old->dc.opcode != Optimizer::i_label)
        old = old->fwd;
    old = old->fwd;
    oldit = Optimizer::intermed_tail;
    Optimizer::intermed_tail = old->back;
    Optimizer::intermed_tail->fwd = nullptr;
    Optimizer::currentBlock = b;
    for (auto sym : *basetype(funcsp->tp)->syms)
    {
        Optimizer::SimpleSymbol* simpleSym = Optimizer::SymbolManager::Get(sym);
        if (sym->tp->type == bt_void || sym->tp->type == bt_ellipse || isstructured(sym->tp))
        {
            continue;
        }
        if (!simpleSym->imvalue || simpleSym->imaddress)
        {
            continue;
        }
        if (funcsp->sb->oldstyle && sym->tp->type == bt_float)
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
            if (sym1->sb->storage_class == sc_auto || sym1->sb->storage_class == sc_parameter)
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
    if (Optimizer::architecture == ARCHITECTURE_MSIL && !isvoid(basetype(funcsp->tp)->btp))
    {
        auto exp = anonymousVar(sc_auto, basetype(funcsp->tp)->btp);
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
    EXPRESSION* funcexp = varNode(en_global, funcsp);
    SYMBOL* tmpl = funcsp;
    if (TotalErrors())
        return;

    insert_file_constructor(funcsp);

    Optimizer::SymbolManager::Get(funcsp)->generated = true;
    // if returning struct by val set up an expression for the return value
    if (isstructured(basetype(funcsp->tp)->btp))
    {
        auto spr = basetype(basetype(funcsp->tp)->btp)->sp->sb->structuredAliasType;
        if (spr)
        {
            flags |= F_RETURNSTRUCTBYVALUE;
            auto val = tempVar(spr);
            auto src = gen_expr(funcsp, val, F_STORE, natural_size(val));
            val = makeParamSubs(val->left, src);
            deref(&stdpointer, &val);
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
        xcexp = varNode(en_auto, funcsp->sb->xc->xctab);
        xcexp = exprNode(en_add, xcexp, intNode(en_c_i, XCTAB_INDEX_OFS));
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
    if (!currentFunction->isinternal && (funcsp->sb->attribs.inheritable.linkage4 == lk_virtual || tmpl))
    {
        funcsp->sb->attribs.inheritable.linkage4 = lk_virtual;
        Optimizer::gen_virtual(Optimizer::SymbolManager::Get(funcsp), false);
    }
    else
    {
        Optimizer::gen_funcref(Optimizer::SymbolManager::Get(funcsp));
        Optimizer::gen_strlab(Optimizer::SymbolManager::Get(funcsp)); /* name of function */
    }
    Optimizer::addblock(-1);
    if (funcsp->sb->attribs.inheritable.linkage == lk_interrupt || funcsp->sb->attribs.inheritable.linkage == lk_fault)
    {
        Optimizer::gen_icode(Optimizer::i_pushcontext, 0, 0, 0);
        /*		if (funcsp->sb->loadds) */
        /*	        Optimizer::gen_icode(Optimizer::i_loadcontext, 0,0,0); */
    }
    Optimizer::gen_icode(Optimizer::i_prologue, 0, 0, 0);
    if (Optimizer::cparams.prm_debug)
    {
        if (basetype(funcsp->tp)->syms->size() && ((SYMBOL*)basetype(funcsp->tp)->syms->front())->sb->thisPtr)
        {
            EXPRESSION* exp = varNode(en_auto, ((SYMBOL*)basetype(funcsp->tp)->syms->front()));
            exp->v.sp->tp->used = true;
            gen_varstart(exp);
        }
    }
    else
    {
        if (basetype(funcsp->tp)->syms->size() && ((SYMBOL*)basetype(funcsp->tp)->syms->front())->sb->thisPtr)
        {
            baseThisPtr = Optimizer::SymbolManager::Get((SYMBOL*)basetype(funcsp->tp)->syms->front());
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
    AllocateLocalContext(emptyBlockdata, funcsp, Optimizer::nextLabel++);
    if (funcsp->sb->allocaUsed && (Optimizer::architecture != ARCHITECTURE_MSIL))
    {
        EXPRESSION* allocaExp = anonymousVar(sc_auto, &stdpointer);
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
    FreeLocalContext(emptyBlockdata, funcsp, Optimizer::nextLabel++);

    /* Code gen from icode */
    Optimizer::AddFunction();

    if (!currentFunction->isinternal && (funcsp->sb->attribs.inheritable.linkage4 == lk_virtual || tmpl))
        Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(funcsp));

    AllocateLocalContext(emptyBlockdata, funcsp, Optimizer::nextLabel);
    XTDumpTab(funcsp);
    FreeLocalContext(emptyBlockdata, funcsp, Optimizer::nextLabel);

    Optimizer::intermed_head = nullptr;
    theCurrentFunc = oldCurrentFunc;
    currentFunction = oldCurrentFunction;
    returnImode = oldReturnImode;
    if (Optimizer::blockCount > maxBlocks)
        maxBlocks = Optimizer::blockCount;
    if (Optimizer::tempCount > maxTemps)
        maxTemps = Optimizer::tempCount;

    // this is explicitly to clean up the this pointer
    for (auto sym : *basetype(funcsp->tp)->syms)
    {
        if (sym->sb->storage_class == sc_parameter)
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
void genASM(std::list<STATEMENT*>* st)
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