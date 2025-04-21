/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
#include <cwchar>
#include "Utils.h"
#include "PreProcessor.h"
#include "ioptimizer.h"
#include "ccerr.h"
#include "declare.h"
#include "config.h"
#include "ildata.h"
#include "initbackend.h"
#include "occparse.h"
#include "declcpp.h"
#include "mangle.h"
#include "lex.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "expr.h"
#include "stmt.h"
#include "help.h"
#include "memory.h"
#include "OptUtils.h"
#include "constopt.h"
#include "declcons.h"
#include "init.h"
#include "inline.h"
#include "beinterf.h"
#include "osutil.h"
#include "types.h"
#include "browse.h"
#include "ildata.h"
#include "libcxx.h"
#include "constexpr.h"
#include "namespace.h"
#include "symtab.h"
#include "ListFactory.h"
#include "class.h"
#include "overload.h"

namespace Parser
{

int inDefaultParam;
char deferralBuf[100000];
SYMBOL* enumSyms;
EnclosingDeclarations enclosingDeclarations;
int expandingParams;
Optimizer::LIST* deferred;
int structLevel;
Optimizer::LIST* openStructs;
int parsingTrailingReturnOrUsing;
int inTypedef;
int resolvingStructDeclarations;
std::map<int, SYMBOL*> localAnonymousUnions;
int declaringInitialType;

static int unnamed_tag_id, unnamed_id;
static char* importFile;
static unsigned symbolKey;
static int nameshim;
static std::map<SYMBOL*, std::list<LexList*>> structureStaticAsserts;

void declare_init(void)
{
    unnamed_tag_id = 1;
    unnamed_id = 1;
    enclosingDeclarations.clear();
    nameSpaceList.clear();
    anonymousNameSpaceName[0] = 0;
    deferred = nullptr;
    structLevel = 0;
    inDefaultParam = 0;
    openStructs = nullptr;
    argumentNesting = 0;
    symbolKey = 0;
    noNeedToSpecialize = 0;
    inConstantExpression = 0;
    parsingUsing = 0;
    structureStaticAsserts.clear();
    declaringInitialType = 0;
}
void EnclosingDeclarations::Add(SYMBOL* symbol)
{
    //    symbol->tp->InstantiateDeferred();
    declarations.push_front(EnclosingDeclaration{symbol, nullptr});
}

void InsertGlobal(SYMBOL* sp) { Optimizer::globalCache.push_back(Optimizer::SymbolManager::Get(sp)); }
void WeedExterns()
{
    for (auto it = Optimizer::externals.begin(); it != Optimizer::externals.end();)
    {
        Optimizer::SimpleSymbol* sym = *it;
        if ((sym->ispure || sym->generated) && !sym->dontinstantiate)
        {
            it = Optimizer::externals.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
static const char* NewTagName(void)
{
    char buf[256];
    Optimizer::my_sprintf(buf, "Unnamed++%d", unnamed_tag_id++);
    return litlate(buf);
}
const char* NewUnnamedID(void)
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
static unsigned TypeCRC(SYMBOL* sp);
const char* AnonymousTypeName(SYMBOL* sp, SymbolTable<SYMBOL>* table)
{
    char buf[512];
    // type name will depend on file name
    auto name = sp->sb->declfile;
    unsigned fileCRC = Utils::CRC32((const unsigned char*)name, strlen(name));
    // type name will also depend on the structure/elements defined for the type
    unsigned typeCRC = TypeCRC(sp);

    // generate type name
    sprintf(buf, "__anontype_%08x_%08x", fileCRC, typeCRC);

    // if it doesn't exist we are done...
    if (search(table, buf) == nullptr)
        return litlate(buf);

    // ok so now we have the rare case where two type names collide, add an index value to make it unique...
    // note this may not result in 'correct' behavior if we also have the case where the two names are colliding
    // and they are also being conditionally included in different source files...   we could use a stronger Keyword::hash_ function
    // i guess but for now this will do...
    int i = 0;
    for (i = 1; i; ++i)
    {
        sprintf(buf, "__anontype_%08x_%08x_%d", fileCRC, typeCRC, i);
        if (search(table, buf) == nullptr)
            return litlate(buf);
    }
    // should never get here...
    return nullptr;
}
SYMBOL* SymAlloc()
{
    SYMBOL* sp = Allocate<SYMBOL>();
    sp->sb = Allocate<sym::_symbody>();
    return sp;
}
SYMBOL* makeID(StorageClass storage_class, Type* tp, SYMBOL* spi, const char* name)
{
    SYMBOL* sp = SymAlloc();
    LexList* lex = context->cur ? context->cur->prev : context->last;
    if (name && strstr(name, "++"))
        sp->sb->compilerDeclared = true;
    sp->name = name;
    sp->sb->storage_class = storage_class;
    sp->tp = tp;
    if (lex)
    {
        sp->sb->declfile = sp->sb->origdeclfile = lex->data->errfile;
        sp->sb->declline = sp->sb->origdeclline = lex->data->errline;
        sp->sb->realdeclline = lex->data->linedata->lineno;
        sp->sb->declfilenum = lex->data->linedata->fileindex;
    }
    if (spi)
    {
        error(ERR_TOO_MANY_IDENTIFIERS_IN_DECLARATION);
        return spi;
    }
    return sp;
}
SYMBOL* makeUniqueID(StorageClass storage_class, Type* tp, SYMBOL* spi, const char* name)
{
    char buf[512];
    sprintf(buf, "%s_%08x", name, identityValue);
    return makeID(storage_class, tp, spi, litlate(buf));
}
void InsertSymbol(SYMBOL* sp, StorageClass storage_class, Linkage linkage, bool allowDups)
{
    SymbolTable<SYMBOL>* table;
    SYMBOL* ssp = enclosingDeclarations.GetFirst();

    if (ssp && sp->sb->parentClass == ssp)
    {
        table = sp->sb->parentClass->tp->syms;
    }
    else if (storage_class == StorageClass::auto_ || storage_class == StorageClass::register_ ||
             storage_class == StorageClass::catchvar_ || storage_class == StorageClass::parameter_ ||
             storage_class == StorageClass::localstatic_)
    {
        table = localNameSpace->front()->syms;
    }
    else if (Optimizer::cparams.prm_cplusplus && sp->tp->IsFunction() && theCurrentFunc && !enclosingDeclarations.GetFirst())
    {
        table = localNameSpace->front()->syms;
    }
    else
    {
        table = globalNameSpace->front()->syms;
    }
    if (table)
    {
        if (sp->tp->IsFunction() && !istype(sp))
        {
            const char* name = sp->sb->castoperator ? overloadNameTab[CI_CAST] : sp->name;
            SYMBOL* funcs = table->Lookup(name);
            if (!funcs)
            {
                auto tp = Type::MakeType(BasicType::aggregate_);
                funcs = makeID(StorageClass::overloads_, tp, 0, name);
                funcs->sb->castoperator = sp->sb->castoperator;
                funcs->sb->parentClass = sp->sb->parentClass;
                funcs->sb->parentNameSpace = sp->sb->parentNameSpace;
                tp->sp = funcs;
                SetLinkerNames(funcs, linkage);
                table->Add(funcs);
                table = funcs->tp->syms = symbols->CreateSymbolTable();
                table->Add(sp);
                sp->sb->overloadName = funcs;
            }
            else if (Optimizer::cparams.prm_cplusplus && funcs->sb->storage_class == StorageClass::overloads_)
            {
                table = funcs->tp->syms;
                if (table->AddOverloadName(sp))
                {
                    // we are going to naively add duplicate overloads on the basis they may
                    // differ in return type, which may make a difference for example for enable_if as a return type
                    // but we won't fully implement this at this time, e.g. if it has enable_if it had better be
                    // defined inline.   If it is defined out of line the lack of matching when we do lookups
                    // will cause multiply defined references in GetOverloadedFunction
                    int n = 0;
                    bool found = false;
                    for (auto sp1 : *table)
                    {
                        if (!strcmp(sp->sb->decoratedName, (sp1)->sb->decoratedName))
                        {
                            n++;
                            if (sp->tp->BaseType()->btp->CompatibleType((sp1)->tp->BaseType()->btp) ||
                                SameTemplate(sp->tp->BaseType()->btp, (sp1)->tp->BaseType()->btp))
                            {
                                found = true;
                                break;
                            }
                        }
                    }
                    if (!found)
                    {
                        sp->sb->overlayIndex = n;
                        table->AddName(sp);
                    }
                    else if (strchr(sp->sb->decoratedName, MANGLE_DEFERRED_TYPE_CHAR))
                    {
                        // it has a deferred argument, try again...
                        //                        InitializeFunctionArguments(sp);
                        if (table->AddOverloadName(sp))
                        {
                            found = false;
                            n = 0;
                            for (auto sp1 : *table)
                            {
                                if (!strcmp(sp->sb->decoratedName, (sp1)->sb->decoratedName))
                                {
                                    n++;
                                    //                                    sp->tp->BaseType()->btp->InstantiateDeferred();
                                    //                                    sp1->tp->BaseType()->btp->InstantiateDeferred();
                                    if (sp->tp->BaseType()->btp->CompatibleType((sp1)->tp->BaseType()->btp) &&
                                        SameTemplate(sp->tp->BaseType()->btp, (sp1)->tp->BaseType()->btp))
                                    {
                                        found = true;
                                        break;
                                    }
                                }
                            }
                            if (!found)
                            {
                                sp->sb->overlayIndex = n;
                                table->AddName(sp);
                            }
                        }
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
        else if (!allowDups || sp != search(table, sp->name))
            table->Add(sp);
    }
    else
    {
        diag("InsertSymbol: cannot insert");
    }
}
SYMBOL* calculateStructAbstractness(SYMBOL* top, SYMBOL* sp)
{
    if (sp->sb->baseClasses)
    {
        for (auto lst : *sp->sb->baseClasses)
        {
            if (lst->cls->sb->isabstract)
            {
                SYMBOL* rv = calculateStructAbstractness(top, lst->cls);
                if (rv)
                    return rv;
            }
        }
    }
    for (auto p : *sp->tp->syms)
    {
        if (p->sb->storage_class == StorageClass::overloads_)
        {
            for (auto pi : *p->tp->syms)
            {
                if (pi->sb->ispure)
                {
                    // ok found a pure function, look it up within top and
                    // check to see if it has been overrridden
                    SYMBOL* pq;
                    enclosingDeclarations.Add(top);
                    pq = classsearch(pi->name, false, false, true);
                    enclosingDeclarations.Drop();
                    if (pq)
                    {
                        for (auto pq1 : *pq->tp->syms)
                        {
                            if (strcmp(pq1->name, pi->name) == 0)
                            {
                                InitializeFunctionArguments(pq1);
                                if (matchOverload(pq1->tp, pi->tp))
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
                            }
                        }
                        // if it either isn't found or was found and is pure...
                        top->sb->isabstract = true;
                        return pi;
                    }
                }
            }
        }
    }
    return nullptr;
}
void checkIncompleteArray(Type* tp, const char* errorfile, int errorline)
{
    tp = tp->BaseType();
    if (tp->syms == nullptr)
        return; /* should get a size error */
    auto sp = tp->syms->back();
    if (sp->tp->IsPtr() && sp->tp->BaseType()->size == 0)
        specerror(ERR_STRUCT_MAY_NOT_CONTAIN_INCOMPLETE_STRUCT, "", errorfile, errorline);
}
static void calculateStructOffsets(SYMBOL* sp, bool toerr = true)
{
    BasicType type = sp->tp->BaseType()->type;
    BasicType bittype = BasicType::none_;
    int startbit = 0;
    int maxbits = 0;
    int nextoffset = 0;
    int maxsize = 0;
    int size = 0;
    int totalAlign = -1;
    if (sp->sb->baseClasses && sp->sb->baseClasses->size())
    {
        auto bases = sp->sb->baseClasses->front();
        if (sp->sb->hasvtab && !bases->cls->sb->hasvtab)
            bases->offset = getSize(BasicType::pointer_);
        else
            bases->offset = 0;
    }
    if (sp->sb->hasvtab && (!sp->sb->baseClasses || !sp->sb->baseClasses->size() ||
                            !sp->sb->baseClasses->front()->cls->sb->hasvtab || sp->sb->baseClasses->front()->isvirtual))
    {
        size += getSize(BasicType::pointer_);
        totalAlign = getAlign(StorageClass::member_, &stdpointer);
    }
    if (sp->sb->baseClasses)
    {
        for (auto lst : *sp->sb->baseClasses)
        {
            SYMBOL* sym = lst->cls;
            int align = sym->sb->attribs.inheritable.structAlign;
            totalAlign = imax(totalAlign, align);
            if (align > 1)
            {
                int al2 = align - size % align;
                if (al2 != align)
                    size += al2;
            }
            if (!lst->isvirtual)
            {
                if (lst != sp->sb->baseClasses->front())
                    lst->offset = size;
                size += sym->sb->sizeNoVirtual;
            }
        }
    }
    for (auto it = sp->tp->syms->begin(); it != sp->tp->syms->end(); ++it)
    {
        SYMBOL* p = *it;
        Type* tp = p->tp->BaseType();
        if (p->sb->storage_class != StorageClass::static_ && p->sb->storage_class != StorageClass::constant_ &&
            p->sb->storage_class != StorageClass::external_ && p->sb->storage_class != StorageClass::overloads_ &&
            p->sb->storage_class != StorageClass::enumconstant_ && !istype(p) && p != sp && p->sb->parentClass == sp)
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
                if (tp->IsStructured())
                    align = tp->sp->sb->attribs.inheritable.structAlign;
                else if (!p->sb->attribs.inheritable.alignedAttribute && p->sb->attribs.inheritable.structAlign)
                    align = p->sb->attribs.inheritable.structAlign;
                else
                    align = getAlign(StorageClass::member_, tp);
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
                if (Optimizer::cparams.c_dialect >= Dialect::c99 && tp->type == BasicType::pointer_ && p->tp->array)
                {
                    auto it1 = it;
                    if (++it1 == sp->tp->syms->end() || p->sb->init)
                    {
                        offset = nextoffset;
                        nextoffset = tp->btp->size;
                        goto join;
                    }
                    if (!p->tp->vla)
                        error(ERR_EMPTY_ARRAY_LAST);
                }
                else if (!Optimizer::cparams.prm_cplusplus && p->sb->storage_class == StorageClass::overloads_)
                    error(ERR_STRUCT_UNION_NO_FUNCTION);
                else /* c90 doesn't allow unsized arrays here */
                    if (Optimizer::cparams.prm_ansi)
                        errorsym(ERR_UNSIZED, p);
            }
            if (!tp->IsUnion() && tp->IsStructured())
            {
                checkIncompleteArray(tp, p->sb->declfile, p->sb->declline);
            }
            if (toerr && tp->IsStructured() && !tp->size && !definingTemplate)
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
                    offset = 0;
                }
                else
                {
                    offset = nextoffset;
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
                bittype = BasicType::none_;
                startbit = 0;
            }
            if (nextoffset == 0)
                nextoffset++;
        join:
            if (type == BasicType::struct_ || type == BasicType::class_)
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
            if (type == BasicType::union_ && offset > maxsize)
            {
                maxsize = offset;
                size = 0;
            }
        }
    }
    size += nextoffset;
    if (type == BasicType::union_ && maxsize > size)
    {
        size = maxsize;
    }

    if (size == 0)
    {
        if (Optimizer::cparams.prm_cplusplus && (!sp->sb->templateLevel || !definingTemplate || instantiatingTemplate))
        {
            // make it non-zero size to avoid further errors...
            size = getSize(BasicType::int_);
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
void EnterStructureStaticAssert(SYMBOL* sym, LexList* deferredCompile) { structureStaticAsserts[sym].push_back(deferredCompile); }
void ParseStructureStaticAssert(SYMBOL* sym)
{
    auto it = structureStaticAsserts.find(sym);
    if (it != structureStaticAsserts.end())
    {
        int n = PushTemplateNamespace(sym);
        enclosingDeclarations.Add(sym);
        auto hold = it->second;
        for (auto deferred : hold)
        {
            auto temp = SetAlternateLex(deferred);
            temp = handleStaticAssert(temp);
            SetAlternateLex(nullptr);
        }
        enclosingDeclarations.Drop();
        PopTemplateNamespace(n);
    }
}
static bool validateAnonymousUnion(SYMBOL* parent, Type* unionType)
{
    bool rv = true;
    unionType = unionType->BaseType();
    {
        for (auto member : *unionType->syms)
        {
            if (Optimizer::cparams.prm_cplusplus && member->sb->storage_class == StorageClass::overloads_)
            {
                for (auto sp1 : *member->tp->BaseType()->syms)
                {
                    if (!sp1->sb->defaulted)
                    {
                        error(ERR_ANONYMOUS_UNION_NO_FUNCTION_OR_TYPE);
                        rv = false;
                        break;
                    }
                }
            }
            else if ((Optimizer::cparams.prm_cplusplus && member->sb->storage_class == StorageClass::type_) ||
                     member->sb->storage_class == StorageClass::typedef_)
            {
                error(ERR_ANONYMOUS_UNION_NO_FUNCTION_OR_TYPE);
                rv = false;
            }
            else if ((Optimizer::cparams.prm_cplusplus && member->sb->access == AccessLevel::private_) ||
                     member->sb->access == AccessLevel::protected_)
            {
                error(ERR_ANONYMOUS_UNION_PUBLIC_MEMBERS);
                rv = false;
            }
            else if (Optimizer::cparams.prm_cplusplus && member->sb->storage_class != StorageClass::member_ &&
                     member->sb->storage_class != StorageClass::mutable_)
            {
                error(ERR_ANONYMOUS_UNION_NONSTATIC_MEMBERS);
                rv = false;
            }
            else
            {
                SYMBOL* spi = nullptr;
                if (parent && (spi = parent->tp->syms->Lookup(member->name)) != nullptr)
                {
                    currentErrorLine = 0;
                    preverrorsym(ERR_DUPLICATE_IDENTIFIER, spi, spi->sb->declfile, spi->sb->declline);
                    rv = false;
                }
            }
        }
    }
    return rv;
}
static void resolveAnonymousGlobalUnion(SYMBOL* sp)
{
    validateAnonymousUnion(nullptr, sp->tp);
    InsertSymbol(sp, sp->sb->storage_class, Linkage::c_, true);
    if (sp->sb->storage_class != StorageClass::auto_)
    {
        insertInitSym(sp);
    }
    for (auto sym : *sp->tp->syms)
    {
        if (sym->sb->storage_class == StorageClass::member_ || sym->sb->storage_class == StorageClass::mutable_)
        {
            sym->sb->storage_class = sp->sb->storage_class;
            sym->sb->label = sp->sb->label;
            sym->sb->anonymousGlobalUnion = true;
            InsertSymbol(sym, sp->sb->storage_class, Linkage::c_, false);
        }
    }
}
void resolveAnonymousUnions(SYMBOL* sp)
{
    for (auto itmember = sp->tp->syms->begin(); itmember != sp->tp->syms->end(); ++itmember)
    {
        SYMBOL* spm = *itmember;
        // anonymous structured type declaring anonymous variable is a candidate for
        // an anonymous structure or union
        if (spm->tp->IsStructured() && spm->sb->anonymous && spm->tp->BaseType()->sp->sb->anonymous)
        {
            resolveAnonymousUnions(spm);
            validateAnonymousUnion(sp, spm->tp);

            auto it = itmember;
            ++it;
            sp->tp->syms->remove(itmember);
            itmember = it;
            if (spm->tp->BaseType()->type == BasicType::union_)
            {
                if (Optimizer::cparams.c_dialect < Dialect::c99 && !Optimizer::cparams.prm_cplusplus)
                {
                    error(ERR_ANONYMOUS_UNION_WARNING);
                }
            }
            else if (Optimizer::cparams.c_dialect < Dialect::c99 && !Optimizer::cparams.prm_cplusplus)
            {
                error(ERR_ANONYMOUS_STRUCT_WARNING);
            }
            bool found = false;
            for (auto newsp : *spm->tp->syms)
            {
                if ((newsp->sb->storage_class == StorageClass::member_ || newsp->sb->storage_class == StorageClass::mutable_) &&
                    !newsp->tp->IsFunction())
                {
                    newsp->sb->offset += spm->sb->offset;
                    newsp->sb->parentClass = sp;
                    itmember = sp->tp->syms->insert(itmember, newsp);
                    ++itmember;
                    found = true;
                }
            }
            if (found)
                --itmember;
        }
    }
}
static bool usesTemplate(SYMBOL* internal, std::list<TEMPLATEPARAMPAIR>* params)
{
    std::list<SYMBOL*> working;
    std::set<SYMBOL*> visited;
    do
    {
        for (auto p : *params)
        {
            if (p.second->type == TplType::typename_)
            {
                if (p.second->byClass.val && p.second->byClass.val->IsStructured())
                {
                    if (visited.find(p.second->byClass.val->BaseType()->sp) == visited.end())
                    {
                        working.push_back(p.second->byClass.val->BaseType()->sp);
                        visited.insert(working.back());
                    }
                }
            }
            else if (p.second->type == TplType::int_)
            {
                if (p.second->byNonType.tp && p.second->byNonType.tp->IsStructured())
                {
                    if (visited.find(p.second->byNonType.tp->BaseType()->sp) == visited.end())
                    {
                        working.push_back(p.second->byNonType.tp->BaseType()->sp);
                        visited.insert(working.back());
                    }
                }
            }
        }
        params = nullptr;
        if (working.size())
        {
            SYMBOL* sp;
            do
            {
                sp = working.front();
                working.pop_front();
                if (internal->tp->CompatibleType(sp->tp) || SameTemplate(internal->tp, sp->tp))
                    return true;
            } while (working.size() && !sp->templateParams);
            if (sp->templateParams)
                params = sp->templateParams;
        }
    } while (params);
    return false;
}
static bool usesClass(SYMBOL* cls, SYMBOL* internal)
{
    if (cls->tp->syms)
    {
        for (auto sym : *cls->tp->syms)
        {
            Type* tp = sym->tp;
            if (istype(sym))
            {
                if (sym->sb->storage_class != StorageClass::typedef_)
                {
                    tp = nullptr;
                }
            }
            if (tp)
            {
                while (tp->IsArray())
                    tp = tp->BaseType()->btp;
                if (internal->tp->CompatibleType(tp) || SameTemplate(internal->tp, tp))
                    return true;
                if (tp->IsStructured())
                {
                    auto sp = tp->BaseType()->sp;
                    if (sp->templateParams)
                    {
                        if (usesTemplate(internal, sp->templateParams))
                            return true;
                    }
                }
            }
        }
    }
    return false;
}
static void GetStructAliasType(SYMBOL* sym)
{
    if (definingTemplate && !instantiatingTemplate)
        return;
    if (Optimizer::architecture == ARCHITECTURE_MSIL)
        return;
    /* this conveniently takes care of situations where a VTAB would be required */
    if (sym->tp->size > Optimizer::chosenAssembler->arch->word_size)
        return;
    if ((!Optimizer::cparams.prm_optimize_for_speed && !Optimizer::cparams.prm_optimize_for_size) || Optimizer::cparams.prm_debug)
        return;
    if ((sym->sb->baseClasses && sym->sb->baseClasses->size()) || (sym->sb->vbaseEntries && sym->sb->vbaseEntries->size()))
        return;
    if (sym->sb->hasvtab)
        return;
    SYMBOL* cache = nullptr;
    for (auto m : *sym->tp->syms)
    {
        if (ismemberdata(m))
        {
            if (cache)
                return;
            else
                cache = m;
        }
        if (m->sb->storage_class == StorageClass::overloads_)
        {
            if (m->tp->syms->front()->sb->isDestructor && !m->tp->syms->front()->sb->defaulted)
                return;
        }
    }
    if (!cache || cache->tp->IsStructured() || cache->tp->IsAtomic() || cache->tp->IsArray() || cache->tp->BaseType()->bits ||
        cache->tp->IsFunctionPtr() || cache->tp->IsRef() || cache->tp->IsFloat() || cache->tp->IsImaginary())
        return;
    sym->sb->structuredAliasType = cache->tp;
}
static void baseFinishDeclareStruct(SYMBOL* funcsp)
{
    (void)funcsp;
    int n = 0, i, j;
    Optimizer::LIST* lst = openStructs;
    std::vector<SYMBOL*> syms;
    lst = openStructs;
    openStructs = nullptr;
    while (lst)
    {
        syms.push_back((SYMBOL*)lst->data);
        lst = lst->next;
    }
    n = syms.size();
    for (i = 0; i < n; i++)
        for (j = i + 1; j < n; j++)
            if ((syms[i] != syms[j] && !SameTemplate(syms[i]->tp, syms[j]->tp) && classRefCount(syms[j], syms[i])) ||
                usesClass(syms[i], syms[j]))
            {
                SYMBOL* x = syms[j];
                memmove(&syms[i + 1], &syms[i], sizeof(SYMBOL*) * (j - i));
                syms[i] = x;
            }
    ++resolvingStructDeclarations;
    for (i = 0; i < n; i++)
    {
        SYMBOL* sp = syms[i];
        bool recursive = sp->sb->declaringRecursive;
        sp->sb->declaringRecursive = false;
        if (!sp->sb->performedStructInitialization)
        {
            if (!definingTemplate)
            {
                if (sp->sb->baseClasses)
                {
                    auto bc = Optimizer::SymbolManager::Get(sp)->baseClasses;
                    for (auto b : *sp->sb->baseClasses)
                    {
                        b->cls->tp = b->cls->tp->InitializeDeferred();
                        if (b->cls->tp->type == BasicType::templateselector_)
                        {
                            auto a = ResolveTemplateSelectors(b->cls, b->cls->tp);
                            if (a->IsStructured())
                            {
                                b->cls = a->sp->tp->BaseType()->sp;
                                bc->sym = Optimizer::SymbolManager::Get(b->cls);
                            }
                        }
                        if (bc)
                            bc = bc->next;
                    }
                }
                for (auto s : *sp->tp->syms)
                {
                    if (s->tp->type != BasicType::aggregate_ && !istype(s))
                    {
                        s->tp = ResolveTemplateSelectors(s, s->tp);
                        s->tp->InstantiateDeferred();
                        s->tp = s->tp->InitializeDeferred();
                    }
                }
            }
            if (recursive)
            {
                calculateStructOffsets(sp);
            }
            GetStructAliasType(sp);
            ConditionallyDeleteClassMethods(sp);
        }
    }
    --resolvingStructDeclarations;
    if (!definingTemplate || instantiatingTemplate)
    {
        for (i = 0; i < n; i++)
        {
            SYMBOL* sp = syms[i];
            for (auto sym : *sp->tp->syms)
            {
                if (sym->sb->storage_class == StorageClass::global_ && sym->tp->IsInt() && sym->sb->init && sym->tp->IsConst() &&
                    sym->sb->init->front()->exp->type == ExpressionNode::templateselector_)
                {
                    optimize_for_constants(&sym->sb->init->front()->exp);
                }
            }
        }
        for (i = 0; i < n; i++)
        {
            SYMBOL* sp = syms[i];
            if (!sp->sb->performedStructInitialization)
            {
                if (/*n > 1 &&*/ !definingTemplate)
                {
                    calculateStructOffsets(sp);

                    if (Optimizer::cparams.prm_cplusplus)
                    {
                        calculateVirtualBaseOffsets(sp);  // undefined in local context
                        calculateVTabEntries(sp, sp, &sp->sb->vtabEntries, 0);
                    }
                }
                resolveAnonymousUnions(sp);
                if (Optimizer::cparams.prm_cplusplus)
                    deferredInitializeStructMembers(sp);
            }
        }
        for (i = 0; i < n; i++)
        {
            if (!syms[i]->sb->performedStructInitialization)
            {
                syms[i]->sb->performedStructInitialization = true;
                if (Optimizer::cparams.prm_cplusplus)
                {
                    if (syms[i]->templateParams && !allTemplateArgsSpecified(syms[i], syms[i]->templateParams))
                    {
                        int oldInstantiatingTemplate = instantiatingTemplate;
                        instantiatingTemplate = 0;
                        definingTemplate++;
                        deferredInitializeStructFunctions(syms[i]);
                        definingTemplate--;
                        instantiatingTemplate = oldInstantiatingTemplate;
                    }
                    else
                    {
                        deferredInitializeStructFunctions(syms[i]);
                    }
                    ParseStructureStaticAssert(syms[i]);
                }
            }
        }
    }
    structureStaticAsserts.clear();
}
static LexList* structbody(LexList* lex, SYMBOL* funcsp, SYMBOL* sp, AccessLevel currentAccess, SymbolTable<SYMBOL>* anonymousTable)
{
    (void)funcsp;
    if (Optimizer::cparams.prm_cplusplus)
    {
        Optimizer::LIST* lst = Allocate<Optimizer::LIST>();
        lst->next = openStructs;
        openStructs = lst;
        lst->data = sp;
    }
    lex = getsym();
    enclosingDeclarations.Add(sp);
    sp->sb->declaring = true;
    while (lex && KW(lex) != Keyword::end_)
    {
        FlushLineData(lex->data->errfile, lex->data->linedata->lineno);
        switch (KW(lex))
        {
            case Keyword::private_:
                sp->sb->accessspecified = true;
                currentAccess = AccessLevel::private_;
                lex = getsym();
                needkw(&lex, Keyword::colon_);
                break;
            case Keyword::public_:
                sp->sb->accessspecified = true;
                currentAccess = AccessLevel::public_;
                lex = getsym();
                needkw(&lex, Keyword::colon_);
                break;
            case Keyword::protected_:
                sp->sb->accessspecified = true;
                currentAccess = AccessLevel::protected_;
                lex = getsym();
                needkw(&lex, Keyword::colon_);
                break;
            default:
                lex = declare(lex, nullptr, nullptr, StorageClass::member_, Linkage::none_, emptyBlockdata, true, false, false,
                              currentAccess);
                break;
        }
    }
    sp->sb->declaring = false;
    enclosingDeclarations.Drop();
    sp->sb->hasvtab = usesVTab(sp);
    calculateStructOffsets(sp, false);

    if (anonymousTable)
    {
        sp->name = AnonymousTypeName(sp, Optimizer::cparams.prm_cplusplus && !sp->sb->parentClass ? globalNameSpace->front()->tags
                                                                                                  : anonymousTable);
        SetLinkerNames(sp, Linkage::cdecl_);
        browse_variable(sp);
        (Optimizer::cparams.prm_cplusplus && !sp->sb->parentClass ? globalNameSpace->front()->tags : anonymousTable)->Add(sp);
    }

    if (Optimizer::cparams.prm_cplusplus)
    {
        createDefaultConstructors(sp);
        calculateStructAbstractness(sp, sp);
        calculateVirtualBaseOffsets(sp);  // undefined in local context
        calculateVTabEntries(sp, sp, &sp->sb->vtabEntries, 0);
        if (sp->sb->vtabEntries && sp->sb->vtabEntries->size())
        {
            char* buf = (char*)alloca(4096);
            Optimizer::my_sprintf(buf, "%s@_.vt", sp->sb->decoratedName);
            sp->sb->vtabsp = makeID(StorageClass::static_, &stdvoid, nullptr, litlate(buf));
            sp->sb->vtabsp->sb->attribs.inheritable.linkage2 = sp->sb->attribs.inheritable.linkage2;
            sp->sb->vtabsp->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
            sp->sb->vtabsp->sb->decoratedName = sp->sb->vtabsp->name;
            Optimizer::SymbolManager::Get(sp->sb->vtabsp)->inlineSym = sp;
            if (sp->sb->vtabsp->sb->attribs.inheritable.linkage2 == Linkage::import_)
            {
                sp->sb->vtabsp->sb->dontinstantiate = true;
            }
            warnCPPWarnings(sp, funcsp != nullptr);
        }
    }
    else
    {
        resolveAnonymousUnions(sp);
        sp->sb->trivialCons = true;
        GetStructAliasType(sp);
    }
    if (!Optimizer::cparams.prm_cplusplus || structLevel == 1)
    {
        structLevel--;
        baseFinishDeclareStruct(funcsp);
        structLevel++;
    }
    if (Optimizer::cparams.prm_cplusplus && sp->tp->syms && !definingTemplate)
    {
        SYMBOL* cons = search(sp->tp->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
        if (!cons)
        {
            for (auto sp1 : *sp->tp->BaseType()->syms)
            {
                if (sp1->sb->storage_class == StorageClass::member_ || sp1->sb->storage_class == StorageClass::mutable_)
                {
                    if (sp1->tp->IsRef())
                    {
                        errorsym(ERR_REF_CLASS_NO_CONSTRUCTORS, sp1);
                    }
                    else if (sp1->tp->IsConst())
                    {
                        errorsym(ERR_CONST_CLASS_NO_CONSTRUCTORS, sp1);
                    }
                }
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
// this ignores typedef declarations and goes for the 'bare' version of the types...
static void TypeCRC(Type* tp, unsigned& crc)
{
    const int constval = 0xffeeccdd;
    const int volval = 0xaabbccdd;
    const int arrval = 0x11223344;
    while (tp)
    {
        if (tp->IsConst())
            crc = Utils::PartialCRC32(crc, (const unsigned char*)&constval, sizeof(constval));
        if (tp->IsVolatile())
            crc = Utils::PartialCRC32(crc, (const unsigned char*)&volval, sizeof(volval));
        crc = Utils::PartialCRC32(crc, (const unsigned char*)&tp->BaseType()->type, sizeof(tp->type));
        if (tp->BaseType()->IsStructured() || tp->BaseType()->type == BasicType::enum_)
            crc = Utils::PartialCRC32(crc, (const unsigned char*)tp->BaseType()->sp->name, strlen(tp->BaseType()->sp->name));
        crc = Utils::PartialCRC32(crc, (const unsigned char*)&tp->BaseType()->size, sizeof(tp->size));
        if (tp->array)
            crc = Utils::PartialCRC32(crc, (const unsigned char*)&arrval, sizeof(arrval));
        crc = Utils::PartialCRC32(crc, (const unsigned char*)&tp->BaseType()->bits, sizeof(tp->bits));
        tp = tp->BaseType()->btp;
    }
}
static unsigned TypeCRC(SYMBOL* sp)
{
    unsigned crc = 0xffffffff;
    crc = Utils::PartialCRC32(crc, (const unsigned char*)&sp->tp->BaseType()->alignment, sizeof(sp->tp->BaseType()->alignment));
    crc = Utils::PartialCRC32(crc, (const unsigned char*)&sp->tp->BaseType()->size, sizeof(sp->tp->BaseType()->size));
    for (auto s : *sp->tp->syms)
    {
        crc = Utils::PartialCRC32(crc, (const unsigned char*)s->name, strlen(s->name));
        crc = Utils::PartialCRC32(crc, (const unsigned char*)&s->sb->offset, sizeof(s->sb->offset));
        TypeCRC(s->tp, crc);
    }
    return crc;
}
static bool compareStructSyms(SymbolTable<SYMBOL>* left, SymbolTable<SYMBOL>* right)
{
    auto itl = left->begin();
    auto itr = right->begin();
    for (; itl != left->end() && itr != right->end(); ++itl, ++itr)
    {
        if ((*itl)->sb->offset != (*itr)->sb->offset)
            return false;
        if ((*itl)->tp->alignment != (*itr)->tp->alignment)
            return false;
        if (!(*itl)->tp->SameType((*itr)->tp))
            return false;
    }
    return itl == left->end() && itr == right->end();
}
LexList* innerDeclStruct(LexList* lex, SYMBOL* funcsp, SYMBOL* sp, bool inTemplate, AccessLevel defaultAccess, bool isfinal,
                         bool* defd, SymbolTable<SYMBOL>* anonymousTable)
{
    int oldParsingTemplateArgs;
    int oldExpressionParsing;
    oldParsingTemplateArgs = parsingDefaultTemplateArgs;
    oldExpressionParsing = inFunctionExpressionParsing;
    parsingDefaultTemplateArgs = 0;
    inFunctionExpressionParsing = false;
    bool hasBody = (Optimizer::cparams.prm_cplusplus && KW(lex) == Keyword::colon_) || KW(lex) == Keyword::begin_;
    SYMBOL* injected = nullptr;

    if (nameSpaceList.size())
        SetTemplateNamespace(sp);
    if (sp->sb->attribs.inheritable.structAlign == 0)
        sp->sb->attribs.inheritable.structAlign = 1;
    structLevel++;
    auto oldsyms = sp->tp->syms, oldtags = sp->tp->tags;
    if (hasBody)
    {
        if (sp->tp->syms || sp->tp->tags)
        {
            if (Optimizer::cparams.c_dialect != Dialect::c2x)
            {
                preverrorsym(ERR_STRUCT_HAS_BODY, sp, sp->sb->declfile, sp->sb->declline);
            }
        }
        sp->tp->syms = symbols->CreateSymbolTable();
        if (Optimizer::cparams.prm_cplusplus)
        {
            sp->tp->tags = symbols->CreateSymbolTable();
            injected = CopySymbol(sp);
            injected->sb->mainsym = sp;   // for constructor/destructor matching
            sp->tp->tags->Add(injected);  // inject self
            injected->sb->access = AccessLevel::public_;
        }
    }
    if (inTemplate && templateDeclarationLevel == 1)
        inTemplateBody++;
    if (Optimizer::cparams.prm_cplusplus)
        if (KW(lex) == Keyword::colon_)
        {
            lex = baseClasses(lex, funcsp, sp, defaultAccess);
            if (injected)
                injected->sb->baseClasses = sp->sb->baseClasses;
            if (!MATCHKW(lex, Keyword::begin_))
                errorint(ERR_NEEDY, '{');
        }
    if (KW(lex) == Keyword::begin_)
    {
        sp->sb->isfinal = isfinal;
        lex = structbody(lex, funcsp, sp, defaultAccess, anonymousTable);
        *defd = true;
    }
    if (hasBody && Optimizer::cparams.c_dialect == Dialect::c2x)
    {
        if (oldsyms)
        {
            if (!compareStructSyms(oldsyms, sp->tp->syms))
            {
                preverrorsym(ERR_STRUCT_HAS_BODY, sp, sp->sb->declfile, sp->sb->declline);
            }
        }
    }
    if (inTemplate && templateDeclarationLevel == 1)
    {
        inTemplateBody--;
        TemplateGetDeferred(sp);
    }
    --structLevel;
    inFunctionExpressionParsing = oldExpressionParsing;
    parsingDefaultTemplateArgs = oldParsingTemplateArgs;
    return lex;
}
static unsigned char* ParseUUID(LexList** lex)
{
    if (MATCHKW(*lex, Keyword::uuid_))
    {
        unsigned vals[16];
        unsigned char* rv = Allocate<unsigned char>(16);
        *lex = getsym();
        needkw(lex, Keyword::openpa_);
        if ((*lex)->data->type == LexType::l_astr_)
        {
            int i;
            int count = ((Optimizer::SLCHAR*)(*lex)->data->value.s.w)->count;
            LCHAR* str = ((Optimizer::SLCHAR*)(*lex)->data->value.s.w)->str;
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
        needkw(lex, Keyword::closepa_);
        return rv;
    }
    return nullptr;
}
LexList* declstruct(LexList* lex, SYMBOL* funcsp, Type** tp, bool inTemplate, bool asfriend, StorageClass storage_class,
                    Linkage linkage2_in, AccessLevel access, bool* defd, bool constexpression)
{
    bool isfinal = false;
    SymbolTable<SYMBOL>* table = nullptr;
    const char* tagname;
    char newName[4096];
    BasicType type = BasicType::none_;
    SYMBOL* sp = nullptr;
    int charindex;
    std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
    SYMBOL* strSym;
    AccessLevel defaultAccess;
    bool addedNew = false;
    int declline = lex->data->errline;
    int realdeclline = lex->data->linedata->lineno;
    bool anonymous = false;
    unsigned char* uuid;
    Linkage linkage1 = Linkage::none_, linkage2 = linkage2_in, linkage3 = Linkage::none_;
    bool searched = false;
    *defd = false;
    switch (KW(lex))
    {
        case Keyword::class_:
            defaultAccess = AccessLevel::private_;
            type = BasicType::class_;
            break;
        default:
        case Keyword::struct_:
            defaultAccess = AccessLevel::public_;
            type = BasicType::struct_;
            break;
        case Keyword::union_:
            defaultAccess = AccessLevel::public_;
            type = BasicType::union_;
            break;
    }
    lex = getsym();
    uuid = ParseUUID(&lex);
    auto oldAttribs = basisAttribs;
    basisAttribs = {0};
    ParseAttributeSpecifiers(&lex, funcsp, true);
    if (MATCHKW(lex, Keyword::declspec_))
    {
        lex = getsym();
        lex = parse_declspec(lex, &linkage1, &linkage2, &linkage3);
    }
    if (MATCHKW(lex, Keyword::rtllinkage_))
    {
        lex = getsym();
        linkage2 = getDefaultLinkage();
    }
    if (MATCHKW(lex, Keyword::classsel_))
    {
        charindex = -1;
        tagname = "";
        lex = nestedSearch(lex, &sp, &strSym, &nsv, nullptr, nullptr, true, storage_class, true, true);
        lex = getsym();
        searched = true;
    }
    else if (ISID(lex))
    {
        charindex = lex->data->charindex;
        tagname = litlate(lex->data->value.s.a);
    }
    else
    {
        if (!MATCHKW(lex, Keyword::begin_) && !MATCHKW(lex, Keyword::colon_))
            errorint(ERR_NEEDY, '{');
        tagname = "";
        charindex = -1;
        anonymous = true;
    }

    if (!searched)
    {
        Utils::StrCpy(newName, tagname);
        if (inTemplate)
            inTemplateSpecialization++;

        lex = tagsearch(lex, newName, sizeof(newName), & sp, &table, &strSym, &nsv, storage_class);

        if (inTemplate)
            inTemplateSpecialization--;
        if (!asfriend && charindex != -1 && Optimizer::cparams.prm_cplusplus && openStructs && MATCHKW(lex, Keyword::semicolon_))
        {
            // forward declaration within class...   erase anything found outside the class
            if (!sp || sp->sb->parentClass != (SYMBOL*)openStructs->data)
            {
                sp = nullptr;
            }
        }
        if (charindex != -1 && Optimizer::cparams.prm_cplusplus && ISID(lex) && !strcmp(lex->data->value.s.a, "final"))
        {
            isfinal = true;
            lex = getsym();
            if (!MATCHKW(lex, Keyword::begin_) && !MATCHKW(lex, Keyword::colon_))
                errorint(ERR_NEEDY, '{');
        }
    }
    if (ISID(lex))
    {
        lex = getsym();
        if (MATCHKW(lex, Keyword::lt_) || MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::colon_))
        {
            lex = backupsym();
            // multiple identifiers, wade through them for error handling
            error(ERR_TOO_MANY_IDENTIFIERS_IN_DECLARATION);
            while (ISID(lex))
            {
                charindex = lex->data->charindex;
                tagname = litlate(lex->data->value.s.a);
                Utils::StrCpy(newName, tagname);
                lex = tagsearch(lex, newName, sizeof(newName), &sp, &table, &strSym, &nsv, storage_class);
            }
        }
        else
        {
            lex = backupsym();
        }
    }
    if (funcsp && !searched && !inFunctionExpressionParsing && (MATCHKW(lex, Keyword::colon_) || MATCHKW(lex, Keyword::begin_)))
    {
        sp = nullptr;
        nsv = nullptr;
        if (localNameSpace->front()->tags)
        {
            sp = localNameSpace->front()->tags->Lookup(newName);
        }
        table = localNameSpace->front()->tags;
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
        sp->sb->storage_class = StorageClass::type_;
        sp->tp = Type::MakeType(type);
        sp->tp->sp = sp;
        sp->tp = AttributeFinish(sp, sp->tp);
        sp->sb->declcharpos = charindex;
        sp->sb->declline = sp->sb->origdeclline = declline;
        sp->sb->realdeclline = realdeclline;
        sp->sb->declfile = sp->sb->origdeclfile = lex->data->errfile;
        sp->sb->declfilenum = lex->data->linedata->fileindex;
        sp->sb->attribs = basisAttribs;
        if ((storage_class == StorageClass::member_ || storage_class == StorageClass::mutable_ ||
             storage_class == StorageClass::auto_) &&
            (MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::colon_) || MATCHKW(lex, Keyword::try_) ||
             MATCHKW(lex, Keyword::semicolon_)))
        {
            if (storage_class == StorageClass::auto_)
                sp->sb->parentClass = theCurrentFunc->sb->parentClass;
            else
                sp->sb->parentClass = enclosingDeclarations.GetFirst();
        }
        if (nsv)
            sp->sb->parentNameSpace = nsv->front()->name;
        else
            sp->sb->parentNameSpace = globalNameSpace->front()->name;
        sp->sb->parent = funcsp;
        if (nsv && nsv->front()->name && !strcmp(sp->name, "initializer_list") && !strcmp(nsv->front()->name->name, "std"))
            sp->sb->initializer_list = true;
        if (inTemplate)
            sp->sb->parentTemplate = sp;
        sp->sb->anonymous = charindex == -1;
        sp->sb->access = access;
        sp->sb->uuid = uuid;
        if (sp->sb->attribs.inheritable.linkage2 == Linkage::none_)
            sp->sb->attribs.inheritable.linkage2 = linkage2;
        if (asfriend)
            sp->sb->parentClass = nullptr;
        if (!anonymous)
        {
            SetLinkerNames(sp, Linkage::cdecl_);
            if (inTemplate && definingTemplate)
            {
                if (MATCHKW(lex, Keyword::lt_))
                    errorsym(ERR_SPECIALIZATION_REQUIRES_PRIMARY, sp);
                sp->templateParams = TemplateGetParams(sp);
                sp->sb->templateLevel = definingTemplate;
                TemplateMatching(lex, nullptr, sp->templateParams, sp,
                                 MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::colon_));
                SetLinkerNames(sp, Linkage::cdecl_);
            }
            browse_variable(sp);
            (Optimizer::cparams.prm_cplusplus && !sp->sb->parentClass ? globalNameSpace->front()->tags : table)->Add(sp);
        }
    }
    else
    {
        if (!sp->sb && sp->tp->type == BasicType::templateparam_ && sp->tp->templateParam->second->byTemplate.val)
        {
            sp = sp->tp->templateParam->second->byTemplate.val;
        }
        if (asfriend && sp->sb && sp->sb->templateLevel)
        {
            sp = sp->sb->parentTemplate;
        }
        // primarily for the type_info definition when building LSCRTL.DLL
        if (linkage1 != Linkage::none_ && linkage1 != sp->sb->attribs.inheritable.linkage)
        {
            if (sp->sb->attribs.inheritable.linkage != Linkage::none_)
                error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
            else
                sp->sb->attribs.inheritable.linkage = linkage1;
        }
        if (linkage2 != Linkage::none_ && linkage2 != sp->sb->attribs.inheritable.linkage2)
        {
            if (sp->sb->attribs.inheritable.linkage2 != Linkage::none_)
                error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
            else
                sp->sb->attribs.inheritable.linkage2 = linkage2;
        }
        if (linkage3 != Linkage::none_ && linkage3 != sp->sb->attribs.inheritable.linkage3)
        {
            if (sp->sb->attribs.inheritable.linkage3 != Linkage::none_)
                error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
            else
                sp->sb->attribs.inheritable.linkage3 = linkage3;
        }
        if (type != sp->tp->type && (type == BasicType::union_ || sp->tp->type == BasicType::union_))
        {
            errorsym(ERR_MISMATCHED_STRUCTURED_TYPE_IN_REDEFINITION, sp);
        }
        else if (inTemplate && definingTemplate)
        {
            // definition or declaration
            if (!sp->sb->templateLevel)
            {
                if (!sp->sb->parentClass || !sp->sb->parentClass->sb->templateLevel)
                    errorsym(ERR_NOT_A_TEMPLATE, sp);
                SetLinkerNames(sp, Linkage::cdecl_);
            }
            else
            {
                if (inTemplate && KW(lex) == Keyword::lt_)
                {
                    std::list<TEMPLATEPARAMPAIR>* origParams = sp->templateParams;
                    std::list<TEMPLATEPARAMPAIR>* templateParams = TemplateGetParams(sp);
                    inTemplateSpecialization++;
                    parsingSpecializationDeclaration = true;
                    lex = GetTemplateArguments(lex, funcsp, nullptr, &templateParams->front().second->bySpecialization.types);
                    parsingSpecializationDeclaration = false;
                    inTemplateSpecialization--;
                    sp = LookupSpecialization(sp, templateParams);
                    if (linkage2 != Linkage::none_)
                        sp->sb->attribs.inheritable.linkage2 = linkage2;
                    sp->templateParams = TemplateMatching(lex, origParams, templateParams, sp,
                                                          MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::colon_));
                    if (sp->templateParams->front().second->bySpecialization.types)
                        for (auto&& t : *templateParams->front().second->bySpecialization.types)
                        {
                            t.second->specializationParam = true;
                        }
                }
                else
                {
                    SYMLIST* instants;
                    std::list<TEMPLATEPARAMPAIR>* templateParams = TemplateGetParams(sp);
                    sp->templateParams = TemplateMatching(lex, sp->templateParams, templateParams, sp,
                                                          MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::colon_));
                    if (sp->sb->parentTemplate->sb->instantiations)
                    {
                        for (auto instant : *sp->sb->parentTemplate->sb->instantiations)
                        {
                            std::list<TEMPLATEPARAMPAIR>::iterator itln = instant->templateParams->begin();
                            ++itln;
                            std::list<TEMPLATEPARAMPAIR>::iterator itlne = instant->templateParams->end();
                            std::list<TEMPLATEPARAMPAIR>::iterator itl = templateParams->begin();
                            ++itl;
                            std::list<TEMPLATEPARAMPAIR>::iterator itle = templateParams->end();
                            for (; itl != itle && itln != itlne; ++itln, ++itl)
                            {
                                itln->first->name = itl->first->name;
                            }
                        }
                    }
                }
                SetLinkerNames(sp, Linkage::cdecl_);
            }
        }
        else if (MATCHKW(lex, Keyword::lt_))
        {
            if (inTemplate)
            {
                // instantiation
                std::list<TEMPLATEPARAMPAIR>* templateParams = TemplateGetParams(sp);
                lex = GetTemplateArguments(lex, funcsp, nullptr, &templateParams->front().second->bySpecialization.types);
            }
            else if (!sp->sb || sp->sb->templateLevel)
            {
                if ((MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::colon_)))
                {
                    errorsym(ERR_IS_ALREADY_DEFINED_AS_A_TEMPLATE, sp);
                }
                else
                {
                    std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                    lex = GetTemplateArguments(lex, funcsp, nullptr, &lst);
                    if (sp->sb)
                        sp = GetClassTemplate(sp, lst, false);
                }
            }
        }
        else if (sp->sb->templateLevel && (MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::colon_)))
        {
            errorsym(ERR_IS_ALREADY_DEFINED_AS_A_TEMPLATE, sp);
        }
    }
    if (sp)
    {
        if (sp->sb)
        {
            if (uuid)
                sp->sb->uuid = uuid;
            if (access != sp->sb->access && sp->tp->syms && (MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::colon_)))
            {
                errorsym(ERR_CANNOT_REDEFINE_ACCESS_FOR, sp);
            }
            lex = innerDeclStruct(lex, funcsp, sp, inTemplate, defaultAccess, isfinal, defd, anonymous ? table : nullptr);
            if (constexpression)
            {
                error(ERR_CONSTEXPR_NO_STRUCT);
            }
        }
        *tp = sp->tp;
    }
    basisAttribs = oldAttribs;
    return lex;
}
static LexList* enumbody(LexList* lex, SYMBOL* funcsp, SYMBOL* spi, StorageClass storage_class, AccessLevel access, Type* fixedType,
                         bool scoped)
{
    long long enumval = 0;
    Type* unfixedType;
    bool fixed = Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c2x ? true : false;
    unfixedType = spi->tp->btp;
    if (!unfixedType)
        unfixedType = &stdint;
    lex = getsym();
    spi->sb->declaring = true;
    if (spi->tp->syms)
    {
        preverrorsym(ERR_ENUM_CONSTANTS_DEFINED, spi, spi->sb->declfile, spi->sb->declline);
    }
    spi->tp->syms = symbols->CreateSymbolTable(); /* holds a list of all the values, e.g. for debug info */
    if (!MATCHKW(lex, Keyword::end_))
    {
        while (lex)
        {
            if (ISID(lex))
            {
                SYMBOL* sp;
                Type* tp;
                if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c2x)
                {
                    tp = (fixedType ? fixedType : unfixedType)->CopyType();
                    tp->scoped = scoped;  // scoped the constants type as well for error checking
                    tp->btp = spi->tp;    // the integer type gets a base type which is the enumeration for error checking
                    tp->rootType = tp;
                    tp->enumConst = true;
                    tp->sp = spi;
                }
                else
                {
                    tp = Type::MakeType(BasicType::int_);
                }
                sp = makeID(StorageClass::enumconstant_, tp, 0, litlate(lex->data->value.s.a));
                sp->name = sp->sb->decoratedName = litlate(lex->data->value.s.a);
                sp->sb->declcharpos = lex->data->charindex;
                sp->sb->declline = sp->sb->origdeclline = lex->data->errline;
                sp->sb->realdeclline = lex->data->linedata->lineno;
                sp->sb->declfile = sp->sb->origdeclfile = lex->data->errfile;
                sp->sb->declfilenum = lex->data->linedata->fileindex;
                sp->sb->parentClass = spi->sb->parentClass;
                sp->sb->access = access;
                browse_variable(sp);
                if (Optimizer::cparams.prm_cplusplus)
                {
                    if (!sp->tp->scoped)  // dump it into the parent table unless it is a C++ scoped enum
                        InsertSymbol(sp, storage_class, Linkage::c_, false);
                }
                else  // in C dump it into the globals
                {
                    if (funcsp)
                        localNameSpace->front()->syms->Add(sp);
                    else
                        globalNameSpace->front()->syms->Add(sp);
                }
                spi->tp->syms->Add(sp);
                lex = getsym();
                if (MATCHKW(lex, Keyword::assign_))
                {
                    Type* tp = nullptr;
                    EXPRESSION* exp = nullptr;
                    lex = getsym();
                    lex = expression_no_comma(lex, funcsp, nullptr, &tp, &exp, nullptr, _F_SCOPEDENUM);
                    if (tp)
                    {
                        optimize_for_constants(&exp);
                    }

                    if (tp && isintconst(exp))  // type is redefined to nullptr here, is this intentional?
                    {
                        if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c2x)
                        {
                            if (!fixedType)
                            {
                                if (tp->type != BasicType::bool_)
                                {
                                    if (!tp->CompatibleType(unfixedType))
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
                                    long long n = ~((v << (fixedType->size * 8 - (fixedType->IsUnsigned() ? 0 : 1))) - 1);
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
                        if (!definingTemplate)
                            error(ERR_CONSTANT_VALUE_EXPECTED);
                        errskim(&lex, skim_end);
                    }
                }
                sp->sb->value.i = enumval++;
                if ((Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c2x) && spi->tp->btp)
                {
                    if (fixedType)
                    {
                        long long v = 1;
                        long long n = ~((v << (fixedType->size * 8 - (fixedType->IsUnsigned() ? 0 : 1))) - 1);
                        if ((sp->sb->value.i & n) != 0 && (sp->sb->value.i & n) != n)
                            error(ERR_ENUMERATION_OUT_OF_RANGE_OF_BASE_TYPE);
                    }
                    else
                    {
                        long long v = 1;
                        long long n = ~((v << (spi->tp->size * 8)) - 1);
                        if ((sp->sb->value.i & n) != 0 && (sp->sb->value.i & n) != n)
                        {
                            spi->tp->btp->type = BasicType::long_long_;
                            spi->tp->size = spi->tp->btp->size = getSize(BasicType::long_long_);
                            unfixedType = spi->tp->btp;
                        }
                    }
                }
                if (!MATCHKW(lex, Keyword::comma_))
                    break;
                else
                {
                    lex = getsym();
                    if (KW(lex) == Keyword::end_)
                    {
                        if (Optimizer::cparams.prm_ansi && Optimizer::cparams.c_dialect < Dialect::c99 &&
                            !Optimizer::cparams.prm_cplusplus)
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
    else if (!MATCHKW(lex, Keyword::end_))
    {
        errorint(ERR_NEEDY, '}');
        errskim(&lex, skim_end);
        skip(&lex, Keyword::end_);
    }
    else
        lex = getsym();
    if (fixed)
        spi->tp->fixed = true;
    spi->sb->declaring = false;
    return lex;
}
LexList* declenum(LexList* lex, SYMBOL* funcsp, Type** tp, StorageClass storage_class, AccessLevel access, bool opaque, bool* defd)
{
    SymbolTable<SYMBOL>* table;
    const char* tagname;
    char newName[4096];
    int charindex;
    bool noname = false;
    bool scoped = false;
    Type* fixedType = nullptr;
    SYMBOL* sp;
    std::list<NAMESPACEVALUEDATA*>* nsv;
    SYMBOL* strSym;
    int declline = lex->data->errline;
    int realdeclline = lex->data->linedata->lineno;
    bool anonymous = false;
    Linkage linkage1 = Linkage::none_, linkage2 = Linkage::none_, linkage3 = Linkage::none_;
    *defd = false;
    lex = getsym();
    auto oldAttribs = basisAttribs;
    basisAttribs = {0};
    ParseAttributeSpecifiers(&lex, funcsp, true);
    if (Optimizer::cparams.prm_cplusplus && (MATCHKW(lex, Keyword::class_) || MATCHKW(lex, Keyword::struct_)))
    {
        scoped = true;
        lex = getsym();
    }
    if (MATCHKW(lex, Keyword::declspec_))
    {
        lex = getsym();
        lex = parse_declspec(lex, &linkage1, &linkage2, &linkage3);
    }
    if (ISID(lex))
    {
        charindex = lex->data->charindex;
        tagname = litlate(lex->data->value.s.a);
    }
    else
    {
        noname = true;
        tagname = "";
        charindex = -1;
        anonymous = true;
    }

    Utils::StrCpy(newName, tagname);

    lex = tagsearch(lex, newName, sizeof(newName), &sp, &table, &strSym, &nsv, storage_class);

    ParseAttributeSpecifiers(&lex, funcsp, true);
    if (funcsp && (MATCHKW(lex, Keyword::colon_) || MATCHKW(lex, Keyword::begin_)))
    {
        sp = nullptr;
        nsv = nullptr;
        if (localNameSpace->front()->tags)
        {
            sp = localNameSpace->front()->tags->Lookup(newName);
        }
        table = localNameSpace->front()->tags;
    }
    if (KW(lex) == Keyword::colon_)
    {
        RequiresDialect::Feature(Dialect::c2x, "Underlying type");
        lex = getsym();
        fixedType = TypeGenerator::TypeId(lex, funcsp, StorageClass::cast_, false, true, false);
        if (!fixedType || !fixedType->IsInt())
        {
            error(ERR_NEED_INTEGER_TYPE);
        }
        else
        {
            fixedType = fixedType->BaseType();  // ignore qualifiers
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
        sp->sb->storage_class = StorageClass::type_;
        sp->tp = Type::MakeType(BasicType::enum_);
        if (fixedType)
        {
            sp->tp->fixed = true;
            sp->tp->btp = fixedType;
        }
        else
        {
            sp->tp->btp = Type::MakeType(BasicType::int_);
        }
        sp->tp->scoped = scoped;
        sp->tp->size = sp->tp->btp->size;
        sp->tp = AttributeFinish(sp, sp->tp);
        sp->sb->declcharpos = charindex;
        sp->sb->declline = sp->sb->origdeclline = declline;
        sp->sb->realdeclline = realdeclline;
        sp->sb->declfile = sp->sb->origdeclfile = lex->data->errfile;
        sp->sb->declfilenum = lex->data->linedata->fileindex;
        if (storage_class == StorageClass::member_ || storage_class == StorageClass::mutable_)
            sp->sb->parentClass = enclosingDeclarations.GetFirst();
        if (nsv)
            sp->sb->parentNameSpace = nsv->front()->name;
        sp->sb->anonymous = charindex == -1;
        if (!anonymous)
        {
            SetLinkerNames(sp, Linkage::cdecl_);
            browse_variable(sp);
            (Optimizer::cparams.prm_cplusplus && !sp->sb->parentClass ? globalNameSpace->front()->tags : table)->Add(sp);
        }
    }
    else if (sp->tp->type != BasicType::enum_)
    {
        errorsym(ERR_ORIGINAL_TYPE_NOT_ENUMERATION, sp);
    }
    else if ((scoped && !sp->tp->scoped) || (fixedType && sp->tp->btp->type != fixedType->type))
    {
        error(ERR_REDEFINITION_OF_ENUMERATION_SCOPE_OR_BASE_TYPE);
    }
    if (noname && (scoped || opaque || KW(lex) != Keyword::begin_))
    {
        error(ERR_ENUMERATED_TYPE_NEEDS_NAME);
    }
    if (KW(lex) == Keyword::begin_)
    {
        if (scoped)
            enumSyms = sp;
        lex = enumbody(lex, funcsp, sp, storage_class, access, fixedType, scoped);
        enumSyms = nullptr;
        *defd = true;
    }
    else if (noname)
    {
        errorint(ERR_NEEDY, '{');
    }
    else if (!Optimizer::cparams.prm_cplusplus && Optimizer::cparams.prm_ansi && !sp->tp->syms)
    {
        errorsym(ERR_ANSI_ENUM_NEEDS_BODY, sp);
    }
    sp->tp->sp = sp;
    *tp = sp->tp;
    if (anonymous)
    {
        sp->name = AnonymousTypeName(sp, Optimizer::cparams.prm_cplusplus && !sp->sb->parentClass ? globalNameSpace->front()->tags
                                                                                                  : table);
        SetLinkerNames(sp, Linkage::cdecl_);
        browse_variable(sp);
        (Optimizer::cparams.prm_cplusplus && !sp->sb->parentClass ? globalNameSpace->front()->tags : table)->Add(sp);
    }
    basisAttribs = oldAttribs;
    return lex;
}
static LexList* getStorageClass(LexList* lex, SYMBOL* funcsp, StorageClass* storage_class, Linkage* linkage,
                                Optimizer::ADDRESS* address, bool* blocked, bool* isExplicit, AccessLevel access)
{
    (void)access;
    bool found = false;
    StorageClass oldsc;
    while (KWTYPE(lex, TT_STORAGE_CLASS))
    {
        switch (KW(lex))
        {
            case Keyword::extern_:
                oldsc = *storage_class;
                if (*storage_class == StorageClass::parameter_ || *storage_class == StorageClass::member_ ||
                    *storage_class == StorageClass::mutable_)
                    errorstr(ERR_INVALID_STORAGE_CLASS, lex->data->kw->name);
                else
                    *storage_class = StorageClass::external_;
                lex = getsym();
                if (Optimizer::cparams.prm_cplusplus)
                {
                    if (lex->data->type == LexType::l_astr_)
                    {
                        Optimizer::SLCHAR* ch = (Optimizer::SLCHAR*)lex->data->value.s.w;
                        char buf[256];
                        int i;
                        Linkage next = Linkage::none_;
                        for (i = 0; i < ch->count; i++)
                        {
                            buf[i] = ch->str[i];
                        }
                        buf[i] = 0;
                        if (oldsc == StorageClass::auto_ || oldsc == StorageClass::register_)
                            error(ERR_NO_LINKAGE_HERE);
                        if (!strcmp(buf, "C++"))
                            next = Linkage::cpp_;
                        if (!strcmp(buf, "C"))
                            next = Linkage::c_;
                        if (!strcmp(buf, "PASCAL"))
                            next = Linkage::pascal_;
                        if (!strcmp(buf, "stdcall"))
                            next = Linkage::stdcall_;
                        if (!strcmp(buf, "stdcall"))
                            next = Linkage::fastcall_;
                        if (next != Linkage::none_)
                        {
                            *linkage = next;
                            lex = getsym();
                            if (MATCHKW(lex, Keyword::begin_))
                            {
                                *blocked = true;
                                lex = getsym();
                                while (lex && !MATCHKW(lex, Keyword::end_))
                                {
                                    lex = declare(lex, nullptr, nullptr, StorageClass::global_, *linkage, emptyBlockdata, true,
                                                  false, false, AccessLevel::public_);
                                }
                                needkw(&lex, Keyword::end_);
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
            case Keyword::virtual_:
                if (*storage_class != StorageClass::member_)
                {
                    errorstr(ERR_INVALID_STORAGE_CLASS, lex->data->kw->name);
                }
                else
                {
                    *storage_class = StorageClass::virtual_;
                }
                lex = getsym();
                break;
            case Keyword::explicit_:
                if (*storage_class != StorageClass::member_)
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
            case Keyword::mutable_:
                if (*storage_class != StorageClass::member_)
                {
                    error(ERR_EXPLICIT_CONSTRUCTOR_OR_CONVERSION_FUNCTION);
                }
                else
                {
                    *storage_class = StorageClass::mutable_;
                }
                lex = getsym();
                break;
            case Keyword::static_:
                if (*storage_class == StorageClass::parameter_ ||
                    (!Optimizer::cparams.prm_cplusplus &&
                     (*storage_class == StorageClass::member_ || *storage_class == StorageClass::mutable_)))
                    errorstr(ERR_INVALID_STORAGE_CLASS, lex->data->kw->name);
                else if (*storage_class == StorageClass::auto_)
                    *storage_class = StorageClass::localstatic_;
                else
                    *storage_class = StorageClass::static_;
                lex = getsym();
                break;
            case Keyword::absolute_:
                if (*storage_class == StorageClass::parameter_ || *storage_class == StorageClass::member_ ||
                    *storage_class == StorageClass::mutable_)
                    errorstr(ERR_INVALID_STORAGE_CLASS, lex->data->kw->name);
                else
                    *storage_class = StorageClass::absolute_;
                lex = getsym();
                if (MATCHKW(lex, Keyword::openpa_))
                {
                    Type* tp = nullptr;
                    EXPRESSION* exp = nullptr;
                    lex = getsym();
                    lex = optimized_expression(lex, funcsp, nullptr, &tp, &exp, false);
                    if (tp && isintconst(exp))
                        *address = exp->v.i;
                    else
                        error(ERR_CONSTANT_VALUE_EXPECTED);
                    if (!MATCHKW(lex, Keyword::closepa_))
                        needkw(&lex, Keyword::closepa_);
                    lex = getsym();
                }
                else
                    error(ERR_ABSOLUTE_NEEDS_ADDRESS);
                break;
            case Keyword::auto_:
                if (*storage_class != StorageClass::auto_)
                    errorstr(ERR_INVALID_STORAGE_CLASS, "auto");
                else
                    *storage_class = StorageClass::auto_;
                lex = getsym();
                break;
            case Keyword::register_:
                if (*storage_class != StorageClass::auto_ && *storage_class != StorageClass::parameter_)
                    errorstr(ERR_INVALID_STORAGE_CLASS, "register");
                else if (*storage_class != StorageClass::parameter_)
                    *storage_class = StorageClass::register_;
                lex = getsym();
                break;
            case Keyword::typedef_:
                if (*storage_class == StorageClass::parameter_)
                    errorstr(ERR_INVALID_STORAGE_CLASS, "typedef");
                else
                    *storage_class = StorageClass::typedef_;
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
LexList* parse_declspec(LexList* lex, Linkage* linkage, Linkage* linkage2, Linkage* linkage3)
{
    (void)linkage;
    if (needkw(&lex, Keyword::openpa_))
    {
        while (1)
        {
            if (ISID(lex))
            {
                if (!strcmp(lex->data->value.s.a, "noreturn"))
                {
                    if (*linkage3 != Linkage::none_)
                        error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                    *linkage3 = Linkage::noreturn_;
                }
                else if (!strcmp(lex->data->value.s.a, "align"))
                {
                    lex = getsym();
                    if (needkw(&lex, Keyword::openpa_))
                    {
                        Type* tp = nullptr;
                        EXPRESSION* exp = nullptr;

                        lex = optimized_expression(lex, nullptr, nullptr, &tp, &exp, false);
                        if (!tp || !tp->IsInt())
                            error(ERR_NEED_INTEGER_TYPE);
                        else if (!isintconst(exp))
                            error(ERR_CONSTANT_VALUE_EXPECTED);
                        int align = exp->v.i;
                        LexList* pos = lex;
                        if (needkw(&lex, Keyword::closepa_))
                            lex = prevsym(pos);
                        basisAttribs.inheritable.structAlign = align;
                        if (basisAttribs.inheritable.structAlign > 0x10000 ||
                            (basisAttribs.inheritable.structAlign & (basisAttribs.inheritable.structAlign - 1)) != 0)
                            error(ERR_INVALID_ALIGNMENT);
                    }
                }
                else if (!strcmp(lex->data->value.s.a, "dllimport") || !strcmp(lex->data->value.s.a, "__dllimport__"))
                {
                    if (*linkage2 != Linkage::none_)
                        error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                    *linkage2 = Linkage::import_;
                }
                else if (!strcmp(lex->data->value.s.a, "dllexport") || !strcmp(lex->data->value.s.a, "__dllexport__"))
                {
                    if (*linkage2 != Linkage::none_)
                        error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                    *linkage2 = Linkage::export_;
                }
                else if (!strcmp(lex->data->value.s.a, "thread") || !strcmp(lex->data->value.s.a, "__thread__"))
                {
                    if (*linkage3 != Linkage::none_)
                        error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                    *linkage3 = Linkage::threadlocal_;
                }
                else if (!strcmp(lex->data->value.s.a, "deprecated") || !strcmp(lex->data->value.s.a, "__deprecated__"))
                {
                    basisAttribs.uninheritable.deprecationText = (char*)-1;
                }
                else
                {
                    error(ERR_IGNORING__DECLSPEC);
                }
            }
            else if (MATCHKW(lex, Keyword::noreturn_))
            {
                if (*linkage3 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage3 = Linkage::noreturn_;
            }
            else
            {
                error(ERR_IGNORING__DECLSPEC);
                break;
            }
            lex = getsym();
            if (MATCHKW(lex, Keyword::openpa_))
            {
                errskim(&lex, skim_closepa);
            }
            if (!MATCHKW(lex, Keyword::comma_))
                break;
            lex = getsym();
        }
        needkw(&lex, Keyword::closepa_);
    }
    return lex;
}
static LexList* getLinkageQualifiers(LexList* lex, Linkage* linkage, Linkage* linkage2, Linkage* linkage3)
{
    while (KWTYPE(lex, TT_LINKAGE))
    {
        Keyword kw = KW(lex);
        lex = getsym();
        switch (kw)
        {
            case Keyword::cdecl_:
                if (*linkage != Linkage::none_ && *linkage != Linkage::cdecl_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = Linkage::cdecl_;
                break;
            case Keyword::stdcall_:
                if (*linkage != Linkage::none_ && *linkage != Linkage::stdcall_ &&
                    (!Optimizer::cparams.prm_cplusplus || *linkage != Linkage::c_))
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = Linkage::stdcall_;
                break;
            case Keyword::fastcall_:
                if (*linkage != Linkage::none_ && *linkage != Linkage::fastcall_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = Linkage::fastcall_;
                break;
            case Keyword::interrupt_:
                if (*linkage != Linkage::none_ && *linkage != Linkage::interrupt_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = Linkage::interrupt_;
                break;
            case Keyword::fault_:
                if (*linkage != Linkage::none_ && *linkage != Linkage::fault_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = Linkage::fault_;
                break;
            case Keyword::inline_:
                if (*linkage != Linkage::none_ && *linkage != Linkage::inline_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = Linkage::inline_;
                break;
            case Keyword::noreturn_:
                if (*linkage3 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage3 = Linkage::noreturn_;
                break;
            case Keyword::thread_local_:
                if (*linkage3 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage3 = Linkage::threadlocal_;
                break;
            case Keyword::dllexport_:
                if (*linkage2 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage2 = Linkage::export_;
                break;
            case Keyword::declspec_:
                lex = parse_declspec(lex, linkage, linkage2, linkage3);
                break;
            case Keyword::rtllinkage_:
                *linkage2 = getDefaultLinkage();
                break;
            case Keyword::entrypoint_:
                if (*linkage3 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage3 = Linkage::entrypoint_;
                break;

            case Keyword::property_:
                if (*linkage2 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage2 = Linkage::property_;
                break;
            case Keyword::msil_rtl_:
                if (*linkage2 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage2 = Linkage::msil_rtl_;
                break;
            case Keyword::unmanaged_:
                if (*linkage2 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage2 = Linkage::unmanaged_;
                break;
            case Keyword::dllimport_:
                if (*linkage2 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage2 = Linkage::import_;
                importFile = nullptr;
                if (MATCHKW(lex, Keyword::openpa_))
                {
                    lex = getsym();
                    if (lex->data->type == LexType::l_astr_)
                    {
                        int i, len = ((Optimizer::SLCHAR*)lex->data->value.s.w)->count;
                        importFile = Allocate<char>(len + 1);
                        for (i = 0; i < len; i++)
                            importFile[i] = (char)((Optimizer::SLCHAR*)lex->data->value.s.w)->str[i];
                        lex = getsym();
                    }
                    needkw(&lex, Keyword::closepa_);
                }
                break;
            default:
                break;
        }
    }
    return lex;
}
LexList* getQualifiers(LexList* lex, Type** tp, Linkage* linkage, Linkage* linkage2, Linkage* linkage3, bool* asFriend)
{
    while (KWTYPE(lex, (TT_TYPEQUAL | TT_LINKAGE)))
    {
        if (asFriend && MATCHKW(lex, Keyword::friend_))
        {
            *asFriend = true;
            lex = getsym();
        }
        else
        {
            *tp = TypeGenerator::PointerQualifiers(lex, *tp, false);
            if (MATCHKW(lex, Keyword::atomic_))
                break;
            lex = getLinkageQualifiers(lex, linkage, linkage2, linkage3);
        }
    }
    ParseAttributeSpecifiers(&lex, theCurrentFunc, true);
    return lex;
}
static LexList* nestedTypeSearch(LexList* lex, SYMBOL** sym)
{
    *sym = nullptr;
    lex = nestedSearch(lex, sym, nullptr, nullptr, nullptr, nullptr, false, StorageClass::global_, false, true);
    if (!*sym || !istype((*sym)))
    {
        error(ERR_TYPE_NAME_EXPECTED);
    }
    return lex;
}
static bool isPointer(LexList* lex)
{
    while (KWTYPE(lex, (TT_TYPEQUAL | TT_LINKAGE)))
        lex = getsym();
    if (ISKW(lex))
        switch (KW(lex))
        {
            case Keyword::and_:
            case Keyword::land_:
            case Keyword::star_:
                return true;
            default:
                return false;
        }
    return false;
}
static void matchFunctionDeclaration(LexList* lex, SYMBOL* sp, SYMBOL* spo, bool checkReturn, bool asFriend)
{
    /* two oldstyle declarations aren't compared */
    if ((spo && !spo->sb->oldstyle && spo->sb->hasproto) || !sp->sb->oldstyle)
    {
        if (spo && spo->tp->IsFunction())
        {
            if (checkReturn)
                spo->tp->BaseType()->btp->InstantiateDeferred();
            if (checkReturn && !spo->sb->isConstructor && !spo->sb->isDestructor &&
                !spo->tp->BaseType()->btp->CompatibleType(sp->tp->BaseType()->btp) &&
                !SameTemplatePointedTo(spo->tp->BaseType()->btp, sp->tp->BaseType()->btp))
            {
                if (!definingTemplate || instantiatingTemplate)
                    preverrorsym(ERR_TYPE_MISMATCH_FUNC_DECLARATION, spo, spo->sb->declfile, spo->sb->declline);
            }
            else
            {
                auto ito1 = spo->tp->BaseType()->syms->begin();
                auto hro1end = spo->tp->BaseType()->syms->end();
                auto it1 = sp->tp->BaseType()->syms->begin();
                auto hr1end = sp->tp->BaseType()->syms->end();
                if (ito1 != hro1end && (*ito1)->sb->thisPtr)
                    ++ito1;
                if (ito1 != hro1end && it1 != hr1end && (*ito1)->tp)
                {
                    while (ito1 != hro1end && it1 != hr1end)
                    {
                        SYMBOL* spo1 = *ito1;
                        SYMBOL* sp1 = *it1;
                        if (!spo1->tp->CompatibleType(sp1->tp) && !SameTemplatePointedTo(spo1->tp, sp1->tp) &&
                            !SameTemplateSelector(sp1->tp, spo1->tp))
                        {
                            break;
                        }
                        ++ito1;
                        ++it1;
                    }
                    if ((ito1 != hro1end || it1 != hr1end) && spo != sp)
                    {
                        preverrorsym(ERR_TYPE_MISMATCH_FUNC_DECLARATION, spo, spo->sb->declfile, spo->sb->declline);
                    }
                    else
                    {
                        bool err = false;
                        SYMBOL* last = nullptr;
                        auto ito1 = spo->tp->BaseType()->syms->begin();
                        auto hro1end = spo->tp->BaseType()->syms->end();
                        auto it1 = sp->tp->BaseType()->syms->begin();
                        auto hr1end = sp->tp->BaseType()->syms->end();
                        if (ito1 != hro1end && (*ito1)->sb && (*ito1)->sb->thisPtr)
                            ++ito1;
                        if (it1 != hr1end && (*it1)->sb && (*it1)->sb->thisPtr)
                            ++it1;
                        while (ito1 != hro1end && it1 != hr1end)
                        {
                            SYMBOL* so = *ito1;
                            SYMBOL* s = *it1;
                            if (so != s && (so->sb->init || so->sb->deferredCompile) && (s->sb->init || s->sb->deferredCompile))
                                errorsym(ERR_CANNOT_REDECLARE_DEFAULT_ARGUMENT, so);
                            if (!err && last && last->sb->init &&
                                !(so->sb->init || s->sb->init || so->sb->deferredCompile || s->sb->deferredCompile))
                            {
                                err = true;
                                errorsym(ERR_MISSING_DEFAULT_ARGUMENT, last);
                            }
                            last = so;
                            if (so->sb->init || so->sb->deferredCompile)
                            {
                                s->sb->init = so->sb->init;
                                s->sb->deferredCompile = so->sb->deferredCompile;
                            }
                            ++ito1;
                            ++it1;
                        }
                        // this is kind of iffy the hr->p values were copied one by one
                        if (MATCHKW(lex, Keyword::colon_) || MATCHKW(lex, Keyword::try_) || MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::try_))
                            spo->tp->BaseType()->syms = sp->tp->BaseType()->syms;
                        else
                            sp->tp->BaseType()->syms = spo->tp->BaseType()->syms;
                    }
                }
            }
        }
    }
    if (!asFriend)
    {
        if ((spo->sb->xc && spo->sb->xc->xcDynamic) || (sp->sb->xc && sp->sb->xc->xcDynamic))
        {
            if (!sp->sb->xc || !sp->sb->xc->xcDynamic)
            {
                if (!MATCHKW(lex, Keyword::begin_))
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
                        if (((Type*)lo->data)->CompatibleType((Type*)li->data) &&
                            ((Type*)lo->data)->SameIntegerType((Type*)li->data))
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
        else if (!definingTemplate && spo->sb->xcMode != sp->sb->xcMode)
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
                if (!MATCHKW(lex, Keyword::begin_))
                    errorsym(ERR_EXCEPTION_SPECIFIER_MUST_MATCH, sp);
            }
            else
            {
                errorsym(ERR_EXCEPTION_SPECIFIER_MUST_MATCH, sp);
            }
        }
    }
}
LexList* getDeferredData(LexList* lex, LexList** savePos, bool braces)
{
    LexList **cur = savePos, *last = nullptr;
    int paren = 0;
    int begin = 0;
    int brack = 0;
    int ltgt = 0;
    bool viaTry = false;
    if (braces) // theoretically we can only have a try at this point for function bodies...
        viaTry = MATCHKW(lex, Keyword::try_);
    while (lex != nullptr)
    {
        Keyword kw = KW(lex);
        if (braces)
        {
            if (kw == Keyword::begin_)
            {
                paren++;
            }
            else if (kw == Keyword::end_ && !--paren)
            {
                *cur = Allocate<LexList>();
                **cur = *lex;
                (*cur)->prev = last;
                last = *cur;
                lex = getsym();
                if (!viaTry || !MATCHKW(lex, Keyword::catch_))
                {
                    break;
                }
            }
        }
        else
        {
            if (kw == Keyword::semicolon_)
            {
                break;
            }
            else if (kw == Keyword::openpa_)
            {
                paren++;
            }
            else if (kw == Keyword::closepa_)
            {
                if (paren-- == 0 && !brack && !begin)
                {
                    break;
                }
            }
            else if (kw == Keyword::begin_)
            {
                begin++;
            }
            else if (kw == Keyword::end_)
            {
                if (begin-- == 0 && !brack && !paren)
                {
                    break;
                }
            }
            else if (kw == Keyword::openbr_)
            {
                brack++;
            }
            else if (kw == Keyword::closebr_)
            {
                brack--;
            }
            else if (kw == Keyword::comma_ && !paren && !brack && !ltgt && !begin)
            {
                break;
            }
            // there is some ambiguity between templates and <
            else if (kw == Keyword::lt_)
            {
                ltgt++;
            }
            else if (kw == Keyword::gt_)
            {
                ltgt--;
            }
        }
        *cur = Allocate<LexList>();
        if (lex->data->type == LexType::l_id_)
            lex->data->value.s.a = litlate(lex->data->value.s.a);
        **cur = *lex;
        (*cur)->prev = last;
        (*cur)->data->linedata = lines && lines->size() ? lines->front() : &nullLineData;
        lines = nullptr;
        last = *cur;
        cur = &(*cur)->next;
        lex = getsym();
    }
    return lex;
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
            al = al->tp->syms->front();
        }
        if (fr)
        {
            fr = fr->tp->syms->front();
        }
        if (al && fr)
        {
            CallSite* epx = Allocate<CallSite>();
            CallSite* ld = Allocate<CallSite>();
            epx->ascall = true;
            epx->fcall = MakeExpression(ExpressionNode::pc_, al);
            epx->sp = al;
            epx->functp = al->tp;
            epx->arguments = argumentListFactory.CreateList();
            auto arg = Allocate<Argument>();
            epx->arguments->push_back(arg);
            arg->tp = &stdint;
            arg->exp = ep2;
            ep2 = MakeExpression(epx);
            ep2 = MakeExpression(ExpressionNode::assign_, ep1, ep2);
            ld->ascall = true;
            ld->fcall = MakeExpression(ExpressionNode::pc_, fr);
            ld->sp = fr;
            ld->functp = fr->tp;
            arg = Allocate<Argument>();
            ld->arguments = argumentListFactory.CreateList();
            ld->arguments->push_back(arg);
            arg->tp = &stdpointer;
            arg->exp = ep1;
            unloader = MakeExpression(ld);
            ep1 = ep2;
        }
        else
        {
            diag("llallocatevla: cannot find allocator");
            return MakeIntExpression(ExpressionNode::c_i_, 0);
        }
    }
    else
    {
        EXPRESSION* var = AnonymousVar(StorageClass::auto_, &stdpointer);
        loader = MakeExpression(ExpressionNode::savestack_, var);
        unloader = MakeExpression(ExpressionNode::loadstack_, var);
        ep1 = MakeExpression(ExpressionNode::assign_, ep1, MakeExpression(ExpressionNode::alloca_, ep2));
        ep1 = MakeExpression(ExpressionNode::comma_, loader, ep1);
    }

    InsertInitializer(&sp->sb->dest, sp->tp, unloader, 0, false);
    return ep1;
}
static EXPRESSION* vlaSetSizes(EXPRESSION*** rptr, EXPRESSION* vlanode, Type* btp, SYMBOL* sp, int* index, int* vlaindex, int sou)
{
    EXPRESSION *mul, *mul1, *store;
    btp = btp->BaseType();
    if (btp->vla)
    {
        mul = vlaSetSizes(rptr, vlanode, btp->btp, sp, index, vlaindex, sou);
        mul1 = Allocate<EXPRESSION>();
        if (!btp->esize)
            btp->esize = MakeIntExpression(ExpressionNode::c_i_, 1);
        *mul1 = *btp->esize;
        mul = mul1 = MakeExpression(ExpressionNode::arraymul_, mul, mul1);
        btp->sp = sp;
        btp->vlaindex = (*vlaindex)++;
    }
    else
    {
#ifdef ERROR
#    error Sizeof vla of vla
#endif
        mul = MakeIntExpression(ExpressionNode::c_i_, btp->size);
        mul1 = Allocate<EXPRESSION>();
        *mul1 = *mul;
    }
    store = MakeExpression(ExpressionNode::add_, vlanode, MakeIntExpression(ExpressionNode::c_i_, *index));
    Dereference(&stdint, &store);
    store = MakeExpression(ExpressionNode::assign_, store, mul1);
    **rptr = MakeExpression(ExpressionNode::comma_, store);
    *rptr = &(**rptr)->right;
    *index += sou;
    return mul;
}
static void allocateVLA(LexList* lex, SYMBOL* sp, SYMBOL* funcsp, std::list<FunctionBlock*>& block, Type* btp, bool bypointer)
{
    EXPRESSION *result = nullptr, **rptr = &result;
    Type* tp1 = btp;
    int count = 0;
    int soa = getSize(BasicType::pointer_);
    int sou = getSize(BasicType::unsigned_);
    while (tp1->vla)
    {
        count++, tp1 = tp1->btp;
    }
    if (!funcsp)
        error(ERR_VLA_BLOCK_SCOPE); /* this is a safety, should already be caught */
    if (!bypointer)
    {
        /* vla */
        if (sp->sb->storage_class != StorageClass::auto_ && sp->sb->storage_class != StorageClass::typedef_)
            error(ERR_VLA_BLOCK_SCOPE);
    }
    else
    {
        /* pointer to vla */
        if (sp->sb->storage_class != StorageClass::auto_ && sp->sb->storage_class != StorageClass::typedef_ &&
            sp->sb->storage_class != StorageClass::localstatic_)
            error(ERR_VLA_BLOCK_SCOPE);
    }
    currentLineData(block, lex, 0);
    if (sp->tp->sp)
    {
        SYMBOL* dest = sp;
        SYMBOL* src = sp->tp->sp;
        *rptr = MakeExpression(ExpressionNode::comma_);
        rptr = &(*rptr)->right;
        result->left = MakeExpression(ExpressionNode::blockassign_, MakeExpression(ExpressionNode::auto_, dest),
                                      MakeExpression(ExpressionNode::auto_, src));
        dest->tp->size = src->tp->size;
        result->left->size = src->tp;
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
        vlanode = MakeExpression(ExpressionNode::auto_, vlasp);
        vlaSetSizes(&rptr, vlanode, btp, vlasp, &index, &vlaindex, sou);

        ep = MakeExpression(ExpressionNode::add_, vlanode, MakeIntExpression(ExpressionNode::c_i_, soa));
        Dereference(&stdint, &ep);
        ep = MakeExpression(ExpressionNode::assign_, ep, MakeIntExpression(ExpressionNode::c_i_, count));
        *rptr = MakeExpression(ExpressionNode::comma_, ep);
        rptr = &(*rptr)->right;

        sp->tp->BaseType()->size = size; /* size field is actually size of VLA header block */
                                         /* sizeof will fetch the size from that */
    }
    if (result != nullptr)
    {
        Statement* st = Statement::MakeStatement(nullptr, block, StatementNode::declare_);
        st->hasvla = true;
        if (sp->sb->storage_class != StorageClass::typedef_ && !bypointer)
        {
            EXPRESSION* ep1 = MakeExpression(ExpressionNode::add_, MakeExpression(ExpressionNode::auto_, sp),
                                             MakeIntExpression(ExpressionNode::c_i_, 0));
            EXPRESSION* ep2 = MakeExpression(ExpressionNode::add_, MakeExpression(ExpressionNode::auto_, sp),
                                             MakeIntExpression(ExpressionNode::c_i_, soa + sou * (count + 1)));
            Dereference(&stdpointer, &ep1);
            Dereference(&stdint, &ep2);
            ep1 = llallocateVLA(sp, ep1, ep2);
            *rptr = (Optimizer::architecture == ARCHITECTURE_MSIL) ? ep1 : MakeExpression(ExpressionNode::comma_, ep1);
            sp->sb->assigned = true;
        }
        st->select = result;
        optimize_for_constants(&st->select);
    }
}
void sizeQualifiers(Type* tp)
{
    if (tp->type != BasicType::atomic_)
    {
        while (true)
        {
            Type* tp1 = tp->BaseType();
            while (tp1 != tp)
            {
                tp->size = tp1->size;
                tp = tp->btp;
            }
            if (tp->IsPtr())
            {
                tp = tp->btp;
            }
            else
                break;
        }
    }
}
static bool sameQuals(SYMBOL* sp1, SYMBOL* sp2)
{
    Type *tp1 = sp1->tp, *tp2 = sp2->tp;
    while (tp1 && tp2)
    {
        if (tp1->IsConst() != tp2->IsConst())
        {
            // constexpr matches const at top level
            if (tp1->BaseType()->btp || tp2->BaseType()->btp || (tp1->IsConst() && !sp2->sb->constexpression) ||
                (tp2->IsConst() && !sp1->sb->constexpression))
                return false;
        }
        if (tp1->IsVolatile() != tp2->IsVolatile())
            return false;
        if (tp1->IsRestrict() != tp2->IsRestrict())
            return false;
        tp1 = tp1->BaseType()->btp;
        tp2 = tp2->BaseType()->btp;
    }
    return true;
}
LexList* getStorageAndType(LexList* lex, SYMBOL* funcsp, SYMBOL** strSym, bool inTemplate, bool assumeType, bool* deduceTemplate,
                           StorageClass* storage_class, StorageClass* storage_class_in, Optimizer::ADDRESS* address, bool* blocked,
                           bool* isExplicit, bool* constexpression, bool* builtin_constexpr, Type** tp, Linkage* linkage,
                           Linkage* linkage2, Linkage* linkage3, AccessLevel access, bool* notype, bool* defd, int* consdest,
                           bool* templateArg, bool* asFriend)
{
    bool foundType = false;
    bool first = true;
    bool flaggedTypedef = false;
    *blocked = false;
    *constexpression = *builtin_constexpr = false;

    while (KWTYPE(lex, TT_STORAGE_CLASS | TT_POINTERQUAL | TT_LINKAGE | TT_DECLARE) ||
           (!foundType && TypeGenerator::StartOfType(lex, nullptr, assumeType)) || MATCHKW(lex, Keyword::complx_) ||
           (*storage_class == StorageClass::typedef_ && !foundType))
    {
        if (KWTYPE(lex, TT_DECLARE))
        {
            // The next line has a recurring check. The 'lex' condition was already verified above
            // the problem is I'm using macros that I want to be independent from each other
            // and I don't want to introduce another macro without the check for readability
            // reasons.  So I'm going to leave the recurring check.
            if (MATCHKW(lex, Keyword::constexpr_))
            {
                *constexpression = true;
            }
            if (MATCHKW(lex, Keyword::builtin_constexpr_))
            {
                *constexpression = true;
                *builtin_constexpr = true;
            }
            lex = getsym();
        }
        else if (KWTYPE(lex, TT_STORAGE_CLASS))
        {
            lex = getStorageClass(lex, funcsp, storage_class, linkage, address, blocked, isExplicit, access);
            if (*blocked)
                break;
            if (*storage_class == StorageClass::typedef_)
            {
                flaggedTypedef = true;
                inTypedef++;
            }
        }
        else if (KWTYPE(lex, TT_POINTERQUAL | TT_LINKAGE))
        {
            lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, asFriend);
            if (MATCHKW(lex, Keyword::atomic_))
            {
                foundType = true;
                *tp = TypeGenerator::UnadornedType(
                    lex, funcsp, *tp, strSym, inTemplate, *storage_class_in, linkage, linkage2, linkage3, access, notype, defd,
                    consdest, templateArg, Optimizer::cparams.cpp_dialect >= Dialect::cpp17 ? deduceTemplate : nullptr,
                    *storage_class == StorageClass::typedef_, true, false, asFriend, *constexpression);
            }
            if (*linkage3 == Linkage::threadlocal_ && *storage_class == StorageClass::member_)
                *storage_class = StorageClass::static_;
        }
        else if (foundType)
        {
            break;
        }
        else
        {
            foundType = true;
            *tp = TypeGenerator::UnadornedType(lex, funcsp, *tp, strSym, inTemplate, *storage_class_in, linkage, linkage2, linkage3,
                                               access, notype, defd, consdest, templateArg,
                                               Optimizer::cparams.cpp_dialect >= Dialect::cpp17 ? deduceTemplate : nullptr,
                                               *storage_class == StorageClass::typedef_, true, false, asFriend, *constexpression);
            if (*linkage3 == Linkage::threadlocal_ && *storage_class == StorageClass::member_)
                *storage_class = StorageClass::static_;
        }
        if (ParseAttributeSpecifiers(&lex, funcsp, true))
            break;
        first = false;
    }
    if (flaggedTypedef)
        inTypedef--;
    return lex;
}
static bool mismatchedOverloadLinkage(SYMBOL* sp, SymbolTable<SYMBOL>* table)
{
    if (((SYMBOL*)(table->front()))->sb->attribs.inheritable.linkage != sp->sb->attribs.inheritable.linkage)
        return true;
    return false;
}
void injectThisPtr(SYMBOL* sp, SymbolTable<SYMBOL>* syms)
{
    if (syms)
    {
        if (syms->size() && syms->front()->sb->thisPtr)
            return;
        auto type = Type::MakeType(BasicType::pointer_, sp->sb->parentClass->tp->BaseType());
        if (sp->tp->IsConst())
        {
            type->btp = Type::MakeType(BasicType::const_, type->btp);
        }
        if (sp->tp->IsVolatile())
        {
            type->btp = Type::MakeType(BasicType::volatile_, type->btp);
        }
        type->UpdateRootTypes();
        auto ths = makeID(StorageClass::parameter_, type, nullptr, "__$$this");
        ths->sb->parent = sp;
        ths->sb->thisPtr = true;
        ths->sb->attribs.inheritable.used = true;
        SetLinkerNames(ths, Linkage::cdecl_);
        syms->insert(syms->begin(), ths);
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
static bool differentTemplateNames(std::list<TEMPLATEPARAMPAIR>* a, std::list<TEMPLATEPARAMPAIR>* b)
{
    if (!a || !b)
        return false;
    auto ita = a->begin();
    auto itae = a->end();
    auto itb = b->begin();
    auto itbe = b->end();
    for (; ita != ita && itb != itbe; ++ita, ++itb)
    {
        if (ita->second->type != itb->second->type)
            return true;
        if (!ita->first || !itb->first)
            return true;
        if (strcmp(ita->first->name, itb->first->name))
            return true;
    }
    return false;
}
static bool sameNameSpace(SYMBOL* left, SYMBOL* right)
{
    if (left == right)
        return true;
    if (left && right)
    {
        if (left->sb->nameSpaceValues == right->sb->nameSpaceValues)
            return true;
    }
    return false;
}
LexList* declare(LexList* lex, SYMBOL* funcsp, Type** tprv, StorageClass storage_class, Linkage defaultLinkage,
                 std::list<FunctionBlock*>& block, bool needsemi, int asExpression, bool inTemplate, AccessLevel access)
{
    bool isExtern = false;
    bool isInline = false;
    Type* btp;
    SYMBOL* sp;
    StorageClass storage_class_in = storage_class;
    Linkage linkage = Linkage::none_;
    Linkage linkage2 = Linkage::none_;
    Linkage linkage3 = Linkage::none_;
    Linkage linkage4 = Linkage::none_;
    std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
    SYMBOL* strSym = nullptr;
    Optimizer::ADDRESS address = 0;
    Type* tp = nullptr;
    bool hasAttributes;
    attributes oldAttribs = basisAttribs;

    basisAttribs = {0};
    hasAttributes = ParseAttributeSpecifiers(&lex, funcsp, true);

    if (!MATCHKW(lex, Keyword::semicolon_))
    {
        if (MATCHKW(lex, Keyword::inline_))
        {
            isInline = true;
            linkage = Linkage::inline_;
            lex = getsym();
            ParseAttributeSpecifiers(&lex, funcsp, true);
        }
        else if (!asExpression && MATCHKW(lex, Keyword::extern_))
        {
            lex = getsym();
            if (MATCHKW(lex, Keyword::template_))
            {
                isExtern = true;
                goto jointemplate;
            }
            lex = backupsym();
        }
        if (!asExpression && MATCHKW(lex, Keyword::template_))
        {
        jointemplate:
            if (funcsp)
            {
                if (storage_class == StorageClass::member_ || storage_class == StorageClass::mutable_)
                    error(ERR_TEMPLATE_NO_LOCAL_CLASS);
                else
                    error(ERR_TEMPLATE_GLOBAL_OR_CLASS_SCOPE);
            }
            if (hasAttributes)
                error(ERR_NO_ATTRIBUTE_SPECIFIERS_HERE);

            lex = TemplateDeclaration(lex, funcsp, access, storage_class, isExtern);
            needsemi = false;
        }
        else if (!asExpression && MATCHKW(lex, Keyword::namespace_))
        {

            int count = 0;
            bool linked;
            struct _ccNamespaceData nsData;
            if (!IsCompiler())
            {
                nsData.declfile = lex->data->errfile;
                nsData.startline = lex->data->errline;
            }
            if (storage_class_in == StorageClass::member_ || storage_class_in == StorageClass::mutable_)
                error(ERR_NAMESPACE_NO_STRUCT);

            if (hasAttributes)
                error(ERR_NO_ATTRIBUTE_SPECIFIERS_HERE);
            lex = getsym();
            hasAttributes = ParseAttributeSpecifiers(&lex, funcsp, true);
            lex = insertNamespace(lex, linkage, storage_class_in, &linked);
            if (linked)
            {
                if (MATCHKW(lex, Keyword::classsel_))
                {
                    RequiresDialect::Feature(Dialect::cpp17, "Nested Namespace Definitions");
                }
                ++count;
                while (linked && MATCHKW(lex, Keyword::classsel_))
                {
                    lex = getsym();
                    lex = insertNamespace(lex, linkage, storage_class_in, &linked);
                    if (linked)
                        ++count;
                }
                if (count > 1 && (hasAttributes || linkage == Linkage::inline_))
                {
                    errorsym(ERR_NESTED_NAMESPACE_DEFINITION_NOT_INLINE_NO_ATTRIBUTES, nameSpaceList.front());
                }
                if (linked && needkw(&lex, Keyword::begin_))
                {
                    while (lex && !MATCHKW(lex, Keyword::end_))
                    {
                        lex = declare(lex, nullptr, nullptr, storage_class, defaultLinkage, emptyBlockdata, true, false, false,
                                      access);
                    }
                    if (!IsCompiler() && lex)
                        nsData.endline = lex->data->errline;
                    needkw(&lex, Keyword::end_);
                }
                for (; count; count--)
                {
                    if (nameSpaceList.size())  /// in case of error
                    {
                        SYMBOL* sp = nameSpaceList.front();
                        sp->sb->value.i--;
                        nameSpaceList.pop_front();
                        globalNameSpace->pop_front();
                        if (!IsCompiler())
                        {
                            if (!sp->sb->ccNamespaceData)
                                sp->sb->ccNamespaceData = ccNameSpaceDataListFactory.CreateList();
                            sp->sb->ccNamespaceData->push_front(&nsData);
                        }
                    }
                }
                needsemi = false;
            }
        }
        else if (!asExpression && MATCHKW(lex, Keyword::using_))
        {
            lex = getsym();
            sp = nullptr;
            lex = insertUsing(lex, &sp, access, storage_class_in, inTemplate, hasAttributes);
            if (sp && tprv)
                *tprv = (Type*)-1;
        }
        else if (!asExpression && MATCHKW(lex, Keyword::static_assert_))
        {
            if (hasAttributes)
                error(ERR_NO_ATTRIBUTE_SPECIFIERS_HERE);
            lex = getsym();
            lex = handleStaticAssert(lex);
        }
        else
        {
            bool blocked;
            bool constexpression, builtin_constexpression;
            bool defd = false;
            bool notype = false;
            bool isExplicit = false;
            bool templateArg = false;
            bool asFriend = false;
            bool deduceTemplate = false;
            int consdest = CT_NONE;
            declaringInitialType++;
            lex = getStorageAndType(lex, funcsp, &strSym, inTemplate, false, &deduceTemplate, &storage_class, &storage_class_in,
                                    &address, &blocked, &isExplicit, &constexpression, &builtin_constexpression, &tp, &linkage,
                                    &linkage2, &linkage3, access, &notype, &defd, &consdest, &templateArg, &asFriend);
            declaringInitialType--;
            bool bindingcandidate = !blocked && Optimizer::cparams.prm_cplusplus && MATCHKW(lex, Keyword::openbr_);
            if (bindingcandidate)
            {
                lex = getsym();
                bindingcandidate = ISID(lex);
                lex = backupsym();
            }
            auto ssp = enclosingDeclarations.GetFirst();
            if (blocked)
            {
                if (tp != nullptr)
                    error(ERR_TOO_MANY_TYPE_SPECIFIERS);

                needsemi = false;
            }
            else if (bindingcandidate && tp->type == BasicType::auto_ &&
                     (storage_class_in == StorageClass::auto_ || storage_class_in == StorageClass::localstatic_ ||
                      storage_class_in == StorageClass::static_ || storage_class_in == StorageClass::global_))
            {
                RequiresDialect::Feature(Dialect::cpp17, "Structured binding declarations");
                lex = GetStructuredBinding(lex, funcsp, storage_class, linkage, block);
            }
            else if (Optimizer::cparams.cpp_dialect >= Dialect::cpp17 && !strSym && inTemplate && tp &&
                     (!ssp || (ssp->tp != tp && !SameTemplate(ssp->tp, tp))) && tp->IsStructured() && tp == tp->BaseType() &&
                     tp->sp->sb->templateLevel && MATCHKW(lex, Keyword::openpa_))
            {
                *tprv = tp;
                lex = GetDeductionGuide(lex, funcsp, storage_class, linkage, tprv);
            }
            else
            {
                if (storage_class_in == StorageClass::member_ && asFriend)
                {
                    storage_class = StorageClass::external_;
                }
                else if (storage_class_in == StorageClass::member_ && storage_class == StorageClass::static_)
                {
                    storage_class = StorageClass::external_;
                }
                do
                {
                    if (isInline)
                        linkage = Linkage::inline_;
                    bool isTemplatedCast = false;
                    Type* tp1 = tp;
                    std::list<NAMESPACEVALUEDATA*>* oldGlobals = nullptr;
                    std::list<SYMBOL*> oldNameSpaceList;
                    bool promotedToTemplate = false;
                    if (!tp1)
                    {
                        // safety net
                        notype = true;
                        tp = tp1 = Type::MakeType(BasicType::int_);
                    }
                    sp = nullptr;
                    if (tp->type == BasicType::templateselector_)
                    {
                        // have to special case a templated cast operator
                        TEMPLATESELECTOR* l = &(*tp->sp->sb->templateSelector).back();
                        if (!strcmp(l->name, overloadNameTab[CI_CAST]))
                        {
                            strSym = (*tp->sp->sb->templateSelector)[1].sp;
                            while (!MATCHKW(lex, Keyword::operator_))
                                lex = lex->prev;
                            notype = true;
                            tp = tp1 = Type::MakeType(BasicType::int_);
                            isTemplatedCast = true;
                        }
                    }
                    lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, &asFriend);
                    tp1 = TypeGenerator::BeforeName(lex, funcsp, tp1, &sp, &strSym, &nsv, inTemplate, storage_class, &linkage,
                                                    &linkage2, &linkage3, &notype, asFriend, consdest, false, false);
                    if (linkage2 == Linkage::import_)
                        if (storage_class == StorageClass::global_)
                            storage_class = StorageClass::external_;
                    if (sp)
                    {
                        tp1 = AttributeFinish(sp, tp1);
                    }
                    inTemplateType = false;
                    if (tp1->IsFunction())
                        sizeQualifiers(tp1->BaseType()->btp);
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
                        oldNameSpaceList = nameSpaceList;
                        oldGlobals = globalNameSpace;
                        if (nsv->size() > 1)
                            nsv->front()->name->sb->value.i++;

                        auto list = Allocate<Optimizer::LIST>();
                        list->next = nullptr;
                        nameSpaceList.clear();
                        nameSpaceList.push_back(nsv->front()->name);
                        globalNameSpace = nsv;
                    }
                    if (strSym)
                    {
                        if (strSym->tp->type == BasicType::enum_)
                        {
                            error(ERR_CANNOT_DEFINE_ENUMERATION_CONSTANT_HERE);
                        }
                        else if (strSym->tp->type != BasicType::templateselector_ &&
                                 strSym->tp->type != BasicType::templatedecltype_)
                        {
                            enclosingDeclarations.Add(strSym);
                        }
                    }
                    if (Optimizer::cparams.prm_cplusplus && tp1->IsFunction() &&
                        (storage_class_in == StorageClass::member_ || storage_class_in == StorageClass::mutable_) && !asFriend)
                    {
                        if (MATCHKW(lex, Keyword::colon_) || MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::try_))
                        {
                            sp->sb->attribs.inheritable.isInline = true;
                        }
                    }
                    if (linkage == Linkage::inline_)
                    {
                        if (Optimizer::cparams.prm_cplusplus)
                        {
                            linkage4 = Linkage::virtual_;
                        }
                        if (sp)
                        {
                            sp->sb->declaredAsInline = true;
                            sp->sb->attribs.inheritable.isInline = true;
                        }
                    }
                    else if (!Optimizer::cparams.prm_profiler && Optimizer::cparams.prm_optimize_for_speed && tp1->IsFunction() &&
                             storage_class_in != StorageClass::member_ && storage_class_in != StorageClass::mutable_)
                    {
                        if (!strSym)
                        {
                            if (MATCHKW(lex, Keyword::colon_) || MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::try_))
                            {
                                if (strcmp(sp->name, "main") != 0 && strcmp(sp->name, "WinMain") != 0)
                                {
                                    sp->sb->attribs.inheritable.isInline = sp->sb->promotedToInline = true;
                                }
                            }
                        }
                    }
                    if (linkage == Linkage::none_ ||
                        ((defaultLinkage == Linkage::c_ || defaultLinkage == Linkage::cpp_) && linkage == Linkage::cdecl_))
                        linkage = defaultLinkage;
                    // defaultLinkage may also be Linkage::none_...
                    if (linkage == Linkage::none_)
                        linkage = Linkage::cdecl_;

                    if (asFriend && tp1->IsPtr())
                        error(ERR_POINTER_ARRAY_NOT_FRIENDS);

                    if (storage_class_in != StorageClass::parameter_ && tp1->type == BasicType::templateparam_ &&
                        tp1->templateParam->second->packed && tp1->templateParam->second->byPack.pack &&
                        tp1->templateParam->second->byPack.pack->size() > 1)
                        error(ERR_PACKED_TEMPLATE_PARAMETER_NOT_ALLOWED_HERE);
                    if (!sp)
                    {
                        if (!tp1->IsStructured() && tp1->type != BasicType::enum_)
                        {
                            error(ERR_IDENTIFIER_EXPECTED);
                        }
                        else
                        {
                            if (tp1->type == BasicType::enum_)
                            {
                                if (oldGlobals)
                                {
                                    if (nsv->size() > 1)
                                        nameSpaceList.front()->sb->value.i--;
                                    nameSpaceList = oldNameSpaceList;
                                    globalNameSpace = oldGlobals;
                                    oldGlobals = nullptr;
                                }
                                if (strSym && strSym->tp->type != BasicType::enum_ &&
                                    strSym->tp->type != BasicType::templateselector_ &&
                                    strSym->tp->type != BasicType::templatedecltype_)
                                {
                                    enclosingDeclarations.Drop();
                                }
                                break;
                            }
                        }
                        tp1 = tp1->BaseType();
                        if (Optimizer::cparams.prm_cplusplus && tp1->type == BasicType::union_ && tp1->sp->sb->anonymous &&
                            (storage_class_in == StorageClass::global_ || storage_class_in == StorageClass::auto_))
                        {
                            StorageClass sc = storage_class_in;
                            if (sc == StorageClass::auto_)
                                sc = storage_class;
                            else if (sc != StorageClass::member_ && sc != StorageClass::mutable_)
                                sc = StorageClass::static_;

                            int label = Optimizer::nextLabel++;
                            char buf[50];
                            sprintf(buf, "__anonymousUnion%d", label);
                            sp = makeID(sc, tp1, nullptr, litlate(buf));
                            sp->sb->label = label;
                            sp->sb->anonymousGlobalUnion = true;
                            sp->sb->anonymous = true;
                            sp->sb->access = access;
                            SetLinkerNames(sp, Linkage::c_);
                            sp->sb->parent = funcsp; /* function vars have a parent */
                            InsertSymbol(sp, sp->sb->storage_class, linkage, false);
                            if (sc != StorageClass::auto_ && sc != StorageClass::static_ && sc != StorageClass::localstatic_)
                            {
                                error(ERR_GLOBAL_ANONYMOUS_UNION_NOT_STATIC);
                            }
                            if (sc == StorageClass::auto_)
                                localAnonymousUnions[label] = sp;
                            resolveAnonymousGlobalUnion(sp);
                        }
                        else if ((storage_class_in == StorageClass::member_ || storage_class_in == StorageClass::mutable_) &&
                                 tp1->IsStructured() && tp1->sp->sb->anonymous)
                        {
                            sp = makeID(storage_class_in, tp1, nullptr, AnonymousName());
                            sp->sb->anonymous = true;
                            SetLinkerNames(sp, Linkage::c_);
                            sp->sb->parent = funcsp; /* function vars have a parent */
                            sp->sb->parentClass = enclosingDeclarations.GetFirst();
                            InsertSymbol(sp, sp->sb->storage_class, linkage, false);
                        }
                        else
                        {
                            if (asFriend)
                            {
                                SYMBOL* sym = nullptr;
                                auto lsit = enclosingDeclarations.begin();
                                auto lsite = enclosingDeclarations.end();
                                while (lsit != lsite && !(*lsit).str)
                                    ++lsit;
                                if (strSym && strSym->tp->type != BasicType::enum_ &&
                                    strSym->tp->type != BasicType::templateselector_ &&
                                    strSym->tp->type != BasicType::templatedecltype_)
                                {
                                    ++lsit;
                                    while (lsit != lsite && !(*lsite).str)
                                        ++lsit;
                                }
                                if (lsit != lsite)
                                    sym = (*lsit).str;
                                if (!sym->sb->friends)
                                    sym->sb->friends = symListFactory.CreateList();
                                sym->sb->friends->push_front(tp1->sp);
                                if (sym->sb->baseClasses)
                                {
                                    for (auto bc : *sym->sb->baseClasses)
                                    {
                                        if (!bc->cls->sb->friends)
                                            bc->cls->sb->friends = symListFactory.CreateList();
                                        bc->cls->sb->friends->push_front(tp1->sp);
                                    }
                                }
                            }
                            if (oldGlobals)
                            {
                                if (nsv->size() > 1)
                                    nameSpaceList.front()->sb->value.i--;
                                nameSpaceList = oldNameSpaceList;
                                globalNameSpace = oldGlobals;
                                oldGlobals = nullptr;
                            }
                            if (strSym && strSym->tp->type != BasicType::enum_ &&
                                strSym->tp->type != BasicType::templateselector_ &&
                                strSym->tp->type != BasicType::templatedecltype_)
                            {
                                enclosingDeclarations.Drop();
                            }
                            break;
                        }
                    }
                    else
                    {
                        SYMBOL* ssp = nullptr;
                        SYMBOL* spi;
                        bool checkReturn = true;
                        if (!definingTemplate && funcsp)
                            tp1 = ResolveTemplateSelectors(funcsp, tp1);
                        ssp = enclosingDeclarations.GetFirst();
                        if (!asFriend &&
                            (((storage_class_in == StorageClass::member_ || storage_class_in == StorageClass::mutable_) && ssp) ||
                             (inTemplate && strSym)))
                        {
                            sp->sb->parentClass = ssp;
                        }
                        if (!inTemplate)
                        {
                            if (sp->tp->BaseType() && sp->tp->IsFunction() && hasTemplateParent(sp))
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
                        else if (ssp && storage_class_in != StorageClass::member_ && storage_class_in != StorageClass::mutable_ &&
                                 !asFriend)
                        {
                            storage_class = StorageClass::member_;
                        }
                        if (Optimizer::cparams.prm_cplusplus && !ssp && storage_class == StorageClass::global_ &&
                            (tp1->IsStructured() || tp1->BaseType()->type == BasicType::enum_))
                            if (tp1->BaseType()->sp->sb->anonymous)
                                storage_class = StorageClass::static_;
                        if (consdest != CT_NONE)
                        {
                            if (Optimizer::cparams.prm_cplusplus)
                                ConsDestDeclarationErrors(sp, notype);
                            if (sp->tp->IsFunction())
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
                        else if (nameSpaceList.size() && storage_class_in != StorageClass::auto_)
                            sp->sb->parentNameSpace = nameSpaceList.front();
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
                        sp->sb->builtin_constexpression = builtin_constexpression;
                        sp->sb->access = access;
                        sp->sb->isExplicit = isExplicit;
                        sp->sb->storage_class = storage_class;
                        if (inTemplate && (!sp->templateParams || MATCHKW(lex, Keyword::colon_) || MATCHKW(lex, Keyword::begin_) ||
                                           MATCHKW(lex, Keyword::try_)))
                            sp->templateParams = TemplateGetParams(sp);
                        if (sp->sb->isDestructor && sp->sb->xcMode == xc_unspecified && !sp->sb->deferredNoexcept)
                        {
                            sp->sb->noExcept = true;
                            sp->sb->xcMode = xc_none;
                        }
                        if (!asFriend)
                            sp->tp = tp1;
                        if (!sp->sb->instantiated)
                        {
                            sp->sb->attribs.inheritable.linkage = linkage;
                            sp->sb->attribs.inheritable.linkage4 = linkage4;
                        }
                        if (ssp && ssp->sb->attribs.inheritable.linkage2 != Linkage::none_ &&
                            sp->sb->storage_class != StorageClass::localstatic_)
                        {
                            if (linkage2 != Linkage::none_ && linkage2 != ssp->sb->attribs.inheritable.linkage2 && !asFriend)
                                errorsym(ERR_DECLSPEC_MEMBER_OF_DECLSPEC_CLASS_NOT_ALLOWED, sp);
                            else if (!ssp->sb->templateLevel || !inTemplate)
                                sp->sb->attribs.inheritable.linkage2 = ssp->sb->attribs.inheritable.linkage2;
                        }
                        else
                        {
                            if (sp->sb->attribs.inheritable.linkage2 == Linkage::none_)
                                sp->sb->attribs.inheritable.linkage2 = linkage2;
                            else if (linkage2 != Linkage::none_)
                                error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                        }
                        if (sp->sb->attribs.inheritable.linkage3 == Linkage::none_)
                            sp->sb->attribs.inheritable.linkage3 = linkage3;
                        else if (linkage3 != Linkage::none_)
                            error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                        if (linkage2 == Linkage::import_)
                        {
                            sp->sb->importfile = importFile;
                        }
                        SetLinkerNames(sp,
                                       storage_class == StorageClass::auto_ && sp->tp->IsStructured() ? Linkage::auto_ : linkage,
                                       !!sp->templateParams);
                        if (inTemplate && templateDeclarationLevel == 1)
                        {
                            inTemplateBody++;
                        }
                        if (Optimizer::cparams.prm_cplusplus && sp->tp->IsFunction() && MATCHKW(lex, Keyword::colon_))
                        {
                            lex = getsym();
                            sp->sb->memberInitializers = GetMemberInitializers(&lex, funcsp, sp);
                        }
                        if (storage_class == StorageClass::absolute_)
                            sp->sb->value.i = address;
                        if ((!Optimizer::cparams.prm_cplusplus || !enclosingDeclarations.GetFirst()) && !istype(sp) &&
                            sp->sb->storage_class != StorageClass::static_ && tp1->BaseType()->IsFunction() &&
                            !MATCHKW(lex, Keyword::begin_) && !MATCHKW(lex, Keyword::try_))
                            sp->sb->storage_class = StorageClass::external_;
                        if (tp1->IsVoid())
                            if (sp->sb->storage_class != StorageClass::parameter_ &&
                                sp->sb->storage_class != StorageClass::typedef_)
                                error(ERR_VOID_NOT_ALLOWED);
                        if (sp->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
                        {
                            RequiresDialect::Keyword(Dialect::c11, "_Thread_local");
                            if (tp1->IsFunction())
                                error(ERR_FUNC_NOT_THREAD_LOCAL);
                            if (sp->sb->storage_class != StorageClass::external_ &&
                                sp->sb->storage_class != StorageClass::global_ && sp->sb->storage_class != StorageClass::static_)
                            {
                                if (sp->sb->storage_class == StorageClass::auto_ ||
                                    sp->sb->storage_class == StorageClass::register_)
                                    error(ERR_THREAD_LOCAL_NOT_AUTO);
                                else
                                    error(ERR_THREAD_LOCAL_INVALID_STORAGE_CLASS);
                            }
                        }
                        if (tp1->IsAtomic())
                        {
                            if (tp1->IsFunction() || (tp1->IsPtr() && tp1->BaseType()->array))
                                error(ERR_ATOMIC_NO_FUNCTION_OR_ARRAY);
                        }
                        if (sp->sb->attribs.inheritable.linkage2 == Linkage::property_ && tp1->IsFunction())
                            error(ERR_PROPERTY_QUALIFIER_NOT_ALLOWED_ON_FUNCTIONS);
                        if (storage_class != StorageClass::typedef_ && storage_class != StorageClass::catchvar_ &&
                            tp1->IsStructured() && tp1->BaseType()->sp->sb->isabstract)
                            errorabstract(ERR_CANNOT_CREATE_INSTANCE_ABSTRACT, tp1->BaseType()->sp);
                        if (sp->packed)
                            error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                        if (storage_class == StorageClass::mutable_ && tp->IsConst())
                            errorsym(ERR_MUTABLE_NON_CONST, sp);
                        // correct for previous errors
                        if (sp->sb->isConstructor && sp->tp->IsPtr())
                            sp->tp = sp->tp->BaseType()->btp;

                        if (ssp && strSym && strSym->tp->type != BasicType::templateselector_ &&
                            strSym->tp->type != BasicType::templatedecltype_)
                        {
                            if (strSym != ssp && strSym->sb->mainsym != ssp)
                            {
                                errorsym(ERR_IDENTIFIER_CANNOT_HAVE_TYPE_QUALIFIER, sp);
                            }
                        }
                        spi = nullptr;
                        if (nsv && !strSym)
                        {
                            unvisitUsingDirectives(nsv->front());
                            auto rvl = tablesearchinline(sp->name, nsv->front(), false);
                            if (rvl.size() != 0)
                                if (rvl.size() > 1)
                                {
                                    auto it = rvl.begin();
                                    ++it;
                                    errorsym2(ERR_AMBIGUITY_BETWEEN, rvl.front(), *it);
                                }
                                else
                                    spi = rvl.front();
                            else
                                errorNotMember(strSym, nsv->front(), sp->sb->decoratedName);
                        }
                        else
                        {
                            if (sp->tp->IsFunction() && sp->sb->instantiated)
                            {
                                // may happen if a template function was declared at a lower level
                                spi = sp;
                            }
                            else
                            {
                                if ((storage_class_in == StorageClass::auto_ || storage_class_in == StorageClass::parameter_) &&
                                    storage_class != StorageClass::external_ && !sp->tp->IsFunction())
                                {
                                    spi = localNameSpace->front()->syms->Lookup(sp->name);
                                }
                                else
                                {
                                    ssp = enclosingDeclarations.GetFirst();
                                    if (ssp && ssp->sb->templateLevel)
                                    {
                                        SYMBOL* ssp2 = FindSpecialization(ssp, ssp->templateParams);
                                        if (ssp2)
                                            ssp = ssp2;
                                    }
                                    if (ssp && ssp->tp->syms && (strSym || !asFriend))
                                    {
                                        spi = ssp->tp->syms->Lookup(sp->name);
                                    }
                                    else
                                    {
                                        spi = globalNameSpace->front()->syms->Lookup(sp->name);
                                    }
                                }
                            }
                        }
                        if (spi && spi->sb->storage_class == StorageClass::overloads_)
                        {
                            SYMBOL* sym = nullptr;
                            if (sp->tp->IsFunction())
                            {
                                for (auto sp : *spi->tp->syms)
                                {
                                    if (!sp->sb->instantiated)
                                        ScrubTemplateValues(sp);
                                }
                                if (!sp->sb->parentClass || !sp->sb->parentClass->sb->declaring)
                                {
                                    if (definingTemplate == 1 && sp->templateParams && sp->templateParams->size() == 1 &&
                                        !sp->templateParams->front().second->bySpecialization.types)
                                        instantiatingTemplate++;
                                    InitializeFunctionArguments(sp);
                                    sym = searchOverloads(sp, spi->tp->syms);
                                    if (definingTemplate == 1 && sp->templateParams && sp->templateParams->size() == 1 &&
                                        !sp->templateParams->front().second->bySpecialization.types)
                                        instantiatingTemplate--;
                                    Type* retVal;
                                    TEMPLATESELECTOR* tsl = nullptr;
                                    if (!sym && storage_class_in != StorageClass::member_ &&
                                        ((retVal = sp->tp->BaseType()->btp)->type == BasicType::templateselector_))
                                    {
                                        auto tsl = &(*retVal->sp->sb->templateSelector)[1];
                                        if (tsl->isTemplate)
                                        {
                                            auto sp1 = GetClassTemplate(tsl->sp, tsl->templateParams, true);
                                            if (sp1)
                                            {
                                                auto save = tsl->sp;
                                                tsl->sp = sp1;
                                                sym = searchOverloads(sp, spi->tp->syms);
                                                tsl->sp = save;
                                            }
                                        }
                                    }
                                }

                                if (sp->sb->attribs.inheritable.linkage == Linkage::c_ ||
                                    (sym && sym->sb->attribs.inheritable.linkage == Linkage::c_))
                                    if (!sym || !sameNameSpace(sp->sb->parentNameSpace, sym->sb->parentNameSpace))
                                        preverrorsym(ERR_CONFLICTS_WITH, sp, spi->sb->declfile, spi->sb->declline);
                                if (sym && sym->templateParams && (!sp->templateParams || sp->templateParams->size() > 1) &&
                                    (!exactMatchOnTemplateParams(sym->templateParams, sp->templateParams) ||
                                     ([](std::list<TEMPLATEPARAMPAIR>* a, std::list<TEMPLATEPARAMPAIR>* b) {
                                         if (a && b)
                                         {
                                             auto ita = a->begin();
                                             auto itae = a->end();
                                             auto itb = b->begin();
                                             auto itbe = b->end();
                                             for (; ita != itae && itb != itbe; ++ita, ++itb)
                                             {
                                                 if (ita->second->byClass.txtdflt && itb->second->byClass.txtdflt)
                                                     return true;
                                             }
                                         }
                                         return false;
                                     })(sym->templateParams, sp->templateParams)))
                                    sym = nullptr;
                            }
                            if (sym && Optimizer::cparams.prm_cplusplus)
                            {
                                if (sym->sb->attribs.inheritable.linkage == Linkage::c_ &&
                                    sp->sb->attribs.inheritable.linkage == Linkage::cdecl_)
                                    sp->sb->attribs.inheritable.linkage = Linkage::c_;
                                if (sp->sb->attribs.inheritable.linkage != sym->sb->attribs.inheritable.linkage &&
                                    ((sp->sb->attribs.inheritable.linkage != Linkage::cdecl_ &&
                                      sp->sb->attribs.inheritable.linkage4 != Linkage::virtual_) ||
                                     (sym->sb->attribs.inheritable.linkage != Linkage::cdecl_ &&
                                      sym->sb->attribs.inheritable.linkage4 != Linkage::virtual_)) &&
                                    !sp->sb->attribs.inheritable.isInline && !sym->sb->attribs.inheritable.isInline)
                                {
                                    preverrorsym(ERR_LINKAGE_MISMATCH_IN_FUNC_OVERLOAD, spi, spi->sb->declfile, spi->sb->declline);
                                }
                                else if (sym && !sym->sb->isConstructor && !sym->sb->isDestructor &&
                                         !sp->tp->BaseType()->btp->CompatibleType((sym)->tp->BaseType()->btp))
                                {
                                    if (Optimizer::cparams.prm_cplusplus && sym->tp->IsFunction() &&
                                        (sym->sb->templateLevel || definingTemplate))
                                        checkReturn = false;
                                }
                            }
                            // this next will find specializations that differ only in return type
                            if (inTemplate)
                                if (!sym)
                                {
                                    // a specialization
                                    // this may result in returning sp depending on what happens...
                                    if (sp->templateParams->front().second->bySpecialization.types)
                                    {
                                        bool found = false;
                                        for (auto sym1 : *spi->tp->syms)
                                        {
                                            if (sym1->sb->templateLevel)
                                            {
                                                auto one = sp->templateParams->front().second->bySpecialization.types->begin();
                                                auto onee = sp->templateParams->front().second->bySpecialization.types->end();
                                                auto two = sym1->templateParams->begin();
                                                ++two;
                                                auto twoe = sym1->templateParams->end();
                                                for (; one != onee && two != twoe; ++one, ++two)
                                                {
                                                    if (one->second->type != two->second->type)
                                                    {
                                                        found = true;
                                                        break;
                                                    }
                                                }
                                                if (one == onee && two == twoe)
                                                {
                                                    found = true;
                                                    break;
                                                }
                                            }
                                        }
                                        if (!found)
                                            errorsym(ERR_SPECIALIZATION_REQUIRES_PRIMARY, sp);
                                        sp->sb->specialized = true;
                                    }
                                    else if (sp->templateParams && sp->templateParams->size() == 1)
                                        sp->sb->specialized = true;
                                    sym = LookupFunctionSpecialization(spi, sp);
                                    if (sym == sp && spi->sb->storage_class != StorageClass::overloads_)
                                    {
                                        sym = spi;
                                    }
                                    else if (sym)
                                    {
                                        InsertSymbol(sym,
                                                     storage_class == StorageClass::typedef_ ? storage_class_in : storage_class,
                                                     linkage, false);
                                    }
                                    spi = nullptr;
                                }
                            if (sym && sym->sb->templateLevel != sp->sb->templateLevel &&
                                (sym->sb->templateLevel || !sp->sb->templateLevel || sp->templateParams->size() > 1) &&
                                (!strSym || !strSym->sb->templateLevel || sym->sb->templateLevel != sp->sb->templateLevel + 1))
                                sym = nullptr;
                            if (sym)
                            {
                                spi = sym;
                                if (Optimizer::cparams.prm_cplusplus)
                                {
                                    auto it1 = spi->tp->BaseType()->syms->begin();
                                    auto it1end = spi->tp->BaseType()->syms->end();
                                    auto it2 = sp->tp->BaseType()->syms->begin();
                                    auto it2end = sp->tp->BaseType()->syms->end();
                                    if (it1 != it1end && it2 != it2end)
                                    {
                                        if ((*it1)->sb->thisPtr)
                                            ++it1;
                                        if ((*it1)->tp->type == BasicType::void_)
                                            ++it1;
                                        if ((*it2)->sb->thisPtr)
                                            ++it2;
                                        if ((*it2)->tp->type == BasicType::void_)
                                            ++it2;
                                        while (it1 != it1end && it2 != it2end)
                                        {
                                            bool b = (*it1)->sb->defaultarg || (*it2)->sb->defaultarg;
                                            (*it1)->sb->defaultarg = b;
                                            (*it2)->sb->defaultarg = b;
                                            ++it1;
                                            ++it2;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if ((nsv || strSym) && storage_class_in != StorageClass::member_ &&
                                    storage_class_in != StorageClass::mutable_ && (!inTemplate || !sp->templateParams))
                                {
                                    errorNotMember(strSym, nsv ? nsv->front() : nullptr, sp->sb->decoratedName);
                                }
                                spi = nullptr;
                            }
                        }
                        else if (!spi)
                        {
                            if (sp->templateParams && sp->templateParams->front().second->bySpecialization.types)
                                errorsym(ERR_SPECIALIZATION_REQUIRES_PRIMARY, sp);
                            if (strSym && storage_class_in != StorageClass::member_ && storage_class_in != StorageClass::mutable_)
                            {
                                errorNotMember(strSym, nsv ? nsv->front() : nullptr, sp->sb->decoratedName);
                            }
                        }
                        else
                        {
                            if (sp->sb->storage_class == StorageClass::member_ && spi->sb->storage_class == StorageClass::external_)
                                sp->sb->storage_class = StorageClass::global_;
                        }
                        if ((!spi || (spi->sb->storage_class != StorageClass::member_ &&
                                      spi->sb->storage_class != StorageClass::mutable_)) &&
                            sp->sb->storage_class == StorageClass::global_ && sp->sb->attribs.inheritable.isInline &&
                            !sp->sb->promotedToInline)
                        {
                            if (!spi || spi->sb->storage_class != StorageClass::external_)
                                sp->sb->storage_class = StorageClass::static_;
                        }
                        if (spi && !sp->sb->parentClass && !spi->tp->IsFunction() &&
                            spi->sb->storage_class != StorageClass::type_ && sp->sb->templateLevel)
                        {
                            SYMBOL* special = FindSpecialization(spi, sp->templateParams);
                            if (!special)
                            {
                                if (!spi->sb->specializations)
                                    spi->sb->specializations = symListFactory.CreateList();
                                spi->sb->specializations->push_front(sp);
                            }
                        }
                        else if (spi)
                        {
                            if (inTemplate)
                            {
                                if (spi != sp)
                                {
                                    std::list<TEMPLATEPARAMPAIR>* templateParams = TemplateGetParams(sp);
                                    if (templateParams->front().second->bySpecialization.types && spi->sb->parentTemplate)
                                    {
                                        spi->templateParams =
                                            TemplateMatching(lex, spi->sb->parentTemplate->templateParams, templateParams, spi,
                                                             MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::try_));
                                    }
                                    else
                                    {
                                        if (!asFriend || MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::try_))
                                        {
                                            auto temp =
                                                TemplateMatching(lex, spi->templateParams, templateParams, spi,
                                                                 MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::try_));
                                            spi->templateParams = temp;
                                        }
                                        else
                                        {
                                            auto temp = TemplateMatching(lex, spi->templateParams, templateParams, spi, true);
                                        }
                                    }
                                }
                            }
                            else if (spi->sb->templateLevel && !spi->sb->instantiated && !definingTemplate)
                            {
                                if ((strSym && !strSym->sb->templateLevel) ||
                                    spi->sb->templateLevel != sp->sb->templateLevel + (strSym != 0))
                                    errorsym(ERR_IS_ALREADY_DEFINED_AS_A_TEMPLATE, sp);
                            }
                            if (spi && spi->sb->parentClass && storage_class_in == StorageClass::member_)
                            {
                                errorsym(ERR_CLASS_MEMBER_ALREADY_DECLARED, spi);
                            }
                            if (spi->tp->IsFunction())
                            {
                                matchFunctionDeclaration(lex, sp, spi, checkReturn, asFriend);
                            }
                            if (sp->sb->parentClass)
                            {
                                if (spi->sb->storage_class != StorageClass::member_ &&
                                    sp->sb->storage_class != StorageClass::member_ &&
                                    spi->sb->storage_class != StorageClass::mutable_ &&
                                    sp->sb->storage_class != StorageClass::mutable_)
                                    if (spi->sb->storage_class != StorageClass::external_ &&
                                        sp->sb->storage_class != StorageClass::external_)
                                        preverrorsym(ERR_DUPLICATE_IDENTIFIER, spi, spi->sb->declfile, spi->sb->declline);
                            }
                            else
                            {
                                sp->sb->parentClass = strSym;
                                if (!sp->sb->parentClass && spi->sb->parentClass)  // error handling
                                    sp->sb->parentClass = spi->sb->parentClass;
                                if (sp->sb->parentClass)
                                    SetLinkerNames(sp,
                                                   storage_class == StorageClass::auto_ && sp->tp->IsStructured() ? Linkage::auto_
                                                                                                                  : linkage,
                                                   !!sp->templateParams);
                            }
                            if (sp->sb->constexpression)
                                spi->sb->constexpression = true;
                            if (istype(spi))
                            {
                                if (Optimizer::cparams.prm_ansi || !sp->tp->CompatibleType((spi)->tp) || !istype(sp))
                                    preverrorsym(ERR_REDEFINITION_OF_TYPE, sp, spi->sb->declfile, spi->sb->declline);
                            }
                            else
                            {
                                if (!sp->tp->IsFunction() && !spi->tp->IsFunction())
                                {
                                    sp->tp->InstantiateDeferred();
                                    spi->tp->InstantiateDeferred();
                                }
                                if (spi->sb->pushedTemplateSpecializationDefinition &&
                                    (MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::try_)))
                                {
                                    spi->sb->pushedTemplateSpecializationDefinition = false;
                                    spi->sb->inlineFunc.stmt = nullptr;
                                    spi->sb->deferredCompile = nullptr;
                                }
                                if ((spi->tp->IsFunction() && (spi->sb->inlineFunc.stmt || spi->sb->deferredCompile) &&
                                     (MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::try_)) &&
                                     (!spi->sb->parentClass || !spi->sb->parentClass->sb->instantiated ||
                                      !spi->sb->copiedTemplateFunction)) &&
                                    spi->sb->parentClass &&
                                    !differentTemplateNames(spi->sb->parentClass->templateParams,
                                                            sp->sb->parentClass->templateParams))
                                {
                                    errorsym(ERR_BODY_ALREADY_DEFINED_FOR_FUNCTION, sp);
                                }
                                else if ((!sp->tp->IsFunction() && !spi->tp->IsFunction() && !sp->tp->CompatibleType((spi)->tp)) ||
                                         istype(sp))
                                {
                                    preverrorsym(ERR_TYPE_MISMATCH_IN_REDECLARATION, sp, spi->sb->declfile, spi->sb->declline);
                                }
                                else if (!sameQuals(sp, spi) && (!definingTemplate || instantiatingTemplate))
                                {
                                    errorsym(ERR_DECLARATION_DIFFERENT_QUALIFIERS, sp);
                                }
                                if (sp->sb->attribs.inheritable.linkage3 != spi->sb->attribs.inheritable.linkage3 &&
                                    (sp->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_ ||
                                     spi->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_))
                                    if (!spi->sb->parentClass)
                                        errorsym(ERR_THREAD_LOCAL_MUST_ALWAYS_APPEAR, sp);
                                if (strSym && storage_class == StorageClass::static_)
                                {
                                    errorstr(ERR_INVALID_STORAGE_CLASS, "static");
                                }
                                if (ismember(spi))
                                {
                                    if (!asFriend && !sp->tp->IsFunction() && sp->sb->storage_class != spi->sb->storage_class &&
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
                                    case StorageClass::auto_:
                                    case StorageClass::register_:
                                    case StorageClass::parameter_:
                                        preverrorsym(ERR_DUPLICATE_IDENTIFIER, sp, spi->sb->declfile, spi->sb->declline);
                                        break;
                                    case StorageClass::external_:
                                        if ((spi)->sb->storage_class == StorageClass::static_)
                                        {
                                            if (!spi->tp->IsFunction())
                                            {
                                                if (spi->sb->constexpression)
                                                {
                                                    spi->sb->storage_class = StorageClass::global_;
                                                }
                                                else
                                                {
                                                    errorsym(ERR_ANSI_FORBID_BOTH_EXTERN_STATIC, spi);
                                                    spi->sb->storage_class = StorageClass::global_;
                                                }
                                            }
                                            else if (spi->sb->attribs.inheritable.isInline &&
                                                     spi->tp->BaseType()->type == BasicType::ifunc_)
                                            {
                                                spi->sb->storage_class = StorageClass::global_;
                                            }
                                        }
                                        break;
                                    case StorageClass::global_:
                                        if (spi->sb->storage_class != StorageClass::static_)
                                        {
                                            spi->sb->declfile = sp->sb->declfile;
                                            spi->sb->declline = sp->sb->declline;
                                            spi->sb->realdeclline = sp->sb->realdeclline;
                                            spi->sb->declfilenum = sp->sb->declfilenum;
                                            sp->sb->wasExternal = spi->sb->wasExternal =
                                                spi->sb->storage_class == StorageClass::external_;
                                            spi->sb->storage_class = StorageClass::global_;
                                            spi->sb->declaredAsInline |= sp->sb->declaredAsInline;
                                        }
                                        break;
                                    case StorageClass::mutable_:
                                        if ((spi)->sb->storage_class != StorageClass::mutable_)
                                        {
                                            preverrorsym(ERR_DUPLICATE_IDENTIFIER, sp, spi->sb->declfile, spi->sb->declline);
                                        }
                                        break;
                                    case StorageClass::member_:
                                        if ((spi)->sb->storage_class == StorageClass::static_)
                                        {
                                            preverrorsym(ERR_DUPLICATE_IDENTIFIER, sp, spi->sb->declfile, spi->sb->declline);
                                        }
                                        break;
                                    case StorageClass::static_:
                                        if ((spi)->sb->storage_class == StorageClass::external_ && !spi->tp->IsFunction() &&
                                            !spi->sb->constexpression)
                                        {
                                            errorsym(ERR_ANSI_FORBID_BOTH_EXTERN_STATIC, spi);
                                            spi->sb->storage_class = StorageClass::global_;
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
                                    case StorageClass::localstatic_:
                                    case StorageClass::constexpr_:
                                        spi->sb->storage_class = sp->sb->storage_class;
                                        break;
                                }
                            }
                            if (spi->tp->IsFunction())
                            {
                                std::list<TEMPLATEPARAMPAIR>* nw = sp->templateParams;
                                if (sp->templateParams)
                                {
                                    for (auto it = sp->templateParams->begin(); it != sp->templateParams->end();)
                                    {
                                        auto nw = *it;
                                        if (nw.first)
                                        {
                                            if (spi->templateParams)
                                            {
                                                bool found = false;
                                                for (auto old : *spi->templateParams)
                                                {
                                                    if (old.first)
                                                    {
                                                        if (!strcmp(nw.first->name, old.first->name))
                                                        {
                                                            found = true;
                                                            break;
                                                        }
                                                    }
                                                }
                                                if (!found)
                                                {
                                                    it = sp->templateParams->erase(it);
                                                }
                                                else
                                                {
                                                    ++it;
                                                }
                                            }
                                            else
                                            {
                                                it = sp->templateParams->erase(it);
                                            }
                                        }
                                        else
                                        {
                                            ++it;
                                        }
                                    }
                                }
                                if (Optimizer::cparams.prm_cplusplus &&
                                    (MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::try_)))
                                {
                                    spi->templateParams = sp->templateParams;
                                }
                                if (!asFriend || MATCHKW(lex, Keyword::try_) || MATCHKW(lex, Keyword::begin_))
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
                                spi->tp->type == BasicType::func_)  // && !spi->sb->parentClass && !spi->sb->specialized)
                            {
                                spi->sb->declfile = sp->sb->declfile;
                                spi->sb->declline = sp->sb->declline;
                                spi->sb->realdeclline = sp->sb->realdeclline;
                                spi->sb->declfilenum = sp->sb->declfilenum;
                            }
                            spi->sb->memberInitializers = sp->sb->memberInitializers;

                            spi->sb->attribs.inheritable.isInline |= sp->sb->attribs.inheritable.isInline;
                            spi->sb->promotedToInline |= sp->sb->promotedToInline;
                            spi->sb->declaredAsInline |= sp->sb->declaredAsInline;

                            spi->sb->parentClass = sp->sb->parentClass;
                            spi->sb->externShim = false;
                            sp->sb->mainsym = spi;

                            sp = spi;
                            sp->sb->redeclared = true;
                        }
                        else
                        {
                            if (inTemplate)
                            {
                                std::list<TEMPLATEPARAMPAIR>* templateParams = TemplateGetParams(sp);
                                if (sp->tp->IsFunction() && templateParams->front().second->bySpecialization.types &&
                                    templateParams->size() > 1)
                                {
                                    error(ERR_FUNCTION_TEMPLATE_NO_PARTIAL_SPECIALIZE);
                                }
                                else if (!tp->IsFunction() && templateParams->size() > 1 &&
                                         templateParams->front().second->bySpecialization.types)
                                {
                                    TemplateValidateSpecialization(templateParams);
                                }
                                else if (definingTemplate == 1)
                                {
                                    TemplateMatching(lex, nullptr, templateParams, sp,
                                                     MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::try_));
                                }
                            }
                            if (sp->tp->IsFunction())
                            {
                                matchFunctionDeclaration(lex, sp, sp, false, asFriend);
                                if (inTemplate)
                                    sp->sb->parentTemplate = sp;
                            }
                            else if (sp->sb->storage_class != StorageClass::type_)
                            {
                                if (inTemplate)
                                    sp->sb->parentTemplate = sp;
                            }
                            if (!asFriend || sp->tp->IsFunction())
                            {
                                if (sp->sb->constexpression && sp->sb->storage_class == StorageClass::global_)
                                    sp->sb->storage_class = StorageClass::static_;
                                if (!asFriend || !definingTemplate || instantiatingTemplate || inTemplate)
                                {
                                    if (sp->sb->storage_class == StorageClass::external_ ||
                                        (asFriend && !MATCHKW(lex, Keyword::begin_) && !MATCHKW(lex, Keyword::try_)))
                                    {
                                        InsertSymbol(sp, StorageClass::external_, linkage, false);
                                    }
                                    else
                                    {
                                        InsertSymbol(sp, storage_class == StorageClass::typedef_ ? storage_class_in : storage_class,
                                                     linkage, false);
                                    }
                                }
                            }
                            if (asFriend && !sp->sb->anonymous && !sp->tp->IsFunction() && !definingTemplate)
                            {
                                error(ERR_DECLARATOR_NOT_ALLOWED_HERE);
                            }
                        }

                        if (asFriend)
                        {
                            if (sp->tp->IsFunction())
                            {
                                SYMBOL* sym = nullptr;
                                auto lsit = enclosingDeclarations.begin();
                                auto lsite = enclosingDeclarations.end();
                                while (lsit != lsite && !(*lsit).str)
                                    ++lsit;
                                if (strSym && strSym->tp->type != BasicType::enum_ &&
                                    strSym->tp->type != BasicType::templateselector_ &&
                                    strSym->tp->type != BasicType::templatedecltype_)
                                {
                                    ++lsit;
                                    while (lsit != lsite && !(*lsit).str)
                                        ++lsit;
                                }
                                if (lsit != lsite)
                                    sym = (*lsit).str;
                                if (!sym->sb->friends)
                                    sym->sb->friends = symListFactory.CreateList();
                                sym->sb->friends->push_front(sp);
                            }
                        }
                        if (!ismember(sp) && !sp->sb->constexpression && !istype(sp))
                        {
                            if (sp->tp->IsFunction() && (sp->tp->IsConst() || sp->tp->IsVolatile()))
                                error(ERR_ONLY_MEMBER_CONST_VOLATILE);
                        }
                        if (inTemplate && templateDeclarationLevel == 1)
                        {
                            inTemplateBody--;
                        }
                    }
                    if (sp)
                    {
                        if (!strcmp(sp->name, overloadNameTab[CI_DESTRUCTOR]))
                        {
                            if (!tp1->IsFunction())
                            {
                                errorsym(ERR_CONSTRUCTOR_OR_DESTRUCTOR_MUST_BE_FUNCTION, sp->sb->parentClass);
                            }
                            else
                            {
                                auto s = tp1->BaseType()->syms->begin();
                                if ((*s)->sb->thisPtr)
                                    ++s;
                                if ((*s)->tp->type != BasicType::void_)
                                    errorsym(ERR_DESTRUCTOR_CANNOT_HAVE_PARAMETERS, sp->sb->parentClass);
                            }
                        }
                        else if (!strcmp(sp->name, overloadNameTab[CI_CONSTRUCTOR]))
                        {
                            if (!tp1->IsFunction())
                                errorsym(ERR_CONSTRUCTOR_OR_DESTRUCTOR_MUST_BE_FUNCTION, sp->sb->parentClass);
                        }
                    }
                    if (!templateArg)
                    {
                        checkDeclarationAccessible(sp, sp->sb->parentClass, sp->tp->IsFunction() ? sp : nullptr);
                    }
                    if (sp->sb->operatorId)
                        checkOperatorArgs(sp, asFriend);
                    if (sp->sb->storage_class == StorageClass::typedef_)
                    {
                        Type** tn = &sp->tp;
                        // all this is so we can have multiple typedefs referring to the same thing...
                        if (!Optimizer::cparams.prm_cplusplus)
                        {
                            if ((*tn)->type == BasicType::typedef_)
                                while (*tn != (*tn)->BaseType() && (*tn)->type != BasicType::va_list_)
                                    tn = &(*tn)->btp;
                            // this next is a little buggy as if there are multiple typedefs for a struct
                            // _Generic won't handle them right.   This is a rare case though and it is for the moment
                            // expedient to do this...
                            if ((*tn)->type != BasicType::struct_ && (*tn)->type != BasicType::union_)
                                *tn = (*tn)->CopyType();
                        }
                        sp->tp = Type::MakeType(BasicType::typedef_, sp->tp);
                        tp->UpdateRootTypes();
                        sp->tp->sp = sp;
                        if (!Optimizer::cparams.prm_cplusplus)
                        {
                            sp->tp->typedefType = sp->tp;
                            if (tn == &sp->tp)
                                sp->tp->btp->typedefType = sp->tp;
                            else
                                (*tn)->typedefType = sp->tp;
                        }
                    }
                    if (sp->tp->IsPtr() && sp->sb->storage_class != StorageClass::parameter_)
                    {
                        btp = sp->tp->BaseType();
                        if (btp->vla)
                        {
                            allocateVLA(lex, sp, funcsp, block, btp, false);
                        }
                        else
                        {
                            btp = btp->btp->BaseType();
                            if (btp->vla)
                            {
                                allocateVLA(lex, sp, funcsp, block, btp, true);
                                btp->size = sp->tp->BaseType()->size;
                            }
                        }
                    }
                    tp1->UpdateRootTypes();
                    sizeQualifiers(tp1);
                    if (sp->sb->attribs.inheritable.isInline)
                    {
                        if (!sp->tp->IsFunction() &&
                            ((sp->sb->storage_class != StorageClass::global_ && sp->sb->storage_class != StorageClass::static_) ||
                             sp->sb->parentClass))
                            error(ERR_INLINE_NOT_ALLOWED);
                    }
                    if (inTemplate && templateDeclarationLevel == 1)
                    {
                        inTemplateBody++;
                    }

                    if (sp->sb->constexpression)
                    {
                        if (ismemberdata(sp))
                            error(ERR_CONSTEXPR_MEMBER_MUST_BE_STATIC);
                        else if (Optimizer::cparams.c_dialect >= Dialect::c2x && sp->tp->IsFunction())
                            error(ERR_CONSTEXPR_MUST_BE_OBJECT);
                        else
                            CheckIsLiteralClass(sp->tp);
                    }
                    if (lex)
                    {
                        if (linkage != Linkage::cdecl_)
                            sp->sb->attribs.inheritable.linkage = linkage;
                        if (linkage2 != Linkage::none_)
                            sp->sb->attribs.inheritable.linkage2 = linkage2;
                        if (linkage4 != Linkage::none_)
                            sp->sb->attribs.inheritable.linkage4 = linkage4;
                        if (linkage2 == Linkage::import_)
                        {
                            sp->sb->importfile = importFile;
                        }
                        if (sp->tp->BaseType()->type == BasicType::func_)
                        {
                            if (sp->tp->BaseType()->btp->IsAutoType())
                                RequiresDialect::Feature(Dialect::cpp14, "Return value deduction");
                            if (sp->sb->hasBody && !instantiatingFunction && !instantiatingTemplate &&
                                (MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::try_)))
                                errorsym(ERR_FUNCTION_HAS_BODY, sp);
                            if (funcsp && storage_class == StorageClass::localstatic_)
                                errorstr(ERR_INVALID_STORAGE_CLASS, "static");
                            if (storage_class == StorageClass::member_ && !Optimizer::cparams.prm_cplusplus)
                                error(ERR_FUNCTION_NOT_IN_STRUCT);
                            if (sp->tp->BaseType()->btp->IsStructured() ||
                                sp->tp->BaseType()->btp->BaseType()->type == BasicType::enum_)
                                if (defd)
                                    errorsym(ERR_TYPE_DEFINITION_NOT_ALLOWED_HERE, sp->tp->btp->BaseType()->sp);
                            if (sp->sb->castoperator)
                            {
                                if (!notype)
                                    errortype(ERR_TYPE_OF_RETURN_IS_IMPLICIT_FOR_OPERATOR_FUNC, sp->tp->BaseType()->btp, nullptr);
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
                                    injectThisPtr(sp, sp->tp->BaseType()->syms);
                                }
                            }
                            if (sp->sb->storage_class == StorageClass::static_ && (Optimizer::architecture == ARCHITECTURE_MSIL))
                            {
                                if (!sp->sb->label)
                                    sp->sb->label = Optimizer::nextLabel++;
                                sp->sb->uniqueID = fileIndex;
                            }

                            if (nameSpaceList.size())
                                SetTemplateNamespace(sp);
                            if (storage_class == StorageClass::external_)
                                sp->sb->declaredAsExtern = true;
                            if (MATCHKW(lex, Keyword::begin_) || MATCHKW(lex, Keyword::try_))
                            {
                                if (asFriend)
                                    sp->sb->friendContext = enclosingDeclarations.GetFirst();
                                if (!definingTemplate)
                                    sp->sb->hasBody = true;
                                Type* tp = sp->tp;
                                if (sp->sb->storage_class == StorageClass::member_ && storage_class_in == StorageClass::member_)
                                    browse_variable(sp);
                                if (sp->sb->storage_class == StorageClass::external_)
                                {
                                    sp->sb->storage_class = StorageClass::global_;
                                }
                                while (tp->type != BasicType::func_)
                                    tp = tp->btp;
                                tp->type = BasicType::ifunc_;

                                for (auto sym : *tp->syms)
                                {
                                    Type* tpl = sym->tp;
                                    while (tpl)
                                    {
                                        if (tpl->unsized)
                                        {
                                            specerror(ERR_UNSIZED_VLA_PARAMETER, sym->name, sym->sb->declfile, sym->sb->declline);
                                            break;  // hmmmmm
                                        }
                                        tpl = tpl->btp;
                                    }
                                }
                                if (storage_class_in == StorageClass::member_ || storage_class_in == StorageClass::mutable_)
                                {
                                    sp->sb->instantiatedInlineInClass = true;
                                }
                                if (storage_class_in != StorageClass::member_ && TemplateFullySpecialized(sp->sb->parentClass))
                                {
                                    sp->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                                    if (sp->sb->constexpression && sp->sb->isConstructor)
                                        ConstexprMembersNotInitializedErrors(sp);
                                    auto startStmt = currentLineData(emptyBlockdata, lex, 0);
                                    if (startStmt)
                                        sp->sb->linedata = startStmt->front()->lineData;
                                    lex = getDeferredData(lex, &sp->sb->deferredCompile, true);
                                    Optimizer::SymbolManager::Get(sp);
                                    sp->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                                    if (!sp->sb->attribs.inheritable.isInline)
                                        InsertInline(sp);
                                }
                                else
                                {
                                    if (Optimizer::cparams.prm_cplusplus && storage_class_in != StorageClass::member_ &&
                                        sp->sb->attribs.inheritable.linkage4 != Linkage::virtual_ &&
                                        sp->sb->attribs.inheritable.linkage != Linkage::c_)
                                    {
                                        if ((!sp->sb->parentClass || !sp->sb->parentClass->templateParams || !definingTemplate ||
                                             (sp->templateParams && sp->templateParams->size() == 1)) &&
                                            strcmp(sp->name, "main") != 0 && strcmp(sp->name, "WinMain") != 0)
                                        {
                                            if (!sp->sb->parentNameSpace ||
                                                (sp->sb->specialized && sp->templateParams->size() == 1) || sp->sb->isDestructor)
                                                sp->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                                            if ((!sp->sb->parentNameSpace && (!definingTemplate || instantiatingTemplate)) ||
                                                (sp->sb->specialized && sp->templateParams->size() == 1) || sp->sb->isDestructor)
                                                InsertInline(sp);
                                        }
                                    }
                                    if (storage_class_in == StorageClass::member_ || storage_class_in == StorageClass::mutable_ ||
                                        definingTemplate == 1 || (asFriend && definingTemplate == 2))
                                    {
                                        auto startStmt = currentLineData(emptyBlockdata, lex, 0);
                                        if (startStmt)
                                            sp->sb->linedata = startStmt->front()->lineData;
                                        lex = getDeferredData(lex, &sp->sb->deferredCompile, true);
                                        Optimizer::SymbolManager::Get(sp);
                                        if (asFriend)
                                            sp->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                                        if (sp->sb->parentClass && sp->templateParams &&
                                            (!definingTemplate || instantiatingTemplate))
                                        {
                                            sp->sb->templateLevel = 0;
                                            sp->tp = SynthesizeType(sp->tp, sp->sb->parentClass->templateParams, false);
                                            sp = TemplateFunctionInstantiate(sp, false);
                                            sp->sb->specialized2 = true;
                                        }
                                        if (sp->templateParams && sp->templateParams->size() == 1 &&
                                            (!definingTemplate || instantiatingTemplate))
                                            InsertInline(sp);
                                    }
                                    else
                                    {
                                        StatementGenerator sg(lex, sp);
                                        sg.FunctionBody();
                                        sg.BodyGen();
                                    }
                                }
                                if (sp->sb->constexpression && !sp->sb->builtin_constexpression)
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
                                if (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, Keyword::assign_))
                                {
                                    lex = getsym();
                                    if (MATCHKW(lex, Keyword::delete_))
                                    {
                                        sp->sb->deleted = true;
                                        sp->sb->constexpression = true;
                                        if (sp->sb->redeclared && !definingTemplate)
                                        {
                                            errorsym(ERR_DELETE_ON_REDECLARATION, sp);
                                        }
                                        lex = getsym();
                                    }
                                    else if (MATCHKW(lex, Keyword::default_))
                                    {
                                        sp->sb->defaulted = true;
                                        SetParams(sp);
                                        // fixme add more
                                        if (strcmp(sp->name, overloadNameTab[CI_CONSTRUCTOR]) != 0 &&
                                            strcmp(sp->name, overloadNameTab[CI_DESTRUCTOR]) != 0 &&
                                            strcmp(sp->name,
                                                   overloadNameTab[(int)Keyword::assign_ - (int)Keyword::new_ + CI_NEW]) !=
                                                0)  // this is meant to be a copy cons but is too loose
                                        {
                                            error(ERR_DEFAULT_ONLY_SPECIAL_FUNCTION);
                                        }
                                        else if (storage_class_in != StorageClass::member_)
                                        {
                                            // default declaration outside class definition forces the item to be instantiated
                                            if (strcmp(sp->name, overloadNameTab[CI_CONSTRUCTOR]) == 0)
                                                createConstructor(sp->sb->parentClass, sp);
                                            else if (strcmp(sp->name, overloadNameTab[CI_DESTRUCTOR]) == 0)
                                                createDestructor(sp->sb->parentClass);
                                            else
                                                createAssignment(sp->sb->parentClass, sp);
                                            sp->sb->forcedefault = true;
                                            InsertInline(sp);
                                        }
                                        lex = getsym();
                                    }
                                    else if (lex->data->type != LexType::i_ || lex->data->value.i != 0)
                                    {
                                        error(ERR_PURE_SPECIFIER_CONST_ZERO);
                                    }
                                    else
                                    {
                                        lex = getsym();
                                        sp->sb->ispure = true;
                                    }
                                }
                                else if (sp->sb->constexpression && sp->sb->storage_class != StorageClass::external_ &&
                                         !sp->tp->IsFunction() && !sp->tp->IsStructured())
                                {
                                    error(ERR_CONSTEXPR_REQUIRES_Initializer);
                                }
                                else if (sp->sb->parentClass && !sp->templateParams &&
                                         !(Optimizer::architecture == ARCHITECTURE_MSIL))
                                    if (!asFriend && storage_class_in != StorageClass::member_ &&
                                        storage_class_in != StorageClass::mutable_ && !sp->sb->templateLevel)
                                        errorsym(ERR_CANNOT_REDECLARE_OUTSIDE_CLASS, sp);
                            }
                        }
                        else
                        {
                            if ((sp->sb->storage_class == StorageClass::auto_ ||
                                 sp->sb->storage_class == StorageClass::register_) &&
                                (Optimizer::cparams.prm_stackprotect & STACK_UNINIT_VARIABLE) && !sp->tp->IsStructured() &&
                                !sp->tp->IsArray() && !sp->tp->IsRef())
                            {
                                sp->sb->runtimeSym = AnonymousVar(StorageClass::auto_, &stdpointer)->v.sp;
                            }
                            LexList* hold = lex;
                            bool structuredArray = false;
                            if (notype)
                            {
                                errorsym(ERR_UNDEFINED_IDENTIFIER_EXPECTING_TYPE, sp);
                                errskim(&lex, skim_semi);
                                return lex;
                            }
                            if (linkage3 == Linkage::entrypoint_)
                            {
                                errorsym(ERR_ENTRYPOINT_FUNC_ONLY, sp);
                            }
                            if (inTemplate)
                                RequiresDialect::Feature(Dialect::cpp14, "Variable Templates");
                            if (sp->sb->storage_class == StorageClass::virtual_)
                            {
                                errorsym(ERR_NONFUNCTION_CANNOT_BE_DECLARED_VIRTUAL, sp);
                            }
                            if (asExpression && !(asExpression & _F_NOCHECKAUTO))
                                checkauto(sp->tp, ERR_AUTO_NOT_ALLOWED);
                            if (sp->sb->storage_class == StorageClass::auto_ || sp->sb->storage_class == StorageClass::register_)
                            {
                                /* special case consts that can later have their address taken, because they are not autos and it
                                 * will cause errors later*/
                                if (!sp->tp->IsInt() || !sp->tp->IsConst())
                                {
                                    Statement* s = Statement::MakeStatement(lex, block, StatementNode::varstart_);
                                    s->select = MakeExpression(ExpressionNode::auto_, sp);
                                }
                            }
                            if (!sp->sb->label &&
                                (sp->sb->storage_class == StorageClass::static_ ||
                                 sp->sb->storage_class == StorageClass::localstatic_) &&
                                (Optimizer::architecture == ARCHITECTURE_MSIL))
                                sp->sb->label = Optimizer::nextLabel++;
                            if (Optimizer::cparams.prm_cplusplus && sp->sb->storage_class != StorageClass::type_ &&
                                sp->sb->storage_class != StorageClass::typedef_ && structLevel && (!instantiatingTemplate) &&
                                !funcsp && (MATCHKW(lex, Keyword::assign_) || MATCHKW(lex, Keyword::begin_)))
                            {
                                if ((MATCHKW(lex, Keyword::assign_) || MATCHKW(lex, Keyword::begin_)) &&
                                    storage_class_in == StorageClass::member_ &&
                                    (sp->sb->storage_class == StorageClass::static_ ||
                                     sp->sb->storage_class == StorageClass::external_))
                                {
                                    if (sp->tp->IsConst())
                                    {
                                        if (sp->tp->IsInt())
                                            goto doInitialize;
                                    }
                                    else if (!sp->sb->constexpression)
                                    {
                                        errorsym(ERR_CANNOT_INITIALIZE_STATIC_MEMBER_IN_CLASS, sp);
                                    }
                                }
                                lex = getDeferredData(lex, &sp->sb->deferredCompile, false);
                            }
                            else
                            {
                            doInitialize:
                                if (Optimizer::cparams.prm_cplusplus && sp->tp->IsStructured())
                                {
                                    SYMBOL* sp1 = sp->tp->BaseType()->sp;
                                    if (!definingTemplate && sp1->sb->templateLevel && sp1->templateParams &&
                                        !sp1->sb->instantiated)
                                    {
                                        auto tn = Type::MakeType(sp1, sp1->templateParams);
                                        tn->InstantiateDeferred();
                                        sp->tp = tn;
                                    }
                                }
                                lex = initialize(lex, funcsp, sp, storage_class_in, asExpression, inTemplate, deduceTemplate,
                                                 0); /* also reserves space */
                                if (sp->sb->parentClass && sp->sb->storage_class == StorageClass::global_)
                                {

                                    if (sp->templateParams && sp->templateParams->size() == 1)
                                    {
                                        SetLinkerNames(sp, Linkage::cdecl_);
                                        InsertInlineData(sp);
                                    }
                                    else if ((!sp->sb->parentClass ||
                                              (sp->sb->parentClass->templateParams &&
                                               allTemplateArgsSpecified(sp->sb->parentClass,
                                                                        sp->sb->parentClass->templateParams))) &&
                                             (!sp->templateParams || allTemplateArgsSpecified(sp, sp->templateParams)))
                                    {
                                        SetLinkerNames(sp, Linkage::cdecl_);
                                        InsertInlineData(sp);
                                    }
                                }
                                if (sp->sb->storage_class == StorageClass::auto_ ||
                                    sp->sb->storage_class == StorageClass::register_ ||
                                    (sp->sb->storage_class == StorageClass::localstatic_ && sp->sb->init))
                                {
                                    bool doit = true;
                                    if (sp->sb->storage_class == StorageClass::localstatic_)
                                    {

                                        if (sp->sb->init)
                                        {
                                            bool found = false;
                                            for (auto init : *sp->sb->init)
                                            {
                                                if (init->exp && !IsConstantExpression(init->exp, false, false))
                                                {
                                                    found = true;
                                                    break;
                                                }
                                            }
                                            doit = found;
                                        }
                                        else
                                        {
                                            doit = false;
                                        }
                                    }
                                    if (doit && (sp->sb->init || (sp->tp->IsArray() && sp->tp->msil)))
                                    {
                                        if (!sp->sb->constexpression)
                                        {
                                            Statement* st;
                                            currentLineData(block, hold, 0);
                                            st = Statement::MakeStatement(hold, block, StatementNode::expr_);
                                            if (!sp->tp->IsStructured() || !sp->tp->BaseType()->sp->sb->islambda)
                                            {
                                                st->select = ConverInitializersToExpression(sp->tp, sp, nullptr, funcsp,
                                                                                            sp->sb->init, nullptr, false);
                                            }
                                            else
                                            {
                                                // this is part of copy elision for lambda declarations
                                                st->select = sp->sb->init->front()->exp;
                                            }
                                            int offset = 0;
                                            if (sp->sb->runtimeSym)
                                            {
                                                auto exp = relptr(st->select->left->left, offset, true);
                                                if (exp)
                                                    SetRuntimeData(lex, exp, sp);
                                            }
                                        }
                                        else if (sp->tp->IsStructured())
                                        {
                                            Statement* st;
                                            currentLineData(block, hold, 0);
                                            st = Statement::MakeStatement(hold, block, StatementNode::expr_);
                                            st->select = MakeExpression(ExpressionNode::constexprconstructor_, sp);
                                            st->select->left = ConverInitializersToExpression(sp->tp, sp, nullptr, funcsp,
                                                                                              sp->sb->init, nullptr, false);
                                        }
                                    }
                                    else if ((sp->tp->IsArray() || sp->tp->IsStructured()) &&
                                             Optimizer::architecture == ARCHITECTURE_MSIL)
                                    {
                                        if (sp->sb->storage_class != StorageClass::localstatic_)
                                        {
                                            Statement* st;
                                            if (block.size() > 1)
                                            {
                                                auto it = block.begin();
                                                ++it;
                                                if ((*it)->type == Keyword::switch_)
                                                {
                                                    // have to put initializations before the switch not in the switch body they
                                                    // cannot
                                                    // be accesed in the switch body
                                                    ++it;
                                                    int n = (*it)->statements->size();
                                                    st = Statement::MakeStatement(hold, emptyBlockdata, StatementNode::expr_);
                                                    auto itb = (*it)->statements->begin();
                                                    for (int i = 0; i < n; i++, ++itb)
                                                        ;
                                                    (*it)->statements->insert(itb, st);
                                                }
                                                else
                                                {
                                                    st = Statement::MakeStatement(hold, emptyBlockdata, StatementNode::expr_);
                                                    block.front()->statements->push_front(st);
                                                }
                                                st->select = MakeExpression(ExpressionNode::initobj_,
                                                                            MakeExpression(ExpressionNode::auto_, sp));
                                            }
                                        }
                                    }
                                }
                            }
                            // get rid of line info from old declarations that didn't make it to the output file...
                            if (storage_class_in != StorageClass::auto_ && lines)
                                lines->clear();
                        }
                        if (sp->tp->size == 0 && sp->tp->BaseType()->type != BasicType::templateparam_ &&
                            sp->tp->BaseType()->type != BasicType::templateselector_ && !sp->tp->IsArray())
                        {
                            if (storage_class != StorageClass::typedef_)
                            {
                                if (storage_class_in == StorageClass::auto_ && sp->sb->storage_class != StorageClass::external_ &&
                                    !sp->tp->IsFunction())
                                    errorsym(ERR_UNSIZED, sp);
                                if (storage_class_in == StorageClass::parameter_)
                                    errorsym(ERR_UNSIZED, sp);
                            }
                        }
                        sp->tp->used = true;
                    }
                    if (isExplicit && !sp->sb->castoperator && !sp->sb->isConstructor)
                        error(ERR_EXPLICIT_CONSTRUCTOR_OR_CONVERSION_FUNCTION);
                    if (inTemplate && templateDeclarationLevel == 1)
                    {
                        inTemplateBody--;
                        TemplateGetDeferred(sp);
                    }
                    if (!strcmp(sp->name, "main") && !sp->sb->parentClass)
                    {
                        // fixme don't check if in parent class...
                        if (globalNameSpace->size() < 2)
                        {
                            if (sp->sb->attribs.inheritable.isInline)
                            {
                                error(ERR_MAIN_CANNOT_BE_INLINE_FUNC);
                            }
                            else if (storage_class == StorageClass::static_)
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
                    linkage = Linkage::none_;
                    linkage2 = Linkage::none_;
                    if (oldGlobals)
                    {
                        if (nsv->size() > 1)
                            nameSpaceList.front()->sb->value.i--;
                        nameSpaceList = oldNameSpaceList;
                        globalNameSpace = oldGlobals;
                        oldGlobals = nullptr;
                    }
                    if (strSym && strSym->tp->type != BasicType::enum_ && strSym->tp->type != BasicType::templateselector_)
                    {
                        enclosingDeclarations.Drop();
                    }
                } while (!asExpression && !inTemplate && MATCHKW(lex, Keyword::comma_) && (lex = getsym()) != nullptr);
            }
        }
    }
    FlushLineData(preProcessor->GetRealFile().c_str(), preProcessor->GetRealLineNo());
    if (needsemi && !asExpression)
        if (definingTemplate || !needkw(&lex, Keyword::semicolon_))
        {
            errskim(&lex, skim_semi_declare);
            skip(&lex, Keyword::semicolon_);
        }
    return lex;
}
}  // namespace Parser