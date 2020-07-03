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
#include "assert.h"
#include <stack>
#include "PreProcessor.h"
#include <malloc.h>
#include "ccerr.h"
#include "symtab.h"
#include "initbackend.h"
#include "declare.h"
#include "declcpp.h"
#include "stmt.h"
#include "expr.h"
#include "symtab.h"
#include "lambda.h"
#include "occparse.h"
#include "help.h"
#include "cpplookup.h"
#include "mangle.h"
#include "lex.h"
#include "constopt.h"
#include "memory.h"
#include "init.h"
#include "rtti.h"
#include "declcons.h"
#include "exprcpp.h"
#include "inline.h"
#include "beinterf.h"
#include "types.h"
#include "template.h"

#ifdef HAVE_UNISTD_H
#    define _alloca alloca
#endif

namespace Parser
{

int dontRegisterTemplate;
int instantiatingTemplate;
int inTemplateBody;
int templateNestingCount = 0;
int templateHeaderCount;
int inTemplateSpecialization = 0;
int inDeduceArgs;
bool parsingSpecializationDeclaration;
bool inTemplateType;
int inTemplateHeader;
SYMBOL* instantiatingMemberFuncClass;
static int instantiatingFunction;
static int inTemplateArgs;

struct templateListData* currents;

static LEXEME* TemplateArg(LEXEME* lex, SYMBOL* funcsp, TEMPLATEPARAMLIST* arg, TEMPLATEPARAMLIST** lst);
static TEMPLATEPARAMLIST* copyParams(TEMPLATEPARAMLIST* t, bool alsoSpecializations);
static bool valFromDefault(TEMPLATEPARAMLIST* params, bool usesParams, INITLIST** args);
static int pushContext(SYMBOL* cls, bool all);
static TEMPLATEPARAMLIST* ResolveTemplateSelectors(SYMBOL* sp, TEMPLATEPARAMLIST* args);
static TEMPLATEPARAMLIST* GetUsingArgs(SYMBOL* sp, TEMPLATEPARAMLIST* find, TEMPLATEPARAMLIST* orig);

void templateInit(void)
{
    inTemplateBody = false;
    inTemplateHeader = false;
    templateNestingCount = 0;
    templateHeaderCount = 0;
    instantiatingTemplate = 0;
    currents = nullptr;
    inTemplateArgs = 0;
    inTemplateType = false;
    dontRegisterTemplate = 0;
    inTemplateSpecialization = 0;
    instantiatingMemberFuncClass = nullptr;
    parsingSpecializationDeclaration = false;
    instantiatingFunction = 0;
    inDeduceArgs = 0;
}
EXPRESSION* GetSymRef(EXPRESSION* n)
{
    EXPRESSION* rv = nullptr;
    std::stack<EXPRESSION*> st;
    st.push(n);
    while (!st.empty())
    {
        EXPRESSION* exp = st.top();
        st.pop();
        switch (exp->type)
        {
            case en_labcon:
            case en_global:
            case en_auto:
            case en_absolute:
            case en_pc:
            case en_threadlocal:
                return exp;
            case en_tempref:
                if (st.empty())
                    return nullptr;
                continue;
            default:
                if (!isintconst(exp) && !isfloatconst(exp))
                {
                    if (exp->right)
                    {
                        st.push(exp->right);
                    }
                    if (exp->left)
                    {
                        st.push(exp->left);
                    }
                }
                break;
        }
    }
    return rv;
}
bool equalTemplateIntNode(EXPRESSION* exp1, EXPRESSION* exp2)
{
    //#ifdef PARSER_ONLY
    //    return true;
    //#else
    if (equalnode(exp1, exp2))
        return true;
    if (isintconst(exp1) && isintconst(exp2) && exp1->v.i == exp2->v.i)
        return true;
    return false;
    //#endif
}
bool templatecompareexpressions(EXPRESSION* exp1, EXPRESSION* exp2)
{
    if (isintconst(exp1) && isintconst(exp2))
        return exp1->v.i == exp2->v.i;
    if (exp1->type != exp2->type)
        return false;
    switch (exp1->type)
    {
        case en_global:
        case en_auto:
        case en_labcon:
        case en_absolute:
        case en_pc:
        case en_const:
        case en_threadlocal:
            return exp1->v.sp == exp2->v.sp;
        case en_func:
            return exp1->v.func->sp == exp2->v.func->sp;
        case en_templateselector:
            return templateselectorcompare(exp1->v.templateSelector, exp2->v.templateSelector);
        default:
            break;
    }
    if (exp1->left && exp2->left)
        if (!templatecompareexpressions(exp1->left, exp2->left))
            return false;
    if (exp1->right && exp2->right)
        if (!templatecompareexpressions(exp1->right, exp2->right))
            return false;
    return true;
}
bool templateselectorcompare(TEMPLATESELECTOR* tsin1, TEMPLATESELECTOR* tsin2)
{
    TEMPLATESELECTOR *ts1 = tsin1->next, *tss1;
    TEMPLATESELECTOR *ts2 = tsin2->next, *tss2;
    if (ts1->isTemplate != ts2->isTemplate || ts1->sp != ts2->sp)
        return false;
    tss1 = ts1->next;
    tss2 = ts2->next;
    while (tss1 && tss2)
    {
        if (strcmp(tss1->name, tss2->name))
            return false;
        tss1 = tss1->next;
        tss2 = tss2->next;
    }
    if (tss1 || tss2)
        return false;
    if (ts1->isTemplate)
    {
        if (!exactMatchOnTemplateParams(ts1->templateParams, ts2->templateParams))
            return false;
    }
    return true;
}
bool templatecomparetypes(TYPE* tp1, TYPE* tp2, bool exact)
{
    if (!tp1 || !tp2)
        return false;
    if (basetype(tp1)->type == bt_templateselector && basetype(tp2)->type == bt_templateselector)
    {
        TEMPLATESELECTOR* left = basetype(tp1)->sp->sb->templateSelector;
        TEMPLATESELECTOR* right = basetype(tp2)->sp->sb->templateSelector;
        left = left->next->next;
        right = right->next->next;
        while (left && right)
        {
            if (strcmp(left->name, right->name))
                return false;
            left = left->next;
            right = right->next;
        }
        return !left && !right;
    }
    else
    {
        if (basetype(tp1)->type == bt_templateselector || basetype(tp2)->type == bt_templateselector)
            return true;
    }
    if (!comparetypes(tp1, tp2, exact))
        return false;
    if (isint(tp1) && basetype(tp1)->btp && basetype(tp1)->btp->type == bt_enum)
        tp1 = basetype(tp1)->btp;
    if (isint(tp2) && basetype(tp2)->btp && basetype(tp2)->btp->type == bt_enum)
        tp2 = basetype(tp2)->btp;
    if (basetype(tp1)->type != basetype(tp2)->type)
        return false;
    if (basetype(tp1)->type == bt_enum)
    {
        if (basetype(tp1)->sp != basetype(tp2)->sp)
            return false;
    }
    return true;
}
void TemplateGetDeferred(SYMBOL* sym)
{
    if (currents)
    {
        if (currents->bodyHead)
        {
            sym->sb->deferredCompile = currents->bodyHead;
        }
    }
}
TEMPLATEPARAMLIST* TemplateLookupSpecializationParam(const char* name)
{
    TEMPLATEPARAMLIST* rv = nullptr;
    if (parsingSpecializationDeclaration)
    {
        TEMPLATEPARAMLIST* search = currents->args->next;
        while (search)
        {
            if (!strcmp(search->argsym->name, name))
                break;
            search = search->next;
        }
        if (search)
            rv = search;
    }
    return rv;
}
TEMPLATEPARAMLIST* TemplateGetParams(SYMBOL* sym)
{
    TEMPLATEPARAMLIST* params = nullptr;
    if (currents)
    {
        int n = -1;
        params = (TEMPLATEPARAMLIST*)(*currents->plast);
        while (sym)
        {
            if (sym->sb->templateLevel && !sym->sb->instantiated)
                n++;
            sym = sym->sb->parentClass;
        }
        if (n > 0 && params)
            while (n-- && params->p->bySpecialization.next)
            {
                params = params->p->bySpecialization.next;
            }
    }
    if (!params)
    {
        params = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
        params->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
    }
    return params;
}
void TemplateRegisterDeferred(LEXEME* lex)
{
    if (lex && templateNestingCount && !dontRegisterTemplate)
    {
        if (!lex->registered)
        {
            LEXEME* cur = (LEXEME*)globalAlloc(sizeof(LEXEME));
            if (lex->type == l_id)
                lex->value.s.a = litlate(lex->value.s.a);
            *cur = *lex;
            cur->next = nullptr;
            if (inTemplateBody)
            {
                if (currents->bodyHead)
                {
                    cur->prev = currents->bodyTail;
                    currents->bodyTail = currents->bodyTail->next = cur;
                }
                else
                {
                    cur->prev = nullptr;
                    currents->bodyHead = currents->bodyTail = cur;
                }
            }
            else
            {
                if (currents->head)
                {
                    cur->prev = currents->tail;
                    currents->tail = currents->tail->next = cur;
                }
                else
                {
                    cur->prev = nullptr;
                    currents->head = currents->tail = cur;
                }
            }
            lex->registered = true;
        }
    }
}
bool exactMatchOnTemplateParams(TEMPLATEPARAMLIST* old, TEMPLATEPARAMLIST* sym)
{
    while (old && sym)
    {
        if (old->p->type != sym->p->type)
            break;
        if (sym->p->packed)
        {
            if (old->p->packed)
            {
                if (sym->p->byPack.pack)
                    if (!exactMatchOnTemplateParams(old->p->byPack.pack, sym->p->byPack.pack))
                        return false;
            }
            else if (sym->p->byPack.pack)
            {
                while (old)
                {
                    if (old->p->type != sym->p->type)
                        return false;
                    old = old->next;
                }
                return !sym->next;
            }
            else
            {
                return !sym->next;
            }
        }
        else if (old->p->type == kw_template)
        {
            if (!exactMatchOnTemplateParams(old->p->byTemplate.args, sym->p->byTemplate.args))
                break;
        }
        else if (old->p->type == kw_int)
        {
            if (!templatecomparetypes(old->p->byNonType.tp, sym->p->byNonType.tp, true))
                break;
            if (old->p->byNonType.dflt && sym->p->byNonType.dflt &&
                !templatecompareexpressions(old->p->byNonType.dflt, sym->p->byNonType.dflt))
                break;
        }
        old = old->next;
        sym = sym->next;
    }
    if (old && old->p->packed)
        old = nullptr;
    return !(old || sym);
}
bool exactMatchOnTemplateArgs(TEMPLATEPARAMLIST* old, TEMPLATEPARAMLIST* sym)
{
    while (old && sym)
    {
        if (old->p->type != sym->p->type)
            return false;
        if (old->p->packed)
        {
            return false;
        }
        switch (old->p->type)
        {
            case kw_typename:
                if (sameTemplate(old->p->byClass.dflt, sym->p->byClass.dflt))
                {
                    if (!exactMatchOnTemplateArgs(basetype(old->p->byClass.dflt)->sp->templateParams,
                                                  basetype(sym->p->byClass.dflt)->sp->templateParams))
                        return false;
                }
                else
                {
                    if (!templatecomparetypes(old->p->byClass.dflt, sym->p->byClass.dflt, true))
                        return false;
                    if (!templatecomparetypes(sym->p->byClass.dflt, old->p->byClass.dflt, true))
                        return false;
                    if (isarray(old->p->byClass.dflt) != isarray(sym->p->byClass.dflt))
                        return false;
                    if (isarray(old->p->byClass.dflt))
                        if (!!basetype(old->p->byClass.dflt)->esize != !!basetype(sym->p->byClass.dflt)->esize)
                            return false;
                }
                {
                    TYPE* ts = sym->p->byClass.dflt;
                    TYPE* to = old->p->byClass.dflt;
                    if (isref(ts))
                        ts = basetype(ts)->btp;
                    if (isref(to))
                        to = basetype(to)->btp;
                    if (isconst(ts) != isconst(to))
                        return false;
                    if (isvolatile(ts) != isvolatile(to))
                        return false;
                }
                break;
            case kw_template:
                if (old->p->byTemplate.dflt != sym->p->byTemplate.dflt)
                    return false;
                break;
            case kw_int:
                if (!templatecomparetypes(old->p->byNonType.tp, sym->p->byNonType.tp, true))
                    return false;
                if (!!old->p->byNonType.dflt != !!sym->p->byNonType.dflt)
                    return false;
                //#ifndef PARSER_ONLY
                if (old->p->byNonType.dflt && sym->p->byNonType.dflt &&
                    !templatecompareexpressions(old->p->byNonType.dflt, sym->p->byNonType.dflt))
                    return false;
                //#endif
                break;
            default:
                break;
        }
        old = old->next;
        sym = sym->next;
    }
    return !old && !sym;
}
static TEMPLATEPARAMLIST* mergeTemplateDefaults(TEMPLATEPARAMLIST* old, TEMPLATEPARAMLIST* sym, bool definition)
{
    TEMPLATEPARAMLIST* rv = sym;
    //#ifndef PARSER_ONLY
    while (old && sym)
    {
        if (!definition && old->argsym)
        {
            sym->argsym = old->argsym;
            sym->argsym->tp->templateParam = sym;
        }
        switch (sym->p->type)
        {
            case kw_template:
                sym->p->byTemplate.args = mergeTemplateDefaults(old->p->byTemplate.args, sym->p->byTemplate.args, definition);
                if (old->p->byTemplate.txtdflt && sym->p->byTemplate.txtdflt)
                {
                    if (!CompareLex(old->p->byNonType.txtdflt, sym->p->byNonType.txtdflt))
                        errorsym(ERR_MULTIPLE_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, sym->argsym);
                }
                else if (!sym->p->byTemplate.txtdflt)
                {
                    sym->p->byTemplate.txtdflt = old->p->byTemplate.txtdflt;
                    sym->p->byTemplate.txtargs = old->p->byTemplate.txtargs;
                }
                break;
            case kw_typename:
                if (old->p->byClass.txtdflt && sym->p->byClass.txtdflt)
                {
                    if (!CompareLex(old->p->byNonType.txtdflt, sym->p->byNonType.txtdflt))
                        errorsym(ERR_MULTIPLE_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, sym->argsym);
                }
                else if (!sym->p->byClass.txtdflt)
                {
                    sym->p->byClass.txtdflt = old->p->byClass.txtdflt;
                    sym->p->byClass.txtargs = old->p->byClass.txtargs;
                }
                break;
            case kw_int:
                if (old->p->byNonType.txtdflt && sym->p->byNonType.txtdflt)
                {
                    if (!CompareLex(old->p->byNonType.txtdflt, sym->p->byNonType.txtdflt))
                        errorsym(ERR_MULTIPLE_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, sym->argsym);
                }
                else if (!sym->p->byNonType.txtdflt)
                {
                    sym->p->byNonType.txtdflt = old->p->byNonType.txtdflt;
                    sym->p->byNonType.txtargs = old->p->byNonType.txtargs;
                }
                break;
            case kw_new:  // specialization
                break;
            default:
                break;
        }
        old = old->next;
        sym = sym->next;
    }
    //#endif
    return rv;
}
static void checkTemplateDefaults(TEMPLATEPARAMLIST* args)
{
    SYMBOL* last = nullptr;
    while (args)
    {
        void* txtdflt = nullptr;
        switch (args->p->type)
        {
            case kw_template:
                checkTemplateDefaults(args->p->byTemplate.args);
                txtdflt = args->p->byTemplate.txtdflt;
                break;
            case kw_typename:
                txtdflt = args->p->byClass.txtdflt;
                break;
            case kw_int:
                txtdflt = args->p->byNonType.txtdflt;
                break;
            default:
                break;
        }
        if (last && !txtdflt)
        {
            errorsym(ERR_MISSING_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, last);
            break;
        }
        if (txtdflt)
            last = args->argsym;
        args = args->next;
    }
}
bool matchTemplateSpecializationToParams(TEMPLATEPARAMLIST* param, TEMPLATEPARAMLIST* special, SYMBOL* sp)
{
    while (param && !param->p->packed && special)
    {
        if (param->p->type != special->p->type)
        {
            if (param->p->type != kw_typename || special->p->type != kw_template)
                errorsym(ERR_INCORRECT_ARGS_PASSED_TO_TEMPLATE, sp);
        }
        else if (param->p->type == kw_template)
        {
            if (!exactMatchOnTemplateParams(param->p->byTemplate.args, special->p->byTemplate.dflt->templateParams->next))
                errorsym(ERR_INCORRECT_ARGS_PASSED_TO_TEMPLATE, sp);
        }
        else if (param->p->type == kw_int)
        {
            if (param->p->byNonType.tp->type != bt_templateparam &&
                !comparetypes(param->p->byNonType.tp, special->p->byNonType.tp, false) &&
                (!ispointer(param->p->byNonType.tp) || !isconstzero(param->p->byNonType.tp, special->p->byNonType.dflt)))
                errorsym(ERR_INCORRECT_ARGS_PASSED_TO_TEMPLATE, sp);
        }
        param = param->next;
        special = special->next;
    }
    if (param)
    {
        if (!param->p->packed)
        {
            errorsym(ERR_TOO_FEW_ARGS_PASSED_TO_TEMPLATE, sp);
        }
        else
        {
            param = nullptr;
            special = nullptr;
        }
    }
    else if (special)
    {
        if (special->p->packed)
            special = nullptr;
        else
            errorsym(ERR_TOO_MANY_ARGS_PASSED_TO_TEMPLATE, sp);
    }
    return !param && !special;
}
static void checkMultipleArgs(TEMPLATEPARAMLIST* sym)
{
    while (sym)
    {
        TEMPLATEPARAMLIST* next = sym->next;
        while (next)
        {
            if (next->argsym && !strcmp(sym->argsym->name, next->argsym->name))
            {
                currentErrorLine = 0;
                errorsym(ERR_DUPLICATE_IDENTIFIER, sym->argsym);
            }
            next = next->next;
        }
        if (sym->p->type == kw_template)
        {
            checkMultipleArgs(sym->p->byTemplate.args);
        }
        sym = sym->next;
    }
}
TEMPLATEPARAMLIST* TemplateMatching(LEXEME* lex, TEMPLATEPARAMLIST* old, TEMPLATEPARAMLIST* sym, SYMBOL* sp, bool definition)
{
    (void)lex;
    TEMPLATEPARAMLIST* rv = nullptr;
    currents->sp = sp;
    if (old)
    {
        if (sym->p->bySpecialization.types)
        {
            TEMPLATEPARAMLIST* transfer;
            matchTemplateSpecializationToParams(old->next, sym->p->bySpecialization.types, sp);
            rv = sym;
            transfer = sym->p->bySpecialization.types;
            old = old->next;
            while (old && transfer && !old->p->packed)
            {
                if (transfer->p->type != kw_typename || basetype(transfer->p->byClass.dflt)->type != bt_templateselector)
                {
                    transfer->p->byClass.txtdflt = old->p->byClass.txtdflt;
                    transfer->p->byClass.txtargs = old->p->byClass.txtargs;
                }
                transfer = transfer->next;
                old = old->next;
            }
        }
        else if (sym->next)
        {
            if (!exactMatchOnTemplateParams(old->next, sym->next))
            {
                error(ERR_TEMPLATE_DEFINITION_MISMATCH);
            }
            else
            {
                rv = mergeTemplateDefaults(old, sym, definition);
                checkTemplateDefaults(rv);
            }
        }
        else
        {
            rv = sym;
        }
    }
    else
    {
        rv = sym;
        checkTemplateDefaults(sym->next);
    }
    checkMultipleArgs(sym->next);
    return rv;
}
bool typeHasTemplateArg(TYPE* t);
static bool structHasTemplateArg(TEMPLATEPARAMLIST* tpl)
{
    std::stack<TEMPLATEPARAMLIST*> tps;
    while (tpl)
    {
        if (tpl->p->type == kw_typename)
        {
            if (tpl->p->packed)
            {
                if (tpl->p->byPack.pack)
                {
                    tps.push(tpl->next);
                    tpl = tpl->p->byPack.pack;
                    continue;
                }
            }
            else
            {
                if (typeHasTemplateArg(tpl->p->byClass.dflt))
                    return true;
            }
        }
        else if (tpl->p->type == kw_template)
        {
            if (structHasTemplateArg(tpl->p->byTemplate.args))
                return true;
        }
        tpl = tpl->next;
        if (!tpl && tps.size())
        {
            tpl = tps.top();
            tps.pop();
        }
    }
    return false;
}
bool typeHasTemplateArg(TYPE* t)
{
    if (t)
    {
        while (ispointer(t) || isref(t))
            t = t->btp;
        if (isfunction(t))
        {
            SYMLIST* hr;
            t = basetype(t);
            if (typeHasTemplateArg(t->btp))
                return true;
            hr = t->syms->table[0];
            while (hr)
            {
                if (typeHasTemplateArg(hr->p->tp))
                    return true;
                hr = hr->next;
            }
        }
        else if (basetype(t)->type == bt_templateparam)
            return true;
        else if (isstructured(t))
        {
            TEMPLATEPARAMLIST* tpl = basetype(t)->sp->templateParams;
            if (structHasTemplateArg(tpl))
                return true;
        }
    }
    return false;
}
void TemplateValidateSpecialization(TEMPLATEPARAMLIST* arg)
{
    TEMPLATEPARAMLIST* t = arg->p->bySpecialization.types;
    while (t)
    {
        if (t->p->type == kw_typename && typeHasTemplateArg((TYPE*)t->p->byClass.dflt))
            break;
        t = t->next;
    }
    if (!t)
    {
        error(ERR_PARTIAL_SPECIALIZATION_MISSING_TEMPLATE_PARAMETERS);
    }
}
static void GetPackedTypes(TEMPLATEPARAMLIST** packs, int* count, TEMPLATEPARAMLIST* args)
{
    while (args)
    {
        if (args->p->type == kw_typename)
        {
            if (args->p->packed)
            {
                packs[(*count)++] = args;
            }
        }
        else if (args->p->type == kw_delete)
        {
            GetPackedTypes(packs, count, args->p->byDeferred.args);
        }
        args = args->next;
    }
}
TEMPLATEPARAMLIST** expandArgs(TEMPLATEPARAMLIST** lst, LEXEME* start, SYMBOL* funcsp, TEMPLATEPARAMLIST* select, bool packable)
{
    TEMPLATEPARAMLIST** beginning = lst;
    // this is going to presume that the expression involved
    // is not too long to be cached by the LEXEME mechanism.
    int oldPack = packIndex;
    int count = 0;
    TEMPLATEPARAMLIST* arg[500];
    if (!packable)
    {
        if (select->p->packed && packIndex >= 0)
        {
            TEMPLATEPARAMLIST* templateParam = select->p->byPack.pack;
            int i;
            for (i = 0; i < packIndex && templateParam; i++)
                templateParam = templateParam->next;
            if (templateParam)
            {
                *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                (*lst)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                *(*lst)->p = *templateParam->p;
                (*lst)->p->byClass.dflt = (*lst)->p->byClass.val;
                lst = &(*lst)->next;
                return lst;
            }
        }
        *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
        (*lst)->p = select->p;
        (*lst)->argsym = select->argsym;
        lst = &(*lst)->next;
        return lst;
    }
    GetPackedTypes(arg, &count, select);
    expandingParams++;
    if (count)
    {
        int i;
        int n = CountPacks(arg[0]->p->byPack.pack);  // undefined in local context
        for (i = 1; i < count; i++)
        {
            if (CountPacks(arg[i]->p->byPack.pack) != n)
            {
                error(ERR_PACK_SPECIFIERS_SIZE_MISMATCH);
                break;
            }
        }
        for (i = 0; i < n; i++)
        {
            LEXEME* lex = SetAlternateLex(start);
            TYPE* tp;
            packIndex = i;
            noSpecializationError++;
            lex = get_type_id(lex, &tp, funcsp, sc_parameter, false, true, false);
            noSpecializationError--;
            SetAlternateLex(nullptr);
            if (tp)
            {
                *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                (*lst)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                (*lst)->p->type = kw_typename;
                (*lst)->p->byClass.dflt = tp;
                lst = &(*lst)->next;
            }
        }
    }
    else if (select)
    {
        *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
        (*lst)->p = select->p;
        (*lst)->argsym = select->argsym;
        lst = &(*lst)->next;
    }
    expandingParams--;
    packIndex = oldPack;
    // make it packed again...   we aren't flattening at this point.
    if (select->p->packed)
    {
        TEMPLATEPARAMLIST* current = *beginning;
        lst = beginning;
        *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
        (*lst)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
        (*lst)->argsym = select->argsym;
        *(*lst)->p = *select->p;
        (*lst)->p->byPack.pack = current;
        (*lst)->p->resolved = true;
        lst = &(*lst)->next;
    }
    return lst;
}
TEMPLATEPARAMLIST** expandTemplateSelector(TEMPLATEPARAMLIST** lst, TEMPLATEPARAMLIST* orig, TYPE* tp)
{
    if (tp->sp && tp->sp->sb->templateSelector)
    {
        STRUCTSYM* s = structSyms;
        TEMPLATEPARAMLIST* clst = nullptr;

        while (s && !clst)
        {
            if (s->tmpl)
            {
                SYMBOL* sym = templatesearch(tp->sp->sb->templateSelector->next->sp->name, s->tmpl);
                if (sym)
                    clst = sym->tp->templateParam;
            }
            s = s->next;
        }
        // i'm not sure why this first clause is in here.   I couldn't find anything we compile that uses it.
        // i'm adding necessary things in the if's else.
        if (clst && clst->p->packed && clst->p->byPack.pack)
        {
            clst = clst->p->byPack.pack;
            *lst = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
            (*lst)->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
            (*lst)->p->type = kw_typename;
            (*lst)->p->packed = true;
            (*lst)->argsym = orig->argsym;
            TEMPLATEPARAMLIST** last = &(*lst)->p->byPack.pack;
            while (clst)
            {
                TEMPLATESELECTOR* sel = tp->sp->sb->templateSelector->next->next;
                TYPE* base = clst->p->byClass.val;
                SYMBOL* s = nullptr;
                while (sel)
                {
                    s = search(sel->name, base->syms);
                    if (!s || (s && sel->next && !isstructured(s->tp)))
                        break;
                    if (sel->next)
                        base = basetype(s->tp);
                    sel = sel->next;
                }
                if (!sel && s)
                {
                    *last = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                    (*last)->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                    if (s->sb->storage_class == sc_constant || s->sb->storage_class == sc_constexpr ||
                        s->sb->storage_class == sc_enumconstant)
                    {
                        (*lst)->p->type = (*last)->p->type = kw_int;
                        (*last)->p->byNonType.dflt = s->sb->init->exp;
                        (*last)->p->byNonType.tp = s->tp;
                    }
                    else
                    {
                        (*lst)->p->type = (*last)->p->type = kw_typename;
                        (*last)->p->byClass.dflt = s->tp;
                    }
                }
                last = &(*last)->next;
                clst = clst->next;
            }
            lst = &(*lst)->next;
        }
        else
        {
            if (templateNestingCount && !inTemplateBody)
            {
                *lst = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                (*lst)->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                (*lst)->p->type = kw_typename;
                (*lst)->argsym = orig->argsym;
                (*lst)->p->byClass.dflt = tp;
                lst = &(*lst)->next;
            }
            else if (!templateNestingCount && tp->sp->sb->templateSelector->next->isTemplate)
            {
                clst = tp->sp->sb->templateSelector->next->templateParams;
                if (clst->p->packed && clst->p->byPack.pack)
                {
                    clst = clst->p->byPack.pack;
                    *lst = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                    (*lst)->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                    (*lst)->p->type = kw_typename;
                    (*lst)->p->packed = true;
                    (*lst)->argsym = orig->argsym;
                    TEMPLATEPARAMLIST** last = &(*lst)->p->byPack.pack;
                    while (clst)
                    {
                        TEMPLATEPARAMLIST* old = clst->next;
                        clst->next = nullptr;
                        SYMBOL* sp = GetClassTemplate(tp->sp->sb->templateSelector->next->sp, clst, false);
                        if (sp)
                        {
                            sp = TemplateClassInstantiate(sp, clst, false, sc_global);
                        }
                        clst->next = old;
                        if (sp)
                        {
                            TEMPLATESELECTOR* sel = tp->sp->sb->templateSelector->next->next;
                            TYPE* base = sp->tp;
                            SYMBOL* s = nullptr;
                            while (sel)
                            {
                                STRUCTSYM ss;
                                ss.str = basetype(base)->sp;
                                addStructureDeclaration(&ss);
                                s = classsearch(sel->name, false, false);
                                dropStructureDeclaration();
                                if (!s || (s && sel->next && !isstructured(s->tp)))
                                    break;
                                if (sel->next)
                                    base = basetype(s->tp);
                                sel = sel->next;
                            }
                            if (!sel && s)
                            {
                                *last = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                (*last)->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                                if (s->sb->storage_class == sc_constant || s->sb->storage_class == sc_constexpr ||
                                    s->sb->storage_class == sc_enumconstant)
                                {
                                    (*lst)->p->type = (*last)->p->type = kw_int;
                                    (*last)->p->byNonType.dflt = s->sb->init->exp;
                                    (*last)->p->byNonType.tp = s->tp;
                                }
                                else
                                {
                                    (*lst)->p->type = (*last)->p->type = kw_typename;
                                    (*last)->p->byClass.dflt = s->tp;
                                }
                                last = &(*last)->next;
                            }
                        }
                        clst = clst->next;
                    }
                    lst = &(*lst)->next;
                }
            }
        }
    }
    return lst;
}

bool constructedInt(LEXEME* lex, SYMBOL* funcsp)
{
    // depends on this starting a type
    bool rv = false;
    TYPE* tp;
    LEXEME* placeholder = lex;
    enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
    bool defd = false;
    bool notype = false;
    bool cont = false;
    tp = nullptr;

    lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
    if (lex->type == l_id || MATCHKW(lex, classsel))
    {
        SYMBOL *sp, *strSym = nullptr;
        LEXEME* placeholder = lex;
        bool dest = false;
        nestedSearch(lex, &sp, &strSym, nullptr, &dest, nullptr, false, sc_global, false, false);
        if (Optimizer::cparams.prm_cplusplus)
            prevsym(placeholder);
        if (sp && sp->sb && sp->sb->storage_class == sc_typedef)
            cont = true;
    }
    else
    {
        if (KWTYPE(lex, TT_BASETYPE))
            cont = true;
    }
    if (cont)
    {
        noSpecializationError++;
        lex = getBasicType(lex, funcsp, &tp, nullptr, false, funcsp ? sc_auto : sc_global, &linkage, &linkage2, &linkage3,
                           ac_public, &notype, &defd, nullptr, nullptr, false, false, false);
        noSpecializationError--;
        lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
        if (isint(tp))
        {
            if (MATCHKW(lex, openpa))
            {
                rv = true;
            }
        }
    }
    lex = prevsym(placeholder);
    return rv;
}
LEXEME* GetTemplateArguments(LEXEME* lex, SYMBOL* funcsp, SYMBOL* templ, TEMPLATEPARAMLIST** lst)
{

    if (templ && !strcmp(templ->name, "__make_tuple_types"))
        printf("hi");
    if (templ && !strcmp(templ->name, "__make_tuple_indices"))
        printf("hi");
    if (templ && !strcmp(templ->name, "__apply_quals"))
        printf("hi");
    TEMPLATEPARAMLIST* orig = nullptr;
    bool first = true;
    TYPE* tp = nullptr;
    EXPRESSION* exp = nullptr;
    if (templ)
    {
        auto templ1 = templ;
        if (templ1->tp->type == bt_aggregate)
            templ1 = templ1->tp->syms->table[0]->p;
            orig = templ1->templateParams
            ? (templ1->templateParams->p->bySpecialization.types ? templ1->templateParams->p->bySpecialization.types
                : templ1->templateParams->next)
            : nullptr;
    }
    // entered with lex set to the opening <
    inTemplateArgs++;
    lex = getsym();
    if (!MATCHKW(lex, rightshift) && !MATCHKW(lex, gt))
    {
        do
        {
            tp = nullptr;
            if (MATCHKW(lex, kw_typename) || (orig && orig->p->type != kw_int) ||
                (!orig && startOfType(lex, true) && !constructedInt(lex, funcsp)))
            {
                LEXEME* start = lex;
                noSpecializationError++;
                lex = get_type_id(lex, &tp, funcsp, sc_parameter, false, true, false);
                noSpecializationError--;
                if (!tp)
                    tp = &stdint;
                else if (tp && !templateNestingCount)
                {
                    tp = PerformDeferredInitialization(tp, nullptr);
                }
                if (MATCHKW(lex, begin))  // initializer list?
                {
                    LEXEME* oldlex = lex;
                    lex = expression_func_type_cast(lex, funcsp, &tp, &exp, _F_NOEVAL);
                    if (lex == oldlex)
                    {
                        lex = getsym();
                        errskim(&lex, skim_end);
                        needkw(&lex, end);
                    }
                    goto initlistjoin;
                    // makes it an expression
                }
                if (MATCHKW(lex, ellipse))
                {
                    lex = getsym();
                    if (tp)
                    {
                        TYPE* tp1 = tp;
                        while (isref(tp))
                            tp = basetype(tp)->btp;
                        if (*lst)
                            lst = &(*lst)->next;
                        if (templateNestingCount && tp->type == bt_templateparam)
                        {
                            *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                            (*lst)->argsym = tp->templateParam->argsym;
                            (*lst)->p = tp->templateParam->p;
                            if (isref(tp1))
                            {
                                if (basetype(tp1)->type == bt_lref)
                                {
                                    (*lst)->p->lref = true;
                                    (*lst)->p->rref = false;
                                }
                                else
                                {
                                    (*lst)->p->rref = true;
                                    (*lst)->p->lref = false;
                                }
                            }
                            if (inTemplateSpecialization && !tp->templateParam->p->packed)
                                error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
                        }
                        else if (tp->type == bt_templateparam)
                        {
                            lst = expandArgs(lst, start, funcsp, tp->templateParam, true);
                        }
                        else if (tp->type == bt_templateselector)
                        {
                            tp->sp->sb->postExpansion = true;
                            lst = expandTemplateSelector(lst, orig, tp);
                        }
                        else if (orig && orig->p->type == kw_typename && orig->p->packed && isstructured(tp))
                        {
                            TEMPLATEPARAMLIST* a = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                            TEMPLATEPARAM* b = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                            a->p = b;
                            b->type = kw_typename;
                            b->byClass.dflt = tp;
                            lst = expandArgs(lst, start, funcsp, a, true);
                        }
                        else
                        {
                            *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                            (*lst)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                            if (orig && orig->p->type == kw_template && isstructured(tp) && basetype(tp)->sp->sb->templateLevel)
                            {
                                (*lst)->p->type = kw_template;
                                (*lst)->p->byTemplate.dflt = basetype(tp)->sp;
                            }
                            else
                            {
                                (*lst)->p->type = kw_typename;
                                (*lst)->p->byClass.dflt = tp;
                            }
                            lst = &(*lst)->next;
                        }
                    }
                }
                else if (tp && tp->type == bt_templateparam)
                {
                    if (inTemplateArgs > 1 && tp->templateParam->p->packed)
                    {
                        // unpacked pack gets treated as a single template param
                        *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                        (*lst)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                        *(*lst)->p = *tp->templateParam->p;
                        (*lst)->p->usedAsUnpacked = true;
                        (*lst)->argsym = tp->templateParam->argsym;
                        lst = &(*lst)->next;
                    }
                    else if (inTemplateSpecialization)
                    {
                        *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                        (*lst)->p = tp->templateParam->p;
                        (*lst)->argsym = tp->templateParam->argsym;
                        lst = &(*lst)->next;
                        if (tp->templateParam->p->packed)
                        {
                            TEMPLATEPARAMLIST* find = tp->templateParam->p->byPack.pack;
                            while (find)
                            {
                                if (!find->p->byClass.dflt)
                                    find->p->byClass.dflt = tp;
                                find = find->next;
                            }
                        }
                        else
                        {
                            if (!tp->templateParam->p->byClass.dflt)
                            {
                                if (tp->templateParam->p->type == kw_template)
                                    tp->templateParam->p->byTemplate.dflt = basetype(tp)->sp;
                                else
                                    tp->templateParam->p->byClass.dflt = tp;
                            }
                        }
                    }
                    else
                    {
                        lst = expandArgs(lst, start, funcsp, tp->templateParam, false);
                    }
                }
                else if (orig && orig->p->packed)
                {
                    TEMPLATEPARAMLIST** p;
                    if (first)
                    {
                        *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                        (*lst)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                        if (orig && orig->p->type == kw_template && isstructured(tp) && basetype(tp)->sp->sb->templateLevel)
                        {
                            (*lst)->p->type = kw_template;
                            (*lst)->p->packed = true;
                        }
                        else
                        {
                            (*lst)->p->type = kw_typename;
                            (*lst)->p->packed = true;
                        }
                        first = false;
                    }
                    p = &(*lst)->p->byPack.pack;
                    while (*p)
                        p = &(*p)->next;
                    *p = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                    (*p)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                    if (orig && orig->p->type == kw_template && isstructured(tp) && basetype(tp)->sp->sb->templateLevel)
                    {
                        (*p)->p->type = kw_template;
                        (*p)->p->byTemplate.dflt = basetype(tp)->sp;
                    }
                    else
                    {
                        (*p)->p->type = kw_typename;
                        (*p)->p->byClass.dflt = tp;
                    }
                    if ((*p)->p->type != (*lst)->p->type)
                    {
                        *p = nullptr;
                        error(ERR_PACKED_TEMPLATE_TYPE_MISMATCH);
                    }
                }
                else
                {
                    *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                    (*lst)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                    if (orig)
                        (*lst)->argsym = orig->argsym;
                    if (orig && orig->p->type == kw_template && isstructured(tp) && basetype(tp)->sp->sb->templateLevel)
                    {
                        (*lst)->p->type = kw_template;
                        (*lst)->p->byTemplate.dflt = basetype(tp)->sp;
                    }
                    else
                    {
                        (*lst)->p->type = kw_typename;
                        (*lst)->p->byClass.dflt = tp;
                    }
                    lst = &(*lst)->next;
                }
            }
            else
            {
                exp = nullptr;
                tp = nullptr;
                if (inTemplateSpecialization)
                {
                    if (lex->type == l_id)
                    {
                        SYMBOL* sp;
                        LEXEME* last = lex;
                        lex = nestedSearch(lex, &sp, nullptr, nullptr, nullptr, nullptr, false, sc_global, false, false);
                        if (sp && sp->tp->templateParam)
                        {
                            lex = getsym();
                            if (!MATCHKW(lex, rightshift) && !MATCHKW(lex, gt) && !MATCHKW(lex, comma))
                            {
                                lex = prevsym(last);
                                goto join;
                            }
                            else
                            {
                                *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                (*lst)->p = sp->tp->templateParam->p;
                                lst = &(*lst)->next;
                            }
                        }
                        else
                        {
                            lex = prevsym(last);
                            goto join;
                        }
                    }
                    else
                    {
                        goto join;
                    }
                }
                else
                {
                    STRUCTSYM* s;
                    SYMBOL* name;
                    LEXEME* start;
                    bool skip;
                join:
                    skip = false;
                    s = structSyms;
                    name = nullptr;
                    start = lex;
                    if (ISID(lex))
                    {
                        while (s && !name)
                        {
                            if (s->tmpl)
                                name = templatesearch(lex->value.s.a, s->tmpl);
                            s = s->next;
                        }
                    }
                    if (name)
                    {
                        if (name->tp->type == bt_templateparam)
                        {
                            bool found = true;
                            lex = getsym();

                            if (MATCHKW(lex, classsel))
                            {
                                NAMESPACEVALUELIST* nsv;
                                lex = prevsym(start);
                                lex = nestedPath(lex, &name, &nsv, nullptr, false, sc_parameter, false);
                                if (name && name->tp->type == bt_templateselector)
                                {
                                    lex = getsym();
                                    if (MATCHKW(lex, ellipse))
                                    {
                                        lex = getsym();
                                        lst = expandTemplateSelector(lst, orig, name->tp);
                                        skip = true;
                                        first = false;
                                    }
                                    else
                                    {
                                        lex = prevsym(start);
                                    }
                                }
                                else
                                {
                                    lex = prevsym(start);
                                }
                            }
                            else if (name->tp->templateParam->p->type == kw_int)
                            {
                                if (MATCHKW(lex, ellipse))
                                {
                                    {
                                        TEMPLATEPARAMLIST* tpl = name->tp->templateParam;
                                        TEMPLATEPARAMLIST** working;
                                        *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                        (*lst)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                                        *(*lst)->p = *tpl->p;
                                        (*lst)->p->packed = true;
                                        (*lst)->argsym = name;
                                        working = &(*lst)->p->byPack.pack;
                                        *working = nullptr;

                                        if (tpl->p->packed)
                                        {
                                            tpl = tpl->p->byPack.pack;
                                            while (tpl)
                                            {
                                                *working = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                                (*working)->p = tpl->p;
                                                working = &(*working)->next;
                                                tpl = tpl->next;
                                            }
                                        }
                                        else
                                        {
                                            *working = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                            (*working)->p = tpl->p;
                                        }
                                        lex = getsym();
                                        skip = true;
                                        first = false;
                                    }
                                }
                                else
                                {
                                    lex = prevsym(start);
                                }
                            }
                            else
                            {
                                lex = prevsym(start);
                            }
                        }
                        if (!skip)
                        {
                            lex = expression_no_comma(lex, funcsp, nullptr, &tp, &exp, nullptr, _F_INTEMPLATEPARAMS);
                            if (tp && tp->type == bt_templateparam)
                            {
                                lst = expandArgs(lst, start, funcsp, tp->templateParam, false);
                                skip = true;
                                first = false;
                            }
                        }
                    }
                    else
                    {
                        lex = expression_no_comma(lex, funcsp, nullptr, &tp, &exp, nullptr, _F_INTEMPLATEPARAMS);
                        if (!tp)
                        {
                            error(ERR_EXPRESSION_SYNTAX);
                        }
                    }
                    if (MATCHKW(lex, begin))
                    {
                        error(ERR_EXPECTED_TYPE_NEED_TYPENAME);
                        lex = getsym();
                        errskim(&lex, skim_end);
                        if (lex)
                            needkw(&lex, end);
                    }
                    if (!skip)
                    {
                        if (0)
                        {
                        initlistjoin:
                            start = nullptr;
                        }
                        if (MATCHKW(lex, ellipse))
                        {
                            // lose p
                            lex = getsym();
                            if (templateNestingCount && tp->type == bt_templateparam)
                            {
                                *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                (*lst)->p = tp->templateParam->p;
                                if (!tp->templateParam->p->packed)
                                    error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
                            }
                            else if (templateNestingCount)
                            {
                                *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                (*lst)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                                (*lst)->p->packed = true;
                                (*lst)->p->type = kw_int;
                                (*lst)->p->bySpecialization.types = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                (*lst)->p->bySpecialization.types->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                                (*lst)->p->bySpecialization.types->p->type = kw_int;
                                (*lst)->p->bySpecialization.types->p->byNonType.tp = tp;
                                (*lst)->p->bySpecialization.types->p->byNonType.dflt = exp;
                            }
                            else if (exp->type != en_packedempty)
                            {
                                // this is going to presume that the expression involved
                                // is not too long to be cached by the LEXEME mechanism.
                                int oldPack = packIndex;
                                int count = 0;
                                SYMBOL* arg[200];
                                GatherPackedVars(&count, arg, exp);
                                expandingParams++;
                                if (count)
                                {
                                    int i;
                                    int n = CountPacks(arg[0]->tp->templateParam->p->byPack.pack);
                                    for (i = 1; i < count; i++)
                                    {
                                        if (CountPacks(arg[i]->tp->templateParam->p->byPack.pack) != n)
                                        {
                                            error(ERR_PACK_SPECIFIERS_SIZE_MISMATCH);
                                            break;
                                        }
                                    }
                                    for (i = 0; i < n; i++)
                                    {
                                        LEXEME* lex = SetAlternateLex(start);
                                        packIndex = i;
                                        expression_assign(lex, funcsp, nullptr, &tp, &exp, nullptr, _F_PACKABLE);
                                        if (exp)
                                        {
                                            optimize_for_constants(&exp);
                                            while (exp->type == en_void && exp->right)
                                                exp = exp->right;
                                        }
                                        SetAlternateLex(nullptr);
                                        if (tp)
                                        {
                                            *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                            (*lst)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                                            (*lst)->p->type = kw_int;
                                            (*lst)->p->byNonType.dflt = exp;
                                            (*lst)->p->byNonType.tp = tp;
                                            if (orig)
                                                (*lst)->argsym = orig->argsym;
                                            lst = &(*lst)->next;
                                        }
                                    }
                                }
                                expandingParams--;
                                packIndex = oldPack;
                            }
                        }
                        else
                        {
                            if (exp)
                            {
                                optimize_for_constants(&exp);
                                while (exp->type == en_void && exp->right)
                                    exp = exp->right;
                            }
                            if (tp && tp->type == bt_templateparam)
                            {
                                *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                (*lst)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                                *(*lst)->p = *tp->templateParam->p;
                                if ((*lst)->p->packed)
                                {
                                    (*lst)->p->byPack.pack = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                    (*lst)->p->byPack.pack->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                                    (*lst)->p->byPack.pack->p->type = kw_int;
                                    (*lst)->p->byPack.pack->p->byNonType.dflt = exp;
                                    (*lst)->p->byPack.pack->p->byNonType.val = nullptr;
                                }
                                else
                                {
                                    (*lst)->p->type = kw_int;
                                    (*lst)->p->byNonType.dflt = exp;
                                    (*lst)->p->byNonType.val = nullptr;
                                }
                                if (orig)
                                    (*lst)->argsym = orig->argsym;
                                lst = &(*lst)->next;
                            }
                            else if (orig && orig->p->packed)
                            {
                                TEMPLATEPARAMLIST** p;
                                if (first)
                                {
                                    *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                    (*lst)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                                    (*lst)->p->type = kw_int;
                                    (*lst)->p->packed = true;
                                    if (orig)
                                        (*lst)->argsym = orig->argsym;
                                    first = false;
                                }
                                p = &(*lst)->p->byPack.pack;
                                while (*p)
                                    p = &(*p)->next;
                                *p = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                (*p)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                                (*p)->p->type = kw_int;
                                (*p)->p->byNonType.dflt = exp;
                                (*p)->p->byNonType.tp = tp;
                            }
                            else
                            {
                                checkUnpackedExpression(exp);
                                *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                (*lst)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                                if (exp->type == en_templateparam && exp->v.sp->tp->templateParam->p->byClass.dflt)
                                {
                                    *(*lst)->p = *exp->v.sp->tp->templateParam->p;
                                }
                                else
                                {
                                    (*lst)->p->type = kw_int;
                                    (*lst)->p->byNonType.dflt = exp;
                                    (*lst)->p->byNonType.tp = tp;
                                }
                                lst = &(*lst)->next;
                            }
                        }
                    }
                }
            }
            if (MATCHKW(lex, comma))
                lex = getsym();
            else
                break;
            if (orig && !orig->p->packed)
            {
                orig = orig->next;
            }
        } while (true);
    }
    if (MATCHKW(lex, rightshift))
    {
        lex = getGTSym(lex);
    }
    else
    {
        if (!MATCHKW(lex, gt) && (tp && tp->type == bt_any && tp->sp))
        {
            errorsym(ERR_EXPECTED_END_OF_TEMPLATE_ARGUMENTS_NEAR_UNDEFINED_TYPE, tp->sp);
            errskim(&lex, skim_templateend);
        }
        else
        {
            needkw(&lex, gt);
        }
    }
    inTemplateArgs--;
    return lex;
}
static bool sameTemplateSpecialization(TYPE* P, TYPE* A)
{
    TEMPLATEPARAMLIST *PL, *PA;
    if (!P || !A)
        return false;
    P = basetype(P);
    A = basetype(A);
    if (isref(P))
        P = basetype(P->btp);
    if (isref(A))
        A = basetype(A->btp);
    if (!isstructured(P) || !isstructured(A))
        return false;
    if (P->sp->sb->parentClass != A->sp->sb->parentClass || strcmp(P->sp->name, A->sp->name) != 0)
        return false;
    if (P->sp->sb->templateLevel != A->sp->sb->templateLevel)
        return false;
    // this next if stmt is a horrible hack.
    if (P->size == 0 && !strcmp(P->sp->sb->decoratedName, A->sp->sb->decoratedName))
        return true;
    PL = P->sp->templateParams;
    PA = A->sp->templateParams;
    if (!PL || !PA)  // errors
        return false;
    if (PL->p->bySpecialization.types || !PA->p->bySpecialization.types)
        return false;
    PL = PL->next;
    PA = PA->p->bySpecialization.types;

    if (PL && PA)
    {
        while (PL && PA)
        {
            if (PL->p->type != PA->p->type)
            {
                break;
            }
            else if (P->sp->sb->instantiated || A->sp->sb->instantiated)
            {
                if (PL->p->type == kw_typename)
                {
                    if (!templatecomparetypes(PL->p->byClass.dflt, PA->p->byClass.val, true))
                        break;
                }
                else if (PL->p->type == kw_template)
                {
                    if (!exactMatchOnTemplateParams(PL->p->byTemplate.args, PA->p->byTemplate.args))
                        break;
                }
                else if (PL->p->type == kw_int)
                {
                    if (!templatecomparetypes(PL->p->byNonType.tp, PA->p->byNonType.tp, true))
                        break;
                    //#ifndef PARSER_ONLY
                    if (PL->p->byNonType.dflt && !equalTemplateIntNode(PL->p->byNonType.dflt, PA->p->byNonType.val))
                        break;
                    //#endif
                }
            }
            PL = PL->next;
            PA = PA->next;
        }
        return !PL && !PA;
    }
    return false;
}
bool exactMatchOnTemplateSpecialization(TEMPLATEPARAMLIST* old, TEMPLATEPARAMLIST* sym)
{
    while (old && sym)
    {
        if (old->p->type != sym->p->type)
            return false;
        switch (old->p->type)
        {
            case kw_typename:
                if (!sameTemplateSpecialization(old->p->byClass.dflt, sym->p->byClass.val))
                {
                    if (!templatecomparetypes(old->p->byClass.dflt, sym->p->byClass.val, true))
                        return false;
                    if (!templatecomparetypes(sym->p->byClass.val, old->p->byClass.dflt, true))
                        return false;
                }
                break;
            case kw_template:
                if (old->p->byTemplate.dflt != sym->p->byTemplate.val)
                    return false;
                break;
            case kw_int:
                if (!templatecomparetypes(old->p->byNonType.tp, sym->p->byNonType.tp, true))
                    return false;
                //#ifndef PARSER_ONLY
                if (old->p->byNonType.dflt && !equalTemplateIntNode(old->p->byNonType.dflt, sym->p->byNonType.val))
                    return false;
                //#endif
                break;
            default:
                break;
        }
        old = old->next;
        sym = sym->next;
    }
    return !old && !sym;
}
SYMBOL* FindSpecialization(SYMBOL* sym, TEMPLATEPARAMLIST* templateParams)
{
    SYMBOL* candidate;
    SYMLIST* lst = sym->sb->specializations;
    while (lst)
    {
        candidate = lst->p;
        if (candidate->templateParams &&
            exactMatchOnTemplateArgs(templateParams->next, candidate->templateParams->p->bySpecialization.types))
        {
            return candidate;
        }
        lst = lst->next;
    }
    return nullptr;
}
SYMBOL* LookupSpecialization(SYMBOL* sym, TEMPLATEPARAMLIST* templateParams)
{
    TYPE* tp;
    SYMBOL* candidate = nullptr;
    SYMLIST *lst = sym->sb->specializations, **last;
    // maybe we know this specialization
    while (lst)
    {
        candidate = lst->p;
        if (candidate->templateParams && exactMatchOnTemplateArgs(templateParams->p->bySpecialization.types,
                                                                  candidate->templateParams->p->bySpecialization.types))
        {
            TEMPLATEPARAMLIST* l = templateParams;
            TEMPLATEPARAMLIST* r = candidate->templateParams;
            while (l && r)
            {
                l = l->next;
                r = r->next;
            }
            if (!l && !r)
                return candidate;
        }
        lst = lst->next;
    }
    // maybe we know this as an instantiation
    lst = sym->sb->instantiations;
    last = &sym->sb->instantiations;
    while (lst)
    {
        candidate = lst->p;
        if (candidate->templateParams &&
            exactMatchOnTemplateSpecialization(templateParams->p->bySpecialization.types, candidate->templateParams->next))
        {
            *last = (*last)->next;
            break;
        }
        last = &(*last)->next;
        lst = lst->next;
    }
    if (!lst)
    {
        candidate = clonesym(sym);
        candidate->tp = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
        *candidate->tp = *sym->tp;
        candidate->tp->sp = candidate;
        UpdateRootTypes(candidate->tp);
    }
    else
    {
        candidate->sb->maintemplate = candidate;
    }
    candidate->templateParams = templateParams;
    lst = (SYMLIST*)Alloc(sizeof(SYMLIST));
    lst->p = candidate;
    lst->next = sym->sb->specializations;
    sym->sb->specializations = lst;
    candidate->sb->overloadName = sym->sb->overloadName;
    candidate->sb->specialized = true;
    if (!candidate->sb->parentTemplate)
        candidate->sb->parentTemplate = sym;
    candidate->sb->baseClasses = nullptr;
    candidate->sb->vbaseEntries = nullptr;
    candidate->sb->vtabEntries = nullptr;
    tp = (TYPE*)Alloc(sizeof(TYPE));
    *tp = *candidate->tp;
    UpdateRootTypes(tp);
    candidate->tp = tp;
    candidate->tp->syms = nullptr;
    candidate->tp->tags = nullptr;
    candidate->sb->baseClasses = nullptr;
    candidate->sb->declline = candidate->sb->origdeclline = preProcessor->GetErrLineNo();
    candidate->sb->realdeclline = preProcessor->GetRealLineNo();
    candidate->sb->declfile = candidate->sb->origdeclfile = litlate(preProcessor->GetErrFile().c_str());
    candidate->sb->trivialCons = false;
    SetLinkerNames(candidate, lk_cdecl);
    return candidate;
}
static bool matchTemplatedType(TYPE* old, TYPE* sym, bool strict)
{
    while (1)
    {
        if (isconst(old) == isconst(sym) && isvolatile(old) == isvolatile(sym))
        {
            old = basetype(old);
            sym = basetype(sym);
            if (old->type == sym->type || (isfunction(old) && isfunction(sym)))
            {
                switch (old->type)
                {
                    case bt_struct:
                    case bt_class:
                    case bt_union:
                        return old->sp == sym->sp;
                    case bt_func:
                    case bt_ifunc:
                        if (!matchTemplatedType(old->btp, sym->btp, strict))
                            return false;
                        {
                            SYMLIST* hro = old->syms->table[0];
                            SYMLIST* hrs = sym->syms->table[0];
                            if ((hro->p)->sb->thisPtr)
                                hro = hro->next;
                            if ((hrs->p)->sb->thisPtr)
                                hrs = hrs->next;
                            while (hro && hrs)
                            {
                                if (!matchTemplatedType((hro->p)->tp, (hrs->p)->tp, strict))
                                    return false;
                                hro = hro->next;
                                hrs = hrs->next;
                            }
                            return !hro && !hrs;
                        }
                    case bt_pointer:
                    case bt_lref:
                    case bt_rref:
                        if (old->array == sym->array && old->size == sym->size)
                        {
                            old = old->btp;
                            sym = sym->btp;
                            break;
                        }
                        return false;
                    case bt_templateparam:
                        return old->templateParam->p->type == sym->templateParam->p->type;
                    default:
                        return true;
                }
            }
            else
            {
                return !strict && old->type == bt_templateparam;
            }
        }
        else
        {
            return false;
        }
    }
}
SYMBOL* SynthesizeResult(SYMBOL* sym, TEMPLATEPARAMLIST* params);
static bool ValidateArgsSpecified(TEMPLATEPARAMLIST* params, SYMBOL* func, INITLIST* args);
static void saveParams(SYMBOL** table, int count)
{
    int i;
    for (i = 0; i < count; i++)
    {
        if (table[i])
        {
            TEMPLATEPARAMLIST* params = table[i]->templateParams;
            while (params)
            {
                if (params->p->type != kw_new)
                    params->p->hold = params->p->byClass.val;
                params = params->next;
            }
        }
    }
}
static void restoreParams(SYMBOL** table, int count)
{
    int i;
    for (i = 0; i < count; i++)
    {
        if (table[i])
        {
            TEMPLATEPARAMLIST* params = table[i]->templateParams;
            while (params)
            {
                if (params->p->type != kw_new)
                    params->p->byClass.val = (TYPE*)params->p->hold;
                params = params->next;
            }
        }
    }
}
SYMBOL* LookupFunctionSpecialization(SYMBOL* overloads, SYMBOL* sp)
{
    SYMBOL* found1 = nullptr;
    SYMBOL* sym = (SYMBOL*)overloads->tp->syms->table[0]->p;
    SYMBOL* sd = getStructureDeclaration();
    saveParams(&sd, 1);
    if (sym->sb->templateLevel && !sym->sb->instantiated &&
        (!sym->sb->parentClass || sym->sb->parentClass->sb->templateLevel != sym->sb->templateLevel))
    {
        found1 = detemplate(overloads->tp->syms->table[0]->p, nullptr, sp->tp);
        if (found1 && allTemplateArgsSpecified(found1, found1->templateParams->next) && exactMatchOnTemplateArgs(found1->templateParams, sp->templateParams))
        {
            TEMPLATEPARAMLIST* tpl;
            SYMLIST* hr = overloads->tp->syms->table[0]->next;
            while (hr)
            {
                if (exactMatchOnTemplateArgs(found1->templateParams, hr->p->templateParams))
                    return hr->p;
                hr = hr->next;
            }
            sp->templateParams->p->bySpecialization.types = copyParams(found1->templateParams->next, false);
            tpl = sp->templateParams->p->bySpecialization.types;
            while (tpl)
            {
                tpl->p->byClass.dflt = tpl->p->byClass.val;
                tpl->p->byClass.val = nullptr;
                tpl = tpl->next;
            }
            SetLinkerNames(sp, lk_cdecl);
            found1 = sp;
        }
        else
        {
            found1 = nullptr;
        }
    }
    restoreParams(&sd, 1);
    return found1;
}
LEXEME* TemplateArgGetDefault(LEXEME** lex, bool isExpression)
{
    LEXEME *rv = nullptr, **cur = &rv;
    LEXEME *current = *lex, *end = current;
    // this presumes that the template or expression is small enough to be cached...
    // may have to adjust it later
    // have to properly parse the default value, because it may have
    // embedded expressions that use '<'
    if (isExpression)
    {
        TYPE* tp;
        EXPRESSION* exp;
        end = expression_no_comma(current, nullptr, nullptr, &tp, &exp, nullptr, _F_INTEMPLATEPARAMS);
    }
    else
    {
        TYPE* tp;
        end = get_type_id(current, &tp, nullptr, sc_cast, false, true, false);
    }
    while (current && current != end)
    {
        *cur = (LEXEME*)Alloc(sizeof(LEXEME));
        **cur = *current;
        (*cur)->next = nullptr;
        if (ISID(current))
            (*cur)->value.s.a = litlate((*cur)->value.s.a);
        current = current->next;
        cur = &(*cur)->next;
    }
    *lex = end;
    return rv;
}
static SYMBOL* templateParamId(TYPE* tp, const char* name)
{
    SYMBOL* rv = (SYMBOL*)Alloc(sizeof(SYMBOL));
    rv->tp = tp;
    rv->name = name;
    return rv;
}
static LEXEME* TemplateHeader(LEXEME* lex, SYMBOL* funcsp, TEMPLATEPARAMLIST** args)
{
    inTemplateHeader++;
    TEMPLATEPARAMLIST **lst = args, **begin = args, *search;
    STRUCTSYM* structSyms = nullptr;
    if (needkw(&lex, lt))
    {
        while (1)
        {
            if (MATCHKW(lex, gt) || MATCHKW(lex, rightshift))
                break;
            *args = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
            (*args)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
            lex = TemplateArg(lex, funcsp, *args, lst);
            if (*args)
            {
                if (!structSyms)
                {
                    structSyms = (STRUCTSYM*)Alloc(sizeof(STRUCTSYM));
                    structSyms->tmpl = *args;
                    addTemplateDeclaration(structSyms);
                }
                args = &(*args)->next;
            }
            if (!MATCHKW(lex, comma))
                break;
            lex = getsym();
        }
        search = *begin;
        while (search)
        {
            if (search->p->byClass.txtdflt)
            {
                Optimizer::LIST *lbegin = nullptr, **hold = &lbegin;
                search = *begin;
                while (search)
                {
                    *hold = (Optimizer::LIST*)(Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
                    (*hold)->data = search->argsym;
                    hold = &(*hold)->next;
                    search = search->next;
                }
                search = (*begin);
                while (search)
                {
                    if (search->p->byClass.txtdflt)
                        search->p->byClass.txtargs = lbegin;
                    search = search->next;
                }
                break;
            }
            search = search->next;
        }
        if (MATCHKW(lex, rightshift))
            lex = getGTSym(lex);
        else
            needkw(&lex, gt);
    }
    inTemplateHeader--;
    return lex;
}
static LEXEME* TemplateArg(LEXEME* lex, SYMBOL* funcsp, TEMPLATEPARAMLIST* arg, TEMPLATEPARAMLIST** lst)
{
    LEXEME *current = lex;
    LEXEME *txttype = nullptr;
    switch (KW(lex))
    {
        TYPE *tp, *tp1;
        EXPRESSION* exp1;
        SYMBOL* sp;
        case kw_class:
        case kw_typename:
            arg->p->type = kw_typename;
            arg->p->packed = false;
            lex = getsym();
            if (MATCHKW(lex, ellipse))
            {
                arg->p->packed = true;
                lex = getsym();
            }
            if (ISID(lex) || MATCHKW(lex, classsel))
            {
                SYMBOL *sym = nullptr, *strsym = nullptr;
                NAMESPACEVALUELIST* nsv = nullptr;

                lex = nestedPath(lex, &strsym, &nsv, nullptr, false, sc_global, false);
                if (strsym)
                {
                    if (strsym->tp->type == bt_templateselector)
                    {
                        TEMPLATESELECTOR* l;
                        l = strsym->sb->templateSelector;
                        while (l->next)
                            l = l->next;
                        sp = sym = templateParamId(strsym->tp, l->name);
                        lex = getsym();
                        tp = strsym->tp;
                        goto non_type_join;
                    }
                    else if (ISID(lex))
                    {
                        TEMPLATESELECTOR** last;
                        tp = (TYPE*)Alloc(sizeof(TYPE));
                        tp->rootType = tp;
                        tp->type = bt_templateselector;
                        sp = sym = templateParamId(tp, litlate(lex->value.s.a));
                        tp->sp = sym;
                        last = &sym->sb->templateSelector;
                        *last = (TEMPLATESELECTOR*)Alloc(sizeof(TEMPLATESELECTOR));
                        (*last)->sp = nullptr;
                        last = &(*last)->next;
                        *last = (TEMPLATESELECTOR*)Alloc(sizeof(TEMPLATESELECTOR));
                        (*last)->sp = strsym;
                        if (strsym->sb->templateLevel)
                        {
                            (*last)->isTemplate = true;
                            (*last)->templateParams = strsym->templateParams;
                        }
                        last = &(*last)->next;
                        *last = (TEMPLATESELECTOR*)Alloc(sizeof(TEMPLATESELECTOR));
                        (*last)->name = litlate(lex->value.s.a);
                        last = &(*last)->next;
                        lex = getsym();
                        goto non_type_join;
                    }
                    else
                    {
                        lex = getsym();
                        error(ERR_TYPE_NAME_EXPECTED);
                        break;
                    }
                }
                else if (ISID(lex))
                {
                    TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
                    tp->rootType = tp;
                    tp->type = bt_templateparam;
                    tp->templateParam = arg;
                    arg->argsym = templateParamId(tp, litlate(lex->value.s.a));
                    lex = getsym();
                }
                else
                {
                    lex = getsym();
                    error(ERR_TYPE_NAME_EXPECTED);
                    break;
                }
            }
            else
            {
                TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
                tp->type = bt_templateparam;
                tp->rootType = tp;
                tp->templateParam = arg;
                arg->argsym = templateParamId(tp, AnonymousName());
            }
            if (MATCHKW(lex, assign))
            {
                if (arg->p->packed)
                {
                    error(ERR_CANNOT_USE_DEFAULT_WITH_PACKED_TEMPLATE_PARAMETER);
                }
                lex = getsym();
                arg->p->byClass.txtdflt = TemplateArgGetDefault(&lex, false);
                if (!arg->p->byClass.txtdflt)
                {
                    error(ERR_CLASS_TEMPLATE_DEFAULT_MUST_REFER_TO_TYPE);
                }
            }
            if (!MATCHKW(lex, gt) && !MATCHKW(lex, leftshift) && !MATCHKW(lex, comma))
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            break;
        case kw_template:
            arg->p->type = kw_template;
            lex = getsym();
            lex = TemplateHeader(lex, funcsp, &arg->p->byTemplate.args);
            if (arg->p->byTemplate.args)
                dropStructureDeclaration();
            arg->p->packed = false;
            if (!MATCHKW(lex, kw_class))
            {
                error(ERR_TEMPLATE_TEMPLATE_PARAMETER_MUST_NAME_CLASS);
            }
            else
            {
                lex = getsym();
            }
            if (MATCHKW(lex, ellipse))
            {
                arg->p->packed = true;
                lex = getsym();
            }
            if (ISID(lex))
            {
                TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
                tp->type = bt_templateparam;
                tp->rootType = tp;
                tp->templateParam = arg;
                arg->argsym = templateParamId(tp, litlate(lex->value.s.a));
                lex = getsym();
            }
            else
            {
                TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
                tp->type = bt_templateparam;
                tp->rootType = tp;
                tp->templateParam = arg;
                arg->argsym = templateParamId(tp, AnonymousName());
            }
            if (MATCHKW(lex, assign))
            {
                if (arg->p->packed)
                {
                    error(ERR_CANNOT_USE_DEFAULT_WITH_PACKED_TEMPLATE_PARAMETER);
                }
                arg->p->byTemplate.txtdflt = TemplateArgGetDefault(&lex, false);
                if (!arg->p->byTemplate.txtdflt)
                {
                    error(ERR_TEMPLATE_TEMPLATE_PARAMETER_MISSING_DEFAULT);
                }
            }
            if (!MATCHKW(lex, gt) && !MATCHKW(lex, leftshift) && !MATCHKW(lex, comma))
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            break;
        default:  // non-type
        {
            enum e_lk linkage, linkage2, linkage3;
            bool defd;
            bool notype;
            linkage = lk_none;
            linkage2 = lk_none;
            linkage3 = lk_none;
            defd = false;
            notype = false;
            arg->p->type = kw_int;
            arg->p->packed = false;
            tp = nullptr;
            sp = nullptr;
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
            lex = getBasicType(lex, funcsp, &tp, nullptr, false, funcsp ? sc_auto : sc_global, &linkage, &linkage2, &linkage3,
                               ac_public, &notype, &defd, nullptr, nullptr, false, true, false);
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
            if (tp->type == bt_any)
            {
                LEXEME *end = current;
                LEXEME **cur = &txttype;
                end = lex;
                // if the type didn't resolve, we want to cache it then evaluate it later...
                while (current && current != end)
                {
                    *cur = (LEXEME*)Alloc(sizeof(LEXEME));
                    **cur = *current;
                    (*cur)->next = nullptr;
                    if (ISID(current))
                        (*cur)->value.s.a = litlate((*cur)->value.s.a);
                    current = current->next;
                    cur = &(*cur)->next;
                }
            }
            // get type qualifiers
            if (!ISID(lex) && !MATCHKW(lex, ellipse))
            {
                lex = getBeforeType(lex, funcsp, &tp, &sp, nullptr, nullptr, false, sc_cast, &linkage, &linkage2, &linkage3, false,
                    false, true, false); /* fixme at file scope init */
            }
            if (MATCHKW(lex, ellipse))
            {
                arg->p->packed = true;
                lex = getsym();
            }
            // get the name
            lex = getBeforeType(lex, funcsp, &tp, &sp, nullptr, nullptr, false, sc_cast, &linkage, &linkage2, &linkage3, false,
                false, false, false); /* fixme at file scope init */
            sizeQualifiers(tp);
            if (!tp || notype)
            {
                if (sp && (*lst)->argsym)
                {
                    while (*lst)
                    {
                        if (!(*lst)->argsym)
                            break;
                        if (!strcmp((*lst)->argsym->name, sp->name))
                        {
                            tp = (*lst)->argsym->tp;
                            if (ISID(lex))
                            {
                                sp = templateParamId(tp, litlate(lex->value.s.a));
                                lex = getsym();
                            }
                            else
                            {
                                sp = templateParamId(tp, AnonymousName());
                            }
                            goto non_type_join;
                        }
                        lst = &(*lst)->next;
                    }
                }
                error(ERR_INVALID_TEMPLATE_PARAMETER);
            }
            else
            {
                TYPE* tpa;
                if (!sp)
                {
                    sp = templateParamId(nullptr, AnonymousName());
                }
            non_type_join:
                tpa = (TYPE*)Alloc(sizeof(TYPE));
                tpa->type = bt_templateparam;
                tpa->templateParam = arg;
                tpa->rootType = tpa;
                if (sp->sb)
                    sp->sb->storage_class = sc_templateparam;
                sp->tp = tpa;
                arg->p->type = kw_int;
                arg->argsym = (SYMBOL*)Alloc(sizeof(SYMBOL));
                *arg->argsym = *sp;
                arg->argsym->sb = nullptr;
                if (isarray(tp) || isfunction(tp))
                {
                    if (isarray(tp))
                        tp = tp->btp;
                    tp1 = (TYPE*)Alloc(sizeof(TYPE));
                    tp1->type = bt_pointer;
                    tp1->size = getSize(bt_pointer);
                    tp1->btp = tp;
                    tp1->rootType = tp1;
                    tp = tp1;
                }
                arg->p->byNonType.tp = tp;
                arg->p->byNonType.txttype = txttype;
                if (basetype(tp)->type != bt_templateparam && basetype(tp)->type != bt_templateselector &&
                    basetype(tp)->type != bt_enum && !isint(tp) && !ispointer(tp) && basetype(tp)->type != bt_lref &&
                    (!templateNestingCount || basetype(tp)->type != bt_any))
                {
                    error(ERR_NONTYPE_TEMPLATE_PARAMETER_INVALID_TYPE);
                }
                if (sp)
                {
                    if (MATCHKW(lex, assign))
                    {
                        tp1 = nullptr;
                        exp1 = nullptr;
                        lex = getsym();
                        arg->p->byNonType.txtdflt = TemplateArgGetDefault(&lex, true);
                        if (!arg->p->byNonType.txtdflt)
                        {
                            error(ERR_IDENTIFIER_EXPECTED);
                        }
                        else
                        {
                            TYPE* tp = nullptr;
                            EXPRESSION *exp = nullptr;
                            LEXEME* lex = SetAlternateLex(arg->p->byNonType.txtdflt);
                            lex = expression_no_comma(lex, nullptr, nullptr, &tp, &exp, nullptr, 0);
                            SetAlternateLex(nullptr);
                            if (tp && isintconst(exp))
                            {
                                arg->p->byNonType.dflt = exp;
//                                arg->p->byNonType.txtdflt = nullptr;
                            }
                        }
                    }
                }
            }
            break;
        }
    }

    return lex;
}
static bool matchArg(TEMPLATEPARAMLIST* param, TEMPLATEPARAMLIST* arg)
{
    if (param->p->type != arg->p->type)
    {
        return false;
    }
    else if (param->p->type == kw_template)
    {
        if (arg->p->byTemplate.dflt)
            if (!exactMatchOnTemplateParams(param->p->byTemplate.args, arg->p->byTemplate.dflt->templateParams->next))
                return false;
    }
    return true;
}
bool TemplateIntroduceArgs(TEMPLATEPARAMLIST* sym, TEMPLATEPARAMLIST* args)
{
    if (sym)
        sym = sym->next;
    while (sym && args)
    {
        if (!matchArg(sym, args))
            return false;
        switch (args->p->type)
        {
            case kw_typename:
                sym->p->byClass.val = args->p->byClass.dflt;
                break;
            case kw_template:
                sym->p->byTemplate.val = args->p->byTemplate.dflt;
                break;
            case kw_int:
                sym->p->byNonType.val = args->p->byNonType.dflt;
                break;
            default:
                break;
        }
        sym = sym->next;
        args = args->next;
    }
    return true;
}
TYPE* SolidifyType(TYPE* tp)
{
    for (auto v = tp; v; v = v->btp)
    {
        if (isstructured(v))
        {
            TYPE *rv = nullptr, **last = &rv;
            for (auto v = tp; v; v = v->btp)
            {
                *last = (TYPE*)Alloc(sizeof(TYPE));
                **last = *v;
                last = &(*last)->btp;
            }
            UpdateRootTypes(rv);
            for (auto v = rv; v; v = v->btp)
                if (isstructured(v))
                {
                    SYMBOL* old = basetype(v)->sp;
                    basetype(v)->sp = clonesym(old);
                    SYMBOL* sym = basetype(v)->sp;
                    if (!sym->sb->mainsym)
                        sym->sb->mainsym = old;
                    TYPE* tp1 = (TYPE*)Alloc(sizeof(TYPE));
                    *tp1 = *sym->tp;
                    sym->tp = tp1;
                    sym->tp->sp = sym;
                    sym->templateParams = SolidifyTemplateParams(copyParams(sym->templateParams, true));
                    UpdateRootTypes(sym->tp);
                    break;
                }
            return rv;
        }
    }
    return tp;
}
TEMPLATEPARAMLIST* SolidifyTemplateParams(TEMPLATEPARAMLIST* in)
{
    for (auto v = in; v; v = v->next)
    {
        if (v->p->type == kw_typename)
        {
            if (v->p->packed)
            {
                for (auto q = v->p->byPack.pack; q; q = q->next)
                    q->p->byClass.val = SolidifyType(q->p->byClass.val);
            }
            else
            {
                v->p->byClass.val = SolidifyType(v->p->byClass.val);
            }
        }
    }
    return in;
}

static TEMPLATEPARAMLIST* copyParams(TEMPLATEPARAMLIST* t, bool alsoSpecializations)
{
    if (t)
    {
        TEMPLATEPARAMLIST *rv = nullptr, **last = &rv, *parse, *rv1;
        parse = t;
        while (parse)
        {
            SYMBOL* sp;
            *last = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
            (*last)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
            *((*last)->p) = *(parse->p);
            (*last)->argsym = parse->argsym ? clonesym(parse->argsym) : nullptr;
            sp = (*last)->argsym;
            if (sp)
            {
                sp = clonesym(sp);
                sp->tp = (TYPE*)Alloc(sizeof(TYPE));
                sp->tp->type = bt_templateparam;
                sp->tp->templateParam = *last;
                sp->tp->rootType = sp->tp;
                (*last)->argsym = sp;
            }

            last = &(*last)->next;
            parse = parse->next;
        }
        if (t->p->type == kw_new && alsoSpecializations)
        {
            last = &rv->p->bySpecialization.types;
            parse = t->p->bySpecialization.types;
            while (parse)
            {
                *last = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                (*last)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                *((*last)->p) = *(parse->p);
                (*last)->argsym = parse->argsym ? clonesym(parse->argsym) : nullptr;
                last = &(*last)->next;
                parse = parse->next;
            }
        }
        parse = t;
        rv1 = rv;
        while (parse)
        {
            if (parse->p->type == kw_int)
            {
                if (parse->p->byNonType.tp->type == bt_templateparam)
                {
                    TEMPLATEPARAMLIST* t1 = t;
                    TEMPLATEPARAMLIST* rv2 = rv;
                    while (t1)
                    {
                        if (t1->p->type == kw_typename)
                        {
                            if (t1->p == parse->p->byNonType.tp->templateParam->p)
                            {
                                TYPE* old = rv1->p->byNonType.tp;
                                rv1->p->byNonType.tp = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
                                *rv1->p->byNonType.tp = *old;
                                UpdateRootTypes(rv1->p->byNonType.tp);
                                rv1->p->byNonType.tp->templateParam = rv2;
                                break;
                            }
                        }
                        t1 = t1->next;
                        rv2 = rv2->next;
                    }
                }
            }
            parse = parse->next;
            rv1 = rv1->next;
        }
        return rv;
    }
    return t;
}
static SYMBOL* SynthesizeTemplate(TYPE* tp, SYMBOL* rvt, sym::_symbody* rvs, TYPE* tpt)
{
    SYMBOL* rv;
    TEMPLATEPARAMLIST *r = nullptr, **last = &r;
    TEMPLATEPARAMLIST* p = tp->sp->templateParams->p->bySpecialization.types;
    if (!p)
        p = tp->sp->templateParams->next;
    while (p)
    {
        *last = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
        (*last)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
        *((*last)->p) = *(p->p);
        last = &(*last)->next;
        p = p->next;
    }
    rv = rvt;
    *rv = *tp->sp;
    rv->sb = rvs;
    *rv->sb = *tp->sp->sb;
    rv->sb->symRef = nullptr;
    rv->tp = tpt;
    *rv->tp = *tp;
    UpdateRootTypes(rv->tp);
    rv->tp->sp = rv;
    rv->templateParams = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
    rv->templateParams->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
    rv->templateParams->p->type = kw_new;  // specialization
    rv->templateParams->p->bySpecialization.types = r;
    return rv;
}

void SynthesizeQuals(TYPE*** last, TYPE** qual, TYPE*** lastQual)
{
    if (*qual)
    {
        TYPE* p = **last;
        TYPE* v = *qual;
        int sz = basetype(**last)->size;
        while (p && p->type == bt_derivedfromtemplate)
        {
            **last = (TYPE*)Alloc(sizeof(TYPE));
            ***last = *p;
            *last = &(**last)->btp;
            p = p->btp;
        }
        if ((*qual)->rootType && isref(*qual))
        {
            while (p && p != basetype(p))
            {
                **last = (TYPE*)Alloc(sizeof(TYPE));
                ***last = *p;
                *last = &(**last)->btp;
                p = p->btp;
            }
        }
        while (v)
        {
            **last = (TYPE*)Alloc(sizeof(TYPE));
            ***last = *v;
            if (!(**last)->rootType || !isref(**last))
                (**last)->size = sz;
            *last = &(**last)->btp;
            v = v->btp;
        }
        **last = nullptr;
        while (p)
        {
            **last = (TYPE*)Alloc(sizeof(TYPE));
            ***last = *p;
            if (p->btp)
                *last = &(**last)->btp;
            p = p->btp;
        }
        *lastQual = qual;
        *qual = nullptr;
    }
}

static EXPRESSION* copy_expression_data(EXPRESSION* exp)
{
    EXPRESSION* n = (EXPRESSION*)nzAlloc(sizeof(EXPRESSION));
    *n = *exp;
    // Easiest way to do a deep copy without including the rest of the tree
    n->left = nullptr;
    n->right = nullptr;
    return n;
}
// A better method to do this overall would probably be in Knuth's work, I did find something in the first volume (specifically
// section 2.3.2), however I was unable to replicate it and could not properly get the termination condition. This specific piece of
// code is translated from: https://stackoverflow.com/a/9647910 Stackoverflow user: kasavbere
EXPRESSION* copy_expression(EXPRESSION* head)
{
    if (head == nullptr)
        return nullptr;
    std::stack<EXPRESSION*> q;
    q.push(head);
    std::stack<EXPRESSION*> q2;
    EXPRESSION* nRoot = copy_expression_data(head);
    q2.push(nRoot);
    while (!q.empty())
    {
        EXPRESSION* n = q.top();
        q.pop();
        EXPRESSION* fresh = q2.top();
        q2.pop();
        if (n->left != nullptr)
        {
            q.push(n->left);
            fresh->left = copy_expression_data(n->left);
            q2.push(fresh->left);
        }
        if (n->right != nullptr)
        {
            q.push(n->right);
            fresh->right = copy_expression_data(n->right);
            q2.push(fresh->right);
        }
    }
    return nRoot;
}
static TEMPLATEPARAMLIST* paramsToDefault(TEMPLATEPARAMLIST* templateParams)
{
    TEMPLATEPARAMLIST *params = nullptr, **pt = &params, *find = templateParams;
    while (find)
    {
        *pt = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
        (*pt)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
        *(*pt)->p = *find->p;
        if (find->p->packed)
        {
            (*pt)->p->byPack.pack = paramsToDefault(find->p->byPack.pack);
            if (find->p->lref || find->p->rref)
            {
                TEMPLATEPARAMLIST* tpl = (*pt)->p->byPack.pack;
                while (tpl)
                {
                    // don't cascade references here...
                    if (isref(tpl->p->byClass.dflt))
                    {
                        TYPE* cursor = tpl->p->byClass.dflt;
                        TYPE *newType = nullptr, **tp1 = &newType;
                        while (1)
                        {
                            *tp1 = (TYPE*)Alloc(sizeof(TYPE));
                            **tp1 = *cursor;
                            if (cursor == cursor->rootType)
                            {
                                // the reference collapsing rules say that if either ref is an lref we get an lref,
                                // else we get an rref
                                (*tp1)->type = find->p->lref || (*tp1)->type == bt_lref ? bt_lref : bt_rref;
                                break;
                            }
                            tp1 = &(*tp1)->btp;
                            cursor = cursor->btp;
                        };
                        (*tp1)->btp = cursor->btp;
                        tpl->p->byClass.dflt = newType;
                    }
                    else
                    {
                        TYPE* tp1 = (TYPE*)Alloc(sizeof(TYPE));
                        tp1->type = find->p->lref ? bt_lref : bt_rref;
                        tp1->size = getSize(bt_pointer);
                        tp1->btp = tpl->p->byClass.dflt;
                        tp1->rootType = tp1;
                        tpl->p->byClass.dflt = tp1;
                    }
                    tpl = tpl->next;
                }
            }
        }
        else
        {
            (*pt)->p->byClass.dflt = find->p->byClass.val;
            (*pt)->p->byClass.val = nullptr;
        }
        pt = &(*pt)->next;
        find = find->next;
    }
    return params;
}

static TEMPLATEPARAMLIST** addStructParam(TEMPLATEPARAMLIST** pt, TEMPLATEPARAMLIST* search, TEMPLATEPARAMLIST* enclosing)
{
    TEMPLATEPARAMLIST* find = enclosing->next;
    if (!search->argsym || (search->p->byClass.dflt && !search->p->byClass.val))
    {
        if (!search->p->byClass.dflt)
            return nullptr;
        *pt = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
        (*pt)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
        *(*pt)->p = *search->p;
        if (!templateNestingCount || instantiatingTemplate)
            (*pt)->p->byClass.dflt = SynthesizeType((*pt)->p->byClass.dflt, enclosing, false);
    }
    else
    {
        while (find && strcmp(search->argsym->name, find->argsym->name) != 0)
        {
            find = find->next;
        }
        if (!find)
        {
            SYMBOL* sym = nullptr;
            STRUCTSYM* s = structSyms;
            while (s && !sym)
            {
                if (s->tmpl)
                    sym = templatesearch(search->argsym->name, s->tmpl);
                s = s->next;
            }
            if (!sym)
                return nullptr;
            if (sym->tp->type != bt_templateparam || sym->tp->templateParam->p->type != kw_typename)
                return nullptr;
            find = sym->tp->templateParam;
        }
        *pt = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
        (*pt)->p = find->p;
    }
    return &(*pt)->next;
}
static TYPE* SynthesizeStructure(TYPE* tp_in, TEMPLATEPARAMLIST* enclosing)
{
    TYPE* tp = basetype(tp_in);
    if (isref(tp))
        tp = basetype(tp->btp);
    if (isstructured(tp))
    {
        SYMBOL* sp = basetype(tp)->sp;
        if (sp->sb->templateLevel && !sp->sb->instantiated)
        {
            if (!allTemplateArgsSpecified(sp, sp->templateParams->next))
            {
                TEMPLATEPARAMLIST *params = nullptr, **pt = &params, *search = sp->templateParams->next;
                while (search)
                {
                    if (search->p->type == kw_typename)
                    {
                        if (search->p->byClass.dflt && search->p->byClass.dflt->type == bt_templateselector &&
                            search->p->byClass.dflt->sp->sb->postExpansion)
                        {
                            auto temp = search->p->byClass.dflt->sp->sb->templateSelector->next->templateParams;
                            // this may needs some work with recursing templateselectors inside templateselectors...
                            search->p->byClass.dflt->sp->sb->templateSelector->next->templateParams =
                                paramsToDefault(search->p->byClass.dflt->sp->sb->templateSelector->next->templateParams);
                            pt = expandTemplateSelector(pt, enclosing, search->p->byClass.dflt);
                            search->p->byClass.dflt->sp->sb->templateSelector->next->templateParams = temp;
                        }
                        else if (search->p->byClass.dflt && (search->p->byClass.dflt)->type == bt_memberptr)
                        {
                            *pt = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                            (*pt)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                            *(*pt)->p = *search->p;
                            (*pt)->p->byClass.dflt = SynthesizeType(search->p->byClass.dflt, enclosing, false);
                            pt = &(*pt)->next;
                        }
                        else
                        {
                            pt = addStructParam(pt, search, enclosing);
                            if (!pt)
                                return nullptr;
                        }
                    }
                    else
                    {
                        *pt = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                        (*pt)->p = search->p;
                        pt = &(*pt)->next;
                    }
                    search = search->next;
                }
                sp = GetClassTemplate(sp, params, false);
            }
            else
            {
                TEMPLATEPARAMLIST* params = paramsToDefault(sp->templateParams->next);
                SYMBOL* sp1 = clonesym(sp);
                sp1->tp = (TYPE*)Alloc(sizeof(TYPE));
                *sp1->tp = *sp->tp;
                UpdateRootTypes(sp1->tp);

                sp1->tp->sp = sp1;
                sp = sp1;
                sp = GetClassTemplate(sp, params, false);
            }

            if (sp)
            {
                TYPE *tp1 = nullptr, **tpp = &tp1;
                int sz = sp->tp->size;
                if (isref(tp_in))
                    sz = tp_in->size;
                if (isconst(tp_in))
                {
                    *tpp = (TYPE*)Alloc(sizeof(TYPE));
                    (*tpp)->size = sz;
                    (*tpp)->type = bt_const;
                    tpp = &(*tpp)->btp;
                }
                if (isvolatile(tp_in))
                {
                    *tpp = (TYPE*)Alloc(sizeof(TYPE));
                    (*tpp)->size = sz;
                    (*tpp)->type = bt_volatile;
                    tpp = &(*tpp)->btp;
                }
                if (isref(tp_in))
                {
                    *tpp = (TYPE*)Alloc(sizeof(TYPE));
                    (*tpp)->size = sz;
                    (*tpp)->type = basetype(tp_in)->type;
                    tpp = &(*tpp)->btp;
                }
                *tpp = sp->tp;
                return tp1;
            }
        }
    }
    return nullptr;
}
static INITLIST* ExpandArguments(EXPRESSION* exp)
{
    INITLIST *rv = nullptr, **ptr = &rv;
    INITLIST* arguments = exp->v.func->arguments;
    while (arguments)
    {
        if (arguments->exp->type == en_func)
        {
            FUNCTIONCALL* call = arguments->exp->v.func;
            INITLIST* args2 = call->arguments;
            if (args2->tp->type == bt_templateparam && args2->tp->templateParam->p->packed)
            {
                INITLIST* rv = nullptr;
                INITLIST** ptr = &rv;
                TEMPLATEPARAMLIST* scan = args2->tp->templateParam->p->byPack.pack;
                *ptr = nullptr;
                TEMPLATEPARAMLIST* old1 = call->templateParams;
                INITLIST* old2 = call->arguments;
                TEMPLATEPARAMLIST tpl = {0};
                TEMPLATEPARAM pm = {kw_typename};
                tpl.p = &pm;
                pm.type = kw_typename;
                call->templateParams = &tpl;
                while (scan)
                {
                    INITLIST* rv1 = (INITLIST*)Alloc(sizeof(INITLIST));
                    rv1->exp = intNode(en_c_i, 0);
                    rv1->tp = scan->p->byClass.val;
                    call->arguments = rv1;
                    call->templateParams->p->byClass.val = call->templateParams->p->byClass.dflt = rv1->tp;
                    rv1->tp = LookupTypeFromExpression(arguments->exp, nullptr, false);
                    *ptr = rv1;
                    ptr = &(*ptr)->next;
                    scan = scan->next;
                }
                call->arguments = old2;  // DAL
                call->templateParams = old1;
                return rv;
            }
            else
            {
                args2->tp = LookupTypeFromExpression(args2->exp, nullptr, false);
            }
        }
        arguments = arguments->next;
    }
    return exp->v.func->arguments;
}
static void PushPopDefaults(std::deque<TYPE*>& defaults, TEMPLATEPARAMLIST* tpl, bool dflt, bool push)
{
    std::stack<TEMPLATEPARAMLIST*> stk;
    while (tpl)
    {
        if (tpl->p->type == kw_typename && tpl->p->byClass.val && isstructured(tpl->p->byClass.val) && basetype(tpl->p->byClass.val)->sp->templateParams)
        {
            if (push)
            {
                defaults.push_back(dflt ? tpl->p->byClass.dflt : tpl->p->byClass.val);
            }
            else if (defaults.size())
            {
                if (dflt)
                {
                    tpl->p->byClass.dflt = defaults.front();                           
                }
                else
                {
                    tpl->p->byClass.val = defaults.front();
                }
                defaults.pop_front();
            }
            else
            {
                if (dflt)
                {
                    tpl->p->byClass.dflt = nullptr;
                }
                else
                {
                    tpl->p->byClass.val = nullptr;
                }
            }
            stk.push(tpl->next);
            tpl = basetype(tpl->p->byClass.val)->sp->templateParams->next;
            continue;
        }
        if (tpl->p->packed)
        {
            stk.push(tpl->next);
            tpl = tpl->p->byPack.pack;
            continue;

        }
        if (push)
        {
            defaults.push_back(dflt ? tpl->p->byClass.dflt : tpl->p->byClass.val);
        }
        else if (defaults.size())
        {
            if (dflt)
            {
                tpl->p->byClass.dflt = defaults.front();
            }
            else
            {
                tpl->p->byClass.val = defaults.front();
            }
            defaults.pop_front();
        }
        else
        {
            if (dflt)
            {
                tpl->p->byClass.dflt = nullptr;
            }
            else
            {
                tpl->p->byClass.val = nullptr;
            }
        }
        tpl = tpl->next;
        while (!tpl && !stk.empty())
        {
            tpl = stk.top();
            stk.pop();
        }
    }
}
TYPE* LookupTypeFromExpression(EXPRESSION* exp, TEMPLATEPARAMLIST* enclosing, bool alt)
{
    EXPRESSION* funcList[100];
    int count = 0;
    switch (exp->type)
    {
        case en_dot:
        case en_pointsto:
        {
            TYPE *tp = LookupTypeFromExpression(exp->left, nullptr, false);
            while (exp->type == en_dot || exp->type == en_pointsto)
            {
                if (exp->type == en_pointsto)
                {
                    if (!ispointer(tp))
                        return nullptr;
                    tp = basetype(tp)->btp;
                }
                EXPRESSION *next = exp->right;
                if (next->type == en_dot || next->type == en_pointsto)
                {
                    next = exp->left;
                }
                STRUCTSYM s;
                while (isref(tp))
                    tp = basetype(tp)->btp;
                s.str = basetype(tp)->sp;
                addStructureDeclaration(&s);
                if (next->type == en_func)
                {
                    TYPE *ctype = tp;
                    SYMBOL *sym = classsearch(next->v.func->sp->name, false, false);
                    if (!sym)
                    {
                        dropStructureDeclaration();
                        break;
                    }
                    FUNCTIONCALL *func = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
                    *func = *next->v.func;
                    func->sp = sym;
                    TYPE *thistp = (TYPE*)Alloc(sizeof(TYPE));
                    thistp->type = bt_pointer;
                    thistp->size = getSize(bt_pointer);
                    thistp->btp = tp;
                    func->thistp = thistp;
                    func->thisptr = intNode(en_c_i, 0);
                    sym =
                        GetOverloadedFunction(&ctype, &func->fcall, sym, func, nullptr, true, false, true, 0);
                    if (!sym)
                    {
                        dropStructureDeclaration();
                        break;
                    }
                    EXPRESSION* temp = varNode(en_func, sym);
                    temp->v.func = func;
                    temp = exprNode(en_thisref, temp, nullptr);
                    temp->v.t.thisptr = intNode(en_c_i, 0);
                    temp->v.t.tp = tp;
                    tp = LookupTypeFromExpression(temp, nullptr, false);
                }
                else
                {
                    SYMBOL *sym = classsearch(GetSymRef(next)->v.sp->name, false, false);
                    if (!sym)
                    {
                        dropStructureDeclaration();
                        break;
                    }
                    tp = sym->tp;
                }
                dropStructureDeclaration();
                exp = exp->right;
            }
            if (exp->type != en_dot && exp->type != en_pointsto)
                return tp;
            return nullptr;
        }
        case en_void:
            return nullptr;
        case en_not_lvalue:
        case en_lvalue:
        case en_argnopush:
        case en_voidnz:
        case en_shiftby:
            return LookupTypeFromExpression(exp->left, enclosing, alt);
        case en_global:
        case en_auto:
        case en_labcon:
        case en_absolute:
        case en_pc:
        case en_const:
        case en_threadlocal:
            return &stdpointer;
        case en_x_label:
        case en_l_ref:
            return &stdpointer;
        case en_c_bit:
        case en_c_bool:
        case en_x_bool:
        case en_x_bit:
        case en_l_bool:
        case en_l_bit:
            return &stdbool;
        case en_c_c:
        case en_x_c:
        case en_l_c:
            return &stdchar;
        case en_c_uc:
        case en_x_uc:
        case en_l_uc:
            return &stdunsignedchar;
        case en_c_wc:
        case en_x_wc:
        case en_l_wc:
            return &stdwidechar;
        case en_c_s:
        case en_x_s:
        case en_l_s:
            return &stdshort;
        case en_c_u16:
        case en_x_u16:
        case en_l_u16:
            return &stdchar16t;
        case en_c_us:
        case en_x_us:
        case en_l_us:
            return &stdunsignedshort;
        case en_c_i:
        case en_x_i:
        case en_l_i:
        case en_structelem:
            return &stdint;
        case en_c_ui:
        case en_x_ui:
        case en_l_ui:
            return &stdunsigned;
        case en_x_inative:
        case en_l_inative:
            return &stdinative;
        case en_x_unative:
        case en_l_unative:
            return &stdunative;
        case en_c_u32:
        case en_x_u32:
        case en_l_u32:
            return &stdchar32t;
        case en_c_l:
        case en_x_l:
        case en_l_l:
            return &stdlong;
        case en_c_ul:
        case en_x_ul:
        case en_l_ul:
            return &stdunsignedlong;
        case en_c_ll:
        case en_x_ll:
        case en_l_ll:
            return &stdlonglong;
        case en_c_ull:
        case en_x_ull:
        case en_l_ull:
            return &stdunsignedlonglong;
        case en_c_f:
        case en_x_f:
        case en_l_f:
            return &stdfloat;
        case en_c_d:
        case en_x_d:
        case en_l_d:
            return &stddouble;
        case en_c_ld:
        case en_x_ld:
        case en_l_ld:
            return &stdlongdouble;
        case en_c_p:
        case en_x_p:
            return &stdpointer;
        case en_c_string:
        case en_l_string:
        case en_x_string:
            return &std__string;
        case en_x_object:
        case en_l_object:
            return &std__object;
        case en_l_p:
        {
            TYPE* tp = LookupTypeFromExpression(exp->left, enclosing, alt);
            if (tp && ispointer(tp))
                tp = basetype(tp)->btp;
            return tp;
        }

        case en_c_sp:
        case en_x_sp:
        case en_l_sp:
            return &stdchar16t;
        case en_c_fp:
        case en_x_fp:
        case en_l_fp:
            return &stdpointer;  // fixme
        case en_c_fc:
        case en_x_fc:
        case en_l_fc:
            return &stdfloatcomplex;
        case en_c_dc:
        case en_x_dc:
        case en_l_dc:
            return &stddoublecomplex;
        case en_c_ldc:
        case en_x_ldc:
        case en_l_ldc:
            return &stdlongdoublecomplex;
        case en_c_fi:
        case en_x_fi:
        case en_l_fi:
            return &stdfloatimaginary;
        case en_c_di:
        case en_x_di:
        case en_l_di:
            return &stddoubleimaginary;
        case en_c_ldi:
        case en_x_ldi:
        case en_l_ldi:
            return &stdlongdoubleimaginary;
        case en_nullptr:
            return &stdnullpointer;
        case en_memberptr:
            return &stdpointer;
        case en_mp_as_bool:
            return &stdbool;
        case en_mp_compare:
            return &stdbool;
        case en_trapcall:
        case en_intcall:
            return &stdvoid;
        case en_funcret:
            while (exp->type == en_funcret)
            {
                funcList[count++] = exp;
                exp = exp->left;
            }
            /* fall through */
        case en_func:
        {
            TYPE* rv;
            EXPRESSION* exp1 = nullptr;
            if (basetype(exp->v.func->functp)->type != bt_aggregate)
            {
                if (exp->v.func->asaddress)
                {
                    rv = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
                    rv->type = bt_pointer;
                    rv->rootType = rv;
                    rv->size = getSize(bt_pointer);
                    rv->btp = exp->v.func->functp;
                }
                else
                {
                    rv = basetype(exp->v.func->functp)->btp;
                }
            }
            else
            {
                std::deque<TYPE*> defaults;
                TYPE* tp1 = nullptr;
                SYMBOL* sp;
                TEMPLATEPARAMLIST* tpl = exp->v.func->templateParams;
                while (tpl)
                {
                    defaults.push_back(tpl->p->byClass.dflt);
                    if (tpl->p->byClass.val)
                    {
                        tpl->p->byClass.dflt = tpl->p->byClass.val;
                    }
                    tpl = tpl->next;
                }
                sp = GetOverloadedFunction(&tp1, &exp1, exp->v.func->sp, exp->v.func, nullptr, false, false, false, 0);
                tpl = exp->v.func->templateParams;
                count = 0;
                while (tpl)
                {
                    tpl->p->byClass.dflt = defaults.front();
                    defaults.pop_front();
                    tpl = tpl->next;
                }
                if (sp)
                {
                    rv = basetype(sp->tp)->btp;
                }
                else
                {
                    rv = nullptr;
                }
            }
            while (count && rv)
            {
                TYPE* rve = rv;
                exp = funcList[--count]->left;
                while (isref(rve))
                    rve = basetype(rve)->btp;
                if (isfuncptr(rve))
                {
                    //                    rv = rve;
                    rv = basetype(basetype(rve)->btp)->btp;
                    if (isconst(rve))
                    {
                        // to make LIBCXX happy
                        rve = (TYPE*)Alloc(sizeof(TYPE));
                        rve->type = bt_const;
                        rve->size = rv->size;
                        rve->btp = rv;
                        rve->rootType = rv->rootType;
                        rv = rve;
                    }
                }
                else if (isfunction(rve))
                {
                    rv = basetype(rve)->btp;
                }
                else if (isstructured(rve))
                {
                    if (exp->v.func)
                    {
                        exp->v.func->arguments = ExpandArguments(exp);
                    }
                    rv = rve;
                    //                    exp->v.func->sp = basetype(rve)->sp;
                    //                   exp->v.func->functp = exp->v.func->sp->tp;
                    if (!exp->v.func)// || !insertOperatorParams(nullptr, &rv, &exp1, exp->v.func, 0))
                        rv = &stdvoid;
                    if (isconst(rve))
                    {
                        // to make LIBCXX happy
                        rve = (TYPE*)Alloc(sizeof(TYPE));
                        rve->type = bt_const;
                        rve->size = rv->size;
                        rve->btp = rv;
                        rve->rootType = rv->rootType;
                        rv = rve;
                    }
                }
                else
                    break;
            }
            return rv;
        }
        case en_lt:
        case en_le:
        case en_gt:
        case en_ge:
        case en_eq:
        case en_ne:
        case en_land:
        case en_lor:
        case en_ugt:
        case en_uge:
        case en_ule:
        case en_ult:
            return &stdbool;
        case en_uminus:
        case en_not:
        case en_compl:
        case en_ascompl:
        case en_lsh:
        case en_rsh:
        case en_ursh:
        case en_rshd:
        case en_autoinc:
        case en_autodec:
        case en_bits:
            return LookupTypeFromExpression(exp->left, enclosing, alt);
        case en_assign:
        {
            TYPE* tp1 = LookupTypeFromExpression(exp->left, enclosing, alt);
            if (tp1)
            {
                while (isref(tp1))
                    tp1 = basetype(tp1)->btp;
                // there probably ought to be a bunch of such sanity checks throughout this function,
                // this is the minimum to make LIBCXX happy.
                if (isconst(tp1))
                    return nullptr;
            }
            return tp1;
        }
        case en_templateparam:
            if (exp->v.sp->tp->templateParam->p->type == kw_typename)
                return exp->v.sp->tp->templateParam->p->byClass.val;
            return nullptr;
        case en_templateselector:
        {
            EXPRESSION* exp1 = copy_expression(exp);
            optimize_for_constants(&exp1);
            if (exp1->type != en_templateselector)
                return LookupTypeFromExpression(exp1, enclosing, alt);
            return nullptr;
        }
        // the following several work because the front end should have cast both expressions already
        case en_cond:
            return LookupTypeFromExpression(exp->right->left, enclosing, alt);
        case en_arraymul:
        case en_arraylsh:
        case en_arraydiv:
        case en_arrayadd:
        case en_structadd:
        case en_add:  // these are a little buggy because of the 'shortening' optimization
        case en_sub:
        case en_mul:
        case en_mod:
        case en_div:
        case en_and:
        case en_or:
        case en_xor:
        case en_umul:
        case en_udiv:
        case en_umod:
            return LookupTypeFromExpression(exp->left, enclosing, alt);
        case en_blockclear:
        case en_stackblock:
        case en_blockassign:
            switch (exp->left->type)
            {
                case en_global:
                case en_auto:
                case en_labcon:
                case en_absolute:
                case en_pc:
                case en_const:
                case en_threadlocal:
                    return exp->left->v.sp->tp;
                default:
                    break;
            }
            if (exp->right)
                switch (exp->right->type)
                {
                    case en_global:
                    case en_auto:
                    case en_labcon:
                    case en_absolute:
                    case en_pc:
                    case en_const:
                    case en_threadlocal:
                        return exp->right->v.sp->tp;
                    default:
                        break;
                }
            return nullptr;
        case en_thisref:
            return LookupTypeFromExpression(exp->left, enclosing, alt);
        default:
            diag("LookupTypeFromExpression: unknown expression type");
            return nullptr;
    }
}
TYPE* TemplateLookupTypeFromDeclType(TYPE* tp)
{
    EXPRESSION* exp = tp->templateDeclType;
    return LookupTypeFromExpression(exp, nullptr, false);
}
int count = 0;
TYPE* SynthesizeType(TYPE* tp, TEMPLATEPARAMLIST* enclosing, bool alt)
{
    TYPE *rv = &stdany, **last = &rv;
    TYPE *qual = nullptr, **lastQual = &qual;
    TYPE* tp_in = tp;
    while (1)
    {
        switch (tp->type)
        {
            case bt_typedef:
                tp = tp->btp;
                break;
            case bt_pointer:
                if (isarray(tp) && tp->etype)
                {
                    tp->etype = SynthesizeType(tp->etype, enclosing, alt);
                }
                *last = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
                **last = *tp;
                (*last)->btp = SynthesizeType(tp->btp, enclosing, alt);
                SynthesizeQuals(&last, &qual, &lastQual);
                UpdateRootTypes(rv);
                return rv;
            case bt_templatedecltype:
                *last = LookupTypeFromExpression(tp->templateDeclType, enclosing, alt);
                if (!*last)
                {
                    return &stdany;
                }
                *last = SynthesizeType(*last, enclosing, alt);
                SynthesizeQuals(&last, &qual, &lastQual);
                UpdateRootTypes(rv);
                return rv;
            case bt_templateselector:
            {
                SYMBOL* sp;
                SYMBOL* ts = tp->sp->sb->templateSelector->next->sp;
                TEMPLATESELECTOR* find = tp->sp->sb->templateSelector->next->next;
                if (tp->sp->sb->templateSelector->next->isTemplate)
                {
                    TEMPLATEPARAMLIST* current = tp->sp->sb->templateSelector->next->templateParams;
                    TEMPLATEPARAMLIST* symtp = ts->templateParams->next;
                    std::deque<TYPE*> defaults;
                    std::stack<TEMPLATEPARAMLIST*> tps;
                    PushPopDefaults(defaults, current, true, true);
                    bool failed = false;
                    while (current)
                    {
                        if (current->p->packed)
                        {
                            tps.push(current->next);
                            current = current->p->byPack.pack;
                        }
                        if (current)
                        {
                            if (current->p->type == kw_typename && current->p->byClass.dflt)
                            {
                                current->p->byClass.dflt = SynthesizeType(current->p->byClass.dflt, enclosing, alt);
                                if (!current->p->byClass.dflt || current->p->byClass.dflt->type == bt_any)
                                {
                                    failed = true;
                                    break;
                                }
                            }
                            else if (current->p->type == kw_int)
                            {
                                if (current->p->byNonType.dflt)
                                {
                                    current->p->byNonType.dflt = copy_expression(current->p->byNonType.dflt);
                                    optimize_for_constants(&current->p->byNonType.dflt);
                                }
                                else if (current->p->byNonType.val)
                                {
                                    current->p->byNonType.dflt = copy_expression(current->p->byNonType.val);
                                    optimize_for_constants(&current->p->byNonType.dflt);
                                }
                            }
                            if (symtp)
                            {
                                if (!current->argsym)
                                    current->argsym = symtp->argsym;
                                symtp = symtp->next;
                            }
                            current = current->next;
                            if (!current && tps.size())
                            {
                                current = tps.top();
                                tps.pop();
                            }
                        }
                    }
                    current = tp->sp->sb->templateSelector->next->templateParams;
                    if (failed)
                    {
                        PushPopDefaults(defaults, current, true, false);
                        return &stdany;
                    }
                    sp = GetClassTemplate(ts, current, true);
                    if (sp)
                        sp = TemplateClassInstantiateInternal(sp, current, false);
                    current = tp->sp->sb->templateSelector->next->templateParams;
                    PushPopDefaults(defaults, current, true, false);
                    if (sp)
                        tp = sp->tp;
                    else
                        tp = &stdany;
                }
                else
                {
                    tp = basetype(ts->tp);
                    if (tp->type == bt_templateselector)
                    {
                        tp = SynthesizeType(tp, enclosing, alt);
                        if (tp->type == bt_any)
                        {
                            return tp;
                        }
                        sp = basetype(tp)->sp;
                    }
                    else if (tp->type == bt_templateparam)
                    {
                        if (tp->templateParam->p->type != kw_typename)
                        {
                            return &stdany;
                        }
                        tp = alt ? tp->templateParam->p->byClass.temp : tp->templateParam->p->byClass.val;
                        if (!tp)
                        {
                            return &stdany;
                        }
                        sp = tp->sp;
                    }
                    else
                    {
                        sp = tp->sp;
                    }
                }
                while (find && sp)
                {
                    if (!isstructured(tp))
                        break;

                    sp = search(find->name, basetype(tp)->syms);
                    if (!sp)
                    {
                        sp = classdata(find->name, basetype(tp)->sp, nullptr, false, false);
                        if (sp == (SYMBOL*)-1)
                            sp = nullptr;
                    }
                    if (sp)
                        tp = sp->tp;
                    else
                        break;
                    find = find->next;
                }
                if (!find && tp)
                {
                    while (tp->type == bt_typedef)
                        tp = tp->btp;
                    if (tp->type == bt_templateparam)
                    {
                        *last = tp->templateParam->p->byClass.dflt;
                        if (!*last)
                            *last = &stdany;
                    }
                    else
                    {
                        *last = tp;
                    }
                    if (rv)
                    {
                        tp = (TYPE*)Alloc(sizeof(TYPE));
                        tp->type = bt_derivedfromtemplate;
                        tp->rootType = tp;
                        tp->btp = rv;
                        tp->size = rv->size;
                        rv = tp;
                        SynthesizeQuals(&last, &qual, &lastQual);
                    }
                    UpdateRootTypes(rv);
                    return rv;
                }
                return &stdany;
            }
            case bt_rref:
                if (qual == nullptr && tp->btp->type == bt_templateparam && tp->btp->templateParam->p->byClass.val &&
                    tp->btp->templateParam->p->byClass.val->type == bt_lref)
                {
                    TYPE* tp1 = tp->btp->templateParam->p->byClass.val;
                    tp->btp->templateParam->p->byClass.val = basetype(tp1)->btp;
                    tp = tp1;
                }
                // fall through
            case bt_lref:
                *last = (TYPE*)Alloc(sizeof(TYPE));
                **last = *tp;
                last = &(*last)->btp;
                tp = tp->btp;
                break;
            case bt_const:
            case bt_volatile:
            case bt_restrict:
            case bt_far:
            case bt_near:
            case bt_seg:
            case bt_lrqual:
            case bt_rrqual:
            case bt_derivedfromtemplate:
                *lastQual = (TYPE*)Alloc(sizeof(TYPE));
                **lastQual = *tp;
                (*lastQual)->btp = nullptr;
                lastQual = &(*lastQual)->btp;
                tp = tp->btp;
                break;
            case bt_memberptr:
                *last = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
                **last = *tp;
                {
                    TYPE* tp1 = tp->sp->tp;
                    if (tp1->type == bt_templateparam)
                    {
                        tp1 = tp1->templateParam->p->byClass.val;
                        (*last)->sp = tp1->sp;
                    }
                }
                (*last)->btp = SynthesizeType(tp->btp, enclosing, alt);
                SynthesizeQuals(&last, &qual, &lastQual);
                UpdateRootTypes(rv);
                return rv;

            case bt_ifunc:
            case bt_func:
            {
                TYPE* func;
                SYMLIST* hr = tp->syms->table[0];
                *last = (TYPE*)Alloc(sizeof(TYPE));
                **last = *tp;
                (*last)->syms = CreateHashTable(1);
                (*last)->btp = nullptr;
                func = *last;
                SynthesizeQuals(&last, &qual, &lastQual);
                if (*last)
                    last = &(*last)->btp;
                while (hr)
                {
                    SYMBOL* sp = hr->p;
                    if (sp->packed)
                    {
                        NormalizePacked(sp->tp);
                        if (sp->tp->templateParam && sp->tp->templateParam->p->packed)
                        {
                            TEMPLATEPARAMLIST* templateParams = sp->tp->templateParam->p->byPack.pack;
                            bool first = true;
                            sp->tp->templateParam->p->index = 0;
                            if (templateParams)
                            {
                                TYPE **last1, *qual1, *temp, **lastQual1 = &temp;
                                TYPE **btp = nullptr, *next;
                                next = sp->tp;
                                while (ispointer(next) || isref(next))
                                    next = basetype(next)->btp;
                                next = basetype(next);
                                qual1 = sp->tp;
                                if (qual1 == next)
                                    qual1 = nullptr;
                                else
                                {
                                    while (qual1->btp != next)
                                        qual1 = qual1->btp;
                                    btp = &qual1->btp;
                                    qual1 = sp->tp;
                                }
                                while (templateParams)
                                {
                                    SYMBOL* clone = clonesym(sp);
                                    TYPE* tp1;
                                    clone->tp = SynthesizeType(templateParams->p->byClass.val, enclosing, alt);
                                    if (!first)
                                    {
                                        clone->name = clone->sb->decoratedName = AnonymousName();
                                        clone->packed = false;
                                    }
                                    else
                                    {
                                        clone->tp->templateParam = sp->tp->templateParam;
                                    }
                                    if (qual1)
                                    {
                                        if (btp != &qual1->btp || qual1->type != bt_rref ||
                                            basetype(clone->tp)->type != bt_lref)  // unadorrned rref = forwarding
                                        {
                                            *btp = nullptr;
                                            last1 = &clone->tp;
                                            SynthesizeQuals(&last1, &qual1, &lastQual1);
                                            *btp = next;
                                        }
                                    }
                                    tp1 = (TYPE*)Alloc(sizeof(TYPE));
                                    tp1->type = bt_derivedfromtemplate;
                                    tp1->rootType = tp1;
                                    tp1->btp = clone->tp;
                                    tp1->size = clone->tp->size;
                                    tp1->templateParam = clone->tp->templateParam;
                                    clone->tp = tp1;
                                    UpdateRootTypes(tp1);
                                    templateParams->p->packsym = clone;
                                    insert(clone, func->syms);
                                    first = false;
                                    templateParams = templateParams->next;
                                    sp->tp->templateParam->p->index++;
                                }
                            }
                            else
                            {
                                SYMBOL* clone = clonesym(sp);
                                clone->tp = SynthesizeType(&stdany, enclosing, alt);
                                clone->tp->templateParam = sp->tp->templateParam;
                                insert(clone, func->syms);
                            }
                        }
                    }
                    else
                    {
                        SYMBOL* clone = clonesym(sp);
                        TYPE* tp1;
                        insert(clone, func->syms);
                        clone->tp = SynthesizeType(clone->tp, enclosing, alt);
                        if (clone->tp->type != bt_void && clone->tp->type != bt_any)
                        {
                            tp1 = (TYPE*)Alloc(sizeof(TYPE));
                            tp1->type = bt_derivedfromtemplate;
                            tp1->rootType = tp1;
                            tp1->btp = clone->tp;
                            tp1->size = clone->tp->size;
                            clone->tp = tp1;
                            UpdateRootTypes(tp1);
                        }
                    }
                    hr = hr->next;
                }
                tp = tp->btp;
                break;
            }
            case bt_templateparam:
            {
                TEMPLATEPARAMLIST* tpa = tp->templateParam;
                if (tpa->p->packed)
                {
                    int i;
                    int index = tpa->p->index;
                    tpa = tpa->p->byPack.pack;
                    for (i = 0; i < index; i++)
                        tpa = tpa->next;
                    if (!tpa)
                    {
                        UpdateRootTypes(rv);
                        return rv;
                    }
                }
                if (tpa->p->type == kw_typename)
                {
                    TYPE *type = alt ? tpa->p->byClass.temp : tpa->p->byClass.val, *typx = type;
                    while (type && type->type == bt_templateparam)
                    {
                        type = type->templateParam->p->byClass.val;
                        if (type == typx)
                        {
                            return &stdany;
                        }
                    }
                    if (type)
                    {
                        TYPE* tx = qual;
                        *last = (TYPE*)Alloc(sizeof(TYPE));
                        **last = *type;
                        (*last)->templateTop = true;
                        while (tx)
                        {
                            if (tx->type == bt_const)
                            {
                                (*last)->templateConst = true;
                                ++count;
                                if (((int)(*last) & 0xfff) == 0xd50)
                                    printf("hi");
                            }
                            if (tx->type == bt_volatile)
                                (*last)->templateVol = true;
                            tx = tx->btp;
                        }
                        tp = (TYPE*)Alloc(sizeof(TYPE));
                        tp->type = bt_derivedfromtemplate;
                        tp->rootType = tp;
                        tp->btp = rv;
                        tp->size = rv->size;
                        rv = tp;
                        SynthesizeQuals(&last, &qual, &lastQual);
                    }
                    else
                    {
                        if (!templateNestingCount && tpa->argsym)
                        {
                            STRUCTSYM* p = structSyms;
                            while (p)
                            {
                                if (p->tmpl)
                                {
                                    SYMBOL* s = templatesearch(tpa->argsym->name, p->tmpl);
                                    if (s && s->tp->templateParam->p->byClass.val)
                                    {
                                        *last = (TYPE*)Alloc(sizeof(TYPE));
                                        **last = *s->tp->templateParam->p->byClass.val;
                                        break;
                                    }
                                }
                                p = p->next;
                            }
                            if (!p)
                            {
                                *last = (TYPE*)Alloc(sizeof(TYPE));
                                **last = *tp;
                            }
                        }
                        else
                        {
                            *last = (TYPE*)Alloc(sizeof(TYPE));
                            **last = *tp;
                        }
                    }
                    UpdateRootTypes(rv);
                    return rv;
                }
                else if (tpa->p->type == kw_template)
                {
                    TYPE* type = alt ? tpa->p->byTemplate.temp->tp : tpa->p->byTemplate.val->tp;
                    if (type)
                    {
                        TYPE* tx = qual;
                        *last = (TYPE*)Alloc(sizeof(TYPE));
                        **last = *type;
                        (*last)->templateTop = true;
                        while (tx)
                        {
                            if (tx->type == bt_const)
                                (*last)->templateConst = true;
                            if (tx->type == bt_volatile)
                                (*last)->templateVol = true;
                            tx = tx->btp;
                        }
                        tp = (TYPE*)Alloc(sizeof(TYPE));
                        tp->type = bt_derivedfromtemplate;
                        tp->rootType = tp;
                        tp->btp = rv;
                        tp->size = rv->size;
                        rv = tp;
                        SynthesizeQuals(&last, &qual, &lastQual);
                    }
                    UpdateRootTypes(rv);
                    return rv;
                }
                else
                {
                    return &stdany;
                }
            }
            default:
                if (alt && isstructured(tp))
                {
                    TEMPLATEPARAMLIST *p = nullptr, **pt = &p, *tpl;
                    tp_in = (TYPE*)Alloc(sizeof(TYPE));
                    *tp_in = *tp;
                    tp_in->sp = clonesym(tp_in->sp);
                    tpl = tp_in->sp->templateParams;
                    while (tpl)
                    {
                        *pt = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                        if (tpl->p->type == kw_typename && tpl->p->byClass.temp)
                        {
                            (*pt)->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                            *(*pt)->p = *tpl->p;
                            (*pt)->p->byClass.val = SynthesizeType(tpl->p->byClass.temp, enclosing, false);
                        }
                        else
                        {
                            (*pt)->p = tpl->p;
                        }
                        pt = &(*pt)->next;
                        tpl = tpl->next;
                    }
                    tp_in->sp->templateParams = p;
                    tp = tp_in;
                }
                else if (enclosing)
                {
                    tp_in = SynthesizeStructure(tp, /*basetype(tp)->sp ? basetype(tp)->sp->templateParams :*/ enclosing);
                    if (tp_in)
                    {
                        tp = (TYPE*)Alloc(sizeof(TYPE));
                        tp->type = bt_derivedfromtemplate;
                        tp->rootType = tp;
                        tp->btp = rv;
                        tp->rootType = tp;
                        tp->size = rv->size;
                        rv = tp;
                        tp = tp_in;
                    }
                }
                *last = tp;
                SynthesizeQuals(&last, &qual, &lastQual);
                UpdateRootTypes(rv);
                return rv;
        }
    }
}
static bool hasPack(TYPE* tp)
{
    bool rv = false;
    while (ispointer(tp))
        tp = tp->btp;
    if (isfunction(tp))
    {
        SYMLIST* hr = tp->syms->table[0];
        while (hr && !rv)
        {
            SYMBOL* sym = hr->p;
            if (sym->packed)
            {
                rv = true;
            }
            else if (isfunction(sym->tp) || isfuncptr(sym->tp))
            {
                rv = hasPack(sym->tp);
            }
            hr = hr->next;
        }
    }
    return rv;
}
static SYMBOL* SynthesizeParentClass(SYMBOL* sym)
{
    SYMBOL* rv = sym;
    SYMBOL* syms[500];
    int count = 0;
    if (templateNestingCount)
        return sym;
    while (sym)
    {
        syms[count++] = sym;
        sym = sym->sb->parentClass;
    }
    if (count)
    {
        int i;
        for (i = count - 1; i >= 0; i--)
        {
            if (syms[i]->sb->templateLevel && !syms[i]->sb->instantiated)
            {
                break;
            }
        }
        if (i >= 0)
        {
            SYMBOL* last = nullptr;
            rv = nullptr;

            // has templated classes
            for (i = count - 1; i >= 0; i--)
            {
                if (syms[i]->sb->templateLevel)
                {
                    SYMBOL* found = TemplateClassInstantiateInternal(syms[i], syms[i]->templateParams, true);
                    if (!found)
                    {
                        diag("SynthesizeParentClass mismatch 1");
                        return sym;
                    }
                    found = clonesym(found);
                    found->templateParams = copyParams(found->templateParams, true);
                    found->sb->parentClass = last;
                    last = found;
                }
                else
                {
                    if (last)
                    {
                        SYMBOL* found = search(syms[i]->name, last->tp->syms);
                        if (!found || !isstructured(found->tp))
                        {
                            diag("SynthesizeParentClass mismatch 2");
                            return sym;
                        }
                        found->sb->parentClass = last;
                        last = found;
                    }
                    else
                    {
                        last = syms[i];
                    }
                }
                rv = last;
            }
        }
    }
    return rv;
}
SYMBOL* SynthesizeResult(SYMBOL* sym, TEMPLATEPARAMLIST* params)
{
    SYMBOL* rsv = clonesym(sym);
    STRUCTSYM s, s1;

    if (sym->sb->parentClass)
    {
        s.str = sym->sb->parentClass;
        addStructureDeclaration(&s);
    }
    s1.tmpl = sym->templateParams;
    addTemplateDeclaration(&s1);
    rsv->sb->parentTemplate = sym;
    rsv->sb->mainsym = sym;
    rsv->sb->parentClass = SynthesizeParentClass(rsv->sb->parentClass);
    rsv->tp = SynthesizeType(sym->tp, params, false);
    if (isfunction(rsv->tp))
    {
        basetype(rsv->tp)->btp = PerformDeferredInitialization(basetype(rsv->tp)->btp, nullptr);
    }
    if (isfunction(rsv->tp))
    {
        basetype(rsv->tp)->sp = rsv;
    }
    rsv->templateParams = params;
    if (sym->sb->parentClass)
    {
        dropStructureDeclaration();
    }
    dropStructureDeclaration();
    return rsv;
}
static TYPE* removeTLQuals(TYPE* A) { return basetype(A); }
static TYPE* rewriteNonRef(TYPE* A)
{
    if (isarray(A))
    {
        TYPE* x = (TYPE*)Alloc(sizeof(TYPE));
        x->type = bt_pointer;
        x->size = getSize(bt_pointer);
        while (isarray(A))
            A = basetype(A)->btp;
        x->btp = A;
        x->rootType = x;
        A = x;
    }
    else if (isfunction(A))
    {
        TYPE* x = (TYPE*)Alloc(sizeof(TYPE));
        x->type = bt_pointer;
        x->size = getSize(bt_pointer);
        A = basetype(A);
        x->btp = A;
        x->rootType = x;
        return x;
    }
    return removeTLQuals(A);
}
static bool hastemplate(EXPRESSION* exp)
{
    if (!exp)
        return false;
    if (exp->type == en_templateparam || exp->type == en_templateselector)
        return true;
    return hastemplate(exp->left) || hastemplate(exp->right);
}
static void clearoutDeduction(TYPE* tp)
{
    while (1)
    {
        switch (tp->type)
        {
            case bt_pointer:
                if (isarray(tp) && tp->etype)
                {
                    clearoutDeduction(tp->etype);
                }
                tp = tp->btp;
                break;
            case bt_templateselector:
                clearoutDeduction(tp->sp->sb->templateSelector->next->sp->tp);
                return;
            case bt_const:
            case bt_volatile:
            case bt_lref:
            case bt_rref:
            case bt_restrict:
            case bt_far:
            case bt_near:
            case bt_seg:
            case bt_lrqual:
            case bt_rrqual:
            case bt_derivedfromtemplate:
                tp = tp->btp;
                break;
            case bt_memberptr:
                clearoutDeduction(tp->sp->tp);
                tp = tp->btp;
                break;
            case bt_func:
            case bt_ifunc:
            {
                SYMLIST* hr = tp->syms->table[0];
                while (hr)
                {
                    clearoutDeduction(hr->p->tp);
                    hr = hr->next;
                }
                tp = tp->btp;
                break;
            }
            case bt_templateparam:
                tp->templateParam->p->byClass.temp = nullptr;
                return;
            default:
                return;
        }
    }
}
static void ClearArgValues(TEMPLATEPARAMLIST* params, bool specialized)
{
    while (params)
    {
        if (params->p->type != kw_new)
        {
            if (params->p->packed)
                params->p->byPack.pack = nullptr;
            else
                params->p->byClass.val = params->p->byClass.temp = nullptr;
            if (params->p->byClass.txtdflt && !specialized)
                params->p->byClass.dflt = nullptr;
            if (params->p->byClass.dflt)
            {
                if (params->p->type == kw_typename)
                {
                    TYPE* tp = params->p->byClass.dflt;
                    while (ispointer(tp))
                        tp = basetype(tp)->btp;
                    tp = basetype(tp);
                    if (tp->type == bt_templateparam)
                    {
                        TEMPLATEPARAMLIST* t = tp->templateParam;
                        t->p->byClass.val = nullptr;
                    }
                    else if (isstructured(tp) && (tp->sp)->sb->attribs.inheritable.linkage != lk_virtual)
                    {
                        ClearArgValues(tp->sp->templateParams, specialized);
                    }
                    else if (tp->type == bt_templateselector)
                    {
                        ClearArgValues(tp->sp->sb->templateSelector->next->templateParams, specialized);
                    }
                }
                else
                {
                    params->p->byClass.val = nullptr;
                }
            }
        }
        params = params->next;
    }
}
static void PushPopValues(TEMPLATEPARAMLIST* params, bool push)
{
    while (params)
    {
        if (params->p->type != kw_new)
        {
            Optimizer::LIST* lst;
            if (push)
            {
                lst = (Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
                lst->next = params->p->stack;
                params->p->stack = lst;
            }
            else
            {
                lst = (Optimizer::LIST*)params->p->stack->data;
                params->p->stack = params->p->stack->next;
            }
            if (params->p->packed)
            {
                if (push)
                {
                    lst->data = params->p->byPack.pack;
                    PushPopValues(params->p->byPack.pack, push);
                }
                else
                {
                    params->p->byPack.pack = (TEMPLATEPARAMLIST*)lst;
                }
            }
            else
            {
                if (push)
                {
                    lst->data = params->p->byClass.val;
                    if (params->p->type == kw_typename)
                    {
                        TYPE* tp = params->p->byClass.val;
                        if (tp)
                        {
                            while (ispointer(tp) || isref(tp))
                                tp = basetype(tp)->btp;
                            if (isstructured(tp))
                            {
                                PushPopValues(basetype(tp)->sp->templateParams, push);
                            }
                            else if (basetype(tp)->type == bt_templateselector)
                            {
                                PushPopValues(basetype(tp)->sp->sb->templateSelector->next->templateParams, push);
                            }
                        }
                    }
                }
                else
                {
                    params->p->byClass.val = (TYPE*)lst;
                }
            }
        }
        params = params->next;
    }
}
static bool Deduce(TYPE* P, TYPE* A, bool change, bool byClass, bool allowSelectors);
static bool DeduceFromTemplates(TYPE* P, TYPE* A, bool change, bool byClass)
{
    TYPE* pP = basetype(P);
    TYPE* pA = basetype(A);
    if (pP->sp && pA->sp && pP->sp->sb && pA->sp->sb && pP->sp->sb->parentTemplate == pA->sp->sb->parentTemplate)
    {
        TEMPLATEPARAMLIST* TP = pP->sp->templateParams;
        TEMPLATEPARAMLIST* TA = pA->sp->templateParams;
        TEMPLATEPARAMLIST* TAo = TA;
        TEMPLATEPARAMLIST* isspecialized = TP->p->bySpecialization.types ? TP->next : nullptr;
        if (!TA || !TP)
            return false;
        if (TA->p->bySpecialization.types)
            TA = TA->p->bySpecialization.types;
        else
            TA = TA->next;
        if (TP->p->bySpecialization.types)
            TP = TP->p->bySpecialization.types;
        else
            TP = TP->next;
        std::stack<TEMPLATEPARAMLIST*> tas;
        while (TP && TA)
        {
            TEMPLATEPARAMLIST* to = TP;
            if (TA->p->packed)
            {
                if (TA->p->byPack.pack == nullptr)
                {
                    if (TA->p->type == TP->p->type)
                    {
                        if (TP->p->packed)
                        {
                            if (TP->p->byPack.pack == nullptr)
                            {
                                TA = TA->next;
                                TP = TP->next;
                                continue;
                            }
                        }
                        else
                        {
                            if (!TP->p->byClass.val)
                            {
                                TA = TA->next;
                                TP = TP->next;
                                continue;
                            }
                        }
                    }
                }
                tas.push(TA->next);
                TA = TA->p->byPack.pack;
            }
            if (!TA)
            {
                if (tas.size())
                {
                    TA = tas.top();
                    tas.pop();
                    continue;
                }
                break;
            }
            if (TP->p->type != TA->p->type)
                return false;
            if (TP->p->packed)
            {
                break;
            }
            if (isspecialized && TP->argsym)
            {
                TEMPLATEPARAMLIST* search = isspecialized;
                while (search)
                {
                    if (search->argsym && !strcmp(search->argsym->name, TP->argsym->name))
                        break;
                    search = search->next;
                }
                if (search)
                    to = search;
            }
            switch (TP->p->type)
            {
                case kw_typename:
                {
                    TYPE** tp = change ? &to->p->byClass.val : &to->p->byClass.temp;
                    if (*tp)
                    {
                        if (!templatecomparetypes(*tp, TA->p->byClass.val, true))
                            return false;
                    }
                    else
                        *tp = TA->p->byClass.val;
                    if (to->p->byClass.dflt && to->p->byClass.val &&
                        !Deduce(to->p->byClass.dflt, to->p->byClass.val, change, byClass, false))
                        return false;
                    break;
                }
                case kw_template:
                {
                    TEMPLATEPARAMLIST* paramT = to->argsym->templateParams;
                    TEMPLATEPARAMLIST* paramA = TA->argsym->templateParams;
                    while (paramT && paramA)
                    {
                        if (paramT->p->type != paramA->p->type)
                            return false;

                        paramT = paramT->next;
                        paramA = paramA->next;
                    }
                    if (paramT || paramA)
                        return false;
                    if (!to->p->byTemplate.val)
                        to->p->byTemplate.val = TA->p->byTemplate.val;
                    else if (!DeduceFromTemplates(to->p->byTemplate.val->tp, TA->p->byTemplate.val->tp, change, byClass))
                        return false;
                    break;
                }
                case kw_int:
                {
                    EXPRESSION** exp;
                    if (TAo->p->bySpecialization.types)
                    {
                        //    #ifndef PARSER_ONLY
                        //                        if (TA->p->byNonType.dflt && !equalTemplateIntNode(TA->p->byNonType.dflt,
                        //                        TA->p->byNonType.val))
                        //                            return false;
                        //    #endif
                    }
                    exp = change ? &to->p->byNonType.val : &to->p->byNonType.temp;
                    if (!*exp)
                        *exp = TA->p->byNonType.val;
                    break;
                }
                default:
                    break;
            }
            TP = TP->next;
            TA = TA->next;
        }
        std::stack<TEMPLATEPARAMLIST*> tps;
        while (!tas.empty())
            tas.pop();
        if (TP && TP->p->packed)
        {
            if (isspecialized && TP->argsym)
            {
                TEMPLATEPARAMLIST* search = isspecialized;
                while (search)
                {
                    if (search->argsym && !strcmp(search->argsym->name, TP->argsym->name))
                        break;
                    search = search->next;
                }
                if (search)
                    TP = search;
            }
            if (TP->p->byPack.pack)
            {
                tps.push(TP->next);
                TP = TP->p->byPack.pack;
                while (TP && TA)
                {
                    if (TP->p->type != TA->p->type)
                    {
                        return false;
                    }
                    if (TA->p->packed)
                    {
                        tas.push(TA->next);
                        TA = TA->p->byPack.pack;
                    }
                    if (TA)
                    {
                        switch (TP->p->type)
                        {
                            case kw_typename:
                            {
                                TYPE** tp = change ? &TP->p->byClass.val : &TP->p->byClass.temp;
                                if (*tp)
                                {
                                    if (!templatecomparetypes(*tp, TA->p->byClass.val, true))
                                    {
                                        return false;
                                    }
                                }
                                else
                                {
                                    *tp = TA->p->byClass.val;
                                }
                                break;
                            }
                            case kw_template:
                            {
                                TEMPLATEPARAMLIST* paramT = TP->argsym->templateParams;
                                TEMPLATEPARAMLIST* paramA = TA->argsym->templateParams;
                                while (paramT && paramA)
                                {
                                    if (paramT->p->type != paramA->p->type)
                                    {
                                        return false;
                                    }

                                    paramT = paramT->next;
                                    paramA = paramA->next;
                                }
                                if (paramT || paramA)
                                {
                                    return false;
                                }
                                if (!DeduceFromTemplates(TP->p->byTemplate.val->tp, TA->p->byTemplate.val->tp, change, byClass))
                                {
                                    return false;
                                }
                                break;
                            }
                            case kw_int:
                            {
                                if (TAo->p->bySpecialization.types)
                                {
                                    //                #ifndef PARSER_ONLY
                                    //                                    if (TA->p->byNonType.dflt &&
                                    //                                    !equalTemplateIntNode(TA->p->byNonType.dflt,
                                    //                                    TA->p->byNonType.val))
                                    //                                        return false;
                                    //                #endif
                                }
                                break;
                            }
                            default:
                                break;
                        }
                        TP = TP->next;
                        TA = TA->next;
                        if (!TP && !TA && tps.size() && tas.size())
                        {
                            TP = tps.top();
                            tps.pop();
                            TA = tas.top();
                            tas.pop();
                        }
                    }
                }
            }
            else
            {
                TEMPLATEPARAMLIST** newList = &TP->p->byPack.pack;
                while (!tas.empty())
                    tas.pop();
                while (TA)
                {
                    if (TP->p->type != TA->p->type)
                    {
                        return false;
                    }
                    if (TA->p->packed)
                    {
                        tas.push(TA->next);
                        TA = TA->p->byPack.pack;
                    }
                    if (TA)
                    {
                        *newList = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                        (*newList)->p = TA->p;
                        newList = &(*newList)->next;
                        TA = TA->next;
                    }
                    if (!TA && tas.size())
                    {
                        TA = tas.top();
                        tas.pop();
                    }
                }
                TP = nullptr;
            }
        }
        return (!TP && !TA);
    }
    return false;
}
static bool DeduceFromBaseTemplates(TYPE* P, SYMBOL* A, bool change, bool byClass)
{
    if (A->sb)
    {
        BASECLASS* lst = A->sb->baseClasses;
        while (lst)
        {
            if (DeduceFromBaseTemplates(P, lst->cls, change, byClass))
                return true;
            if (DeduceFromTemplates(P, lst->cls->tp, change, byClass))
                return true;
            lst = lst->next;
        }
    }
    return false;
}
static bool DeduceFromMemberPointer(TYPE* P, TYPE* A, bool change, bool byClass)
{
    TYPE* Pb = basetype(P);
    TYPE* Ab = basetype(A);
    if (Ab->type == bt_memberptr)
    {
        if (Pb->type != bt_memberptr || !Deduce(Pb->sp->tp, Ab->sp->tp, change, byClass, false))
            return false;
        if (!Deduce(Pb->btp, Ab->btp, change, byClass, false))
            return false;
        return true;
    }
    else  // should only get here for functions
    {
        if (ispointer(Ab))
            Ab = basetype(Ab)->btp;
        if (!isfunction(Ab))
            return false;
        if (basetype(Ab)->sp->sb->parentClass == nullptr || !ismember(basetype(Ab)->sp) || Pb->type != bt_memberptr ||
            !Deduce(Pb->sp->tp, basetype(Ab)->sp->sb->parentClass->tp, change, byClass, false))
            return false;
        if (!Deduce(Pb->btp, Ab, change, byClass, false))
            return false;
        return true;
    }
}
static TYPE* FixConsts(TYPE* P, TYPE* A)
{
    int pn = 0, an = 0;
    TYPE* Pb = P;
    TYPE *q = P, **last = &q;
    int i;
    while (ispointer(q))
    {
        q = basetype(q)->btp;
        pn++;
    }
    q = A;
    while (ispointer(q))
    {
        q = basetype(q)->btp;
        an++;
    }
    *last = nullptr;
    if (pn > an)
    {
        for (i = 0; i < pn - an; i++)
            P = basetype(P)->btp;
    }
    while (P && A)
    {
        bool constant = false;
        bool vol = false;
        if (isconst(P) && !isconst(A))
            constant = true;
        if (isvolatile(P) && !isvolatile(A))
            vol = true;
        while (isconst(P) || isvolatile(P))
        {
            if ((constant && isconst(P)) || (vol && isvolatile(P)))
            {
                *last = (TYPE*)Alloc(sizeof(TYPE));
                **last = *P;
                last = &(*last)->btp;
                *last = nullptr;
            }
            P = P->btp;
        }
        while (A != basetype(A))
        {
            if (A->type == bt_const && !isconst(Pb))
            {
                *last = (TYPE*)Alloc(sizeof(TYPE));
                **last = *A;
                last = &(*last)->btp;
                *last = nullptr;
            }
            else if (A->type == bt_volatile && !isvolatile(Pb))
            {
                *last = (TYPE*)Alloc(sizeof(TYPE));
                **last = *A;
                last = &(*last)->btp;
                *last = nullptr;
            }
            A = A->btp;
        }
        A = basetype(A);
        *last = (TYPE*)Alloc(sizeof(TYPE));
        **last = *A;
        last = &(*last)->btp;
        *last = nullptr;
        A = A->btp;
        P = P->btp;
    }
    (*last) = A;
    UpdateRootTypes(q);
    return q;
}
static bool DeduceTemplateParam(TEMPLATEPARAMLIST* Pt, TYPE* P, TYPE* A, bool change)
{
    if (Pt->p->type == kw_typename)
    {
        TYPE** tp = change ? &Pt->p->byClass.val : &Pt->p->byClass.temp;
        if (*tp)
        {
            if (/*!Pt->p->sb->initialized &&*/ !templatecomparetypes(*tp, A, true))
                return false;
        }
        else
        {
            if (P)
            {
                TYPE* q = A;
                while (q)
                {
                    if (isconst(q))
                    {
                        *tp = FixConsts(P, A);
                        return true;
                    }
                    q = basetype(q)->btp;
                }
            }
            *tp = A;
        }
        return true;
    }
    else if (Pt->p->type == kw_template && isstructured(A) && basetype(A)->sp->sb->templateLevel)
    {
        TEMPLATEPARAMLIST* primary = Pt->p->byTemplate.args;
        TEMPLATEPARAMLIST* match = basetype(A)->sp->templateParams->next;
        while (primary && match)
        {
            if (primary->p->type != match->p->type)
                return false;
            if (primary->p->packed)
            {
                primary->p->byPack.pack = match;
                match = nullptr;
                primary = primary->next;
                break;
            }
            else if (!DeduceTemplateParam(primary, primary->p->byClass.val, match->p->byClass.val, change))
                return false;
            primary = primary->next;
            match = match->next;
        }
        if (!primary && !match)
        {
            SYMBOL** sp = change ? &Pt->p->byTemplate.val : &Pt->p->byTemplate.temp;
            *sp = basetype(A)->sp;
            sp = change ? &Pt->p->byTemplate.orig->p->byTemplate.val : &Pt->p->byTemplate.orig->p->byTemplate.temp;
            *sp = basetype(A)->sp;
            return true;
        }
    }
    return false;
}
static bool Deduce(TYPE* P, TYPE* A, bool change, bool byClass, bool allowSelectors)
{
    TYPE *Pin = P, *Ain = A;
    if (!P || !A)
        return false;
    while (1)
    {
        TYPE* Ab = basetype(A);
        TYPE* Pb = basetype(P);
        if (isstructured(Pb) && Pb->sp->sb->templateLevel && Pb->sp->sb->attribs.inheritable.linkage != lk_virtual &&
            isstructured(Ab))
        {
            if (DeduceFromTemplates(P, A, change, byClass))
                return true;
            else
                return DeduceFromBaseTemplates(P, basetype(A)->sp, change, byClass);
        }
        if (Pb->type == bt_memberptr)
            return DeduceFromMemberPointer(P, A, change, byClass);
        if (Pb->type == bt_enum)
        {
            if (Ab->type == bt_enum && Ab->sp == Pb->sp)
                return true;
            if (isint(Ab))  // && Ab->enumConst)
                return true;
            return false;
        }
        if (Ab->type != Pb->type && (!isfunction(Ab) || !isfunction(Pb)) && Pb->type != bt_templateparam &&
            (!allowSelectors || Pb->type != bt_templateselector))
            return false;
        switch (Pb->type)
        {
            case bt_pointer:
                if (isarray(Pb))
                {
                    if (!!basetype(Pb)->esize != !!basetype(Ab)->esize)
                        return false;
                    if (basetype(Pb)->esize && basetype(Pb)->esize->type == en_templateparam)
                    {
                        SYMBOL* sym = basetype(Pb)->esize->v.sp;
                        if (sym->tp->type == bt_templateparam)
                        {
                            sym->tp->templateParam->p->byNonType.val = basetype(Ab)->esize;
                        }
                    }
                }
                if (isarray(Pb) != isarray(Ab))
                    return false;
                P = Pb->btp;
                A = Ab->btp;
                break;
            case bt_templateselector:
                return true;
            case bt_templatedecltype:
                return false;
            case bt_lref:
            case bt_rref:
            case bt_restrict:
            case bt_far:
            case bt_near:
            case bt_seg:
                P = Pb->btp;
                A = Ab->btp;
                break;
            case bt_func:
            case bt_ifunc:
            {
                SYMLIST* hrp = Pb->syms->table[0];
                SYMLIST* hra = Ab->syms->table[0];
                if (islrqual(Pin) != islrqual(A) || isrrqual(Pin) != isrrqual(Ain))
                    return false;
                if (isconst(Pin) != isconst(Ain) || isvolatile(Pin) != isvolatile(Ain))
                    return false;
                if ((hrp->p)->sb->thisPtr)
                    hrp = hrp->next;
                if ((hra->p)->sb->thisPtr)
                    hra = hra->next;
                clearoutDeduction(P);
                if (Pb->btp->type != bt_auto && !Deduce(Pb->btp, Ab->btp, change, byClass, allowSelectors))
                    return false;
                while (hra && hrp)
                {
                    SYMBOL* sp = (SYMBOL*)hrp->p;
                    if (!Deduce(sp->tp, (hra->p)->tp, change, byClass, allowSelectors))
                        return false;
                    if (sp->tp->type == bt_templateparam)
                    {
                        if (sp->tp->templateParam->p->packed)
                        {
                            SYMBOL *sra, *srp;
                            while (hrp->next)
                                hrp = hrp->next;
                            while (hra->next)
                                hra = hra->next;

                            sra = (SYMBOL*)hra->p;
                            srp = (SYMBOL*)hrp->p;
                            if ((sra->tp->type != bt_ellipse && srp->tp->type != bt_ellipse) || sra->tp->type == srp->tp->type)
                            {
                                hrp = nullptr;
                                hra = nullptr;
                            }
                            break;
                        }
                    }
                    hrp = hrp->next;
                    hra = hra->next;
                }
                if (hra)
                    return false;
                if (hrp && !(hrp->p)->sb->init)
                    return false;
                return true;
            }
            case bt_templateparam:
                return DeduceTemplateParam(Pb->templateParam, P, A, change);
            case bt_struct:
            case bt_union:
            case bt_class:
                return templatecomparetypes(Pb, Ab, true);
            default:

                return true;
        }
    }
}
static int eval(EXPRESSION* exp)
{
    optimize_for_constants(&exp);
    if (IsConstantExpression(exp, false, false))
        return exp->v.i;
    return 0;
}
static bool ValidExp(EXPRESSION** exp_in)
{
    bool rv = true;
    EXPRESSION* exp = *exp_in;
    if (exp->type == en_templateselector)
        return false;
    if (exp->left)
        rv &= ValidExp(&exp->left);
    if (exp->right)
        rv &= ValidExp(&exp->right);
    if (exp->type == en_templateparam)
        if (!exp->v.sp->templateParams || !exp->v.sp->templateParams->p->byClass.val)
            return false;
    return rv;
}
static bool ValidArg(TYPE* tp)
{
    while (1)
    {
        switch (tp->type)
        {
            case bt_pointer:
                if (isarray(tp))
                {
                    while (isarray(tp))
                    {
                        tp = basetype(tp)->btp;
                        if (tp->etype)
                        {
                            int n = eval(tp->esize);
                            if (n <= 0)
                                return false;
                        }
                    }
                    if (tp->type == bt_templateparam)
                    {
                        if (tp->templateParam->p->type != kw_typename)
                            return false;
                        tp = tp->templateParam->p->byClass.val;
                        if (!tp)
                            return false;
                    }
                    if (tp->type == bt_void || isfunction(tp) || isref(tp) ||
                        (isstructured(tp) && basetype(tp)->sp->sb->isabstract))
                        return false;
                }
                if (ispointer(tp))
                {
                    while (ispointer(tp))
                        tp = tp->btp;
                    if (tp->type == bt_templateparam)
                    {
                        if (tp->templateParam->p->type != kw_typename)
                            return false;
                        return ValidArg(tp);
                    }
                    else if (tp->type == bt_templateselector)
                    {
                        return ValidArg(tp);
                    }
                    if (isref(tp))
                        return false;
                }
                return true;
            case bt_templatedecltype:
                tp = TemplateLookupTypeFromDeclType(tp);
                return !!tp;
                break;
            case bt_templateselector:
            {
                SYMBOL* ts = tp->sp->sb->templateSelector->next->sp;
                SYMBOL* sp = nullptr;
                TEMPLATESELECTOR* find = tp->sp->sb->templateSelector->next->next;
                if (tp->sp->sb->templateSelector->next->isTemplate)
                {
                    TEMPLATEPARAMLIST* current = tp->sp->sb->templateSelector->next->templateParams;
                    sp = GetClassTemplate(ts, current, false);
                    tp = nullptr;
                }
                else if (basetype(ts->tp)->templateParam->p->type == kw_typename)
                {
                    tp = basetype(ts->tp)->templateParam->p->byClass.val;
                    if (!tp)
                        return false;
                    sp = tp->sp;
                }
                else if (basetype(ts->tp)->templateParam->p->type == kw_delete)
                {
                    TEMPLATEPARAMLIST* args = basetype(ts->tp)->templateParam->p->byDeferred.args;
                    TEMPLATEPARAMLIST* val = nullptr;
                    sp = tp->templateParam->argsym;
                    sp = TemplateClassInstantiateInternal(sp, args, true);
                }
                if (sp)
                {
                    sp->tp = basetype(PerformDeferredInitialization(sp->tp, nullptr));
                    sp = sp->tp->sp;
                    while (find && sp)
                    {
                        SYMBOL* spo = sp;
                        if (!isstructured(spo->tp))
                            break;

                        sp = search(find->name, spo->tp->syms);
                        if (!sp)
                        {
                            sp = classdata(find->name, spo, nullptr, false, false);
                            if (sp == (SYMBOL*)-1)
                                sp = nullptr;
                        }
                        find = find->next;
                    }
                    return !find && sp && istype(sp);
                }
                return false;
            }
            case bt_lref:
            case bt_rref:
                tp = basetype(tp)->btp;
                if (tp->type == bt_templateparam)
                {
                    if (tp->templateParam->p->type != kw_typename)
                        return false;
                    return ValidArg(tp);
                }
                if (!tp || isref(tp))
                    return false;
                break;
            case bt_memberptr:
            {
                TYPE* tp1 = tp->sp->tp;
                if (tp1->type == bt_templateparam)
                {
                    if (tp1->templateParam->p->type != kw_typename)
                        return false;
                    tp1 = tp1->templateParam->p->byClass.val;
                    if (!tp1)
                        return false;
                }
                if (!isstructured(tp1))
                    return false;
            }
                tp = tp->btp;
                break;
            case bt_const:
            case bt_volatile:
            case bt_restrict:
            case bt_far:
            case bt_near:
            case bt_seg:
            case bt_lrqual:
            case bt_rrqual:
            case bt_derivedfromtemplate:
                tp = tp->btp;
                break;
            case bt_func:
            case bt_ifunc:
            {
                SYMLIST* hr = tp->syms->table[0];
                while (hr)
                {
                    if (!ValidArg(hr->p->tp))
                        return false;
                    hr = hr->next;
                }
                tp = tp->btp;
                if (tp->type == bt_templateparam)
                {
                    if (tp->templateParam->p->type != kw_typename)
                        return false;
                    tp = tp->templateParam->p->byClass.val;
                    if (!tp)
                        return false;
                }
                if (isfunction(tp) || isarray(tp) || (isstructured(tp) && tp->sp->sb->isabstract))
                    return false;
                break;
            }
            case bt_templateparam:
                if (tp->templateParam->p->type == kw_template)
                {
                    TEMPLATEPARAMLIST* tpl;
                    if (tp->templateParam->p->packed)
                        return true;
                    if (tp->templateParam->p->byTemplate.val == nullptr)
                        return false;
                    tpl = tp->templateParam->p->byTemplate.args;
                    while (tpl)
                    {
                        if (tpl->p->type == kw_typename)
                        {
                            if (tpl->p->packed)
                            {
                                // this should be recursive...
                                TEMPLATEPARAMLIST* tpl1 = tpl->p->byPack.pack;
                                while (tpl1)
                                {
                                    if (tpl1->p->type == kw_typename && !tpl1->p->packed)
                                    {
                                        if (!ValidArg(tpl1->p->byClass.val))
                                            return false;
                                    }
                                    tpl1 = tpl1->next;
                                }
                            }
                            else if (!ValidArg(tpl->p->byClass.val))
                                return false;
                        }
                        // this really should check nested templates...
                        tpl = tpl->next;
                    }
                }
                else
                {
                    if (tp->templateParam->p->type != kw_typename)
                        return false;
                    if (tp->templateParam->p->packed)
                        return true;
                    if (tp->templateParam->p->byClass.val == nullptr)
                        return false;
                    if (tp->templateParam->p->byClass.val->type == bt_void)
                        return false;
                    if (tp->templateParam->p->byClass.val == tp)  // error catcher
                        return false;
                    return ValidArg(tp->templateParam->p->byClass.val);
                }
            default:
                return true;
        }
    }
}
static bool valFromDefault(TEMPLATEPARAMLIST* params, bool usesParams, INITLIST** args)
{
    while (params && (!usesParams || *args))
    {
        if (params->p->packed)
        {
            if (!valFromDefault(params->p->byPack.pack, usesParams, args))
                return false;
        }
        else
        {
            if (!params->p->byClass.val)
                params->p->byClass.val = params->p->byClass.dflt;
            if (!params->p->byClass.val)
                return false;
            if (*args)
                *args = (*args)->next;
        }
        params = params->next;
    }
    return true;
}
static void FillNontypeTemplateParamDefaults(TEMPLATEPARAMLIST* fill, TEMPLATEPARAMLIST* enclosing);
static void FillNontypeExpressionDefaults(EXPRESSION* exp, TEMPLATEPARAMLIST* enclosing)
{
    if (exp->left)
        FillNontypeExpressionDefaults(exp->left, enclosing);
    if (exp->right)
        FillNontypeExpressionDefaults(exp->right, enclosing);
    if (exp->v.templateParam)
    {
        FillNontypeTemplateParamDefaults(exp->v.templateParam, enclosing);
    }
    if (exp->v.templateSelector)
    {
        FillNontypeTemplateParamDefaults(exp->v.templateSelector->next->templateParams, enclosing);
    }
}
static void FillNontypeTypeDefaults(TYPE* tp, TEMPLATEPARAMLIST* enclosing)
{
    TEMPLATEPARAMLIST* fill = tp->templateParam;
    if (tp->type == bt_templateselector)
        fill = tp->sp->sb->templateSelector->next->templateParams;

    FillNontypeTemplateParamDefaults(fill, enclosing);
}
static bool SetTemplateParamValue(TEMPLATEPARAMLIST* p, TEMPLATEPARAMLIST* enclosing)
{
    if (p->argsym)
    {
        while (enclosing)
        {
            if (enclosing->argsym)
                if (!strcmp(p->argsym->name, enclosing->argsym->name))
                {
                    if (p->p->packed)
                    {
                        if (enclosing->p->packed)
                        {
                            p->p->byPack.pack = enclosing->p->byPack.pack;
                        }
                        else
                        {
                            p->p->byPack.pack = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                            p->p->byPack.pack->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                            *p->p->byPack.pack->p = *enclosing->p;
                            p->p->byPack.pack->next = nullptr;
                        }
                    }
                    else
                    {
                        if (enclosing->p->byClass.val)
                            p->p->byClass.val = enclosing->p->byClass.val;
                        else
                            p->p->byClass.val = enclosing->p->byClass.dflt;
                    }
                    return true;
                }
            enclosing = enclosing->next;
        }
    }
    return false;
}
static void FillNontypeTemplateParamDefaults(TEMPLATEPARAMLIST* fill, TEMPLATEPARAMLIST* enclosing)
{
    while (fill)
    {
        if (fill->p->packed)
        {
            if (!SetTemplateParamValue(fill, enclosing))
                FillNontypeTemplateParamDefaults(fill->p->byPack.pack, enclosing);
        }
        else if (fill->p->type == kw_int)
        {
            if (fill->p->byNonType.dflt)
                FillNontypeExpressionDefaults(fill->p->byNonType.dflt, enclosing);
            else
                SetTemplateParamValue(fill, enclosing);
        }
        else if (fill->p->type == kw_typename)
        {
            if (!fill->p->byClass.dflt)
            {
                SetTemplateParamValue(fill, enclosing);
            }
            else
            {
                switch (fill->p->byClass.dflt->type)
                {
                    case bt_class:
                        FillNontypeTemplateParamDefaults(fill->p->byClass.dflt->sp->templateParams, enclosing);
                        break;
                    case bt_templateparam:
                        SetTemplateParamValue(fill->p->byClass.dflt->templateParam, enclosing);
                        break;
                    case bt_templateselector:
                        FillNontypeTemplateParamDefaults(fill->p->byClass.dflt->sp->sb->templateSelector->next->templateParams,
                                                         enclosing);
                        break;
                }
            }
        }
        fill = fill->next;
    }
}
static bool checkNonTypeTypes(TEMPLATEPARAMLIST* params, TEMPLATEPARAMLIST* enclosing)
{
    while (params)
    {
        if (params->p->type == kw_int &&
            (params->p->byNonType.tp->type == bt_templateselector || params->p->byNonType.tp->type == bt_templateparam))
        {
            FillNontypeTypeDefaults(params->p->byNonType.tp, enclosing);
            TYPE* tp1 = SynthesizeType(params->p->byNonType.tp, enclosing, false);
            if (!tp1 || tp1->type == bt_any || isstructured(tp1) || isref(tp1) || isfloat(tp1) || iscomplex(tp1) ||
                isimaginary(tp1))
                return false;
        }
        params = params->next;
    }
    return true;
}
static bool ValidateArgsSpecified(TEMPLATEPARAMLIST* params, SYMBOL* func, INITLIST* args)
{
    bool usesParams = !!args;
    INITLIST* check = args;
    SYMLIST* hr = basetype(func->tp)->syms->table[0];
    STRUCTSYM s, s1;
    inDefaultParam++;
    if (!valFromDefault(params, usesParams, &args))
    {
        inDefaultParam--;
        return false;
    }
    if (!checkNonTypeTypes(params, params))
    {
        inDefaultParam--;
        return false;
    }

    while (params)
    {
        if (params->p->type == kw_typename || params->p->type == kw_template || params->p->type == kw_int)
            if (!params->p->packed && !params->p->byClass.val)
            {
                inDefaultParam--;
                return false;
            }
        params = params->next;
    }
    if (hr && hr->p->tp->type == bt_void)
    {
        inDefaultParam--;
        return true;
    }
    if (hr)
    {
        bool packedOrEllipse = false;
        if (func->sb->parentClass)
        {
            s1.str = func->sb->parentClass;
            addStructureDeclaration(&s1);
        }
        s.tmpl = func->templateParams;
        addTemplateDeclaration(&s);
        args = check;
        while (args && hr)
        {
            SYMBOL* sp1 = (SYMBOL*)hr->p;
            if (basetype(sp1->tp)->type == bt_ellipse || sp1->packed)
                packedOrEllipse = true;
            args = args->next;
            hr = hr->next;
        }
        if (args && !packedOrEllipse)
        {
            dropStructureDeclaration();
            if (func->sb->parentClass)
                dropStructureDeclaration();
            inDefaultParam--;
            return false;
        }
        while (hr)
        {
            SYMBOL* sp = hr->p;
            if (sp->sb->deferredCompile)
            {
                LEXEME* lex;
                dontRegisterTemplate += templateNestingCount != 0;
                lex = SetAlternateLex(sp->sb->deferredCompile);
                sp->sb->init = nullptr;
                lex = initialize(lex, func, sp, sc_parameter, true, _F_TEMPLATEARGEXPANSION);
                SetAlternateLex(nullptr);
                dontRegisterTemplate -= templateNestingCount != 0;
                if (sp->sb->init && sp->sb->init->exp && !ValidExp(&sp->sb->init->exp))
                {
                    dropStructureDeclaration();
                    if (func->sb->parentClass)
                        dropStructureDeclaration();
                    inDefaultParam--;
                    return false;
                }
            }
            hr = hr->next;
        }
        dropStructureDeclaration();
        if (func->sb->parentClass)
            dropStructureDeclaration();
    }
    s.tmpl = func->templateParams;
    addTemplateDeclaration(&s);
    //    if (!ValidArg(basetype(func->tp)->btp))
    //        return false;
    hr = basetype(func->tp)->syms->table[0];
    while (hr)  // && (!usesParams || check))
    {
        if (!ValidArg(hr->p->tp))
        {
            dropStructureDeclaration();
            inDefaultParam--;
            return false;
        }
        if (check)
            check = check->next;
        hr = hr->next;
    }
    if (!ValidArg(basetype(func->tp)->btp))
    {
        dropStructureDeclaration();
        inDefaultParam--;
        return false;
    }
    dropStructureDeclaration();
    inDefaultParam--;
    return true;
}
static bool TemplateDeduceFromArg(TYPE* orig, TYPE* sym, EXPRESSION* exp, bool byClass, bool allowSelectors)
{
    TYPE *P = orig, *A = sym;
    if (!isref(P))
    {
        A = rewriteNonRef(A);
    }
    P = removeTLQuals(P);
    if (isref(P))
    {
        P = basetype(P)->btp;
        if (isref(A))
            A = basetype(A)->btp;
    }

    if (basetype(orig)->type == bt_rref)
    {
        if (!isconst(orig) && !isvolatile(orig) && P->type == bt_templateparam)
        {
            if (/*lvalue(exp) ||*/ (basetype(sym)->type == bt_lref && !basetype(sym)->rref))
            {
                // special case: if the deduced type for an rref is an lref it is a forwarding instance
                TYPE* x = (TYPE*)Alloc(sizeof(TYPE));
                if (isref(A))
                    A = basetype(A)->btp;
                x->type = bt_lref;
                x->size = getSize(bt_pointer);
                x->btp = A;
                x->rootType = x;
                P->templateParam->p->byClass.val = x;
                return true;
            }
        }
    }
    if (Deduce(P, A, true, byClass, allowSelectors))
        return true;
    if (isfuncptr(P) || (isref(P) && isfunction(basetype(P)->btp)))
    {
        if (exp->type == en_func)
        {
            if (exp->v.func->sp->sb->storage_class == sc_overloads)
            {
                SYMLIST* hr = basetype(exp->v.func->sp->tp)->syms->table[0];
                SYMBOL* candidate = nullptr;
                while (hr)
                {
                    SYMBOL* sym = hr->p;
                    if (sym->sb->templateLevel)
                        return false;
                    hr = hr->next;
                }
                // no templates, we can try each function one at a time
                hr = basetype(exp->v.func->sp->tp)->syms->table[0];
                while (hr)
                {
                    SYMBOL* sym = hr->p;
                    clearoutDeduction(P);
                    if (Deduce(P->btp, sym->tp, false, byClass, allowSelectors))
                    {
                        if (candidate)
                            return false;
                        else
                            candidate = sym;
                    }
                    hr = hr->next;
                }
                if (candidate)
                    return Deduce(P, candidate->tp, true, byClass, allowSelectors);
            }
        }
    }
    return false;
}
void NormalizePacked(TYPE* tpo)
{
    TYPE* tp = tpo;
    while (isref(tp) || ispointer(tp))
        tp = basetype(tp)->btp;
    if (basetype(tp)->templateParam)
        tpo->templateParam = basetype(tp)->templateParam;
}
static bool TemplateDeduceArgList(SYMLIST* funcArgs, SYMLIST* templateArgs, INITLIST* symArgs, bool allowSelectors)
{
    bool rv = true;
    while (templateArgs && symArgs)
    {
        SYMBOL* sp = (SYMBOL*)templateArgs->p;
        if (sp->packed)
        {
            NormalizePacked(sp->tp);
            if (sp->tp->templateParam && sp->tp->templateParam->p->packed)
            {
                TEMPLATEPARAMLIST* params = sp->tp->templateParam->p->byPack.pack;
                while (params && symArgs)
                {
                    if (!TemplateDeduceFromArg(params->p->byClass.val, symArgs->tp, symArgs->exp, false, allowSelectors))
                    {
                        rv = false;
                    }
                    params = params->next;
                    symArgs = symArgs->next;
                    if (funcArgs)
                        funcArgs = funcArgs->next;
                }
            }
            else
            {
                symArgs = symArgs->next;
                if (funcArgs)
                    funcArgs = funcArgs->next;
            }
        }
        else if ((symArgs->nested || (!symArgs->tp && !symArgs->exp)) && funcArgs)
        {
            symArgs = symArgs->next;
            if (funcArgs)
                funcArgs = funcArgs->next;
        }
        else
        {
            if (!TemplateDeduceFromArg(sp->tp, symArgs->tp, symArgs->exp, false, allowSelectors))
                rv = false;
            symArgs = symArgs->next;
            if (funcArgs)
                funcArgs = funcArgs->next;
        }
        templateArgs = templateArgs->next;
    }
    return rv && !symArgs;
}
static void SwapDefaultNames(TEMPLATEPARAMLIST* params, Optimizer::LIST* origNames)
{
    while (params && origNames)
    {
        const char* temp = ((SYMBOL*)origNames->data)->name;
        if (params->argsym)
        {
            ((SYMBOL*)origNames->data)->name = params->argsym->name;
            params->argsym->name = temp;
        }
        else if (params->argsym == (SYMBOL*)origNames->data)
        {
            params->argsym = nullptr;
        }
        else
        {
            params->argsym = (SYMBOL*)origNames->data;
        }
        params = params->next;
        origNames = origNames->next;
    }
}
bool TemplateParseDefaultArgs(SYMBOL* declareSym, TEMPLATEPARAMLIST* dest, TEMPLATEPARAMLIST* src,
                                     TEMPLATEPARAMLIST* enclosing)
{
    TEMPLATEPARAMLIST *primaryList = nullptr, *primaryDefaultList = nullptr;
    Optimizer::LIST* oldOpenStructs = openStructs;
    int oldStructLevel = structLevel;
    STRUCTSYM s, primary;
    LEXEME* head = nullptr;
    LEXEME* tail = nullptr;
    SYMBOL* oldMemberClass = instantiatingMemberFuncClass;
    memset(&primary, 0, sizeof(primary));
    if (declareSym->sb->specialized && declareSym->sb->parentTemplate &&
        !declareSym->sb->parentTemplate->templateParams->p->bySpecialization.types)
    {
        primaryList = declareSym->sb->parentTemplate->templateParams->next;
    }

    instantiatingMemberFuncClass = declareSym->sb->parentClass;
    if (currents)
    {
        head = currents->bodyHead;
        tail = currents->bodyTail;
    }
    s.tmpl = enclosing;
    addTemplateDeclaration(&s);
    while (src && dest)
    {
        if (!dest->p->byClass.val && !dest->p->packed && (!primaryList || !primaryList->p->packed))
        {
            LEXEME* lex;
            int n, pushCount;
            if (!src->p->byClass.txtdflt)
            {
                dropStructureDeclaration();
                instantiatingMemberFuncClass = oldMemberClass;
                return false;
            }
            SwapDefaultNames(enclosing, src->p->byClass.txtargs);
            n = PushTemplateNamespace(declareSym);
            if (primaryList && primaryList->p->byClass.txtdflt && primaryList->p->byClass.txtdflt == src->p->byClass.txtdflt)
            {
                if (!primaryDefaultList)
                {
                    TEMPLATEPARAMLIST** lst = &primaryDefaultList;
                    TEMPLATEPARAMLIST* one = declareSym->sb->parentTemplate->templateParams->next;
                    TEMPLATEPARAMLIST* two = declareSym->templateParams->p->bySpecialization.types;
                    while (one && two)
                    {
                        *lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                        (*lst)->argsym = one->argsym;
                        (*lst)->p = two->p;
                        lst = &(*lst)->next;
                        one = one->next;
                        two = two->next;
                    }
                }
                primary.tmpl = primaryDefaultList;
                addTemplateDeclaration(&primary);
                pushCount = 1;
            }
            else
            {
                pushCount = pushContext(declareSym, false);
            }
            dest->p->byClass.txtdflt = src->p->byClass.txtdflt;
            dest->p->byClass.txtargs = src->p->byClass.txtargs;
            lex = SetAlternateLex(src->p->byClass.txtdflt);
            switch (dest->p->type)
            {
                case kw_typename:
                {
                    lex = get_type_id(lex, &dest->p->byClass.val, nullptr, sc_cast, false, true, false);
                    if (!dest->p->byClass.val || dest->p->byClass.val->type == bt_any)
                    {
                        SwapDefaultNames(enclosing, src->p->byClass.txtargs);
                        while (pushCount--)
                            dropStructureDeclaration();
                        PopTemplateNamespace(n);
                        SetAlternateLex(nullptr);
                        dropStructureDeclaration();
                        instantiatingMemberFuncClass = oldMemberClass;
                        return false;
                    }
                    break;
                }
                case kw_template:
                {
                    char buf[256];
                    strcpy(buf, lex->value.s.a);
                    lex = id_expression(lex, nullptr, &dest->p->byTemplate.val, nullptr, nullptr, nullptr, false, false, buf);

                    if (!dest->p->byTemplate.val)
                    {
                        SwapDefaultNames(enclosing, src->p->byClass.txtargs);
                        while (pushCount--)
                            dropStructureDeclaration();
                        PopTemplateNamespace(n);
                        SetAlternateLex(nullptr);
                        dropStructureDeclaration();
                        instantiatingMemberFuncClass = oldMemberClass;
                        return false;
                    }
                }
                break;
                case kw_int:
                {
                    TYPE* tp1;
                    EXPRESSION* exp1 = nullptr;
                    if (dest->p->byNonType.txttype)
                    {
                        LEXEME* start = lex;
                        lex = SetAlternateLex(src->p->byNonType.txttype);
                        openStructs = nullptr;
                        structLevel = 0;
                        lex = expression_no_comma(lex, nullptr, nullptr, &tp1, &exp1, nullptr, _F_INTEMPLATEPARAMS);
                        openStructs = oldOpenStructs;
                        structLevel = oldStructLevel;
                        SetAlternateLex(nullptr);
                        lex = start;
                        if (tp1->type == bt_any)
                        {
                            SwapDefaultNames(enclosing, src->p->byClass.txtargs);
                            while (pushCount--)
                                dropStructureDeclaration();
                            PopTemplateNamespace(n);
                            SetAlternateLex(nullptr);
                            dropStructureDeclaration();
                            instantiatingMemberFuncClass = oldMemberClass;
                            return false;
                        }
                        dest->p->byNonType.tp = tp1;
                    }
                    openStructs = nullptr;
                    structLevel = 0;
                    lex = expression_no_comma(lex, nullptr, nullptr, &tp1, &exp1, nullptr, _F_INTEMPLATEPARAMS);
                    optimize_for_constants(&exp1);
                    openStructs = oldOpenStructs;
                    structLevel = oldStructLevel;
                    dest->p->byNonType.val = exp1;
                    if (!templatecomparetypes(dest->p->byNonType.tp, tp1, true))
                    {
                        if (!ispointer(tp1) && !isint(tp1) && !isconstzero(tp1, exp1))
                        {
                            SwapDefaultNames(enclosing, src->p->byClass.txtargs);
                            while (pushCount--)
                                dropStructureDeclaration();
                            PopTemplateNamespace(n);
                            SetAlternateLex(nullptr);
                            dropStructureDeclaration();
                            instantiatingMemberFuncClass = oldMemberClass;
                            return false;
                        }
                    }
                }
                break;
                default:
                    break;
            }
            SwapDefaultNames(enclosing, src->p->byClass.txtargs);
            while (pushCount--)
                dropStructureDeclaration();
            PopTemplateNamespace(n);
            SetAlternateLex(nullptr);
        }
        if (primaryList)
            primaryList = primaryList->next;
        src = src->next;
        dest = dest->next;
    }
    if (currents)
    {
        currents->bodyHead = head;
        currents->bodyTail = tail;
    }
    dropStructureDeclaration();
    instantiatingMemberFuncClass = oldMemberClass;
    return true;
}
void ScrubTemplateValues(SYMBOL* func)
{
    SYMLIST* templateArgs = basetype(func->tp)->syms->table[0];
    ClearArgValues(func->templateParams, func->sb->specialized);
    while (templateArgs)
    {
        TYPE* tp = ((SYMBOL*)templateArgs->p)->tp;
        while (isref(tp) || ispointer(tp))
            tp = basetype(tp)->btp;
        if (isstructured(tp) && basetype(tp)->sp->templateParams && !basetype(tp)->sp->sb->instantiated &&
            !basetype(tp)->sp->sb->declaring)
            ClearArgValues(basetype(tp)->sp->templateParams, basetype(tp)->sp->sb->specialized);
        templateArgs = templateArgs->next;
    }
    TYPE* retval = basetype(basetype(func->tp)->btp);
    if (isstructured(retval) && retval->sp->templateParams && !retval->sp->sb->instantiated && !retval->sp->sb->declaring)
        ClearArgValues(retval->sp->templateParams, retval->sp->sb->specialized);
}
void PushPopTemplateArgs(SYMBOL* func, bool push)
{
    SYMLIST* templateArgs = basetype(func->tp)->syms->table[0];
    PushPopValues(func->templateParams, push);
    while (templateArgs)
    {
        TYPE* tp = ((SYMBOL*)templateArgs->p)->tp;
        while (isref(tp) || ispointer(tp))
            tp = basetype(tp)->btp;
        if (isstructured(tp) && basetype(tp)->sp->templateParams && !basetype(tp)->sp->sb->instantiated &&
            !basetype(tp)->sp->sb->declaring)
            PushPopValues(basetype(tp)->sp->templateParams, push);
        templateArgs = templateArgs->next;
    }
    TYPE* retval = basetype(basetype(func->tp)->btp);
    if (isstructured(retval) && retval->sp->templateParams && !retval->sp->sb->instantiated && !retval->sp->sb->declaring)
        PushPopValues(retval->sp->templateParams, push);
}
static void TransferClassTemplates(TEMPLATEPARAMLIST* dflt, TEMPLATEPARAMLIST* val, TEMPLATEPARAMLIST* params);
SYMBOL* TemplateDeduceArgsFromArgs(SYMBOL* sym, FUNCTIONCALL* args)
{
    TEMPLATEPARAMLIST* nparams = sym->templateParams;
    TYPE* thistp = args->thistp;
    INITLIST* arguments = args->arguments;
    SYMBOL* rv;
    if (!thistp && ismember(sym) && arguments)
    {
        arguments = arguments->next;
        thistp = args->arguments->tp;
    }
    if (args && thistp && sym->sb->parentClass && !nparams)
    {
        TYPE* tp = basetype(basetype(thistp)->btp);
        TEMPLATEPARAMLIST* src = tp->sp->templateParams;
        TEMPLATEPARAMLIST* dest = sym->sb->parentClass->templateParams;
        if (src && dest)
        {
            src = src->next;
            dest = dest->next;
        }
        while (src && dest)
        {
            dest->p->byNonType.dflt = src->p->byNonType.dflt;
            dest->p->byNonType.val = src->p->byNonType.val;
            dest->p->byNonType.txtdflt = src->p->byNonType.txtdflt;
            dest->p->byNonType.txtargs = src->p->byNonType.txtargs;
            dest->p->byNonType.temp = src->p->byNonType.temp;
            dest->p->byNonType.tp = src->p->byNonType.tp;
            src = src->next;
            dest = dest->next;
        }

        if (src || dest)
            return nullptr;
    }
    if (nparams)
    {
        TEMPLATEPARAMLIST* params = nparams->next;
        SYMLIST *templateArgs = basetype(sym->tp)->syms->table[0], *temp;
        INITLIST* symArgs = arguments;
        TEMPLATEPARAMLIST* initial = args->templateParams;
        ScrubTemplateValues(sym);
        templateArgs = basetype(sym->tp)->syms->table[0];
        // fill in params that have been initialized in the arg list
        std::stack<TEMPLATEPARAMLIST*> tis;
        while (initial && params)
        {
            if (initial->p->type != params->p->type)
                return nullptr;
            params->p->initialized = true;
            if (initial->p->packed)
            {
                if (initial->p->byPack.pack)
                {
                    tis.push(initial->next);
                    initial = initial->p->byPack.pack;
                }
                else
                {
                    initial = initial->next;
                    if (params->p->packed)
                        params = params->next;
                }
            }
            if (!initial)
                break;
            if (params->p->packed)
            {
                TEMPLATEPARAMLIST* nparam = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                TEMPLATEPARAMLIST** p = &params->p->byPack.pack;
                nparam->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                while (*p)
                    p = &(*p)->next;
                nparam->p->type = params->p->type;
                nparam->p->byClass.val = initial->p->byClass.dflt;
                *p = nparam;
            }
            else
            {
                params->p->byClass.val = initial->p->byClass.dflt;
                params = params->next;
            }
            initial = initial->next;
            if (!initial && tis.size())
            {
                initial = tis.top();
                tis.pop();
            }
        }

        // check the specialization list for validity
        params = nparams->p->bySpecialization.types;
        initial = args->templateParams;
        while (initial && params)
        {
            if (initial->p->type != params->p->type)
                return nullptr;
            switch (initial->p->type)
            {
                case kw_typename:
                    if (!templatecomparetypes(initial->p->byClass.dflt, params->p->byClass.dflt, true))
                        return nullptr;
                    break;
                case kw_template:
                    if (!exactMatchOnTemplateParams(initial->p->byTemplate.dflt->templateParams->next,
                                                    params->p->byTemplate.dflt->templateParams->next))
                        return nullptr;
                    break;
                case kw_int:
                    if (!templatecomparetypes(initial->p->byNonType.tp, params->p->byNonType.tp, true) &&
                        (!ispointer(params->p->byNonType.tp) || !isconstzero(initial->p->byNonType.tp, params->p->byNonType.dflt)))
                        return nullptr;
                    break;
                default:
                    break;
            }
            initial = initial->next;
            if (!params->p->packed)
                params = params->next;
        }
        // Deduce any args that we can
        if (((SYMBOL*)(templateArgs->p))->sb->thisPtr)
            templateArgs = templateArgs->next;
        temp = templateArgs;
        while (temp)
        {
            if (((SYMBOL*)temp->p)->packed)
                break;
            temp = temp->next;
        }
        if (temp)
        {
            // we have to gather the args list
            params = nparams->next;
            while (templateArgs && symArgs)
            {
                SYMBOL* sp = (SYMBOL*)templateArgs->p;
                if (sp->packed)
                    break;
                if (!params || !params->p->byClass.dflt)
                {
                    if (TemplateDeduceFromArg(sp->tp, symArgs->tp, symArgs->exp, false, false))
                    {
                        if (isstructured(sp->tp) && basetype(sp->tp)->sp->templateParams)
                        {
                            TEMPLATEPARAMLIST* params = basetype(sp->tp)->sp->templateParams;
                            TEMPLATEPARAMLIST* special =
                                params->p->bySpecialization.types ? params->p->bySpecialization.types : params->next;
                            TransferClassTemplates(special, special, sym->templateParams->next);
                        }
                    }
                    symArgs = symArgs->next;
                }
                templateArgs = templateArgs->next;
                if (params)
                    params = params->next;
            }
            if (templateArgs)
            {
                SYMBOL* sp = (SYMBOL*)templateArgs->p;
                TYPE* tp = sp->tp;
                TEMPLATEPARAMLIST* base;
                if (isref(tp))
                    tp = basetype(tp)->btp;
                base = basetype(tp)->templateParam;
                if (base && base->p->type == kw_typename)
                {
                    TEMPLATEPARAMLIST** p = &base->p->byPack.pack;
                    while (symArgs)
                    {
                        *p = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                        (*p)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                        (*p)->p->type = kw_typename;
                        if (sp->tp->type == bt_rref && !symArgs->tp->rref &&
                            basetype(symArgs->tp)->type != bt_rref)  // unqualifed rref, candidate for forwarding
                        {

                            TYPE* x = (TYPE*)Alloc(sizeof(TYPE));
                            x->type = bt_lref;
                            x->size = getSize(bt_pointer);
                            TYPE* t = symArgs->tp;
                            if (isref(t))
                                t = basetype(t)->btp;
                            x->btp = t;
                            x->rootType = x;
                            (*p)->p->byClass.val = x;
                        }
                        else
                        {
                            (*p)->p->byClass.val = rewriteNonRef(symArgs->tp);
                        }
                        p = &(*p)->next;
                        symArgs = symArgs->next;
                    }
                }
            }
        }
        else
        {
            bool rv = TemplateDeduceArgList(basetype(sym->tp)->syms->table[0], templateArgs, symArgs, false);
            if (!rv)
            {
                params = nparams->next;
                while (params)
                {
                    if (params->p->packed && !params->p->byPack.pack)
                        return nullptr;
                    params = params->next;
                }
            }
            //            else
            {
                SYMLIST* hr = basetype(sym->tp)->syms->table[0];
                while (hr)
                {
                    SYMBOL* sp = hr->p;
                    TYPE* tp = sp->tp;
                    while (isref(tp) || ispointer(tp))
                        tp = basetype(tp)->btp;

                    if (isstructured(tp) && basetype(tp)->sp->templateParams)
                    {
                        TEMPLATEPARAMLIST* params = basetype(tp)->sp->templateParams;
                        TEMPLATEPARAMLIST* special =
                            params->p->bySpecialization.types ? params->p->bySpecialization.types : params->next;
                        if (special)
                            TransferClassTemplates(special, special, sym->templateParams->next);
                    }
                    hr = hr->next;
                }
            }
        }
        // set up default values for non-deduced and non-initialized args
        params = nparams->next;

        if (TemplateParseDefaultArgs(sym, params, params, params) &&
            ValidateArgsSpecified(sym->templateParams->next, sym, arguments))
        {
            if (isstructured(basetype(sym->tp)->btp))
            {
                TEMPLATEPARAMLIST* params = basetype(basetype(sym->tp)->btp)->sp->templateParams;
                if (params)
                {
                    TEMPLATEPARAMLIST* special =
                        params->p->bySpecialization.types ? params->p->bySpecialization.types : params->next;
                    while (special)
                    {
                        TransferClassTemplates(sym->templateParams->next, sym->templateParams->next, special);
                        special = special->next;
                    }
                }
            }
            rv = SynthesizeResult(sym, nparams);
            rv->sb->maintemplate = sym;
            return rv;
        }
        return nullptr;
    }
    rv = SynthesizeResult(sym, nparams);
    rv->sb->maintemplate = sym;
    return rv;
}
static bool TemplateDeduceFromType(TYPE* P, TYPE* A)
{
    if (P->type == bt_templatedecltype)
        P = LookupTypeFromExpression(P->templateDeclType, nullptr, false);
    if (P)
        return Deduce(P, A, true, false, false);
    return false;
}
SYMBOL* TemplateDeduceWithoutArgs(SYMBOL* sym)
{
    TEMPLATEPARAMLIST* nparams = sym->templateParams;
    TEMPLATEPARAMLIST* params = nparams->next;
    if (TemplateParseDefaultArgs(sym, params, params, params) && ValidateArgsSpecified(sym->templateParams->next, sym, nullptr))
    {
        return SynthesizeResult(sym, nparams);
    }
    return nullptr;
}
static bool TemplateDeduceFromConversionType(TYPE* orig, TYPE* tp)
{
    TYPE *P = orig, *A = tp;
    if (isref(P))
        P = basetype(P)->btp;
    if (!isref(A))
    {
        P = rewriteNonRef(P);
    }
    A = removeTLQuals(A);
    if (TemplateDeduceFromType(P, A))
        return true;
    if (ispointer(P))
    {
        bool doit = false;
        while (ispointer(P) && ispointer(A))
        {
            if ((isconst(P) && !isconst(A)) || (isvolatile(P) && !isvolatile(A)))
                return false;
            P = basetype(P)->btp;
            A = basetype(A)->btp;
        }
        P = basetype(P);
        A = basetype(A);
        if (doit && TemplateDeduceFromType(P, A))
            return true;
    }
    return false;
}
SYMBOL* TemplateDeduceArgsFromType(SYMBOL* sym, TYPE* tp)
{
    TEMPLATEPARAMLIST* nparams = sym->templateParams;
    ClearArgValues(nparams, sym->sb->specialized);
    if (sym->sb->castoperator)
    {
        TemplateDeduceFromConversionType(basetype(sym->tp)->btp, tp);
        return SynthesizeResult(sym, nparams);
    }
    else
    {
        SYMLIST* templateArgs = basetype(tp)->syms->table[0];
        SYMLIST* symArgs = basetype(sym->tp)->syms->table[0];
        TEMPLATEPARAMLIST* params;
        while (templateArgs && symArgs)
        {
            SYMBOL* sp = (SYMBOL*)symArgs->p;
            if (sp->packed)
                break;
            TemplateDeduceFromType(sp->tp, ((SYMBOL*)templateArgs->p)->tp);
            templateArgs = templateArgs->next;
            symArgs = symArgs->next;
        }
        if (templateArgs && symArgs)
        {
            SYMBOL* sp = (SYMBOL*)symArgs->p;
            TYPE* tp = sp->tp;
            TEMPLATEPARAMLIST* base;
            if (isref(tp))
                tp = basetype(tp)->btp;
            base = tp->templateParam;
            if (base->p->type == kw_typename)
            {
                TEMPLATEPARAMLIST** p = &base->p->byPack.pack;
                while (symArgs)
                {
                    *p = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                    (*p)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                    (*p)->p->type = kw_typename;
                    (*p)->p->byClass.val = sp->tp;
                    symArgs = symArgs->next;
                }
            }
        }
        TemplateDeduceFromType(basetype(sym->tp)->btp, basetype(tp)->btp);
        if (nparams)
        {
            params = nparams->next;
            if (TemplateParseDefaultArgs(sym, params, params, params) &&
                ValidateArgsSpecified(sym->templateParams->next, sym, nullptr))
            {
                return SynthesizeResult(sym, nparams);
            }
        }
    }
    return nullptr;
}
int TemplatePartialDeduceFromType(TYPE* orig, TYPE* sym, bool byClass)
{
    TYPE *P = orig, *A = sym;
    int which = -1;
    if (isref(P))
        P = basetype(P)->btp;
    if (isref(A))
        A = basetype(A)->btp;
    if (isref(orig) && isref(sym))
    {
        bool p = false, a = false;
        if ((isconst(P) && !isconst(A)) || (isvolatile(P) && !isvolatile(A)))
            p = true;
        if ((isconst(A) && !isconst(P)) || (isvolatile(A) && !isvolatile(P)))
            a = true;
        if (a && !p)
            which = 1;
    }
    A = removeTLQuals(A);
    P = removeTLQuals(P);
    if (!Deduce(P, A, true, byClass, false))
        return 0;
    return which;
}
int TemplatePartialDeduce(TYPE* origl, TYPE* origr, TYPE* syml, TYPE* symr, bool byClass)
{
    TEMPLATEPARAMLIST *left, *right;
    int n = TemplatePartialDeduceFromType(origl, symr, byClass);
    int m = TemplatePartialDeduceFromType(origr, syml, byClass);
    if (n && m)
    {
        if (basetype(origl)->type == bt_lref)
        {
            if (basetype(origr)->type != bt_lref)
                return -1;
            else
                return -1;  // originally checked n & m but since that's already checked just do this, pointing this out since
                            // it's GAURENTEED to return -1
            return 1;
        }
        else if (basetype(origr)->type == bt_lref)
        {
            return 1;
        }
        if (n > 0 && m <= 0)
            return -1;
        else if (m > 0 && n <= 0)
            return 1;
        n = m = 0;
    }
    if (n)
        return -1;
    if (m)
        return 1;
    if (isref(origl))
        origl = basetype(origl)->btp;
    if (isref(origr))
        origr = basetype(origr)->btp;
    left = basetype(origl)->templateParam;
    right = basetype(origr)->templateParam;
    if (left && right)
    {
        if (left->p->packed && !right->p->packed)
            return -1;
        if (right->p->packed && !left->p->packed)
            return 1;
    }
    return 0;
}
int TemplatePartialDeduceArgsFromType(SYMBOL* syml, SYMBOL* symr, TYPE* tpl, TYPE* tpr, FUNCTIONCALL* fcall)
{
    int which = 0;
    int arr[200], n;
    ClearArgValues(syml->templateParams, syml->sb->specialized);
    ClearArgValues(symr->templateParams, symr->sb->specialized);
    if (isstructured(syml->tp))
    {
        which = TemplatePartialDeduce(syml->tp, symr->tp, tpl, tpr, true);
    }
    else if (syml->sb->castoperator)
    {
        which =
            TemplatePartialDeduce(basetype(syml->tp)->btp, basetype(symr->tp)->btp, basetype(tpl)->btp, basetype(tpr)->btp, false);
    }
    else if (!isfunction(syml->tp))
    {
        which = TemplatePartialDeduce(syml->tp, symr->tp, tpl, tpr, true);
    }
    else
    {
        int i;
        SYMLIST* tArgsl = basetype(tpl)->syms->table[0];
        SYMLIST* sArgsl = basetype(syml->tp)->syms->table[0];
        SYMLIST* tArgsr = basetype(tpr)->syms->table[0];
        SYMLIST* sArgsr = basetype(symr->tp)->syms->table[0];
        bool usingargs = fcall && fcall->ascall;
        INITLIST* args = fcall ? fcall->arguments : nullptr;
        if (fcall && fcall->thisptr)
        {
            tArgsl = tArgsl->next;
            sArgsl = sArgsl->next;
            tArgsr = tArgsr->next;
            sArgsr = sArgsr->next;
        }
        n = 0;
        while (tArgsl && tArgsr && sArgsl && sArgsr && (!usingargs || args))
        {
            arr[n++] = TemplatePartialDeduce(((SYMBOL*)sArgsl->p)->tp, ((SYMBOL*)sArgsr->p)->tp, ((SYMBOL*)tArgsl->p)->tp,
                                             ((SYMBOL*)tArgsr->p)->tp, false);
            if (args)
                args = args->next;
            tArgsl = tArgsl->next;
            sArgsl = sArgsl->next;
            tArgsr = tArgsr->next;
            sArgsr = sArgsr->next;
        }
        for (i = 0; i < n; i++)
            if (arr[i] == 100)
                return 0;
        for (i = 0; i < n; i++)
            if (!which)
                which = arr[i];
            else if (which && arr[i] && which != arr[i])
                return 0;
    }
    if (which == 100)
        which = 0;
    if (!which)
    {
        /*
        if (!syml->sb->specialized && symr->sb->specialized)
        {
            TEMPLATEPARAMLIST *l = syml->templateParams->next;
            TEMPLATEPARAMLIST *r = symr->templateParams->p->bySpecialization.types;
            while (l && r)
            {
                if (!templatecomparetypes(l->p->byClass.val, r->p->byClass.val, true))
                    return -1;
                l = l->next;
                r = r->next;
            }
            return 0;
        }
        else
            */
        {
            TEMPLATEPARAMLIST* l = syml->templateParams->p->bySpecialization.types ? syml->templateParams->p->bySpecialization.types : syml->templateParams->next;
            TEMPLATEPARAMLIST* r = symr->templateParams->p->bySpecialization.types ? symr->templateParams->p->bySpecialization.types : symr->templateParams->next;
            int i;
            n = 0;
            while (l && r)
            {
                int l1 = l->p->type == kw_typename ? !!l->p->byClass.val : 0;
                int r1 = r->p->type == kw_typename ? !!r->p->byClass.val : 0;
                if (l1 && !r1)
                    arr[n++] = -1;
                else if (r1 && !l1)
                    arr[n++] = 1;
                l = l->next;
                r = r->next;
            }
            for (i = 0; i < n; i++)
                if (!which)
                    which = arr[i];
                else if (which && which != arr[i])
                    return 0;
        }
    }
    return which;
}
void TemplatePartialOrdering(SYMBOL** table, int count, FUNCTIONCALL* funcparams, TYPE* atype, bool asClass, bool save)
{
    (void)atype;
    int i, j, c = 0;
    int cn = 0;
    for (i = 0; i < count; i++)
        if (table[i])
            c++;
    if (c)
    {
        if (funcparams && funcparams->templateParams)
        {
            TEMPLATEPARAMLIST* t = funcparams->templateParams;
            while (t)
            {
                cn++;
                t = t->next;
            }
        }
    }
    if (c > 1)
    {
        int len = 0;
        for (i = 0; i < count; i++)
            if (table[i] && table[i]->sb->templateLevel)
                len++;
        Optimizer::LIST *types = nullptr, *exprs = nullptr, *classes = nullptr;
        TYPE** typetab = (TYPE**)_alloca(sizeof(TYPE*) * count);
        SYMBOL* allocedSyms = (SYMBOL*)_alloca(sizeof(SYMBOL) * len);
        sym::_symbody* allocedBodies = (sym::_symbody*)_alloca(sizeof(sym::_symbody) * len);
        TYPE* allocedTypes = (TYPE*)_alloca(sizeof(TYPE) * len);
        /*
        TYPE** typetab = (TYPE**)Alloc(sizeof(TYPE*) * count);
        SYMBOL *allocedSyms = (SYMBOL *)Alloc(sizeof(SYMBOL) *len);
        sym::_symbody *allocedBodies = (sym::_symbody *)Alloc(sizeof(sym::_symbody) * len);
        TYPE *allocedTypes = (TYPE *)Alloc(sizeof(TYPE) *len);
        */
        int j = 0;
        if (save)
            saveParams(table, count);
        for (i = 0; i < count; i++)
        {
            if (table[i] && table[i]->sb->templateLevel)
            {
                SYMBOL* sym = table[i];
                TEMPLATEPARAMLIST* params;
                Optimizer::LIST *typechk, *exprchk, *classchk;
                if (!asClass)
                {
                    sym = sym->sb->parentTemplate;
                }
                params = sym->templateParams->next;
                typechk = types;
                exprchk = exprs;
                classchk = classes;
                while (params)
                {
                    switch (params->p->type)
                    {
                        case kw_typename:
                            if (typechk)
                            {
                                params->p->byClass.temp = (TYPE*)typechk->data;
                                typechk = typechk->next;
                            }
                            else
                            {
                                Optimizer::LIST* lst = (Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
                                TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
                                tp->type = bt_class;
                                tp->sp = params->argsym;
                                tp->size = tp->sp->tp->size;
                                tp->rootType = tp;
                                params->p->byClass.temp = tp;
                                lst->data = tp;
                                lst->next = types;
                                types = lst;
                            }
                            break;
                        case kw_template:
                            params->p->byTemplate.temp = params->argsym;
                            break;
                        case kw_int:
                            /*
                            if (exprchk)
                            {
                                params->p->byNonType.temp = (EXPRESSION *)exprchk->data;
                                exprchk = exprchk->next;
                            }
                            else
                            {
                                Optimizer::LIST *lst = (Optimizer::LIST *)Alloc(sizeof(Optimizer::LIST));
                                EXPRESSION *exp = intNode(en_c_i, 47);
                                params->p->byNonType.temp = exp;
                                lst->data = exp;
                                lst->next = exprs;
                                exprs = lst;
                            }
                            */
                            break;
                        default:
                            break;
                    }
                    params = params->next;
                }
                if (isstructured(sym->tp))
                    typetab[i] = SynthesizeTemplate(sym->tp, &allocedSyms[j], &allocedBodies[j], &allocedTypes[j])->tp;
                else
                    typetab[i] = SynthesizeType(sym->tp, nullptr, true);
                j++;
            }
        }
        for (i = 0; i < count - 1; i++)
        {
            if (table[i])
            {
                for (j = i + 1; table[i] && j < count; j++)
                {
                    if (table[j])
                    {
                        int which = TemplatePartialDeduceArgsFromType(asClass ? table[i] : table[i]->sb->parentTemplate,
                                                                      asClass ? table[j] : table[j]->sb->parentTemplate, typetab[i],
                                                                      typetab[j], funcparams);
                        if (which < 0)
                        {
                            table[i] = 0;
                        }
                        else if (which > 0)
                        {
                            table[j] = 0;
                        }
                    }
                }
            }
        }
        if (save)
            restoreParams(table, count);
    }
}
static bool comparePointerTypes(TYPE* tpo, TYPE* tps)
{
    while (tpo && tps)
    {
        tpo = basetype(tpo);
        tps = basetype(tps);
        if (!tpo || !tps)
            return false;
        if (tpo->type == bt_templateparam)
            tpo = tpo->templateParam->p->byClass.dflt;
        if (tps->type == bt_templateparam)
            tps = tps->templateParam->p->byClass.dflt;
        if (!tpo || !tps || tpo->type == bt_templateparam || tps->type == bt_templateparam)
            return false;
        if (tpo->type != tps->type)
            return false;
        tpo = tpo->btp;
        tps = tps->btp;
    }
    return tpo == tps;
}
static bool TemplateInstantiationMatchInternal(TEMPLATEPARAMLIST* porig, TEMPLATEPARAMLIST* psym)
{
    if (porig && psym)
    {
        if (porig->p->bySpecialization.types)
        {
            porig = porig->p->bySpecialization.types;
        }
        else
        {
            porig = porig->next;
        }

        if (psym->p->bySpecialization.types)
        {
            psym = psym->p->bySpecialization.types;
        }
        else
        {
            psym = psym->next;
        }
        while (porig && psym)
        {
            void *xorig, *xsym;
            xorig = porig->p->byClass.val;
            xsym = psym->p->byClass.val;
            switch (porig->p->type)
            {
                case kw_typename:
                {
                    if (porig->p->packed != psym->p->packed)
                        return false;
                    if (porig->p->packed)
                    {
                        TEMPLATEPARAMLIST* packorig = porig->p->byPack.pack;
                        TEMPLATEPARAMLIST* packsym = psym->p->byPack.pack;
                        while (packorig && packsym)
                        {
                            TYPE* torig = (TYPE*)packorig->p->byClass.val;
                            TYPE* tsym = (TYPE*)packsym->p->byClass.val;
                            if (isref(torig) != isref(tsym))
                                return false;
                            if (basetype(torig)->array != basetype(tsym)->array)
                                return false;
                            if (basetype(torig)->array && !!basetype(torig)->esize != !!basetype(tsym)->esize)
                                return false;
                            if ((basetype(torig)->type == bt_enum) != (basetype(tsym)->type == bt_enum))
                                return false;
                            if (tsym->type == bt_templateparam)
                                tsym = tsym->templateParam->p->byClass.val;
                            if (!templatecomparetypes(torig, tsym, true) && !sameTemplate(torig, tsym))
                                return false;
                            if (isref(torig))
                                torig = basetype(torig)->btp;
                            if (isref(tsym))
                                tsym = basetype(tsym)->btp;
                            if (isconst(torig) != isconst(tsym) || isvolatile(torig) != isvolatile(tsym))
                                return false;
                            packorig = packorig->next;
                            packsym = packsym->next;
                        }
                        if (packorig || packsym)
                            return false;
                    }
                    else
                    {
                        TYPE* torig = (TYPE*)xorig;
                        TYPE* tsym = (TYPE*)xsym;
                        //                        if (tsym->type == bt_templateparam)
                        //                            tsym = tsym->templateParam->p->byClass.val;
                        if (isref(torig) != isref(tsym))
                            return false;
                        if (basetype(torig)->array != basetype(tsym)->array)
                            return false;
                        if (basetype(torig)->array && !!basetype(torig)->esize != !!basetype(tsym)->esize)
                            return false;
                        if ((basetype(torig)->type == bt_enum) != (basetype(tsym)->type == bt_enum))
                            return false;
                        if ((!templatecomparetypes(torig, tsym, true) || !templatecomparetypes(tsym, torig, true)) &&
                            !sameTemplate(torig, tsym))
                            return false;
                        if (!comparePointerTypes(torig, tsym))
                            return false;
                        if (isref(torig))
                            torig = basetype(torig)->btp;
                        if (isref(tsym))
                            tsym = basetype(tsym)->btp;
                        if (isconst(torig) != isconst(tsym) || isvolatile(torig) != isvolatile(tsym))
                            return false;
                        if (basetype(tsym)->type == bt_enum || basetype(tsym)->enumConst)
                        {
                            if (basetype(torig)->sp != basetype(tsym)->sp)
                                return false;
                        }
                    }
                    break;
                }
                case kw_template:
                    if (xorig != xsym)
                        return false;
                    break;
                case kw_int:
                    if (porig->p->packed != psym->p->packed)
                        return false;
                    if (porig->p->packed)
                    {
                        TEMPLATEPARAMLIST* packorig = porig->p->byPack.pack;
                        TEMPLATEPARAMLIST* packsym = psym->p->byPack.pack;
                        while (packorig && packsym)
                        {
                            EXPRESSION* torig = (EXPRESSION*)packorig->p->byClass.val;
                            EXPRESSION* tsym = (EXPRESSION*)packsym->p->byClass.val;
                            if (!templatecomparetypes(packorig->p->byNonType.tp, packsym->p->byNonType.tp, true))
                                return false;
                            //#ifndef PARSER_ONLY
                            if (tsym && !equalTemplateIntNode((EXPRESSION*)torig, (EXPRESSION*)tsym))
                                return false;
                            //#endif
                            packorig = packorig->next;
                            packsym = packsym->next;
                        }
                        if (packorig || packsym)
                            return false;
                    }
                    else
                    {
                        if (!templatecomparetypes(porig->p->byNonType.tp, psym->p->byNonType.tp, true))
                            return false;
                        //#ifndef PARSER_ONLY
                        if (xsym && xorig && !equalTemplateIntNode((EXPRESSION*)xorig, (EXPRESSION*)xsym))
                            return false;
                        //#endif
                    }
                    break;
                default:
                    break;
            }
            porig = porig->next;
            psym = psym->next;
        }
        if (psym && psym->p->packed && !psym->p->byPack.pack)
            return true;
    }
    return !porig && !psym;
}
bool TemplateInstantiationMatch(SYMBOL* orig, SYMBOL* sym)
{
    if (orig && orig->sb->parentTemplate == sym->sb->parentTemplate)
    {
        if (!TemplateInstantiationMatchInternal(orig->templateParams, sym->templateParams))
            return false;
        while (orig->sb->parentClass && sym->sb->parentClass)
        {
            orig = orig->sb->parentClass;
            sym = sym->sb->parentClass;
        }
        if (orig->sb->parentClass || sym->sb->parentClass)
            return false;
        return true;
    }
    return false;
}
static void TemplateTransferClassDeferred(SYMBOL* newCls, SYMBOL* tmpl)
{
    if (newCls->tp->syms && (!newCls->templateParams || !newCls->templateParams->p->bySpecialization.types))
    {
        SYMLIST* ns = newCls->tp->syms->table[0];
        SYMLIST* os = tmpl->tp->syms->table[0];
        while (ns && os)
        {
            SYMBOL* ss = (SYMBOL*)ns->p;
            SYMBOL* ts = (SYMBOL*)os->p;
            if (strcmp(ss->name, ts->name) != 0)
            {
                ts = search(ss->name, tmpl->tp->syms);
                // we might get here with ts = nullptr for example when a using statement inside a template
                // references base class template members which aren't defined yet.
            }
            if (ts)
            {
                if (ss->tp->type == bt_aggregate && ts->tp->type == bt_aggregate)
                {
                    SYMLIST* os2 = ts->tp->syms->table[0];
                    SYMLIST* ns2 = ss->tp->syms->table[0];
                    // these lists may be mismatched, in particular the old symbol table
                    // may have partial specializations for templates added after the class was defined...
                    while (ns2 && os2)
                    {
                        SYMBOL* ts2 = (SYMBOL*)os2->p;
                        SYMBOL* ss2 = (SYMBOL*)ns2->p;
                        if (ts2->sb->defaulted || ss2->sb->defaulted)
                            break;
                        ss2->sb->copiedTemplateFunction = true;
                        SYMLIST* tsf = basetype(ts2->tp)->syms->table[0];
                        if (ts2->sb->deferredCompile && !ss2->sb->deferredCompile)
                        {
                            SYMLIST* ssf = basetype(ss2->tp)->syms->table[0];
                            while (tsf && ssf)
                            {
                                if (!ssf->p->sb->anonymous || !tsf->p->sb->anonymous)
                                    ssf->p->name = tsf->p->name;
                                tsf = tsf->next;
                                ssf = ssf->next;
                            }
                            ss2->sb->deferredCompile = ts2->sb->deferredCompile;
                        }
                        ss2->sb->maintemplate = ts2;
                        ns2 = ns2->next;
                        os2 = os2->next;
                    }
                }
            }
            ns = ns->next;
            os = os->next;
        }
        ns = newCls->tp->tags->table[0]->next;
        os = tmpl->tp->tags->table[0]->next;
        while (ns && os)
        {
            SYMBOL* ss = (SYMBOL*)ns->p;
            SYMBOL* ts = (SYMBOL*)os->p;
            if (strcmp(ss->name, ts->name) != 0)
            {
                ts = search(ss->name, tmpl->tp->syms);
                // we might get here with ts = nullptr for example when a using statement inside a template
                // references base class template members which aren't defined yet.
            }
            if (ts)
            {
                if (isstructured(ss->tp))
                {
                    if (!ss->sb->deferredCompile)
                    {
                        ss->sb->deferredCompile = ts->sb->deferredCompile;
                        PerformDeferredInitialization(ss->tp, nullptr);
                    }
                    TemplateTransferClassDeferred(ss, ts);
                }
            }
            ns = ns->next;
            os = os->next;
        }
    }
}
static bool ValidSpecialization(TEMPLATEPARAMLIST* special, TEMPLATEPARAMLIST* args, bool templateMatch)
{
    while (special && args)
    {
        if (special->p->type != args->p->type)
        {
            if (args->p->type != kw_typename ||
                (args->p->byClass.dflt->type != bt_templateselector && args->p->byClass.dflt->type != bt_templatedecltype))
                return false;
        }
        if (!templateMatch)
        {
            if ((special->p->byClass.val && !args->p->byClass.dflt) || (!special->p->byClass.val && args->p->byClass.dflt))
                return false;
            switch (args->p->type)
            {
                case kw_typename:
                    if (args->p->byClass.dflt && !templatecomparetypes(special->p->byClass.val, args->p->byClass.dflt, true))
                        return false;
                    break;
                case kw_template:
                    if (args->p->byTemplate.dflt &&
                        !ValidSpecialization(special->p->byTemplate.args, args->p->byTemplate.dflt->templateParams, true))
                        return false;
                    break;
                case kw_int:
                    if (!templatecomparetypes(special->p->byNonType.tp, args->p->byNonType.tp, true))
                        if (!isint(special->p->byNonType.tp) || !isint(args->p->byNonType.tp))
                            return false;
                    break;
                default:
                    break;
            }
        }
        special = special->next;
        args = args->next;
    }
    return (!special || special->p->byClass.txtdflt) && !args;
}
static SYMBOL* MatchSpecialization(SYMBOL* sym, TEMPLATEPARAMLIST* args)
{
    if (sym->sb->specialized)
    {
        if (ValidSpecialization(sym->templateParams->p->bySpecialization.types, args, false))
            return sym;
    }
    else
    {
        if (ValidSpecialization(sym->templateParams->next, args, true))
            return sym;
    }
    return nullptr;
}
static int pushContext(SYMBOL* cls, bool all)
{
    STRUCTSYM* s;
    int rv;
    if (!cls)
        return 0;
    rv = pushContext(cls->sb->parentClass, true);
    if (cls->sb->templateLevel)
    {
        s = (STRUCTSYM*)Alloc(sizeof(STRUCTSYM));
        s->tmpl = copyParams(cls->templateParams, false);
        addTemplateDeclaration(s);
        rv++;
    }
    if (all)
    {
        s = (STRUCTSYM*)Alloc(sizeof(STRUCTSYM));
        s->str = cls;
        addStructureDeclaration(s);
        rv++;
    }
    return rv;
}
void SetTemplateNamespace(SYMBOL* sym)
{
    Optimizer::LIST* list = nameSpaceList;
    sym->sb->templateNameSpace = nullptr;
    while (list)
    {
        Optimizer::LIST* nlist = (Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
        nlist->data = list->data;
        nlist->next = sym->sb->templateNameSpace;
        sym->sb->templateNameSpace = nlist;
        list = list->next;
    }
}
int PushTemplateNamespace(SYMBOL* sym)
{
    int rv = 0;
    Optimizer::LIST* list = nameSpaceList;
    while (list)
    {
        SYMBOL* sp = (SYMBOL*)list->data;
        sp->sb->value.i++;
        list = list->next;
    }
    list = sym ? sym->sb->templateNameSpace : nullptr;
    while (list)
    {
        SYMBOL* sp = (SYMBOL*)list->data;
        if (!sp->sb->value.i || (!list->next && nameSpaceList->data != sp))
        {
            Optimizer::LIST* nlist;
            sp->sb->value.i++;

            nlist = (Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
            nlist->next = nameSpaceList;
            nlist->data = sp;
            nameSpaceList = nlist;

            NAMESPACEVALUELIST* vl = (NAMESPACEVALUELIST*)Alloc(sizeof(NAMESPACEVALUELIST));
            vl->valueData = sp->sb->nameSpaceValues->valueData;
            vl->next = globalNameSpace;
            globalNameSpace = vl;

            rv++;
        }
        list = list->next;
    }
    return rv;
}
void PopTemplateNamespace(int n)
{
    int i;
    Optimizer::LIST* list;
    for (i = 0; i < n; i++)
    {
        Optimizer::LIST* nlist;
        SYMBOL* sp;
        globalNameSpace = globalNameSpace->next;
        nlist = nameSpaceList;
        sp = (SYMBOL*)nlist->data;
        sp->sb->value.i--;
        nameSpaceList = nameSpaceList->next;
    }
    list = nameSpaceList;
    while (list)
    {
        SYMBOL* sp = (SYMBOL*)list->data;
        sp->sb->value.i--;
        list = list->next;
    }
}
static void SetTemplateArgAccess(SYMBOL* sym, bool accessible)
{
    if (accessible)
    {
        //        if (!instantiatingTemplate && !isExpressionAccessible(theCurrentFunc ? theCurrentFunc->sb->parentClass : nullptr,
        //        sym, theCurrentFunc, nullptr, false))
        //            errorsym(ERR_CANNOT_ACCESS, sym);

        sym->sb->accessibleTemplateArgument++;
    }
    else
    {
        sym->sb->accessibleTemplateArgument--;
    }
}
static void SetAccessibleTemplateArgs(TEMPLATEPARAMLIST* args, bool accessible)
{
    while (args)
    {
        if (args->p->packed)
        {
            SetAccessibleTemplateArgs(args->p->byPack.pack, accessible);
        }
        else
            switch (args->p->type)
            {
                case kw_int:
                {
                    EXPRESSION* exp = args->p->byNonType.val;
                    //#ifndef PARSER_ONLY
                    if (exp)
                        exp = GetSymRef(exp);
                    if (exp)
                    {
                        SetTemplateArgAccess(exp->v.sp, accessible);
                    }
                    //#endif
                    break;
                }
                case kw_template:
                {
                    TEMPLATEPARAMLIST* tpl = args->p->byTemplate.args;
                    while (tpl)
                    {
                        if (!allTemplateArgsSpecified(nullptr, tpl))
                            return;
                        tpl = tpl->next;
                    }
                    if (args->p->byTemplate.val)
                        SetTemplateArgAccess(args->p->byTemplate.val, accessible);
                }
                break;
                case kw_typename:
                    if (args->p->byClass.val)
                    {
                        if (isstructured(args->p->byClass.val))
                        {
                            SetTemplateArgAccess(basetype(args->p->byClass.val)->sp, accessible);
                        }
                        else if (basetype(args->p->byClass.val)->type == bt_enum)  // DAL FIXED
                        {
                            SetTemplateArgAccess(basetype(args->p->byClass.val)->sp, accessible);
                        }
                    }
                    break;
                default:
                    break;
            }
        args = args->next;
    }
}
void SwapMainTemplateArgs(SYMBOL* cls)
{
    if (cls->sb->maintemplate)
    {
        TEMPLATEPARAMLIST* old = cls->sb->maintemplate->templateParams;
        TEMPLATEPARAMLIST* nw = cls->templateParams;
        while (old && nw)
        {
            SYMBOL* sp = old->argsym;
            old->argsym = nw->argsym;
            nw->argsym = sp;
            old = old->next;
            nw = nw->next;
        }
    }
}
SYMBOL* TemplateClassInstantiateInternal(SYMBOL* sym, TEMPLATEPARAMLIST* args, bool isExtern)
{
    (void)args;
    LEXEME* lex = nullptr;
    SYMBOL* cls = sym;
    int pushCount;
    if (cls->sb->attribs.inheritable.linkage == lk_virtual)
        return cls;
    if (packIndex == -1 && sym->sb->maintemplate)
    {
        TEMPLATEPARAMLIST *tl, *ts;
        tl = sym->templateParams;
        ts = sym->sb->maintemplate->templateParams;
        while (tl && ts)
        {
            if (tl->p->packed && !ts->p->packed)
                return cls;
            tl = tl->next;
            ts = ts->next;
        }
    }

    if (!isExtern)
    {
        if (sym->sb->maintemplate && (!sym->sb->specialized || sym->sb->maintemplate->sb->specialized))
        {
            lex = sym->sb->maintemplate->sb->deferredCompile;
            if (lex)
                sym->tp = sym->sb->maintemplate->tp;
        }
        if (!lex)
            lex = sym->sb->deferredCompile;
        if (!lex && sym->sb->parentTemplate && (!sym->sb->specialized || sym->sb->parentTemplate->sb->specialized))
            lex = sym->sb->parentTemplate->sb->deferredCompile;
        if (lex)
        {
            int oldHeaderCount = templateHeaderCount;
            Optimizer::LIST* oldDeferred = deferred;
            bool defd = false;
            SYMBOL old;
            int nsl = PushTemplateNamespace(sym);
            LEXEME* reinstateLex = lex;
            bool oldTemplateType = inTemplateType;
            LAMBDA* oldLambdas = lambdas;
            int oldPackIndex = packIndex;
            int oldExpandingParams = expandingParams;
            int oldRegisterTemplate = dontRegisterTemplate;
            HASHTABLE* oldSyms = localNameSpace->valueData->syms;
            NAMESPACEVALUELIST* oldNext = localNameSpace->next;
            HASHTABLE* oldTags = localNameSpace->valueData->tags;
            int oldInArgs = inTemplateArgs;
            int oldArgumentNesting = argument_nesting;
            int oldFuncLevel = funcLevel;
            funcLevel = 0;
            argument_nesting = 0;
            inTemplateArgs = 0;
            expandingParams = 0;
            localNameSpace->valueData->syms = nullptr;
            localNameSpace->valueData->tags = nullptr;
            localNameSpace->next = nullptr;
            SetAccessibleTemplateArgs(cls->templateParams, true);
            packIndex = -1;
            deferred = nullptr;
            templateHeaderCount = 0;
            lambdas = nullptr;
            old = *cls;
#ifdef PARSER_ONLY
            cls->sb->parserSet = false;
#endif
            cls->sb->attribs.inheritable.linkage = lk_virtual;
            cls->sb->parentClass = SynthesizeParentClass(cls->sb->parentClass);
            /*            if (cls->sb->parentTemplate)
                        {
                            TEMPLATEPARAMLIST *left = cls->templateParams;
                            TEMPLATEPARAMLIST *right = cls->sb->parentTemplate->templateParams;
                            left = left->next;
                            right = right->next;
                            while (left && right)
                            {
                                if (!left->argsym)
                                   left->argsym = right->argsym;
                                left = left->next;
                                right = right->next;
                            }
                        }
                        */
            SwapMainTemplateArgs(cls);
            pushCount = pushContext(cls, false);
            cls->sb->attribs.inheritable.linkage = lk_virtual;
            cls->tp = (TYPE*)Alloc(sizeof(TYPE));
            *cls->tp = *old.tp;
            UpdateRootTypes(cls->tp);
            cls->tp->syms = nullptr;
            cls->tp->tags = nullptr;
            cls->tp->sp = cls;
            cls->sb->baseClasses = nullptr;
            cls->sb->vbaseEntries = nullptr;
            instantiatingTemplate++;
            dontRegisterTemplate = templateNestingCount != 0;
            SwapMainTemplateArgs(cls);
            lex = SetAlternateLex(lex);
            lex = innerDeclStruct(lex, nullptr, cls, false, cls->tp->type == bt_class ? ac_private : ac_public, cls->sb->isfinal,
                                  &defd);
            SetAlternateLex(nullptr);
            SwapMainTemplateArgs(cls);
            lex = reinstateLex;
            while (lex)
            {
                lex->registered = false;
                lex = lex->next;
            }
            SetAccessibleTemplateArgs(cls->templateParams, false);
            if (old.tp->syms)
                TemplateTransferClassDeferred(cls, &old);
            PopTemplateNamespace(nsl);
            dontRegisterTemplate = oldRegisterTemplate;
            packIndex = oldPackIndex;
            lambdas = oldLambdas;
            instantiatingTemplate--;
            inTemplateType = oldTemplateType;
            deferred = oldDeferred;
            cls->sb->instantiated = true;
            localNameSpace->next = oldNext;
            localNameSpace->valueData->syms = oldSyms;
            localNameSpace->valueData->tags = oldTags;
            inTemplateArgs = oldInArgs;
            argument_nesting = oldArgumentNesting;
            expandingParams = oldExpandingParams;
            funcLevel = oldFuncLevel;
            templateHeaderCount = oldHeaderCount;
            while (pushCount--)
                dropStructureDeclaration();
            SwapMainTemplateArgs(cls);
        }
        else
        {
            //            errorsym(ERR_TEMPLATE_CANT_INSTANTIATE_NOT_DEFINED, sym->sb->parentTemplate);
        }
    }
    return cls;
}
SYMBOL* TemplateClassInstantiate(SYMBOL* sym, TEMPLATEPARAMLIST* args, bool isExtern, enum e_sc storage_class)
{
    if (templateNestingCount)
    {
        SYMBOL* sym1 = MatchSpecialization(sym, args);
        if (sym1 && (storage_class == sc_parameter || !inTemplateBody))
        {
            TEMPLATEPARAMLIST* tpm;
            TYPE **tpx, *tp = sym1->tp;
            tpm = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
            tpm->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
            tpm->p->type = kw_new;
            tpm->next = args;
            sym1 = clonesym(sym1);
            sym1->templateParams = tpm;
            tpx = &sym1->tp;
            while (tp)
            {
                *tpx = (TYPE*)Alloc(sizeof(TYPE));
                **tpx = *tp;
                UpdateRootTypes(*tpx);
                if (!tp->btp)
                {
                    (*tpx)->sp = sym1;
                    (*tpx)->templateParam = tpm;
                }
                else
                {
                    tpx = &(*tpx)->btp;
                }
                tp = tp->btp;
            }
        }
        return sym1;
    }
    else
    {
        return TemplateClassInstantiateInternal(sym, args, isExtern);
    }
}
void TemplateDataInstantiate(SYMBOL* sym, bool warning, bool isExtern)
{
    (void)isExtern;
    if (!sym->sb->gentemplate)
    {
        InsertInlineData(sym);
        sym->sb->gentemplate = true;
    }
    else if (warning)
    {
        errorsym(ERR_TEMPLATE_ALREADY_INSTANTIATED, sym);
    }
}
SYMBOL* TemplateFunctionInstantiate(SYMBOL* sym, bool warning, bool isExtern)
{
    SYMBOL* orig = sym;
    STRUCTSYM* old;
    SYMLIST* hr;
    LEXEME* lex;
    int pushCount;
    bool found = false;
    STRUCTSYM s;
    LAMBDA* oldLambdas;
    hr = sym->sb->overloadName->tp->syms->table[0];
    while (hr)
    {
        SYMBOL* data = hr->p;
        if (data->sb->instantiated && TemplateInstantiationMatch(data, sym) && matchOverload(sym->tp, data->tp, true))
        {
            sym = data;
            if (sym->sb->attribs.inheritable.linkage == lk_virtual || isExtern)
                return sym;
            found = true;
            break;
        }
        hr = hr->next;
    }
    oldLambdas = lambdas;
    lambdas = nullptr;
    old = structSyms;
    structSyms = 0;
    sym->templateParams = copyParams(sym->templateParams, true);
    sym->sb->instantiated = true;
    SetLinkerNames(sym, lk_cdecl);
    sym->sb->gentemplate = true;
    pushCount = pushContext(sym->sb->parentClass, true);
    s.tmpl = sym->templateParams;
    addTemplateDeclaration(&s);
    pushCount++;
#ifdef PARSER_ONLY
    sym->sb->parserSet = false;
#endif
    if (!found)
    {
        bool ok = true;
        SYMLIST* hr = sym->sb->overloadName->tp->syms->table[0];
        while (hr)
        {
            if (matchOverload(sym->tp, hr->p->tp, true))
            {
                ok = false;
                break;
            }
            hr = hr->next;
        }
        if (ok)
        {
            SYMLIST* hr = sym->sb->overloadName->tp->syms->table[0];
            insertOverload(sym, sym->sb->overloadName->tp->syms);
            while (hr)
            {
                if (matchOverload(sym->tp, hr->p->tp, true))
                {
                    ok = false;
                    break;
                }
                hr = hr->next;
            }
        }

        if (ismember(sym))
        {
            injectThisPtr(sym, basetype(sym->tp)->syms);
        }
    }
    if (!isExtern && !sym->sb->dontinstantiate)
    {
        lex = sym->sb->deferredCompile;
        if (lex)
        {
            Optimizer::LINEDATA* oldLinesHead = linesHead;
            Optimizer::LINEDATA* oldLinesTail = linesTail;
            int oldHeaderCount = templateHeaderCount;
            bool oldTemplateType = inTemplateType;
            int oldPackIndex = packIndex;
            int oldExpandingParams = expandingParams;
            int nsl = PushTemplateNamespace(sym);
            int oldArgumentNesting = argument_nesting;
            expandingParams = 0;
            instantiatingFunction++;
            argument_nesting = 0;
            packIndex = -1;
            linesHead = linesTail = nullptr;
            if (sym->sb->storage_class != sc_member && sym->sb->storage_class != sc_mutable && sym->sb->storage_class != sc_virtual)
                sym->sb->storage_class = sc_global;
            sym->sb->attribs.inheritable.linkage = lk_virtual;
            sym->sb->xc = nullptr;
            sym->sb->maintemplate = orig;
            sym->sb->redeclared = false;
            instantiatingTemplate++;

            lex = SetAlternateLex(sym->sb->deferredCompile);
            if (MATCHKW(lex, kw_try) || MATCHKW(lex, colon))
            {
                bool viaTry = MATCHKW(lex, kw_try);
                if (viaTry)
                {
                    sym->sb->hasTry = true;
                    lex = getsym();
                }
                if (MATCHKW(lex, colon))
                {
                    lex = getsym();
                    sym->sb->memberInitializers = GetMemberInitializers(&lex, nullptr, sym);
                }
            }
            templateHeaderCount = 0;
            lex = body(lex, sym);
            Optimizer::SymbolManager::Get(sym)->xc = false;
            templateHeaderCount = oldHeaderCount;
            lex = sym->sb->deferredCompile;
            while (lex)
            {
                lex->registered = false;
                lex = lex->next;
            }
            SetAlternateLex(nullptr);
            PopTemplateNamespace(nsl);
            argument_nesting = oldArgumentNesting;
            packIndex = oldPackIndex;
            inTemplateType = oldTemplateType;
            linesHead = oldLinesHead;
            linesTail = oldLinesTail;
            instantiatingTemplate--;
            instantiatingFunction--;
            expandingParams = oldExpandingParams;
        }
        else
        {
            if (!ismember(sym))
                sym->sb->storage_class = sc_external;
            InsertInline(sym);
        }
    }
    while (pushCount--)
        dropStructureDeclaration();
    lambdas = oldLambdas;
    structSyms = old;
    return sym;
}
static bool CheckConstCorrectness(TYPE* P, TYPE* A, bool byClass)
{
    while (P && A)
    {
        P = basetype(P);
        A = basetype(A);
        if (P->type != A->type)
            break;
        P = P->btp;
        A = A->btp;
        if (P && A)
        {
            if (byClass)
            {
                if ((isconst(A) != isconst(P)) || (isvolatile(A) != isvolatile(P)))
                    return false;
            }
            else
            {
                if ((isconst(A) && !isconst(P)) || (isvolatile(A) && !isvolatile(P)))
                    return false;
            }
        }
    }
    return true;
}
static void TemplateConstOrdering(SYMBOL** spList, int n, TEMPLATEPARAMLIST* params)
{
    int i;
    char hconst[200];
    char hvolatile[200];
    int count = 0;
    for (i = 0; i < n; i++)
    {
        if (spList[i])
        {
            int current = 0;
            TEMPLATEPARAMLIST* P = spList[i]->templateParams->p->bySpecialization.types;
            while (P)
            {
                if (current >= count)
                {
                    hconst[current] = 0;
                    hvolatile[current] = 0;
                    count++;
                }
                if (P->p->type == kw_typename)
                {
                    TYPE* tp = P->p->byClass.dflt;
                    if (tp)
                    {
                        while (isref(tp))
                            tp = basetype(tp)->btp;

                        if (isconst(tp))
                            hconst[current] = 1;
                        if (isvolatile(tp))
                            hvolatile[current] = 1;
                    }
                }
                current++;
                P = P->next;
            }
        }
    }
    for (i = 0; i < n; i++)
        if (spList[i])
        {
            TEMPLATEPARAMLIST* P = spList[i]->templateParams->p->bySpecialization.types;
            TEMPLATEPARAMLIST* A = params;
            int current = 0;
            std::stack<TEMPLATEPARAMLIST*> tas;
            while (P && A)
            {
                if (hconst[current] || hvolatile[current])
                {
                    if (A->p->packed)
                    {
                        tas.push(A->next);
                        A = A->p->byPack.pack;
                    }
                    if (A && P->p->type == kw_typename)
                    {
                        TYPE* ta = A->p->byClass.dflt;
                        TYPE* tp = P->p->byClass.dflt;
                        if (ta && tp)
                        {
                            while (isref(ta))
                                ta = basetype(ta)->btp;
                            while (isref(tp))
                                tp = basetype(tp)->btp;
                            if ((isconst(ta) && !isconst(tp) && hconst[current]) ||
                                (isvolatile(ta) && !isvolatile(tp) && hvolatile[current]) || !CheckConstCorrectness(tp, ta, true))
                            {
                                spList[i] = 0;
                                break;
                            }
                        }
                    }
                }
                current++;
                if (A)
                    A = A->next;
                P = P->next;
                if (!A && tas.size())
                {
                    A = tas.top();
                    tas.pop();
                }
            }
        }
}
static bool TemplateConstMatchingInternal(TEMPLATEPARAMLIST* P)
{
    bool found = true;
    while (P)
    {
        if (P->p->type == kw_typename)
        {
            if (!P->p->packed)
            {
                TYPE* td = P->p->byClass.dflt;
                TYPE* tv = P->p->byClass.val;
                if (!tv && isstructured(P->p->byClass.dflt) && basetype(P->p->byClass.dflt)->sp->sb->templateLevel)
                {
                    if (!TemplateConstMatchingInternal(basetype(P->p->byClass.dflt)->sp->templateParams))
                    {
                        found = false;
                        break;
                    }
                }
                else if (!tv)
                {
                    return false;
                }
                else
                {
                    if (isref(td))
                        td = basetype(td)->btp;
                    if (isref(tv))
                        tv = basetype(tv)->btp;
                    if (td->type == bt_memberptr)
                        td = basetype(td)->btp;
                    if (tv->type == bt_memberptr)
                        tv = basetype(tv)->btp;
                    if ((isconst(td) != isconst(tv)) || ((isvolatile(td) != isvolatile(tv))) ||
                        !CheckConstCorrectness(td, tv, true))
                    {
                        found = false;
                        break;
                    }
                }
            }
        }
        P = P->next;
    }
    return found;
}
static void TemplateConstMatching(SYMBOL** spList, int n, TEMPLATEPARAMLIST* params)
{
    (void)params;
    int i;
    bool found = false;
    for (i = 0; i < n && !found; i++)
        if (spList[i])
        {
            TEMPLATEPARAMLIST* P;
            found = true;
            if (i == 0)
            {
                P = spList[i]->templateParams->next;
                while (P)
                {
                    if (P->p->type == kw_typename)
                    {
                        if (!P->p->packed)
                        {
                            if (P->p->byClass.val)
                            {
                                TYPE* tv = P->p->byClass.val;
                                if (isref(tv))
                                    tv = basetype(tv)->btp;
                                if (isconst(tv) || isvolatile(tv))
                                {
                                    found = false;
                                    break;
                                }
                            }
                        }
                    }
                    P = P->next;
                }
            }
            else
            {
                found = TemplateConstMatchingInternal(spList[i]->templateParams->p->bySpecialization.types);
            }
        }
    if (found)
    {
        for (i = 0; i < n; i++)
            if (spList[i])
            {
                TEMPLATEPARAMLIST* P;
                if (i == 0)
                {
                    P = spList[i]->templateParams->next;
                    while (P)
                    {
                        if (P->p->type == kw_typename)
                        {
                            if (!P->p->packed)
                            {
                                if (P->p->byClass.val)
                                {
                                    TYPE* tv = P->p->byClass.val;
                                    if (isref(tv))
                                        tv = basetype(tv)->btp;
                                    if (isconst(tv) || isvolatile(tv))
                                    {
                                        spList[i] = 0;
                                    }
                                }
                                else
                                {
                                    spList[i] = 0;
                                }
                            }
                        }
                        P = P->next;
                    }
                }
                else
                {
                    if (!TemplateConstMatchingInternal(spList[i]->templateParams->p->bySpecialization.types))
                        spList[i] = 0;
                }
            }
    }
}
static void TransferClassTemplates(TEMPLATEPARAMLIST* dflt, TEMPLATEPARAMLIST* val, TEMPLATEPARAMLIST* params)
{
    if (val->p->type == kw_typename && val->p->byClass.dflt && val->p->byClass.val && isstructured(val->p->byClass.dflt) &&
        isstructured(val->p->byClass.val))
    {
        TEMPLATEPARAMLIST* tpdflt = basetype(val->p->byClass.dflt)->sp->templateParams;
        TEMPLATEPARAMLIST* tpval = basetype(val->p->byClass.val)->sp->templateParams;
        while (tpdflt && tpval)
        {
            TransferClassTemplates(tpdflt, tpval, params);
            tpdflt = tpdflt->next;
            tpval = tpval->next;
        }
    }
    else if (val->p->type == kw_typename && val->p->byClass.dflt && val->p->byClass.val && isfunction(val->p->byClass.dflt) &&
             isfunction(val->p->byClass.val))
    {
        SYMLIST *hrd, *hrv;
        TYPE* tpd = basetype(val->p->byClass.dflt)->btp;
        TYPE* tpv = basetype(val->p->byClass.val)->btp;
        if (tpd->type == bt_templateparam && tpd->templateParam->argsym)
        {
            TEMPLATEPARAMLIST* find = params;
            while (find)
            {
                if (find->argsym && !strcmp(find->argsym->name, tpd->templateParam->argsym->name))
                    break;
                find = find->next;
            }
            if (find)
            {
                if (tpd->templateParam->p->packed && !find->p->byPack.pack)
                {
                    if (tpv->type == bt_templateparam)
                    {
                        find->p->byPack.pack = tpv->templateParam->p->byPack.pack;
                    }
                    else
                    {
                        find->p->byPack.pack = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                        find->p->byPack.pack->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                        find->p->byPack.pack->p->type = kw_typename;
                        find->p->byPack.pack->p->byClass.val = tpv;
                    }
                }
                if (!find->p->byClass.val)
                    find->p->byClass.val = tpv->type == bt_templateparam ? tpv->templateParam->p->byClass.val : tpv;
            }
        }
        hrd = basetype(val->p->byClass.dflt)->syms->table[0];
        hrv = basetype(val->p->byClass.val)->syms->table[0];
        while (hrd && hrv)
        {
            tpd = (hrd->p)->tp;
            tpv = (hrv->p)->tp;
            if (tpd->type == bt_templateparam && tpd->templateParam->argsym)
            {
                TEMPLATEPARAMLIST* find = params;
                while (find)
                {
                    if (find->argsym && !strcmp(find->argsym->name, tpd->templateParam->argsym->name))
                        break;
                    find = find->next;
                }
                if (find)
                {
                    if (tpd->templateParam->p->packed)
                    {
                        if (!find->p->byPack.pack)
                        {
                            if (tpv->type == bt_templateparam)
                            {
                                find->p->byPack.pack = tpv->templateParam->p->byPack.pack;
                                hrd = hrd->next;
                            }
                            else
                            {
                                find->p->byPack.pack = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                find->p->byPack.pack->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                                find->p->byPack.pack->p->type = kw_typename;
                                find->p->byPack.pack->p->byClass.val = tpv;
                            }
                        }
                        else
                        {
                            TEMPLATEPARAMLIST* next = find->p->byPack.pack;
                            while (next->next)
                                next = next->next;
                            next->next = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                            next->next->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                            next->next->p->type = kw_typename;
                            next->next->p->byClass.val = tpv;
                        }
                    }
                    else
                    {
                        hrd = hrd->next;
                    }
                    if (!find->p->byClass.val)
                        find->p->byClass.val = tpv->type == bt_templateparam ? tpv->templateParam->p->byClass.val : tpv;
                }
            }
            else
            {
                hrd = hrd->next;
            }
            hrv = hrv->next;
        }
    }
    // here to support return types, templateselectors would otherwise be resolved by now
    else
    {
        if (dflt->argsym)
        {
            while (params)
            {
                if (params->argsym && !strcmp(dflt->argsym->name, params->argsym->name))
                {
                    if (dflt->p->type == params->p->type && dflt->p->packed == params->p->packed)
                    {
                        if (dflt->p->packed && !params->p->byPack.pack)
                            params->p->byPack.pack = val->p->byPack.pack;
                        if (!params->p->byClass.val)
                            params->p->byClass.val = val->p->byClass.val;
                    }
                    break;
                }
                params = params->next;
            }
        }
        if (params && params->p->type == kw_typename && !params->p->packed && params->p->byClass.dflt &&
basetype(params->p->byClass.dflt)->type == bt_templateselector &&
basetype(params->p->byClass.dflt)->sp->sb->templateSelector->next->isTemplate)
        {
        TEMPLATEPARAMLIST* param1 = basetype(params->p->byClass.dflt)->sp->sb->templateSelector->next->templateParams;
        while (param1)
        {
            TransferClassTemplates(dflt, val, param1);
            param1 = param1->next;
        }
        }
    }
}
static SYMBOL* ValidateClassTemplate(SYMBOL* sp, TEMPLATEPARAMLIST* unspecialized, TEMPLATEPARAMLIST* args)
{
    (void)unspecialized;
    SYMBOL* rv = nullptr;
    TEMPLATEPARAMLIST* nparams = sp->templateParams;
    if (nparams)
    {
        TEMPLATEPARAMLIST* spsyms = nparams->p->bySpecialization.types;
        TEMPLATEPARAMLIST *params = spsyms ? spsyms : nparams->next, *origParams = params;
        TEMPLATEPARAMLIST* primary = spsyms ? spsyms : nparams->next;
        TEMPLATEPARAMLIST* initial = args;
        TEMPLATEPARAMLIST* max = nparams->next ? nparams->next : spsyms;
        rv = sp;
        if (!spsyms)
        {
            ClearArgValues(params, sp->sb->specialized);
        }
        ClearArgValues(spsyms, sp->sb->specialized);
        ClearArgValues(sp->templateParams, sp->sb->specialized);
        std::stack<TEMPLATEPARAMLIST*> tis;
        while (initial && params)
        {
            if (initial->p->packed)
            {
                tis.push(initial->next);
                initial = initial->p->byPack.pack;
                if (!initial && params->p->packed && params->next)
                    params = params->next;
            }
            if (initial && params)
            {
                if (params->p->packed)
                {
                    while (initial && !initial->p->packed && params->p->type == initial->p->type)
                    {
                        TEMPLATEPARAMLIST* test = initial;
                        void* dflt;
                        dflt = initial->p->byClass.val;
                        if (!dflt)
                            dflt = initial->p->byClass.dflt;
                        if (dflt)
                        {
                            TEMPLATEPARAMLIST* nparam = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                            TEMPLATEPARAMLIST** p = &params->p->byPack.pack;
                            nparam->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                            while (*p)
                                p = &(*p)->next;
                            nparam->p->type = params->p->type;
                            nparam->p->byClass.val = (TYPE*)dflt;
                            if (params->p->type == kw_int)
                                nparam->p->byNonType.tp = params->p->byNonType.tp;
                            *p = nparam;
                            params->p->initialized = true;
                        }
                        initial = initial->next;
                        if (max)
                            max = max->next;
                    }
                    if (params->next)
                    {
                        params = params->next;
                    }
                    if (initial && tis.size())
                    {
                        rv = nullptr;
                        break;
                    }
                }
                else if (initial->p->type != params->p->type)
                {
                    if (initial->p->type == kw_typename && params->p->type == kw_template)
                    {
                        void* dflt;
                        dflt = initial->p->byClass.val;
                        if (!dflt)
                            dflt = initial->p->byClass.dflt;
                        if (dflt && isstructured((TYPE*)dflt))
                        {
                            if (!exactMatchOnTemplateParams(basetype((TYPE*)dflt)->sp->templateParams->next, params->p->byTemplate.args))
                            {
                                rv = nullptr;
                            }
                            else
                            {
                                params->p->byTemplate.val = ((TYPE*)dflt)->sp;
                            }
                            params->p->initialized = true;
                            params = params->next;
                            primary = primary->next;
                            initial = initial->next;
                            if (max)
                                max = max->next;
                        }
                        else
                        {
                            rv = nullptr;
                            break;
                        }
                    }
                    else
                    {
                        rv = nullptr;
                        break;
                    }
                }
                else
                {
                    void* dflt;
                    dflt = initial->p->byClass.val;
                    if (!dflt)
                        dflt = initial->p->byClass.dflt;
                    if (initial->p->type == kw_template)
                    {
                        if (dflt && !exactMatchOnTemplateParams(((SYMBOL*)dflt)->templateParams->next, params->p->byTemplate.args))
                            rv = nullptr;
                    }
                    if (params->p->byClass.val)
                    {
                        switch (initial->p->type)
                        {
                            case kw_typename:
                                if (!templatecomparetypes(params->p->byClass.val, (TYPE*)dflt, true))
                                    rv = nullptr;
                                break;
                            case kw_int:
                                //#ifndef PARSER_ONLY
                                {
                                    EXPRESSION* exp = copy_expression(params->p->byNonType.val);
                                    optimize_for_constants(&exp);
                                    if (params->p->byNonType.val && !equalTemplateIntNode(exp, (EXPRESSION*)dflt))
                                        rv = nullptr;
                                }
                                //#endif
                                break;
                            default:
                                break;
                        }
                    }
                    if (!max && params->p->byClass.dflt && params->p->byClass.dflt->type == bt_templateselector)
                    {
                        TEMPLATEPARAMLIST* next = params->next;
                        params->next = nullptr;
                        auto temp = ResolveTemplateSelectors(sp, params);
                        params->p->byClass.val = temp->p->byClass.dflt;
                        params->next = next;
                    }
                    else
                    {
                        params->p->byClass.val = (TYPE*)dflt;
                        if (spsyms)
                        {
                            if (params->p->type == kw_typename)
                            {
                                if (params->p->byClass.dflt &&
                                    !Deduce(params->p->byClass.dflt, params->p->byClass.val, true, true, false))
                                    rv = nullptr;
                                else
                                    TransferClassTemplates(params, params, nparams->next);
                            }
                            else if (params->p->type == kw_template)
                            {
                                if (params->p->byClass.dflt->type == bt_templateparam)
                                {
                                    if (!DeduceTemplateParam(params->p->byClass.dflt->templateParam, nullptr,
                                        params->p->byTemplate.dflt->tp, true))
                                        rv = nullptr;
                                }
                                else
                                {
                                    rv = nullptr;
                                }
                            }
                            else if (params->p->type == kw_int)
                            {
                                EXPRESSION* exp = params->p->byNonType.val;
                                if (exp && !isintconst(exp))
                                {
                                    exp = copy_expression(exp);
                                    optimize_for_constants(&exp);
                                }                                   
                                if (exp && params->p->byNonType.dflt && params->p->byNonType.dflt->type != en_templateparam &&
                                    !equalTemplateIntNode(params->p->byNonType.dflt, exp))
                                    rv = nullptr;
                            }
                        }
                    }
                    params->p->initialized = true;
                    params = params->next;
                    primary = primary->next;
                    initial = initial->next;
                    if (max)
                        max = max->next;
                }
            }
            if ((!initial || (params && initial->p->type != params->p->type)) && tis.size())
            {
                initial = tis.top();
                tis.pop();
            }
        }
        if (initial && (max || !spsyms))
            rv = nullptr;
        if (spsyms)
        {
            primary = params;
            while (primary)
            {
                if (primary->p->type == kw_typename)
                {
                    TEMPLATEPARAMLIST* next = primary->next;
                    primary->next = nullptr;
                    auto temp = ResolveTemplateSelectors(sp, primary);
                    if (primary->p->byClass.dflt != temp->p->byClass.dflt)
                    {
                        primary->p->byClass.val = temp->p->byClass.dflt;
                    }
                    primary->next = next;
                }
                primary = primary->next;
            }
        }
        primary = params;
        while (primary)
        {
            if (primary->p->usedAsUnpacked)
                break;
            primary = primary->next;
        }
        if ((!templateNestingCount || instantiatingTemplate || (inTemplateHeader && templateNestingCount == 1)) && (inTemplateArgs < 1 || !primary))
        {
            primary = spsyms ? spsyms : nparams->next;
            if (!TemplateParseDefaultArgs(sp, origParams, primary, primary))
                rv = nullptr;
            if (spsyms)
            {
                while (params)
                {
                    if (params->p->packed)
                    {
                        if (!params->p->byPack.pack)
                        {
                            params = params->next;
                            continue;
                        }
                        else
                        {
                            tis.push(params->next);
                            params = params->p->byPack.pack;
                        }
                    }
                    if (params->p->byClass.val && params->p->byClass.dflt)
                    {
                        switch (params->p->type)
                        {
                            case kw_typename:
                                if (params->p->byClass.dflt->type != bt_templateparam &&
                                    (!isstructured(params->p->byClass.dflt) || !basetype(params->p->byClass.dflt)->sp->templateParams) &&
                                    !templatecomparetypes(params->p->byClass.val, params->p->byClass.dflt, true))
                                    rv = nullptr;
                                break;
                            case kw_int:
                                //#ifndef PARSER_ONLY
                                {
                                    EXPRESSION* exp = copy_expression(params->p->byNonType.val);
                                    optimize_for_constants(&exp);
                                    if (params->p->byNonType.dflt && !equalTemplateIntNode(exp, params->p->byNonType.dflt))
                                        rv = nullptr;
                                }
                                //#endif
                                break;
                            default:
                                break;
                        }
                    }
                    params = params->next;
                    if (!params && !tis.empty())
                    {
                        params = tis.top();
                        tis.pop();
                    }
                }
            }
            params = args;
            while (params && primary)
            {
                if (!primary->p->byClass.val && !primary->p->packed)
                {
                    rv = nullptr;
                    break;
                }
                if (!primary->next && primary->p->packed)
                    break;
                primary = primary->next;
                params = params->next;
            }
            if (params && !primary)
            {
                rv = nullptr;
            }
        }
        else if (inTemplateSpecialization)
        {
            bool packed = false;
            params = origParams;
            while (params && args)
            {
                if (params->p->packed)
                    packed = true;
                switch (params->p->type)
                {
                    case kw_typename:
                        if (params->p->byClass.dflt && !params->p->packed && params->p->byClass.dflt->type != bt_templateparam &&
                            (!args->p->byClass.dflt || !templatecomparetypes(params->p->byClass.dflt, args->p->byClass.dflt, true)))
                            rv = nullptr;
                        break;
                    case kw_int:
                        //#ifndef PARSER_ONLY
                        {
                            if (params->p->byNonType.dflt &&
                                (!args->p->byNonType.dflt ||
                                 !templatecomparetypes(params->p->byNonType.tp, args->p->byNonType.tp, true)))
                                rv = nullptr;
                        }
                        //#endif
                        break;
                    default:
                        break;
                }
                args = args->next;
                params = params->next;
            }
            if (params)
            {
                if (params->p->packed || !params->p->byClass.txtdflt || (spsyms && params->p->byClass.dflt))
                    rv = nullptr;
            }
            else if (args && !packed)
            {
                rv = nullptr;
            }
        }
        else
        {
            bool packed = false;
            params = origParams;
            while (params && args)
            {
                if (params->p->packed)
                    packed = true;
                args = args->next;
                params = params->next;
            }
            if (params)
            {
                if (params->p->packed || !params->p->byClass.txtdflt || (spsyms && params->p->byClass.dflt))
                    rv = nullptr;
            }
            else if (args && !packed)
            {
                rv = nullptr;
            }
        }
    }
    return rv;
}
static bool checkArgType(TYPE* tp)
{
    while (ispointer(tp) || isref(tp))
        tp = basetype(tp)->btp;
    if (isfunction(tp))
    {
        SYMLIST* hr;
        SYMBOL* sym = basetype(tp)->sp;
        if (!checkArgType(basetype(tp)->btp))
            return false;
        if (sym->tp->syms)
        {
            hr = sym->tp->syms->table[0];
            while (hr)
            {
                if (!checkArgType(hr->p->tp))
                    return false;
                hr = hr->next;
            }
        }
    }
    else if (isstructured(tp))
    {
        if (basetype(tp)->sp->sb)
        {
            if (basetype(tp)->sp->sb->instantiated)
                return true;
            if (basetype(tp)->sp->sb->templateLevel)
            {
                return allTemplateArgsSpecified(basetype(tp)->sp, basetype(tp)->sp->templateParams->next);
            }
        }
    }
    else if (basetype(tp)->type == bt_templateparam)
    {
        if (!basetype(tp)->templateParam->p->byClass.val)
            return false;
    }
    else if (basetype(tp)->type == bt_templatedecltype)
        return false;
    else if (basetype(tp)->type == bt_templateselector)
    {
        return false;
    }
    else if (basetype(tp)->type == bt_memberptr)
    {
        if (!checkArgType(basetype(tp)->sp->tp))
            return false;
        if (!checkArgType(basetype(tp)->btp))
            return false;
    }
    return true;
}
static bool checkArgSpecified(TEMPLATEPARAMLIST* args)
{
    if (!args->p->byClass.val)
        return false;
    switch (args->p->type)
    {
        case kw_int:
            if (args->p->byNonType.val && !isarithmeticconst(args->p->byNonType.val))
            {
                EXPRESSION* exp = copy_expression(args->p->byNonType.val);
                while (exp->type == en_void)
                    exp = exp->right;
                optimize_for_constants(&exp);
                if (!isarithmeticconst(exp))
                {
                    if (exp && (args->p->byNonType.tp->type != bt_templateparam ||
                                (args->p->byNonType.tp->templateParam->p->byClass.val &&
                                 args->p->byNonType.tp->templateParam->p->byClass.val->type != bt_templateparam)))
                    {
                        while (castvalue(exp) || lvalue(exp))
                            exp = exp->left;
                        switch (exp->type)
                        {
                            case en_pc:
                            case en_global:
                            case en_func:
                                return true;
                            default:
                                break;
                        }
                    }
                    return false;
                }
            }
            break;
        case kw_template:
        {
            return true;
        }
        case kw_typename:
        {
            return checkArgType(args->p->byClass.val);
        }
        default:
            break;
    }
    return true;
}
bool allTemplateArgsSpecified(SYMBOL* sym, TEMPLATEPARAMLIST* args)
{
    while (args)
    {
        if (args->p->packed)
        {
            if ((templateNestingCount && !args->p->byPack.pack) || !allTemplateArgsSpecified(sym, args->p->byPack.pack))
                return false;
        }
        else
        {
            if (sym)
            {
                if (args->p->type == kw_typename)
                {
                    TYPE* tp = args->p->byClass.val;
                    if (tp && basetype(tp)->type == bt_any)
                        return false;
                    if (sameTemplate(tp, sym->tp))
                        return false;
                }
            }
            if (!checkArgSpecified(args))
            {
                return false;
            }
        }
        args = args->next;
    }

    return true;
}
void TemplateArgsAdd(TEMPLATEPARAMLIST* current, TEMPLATEPARAMLIST* dflt, TEMPLATEPARAMLIST* base)
{
    for (; base; base = base->next)
    {
        if (!base->p->byClass.val && base->argsym && dflt->argsym && !strcmp(base->argsym->name, dflt->argsym->name))
        {
            base->p->byClass.val = current->p->byClass.val;
        }
    }
}
void TemplateArgsTemplateAdd(TEMPLATEPARAMLIST* current, TEMPLATEPARAMLIST* special, TEMPLATEPARAMLIST* base)
{
    // first move the template instance
    for (auto tpb = base ; tpb; tpb = tpb->next)
    {
        if (!tpb->p->byTemplate.val && tpb->argsym && current->p->byTemplate.val && !strcmp(tpb->argsym->name, current->p->byTemplate.dflt->name))
        {
            base->p->byTemplate.val = current->p->byTemplate.val;
        }
    }
    // now move any arguments...
    auto tpl = current->p->byTemplate.args;
    auto spl = special->p->byTemplate.args;
    while (tpl && spl)
    {
        TemplateArgsAdd(tpl, spl, base);
        tpl = tpl->next;
        spl = spl->next;
    }
}
void TemplateArgsScan(TEMPLATEPARAMLIST* current, TEMPLATEPARAMLIST* base)
{
    while (current)
    {
        if (current->argsym && current->p->byClass.val)
        {
            TemplateArgsAdd(current, current, base);
        }
        if (current->p->type == kw_template)
        {
            if (current->p->byTemplate.val)
            {
                for (auto tpl = base->p->bySpecialization.types; tpl; tpl = tpl->next)
                {
                    if (tpl->p->type == kw_template && tpl->p->byTemplate.dflt && !strcmp(tpl->p->byTemplate.dflt->name, current->p->byTemplate.dflt->name))
                    {
                        TemplateArgsTemplateAdd(current, tpl, base);
                        break;
                    }
                }
            }
        }
        else if (current->p->type == kw_typename)
        {
            if (current->p->byClass.val)
                if (isstructured(current->p->byClass.dflt))
                {
                    auto tpv = basetype(current->p->byClass.dflt)->sp->templateParams;
                    for (auto tpl = basetype(current->p->byClass.val)->sp->templateParams; tpl && tpv; tpl = tpl->next, tpv = tpv->next)
                    {
                        TemplateArgsAdd(tpl, tpv, base);
                    }

                }
                else if (isstructured(current->p->byClass.val))
                {
                    for (auto tpl = basetype(current->p->byClass.val)->sp->templateParams; tpl; tpl = tpl->next)
                    {
                        TemplateArgsAdd(tpl, tpl, base);
                    }
                }
                else if (current->p->byClass.val->type == bt_templateselector)
                {
                    TemplateArgsScan(current->p->byClass.val->sp->sb->templateSelector->next->templateParams, base);
                }
        }
        current = current->next;
    }
}
void TemplateArgsCopy(TEMPLATEPARAMLIST *base)
{
    if (base->p->bySpecialization.types)
    {
        TEMPLATEPARAMLIST *p = base->next;
        while (p)
        {
            if (!p->p->byClass.val)
                break;
            p = p->next;
        }
        if (p)
        {
            TemplateArgsScan(base->p->bySpecialization.types, base);
        }
    }
}
void DuplicateTemplateParamList(TEMPLATEPARAMLIST** pptr)
{
    TEMPLATEPARAMLIST* params = *pptr;
    while (params)
    {
        *pptr = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
        if (params->p->type == kw_typename)
        {
            (*pptr)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
            *(*pptr)->p = *params->p;
            if (params->p->packed)
            {
                TEMPLATEPARAMLIST** pptr1 = &(*pptr)->p->byPack.pack;
                DuplicateTemplateParamList(pptr1);
            }
            else
            {
                (*pptr)->p->byClass.dflt = SynthesizeType(params->p->byClass.val, nullptr, false);
            }
        }
        else
        {
            (*pptr)->p = params->p;
        }
        params = params->next;
        pptr = &(*pptr)->next;
    }
}

static bool constOnly(SYMBOL** spList, SYMBOL** origList, int n)
{
    int i;
    for (i = 1; i < n; i++)
    {
        if (spList[i])
        {
            TEMPLATEPARAMLIST* tpl = origList[i]->templateParams->p->bySpecialization.types;
            while (tpl)
            {
                if (tpl->p->type == kw_typename && tpl->p->byClass.dflt)
                {
                    TYPE* tp1 = tpl->p->byClass.dflt;
                    if (isconst(tp1) || isvolatile(tp1))
                        break;
                }
                tpl = tpl->next;
            }
            if (!tpl)
                return false;
        }
    }
    return true;
}
static int SpecializationComplexity(TEMPLATEPARAMLIST* tpl)
{
    int count = 0;
    while (tpl)
    {
        if (tpl->p->type == kw_typename)
        {
            TYPE* tp = tpl->p->byClass.dflt;
            if (tpl->p->packed)
            {
                count += 1 + SpecializationComplexity(tpl->p->byPack.pack);
            }
            else
            {
                while (tp && tp->btp)
                {
                    if (tp->type != bt_typedef)
                        count++;
                    tp = tp->btp;
                }
                if (tp && isstructured(tp))
                    count += 1 + SpecializationComplexity(tp->sp->templateParams);
            }
        }
        else if (tpl->p->type == kw_int)
        {
            EXPRESSION* exp = tpl->p->byNonType.dflt;
            if (exp)
            {
                std::stack<EXPRESSION*> working;
                working.push(exp);
                while (working.size())
                {
                    exp = working.top();
                    working.pop();
                    count++;
                    if (exp->type == en_templateselector)
                    {
                        TEMPLATESELECTOR* ts = exp->v.templateSelector->next;
                        count += SpecializationComplexity(ts->templateParams);
                        while (ts)
                        {
                            count++;
                            ts = ts->next;
                        }
                    }
                    if (exp->left)
                    {
                        working.push(exp->left);
                    }
                    if (exp->right)
                    {
                        working.push(exp->right);
                    }
                }
            }
        }
        tpl = tpl->next;
    }
    return count;
}
static int MoreSpecialized(SYMBOL* left, SYMBOL* right)
{
    TEMPLATEPARAMLIST* pl = left->templateParams->p->bySpecialization.types;
    TEMPLATEPARAMLIST* pr = right->templateParams->p->bySpecialization.types;
    int il = SpecializationComplexity(pl);
    int ir = SpecializationComplexity(pr);
    if (il < ir)
        return 1;
    if (il > ir)
        return -1;
    return 0;
}
static bool LessParams(SYMBOL* left, SYMBOL* right)
{
    TEMPLATEPARAMLIST* pl = left->templateParams->p->bySpecialization.types;
    TEMPLATEPARAMLIST* pr = right->templateParams->p->bySpecialization.types;
    int il = 0;
    int ir = 0;
    while (pl)
        il++, pl = pl->next;
    while (pr)
        ir++, pr = pr->next;
    return il < ir;
}
static void ChooseMoreSpecialized(SYMBOL** list, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (list[i] && list[j])
            {
                int which = MoreSpecialized(list[i], list[j]);
                if (which < 0)
                    list[j] = nullptr;
                else if (which > 0)
                    list[i] = nullptr;
                else
                {
                    if (LessParams(list[i], list[j]))
                        list[j] = nullptr;
                    else
                        list[j] = nullptr;
                }
            }
        }
    }
}
static SYMBOL* FindTemplateSelector(TEMPLATESELECTOR* tso)
{
    if (!templateNestingCount)
    {
        SYMBOL* ts = tso->next->sp;
        SYMBOL* sp = nullptr;
        TYPE* tp;

        if (ts && ts->sb && ts->sb->instantiated)
        {
            sp = ts;
        }
        else
        {
            auto tp = ts->tp;
            if (basetype(ts->tp)->type == bt_templateparam && basetype(ts->tp)->templateParam->p->type == kw_typename)
            {
                tp = basetype(ts->tp)->templateParam->p->byClass.val;
            }
            if (!tp || !isstructured(tp))
            {
                sp = nullptr;
            }
            else
            {
                ts = basetype(tp)->sp;
                if (tso->next->isTemplate)
                {
                    if (tso->next->templateParams)
                    {
                        TEMPLATEPARAMLIST* current = tso->next->templateParams;
                        std::deque<TYPE*> types;
                        std::deque<EXPRESSION*> expressions;
                        while (current)
                        {
                            if (current->p->type == kw_typename)
                            {
                                types.push_back(current->p->byClass.dflt);
                                if (current->p->byClass.val)
                                    current->p->byClass.dflt = current->p->byClass.val;
                            }
                            else if (current->p->type == kw_int)
                            {
                                expressions.push_back(current->p->byNonType.dflt);
                                if (current->p->byNonType.val)
                                    current->p->byNonType.dflt = current->p->byNonType.val;
                            }
                            current = current->next;
                        }
                        current = tso->next->templateParams;
                        sp = GetClassTemplate(ts, current, false);
                        tp = nullptr;
                        if (sp)
                            sp = TemplateClassInstantiateInternal(sp, current, false);
                        current = tso->next->templateParams;
                        while (current)
                        {
                            if (current->p->type == kw_typename)
                            {
                                if (types.size())
                                {
                                    current->p->byClass.dflt = types.front();
                                    types.pop_front();
                                }
                            }
                            else if (current->p->type == kw_int)
                            {
                                if (expressions.size())
                                {
                                    current->p->byNonType.dflt = expressions.front();
                                    expressions.pop_front();
                                }
                            }
                            current = current->next;
                        }
                    }
                    else
                    {
                        sp = nullptr;
                    }
                }
                else if (basetype(ts->tp)->type == bt_templateselector)
                {
                    sp = nullptr;
                }
                else if (isstructured(ts->tp))
                {
                    sp = ts;
                }
            }
        }
        if (sp)
        {
            TEMPLATESELECTOR* find = tso->next->next;
            sp = basetype(PerformDeferredInitialization(sp->tp, nullptr))->sp;
            if ((sp->sb->templateLevel == 0 || sp->sb->instantiated) && (!sp->templateParams || allTemplateArgsSpecified(sp, sp->templateParams->next)))
            {
                while (find && sp)
                {
                    SYMBOL* spo = sp;
                    if (!isstructured(spo->tp))
                        break;

                    sp = search(find->name, spo->tp->syms);
                    if (!sp)
                    {
                        sp = classdata(find->name, spo, nullptr, false, false);
                        if (sp == (SYMBOL*)-1)
                            sp = nullptr;
                    }
                    find = find->next;
                }
                if (sp && !find)
                    return sp;
            }
        }
    }
    return nullptr;
}
static void FixIntSelectors(EXPRESSION** exp)
{
    if ((*exp)->left)
        FixIntSelectors(&(*exp)->left);
    if ((*exp)->right)
        FixIntSelectors(&(*exp)->right);
    if ((*exp)->type == en_templateselector)
    {
        EXPRESSION* orig = *exp;
        TEMPLATEPARAMLIST* current = (*exp)->v.templateSelector->next->templateParams;
        std::deque<TYPE*> types;
        std::deque<EXPRESSION*> expressions;
        while (current)
        {
            if (current->p->type == kw_typename)
            {
                types.push_back(current->p->byClass.dflt);
                if (current->p->byClass.val)
                    current->p->byClass.dflt = current->p->byClass.val;
            }
            else if (current->p->type == kw_int)
            {
                expressions.push_back(current->p->byNonType.dflt);
                if (current->p->byNonType.val)
                    current->p->byNonType.dflt = current->p->byNonType.val;
            }
            current = current->next;
        }
        optimize_for_constants(exp);
        current = orig->v.templateSelector->next->templateParams;
        while (current)
        {
            if (current->p->type == kw_typename)
            {
                current->p->byClass.dflt = types.front();
                types.pop_front();
            }
            else if (current->p->type == kw_int)
            {
                current->p->byNonType.dflt = expressions.front();
                expressions.pop_front();
            }
            current = current->next;
        }
        /*
        SYMBOL *sp = FindTemplateSelector((*exp)->v.templateSelector);
        if (sp && !istype(sp))
        {
            *exp = copy_expression(sp->sb->init->exp);
            FixIntSelectors(exp);
        }
        */
    }
}
static TEMPLATEPARAMLIST* ResolveTemplateSelectors(SYMBOL* sp, TEMPLATEPARAMLIST* args)
{
    TEMPLATEPARAMLIST* locate = args;
    std::stack<TEMPLATEPARAMLIST*> tls;
    while (locate)
    {

        if (locate->p->packed)
        {
            tls.push(locate->next);
            locate = locate->p->byPack.pack;
        }
        if (locate)
        {
            TYPE* tp = locate->p->byClass.dflt;
            if (locate->p->type == kw_typename && tp)
            {
                while (ispointer(tp) || isref(tp))
                    tp = basetype(tp)->btp;
                tp = basetype(tp);
                if (tp->type == bt_templateselector)
                    break;
            }
            if (locate->p->type == kw_int && tp)
            {
                if (!isintconst(locate->p->byNonType.dflt) && !isfloatconst(locate->p->byNonType.dflt))
                    break;
            }
            locate = locate->next;
        }
        if (!locate && tls.size())
        {
            locate = tls.top();
            tls.pop();
        }
    }
    if (locate)
    {
        // this flattens packed args
        TEMPLATEPARAMLIST *rv = nullptr, **last = &rv;
        locate = args;
        while (!tls.empty())
            tls.pop();
        while (locate)
        {
            if (locate->p->packed)
            {
                tls.push(locate->next);
                locate = locate->p->byPack.pack;
            }
            if (locate)
            {
                TEMPLATESELECTOR* tso = nullptr;
                TYPE* tp = locate->p->byClass.dflt;
                *last = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                if (locate->p->type == kw_typename && tp)
                {
                    while (ispointer(tp) || isref(tp))
                        tp = basetype(tp)->btp;
                    tp = basetype(tp);
                    if (tp->type == bt_templateselector)
                        tso = tp->sp->sb->templateSelector;
                    if (tso)
                    {
                        SYMBOL* sp = FindTemplateSelector(tso);
                        if (sp)
                        {
                            if (istype(sp))
                            {
                                TYPE** tx;
                                (*last)->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                                *(*last)->p = *locate->p;
                                (*last)->argsym = locate->argsym;
                                tx = &(*last)->p->byClass.dflt;
                                tp = locate->p->byClass.dflt;
                                while (tp->type != bt_templateselector)
                                {
                                    *tx = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
                                    **tx = *tp;
                                    tx = &(*tx)->btp;
                                    tp = tp->btp;
                                }
                                UpdateRootTypes(sp->tp);
                                *tx = sp->tp;
                                if (isstructured(*tx) && !templateNestingCount && basetype(*tx)->sp->sb->templateLevel &&
                                    !basetype(*tx)->sp->sb->instantiated)
                                {
                                    SYMBOL* sp1 = basetype(*tx)->sp;
                                    sp1 = GetClassTemplate(tso->next->sp, sp1->templateParams, false);
                                }
                            }
                            else
                            {
                                (*last)->p = locate->p;
                                (*last)->argsym = locate->argsym;
                            }
                        }
                        else
                        {
                            (*last)->p = locate->p;
                            (*last)->argsym = locate->argsym;
                        }
                    }
                    else
                    {
                        (*last)->p = locate->p;
                        (*last)->argsym = locate->argsym;
                    }
                }
                else if (locate->p->type == kw_int && tp)
                {
                    (*last)->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                    *(*last)->p = *locate->p;
                    (*last)->p->byNonType.dflt = copy_expression(locate->p->byNonType.dflt);
                    (*last)->argsym = locate->argsym;
                    FixIntSelectors(&(*last)->p->byNonType.dflt);
                    optimize_for_constants(&(*last)->p->byNonType.dflt);
                }
                else
                {
                    (*last)->p = locate->p;
                    (*last)->argsym = locate->argsym;
                }
                last = &(*last)->next;
                locate = locate->next;
            }
            if (!locate && tls.size())
            {
                locate = tls.top();
                tls.pop();
            }
        }
        return rv;
    }
    else
    {
        return args;
    }
}
static void copySyms(SYMBOL* found1, SYMBOL* sym)
{
    TEMPLATEPARAMLIST *src, *dest;
    src = sym->templateParams->next;
    dest = found1->templateParams->next;
    while (src && dest)
    {
        SYMBOL* hold = dest->argsym;
        TYPE *tp = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
        *tp = *src->argsym->tp;
        dest->argsym = clonesym(src->argsym);
        dest->argsym->tp = tp;
        if (hold)
        {
            dest->argsym->name = hold->name;
        }
        UpdateRootTypes(dest->argsym->tp);
        dest->argsym->tp->templateParam = dest;
        dest = dest->next;
        src = src->next;
    }
}
TEMPLATEPARAMLIST * ResolveDeclType(SYMBOL* sp, TEMPLATEPARAMLIST* args)
{
    TEMPLATEPARAMLIST *rv = args;
    if (!templateNestingCount)
    {
        auto locate = args;
        while (locate)
        {
            if (locate->p->type == kw_typename && locate->p->byClass.dflt && locate->p->byClass.dflt->type == bt_templatedecltype)
            {
                break;
            }
            locate = locate->next;
        }
        if (locate)
        {
            rv = nullptr;
            TEMPLATEPARAMLIST **last = &rv;
            locate = args;
            STRUCTSYM s;
            s.tmpl = args;
            addTemplateDeclaration(&s);
               
            while (locate)
            {
                *last = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                if (locate->p->type == kw_typename && locate->p->byClass.dflt && locate->p->byClass.dflt->type == bt_templatedecltype)
                {
                    (*last)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                    *(*last)->p = *locate->p;
                    (*last)->p->byClass.dflt = TemplateLookupTypeFromDeclType((*last)->p->byClass.dflt);
                    if (!(*last)->p->byClass.dflt)
                        (*last)->p->byClass.dflt = &stdany;
                }
                else
                {
                    (*last)->p = locate->p;
                }
                last = &(*last)->next;
                locate = locate->next;
            }
            dropStructureDeclaration();
        }
    }
    return rv;
}
SYMBOL* GetClassTemplate(SYMBOL* sp, TEMPLATEPARAMLIST* args, bool noErr)
{
    if (!strcmp(sp->name, "__all_default_constructible"))
        printf("hi");
    if (!strcmp(sp->name, "__make_tuple_types"))
        printf("hi");
    if (!strcmp(sp->name, "__make_tuple_types_flat"))
        printf("hi");
    int n = 1, i = 0;
    TEMPLATEPARAMLIST* unspecialized = sp->templateParams->next;
    SYMBOL *found1 = nullptr, *found2 = nullptr;
    SYMBOL **spList, **origList;
    TEMPLATEPARAMLIST* search = args;
    int count = 0;
    SYMLIST* l;

    noErr |= matchOverloadLevel;
    args = ResolveTemplateSelectors(sp, args);
    args = ResolveDeclType(sp, args);

    if (sp->sb->parentTemplate && sp)
        sp = sp->sb->parentTemplate;
    l = sp->sb->specializations;
    while (l)
    {
        n++;
        l = l->next;
    }
    spList = (SYMBOL**)Alloc(sizeof(SYMBOL*) * n);
    origList = (SYMBOL**)Alloc(sizeof(SYMBOL*) * n);
    origList[i++] = sp;
    l = sp->sb->specializations;
    while (i < n)
    {
        origList[i++] = l->p;
        l = l->next;
    }
    saveParams(origList, n);
    for (i = 0; i < n; i++)
    {
        spList[i] = ValidateClassTemplate(origList[i], unspecialized, args);
    }
    for (i = 0, count = 0; i < n; i++)
    {
        if (spList[i])
            count++;
    }
    if (count > 1)
    {
        int count1 = 0;
        bool match0 = !!spList[0];
        spList[0] = 0;
        for (i = 0; i < n; i++)
            if (spList[i])
                count1++;
        if (count1 > 1)
        {
            TemplatePartialOrdering(spList, n, nullptr, nullptr, true, false);
            count1 = 0;
            for (i = 0; i < n; i++)
                if (spList[i])
                   count1++;
        }
        if (count1 > 1 || (count1 == 1 && match0 && constOnly(spList, origList, n)))
        {
            for (i = 0; i < n; i++)
            {
                if ((match0 && i == 0) || spList[i])
                {
                    spList[i] = ValidateClassTemplate(origList[i], unspecialized, args);
                }
            }
            TemplateConstMatching(spList, n, args);
            count1 = 0;
            for (i = 0; i < n; i++)
                if (spList[i])
                    count1++;
        }
        if (count1 > 1)
        {
            spList[0] = 0;
            TemplateConstOrdering(spList, n, args);
            count1 = 0;
            for (i = 0; i < n; i++)
                if (spList[i])
                    count1++;
        }
        if (count1 > 1 && templateNestingCount)
        {
            // if it is going to be ambiguous but we are gathering a template, just choose the first one
            for (i = 0; i < n; i++)
                if (spList[i])
                    break;
            for (i = i + 1; i < n; i++)
                spList[i] = 0;
            count1 = 0;
            for (i = 0; i < n; i++)
                if (spList[i])
                    count1++;
        }
        if (count1 > 1)
        {
            ChooseMoreSpecialized(spList, n);
        }
    }
    for (i = 0; i < n && !found1; i++)
    {
        int j;
        found1 = spList[i];
        for (j = i + 1; j < n && found1 && !found2; j++)
        {
            if (spList[j])
            {
                found2 = spList[j];
            }
        }
    }
    if (found1 && !found2)
    {
        found1 = ValidateClassTemplate(origList[i - 1], unspecialized, args);
    }
    if (!found1 && !templateNestingCount && spList[0])
    {
        if (!noErr)
        {
            errorsym(ERR_NO_TEMPLATE_MATCHES, sp);
        }
        // might get more error info by procedeing;
        if (!sp->sb->specializations)
        {
            TEMPLATEPARAMLIST* params = sp->templateParams->next;
            while (params)
            {
                if (!params->p->byClass.val)
                    break;
                params = params->next;
            }
            if (!params)
                found1 = sp;
        }
    }
    else if (found2)
    {
        restoreParams(origList, n);
        errorsym(ERR_NO_TEMPLATE_MATCHES, sp);
        return nullptr;
    }
    if (found1 && !found2)
    {
        SYMBOL* sym = found1;
        TemplateArgsCopy(found1->templateParams);
        if (found1->sb->parentTemplate && allTemplateArgsSpecified(found1, found1->templateParams->next))
        {
            bool partialCreation = false;
            SYMBOL test = *found1;
            SYMBOL* parent = found1->sb->parentTemplate;
            TEMPLATEPARAMLIST* dflts = found1->templateParams;
            SYMLIST* instants = parent->sb->instantiations;

            while (dflts && !partialCreation)
            {
                if (dflts && dflts->p->type == kw_int && dflts->p->byNonType.val)
                    partialCreation = !isarithmeticconst(dflts->p->byNonType.val);
                dflts = dflts->next;
            }
            if (partialCreation)
            {
                test.templateParams = copyParams(test.templateParams, true);
                dflts = test.templateParams;
                while (dflts)
                {
                    if (dflts->p->type == kw_int && dflts->p->byNonType.val)
                        if (!isarithmeticconst(dflts->p->byNonType.val))
                        {
                            dflts->p->byNonType.val = copy_expression(dflts->p->byNonType.val);
                            optimize_for_constants(&dflts->p->byNonType.val);
                        }
                    dflts = dflts->next;
                }
            }
            while (instants)
            {
                if (TemplateInstantiationMatch(instants->p, &test))
                {
                    restoreParams(origList, n);
                    return instants->p;
                }
                instants = instants->next;
            }
            found1 = clonesym(&test);
            found1->sb->maintemplate = sym;
            found1->tp = (TYPE*)Alloc(sizeof(TYPE));
            *found1->tp = *sym->tp;
            UpdateRootTypes(found1->tp);
            found1->tp->sp = found1;
            found1->sb->gentemplate = true;
            found1->sb->instantiated = true;
            found1->sb->performedStructInitialization = false;
            if (!partialCreation)
                found1->templateParams = copyParams(found1->templateParams, true);
            if (found1->templateParams->p->bySpecialization.types)
            {
                TEMPLATEPARAMLIST** pptr = &found1->templateParams->p->bySpecialization.types;
                DuplicateTemplateParamList(pptr);
            }
            copySyms(found1, sym);
            SetLinkerNames(found1, lk_cdecl);
            instants = (SYMLIST*)Alloc(sizeof(SYMLIST));
            instants->p = found1;
            instants->next = parent->sb->instantiations;
            parent->sb->instantiations = instants;
        }
        else
        {
            found1 = clonesym(found1);
            found1->sb->maintemplate = sym;
            found1->tp = (TYPE*)Alloc(sizeof(TYPE));
            *found1->tp = *sym->tp;
            UpdateRootTypes(found1->tp);
            found1->tp->sp = found1;

            found1->templateParams = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
            found1->templateParams->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
            *found1->templateParams->p = *sym->templateParams->p;
            if (args)
            {
                auto next = sym->templateParams->next;
                found1->templateParams->next = args;
                while (args->next && next)
                {
                    next = next->next;
                    args = args->next;
                }
                if (next && next->next && next->next->p->byClass.txtdflt)
                    args->next = next->next;
                copySyms(found1, sym);
            }
            else
            {
                found1->templateParams->next = sym->templateParams->next;
            }
        }
    }
    restoreParams(origList, n);
    return found1;
}
SYMBOL* GetVariableTemplate(SYMBOL* sp, TEMPLATEPARAMLIST* args)
{
    // this implementation does simple variables and pointers, but not arrays/function pointers
    TEMPLATEPARAMLIST* unspecialized = sp->templateParams->next;
    SYMBOL **origList, **spList, *found1, *found2;
    SYMLIST* l = sp->sb->specializations;
    int n = 1;
    int count1, i;
    TYPE** tpi;
    while (l)
    {
        n++;
        l = l->next;
    }
    spList = (SYMBOL**)Alloc(sizeof(SYMBOL*) * n);
    origList = (SYMBOL**)Alloc(sizeof(SYMBOL*) * n);
    origList[0] = sp;
    spList[0] = ValidateClassTemplate(sp, unspecialized, args);
    tpi = &spList[0]->tp;
    while (isref(*tpi) || ispointer(*tpi))
        tpi = &basetype(*tpi)->btp;
    if (isstructured(*tpi) && basetype(*tpi)->sp->sb->templateLevel)
    {
        SYMBOL* sym = GetClassTemplate(basetype(*tpi)->sp, args, true);
        if (sym)
        {
            *tpi = TemplateClassInstantiate(sym, args, false, sc_global)->tp;
        }
    }
    l = sp->sb->specializations;
    n = 1;
    while (l)
    {
        origList[n] = (SYMBOL*)l->p;
        spList[n] = ValidateClassTemplate(origList[n], unspecialized, args);
        tpi = &spList[n]->tp;
        while (isref(*tpi) || ispointer(*tpi))
            tpi = &basetype(*tpi)->btp;
        if (isstructured(*tpi) && basetype(*tpi)->sp->sb->templateLevel)
        {
            SYMBOL* sym = GetClassTemplate(basetype(*tpi)->sp, args, true);
            if (sym)
            {
                *tpi = TemplateClassInstantiate(sym, args, false, sc_global)->tp;
            }
        }
        n++;
        l = l->next;
    }
    saveParams(spList, n);
    if (n > 1)
        TemplatePartialOrdering(spList, n, nullptr, nullptr, true, false);
    count1 = 0;
    for (i = 0; i < n; i++)
        if (spList[i])
            count1++;
    count1 = 0;
    for (i = 0; i < n; i++)
        if (spList[i])
            count1++;
    if (count1 > 1)
    {
        spList[0] = 0;
        TemplateConstOrdering(spList, n, args);
    }
    count1 = 0;
    for (i = 0; i < n; i++)
        if (spList[i])
            count1++;
    if (count1 > 1 && templateNestingCount)
    {
        // if it is going to be ambiguous but we are gathering a template, just choose the first one
        for (i = 0; i < n; i++)
            if (spList[i])
                break;
        for (i = i + 1; i < n; i++)
            spList[i] = 0;
    }
    count1 = 0;
    for (i = 0; i < n; i++)
        if (spList[i])
            count1++;
    if (count1 > 1)
        ChooseMoreSpecialized(spList, n);
    /*
    count1 = 0;
    for (i = 0; i < n; i++)
        if (spList[i])
            count1++;
    if (count1 > 1)
        ChooseShorterParamList(spList, n);
    */
    found1 = found2 = nullptr;
    for (i = 0; i < n && !found1; i++)
    {
        int j;
        found1 = spList[i];
        for (j = i + 1; j < n && found1 && !found2; j++)
        {
            if (spList[j])
            {
                found2 = spList[j];
            }
        }
    }
    if (found1 && !found2)
    {
        SYMBOL* sym = found1;
        if (found1->sb->parentTemplate && allTemplateArgsSpecified(found1, found1->templateParams->next))
        {
            bool partialCreation = false;
            SYMBOL test = *found1;
            SYMBOL* parent = found1->sb->parentTemplate;
            TEMPLATEPARAMLIST* dflts = found1->templateParams;
            SYMLIST* instants = parent->sb->instantiations;

            while (dflts && !partialCreation)
            {
                if (dflts && dflts->p->type == kw_int && dflts->p->byNonType.val)
                    partialCreation = !isarithmeticconst(dflts->p->byNonType.val);
                dflts = dflts->next;
            }
            if (partialCreation)
            {
                test.templateParams = copyParams(test.templateParams, true);
                dflts = test.templateParams;

                while (dflts)
                {
                    if (dflts->p->type == kw_int && dflts->p->byNonType.val)
                        if (!isarithmeticconst(dflts->p->byNonType.val))
                        {
                            dflts->p->byNonType.val = copy_expression(dflts->p->byNonType.val);
                            optimize_for_constants(&dflts->p->byNonType.val);
                        }
                    dflts = dflts->next;
                }
            }
            while (instants)
            {
                if (TemplateInstantiationMatch(instants->p, &test))
                {
                    return instants->p;
                }
                instants = instants->next;
            }
            found1 = clonesym(&test);
            found1->sb->maintemplate = sym;
            found1->tp = (TYPE*)Alloc(sizeof(TYPE));
            *found1->tp = *sym->tp;
            UpdateRootTypes(found1->tp);
            found1->tp->sp = found1;
            found1->sb->gentemplate = true;
            found1->sb->instantiated = true;
            if (!partialCreation)
                found1->templateParams = copyParams(found1->templateParams, true);
            if (found1->templateParams->p->bySpecialization.types)
            {
                TEMPLATEPARAMLIST** pptr = &found1->templateParams->p->bySpecialization.types;
                DuplicateTemplateParamList(pptr);
            }
            copySyms(found1, sym);
            SetLinkerNames(found1, lk_cdecl);
            instants = (SYMLIST*)Alloc(sizeof(SYMLIST));
            instants->p = found1;
            instants->next = parent->sb->instantiations;
            parent->sb->instantiations = instants;
            found1->tp = SynthesizeType(found1->tp, nullptr, false);
            if (found1->sb->init)
            {
                INITIALIZER* in = found1->sb->init;
                INITIALIZER *p = nullptr, **out = &p, ***outptr = &out;
                RecalculateVariableTemplateInitializers(&in, outptr, found1->tp, 0);
                found1->sb->init = p;
            }
            found1->sb->attribs.inheritable.linkage = lk_virtual;
            InsertInlineData(found1);
        }
        else
        {
            found1 = clonesym(found1);
            found1->sb->maintemplate = sym;
            found1->tp = (TYPE*)Alloc(sizeof(TYPE));
            *found1->tp = *sym->tp;
            UpdateRootTypes(found1->tp);
            found1->tp->sp = found1;

            found1->templateParams = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
            found1->templateParams->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
            *found1->templateParams->p = *sym->templateParams->p;
            if (args)
            {
                found1->templateParams->next = args;
                copySyms(found1, sym);
            }
            else
            {
                found1->templateParams->next = sym->templateParams->next;
            }
            found1->tp = &stdint;
        }
    }
    restoreParams(spList, n);
    return found1;
}
static TYPE* SpecifyArgType(SYMBOL* sym, TYPE* tp, TEMPLATEPARAMLIST *current, TEMPLATEPARAMLIST* inargs, TEMPLATEPARAMLIST* orig)
{
    TYPE *rv = nullptr, **last = &rv;
    while (tp)
    {
        *last = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
        **last = *tp;
        tp = tp->btp;
        last = &(*last)->btp;
    }
    UpdateRootTypes(rv);
    tp = rv;
    while (ispointer(tp) || isref(tp))
        tp = basetype(tp)->btp;
    if (isfunction(tp))
    {
        SYMLIST* hr;
        SYMBOL* sym = basetype(tp)->sp = clonesym(basetype(tp)->sp);
        basetype(tp)->btp = SpecifyArgType(sym, basetype(tp)->btp, current, inargs, nullptr);
        if (sym->tp->syms)
        {
            hr = sym->tp->syms->table[0];
            while (hr)
            {
                hr->p->tp = SpecifyArgType(sym, hr->p->tp, current, inargs, nullptr);
                hr = hr->next;
            }
        }
    }
    else if (isstructured(tp))
    {
        if (basetype(tp)->sp->sb->templateLevel)
        {
            if (!orig)
                orig = basetype(tp)->sp->templateParams;
            else
                orig = orig->p->byClass.dflt->sp->templateParams;
            auto args1 = GetUsingArgs(basetype(tp)->sp, inargs, orig->next);
            auto tpl = args1;
            std::stack<TEMPLATEPARAMLIST*> stk;
            while (tpl)
            {
                if (tpl->p->packed)
                {
                    if (tpl->p->byPack.pack)
                    {
                        stk.push(tpl->next);
                        tpl = tpl->p->byPack.pack;
                    }
                    else
                    {
                        tpl = tpl->next;
                    }
                    continue;
                }
                tpl->p->byClass.dflt = tpl->p->byClass.val;
                tpl = tpl->next;
                while (!tpl && !stk.empty())
                {
                    tpl = stk.top();
                    stk.pop();
                }
            }
            SYMBOL *sym = GetClassTemplate(basetype(tp)->sp, args1, true);
            if (sym)
                basetype(tp)->sp = sym;
        }
    }
    else if (basetype(tp)->type == bt_templateparam)
    {
    }
    else if (basetype(tp)->type == bt_templatedecltype)
    {
    }
    else if (basetype(tp)->type == bt_templateselector)
    {
    }
    else if (basetype(tp)->type == bt_memberptr)
    {
        basetype(tp)->sp = clonesym(basetype(tp)->sp);
        basetype(tp)->sp->tp = SpecifyArgType(sym, basetype(tp)->sp->tp, current, inargs, nullptr);
        basetype(tp)->btp = SpecifyArgType(sym, basetype(tp)->btp, current, inargs, nullptr);
    }
    return rv;
}
static void SpecifyArg(SYMBOL* sym, TEMPLATEPARAMLIST* current, TEMPLATEPARAMLIST* args, TEMPLATEPARAMLIST* orig)
{
    bool playout = false;
    if (current->p->packed)
    {
        playout = true;
        current = current->p->byPack.pack;
    }
    if (current)
    {
        do
        {
            switch (current->p->type)
            {
            case kw_int:
                break;
            case kw_template:
            {
                current->p->byTemplate.args = GetUsingArgs(sym, args, orig->p->byTemplate.args);
            }
            case kw_typename:
            {
                current->p->byClass.val = SpecifyArgType(sym, current->p->byClass.val, current, args, orig);
            }
            default:
                break;
            }
            current = current->next;
        } while (playout && current);
    }
}
static EXPRESSION* SpecifyReplaceInt(EXPRESSION* orig, EXPRESSION* subs, const char* name)
{
    if (orig->left)
    {
        orig->left = SpecifyReplaceInt(orig->left, subs, name);
    }
    if (orig->right)
    {
        orig->right = SpecifyReplaceInt(orig->right, subs, name);
    }
    if (orig->type == en_auto)
    {
        if (!strcmp(name, orig->v.sp->name))
        {
            return subs;
        }
    }
    return orig;
}
static TEMPLATEPARAMLIST* SpecifyPackedInt(TEMPLATEPARAMLIST* arg, TEMPLATEPARAMLIST* orig, TEMPLATEPARAMLIST* inargs)
{
    if (orig->p->byPack.pack && inargs)
    {
        if (orig->p->type == kw_int && orig->p->packed)
        {
            TEMPLATEPARAM temp = *(arg->p);
            arg->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
            *(arg->p) = temp;
            TEMPLATEPARAMLIST** x = &arg->p->byPack.pack;
            TEMPLATEPARAMLIST* old = *x;
            *x = nullptr;

            EXPRESSION* exp = orig->p->byPack.pack->p->byNonType.val;
            for (TEMPLATEPARAMLIST* test = inargs->p->byPack.pack; test; test = test->next)
            {
                *x = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                (*x)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                *((*x)->p) = *(old->p);
                TEMPLATEPARAMLIST* temp = (*x);
                x = &(*x)->next;
                temp->p->byNonType.val = SpecifyReplaceInt(copy_expression(exp), test->p->byNonType.val, inargs->argsym->name);
                optimize_for_constants(&temp->p->byNonType.val);
                while (temp->p->byNonType.val->type == en_void && temp->p->byNonType.val->right)
                    temp->p->byNonType.val = temp->p->byNonType.val->right;
            }
        }
    }
    return arg;
}
static TEMPLATEPARAMLIST* GetUsingArgs(SYMBOL* sp, TEMPLATEPARAMLIST* find, TEMPLATEPARAMLIST* orig)
{
    TEMPLATEPARAMLIST *args1 = nullptr, **last = &args1;
    TEMPLATEPARAMLIST* temp = orig;
    while (temp)
    {
        *last = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
        (*last)->argsym = temp->argsym;
        (*last)->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
        *(*last)->p = *(temp->p);
        if (!(*last)->p->packed)
        {
            (*last)->p->byClass.val = temp->p->byClass.dflt;
        }
        if ((*last)->argsym)
        {
            for (TEMPLATEPARAMLIST* test = find->next; test; test = test->next)
            {
                if (test->argsym && !strcmp(test->argsym->name, (*last)->argsym->name))
                {
                    if ((*last)->p->packed)
                        (*last)->p->byPack.pack = test->p->byPack.pack;
                    else
                        (*last)->p->byClass.val = test->p->byClass.dflt;
                    (*last)->argsym = test->argsym;
                }
                else switch ((*last)->p->type)
                {
                case kw_int:
                    if ((*last)->p->byNonType.val && (*last)->p->byNonType.val->type == en_templateparam)
                    {
                        if (test->argsym && !strcmp(test->argsym->name, (*last)->p->byNonType.val->v.sp->name))
                        {
                            if ((*last)->p->packed)
                                (*last)->p->byPack.pack = test->p->byPack.pack;
                            else
                                (*last)->p->byNonType.val = test->p->byNonType.dflt;
                        }
                    }
                    break;
                }
            }
        }
        temp = temp->next;
        last = &(*last)->next;
    }
    TEMPLATEPARAMLIST* x = orig;
    TEMPLATEPARAMLIST* f = find->next;
    for (TEMPLATEPARAMLIST* temp = args1; temp && x && f; temp = temp->next, x = x->next, f = f->next)
    {
        SpecifyPackedInt(temp, x, f);
    }
    x = orig = orig;
    for (TEMPLATEPARAMLIST* temp = args1; temp && x; temp = temp->next, x = x->next)
    {
        SpecifyArg(sp, temp, find, x);
    }
    args1 = ResolveTemplateSelectors(sp, args1);
    args1 = ResolveDeclType(sp, args1);
    return args1;
}
SYMBOL* GetTypedefSpecialization(SYMBOL* sp, TEMPLATEPARAMLIST* args, bool nested)
{
    if (!strcmp(sp->name, "_Maker"))
        printf("hi");
    SYMBOL* found1 = sp;
    TYPE** tpi;
    args = ResolveTemplateSelectors(sp, args);
    found1 = ValidateClassTemplate(sp, sp->templateParams->next, args);
    if (!found1)
        return sp;
    found1 = clonesymfalse(found1);
    tpi = &found1->tp;
    if ((!templateNestingCount || instantiatingTemplate) && allTemplateArgsSpecified(found1, found1->templateParams->next))
    {
        if ((*tpi)->type == bt_typedef)
        {
            TYPE* tp1 = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
            *tp1 = *(*tpi);
            *tpi = tp1;
            tpi = &(*tpi)->btp;
        }
        if ((*tpi)->type == bt_typedef && (*tpi)->sp->sb->templateLevel)
        {
            std::deque<TYPE*> defaults;
            auto tpl = (*tpi)->sp->templateParams->next;
            PushPopDefaults(defaults, tpl, false, true);
            while (tpl)
            {
                if (tpl->p->type == kw_int && tpl->p->byNonType.val)
                    optimize_for_constants(&tpl->p->byNonType.val);
                tpl->p->byClass.dflt = tpl->p->byClass.val;
                tpl = tpl->next;
            }
            auto args1 = GetUsingArgs((*tpi)->sp, found1->templateParams, (*tpi)->sp->templateParams->next);
            auto rv = GetTypedefSpecialization(sp->tp->btp->sp, args1, true);
            tpl = (*tpi)->sp->templateParams->next;
            PushPopDefaults(defaults, tpl, false, false);
            return rv;
        }
        while (isref(*tpi) || ispointer(*tpi))
        {
            TYPE* tp1 = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
            *tp1 = *(*tpi);
            *tpi = tp1;
            tpi = &basetype(*tpi)->btp;
        }
        if (isstructured(*tpi) && basetype(*tpi)->sp->sb->templateLevel)
        {
            std::deque<TYPE*> defaults;
            auto tpl = (*tpi)->sp->templateParams->next;
            PushPopDefaults(defaults, tpl, false, true);
            SYMBOL* sym = basetype(*tpi)->sp;
            if (!nested)
            {
                tpl = basetype(*tpi)->sp->templateParams;
                while (tpl)
                {
                    if (tpl->p->byClass.val)
                        tpl->p->byClass.dflt = tpl->p->byClass.val;
                    tpl = tpl->next;
                }
            }
            TEMPLATEPARAMLIST *args1 = GetUsingArgs(basetype(*tpi)->sp, args, basetype(*tpi)->sp->templateParams);
            tpl = args1;
            while (tpl)
            {
                if (tpl->p->type == kw_int)
                    optimize_for_constants(&tpl->p->byNonType.val);
                tpl->p->byClass.dflt = tpl->p->byClass.val;
                tpl = tpl->next;
            }
            sym = GetClassTemplate(sym, args1->next, true);
            tpl = (*tpi)->sp->templateParams->next;
            if (sym)
            {
                TYPE* tp1 = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
                *tp1 = *(*tpi);
                *tpi = tp1;
                (*tpi)->sp = TemplateClassInstantiateInternal(sym, args1->next, false);
            }
            PushPopDefaults(defaults, tpl, false, false);
        }
    }
    if (allTemplateArgsSpecified(found1, found1->templateParams->next))
    {
        bool partialCreation = false;
        SYMBOL test = *found1;
        TEMPLATEPARAMLIST* dflts = found1->templateParams;

        while (dflts && !partialCreation)
        {
            if (dflts && dflts->p->type == kw_int && dflts->p->byNonType.val)
                partialCreation = !isarithmeticconst(dflts->p->byNonType.val);
            dflts = dflts->next;
        }
        if (partialCreation)
        {
            test.templateParams = copyParams(test.templateParams, true);
            dflts = test.templateParams;

            std::stack<TEMPLATEPARAMLIST*> tds;
            while (dflts)
            {
                if (dflts->p->packed)
                {
                    tds.push(dflts->next);
                    dflts = dflts->p->byPack.pack;
                }
                if (dflts)
                {
                    if (dflts->p->type == kw_int && dflts->p->byNonType.val)
                        if (!isarithmeticconst(dflts->p->byNonType.val))
                        {
                            dflts->p->byNonType.val = copy_expression(dflts->p->byNonType.val);
                            optimize_for_constants(&dflts->p->byNonType.val);
                        }
                    dflts = dflts->next;
                }
                if (!dflts && tds.size())
                {
                    dflts = tds.top();
                    tds.pop();
                }
            }
        }
        found1 = clonesym(&test);
        found1->sb->maintemplate = test.sb->maintemplate;
        found1->tp = (TYPE*)Alloc(sizeof(TYPE));
        *found1->tp = *test.tp;
        UpdateRootTypes(found1->tp);
        found1->sb->gentemplate = true;
        found1->sb->instantiated = true;
        if (!partialCreation)
            found1->templateParams = copyParams(found1->templateParams, true);
        if (found1->templateParams->p->bySpecialization.types)
        {
            TEMPLATEPARAMLIST** pptr = &found1->templateParams->p->bySpecialization.types;
            DuplicateTemplateParamList(pptr);
        }
        copySyms(found1, sp);
        SetLinkerNames(found1, lk_cdecl);
    }
    else
    {
        found1 = clonesym(found1);
        found1->sb->maintemplate = sp;
        found1->tp = (TYPE*)Alloc(sizeof(TYPE));
        *found1->tp = *sp->tp;
        UpdateRootTypes(found1->tp);

        found1->sb->mainsym = sp;
        found1->templateParams = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
        found1->templateParams->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
        *found1->templateParams->p = *sp->templateParams->p;
        if (args)
        {
            found1->templateParams->next = args;
            copySyms(found1, sp);
        }
        else
        {
            found1->templateParams->next = sp->templateParams->next;
        }
    }

    return found1;
}
void DoInstantiateTemplateFunction(TYPE* tp, SYMBOL** sp, NAMESPACEVALUELIST* nsv, SYMBOL* strSym,
                                   TEMPLATEPARAMLIST* templateParams, bool isExtern)
{
    SYMBOL* sym = *sp;
    SYMBOL *spi = nullptr, *ssp;
    SYMLIST** p = nullptr;
    if (nsv)
    {
        Optimizer::LIST* rvl = tablesearchone(sym->name, nsv, false);
        if (rvl)
            spi = (SYMBOL*)rvl->data;
        else
            errorNotMember(strSym, nsv, sym->name);
    }
    else
    {
        ssp = getStructureDeclaration();
        if (ssp)
            p = LookupName(sym->name, ssp->tp->syms);
        if (!p)
            p = LookupName(sym->name, globalNameSpace->valueData->syms);
        if (p)
        {
            spi = (SYMBOL*)(*p)->p;
        }
    }
    if (spi)
    {
        if (spi->sb->storage_class == sc_overloads)
        {
            FUNCTIONCALL* funcparams = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
            SYMBOL* instance;
            SYMLIST* hr = basetype(tp)->syms->table[0];
            INITLIST** init = &funcparams->arguments;
            funcparams->templateParams = templateParams->p->bySpecialization.types;
            funcparams->ascall = true;
            if (templateParams->p->bySpecialization.types)
                funcparams->astemplate = true;
            if (hr->p->sb->thisPtr)
                hr = hr->next;
            while (hr)
            {
                *init = (INITLIST*)Alloc(sizeof(INITLIST));
                (*init)->tp = hr->p->tp;
                (*init)->exp = intNode(en_c_i, 0);
                init = &(*init)->next;
                hr = hr->next;
            }
            if (spi->sb->parentClass)
            {
                TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
                tp->type = bt_pointer;
                tp->size = getSize(bt_pointer);
                tp->btp = spi->sb->parentClass->tp;
                tp->rootType = tp;
                funcparams->thistp = tp;
                funcparams->thisptr = intNode(en_c_i, 0);
            }
            instance = GetOverloadedTemplate(spi, funcparams);
            if (instance)
            {
                if (instance->sb->templateLevel)
                    instance = TemplateFunctionInstantiate(instance, true, isExtern);

                *sp = instance;
            }
        }
        else
        {
            errorsym(ERR_NOT_A_TEMPLATE, sym);
        }
    }
}
static void referenceInstanceMembers(SYMBOL* cls)
{
    if (Optimizer::cparams.prm_xcept)
        RTTIDumpType(cls->tp);
    if (cls->sb->vtabsp)
    {
        Optimizer::SymbolManager::Get(cls->sb->vtabsp);
    }
    if (cls->tp->syms)
    {
        SYMLIST* hr = cls->tp->syms->table[0];
        BASECLASS* lst;
        while (hr)
        {
            SYMBOL* sym = hr->p;
            if (sym->sb->storage_class == sc_overloads)
            {
                SYMLIST* hr2 = sym->tp->syms->table[0];
                while (hr2)
                {
                    sym = (SYMBOL*)hr2->p;
                    if (sym->sb->templateLevel <= cls->sb->templateLevel)
                    {
                        if (sym->sb->deferredCompile && !sym->sb->inlineFunc.stmt)
                        {
                            deferredCompileOne(sym);
                        }
                        InsertInline(sym);
                        Optimizer::SymbolManager::Get(sym)->genreffed = true;
                    }
                    hr2 = hr2->next;
                }
            }
            else if (!ismember(sym) && !istype(sym))
            {
                if (cls->sb->templateLevel || sym->sb->templateLevel)
                    InsertInlineData(sym);
                Optimizer::SymbolManager::Get(sym)->genreffed = true;
            }
            hr = hr->next;
        }
        hr = cls->tp->tags->table[0]->next;  // past the definition of self
        while (hr)
        {
            SYMBOL* sym = hr->p;
            if (isstructured(sym->tp))
                referenceInstanceMembers(sym);
            hr = hr->next;
        }
        lst = cls->sb->baseClasses;
        while (lst)
        {
            if (lst->cls->sb->templateLevel)
            {
                referenceInstanceMembers(lst->cls);
            }
            lst = lst->next;
        }
    }
}

static bool fullySpecialized(TEMPLATEPARAMLIST* tpl)
{
    switch (tpl->p->type)
    {
        case kw_typename:
            return !typeHasTemplateArg(tpl->p->byClass.dflt);
        case kw_template:
            tpl = tpl->p->byTemplate.args;
            while (tpl)
            {
                if (!fullySpecialized(tpl))
                    return false;
                tpl = tpl->next;
            }
            return true;
        case kw_int:
            if (!tpl->p->byNonType.dflt)
                return false;
            if (!isarithmeticconst(tpl->p->byNonType.dflt))
            {
                EXPRESSION* exp = tpl->p->byNonType.dflt;
                if (exp && tpl->p->byNonType.tp->type != bt_templateparam)
                {
                    while (castvalue(exp) || lvalue(exp))
                        exp = exp->left;
                    switch (exp->type)
                    {
                        case en_pc:
                        case en_global:
                        case en_func:
                            return true;
                        default:
                            break;
                    }
                }
                return false;
            }
            else
            {
                return true;
            }
            break;
        default:
            return false;
    }
}
bool TemplateFullySpecialized(SYMBOL* sp)
{
    if (sp)
    {
        if (sp->templateParams && sp->templateParams->p->bySpecialization.types)
        {
            TEMPLATEPARAMLIST* tpl = sp->templateParams->p->bySpecialization.types;
            while (tpl)
            {
                if (!fullySpecialized(tpl))
                    return false;
                tpl = tpl->next;
            }
            return true;
        }
    }
    return false;
}

void propagateTemplateDefinition(SYMBOL* sym)
{
    int oldCount = templateNestingCount;
    struct templateListData* oldList = currents;
    templateNestingCount = 0;
    currents = nullptr;
    if (!sym->sb->deferredCompile && !sym->sb->inlineFunc.stmt)
    {
        SYMBOL* parent = sym->sb->parentClass;
        if (parent)
        {
            SYMBOL* old = parent->sb->maintemplate;
            if (!old)
                old = parent;
            if (old && old->tp->syms)
            {
                SYMLIST** p = LookupName(sym->name, old->tp->syms);
                if (p)
                {
                    SYMLIST* hr;
                    hr = basetype(((SYMBOL*)(*p)->p)->tp)->syms->table[0];
                    while (hr)
                    {
                        SYMBOL* cur = hr->p;
                        //                        if (sym->sb->maintemplate && !strcmp(sym->sb->maintemplate->sb->decoratedName,
                        //                        cur->sb->decoratedName) && cur->sb->parentClass && cur->sb->deferredCompile)// &&
                        //                        matchTemplateFunc(cur, sym))
                        if (sym && sym->sb->origdeclline == cur->sb->origdeclline &&
                            !strcmp(sym->sb->origdeclfile, cur->sb->origdeclfile) && cur->sb->deferredCompile)
                        {
                            if (matchesCopy(cur, false) == matchesCopy(sym, false) &&
                                matchesCopy(cur, true) == matchesCopy(sym, true))
                            {
                                sym->sb->deferredCompile = cur->sb->deferredCompile;
                                sym->sb->memberInitializers = cur->sb->memberInitializers;
                                sym->sb->pushedTemplateSpecializationDefinition = 1;
                                if (basetype(sym->tp)->syms && basetype(cur->tp)->syms)
                                {
                                    SYMLIST* src = basetype(cur->tp)->syms->table[0];
                                    SYMLIST* dest = basetype(sym->tp)->syms->table[0];
                                    while (src && dest)
                                    {
                                        dest->p->name = src->p->name;
                                        src = src->next;
                                        dest = dest->next;
                                    }
                                }
                                {
                                    STRUCTSYM t, s, r;
                                    SYMBOL* thsprospect = (SYMBOL*)basetype(sym->tp)->syms->table[0]->p;
                                    t.tmpl = nullptr;
                                    r.tmpl = nullptr;
                                    if (cur->templateParams)
                                    {
                                        r.tmpl = cur->templateParams;
                                        addTemplateDeclaration(&r);
                                    }
                                    if (thsprospect && thsprospect->sb->thisPtr)
                                    {
                                        SYMBOL* spt = basetype(basetype(thsprospect->tp)->btp)->sp;
                                        t.tmpl = spt->templateParams;
                                        if (t.tmpl)
                                            addTemplateDeclaration(&t);
                                    }
                                    s.str = sym->sb->parentClass;
                                    addStructureDeclaration(&s);
                                    deferredCompileOne(sym);
                                    dropStructureDeclaration();
                                    if (r.tmpl)
                                        dropStructureDeclaration();
                                    if (t.tmpl)
                                        dropStructureDeclaration();
                                }
                            }
                        }
                        hr = hr->next;
                    }
                }
            }
        }
        else
        {
            SYMBOL* old;
            int tns = PushTemplateNamespace(sym);
            old = namespacesearch(sym->name, globalNameSpace, false, false);
            if (old)
            {
                SYMLIST* hr;
                hr = basetype(old->tp)->syms->table[0];
                while (hr)
                {
                    SYMBOL* cur = hr->p;
                    if (sym && sym->sb->declline == cur->sb->declline && !strcmp(sym->sb->declfile, cur->sb->declfile) &&
                        cur->sb->deferredCompile)
                    //                    if (cur->sb->templateLevel && cur->sb->deferredCompile && matchTemplateFunc(cur, sym))
                    {
                        sym->sb->deferredCompile = cur->sb->deferredCompile;
                        cur->sb->pushedTemplateSpecializationDefinition = 1;
                        if (basetype(sym->tp)->syms && basetype(cur->tp)->syms)
                        {
                            SYMLIST* src = basetype(cur->tp)->syms->table[0];
                            SYMLIST* dest = basetype(sym->tp)->syms->table[0];
                            while (src && dest)
                            {
                                dest->p->name = src->p->name;
                                src = src->next;
                                dest = dest->next;
                            }
                        }
                        {
                            STRUCTSYM t;
                            SYMBOL* thsprospect = (SYMBOL*)basetype(sym->tp)->syms->table[0]->p;
                            t.tmpl = nullptr;
                            if (thsprospect && thsprospect->sb->thisPtr)
                            {
                                SYMBOL* spt = basetype(basetype(thsprospect->tp)->btp)->sp;
                                t.tmpl = spt->templateParams;
                                if (t.tmpl)
                                    addTemplateDeclaration(&t);
                            }
                            //                                    TemplateFunctionInstantiate(cur, false, false);
                            deferredCompileOne(sym);
                            if (t.tmpl)
                                dropStructureDeclaration();
                        }
                    }
                    hr = hr->next;
                }
            }
            PopTemplateNamespace(tns);
        }
    }
    else if (sym->sb->deferredCompile && !sym->sb->inlineFunc.stmt)
    {
        int n = PushTemplateNamespace(sym);
        if (sym->sb->parentClass)
            SwapMainTemplateArgs(sym->sb->parentClass);
        STRUCTSYM t, s, r;
        SYMBOL* thsprospect = (SYMBOL*)basetype(sym->tp)->syms->table[0]->p;
        t.tmpl = nullptr;
        r.tmpl = nullptr;
        if (sym->templateParams)
        {
            r.tmpl = sym->templateParams;
            addTemplateDeclaration(&r);
        }
        if (thsprospect && thsprospect->sb->thisPtr)
        {
            SYMBOL* spt = basetype(basetype(thsprospect->tp)->btp)->sp;
            t.tmpl = spt->templateParams;
            if (t.tmpl)
                addTemplateDeclaration(&t);
        }
        s.str = sym->sb->parentClass;
        addStructureDeclaration(&s);
        deferredCompileOne(sym);
        dropStructureDeclaration();
        if (r.tmpl)
            dropStructureDeclaration();
        if (t.tmpl)
            dropStructureDeclaration();
        if (sym->sb->parentClass)
            SwapMainTemplateArgs(sym->sb->parentClass);
        PopTemplateNamespace(n);
    }
    currents = oldList;
    templateNestingCount = oldCount;
}
static void MarkDllLinkage(SYMBOL* sp, enum e_lk linkage)
{
    if (linkage != lk_none && sp->sb->attribs.inheritable.linkage2 != linkage)
    {
        if (sp->sb->attribs.inheritable.linkage2 != lk_none)
        {
            errorsym(ERR_ATTEMPING_TO_REDEFINE_DLL_LINKAGE, sp);
        }
        else
        {
            sp->sb->attribs.inheritable.linkage2 = linkage;
            Optimizer::SymbolManager::Get(sp)->isexport = linkage == lk_export;
            Optimizer::SymbolManager::Get(sp)->isimport = linkage == lk_import;
            if (sp->sb->vtabsp)
            {
                sp->sb->vtabsp->sb->attribs.inheritable.linkage2 = linkage;
                if (sp->sb->vtabsp->sb->symRef)
                {
                    Optimizer::SymbolManager::Get(sp->sb->vtabsp)->isexport = linkage == lk_export;
                    Optimizer::SymbolManager::Get(sp->sb->vtabsp)->isimport = linkage == lk_import;
                }
                if (sp->sb->vtabsp->sb->attribs.inheritable.linkage2 == lk_import)
                {
                    sp->sb->vtabsp->sb->dontinstantiate = true;
                    Optimizer::SymbolManager::Get(sp->sb->vtabsp)->dontinstantiate = true;
                }
            }
            if (sp->tp->syms)
            {
                SYMLIST* hr = sp->tp->syms->table[0];
                while (hr)
                {
                    SYMBOL* sym = hr->p;
                    if (sym->sb->storage_class == sc_overloads)
                    {
                        SYMLIST* hr2 = sym->tp->syms->table[0];
                        while (hr2)
                        {
                            if (!(hr2->p)->templateParams)
                            {
                                (hr2->p)->sb->attribs.inheritable.linkage2 = linkage;
                                (hr2->p)->sb->attribs.inheritable.isInline = false;
                                Optimizer::SymbolManager::Get(hr2->p)->isexport = linkage == lk_export;
                                Optimizer::SymbolManager::Get(hr2->p)->isimport = linkage == lk_import;
                            }
                            hr2 = hr2->next;
                        }
                    }
                    else if (!ismember(sym) && !istype(sym))
                    {
                        sym->sb->attribs.inheritable.linkage2 = linkage;
                        Optimizer::SymbolManager::Get(sym)->isexport = linkage == lk_export;
                        Optimizer::SymbolManager::Get(sym)->isimport = linkage == lk_import;
                    }
                    hr = hr->next;
                }
            }
        }
    }
}
static void DoInstantiate(SYMBOL* strSym, SYMBOL* sym, TYPE* tp, NAMESPACEVALUELIST* nsv, bool isExtern)
{
    STRUCTSYM s;
    if (strSym)
    {
        s.str = strSym;
        addStructureDeclaration(&s);
    }
    if (isfunction(tp))
    {
        SYMBOL* sp = sym;
        TEMPLATEPARAMLIST* templateParams = TemplateGetParams(sym->sb->parentClass);
        DoInstantiateTemplateFunction(tp, &sp, nsv, strSym, templateParams, isExtern);
        sp->sb->attribs.inheritable.linkage2 = sym->sb->attribs.inheritable.linkage2;
        sym = sp;
        sym->sb->parentClass = strSym;
        SetLinkerNames(sym, lk_cdecl);
        if (!comparetypes(basetype(sp->tp)->btp, basetype(tp)->btp, true))
        {
            errorsym(ERR_TYPE_MISMATCH_IN_REDECLARATION, sp);
        }
        Optimizer::SymbolManager::Get(sp)->genreffed = true;
        if (isExtern)
        {
            sp->sb->dontinstantiate = true;
        }
        else
        {
            sp->sb->dontinstantiate = false;
            InsertInline(sp);
        }
    }
    else
    {
        SYMBOL *spi = nullptr, *ssp;
        SYMLIST** p = nullptr;
        if (nsv)
        {
            Optimizer::LIST* rvl = tablesearchone(sym->name, nsv, false);
            if (rvl)
                spi = (SYMBOL*)rvl->data;
            else
                errorNotMember(strSym, nsv, sym->name);
        }
        else
        {
            ssp = getStructureDeclaration();
            if (ssp)
                p = LookupName(sym->name, ssp->tp->syms);
            else
                p = LookupName(sym->name, globalNameSpace->valueData->syms);
            if (p)
            {
                spi = (SYMBOL*)(*p)->p;
            }
        }
        if (spi)
        {
            SYMBOL* tmpl = spi;
            while (tmpl)
                if (tmpl->sb->templateLevel)
                    break;
                else
                    tmpl = tmpl->sb->parentClass;
            if ((tmpl && spi->sb->storage_class == sc_static) || spi->sb->storage_class == sc_external)
            {
                TemplateDataInstantiate(spi, true, isExtern);
                spi->sb->dontinstantiate = isExtern;
                Optimizer::SymbolManager::Get(spi)->genreffed = true;
                if (!comparetypes(sym->tp, spi->tp, true))
                    preverrorsym(ERR_TYPE_MISMATCH_IN_REDECLARATION, spi, sym->sb->declfile, sym->sb->declline);
            }
            else
            {
                errorsym(ERR_NOT_A_TEMPLATE, sym);
            }
        }
        else
        {
            errorsym(ERR_NOT_A_TEMPLATE, sym);
        }
    }
    if (strSym)
    {
        dropStructureDeclaration();
    }
}
LEXEME* TemplateDeclaration(LEXEME* lex, SYMBOL* funcsp, enum e_ac access, enum e_sc storage_class, bool isExtern)
{
    HASHTABLE* oldSyms = localNameSpace->valueData->syms;
    lex = getsym();
    localNameSpace->valueData->syms = nullptr;
    if (MATCHKW(lex, lt))
    {
        int lasttemplateHeaderCount = templateHeaderCount;
        TYPE* tp = nullptr;
        struct templateListData l;
        int count = 0;
        int oldInstantiatingTemplate = instantiatingTemplate;
        lex = backupsym();
        if (isExtern)
            error(ERR_DECLARE_SYNTAX);

        if (templateNestingCount == 0)
        {
            l.args = nullptr;
            l.ptail = &l.args;
            l.sp = nullptr;
            l.head = l.tail = nullptr;
            l.bodyHead = l.bodyTail = nullptr;
            currents = &l;
        }
        currents->plast = currents->ptail;
        templateNestingCount++;
        instantiatingTemplate = 0;
        while (MATCHKW(lex, kw_template))
        {
            TEMPLATEPARAMLIST* temp;
            templateHeaderCount++;
            temp = (*currents->ptail) = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
            temp->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
            temp->p->type = kw_new;
            lex = getsym();
            currents->ptail = &(*currents->ptail)->p->bySpecialization.next;

            lex = TemplateHeader(lex, funcsp, &(temp->next));
            if (temp->next)
            {
                count++;
            }
        }
        templateNestingCount--;
        if (lex)
        {
            templateNestingCount++;
            inTemplateType = true;
            lex = declare(lex, funcsp, &tp, storage_class, lk_none, nullptr, true, false, true, access);
            inTemplateType = false;
            templateNestingCount--;
            instantiatingTemplate = oldInstantiatingTemplate;
            if (!templateNestingCount)
            {
                if (!tp)
                {
                    error(ERR_TEMPLATES_MUST_BE_CLASSES_OR_FUNCTIONS);
                }
                if (l.sp)
                {
                    if (l.sp && isfunction(l.sp->tp) && l.sp->sb->parentClass && !l.sp->sb->deferredCompile)
                    {
                        SYMBOL* srch = l.sp->sb->parentClass;
                        while (srch)
                        {
                            if (srch->sb->deferredCompile)
                                break;
                            srch = srch->sb->parentClass;
                        }
                        if (srch)
                        {
                            TEMPLATEPARAMLIST** srch1 = currents->plast;
                            while (srch1 && srch1 != currents->ptail)
                            {
                                if ((*srch1)->next != nullptr)
                                    break;
                                srch1 = &(*srch1)->p->bySpecialization.next;
                            }
                            if (srch1 == currents->ptail)
                            {
                                DoInstantiate(l.sp->sb->parentClass, l.sp, l.sp->tp, l.sp->sb->nameSpaceValues, false);
                            }
                        }
                    }
                }
                FlushLineData("", INT_MAX);
            }
        }
        while (count--)
            dropStructureDeclaration();
        templateHeaderCount = lasttemplateHeaderCount;
        (*currents->plast) = nullptr;
        currents->ptail = currents->plast;
        if (templateNestingCount == 0)
            currents = nullptr;
    }
    else  // instantiation
    {
        if (KWTYPE(lex, TT_STRUCT))
        {
            enum e_lk linkage1 = lk_none, linkage2 = lk_none, linkage3 = lk_none;
            lex = getsym();
            if (MATCHKW(lex, kw__declspec))
            {
                lex = getsym();
                lex = parse_declspec(lex, &linkage1, &linkage2, &linkage3);
            }
            if (!ISID(lex))
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            else
            {
                char idname[512];
                SYMBOL* cls = nullptr;
                SYMBOL* strSym = nullptr;
                NAMESPACEVALUELIST* nsv = nullptr;
                lex = id_expression(lex, funcsp, &cls, &strSym, &nsv, nullptr, false, false, idname);
                if (!cls || !isstructured(cls->tp))
                {
                    if (!cls)
                    {
                        errorstr(ERR_NOT_A_TEMPLATE, idname);
                    }
                    else
                    {
                        errorsym(ERR_CLASS_TYPE_EXPECTED, cls);
                    }
                }
                else
                {
                    TEMPLATEPARAMLIST* templateParams = nullptr;
                    SYMBOL* instance;
                    lex = getsym();
                    lex = GetTemplateArguments(lex, funcsp, cls, &templateParams);
                    instance = GetClassTemplate(cls, templateParams, false);
                    if (instance)
                    {
                        MarkDllLinkage(instance, linkage2);
                        if (!isExtern)
                        {
                            instance->sb->dontinstantiate = false;
                            instance = TemplateClassInstantiate(instance, templateParams, false, sc_global);
                            referenceInstanceMembers(instance);
                        }
                        else
                        {
                            instance->sb->dontinstantiate = true;
                        }
                    }
                    else
                    {
                        errorsym(ERR_NOT_A_TEMPLATE, cls);
                    }
                }
            }
        }
        else
        {
            SYMBOL* sym = nullptr;
            enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
            TYPE* tp = nullptr;
            bool defd = false;
            bool notype = false;
            NAMESPACEVALUELIST* nsv = nullptr;
            SYMBOL* strSym = nullptr;
            int consdest = 0;
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
            lex = getBasicType(lex, funcsp, &tp, &strSym, true, funcsp ? sc_auto : sc_global, &linkage, &linkage2, &linkage3,
                               ac_public, &notype, &defd, &consdest, nullptr, false, true, false);
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
            lex = getBeforeType(lex, funcsp, &tp, &sym, &strSym, &nsv, true, sc_cast, &linkage, &linkage2, &linkage3, false,
                                consdest, false, false);
            sizeQualifiers(tp);
            if (!sym)
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            else if (notype && !consdest)
            {
                error(ERR_TYPE_NAME_EXPECTED);
            }
            else
            {
                if (sym->sb->attribs.inheritable.linkage2 == lk_none)
                    sym->sb->attribs.inheritable.linkage2 = linkage2;
                DoInstantiate(strSym, sym, tp, nsv, isExtern);
            }
        }
    }
    localNameSpace->valueData->syms = oldSyms;
    return lex;
}
}  // namespace Parser