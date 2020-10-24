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
#include <wchar.h>
#include "Utils.h"
#include "PreProcessor.h"
#include "ioptimizer.h"
#include "ccerr.h"
#include "declare.h"
#include "config.h"
#include "symtab.h"
#include "ildata.h"
#include "initbackend.h"
#include "occparse.h"
#include "declcpp.h"
#include "mangle.h"
#include "lex.h"
#include "lambda.h"
#include "template.h"
#include "expr.h"
#include "stmt.h"
#include "help.h"
#include "memory.h"
#include "template.h"
#include "symtab.h"
#include "cpplookup.h"
#include "OptUtils.h"
#include "constopt.h"
#include "declcons.h"
#include "init.h"
#include "inline.h"
#include "beinterf.h"
#include "osutil.h"
#include "types.h"
#include "browse.h"
#include "Property.h"
#include "ildata.h"

namespace Parser
{

int inDefaultParam;
char deferralBuf[100000];
SYMBOL* enumSyms;
STRUCTSYM* structSyms;
int expandingParams;
int noSpecializationError;
Optimizer::LIST* deferred;
int structLevel;
Optimizer::LIST* openStructs;

static int unnamed_tag_id, unnamed_id;
static char* importFile;
static unsigned symbolKey;

static LEXEME* getStorageAndType(LEXEME* lex, SYMBOL* funcsp, SYMBOL** strSym, bool inTemplate, bool assumeType,
                                 enum e_sc* storage_class, enum e_sc* storage_class_in, Optimizer::ADDRESS* address, bool* blocked,
                                 bool* isExplicit, bool* constexpression, TYPE** tp, enum e_lk* linkage, enum e_lk* linkage2,
                                 enum e_lk* linkage3, enum e_ac access, bool* notype, bool* defd, int* consdest, bool* templateArg,
                                 bool* asFriend);

void declare_init(void)
{
    unnamed_tag_id = 1;
    unnamed_id = 1;
    structSyms = nullptr;
    nameSpaceList = nullptr;
    anonymousNameSpaceName[0] = 0;
    noSpecializationError = 0;
    deferred = nullptr;
    structLevel = 0;
    inDefaultParam = 0;
    openStructs = nullptr;
    argument_nesting = 0;
    symbolKey = 0;
}

void InsertGlobal(SYMBOL* sp) { Optimizer::globalCache.push_back(Optimizer::SymbolManager::Get(sp)); }
void WeedExterns()
{
    for (auto it = Optimizer::externals.begin(); it != Optimizer::externals.end();)
    {
        Optimizer::SimpleSymbol* sym = *it;
        if (!sym->ispure &&
            ((sym->dontinstantiate && sym->genreffed) ||
             (!sym->initialized &&
              (sym->tp->type == Optimizer::st_func ||
               (sym->tp->type != Optimizer::st_func && sym->storage_class != Optimizer::scc_global &&
                sym->storage_class != Optimizer::scc_static && sym->storage_class != Optimizer::scc_localstatic)) &&
              (sym->genreffed && (sym->parentClass || sym->storage_class == Optimizer::scc_external)))) &&
            !sym->noextern)
        {
            ++it;
        }
        else
        {
            it = Optimizer::externals.erase(it);
        }
    }
}

static const char* NewTagName(void)
{
    char buf[256];
    Optimizer::my_sprintf(buf, "Unnamed++%d", unnamed_tag_id++);
    return litlate(buf);
}
static const char* NewUnnamedID(void)
{
    char buf[256];
    Optimizer::my_sprintf(buf, "Unnamed++Identifier %d", unnamed_id++);
    return litlate(buf);
}
const char* AnonymousName(void)
{
    char buf[256];
    Optimizer::my_sprintf(buf, "Anonymous++Identifier %d", unnamed_id++);
    return litlate(buf);
}
const char* AnonymousTypeName(void)
{
    char buf[512];
    std::string name = preProcessor->GetRealFile();
    sprintf(buf, "__anontype_%08x_%04d", Utils::CRC32((const unsigned char*)name.c_str(), name.size()),
            preProcessor->GetAnonymousIndex());
    return litlate(buf);
}
SYMBOL* SymAlloc()
{
    SYMBOL* sp = (SYMBOL*)Alloc(sizeof(SYMBOL));
    sp->sb = (sym::_symbody*)Alloc(sizeof(sym::_symbody));
    return sp;
}
SYMBOL* makeID(enum e_sc storage_class, TYPE* tp, SYMBOL* spi, const char* name)
{
    SYMBOL* sp = SymAlloc();
    LEXEME* lex = context->cur ? context->cur->prev : context->last;
    if (name && strstr(name, "++"))
        sp->sb->compilerDeclared = true;
    sp->name = name;
    sp->sb->storage_class = storage_class;
    sp->tp = tp;
    if (lex)
    {
        sp->sb->declfile = sp->sb->origdeclfile = lex->errfile;
        sp->sb->declline = sp->sb->origdeclline = lex->errline;
        sp->sb->realdeclline = lex->linedata->lineno;
        sp->sb->declfilenum = lex->linedata->fileindex;
    }
    if (spi)
    {
        error(ERR_TOO_MANY_IDENTIFIERS_IN_DECLARATION);
        return spi;
    }
    return sp;
}
SYMBOL* makeUniqueID(enum e_sc storage_class, TYPE* tp, SYMBOL* spi, const char* name)
{
    char buf[512];
    sprintf(buf, "%s_%08x", name, identityValue);
    return makeID(storage_class, tp, spi, litlate(buf));
}
void addStructureDeclaration(STRUCTSYM* decl)
{
    decl->next = structSyms;
    decl->tmpl = nullptr;
    structSyms = decl;
}
void addTemplateDeclaration(STRUCTSYM* decl)
{
    decl->next = structSyms;
    decl->str = nullptr;
    structSyms = decl;
}
void dropStructureDeclaration(void) { structSyms = structSyms->next; }
SYMBOL* getStructureDeclaration(void)
{
    STRUCTSYM* l = structSyms;
    while (l && !l->str)
        l = l->next;
    if (l)
        return l->str;
    return nullptr;
}
void InsertSymbol(SYMBOL* sp, enum e_sc storage_class, enum e_lk linkage, bool allowDups)
{
    HASHTABLE* table;
    SYMBOL* ssp = getStructureDeclaration();

    if (ssp && sp->sb->parentClass == ssp)
    {
        table = sp->sb->parentClass->tp->syms;
    }
    else if (storage_class == sc_auto || storage_class == sc_register || storage_class == sc_catchvar ||
             storage_class == sc_parameter || storage_class == sc_localstatic)
    {
        table = localNameSpace->valueData->syms;
    }
    else if (Optimizer::cparams.prm_cplusplus && isfunction(sp->tp) && theCurrentFunc && !getStructureDeclaration())
    {
        table = localNameSpace->valueData->syms;
    }
    else
    {
        table = globalNameSpace->valueData->syms;
    }
    if (table)
    {
        if (isfunction(sp->tp) && !istype(sp))
        {
            const char* name = sp->sb->castoperator ? overloadNameTab[CI_CAST] : sp->name;
            SYMLIST** hr = LookupName(name, table);
            SYMBOL* funcs = nullptr;
            if (hr)
                funcs = (SYMBOL*)(*hr)->p;
            if (!funcs)
            {
                TYPE* tp = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
                tp->type = bt_aggregate;
                tp->rootType = tp;
                funcs = makeID(sc_overloads, tp, 0, name);
                funcs->sb->castoperator = sp->sb->castoperator;
                funcs->sb->parentClass = sp->sb->parentClass;
                funcs->sb->parentNameSpace = sp->sb->parentNameSpace;
                tp->sp = funcs;
                SetLinkerNames(funcs, linkage);
                insert(funcs, table);
                table = funcs->tp->syms = CreateHashTable(1);
                insert(sp, table);
                sp->sb->overloadName = funcs;
            }
            else if (Optimizer::cparams.prm_cplusplus && funcs->sb->storage_class == sc_overloads)
            {
                table = funcs->tp->syms;
                if (AddOverloadName(sp, table))
                {
                    // we are going to naively add duplicate overloads on the basis they may
                    // differ in return type, which may make a difference for example for enable_if as a return type
                    // but we won't fully implement this at this time, e.g. if it has enable_if it had better be
                    // defined inline.   If it is defined out of line the lack of matching when we do lookups
                    // will cause multiply defined references in GetOverloadedFunction
                    SYMLIST** hr1 = &table->table[0];
                    int n = 0;
                    while (*hr1)
                    {
                        if (!strcmp(sp->sb->decoratedName, ((SYMBOL*)(*hr1)->p)->sb->decoratedName))
                        {
                            n++;
                            if (comparetypes(basetype(sp->tp)->btp, basetype(((SYMBOL*)(*hr1)->p)->tp)->btp, true))
                                break;
                        }
                        hr1 = &(*hr1)->next;
                    }
                    if (!*hr1)
                    {
                        sp->sb->overlayIndex = n;
                        *hr1 = (SYMLIST*)Alloc(sizeof(SYMLIST));
                        (*hr1)->p = (SYMBOL*)sp;
                    }
                }
                sp->sb->overloadName = funcs;
                if (sp->sb->parent != funcs->sb->parent || sp->sb->parentNameSpace != funcs->sb->parentNameSpace)
                    funcs->sb->wasUsing = true;
            }
            else
            {
                errorsym(ERR_DUPLICATE_IDENTIFIER, sp);
            }
        }
        else if (!allowDups || sp != search(sp->name, table))
            insert(sp, table);
    }
    else
    {
        diag("InsertSymbol: cannot insert");
    }
}
LEXEME* tagsearch(LEXEME* lex, char* name, SYMBOL** rsp, HASHTABLE** table, SYMBOL** strSym_out, NAMESPACEVALUELIST** nsv_out,
                  enum e_sc storage_class)
{
    NAMESPACEVALUELIST* nsv = nullptr;
    SYMBOL* strSym = nullptr;

