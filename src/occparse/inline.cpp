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
#include <unordered_set>
#include "config.h"
#include "initbackend.h"
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
#include "ioptimizer.h"
#include "libcxx.h"
#include "symtab.h"
#include "ListFactory.h"
#include "rtti.h"
#include "iexpr.h"
#include "iblock.h"
#include "iinline.h"

namespace Parser
{
static std::unordered_set<SYMBOL*> enteredInlines;
static std::list<SYMBOL*> inlines, inlineVTabs, inlineData, inlineRttis;
static std::unordered_map<SYMBOL *, SYMBOL*> contextMap;
static SYMBOL* inlinesp_list[MAX_INLINE_NESTING];
static int PushInline(SYMBOL* sym, bool traceback);
static std::list<std::tuple<int, Optimizer::SimpleSymbol*, int, int>> inlineMemberPtrData;
static std::list<std::pair<SYMBOL*, EXPRESSION *>> inlineLocalUninitializers;

static int inlinesp_count;
static SymbolTable<SYMBOL>* vc1Thunks;
static std::unordered_set<std::string, StringHash> didInlines;

static FUNCTIONCALL* function_list[MAX_INLINE_NESTING];
static int function_listcount;
static int namenumber;

void inlineinit(void)
{
    namenumber = 0;
    inlinesp_count = 0;
    inlines.clear();
    inlineVTabs.clear();
    inlineData.clear();
    inlineRttis.clear();
    vc1Thunks = symbols.CreateSymbolTable();
    didInlines.clear();
    enteredInlines.clear();
    contextMap.clear();
    inlineMemberPtrData.clear();
}

static void GenInline(SYMBOL* sym);
static bool inSearch(SYMBOL* sp) { return didInlines.find(sp->sb->decoratedName) != didInlines.end(); }
static void inInsert(SYMBOL* sym) { didInlines.insert(sym->sb->decoratedName); }
static void UndoPreviousCodegen(SYMBOL* sym) {}
static void DumpInlineLocalUninitializer(std::pair<SYMBOL*, EXPRESSION *>& uninit);

void dumpInlines(void)
{
    if (IsCompiler())
    {
        if (!TotalErrors())
        {
            bool done;
            do
            {
                done = true;
                for (auto sym : inlines)
                {
                    if (!sym->sb->dontinstantiate &&
                        (Optimizer::SymbolManager::Test(sym) && !Optimizer::SymbolManager::Test(sym)->generated))
                    {
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
                                int n = PushTemplateNamespace(sym);
                                if (sym->sb->parentClass)
                                    SwapMainTemplateArgs(sym->sb->parentClass);
                                structSyms.clear();
//                                PushInline(sym, true);
                                if ((sym->sb->attribs.inheritable.isInline || sym->sb->attribs.inheritable.linkage4 == Linkage::virtual_ ||
                                     sym->sb->forcedefault) &&
                                    CompileInline(sym, true))
                                {
                                    inInsert(sym);
                                    UndoPreviousCodegen(sym);
                                    GenInline(sym);
                                    done = false;
                                    sym->sb->didinline = true;
                                }
                                instantiationList.clear();
                                structSyms.clear();
                                if (sym->sb->parentClass)
                                    SwapMainTemplateArgs(sym->sb->parentClass);
                                PopTemplateNamespace(n);
                            }
                        }
                    }
                }
                startlab = retlab = 0;
                for (auto sym : inlineVTabs)
                {
                    if (Optimizer::SymbolManager::Test(sym->sb->vtabsp) && hasVTab(sym) && !sym->sb->vtabsp->sb->didinline && !sym->sb->dontinstantiate)
                    {
                        if (inSearch(sym))
                        {
                            sym->sb->didinline = true;
                        }
                        else
                        {
                            inInsert(sym);
                            if (sym->sb->dontinstantiate || sym->sb->vtabsp->sb->dontinstantiate)
                            {
                                Optimizer::SymbolManager::Get(sym->sb->vtabsp)->dontinstantiate = true;
                                sym->sb->vtabsp->sb->storage_class = StorageClass::external_;
                                sym->sb->vtabsp->sb->attribs.inheritable.linkage = Linkage::c_;
                            }
                            else
                            {
                                theCurrentFunc = contextMap[sym];
                                sym->sb->vtabsp->sb->didinline = true;
                                Optimizer::SymbolManager::Get(sym->sb->vtabsp)->generated = true;
                                dumpVTab(sym);
                                done = false;
                                theCurrentFunc = nullptr;
                            }
                        }
                    }
                }
                for (auto sym : inlineRttis)
                {
                    if (Optimizer::SymbolManager::Test(sym) && !Optimizer::SymbolManager::Get(sym)->generated && !sym->sb->didinline)
                    {
                        if (inSearch(sym))
                        {
                            sym->sb->didinline = true;
                        }
                        else
                        {
                            inInsert(sym);
                            RTTIDumpType(sym->tp);
                            InsertGlobal(sym);
                        }
                    }
                }
                for (auto&& local : inlineLocalUninitializers)
                {
                     DumpInlineLocalUninitializer(local);
                }
                inlineLocalUninitializers.clear();
            } while (!done);
            for (auto sym : inlineData)
            {
                if (sym->sb->attribs.inheritable.linkage2 != Linkage::import_)
                {
                    if (sym->sb->parentClass && sym->sb->parentClass->sb->parentTemplate && (Optimizer::SymbolManager::Test(sym) && !Optimizer::SymbolManager::Test(sym)->generated))
                    {
                        SYMBOL* parentTemplate = sym->sb->parentClass->sb->parentTemplate;
                        SYMBOL* origsym;
                        std::string argumentName;
                        auto found2 = classTemplateMap2[sym->sb->parentClass->sb->decoratedName];
                        if (found2)
                        {
                            parentTemplate = found2;
                        }
                        origsym = search(parentTemplate->tp->syms, sym->name);

                        if (!origsym || origsym->sb->storage_class != StorageClass::global_)
                        {
                            parentTemplate = sym->sb->parentClass->sb->parentTemplate;
                            origsym = search(parentTemplate->tp->syms, sym->name);
                        }

                        if (sym->sb->parentClass && sym->sb->parentClass->sb->dontinstantiate)
                        {
                            sym->sb->dontinstantiate = true;
                        }
                        if (origsym && origsym->sb->storage_class == StorageClass::global_ && !sym->sb->didinline && !sym->sb->dontinstantiate)
                        {
                            Optimizer::SymbolManager::Get(sym)->generated = true;
                            sym->sb->didinline = true;
                            sym->sb->storage_class = StorageClass::global_;
                            sym->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                            if (origsym->sb->deferredCompile)
                            {
                                STRUCTSYM s1, s;
                                LEXLIST* lex;
                                SYMBOL* pc = sym;
                                while (pc->sb->parentClass)
                                    pc = pc->sb->parentClass;
                                s1.str = sym->sb->parentClass;
                                addStructureDeclaration(&s1);
                                s.tmpl = sym->templateParams;
                                addTemplateDeclaration(&s);
                                int n = PushTemplateNamespace(pc);
                                lex = SetAlternateLex(origsym->sb->deferredCompile);
                                sym->sb->init = nullptr;
                                lex = initialize(lex, nullptr, sym, StorageClass::global_, true, false, _F_NOCONSTGEN);
                                SetAlternateLex(nullptr);
                                PopTemplateNamespace(n);
                                dropStructureDeclaration();
                                dropStructureDeclaration();
                            }
                            Optimizer::SymbolManager::Get(sym)->generated = true;
                            Optimizer::gen_virtual(Optimizer::SymbolManager::Get(sym), true);
                            if (sym->sb->init)
                            {
                                if (isstructured(sym->tp) || isarray(sym->tp))
                                {
                                    dumpInitGroup(sym, sym->tp);
                                }
                                else
                                {
                                    int s = dumpInit(sym, sym->sb->init->front());
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
                                        int s = dumpInit(sym, sym->sb->init->front());
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
            }
        }
    }
}
void dumpImportThunks(void)
{
    if (IsCompiler())
    {
        for (auto sym : importThunks)
        {
            Optimizer::gen_virtual(Optimizer::SymbolManager::Get(sym), false);
            Optimizer::gen_importThunk(Optimizer::SymbolManager::Get((sym)->sb->mainsym));
            Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(sym));
        }
    }
}
void dumpvc1Thunks(void)
{
    if (IsCompiler())
    {
        Optimizer::cseg();
        for (auto sym : *vc1Thunks)
        {
            Optimizer::gen_virtual(Optimizer::SymbolManager::Get(sym), false);
            Optimizer::gen_vc1(Optimizer::SymbolManager::Get(sym));
            Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(sym));
        }
    }
}
void dumpMemberPointer(std::tuple<int, Optimizer::SimpleSymbol*, int, int> &data)
{
    int lbl = std::get<0>(data);
    Optimizer::SimpleSymbol* sym = std::get<1>(data);
    int offset1 = std::get<2>(data);
    int offset2 = std::get<3>(data);
    if (lbl != -1)
    {
        Optimizer::put_label(lbl);
    }
    if (sym)
    {
        if (sym == (Optimizer::SimpleSymbol*)-1)
            Optimizer::genaddress(0);
        else
            Optimizer::genref(sym, 0);
    }
    Optimizer::genint(offset1);
    Optimizer::genint(offset2);
    if (!sym)
        Optimizer::genint(0); // padding so non-function structures will be the same size as function structures
}
void dumpMemberPointers() 
{
    Optimizer::dseg();
    for (auto&& p : inlineMemberPtrData)
    {
         dumpMemberPointer(p);
    }
}
static void DumpInlineLocalUninitializer(std::pair<SYMBOL*, EXPRESSION *>& uninit)
{
    auto newFunc = uninit.first;
    auto body = uninit.second;
    Optimizer::temporarySymbols.clear();
    Optimizer::functionVariables.clear();
    structret_imode = 0;
    Optimizer::tempCount = 0;
    Optimizer::blockCount = 0;
    Optimizer::exitBlock = 0;
    consIndex = 0;
    retcount = 0;
    currentFunction = Optimizer::SymbolManager::Get(newFunc);
    currentFunction->initialized = true;
    newFunc->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
    Optimizer::cseg();

    iexpr_func_init();
    gen_func(varNode(ExpressionNode::global_, newFunc), 1);
    Optimizer::gen_virtual(currentFunction, false);
    Optimizer::addblock(-1);
    Optimizer::gen_icode(Optimizer::i_prologue, 0, 0, 0);
    codeLabelOffset = Optimizer::nextLabel - INT_MIN;
    Optimizer::nextLabel += 2;

    Optimizer::gen_label(Optimizer::nextLabel - 2);
    noinline++;
    gen_expr(newFunc, body, F_NOVALUE, ISZ_UINT);
    noinline--;
    Optimizer::gen_label(Optimizer::nextLabel - 1);

    Optimizer::gen_icode(Optimizer::i_epilogue, 0, 0, 0);

    gen_icode(Optimizer::i_ret, nullptr, Optimizer::make_immed(ISZ_UINT, 0), nullptr);

    Optimizer::AddFunction();
    Optimizer::gen_endvirtual(currentFunction);
    Optimizer::intermed_head = nullptr;
    currentFunction = nullptr;
}
SYMBOL* getvc1Thunk(int offset)
{
    char name[256];
    SYMBOL* rv;
    Optimizer::my_sprintf(name, "@.vc1$B0$%d$0", offset + 1);
    rv = search(vc1Thunks, name);
    if (!rv)
    {
        rv = SymAlloc();
        rv->name = rv->sb->decoratedName = litlate(name);
        rv->sb->storage_class = StorageClass::static_;
        rv->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
        rv->sb->offset = offset;
        rv->tp = &stdvoid;
        vc1Thunks->Add(rv);
    }
    return rv;
}
static int PushInline(SYMBOL* sym, bool traceback)
{
    std::stack<SYMBOL*> reverseOrder;
    reverseOrder.push(sym);
    if (traceback)
    {
        std::unordered_set<SYMBOL*> visited;
        visited.insert(sym);
        while ((sym = contextMap[sym]))
        {
            if (visited.find(sym) == visited.end())
            {
                visited.insert(sym);
                reverseOrder.push(sym);
            }
            else
            {
                break;
            }
        }
    }
    int n = 0;
    while (!reverseOrder.empty())
    {
        sym = reverseOrder.top();
        reverseOrder.pop();
        EnterInstantiation(nullptr, sym);
        STRUCTSYM t, s, r;
        t.tmpl = nullptr;
        r.tmpl = nullptr;
        if (sym->templateParams)
        {
            r.tmpl = sym->templateParams;
            addTemplateDeclaration(&r);
            n++;
        }
        if (!sym->sb->parentClass && sym->sb->friendContext)
        {
            s.str = sym->sb->friendContext;
            addStructureDeclaration(&s);
            n++;
            SYMBOL* spt = basetype(s.str->tp)->sp;
            t.tmpl = spt->templateParams;
            if (t.tmpl)
            {
                addTemplateDeclaration(&t);
                n++;
            }
        }
    }
    return n;
}
bool CompileInline(SYMBOL* sym, bool toplevel)
{
    if (!toplevel)
    {
        contextMap[sym] = theCurrentFunc;
    }
    if (sym->sb->deferredCompile && !sym->sb->inlineFunc.stmt)
    {
        int oldPackIndex = packIndex;
        int oldArgumentNesting = argumentNesting;
        int oldExpandingParams = expandingParams;
        packIndex = -1;
        argumentNesting = 0;
        expandingParams = 0;
        if (sym->sb->specialized && sym->templateParams->size() == 1)
            sym->sb->instantiated = true;
        int n1 = 0;
        auto hold = std::move(structSyms);
        auto hold2 = std::move(instantiationList);
        n1 = PushInline(sym, true);
        ++instantiatingTemplate;
        deferredCompileOne(sym);
        --instantiatingTemplate;
        instantiationList.clear();
        instantiationList = std::move(hold2);
        structSyms.clear();
        structSyms = std::move(hold);
        expandingParams = oldExpandingParams;
        argumentNesting = oldArgumentNesting;
        packIndex = oldPackIndex;
    }
    return sym->sb->inlineFunc.stmt;
}
static void GenInline(SYMBOL* sym) 
{
    startlab = Optimizer::nextLabel++;
    retlab = Optimizer::nextLabel++;
    int n = PushTemplateNamespace(sym);
    int n1 = PushInline(sym, false);
    genfunc(sym, true);
    for (int i = 0; i < n1; i++)
        dropStructureDeclaration();
    PopTemplateNamespace(n);
}
void InsertInline(SYMBOL* sym)
{
    if (enteredInlines.find(sym) == enteredInlines.end())
    {
        enteredInlines.insert(sym);
        contextMap[sym] = theCurrentFunc;
        if (isfunction(sym->tp))
        {
            inlines.push_back(sym);
        }
        else
        {
            inlineVTabs.push_back(sym);
        }
    }
}
void InsertInlineData(SYMBOL* sym)
{
    if (enteredInlines.find(sym) == enteredInlines.end())
    {
        enteredInlines.insert(sym);
        inlineData.push_back(sym);
    }
}
void InsertRttiType(SYMBOL* xtsym)
{
    if (enteredInlines.find(xtsym) == enteredInlines.end())
    {
        enteredInlines.insert(xtsym);
        inlineRttis.push_back(xtsym);
    }
}
void InsertMemberPointer(int label, Optimizer::SimpleSymbol* sym, int offset1, int offset2)
{
    std::tuple<int, Optimizer::SimpleSymbol*, int, int> value(label, sym, offset1, offset2);
    if (label == -1)
        dumpMemberPointer(value);
    else
        inlineMemberPtrData.push_back(value);
}
void InsertLocalStaticUnInitializer(SYMBOL* func, EXPRESSION* body)
{
	inlineLocalUninitializers.push_back(std::pair<SYMBOL*, EXPRESSION*>(func, body));
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
    temp = Allocate<EXPRESSION>();
    memcpy(temp, node, sizeof(EXPRESSION));
    switch (temp->type)
    {
        case ExpressionNode::thisshim_:
            break;
        case ExpressionNode::c_ll_:
        case ExpressionNode::c_ull_:
        case ExpressionNode::c_d_:
        case ExpressionNode::c_ld_:
        case ExpressionNode::c_f_:
        case ExpressionNode::c_dc_:
        case ExpressionNode::c_ldc_:
        case ExpressionNode::c_fc_:
        case ExpressionNode::c_di_:
        case ExpressionNode::c_ldi_:
        case ExpressionNode::c_fi_:
        case ExpressionNode::c_i_:
        case ExpressionNode::c_l_:
        case ExpressionNode::c_ui_:
        case ExpressionNode::c_ul_:
        case ExpressionNode::c_c_:
        case ExpressionNode::c_bool_:
        case ExpressionNode::c_uc_:
        case ExpressionNode::c_wc_:
        case ExpressionNode::c_u16_:
        case ExpressionNode::c_u32_:
        case ExpressionNode::c_string_:
        case ExpressionNode::nullptr_:
        case ExpressionNode::structelem_:
            break;
        case ExpressionNode::global_:
        case ExpressionNode::pc_:
        case ExpressionNode::labcon_:
        case ExpressionNode::const_:
        case ExpressionNode::threadlocal_:
            break;
        case ExpressionNode::auto_:
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
                int n = function_listcount;
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
        case ExpressionNode::l_sp_:
        case ExpressionNode::l_fp_:
        case ExpressionNode::bits_:
        case ExpressionNode::l_f_:
        case ExpressionNode::l_d_:
        case ExpressionNode::l_ld_:
        case ExpressionNode::l_fi_:
        case ExpressionNode::l_di_:
        case ExpressionNode::l_ldi_:
        case ExpressionNode::l_fc_:
        case ExpressionNode::l_dc_:
        case ExpressionNode::l_ldc_:
        case ExpressionNode::l_wc_:
        case ExpressionNode::l_c_:
        case ExpressionNode::l_s_:
        case ExpressionNode::l_u16_:
        case ExpressionNode::l_u32_:
        case ExpressionNode::l_ul_:
        case ExpressionNode::l_l_:
        case ExpressionNode::l_p_:
        case ExpressionNode::l_ref_:
        case ExpressionNode::l_i_:
        case ExpressionNode::l_ui_:
        case ExpressionNode::l_inative_:
        case ExpressionNode::l_unative_:
        case ExpressionNode::l_uc_:
        case ExpressionNode::l_us_:
        case ExpressionNode::l_bool_:
        case ExpressionNode::l_bit_:
        case ExpressionNode::l_ll_:
        case ExpressionNode::l_ull_:
        case ExpressionNode::l_string_:
        case ExpressionNode::l_object_:
            /*
            if (node->left->type == ExpressionNode::auto_)
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
        case ExpressionNode::uminus_:
        case ExpressionNode::compl_:
        case ExpressionNode::not_:
        case ExpressionNode::x_f_:
        case ExpressionNode::x_d_:
        case ExpressionNode::x_ld_:
        case ExpressionNode::x_fi_:
        case ExpressionNode::x_di_:
        case ExpressionNode::x_ldi_:
        case ExpressionNode::x_fc_:
        case ExpressionNode::x_dc_:
        case ExpressionNode::x_ldc_:
        case ExpressionNode::x_ll_:
        case ExpressionNode::x_ull_:
        case ExpressionNode::x_i_:
        case ExpressionNode::x_ui_:
        case ExpressionNode::x_inative_:
        case ExpressionNode::x_unative_:
        case ExpressionNode::x_c_:
        case ExpressionNode::x_uc_:
        case ExpressionNode::x_u16_:
        case ExpressionNode::x_u32_:
        case ExpressionNode::x_wc_:
        case ExpressionNode::x_bool_:
        case ExpressionNode::x_bit_:
        case ExpressionNode::x_s_:
        case ExpressionNode::x_us_:
        case ExpressionNode::x_l_:
        case ExpressionNode::x_ul_:
        case ExpressionNode::x_p_:
        case ExpressionNode::x_fp_:
        case ExpressionNode::x_sp_:
        case ExpressionNode::trapcall_:
        case ExpressionNode::shiftby_:
            /*        case ExpressionNode::movebyref_: */
        case ExpressionNode::substack_:
        case ExpressionNode::alloca_:
        case ExpressionNode::loadstack_:
        case ExpressionNode::savestack_:
        case ExpressionNode::not__lvalue_:
        case ExpressionNode::lvalue_:
        case ExpressionNode::literalclass_:
        case ExpressionNode::x_string_:
        case ExpressionNode::x_object_:
            temp->left = inlineexpr(node->left, nullptr);
            break;
        case ExpressionNode::auto_inc_:
        case ExpressionNode::auto_dec_:
        case ExpressionNode::add_:
        case ExpressionNode::structadd_:
        case ExpressionNode::sub_:
            /*        case ExpressionNode::addcast_: */
        case ExpressionNode::lsh_:
        case ExpressionNode::arraylsh_:
        case ExpressionNode::rsh_:
        case ExpressionNode::assign_:
        case ExpressionNode::void_:
        case ExpressionNode::void_nz_:
            /*        case ExpressionNode::dvoid_: */
        case ExpressionNode::arraymul_:
        case ExpressionNode::arrayadd_:
        case ExpressionNode::arraydiv_:
        case ExpressionNode::mul_:
        case ExpressionNode::div_:
        case ExpressionNode::umul_:
        case ExpressionNode::udiv_:
        case ExpressionNode::umod_:
        case ExpressionNode::ursh_:
        case ExpressionNode::mod_:
        case ExpressionNode::and_:
        case ExpressionNode::or_:
        case ExpressionNode::xor_:
        case ExpressionNode::lor_:
        case ExpressionNode::land_:
        case ExpressionNode::eq_:
        case ExpressionNode::ne_:
        case ExpressionNode::gt_:
        case ExpressionNode::ge_:
        case ExpressionNode::lt_:
        case ExpressionNode::le_:
        case ExpressionNode::ugt_:
        case ExpressionNode::uge_:
        case ExpressionNode::ult_:
        case ExpressionNode::ule_:
        case ExpressionNode::cond_:
        case ExpressionNode::intcall_:
        case ExpressionNode::stackblock_:
        case ExpressionNode::blockassign_:
        case ExpressionNode::mp_compare_:
        case ExpressionNode::initblk_:
        case ExpressionNode::cpblk_:
        case ExpressionNode::dot_:
        case ExpressionNode::pointsto_:
        case ExpressionNode::construct_:
            break;
            /*		case ExpressionNode::array_: */
            temp->right = inlineexpr(node->right, nullptr);
        case ExpressionNode::mp_as_bool_:
        case ExpressionNode::blockclear_:
        case ExpressionNode::argnopush_:
        case ExpressionNode::thisref_:
        case ExpressionNode::funcret_:
        case ExpressionNode::initobj_:
        case ExpressionNode::sizeof_:
        case ExpressionNode::select_:
            temp->left = inlineexpr(node->left, nullptr);
            break;
        case ExpressionNode::atomic_:
            temp->v.ad->flg = inlineexpr(node->v.ad->flg, nullptr);
            temp->v.ad->memoryOrder1 = inlineexpr(node->v.ad->memoryOrder1, nullptr);
            temp->v.ad->memoryOrder2 = inlineexpr(node->v.ad->memoryOrder2, nullptr);
            temp->v.ad->address = inlineexpr(node->v.ad->address, nullptr);
            temp->v.ad->value = inlineexpr(node->v.ad->value, nullptr);
            temp->v.ad->third = inlineexpr(node->v.ad->third, nullptr);
            break;
        case ExpressionNode::func_:
            temp->v.func = nullptr;
            fp = node->v.func;
            if (fp->sp->sb->attribs.inheritable.linkage4 == Linkage::virtual_)
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
                temp->v.func = Allocate<FUNCTIONCALL>();
                *temp->v.func = *fp;
                if (temp->v.func->arguments)
                {
                    auto args = *temp->v.func->arguments;
                    temp->v.func->arguments->clear();
                    for (auto a : args)
                    {
                        auto il = Allocate<INITLIST>();
                        *il = *a;
                        il->exp = inlineexpr(a->exp, nullptr);
                        temp->v.func->arguments->push_back(il);
                    }
                }
                if (temp->v.func->thisptr)
                    temp->v.func->thisptr = inlineexpr(temp->v.func->thisptr, nullptr);
            }
            break;
        case ExpressionNode::stmt_:
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

std::list<STATEMENT*>* inlinestmt(std::list<STATEMENT*>* blocks)
{
    if (blocks)
    {
        std::list<STATEMENT*>* out = stmtListFactory.CreateList();
        for (auto block : *blocks)
        {
            auto stmt = Allocate<STATEMENT>();
            out->push_back(stmt);
            *stmt = *block;
            block = stmt;
            switch (block->type)
            {
            case StatementNode::genword_:
                break;
            case StatementNode::try_:
            case StatementNode::catch_:
            case StatementNode::seh_try_:
            case StatementNode::seh_catch_:
            case StatementNode::seh_finally_:
            case StatementNode::seh_fault_:
                block->lower = inlinestmt(block->lower);
                block->blockTail = inlinestmt(block->blockTail);
                break;
            case StatementNode::return_:
            case StatementNode::expr_:
            case StatementNode::declare_:
                block->select = inlineexpr(block->select, nullptr);
                break;
            case StatementNode::goto_:
            case StatementNode::label_:
                break;
            case StatementNode::select_:
            case StatementNode::notselect_:
                block->select = inlineexpr(block->select, nullptr);
                break;
            case StatementNode::switch_:
                block->select = inlineexpr(block->select, nullptr);
                block->lower = inlinestmt(block->lower);
                break;
            case StatementNode::block_:
                block->lower = inlinestmt(block->lower);
                block->blockTail = inlinestmt(block->blockTail);
                break;
            case StatementNode::passthrough_:
                if (block->lower)
                    inlineAsmStmt(block->lower);
                break;
            case StatementNode::nop_:
                break;
            case StatementNode::datapassthrough_:
                break;
            case StatementNode::line_:
            case StatementNode::varstart_:
            case StatementNode::dbgblock_:
                break;
            default:
                diag("Invalid block type in inlinestmt");
                break;
            }
        }
        return out;
    }
    return nullptr;
}
static void inlineResetReturn(STATEMENT* block, TYPE* rettp, EXPRESSION* retnode)
{
    EXPRESSION* exp;
    if (isstructured(rettp))
    {
        diag("structure in inlineResetReturn");
        exp = intNode(ExpressionNode::c_i_, 0);
    }
    else
    {
        exp = block->select;
        cast(rettp, &exp);
        exp = exprNode(ExpressionNode::assign_, retnode, exp);
    }
    block->type = StatementNode::expr_;
    block->select = exp;
}
static EXPRESSION* newReturn(TYPE* tp)
{
    EXPRESSION* exp;
    if (!isstructured(tp) && !isvoid(tp))
    {
        exp = anonymousVar(StorageClass::auto_, tp);
        deref(tp, &exp);
    }
    else
        exp = intNode(ExpressionNode::c_i_, 0);
    return exp;
}
static void reduceReturns(std::list<STATEMENT*>* blocks, TYPE* rettp, EXPRESSION* retnode)
{
    if (blocks)
    {
        for (auto block : *blocks)
        {
            switch (block->type)
            {
            case StatementNode::genword_:
                break;
            case StatementNode::try_:
            case StatementNode::catch_:
            case StatementNode::seh_try_:
            case StatementNode::seh_catch_:
            case StatementNode::seh_finally_:
            case StatementNode::seh_fault_:
                reduceReturns(block->lower, rettp, retnode);
                break;
            case StatementNode::return_:
                inlineResetReturn(block, rettp, retnode);
                break;
            case StatementNode::goto_:
            case StatementNode::label_:
                break;
            case StatementNode::expr_:
                /*			case StatementNode::functailexpr_: */
            case StatementNode::declare_:
            case StatementNode::select_:
            case StatementNode::notselect_:
                break;
            case StatementNode::switch_:
                reduceReturns(block->lower, rettp, retnode);
                break;
            case StatementNode::block_:
                reduceReturns(block->lower, rettp, retnode);
                /* skipping block tail as it will have no returns  */
                break;
            case StatementNode::passthrough_:
            case StatementNode::datapassthrough_:
                break;
            case StatementNode::nop_:
                break;
            case StatementNode::line_:
            case StatementNode::varstart_:
            case StatementNode::dbgblock_:
                break;
            default:
                diag("Invalid block type in reduceReturns");
                break;
            }
        }
    }
}
static EXPRESSION* scanReturn(std::list<STATEMENT*>* blocks, TYPE* rettp)
{
    EXPRESSION* rv = nullptr;
    if (blocks)
        for (auto block : *blocks)
        {
            switch (block->type)
            {
            case StatementNode::genword_:
                break;
            case StatementNode::try_:
            case StatementNode::catch_:
            case StatementNode::seh_try_:
            case StatementNode::seh_catch_:
            case StatementNode::seh_finally_:
            case StatementNode::seh_fault_:
                rv = scanReturn(block->lower, rettp);
                break;
            case StatementNode::return_:
                rv = block->select;
                if (!isstructured(rettp))
                {
                    if (isref(rettp))
                        deref(&stdpointer, &rv);
                    else
                        cast(rettp, &rv);
                }
                block->type = StatementNode::expr_;
                block->select = rv;
                return rv;
            case StatementNode::goto_:
            case StatementNode::label_:
                break;
            case StatementNode::expr_:
                /*			case StatementNode::functailexpr_: */
            case StatementNode::declare_:
            case StatementNode::select_:
            case StatementNode::notselect_:
                break;
            case StatementNode::switch_:
                rv = scanReturn(block->lower, rettp);
                break;
            case StatementNode::block_:
                rv = scanReturn(block->lower, rettp);
                /* skipping block tail as it will have no returns  */
                break;
            case StatementNode::passthrough_:
            case StatementNode::datapassthrough_:
                break;
            case StatementNode::nop_:
                break;
            case StatementNode::line_:
            case StatementNode::varstart_:
            case StatementNode::dbgblock_:
                break;
            default:
                diag("Invalid block type in scanReturn");
                break;
            }
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
        case ExpressionNode::c_ll_:
        case ExpressionNode::c_ull_:
        case ExpressionNode::c_d_:
        case ExpressionNode::c_ld_:
        case ExpressionNode::c_f_:
        case ExpressionNode::c_dc_:
        case ExpressionNode::c_ldc_:
        case ExpressionNode::c_fc_:
        case ExpressionNode::c_di_:
        case ExpressionNode::c_ldi_:
        case ExpressionNode::c_fi_:
        case ExpressionNode::c_i_:
        case ExpressionNode::c_l_:
        case ExpressionNode::c_ui_:
        case ExpressionNode::c_ul_:
        case ExpressionNode::c_c_:
        case ExpressionNode::c_bool_:
        case ExpressionNode::c_uc_:
        case ExpressionNode::c_wc_:
        case ExpressionNode::c_u16_:
        case ExpressionNode::c_u32_:
        case ExpressionNode::c_string_:
        case ExpressionNode::nullptr_:
        case ExpressionNode::structelem_:
            rv = false;
            break;
        case ExpressionNode::global_:
        case ExpressionNode::pc_:
        case ExpressionNode::threadlocal_:
        case ExpressionNode::labcon_:
        case ExpressionNode::const_:
        case ExpressionNode::auto_:
        case ExpressionNode::sizeof_:
        case ExpressionNode::construct_:
            rv = false;
            break;
        case ExpressionNode::l_sp_:
        case ExpressionNode::l_fp_:
        case ExpressionNode::bits_:
        case ExpressionNode::l_f_:
        case ExpressionNode::l_d_:
        case ExpressionNode::l_ld_:
        case ExpressionNode::l_fi_:
        case ExpressionNode::l_di_:
        case ExpressionNode::l_ldi_:
        case ExpressionNode::l_fc_:
        case ExpressionNode::l_dc_:
        case ExpressionNode::l_ldc_:
        case ExpressionNode::l_wc_:
        case ExpressionNode::l_c_:
        case ExpressionNode::l_s_:
        case ExpressionNode::l_u16_:
        case ExpressionNode::l_u32_:
        case ExpressionNode::l_ul_:
        case ExpressionNode::l_l_:
        case ExpressionNode::l_p_:
        case ExpressionNode::l_ref_:
        case ExpressionNode::l_i_:
        case ExpressionNode::l_ui_:
        case ExpressionNode::l_inative_:
        case ExpressionNode::l_unative_:
        case ExpressionNode::l_uc_:
        case ExpressionNode::l_us_:
        case ExpressionNode::l_bool_:
        case ExpressionNode::l_bit_:
        case ExpressionNode::l_ll_:
        case ExpressionNode::l_ull_:
        case ExpressionNode::l_string_:
        case ExpressionNode::l_object_:
        case ExpressionNode::literalclass_:
            rv = sideEffects(node->left);
            break;
        case ExpressionNode::uminus_:
        case ExpressionNode::compl_:
        case ExpressionNode::not_:
        case ExpressionNode::x_f_:
        case ExpressionNode::x_d_:
        case ExpressionNode::x_ld_:
        case ExpressionNode::x_fi_:
        case ExpressionNode::x_di_:
        case ExpressionNode::x_ldi_:
        case ExpressionNode::x_fc_:
        case ExpressionNode::x_dc_:
        case ExpressionNode::x_ldc_:
        case ExpressionNode::x_ll_:
        case ExpressionNode::x_ull_:
        case ExpressionNode::x_i_:
        case ExpressionNode::x_ui_:
        case ExpressionNode::x_inative_:
        case ExpressionNode::x_unative_:
        case ExpressionNode::x_c_:
        case ExpressionNode::x_uc_:
        case ExpressionNode::x_u16_:
        case ExpressionNode::x_u32_:
        case ExpressionNode::x_wc_:
        case ExpressionNode::x_bool_:
        case ExpressionNode::x_bit_:
        case ExpressionNode::x_s_:
        case ExpressionNode::x_us_:
        case ExpressionNode::x_l_:
        case ExpressionNode::x_ul_:
        case ExpressionNode::x_p_:
        case ExpressionNode::x_fp_:
        case ExpressionNode::x_sp_:
        case ExpressionNode::shiftby_:
        case ExpressionNode::x_string_:
        case ExpressionNode::x_object_:
            /*        case ExpressionNode::movebyref_: */
        case ExpressionNode::not__lvalue_:
        case ExpressionNode::lvalue_:
            rv = sideEffects(node->left);
            break;
        case ExpressionNode::substack_:
        case ExpressionNode::alloca_:
        case ExpressionNode::loadstack_:
        case ExpressionNode::savestack_:
        case ExpressionNode::assign_:
        case ExpressionNode::auto_inc_:
        case ExpressionNode::auto_dec_:
        case ExpressionNode::trapcall_:
        case ExpressionNode::initblk_:
        case ExpressionNode::cpblk_:
        case ExpressionNode::initobj_:
            rv = true;
            break;
        case ExpressionNode::add_:
        case ExpressionNode::sub_:
            /*        case ExpressionNode::addcast_: */
        case ExpressionNode::lsh_:
        case ExpressionNode::arraylsh_:
        case ExpressionNode::rsh_:
        case ExpressionNode::void_:
        case ExpressionNode::void_nz_:
            /*        case ExpressionNode::dvoid_: */
        case ExpressionNode::arraymul_:
        case ExpressionNode::arrayadd_:
        case ExpressionNode::arraydiv_:
        case ExpressionNode::structadd_:
        case ExpressionNode::mul_:
        case ExpressionNode::div_:
        case ExpressionNode::umul_:
        case ExpressionNode::udiv_:
        case ExpressionNode::umod_:
        case ExpressionNode::ursh_:
        case ExpressionNode::mod_:
        case ExpressionNode::and_:
        case ExpressionNode::or_:
        case ExpressionNode::xor_:
        case ExpressionNode::lor_:
        case ExpressionNode::land_:
        case ExpressionNode::eq_:
        case ExpressionNode::ne_:
        case ExpressionNode::gt_:
        case ExpressionNode::ge_:
        case ExpressionNode::lt_:
        case ExpressionNode::le_:
        case ExpressionNode::ugt_:
        case ExpressionNode::uge_:
        case ExpressionNode::ult_:
        case ExpressionNode::ule_:
        case ExpressionNode::cond_:
        case ExpressionNode::intcall_:
        case ExpressionNode::stackblock_:
        case ExpressionNode::blockassign_:
        case ExpressionNode::mp_compare_:
        case ExpressionNode::dot_:
        case ExpressionNode::pointsto_:
            break;
            /*		case ExpressionNode::array_: */
            rv = sideEffects(node->right);
        case ExpressionNode::mp_as_bool_:
        case ExpressionNode::blockclear_:
        case ExpressionNode::argnopush_:
        case ExpressionNode::thisref_:
        case ExpressionNode::funcret_:
        case ExpressionNode::select_:
            rv |= sideEffects(node->left);
            break;
        case ExpressionNode::atomic_:
            rv = sideEffects(node->v.ad->flg);
            rv |= sideEffects(node->v.ad->memoryOrder1);
            rv |= sideEffects(node->v.ad->memoryOrder2);
            rv |= sideEffects(node->v.ad->address);
            rv |= sideEffects(node->v.ad->value);
            rv |= sideEffects(node->v.ad->third);
            break;
        case ExpressionNode::func_:
            rv = true;
            break;
        case ExpressionNode::stmt_:
            rv = true;
            break;
        case ExpressionNode::thisshim_:
            break;
        default:
            diag("sideEffects");
            break;
    }
    return rv;
}
static void setExp(SYMBOL* sx, EXPRESSION* exp, std::list<STATEMENT*> **stp)
{
    if (!sx->sb->altered && !sx->sb->addressTaken && !sideEffects(exp))
    {
        // well if the expression is too complicated it gets evaluated over and over
        // but maybe the backend can clean it up again...
        sx->sb->inlineFunc.stmt = (std::list<STATEMENT*>*)exp;
    }
    else
    {
        EXPRESSION* tnode = anonymousVar(StorageClass::auto_, sx->tp);
        deref(sx->tp, &tnode);
        sx->sb->inlineFunc.stmt = (std::list<STATEMENT*>*)tnode;
        tnode = exprNode(ExpressionNode::assign_, tnode, exp);
        auto stmt = Allocate<STATEMENT>();
        stmt->type = StatementNode::expr_;
        stmt->select = tnode;
        if (!*stp)
            *stp = stmtListFactory.CreateList();
        (*stp)->push_back(stmt);
    }
}
static std::list<STATEMENT*>* SetupArguments1(FUNCTIONCALL* params)
{
    std::list<STATEMENT*>* st = nullptr;
    std::list<INITLIST*>::iterator al, ale = al;
    if (params->arguments)
    {
        al = params->arguments->begin();
        ale = params->arguments->end();
    }
    auto it = basetype(params->sp->tp)->syms->begin();
    auto ite = basetype(params->sp->tp)->syms->end();
    if (ismember(params->sp))
    {
        setExp(*it, params->thisptr, &st);
        ++it;
    }
    while (al != ale && it != ite)
    {
        setExp(*it, (*al)->exp, &st);
        ++al;
        ++it;
    }
    return st;
}
/*-------------------------------------------------------------------------*/

void SetupVariables(SYMBOL* sym)
/* Copy all the func args into the xsyms table.
 * This copies the function parameters twice...
 */
{
    SymbolTable<SYMBOL>* syms = sym->sb->inlineFunc.syms;
    while (syms)
    {
        for (auto sx : *syms)
        {
            if (sx->sb->storage_class == StorageClass::auto_)
            {
                EXPRESSION* ev = anonymousVar(StorageClass::auto_, sx->tp);
                deref(sx->tp, &ev);
                sx->sb->inlineFunc.stmt = (std::list<STATEMENT*>*)ev;
            }
        }
        syms = syms->Next();
    }
}
/*-------------------------------------------------------------------------*/

EXPRESSION* doinline(FUNCTIONCALL* params, SYMBOL* funcsp)
{
    bool found = false;
    EXPRESSION* newExpression;
    bool allocated = false;

    if (function_listcount >= MAX_INLINE_NESTING)
    {
        return nullptr;
    }
    if (!isfunction(params->functp))
    {
        return nullptr;
    }
    if (!params->sp->sb->attribs.inheritable.isInline)
    {
        return nullptr;
    }
    if (params->sp->templateParams)
    {
        return nullptr;
    }
    if (params->sp->sb->noinline)
    {
        return nullptr;
    }
    if (!params->sp->sb->inlineFunc.syms)
    {
        return nullptr;
    }
    if (!params->sp->sb->inlineFunc.stmt)
    {
        // recursive
        return nullptr;
    }
    if (!localNameSpace->front()->syms)
    {
        allocated = true;
        AllocateLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
    }
    std::list<STATEMENT*>* stmt = stmtListFactory.CreateList();
    auto stmt1 = SetupArguments1(params);
    SetupVariables(params->sp);
    function_list[function_listcount++] = params;
    stmt = inlinestmt(params->sp->sb->inlineFunc.stmt);
    if (stmt1)
    {
        // this will kill the ret val but we don't care since we've modified params

        auto stmt2 = Allocate<STATEMENT>();
        stmt2->type = StatementNode::block_;
        stmt2->lower = stmt1;
        stmt->push_front(stmt2);
    }

    newExpression = exprNode(ExpressionNode::stmt_, nullptr, nullptr);
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
        FreeLocalContext(emptyBlockdata, nullptr, Optimizer::nextLabel++);
    }
    function_listcount--;
    if (newExpression->type == ExpressionNode::stmt_)
        if (newExpression->v.stmt->front()->type == StatementNode::block_)
            if (!newExpression->v.stmt->front()->lower)
                newExpression = intNode(ExpressionNode::c_i_, 0);  // noop if there is no body
    if (newExpression->type == ExpressionNode::stmt_)
    {
        newExpression->left = intNode(ExpressionNode::c_i_, 0);
        if (isstructured(basetype(params->sp->tp)->btp))
            cast(&stdpointer, &newExpression->left);
        else
            cast(basetype(basetype(params->sp->tp)->btp), &newExpression->left);
    }
    return newExpression;
}
static bool IsEmptyBlocks(std::list<STATEMENT*>* blocks)
{
    bool rv = true;
    if (blocks)
        for (auto block : *blocks)
        {
            switch (block->type)
            {
                case StatementNode::line_:
                case StatementNode::varstart_:
                case StatementNode::dbgblock_:
                    break;
                case StatementNode::genword_:
                case StatementNode::try_:
                case StatementNode::catch_:
                case StatementNode::seh_try_:
                case StatementNode::seh_catch_:
                case StatementNode::seh_finally_:
                case StatementNode::seh_fault_:
                case StatementNode::return_:
                case StatementNode::goto_:
                case StatementNode::expr_:
                case StatementNode::declare_:
                case StatementNode::select_:
                case StatementNode::notselect_:
                case StatementNode::switch_:
                case StatementNode::passthrough_:
                case StatementNode::datapassthrough_:
                    rv = false;
                    break;
                case StatementNode::nop_:
                    break;
                case StatementNode::label_:
                    break;
                case StatementNode::block_:
                    rv = IsEmptyBlocks(block->lower) && block->blockTail == nullptr;
                    break;
                default:
                    diag("Invalid block type in IsEmptyBlocks");
                    break;
            }
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
    return true;
}
}  // namespace Parser