    SYMLIST** hr;
    *rsp = nullptr;
    if (ISID(lex) || MATCHKW(lex, classsel))
    {
        lex = nestedSearch(lex, rsp, &strSym, &nsv, nullptr, nullptr, true, storage_class, false, false);
        if (*rsp)
        {
            strcpy(name, (*rsp)->name);
            lex = getsym();
            if (MATCHKW(lex, begin))
            {
                // specify EXACTLY the first result if it is a definition
                // otherwise what is found by nestedSearch is fine...
                if (strSym)
                    hr = LookupName((*rsp)->name, strSym->tp->tags);
                else if (nsv)
                    hr = LookupName((*rsp)->name, nsv->valueData->tags);
                else if (Optimizer::cparams.prm_cplusplus && (storage_class == sc_member || storage_class == sc_mutable))
                    hr = LookupName((*rsp)->name, getStructureDeclaration()->tp->tags);
                else if (storage_class == sc_auto)
                    hr = LookupName((*rsp)->name, localNameSpace->valueData->tags);
                else
                    hr = LookupName((*rsp)->name, globalNameSpace->valueData->tags);
                if (hr)
                    *rsp = (SYMBOL*)(*hr)->p;
                else
                {
                    if (nsv || strSym)
                    {
                        errorNotMember(strSym, nsv, (*rsp)->name);
                    }
                    *rsp = nullptr;
                }
            }
        }
        else if (ISID(lex))
        {
            strcpy(name, lex->value.s.a);
            lex = getsym();
            if (MATCHKW(lex, begin))
            {
                if (nsv || strSym)
                {
                    errorNotMember(strSym, nsv, name);
                }
            }
        }
    }
    if (nsv)
    {
        *table = nsv->valueData->tags;
    }
    else if (strSym)
    {
        *table = strSym->tp->tags;
    }
    else if (Optimizer::cparams.prm_cplusplus && (storage_class == sc_member || storage_class == sc_mutable))
    {
        strSym = getStructureDeclaration();
        *table = strSym->tp->tags;
    }
    else
    {
        if (storage_class == sc_auto)
        {
            *table = localNameSpace->valueData->tags;
            nsv = localNameSpace;
        }
        else
        {
            *table = globalNameSpace->valueData->tags;
            nsv = globalNameSpace;
        }
    }
    *nsv_out = nsv;
    *strSym_out = strSym;
    return lex;
}
static void checkIncompleteArray(TYPE* tp, const char* errorfile, int errorline)
{
    SYMLIST* hr;
    tp = basetype(tp);
    if (tp->syms == nullptr)
        return; /* should get a size error */
    hr = tp->syms->table[0];
    while (hr)
    {
        SYMBOL* sp;
        sp = hr->p;
        if (hr->next == nullptr)
        {
            if (ispointer(sp->tp) && basetype(sp->tp)->size == 0)
                specerror(ERR_STRUCT_MAY_NOT_CONTAIN_INCOMPLETE_STRUCT, "", errorfile, errorline);
        }
        hr = hr->next;
    }
}
LEXEME* get_type_id(LEXEME* lex, TYPE** tp, SYMBOL* funcsp, enum e_sc storage_class, bool beforeOnly, bool toErr, bool inUsing)
{
    enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
    bool defd = false;
    SYMBOL* sp = nullptr;
    bool notype = false;
    bool oldTemplateType = inTemplateType;
    *tp = nullptr;

    lex = getQualifiers(lex, tp, &linkage, &linkage2, &linkage3, nullptr);
        lex = getBasicType(lex, funcsp, tp, nullptr, false, funcsp ? sc_auto : sc_global, &linkage, &linkage2, &linkage3, ac_public,
                       &notype, &defd, nullptr, nullptr, false, false, inUsing);
    lex = getQualifiers(lex, tp, &linkage, &linkage2, &linkage3, nullptr);
    lex = getBeforeType(lex, funcsp, tp, &sp, nullptr, nullptr, false, storage_class, &linkage, &linkage2, &linkage3, false, false,
                        beforeOnly, false); /* fixme at file scope init */
    sizeQualifiers(*tp);
    if (notype)
        *tp = nullptr;
    else if (sp && !sp->sb->anonymous && toErr)
        if (sp->tp->type != bt_templateparam)
            error(ERR_TOO_MANY_IDENTIFIERS);
    if (sp && sp->sb->anonymous && linkage != lk_none)
    {
        if (sp->sb->attribs.inheritable.linkage != lk_none)
            error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
        else
            sp->sb->attribs.inheritable.linkage = linkage;
    }
    inTemplateType = oldTemplateType;
    return lex;
}
SYMBOL* calculateStructAbstractness(SYMBOL* top, SYMBOL* sp)
{
    BASECLASS* bases = sp->sb->baseClasses;
    SYMLIST* hr = sp->tp->syms->table[0];
    while (bases)
    {
        if (bases->cls->sb->isabstract)
        {
            SYMBOL* rv = calculateStructAbstractness(top, bases->cls);
            if (rv)
                return rv;
        }
        bases = bases->next;
    }
    while (hr)
    {
        SYMBOL* p = hr->p;
        if (p->sb->storage_class == sc_overloads)
        {
            SYMLIST* hri = p->tp->syms->table[0];
            while (hri)
            {
                SYMBOL* pi = (SYMBOL*)hri->p;
                if (pi->sb->ispure)
                {
                    // ok found a pure function, look it up within top and
                    // check to see if it has been overrridden
                    SYMBOL* pq;
                    STRUCTSYM l;
                    l.str = (SYMBOL*)top;
                    addStructureDeclaration(&l);
                    pq = classsearch(pi->name, false, true);
                    dropStructureDeclaration();
                    if (pq)
                    {
                        SYMLIST* hrq = pq->tp->syms->table[0];
                        while (hrq)
                        {
                            SYMBOL* pq1 = (SYMBOL*)hrq->p;
                            const char* p1 = strrchr(pq1->sb->decoratedName, '@');
                            const char* p2 = strrchr(pi->sb->decoratedName, '@');
                            if (p1 && p2 && !strcmp(p1, p2))
                            {
                                if (!pq1->sb->ispure)
                                {
                                    return nullptr;
                                }
                                else
                                {
                                    break;
                                }
                            }
                            hrq = hrq->next;
                        }
                        // if it either isn't found or was found and is pure...
                        top->sb->isabstract = true;
                        return pi;
                    }
                }
                hri = hri->next;
            }
        }
        hr = hr->next;
    }
    return nullptr;
}
void calculateStructOffsets(SYMBOL* sp)
{
    enum e_bt type = basetype(sp->tp)->type;
    enum e_bt bittype = bt_none;
    int startbit = 0;
    int maxbits = 0;
    int nextoffset = 0;
    int maxsize = 0;
    SYMLIST* hr = sp->tp->syms->table[0];
    int size = 0;
    int totalAlign = -1;
    BASECLASS* bases = sp->sb->baseClasses;

    if (bases)
    {
        if (sp->sb->hasvtab && !bases->cls->sb->hasvtab)
            bases->offset = getSize(bt_pointer);
        else
            bases->offset = 0;
    }
    if (sp->sb->hasvtab && (!sp->sb->baseClasses || !sp->sb->baseClasses->cls->sb->hasvtab || sp->sb->baseClasses->isvirtual))
    {
        size += getSize(bt_pointer);
        totalAlign = getAlign(sc_member, &stdpointer);
    }
    while (bases)
    {
        SYMBOL* sym = bases->cls;
        int align = sym->sb->attribs.inheritable.structAlign;
        totalAlign = imax(totalAlign, align);
        if (align > 1)
        {
            int al2 = align - size % align;
            if (al2 != align)
                size += al2;
        }
        if (!bases->isvirtual)
        {
            if (bases != sp->sb->baseClasses)
                bases->offset = size;
            size += sym->sb->sizeNoVirtual;
        }
        bases = bases->next;
    }
    while (hr)
    {
        SYMBOL* p = hr->p;
        TYPE* tp = basetype(p->tp);
        if (p->sb->storage_class != sc_static && p->sb->storage_class != sc_external && p->sb->storage_class != sc_overloads &&
            p->sb->storage_class != sc_enumconstant && !istype(p) && p != sp && p->sb->parentClass == sp)
        // not function, also not injected self or base class or static variable
        {
            int align;
            int offset;

            if (p->sb->attribs.inheritable.packed)
            {
                align = 0;
            }
            else
            {
                if (isstructured(tp))
                    align = tp->sp->sb->attribs.inheritable.structAlign;
                else if (!p->sb->attribs.inheritable.alignedAttribute && p->sb->attribs.inheritable.structAlign)
                    align = p->sb->attribs.inheritable.structAlign;
                else
                    align = getAlign(sc_member, tp);
                if (p->sb->attribs.inheritable.alignedAttribute)
                    align = imax(align, p->sb->attribs.inheritable.structAlign);
            }
            if (p->sb->attribs.inheritable.warnAlign && p->sb->attribs.inheritable.warnAlign > align)
            {
                errorsym(ERR_MINIMUM_ALIGN_NOT_ACHIEVED, p);
            }
            totalAlign = imax(totalAlign, align);
            p->sb->parent = sp;
            if (tp->size == 0)
            {
                if (Optimizer::cparams.prm_c99 && tp->type == bt_pointer && p->tp->array)
                {
                    if (!hr->next || p->sb->init)
                    {
                        offset = nextoffset;
                        nextoffset = tp->btp->size;
                        goto join;
                    }
                    if (!p->tp->vla)
                        error(ERR_EMPTY_ARRAY_LAST);
                }
                else if (!Optimizer::cparams.prm_cplusplus && p->sb->storage_class == sc_overloads)
                    error(ERR_STRUCT_UNION_NO_FUNCTION);
                else /* c90 doesn't allow unsized arrays here */
                    if (Optimizer::cparams.prm_ansi)
                    errorsym(ERR_UNSIZED, p);
            }
            if (!isunion(tp) && isstructured(tp))
            {
                checkIncompleteArray(tp, p->sb->declfile, p->sb->declline);
            }
            if (isstructured(tp) && !tp->size && !templateNestingCount)
            {
                errorsym(ERR_STRUCT_NOT_DEFINED, p);
            }
            if (tp->hasbits)
            {
                /* tp->bits == 0 or change of type
                 * means don't pack any more into this storage unit
                 */
                if (tp->bits != 0 && bittype == tp->type && startbit + tp->bits <= maxbits)
                {
                    tp->startbit = startbit;
                    startbit += tp->bits;
                    nextoffset = tp->size;
                    //				nextoffset = (startbit+ Optimizer::chosenAssembler->arch->bits_per_mau -1)
                    //						/Optimizer::chosenAssembler->arch->bits_per_mau;
                    offset = 0;
                }
                else
                {
                    offset = nextoffset;
                    //				nextoffset = (tp->bits + Optimizer::chosenAssembler->arch->bits_per_mau -1)
                    //					/Optimizer::chosenAssembler->arch->bits_per_mau;
                    nextoffset = tp->size;
                    bittype = tp->type;
                    startbit = tp->bits;
                    tp->startbit = 0;
                    maxbits = Optimizer::chosenAssembler->arch->bits_per_mau * tp->size;
                }
            }
            else
            {
                offset = nextoffset;
                nextoffset = tp->size;
                bittype = bt_none;
                startbit = 0;
            }
            if (nextoffset == 0)
                nextoffset++;
        join:
            if (type == bt_struct || type == bt_class)
            {
                size += offset;
                if (offset && align > 1)
                {
                    int al2 = align - size % align;
                    if (al2 != align)
                        size += al2;
                }
            }
            p->sb->offset = size;
            if (type == bt_union && offset > maxsize)
            {
                maxsize = offset;
                size = 0;
            }
        }
        hr = hr->next;
    }
    size += nextoffset;
    if (type == bt_union && maxsize > size)
    {
        size = maxsize;
    }

    if (size == 0)
    {
        if (Optimizer::cparams.prm_cplusplus)
        {
            // make it non-zero size to avoid further errors...
            size = getSize(bt_int);
        }
    }
    sp->tp->size = size;
    sp->sb->attribs.inheritable.structAlign = totalAlign == -1 ? 1 : totalAlign;
    //    if (Optimizer::cparams.prm_cplusplus)
    {
        // align the size of the structure to make the structure alignable when used as an array element
        if (totalAlign)
        {
            int skew = sp->tp->size % totalAlign;
            if (skew)
                skew = totalAlign - skew;
            sp->tp->size += skew;
        }
    }
}
static bool validateAnonymousUnion(SYMBOL* parent, TYPE* unionType)
{
    bool rv = true;
    unionType = basetype(unionType);
    {
        SYMLIST* newhr = unionType->syms->table[0];
        while (newhr)
        {
            SYMLIST** hhr;
            SYMBOL* member = (SYMBOL*)newhr->p;
            if (Optimizer::cparams.prm_cplusplus && member->sb->storage_class == sc_overloads)
            {
                SYMLIST* hr = basetype(member->tp)->syms->table[0];
                while (hr)
                {
                    if (!hr->p->sb->defaulted)
                    {
                        error(ERR_ANONYMOUS_UNION_NO_FUNCTION_OR_TYPE);
                        rv = false;
                        break;
                    }
                    hr = hr->next;
                }
            }
            else if ((Optimizer::cparams.prm_cplusplus && member->sb->storage_class == sc_type) ||
                     member->sb->storage_class == sc_typedef)
            {
                error(ERR_ANONYMOUS_UNION_NO_FUNCTION_OR_TYPE);
                rv = false;
            }
            else if ((Optimizer::cparams.prm_cplusplus && member->sb->access == ac_private) || member->sb->access == ac_protected)
            {
                error(ERR_ANONYMOUS_UNION_PUBLIC_MEMBERS);
                rv = false;
            }
            else if (Optimizer::cparams.prm_cplusplus && member->sb->storage_class != sc_member &&
                     member->sb->storage_class != sc_mutable)
            {
                error(ERR_ANONYMOUS_UNION_NONSTATIC_MEMBERS);
                rv = false;
            }
            else if (parent && (hhr = LookupName(member->name, parent->tp->syms)) != nullptr)
            {
                SYMBOL* spi = (SYMBOL*)(*hhr)->p;
                currentErrorLine = 0;
                preverrorsym(ERR_DUPLICATE_IDENTIFIER, spi, spi->sb->declfile, spi->sb->declline);
                rv = false;
            }
            newhr = newhr->next;
        }
    }
    return rv;
}
static void resolveAnonymousGlobalUnion(SYMBOL* sp)
{
    SYMLIST* hr = sp->tp->syms->table[0];
    validateAnonymousUnion(nullptr, sp->tp);
    sp->sb->label = Optimizer::nextLabel++;
    sp->sb->storage_class = sc_localstatic;
    insertInitSym(sp);
    while (hr)
    {
        SYMBOL* sym = hr->p;
        if (sym->sb->storage_class == sc_member || sym->sb->storage_class == sc_mutable)
        {
            SYMBOL* spi;
            if ((spi = gsearch(sym->name)) != nullptr)
            {
                currentErrorLine = 0;
                preverrorsym(ERR_DUPLICATE_IDENTIFIER, spi, spi->sb->declfile, spi->sb->declline);
            }
            else
            {
                sym->sb->storage_class = sc_localstatic;
                sym->sb->label = sp->sb->label;
                InsertSymbol(sym, sc_static, lk_c, false);
            }
        }
        hr = hr->next;
    }
}
void resolveAnonymousUnions(SYMBOL* sp)
{
    SYMLIST** member = (SYMLIST**)&sp->tp->syms->table[0];
    if (TotalErrors())
        return;
    while (*member)
    {
        SYMBOL* spm = (SYMBOL*)(*member)->p;
        // anonymous structured type declaring anonymous variable is a candidate for
        // an anonymous structure or union
        if (isstructured(spm->tp) && spm->sb->anonymous && basetype(spm->tp)->sp->sb->anonymous)
        {
#ifdef ERROR
#    error NESTEDANONYUNION
#endif
            SYMLIST* next = (*member)->next;
            resolveAnonymousUnions(spm);
            validateAnonymousUnion(sp, spm->tp);
            *member = spm->tp->syms->table[0];
            if (basetype(spm->tp)->type == bt_union)
            {
                if (!Optimizer::cparams.prm_c99 && !Optimizer::cparams.prm_cplusplus)
                {
                    error(ERR_ANONYMOUS_UNION_WARNING);
                }
            }
            else
            {
                error(ERR_ANONYMOUS_STRUCT_WARNING);
            }
            while (*member)
            {
                SYMBOL* newsp = (SYMBOL*)(*member)->p;
                if ((newsp->sb->storage_class == sc_member || newsp->sb->storage_class == sc_mutable) && !isfunction(newsp->tp))
                {
                    newsp->sb->offset += spm->sb->offset;
                    newsp->sb->parentClass = sp;
                    member = &(*member)->next;
                }
                else
                {
                    *member = (*member)->next;
                }
            }
            *member = next;
        }
        else
        {
            member = &(*member)->next;
        }
    }
}
static bool usesClass(SYMBOL* cls, SYMBOL* internal)
{
    if (cls->tp->syms)
    {
        SYMLIST* hr = cls->tp->syms->table[0];
        while (hr)
        {
            SYMBOL* sym = hr->p;
            TYPE* tp = sym->tp;
            if (istype(sym))
            {
                if (sym->sb->storage_class != sc_typedef)
                {
                    tp = nullptr;
                }
            }
            if (tp)
            {
                while (isarray(tp))
                    tp = basetype(tp)->btp;
                if (comparetypes(internal->tp, tp, true) || sameTemplate(internal->tp, tp))
                    return true;
            }
            hr = hr->next;
        }
    }
    return false;
}
static void baseFinishDeclareStruct(SYMBOL* funcsp)
{
    (void)funcsp;
    int n = 0, i, j;
    Optimizer::LIST* lst = openStructs;
    SYMBOL** syms;
    while (lst)
        n++, lst = lst->next;
    syms = (SYMBOL**)(SYMBOL**)Alloc(sizeof(SYMBOL*) * n);
    n = 0;
    lst = openStructs;
    openStructs = nullptr;
    while (lst)
    {
        syms[n++] = (SYMBOL*)lst->data, lst = lst->next;
    }
    for (i = 0; i < n; i++)
        for (j = i + 1; j < n; j++)
            if ((syms[i] != syms[j] && !sameTemplate(syms[i]->tp, syms[j]->tp) && classRefCount(syms[j], syms[i])) ||
                usesClass(syms[i], syms[j]))
            {
                SYMBOL* x = syms[j];
                memmove(&syms[i + 1], &syms[i], sizeof(SYMBOL*) * (j - i));
                syms[i] = x;
                //                if (j != i + 1)
                //                    j = i;
            }
    for (i = 0; i < n; i++)
    {
        if (!syms[i]->sb->performedStructInitialization)
        {
            syms[i]->sb->performedStructInitialization = true;
            if (Optimizer::cparams.prm_cplusplus)
            {
                if (syms[i]->templateParams && !allTemplateArgsSpecified(syms[i], syms[i]->templateParams->next))
                {
                    int oldInstantiatingTemplate = instantiatingTemplate;
                    instantiatingTemplate = 0;
                    templateNestingCount++;
                    deferredInitializeStructFunctions(syms[i]);
                    templateNestingCount--;
                    instantiatingTemplate = oldInstantiatingTemplate;
                }
                else
                {
                    deferredInitializeStructFunctions(syms[i]);
                }
            }
        }
    }
}
static void makeFastTable(SYMBOL* sp)
{
    int n = 0;
    SYMLIST* hr = sp->tp->syms->table[0];
    for (; hr; hr = hr->next, n++)
        ;
    n /= 5;
    if (n > 1)
    {
        sp->tp->syms->fast = CreateHashTable(n + 1);
        hr = sp->tp->syms->table[0];
        for (; hr; hr = hr->next)
            insert(hr->p, sp->tp->syms->fast);
    }
}
static LEXEME* structbody(LEXEME* lex, SYMBOL* funcsp, SYMBOL* sp, enum e_ac currentAccess)
{
    STRUCTSYM sl;
    (void)funcsp;
    if (Optimizer::cparams.prm_cplusplus)
    {
        Optimizer::LIST* lst = (Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
        lst->next = openStructs;
        openStructs = lst;
        lst->data = sp;
    }
    lex = getsym();
    sl.str = sp;
    addStructureDeclaration(&sl);
    while (lex && KW(lex) != end)
    {
        FlushLineData(lex->errfile, lex->linedata->lineno);
        switch (KW(lex))
        {
            case kw_private:
                sp->sb->accessspecified = true;
                currentAccess = ac_private;
                lex = getsym();
                needkw(&lex, colon);
                break;
            case kw_public:
                sp->sb->accessspecified = true;
                currentAccess = ac_public;
                lex = getsym();
                needkw(&lex, colon);
                break;
            case kw_protected:
                sp->sb->accessspecified = true;
                currentAccess = ac_protected;
                lex = getsym();
                needkw(&lex, colon);
                break;
            default:
                lex = declare(lex, nullptr, nullptr, sc_member, lk_none, nullptr, true, false, false, currentAccess);
                break;
        }
    }
    dropStructureDeclaration();
    sp->sb->hasvtab = usesVTab(sp);
    calculateStructOffsets(sp);

    if (Optimizer::cparams.prm_cplusplus)
    {
        createDefaultConstructors(sp);
        calculateStructAbstractness(sp, sp);
        calculateVirtualBaseOffsets(sp);  // undefined in local context
        calculateVTabEntries(sp, sp, &sp->sb->vtabEntries, 0);
        if (sp->sb->vtabEntries)
        {
            char buf[4096];
            //            InsertInline(sp);
            Optimizer::my_sprintf(buf, "%s@_$vt", sp->sb->decoratedName);
            sp->sb->vtabsp = makeID(sc_static, &stdvoid, nullptr, litlate(buf));
            sp->sb->vtabsp->sb->attribs.inheritable.linkage2 = sp->sb->attribs.inheritable.linkage2;
            sp->sb->vtabsp->sb->attribs.inheritable.linkage = lk_virtual;
            sp->sb->vtabsp->sb->decoratedName = sp->sb->vtabsp->name;
            if (sp->sb->vtabsp->sb->attribs.inheritable.linkage2 == lk_import)
            {
                sp->sb->vtabsp->sb->dontinstantiate = true;
            }
            else if (sp->sb->vtabsp->sb->attribs.inheritable.linkage2 == lk_export)
            {
                Optimizer::SymbolManager::Get(sp->sb->vtabsp);
            }
            InsertInline(sp);
            warnCPPWarnings(sp, funcsp != nullptr);
        }
    }
    else
    {
        sp->sb->trivialCons = true;
    }
    resolveAnonymousUnions(sp);
    makeFastTable(sp);
    if (Optimizer::cparams.prm_cplusplus)
        deferredInitializeStructMembers(sp);
    if (!Optimizer::cparams.prm_cplusplus || structLevel == 1)
    {
        structLevel--;
        baseFinishDeclareStruct(funcsp);
        structLevel++;
    }
    if (Optimizer::cparams.prm_cplusplus && sp->tp->syms && !templateNestingCount)
    {
        SYMBOL* cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(sp->tp)->syms);
        if (!cons)
        {
            SYMLIST* hr = basetype(sp->tp)->syms->table[0];
            while (hr)
            {
                SYMBOL* sp1 = hr->p;
                if (sp1->sb->storage_class == sc_member || sp1->sb->storage_class == sc_mutable)
                {
                    if (isref(sp1->tp))
                    {
                        errorsym(ERR_REF_CLASS_NO_CONSTRUCTORS, sp1);
                    }
                    else if (isconst(sp1->tp))
                    {
                        errorsym(ERR_CONST_CLASS_NO_CONSTRUCTORS, sp1);
                    }
                }
                hr = hr->next;
            }
        }
    }
    if (!lex)
    {
        error(ERR_UNEXPECTED_EOF);
    }
    else
    {
        lex = getsym();
        FlushLineData("", 0);
    }
    return lex;
}
LEXEME* innerDeclStruct(LEXEME* lex, SYMBOL* funcsp, SYMBOL* sp, bool inTemplate, enum e_ac defaultAccess, bool isfinal, bool* defd)
{
    bool hasBody = (Optimizer::cparams.prm_cplusplus && KW(lex) == colon) || KW(lex) == begin;
    SYMBOL* injected = nullptr;

    if (/*templateNestingCount &&*/ nameSpaceList)
        SetTemplateNamespace(sp);
    if (sp->sb->attribs.inheritable.structAlign == 0)
        sp->sb->attribs.inheritable.structAlign = 1;
    structLevel++;
    sp->sb->declaring = true;
    if (hasBody)
    {
        if (sp->tp->syms || sp->tp->tags)
        {
            preverrorsym(ERR_STRUCT_HAS_BODY, sp, sp->sb->declfile, sp->sb->declline);
        }
        sp->tp->syms = CreateHashTable(1);
        if (Optimizer::cparams.prm_cplusplus)
        {
            sp->tp->tags = CreateHashTable(1);
            injected = clonesym(sp);
            injected->sb->mainsym = sp;      // for constructor/destructor matching
            insert(injected, sp->tp->tags);  // inject self
            injected->sb->access = ac_public;
        }
    }
    if (inTemplate && templateNestingCount == 1)
        inTemplateBody++;
    if (Optimizer::cparams.prm_cplusplus)
        if (KW(lex) == colon)
        {
            lex = baseClasses(lex, funcsp, sp, defaultAccess);
            if (injected)
                injected->sb->baseClasses = sp->sb->baseClasses;
            if (!MATCHKW(lex, begin))
                errorint(ERR_NEEDY, '{');
        }
    if (inTemplate && templateNestingCount == 1)
        noSpecializationError++;
    if (KW(lex) == begin)
    {
        sp->sb->isfinal = isfinal;
        lex = structbody(lex, funcsp, sp, defaultAccess);
        *defd = true;
    }
    if (inTemplate && templateNestingCount == 1)
    {
        inTemplateBody--;
        noSpecializationError--;
        TemplateGetDeferred(sp);
    }
    sp->sb->declaring = false;
    --structLevel;
    return lex;
}
static unsigned char* ParseUUID(LEXEME** lex)
{
    if (MATCHKW(*lex, kw__uuid))
    {
        unsigned vals[16];
        unsigned char* rv = (unsigned char*)Alloc(16);
        *lex = getsym();
        needkw(lex, openpa);
        if ((*lex)->type == l_astr)
        {
            int i;
            int count = ((Optimizer::SLCHAR*)(*lex)->value.s.w)->count;
            LCHAR* str = ((Optimizer::SLCHAR*)(*lex)->value.s.w)->str;
            wchar_t buf[200];
            for (i = 0; i < count; i++)
                buf[i] = *str++;
            buf[i] = 0;
            if (11 == swscanf(buf, L"%x-%x-%x-%02x%02x-%02x%02x%02x%02x%02x%02x", &vals[0], &vals[1], &vals[2], &vals[8], &vals[9],
                              &vals[10], &vals[11], &vals[12], &vals[13], &vals[14], &vals[15]))
            {
                rv[0] = vals[0] & 0xff;
                rv[1] = (vals[0] >> 8) & 0xff;
                rv[2] = (vals[0] >> 16) & 0xff;
                rv[3] = (vals[0] >> 24) & 0xff;
                rv[4] = vals[1] & 0xff;
                rv[5] = (vals[1] >> 8) & 0xff;
                rv[6] = vals[2] & 0xff;
                rv[7] = (vals[2] >> 8) & 0xff;
                for (int j = 8; j < 16; j++)
                    rv[j] = vals[j];
            }
            *lex = getsym();
        }
        needkw(lex, closepa);
        return rv;
    }
    return nullptr;
}
static LEXEME* declstruct(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, bool inTemplate, enum e_sc storage_class, enum e_ac access,
                          bool* defd)
{
    bool isfinal = false;
    HASHTABLE* table;
    const char* tagname;
    char newName[4096];
    enum e_bt type = bt_none;
    SYMBOL* sp;
    int charindex;
    NAMESPACEVALUELIST* nsv;
    SYMBOL* strSym;
    enum e_ac defaultAccess;
    bool addedNew = false;
    int declline = lex->errline;
    int realdeclline = lex->linedata->lineno;
    bool anonymous = false;
    unsigned char* uuid;
    enum e_lk linkage1 = lk_none, linkage2 = lk_none, linkage3 = lk_none;
    *defd = false;
    switch (KW(lex))
    {
        case kw_class:
            defaultAccess = ac_private;
            type = bt_class;
            break;
        default:
        case kw_struct:
            defaultAccess = ac_public;
            type = bt_struct;
            break;
        case kw_union:
            defaultAccess = ac_public;
            type = bt_union;
            break;
    }
    lex = getsym();
    uuid = ParseUUID(&lex);
    auto oldAttribs = basisAttribs;
    basisAttribs = {0};
    ParseAttributeSpecifiers(&lex, funcsp, true);
    if (MATCHKW(lex, kw__declspec))
    {
        lex = getsym();
        lex = parse_declspec(lex, &linkage1, &linkage2, &linkage3);
    }
    if (MATCHKW(lex, kw__rtllinkage))
    {
        lex = getsym();
        linkage2 = getDefaultLinkage();
    }
    if (ISID(lex))
    {
        charindex = lex->charindex;
        tagname = litlate(lex->value.s.a);
    }
    else
    {
        if (!MATCHKW(lex, begin) && !MATCHKW(lex, colon))
            errorint(ERR_NEEDY, '{');
        tagname = AnonymousTypeName();
        charindex = -1;
        anonymous = true;
    }

    strcpy(newName, tagname);
    if (inTemplate)
        inTemplateSpecialization++;
    lex = tagsearch(lex, newName, &sp, &table, &strSym, &nsv, storage_class);
    if (inTemplate)
        inTemplateSpecialization--;

    if (charindex != -1 && Optimizer::cparams.prm_cplusplus && ISID(lex) && !strcmp(lex->value.s.a, "final"))
    {
        isfinal = true;
        lex = getsym();
        if (!MATCHKW(lex, begin) && !MATCHKW(lex, colon))
            errorint(ERR_NEEDY, '{');
    }
    if (ISID(lex))
    {
        lex = getsym();
        if (MATCHKW(lex, lt) || MATCHKW(lex, begin) || MATCHKW(lex, colon))
        {
            lex = backupsym();
            // multiple identifiers, wade through them for error handling
            error(ERR_TOO_MANY_IDENTIFIERS_IN_DECLARATION);
            while (ISID(lex))
            {
                charindex = lex->charindex;
                tagname = litlate(lex->value.s.a);
                strcpy(newName, tagname);
                lex = tagsearch(lex, newName, &sp, &table, &strSym, &nsv, storage_class);
            }
        }
        else
        {
            lex = backupsym();
        }
    }
    if (!sp)
    {
        addedNew = true;
        sp = SymAlloc();
        if (!strcmp(newName, tagname))
            sp->name = tagname;
        else
            sp->name = litlate(newName);
        sp->sb->anonymous = anonymous;
        sp->sb->storage_class = sc_type;
        sp->tp = (TYPE*)Alloc(sizeof(TYPE));
        sp->tp->type = type;
        sp->tp->rootType = sp->tp;
        sp->tp->sp = sp;
        sp->tp = AttributeFinish(sp, sp->tp);
        sp->sb->declcharpos = charindex;
        sp->sb->declline = sp->sb->origdeclline = declline;
        sp->sb->realdeclline = realdeclline;
        sp->sb->declfile = sp->sb->origdeclfile = lex->errfile;
        sp->sb->declfilenum = lex->linedata->fileindex;
        if ((storage_class == sc_member || storage_class == sc_mutable) &&
            (MATCHKW(lex, begin) || MATCHKW(lex, colon) || MATCHKW(lex, kw_try) || MATCHKW(lex, semicolon)))
            sp->sb->parentClass = getStructureDeclaration();
        if (nsv)
            sp->sb->parentNameSpace = nsv->valueData->name;
        if (inTemplate)
            sp->sb->parentTemplate = sp;
        sp->sb->anonymous = charindex == -1;
        sp->sb->access = access;
        sp->sb->uuid = uuid;
        sp->sb->attribs.inheritable.linkage2 = linkage2;
        SetLinkerNames(sp, lk_cdecl);
        if (inTemplate && templateNestingCount)
        {
            if (MATCHKW(lex, lt))
                errorsym(ERR_SPECIALIZATION_REQUIRES_PRIMARY, sp);
            sp->templateParams = TemplateGetParams(sp);
            sp->sb->templateLevel = templateNestingCount;
            TemplateMatching(lex, nullptr, sp->templateParams, sp, MATCHKW(lex, begin) || MATCHKW(lex, colon));
            SetLinkerNames(sp, lk_cdecl);
        }
        browse_variable(sp);

        insert(sp, Optimizer::cparams.prm_cplusplus && !sp->sb->parentClass ? globalNameSpace->valueData->tags : table);
    }
    else
    {
        // primarily for the type_info definition when building LSCRTL.DLL
        if (linkage1 != lk_none && linkage1 != sp->sb->attribs.inheritable.linkage)
        {
            if (sp->sb->attribs.inheritable.linkage != lk_none)
                error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
            else
                sp->sb->attribs.inheritable.linkage = linkage1;
        }
        if (linkage2 != lk_none && linkage2 != sp->sb->attribs.inheritable.linkage2)
        {
            if (sp->sb->attribs.inheritable.linkage2 != lk_none)
                error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
            else
                sp->sb->attribs.inheritable.linkage2 = linkage2;
        }
        if (linkage3 != lk_none && linkage3 != sp->sb->attribs.inheritable.linkage3)
        {
            if (sp->sb->attribs.inheritable.linkage3 != lk_none)
                error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
            else
                sp->sb->attribs.inheritable.linkage3 = linkage3;
        }
        if (type != sp->tp->type && (type == bt_union || sp->tp->type == bt_union))
        {
            errorsym(ERR_MISMATCHED_STRUCTURED_TYPE_IN_REDEFINITION, sp);
        }
        else if (inTemplate && templateNestingCount)
        {

            // definition or declaration
            if (!sp->sb->templateLevel)
            {
                if (!sp->sb->parentClass || !sp->sb->parentClass->sb->templateLevel)
                    errorsym(ERR_NOT_A_TEMPLATE, sp);
                SetLinkerNames(sp, lk_cdecl);
            }
            else
            {
                if (inTemplate && KW(lex) == lt)
                {
                    TEMPLATEPARAMLIST* origParams = sp->templateParams;
                    TEMPLATEPARAMLIST* templateParams = TemplateGetParams(sp);
                    inTemplateSpecialization++;
                    parsingSpecializationDeclaration = true;
                    lex = GetTemplateArguments(lex, funcsp, nullptr, &templateParams->p->bySpecialization.types);
                    parsingSpecializationDeclaration = false;
                    inTemplateSpecialization--;
                    sp = LookupSpecialization(sp, templateParams);
                    if (linkage2 != lk_none)
                        sp->sb->attribs.inheritable.linkage2 = linkage2;
                    sp->templateParams =
                        TemplateMatching(lex, origParams, templateParams, sp, MATCHKW(lex, begin) || MATCHKW(lex, colon));
                }
                else
                {
                    SYMLIST* instants;
                    TEMPLATEPARAMLIST* templateParams = TemplateGetParams(sp);
                    sp->templateParams =
                        TemplateMatching(lex, sp->templateParams, templateParams, sp, MATCHKW(lex, begin) || MATCHKW(lex, colon));
                    instants = sp->sb->parentTemplate->sb->instantiations;
                    while (instants)
                    {
                        SYMBOL* instant = instants->p;
                        TEMPLATEPARAMLIST* tpln = instant->templateParams->next;
                        TEMPLATEPARAMLIST* tpl = templateParams->next;
                        while (tpl && tpln)
                        {
                            tpln->argsym->name = tpl->argsym->name;
                            tpln = tpln->next;
                            tpl = tpl->next;
                        }
                        instants = instants->next;
                    }
                }
                SetLinkerNames(sp, lk_cdecl);
            }
        }
        else if (MATCHKW(lex, lt))
        {
            if (inTemplate)
            {
                // instantiation
                TEMPLATEPARAMLIST* templateParams = TemplateGetParams(sp);
                lex = GetTemplateArguments(lex, funcsp, nullptr, &templateParams->p->bySpecialization.types);
            }
            else if (sp->sb->templateLevel)
            {
                if ((MATCHKW(lex, begin) || MATCHKW(lex, colon)))
                {
                    errorsym(ERR_IS_ALREADY_DEFINED_AS_A_TEMPLATE, sp);
                }
                else
                {
                    TEMPLATEPARAMLIST* lst = nullptr;
                    lex = GetTemplateArguments(lex, funcsp, nullptr, &lst);
                    sp = GetClassTemplate(sp, lst, false);
                }
            }
        }
        else if (sp->sb->templateLevel && (MATCHKW(lex, begin) || MATCHKW(lex, colon)))
        {
            errorsym(ERR_IS_ALREADY_DEFINED_AS_A_TEMPLATE, sp);
        }
    }
    if (sp)
    {
        if (uuid)
            sp->sb->uuid = uuid;
        if (access != sp->sb->access && sp->tp->syms && (MATCHKW(lex, begin) || MATCHKW(lex, colon)))
        {
            errorsym(ERR_CANNOT_REDEFINE_ACCESS_FOR, sp);
        }
        lex = innerDeclStruct(lex, funcsp, sp, inTemplate, defaultAccess, isfinal, defd);
        *tp = sp->tp;
    }
    basisAttribs = oldAttribs;
    return lex;
}
static LEXEME* enumbody(LEXEME* lex, SYMBOL* funcsp, SYMBOL* spi, enum e_sc storage_class, enum e_ac access, TYPE* fixedType,
                        bool scoped)
{
    long long enumval = 0;
    TYPE* unfixedType;
    bool fixed = Optimizer::cparams.prm_cplusplus ? true : false;
    unfixedType = spi->tp->btp;
    if (!unfixedType)
        unfixedType = &stdint;
    lex = getsym();
    spi->sb->declaring = true;
    if (spi->tp->syms)
    {
        preverrorsym(ERR_ENUM_CONSTANTS_DEFINED, spi, spi->sb->declfile, spi->sb->declline);
    }
    spi->tp->syms = CreateHashTable(1); /* holds a list of all the enum values, e.g. for debug info */
    if (!MATCHKW(lex, end))
    {
        while (lex)
        {
            if (ISID(lex))
            {
                SYMBOL* sp;
                TYPE* tp;
                if (Optimizer::cparams.prm_cplusplus)
                {
                    tp = (TYPE*)Alloc(sizeof(TYPE));
                    if (fixedType)
                        *tp = *fixedType;
                    else
                        *tp = *unfixedType;
                    tp->scoped = scoped;  // scoped the constants type as well for error checking
                    tp->btp = spi->tp;    // the integer type gets a base type which is the enumeration for error checking
                    tp->rootType = tp;
                    tp->enumConst = true;
                    tp->sp = spi;
                }
                else
                {
                    tp = (TYPE*)Alloc(sizeof(TYPE));
                    tp->type = bt_int;
                    tp->size = getSize(bt_int);
                    tp->rootType = tp;
                }
                sp = makeID(sc_enumconstant, tp, 0, litlate(lex->value.s.a));
                sp->name = sp->sb->decoratedName = litlate(lex->value.s.a);
                sp->sb->declcharpos = lex->charindex;
                sp->sb->declline = sp->sb->origdeclline = lex->errline;
                sp->sb->realdeclline = lex->linedata->lineno;
                sp->sb->declfile = sp->sb->origdeclfile = lex->errfile;
                sp->sb->declfilenum = lex->linedata->fileindex;
                sp->sb->parentClass = spi->sb->parentClass;
                sp->sb->access = access;
                browse_variable(sp);
                if (Optimizer::cparams.prm_cplusplus)
                {
                    if (!sp->tp->scoped)  // dump it into the parent table unless it is a C++ scoped enum
                        InsertSymbol(sp, storage_class, lk_c, false);
                }
                else  // in C dump it into the globals
                {
                    if (funcsp)
                        insert(sp, localNameSpace->valueData->syms);
                    else
                        insert(sp, globalNameSpace->valueData->syms);
                }
                insert(sp, spi->tp->syms);
                lex = getsym();
                if (MATCHKW(lex, assign))
                {
                    TYPE* tp = nullptr;
                    EXPRESSION* exp = nullptr;
                    lex = getsym();
                    lex = optimized_expression(lex, funcsp, nullptr, &tp, &exp, false);
                    if (tp && isintconst(exp))  // type is redefined to nullptr here, is this intentional?
                    {
                        if (Optimizer::cparams.prm_cplusplus)
                        {
                            if (!fixedType)
                            {
                                if (tp->type != bt_bool)
                                {
                                    if (!comparetypes(tp, unfixedType, true))
                                        fixed = false;
                                    unfixedType = tp;
                                    sp->tp->type = tp->type;
                                    sp->tp->size = tp->size;
                                    //                                *sp->tp = *tp;
                                }
                            }
                            else
                            {
                                if (tp->type != fixedType->type)
                                {
                                    long long v = 1;
                                    long long n = ~((v << (fixedType->size * 8 - 1)) - 1);
                                    if ((exp->v.i & n) != 0 && (exp->v.i & n) != n)
                                        error(ERR_ENUMERATION_OUT_OF_RANGE_OF_BASE_TYPE);
                                    cast(fixedType, &exp);
                                    optimize_for_constants(&exp);
                                }
                            }
                        }
                        enumval = exp->v.i;
                    }
                    else
                    {
                        if (!templateNestingCount)
                            error(ERR_CONSTANT_VALUE_EXPECTED);
                        errskim(&lex, skim_end);
                    }
                }
                sp->sb->value.i = enumval++;
                if (Optimizer::cparams.prm_cplusplus && spi->tp->btp)
                {
                    if (fixedType)
                    {
                        long long v = 1;
                        long long n = ~((v << (fixedType->size * 8 - 1)) - 1);
                        if ((sp->sb->value.i & n) != 0 && (sp->sb->value.i & n) != n)
                            error(ERR_ENUMERATION_OUT_OF_RANGE_OF_BASE_TYPE);
                    }
                    else
                    {
                        long long v = 1;
                        long long n = ~((v << (spi->tp->size * 8)) - 1);
                        if ((sp->sb->value.i & n) != 0 && (sp->sb->value.i & n) != n)
                        {
                            spi->tp->btp->type = bt_long_long;
                            spi->tp->size = spi->tp->btp->size = getSize(bt_long_long);
                            unfixedType = spi->tp->btp;
                        }
                    }
                }
                if (!MATCHKW(lex, comma))
                    break;
                else
                {
                    lex = getsym();
                    if (KW(lex) == end)
                    {
                        if (Optimizer::cparams.prm_ansi && !Optimizer::cparams.prm_c99 && !Optimizer::cparams.prm_cplusplus)
                        {
                            error(ERR_ANSI_ENUM_NO_COMMA);
                        }
                        break;
                    }
                }
            }
            else
            {
                error(ERR_IDENTIFIER_EXPECTED);
                errskim(&lex, skim_end);
                break;
            }
        }
    }
    if (!lex)
        error(ERR_UNEXPECTED_EOF);
    else if (!MATCHKW(lex, end))
    {
        errorint(ERR_NEEDY, '}');
        errskim(&lex, skim_end);
        skip(&lex, end);
    }
    else
        lex = getsym();
    if (fixed)
        spi->tp->fixed = true;
    spi->sb->declaring = false;
    return lex;
}
static LEXEME* declenum(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, enum e_sc storage_class, enum e_ac access, bool opaque, bool* defd)
{
    HASHTABLE* table;
    const char* tagname;
    char newName[4096];
    int charindex;
    bool noname = false;
    bool scoped = false;
    TYPE* fixedType = nullptr;
    SYMBOL* sp;
    NAMESPACEVALUELIST* nsv;
    SYMBOL* strSym;
    int declline = lex->errline;
    int realdeclline = lex->linedata->lineno;
    bool anonymous = false;
    enum e_lk linkage1 = lk_none, linkage2 = lk_none, linkage3 = lk_none;
    *defd = false;
    lex = getsym();
    auto oldAttribs = basisAttribs;
    basisAttribs = {0};
    ParseAttributeSpecifiers(&lex, funcsp, true);
    if (Optimizer::cparams.prm_cplusplus && (MATCHKW(lex, kw_class) || MATCHKW(lex, kw_struct)))
    {
        scoped = true;
        lex = getsym();
    }
    if (MATCHKW(lex, kw__declspec))
    {
        lex = getsym();
        lex = parse_declspec(lex, &linkage1, &linkage2, &linkage3);
    }
    if (ISID(lex))
    {
        charindex = lex->charindex;
        tagname = litlate(lex->value.s.a);
    }
    else
    {
        noname = true;
        if (!MATCHKW(lex, begin) && !MATCHKW(lex, classsel))
            errorint(ERR_NEEDY, '{');
        tagname = AnonymousTypeName();
        charindex = -1;
        anonymous = true;
    }

    strcpy(newName, tagname);
    lex = tagsearch(lex, newName, &sp, &table, &strSym, &nsv, storage_class);
    if (Optimizer::cparams.prm_cplusplus && KW(lex) == colon)
    {
        lex = getsym();
        lex = get_type_id(lex, &fixedType, funcsp, sc_cast, false, true, false);
        if (!fixedType || !isint(fixedType))
        {
            error(ERR_NEED_INTEGER_TYPE);
        }
        else
        {
            fixedType = basetype(fixedType);  // ignore qualifiers
        }
    }
    else if (opaque)
    {
        error(ERR_CANNOT_OMIT_ENUMERATION_BASE);
    }
    else if (scoped)
    {
        // scoped type with unspecified base is fixed...
        fixedType = &stdint;
    }
    if (!sp)
    {
        sp = SymAlloc();
        if (!strcmp(newName, tagname))
            sp->name = tagname;
        else
            sp->name = litlate(newName);
        sp->sb->access = access;
        sp->sb->anonymous = anonymous;
        sp->sb->storage_class = sc_type;
        sp->tp = (TYPE*)Alloc(sizeof(TYPE));
        sp->tp->type = bt_enum;
        sp->tp->rootType = sp->tp;
        if (fixedType)
        {
            sp->tp->fixed = true;
            sp->tp->btp = fixedType;
        }
        else
        {
            sp->tp->btp = (TYPE*)Alloc(sizeof(TYPE));
            sp->tp->btp->type = bt_int;
            sp->tp->btp->size = getSize(bt_int);
            sp->tp->btp->rootType = sp->tp->btp;
        }
        sp->tp->scoped = scoped;
        sp->tp->size = sp->tp->btp->size;
        sp->tp = AttributeFinish(sp, sp->tp);
        sp->sb->declcharpos = charindex;
        sp->sb->declline = sp->sb->origdeclline = declline;
        sp->sb->realdeclline = realdeclline;
        sp->sb->declfile = sp->sb->origdeclfile = lex->errfile;
        sp->sb->declfilenum = lex->linedata->fileindex;
        if (storage_class == sc_member || storage_class == sc_mutable)
            sp->sb->parentClass = getStructureDeclaration();
        if (nsv)
            sp->sb->parentNameSpace = nsv->valueData->name;
        sp->sb->anonymous = charindex == -1;
        SetLinkerNames(sp, lk_cdecl);
        browse_variable(sp);
        insert(sp, Optimizer::cparams.prm_cplusplus && !sp->sb->parentClass ? globalNameSpace->valueData->tags : table);
    }
    else if (sp->tp->type != bt_enum)
    {
        errorsym(ERR_ORIGINAL_TYPE_NOT_ENUMERATION, sp);
    }
    else if (scoped != (bool)sp->tp->scoped || (fixedType && sp->tp->btp->type != fixedType->type))
    {
        error(ERR_REDEFINITION_OF_ENUMERATION_SCOPE_OR_BASE_TYPE);
    }
    if (noname && (scoped || opaque || KW(lex) != begin))
    {
        error(ERR_ENUMERATED_TYPE_NEEDS_NAME);
    }
    if (KW(lex) == begin)
    {
        if (scoped)
            enumSyms = sp;
        lex = enumbody(lex, funcsp, sp, storage_class, access, fixedType, scoped);
        enumSyms = nullptr;
        *defd = true;
    }
    else if (!Optimizer::cparams.prm_cplusplus && Optimizer::cparams.prm_ansi && !sp->tp->syms)
    {
        errorsym(ERR_ANSI_ENUM_NEEDS_BODY, sp);
    }
    sp->tp->sp = sp;
    *tp = sp->tp;
    basisAttribs = oldAttribs;
    return lex;
}
static LEXEME* getStorageClass(LEXEME* lex, SYMBOL* funcsp, enum e_sc* storage_class, enum e_lk* linkage,
                               Optimizer::ADDRESS* address, bool* blocked, bool* isExplicit, enum e_ac access)
{
    (void)access;
    bool found = false;
    enum e_sc oldsc;
    while (KWTYPE(lex, TT_STORAGE_CLASS))
    {
        switch (KW(lex))
        {
            case kw_extern:
                oldsc = *storage_class;
                if (*storage_class == sc_parameter || *storage_class == sc_member || *storage_class == sc_mutable)
                    errorstr(ERR_INVALID_STORAGE_CLASS, lex->kw->name);
                else
                    *storage_class = sc_external;
                lex = getsym();
                if (Optimizer::cparams.prm_cplusplus)
                {
                    if (lex->type == l_astr)
                    {
                        Optimizer::SLCHAR* ch = (Optimizer::SLCHAR*)lex->value.s.w;
                        char buf[256];
                        int i;
                        enum e_lk next = lk_none;
                        for (i = 0; i < ch->count; i++)
                        {
                            buf[i] = ch->str[i];
                        }
                        buf[i] = 0;
                        if (oldsc == sc_auto || oldsc == sc_register)
                            error(ERR_NO_LINKAGE_HERE);
                        if (!strcmp(buf, "C++"))
                            next = lk_cpp;
                        if (!strcmp(buf, "C"))
                            next = lk_c;
                        if (!strcmp(buf, "PASCAL"))
                            next = lk_pascal;
                        if (!strcmp(buf, "stdcall"))
                            next = lk_stdcall;
                        if (!strcmp(buf, "stdcall"))
                            next = lk_fastcall;
                        if (next != lk_none)
                        {
                            *linkage = next;
                            lex = getsym();
                            if (MATCHKW(lex, begin))
                            {
                                *blocked = true;
                                lex = getsym();
                                while (lex && !MATCHKW(lex, end))
                                {
                                    lex =
                                        declare(lex, nullptr, nullptr, sc_global, *linkage, nullptr, true, false, false, ac_public);
                                }
                                needkw(&lex, end);
                                return lex;
                            }
                        }
                        else
                        {
                            char buf[512], *q = buf;
                            LCHAR* p = ch->str;
                            int count = ch->count;
                            while (count--)
                                *q++ = *p++;
                            *q = 0;
                            errorstr(ERR_UNKNOWN_LINKAGE, buf);
                            lex = getsym();
                        }
                    }
                }
                break;
            case kw_virtual:
                if (*storage_class != sc_member)
                {
                    errorstr(ERR_INVALID_STORAGE_CLASS, lex->kw->name);
                }
                else
                {
                    *storage_class = sc_virtual;
                }
                lex = getsym();
                break;
            case kw_explicit:
                if (*storage_class != sc_member)
                {
                    error(ERR_EXPLICIT_CONSTRUCTOR_OR_CONVERSION_FUNCTION);
                }
                else
                {
                    if (isExplicit)
                        *isExplicit = true;
                    else
                        error(ERR_EXPLICIT_CONSTRUCTOR_OR_CONVERSION_FUNCTION);
                }
                lex = getsym();
                break;
            case kw_mutable:
                if (*storage_class != sc_member)
                {
                    error(ERR_EXPLICIT_CONSTRUCTOR_OR_CONVERSION_FUNCTION);
                }
                else
                {
                    *storage_class = sc_mutable;
                }
                lex = getsym();
                break;
            case kw_static:
                if (*storage_class == sc_parameter ||
                    (!Optimizer::cparams.prm_cplusplus && (*storage_class == sc_member || *storage_class == sc_mutable)))
                    errorstr(ERR_INVALID_STORAGE_CLASS, lex->kw->name);
                else if (*storage_class == sc_auto)
                    *storage_class = sc_localstatic;
                else
                    *storage_class = sc_static;
                lex = getsym();
                break;
            case kw__absolute:
                if (*storage_class == sc_parameter || *storage_class == sc_member || *storage_class == sc_mutable)
                    errorstr(ERR_INVALID_STORAGE_CLASS, lex->kw->name);
                else
                    *storage_class = sc_absolute;
                lex = getsym();
                if (MATCHKW(lex, openpa))
                {
                    TYPE* tp = nullptr;
                    EXPRESSION* exp = nullptr;
                    lex = getsym();
                    lex = optimized_expression(lex, funcsp, nullptr, &tp, &exp, false);
                    if (tp && isintconst(exp))
                        *address = exp->v.i;
                    else
                        error(ERR_CONSTANT_VALUE_EXPECTED);
                    if (!MATCHKW(lex, closepa))
                        needkw(&lex, closepa);
                    lex = getsym();
                }
                else
                    error(ERR_ABSOLUTE_NEEDS_ADDRESS);
                break;
            case kw_auto:
                if (*storage_class != sc_auto)
                    errorstr(ERR_INVALID_STORAGE_CLASS, "auto");
                else
                    *storage_class = sc_auto;
                lex = getsym();
                break;
            case kw_register:
                if (*storage_class != sc_auto && *storage_class != sc_parameter)
                    errorstr(ERR_INVALID_STORAGE_CLASS, "register");
                else if (*storage_class != sc_parameter)
                    *storage_class = sc_register;
                lex = getsym();
                break;
            case kw_typedef:
                if (*storage_class == sc_parameter)
                    errorstr(ERR_INVALID_STORAGE_CLASS, "typedef");
                else
                    *storage_class = sc_typedef;
                lex = getsym();
                break;
            default:
                break;
        }
        if (found)
            error(ERR_TOO_MANY_STORAGE_CLASS_SPECIFIERS);
        found = true;
    }
    return lex;
}
static LEXEME* getPointerQualifiers(LEXEME* lex, TYPE** tp, bool allowstatic)
{
    while (KWTYPE(lex, TT_TYPEQUAL) || (allowstatic && MATCHKW(lex, kw_static)))
    {
        TYPE* tpn;
        TYPE* tpl;
        if (MATCHKW(lex, kw__intrinsic))
        {
            lex = getsym();
            continue;
        }
        tpn = (TYPE*)Alloc(sizeof(TYPE));
        if (*tp)
            tpn->size = (*tp)->size;
        switch (KW(lex))
        {
            case kw_static:
                tpn->type = bt_static;
                break;
            case kw_const:
                tpn->type = bt_const;
                break;
            case kw_atomic:
                lex = getsym();
                if (MATCHKW(lex, openpa))
                {
                    // being used as  a type specifier not a qualifier
                    lex = backupsym();
                    return lex;
                }
                lex = backupsym();
                tpn->type = bt_atomic;
                break;
            case kw_volatile:
                tpn->type = bt_volatile;
                break;
            case kw_restrict:
                tpn->type = bt_restrict;
                break;
            case kw__far:
                tpn->type = bt_far;
                break;
            case kw__near:
                tpn->type = bt_near;
                break;
            case kw___va_list__:
                tpn->type = bt_va_list;
                break;
            default:
                break;
        }
        /*
                tpl = *tp;
                while (tpl && tpl->type != bt_pointer)
                {
                    if (tpl->type == tpn->type)
                        errorstr(ERR_DUPLICATE_TYPE_QUALIFIER, lex->kw->name);
                    tpl = tpl->btp;
                }
        */
        tpn->btp = *tp;
        if (*tp)
            tpn->rootType = (*tp)->rootType;
        *tp = tpn;
        lex = getsym();
    }
    return lex;
}
LEXEME* parse_declspec(LEXEME* lex, enum e_lk* linkage, enum e_lk* linkage2, enum e_lk* linkage3)
{
    (void)linkage;
    if (needkw(&lex, openpa))
    {
        while (1)
        {
            if (ISID(lex))
            {
                if (!strcmp(lex->value.s.a, "noreturn"))
                {
                    if (*linkage3 != lk_none)
                        error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                    *linkage3 = lk_noreturn;
                }
                else if (!strcmp(lex->value.s.a, "align"))
                {
                    lex = getsym();
                    if (needkw(&lex, openpa))
                    {
                        TYPE* tp = nullptr;
                        EXPRESSION* exp = nullptr;

                        lex = optimized_expression(lex, nullptr, nullptr, &tp, &exp, false);
                        if (!tp || !isint(tp))
                            error(ERR_NEED_INTEGER_TYPE);
                        else if (!isintconst(exp))
                            error(ERR_CONSTANT_VALUE_EXPECTED);
                        int align = exp->v.i;
                        LEXEME* pos = lex;
                        if (needkw(&lex, closepa))
                            lex = prevsym(pos);
                        basisAttribs.inheritable.structAlign = align;
                        if (basisAttribs.inheritable.structAlign > 0x10000 ||
                            (basisAttribs.inheritable.structAlign & (basisAttribs.inheritable.structAlign - 1)) != 0)
                            error(ERR_INVALID_ALIGNMENT);
                    }
                }
                else if (!strcmp(lex->value.s.a, "dllimport") || !strcmp(lex->value.s.a, "__dllimport__"))
                {
                    if (*linkage2 != lk_none)
                        error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                    *linkage2 = lk_import;
                }
                else if (!strcmp(lex->value.s.a, "dllexport") || !strcmp(lex->value.s.a, "__dllexport__"))
                {
                    if (*linkage2 != lk_none)
                        error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                    *linkage2 = lk_export;
                }
                else
                {
                    error(ERR_IGNORING__DECLSPEC);
                }
            }
            else if (MATCHKW(lex, kw_noreturn))
            {
                if (*linkage3 != lk_none)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage3 = lk_noreturn;
            }
            else
            {
                error(ERR_IGNORING__DECLSPEC);
                break;
            }
            lex = getsym();
            if (MATCHKW(lex, openpa))
            {
                errskim(&lex, skim_closepa);
            }
            if (!MATCHKW(lex, comma))
                break;
            lex = getsym();
        }
        needkw(&lex, closepa);
    }
    return lex;
}
static LEXEME* getLinkageQualifiers(LEXEME* lex, enum e_lk* linkage, enum e_lk* linkage2, enum e_lk* linkage3)
{
    while (KWTYPE(lex, TT_LINKAGE))
    {
        enum e_kw kw = KW(lex);
        lex = getsym();
        switch (kw)
        {
            case kw__cdecl:
                if (*linkage != lk_none && *linkage != lk_cdecl)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = lk_cdecl;
                break;
            case kw__stdcall:
                if (*linkage != lk_none && *linkage != lk_stdcall && (!Optimizer::cparams.prm_cplusplus || *linkage != lk_c))
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = lk_stdcall;
                break;
            case kw__fastcall:
                if (*linkage != lk_none && *linkage != lk_fastcall)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = lk_fastcall;
                break;
            case kw__interrupt:
                if (*linkage != lk_none && *linkage != lk_interrupt)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = lk_interrupt;
                break;
            case kw__fault:
                if (*linkage != lk_none && *linkage != lk_fault)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = lk_fault;
                break;
            case kw_inline:
                if (*linkage != lk_none && *linkage != lk_inline)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = lk_inline;
                break;
            case kw_noreturn:
                if (*linkage3 != lk_none)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage3 = lk_noreturn;
                break;
            case kw_thread_local:
                if (*linkage3 != lk_none)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage3 = lk_threadlocal;
                break;
            case kw__export:
                if (*linkage2 != lk_none)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage2 = lk_export;
                break;
            case kw__declspec:
                lex = parse_declspec(lex, linkage, linkage2, linkage3);
                break;
            case kw__rtllinkage:
                *linkage2 = getDefaultLinkage();
                break;
            case kw__entrypoint:
                if (*linkage3 != lk_none)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage3 = lk_entrypoint;
                break;

            case kw__property:
                if (*linkage2 != lk_none)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage2 = lk_property;
                break;
            case kw__msil_rtl:
                if (*linkage2 != lk_none)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage2 = lk_msil_rtl;
                break;
            case kw__unmanaged:
                if (*linkage2 != lk_none)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage2 = lk_unmanaged;
                break;
            case kw__import:
                if (*linkage2 != lk_none)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage2 = lk_import;
                importFile = nullptr;
                if (MATCHKW(lex, openpa))
                {
                    lex = getsym();
                    if (lex->type == l_astr)
                    {
                        int i, len = ((Optimizer::SLCHAR*)lex->value.s.w)->count;
                        importFile = (char*)Alloc(len + 1);
                        for (i = 0; i < len; i++)
                            importFile[i] = (char)((Optimizer::SLCHAR*)lex->value.s.w)->str[i];
                        lex = getsym();
                    }
                    needkw(&lex, closepa);
                }
                break;
            default:
                break;
        }
    }
    return lex;
}
LEXEME* getQualifiers(LEXEME* lex, TYPE** tp, enum e_lk* linkage, enum e_lk* linkage2, enum e_lk* linkage3, bool* asFriend)
{
    while (KWTYPE(lex, (TT_TYPEQUAL | TT_LINKAGE)))
    {
        if (asFriend && MATCHKW(lex, kw_friend))
        {
            *asFriend = true;
            lex = getsym();
        }
        else
        {
            lex = getPointerQualifiers(lex, tp, false);
            if (MATCHKW(lex, kw_atomic))
                break;
            lex = getLinkageQualifiers(lex, linkage, linkage2, linkage3);
        }
    }
    return lex;
}
static LEXEME* nestedTypeSearch(LEXEME* lex, SYMBOL** sym)
{
    *sym = nullptr;
    lex = nestedSearch(lex, sym, nullptr, nullptr, nullptr, nullptr, false, sc_global, false, true);
    if (!*sym || !istype((*sym)))
    {
        error(ERR_TYPE_NAME_EXPECTED);
    }
    return lex;
}
static bool isPointer(LEXEME* lex)
{
    while (KWTYPE(lex, (TT_TYPEQUAL | TT_LINKAGE)))
        lex = getsym();
    if (ISKW(lex))
        switch (KW(lex))
        {
            case andx:
            case land:
            case star:
                return true;
            default:
                return false;
        }
    return false;
}
LEXEME* getBasicType(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, SYMBOL** strSym_out, bool inTemplate, enum e_sc storage_class,
                     enum e_lk* linkage_in, enum e_lk* linkage2_in, enum e_lk* linkage3_in, enum e_ac access, bool* notype,
                     bool* defd, int* consdest, bool* templateArg, bool isTypedef, bool templateErr, bool inUsing)
{
    enum e_bt type = bt_none;
    TYPE* tn = nullptr;
    TYPE* quals = nullptr;
    TYPE** tl;
    bool extended;
    bool iscomplex = false;
    bool imaginary = false;
    bool flagerror = false;
    bool foundint = false;
    int foundunsigned = 0;
    int foundsigned = 0;
    bool foundtypeof = false;
    bool foundsomething = false;
    bool int64 = false;
    enum e_lk linkage = lk_none;
    enum e_lk linkage2 = lk_none;
    enum e_lk linkage3 = lk_none;

    *defd = false;
    while (KWTYPE(lex, TT_BASETYPE) || MATCHKW(lex, kw_decltype))
    {
        if (foundtypeof)
            flagerror = true;
        switch (KW(lex))
        {
            case kw_int:
                if (foundint)
                    flagerror = true;
                foundint = true;
                switch (type)
                {
                    case bt_unsigned:
                    case bt_short:
                    case bt_unsigned_short:
                    case bt_long:
                    case bt_unsigned_long:
                    case bt_long_long:
                    case bt_unsigned_long_long:
                    case bt_inative:
                    case bt_unative:
                        break;
                    case bt_none:
                    case bt_signed:
                        type = bt_int;
                        break;
                    default:
                        flagerror = true;
                }
                break;
            case kw_native:
                if (type == bt_unsigned)
                    type = bt_unative;
                else if (type == bt_int || type == bt_none)
                    type = bt_inative;
                else
                    flagerror = true;
                break;
            case kw_char:
                switch (type)
                {
                    case bt_none:
                        type = bt_char;
                        break;
                    case bt_int:
                    case bt_signed:
                        type = bt_signed_char;
                        break;
                    case bt_unsigned:
                        type = bt_unsigned_char;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case kw_char16_t:
                if (type != bt_none)
                    flagerror = true;
                type = bt_char16_t;
                break;
            case kw_char32_t:
                if (type != bt_none)
                    flagerror = true;
                type = bt_char32_t;
                break;
            case kw_short:
                switch (type)
                {
                    case bt_none:
                    case bt_int:
                    case bt_signed:
                        type = bt_short;
                        break;
                    case bt_unsigned:
                        type = bt_unsigned_short;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case kw_long:
                switch (type)
                {
                    case bt_double:
                        if (iscomplex)
                            type = bt_long_double_complex;
                        else if (imaginary)
                            type = bt_long_double_imaginary;
                        else
                            type = bt_long_double;
                        break;
                    case bt_none:
                    case bt_int:
                    case bt_signed:
                        type = bt_long;
                        break;
                    case bt_unsigned:
                        type = bt_unsigned_long;
                        break;
                    case bt_long:
                        type = bt_long_long;
                        break;
                    case bt_unsigned_long:
                        type = bt_unsigned_long_long;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case kw_signed:
                if (foundsigned || foundunsigned)
                    flagerror = true;
                if (type == bt_none)
                    type = bt_signed;
                else if (type == bt_char)
                    type = bt_signed_char;
                foundsigned++;
                break;
            case kw_unsigned:
                if (foundsigned || foundunsigned)
                    flagerror = true;
                foundunsigned++;
                switch (type)
                {
                    case bt_char:
                        type = bt_unsigned_char;
                        break;
                    case bt_short:
                        type = bt_unsigned_short;
                        break;
                    case bt_long:
                        type = bt_unsigned_long;
                        break;
                    case bt_long_long:
                        type = bt_unsigned_long_long;
                        break;
                    case bt_none:
                    case bt_int:
                        type = bt_unsigned;
                        break;
                    case bt_inative:
                        type = bt_unative;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case kw___int64:
                switch (type)
                {
                    case bt_unsigned:
                        int64 = true;
                        type = bt_unsigned_long_long;
                        break;
                    case bt_none:
                    case bt_signed:
                        int64 = true;
                        type = bt_long_long;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case kw_wchar_t:
                if (type == bt_none)
                    type = bt_wchar_t;
                else
                    flagerror = true;
                break;
            case kw_auto:
                if (type == bt_none)
                    type = bt_auto;
                else
                    flagerror = true;
                break;
            case kw_bool:
                if (type == bt_none)
                    if (!(Optimizer::chosenAssembler->arch->boolmode & ABM_USESIZE))
                    {
                        if (storage_class == sc_auto)
                            if (!(Optimizer::chosenAssembler->arch->boolmode & ABM_LOCALBITS))
                            {
                                type = bt_bool;
                            }
                            else
                            {
                                type = bt_bit;
                            }
                        else if (storage_class == sc_global)
                        {
                            if (!(Optimizer::chosenAssembler->arch->boolmode & ABM_GLOBALBITS))
                            {
                                type = bt_bool;
                            }
                            else
                            {
                                type = bt_bit;
                            }
                        }
                        else
                            type = bt_bool;
                    }
                    else
                        type = bt_bool;
                else
                    flagerror = true;
                break;
            case kw_float:
                if (type == bt_none)
                    if (iscomplex)
                        type = bt_float_complex;
                    else if (imaginary)
                        type = bt_float_imaginary;
                    else
                        type = bt_float;
                else
                    flagerror = true;
                break;
            case kw_double:
                if (type == bt_none)
                    if (iscomplex)
                        type = bt_double_complex;
                    else if (imaginary)
                        type = bt_double_imaginary;
                    else
                        type = bt_double;
                else if (type == bt_long)
                    if (iscomplex)
                        type = bt_long_double_complex;
                    else if (imaginary)
                        type = bt_long_double_imaginary;
                    else
                        type = bt_long_double;
                else
                    flagerror = true;
                break;
            case kw__Complex:
                switch (type)
                {
                    case bt_float:
                        type = bt_float_complex;
                        break;
                    case bt_double:
                        type = bt_double_complex;
                        break;
                    case bt_long_double:
                        type = bt_long_double_complex;
                        break;
                    case bt_none:
                        if (iscomplex || imaginary)
                            flagerror = true;
                        iscomplex = true;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case kw__Imaginary:
                switch (type)
                {
                    case bt_float:
                        type = bt_float_imaginary;
                        break;
                    case bt_double:
                        type = bt_double_imaginary;
                        break;
                    case bt_long_double:
                        type = bt_long_double_imaginary;
                        break;
                    case bt_none:
                        if (imaginary || iscomplex)
                            flagerror = true;
                        imaginary = true;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case kw___object:
                if (type != bt_none)
                    flagerror = true;
                else
                    type = bt___object;
                break;
            case kw___string:
                if (type != bt_none)
                    flagerror = true;
                else
                    type = bt___string;
                break;
            case kw_struct:
            case kw_class:
            case kw_union:
                inTemplateType = false;
                if (foundsigned || foundunsigned || type != bt_none)
                    flagerror = true;
                lex = declstruct(lex, funcsp, &tn, inTemplate, storage_class, access, defd);
                goto exit;
            case kw_enum:
                if (foundsigned || foundunsigned || type != bt_none)
                    flagerror = true;
                lex = declenum(lex, funcsp, &tn, storage_class, access, false, defd);
                goto exit;
            case kw_void:
                if (type != bt_none)
                    flagerror = true;
                type = bt_void;
                break;
            case kw_decltype:
                //                lex = getDeclType(lex, funcsp, &tn);
                type = bt_void; /* won't really be used */
                foundtypeof = true;
                if (foundsomething)
                    flagerror = true;
                goto founddecltype;
            case kw_typeof:
                type = bt_void; /* won't really be used */
                foundtypeof = true;
                if (foundsomething)
                    flagerror = true;
                lex = getsym();
                if (MATCHKW(lex, openpa))
                {
                    EXPRESSION* exp;
                    lex = getsym();
                    lex = expression_no_check(lex, nullptr, nullptr, &tn, &exp, 0);
                    if (tn)
                    {
                        optimize_for_constants(&exp);
                    }
                    if (!tn)
                    {
                        error(ERR_EXPRESSION_SYNTAX);
                    }
                    if (!MATCHKW(lex, closepa))
                        needkw(&lex, closepa);
                }
                else if (ISID(lex) || MATCHKW(lex, classsel))
                {
                    SYMBOL* sp;
                    lex = nestedSearch(lex, &sp, nullptr, nullptr, nullptr, nullptr, false, storage_class, true, true);
                    if (sp)
                        tn = sp->tp;
                }
                else
                {
                    error(ERR_IDENTIFIER_EXPECTED);
                    errskim(&lex, skim_semi_declare);
                }
                break;
            case kw_atomic:
                lex = getsym();
                if (needkw(&lex, openpa))
                {
                    tn = nullptr;
                    lex = get_type_id(lex, &tn, funcsp, sc_cast, false, true, false);
                    if (tn)
                    {
                        TYPE *tq = (TYPE*)Alloc(sizeof(TYPE)), *tz;
                        tq->type = bt_atomic;
                        tq->btp = quals;
                        tq->rootType = tq;
                        quals = tq;
                        tz = tn;
                        while (tz->type == bt_typedef)
                            tz = tz->btp;
                        if (basetype(tz) != tz)
                            error(ERR_ATOMIC_TYPE_SPECIFIER_NO_QUALS);
                    }
                    else
                    {
                        error(ERR_EXPRESSION_SYNTAX);
                    }
                    if (!MATCHKW(lex, closepa))
                    {
                        needkw(&lex, closepa);
                        errskim(&lex, skim_closepa);
                        skip(&lex, closepa);
                    }
                }
                else
                {
                    errskim(&lex, skim_closepa);
                    skip(&lex, closepa);
                }
                break;
            default:
                break;
        }
        foundsomething = true;
        lex = getsym();
        lex = getQualifiers(lex, &quals, &linkage, &linkage2, &linkage3, nullptr);
        if (linkage != lk_none)
        {
            *linkage_in = linkage;
        }
        if (linkage2 != lk_none)
            *linkage2_in = linkage2;
        if (linkage3 != lk_none)
            *linkage3_in = linkage3;
    }
    if (type == bt_signed)  // bt_signed is just an internal placeholder
        type = bt_int;
founddecltype:
    if (!foundsomething)
    {
        bool typeName = false;
        type = bt_int;
        if (MATCHKW(lex, kw_typename))
        {
            typeName = true;
            //            foundsomething = true;
            lex = getsym();
        }
        if (iscomplex || imaginary)
            error(ERR_MISSING_TYPE_SPECIFIER);
        else if (ISID(lex) || MATCHKW(lex, classsel) || MATCHKW(lex, complx) || MATCHKW(lex, kw_decltype))
        {
            NAMESPACEVALUELIST* nsv = nullptr;
            SYMBOL* strSym = nullptr;
            SYMBOL* sp = nullptr;
            bool destructor = false;
            LEXEME* placeholder = lex;
            bool inTemplate = false;  // hides function parameter
            lex = nestedSearch(lex, &sp, &strSym, &nsv, &destructor, &inTemplate, false, storage_class, false, true);
            if (sp && (istype(sp) || (sp->sb && ((sp->sb->storage_class == sc_type && inTemplate) ||
                                                 (sp->sb->storage_class == sc_typedef && sp->sb->templateLevel)))))
            {
                if (sp->sb && sp->sb->attribs.uninheritable.deprecationText && !isstructured(sp->tp))
                    deprecateMessage(sp);
                lex = getsym();
                if (sp->sb && sp->sb->storage_class == sc_typedef && sp->sb->templateLevel)
                {
                    if (MATCHKW(lex, lt))
                    {
                        // throwaway
                        TEMPLATEPARAMLIST* lst = nullptr;
                        SYMBOL* sp1;
                        lex = GetTemplateArguments(lex, funcsp, sp, &lst);
                        sp1 = GetTypeAliasSpecialization(sp, lst);
                        if (sp1)
                        {
                            sp = sp1;
                            if (!inUsing || !templateNestingCount)
                            {
                                if (isstructured(sp->tp))
                                    sp->tp = PerformDeferredInitialization(sp->tp, funcsp);
                                else
                                    sp->tp = SynthesizeType(sp->tp, nullptr, false);
                            }
                        }
                        tn = sp->tp;
                        foundsomething = true;
                    }
                    else
                    {
                        if (!noSpecializationError && !instantiatingTemplate)
                            errorsym(ERR_NEED_SPECIALIZATION_PARAMETERS, sp);
                        tn = sp->tp;
                    }
                }
                else
                {
                    SYMBOL* ssp = getStructureDeclaration();
                    TYPE* tpx = basetype(sp->tp);
                    foundsomething = true;
                    if (tpx->type == bt_templateparam)
                    {
                        tn = nullptr;
                        if (templateArg)
                            *templateArg = true;
                        if (!tpx->templateParam->p->packed)
                        {
                            if (tpx->templateParam->p->type == kw_typename)
                            {
                                tn = tpx->templateParam->p->byClass.val;
                                if (*tp && tn)
                                {
                                    // should only be const vol specifiers
                                    TYPE* tpy = *tp;
                                    while (tpy->btp)
                                        tpy = tpy->btp;
                                    tpy->btp = tpx;
                                    tn = SynthesizeType(*tp, tpx->templateParam, false);
                                    *tp = nullptr;
                                }
                                if (inTemplate)
                                {
                                    if (MATCHKW(lex, lt))
                                    {
                                        // throwaway
                                        TEMPLATEPARAMLIST* lst = nullptr;
                                        lex = GetTemplateArguments(lex, funcsp, nullptr, &lst);
                                    }
                                    errorsym(ERR_NOT_A_TEMPLATE, sp);
                                }
                            }
                            else if (tpx->templateParam->p->type == kw_template)
                            {
                                if (MATCHKW(lex, lt))
                                {

                                    TEMPLATEPARAMLIST* lst = nullptr;
                                    SYMBOL* sp1 = tpx->templateParam->p->byTemplate.val;
                                    lex = GetTemplateArguments(lex, funcsp, sp1, &lst);
                                    if (sp1)
                                    {
                                        sp1 = GetClassTemplate(sp1, lst, !templateErr);
                                        tn = nullptr;
                                        if (sp1)
                                            tn = sp1->tp;
                                    }
                                    else if (templateNestingCount)
                                    {
                                        TEMPLATEPARAMLIST *told, **tnew;
                                        sp1 = clonesym(sp);
                                        sp1->tp = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
                                        *sp1->tp = *sp->tp;
                                        sp1->tp->rootType = sp1->tp;
                                        sp1->tp->sp = sp1;
                                        sp1->tp->templateParam =
                                            (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                        sp1->tp->templateParam->next = sp->tp->templateParam->next;
                                        sp1->tp->templateParam->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                                        *sp1->tp->templateParam->p = *sp->tp->templateParam->p;
                                        tnew = &sp1->tp->templateParam->p->byTemplate.args;
                                        told = *tnew;
                                        sp1->tp->templateParam->p->byTemplate.orig = sp->tp->templateParam;
                                        while (told && lst)
                                        {
                                            *tnew = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                            if (told->p->type == kw_new)
                                            {
                                                (*tnew)->p = told->p;
                                            }
                                            else if (!lst->argsym)
                                            {
                                                (*tnew)->p = lst->p;
                                            }
                                            else
                                            {
                                                SYMBOL* sp = classsearch(lst->argsym->name, false, false);
                                                if (sp && sp->tp->type == bt_templateparam)
                                                {
                                                    (*tnew)->p = sp->tp->templateParam->p;
                                                    (*tnew)->argsym = sp->tp->templateParam->argsym;
                                                }
                                                else
                                                {
                                                    (*tnew)->p = lst->p;
                                                    (*tnew)->argsym = lst->argsym;
                                                }
                                                lst = lst->next;
                                            }
                                            told = told->next;
                                            tnew = &(*tnew)->next;
                                        }
                                        sp = sp1;
                                    }
                                }
                                else
                                {
                                    if (!noSpecializationError && !instantiatingTemplate)
                                        errorsym(ERR_NEED_SPECIALIZATION_PARAMETERS, sp);
                                    tn = sp->tp;
                                }
                            }
                            else
                            {
                                diag("getBasicType: expected typename template param");
                            }
                        }
                        else if (expandingParams && tpx->type == bt_templateparam)
                        {
                            TEMPLATEPARAMLIST* packed = tpx->templateParam->p->byPack.pack;
                            int i;
                            for (i = 0; i < packIndex && packed; i++)
                                packed = packed->next;
                            if (packed)
                                tn = packed->p->byClass.val;
                        }
                        if (!tn)
                            tn = sp->tp;
                    }
                    else if (tpx->type == bt_templatedecltype)
                    {
                        if (templateArg)
                            *templateArg = true;
                        if (!templateNestingCount)
                            TemplateLookupTypeFromDeclType(tpx);
                    }
                    else if (tpx->type == bt_templateselector)
                    {
                        if (templateArg)
                            *templateArg = true;
                        if (!templateNestingCount)
                        {
                            tn = SynthesizeType(sp->tp, nullptr, false);
                        }
                        else
                        {
                            tn = sp->tp;
                        }
                        if (tpx->sp->sb->templateSelector->next->next && tpx->sp->sb->templateSelector->next->sp &&
                            !strcmp(tpx->sp->sb->templateSelector->next->sp->name, tpx->sp->sb->templateSelector->next->next->name))
                        {
                            if (destructor)
                            {
                                *consdest = CT_DEST;
                            }
                            else
                            {
                                *consdest = CT_CONS;
                            }
                            if (strSym_out)
                                *strSym_out = tpx->sp->sb->templateSelector->next->sp;
                            tn = tpx->sp->sb->templateSelector->next->sp->tp;
                            *notype = true;
                            goto exit;
                        }
                        else
                        {
                            // discard template params, they've already been gathered..
                            TEMPLATESELECTOR* l = tpx->sp->sb->templateSelector->next->next;
                            if (l)
                            {
                                while (l->next)
                                    l = l->next;

                                if (l->isTemplate)
                                {
                                    if (MATCHKW(lex, lt))
                                    {
                                        TEMPLATEPARAMLIST* current = nullptr;
                                        lex = GetTemplateArguments(lex, nullptr, nullptr, &current);
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        lex = getQualifiers(lex, &quals, &linkage, &linkage2, &linkage3, nullptr);
                        if (linkage != lk_none)
                        {
                            *linkage_in = linkage;
                        }
                        if (linkage2 != lk_none)
                            *linkage2_in = linkage2;
                        if (sp->sb->templateLevel)
                        {
                            TEMPLATEPARAMLIST* lst = nullptr;
                            if (templateNestingCount)
                                tn = sp->tp;
                            else
                                tn = nullptr;
                            if (MATCHKW(lex, lt))
                            {
                                if (sp->sb->parentTemplate)
                                    sp = sp->sb->parentTemplate;

                                lex = GetTemplateArguments(lex, funcsp, sp, &lst);
                                sp = GetClassTemplate(sp, lst, !templateErr);
                                if (sp)
                                {
                                    if (sp && (!templateNestingCount || inTemplateBody))
                                        sp->tp = PerformDeferredInitialization(sp->tp, funcsp);
                                }
                            }
                            else
                            {
                                if (!noSpecializationError && !instantiatingTemplate &&
                                    (!sp->sb->mainsym || (sp->sb->mainsym != strSym && sp->sb->mainsym != ssp)))
                                {
                                    if (instantiatingMemberFuncClass &&
                                        instantiatingMemberFuncClass->sb->parentClass == sp->sb->parentClass)
                                        sp = instantiatingMemberFuncClass;
                                    else
                                        errorsym(ERR_NEED_SPECIALIZATION_PARAMETERS, sp);
                                }
                            }
                            if (sp)
                                tn = sp->tp;
                        }
                        else
                        {
                            if (inTemplate)
                            {
                                if (MATCHKW(lex, lt))
                                {
                                    // throwaway
                                    TEMPLATEPARAMLIST* lst = nullptr;
                                    lex = GetTemplateArguments(lex, funcsp, nullptr, &lst);
                                }
                                errorsym(ERR_NOT_A_TEMPLATE, sp);
                            }
                            if (sp->tp->type == bt_typedef)
                            {
                                tn = sp->tp;
                                while (tn != basetype(sp->tp) && tn->type != bt_va_list) tn = tn->btp;
                            }
                            else
                            {
                                tn = sp->tp;
                            }
                            if (!templateNestingCount && isstructured(tn) && basetype(tn)->sp->sb->templateLevel &&
                                !basetype(tn)->sp->sb->instantiated)
                            {
                                sp = GetClassTemplate(basetype(tn)->sp, basetype(tn)->sp->templateParams->next, false);
                                if (sp)
                                    tn = PerformDeferredInitialization(sp->tp, funcsp);
                            }
                            //                        if (sp->sb->parentClass && !isAccessible(sp->sb->parentClass, ssp ? ssp :
                            //                        sp->sb->parentClass, sp, funcsp, ssp == sp->sb->parentClass ? ac_protected :
                            //                        ac_public, false))
                            //                           errorsym(ERR_CANNOT_ACCESS, sp);
                        }
                        // DAL
                        if (Optimizer::cparams.prm_cplusplus && sp && MATCHKW(lex, openpa) &&
                            ((strSym && ((strSym->sb->mainsym && strSym->sb->mainsym == sp->sb->mainsym) ||
                                         strSym == sp->sb->mainsym || sameTemplate(strSym->tp, sp->tp))) ||
                             (!strSym && (storage_class == sc_member || storage_class == sc_mutable) && ssp &&
                              ssp == sp->sb->mainsym)))
                        {
                            if (destructor)
                            {
                                *consdest = CT_DEST;
                            }
                            else
                            {
                                *consdest = CT_CONS;
                            }
                            if (ssp && strSym && ssp == strSym->sb->mainsym)
                                strSym = nullptr;
                            if (strSym_out)
                                *strSym_out = strSym;
                            *notype = true;
                            goto exit;
                        }
                        else if (destructor)
                        {
                            error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
                        }
                    }
                }
            }
            else if (strSym && basetype(strSym->tp)->type == bt_templateselector)
            {
                if (!templateNestingCount && !inTemplateSpecialization && allTemplateArgsSpecified(strSym, strSym->templateParams))
                    tn = SynthesizeType(strSym->tp, nullptr, false);
                else
                    tn = nullptr;
                if (!tn || tn->type == bt_any || basetype(tn)->type == bt_templateparam)
                    tn = strSym->tp;
                else
                    tn = PerformDeferredInitialization(tn, funcsp);
                foundsomething = true;
                lex = getsym();
            }
            else if (strSym && basetype(strSym->tp)->type == bt_templatedecltype)
            {
                tn = strSym->tp;
                foundsomething = true;
                lex = getsym();
            }
            else if (strSym && strSym->sb->templateLevel && !templateNestingCount)
            {
                STRUCTSYM s;
                tn = PerformDeferredInitialization(strSym->tp, funcsp);
                s.str = tn->sp;
                addStructureDeclaration(&s);
                sp = classsearch(lex->value.s.a, false, true);
                if (sp)
                {
                    tn = sp->tp;
                    foundsomething = true;
                }
                dropStructureDeclaration();
                lex = getsym();
            }
            else if (MATCHKW(lex, kw_decltype))
            {
                lex = getDeclType(lex, funcsp, &tn);
                if (tn)
                    foundsomething = true;
            }
            else if (isTypedef)
            {
                if (!templateNestingCount && !typeName)
                {
                    error(ERR_TYPE_NAME_EXPECTED);
                }
                tn = (TYPE*)Alloc(sizeof(TYPE));
                tn->type = bt_any;
                tn->rootType = tn;
                tn->size = getSize(bt_int);
                lex = getsym();
                foundsomething = true;
            }
            else if (Optimizer::cparams.prm_cplusplus)
            {
                if (typeName || ISID(lex))
                {
                    tn = (TYPE*)Alloc(sizeof(TYPE));
                    tn->type = bt_any;
                    tn->rootType = tn;
                    tn->size = getSize(bt_int);
                    if (lex->type == l_id)
                    {
                        SYMBOL* sp = makeID(sc_global, tn, nullptr, litlate(lex->value.s.a));
                        tn->sp = sp;
                    }
                    lex = getsym();
                    foundsomething = true;
                }
                else
                {
                    if (destructor)
                    {
                        error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
                    }
                    lex = prevsym(placeholder);
                }
            }
        }
        if (!foundsomething && (Optimizer::cparams.prm_c99 || Optimizer::cparams.prm_cplusplus))
        {
            if (notype)
                *notype = true;
            else
                error(ERR_MISSING_TYPE_SPECIFIER);
        }
    }
exit:
    if ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions && tn)
    {
        // select an unboxed variable type for use in compiler
        // will be converted back to boxed as needed
        TYPE* tnn = find_unboxed_type(tn);
        if (tnn)
        {
            tn = tnn;
            type = tn->type;
        }
    }
    if (!Optimizer::cparams.prm_c99)
        switch (type)
        {
            case bt_bool:
                if (!Optimizer::cparams.prm_cplusplus)
                    errorstr(ERR_TYPE_C99, "_Bool");
                break;
            case bt_long_long:
            case bt_unsigned_long_long:
                if (!int64 && !Optimizer::cparams.prm_cplusplus)
                    errorstr(ERR_TYPE_C99, "long long");
                break;
            case bt_float_complex:
            case bt_double_complex:
            case bt_long_double_complex:
                errorstr(ERR_TYPE_C99, "_Complex");
                break;
            case bt_long_double_imaginary:
            case bt_float_imaginary:
            case bt_double_imaginary:
                errorstr(ERR_TYPE_C99, "_Imaginary");
                break;
            default:
                break;
        }
    if (flagerror)
        error(ERR_TOO_MANY_TYPE_SPECIFIERS);
    if (!tn)
    {
        tn = (TYPE*)Alloc(sizeof(TYPE));
        tn->type = type;
        tn->rootType = tn;
        tn->size = getSize(type);
    }
    if (quals)
    {
        tl = &quals;
        while (*tl)
        {
            (*tl)->size = tn->size;
            tl = &(*tl)->btp;
        }
        *tl = tn;
        tn = quals;
    }
    if (*tp)
    {
        tl = tp;
        while (*tl)
        {
            tl = &(*tl)->btp;
        }
        *tl = tn;
    }
    else
        *tp = tn;
    UpdateRootTypes(*tp);
    if (isatomic(*tp))
    {

        int sz = basetype(*tp)->size;
        if (needsAtomicLockFromType(*tp))
        {
            int n = getAlign(sc_global, &stdchar32tptr);
            n = n - (*tp)->size % n;
            if (n != 4)
            {
                sz += n;
            }
            sz += ATOMIC_FLAG_SPACE;
        }
        (*tp)->size = sz;
    }
    sizeQualifiers(*tp);
    return lex;
}
static LEXEME* getArrayType(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, enum e_sc storage_class, bool* vla, TYPE** quals, bool first,
                            bool msil)
{
    EXPRESSION* constant = nullptr;
    TYPE* tpc = nullptr;
    TYPE* typein = *tp;
    bool unsized = false;
    bool empty = false;
    lex = getsym(); /* past '[' */
    *tp = PerformDeferredInitialization(*tp, funcsp);
    lex = getPointerQualifiers(lex, quals, true);
    if (MATCHKW(lex, star))
    {
        if (!Optimizer::cparams.prm_c99 && !Optimizer::cparams.prm_cplusplus)
            error(ERR_VLA_c99);
        if (storage_class != sc_parameter)
            error(ERR_UNSIZED_VLA_PARAMETER);
        lex = getsym();
        unsized = true;
    }
    else if (!MATCHKW(lex, closebr))
    {
        lex = expression_no_comma(lex, funcsp, nullptr, &tpc, &constant, nullptr, 0);
        if (tpc)
        {
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
            {
                RemoveSizeofOperators(constant);
            }
            optimize_for_constants(&constant);
        }
        lex = getPointerQualifiers(lex, quals, true);
        if (!tpc)
        {
            tpc = &stdint;
            error(ERR_EXPRESSION_SYNTAX);
        }
        if (*quals && !Optimizer::cparams.prm_c99)
        {
            error(ERR_ARRAY_QUALIFIERS_C99);
        }
        if (*quals && storage_class != sc_parameter)
            error(ERR_ARRAY_QUAL_PARAMETER_ONLY);
    }
    else
    {
        if (!first) /* previous was empty */
            error(ERR_ONLY_FIRST_INDEX_MAY_BE_EMPTY);
        empty = true;
    }
    if (MATCHKW(lex, closebr))
    {
        TYPE* tpp;
        lex = getsym();
        ParseAttributeSpecifiers(&lex, funcsp, true);
        if (MATCHKW(lex, openbr))
        {
            if (*quals)
                error(ERR_QUAL_LAST_ARRAY_ELEMENT);
            lex = getArrayType(lex, funcsp, tp, storage_class, vla, quals, false, msil);
        }
        tpp = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
        tpp->type = bt_pointer;
        tpp->btp = *tp;
        tpp->btp->msil = msil;  // tag the base type as managed, e.g. so we can't take address of it
        tpp->rootType = tpp;
        tpp->array = true;
        tpp->unsized = unsized;
        tpp->msil = msil;
        if (!unsized)
        {
            if (empty)
                tpp->size = 0;
            else
            {
                if (!isint(tpc))
                    error(ERR_ARRAY_INDEX_INTEGER_TYPE);
                else if (tpc->type != bt_templateparam && isintconst(constant) && constant->v.i <= 0 - !!getStructureDeclaration())
                    if (!templateNestingCount)
                        error(ERR_ARRAY_INVALID_INDEX);
                if (tpc->type == bt_templateparam)
                {
                    tpp->size = basetype(tpp->btp)->size;
                    tpp->esize = intNode(en_c_i, 1);
                }
                else if (isarithmeticconst(constant))
                {
                    tpp->size = basetype(tpp->btp)->size;
                    tpp->size *= constant->v.i;
                    tpp->esize = intNode(en_c_i, constant->v.i);
                }
                else
                {
                    if (!Optimizer::cparams.prm_c99 && !Optimizer::cparams.prm_cplusplus && !templateNestingCount && !msil)
                        error(ERR_VLA_c99);
                    tpp->esize = constant;
                    tpp->etype = tpc;
                    *vla = !msil && !templateNestingCount;
                }
            }
            *tp = tpp;
        }
        else
        {
            tpp->size = tpp->btp->size;
            tpp->esize = intNode(en_c_i, tpp->btp->size);
            *tp = tpp;
        }
        if (typein && isstructured(typein))
        {
            checkIncompleteArray(typein, lex->errfile, lex->errline);
        }
    }
    else
    {
        error(ERR_ARRAY_NEED_CLOSEBRACKET);
        errskim(&lex, skim_comma);
    }
    return lex;
}
static void resolveVLAs(TYPE* tp)
{
    while (tp && tp->type == bt_pointer && tp->array)
    {
        tp->vla = true;
        tp = tp->btp;
    }
}
bool intcmp(TYPE* t1, TYPE* t2)
{
    while (isref(t1))
        t1 = basetype(t1)->btp;
    while (isref(t2))
        t2 = basetype(t2)->btp;

    while (ispointer(t1) && ispointer(t2))
    {
        t1 = basetype(t1)->btp;
        t2 = basetype(t2)->btp;
    }
    return t1->type == t2->type;
}
static void matchFunctionDeclaration(LEXEME* lex, SYMBOL* sp, SYMBOL* spo, bool checkReturn)
{
    /* two oldstyle declarations aren't compared */
    if ((spo && !spo->sb->oldstyle && spo->sb->hasproto) || !sp->sb->oldstyle)
    {
        if (spo && isfunction(spo->tp))
        {
            SYMLIST *hro1, *hr1;
            if (checkReturn && !spo->sb->isConstructor && !spo->sb->isDestructor &&
                !comparetypes(basetype(spo->tp)->btp, basetype(sp->tp)->btp, true) &&
                !sameTemplatePointedTo(basetype(spo->tp)->btp, basetype(sp->tp)->btp))
            {
                comparetypes(basetype(spo->tp)->btp, basetype(sp->tp)->btp, true);
                preverrorsym(ERR_TYPE_MISMATCH_FUNC_DECLARATION, spo, spo->sb->declfile, spo->sb->declline);
            }
            else
            {
                hro1 = basetype(spo->tp)->syms->table[0];
                hr1 = basetype(sp->tp)->syms->table[0];
                if (hro1 && ((SYMBOL*)(hro1->p))->sb->thisPtr)
                    hro1 = hro1->next;
                if (hro1 && hr1 && ((SYMBOL*)(hro1->p))->tp)
                {
                    while (hro1 && hr1)
                    {
                        SYMBOL* spo1 = (SYMBOL*)hro1->p;
                        SYMBOL* sp1 = (SYMBOL*)hr1->p;
                        if (!comparetypes(spo1->tp, sp1->tp, true) && !sameTemplatePointedTo(spo1->tp, sp1->tp))
                        {
                            break;
                        }
                        hro1 = hro1->next;
                        hr1 = hr1->next;
                    }
                    if ((hro1 || hr1) && spo != sp)
                    {
                        preverrorsym(ERR_TYPE_MISMATCH_FUNC_DECLARATION, spo, spo->sb->declfile, spo->sb->declline);
                    }
                    else
                    {
                        bool err = false;
                        SYMBOL* last = nullptr;
                        hro1 = basetype(spo->tp)->syms->table[0];
                        hr1 = basetype(sp->tp)->syms->table[0];
                        if (hro1 && ((SYMBOL*)(hro1->p))->sb->thisPtr)
                            hro1 = hro1->next;
                        if (hr1 && ((SYMBOL*)(hr1->p))->sb->thisPtr)
                            hr1 = hr1->next;
                        while (hro1 && hr1)
                        {
                            SYMBOL* so = (SYMBOL*)hro1->p;
                            SYMBOL* s = (SYMBOL*)hr1->p;
                            if (so != s && so->sb->init && s->sb->init)
                                errorsym(ERR_CANNOT_REDECLARE_DEFAULT_ARGUMENT, so);
                            if (!err && last && last->sb->init && !(so->sb->init || s->sb->init))
                            {
                                err = true;
                                errorsym(ERR_MISSING_DEFAULT_ARGUMENT, last);
                            }
                            last = so;
                            if (so->sb->init)
                                s->sb->init = so->sb->init;
                            if (MATCHKW(lex, colon) || MATCHKW(lex, kw_try) || MATCHKW(lex, begin))
                                hro1->p = hr1->p;
                            else
                                hr1->p = hro1->p;
                            hro1 = hro1->next;
                            hr1 = hr1->next;
                        }
                    }
                }
            }
        }
    }
    if ((spo->sb->xc && spo->sb->xc->xcDynamic) || (sp->sb->xc && sp->sb->xc->xcDynamic))
    {
        if (!sp->sb->xc || !sp->sb->xc->xcDynamic)
        {
            if (!MATCHKW(lex, begin))
                errorsym(ERR_EXCEPTION_SPECIFIER_MUST_MATCH, sp);
        }
        else if (!spo->sb->xc || !spo->sb->xc->xcDynamic || spo->sb->xcMode != sp->sb->xcMode)
        {
            errorsym(ERR_EXCEPTION_SPECIFIER_MUST_MATCH, sp);
        }
        else
        {
            Optimizer::LIST* lo = spo->sb->xc->xcDynamic;
            while (lo)
            {
                Optimizer::LIST* li = sp->sb->xc->xcDynamic;
                while (li)
                {
                    if (comparetypes((TYPE*)lo->data, (TYPE*)li->data, true) && intcmp((TYPE*)lo->data, (TYPE*)li->data))
                    {
                        break;
                    }
                    li = li->next;
                }
                if (!li)
                {
                    errorsym(ERR_EXCEPTION_SPECIFIER_MUST_MATCH, sp);
                }
                lo = lo->next;
            }
        }
    }
    else if (!templateNestingCount && spo->sb->xcMode != sp->sb->xcMode)
    {
        if (spo->sb->xcMode == xc_none && sp->sb->xcMode == xc_dynamic)
        {
            if (sp->sb->xc->xcDynamic)
                errorsym(ERR_EXCEPTION_SPECIFIER_MUST_MATCH, sp);
        }
        else if (sp->sb->xcMode == xc_none && spo->sb->xcMode == xc_dynamic)
        {
            if (spo->sb->xc->xcDynamic)
                errorsym(ERR_EXCEPTION_SPECIFIER_MUST_MATCH, sp);
        }
        else if (sp->sb->xcMode == xc_unspecified)
        {
            if (!MATCHKW(lex, begin))
                errorsym(ERR_EXCEPTION_SPECIFIER_MUST_MATCH, sp);
        }
        else
        {
            errorsym(ERR_EXCEPTION_SPECIFIER_MUST_MATCH, sp);
        }
    }
}
LEXEME* getDeferredData(LEXEME* lex, LEXEME** savePos, bool braces)
{
    LEXEME **cur = savePos, *last = nullptr;
    int paren = 0;
    int brack = 0;
    int ltgt = 0;
    while (lex != nullptr)
    {
        enum e_kw kw = KW(lex);
        if (braces)
        {
            if (kw == begin)
            {
                paren++;
            }
            else if (kw == end && !--paren)
            {
                *cur = (LEXEME*)Alloc(sizeof(LEXEME));
                **cur = *lex;
                (*cur)->prev = last;
                last = *cur;
                lex = getsym();
                break;
            }
        }
        else
        {
            if (kw == semicolon)
            {
                break;
            }
            else if (kw == openpa)
            {
                paren++;
            }
            else if (kw == closepa)
            {
                if (paren-- == 0 && !brack)
                {
                    break;
                }
            }
            else if (kw == openbr)
            {
                brack++;
            }
            else if (kw == closebr)
            {
                brack--;
            }
            else if (kw == comma && !paren && !brack && !ltgt)
            {
                break;
            }
            // there is some ambiguity between templates and <
            else if (kw == lt)
            {
                ltgt++;
            }
            else if (kw == gt)
            {
                ltgt--;
            }
        }
        *cur = (LEXEME*)Alloc(sizeof(LEXEME));
        if (lex->type == l_id)
            lex->value.s.a = litlate(lex->value.s.a);
        **cur = *lex;
        (*cur)->prev = last;
        if (linesHead)
        {
            (*cur)->linedata = linesHead;
            linesHead = linesTail = nullptr;
        }
        last = *cur;
        cur = &(*cur)->next;
        lex = getsym();
    }
    return lex;
}
LEXEME* getFunctionParams(LEXEME* lex, SYMBOL* funcsp, SYMBOL** spin, TYPE** tp, bool inTemplate, enum e_sc storage_class)
{
    (void)storage_class;
    SYMBOL* sp = *spin;
    SYMBOL* spi;
    SYMLIST *hri, **hrp;
    TYPE* tp1;
    bool isvoid = false;
    bool pastfirst = false;
    bool voiderror = false;
    bool hasellipse = false;
    HASHTABLE* locals = localNameSpace->valueData->syms;
    LEXEME* placeholder = lex;
    STRUCTSYM s;
    s.tmpl = nullptr;
    lex = getsym();
    if (*tp == nullptr)
        *tp = &stdint;
    tp1 = (TYPE*)Alloc(sizeof(TYPE));
    tp1->type = bt_func;
    tp1->size = getSize(bt_pointer);
    tp1->btp = *tp;
    tp1->rootType = tp1;
    tp1->sp = sp;
    sp->tp = *tp = tp1;
    localNameSpace->valueData->syms = tp1->syms = CreateHashTable(1);
    attributes oldAttribs = basisAttribs;

    basisAttribs = {0};
    ParseAttributeSpecifiers(&lex, funcsp, true);
    if (inTemplate && templateNestingCount == 1)
        noSpecializationError++;
    if (startOfType(lex, true))
    {
        sp->sb->hasproto = true;
        while (startOfType(lex, true) || MATCHKW(lex, ellipse))
        {
            bool templType = inTemplateType;
            inTemplateType = !!templateNestingCount;
            if (MATCHKW(lex, ellipse))
            {
                if (!pastfirst)
                    break;
                ;
                if (hasellipse)
                    break;
                if (isvoid)
                    voiderror = true;
                spi = makeID(sc_parameter, tp1, nullptr, NewUnnamedID());
                spi->sb->anonymous = true;
                SetLinkerNames(spi, lk_none);
                spi->tp = (TYPE*)Alloc(sizeof(TYPE));
                spi->tp->type = bt_ellipse;
                spi->tp->rootType = spi->tp;
                insert(spi, (*tp)->syms);
                lex = getsym();
                hasellipse = true;
            }
            else
            {
                enum e_sc storage_class = sc_parameter;
                bool blocked;
                bool constexpression;
                Optimizer::ADDRESS address;
                TYPE* tpb;
                enum e_lk linkage = lk_none;
                enum e_lk linkage2 = lk_none;
                enum e_lk linkage3 = lk_none;
                bool defd = false;
                bool notype = false;
                bool clonedParams = false;
                TYPE* tp2;
                spi = nullptr;
                tp1 = nullptr;
                lex = getStorageAndType(lex, funcsp, nullptr, false, true, &storage_class, &storage_class, &address, &blocked,
                                        nullptr, &constexpression, &tp1, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd,
                                        nullptr, nullptr, nullptr);
                if (!basetype(tp1))
                    error(ERR_TYPE_NAME_EXPECTED);
                else if (isautotype(tp1) && !lambdas)
                    error(ERR_AUTO_NOT_ALLOWED_IN_PARAMETER);
                else if (Optimizer::cparams.prm_cplusplus && isstructured((*tp)->btp) && MATCHKW(lex, openpa))
                {
                    LEXEME* cur = lex;
                    lex = getsym();
                    if (!MATCHKW(lex, star) && !MATCHKW(lex, andx) && !startOfType(lex, true))
                    {
                        if (*spin)
                        {
                            (*spin)->tp = (*tp) = (*tp)->btp;
                            // constructor initialization
                            // will do initialization later...
                        }
                        localNameSpace->valueData->syms = locals;
                        if (inTemplate && templateNestingCount == 1)
                            noSpecializationError--;
                        lex = prevsym(placeholder);
                        return lex;
                    }
                    lex = prevsym(cur);
                }
                lex = getBeforeType(lex, funcsp, &tp1, &spi, nullptr, nullptr, false, storage_class, &linkage, &linkage2, &linkage3,
                                    false, false, false, false);
                if (!templateNestingCount && !structLevel)
                {
                    tp1 = PerformDeferredInitialization(tp1, funcsp);
                }
                if (!spi)
                {
                    spi = makeID(sc_parameter, tp1, nullptr, NewUnnamedID());
                    spi->sb->anonymous = true;
                    SetLinkerNames(spi, lk_none);
                }
                spi->sb->parent = sp;
                tp1 = AttributeFinish(spi, tp1);
                tp2 = tp1;
                while (ispointer(tp2) || isref(tp2))
                    tp2 = basetype(tp2)->btp;
                tp2 = basetype(tp2);
                if (tp2->type == bt_templateparam && tp2->templateParam->p->packed)
                {
                    spi->packed = true;
                    if (spi->sb->anonymous && MATCHKW(lex, ellipse))
                    {
                        lex = getsym();
                    }
                }
                spi->tp = tp1;
                spi->sb->attribs.inheritable.linkage = linkage;
                spi->sb->attribs.inheritable.linkage2 = linkage2;
                if (spi->packed)
                {
                    checkPackedType(spi);
                    if (!templateNestingCount)
                    {
                        if (tp2->templateParam && tp2->templateParam->p->packed)
                        {
                            TYPE* newtp1 = (TYPE*)Alloc(sizeof(TYPE));
                            TEMPLATEPARAMLIST* templateParams = tp2->templateParam->p->byPack.pack;
                            TEMPLATEPARAMLIST* tplnew = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                            TEMPLATEPARAM* tpnew = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                            TEMPLATEPARAMLIST** newPack;
                            bool first = true;
                            *tplnew = *tp2->templateParam;
                            *tpnew = *tp2->templateParam->p;
                            tplnew->p = tpnew;
                            *newtp1 = *tp2;
                            newtp1->templateParam = tplnew;
                            tp1 = newtp1;
                            newPack = &tp1->templateParam->p->byPack.pack;
                            tp1->templateParam->p->index = 0;
                            if (templateParams)
                            {
                                while (templateParams)
                                {
                                    SYMBOL* clone = clonesym(spi);
                                    clone->tp = (TYPE*)Alloc(sizeof(TYPE));
                                    *clone->tp = *templateParams->p->byClass.val;
                                    UpdateRootTypes(clone->tp);
                                    SetLinkerNames(clone, lk_none);
                                    sizeQualifiers(clone->tp);
                                    if (!first)
                                    {
                                        clone->name = clone->sb->decoratedName = AnonymousName();
                                        clone->packed = false;
                                    }
                                    else
                                    {
                                        clone->tp->templateParam = tp1->templateParam;
                                    }
                                    *newPack = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                    **newPack = *templateParams;
                                    (*newPack)->p = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                                    *(*newPack)->p = *templateParams->p;
                                    (*newPack)->p->packsym = clone;
                                    newPack = &(*newPack)->next;
                                    insert(clone, (*tp)->syms);
                                    first = false;
                                    templateParams = templateParams->next;
                                    tp1->templateParam->p->index++;
                                    UpdateRootTypes(clone->tp);
                                }
                            }
                            else
                            {
                                SYMBOL* clone = clonesym(spi);
                                TYPE* tp2 = (TYPE*)Alloc(sizeof(TYPE));
                                *tp2 = *clone->tp;
                                clone->tp = tp2;
                                clone->tp->templateParam = tp1->templateParam;
                                SetLinkerNames(clone, lk_none);
                                UpdateRootTypes(clone->tp);
                                sizeQualifiers(clone->tp);
                                insert(clone, (*tp)->syms);
                                UpdateRootTypes(clone->tp);
                            }
                            clonedParams = true;
                        }
                    }
                }
                if (!clonedParams)
                {
                    if (tp1 && isfunction(tp1))
                    {
                        TYPE* tp2 = (TYPE*)Alloc(sizeof(TYPE));
                        tp2->type = bt_pointer;
                        tp2->size = getSize(bt_pointer);
                        tp2->btp = tp1;
                        tp2->rootType = tp2;
                        tp1 = tp2;
                    }
                    if (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, assign))
                    {
                        if (storage_class == sc_member || storage_class == sc_mutable || structLevel ||
                            (templateNestingCount == 1 && !instantiatingTemplate))
                        {
                            lex = getDeferredData(lex, &spi->sb->deferredCompile, false);
                        }
                        else
                        {
                            TYPE* tp2 = spi->tp;
                            inDefaultParam++;
                            if (isref(tp2))
                                tp2 = basetype(tp2)->btp;
                            if (isstructured(tp2))
                            {
                                SYMBOL* sym;
                                anonymousNotAlloc++;
                                sym = anonymousVar(sc_auto, tp2)->v.sp;
                                anonymousNotAlloc--;
                                sym->sb->stackblock = !isref(spi->tp);
                                lex = initialize(lex, funcsp, sym, sc_auto, true, 0); /* also reserves space */
                                spi->sb->init = sym->sb->init;
                                if (spi->sb->init->exp->type == en_thisref)
                                {
                                    EXPRESSION** expr = &spi->sb->init->exp->left->v.func->thisptr;
                                    if ((*expr)->type == en_add && isconstzero(&stdint, (*expr)->right))
                                        spi->sb->init->exp->v.t.thisptr = (*expr) = (*expr)->left;
                                }
                            }
                            else
                            {
                                lex = initialize(lex, funcsp, spi, sc_auto, true, 0); /* also reserves space */
                            }
                            if (spi->sb->init)
                            {
                                checkDefaultArguments(spi);
                            }
                            inDefaultParam--;
                        }
                        if (isfuncptr(spi->tp) && spi->sb->init && lvalue(spi->sb->init->exp))
                            error(ERR_NO_POINTER_TO_FUNCTION_DEFAULT_ARGUMENT);
                        if (sp->sb->storage_class == sc_typedef)
                            error(ERR_NO_DEFAULT_ARGUMENT_IN_TYPEDEF);
                    }
                    SetLinkerNames(spi, lk_none);
                    spi->tp = tp1;
                    insert(spi, (*tp)->syms);
                    tpb = basetype(tp1);
                    if (tpb->array)
                    {
                        if (Optimizer::cparams.prm_cplusplus && isstructured(tpb->btp) && !basetype(tpb->btp)->sp->sb->trivialCons)
                            error(ERR_CANNOT_USE_ARRAY_OF_STRUCTURES_AS_FUNC_ARG);
                        if (tpb->vla)
                        {
                            TYPE* tpx = (TYPE*)Alloc(sizeof(TYPE));
                            TYPE* tpn = tpb;
                            tpx->type = bt_pointer;
                            tpx->btp = tpb;
                            tpx->rootType = tpx;
                            tpx->size = tpb->size = getSize(bt_pointer) + getSize(bt_unsigned) * 2;
                            while (tpn->vla)
                            {
                                tpx->size += getSize(bt_unsigned);
                                tpb->size += getSize(bt_unsigned);
                                tpn->sp = spi;
                                tpn = tpn->btp;
                            }
                        }
                    }
                    if (tpb->type == bt_void)
                        if (pastfirst || !spi->sb->anonymous)
                            voiderror = true;
                        else
                            isvoid = true;
                    else if (isvoid)
                        voiderror = true;
                }
            }
            inTemplateType = templType;
            if (!MATCHKW(lex, comma) && (!Optimizer::cparams.prm_cplusplus || !MATCHKW(lex, ellipse)))
                break;
            if (MATCHKW(lex, comma))
                lex = getsym();
            pastfirst = true;
            basisAttribs = {0};
            ParseAttributeSpecifiers(&lex, funcsp, true);
        }
        if (!needkw(&lex, closepa))
        {
            errskim(&lex, skim_closepa);
            skip(&lex, closepa);
        }
        // weed out temporary syms that were added as part of the default; they will be
        // reinstated as stackblock syms later
        hrp = &(*tp)->syms->table[0];
        while (*hrp)
        {
            SYMBOL* sym = (SYMBOL*)(*hrp)->p;
            if (sym->sb->storage_class != sc_parameter)
                *hrp = (*hrp)->next;
            else
                hrp = &(*hrp)->next;
        }
    }
    else if (!Optimizer::cparams.prm_cplusplus && !(Optimizer::architecture == ARCHITECTURE_MSIL) && ISID(lex))
    {
        SYMBOL* spo = nullptr;
        sp->sb->oldstyle = true;
        if (sp->sb->storage_class != sc_member && sp->sb->storage_class != sc_mutable)
        {
            spo = gsearch(sp->name);
            /* temporary for C */
            if (spo && spo->sb->storage_class == sc_overloads)
                spo = (SYMBOL*)spo->tp->syms->table[0]->p;
            if (spo && isfunction(spo->tp) && spo->sb->hasproto)
            {
                if (!comparetypes(spo->tp->btp, sp->tp->btp, true))
                {
                    preverrorsym(ERR_TYPE_MISMATCH_FUNC_DECLARATION, spo, spo->sb->declfile, spo->sb->declline);
                }
            }
        }
        while (ISID(lex) || MATCHKW(lex, ellipse))
        {
            if (MATCHKW(lex, ellipse))
            {
                if (!pastfirst)
                    break;
                ;
                if (hasellipse)
                    break;
                if (isvoid)
                    voiderror = true;
                spi = makeID(sc_parameter, tp1, nullptr, NewUnnamedID());
                spi->sb->anonymous = true;
                SetLinkerNames(spi, lk_none);
                spi->tp = (TYPE*)Alloc(sizeof(TYPE));
                spi->tp->type = bt_ellipse;
                spi->tp->rootType = spi->tp;
                lex = getsym();
                hasellipse = true;
                insert(spi, (*tp)->syms);
            }
            else
            {
                spi = makeID(sc_parameter, 0, 0, litlate(lex->value.s.a));
                SetLinkerNames(spi, lk_none);
                insert(spi, (*tp)->syms);
            }
            lex = getsym();
            if (!MATCHKW(lex, comma))
                break;
            lex = getsym();
        }
        if (!needkw(&lex, closepa))
        {
            errskim(&lex, skim_closepa);
            skip(&lex, closepa);
        }
        if (startOfType(lex, false))
        {
            while (startOfType(lex, false))
            {
                Optimizer::ADDRESS address;
                bool blocked;
                bool constexpression;
                enum e_lk linkage = lk_none;
                enum e_lk linkage2 = lk_none;
                enum e_lk linkage3 = lk_none;
                enum e_sc storage_class = sc_parameter;
                bool defd = false;
                bool notype = false;
                tp1 = nullptr;
                lex = getStorageAndType(lex, funcsp, nullptr, false, false, &storage_class, &storage_class, &address, &blocked,
                                        nullptr, &constexpression, &tp1, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd,
                                        nullptr, nullptr, nullptr);

                while (1)
                {
                    TYPE* tpx = tp1;
                    spi = nullptr;
                    lex = getBeforeType(lex, funcsp, &tpx, &spi, nullptr, nullptr, false, sc_parameter, &linkage, &linkage2,
                                        &linkage3, false, false, false, false);
                    sizeQualifiers(tpx);
                    if (!spi || spi->sb->anonymous)
                    {
                        error(ERR_IDENTIFIER_EXPECTED);
                        errskim(&lex, skim_end);
                        break;
                    }
                    else
                    {
                        SYMBOL* spo;
                        TYPE* tpb;
                        spi->sb->attribs.inheritable.linkage = linkage;
                        spi->sb->attribs.inheritable.linkage2 = linkage2;
                        SetLinkerNames(spi, lk_none);
                        if (tpx && isfunction(tpx))
                        {
                            TYPE* tp2 = (TYPE*)Alloc(sizeof(TYPE));
                            tp2->type = bt_pointer;
                            tp2->size = getSize(bt_pointer);
                            tp2->btp = tpx;
                            tp2->rootType = tp2;
                            tpx = tp2;
                        }
                        spi->tp = tpx;
                        tpb = basetype(tpx);
                        if (tpb->array)
                        {
                            if (tpb->vla)
                            {
                                TYPE* tpx = (TYPE*)Alloc(sizeof(TYPE));
                                TYPE* tpn = tpb;
                                tpx->type = bt_pointer;
                                tpx->btp = tpb;
                                tpx->rootType = tpx;
                                tpb = tpx;
                                tpx->size = tpb->size = getSize(bt_pointer) + getSize(bt_unsigned) * 2;
                                while (tpn->vla)
                                {
                                    tpx->size += getSize(bt_unsigned);
                                    tpb->size += getSize(bt_unsigned);
                                    tpn->sp = spi;
                                    tpn = tpn->btp;
                                }
                            }
                            else
                            {
                                //								tpb->array = false;
                                //								tpb->size = getSize(bt_pointer);
                            }
                        }
                        sizeQualifiers(tpx);
                        spo = search(spi->name, (*tp)->syms);
                        if (!spo)
                            errorsym(ERR_UNDEFINED_IDENTIFIER, spi);
                        else
                        {
                            spo->tp = tpx;
                        }
                        if (MATCHKW(lex, comma))
                        {
                            lex = getsym();
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                needkw(&lex, semicolon);
            }
        }
        hri = (*tp)->syms->table[0];
        if (hri)
        {
            while (hri)
            {
                spi = (SYMBOL*)hri->p;
                if (spi->tp == nullptr)
                {
                    if (Optimizer::cparams.prm_c99)
                        errorsym(ERR_MISSING_TYPE_FOR_PARAMETER, spi);
                    spi->tp = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
                    spi->tp->type = bt_int;
                    spi->tp->size = getSize(bt_int);
                    spi->tp->rootType = spi->tp;
                }
                hri = hri->next;
            }
        }
        else if (spo)
            (*tp)->syms->table[0] = spo->tp->syms->table[0];
        if (!MATCHKW(lex, begin))
            error(ERR_FUNCTION_BODY_EXPECTED);
    }
    else if (MATCHKW(lex, ellipse))
    {
        spi = makeID(sc_parameter, tp1, nullptr, NewUnnamedID());
        spi->sb->anonymous = true;
        SetLinkerNames(spi, lk_none);
        spi->tp = (TYPE*)Alloc(sizeof(TYPE));
        spi->tp->type = bt_ellipse;
        spi->tp->rootType = spi->tp;
        insert(spi, (*tp)->syms);
        lex = getsym();
        if (!MATCHKW(lex, closepa))
        {
            error(ERR_FUNCTION_PARAMETER_EXPECTED);
            errskim(&lex, skim_closepa);
        }
        skip(&lex, closepa);
    }
    else if (Optimizer::cparams.prm_cplusplus || ((Optimizer::architecture == ARCHITECTURE_MSIL) &&
                                                  Optimizer::cparams.msilAllowExtensions && !MATCHKW(lex, closepa) && *spin))
    {
        // () is a function
        if (MATCHKW(lex, closepa))
        {
            spi = makeID(sc_parameter, tp1, nullptr, NewUnnamedID());
            spi->sb->anonymous = true;
            spi->sb->attribs.inheritable.structAlign = getAlign(sc_parameter, &stdpointer);
            SetLinkerNames(spi, lk_none);
            spi->tp = (TYPE*)Alloc(sizeof(TYPE));
            spi->tp->type = bt_void;
            spi->tp->rootType = spi->tp;
            insert(spi, (*tp)->syms);
            lex = getsym();
        }
        // else may have a constructor
        else if (*spin)
        {
            (*spin)->tp = (*tp) = (*tp)->btp;
            // constructor initialization
            lex = backupsym();
            // will do initialization later...
        }
        else
        {
            currentErrorLine = 0;
            error(ERR_FUNCTION_PARAMETER_EXPECTED);
            errskim(&lex, skim_closepa);
        }
    }
    else
    {
        SYMBOL* spo;
        if (sp->sb->storage_class != sc_member && sp->sb->storage_class != sc_mutable)
        {
            spo = gsearch(sp->name);
            if (spo && spo->sb->storage_class == sc_overloads)
            {
                spo = (SYMBOL*)spo->tp->syms->table[0]->p;
                if (spo)
                {
                    SYMLIST* hr = spo->tp->syms->table[0];
                    if (hr)
                    {
                        SYMBOL* sp2 = hr->p;
                        if (sp2->tp->type == bt_void)
                        {
                            (*tp)->syms->table[0] = hr;
                        }
                    }
                }
            }
        }
        if (!MATCHKW(lex, closepa))
        {
            error(ERR_FUNCTION_PARAMETER_EXPECTED);
            errskim(&lex, skim_closepa);
        }
        skip(&lex, closepa);
    }
    localNameSpace->valueData->syms = locals;
    basisAttribs = oldAttribs;
    ParseAttributeSpecifiers(&lex, funcsp, true);
    if (voiderror)
        error(ERR_VOID_ONLY_PARAMETER);
    if (inTemplate && templateNestingCount == 1)
        noSpecializationError--;
    return lex;
}
LEXEME* getExceptionSpecifiers(LEXEME* lex, SYMBOL* funcsp, SYMBOL* sp, enum e_sc storage_class)
{
    (void)storage_class;
    switch (KW(lex))
    {
        case kw_throw:
            lex = getsym();
            if (MATCHKW(lex, openpa))
            {
                lex = getsym();
                if (MATCHKW(lex, closepa))
                {
                    sp->sb->xcMode = xc_none;
                    if (!sp->sb->xc)
                        sp->sb->xc = (xcept*)Alloc(sizeof(struct xcept));
                }
                else
                {
                    sp->sb->xcMode = xc_dynamic;
                    if (!sp->sb->xc)
                        sp->sb->xc = (xcept*)Alloc(sizeof(struct xcept));
                    lex = backupsym();
                    do
                    {
                        TYPE* tp = nullptr;
                        lex = getsym();
                        if (!MATCHKW(lex, closepa))
                        {
                            lex = get_type_id(lex, &tp, funcsp, sc_cast, false, true, false);
                            if (!tp)
                            {
                                error(ERR_TYPE_NAME_EXPECTED);
                            }
                            else
                            {
                                // this is reverse order but who cares?
                                Optimizer::LIST* p = (Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
                                if (tp->type == bt_templateparam && tp->templateParam->p->packed)
                                {
                                    if (!MATCHKW(lex, ellipse))
                                    {
                                        error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                                    }
                                    else
                                    {
                                        lex = getsym();
                                    }
                                }
                                p->next = sp->sb->xc->xcDynamic;
                                p->data = tp;
                                sp->sb->xc->xcDynamic = p;
                            }
                        }
                    } while (MATCHKW(lex, comma));
                }
                needkw(&lex, closepa);
            }
            else
            {
                needkw(&lex, openpa);
            }
            break;
        case kw_noexcept:
            lex = getsym();
            if (MATCHKW(lex, openpa))
            {
                lex = getsym();
                lex = getDeferredData(lex, &sp->sb->deferredNoexcept, false);
                needkw(&lex, closepa);
            }
            else
            {
                sp->sb->xcMode = xc_none;
                if (!sp->sb->xc)
                    sp->sb->xc = (xcept*)Alloc(sizeof(struct xcept));
            }
            break;
        default:
            break;
    }

    return lex;
}
static LEXEME* getAfterType(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, SYMBOL** sp, bool inTemplate, enum e_sc storage_class,
                            int consdest)
{
    bool isvla = false;
    TYPE* quals = nullptr;
    TYPE* tp1 = nullptr;
    EXPRESSION* exp = nullptr;
    if (ISKW(lex))
    {
        switch (KW(lex))
        {
            case openpa:
                if (*sp)
                {
                    lex = getFunctionParams(lex, funcsp, sp, tp, inTemplate, storage_class);
                    tp1 = *tp;
                    if (tp1->type == bt_func)
                    {
                        *tp = (*tp)->btp;
                        lex = getAfterType(lex, funcsp, tp, sp, inTemplate, storage_class, consdest);
                        tp1->btp = *tp;
                        *tp = tp1;
                        if (Optimizer::cparams.prm_cplusplus)
                        {
                            bool foundFinal = false;
                            bool foundOverride = false;
                            bool done = false;
                            bool foundConst = false;
                            bool foundVolatile = false;
                            bool foundand = false;
                            bool foundland = false;
                            while (lex != nullptr && !done)
                            {
                                if (ISID(lex))
                                {
                                    if (!strcmp(lex->value.s.a, "final"))
                                    {
                                        if (foundFinal)
                                            error(ERR_FUNCTION_CAN_HAVE_ONE_FINAL_OR_OVERRIDE);
                                        foundFinal = true;
                                        (*sp)->sb->isfinal = true;
                                        lex = getsym();
                                    }
                                    else if (!strcmp(lex->value.s.a, "override"))
                                    {
                                        if (foundOverride)
                                            error(ERR_FUNCTION_CAN_HAVE_ONE_FINAL_OR_OVERRIDE);
                                        foundOverride = true;
                                        (*sp)->sb->isoverride = true;
                                        lex = getsym();
                                    }
                                    else
                                        done = true;
                                }
                                else
                                    switch (KW(lex))
                                    {
                                        case kw_const:
                                            foundConst = true;
                                            lex = getsym();
                                            break;
                                        case kw_volatile:
                                            foundVolatile = true;
                                            lex = getsym();
                                            break;
                                        case andx:
                                            foundand = true;
                                            lex = getsym();
                                            break;
                                        case land:
                                            foundland = true;
                                            lex = getsym();
                                            break;
                                        default:
                                            done = true;
                                            break;
                                    }
                            }
                            if (foundand && foundland)
                                error(ERR_TOO_MANY_QUALIFIERS);
                            if (foundVolatile)
                            {
                                tp1 = (TYPE*)Alloc(sizeof(TYPE));
                                tp1->size = (*tp)->size;
                                tp1->type = bt_volatile;
                                tp1->btp = *tp;
                                tp1->rootType = (*tp)->rootType;
                                *tp = tp1;
                            }
                            if (foundConst)
                            {
                                tp1 = (TYPE*)Alloc(sizeof(TYPE));
                                tp1->size = (*tp)->size;
                                tp1->type = bt_const;
                                tp1->btp = *tp;
                                tp1->rootType = (*tp)->rootType;
                                *tp = tp1;
                            }
                            if (foundand)
                            {
                                tp1 = (TYPE*)Alloc(sizeof(TYPE));
                                tp1->size = (*tp)->size;
                                tp1->type = bt_lrqual;
                                tp1->btp = *tp;
                                tp1->rootType = (*tp)->rootType;
                                *tp = tp1;
                            }
                            else if (foundland)
                            {
                                tp1 = (TYPE*)Alloc(sizeof(TYPE));
                                tp1->size = (*tp)->size;
                                tp1->type = bt_rrqual;
                                tp1->btp = *tp;
                                tp1->rootType = (*tp)->rootType;
                                *tp = tp1;
                            }
                            if (Optimizer::cparams.prm_cplusplus && *sp)
                            {
                                funcLevel++;
                                lex = getExceptionSpecifiers(lex, funcsp, *sp, storage_class);
                                funcLevel--;
                            }
                            ParseAttributeSpecifiers(&lex, funcsp, true);
                            if (MATCHKW(lex, pointsto))
                            {
                                TYPE* tpx = nullptr;
                                HASHTABLE* locals = localNameSpace->valueData->syms;
                                if (inTemplate && templateNestingCount == 1)
                                    noSpecializationError++;
                                localNameSpace->valueData->syms = basetype(*tp)->syms;
                                funcLevel++;
                                lex = getsym();
                                ParseAttributeSpecifiers(&lex, funcsp, true);
                                lex = get_type_id(lex, &tpx, funcsp, sc_cast, false, true, false);
                                if (tpx)
                                {
                                    if (!isautotype(basetype(*tp)->btp))
                                        error(ERR_MULTIPLE_RETURN_TYPES_SPECIFIED);
                                    if (isarray(tpx))
                                    {
                                        TYPE* tpn = nullptr;
                                        tpn = (TYPE*)Alloc(sizeof(TYPE));
                                        tpn->type = bt_pointer;
                                        tpn->size = getSize(bt_pointer);
                                        tpn->btp = basetype(tpx)->btp;
                                        tpn->rootType = tpn;
                                        if (isconst(tpx))
                                        {
                                            TYPE* tpq = (TYPE*)Alloc(sizeof(TYPE));
                                            tpq->size = tpn->size;
                                            tpq->type = bt_const;
                                            tpq->btp = tpn;
                                            tpq->rootType = tpn->rootType;
                                            tpn = tpq;
                                        }
                                        if (isvolatile(tpx))
                                        {
                                            TYPE* tpq = (TYPE*)Alloc(sizeof(TYPE));
                                            tpq->size = tpn->size;
                                            tpq->type = bt_volatile;
                                            tpq->btp = tpn;
                                            tpq->rootType = tpn->rootType;
                                            tpn = tpq;
                                        }
                                        basetype(*tp)->btp = tpn;
                                        basetype(*tp)->rootType = tpn->rootType;
                                    }
                                    else
                                    {
                                        basetype(*tp)->btp = tpx;
                                        basetype(*tp)->rootType = tpx->rootType;
                                    }
                                }
                                localNameSpace->valueData->syms = locals;
                                funcLevel--;
                                if (inTemplate && templateNestingCount == 1)
                                    noSpecializationError--;
                            }
                        }
                    }
                    UpdateRootTypes(*tp);
                }
                else
                {
                    error(ERR_IDENTIFIER_EXPECTED);
                }
                break;
            case openbr:
                lex = getArrayType(lex, funcsp, tp, (*sp) ? (*sp)->sb->storage_class : storage_class, &isvla, &quals, true, false);
                if (isvla)
                {
                    resolveVLAs(*tp);
                }
                else if (ispointer(*tp) && (*tp)->btp->vla)
                {
                    (*tp)->vla = true;
                }
                if (quals)
                {
                    TYPE* q2 = quals;
                    while (q2->btp)
                        q2 = q2->btp;
                    q2->btp = *tp;
                    *tp = quals;
                }
                UpdateRootTypes(*tp);
                break;
            case colon:
                if (consdest == CT_CONS)
                {
                    // defer to later
                }
                else if (*sp && ((*sp)->sb->storage_class == sc_member || (*sp)->sb->storage_class == sc_mutable))
                {
                    // error(ERR_BIT_STRUCT_MEMBER);
                    if ((*sp)->tp)
                    {
                        if (Optimizer::cparams.prm_ansi)
                        {
                            if ((*sp)->tp->type != bt_int && (*sp)->tp->type != bt_unsigned && (*sp)->tp->type != bt_bool)
                                error(ERR_ANSI_INT_BIT);
                        }
                        else if (!isint((*sp)->tp) && basetype((*sp)->tp)->type != bt_enum)
                            error(ERR_BIT_FIELD_INTEGER_TYPE);
                    }
                    lex = getsym();
                    lex = optimized_expression(lex, funcsp, nullptr, &tp1, &exp, false);
                    if (tp1 && exp && isintconst(exp))
                    {
                        int n = (*tp)->size * Optimizer::chosenAssembler->arch->bits_per_mau;
                        TYPE **tp1 = tp, *tpb = basetype(*tp1);
                        *tp1 = (TYPE*)Alloc(sizeof(TYPE));
                        **tp1 = *tpb;
                        (*tp1)->bits = exp->v.i;
                        (*tp1)->hasbits = true;
                        (*tp1)->anonymousbits = (*sp)->sb->anonymous;
                        UpdateRootTypes(*tp1);
                        if ((*tp1)->bits > n)
                            error(ERR_BIT_FIELD_TOO_LARGE);
                        else if ((*tp1)->bits < 0 || ((*tp1)->bits == 0 && !(*sp)->sb->anonymous))
                            error(ERR_BIT_FIELD_MUST_CONTAIN_AT_LEAST_ONE_BIT);
                    }
                    else
                    {
                        (*tp)->bits = 1;
                        error(ERR_CONSTANT_VALUE_EXPECTED);
                        errskim(&lex, skim_semi_declare);
                    }
                }
                break;
            case lt:
                if (Optimizer::cparams.prm_cplusplus && inTemplate)
                {
                    TEMPLATEPARAMLIST* templateParams = TemplateGetParams(*sp);
                    lex = GetTemplateArguments(lex, funcsp, *sp, &templateParams->p->bySpecialization.types);
                    lex = getAfterType(lex, funcsp, tp, sp, inTemplate, storage_class, consdest);
                }
                else
                {
                    TEMPLATEPARAM* templateParam = (TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                    templateParam->type = kw_new;
                    lex = GetTemplateArguments(lex, funcsp, *sp, &templateParam->bySpecialization.types);
                    lex = getAfterType(lex, funcsp, tp, sp, inTemplate, storage_class, consdest);
                    if (*tp && isfunction(*tp))
                    {
                        TEMPLATEPARAMLIST* lst = (TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                        lst->p = templateParam;
                        DoInstantiateTemplateFunction(*tp, sp, nullptr, nullptr, lst, true);
                        if (!(*sp)->templateParams)
                            (*sp)->templateParams = lst;
                    }
                }
                break;
            default:
                break;
        }
    }
    return lex;
}
static void stripfarquals(TYPE** tp)
{
#ifdef XXXXX
    bool found = false;
    bool erred = false;
    TYPE** tpl = tp;
    while (*tpl)
    {
        if ((*tpl)->type == bt_near || (*tpl)->type == bt_far || (*tpl)->type == bt_seg)
        {
            if (found && !erred)
                error(ERR_TOO_MANY_QUALIFIERS);
            (*tpl) = (*tpl)->btp;
        }
        else
            tpl = &(*tpl)->btp;
    }
    *tp = *tpl;
#else
    (void)tp;
#endif
}
LEXEME* getBeforeType(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, SYMBOL** spi, SYMBOL** strSym, NAMESPACEVALUELIST** nsv,
                      bool inTemplate, enum e_sc storage_class, enum e_lk* linkage, enum e_lk* linkage2, enum e_lk* linkage3,
                      bool asFriend, int consdest, bool beforeOnly, bool funcptr)
{
    SYMBOL* sp;
    TYPE* ptype = nullptr;
    enum e_bt xtype = bt_none;
    LEXEME* pos = lex;
    bool doneAfter = false;

    if ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions && MATCHKW(lex, openbr))
    {
        // managed array
        bool isvla = false;
        TYPE* quals = nullptr;
        lex = getArrayType(lex, funcsp, tp, storage_class, &isvla, &quals, true, true);
        if (quals)
        {
            TYPE* q2 = quals;
            while (q2->btp)
                q2 = q2->btp;
            q2->btp = *tp;
            *tp = quals;
        }
        UpdateRootTypes(*tp);
        //        doneAfter = true;
    }
    if (ISID(lex) || MATCHKW(lex, classsel) || MATCHKW(lex, kw_operator) ||
        (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, ellipse)))
    {
        SYMBOL* strSymX = nullptr;
        NAMESPACEVALUELIST* nsvX = nullptr;
        STRUCTSYM s, s1;
        bool oldTemplateSpecialization = inTemplateSpecialization;
        s.tmpl = nullptr;
        inTemplateSpecialization = inTemplateType;
        inTemplateType = false;
        if (Optimizer::cparams.prm_cplusplus)
        {
            STRUCTSYM s;
            bool throughClass = false;
            bool pack = false;
            if (MATCHKW(lex, ellipse))
            {
                pack = true;
                lex = getsym();
            }
            lex = nestedPath(lex, &strSymX, &nsvX, &throughClass, false, storage_class, false);
            inTemplateSpecialization = oldTemplateSpecialization;
            if (strSymX)
            {
                if (strSym)
                    *strSym = strSymX;
                s1.str = strSymX;
                addStructureDeclaration(&s1);
            }
            if (nsv)
                *nsv = nsvX;
            if (strSymX && MATCHKW(lex, star))
            {
                bool inparen = false;
                if (pack)
                    error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                lex = getsym();
                if (funcptr && MATCHKW(lex, openpa))
                {
                    inparen = true;
                    lex = getsym();
                }
                ptype = (TYPE*)Alloc(sizeof(TYPE));
                ptype->type = bt_memberptr;
                ptype->rootType = ptype;
                if (strSymX->tp->type == bt_templateselector)
                    ptype->sp = strSymX->sb->templateSelector->next->sp;
                else
                    ptype->sp = strSymX;
                ptype->btp = *tp;
                UpdateRootTypes(*tp);
                *tp = ptype;
                lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, nullptr);
                if (strSym)
                    *strSym = nullptr;
                if (nsv)
                    *nsv = nullptr;
                lex = getBeforeType(lex, funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2, linkage3,
                                    asFriend, false, beforeOnly, false);
                if (*tp && (ptype != *tp && isref(*tp)))
                {
                    error(ERR_NO_REF_POINTER_REF);
                }
                ptype->size = getSize(bt_pointer) + getSize(bt_int) * 2;
                if (inparen)
                {
                    if (!needkw(&lex, closepa))
                    {
                        errskim(&lex, skim_closepa);
                        skip(&lex, closepa);
                    }
                }
            }
            else
            {
                char buf[512];
                int ov = 0;
                TYPE* castType = nullptr;
                if (MATCHKW(lex, complx))
                {
                    lex = getsym();
                    if (!ISID(lex) || !*strSym || strcmp((*strSym)->name, lex->value.s.a))
                    {
                        error(ERR_DESTRUCTOR_MUST_MATCH_CLASS);
                    }
                    else
                    {
                        strcpy(buf, overloadNameTab[CI_DESTRUCTOR]);
                        consdest = CT_DEST;
                    }
                }
                else
                {
                    lex = getIdName(lex, funcsp, buf, &ov, &castType);
                }
                if (!buf[0])
                {
                    if (!pack)
                    {
                        error(ERR_IDENTIFIER_EXPECTED);
                        sp = makeID(storage_class, *tp, *spi, NewUnnamedID());
                        sp->packed = pack;
                        SetLinkerNames(sp, lk_none);
                        *spi = sp;
                    }
                    else
                    {
                        lex = prevsym(pos);
                    }
                }
                else
                {
                    //                    if (consdest == CT_DEST)
                    //                    {
                    //                        error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
                    //                    }
                    if (ov == CI_CAST)
                    {
                        sp = makeID(storage_class, castType, *spi, litlate(buf));
                        sp->packed = pack;
                        *tp = castType;
                        sp->sb->castoperator = true;
                    }
                    else
                    {
                        sp = makeID(storage_class, *tp, *spi, litlate(buf));
                        sp->packed = pack;
                        lex = getsym();
                    }
                    sp->sb->operatorId = ov;
                    if (lex)
                        sp->sb->declcharpos = lex->charindex;
                    *spi = sp;
                }
            }
        }
        else
        {
            inTemplateSpecialization = oldTemplateSpecialization;
            sp = makeID(storage_class, *tp, *spi, litlate(lex->value.s.a));
            sp->sb->declcharpos = lex->charindex;
            *spi = sp;
            lex = getsym();
        }
        if (inTemplate && *spi)
        {
            TEMPLATEPARAMLIST* templateParams;
            SYMBOL* ssp = strSymX;
            if (!ssp)
                ssp = getStructureDeclaration();
            (*spi)->sb->parentClass = ssp;
            (*spi)->sb->templateLevel = templateNestingCount;
            templateParams = TemplateGetParams(*spi);
            (*spi)->sb->templateLevel = 0;
            (*spi)->sb->parentClass = nullptr;
            if (templateParams)
            {
                s.tmpl = templateParams;
                addTemplateDeclaration(&s);
            }
        }
        if (nsvX)
        {
            Optimizer::LIST* nlist;

            nlist = (Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
            nlist->next = nameSpaceList;
            nlist->data = nsvX->valueData->name;
            nameSpaceList = nlist;

            nsvX->valueData->name->sb->nameSpaceValues->next = globalNameSpace;
            globalNameSpace = nsvX->valueData->name->sb->nameSpaceValues;
        }
        ParseAttributeSpecifiers(&lex, funcsp, true);
        if (!doneAfter)
            lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest);
        if (nsvX)
        {
            nameSpaceList = nameSpaceList->next;
            globalNameSpace = globalNameSpace->next;
        }
        if (s.tmpl)
            dropStructureDeclaration();
        if (strSymX)
            dropStructureDeclaration();
    }
    else
        switch (KW(lex))
        {
            LEXEME* start;
            case openpa:
                if (beforeOnly)
                    break;
                start = lex;
                lex = getsym();
                /* in a parameter, open paren followed by a type is an  unnamed function */
                if (storage_class == sc_parameter &&
                    (MATCHKW(lex, closepa) || (startOfType(lex, false) && (!ISKW(lex) || !(lex->kw->tokenTypes & TT_LINKAGE)))))
                {
                    TYPE* tp1;
                    if (!*spi)
                    {
                        sp = makeID(storage_class, *tp, *spi, NewUnnamedID());
                        SetLinkerNames(sp, lk_none);
                        sp->sb->anonymous = true;
                        sp->sb->declcharpos = lex->charindex;
                        *spi = sp;
                    }
                    *tp = (*spi)->tp;
                    lex = prevsym(start);
                    lex = getFunctionParams(lex, funcsp, spi, tp, inTemplate, storage_class);
                    tp1 = *tp;
                    *tp = (*tp)->btp;
                    lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest);
                    tp1->btp = *tp;
                    *tp = tp1;
                    UpdateRootTypes(tp1);
                    return lex;
                }
                else if (Optimizer::cparams.prm_cplusplus && consdest)
                {
                    // constructor or destructor name
                    STRUCTSYM s;
                    const char* name;
                    if (consdest == CT_DEST)
                    {
                        *tp = &stdvoid;
                        name = overloadNameTab[CI_DESTRUCTOR];
                    }
                    else
                    {
                        TYPE* tp1 = (TYPE*)Alloc(sizeof(TYPE));
                        tp1->type = bt_pointer;
                        tp1->size = getSize(bt_pointer);
                        tp1->btp = *tp;
                        tp1->rootType = tp1;
                        *tp = tp1;
                        name = overloadNameTab[CI_CONSTRUCTOR];
                    }
                    sp = makeID(storage_class, *tp, *spi, name);
                    sp->sb->declcharpos = lex->charindex;
                    *spi = sp;
                    if (*strSym)
                    {
                        s.str = *strSym;
                        addStructureDeclaration(&s);
                    }
                    lex = prevsym(start);
                    lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest);
                    if (*strSym)
                    {
                        dropStructureDeclaration();
                    }
                }
                else if (MATCHKW(lex, openbr))
                {
                    lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest);
                    needkw(&lex, closepa);
                }
                else
                {
                    TYPE* quals = nullptr;
                    /* stdcall et.al. before the ptr means one thing in borland,
                     * stdcall after means another
                     * we are treating them both the same, e.g. the resulting
                     * pointer-to-function will be stdcall linkage either way
                     */
                    attributes oldAttribs = basisAttribs;

                    basisAttribs = {0};

                    ParseAttributeSpecifiers(&lex, funcsp, true);
                    lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, nullptr);
                    lex = getBeforeType(lex, funcsp, &ptype, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2,
                                        linkage3, asFriend, false, beforeOnly, true);
                    basisAttribs = oldAttribs;
                    if (!ptype ||
                        (!isref(ptype) && !ispointer(ptype) && !isfunction(ptype) && basetype(ptype)->type != bt_memberptr))
                    {
                        // if here is not a potential pointer to func
                        if (!ptype)
                            *tp = ptype;
                        ptype = nullptr;
                        if (quals)
                        {
                            TYPE* atype = *tp;
                            *tp = quals;
                            while (quals->btp)
                                quals = quals->btp;
                            quals->btp = atype;
                            UpdateRootTypes(*tp);
                        }
                    }
                    if (!needkw(&lex, closepa))
                    {
                        errskim(&lex, skim_closepa);
                        skip(&lex, closepa);
                    }
                    lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest);
                    if (ptype)
                    {
                        // pointer to func or pointer to memberfunc
                        TYPE* atype = *tp;
                        *tp = ptype;
                        while ((isref(ptype) || isfunction(ptype) || ispointer(ptype) || basetype(ptype)->type == bt_memberptr) &&
                               ptype->btp)
                            if (ptype->btp->type == bt_any)
                            {
                                break;
                            }
                            else
                            {
                                ptype = ptype->btp;
                            }
                        ptype->btp = atype;
                        ptype->rootType = atype->rootType;
                        UpdateRootTypes(*tp);

                        if (quals)
                        {
                            atype = *tp;
                            *tp = quals;
                            while (quals->btp)
                                quals = quals->btp;
                            quals->btp = atype;
                            quals->rootType = atype->rootType;
                            UpdateRootTypes(*tp);
                            sizeQualifiers(*tp);
                        }
                        atype = basetype(*tp);
                        if (atype->type == bt_memberptr && isfunction(atype->btp))
                            atype->size = getSize(bt_int) * 2 + getSize(bt_pointer);
                    }
                    if (*spi)
                        (*spi)->tp = *tp;
                    if (*tp && isfunction(*tp))
                        sizeQualifiers(basetype(*tp)->btp);
                    if (*tp)
                        sizeQualifiers(*tp);
                }
                break;
            case star:
            {
                if (*tp && isref(*tp))
                {
                    error(ERR_NO_REF_POINTER_REF);
                }
                bool inparen = false;
                lex = getsym();
                if (funcptr && MATCHKW(lex, openpa))
                {
                    lex = getsym();
                    // this isn't perfect, it doesn't work with nested parens around the identifier

                    if (!ISID(lex))
                        lex = backupsym();
                    else
                        inparen = true;
                }
                ParseAttributeSpecifiers(&lex, funcsp, true);
                ptype = *tp;
                while (ptype && ptype->type != bt_pointer && xtype == bt_none)
                {
                    if (ptype->type == bt_far)
                        xtype = bt_pointer;
                    else if (ptype->type == bt_near)
                        xtype = bt_pointer;
                    else if (ptype->type == bt_seg)
                        xtype = bt_seg;
                    ptype = ptype->btp;
                }
                if (xtype == bt_none)
                    xtype = bt_pointer;
                ptype = (TYPE*)Alloc(sizeof(TYPE));
                ptype->type = xtype;
                ptype->btp = *tp;
                ptype->rootType = ptype;
                ptype->size = getSize(xtype);
                *tp = ptype;
                lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, nullptr);
                lex = getBeforeType(lex, funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2, linkage3,
                                    asFriend, false, beforeOnly, false);
                if (inparen)
                {
                    if (!needkw(&lex, closepa))
                    {
                        errskim(&lex, skim_closepa);
                        skip(&lex, closepa);
                    }
                }
            }
            break;
            case andx:
            case land:
                if (storage_class == sc_catchvar)
                {
                    // already a ref;
                    lex = getsym();
                    ParseAttributeSpecifiers(&lex, funcsp, true);
                    lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, nullptr);
                    lex = getBeforeType(lex, funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2, linkage3,
                                        asFriend, false, beforeOnly, false);
                    break;
                }
                // using the C++ reference operator as the ref keyword...
                if (Optimizer::cparams.prm_cplusplus ||
                    ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions &&
                     storage_class == sc_parameter && KW(lex) == andx))
                {
                    if (*tp && isref(*tp) && !instantiatingTemplate && !templateNestingCount)
                    {
                        error(ERR_NO_REF_POINTER_REF);
                    }
                    TYPE* tp2;
                    if (!*tp || !isref(*tp))
                    {
                        ptype = (TYPE*)Alloc(sizeof(TYPE));
                        if (MATCHKW(lex, andx))
                            ptype->type = bt_lref;
                        else
                            ptype->type = bt_rref;
                        ptype->size = getSize(bt_pointer);
                        ptype->btp = *tp;
                        ptype->rootType = ptype;
                        *tp = ptype;
                    }
                    lex = getsym();
                    ParseAttributeSpecifiers(&lex, funcsp, true);
                    lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, nullptr);
                    lex = getBeforeType(lex, funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2, linkage3,
                                        asFriend, false, beforeOnly, false);
                    if (storage_class != sc_typedef && !isfunction(*tp) && !templateNestingCount && !instantiatingTemplate)
                    {
                        tp2 = *tp;
                        while (tp2 && tp2->type != bt_lref && tp2->type != bt_rref)
                        {
                            if ((tp2->type == bt_const || tp2->type == bt_volatile))
                            {
                                error(ERR_REF_NO_QUALIFIERS);
                                break;
                            }
                            tp2 = tp2->btp;
                        }
                    }
                }
                break;
            case colon: /* may have unnamed bit fields */
                *spi = makeID(storage_class, *tp, *spi, NewUnnamedID());
                SetLinkerNames(*spi, lk_none);
                (*spi)->sb->anonymous = true;
                lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest);
                break;
            case gt:
            case comma:
                break;
            default:
                if (beforeOnly)
                    return lex;
                if (*tp && (isstructured(*tp) || (*tp)->type == bt_enum) && KW(lex) == semicolon)
                {
                    lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest);
                    *spi = nullptr;
                    return lex;
                }
                if (storage_class != sc_parameter && storage_class != sc_cast && storage_class != sc_catchvar && !asFriend)
                {
                    if (MATCHKW(lex, openpa) || MATCHKW(lex, openbr) || MATCHKW(lex, assign) || MATCHKW(lex, semicolon))
                        error(ERR_IDENTIFIER_EXPECTED);
                    else
                        errortype(ERR_IMPROPER_USE_OF_TYPE, *tp, nullptr);
                }
                if (!*tp)
                {
                    *spi = nullptr;
                    return lex;
                }
                *spi = makeID(storage_class, *tp, *spi, NewUnnamedID());
                SetLinkerNames(*spi, lk_none);
                (*spi)->sb->anonymous = true;
                lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest);
                break;
        }
    if (*tp && (ptype = basetype(*tp)))
    {
        if (isfuncptr(ptype))
            ptype = basetype(ptype->btp);
        if (isfunction(ptype))
        {
            if (ptype->btp->type == bt_func || (ptype->btp->type == bt_pointer && ptype->btp->array && !ptype->btp->msil))
                error(ERR_FUNCTION_NO_RETURN_FUNCTION_ARRAY);
        }
        if (*spi)
        {
            stripfarquals(tp);
        }
    }
    else
    {
        // an error occurred
        *tp = &stdint;
    }
    ParseAttributeSpecifiers(&lex, funcsp, true);
    return lex;
}
static EXPRESSION* vlaSetSizes(EXPRESSION*** rptr, EXPRESSION* vlanode, TYPE* btp, SYMBOL* sp, int* index, int* vlaindex, int sou)
{
    EXPRESSION *mul, *mul1, *store;
    btp = basetype(btp);
    if (btp->vla)
    {
        mul = vlaSetSizes(rptr, vlanode, btp->btp, sp, index, vlaindex, sou);
        mul1 = (EXPRESSION*)Alloc(sizeof(EXPRESSION));
        if (!btp->esize)
            btp->esize = intNode(en_c_i, 1);
        *mul1 = *btp->esize;
        mul = mul1 = exprNode(en_arraymul, mul, mul1);
        btp->sp = sp;
        btp->vlaindex = (*vlaindex)++;
    }
    else
    {
#ifdef ERROR
#    error Sizeof vla of vla
#endif
        mul = intNode(en_c_i, btp->size);
        mul1 = (EXPRESSION*)Alloc(sizeof(EXPRESSION));
        *mul1 = *mul;
    }
    store = exprNode(en_add, vlanode, intNode(en_c_i, *index));
    deref(&stdint, &store);
    store = exprNode(en_assign, store, mul1);
    **rptr = exprNode(en_void, store, nullptr);
    *rptr = &(**rptr)->right;
    *index += sou;
    return mul;
}
static EXPRESSION* llallocateVLA(SYMBOL* sp, EXPRESSION* ep1, EXPRESSION* ep2)
{
    EXPRESSION *loader, *unloader;

    if (Optimizer::architecture == ARCHITECTURE_MSIL)
    {
        SYMBOL* al = gsearch("malloc");
        SYMBOL* fr = gsearch("free");
        if (al)
        {
            al = (SYMBOL*)al->tp->syms->table[0]->p;
        }
        if (fr)
        {
            fr = (SYMBOL*)fr->tp->syms->table[0]->p;
        }
        if (al && fr)
        {
            FUNCTIONCALL* epx = (FUNCTIONCALL*)(FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
            FUNCTIONCALL* ld = (FUNCTIONCALL*)(FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
            epx->ascall = true;
            epx->fcall = varNode(en_pc, al);
            epx->sp = al;
            epx->functp = al->tp;
            epx->arguments = (INITLIST*)(INITLIST*)Alloc(sizeof(INITLIST));
            epx->arguments->tp = &stdint;
            epx->arguments->exp = ep2;
            ep2 = intNode(en_func, 0);
            ep2->v.func = epx;
            ep2 = exprNode(en_assign, ep1, ep2);
            ld->ascall = true;
            ld->fcall = varNode(en_pc, fr);
            ld->sp = fr;
            ld->functp = fr->tp;
            ld->arguments = (INITLIST*)(INITLIST*)Alloc(sizeof(INITLIST));
            ld->arguments->tp = &stdpointer;
            ld->arguments->exp = ep1;
            unloader = intNode(en_func, 0);
            unloader->v.func = ld;
            ep1 = ep2;
        }
        else
        {
            diag("llallocatevla: cannot find allocator");
            return intNode(en_c_i, 0);
        }
    }
    else
    {
        EXPRESSION* var = anonymousVar(sc_auto, &stdpointer);
        loader = exprNode(en_savestack, var, nullptr);
        unloader = exprNode(en_loadstack, var, nullptr);
        ep1 = exprNode(en_assign, ep1, exprNode(en_alloca, ep2, nullptr));
        ep1 = exprNode(en_void, loader, ep1);
    }

    initInsert(&sp->sb->dest, sp->tp, unloader, 0, false);
    return ep1;
}
static void allocateVLA(LEXEME* lex, SYMBOL* sp, SYMBOL* funcsp, BLOCKDATA* block, TYPE* btp, bool bypointer)
{
    EXPRESSION *result = nullptr, **rptr = &result;
    TYPE* tp1 = btp;
    int count = 0;
    int soa = getSize(bt_pointer);
    int sou = getSize(bt_unsigned);
    while (tp1->vla)
    {
        count++, tp1 = tp1->btp;
    }
    if (!funcsp)
        error(ERR_VLA_BLOCK_SCOPE); /* this is a safety, should already be caught */
    if (!bypointer)
    {
        /* vla */
        if (sp->sb->storage_class != sc_auto && sp->sb->storage_class != sc_typedef)
            error(ERR_VLA_BLOCK_SCOPE);
    }
    else
    {
        /* pointer to vla */
        if (sp->sb->storage_class != sc_auto && sp->sb->storage_class != sc_typedef && sp->sb->storage_class != sc_localstatic)
            error(ERR_VLA_BLOCK_SCOPE);
    }
    currentLineData(block, lex, 0);
    if (sp->tp->sp)
    {
        SYMBOL* dest = sp;
        SYMBOL* src = sp->tp->sp;
        *rptr = exprNode(en_void, nullptr, nullptr);
        rptr = &(*rptr)->right;
        result->left = exprNode(en_blockassign, varNode(en_auto, dest), varNode(en_auto, src));
        result->left->size = dest->tp->size = src->tp->size;
        result->altdata = (void*)src->tp;
    }
    else
    {
        int size;
        SYMBOL* vlasp = sp;
        EXPRESSION* vlanode;
        EXPRESSION* ep;
        int index = soa + sou;
        int vlaindex = 0;
        size = (count + 2) * (sou) + soa;
        vlanode = varNode(en_auto, vlasp);
        vlaSetSizes(&rptr, vlanode, btp, vlasp, &index, &vlaindex, sou);

        ep = exprNode(en_add, vlanode, intNode(en_c_i, soa));
        deref(&stdint, &ep);
        ep = exprNode(en_assign, ep, intNode(en_c_i, count));
        *rptr = exprNode(en_void, ep, nullptr);
        rptr = &(*rptr)->right;

        basetype(sp->tp)->size = size; /* size field is actually size of VLA header block */
                                       /* sizeof will fetch the size from that */
    }
    if (result != nullptr)
    {
        STATEMENT* st = stmtNode(nullptr, block, st_declare);
        st->hasvla = true;
        if (sp->sb->storage_class != sc_typedef && !bypointer)
        {
            EXPRESSION* ep1 = exprNode(en_add, varNode(en_auto, sp), intNode(en_c_i, 0));
            EXPRESSION* ep2 = exprNode(en_add, varNode(en_auto, sp), intNode(en_c_i, soa + sou * (count + 1)));
            deref(&stdpointer, &ep1);
            deref(&stdint, &ep2);
            ep1 = llallocateVLA(sp, ep1, ep2);  // exprNode(en_assign, ep1, exprNode(en_alloca, ep2, nullptr));
            *rptr = (Optimizer::architecture == ARCHITECTURE_MSIL) ? ep1 : exprNode(en_void, ep1, nullptr);
            sp->sb->assigned = true;
        }
        st->select = result;
        optimize_for_constants(&st->select);
    }
}
void sizeQualifiers(TYPE* tp)
{
    if (tp->type != bt_atomic)
    {
        while (true)
        {
            TYPE* tp1 = basetype(tp);
            while (tp1 != tp)
            {
                tp->size = tp1->size;
                tp = tp->btp;
            }
            if (ispointer(tp))
            {
                tp = tp->btp;
            }
            else
                break;
        }
    }
}
static bool sameQuals(TYPE* tp1, TYPE* tp2)
{
    while (tp1 && tp2)
    {
        if (isconst(tp1) != isconst(tp2))
            return false;
        if (isvolatile(tp1) != isvolatile(tp2))
            return false;
        if (isrestrict(tp1) != isrestrict(tp2))
            return false;
        tp1 = basetype(tp1)->btp;
        tp2 = basetype(tp2)->btp;
    }
    return true;
}
static LEXEME* getStorageAndType(LEXEME* lex, SYMBOL* funcsp, SYMBOL** strSym, bool inTemplate, bool assumeType,
                                 enum e_sc* storage_class, enum e_sc* storage_class_in, Optimizer::ADDRESS* address, bool* blocked,
                                 bool* isExplicit, bool* constexpression, TYPE** tp, enum e_lk* linkage, enum e_lk* linkage2,
                                 enum e_lk* linkage3, enum e_ac access, bool* notype, bool* defd, int* consdest, bool* templateArg,
                                 bool* asFriend)
{
    bool foundType = false;
    bool first = true;
    *blocked = false;
    *constexpression = false;

    while (KWTYPE(lex, TT_STORAGE_CLASS | TT_POINTERQUAL | TT_LINKAGE | TT_DECLARE) ||
           (!foundType && startOfType(lex, assumeType)) || MATCHKW(lex, complx) || (*storage_class == sc_typedef && !foundType))
    {
        if (KWTYPE(lex, TT_DECLARE))
        {
            // The next line has a recurring check. The 'lex' condition was already verified above
            // the problem is I'm using macros that I want to be independent from each other
            // and I don't want to introduce another macro without the check for readability
            // reasons.  So I'm going to leave the recurring check.
            if (MATCHKW(lex, kw_constexpr))
            {
                *constexpression = true;
            }
            lex = getsym();
        }
        else if (KWTYPE(lex, TT_STORAGE_CLASS))
        {
            lex = getStorageClass(lex, funcsp, storage_class, linkage, address, blocked, isExplicit, access);
            if (*blocked)
                break;
        }
        else if (KWTYPE(lex, TT_POINTERQUAL | TT_LINKAGE))
        {
            lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, asFriend);
            if (MATCHKW(lex, kw_atomic))
            {
                foundType = true;
                lex = getBasicType(lex, funcsp, tp, strSym, inTemplate, *storage_class_in, linkage, linkage2, linkage3, access,
                                   notype, defd, consdest, templateArg, *storage_class == sc_typedef, true, false);
            }
            if (*linkage3 == lk_threadlocal && *storage_class == sc_member)
                *storage_class = sc_static;
        }
        else if (foundType)
        {
            break;
        }
        else
        {
            foundType = true;
            lex = getBasicType(lex, funcsp, tp, strSym, inTemplate, *storage_class_in, linkage, linkage2, linkage3, access, notype,
                               defd, consdest, templateArg, *storage_class == sc_typedef, true, false);
            if (*linkage3 == lk_threadlocal && *storage_class == sc_member)
                *storage_class = sc_static;
        }
        if (ParseAttributeSpecifiers(&lex, funcsp, true))
            break;
        first = false;
    }
    return lex;
}
static bool mismatchedOverloadLinkage(SYMBOL* sp, HASHTABLE* table)
{
    if (((SYMBOL*)(table->table[0]->p))->sb->attribs.inheritable.linkage != sp->sb->attribs.inheritable.linkage)
        return true;
    return false;
}
void injectThisPtr(SYMBOL* sp, HASHTABLE* syms)
{
    if (syms)
    {
        SYMLIST** hr = &syms->table[0];
        SYMBOL* ths;
        TYPE *type, *tpx;
        if (*hr && ((SYMBOL*)(*hr)->p)->sb->thisPtr)
            return;
        type = tpx = (TYPE*)Alloc(sizeof(TYPE));
        tpx->type = bt_pointer;
        tpx->size = getSize(bt_pointer);
        if (isconst(sp->tp))
        {
            tpx = tpx->btp = (TYPE*)Alloc(sizeof(TYPE));
            tpx->type = bt_const;
            tpx->size = basetype(sp->sb->parentClass->tp)->size;
        }
        if (isvolatile(sp->tp))
        {
            tpx = tpx->btp = (TYPE*)Alloc(sizeof(TYPE));
            tpx->type = bt_volatile;
            tpx->size = basetype(sp->sb->parentClass->tp)->size;
        }
        tpx->btp = basetype(sp->sb->parentClass->tp);
        UpdateRootTypes(type);
        ths = makeID(sc_parameter, type, nullptr, "__$$this");
        ths->sb->parent = sp;
        ths->sb->thisPtr = true;
        ths->sb->attribs.inheritable.used = true;
        SetLinkerNames(ths, lk_cdecl);
        //    if ((*hr) && ((SYMBOL*sym)(*hr)->p)->tp->type == bt_void)
        //    {
        //        (*hr)->p = ths;
        //    }
        //    else
        {
            SYMLIST* hr1 = (SYMLIST*)Alloc(sizeof(SYMLIST));
            hr1->p = (SYMBOL*)ths;
            hr1->next = *hr;
            *hr = hr1;
        }
    }
}
static bool hasTemplateParent(SYMBOL* sp)
{
    sp = sp->sb->parentClass;
    while (sp)
    {
        if (sp->sb->templateLevel)
            return true;
        sp = sp->sb->parentClass;
    }
    return false;
}
static bool differentTemplateNames(TEMPLATEPARAMLIST* a, TEMPLATEPARAMLIST* b)
{
    while (a && b)
    {
        if (a->p->type != b->p->type)
            return true;
        if (!a->argsym || !b->argsym)
            return true;
        if (strcmp(a->argsym->name, b->argsym->name))
            return true;
        a = a->next;
        b = b->next;
    }
    return false;
}
static bool sameNameSpace(SYMBOL* left, SYMBOL* right)
{
    if (left == right)
        return true;
    if (left && right)
    {
        if (left->sb->nameSpaceValues->valueData == right->sb->nameSpaceValues->valueData)
            return true;
    }
    return false;
}
LEXEME* declare(LEXEME* lex, SYMBOL* funcsp, TYPE** tprv, enum e_sc storage_class, enum e_lk defaultLinkage, BLOCKDATA* block,
                bool needsemi, int asExpression, bool inTemplate, enum e_ac access)
{
    bool isExtern = false;
    TYPE* btp;
    SYMBOL* sp;
    enum e_sc storage_class_in = storage_class;
    enum e_lk linkage = lk_none;
    enum e_lk linkage2 = lk_none;
    enum e_lk linkage3 = lk_none;
    NAMESPACEVALUELIST* nsv = nullptr;
    SYMBOL* strSym = nullptr;
    Optimizer::ADDRESS address = 0;
    TYPE* tp = nullptr;
    bool hasAttributes;
    attributes oldAttribs = basisAttribs;

    basisAttribs = {0};
    hasAttributes = ParseAttributeSpecifiers(&lex, funcsp, true);

    if (!MATCHKW(lex, semicolon))
    {
        if (MATCHKW(lex, kw_inline))
        {
            linkage = lk_inline;
            lex = getsym();
        }
        else if (!asExpression && MATCHKW(lex, kw_extern))
        {
            lex = getsym();
            if (MATCHKW(lex, kw_template))
            {
                isExtern = true;
                goto jointemplate;
            }
            lex = backupsym();
        }
        if (!asExpression && MATCHKW(lex, kw_template))
        {
        jointemplate:
            if (funcsp)
            {
                if (storage_class == sc_member || storage_class == sc_mutable)
                    error(ERR_TEMPLATE_NO_LOCAL_CLASS);
                else
                    error(ERR_TEMPLATE_GLOBAL_OR_CLASS_SCOPE);
            }
            if (hasAttributes)
                error(ERR_NO_ATTRIBUTE_SPECIFIERS_HERE);

            lex = TemplateDeclaration(lex, funcsp, access, storage_class, isExtern);
            needsemi = false;
        }
        else if (!asExpression && MATCHKW(lex, kw_namespace))
        {
            bool linked;
#ifdef PARSER_ONLY
            struct _ccNamespaceData nsData;
            nsData.declfile = lex->errfile;
            nsData.startline = lex->errline;
#endif
            if (storage_class_in == sc_member || storage_class_in == sc_mutable)
                error(ERR_NAMESPACE_NO_STRUCT);

            if (hasAttributes)
                error(ERR_NO_ATTRIBUTE_SPECIFIERS_HERE);
            lex = getsym();
            lex = insertNamespace(lex, linkage, storage_class_in, &linked);
            if (linked)
            {
                if (needkw(&lex, begin))
                {
                    while (lex && !MATCHKW(lex, end))
                    {
                        lex = declare(lex, nullptr, nullptr, storage_class, defaultLinkage, nullptr, true, false, false, access);
                    }
                }
#ifdef PARSER_ONLY
                if (lex)
                    nsData.endline = lex->errline;
#endif
                needkw(&lex, end);
                if (linked)
                {
                    SYMBOL* sp = (SYMBOL*)nameSpaceList->data;
                    sp->sb->value.i--;
                    nameSpaceList = nameSpaceList->next;
                    globalNameSpace = globalNameSpace->next;
#ifdef PARSER_ONLY
                    nsData.next = sp->sb->ccNamespaceData;
                    sp->sb->ccNamespaceData = (_ccNamespaceData*)Alloc(sizeof(struct _ccNamespaceData));
                    *sp->sb->ccNamespaceData = nsData;
#endif
                }
                needsemi = false;
            }
        }
        else if (!asExpression && MATCHKW(lex, kw_using))
        {
            lex = getsym();
            sp = nullptr;
            lex = insertUsing(lex, &sp, access, storage_class_in, inTemplate, hasAttributes);
            if (sp && tprv)
                *tprv = (TYPE*)-1;
        }
        else if (!asExpression && MATCHKW(lex, kw_static_assert))
        {
            if (hasAttributes)
                error(ERR_NO_ATTRIBUTE_SPECIFIERS_HERE);
            lex = getsym();
            lex = handleStaticAssert(lex);
        }
        else
        {
            bool blocked;
            bool constexpression;
            bool defd = false;
            bool notype = false;
            bool isExplicit = false;
            bool templateArg = false;
            bool asFriend = false;
            int consdest = CT_NONE;

            lex = getStorageAndType(lex, funcsp, &strSym, inTemplate, false, &storage_class, &storage_class_in, &address, &blocked,
                                    &isExplicit, &constexpression, &tp, &linkage, &linkage2, &linkage3, access, &notype, &defd,
                                    &consdest, &templateArg, &asFriend);
            if (blocked)
            {
                if (tp != nullptr)
                    error(ERR_TOO_MANY_TYPE_SPECIFIERS);

                needsemi = false;
            }
            else
            {
                if (storage_class_in == sc_member && asFriend)
                {
                    storage_class = sc_external;
                }
                else if (storage_class_in == sc_member && storage_class == sc_static)
                {
                    storage_class = sc_external;
                }
                do
                {
                    bool isTemplatedCast = false;
                    TYPE* tp1 = tp;
                    NAMESPACEVALUELIST* oldGlobals = nullptr;
                    bool promotedToTemplate = false;
                    if (!tp1)
                    {
                        // safety net
                        notype = true;
                        tp = tp1 = (TYPE*)Alloc(sizeof(TYPE));
                        tp->type = bt_int;
                        tp->size = getSize(tp->type);
                        tp->rootType = tp;
                    }
                    sp = nullptr;
                    if (tp->type == bt_templateselector)
                    {
                        // have to special case a templated cast operator
                        TEMPLATESELECTOR* l = tp->sp->sb->templateSelector;
                        while (l->next)
                            l = l->next;
                        if (!strcmp(l->name, overloadNameTab[CI_CAST]))
                        {
                            strSym = tp->sp->sb->templateSelector->next->sp;
                            while (!MATCHKW(lex, kw_operator))
                                lex = lex->prev;
                            notype = true;
                            tp = tp1 = (TYPE*)Alloc(sizeof(TYPE));
                            tp->type = bt_int;
                            tp->size = getSize(tp->type);
                            tp->rootType = tp;
                            isTemplatedCast = true;
                        }
                    }
                    lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, &asFriend);
                    lex = getBeforeType(lex, funcsp, &tp1, &sp, &strSym, &nsv, inTemplate, storage_class, &linkage, &linkage2,
                                        &linkage3, asFriend, consdest, false, false);
                    if (linkage2 == lk_import)
                        if (storage_class == sc_global)
                            storage_class = sc_external;
                    if (sp)
                    {
                        tp1 = AttributeFinish(sp, tp1);
                    }
                    inTemplateType = false;
                    if (isfunction(tp1))
                        sizeQualifiers(basetype(tp1)->btp);
                    if (strSym)
                    {
                        if (!strcmp(strSym->name, sp->name))
                        {
                            sp->name = overloadNameTab[CI_CONSTRUCTOR];
                            consdest = CT_CONS;
                        }
                        else if (!strcmp(sp->name, overloadNameTab[CI_DESTRUCTOR]))
                        {
                            consdest = CT_DEST;
                        }
                    }
                    if (tprv && (!notype || consdest || isTemplatedCast || (sp && sp->sb->castoperator)))
                        *tprv = tp1;
                    // if defining something outside its scope set the symbol tables
                    // to the original scope it lives in
                    if (nsv)
                    {
                        oldGlobals = globalNameSpace;
                        globalNameSpace = nsv;
                    }
                    if (strSym)
                    {
                        if (strSym->tp->type == bt_enum)
                        {
                            error(ERR_CANNOT_DEFINE_ENUMERATION_CONSTANT_HERE);
                        }
                        else if (strSym->tp->type != bt_templateselector && strSym->tp->type != bt_templatedecltype)
                        {
                            STRUCTSYM* l = (STRUCTSYM*)Alloc(sizeof(STRUCTSYM));
                            l->str = strSym;
                            addStructureDeclaration(l);
                        }
                    }
                    if (Optimizer::cparams.prm_cplusplus && isfunction(tp1) &&
                        (storage_class_in == sc_member || storage_class_in == sc_mutable) && !asFriend)
                    {
                        if (MATCHKW(lex, colon) || MATCHKW(lex, begin) || MATCHKW(lex, kw_try))
                        {
                            sp->sb->attribs.inheritable.isInline = true;
                        }
                    }
                    if (linkage == lk_inline)
                    {
                        if (Optimizer::cparams.prm_cplusplus)
                            linkage = lk_virtual;
                        else
                            linkage = lk_none;
                        if (sp)
                            sp->sb->attribs.inheritable.isInline = true;
                    }
                    else if (!Optimizer::cparams.prm_profiler && Optimizer::cparams.prm_optimize_for_speed && isfunction(tp1) &&
                             storage_class_in != sc_member && storage_class_in != sc_mutable)
                    {
                        if (!strSym)
                        {
                            if (MATCHKW(lex, colon) || MATCHKW(lex, begin) || MATCHKW(lex, kw_try))
                            {
                                if (strcmp(sp->name, "main") != 0)
                                {
                                    sp->sb->attribs.inheritable.isInline = sp->sb->dumpInlineToFile = sp->sb->promotedToInline = true;
                                }
                            }
                        }
                    }
                    if (linkage == lk_none || ((defaultLinkage == lk_c || defaultLinkage == lk_cpp) && linkage == lk_cdecl))
                        linkage = defaultLinkage;
                    // defaultLinkage may also be lk_none...
                    if (linkage == lk_none)
                        linkage = lk_cdecl;

                    if (asFriend && ispointer(tp1))
                        error(ERR_POINTER_ARRAY_NOT_FRIENDS);

                    if (storage_class_in != sc_parameter && tp1->type == bt_templateparam && tp1->templateParam->p->packed)
                        error(ERR_PACKED_TEMPLATE_PARAMETER_NOT_ALLOWED_HERE);
                    if (!sp)
                    {
                        if (!isstructured(tp1) && tp1->type != bt_enum)
                        {
                            error(ERR_IDENTIFIER_EXPECTED);
                        }
                        else
                        {
                            if (tp1->type == bt_enum)
                            {
                                if (nsv)
                                {
                                    globalNameSpace = oldGlobals;
                                }
                                if (strSym && strSym->tp->type != bt_enum && strSym->tp->type != bt_templateselector &&
                                    strSym->tp->type != bt_templatedecltype)
                                {
                                    dropStructureDeclaration();
                                }
                                break;
                            }
                        }
                        tp1 = basetype(tp1);
                        if (Optimizer::cparams.prm_cplusplus && storage_class_in == sc_global && tp1->type == bt_union &&
                            tp1->sp->sb->anonymous)
                        {
                            enum e_sc sc = storage_class_in;
                            if (sc != sc_member && sc != sc_mutable)
                                sc = sc_static;

                            sp = makeID(sc, tp1, nullptr, AnonymousName());
                            sp->sb->anonymous = true;
                            sp->sb->access = access;
                            SetLinkerNames(sp, lk_c);
                            sp->sb->parent = funcsp; /* function vars have a parent */
                            InsertSymbol(sp, sp->sb->storage_class, linkage, false);
                            if (storage_class != sc_static)
                            {
                                error(ERR_GLOBAL_ANONYMOUS_UNION_NOT_STATIC);
                            }
                            resolveAnonymousGlobalUnion(sp);
                        }
                        else if ((storage_class_in == sc_member || storage_class_in == sc_mutable) && isstructured(tp1) &&
                                 tp1->sp->sb->anonymous)
                        {
                            sp = makeID(storage_class_in, tp1, nullptr, AnonymousName());
                            sp->sb->anonymous = true;
                            SetLinkerNames(sp, lk_c);
                            sp->sb->parent = funcsp; /* function vars have a parent */
                            sp->sb->parentClass = getStructureDeclaration();
                            InsertSymbol(sp, sp->sb->storage_class, linkage, false);
                        }
                        else
                        {
                            if (asFriend)
                            {
                                SYMBOL* sym = nullptr;
                                Optimizer::LIST* l = (Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
                                STRUCTSYM* ls = structSyms;
                                while (ls && !ls->str)
                                    ls = ls->next;
                                if (strSym && strSym->tp->type != bt_enum && strSym->tp->type != bt_templateselector &&
                                    strSym->tp->type != bt_templatedecltype)
                                {
                                    ls = ls->next;
                                    while (ls && !ls->str)
                                        ls = ls->next;
                                }
                                if (ls)
                                    sym = ls->str;
                                l->data = (void*)tp1->sp;
                                l->next = sym->sb->friends;
                                sym->sb->friends = l;
                            }
                            if (nsv)
                            {
                                globalNameSpace = oldGlobals;
                            }
                            if (strSym && strSym->tp->type != bt_enum && strSym->tp->type != bt_templateselector &&
                                strSym->tp->type != bt_templatedecltype)
                            {
                                dropStructureDeclaration();
                            }
                            break;
                        }
                    }
                    else

                    {
                        SYMBOL* ssp = nullptr;
                        SYMBOL* spi;
                        bool checkReturn = true;
                        if ((isstructured(tp1) && storage_class != sc_typedef) || (!templateNestingCount && !structLevel))
                            tp1 = PerformDeferredInitialization(tp1, funcsp);
                        ssp = getStructureDeclaration();
                        if (!asFriend &&
                            (((storage_class_in == sc_member || storage_class_in == sc_mutable) && ssp) || (inTemplate && strSym)))
                        {
                            sp->sb->parentClass = ssp;
                        }
                        if (!inTemplate)
                        {
                            if (isfunction(sp->tp) && hasTemplateParent(sp))
                            {
                                SYMBOL* parent = sp->sb->parentClass;
                                while (parent)
                                {
                                    if (parent->sb->templateLevel)
                                        break;

                                    parent = parent->sb->parentClass;
                                }
                                //                                sp->templateParams = parent->templateParams;
                                promotedToTemplate = true;
                            }
                        }
                        else if (ssp && storage_class_in != sc_member && storage_class_in != sc_mutable && !asFriend)
                        {
                            storage_class = sc_member;
                        }
                        if (Optimizer::cparams.prm_cplusplus && !ssp && storage_class == sc_global &&
                            (isstructured(tp1) || basetype(tp1)->type == bt_enum))
                            if (basetype(tp1)->sp->sb->anonymous)
                                storage_class = sc_static;
                        if (consdest != CT_NONE)
                        {
                            if (Optimizer::cparams.prm_cplusplus)
                                ConsDestDeclarationErrors(sp, notype);
                            if (isfunction(sp->tp))
                                if (consdest == CT_CONS)
                                    sp->sb->isConstructor = true;
                                else
                                    sp->sb->isDestructor = true;
                            else
                                sp->name = litlate(AnonymousName());
                        }
                        sp->sb->parent = funcsp; /* function vars have a parent */
                        if (instantiatingTemplate)
                        {
                            if (sp->sb->parentClass)
                                sp->sb->parentNameSpace = sp->sb->parentClass->sb->parentNameSpace;
                        }
                        else if (nameSpaceList && storage_class_in != sc_auto)
                            sp->sb->parentNameSpace = (SYMBOL*)nameSpaceList->data;
                        if (inTemplate || promotedToTemplate)
                        {
                            if (asFriend)
                            {
                                SYMBOL* sp1 = sp->sb->parentClass;
                                sp->sb->templateLevel = inTemplate;
                                while (sp1)
                                {
                                    if (sp1->sb->templateLevel)
                                        sp->sb->templateLevel++;
                                    sp1 = sp1->sb->parentClass;
                                }
                            }
                            else
                            {
                                sp->sb->templateLevel = templateHeaderCount;
                            }
                        }
                        sp->sb->constexpression = constexpression;
                        sp->sb->access = access;
                        sp->sb->isExplicit = isExplicit;
                        sp->sb->storage_class = storage_class;
                        if (inTemplate &&
                            (!sp->templateParams || MATCHKW(lex, colon) || MATCHKW(lex, begin) || MATCHKW(lex, kw_try)))
                            sp->templateParams = TemplateGetParams(sp);
                        if (sp->sb->isDestructor && sp->sb->xcMode == xc_unspecified)
                            sp->sb->xcMode = xc_none;
                        /* removed in C++ 2014*/
                        /*
                        if (sp->sb->constexpression && !sp->sb->isDestructor && !sp->sb->isConstructor)
                        {
                            TYPE *tpx = (TYPE *)Alloc(sizeof(TYPE));
                            tpx->type = bt_const;
                            tpx->size = basetype(tp1)->size;
                            tpx->btp = tp1;
                            tpx->rootType = tp1->rootType;
                            tp1 = tpx;
                        }
                        */
                        if (!asFriend)
                            sp->tp = tp1;
                        if (!sp->sb->instantiated)
                            sp->sb->attribs.inheritable.linkage = linkage;
                        if (ssp && ssp->sb->attribs.inheritable.linkage2 != lk_none && sp->sb->storage_class != sc_localstatic)
                        {
                            if (linkage2 != lk_none && !asFriend)
                                errorsym(ERR_DECLSPEC_MEMBER_OF_DECLSPEC_CLASS_NOT_ALLOWED, sp);
                            else if (!ssp->sb->templateLevel || !inTemplate)
                                sp->sb->attribs.inheritable.linkage2 = ssp->sb->attribs.inheritable.linkage2;
                        }
                        else
                        {
                            if (sp->sb->attribs.inheritable.linkage2 == lk_none)
                                sp->sb->attribs.inheritable.linkage2 = linkage2;
                            else if (linkage2 != lk_none)
                                error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                        }
                        if (sp->sb->attribs.inheritable.linkage3 == lk_none)
                            sp->sb->attribs.inheritable.linkage3 = linkage3;
                        else if (linkage3 != lk_none)
                            error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                        if (linkage2 == lk_import)
                        {
                            sp->sb->importfile = importFile;
                        }
                        SetLinkerNames(sp, storage_class == sc_auto && isstructured(sp->tp) ? lk_auto : linkage);
                        if (inTemplate && templateNestingCount == 1)
                        {
                            inTemplateBody++;
                        }
                        if (Optimizer::cparams.prm_cplusplus && isfunction(sp->tp) && (MATCHKW(lex, kw_try) || MATCHKW(lex, colon)))
                        {
                            bool viaTry = MATCHKW(lex, kw_try);
                            if (viaTry)
                            {
                                sp->sb->hasTry = true;
                                lex = getsym();
                            }
                            if (MATCHKW(lex, colon))
                            {
                                lex = getsym();
                                sp->sb->memberInitializers = GetMemberInitializers(&lex, funcsp, sp);
                            }
                        }
                        if (inTemplate && templateNestingCount == 1)
                        {
                            noSpecializationError++;
                        }
                        if (storage_class == sc_absolute)
                            sp->sb->value.i = address;
                        if ((!Optimizer::cparams.prm_cplusplus || !getStructureDeclaration()) && !istype(sp) &&
                            sp->sb->storage_class != sc_static && isfunction(basetype(tp1)) && !MATCHKW(lex, begin))
                            sp->sb->storage_class = sc_external;
                        if (isvoid(tp1))
                            if (sp->sb->storage_class != sc_parameter && sp->sb->storage_class != sc_typedef)
                                error(ERR_VOID_NOT_ALLOWED);
                        if (sp->sb->attribs.inheritable.linkage3 == lk_threadlocal)
                        {
                            if (isfunction(tp1))
                                error(ERR_FUNC_NOT_THREAD_LOCAL);
                            if (sp->sb->storage_class != sc_external && sp->sb->storage_class != sc_global &&
                                sp->sb->storage_class != sc_static)
                            {
                                if (sp->sb->storage_class == sc_auto || sp->sb->storage_class == sc_register)
                                    error(ERR_THREAD_LOCAL_NOT_AUTO);
                                else
                                    error(ERR_THREAD_LOCAL_INVALID_STORAGE_CLASS);
                            }
                        }
                        if (isatomic(tp1))
                        {
                            if (isfunction(tp1) || (ispointer(tp1) && basetype(tp1)->array))
                                error(ERR_ATOMIC_NO_FUNCTION_OR_ARRAY);
                        }
                        if (sp->sb->attribs.inheritable.linkage2 == lk_property && isfunction(tp1))
                            error(ERR_PROPERTY_QUALIFIER_NOT_ALLOWED_ON_FUNCTIONS);
                        if (storage_class != sc_typedef && isstructured(tp1) && basetype(tp1)->sp->sb->isabstract)
                            errorabstract(ERR_CANNOT_CREATE_INSTANCE_ABSTRACT, basetype(tp1)->sp);
                        if (sp->packed)
                            error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                        if (storage_class == sc_mutable && isconst(tp))
                            errorsym(ERR_MUTABLE_NON_CONST, sp);
                        // correct for previous errors
                        if (sp->sb->isConstructor && ispointer(sp->tp))
                            sp->tp = basetype(sp->tp)->btp;

                        if (ssp && strSym && strSym->tp->type != bt_templateselector && strSym->tp->type != bt_templatedecltype)
                        {
                            if (strSym != ssp && strSym->sb->mainsym != ssp)
                            {
                                errorsym(ERR_IDENTIFIER_CANNOT_HAVE_TYPE_QUALIFIER, sp);
                            }
                        }
                        spi = nullptr;
                        if (nsv && !strSym)
                        {
                            Optimizer::LIST* rvl;
                            unvisitUsingDirectives(nsv);
                            rvl = tablesearchinline(sp->name, nsv, false);
                            if (rvl)
                                if (rvl->next)
                                    errorsym2(ERR_AMBIGUITY_BETWEEN, (SYMBOL*)rvl->data, (SYMBOL*)rvl->next->data);
                                else
                                    spi = (SYMBOL*)rvl->data;
                            else
                                errorNotMember(strSym, nsv, sp->name);
                        }
                        else
                        {
                            if (isfunction(sp->tp) && sp->sb->instantiated)
                            {
                                // may happen if a template function was declared at a lower level
                                spi = sp;
                            }
                            else
                            {
                                SYMLIST** p;
                                if ((storage_class_in == sc_auto || storage_class_in == sc_parameter) &&
                                    storage_class != sc_external && !isfunction(sp->tp))
                                {
                                    p = LookupName(sp->name, localNameSpace->valueData->syms);
                                }
                                else
                                {
                                    ssp = getStructureDeclaration();
                                    if (ssp && /*!ssp->tp->syms &&*/ ssp->sb->templateLevel)
                                    {
                                        SYMBOL* ssp2 = FindSpecialization(ssp, ssp->templateParams);
                                        if (ssp2)
                                            ssp = ssp2;
                                    }
                                    if (ssp && ssp->tp->syms && (strSym || !asFriend))
                                    {
                                        p = LookupName(sp->name, ssp->tp->syms);
                                    }
                                    else
                                    {
                                        p = LookupName(sp->name, globalNameSpace->valueData->syms);
                                    }
                                }
                                if (p)
                                {
                                    spi = (SYMBOL*)(*p)->p;
                                }
                            }
                        }
                        if (spi && spi->sb->storage_class == sc_overloads)
                        {
                            SYMBOL* sym = nullptr;
                            if (isfunction(sp->tp))
                            {
                                SYMLIST* hr = spi->tp->syms->table[0];
                                while (hr)
                                {
                                    if (!hr->p->sb->instantiated)
                                        ScrubTemplateValues(hr->p);
                                    hr = hr->next;
                                }
                                sym = searchOverloads(sp, spi->tp->syms);
                                if (sp->sb->attribs.inheritable.linkage == lk_c ||
                                    (sym && sym->sb->attribs.inheritable.linkage == lk_c))
                                    if (!sym || !sameNameSpace(sp->sb->parentNameSpace, sym->sb->parentNameSpace))
                                        preverrorsym(ERR_CONFLICTS_WITH, sp, spi->sb->declfile, spi->sb->declline);
                                if (sym && sym->templateParams && (!sp->templateParams || sp->templateParams->next) &&
                                    (!exactMatchOnTemplateParams(sym->templateParams, sp->templateParams) || ([](TEMPLATEPARAMLIST* a, TEMPLATEPARAMLIST* b) 
                                        {
                                            while (a && b)
                                            {
                                                if (a->p->byClass.txtdflt && b->p->byClass.txtdflt)
                                                    return true;
                                                a = a->next;
                                                b = b->next;
                                            }
                                            return false;
                                        })(sym->templateParams, sp->templateParams)))
                                    sym = nullptr;
                            }
                            if (sym && Optimizer::cparams.prm_cplusplus)
                            {
                                if (sym->sb->attribs.inheritable.linkage == lk_c && sp->sb->attribs.inheritable.linkage == lk_cdecl)
                                    sp->sb->attribs.inheritable.linkage = lk_c;
                                if (sp->sb->attribs.inheritable.linkage != sym->sb->attribs.inheritable.linkage &&
                                    ((sp->sb->attribs.inheritable.linkage != lk_cdecl &&
                                      sp->sb->attribs.inheritable.linkage != lk_virtual) ||
                                     (sym->sb->attribs.inheritable.linkage != lk_cdecl &&
                                      sym->sb->attribs.inheritable.linkage != lk_virtual)) &&
                                    !sp->sb->attribs.inheritable.isInline && !sym->sb->attribs.inheritable.isInline)
                                {
                                    preverrorsym(ERR_LINKAGE_MISMATCH_IN_FUNC_OVERLOAD, spi, spi->sb->declfile, spi->sb->declline);
                                }
                                else if (sym && !sym->sb->isConstructor && !sym->sb->isDestructor &&
                                         !comparetypes(basetype(sp->tp)->btp, basetype((sym)->tp)->btp, true))
                                {
                                    if (Optimizer::cparams.prm_cplusplus && isfunction(sym->tp) &&
                                        (sym->sb->templateLevel || templateNestingCount))
                                        checkReturn = false;
                                }
                            }
                            if (0 && inTemplate) // DAL look to remove
                                if (!sym)
                                {
                                    // a specialization
                                    // this may result in returning sp depending on what happens...
                                    if (sp->templateParams->p->bySpecialization.types)
                                    {
                                        SYMLIST* hr = spi->tp->syms->table[0];
                                        while (hr)
                                        {
                                            SYMBOL* sym1 = hr->p;
                                            if (sym1->sb->templateLevel)
                                            {
                                                TEMPLATEPARAMLIST* one = sp->templateParams->p->bySpecialization.types;
                                                TEMPLATEPARAMLIST* two = sym1->templateParams->next;
                                                while (one && two)
                                                {
                                                    if (one->p->type != two->p->type)
                                                        break;
                                                    one = one->next;
                                                    two = two->next;
                                                }
                                                if (!one && !two)
                                                    break;
                                            }
                                            hr = hr->next;
                                        }
                                        if (!hr)
                                            errorsym(ERR_SPECIALIZATION_REQUIRES_PRIMARY, sp);
                                        sp->sb->specialized = true;
                                    }
                                    else if (sp->templateParams && !sp->templateParams->next)
                                        sp->sb->specialized = true;
                                    sym = LookupFunctionSpecialization(spi, sp);
                                    if (sym == sp && spi->sb->storage_class != sc_overloads)
                                    {
                                        sym = spi;
                                    }
                                    else if (sym)
                                    {
                                        InsertSymbol(sym, storage_class == sc_typedef ? storage_class_in : storage_class, linkage,
                                                     false);
                                    }
                                    spi = nullptr;
                                }
                            if (sym && sym->sb->templateLevel != sp->sb->templateLevel &&
                                (sym->sb->templateLevel || !sp->sb->templateLevel || sp->templateParams->next) &&
                                (!strSym || !strSym->sb->templateLevel || sym->sb->templateLevel != sp->sb->templateLevel + 1))
                                sym = nullptr;
                            if (sym)
                                spi = sym;
                            else
                            {
                                if ((nsv || strSym) && storage_class_in != sc_member && storage_class_in != sc_mutable &&
                                    (!inTemplate || !sp->templateParams))
                                {
                                    char buf[256];
                                    if (!strcmp(sp->name, overloadNameTab[CI_CONSTRUCTOR]))
                                        strcpy(buf, strSym->name);
                                    else if (!strcmp(sp->name, overloadNameTab[CI_DESTRUCTOR]))
                                    {
                                        buf[0] = '~';
                                        strcpy(buf + 1, strSym->name);
                                    }
                                    else
                                        strcpy(buf, sp->name);
                                    errorNotMember(strSym, nsv, buf);
                                }
                                spi = nullptr;
                            }
                        }
                        else if (!spi)
                        {
                            if (sp->templateParams && sp->templateParams->p->bySpecialization.types)
                                errorsym(ERR_SPECIALIZATION_REQUIRES_PRIMARY, sp);
                            if (strSym && storage_class_in != sc_member && storage_class_in != sc_mutable)
                            {
                                errorNotMember(strSym, nsv, sp->name);
                            }
                        }
                        else
                        {
                            if (sp->sb->storage_class == sc_member && spi->sb->storage_class == sc_external)
                                sp->sb->storage_class = sc_global;
                        }
                        if ((!spi || (spi->sb->storage_class != sc_member && spi->sb->storage_class != sc_mutable)) &&
                            sp->sb->storage_class == sc_global && sp->sb->attribs.inheritable.isInline && !sp->sb->promotedToInline)
                        {
                            if (spi && spi->sb->storage_class == sc_external)
                                sp->sb->dumpInlineToFile = true;
                            else
                                sp->sb->storage_class = sc_static;
                        }
                        if (spi && !sp->sb->parentClass && !isfunction(spi->tp) && spi->sb->storage_class != sc_type &&
                            sp->sb->templateLevel)
                        {
                            SYMBOL* special = FindSpecialization(spi, sp->templateParams);
                            if (!special)
                            {
                                SYMLIST* member = (SYMLIST*)Alloc(sizeof(SYMLIST));
                                member->p = sp;
                                member->next = spi->sb->specializations;
                                spi->sb->specializations = member;
                                sp->sb->parentTemplate = spi;
                            }
                        }
                        else if (spi)
                        {
                            if (inTemplate)
                            {
                                if (spi != sp)
                                {
                                    TEMPLATEPARAMLIST* templateParams = TemplateGetParams(sp);
                                    if (templateParams->p->bySpecialization.types && spi->sb->parentTemplate)
                                    {
                                        spi->templateParams =
                                            TemplateMatching(lex, spi->sb->parentTemplate->templateParams, templateParams, spi,
                                                             MATCHKW(lex, begin) || MATCHKW(lex, colon));
                                    }
                                    else
                                    {
                                        spi->templateParams = TemplateMatching(lex, spi->templateParams, templateParams, spi,
                                                                               MATCHKW(lex, begin) || MATCHKW(lex, colon));
                                    }
                                }
                            }
                            else if (spi->sb->templateLevel && !spi->sb->instantiated && !templateNestingCount)
                            {
                                if (!strSym || !strSym->sb->templateLevel || spi->sb->templateLevel != sp->sb->templateLevel + 1)
                                    errorsym(ERR_IS_ALREADY_DEFINED_AS_A_TEMPLATE, sp);
                            }
                            if (isfunction(spi->tp))
                            {
                                matchFunctionDeclaration(lex, sp, spi, checkReturn);
                            }
                            if (sp->sb->parentClass)
                            {
                                if (spi->sb->storage_class != sc_member && sp->sb->storage_class != sc_member &&
                                    spi->sb->storage_class != sc_mutable && sp->sb->storage_class != sc_mutable)
                                    if (spi->sb->storage_class != sc_external && sp->sb->storage_class != sc_external)
                                        preverrorsym(ERR_DUPLICATE_IDENTIFIER, spi, spi->sb->declfile, spi->sb->declline);
                            }
                            else
                            {
                                sp->sb->parentClass = strSym;
                                if (!sp->sb->parentClass && spi->sb->parentClass)  // error handling
                                    sp->sb->parentClass = spi->sb->parentClass;
                            }
                            if (sp->sb->constexpression)
                                spi->sb->constexpression = true;
                            if (istype(spi))
                            {
                                if (Optimizer::cparams.prm_ansi || !comparetypes(sp->tp, (spi)->tp, true) || !istype(sp))
                                    preverrorsym(ERR_REDEFINITION_OF_TYPE, sp, spi->sb->declfile, spi->sb->declline);
                            }
                            else
                            {
                                if (spi->sb->pushedTemplateSpecializationDefinition && (MATCHKW(lex, begin) || MATCHKW(lex, colon)))
                                {
                                    spi->sb->pushedTemplateSpecializationDefinition = false;
                                    spi->sb->inlineFunc.stmt = nullptr;
                                    spi->sb->deferredCompile = nullptr;
                                }
                                if ((isfunction(spi->tp) && (spi->sb->inlineFunc.stmt || spi->sb->deferredCompile) &&
                                     (MATCHKW(lex, begin) || MATCHKW(lex, colon)) &&
                                     (!spi->sb->parentClass || !spi->sb->parentClass->sb->instantiated ||
                                      !spi->sb->copiedTemplateFunction)) &&
                                    spi->sb->parentClass &&
                                    !differentTemplateNames(spi->sb->parentClass->templateParams,
                                                            sp->sb->parentClass->templateParams))
                                {
                                    errorsym(ERR_BODY_ALREADY_DEFINED_FOR_FUNCTION, sp);
                                }
                                else if ((!isfunction(sp->tp) && !isfunction(spi->tp) && !comparetypes(sp->tp, (spi)->tp, true)) ||
                                         istype(sp))
                                {
                                    preverrorsym(ERR_TYPE_MISMATCH_IN_REDECLARATION, sp, spi->sb->declfile, spi->sb->declline);
                                }
                                else if (!sameQuals(sp->tp, spi->tp))
                                {
                                    errorsym(ERR_DECLARATION_DIFFERENT_QUALIFIERS, sp);
                                }
                                if (sp->sb->attribs.inheritable.linkage3 != spi->sb->attribs.inheritable.linkage3 &&
                                    (sp->sb->attribs.inheritable.linkage3 == lk_threadlocal ||
                                     spi->sb->attribs.inheritable.linkage3 == lk_threadlocal))
                                    if (!spi->sb->parentClass)
                                        errorsym(ERR_THREAD_LOCAL_MUST_ALWAYS_APPEAR, sp);
                                if (strSym && storage_class == sc_static)
                                {
                                    errorstr(ERR_INVALID_STORAGE_CLASS, "static");
                                }
                                if (ismember(spi))
                                {
                                    if (!asFriend && !isfunction(sp->tp) && sp->sb->storage_class != spi->sb->storage_class &&
                                        !sp->sb->templateLevel)
                                        errorsym(ERR_CANNOT_REDECLARE_OUTSIDE_CLASS, sp);
                                    if (strSym)
                                    {
                                        sp->sb->storage_class = spi->sb->storage_class;
                                    }
                                }
                                switch (sp->sb->storage_class)
                                {
                                    default:
                                        break;
                                    case sc_auto:
                                    case sc_register:
                                    case sc_parameter:
                                        preverrorsym(ERR_DUPLICATE_IDENTIFIER, sp, spi->sb->declfile, spi->sb->declline);
                                        break;
                                    case sc_external:
                                        if ((spi)->sb->storage_class == sc_static)
                                        {
                                            if (!isfunction(spi->tp))
                                            {
                                                if (spi->sb->constexpression)
                                                {
                                                    spi->sb->storage_class = sc_global;
                                                }
                                                else
                                                {
                                                    errorsym(ERR_ANSI_FORBID_BOTH_EXTERN_STATIC, spi);
                                                    spi->sb->storage_class = sc_global;
                                                }
                                            }
                                            else if (spi->sb->attribs.inheritable.isInline && basetype(spi->tp)->type == bt_ifunc)
                                            {
                                                spi->sb->storage_class = sc_global;
                                            }
                                        }
                                        break;
                                    case sc_global:
                                        if (spi->sb->storage_class != sc_static)
                                        {
                                            spi->sb->declfile = sp->sb->declfile;
                                            spi->sb->declline = sp->sb->declline;
                                            spi->sb->realdeclline = sp->sb->realdeclline;
                                            spi->sb->declfilenum = sp->sb->declfilenum;
                                            spi->sb->storage_class = sc_global;
                                        }
                                        break;
                                    case sc_mutable:
                                        if ((spi)->sb->storage_class != sc_mutable)
                                        {
                                            preverrorsym(ERR_DUPLICATE_IDENTIFIER, sp, spi->sb->declfile, spi->sb->declline);
                                        }
                                        break;
                                    case sc_member:
                                        if ((spi)->sb->storage_class == sc_static)
                                        {
                                            preverrorsym(ERR_DUPLICATE_IDENTIFIER, sp, spi->sb->declfile, spi->sb->declline);
                                        }
                                        break;
                                    case sc_static:
                                        if ((spi)->sb->storage_class == sc_external && !isfunction(spi->tp) &&
                                            !spi->sb->constexpression)
                                        {
                                            errorsym(ERR_ANSI_FORBID_BOTH_EXTERN_STATIC, spi);
                                            spi->sb->storage_class = sc_global;
                                        }
                                        else if (ismember(spi))
                                        {
                                            preverrorsym(ERR_DUPLICATE_IDENTIFIER, sp, spi->sb->declfile, spi->sb->declline);
                                        }
                                        else
                                        {
                                            spi->sb->storage_class = sp->sb->storage_class;
                                        }
                                        break;
                                    case sc_localstatic:
                                    case sc_constexpr:
                                        spi->sb->storage_class = sp->sb->storage_class;
                                        break;
                                }
                            }
                            if (isfunction(spi->tp))
                            {
                                TEMPLATEPARAMLIST** nw = &sp->templateParams;
                                while (*nw)
                                {
                                    if ((*nw)->argsym)
                                    {
                                        TEMPLATEPARAMLIST* old = spi->templateParams;
                                        while (old)
                                        {
                                            if (old->argsym)
                                            {
                                                if (!strcmp((*nw)->argsym->name, old->argsym->name))
                                                    break;
                                            }
                                            old = old->next;
                                        }
                                        if (!old)
                                        {
                                            *nw = (*nw)->next;
                                        }
                                        else
                                        {
                                            nw = &(*nw)->next;
                                        }
                                    }
                                    else
                                    {
                                        nw = &(*nw)->next;
                                    }
                                }
                                if (Optimizer::cparams.prm_cplusplus &&
                                    (MATCHKW(lex, begin) || MATCHKW(lex, colon) || MATCHKW(lex, kw_try)))
                                {
                                    spi->templateParams = sp->templateParams;
                                }
                                if (!asFriend || MATCHKW(lex, colon) || MATCHKW(lex, begin))
                                {
                                    spi->tp = sp->tp;
                                    spi->tp->sp = spi;
                                }
                            }
                            else if (spi->tp->size == 0)
                                spi->tp = sp->tp;
                            if (sp->sb->oldstyle)
                                spi->sb->oldstyle = true;
                            if (!spi->sb->declfile ||
                                spi->tp->type == bt_func)  // && !spi->sb->parentClass && !spi->sb->specialized)
                            {
                                spi->sb->declfile = sp->sb->declfile;
                                spi->sb->declline = sp->sb->declline;
                                spi->sb->realdeclline = sp->sb->realdeclline;
                                spi->sb->declfilenum = sp->sb->declfilenum;
                            }
                            spi->sb->memberInitializers = sp->sb->memberInitializers;
                            spi->sb->hasTry = sp->sb->hasTry;

                            spi->sb->attribs.inheritable.isInline |= sp->sb->attribs.inheritable.isInline;
                            spi->sb->dumpInlineToFile |= sp->sb->dumpInlineToFile;
                            spi->sb->promotedToInline |= sp->sb->promotedToInline;

                            spi->sb->parentClass = sp->sb->parentClass;

                            sp->sb->mainsym = spi;

                            sp = spi;
                            sp->sb->redeclared = true;
                        }
                        else
                        {
                            if (inTemplate)
                            {
                                TEMPLATEPARAMLIST* templateParams = TemplateGetParams(sp);
                                if (isfunction(sp->tp) && templateParams->p->bySpecialization.types && templateParams->next)
                                {
                                    error(ERR_FUNCTION_TEMPLATE_NO_PARTIAL_SPECIALIZE);
                                }
                                else if (!isfunction(tp) && templateParams->next && templateParams->p->bySpecialization.types)
                                {
                                    TemplateValidateSpecialization(templateParams);
                                }
                                else if (templateNestingCount == 1)
                                {
                                    TemplateMatching(lex, nullptr, templateParams, sp, MATCHKW(lex, begin) || MATCHKW(lex, colon));
                                }
                            }
                            if (isfunction(sp->tp))
                            {
                                matchFunctionDeclaration(lex, sp, sp, checkReturn);
                                if (inTemplate)
                                    sp->sb->parentTemplate = sp;
                            }
                            else if (sp->sb->storage_class != sc_type)
                            {
                                if (inTemplate)
                                    sp->sb->parentTemplate = sp;
                            }
                            if (!asFriend || isfunction(sp->tp))
                            {
                                if (sp->sb->constexpression && sp->sb->storage_class == sc_global)
                                    sp->sb->storage_class = sc_static;
                                if (!asFriend || !templateNestingCount || inTemplate)
                                {
                                    if (sp->sb->storage_class == sc_external ||
                                        (asFriend && !MATCHKW(lex, begin) && !MATCHKW(lex, colon)))
                                    {
                                        InsertSymbol(sp, sc_external, linkage, false);
                                        if (sp->sb->templateLevel)
                                        {
                                            InsertInline(sp);
                                        }
                                    }
                                    else
                                    {
                                        InsertSymbol(sp, storage_class == sc_typedef ? storage_class_in : storage_class, linkage,
                                                     false);
                                        if (isfunction(sp->tp))
                                        {
                                            if (getStructureDeclaration() || asFriend)
                                            {
                                                SYMBOL* parent = getStructureDeclaration();

                                                while (parent)
                                                {
                                                    if (parent->sb->templateLevel)
                                                    {
                                                        InsertInline(sp);
                                                        break;
                                                    }
                                                    parent = parent->sb->parentClass;
                                                }
                                            }
                                        }
                                        else if (sp->sb->templateLevel && (sp->sb->storage_class == sc_type || isfunction(sp->tp)))
                                        {
                                            InsertInline(sp);
                                        }
                                    }
                                }
                            }
                            if (asFriend && !sp->sb->anonymous && !isfunction(sp->tp) && !templateNestingCount)
                            {
                                error(ERR_DECLARATOR_NOT_ALLOWED_HERE);
                            }
                        }

                        if (asFriend)
                        {
                            if (isfunction(sp->tp))
                            {
                                SYMBOL* sym = nullptr;
                                Optimizer::LIST* l = (Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
                                STRUCTSYM* ls = structSyms;
                                while (ls && !ls->str)
                                    ls = ls->next;
                                if (strSym && strSym->tp->type != bt_enum && strSym->tp->type != bt_templateselector &&
                                    strSym->tp->type != bt_templatedecltype)
                                {
                                    ls = ls->next;
                                    while (ls && !ls->str)
                                        ls = ls->next;
                                }
                                if (ls)
                                    sym = ls->str;
                                l->data = (void*)sp;
                                l->next = sym->sb->friends;
                                sym->sb->friends = l;
                            }
                        }
                        if (!ismember(sp) && !sp->sb->constexpression && !istype(sp))
                        {
                            if (isfunction(sp->tp) && (isconst(sp->tp) || isvolatile(sp->tp)))
                                error(ERR_ONLY_MEMBER_CONST_VOLATILE);
                        }
                        if (inTemplate && templateNestingCount == 1)
                        {
                            inTemplateBody--;
                            noSpecializationError--;
                        }
                    }
                    if (sp)
                    {
                        if (!strcmp(sp->name, overloadNameTab[CI_DESTRUCTOR]))
                        {
                            if (!isfunction(tp1))
                            {
                                errorsym(ERR_CONSTRUCTOR_OR_DESTRUCTOR_MUST_BE_FUNCTION, sp->sb->parentClass);
                            }
                            else
                            {
                                SYMLIST* hr = basetype(tp1)->syms->table[0];
                                SYMBOL* s = hr->p;
                                if (s->tp->type != bt_void)
                                    errorsym(ERR_DESTRUCTOR_CANNOT_HAVE_PARAMETERS, sp->sb->parentClass);
                            }
                        }
                        else if (!strcmp(sp->name, overloadNameTab[CI_CONSTRUCTOR]))
                        {
                            if (!isfunction(tp1))
                                errorsym(ERR_CONSTRUCTOR_OR_DESTRUCTOR_MUST_BE_FUNCTION, sp->sb->parentClass);
                        }
                    }
                    if (!templateArg)
                        checkDeclarationAccessible(sp->tp, isfunction(sp->tp) ? sp : nullptr);
                    if (sp->sb->operatorId)
                        checkOperatorArgs(sp, asFriend);
                    if (sp->sb->storage_class == sc_typedef)
                    {
                        // all this is so we can have multiple typedefs referring to the same thing...
                        TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
                        tp->type = bt_typedef;
                        tp->btp = sp->tp;
                        UpdateRootTypes(tp);
                        sp->tp = tp;
                        tp->sp = sp;
                        tp->size = tp->btp->size;
                    }
                    if (ispointer(sp->tp) && sp->sb->storage_class != sc_parameter)
                    {
                        btp = basetype(sp->tp);
                        if (btp->vla)
                        {
                            allocateVLA(lex, sp, funcsp, block, btp, false);
                        }
                        else
                        {
                            btp = basetype(btp->btp);
                            if (btp->vla)
                            {
                                allocateVLA(lex, sp, funcsp, block, btp, true);
                                btp->size = basetype(sp->tp)->size;
                            }
                        }
                    }
                    UpdateRootTypes(tp1);
                    sizeQualifiers(tp1);
                    if (sp->sb->attribs.inheritable.isInline)
                    {
                        if (!isfunction(sp->tp))
                            error(ERR_INLINE_NOT_ALLOWED);
                    }
                    if (inTemplate && templateNestingCount == 1)
                    {
                        inTemplateBody++;
                        noSpecializationError++;
                    }

                    if (lex)
                    {
                        if (linkage != lk_cdecl)
                            sp->sb->attribs.inheritable.linkage = linkage;
                        if (linkage2 != lk_none)
                            sp->sb->attribs.inheritable.linkage2 = linkage2;
                        if (linkage2 == lk_import)
                        {
                            sp->sb->importfile = importFile;
                        }
                        if (basetype(sp->tp)->type == bt_func)
                        {
                            if (funcsp && storage_class == sc_localstatic)
                                errorstr(ERR_INVALID_STORAGE_CLASS, "static");
                            if (storage_class == sc_member && !Optimizer::cparams.prm_cplusplus)
                                error(ERR_FUNCTION_NOT_IN_STRUCT);
                            if (isstructured(basetype(sp->tp)->btp) || basetype(basetype(sp->tp)->btp)->type == bt_enum)
                                if (defd)
                                    errorsym(ERR_TYPE_DEFINITION_NOT_ALLOWED_HERE, basetype(sp->tp->btp)->sp);
                            if (sp->sb->castoperator)
                            {
                                if (!notype)
                                    errortype(ERR_TYPE_OF_RETURN_IS_IMPLICIT_FOR_OPERATOR_FUNC, basetype(sp->tp)->btp, nullptr);
                            }
                            else
                            {
                                if (notype)
                                    if (!sp->sb->isConstructor && !sp->sb->isDestructor)
                                        error(ERR_MISSING_TYPE_SPECIFIER);
                            }
                            if (Optimizer::cparams.prm_cplusplus)
                            {
                                if (ismember(sp))
                                {
                                    injectThisPtr(sp, basetype(sp->tp)->syms);
                                }
                            }
                            if (sp->sb->storage_class == sc_static && (Optimizer::architecture == ARCHITECTURE_MSIL))
                            {
                                if (!sp->sb->label)
                                    sp->sb->label = Optimizer::nextLabel++;
                                sp->sb->uniqueID = fileIndex;
                            }

                            if (/*templateNestingCount &&*/ nameSpaceList)
                                SetTemplateNamespace(sp);
                            if (MATCHKW(lex, begin))
                            {
                                TYPE* tp = sp->tp;
                                SYMLIST* hr;
                                if (sp->sb->storage_class == sc_member && storage_class_in == sc_member)
                                    browse_variable(sp);
                                if (sp->sb->storage_class == sc_external)
                                    sp->sb->storage_class = sc_global;
                                while (tp->type != bt_func)
                                    tp = tp->btp;
                                tp->type = bt_ifunc;
                                hr = tp->syms->table[0];

                                while (hr)
                                {
                                    SYMBOL* sym = hr->p;
                                    TYPE* tpl = sym->tp;
                                    while (tpl)
                                    {
                                        if (tpl->unsized)
                                        {
                                            specerror(ERR_UNSIZED_VLA_PARAMETER, sym->name, sym->sb->declfile, sym->sb->declline);
                                            while (hr->next)
                                                hr = hr->next;
                                            break;  // hmmmmm
                                        }
                                        tpl = tpl->btp;
                                    }
                                    hr = hr->next;
                                }
                                if (storage_class_in == sc_member || storage_class_in == sc_mutable)
                                {
                                    sp->sb->instantiatedInlineInClass = true;
                                }
                                if (storage_class_in != sc_member && TemplateFullySpecialized(sp->sb->parentClass))
                                {
                                    sp->sb->attribs.inheritable.linkage = lk_virtual;
                                    lex = body(lex, sp);
                                }
                                else if (storage_class_in == sc_member || storage_class_in == sc_mutable ||
                                         templateNestingCount == 1 || (asFriend && templateNestingCount == 2))
                                {
                                    STATEMENT* startStmt = currentLineData(nullptr, lex, 0);
                                    if (startStmt)
                                        sp->sb->linedata = startStmt->lineData;
                                    lex = getDeferredData(lex, &sp->sb->deferredCompile, true);
                                    InsertInline(sp);
                                    if (sp->sb->parentClass && !templateNestingCount &&
                                        (sp->sb->storage_class == sc_virtual || sp->sb->storage_class == sc_global))
                                    {
                                        if (sp->templateParams && !sp->templateParams->next)
                                        {
                                            sp->sb->templateLevel = 0;
                                            sp->tp = SynthesizeType(sp->tp, sp->sb->parentClass->templateParams, false);
                                            sp = TemplateFunctionInstantiate(sp, false, false);
                                            sp->sb->specialized2 = true;
                                        }
                                    }
                                }
                                else
                                {
                                    lex = body(lex, sp);
                                }
                                if (sp->sb->constexpression)
                                {
                                    sp->sb->attribs.inheritable.isInline = true;
                                    if (sp->sb->nonConstVariableUsed)
                                        errorsym(ERR_CONSTANT_FUNCTION_EXPECTED, sp);
                                }
                                needsemi = false;
                            }
                            else
                            {
                                browse_variable(sp);
                                if (sp->sb->memberInitializers)
                                    errorsym(ERR_CONSTRUCTOR_MUST_HAVE_BODY, sp);
                                else if (sp->sb->hasTry)
                                    error(ERR_EXPECTED_TRY_BLOCK);
                                if (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, assign))
                                {
                                    lex = getsym();
                                    if (MATCHKW(lex, kw_delete))
                                    {
                                        sp->sb->deleted = true;
                                        if (sp->sb->redeclared && !templateNestingCount)
                                        {
                                            errorsym(ERR_DELETE_ON_REDECLARATION, sp);
                                        }
                                        lex = getsym();
                                    }
                                    else if (MATCHKW(lex, kw_default))
                                    {
                                        sp->sb->defaulted = true;
                                        SetParams(sp);
                                        // fixme add more
                                        if (strcmp(sp->name, overloadNameTab[CI_CONSTRUCTOR]) != 0 &&
                                            strcmp(sp->name, overloadNameTab[CI_DESTRUCTOR]) != 0 &&
                                            strcmp(sp->name, overloadNameTab[assign - kw_new + CI_NEW]) !=
                                                0)  // this is meant to be a copy cons but is too loose
                                            error(ERR_DEFAULT_ONLY_SPECIAL_FUNCTION);

                                        lex = getsym();
                                    }
                                    else if (lex->type != l_i || lex->value.i != 0)
                                    {
                                        error(ERR_PURE_SPECIFIER_CONST_ZERO);
                                    }
                                    else
                                    {
                                        lex = getsym();
                                        sp->sb->ispure = true;
                                    }
                                }
                                else if (sp->sb->constexpression && sp->sb->storage_class != sc_external && !isfunction(sp->tp))
                                {
                                    error(ERR_CONSTEXPR_REQUIRES_INITIALIZER);
                                }
                                else if (sp->sb->parentClass && !sp->templateParams &&
                                         !(Optimizer::architecture == ARCHITECTURE_MSIL))
                                    if (!asFriend && storage_class_in != sc_member && storage_class_in != sc_mutable &&
                                        !sp->sb->templateLevel)
                                        errorsym(ERR_CANNOT_REDECLARE_OUTSIDE_CLASS, sp);
                            }
                        }
                        else
                        {
                            LEXEME* hold = lex;
                            bool structuredArray = false;
                            if (notype)
                                errorsym(ERR_UNDEFINED_IDENTIFIER_EXPECTING_TYPE, sp);
                            if (linkage3 == lk_entrypoint)
                            {
                                errorsym(ERR_ENTRYPOINT_FUNC_ONLY, sp);
                            }
                            if (sp->sb->storage_class == sc_virtual)
                            {
                                errorsym(ERR_NONFUNCTION_CANNOT_BE_DECLARED_VIRTUAL, sp);
                            }
                            if (asExpression && !(asExpression & _F_NOCHECKAUTO))
                                checkauto(sp->tp, ERR_AUTO_NOT_ALLOWED);
                            if (sp->sb->storage_class == sc_auto || sp->sb->storage_class == sc_register)
                            {
                                STATEMENT* s = stmtNode(lex, block, st_varstart);
                                s->select = varNode(en_auto, sp);
                            }
                            if (!sp->sb->label && (sp->sb->storage_class == sc_static || sp->sb->storage_class == sc_localstatic) &&
                                (Optimizer::architecture == ARCHITECTURE_MSIL))
                                sp->sb->label = Optimizer::nextLabel++;
                            if (Optimizer::cparams.prm_cplusplus && sp->sb->storage_class != sc_type &&
                                sp->sb->storage_class != sc_typedef && structLevel && (!instantiatingTemplate) && !funcsp &&
                                (MATCHKW(lex, assign) || MATCHKW(lex, begin)))
                            {
                                if ((MATCHKW(lex, assign) || MATCHKW(lex, begin)) && storage_class_in == sc_member &&
                                    (sp->sb->storage_class == sc_static || sp->sb->storage_class == sc_external))
                                {
                                    if (isconst(sp->tp) || sp->sb->constexpression)
                                    {
                                        if (isint(sp->tp))
                                            goto doInitialize;
                                    }
                                    else
                                    {
                                        errorsym(ERR_CANNOT_INITIALIZE_STATIC_MEMBER_IN_CLASS, sp);
                                    }
                                }
                                lex = getDeferredData(lex, &sp->sb->deferredCompile, false);
                            }
                            else
                            {
                            doInitialize:
                                if (Optimizer::cparams.prm_cplusplus && isstructured(sp->tp))
                                {
                                    SYMBOL* sp1 = basetype(sp->tp)->sp;
                                    if (!templateNestingCount && sp1->sb->templateLevel && sp1->templateParams &&
                                        !sp1->sb->instantiated)
                                    {
                                        if (!allTemplateArgsSpecified(sp1, sp1->templateParams))
                                            sp1 = GetClassTemplate(sp1, sp1->templateParams->next, false);
                                        if (sp1)
                                            sp->tp = PerformDeferredInitialization(
                                                sp1->tp, funcsp);  // TemplateClassInstantiate(sp1, sp1->templateParams, false,
                                                                   // sc_global)->tp;
                                    }
                                }
                                lex = initialize(lex, funcsp, sp, storage_class_in, asExpression, 0); /* also reserves space */
                                if (sp->sb->parentClass && sp->sb->storage_class == sc_global)
                                {
                                    if (sp->templateParams && !sp->templateParams->next)
                                    {
                                        SetLinkerNames(sp, lk_cdecl);
                                        InsertInlineData(sp);
                                    }
                                    else if ((!sp->sb->parentClass ||
                                              (sp->sb->parentClass->templateParams &&
                                               allTemplateArgsSpecified(sp->sb->parentClass,
                                                                        sp->sb->parentClass->templateParams->next))) &&
                                             (!sp->templateParams || allTemplateArgsSpecified(sp, sp->templateParams->next)))
                                    {
                                        SetLinkerNames(sp, lk_cdecl);
                                        InsertInlineData(sp);
                                    }
                                }
                                if (sp->sb->storage_class == sc_auto || sp->sb->storage_class == sc_register ||
                                    (sp->sb->storage_class == sc_localstatic && sp->sb->init))
                                {
                                    bool doit = true;
                                    if (sp->sb->storage_class == sc_localstatic)
                                    {
                                        INITIALIZER* init = sp->sb->init;
                                        while (init)
                                        {
                                            if (init->exp && !IsConstantExpression(init->exp, false, false))
                                                break;
                                            init = init->next;
                                        }
                                        if (!init)
                                            doit = false;
                                    }
                                    if (doit && (sp->sb->init || (isarray(sp->tp) && sp->tp->msil)))
                                    {
                                        STATEMENT* st;
                                        currentLineData(block, hold, 0);
                                        st = stmtNode(hold, block, st_expr);
                                        st->select =
                                            convertInitToExpression(sp->tp, sp, nullptr, funcsp, sp->sb->init, nullptr, false);
                                    }
                                    else if ((isarray(sp->tp) || isstructured(sp->tp)) && Optimizer::architecture == ARCHITECTURE_MSIL)
                                    {
                                        if (sp->sb->storage_class != sc_localstatic)
                                        {
                                            STATEMENT* st;
                                            if (block->next->type == kw_switch)
                                            {
                                                // have to put initializations before the switch not in the switch body they cannot be accesed in the switch body
                                                STATEMENT **bd = &block->next->next->head;
                                                while ((*bd)->next)
                                                    bd = &(*bd)->next;
                                                st = stmtNode(hold, nullptr, st_expr);
                                                st->next = *bd;
                                                *bd = st;
                                            }
                                            else
                                            {
                                                st = stmtNode(hold, block, st_expr);
                                            }
                                            st->select = exprNode(en__initobj, varNode(en_auto, sp), nullptr);
                                        }
                                    }
                                }
                            }
                        }
                        if (sp->tp->size == 0 && basetype(sp->tp)->type != bt_templateparam &&
                            basetype(sp->tp)->type != bt_templateselector && !isarray(sp->tp))
                        {
                            if (storage_class != sc_typedef)
                            {
                                if (storage_class_in == sc_auto && sp->sb->storage_class != sc_external && !isfunction(sp->tp))
                                    errorsym(ERR_UNSIZED, sp);
                                if (storage_class_in == sc_parameter)
                                    errorsym(ERR_UNSIZED, sp);
                            }
                        }
                        sp->tp->used = true;
                    }
                    if (isExplicit && !sp->sb->castoperator && !sp->sb->isConstructor)
                        error(ERR_EXPLICIT_CONSTRUCTOR_OR_CONVERSION_FUNCTION);
                    if (inTemplate && templateNestingCount == 1)
                    {
                        inTemplateBody--;
                        noSpecializationError--;
                        TemplateGetDeferred(sp);
                    }
                    if (!strcmp(sp->name, "main") && !sp->sb->parentClass)
                    {
                        // fixme don't check if in parent class...
                        if (!globalNameSpace->next)
                        {
                            if (sp->sb->attribs.inheritable.isInline)
                            {
                                error(ERR_MAIN_CANNOT_BE_INLINE_FUNC);
                            }
                            else if (storage_class == sc_static)
                            {
                                error(ERR_MAIN_CANNOT_BE_STATIC_FUNC);
                            }
                            else if (sp->sb->constexpression)
                            {
                                error(ERR_MAIN_CANNOT_BE_CONSTEXPR);
                            }
                            else if (sp->sb->deleted)
                            {
                                error(ERR_MAIN_CANNOT_BE_DELETED);
                            }
                        }
                    }
                    linkage = lk_none;
                    linkage2 = lk_none;
                    if (nsv)
                    {
                        globalNameSpace = oldGlobals;
                    }
                    if (strSym && strSym->tp->type != bt_enum && strSym->tp->type != bt_templateselector)
                    {
                        dropStructureDeclaration();
                    }
                } while (!asExpression && !inTemplate && MATCHKW(lex, comma) && (lex = getsym()) != nullptr);
            }
        }
    }
    FlushLineData(preProcessor->GetRealFile().c_str(), preProcessor->GetRealLineNo());
    if (needsemi && !asExpression)
        if (templateNestingCount || !needkw(&lex, semicolon))
        {
            errskim(&lex, skim_semi_declare);
            skip(&lex, semicolon);
        }
    return lex;
}
}  // namespace Parser