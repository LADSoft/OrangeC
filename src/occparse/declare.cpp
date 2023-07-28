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
#include "lambda.h"
#include "template.h"
#include "expr.h"
#include "stmt.h"
#include "help.h"
#include "memory.h"
#include "template.h"
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
#include "template.h"
#include "libcxx.h"
#include "constexpr.h"
#include "symtab.h"
#include "ListFactory.h"

namespace Parser
{

int inDefaultParam;
char deferralBuf[100000];
SYMBOL* enumSyms;
std::list<STRUCTSYM> structSyms;
int expandingParams;
Optimizer::LIST* deferred;
int structLevel;
Optimizer::LIST* openStructs;
int parsingTrailingReturnOrUsing;
int inTypedef;
int resolvingStructDeclarations;

static int unnamed_tag_id, unnamed_id;
static char* importFile;
static unsigned symbolKey;
static int nameshim;

static LEXLIST* getStorageAndType(LEXLIST* lex, SYMBOL* funcsp, SYMBOL** strSym, bool inTemplate, bool assumeType,
                                  enum StorageClass* storage_class, StorageClass* storage_class_in, Optimizer::ADDRESS* address, bool* blocked,
                                  bool* isExplicit, bool* constexpression, TYPE** tp, Linkage* linkage, Linkage* linkage2,
                                  enum Linkage* linkage3, AccessLevel access, bool* notype, bool* defd, int* consdest, bool* templateArg,
                                  bool* asFriend);

void declare_init(void)
{
    unnamed_tag_id = 1;
    unnamed_id = 1;
    structSyms.clear();
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
static unsigned TypeCRC(SYMBOL* sp);
const char* AnonymousTypeName(SYMBOL* sp, SymbolTable<SYMBOL>* table)
{
    char buf[512];
    // type name will depend on file name
    auto name = preProcessor->GetRealFile().c_str();
    unsigned fileCRC = Utils::CRC32((const unsigned char *)name, strlen(name));
    // type name will also depend on the structure/enum elements defined for the type
    unsigned typeCRC = TypeCRC(sp);

    // generate type name
    sprintf(buf, "__anontype_%08x_%08x", fileCRC, typeCRC);
    // if it doesn't exist we are done...
    if (search(table, buf) == nullptr)
        return litlate(buf);

    // ok so now we have the rare case where two type names collide, add an index value to make it unique...
    // note this may not result in 'correct' behavior if we also have the case where the two names are colliding
    // and they are also being conditionally included in different source files...   we could use a stronger Keyword::_hash function
    // i guess but for now this will do...
    int i = 0;
    for (i=1; i; ++i)
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
SYMBOL* makeID(StorageClass storage_class, TYPE* tp, SYMBOL* spi, const char* name)
{    
    SYMBOL* sp = SymAlloc();
    LEXLIST* lex = context->cur ? context->cur->prev : context->last;
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
SYMBOL* makeUniqueID(StorageClass storage_class, TYPE* tp, SYMBOL* spi, const char* name)
{
    char buf[512];
    sprintf(buf, "%s_%08x", name, identityValue);
    return makeID(storage_class, tp, spi, litlate(buf));
}
TYPE* MakeType(TYPE& tp, BasicType type, TYPE* base)
{
    tp.type = type;
    tp.btp = base;
    tp.rootType = &tp;
    tp.size = getSize(type);
    switch (type)
    {
        case BasicType::void_:
        case BasicType::ellipse:
        case BasicType::memberptr:
            break;
        case BasicType::any:
            tp.size = getSize(BasicType::int_);
            break;
        case BasicType::far:
        case BasicType::near:
        case BasicType::const_:
        case BasicType::va_list:
        case BasicType::objectArray:
        case BasicType::volatile_:
        case BasicType::restrict_:
        case BasicType::static_:
        case BasicType::atomic:
        case BasicType::typedef_:
        case BasicType::lrqual:
        case BasicType::rrqual:
        case BasicType::derivedfromtemplate:
            if (base)
            {
                tp.rootType = base->rootType;
                if (tp.rootType)
                    tp.size = tp.rootType->size;
            }
            break;

        case BasicType::lref:
        case BasicType::rref:
        case BasicType::func:
        case BasicType::ifunc:
        case BasicType::__object:
            tp.size = getSize(BasicType::pointer);
            break;
        case BasicType::enum_:
            tp.size = getSize(BasicType::int_);
            break;
        default:
            tp.size = getSize(type);
            break;
    }
    return &tp;
}
TYPE* MakeType(BasicType type, TYPE* base)
{
    TYPE* rv = Allocate<TYPE>();
    return MakeType(*rv, type, base);
}
TYPE* CopyType(TYPE* tp, bool deep, std::function<void(TYPE*&, TYPE*&)> callback)
{
    TYPE* rv = nullptr;
    if (deep)
    {
        TYPE** last = &rv;
        for (; tp; tp = tp->btp, last = &(*last)->btp)
        {
            *last = Allocate<TYPE>();
            **last = *tp;
            callback ? callback(tp, *last) : (void)0;
        }
        UpdateRootTypes(rv);
    }
    else
    {
        rv = Allocate<TYPE>();
        *rv = *tp;
        if (tp->rootType == tp)
            rv->rootType = rv;
    }
    return rv;
}
void addStructureDeclaration(STRUCTSYM* decl)
{
    decl->tmpl = nullptr;
    structSyms.push_front(*decl);
}
void addTemplateDeclaration(STRUCTSYM* decl)
{
    decl->str = nullptr;
    structSyms.push_front(*decl);
}
void dropStructureDeclaration(void) 
{ 
    structSyms.pop_front(); 
}
SYMBOL* getStructureDeclaration(void)
{
    for (auto&& l : structSyms)
        if (l.str)
            return l.str;
    return nullptr;
}
void InsertSymbol(SYMBOL* sp, StorageClass storage_class, Linkage linkage, bool allowDups)
{
    SymbolTable<SYMBOL>* table;
    SYMBOL* ssp = getStructureDeclaration();

    if (ssp && sp->sb->parentClass == ssp)
    {
        table = sp->sb->parentClass->tp->syms;
    }
    else if (storage_class == StorageClass::auto_ || storage_class == StorageClass::register_ || storage_class == StorageClass::catchvar ||
             storage_class == StorageClass::parameter || storage_class == StorageClass::localstatic)
    {
        table = localNameSpace->front()->syms;
    }
    else if (Optimizer::cparams.prm_cplusplus && isfunction(sp->tp) && theCurrentFunc && !getStructureDeclaration())
    {
        table = localNameSpace->front()->syms;
    }
    else
    {
        table = globalNameSpace->front()->syms;
    }
    if (table)
    {
        if (isfunction(sp->tp) && !istype(sp))
        {
            const char* name = sp->sb->castoperator ? overloadNameTab[CI_CAST] : sp->name;
            SYMBOL* funcs = table->Lookup(name);
            if (!funcs)
            {
                auto tp = MakeType(BasicType::aggregate);
                funcs = makeID(StorageClass::overloads, tp, 0, name);
                funcs->sb->castoperator = sp->sb->castoperator;
                funcs->sb->parentClass = sp->sb->parentClass;
                funcs->sb->parentNameSpace = sp->sb->parentNameSpace;
                tp->sp = funcs;
                SetLinkerNames(funcs, linkage);
                table->Add(funcs);
                table = funcs->tp->syms = symbols.CreateSymbolTable();
                table->Add(sp);
                sp->sb->overloadName = funcs;
            }
            else if (Optimizer::cparams.prm_cplusplus && funcs->sb->storage_class == StorageClass::overloads)
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
                            if (comparetypes(basetype(sp->tp)->btp, basetype((sp1)->tp)->btp, true) && sameTemplate(basetype(sp->tp)->btp, basetype((sp1)->tp)->btp))
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
LEXLIST* tagsearch(LEXLIST* lex, char* name, SYMBOL** rsp, SymbolTable<SYMBOL>** table, SYMBOL** strSym_out, std::list<NAMESPACEVALUEDATA*>** nsv_out,
                   enum StorageClass storage_class)
{
    std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
    SYMBOL* strSym = nullptr;

    *rsp = nullptr;
    if (ISID(lex) || MATCHKW(lex, Keyword::_classsel))
    {
        lex = nestedSearch(lex, rsp, &strSym, &nsv, nullptr, nullptr, true, storage_class, false, false);
        if (*rsp)
        {
            strcpy(name, (*rsp)->name);
            lex = getsym();
            if (MATCHKW(lex, Keyword::_begin))
            {
                // specify EXACTLY the first result if it is a definition
                // otherwise what is found by nestedSearch is fine...
                if (strSym)
                    *rsp = strSym->tp->tags->Lookup((*rsp)->name);
                else if (nsv)
                    *rsp = nsv->front()->tags->Lookup((*rsp)->name);
                else if (Optimizer::cparams.prm_cplusplus && (storage_class == StorageClass::member || storage_class == StorageClass::mutable_))
                    *rsp = getStructureDeclaration()->tp->tags->Lookup((*rsp)->name);
                else if (storage_class == StorageClass::auto_)
                    *rsp = localNameSpace->front()->tags->Lookup((*rsp)->name);
                else
                    *rsp = globalNameSpace->front()->tags->Lookup((*rsp)->name);
                if (!*rsp)
                {
                    if (nsv || strSym)
                    {
                        errorNotMember(strSym, nsv->front(), (*rsp)->name);
                    }
                    *rsp = nullptr;
                }
            }
        }
        else if (ISID(lex))
        {
            strcpy(name, lex->data->value.s.a);
            lex = getsym();
            if (MATCHKW(lex, Keyword::_begin))
            {
                if (nsv || strSym)
                {
                    errorNotMember(strSym, nsv->front(), name);
                }
            }
        }
    }
    if (nsv)
    {
        *table = nsv->front()->tags;
    }
    else if (strSym)
    {
        *table = strSym->tp->tags;
    }
    else if (Optimizer::cparams.prm_cplusplus && (storage_class == StorageClass::member || storage_class == StorageClass::mutable_))
    {
        strSym = getStructureDeclaration();
        *table = strSym->tp->tags;
    }
    else
    {
        if (storage_class == StorageClass::auto_)
        {
            *table = localNameSpace->front()->tags;
            nsv = localNameSpace;
        }
        else
        {
            *table = globalNameSpace->front()->tags;
            nsv = globalNameSpace;
        }
    }
    *nsv_out = nsv;
    *strSym_out = strSym;
    return lex;
}
static void checkIncompleteArray(TYPE* tp, const char* errorfile, int errorline)
{
    tp = basetype(tp);
    if (tp->syms == nullptr)
        return; /* should get a size error */
    auto sp = tp->syms->back();
    if (ispointer(sp->tp) && basetype(sp->tp)->size == 0)
        specerror(ERR_STRUCT_MAY_NOT_CONTAIN_INCOMPLETE_STRUCT, "", errorfile, errorline);
}
LEXLIST* get_type_id(LEXLIST* lex, TYPE** tp, SYMBOL* funcsp, StorageClass storage_class, bool beforeOnly, bool toErr, bool inUsing)
{
    enum Linkage linkage = Linkage::none_, linkage2 = Linkage::none_, linkage3 = Linkage::none_;
    bool defd = false;
    SYMBOL* sp = nullptr;
    SYMBOL* strSym = nullptr;
    std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
    bool notype = false;
    bool oldTemplateType = inTemplateType;
    *tp = nullptr;

    lex = getQualifiers(lex, tp, &linkage, &linkage2, &linkage3, nullptr);
    lex = getBasicType(lex, funcsp, tp, nullptr, false, funcsp ? StorageClass::auto_ : StorageClass::global, &linkage, &linkage2, &linkage3, AccessLevel::public_,
                       &notype, &defd, nullptr, nullptr, false, false, inUsing, false, false);
    lex = getQualifiers(lex, tp, &linkage, &linkage2, &linkage3, nullptr);
    lex = getBeforeType(lex, funcsp, tp, &sp, &strSym, &nsv, false, storage_class, &linkage, &linkage2, &linkage3, &notype, false,
                        false, beforeOnly, false); /* fixme at file scope init */
    sizeQualifiers(*tp);
    if (notype)
        *tp = nullptr;
    else if (sp && !sp->sb->anonymous && toErr)
        if (sp->tp->type != BasicType::templateparam)
            error(ERR_TOO_MANY_IDENTIFIERS);
    if (sp && sp->sb->anonymous && linkage != Linkage::none_)
    {
        if (sp->sb->attribs.inheritable.linkage != Linkage::none_)
            error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
        else
            sp->sb->attribs.inheritable.linkage = linkage;
    }
    inTemplateType = oldTemplateType;
    return lex;
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
        if (p->sb->storage_class == StorageClass::overloads)
        {
            for (auto pi: *p->tp->syms)
            {
                if (pi->sb->ispure)
                {
                    // ok found a pure function, look it up within top and
                    // check to see if it has been overrridden
                    SYMBOL* pq;
                    STRUCTSYM l;
                    l.str = (SYMBOL*)top;
                    addStructureDeclaration(&l);
                    pq = classsearch(pi->name, false, false, true);
                    dropStructureDeclaration();
                    if (pq)
                    {
                        for (auto pq1 : *pq->tp->syms)
                        {
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
void calculateStructOffsets(SYMBOL* sp)
{
    enum BasicType type = basetype(sp->tp)->type;
    enum BasicType bittype = BasicType::none;
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
            bases->offset = getSize(BasicType::pointer);
        else
            bases->offset = 0;
    }
    if (sp->sb->hasvtab && (!sp->sb->baseClasses || !sp->sb->baseClasses->size() || !sp->sb->baseClasses->front()->cls->sb->hasvtab || sp->sb->baseClasses->front()->isvirtual))
    {
        size += getSize(BasicType::pointer);
        totalAlign = getAlign(StorageClass::member, &stdpointer);
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
        TYPE* tp = basetype(p->tp);
        if (p->sb->storage_class != StorageClass::static_ && p->sb->storage_class != StorageClass::const_ant && p->sb->storage_class != StorageClass::external &&
            p->sb->storage_class != StorageClass::overloads && p->sb->storage_class != StorageClass::enumconstant && !istype(p) && p != sp &&
            p->sb->parentClass == sp)
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
                    align = getAlign(StorageClass::member, tp);
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
                if (Optimizer::cparams.c_dialect >= Dialect::c99 && tp->type == BasicType::pointer && p->tp->array)
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
                else if (!Optimizer::cparams.prm_cplusplus && p->sb->storage_class == StorageClass::overloads)
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
                bittype = BasicType::none;
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
        if (Optimizer::cparams.prm_cplusplus)
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
static bool validateAnonymousUnion(SYMBOL* parent, TYPE* unionType)
{
    bool rv = true;
    unionType = basetype(unionType);
    {
        for (auto member : *unionType->syms)
        {
            if (Optimizer::cparams.prm_cplusplus && member->sb->storage_class == StorageClass::overloads)
            {
                for (auto sp1 : *basetype(member->tp)->syms)
                {
                    if (!sp1->sb->defaulted)
                    {
                        error(ERR_ANONYMOUS_UNION_NO_FUNCTION_OR_TYPE);
                        rv = false;
                        break;
                    }
                }
            }
            else if ((Optimizer::cparams.prm_cplusplus && member->sb->storage_class == StorageClass::type) ||
                     member->sb->storage_class == StorageClass::typedef_)
            {
                error(ERR_ANONYMOUS_UNION_NO_FUNCTION_OR_TYPE);
                rv = false;
            }
            else if ((Optimizer::cparams.prm_cplusplus && member->sb->access == AccessLevel::private_) || member->sb->access == AccessLevel::protected_)
            {
                error(ERR_ANONYMOUS_UNION_PUBLIC_MEMBERS);
                rv = false;
            }
            else if (Optimizer::cparams.prm_cplusplus && member->sb->storage_class != StorageClass::member &&
                     member->sb->storage_class != StorageClass::mutable_)
            {
                error(ERR_ANONYMOUS_UNION_NONSTATIC_MEMBERS);
                rv = false;
            }
            else {
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
    sp->sb->label = Optimizer::nextLabel++;
    sp->sb->storage_class = StorageClass::localstatic;
    insertInitSym(sp);
    for (auto sym : *sp->tp->syms)
    {
        if (sym->sb->storage_class == StorageClass::member || sym->sb->storage_class == StorageClass::mutable_)
        {
            SYMBOL* spi;
            if ((spi = gsearch(sym->name)) != nullptr)
            {
                currentErrorLine = 0;
                preverrorsym(ERR_DUPLICATE_IDENTIFIER, spi, spi->sb->declfile, spi->sb->declline);
            }
            else
            {
                sym->sb->storage_class = StorageClass::localstatic;
                sym->sb->label = sp->sb->label;
                InsertSymbol(sym, StorageClass::static_, Linkage::c_, false);
            }
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
        if (isstructured(spm->tp) && spm->sb->anonymous && basetype(spm->tp)->sp->sb->anonymous)
        {
            resolveAnonymousUnions(spm);
            validateAnonymousUnion(sp, spm->tp);

            auto it = itmember;
            ++it;
            sp->tp->syms->remove(itmember);
            itmember = it;
            if (basetype(spm->tp)->type == BasicType::union_)
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
                if ((newsp->sb->storage_class == StorageClass::member || newsp->sb->storage_class == StorageClass::mutable_) && !isfunction(newsp->tp))
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
static bool usesClass(SYMBOL* cls, SYMBOL* internal)
{
    if (cls->tp->syms)
    {
        for (auto sym : *cls->tp->syms)
        {
            TYPE* tp = sym->tp;
            if (istype(sym))
            {
                if (sym->sb->storage_class != StorageClass::typedef_)
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
        }
    }
    return false;
}
static void GetStructAliasType(SYMBOL* sym)
{
    if (templateNestingCount && !instantiatingTemplate)
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
        if (m->sb->storage_class == StorageClass::overloads)
        {
            if (m->tp->syms->front()->sb->isDestructor && !m->tp->syms->front()->sb->defaulted)
                return;
        }
    }
    if (!cache || isstructured(cache->tp) || isatomic(cache->tp) || isarray(cache->tp) || basetype(cache->tp)->bits ||
        isfuncptr(cache->tp) || isref(cache->tp) || isfloat(cache->tp) || isimaginary(cache->tp))
        return;
    sym->sb->structuredAliasType = cache->tp;
}
static void baseFinishDeclareStruct(SYMBOL* funcsp)
{
    (void)funcsp;
    int n = 0, i, j;
    Optimizer::LIST* lst = openStructs;
    SYMBOL** syms;
    while (lst)
        n++, lst = lst->next;
    syms = Allocate<SYMBOL*>(n);
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
            }
    ++ resolvingStructDeclarations;
    for (i = 0; i < n; i++)
    {
        SYMBOL* sp = syms[i];
        bool recursive = sp->sb->declaringRecursive;
        sp->sb->declaringRecursive = false;
        if (!sp->sb->performedStructInitialization)
        {
            if (!templateNestingCount)
            {
                for (auto s : *sp->tp->syms)
                {
                    if (s->tp->type == BasicType::aggregate)
                    {
                        for (auto f : *s->tp->syms)
                        {
                            if (!f->sb->templateLevel)
                            {
                                basetype(f->tp)->btp = ResolveTemplateSelectors(f, basetype(f->tp)->btp);
                                basetype(f->tp)->btp = PerformDeferredInitialization(basetype(f->tp)->btp, funcsp);
                                for (auto a : *basetype(f->tp)->syms)
                                {
                                    a->tp = ResolveTemplateSelectors(a, a->tp);
                                    a->tp = PerformDeferredInitialization(a->tp, funcsp);
                                }
                            }
                        }
                    }
                    else if (!istype(s))
                    {
                        s->tp = ResolveTemplateSelectors(s, s->tp);
                        s->tp = PerformDeferredInitialization(s->tp, funcsp);
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
    -- resolvingStructDeclarations;
    if (!templateNestingCount || instantiatingTemplate)
    {
        for (i = 0; i < n; i++)
        {
            SYMBOL* sp = syms[i];
            for (auto sym : *sp->tp->syms)
            {
                if (sym->sb->storage_class == StorageClass::global && isconst(sym->tp) && isint(sym->tp) && sym->sb->init &&
                    sym->sb->init->front()->exp->type == ExpressionNode::templateselector)
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
                if (n > 1 && !templateNestingCount)
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
}
static LEXLIST* structbody(LEXLIST* lex, SYMBOL* funcsp, SYMBOL* sp, AccessLevel currentAccess, SymbolTable<SYMBOL>* anonymousTable)
{
    STRUCTSYM sl;
    (void)funcsp;
    if (Optimizer::cparams.prm_cplusplus)
    {
        Optimizer::LIST* lst = Allocate<Optimizer::LIST>();
        lst->next = openStructs;
        openStructs = lst;
        lst->data = sp;
    }
    lex = getsym();
    sl.str = sp;
    addStructureDeclaration(&sl);
    sp->sb->declaring = true;
    while (lex && KW(lex) != Keyword::_end)
    {
        FlushLineData(lex->data->errfile, lex->data->linedata->lineno);
        switch (KW(lex))
        {
            case Keyword::_private:
                sp->sb->accessspecified = true;
                currentAccess = AccessLevel::private_;
                lex = getsym();
                needkw(&lex, Keyword::_colon);
                break;
            case Keyword::_public:
                sp->sb->accessspecified = true;
                currentAccess = AccessLevel::public_;
                lex = getsym();
                needkw(&lex, Keyword::_colon);
                break;
            case Keyword::_protected:
                sp->sb->accessspecified = true;
                currentAccess = AccessLevel::protected_;
                lex = getsym();
                needkw(&lex, Keyword::_colon);
                break;
            default:
                lex = declare(lex, nullptr, nullptr, StorageClass::member, Linkage::none_, emptyBlockdata, true, false, false, currentAccess);
                break;
        }
    }
    sp->sb->declaring = false;
    dropStructureDeclaration();
    sp->sb->hasvtab = usesVTab(sp);
    calculateStructOffsets(sp);

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
        if (Optimizer::cparams.prm_cplusplus)
            deferredInitializeStructMembers(sp);
        GetStructAliasType(sp);
    }
    if (!Optimizer::cparams.prm_cplusplus || structLevel == 1)
    {
        structLevel--;
        baseFinishDeclareStruct(funcsp);
        structLevel++;
    }
    if (Optimizer::cparams.prm_cplusplus && sp->tp->syms && !templateNestingCount)
    {
        SYMBOL* cons = search(basetype(sp->tp)->syms, overloadNameTab[CI_CONSTRUCTOR]);
        if (!cons)
        {
            for (auto sp1 : *basetype(sp->tp)->syms)
            {
                if (sp1->sb->storage_class == StorageClass::member || sp1->sb->storage_class == StorageClass::mutable_)
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
static void TypeCRC(TYPE *tp, unsigned& crc)
{
    const int constval = 0xffeeccdd;
    const int volval = 0xaabbccdd;
    const int arrval = 0x11223344;
    while (tp)
    {
        if (isconst(tp))
            crc = Utils::PartialCRC32(crc, (const unsigned char *)&constval, sizeof(constval));
        if (isvolatile(tp))
            crc = Utils::PartialCRC32(crc, (const unsigned char *)&volval, sizeof(volval));
        crc = Utils::PartialCRC32(crc, (const unsigned char *)&basetype(tp)->type, sizeof(tp->type));
        if (basetype(tp)->type == BasicType::struct_ || basetype(tp)->type == BasicType::enum_)
            crc = Utils::PartialCRC32(crc, (const unsigned char *)basetype(tp)->sp->name, strlen(basetype(tp)->sp->name));
        crc = Utils::PartialCRC32(crc, (const unsigned char *)&basetype(tp)->size, sizeof(tp->size));
        if (tp->array)
           crc = Utils::PartialCRC32(crc, (const unsigned char *)&arrval, sizeof(arrval));
        crc = Utils::PartialCRC32(crc, (const unsigned char *)&basetype(tp)->bits, sizeof(tp->bits));
        tp = basetype(tp)->btp;    
    }
}
static unsigned TypeCRC(SYMBOL* sp)
{
    unsigned crc = 0xffffffff;
    crc = Utils::PartialCRC32(crc, (const unsigned char *)&basetype(sp->tp)->alignment, sizeof(basetype(sp->tp)->alignment));
    crc = Utils::PartialCRC32(crc, (const unsigned char *)&basetype(sp->tp)->size, sizeof(basetype(sp->tp)->size));
    for (auto s : *sp->tp->syms)
    {
        crc = Utils::PartialCRC32(crc, (const unsigned char *)s->name, strlen(s->name));
        crc = Utils::PartialCRC32(crc, (const unsigned char *)&s->sb->offset, sizeof(s->sb->offset));
        TypeCRC(s->tp, crc);
    }
    return crc;
}
LEXLIST* innerDeclStruct(LEXLIST* lex, SYMBOL* funcsp, SYMBOL* sp, bool inTemplate, AccessLevel defaultAccess, bool isfinal,
                         bool* defd, SymbolTable<SYMBOL>* anonymousTable)
{
    int oldParsingTemplateArgs;
    oldParsingTemplateArgs = parsingDefaultTemplateArgs;
    parsingDefaultTemplateArgs = 0;
    bool hasBody = (Optimizer::cparams.prm_cplusplus && KW(lex) == Keyword::_colon) || KW(lex) == Keyword::_begin;
    SYMBOL* injected = nullptr;

    if (nameSpaceList.size())
        SetTemplateNamespace(sp);
    if (sp->sb->attribs.inheritable.structAlign == 0)
        sp->sb->attribs.inheritable.structAlign = 1;
    structLevel++;
    if (hasBody)
    {
        if (sp->tp->syms || sp->tp->tags)
        {
            preverrorsym(ERR_STRUCT_HAS_BODY, sp, sp->sb->declfile, sp->sb->declline);
        }
        sp->tp->syms = symbols.CreateSymbolTable();
        if (Optimizer::cparams.prm_cplusplus)
        {
            sp->tp->tags = symbols.CreateSymbolTable();
            injected = CopySymbol(sp);
            injected->sb->mainsym = sp;      // for constructor/destructor matching
            sp->tp->tags->Add(injected);  // inject self
            injected->sb->access = AccessLevel::public_;
        }
    }
    if (inTemplate && templateNestingCount == 1)
        inTemplateBody++;
    if (Optimizer::cparams.prm_cplusplus)
        if (KW(lex) == Keyword::_colon)
        {
            lex = baseClasses(lex, funcsp, sp, defaultAccess);
            if (injected)
                injected->sb->baseClasses = sp->sb->baseClasses;
            if (!MATCHKW(lex, Keyword::_begin))
                errorint(ERR_NEEDY, '{');
        }
    if (KW(lex) == Keyword::_begin)
    {
        sp->sb->isfinal = isfinal;
        lex = structbody(lex, funcsp, sp, defaultAccess, anonymousTable);
        *defd = true;
    }
    if (inTemplate && templateNestingCount == 1)
    {
        inTemplateBody--;
        TemplateGetDeferred(sp);
    }
    --structLevel;
    parsingDefaultTemplateArgs = oldParsingTemplateArgs;
    return lex;
}
static unsigned char* ParseUUID(LEXLIST** lex)
{
    if (MATCHKW(*lex, Keyword::___uuid))
    {
        unsigned vals[16];
        unsigned char* rv = Allocate<unsigned char>(16);
        *lex = getsym();
        needkw(lex, Keyword::_openpa);
        if ((*lex)->data->type == l_astr)
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
        needkw(lex, Keyword::_closepa);
        return rv;
    }
    return nullptr;
}
static LEXLIST* declstruct(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, bool inTemplate, bool asfriend, StorageClass storage_class,
                           enum AccessLevel access, bool* defd, bool constexpression)
{
    bool isfinal = false;
    SymbolTable<SYMBOL>* table = nullptr;
    const char* tagname;
    char newName[4096];
    enum BasicType type = BasicType::none;
    SYMBOL* sp;
    int charindex;
    std::list<NAMESPACEVALUEDATA*>* nsv;
    SYMBOL* strSym;
    enum AccessLevel defaultAccess;
    bool addedNew = false;
    int declline = lex->data->errline;
    int realdeclline = lex->data->linedata->lineno;
    bool anonymous = false;
    unsigned char* uuid;
    enum Linkage linkage1 = Linkage::none_, linkage2 = Linkage::none_, linkage3 = Linkage::none_;
    *defd = false;
    switch (KW(lex))
    {
        case Keyword::_class:
            defaultAccess = AccessLevel::private_;
            type = BasicType::class_;
            break;
        default:
        case Keyword::_struct:
            defaultAccess = AccessLevel::public_;
            type = BasicType::struct_;
            break;
        case Keyword::_union:
            defaultAccess = AccessLevel::public_;
            type = BasicType::union_;
            break;
    }
    lex = getsym();
    uuid = ParseUUID(&lex);
    auto oldAttribs = basisAttribs;
    basisAttribs = {0};
    ParseAttributeSpecifiers(&lex, funcsp, true);
    if (MATCHKW(lex, Keyword::___declspec))
    {
        lex = getsym();
        lex = parse_declspec(lex, &linkage1, &linkage2, &linkage3);
    }
    if (MATCHKW(lex, Keyword::___rtllinkage))
    {
        lex = getsym();
        linkage2 = getDefaultLinkage();
    }
    if (ISID(lex))
    {
        charindex = lex->data->charindex;
        tagname = litlate(lex->data->value.s.a);
    }
    else
    {
        if (!MATCHKW(lex, Keyword::_begin) && !MATCHKW(lex, Keyword::_colon))
            errorint(ERR_NEEDY, '{');
        tagname = "";
        charindex = -1;
        anonymous = true;
    }

    strcpy(newName, tagname);
    if (inTemplate)
        inTemplateSpecialization++;

    lex = tagsearch(lex, newName, &sp, &table, &strSym, &nsv, storage_class);

    if (inTemplate)
        inTemplateSpecialization--;

    if (!asfriend && charindex != -1 && Optimizer::cparams.prm_cplusplus && openStructs && MATCHKW(lex, Keyword::_semicolon))
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
        if (!MATCHKW(lex, Keyword::_begin) && !MATCHKW(lex, Keyword::_colon))
            errorint(ERR_NEEDY, '{');
    }
    if (ISID(lex))
    {
        lex = getsym();
        if (MATCHKW(lex, Keyword::_lt) || MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::_colon))
        {
            lex = backupsym();
            // multiple identifiers, wade through them for error handling
            error(ERR_TOO_MANY_IDENTIFIERS_IN_DECLARATION);
            while (ISID(lex))
            {
                charindex = lex->data->charindex;
                tagname = litlate(lex->data->value.s.a);
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
        sp->sb->storage_class = StorageClass::type;
        sp->tp = MakeType(type);
        sp->tp->sp = sp;
        sp->tp = AttributeFinish(sp, sp->tp);
        sp->sb->declcharpos = charindex;
        sp->sb->declline = sp->sb->origdeclline = declline;
        sp->sb->realdeclline = realdeclline;
        sp->sb->declfile = sp->sb->origdeclfile = lex->data->errfile;
        sp->sb->declfilenum = lex->data->linedata->fileindex;
        sp->sb->attribs = basisAttribs;
        if ((storage_class == StorageClass::member || storage_class == StorageClass::mutable_) &&
            (MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::_colon) || MATCHKW(lex, Keyword::___try) || MATCHKW(lex, Keyword::_semicolon)))
            sp->sb->parentClass = getStructureDeclaration();
        if (nsv)
            sp->sb->parentNameSpace = nsv->front()->name;
        else
            sp->sb->parentNameSpace = globalNameSpace->front()->name;
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
            if (inTemplate && templateNestingCount)
            {
                if (MATCHKW(lex, Keyword::_lt))
                    errorsym(ERR_SPECIALIZATION_REQUIRES_PRIMARY, sp);
                sp->templateParams = TemplateGetParams(sp);
                sp->sb->templateLevel = templateNestingCount;
                TemplateMatching(lex, nullptr, sp->templateParams, sp, MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::_colon));
                SetLinkerNames(sp, Linkage::cdecl_);
            }
            browse_variable(sp);
            (Optimizer::cparams.prm_cplusplus && !sp->sb->parentClass ? globalNameSpace->front()->tags : table)->Add(sp);
        }
    }
    else
    {
        if (asfriend && sp->sb->templateLevel)
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
        else if (inTemplate && templateNestingCount)
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
                if (inTemplate && KW(lex) == Keyword::_lt)
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
                    sp->templateParams =
                        TemplateMatching(lex, origParams, templateParams, sp, MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::_colon));
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
                    sp->templateParams =
                        TemplateMatching(lex, sp->templateParams, templateParams, sp, MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::_colon));
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
        else if (MATCHKW(lex, Keyword::_lt))
        {
            if (inTemplate)
            {
                // instantiation
                std::list<TEMPLATEPARAMPAIR>* templateParams = TemplateGetParams(sp);
                lex = GetTemplateArguments(lex, funcsp, nullptr, &templateParams->front().second->bySpecialization.types);
            }
            else if (sp->sb->templateLevel)
            {
                if ((MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::_colon)))
                {
                    errorsym(ERR_IS_ALREADY_DEFINED_AS_A_TEMPLATE, sp);
                }
                else
                {
                    std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                    lex = GetTemplateArguments(lex, funcsp, nullptr, &lst);
                    sp = GetClassTemplate(sp, lst, false);
                }
            }
        }
        else if (sp->sb->templateLevel && (MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::_colon)))
        {
            errorsym(ERR_IS_ALREADY_DEFINED_AS_A_TEMPLATE, sp);
        }
    }
    if (sp)
    {
        if (uuid)
            sp->sb->uuid = uuid;
        if (access != sp->sb->access && sp->tp->syms && (MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::_colon)))
        {
            errorsym(ERR_CANNOT_REDEFINE_ACCESS_FOR, sp);
        }
        lex = innerDeclStruct(lex, funcsp, sp, inTemplate, defaultAccess, isfinal, defd, anonymous ? table : nullptr);
        if (constexpression)
        {
            error(ERR_CONSTEXPR_NO_STRUCT);
        }
        *tp = sp->tp;
    }
    basisAttribs = oldAttribs;
    return lex;
}
static LEXLIST* enumbody(LEXLIST* lex, SYMBOL* funcsp, SYMBOL* spi, StorageClass storage_class, AccessLevel access, TYPE* fixedType,
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
    spi->tp->syms = symbols.CreateSymbolTable(); /* holds a list of all the enum values, e.g. for debug info */
    if (!MATCHKW(lex, Keyword::_end))
    {
        while (lex)
        {
            if (ISID(lex))
            {
                SYMBOL* sp;
                TYPE* tp;
                if (Optimizer::cparams.prm_cplusplus)
                {
                    tp = CopyType(fixedType ? fixedType : unfixedType);
                    tp->scoped = scoped;  // scoped the constants type as well for error checking
                    tp->btp = spi->tp;    // the integer type gets a base type which is the enumeration for error checking
                    tp->rootType = tp;
                    tp->enumConst = true;
                    tp->sp = spi;
                }
                else
                {
                    tp = MakeType(BasicType::int_);
                }
                sp = makeID(StorageClass::enumconstant, tp, 0, litlate(lex->data->value.s.a));
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
                if (MATCHKW(lex, Keyword::_assign))
                {
                    TYPE* tp = nullptr;
                    EXPRESSION* exp = nullptr;
                    lex = getsym();
                    lex = expression_no_comma(lex, funcsp, nullptr, &tp, &exp, nullptr, _F_SCOPEDENUM);
                    if (tp)
                    {
                        optimize_for_constants(&exp);
                    }

                    if (tp && isintconst(exp))  // type is redefined to nullptr here, is this intentional?
                    {
                        if (Optimizer::cparams.prm_cplusplus)
                        {
                            if (!fixedType)
                            {
                                if (tp->type != BasicType::bool_)
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
                            spi->tp->btp->type = BasicType::long_long;
                            spi->tp->size = spi->tp->btp->size = getSize(BasicType::long_long);
                            unfixedType = spi->tp->btp;
                        }
                    }
                }
                if (!MATCHKW(lex, Keyword::_comma))
                    break;
                else
                {
                    lex = getsym();
                    if (KW(lex) == Keyword::_end)
                    {
                        if (Optimizer::cparams.prm_ansi && Optimizer::cparams.c_dialect < Dialect::c99 && !Optimizer::cparams.prm_cplusplus)
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
    else if (!MATCHKW(lex, Keyword::_end))
    {
        errorint(ERR_NEEDY, '}');
        errskim(&lex, skim_end);
        skip(&lex, Keyword::_end);
    }
    else
        lex = getsym();
    if (fixed)
        spi->tp->fixed = true;
    spi->sb->declaring = false;
    return lex;
}
static LEXLIST* declenum(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, StorageClass storage_class, AccessLevel access, bool opaque,
                         bool* defd)
{
    SymbolTable<SYMBOL>* table;
    const char* tagname;
    char newName[4096];
    int charindex;
    bool noname = false;
    bool scoped = false;
    TYPE* fixedType = nullptr;
    SYMBOL* sp;
    std::list<NAMESPACEVALUEDATA*>* nsv;
    SYMBOL* strSym;
    int declline = lex->data->errline;
    int realdeclline = lex->data->linedata->lineno;
    bool anonymous = false;
    enum Linkage linkage1 = Linkage::none_, linkage2 = Linkage::none_, linkage3 = Linkage::none_;
    *defd = false;
    lex = getsym();
    auto oldAttribs = basisAttribs;
    basisAttribs = {0};
    ParseAttributeSpecifiers(&lex, funcsp, true);
    if (Optimizer::cparams.prm_cplusplus && (MATCHKW(lex, Keyword::_class) || MATCHKW(lex, Keyword::_struct)))
    {
        scoped = true;
        lex = getsym();
    }
    if (MATCHKW(lex, Keyword::___declspec))
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

    strcpy(newName, tagname);
    lex = tagsearch(lex, newName, &sp, &table, &strSym, &nsv, storage_class);
    if (Optimizer::cparams.prm_cplusplus && KW(lex) == Keyword::_colon)
    {
        lex = getsym();
        lex = get_type_id(lex, &fixedType, funcsp, StorageClass::cast, false, true, false);
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
        sp->sb->storage_class = StorageClass::type;
        sp->tp = MakeType(BasicType::enum_);
        if (fixedType)
        {
            sp->tp->fixed = true;
            sp->tp->btp = fixedType;
        }
        else
        {
            sp->tp->btp = MakeType(BasicType::int_);
        }
        sp->tp->scoped = scoped;
        sp->tp->size = sp->tp->btp->size;
        sp->tp = AttributeFinish(sp, sp->tp);
        sp->sb->declcharpos = charindex;
        sp->sb->declline = sp->sb->origdeclline = declline;
        sp->sb->realdeclline = realdeclline;
        sp->sb->declfile = sp->sb->origdeclfile = lex->data->errfile;
        sp->sb->declfilenum = lex->data->linedata->fileindex;
        if (storage_class == StorageClass::member || storage_class == StorageClass::mutable_)
            sp->sb->parentClass = getStructureDeclaration();
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
    else if (scoped != (bool)sp->tp->scoped || (fixedType && sp->tp->btp->type != fixedType->type))
    {
        error(ERR_REDEFINITION_OF_ENUMERATION_SCOPE_OR_BASE_TYPE);
    }
    if (noname && (scoped || opaque || KW(lex) != Keyword::_begin))
    {
        error(ERR_ENUMERATED_TYPE_NEEDS_NAME);
    }
    if (KW(lex) == Keyword::_begin)
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
        sp->name = AnonymousTypeName(sp, Optimizer::cparams.prm_cplusplus && !sp->sb->parentClass ? globalNameSpace->front()->tags : table);
        SetLinkerNames(sp, Linkage::cdecl_);
        browse_variable(sp);
        (Optimizer::cparams.prm_cplusplus && !sp->sb->parentClass ? globalNameSpace->front()->tags : table)->Add(sp);     
    }
    basisAttribs = oldAttribs;
    return lex;
}
static LEXLIST* getStorageClass(LEXLIST* lex, SYMBOL* funcsp, StorageClass* storage_class, Linkage* linkage,
                                Optimizer::ADDRESS* address, bool* blocked, bool* isExplicit, AccessLevel access)
{
    (void)access;
    bool found = false;
    enum StorageClass oldsc;
    while (KWTYPE(lex, TT_STORAGE_CLASS))
    {
        switch (KW(lex))
        {
            case Keyword::_extern:
                oldsc = *storage_class;
                if (*storage_class == StorageClass::parameter || *storage_class == StorageClass::member || *storage_class == StorageClass::mutable_)
                    errorstr(ERR_INVALID_STORAGE_CLASS, lex->data->kw->name);
                else
                    *storage_class = StorageClass::external;
                lex = getsym();
                if (Optimizer::cparams.prm_cplusplus)
                {
                    if (lex->data->type == l_astr)
                    {
                        Optimizer::SLCHAR* ch = (Optimizer::SLCHAR*)lex->data->value.s.w;
                        char buf[256];
                        int i;
                        enum Linkage next = Linkage::none_;
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
                            if (MATCHKW(lex, Keyword::_begin))
                            {
                                *blocked = true;
                                lex = getsym();
                                while (lex && !MATCHKW(lex, Keyword::_end))
                                {
                                    lex =
                                        declare(lex, nullptr, nullptr, StorageClass::global, *linkage, emptyBlockdata, true, false, false, AccessLevel::public_);
                                }
                                needkw(&lex, Keyword::_end);
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
            case Keyword::_virtual:
                if (*storage_class != StorageClass::member)
                {
                    errorstr(ERR_INVALID_STORAGE_CLASS, lex->data->kw->name);
                }
                else
                {
                    *storage_class = StorageClass::virtual_;
                }
                lex = getsym();
                break;
            case Keyword::_explicit:
                if (*storage_class != StorageClass::member)
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
            case Keyword::_mutable:
                if (*storage_class != StorageClass::member)
                {
                    error(ERR_EXPLICIT_CONSTRUCTOR_OR_CONVERSION_FUNCTION);
                }
                else
                {
                    *storage_class = StorageClass::mutable_;
                }
                lex = getsym();
                break;
            case Keyword::_static:
                if (*storage_class == StorageClass::parameter ||
                    (!Optimizer::cparams.prm_cplusplus && (*storage_class == StorageClass::member || *storage_class == StorageClass::mutable_)))
                    errorstr(ERR_INVALID_STORAGE_CLASS, lex->data->kw->name);
                else if (*storage_class == StorageClass::auto_)
                    *storage_class = StorageClass::localstatic;
                else
                    *storage_class = StorageClass::static_;
                lex = getsym();
                break;
            case Keyword::__absolute:
                if (*storage_class == StorageClass::parameter || *storage_class == StorageClass::member || *storage_class == StorageClass::mutable_)
                    errorstr(ERR_INVALID_STORAGE_CLASS, lex->data->kw->name);
                else
                    *storage_class = StorageClass::absolute;
                lex = getsym();
                if (MATCHKW(lex, Keyword::_openpa))
                {
                    TYPE* tp = nullptr;
                    EXPRESSION* exp = nullptr;
                    lex = getsym();
                    lex = optimized_expression(lex, funcsp, nullptr, &tp, &exp, false);
                    if (tp && isintconst(exp))
                        *address = exp->v.i;
                    else
                        error(ERR_CONSTANT_VALUE_EXPECTED);
                    if (!MATCHKW(lex, Keyword::_closepa))
                        needkw(&lex, Keyword::_closepa);
                    lex = getsym();
                }
                else
                    error(ERR_ABSOLUTE_NEEDS_ADDRESS);
                break;
            case Keyword::_auto:
                if (*storage_class != StorageClass::auto_)
                    errorstr(ERR_INVALID_STORAGE_CLASS, "auto");
                else
                    *storage_class = StorageClass::auto_;
                lex = getsym();
                break;
            case Keyword::_register:
                if (*storage_class != StorageClass::auto_ && *storage_class != StorageClass::parameter)
                    errorstr(ERR_INVALID_STORAGE_CLASS, "register");
                else if (*storage_class != StorageClass::parameter)
                    *storage_class = StorageClass::register_;
                lex = getsym();
                break;
            case Keyword::_typedef:
                if (*storage_class == StorageClass::parameter)
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
static LEXLIST* getPointerQualifiers(LEXLIST* lex, TYPE** tp, bool allowstatic)
{
    while (KWTYPE(lex, TT_TYPEQUAL) || (allowstatic && MATCHKW(lex, Keyword::_static)))
    {
        TYPE* tpn;
        TYPE* tpl;
        if (MATCHKW(lex, Keyword::__intrinsic))
        {
            lex = getsym();
            continue;
        }
        tpn = Allocate<TYPE>();
        if (*tp)
            tpn->size = (*tp)->size;
        switch (KW(lex))
        {
            case Keyword::_static:
                tpn->type = BasicType::static_;
                break;
            case Keyword::_const:
                tpn->type = BasicType::const_;
                break;
            case Keyword::_atomic:
                lex = getsym();
                if (MATCHKW(lex, Keyword::_openpa))
                {
                    // being used as  a type specifier not a qualifier
                    lex = backupsym();
                    return lex;
                }
                lex = backupsym();
                tpn->type = BasicType::atomic;
                break;
            case Keyword::_volatile:
                tpn->type = BasicType::volatile_;
                break;
            case Keyword::___restrict:
                tpn->type = BasicType::restrict_;
                break;
            case Keyword::__far:
                tpn->type = BasicType::far;
                break;
            case Keyword::__near:
                tpn->type = BasicType::near;
                break;
            case Keyword::___va_list__:
                tpn->type = BasicType::va_list;
                break;
            default:
                break;
        }
        /*
                tpl = *tp;
                while (tpl && tpl->type != BasicType::pointer)
                {
                    if (tpl->type == tpn->type)
                        errorstr(ERR_DUPLICATE_TYPE_QUALIFIER, lex->data->kw->name);
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
LEXLIST* parse_declspec(LEXLIST* lex, Linkage* linkage, Linkage* linkage2, Linkage* linkage3)
{
    (void)linkage;
    if (needkw(&lex, Keyword::_openpa))
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
                    if (needkw(&lex, Keyword::_openpa))
                    {
                        TYPE* tp = nullptr;
                        EXPRESSION* exp = nullptr;

                        lex = optimized_expression(lex, nullptr, nullptr, &tp, &exp, false);
                        if (!tp || !isint(tp))
                            error(ERR_NEED_INTEGER_TYPE);
                        else if (!isintconst(exp))
                            error(ERR_CONSTANT_VALUE_EXPECTED);
                        int align = exp->v.i;
                        LEXLIST* pos = lex;
                        if (needkw(&lex, Keyword::_closepa))
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
                else if (!strcmp(lex->data->value.s.a, "deprecated") || !strcmp(lex->data->value.s.a, "__deprecated__"))
                {
                    basisAttribs.uninheritable.deprecationText = (char*)-1;
                }
                else
                {
                    error(ERR_IGNORING__DECLSPEC);
                }
            }
            else if (MATCHKW(lex, Keyword::_noreturn))
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
            if (MATCHKW(lex, Keyword::_openpa))
            {
                errskim(&lex, skim_closepa);
            }
            if (!MATCHKW(lex, Keyword::_comma))
                break;
            lex = getsym();
        }
        needkw(&lex, Keyword::_closepa);
    }
    return lex;
}
static LEXLIST* getLinkageQualifiers(LEXLIST* lex, Linkage* linkage, Linkage* linkage2, Linkage* linkage3)
{
    while (KWTYPE(lex, TT_LINKAGE))
    {
        Keyword kw = KW(lex);
        lex = getsym();
        switch (kw)
        {
            case Keyword::___cdecl:
                if (*linkage != Linkage::none_ && *linkage != Linkage::cdecl_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = Linkage::cdecl_;
                break;
            case Keyword::___stdcall:
                if (*linkage != Linkage::none_ && *linkage != Linkage::stdcall_ && (!Optimizer::cparams.prm_cplusplus || *linkage != Linkage::c_))
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = Linkage::stdcall_;
                break;
            case Keyword::___fastcall:
                if (*linkage != Linkage::none_ && *linkage != Linkage::fastcall_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = Linkage::fastcall_;
                break;
            case Keyword::__interrupt:
                if (*linkage != Linkage::none_ && *linkage != Linkage::interrupt_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = Linkage::interrupt_;
                break;
            case Keyword::seh_fault_:
                if (*linkage != Linkage::none_ && *linkage != Linkage::fault_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = Linkage::fault_;
                break;
            case Keyword::___inline:
                if (*linkage != Linkage::none_ && *linkage != Linkage::inline_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = Linkage::inline_;
                break;
            case Keyword::_noreturn:
                if (*linkage3 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage3 = Linkage::noreturn_;
                break;
            case Keyword::_thread_local:
                if (*linkage3 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage3 = Linkage::threadlocal_;
                break;
            case Keyword::__exportx:
                if (*linkage2 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage2 = Linkage::export_;
                break;
            case Keyword::___declspec:
                lex = parse_declspec(lex, linkage, linkage2, linkage3);
                break;
            case Keyword::___rtllinkage:
                *linkage2 = getDefaultLinkage();
                break;
            case Keyword::__entrypoint:
                if (*linkage3 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage3 = Linkage::entrypoint_;
                break;

            case Keyword::__property:
                if (*linkage2 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage2 = Linkage::property_;
                break;
            case Keyword::___msil_rtl:
                if (*linkage2 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage2 = Linkage::msil_rtl_;
                break;
            case Keyword::___unmanaged:
                if (*linkage2 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage2 = Linkage::unmanaged_;
                break;
            case Keyword::__importx:
                if (*linkage2 != Linkage::none_)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage2 = Linkage::import_;
                importFile = nullptr;
                if (MATCHKW(lex, Keyword::_openpa))
                {
                    lex = getsym();
                    if (lex->data->type == l_astr)
                    {
                        int i, len = ((Optimizer::SLCHAR*)lex->data->value.s.w)->count;
                        importFile = Allocate<char>(len + 1);
                        for (i = 0; i < len; i++)
                            importFile[i] = (char)((Optimizer::SLCHAR*)lex->data->value.s.w)->str[i];
                        lex = getsym();
                    }
                    needkw(&lex, Keyword::_closepa);
                }
                break;
            default:
                break;
        }
    }
    return lex;
}
LEXLIST* getQualifiers(LEXLIST* lex, TYPE** tp, Linkage* linkage, Linkage* linkage2, Linkage* linkage3, bool* asFriend)
{
    while (KWTYPE(lex, (TT_TYPEQUAL | TT_LINKAGE)))
    {
        if (asFriend && MATCHKW(lex, Keyword::_friend))
        {
            *asFriend = true;
            lex = getsym();
        }
        else
        {
            lex = getPointerQualifiers(lex, tp, false);
            if (MATCHKW(lex, Keyword::_atomic))
                break;
            lex = getLinkageQualifiers(lex, linkage, linkage2, linkage3);
        }
    }
    ParseAttributeSpecifiers(&lex, theCurrentFunc, true);
    return lex;
}
static LEXLIST* nestedTypeSearch(LEXLIST* lex, SYMBOL** sym)
{
    *sym = nullptr;
    lex = nestedSearch(lex, sym, nullptr, nullptr, nullptr, nullptr, false, StorageClass::global, false, true);
    if (!*sym || !istype((*sym)))
    {
        error(ERR_TYPE_NAME_EXPECTED);
    }
    return lex;
}
static bool isPointer(LEXLIST* lex)
{
    while (KWTYPE(lex, (TT_TYPEQUAL | TT_LINKAGE)))
        lex = getsym();
    if (ISKW(lex))
        switch (KW(lex))
        {
            case Keyword::_and:
            case Keyword::_land:
            case Keyword::_star:
                return true;
            default:
                return false;
        }
    return false;
}
LEXLIST* getBasicType(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, SYMBOL** strSym_out, bool inTemplate, StorageClass storage_class,
                      enum Linkage* linkage_in, Linkage* linkage2_in, Linkage* linkage3_in, AccessLevel access, bool* notype,
                      bool* defd, int* consdest, bool* templateArg, bool isTypedef, bool templateErr, bool inUsing, bool asfriend,
                      bool constexpression)
{
    enum BasicType type = BasicType::none;
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
    enum Linkage linkage = Linkage::none_;
    enum Linkage linkage2 = Linkage::none_;
    enum Linkage linkage3 = Linkage::none_;

    *defd = false;
    while (KWTYPE(lex, TT_BASETYPE) || MATCHKW(lex, Keyword::_decltype))
    {
        if (foundtypeof)
            flagerror = true;
        switch (KW(lex))
        {
            case Keyword::_int:
                if (foundint)
                    flagerror = true;
                foundint = true;
                switch (type)
                {
                    case BasicType::unsigned_:
                    case BasicType::short_:
                    case BasicType::unsigned_short:
                    case BasicType::long_:
                    case BasicType::unsigned_long:
                    case BasicType::long_long:
                    case BasicType::unsigned_long_long:
                    case BasicType::inative:
                    case BasicType::unative:
                        break;
                    case BasicType::none:
                    case BasicType::signed_:
                        type = BasicType::int_;
                        break;
                    default:
                        flagerror = true;
                }
                break;
            case Keyword::_native:
                if (type == BasicType::unsigned_)
                    type = BasicType::unative;
                else if (type == BasicType::int_ || type == BasicType::none)
                    type = BasicType::inative;
                else
                    flagerror = true;
                break;
            case Keyword::_char:
                switch (type)
                {
                    case BasicType::none:
                        type = BasicType::char_;
                        break;
                    case BasicType::int_:
                    case BasicType::signed_:
                        type = BasicType::signed_char;
                        break;
                    case BasicType::unsigned_:
                        type = BasicType::unsigned_char;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case Keyword::_char16_t:
                if (type != BasicType::none)
                    flagerror = true;
                type = BasicType::char16_t_;
                break;
            case Keyword::_char32_t:
                if (type != BasicType::none)
                    flagerror = true;
                type = BasicType::char32_t_;
                break;
            case Keyword::_short:
                switch (type)
                {
                    case BasicType::none:
                    case BasicType::int_:
                    case BasicType::signed_:
                        type = BasicType::short_;
                        break;
                    case BasicType::unsigned_:
                        type = BasicType::unsigned_short;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case Keyword::_long:
                switch (type)
                {
                    case BasicType::double_:
                        if (iscomplex)
                            type = BasicType::long_double_complex;
                        else if (imaginary)
                            type = BasicType::long_double_imaginary;
                        else
                            type = BasicType::long_double;
                        break;
                    case BasicType::none:
                    case BasicType::int_:
                    case BasicType::signed_:
                        type = BasicType::long_;
                        break;
                    case BasicType::unsigned_:
                        type = BasicType::unsigned_long;
                        break;
                    case BasicType::long_:
                        type = BasicType::long_long;
                        break;
                    case BasicType::unsigned_long:
                        type = BasicType::unsigned_long_long;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case Keyword::_signed:
                if (foundsigned || foundunsigned)
                    flagerror = true;
                if (type == BasicType::none)
                    type = BasicType::signed_;
                else if (type == BasicType::char_)
                    type = BasicType::signed_char;
                foundsigned++;
                break;
            case Keyword::_unsigned:
                if (foundsigned || foundunsigned)
                    flagerror = true;
                foundunsigned++;
                switch (type)
                {
                    case BasicType::char_:
                        type = BasicType::unsigned_char;
                        break;
                    case BasicType::short_:
                        type = BasicType::unsigned_short;
                        break;
                    case BasicType::long_:
                        type = BasicType::unsigned_long;
                        break;
                    case BasicType::long_long:
                        type = BasicType::unsigned_long_long;
                        break;
                    case BasicType::none:
                    case BasicType::int_:
                        type = BasicType::unsigned_;
                        break;
                    case BasicType::inative:
                        type = BasicType::unative;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case Keyword::___int64:
                switch (type)
                {
                    case BasicType::unsigned_:
                        int64 = true;
                        type = BasicType::unsigned_long_long;
                        break;
                    case BasicType::none:
                    case BasicType::signed_:
                        int64 = true;
                        type = BasicType::long_long;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case Keyword::_wchar_t:
                if (type == BasicType::none)
                    type = BasicType::wchar_t_;
                else
                    flagerror = true;
                break;
            case Keyword::_auto:
                if (type == BasicType::none)
                    type = BasicType::auto_;
                else
                    flagerror = true;
                break;
            case Keyword::_bool:
                if (type == BasicType::none)
                    if (!(Optimizer::chosenAssembler->arch->boolmode & ABM_USESIZE))
                    {
                        if (storage_class == StorageClass::auto_)
                            if (!(Optimizer::chosenAssembler->arch->boolmode & ABM_LOCALBITS))
                            {
                                type = BasicType::bool_;
                            }
                            else
                            {
                                type = BasicType::bit;
                            }
                        else if (storage_class == StorageClass::global)
                        {
                            if (!(Optimizer::chosenAssembler->arch->boolmode & ABM_GLOBALBITS))
                            {
                                type = BasicType::bool_;
                            }
                            else
                            {
                                type = BasicType::bit;
                            }
                        }
                        else
                            type = BasicType::bool_;
                    }
                    else
                        type = BasicType::bool_;
                else
                    flagerror = true;
                break;
            case Keyword::_float:
                if (type == BasicType::none)
                    if (iscomplex)
                        type = BasicType::float__complex;
                    else if (imaginary)
                        type = BasicType::float__imaginary;
                    else
                        type = BasicType::float_;
                else
                    flagerror = true;
                break;
            case Keyword::_double:
                if (type == BasicType::none)
                    if (iscomplex)
                        type = BasicType::double__complex;
                    else if (imaginary)
                        type = BasicType::double__imaginary;
                    else
                        type = BasicType::double_;
                else if (type == BasicType::long_)
                    if (iscomplex)
                        type = BasicType::long_double_complex;
                    else if (imaginary)
                        type = BasicType::long_double_imaginary;
                    else
                        type = BasicType::long_double;
                else
                    flagerror = true;
                break;
            case Keyword::__Complex:
                switch (type)
                {
                    case BasicType::float_:
                        type = BasicType::float__complex;
                        break;
                    case BasicType::double_:
                        type = BasicType::double__complex;
                        break;
                    case BasicType::long_double:
                        type = BasicType::long_double_complex;
                        break;
                    case BasicType::none:
                        if (iscomplex || imaginary)
                            flagerror = true;
                        iscomplex = true;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case Keyword::__Imaginary:
                switch (type)
                {
                    case BasicType::float_:
                        type = BasicType::float__imaginary;
                        break;
                    case BasicType::double_:
                        type = BasicType::double__imaginary;
                        break;
                    case BasicType::long_double:
                        type = BasicType::long_double_imaginary;
                        break;
                    case BasicType::none:
                        if (imaginary || iscomplex)
                            flagerror = true;
                        imaginary = true;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case Keyword::___object:
                if (type != BasicType::none)
                    flagerror = true;
                else
                    type = BasicType::__object;
                break;
            case Keyword::___string:
                if (type != BasicType::none)
                    flagerror = true;
                else
                    type = BasicType::__string;
                break;
            case Keyword::_struct:
            case Keyword::_class:
            case Keyword::_union:
                inTemplateType = false;
                if (foundsigned || foundunsigned || type != BasicType::none)
                    flagerror = true;
                lex = declstruct(lex, funcsp, &tn, inTemplate, asfriend, storage_class, access, defd, constexpression);
                goto exit;
            case Keyword::_enum:
                if (foundsigned || foundunsigned || type != BasicType::none)
                    flagerror = true;
                lex = declenum(lex, funcsp, &tn, storage_class, access, false, defd);
                goto exit;
            case Keyword::_void:
                if (type != BasicType::none)
                    flagerror = true;
                type = BasicType::void_;
                break;
            case Keyword::_decltype:
                //                lex = getDeclType(lex, funcsp, &tn);
                type = BasicType::void_; /* won't really be used */
                foundtypeof = true;
                if (foundsomething)
                    flagerror = true;
                goto founddecltype;
            case Keyword::_typeof:
                type = BasicType::void_; /* won't really be used */
                foundtypeof = true;
                if (foundsomething)
                    flagerror = true;
                lex = getsym();
                if (MATCHKW(lex, Keyword::_openpa))
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
                    if (!MATCHKW(lex, Keyword::_closepa))
                        needkw(&lex, Keyword::_closepa);
                }
                else if (ISID(lex) || MATCHKW(lex, Keyword::_classsel))
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
            case Keyword::_atomic:
                lex = getsym();
                if (needkw(&lex, Keyword::_openpa))
                {
                    tn = nullptr;
                    lex = get_type_id(lex, &tn, funcsp, StorageClass::cast, false, true, false);
                    if (tn)
                    {
                        quals = MakeType(BasicType::atomic, quals);
                        auto tz = tn;
                        while (tz->type == BasicType::typedef_)
                            tz = tz->btp;
                        if (basetype(tz) != tz)
                            error(ERR_ATOMIC_TYPE_SPECIFIER_NO_QUALS);
                    }
                    else
                    {
                        error(ERR_EXPRESSION_SYNTAX);
                    }
                    if (!MATCHKW(lex, Keyword::_closepa))
                    {
                        needkw(&lex, Keyword::_closepa);
                        errskim(&lex, skim_closepa);
                        skip(&lex, Keyword::_closepa);
                    }
                }
                else
                {
                    errskim(&lex, skim_closepa);
                    skip(&lex, Keyword::_closepa);
                }
                break;
            case Keyword::___underlying_type: {
                lex = getsym();
                underlying_type(&lex, funcsp, nullptr, &tn, nullptr);
                lex = backupsym();
            }
            break;
            default:
                break;
        }
        foundsomething = true;
        lex = getsym();
        lex = getQualifiers(lex, &quals, &linkage, &linkage2, &linkage3, nullptr);
        if (linkage != Linkage::none_)
        {
            *linkage_in = linkage;
        }
        if (linkage2 != Linkage::none_)
            *linkage2_in = linkage2;
        if (linkage3 != Linkage::none_)
            *linkage3_in = linkage3;
    }
    if (type == BasicType::signed_)  // BasicType::signed_ is just an internal placeholder
        type = BasicType::int_;
founddecltype:
    if (!foundsomething)
    {
        bool typeName = false;
        type = BasicType::int_;
        if (MATCHKW(lex, Keyword::_typename))
        {
            typeName = true;
            //   lex = getsym();
        }
        if (iscomplex || imaginary)
            error(ERR_MISSING_TYPE_SPECIFIER);
        else if (ISID(lex) || MATCHKW(lex, Keyword::_classsel) || MATCHKW(lex, Keyword::_complx) || MATCHKW(lex, Keyword::_decltype) ||
                 MATCHKW(lex, Keyword::_typename))
        {
            std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
            SYMBOL* strSym = nullptr;
            SYMBOL* sp = nullptr;
            bool destructor = false;
            LEXLIST* placeholder = lex;
            bool inTemplate = false;  // hides function parameter
            lex = nestedSearch(lex, &sp, &strSym, &nsv, &destructor, &inTemplate, false, storage_class, false, true);
            if (sp && (istype(sp) || (sp->sb && ((sp->sb->storage_class == StorageClass::type && inTemplate) ||
                                                 (sp->sb->storage_class == StorageClass::typedef_ && sp->sb->templateLevel)))))
            {
                if (sp->sb && sp->sb->attribs.uninheritable.deprecationText && !isstructured(sp->tp))
                    deprecateMessage(sp);
                lex = getsym();
                if (sp->sb && sp->sb->storage_class == StorageClass::typedef_ && sp->sb->templateLevel)
                {
                    if (MATCHKW(lex, Keyword::_lt))
                    {
                        // throwaway
                        std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                        SYMBOL* sp1;
                        lex = GetTemplateArguments(lex, funcsp, sp, &lst);
                        if (!parsingTrailingReturnOrUsing)
                        {
                            sp1 = GetTypeAliasSpecialization(sp, lst);
                            if (sp1)
                            {
                                sp = sp1;
                                if (/*!inUsing || */ !templateNestingCount)
                                {
                                    if (isstructured(sp->tp))
                                        sp->tp = PerformDeferredInitialization(sp->tp, funcsp);
                                    else
                                        sp->tp = SynthesizeType(sp->tp, nullptr, false);
                                }
                            }
                        }
                        else
                        {
                            auto oldsp = sp;
                            sp = CopySymbol(sp);
                            sp->sb->mainsym = oldsp;
                            sp->tp = CopyType(sp->tp);
                            sp->tp->sp = sp;
                            sp->templateParams = lst;
                            sp->templateParams->push_front(TEMPLATEPARAMPAIR());
                            sp->templateParams->front().second = Allocate<TEMPLATEPARAM>();
                            sp->templateParams->front().second->type = Keyword::_new;
                        }

                        tn = sp->tp;
                        foundsomething = true;
                    }
                    else
                    {
                        SpecializationError(sp);
                        tn = sp->tp;
                    }
                }
                else
                {
                    SYMBOL* ssp = getStructureDeclaration();
                    TYPE* tpx = basetype(sp->tp);
                    foundsomething = true;
                    if (tpx->type == BasicType::templateparam)
                    {
                        tn = nullptr;
                        if (templateArg)
                            *templateArg = true;
                        if (!tpx->templateParam->second->packed)
                        {
                            if (tpx->templateParam->second->type == Keyword::_typename)
                            {
                                tn = tpx->templateParam->second->byClass.val;
                                if (*tp && tn)
                                {
                                    // should only be const vol specifiers
                                    TYPE* tpy = *tp;
                                    while (tpy->btp)
                                        tpy = tpy->btp;
                                    tpy->btp = tpx;
                                    // used to pass a pointer to tpx->templateParam
                                    tn = SynthesizeType(*tp, nullptr, false);
                                    *tp = nullptr;
                                }
                                if (inTemplate)
                                {
                                    if (MATCHKW(lex, Keyword::_lt))
                                    {
                                        // throwaway
                                        std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                                        lex = GetTemplateArguments(lex, funcsp, nullptr, &lst);
                                    }
                                    errorsym(ERR_NOT_A_TEMPLATE, sp);
                                }
                            }
                            else if (tpx->templateParam->second->type == Keyword::_template)
                            {
                                if (MATCHKW(lex, Keyword::_lt))
                                {

                                    std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                                    SYMBOL* sp1 = tpx->templateParam->second->byTemplate.val;
                                    lex = GetTemplateArguments(lex, funcsp, sp1, &lst);
                                    if (sp1)
                                    {
                                        sp1 = GetClassTemplate(sp1, lst, !templateErr);
                                        tn = nullptr;
                                        if (sp1)
                                        {
                                            if (sp1->tp->type == BasicType::typedef_)
                                            {
                                                tn = SynthesizeType(sp1->tp, nullptr, false);
                                            }
                                            else
                                            {
                                                tn = sp1->tp;
                                            }
                                        }
                                    }
                                    else if (templateNestingCount)
                                    {
                                        sp1 = CopySymbol(sp);
                                        sp1->tp = CopyType(sp->tp);
                                        sp1->tp->sp = sp1;
                                        // used to patch in the remainder of the list in the 'next' field
                                        sp1->tp->templateParam = Allocate<TEMPLATEPARAMPAIR>();
                                        //sp1->tp->templateParam->next = sp->tp->templateParam->next;
                                        sp1->tp->templateParam->second = Allocate<TEMPLATEPARAM>();
                                        *sp1->tp->templateParam->second = *sp->tp->templateParam->second;
                                        sp1->tp->templateParam->second->byTemplate.orig = sp->tp->templateParam;
                                        auto itl = lst->begin();
                                        auto itle = lst->end();
                                        auto ito = sp1->tp->templateParam->second->byTemplate.args->begin();
                                        auto itoe = sp1->tp->templateParam->second->byTemplate.args->end();
                                        auto tnew = templateParamPairListFactory.CreateList();
                                        while (itl != itle && ito != itoe)
                                        {
                                            if (ito->second->type == Keyword::_new)
                                            {
                                                tnew->push_back(TEMPLATEPARAMPAIR{ nullptr, ito->second });
                                            }
                                            else if (!itl->first)
                                            {
                                                tnew->push_back(TEMPLATEPARAMPAIR{ nullptr, itl->second });
                                            }
                                            else
                                            {
                                                SYMBOL* sp = classsearch(itl->first->name, false, false, false);
                                                if (sp && sp->tp->type == BasicType::templateparam)
                                                {
                                                    tnew->push_back(TEMPLATEPARAMPAIR{ sp->tp->templateParam->first, sp->tp->templateParam->second });
                                                }
                                                else
                                                {
                                                    tnew->push_back(TEMPLATEPARAMPAIR{ itl->first, itl->second });
                                                }
                                            }
                                            ++itl;
                                            ++ito;
                                        }
                                        sp1->tp->templateParam->second->byTemplate.args = tnew;
                                        sp = sp1;
                                    }
                                }
                                else
                                {
                                    SYMBOL* sp1 = tpx->templateParam->second->byTemplate.val;
                                    if (sp1 && allTemplateArgsSpecified(sp1, tpx->templateParam->second->byTemplate.args))
                                    {
                                        tn = sp1->tp;
                                    }
                                    else
                                    {

                                        tn = sp->tp;
                                    }
                                }
                            }
                            else
                            {
                                diag("getBasicType: expected typename template param");
                            }
                        }
                        else if (expandingParams && tpx->type == BasicType::templateparam && tpx->templateParam->second->byPack.pack)
                        {

                            auto packed = tpx->templateParam->second->byPack.pack->begin();
                            auto packede = tpx->templateParam->second->byPack.pack->end();
                            int i;
                            for (i = 0; i < packIndex && packed != packede; i++, ++packed);
                            if (packed != packede)
                                tn = packed->second->byClass.val;
                        }
                        if (!tn)
                            tn = sp->tp;
                    }
                    else if (tpx->type == BasicType::templatedecltype)
                    {
                        if (templateArg)
                            *templateArg = true;
                        if (!templateNestingCount)
                            TemplateLookupTypeFromDeclType(tpx);
                    }
                    else if (tpx->type == BasicType::templateselector)
                    {
                        if (templateArg)
                            *templateArg = true;
                        //                        if (!templateNestingCount)
                        //                        {
                        //                            tn = SynthesizeType(sp->tp, nullptr, false);
                        //                        }
                        //                        else
                        {
                            tn = sp->tp;
                        }
                        if ((*tpx->sp->sb->templateSelector).size() > 2 && (*tpx->sp->sb->templateSelector)[1].sp &&
                            !(*tpx->sp->sb->templateSelector)[1].isDeclType &&
                            !strcmp((*tpx->sp->sb->templateSelector)[1].sp->name, (*tpx->sp->sb->templateSelector)[2].name))
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
                                *strSym_out = (*tpx->sp->sb->templateSelector)[1].sp;
                            tn = (*tpx->sp->sb->templateSelector)[1].sp->tp;
                            *notype = true;
                            goto exit;
                        }
                        else
                        {
                            // discard template params, they've already been gathered..
                            TEMPLATESELECTOR* l = &(*tpx->sp->sb->templateSelector).back();
                            if (l->isTemplate)
                            {
                                if (MATCHKW(lex, Keyword::_lt))
                                {
                                    std::list<TEMPLATEPARAMPAIR>* current = nullptr;
                                    lex = GetTemplateArguments(lex, nullptr, nullptr, &current);
                                }
                            }
                        }
                    }
                    else
                    {
                        lex = getQualifiers(lex, &quals, &linkage, &linkage2, &linkage3, nullptr);
                        if (linkage != Linkage::none_)
                        {
                            *linkage_in = linkage;
                        }
                        if (linkage2 != Linkage::none_)
                            *linkage2_in = linkage2;
                        if (sp->sb->templateLevel)
                        {
                            std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                            if (templateNestingCount)
                                tn = sp->tp;
                            else
                                tn = nullptr;
                            if (MATCHKW(lex, Keyword::_lt))
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
                                if (!sp->sb->mainsym || (sp->sb->mainsym != strSym && sp->sb->mainsym != ssp))
                                {
                                    if (instantiatingMemberFuncClass &&
                                        instantiatingMemberFuncClass->sb->parentClass == sp->sb->parentClass)
                                        sp = instantiatingMemberFuncClass;
                                    else
                                        SpecializationError(sp);
                                }
                            }
                            if (sp)
                                tn = sp->tp;
                        }
                        else
                        {
                            if (inTemplate)
                            {
                                if (MATCHKW(lex, Keyword::_lt))
                                {
                                    // throwaway
                                    std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                                    lex = GetTemplateArguments(lex, funcsp, nullptr, &lst);
                                }
                                errorsym(ERR_NOT_A_TEMPLATE, sp);
                            }
                            if (sp->tp->type == BasicType::typedef_)
                            {
                                tn = PerformDeferredInitialization(sp->tp, funcsp);
                                if (!Optimizer::cparams.prm_cplusplus)
                                    while (tn != basetype(tn) && tn->type != BasicType::va_list)
                                        tn = tn->btp;
                            }
                            else
                            {
                                tn = sp->tp;
                            }
                            if (!templateNestingCount && isstructured(tn) && basetype(tn)->sp->sb->templateLevel &&
                                !basetype(tn)->sp->sb->instantiated)
                            {
                                sp = GetClassTemplate(basetype(tn)->sp, basetype(tn)->sp->templateParams, false);
                                if (sp)
                                    tn = PerformDeferredInitialization(sp->tp, funcsp);
                            }
                            //                        if (sp->sb->parentClass && !isAccessible(sp->sb->parentClass, ssp ? ssp :
                            //                        sp->sb->parentClass, sp, funcsp, ssp == sp->sb->parentClass ? AccessLevel::protected_ :
                            //                        AccessLevel::public_, false))
                            //                           errorsym(ERR_CANNOT_ACCESS, sp);
                        }
                        // DAL
                        if (Optimizer::cparams.prm_cplusplus && sp && MATCHKW(lex, Keyword::_openpa) &&
                            ((strSym && ((strSym->sb->mainsym && strSym->sb->mainsym == sp->sb->mainsym) ||
                                         strSym == sp->sb->mainsym || sameTemplate(strSym->tp, sp->tp))) ||
                             (!strSym && (storage_class == StorageClass::member || storage_class == StorageClass::mutable_) && ssp &&
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
            else if (strSym && basetype(strSym->tp)->type == BasicType::templateselector)
            {
                //                if (!templateNestingCount && !inTemplateSpecialization && allTemplateArgsSpecified(strSym,
                //                strSym->templateParams))
                //                    tn = SynthesizeType(strSym->tp, nullptr, false);
                //                else
                //                    tn = nullptr;
                if (!tn || tn->type == BasicType::any || basetype(tn)->type == BasicType::templateparam)
            {
                    SYMBOL* sym = (*basetype(strSym->tp)->sp->sb->templateSelector)[1].sp;
                    if ((!templateNestingCount || instantiatingTemplate) && isstructured(sym->tp) && (sym->sb && sym->sb->instantiated && !declaringTemplate(sym) && (!sym->sb->templateLevel || allTemplateArgsSpecified(sym, (*strSym->tp->sp->sb->templateSelector)[1].templateParams))))
                    {

                        errorNotMember(sym, nsv ? nsv->front() : nullptr , (*strSym->tp->sp->sb->templateSelector)[2].name);
                    }
                    tn = strSym->tp;
                }
                else
                {
                    tn = PerformDeferredInitialization(tn, funcsp);
                }
                foundsomething = true;
                lex = getsym();
            }
            else if (strSym && basetype(strSym->tp)->type == BasicType::templatedecltype)
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
                sp = classsearch(lex->data->value.s.a, false, false, true);
                if (sp)
                {
                    tn = sp->tp;
                    foundsomething = true;
                }
                dropStructureDeclaration();
                lex = getsym();
            }
            else if (MATCHKW(lex, Keyword::_decltype))
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
                tn = MakeType(BasicType::any);
                lex = getsym();
                foundsomething = true;
            }
            else if (Optimizer::cparams.prm_cplusplus)
            {
                if (typeName || ISID(lex))
                {
                    tn = MakeType(BasicType::any);
                    if (lex->data->type == l_id)
                    {
                        SYMBOL* sp = makeID(StorageClass::global, tn, nullptr, litlate(lex->data->value.s.a));
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
        if (!foundsomething && (Optimizer::cparams.c_dialect >= Dialect::c99 || Optimizer::cparams.prm_cplusplus))
        {
            if (notype)
                *notype = true;
            else
            {
                error(ERR_MISSING_TYPE_SPECIFIER);
            }
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
    if (Optimizer::cparams.c_dialect < Dialect::c99)
        switch (type)
        {
            case BasicType::bool_:
                if (!Optimizer::cparams.prm_cplusplus)
                    errorstr(ERR_TYPE_C99, "_Bool");
                break;
            case BasicType::long_long:
            case BasicType::unsigned_long_long:
                if (!int64 && !Optimizer::cparams.prm_cplusplus)
                    errorstr(ERR_TYPE_C99, "long long");
                break;
            case BasicType::float__complex:
            case BasicType::double__complex:
            case BasicType::long_double_complex:
                errorstr(ERR_TYPE_C99, "_Complex");
                break;
            case BasicType::long_double_imaginary:
            case BasicType::float__imaginary:
            case BasicType::double__imaginary:
                errorstr(ERR_TYPE_C99, "_Imaginary");
                break;
            default:
                break;
        }
    if (flagerror)
        error(ERR_TOO_MANY_TYPE_SPECIFIERS);
    if (!tn)
    {
        tn = MakeType(type);
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
            int n = getAlign(StorageClass::global, &stdchar32tptr);
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
static LEXLIST* getArrayType(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, StorageClass storage_class, bool* vla, TYPE** quals, bool first,
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
    if (MATCHKW(lex, Keyword::_star))
    {
        if (Optimizer::cparams.c_dialect < Dialect::c99 && !Optimizer::cparams.prm_cplusplus)
            error(ERR_VLA_c99);
        if (storage_class != StorageClass::parameter)
            error(ERR_UNSIZED_VLA_PARAMETER);
        lex = getsym();
        unsized = true;
    }
    else if (!MATCHKW(lex, Keyword::_closebr))
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
        if (*quals && Optimizer::cparams.c_dialect < Dialect::c99)
        {
            error(ERR_ARRAY_QUALIFIERS_C99);
        }
        if (*quals && storage_class != StorageClass::parameter)
            error(ERR_ARRAY_QUAL_PARAMETER_ONLY);
    }
    else
    {
        if (!first) /* previous was empty */
            error(ERR_ONLY_FIRST_INDEX_MAY_BE_EMPTY);
        empty = true;
    }
    if (MATCHKW(lex, Keyword::_closebr))
    {
        TYPE* tpp, *tpb = *tp;
        lex = getsym();
        ParseAttributeSpecifiers(&lex, funcsp, true);
        if (MATCHKW(lex, Keyword::_openbr))
        {
            if (*quals)
                error(ERR_QUAL_LAST_ARRAY_ELEMENT);
            lex = getArrayType(lex, funcsp, tp, storage_class, vla, quals, false, msil);
        }
        tpp = MakeType(BasicType::pointer, *tp);
        tpp->btp->msil = msil;  // tag the base type as managed, e.g. so we can't take address of it
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
                else if (tpc->type != BasicType::templateparam && isintconst(constant) && constant->v.i <= 0 - !!getStructureDeclaration())
                    if (!templateNestingCount)
                        error(ERR_ARRAY_INVALID_INDEX);
                if (tpc->type == BasicType::templateparam)
                {
                    tpp->size = basetype(tpp->btp)->size;
                    tpp->esize = intNode(ExpressionNode::c_i, 1);
                }
                else if (isarithmeticconst(constant))
                {
                    tpp->size = basetype(tpp->btp)->size;
                    tpp->size *= constant->v.i;
                    tpp->esize = intNode(ExpressionNode::c_i, constant->v.i);
                }
                else
                {
                    if (Optimizer::cparams.c_dialect < Dialect::c99 && !Optimizer::cparams.prm_cplusplus && !templateNestingCount && !msil)
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
            tpp->esize = intNode(ExpressionNode::c_i, tpp->btp->size);
            *tp = tpp;
        }
        if (typein && isstructured(typein))
        {
            checkIncompleteArray(typein, lex->data->errfile, lex->data->errline);
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
    while (tp && tp->type == BasicType::pointer && tp->array)
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
static void matchFunctionDeclaration(LEXLIST* lex, SYMBOL* sp, SYMBOL* spo, bool checkReturn, bool asFriend)
{
    /* two oldstyle declarations aren't compared */
    if ((spo && !spo->sb->oldstyle && spo->sb->hasproto) || !sp->sb->oldstyle)
    {
        if (spo && isfunction(spo->tp))
        {
            if (checkReturn && !spo->sb->isConstructor && !spo->sb->isDestructor &&
                !comparetypes(basetype(spo->tp)->btp, basetype(sp->tp)->btp, true) &&
                !sameTemplatePointedTo(basetype(spo->tp)->btp, basetype(sp->tp)->btp))
            {
                if (!templateNestingCount || instantiatingTemplate)
                    preverrorsym(ERR_TYPE_MISMATCH_FUNC_DECLARATION, spo, spo->sb->declfile, spo->sb->declline);
            }
            else
            {
                auto ito1 = basetype(spo->tp)->syms->begin();
                auto hro1end = basetype(spo->tp)->syms->end();
                auto it1 = basetype(sp->tp)->syms->begin();
                auto hr1end = basetype(sp->tp)->syms->end();
                if (ito1 != hro1end && (*ito1)->sb->thisPtr)
                    ++ito1;
                if (ito1 != hro1end && it1 != hr1end && (*ito1)->tp)
                {
                    while (ito1 != hro1end && it1 != hr1end)
                    {
                        SYMBOL* spo1 = *ito1;
                        SYMBOL* sp1 = *it1;
                        if (!comparetypes(spo1->tp, sp1->tp, true) && !sameTemplatePointedTo(spo1->tp, sp1->tp) && !sameTemplateSelector(sp1->tp, spo1->tp))
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
                        auto ito1 = basetype(spo->tp)->syms->begin();
                        auto hro1end = basetype(spo->tp)->syms->end();
                        auto it1 = basetype(sp->tp)->syms->begin();
                        auto hr1end = basetype(sp->tp)->syms->end();
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
                        if (MATCHKW(lex, Keyword::_colon) || MATCHKW(lex, Keyword::___try) || MATCHKW(lex, Keyword::_begin))
                            basetype(spo->tp)->syms = basetype(sp->tp)->syms;
                        else
                            basetype(sp->tp)->syms = basetype(spo->tp)->syms;
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
                if (!MATCHKW(lex, Keyword::_begin))
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
                if (!MATCHKW(lex, Keyword::_begin))
                    errorsym(ERR_EXCEPTION_SPECIFIER_MUST_MATCH, sp);
            }
            else
            {
                errorsym(ERR_EXCEPTION_SPECIFIER_MUST_MATCH, sp);
            }
        }
    }
}
LEXLIST* getDeferredData(LEXLIST* lex, LEXLIST** savePos, bool braces)
{
    LEXLIST** cur = savePos, * last = nullptr;
    int paren = 0;
    int brack = 0;
    int ltgt = 0;
    while (lex != nullptr)
    {
        Keyword kw = KW(lex);
        if (braces)
        {
            if (kw == Keyword::_begin)
            {
                paren++;
            }
            else if (kw == Keyword::_end && !--paren)
            {
                *cur = Allocate<LEXLIST>();
                **cur = *lex;
                (*cur)->prev = last;
                last = *cur;
                lex = getsym();
                break;
            }
        }
        else
        {
            if (kw == Keyword::_semicolon)
            {
                break;
            }
            else if (kw == Keyword::_openpa)
            {
                paren++;
            }
            else if (kw == Keyword::_closepa)
            {
                if (paren-- == 0 && !brack)
                {
                    break;
                }
            }
            else if (kw == Keyword::_openbr)
            {
                brack++;
            }
            else if (kw == Keyword::_closebr)
            {
                brack--;
            }
            else if (kw == Keyword::_comma && !paren && !brack && !ltgt)
            {
                break;
            }
            // there is some ambiguity between templates and <
            else if (kw == Keyword::_lt)
            {
                ltgt++;
            }
            else if (kw == Keyword::_gt)
            {
                ltgt--;
            }
        }
        *cur = Allocate<LEXLIST>();
        if (lex->data->type == l_id)
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
LEXLIST* getFunctionParams(LEXLIST* lex, SYMBOL* funcsp, SYMBOL** spin, TYPE** tp, bool inTemplate, StorageClass storage_class,
                           bool funcptr)
{
    (void)storage_class;
    SYMBOL* sp = *spin;
    SYMBOL* spi;
    TYPE* tp1;
    bool isvoid = false;
    bool pastfirst = false;
    bool voiderror = false;
    bool hasellipse = false;
    LEXLIST* placeholder = lex;
    STRUCTSYM s;
    NAMESPACEVALUEDATA internalNS = {};
    SymbolTable<SYMBOL>* symbolTable = symbols.CreateSymbolTable();
    s.tmpl = nullptr;
    lex = getsym();
    if (*tp == nullptr)
        *tp = &stdint;
    tp1 = MakeType(BasicType::func, *tp);
    tp1->size = getSize(BasicType::pointer);
    tp1->sp = sp;
    sp->tp = *tp = tp1;
    localNameSpace->push_front(&internalNS);
    localNameSpace->front()->syms = tp1->syms = symbolTable;
    attributes oldAttribs = basisAttribs;

    basisAttribs = {0};
    ParseAttributeSpecifiers(&lex, funcsp, true);
    bool structured = false;
    if (startOfType(lex, &structured, true) && (!Optimizer::cparams.prm_cplusplus || resolveToDeclaration(lex, structured)) ||
        MATCHKW(lex, Keyword::_constexpr))
    {
        sp->sb->hasproto = true;
        while (startOfType(lex, nullptr, true) || MATCHKW(lex, Keyword::_ellipse) || MATCHKW(lex, Keyword::_constexpr))
        {
            if (MATCHKW(lex, Keyword::_constexpr))
            {
                lex = getsym();
                error(ERR_CONSTEXPR_NO_PARAM);
            }
            bool templType = inTemplateType;
            inTemplateType = !!templateNestingCount;
            if (MATCHKW(lex, Keyword::_ellipse))
            {
                if (!pastfirst)
                    break;
                ;
                if (hasellipse)
                    break;
                if (isvoid)
                    voiderror = true;
                spi = makeID(StorageClass::parameter, tp1, nullptr, NewUnnamedID());
                spi->sb->anonymous = true;
                SetLinkerNames(spi, Linkage::none_);
                spi->tp = MakeType(BasicType::ellipse);
                (*tp)->syms->Add(spi);
                lex = getsym();
                hasellipse = true;
            }
            else
            {
                enum StorageClass storage_class = StorageClass::parameter;
                bool blocked;
                bool constexpression;
                Optimizer::ADDRESS address;
                TYPE* tpb;
                enum Linkage linkage = Linkage::none_;
                enum Linkage linkage2 = Linkage::none_;
                enum Linkage linkage3 = Linkage::none_;
                bool defd = false;
                bool notype = false;
                bool clonedParams = false;
                TYPE* tp2;
                spi = nullptr;
                tp1 = nullptr;

                noTypeNameError++;
                lex = getStorageAndType(lex, funcsp, nullptr, false, true, &storage_class, &storage_class, &address, &blocked,
                                        nullptr, &constexpression, &tp1, &linkage, &linkage2, &linkage3, AccessLevel::public_, &notype, &defd,
                                        nullptr, nullptr, nullptr);
                noTypeNameError--;
                if (!basetype(tp1))
                    error(ERR_TYPE_NAME_EXPECTED);
                else if (isautotype(tp1) && !lambdas.size())
                    error(ERR_AUTO_NOT_ALLOWED_IN_PARAMETER);
                else if (Optimizer::cparams.prm_cplusplus && isstructured((*tp)->btp) && (MATCHKW(lex, Keyword::_openpa) || MATCHKW(lex, Keyword::_begin)))
                {
                    LEXLIST* cur = lex;
                    lex = getsym();
                    if (!MATCHKW(lex, Keyword::_star) && !MATCHKW(lex, Keyword::_and) && !startOfType(lex, nullptr, true))
                    {
                        if (*spin)
                        {
                            (*spin)->tp = (*tp) = (*tp)->btp;
                            // constructor initialization
                            // will do initialization later...
                        }
                        localNameSpace->pop_front();
                        lex = prevsym(placeholder);
                        return lex;
                    }
                    lex = prevsym(cur);
                }
                lex = getBeforeType(lex, funcsp, &tp1, &spi, nullptr, nullptr, false, storage_class, &linkage, &linkage2, &linkage3,
                                    nullptr, false, false, false, false);
                if (!templateNestingCount && !structLevel)
                {
                    tp1 = PerformDeferredInitialization(tp1, funcsp);
                }
                if (!spi)
                {
                    spi = makeID(StorageClass::parameter, tp1, nullptr, NewUnnamedID());
                    spi->sb->anonymous = true;
                    SetLinkerNames(spi, Linkage::none_);
                }
                spi->sb->parent = sp;
                tp1 = AttributeFinish(spi, tp1);
                tp2 = tp1;
                while (ispointer(tp2) || isref(tp2))
                    tp2 = basetype(tp2)->btp;
                tp2 = basetype(tp2);
                if (tp2->type == BasicType::templateparam && tp2->templateParam->second->packed)
                {
                    spi->packed = true;
                    if (spi->sb->anonymous && MATCHKW(lex, Keyword::_ellipse))
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
                    if (!templateNestingCount || (!funcptr && tp2->type == BasicType::templateparam && tp2->templateParam->first &&
                                                  !inCurrentTemplate(tp2->templateParam->first->name) &&
                                                  definedInTemplate(tp2->templateParam->first->name)))
                    {
                        if (tp2->templateParam && tp2->templateParam->second->packed)
                        {
                            TYPE* newtp1 = Allocate<TYPE>();
                            std::list<TEMPLATEPARAMPAIR>* templateParams = tp2->templateParam->second->byPack.pack;
                            auto tpp = Allocate < TEMPLATEPARAMPAIR>();
                            TEMPLATEPARAM* tpnew = Allocate<TEMPLATEPARAM>();
                            std::list<TEMPLATEPARAMPAIR>* newPack = templateParamPairListFactory.CreateList();
                            bool first = true;
                            *tpnew = *tp2->templateParam->second;
                            *tpp = TEMPLATEPARAMPAIR{ tp2->templateParam->first, tpnew };
                            *newtp1 = *tp2;
                            newtp1->templateParam = tpp;
                            tp1 = newtp1;
                            tp1->templateParam->second->byPack.pack = newPack;
                            tp1->templateParam->second->index = 0;
                            if (templateParams)
                            {
                                for (auto&& tpp : *templateParams)
                                {
                                    SYMBOL* clone = CopySymbol(spi);

                                    clone->tp = CopyType(clone->tp, true, [&tpp](TYPE*& old, TYPE*& newx) {
                                        if (old->type == BasicType::templateparam)
                                        {
                                            old = tpp.second->byClass.val;
                                            *newx = *old;
                                        }
                                    });
                                    CollapseReferences(clone->tp);
                                    SetLinkerNames(clone, Linkage::none_);
                                    sizeQualifiers(clone->tp);
                                    if (!first)
                                    {
                                        clone->name = clone->sb->decoratedName = AnonymousName();
                                        clone->packed = false;
                                    }
                                    else
                                    {
                                        clone->synthesized = true;
                                        clone->tp->templateParam = tp1->templateParam;
                                    }
                                    auto second = Allocate<TEMPLATEPARAM>();
                                    *second = *tpp.second;
                                    second->packsym = clone;
                                    newPack->push_back(TEMPLATEPARAMPAIR{ nullptr, second });
                                    (*tp)->syms->Add(clone);
                                    first = false;
                                    tp1->templateParam->second->index++;
                                    UpdateRootTypes(clone->tp);
                                }
                            }
                            else
                            {
                                SYMBOL* clone = CopySymbol(spi);
                                clone->tp = CopyType(clone->tp);
                                clone->tp->templateParam = tp1->templateParam;
                                SetLinkerNames(clone, Linkage::none_);
                                UpdateRootTypes(clone->tp);
                                sizeQualifiers(clone->tp);
                                (*tp)->syms->Add(clone);
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
                        tp1 = MakeType(BasicType::pointer, tp1);
                    }
                    if (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, Keyword::_assign))
                    {
                        if (storage_class == StorageClass::member || storage_class == StorageClass::mutable_ || structLevel ||
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
                                sym = anonymousVar(StorageClass::auto_, tp2)->v.sp;
                                anonymousNotAlloc--;
                                sym->sb->stackblock = !isref(spi->tp);
                                lex = initialize(lex, funcsp, sym, StorageClass::auto_, true, false, 0); /* also reserves space */
                                spi->sb->init = sym->sb->init;
                                if (spi->sb->init->front()->exp && spi->sb->init->front()->exp->type == ExpressionNode::thisref)
                                {
                                    EXPRESSION** expr = &spi->sb->init->front()->exp->left->v.func->thisptr;
                                    if (*expr && (*expr)->type == ExpressionNode::add && isconstzero(&stdint, (*expr)->right))
                                        spi->sb->init->front()->exp->v.t.thisptr = (*expr) = (*expr)->left;
                                }
                            }
                            else
                            {
                                lex = initialize(lex, funcsp, spi, StorageClass::auto_, true, false, 0); /* also reserves space */
                            }
                            if (spi->sb->init)
                            {
                                checkDefaultArguments(spi);
                            }
                            inDefaultParam--;
                        }
                        spi->sb->defaultarg = true;
                        if (isfuncptr(spi->tp) && spi->sb->init && lvalue(spi->sb->init->front()->exp))
                            error(ERR_NO_POINTER_TO_FUNCTION_DEFAULT_ARGUMENT);
                        if (sp->sb->storage_class == StorageClass::typedef_)
                            error(ERR_NO_DEFAULT_ARGUMENT_IN_TYPEDEF);
                    }
                    SetLinkerNames(spi, Linkage::none_);
                    spi->tp = tp1;
                    (*tp)->syms->Add(spi);
                    tpb = basetype(tp1);
                    if (tpb->array)
                    {
                        if (tpb->vla)
                        {
                            auto tpx = MakeType(BasicType::pointer, tpb);
                            TYPE* tpn = tpb;
                            tpx->size = tpb->size = getSize(BasicType::pointer) + getSize(BasicType::unsigned_) * 2;
                            while (tpn->vla)
                            {
                                tpx->size += getSize(BasicType::unsigned_);
                                tpb->size += getSize(BasicType::unsigned_);
                                tpn->sp = spi;
                                tpn = tpn->btp;
                            }
                        }
                    }
                    if (tpb->type == BasicType::void_)
                        if (pastfirst || !spi->sb->anonymous)
                            voiderror = true;
                        else
                            isvoid = true;
                    else if (isvoid)
                        voiderror = true;
                }
            }
            inTemplateType = templType;
            if (!MATCHKW(lex, Keyword::_comma) && (!Optimizer::cparams.prm_cplusplus || !MATCHKW(lex, Keyword::_ellipse)))
                break;
            if (MATCHKW(lex, Keyword::_comma))
                lex = getsym();
            pastfirst = true;
            basisAttribs = {0};
            ParseAttributeSpecifiers(&lex, funcsp, true);
        }
        if (!needkw(&lex, Keyword::_closepa))
        {
            errskim(&lex, skim_closepa);
            skip(&lex, Keyword::_closepa);
        }
        // weed out temporary syms that were added as part of the default; they will be
        // reinstated as stackblock syms later
        for (SymbolTable<SYMBOL>::iterator it = (*tp)->syms->begin(); it != (*tp)->syms->end();)
        {
            SYMBOL* sym = *it;
            auto it1 = it;
            it1++;
            if (sym->sb->storage_class != StorageClass::parameter)
                (*tp)->syms->remove(it);
            it = it1;
            
        }
    }
    else if (!Optimizer::cparams.prm_cplusplus && !(Optimizer::architecture == ARCHITECTURE_MSIL) && ISID(lex))
    {
        SYMBOL* spo = nullptr;
        sp->sb->oldstyle = true;
        if (sp->sb->storage_class != StorageClass::member && sp->sb->storage_class != StorageClass::mutable_)
        {
            spo = gsearch(sp->name);
            /* temporary for C */
            if (spo && spo->sb->storage_class == StorageClass::overloads)
                spo = spo->tp->syms->front();
            if (spo && isfunction(spo->tp) && spo->sb->hasproto)
            {
                if (!comparetypes(spo->tp->btp, sp->tp->btp, true))
                {
                    preverrorsym(ERR_TYPE_MISMATCH_FUNC_DECLARATION, spo, spo->sb->declfile, spo->sb->declline);
                }
            }
        }
        while (ISID(lex) || MATCHKW(lex, Keyword::_ellipse))
        {
            if (MATCHKW(lex, Keyword::_ellipse))
            {
                if (!pastfirst)
                    break;
                ;
                if (hasellipse)
                    break;
                if (isvoid)
                    voiderror = true;
                spi = makeID(StorageClass::parameter, tp1, nullptr, NewUnnamedID());
                spi->sb->anonymous = true;
                SetLinkerNames(spi, Linkage::none_);
                spi->tp = MakeType(BasicType::ellipse);
                lex = getsym();
                hasellipse = true;
                (*tp)->syms->Add(spi);
            }
            else
            {
                spi = makeID(StorageClass::parameter, 0, 0, litlate(lex->data->value.s.a));
                SetLinkerNames(spi, Linkage::none_);
                (*tp)->syms->Add(spi);
            }
            lex = getsym();
            if (!MATCHKW(lex, Keyword::_comma))
                break;
            lex = getsym();
        }
        if (!needkw(&lex, Keyword::_closepa))
        {
            errskim(&lex, skim_closepa);
            skip(&lex, Keyword::_closepa);
        }
        if (startOfType(lex, nullptr, false))
        {
            while (startOfType(lex, nullptr, false))
            {
                Optimizer::ADDRESS address;
                bool blocked;
                bool constexpression;
                enum Linkage linkage = Linkage::none_;
                enum Linkage linkage2 = Linkage::none_;
                enum Linkage linkage3 = Linkage::none_;
                enum StorageClass storage_class = StorageClass::parameter;
                bool defd = false;
                bool notype = false;
                tp1 = nullptr;
                lex = getStorageAndType(lex, funcsp, nullptr, false, false, &storage_class, &storage_class, &address, &blocked,
                                        nullptr, &constexpression, &tp1, &linkage, &linkage2, &linkage3, AccessLevel::public_, &notype, &defd,
                                        nullptr, nullptr, nullptr);

                while (1)
                {
                    TYPE* tpx = tp1;
                    spi = nullptr;
                    lex = getBeforeType(lex, funcsp, &tpx, &spi, nullptr, nullptr, false, StorageClass::parameter, &linkage, &linkage2,
                                        &linkage3, nullptr, false, false, false, false);
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
                        SetLinkerNames(spi, Linkage::none_);
                        if (tpx && isfunction(tpx))
                        {
                            tpx = MakeType(BasicType::pointer, tpx);
                        }
                        spi->tp = tpx;
                        tpb = basetype(tpx);
                        if (tpb->array)
                        {
                            if (tpb->vla)
                            {
                                auto tpx = MakeType(BasicType::pointer, tpb);
                                TYPE* tpn = tpb;
                                tpb = tpx;
                                tpx->size = tpb->size = getSize(BasicType::pointer) + getSize(BasicType::unsigned_) * 2;
                                while (tpn->vla)
                                {
                                    tpx->size += getSize(BasicType::unsigned_);
                                    tpb->size += getSize(BasicType::unsigned_);
                                    tpn->sp = spi;
                                    tpn = tpn->btp;
                                }
                            }
                            else
                            {
                                //								tpb->array = false;
                                //								tpb->size = getSize(BasicType::pointer);
                            }
                        }
                        sizeQualifiers(tpx);
                        spo = search((*tp)->syms, spi->name);
                        if (!spo)
                            errorsym(ERR_UNDEFINED_IDENTIFIER, spi);
                        else
                        {
                            spo->tp = tpx;
                        }
                        if (MATCHKW(lex, Keyword::_comma))
                        {
                            lex = getsym();
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                needkw(&lex, Keyword::_semicolon);
            }
        }
        if ((*tp)->syms->size())
        {
            for (auto spi : *(*tp)->syms)
            {
                if (spi->tp == nullptr)
                {
                    if (Optimizer::cparams.c_dialect >= Dialect::c99)
                        errorsym(ERR_MISSING_TYPE_FOR_PARAMETER, spi);
                    spi->tp = MakeType(BasicType::int_);
                }
            }
        }
        else if (spo)
            (*tp)->syms = spo->tp->syms;
        if (!MATCHKW(lex, Keyword::_begin))
            error(ERR_FUNCTION_BODY_EXPECTED);
    }
    else if (MATCHKW(lex, Keyword::_ellipse))
    {
        spi = makeID(StorageClass::parameter, tp1, nullptr, NewUnnamedID());
        spi->sb->anonymous = true;
        SetLinkerNames(spi, Linkage::none_);
        spi->tp = MakeType(BasicType::ellipse);
        (*tp)->syms->Add(spi);
        lex = getsym();
        if (!MATCHKW(lex, Keyword::_closepa))
        {
            error(ERR_FUNCTION_PARAMETER_EXPECTED);
            errskim(&lex, skim_closepa);
        }
        skip(&lex, Keyword::_closepa);
    }
    else if (Optimizer::cparams.prm_cplusplus || ((Optimizer::architecture == ARCHITECTURE_MSIL) &&
                                                  Optimizer::cparams.msilAllowExtensions && !MATCHKW(lex, Keyword::_closepa) && *spin))
    {
        // () is a function
        if (MATCHKW(lex, Keyword::_closepa))
        {
            spi = makeID(StorageClass::parameter, tp1, nullptr, NewUnnamedID());
            spi->sb->anonymous = true;
            spi->sb->attribs.inheritable.structAlign = getAlign(StorageClass::parameter, &stdpointer);
            SetLinkerNames(spi, Linkage::none_);
            spi->tp = MakeType(BasicType::void_);
            (*tp)->syms->Add(spi);
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
        if (sp->sb->storage_class != StorageClass::member && sp->sb->storage_class != StorageClass::mutable_)
        {
            spo = gsearch(sp->name);
            if (spo && spo->sb->storage_class == StorageClass::overloads)
            {

                if (spo->tp->syms)
                {
                    spo = spo->tp->syms->front();
                    auto symtab = spo->tp->syms;
                    if (symtab && symtab->size())
                    {
                        auto sp2 = symtab->front();
                        if (sp2->tp->type == BasicType::void_)
                        {
                            (*tp)->syms = symtab;
                        }
                    }
                }
            }
        }
        if (!MATCHKW(lex, Keyword::_closepa))
        {
            error(ERR_FUNCTION_PARAMETER_EXPECTED);
            errskim(&lex, skim_closepa);
        }
        skip(&lex, Keyword::_closepa);
    }
    localNameSpace->pop_front();
    basisAttribs = oldAttribs;
    ParseAttributeSpecifiers(&lex, funcsp, true);
    if (voiderror)
        error(ERR_VOID_ONLY_PARAMETER);
    return lex;
}
LEXLIST* getExceptionSpecifiers(LEXLIST* lex, SYMBOL* funcsp, SYMBOL* sp, StorageClass storage_class)
{
    (void)storage_class;
    switch (KW(lex))
    {
        case Keyword::_throw:
            lex = getsym();
            if (MATCHKW(lex, Keyword::_openpa))
            {
                lex = getsym();
                if (MATCHKW(lex, Keyword::_closepa))
                {
                    sp->sb->xcMode = xc_none;
                    sp->sb->noExcept = true;
                    if (!sp->sb->xc)
                        sp->sb->xc = Allocate<xcept>();
                }
                else
                {
                    sp->sb->xcMode = xc_dynamic;
                    if (!sp->sb->xc)
                        sp->sb->xc = Allocate<xcept>();
                    lex = backupsym();
                    do
                    {
                        TYPE* tp = nullptr;
                        lex = getsym();
                        if (!MATCHKW(lex, Keyword::_closepa))
                        {
                            lex = get_type_id(lex, &tp, funcsp, StorageClass::cast, false, true, false);
                            if (!tp)
                            {
                                error(ERR_TYPE_NAME_EXPECTED);
                            }
                            else
                            {
                                // this is reverse order but who cares?
                                Optimizer::LIST* p = Allocate<Optimizer::LIST>();
                                if (tp->type == BasicType::templateparam && tp->templateParam->second->packed)
                                {
                                    if (!MATCHKW(lex, Keyword::_ellipse))
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
                    } while (MATCHKW(lex, Keyword::_comma));
                }
                needkw(&lex, Keyword::_closepa);
            }
            else
            {
                needkw(&lex, Keyword::_openpa);
            }
            break;
        case Keyword::_noexcept:
            lex = getsym();
            if (MATCHKW(lex, Keyword::_openpa))
            {
                lex = getsym();
                lex = getDeferredData(lex, &sp->sb->deferredNoexcept, false);
                needkw(&lex, Keyword::_closepa);
            }
            else
            {
                sp->sb->xcMode = xc_none;
                sp->sb->noExcept = true;
                sp->sb->deferredNoexcept = (LEXLIST*)-1;
                if (!sp->sb->xc)
                    sp->sb->xc = Allocate<xcept>();
            }
            break;
        default:
            break;
    }

    return lex;
}
static LEXLIST* GetFunctionQualifiersAndTrailingReturn(LEXLIST* lex, SYMBOL* funcsp, SYMBOL** sp, TYPE** tp, StorageClass storage_class)
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
            if (!strcmp(lex->data->value.s.a, "final"))
            {
                if (foundFinal)
                    error(ERR_FUNCTION_CAN_HAVE_ONE_FINAL_OR_OVERRIDE);
                foundFinal = true;
                (*sp)->sb->isfinal = true;
                lex = getsym();
            }
            else if (!strcmp(lex->data->value.s.a, "override"))
            {
                if (foundOverride)
                    error(ERR_FUNCTION_CAN_HAVE_ONE_FINAL_OR_OVERRIDE);
                foundOverride = true;
                (*sp)->sb->isoverride = true;
                lex = getsym();
            }
            else
            {
                if (templateNestingCount)
                {
                    while (lex && ISID(lex))
                        lex = getsym();
                }
                done = true;
            }
        }
        else
            switch (KW(lex))
            {
            case Keyword::_const:
                foundConst = true;
                lex = getsym();
                break;
            case Keyword::_volatile:
                foundVolatile = true;
                lex = getsym();
                break;
            case Keyword::_and:
                foundand = true;
                lex = getsym();
                break;
            case Keyword::_land:
                foundland = true;
                lex = getsym();
                break;
            case Keyword::_throw:
            case Keyword::_noexcept:
                if (Optimizer::cparams.prm_cplusplus && *sp)
                {
                    funcLevel++;
                    lex = getExceptionSpecifiers(lex, funcsp, *sp, storage_class);
                    funcLevel--;
                }
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
        *tp = MakeType(BasicType::volatile_, *tp);
    }
    if (foundConst)
    {
        *tp = MakeType(BasicType::const_, *tp);
    }
    if (foundand)
    {
        *tp = MakeType(BasicType::lrqual, *tp);
    }
    else if (foundland)
    {
        *tp = MakeType(BasicType::rrqual, *tp);
    }
    ParseAttributeSpecifiers(&lex, funcsp, true);
    if (MATCHKW(lex, Keyword::_pointsto))
    {
        TYPE* tpx = nullptr;
        SymbolTable<SYMBOL>* locals = localNameSpace->front()->syms;
        localNameSpace->front()->syms = basetype(*tp)->syms;
        funcLevel++;
        lex = getsym();
        ParseAttributeSpecifiers(&lex, funcsp, true);
        parsingTrailingReturnOrUsing++;
        lex = get_type_id(lex, &tpx, funcsp, StorageClass::cast, false, true, false);
        parsingTrailingReturnOrUsing--;
        // weed out temporary syms that were added as part of a decltype; they will be
        // reinstated as stackblock syms later
        auto itp = localNameSpace->front()->syms->begin();
        auto itpe = localNameSpace->front()->syms->end();
        while (itp != itpe)
        {
            SYMBOL* sym = *itp;
            auto it1 = itp;
            ++it1;
            if (sym->sb->storage_class != StorageClass::parameter)
                localNameSpace->front()->syms->remove(itp);
            itp = it1;
        }
        if (tpx)
        {
            if (!isautotype(basetype(*tp)->btp))
                error(ERR_MULTIPLE_RETURN_TYPES_SPECIFIED);
            if (isarray(tpx))
            {
                TYPE* tpn = MakeType(BasicType::pointer, basetype(tpx)->btp);
                if (isconst(tpx))
                {
                    tpn = MakeType(BasicType::const_, tpn);
                }
                if (isvolatile(tpx))
                {
                    tpn = MakeType(BasicType::volatile_, tpn);
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
        localNameSpace->front()->syms = locals;
        funcLevel--;
    }
    return lex;
}
static LEXLIST* getAfterType(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, SYMBOL** sp, bool inTemplate, StorageClass storage_class,
                             int consdest, bool funcptr)
{
    bool isvla = false;
    TYPE* quals = nullptr;
    TYPE* tp1 = nullptr;
    EXPRESSION* exp = nullptr;
    if (ISKW(lex))
    {
        switch (KW(lex))
        {
            case Keyword::_openpa:
                if (*sp)
                {
                    lex = getFunctionParams(lex, funcsp, sp, tp, inTemplate, storage_class, funcptr);
                    tp1 = *tp;
                    if (tp1->type == BasicType::func)
                    {
                        *tp = (*tp)->btp;
                        lex = getAfterType(lex, funcsp, tp, sp, inTemplate, storage_class, consdest, false);
                        tp1->btp = *tp;
                        *tp = tp1;
                        if (Optimizer::cparams.prm_cplusplus)
                        {
                            lex = GetFunctionQualifiersAndTrailingReturn(lex, funcsp, sp, tp, storage_class);
                        }
                    }
                    UpdateRootTypes(*tp);
                }
                else
                {
                    error(ERR_IDENTIFIER_EXPECTED);
                }
                break;
            case Keyword::_openbr:
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
            case Keyword::_colon:
                if (consdest == CT_CONS)
                {
                    // defer to later
                }
                else if (*sp && ((*sp)->sb->storage_class == StorageClass::member || (*sp)->sb->storage_class == StorageClass::mutable_))
                {
                    // error(ERR_BIT_STRUCT_MEMBER);
                    if ((*sp)->tp)
                    {
                        if (Optimizer::cparams.prm_ansi)
                        {
                            if ((*sp)->tp->type != BasicType::int_ && (*sp)->tp->type != BasicType::unsigned_ && (*sp)->tp->type != BasicType::bool_)
                                error(ERR_ANSI_INT_BIT);
                        }
                        else if (!isint((*sp)->tp) && basetype((*sp)->tp)->type != BasicType::enum_)
                            error(ERR_BIT_FIELD_INTEGER_TYPE);
                    }
                    lex = getsym();
                    lex = optimized_expression(lex, funcsp, nullptr, &tp1, &exp, false);
                    if (tp1 && exp && isintconst(exp))
                    {
                        int n = (*tp)->size * Optimizer::chosenAssembler->arch->bits_per_mau;
                        TYPE **tp1 = tp, *tpb = basetype(*tp1);
                        *tp1 = CopyType(tpb);
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
            case Keyword::_lt:
                if (Optimizer::cparams.prm_cplusplus && inTemplate)
                {
                    std::list<TEMPLATEPARAMPAIR>* templateParams = TemplateGetParams(*sp);
                    lex = GetTemplateArguments(lex, funcsp, *sp, &templateParams->front().second->bySpecialization.types);
                    lex = getAfterType(lex, funcsp, tp, sp, inTemplate, storage_class, consdest, false);
                }
                else
                {
                    TEMPLATEPARAM* templateParam = Allocate<TEMPLATEPARAM>();
                    templateParam->type = Keyword::_new;
                    lex = GetTemplateArguments(lex, funcsp, *sp, &templateParam->bySpecialization.types);
                    lex = getAfterType(lex, funcsp, tp, sp, inTemplate, storage_class, consdest, false);
                    if (*tp && isfunction(*tp))
                    {
                        std::list<TEMPLATEPARAMPAIR>* lst = templateParamPairListFactory.CreateList();
                        lst->push_back(TEMPLATEPARAMPAIR{ nullptr, templateParam });
                        DoInstantiateTemplateFunction(*tp, sp, nullptr, nullptr, lst, true);
                        if ((*sp)->sb->attribs.inheritable.linkage4 == Linkage::virtual_)
                            basisAttribs.inheritable.linkage4 = Linkage::virtual_;
                        if (!(*sp)->templateParams)
                        {
                            (*sp)->templateParams = lst;
                        }
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
        if ((*tpl)->type == BasicType::near || (*tpl)->type == BasicType::far || (*tpl)->type == BasicType::seg)
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
LEXLIST* getBeforeType(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, SYMBOL** spi, SYMBOL** strSym, std::list<NAMESPACEVALUEDATA*>** nsv,
                       bool inTemplate, StorageClass storage_class, Linkage* linkage, Linkage* linkage2, Linkage* linkage3,
                       bool* notype, bool asFriend, int consdest, bool beforeOnly, bool funcptr)
{
    SYMBOL* sp;
    TYPE* ptype = nullptr;
    enum BasicType xtype = BasicType::none;
    LEXLIST* pos = lex;
    bool doneAfter = false;

    if ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions && MATCHKW(lex, Keyword::_openbr))
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
    if (ISID(lex) || MATCHKW(lex, Keyword::_classsel) || MATCHKW(lex, Keyword::_operator) ||
        (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, Keyword::_ellipse)))
    {
        SYMBOL* strSymX = nullptr;
        std::list<NAMESPACEVALUEDATA*>* nsvX = nullptr;
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
            if (MATCHKW(lex, Keyword::_ellipse))
            {
                pack = true;
                lex = getsym();
            }
            lex = nestedPath(lex, &strSymX, &nsvX, &throughClass, false, storage_class, false);
            inTemplateSpecialization = oldTemplateSpecialization;
            if (strSymX)
            {
                if (structLevel && strSymX->tp->type == BasicType::templateselector && !MATCHKW(lex, Keyword::_star) && !MATCHKW(lex, Keyword::_complx))
                {
                    *tp = strSymX->tp;
                    strSymX = nullptr;
                    lex = getsym();
                    if (notype)
                        *notype = false;
                }
                else
                {
                    if (strSym)
                        *strSym = strSymX;
                    s1.str = strSymX;
                    addStructureDeclaration(&s1);
                }
            }
            if (nsv)
                *nsv = nsvX;
            if (strSymX && MATCHKW(lex, Keyword::_star))
            {
                bool inparen = false;
                if (pack)
                    error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                lex = getsym();
                if (funcptr && MATCHKW(lex, Keyword::_openpa))
                {
                    inparen = true;
                    lex = getsym();
                }
                ptype = MakeType(BasicType::memberptr, *tp);
                if (strSymX->tp->type == BasicType::templateselector)
                    ptype->sp = (*strSymX->sb->templateSelector)[1].sp;
                else
                    ptype->sp = strSymX;
                UpdateRootTypes(*tp);
                *tp = ptype;
                lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, nullptr);
                if (strSym)
                    *strSym = nullptr;
                if (nsv)
                    *nsv = nullptr;
                lex = getBeforeType(lex, funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2, linkage3,
                                    nullptr, asFriend, false, beforeOnly, false);
                if (*tp && (ptype != *tp && isref(*tp)))
                {
                    error(ERR_NO_REF_POINTER_REF);
                }
                ptype->size = getSize(BasicType::pointer) + getSize(BasicType::int_) * 2;
                if (inparen)
                {
                    if (!needkw(&lex, Keyword::_closepa))
                    {
                        errskim(&lex, skim_closepa);
                        skip(&lex, Keyword::_closepa);
                    }
                }
            }
            else
            {
                char buf[512];
                int ov = 0;
                TYPE* castType = nullptr;
                if (MATCHKW(lex, Keyword::_complx))
                {
                    lex = getsym();
                    if (!ISID(lex) || !*strSym || strcmp((*strSym)->name, lex->data->value.s.a))
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
                        SetLinkerNames(sp, Linkage::none_);
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
                        sp->sb->declcharpos = lex->data->charindex;
                    *spi = sp;
                }
            }
        }
        else
        {
            inTemplateSpecialization = oldTemplateSpecialization;
            sp = makeID(storage_class, *tp, *spi, litlate(lex->data->value.s.a));
            sp->sb->declcharpos = lex->data->charindex;
            *spi = sp;
            lex = getsym();
        }
        if (inTemplate && *spi)
        {
            std::list<TEMPLATEPARAMPAIR>* templateParams;
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
        if (nsvX && nsvX->front()->name)
        {
            Optimizer::LIST* nlist;

            nameSpaceList.push_front(nsvX->front()->name);

            globalNameSpace->push_front(nsvX->front()->name->sb->nameSpaceValues->front());
        }
        else
        {
            nsvX = nullptr;
        }
        ParseAttributeSpecifiers(&lex, funcsp, true);
        if (!doneAfter)
            lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest, false);
        if (nsvX)
        {
            nameSpaceList.pop_front();
            globalNameSpace->pop_front();
        }
        if (s.tmpl)
            dropStructureDeclaration();
        if (strSymX)
            dropStructureDeclaration();
    }
    else
        switch (KW(lex))
        {
            LEXLIST* start;
            case Keyword::_openpa:
                if (beforeOnly)
                    break;
                start = lex;
                lex = getsym();
                /* in a parameter, open paren followed by a type is an  unnamed function */
                if ((storage_class == StorageClass::parameter || parsingUsing) &&
                    (MATCHKW(lex, Keyword::_closepa) ||
                     (startOfType(lex, nullptr, false) && (!ISKW(lex) || !(lex->data->kw->tokenTypes & TT_LINKAGE)))))
                {
                    TYPE* tp1;
                    if (!*spi)
                    {
                        sp = makeID(storage_class, *tp, *spi, NewUnnamedID());
                        SetLinkerNames(sp, Linkage::none_);
                        sp->sb->anonymous = true;
                        sp->sb->declcharpos = lex->data->charindex;
                        *spi = sp;
                    }
                    *tp = (*spi)->tp;
                    lex = prevsym(start);
                    tp1 = *tp;
                    if (!parsingUsing)
                    {
                        lex = getFunctionParams(lex, funcsp, spi, tp, inTemplate, storage_class, false);
                        tp1 = *tp;
                        *tp = (*tp)->btp;
                    }
                    lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest, false);
                    if (!parsingUsing)
                    {
                        tp1->btp = *tp;
                        *tp = tp1;
                    }
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
                        *tp = MakeType(BasicType::pointer, *tp);
                        name = overloadNameTab[CI_CONSTRUCTOR];
                    }
                    sp = makeID(storage_class, *tp, *spi, name);
                    sp->sb->declcharpos = lex->data->charindex;
                    *spi = sp;
                    if (*strSym)
                    {
                        s.str = *strSym;
                        addStructureDeclaration(&s);
                    }
                    lex = prevsym(start);
                    lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest, false);
                    if (*strSym)
                    {
                        dropStructureDeclaration();
                    }
                }
                else if (MATCHKW(lex, Keyword::_openbr))
                {
                    lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest, false);
                    needkw(&lex, Keyword::_closepa);
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
                                        linkage3, nullptr, asFriend, false, beforeOnly, true);
                    basisAttribs = oldAttribs;
                    if (!ptype ||
                        (!isref(ptype) && !ispointer(ptype) && !isfunction(ptype) && basetype(ptype)->type != BasicType::memberptr))
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
                    if (!needkw(&lex, Keyword::_closepa))
                    {
                        errskim(&lex, skim_closepa);
                        skip(&lex, Keyword::_closepa);
                    }
                    lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest, true);
                    if (ptype)
                    {
                        // pointer to func or pointer to memberfunc
                        TYPE* atype = *tp;
                        *tp = ptype;
                        if (isref(ptype) && basetype(atype)->array)
                            basetype(atype)->byRefArray = true;
                        while ((isref(ptype) || isfunction(ptype) || ispointer(ptype) || basetype(ptype)->type == BasicType::memberptr) &&
                               ptype->btp)
                            if (ptype->btp->type == BasicType::any)
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
                        if (atype->type == BasicType::memberptr && isfunction(atype->btp))
                            atype->size = getSize(BasicType::int_) * 2 + getSize(BasicType::pointer);

                    }
                    if (*spi)
                        (*spi)->tp = *tp;
                    if (*tp && isfunction(*tp))
                        sizeQualifiers(basetype(*tp)->btp);
                    if (*tp)
                        sizeQualifiers(*tp);
                }
                break;
            case Keyword::_star: {
                if (*tp && isref(*tp))
                {
                    error(ERR_NO_REF_POINTER_REF);
                }
                bool inparen = false;
                lex = getsym();
                if (funcptr && MATCHKW(lex, Keyword::_openpa))
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
                while (ptype && ptype->type != BasicType::pointer && xtype == BasicType::none)
                {
                    if (ptype->type == BasicType::far)
                        xtype = BasicType::pointer;
                    else if (ptype->type == BasicType::near)
                        xtype = BasicType::pointer;
                    else if (ptype->type == BasicType::seg)
                        xtype = BasicType::seg;
                    ptype = ptype->btp;
                }
                if (xtype == BasicType::none)
                    xtype = BasicType::pointer;
                *tp = MakeType(xtype, *tp);
                lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, nullptr);
                lex = getBeforeType(lex, funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2, linkage3,
                                    nullptr, asFriend, false, beforeOnly, false);
                if (inparen)
                {
                    if (!needkw(&lex, Keyword::_closepa))
                    {
                        errskim(&lex, skim_closepa);
                        skip(&lex, Keyword::_closepa);
                    }
                }
            }
            break;
            case Keyword::_and:
            case Keyword::_land:
                if (storage_class == StorageClass::catchvar)
                {
                    // already a ref;
                    lex = getsym();
                    ParseAttributeSpecifiers(&lex, funcsp, true);
                    lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, nullptr);
                    lex = getBeforeType(lex, funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2, linkage3,
                                        nullptr, asFriend, false, beforeOnly, false);
                    break;
                }
                // using the C++ reference operator as the ref keyword...
                if (Optimizer::cparams.prm_cplusplus ||
                    ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions &&
                     storage_class == StorageClass::parameter && KW(lex) == Keyword::_and))
                {
                    if (*tp && isref(*tp) && !instantiatingTemplate && !templateNestingCount)
                    {
                        error(ERR_NO_REF_POINTER_REF);
                    }
                    *tp = MakeType(MATCHKW(lex, Keyword::_and) ? BasicType::lref : BasicType::rref, *tp);
                    if (instantiatingTemplate)
                        CollapseReferences(*tp);
                    lex = getsym();
                    ParseAttributeSpecifiers(&lex, funcsp, true);
                    lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, nullptr);
                    lex = getBeforeType(lex, funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2, linkage3,
                                        nullptr, asFriend, false, beforeOnly, false);
                    if (storage_class != StorageClass::typedef_ && !isfunction(*tp) && !templateNestingCount && !instantiatingTemplate)
                    {
                        auto tp2 = *tp;
                        while (tp2 && tp2->type != BasicType::lref && tp2->type != BasicType::rref)
                        {
                            if ((tp2->type == BasicType::const_ || tp2->type == BasicType::volatile_))
                            {
                                error(ERR_REF_NO_QUALIFIERS);
                                break;
                            }
                            tp2 = tp2->btp;
                        }
                    }
                }
                break;
            case Keyword::_colon: /* may have unnamed bit fields */
                *spi = makeID(storage_class, *tp, *spi, NewUnnamedID());
                SetLinkerNames(*spi, Linkage::none_);
                (*spi)->sb->anonymous = true;
                lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest, false);
                break;
            case Keyword::_gt:
            case Keyword::_comma:
                break;
            default:
                if (beforeOnly)
                    return lex;
                if (*tp && (isstructured(*tp) || (*tp)->type == BasicType::enum_) && KW(lex) == Keyword::_semicolon)
                {
                    lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest, false);
                    *spi = nullptr;
                    return lex;
                }
                if (storage_class != StorageClass::parameter && storage_class != StorageClass::cast && storage_class != StorageClass::catchvar && !asFriend)
                {
                    if (MATCHKW(lex, Keyword::_openpa) || MATCHKW(lex, Keyword::_openbr) || MATCHKW(lex, Keyword::_assign) || MATCHKW(lex, Keyword::_semicolon))
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
                SetLinkerNames(*spi, Linkage::none_);
                (*spi)->sb->anonymous = true;
                lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest, false);
                break;
        }
    if (*tp && (ptype = basetype(*tp)))
    {
        if (isfuncptr(ptype))
            ptype = basetype(ptype->btp);
        if (isfunction(ptype))
        {
            if (ptype->btp->type == BasicType::func || (ptype->btp->type == BasicType::pointer && ptype->btp->array && !ptype->btp->msil))
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
        mul1 = Allocate<EXPRESSION>();
        if (!btp->esize)
            btp->esize = intNode(ExpressionNode::c_i, 1);
        *mul1 = *btp->esize;
        mul = mul1 = exprNode(ExpressionNode::arraymul, mul, mul1);
        btp->sp = sp;
        btp->vlaindex = (*vlaindex)++;
    }
    else
    {
#ifdef ERROR
#    error Sizeof vla of vla
#endif
        mul = intNode(ExpressionNode::c_i, btp->size);
        mul1 = Allocate<EXPRESSION>();
        *mul1 = *mul;
    }
    store = exprNode(ExpressionNode::add, vlanode, intNode(ExpressionNode::c_i, *index));
    deref(&stdint, &store);
    store = exprNode(ExpressionNode::assign, store, mul1);
    **rptr = exprNode(ExpressionNode::void_, store, nullptr);
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
            al = al->tp->syms->front();
        }
        if (fr)
        {
            fr = fr->tp->syms->front();
        }
        if (al && fr)
        {
            FUNCTIONCALL* epx = Allocate<FUNCTIONCALL>();
            FUNCTIONCALL* ld = Allocate<FUNCTIONCALL>();
            epx->ascall = true;
            epx->fcall = varNode(ExpressionNode::pc, al);
            epx->sp = al;
            epx->functp = al->tp;
            epx->arguments = initListListFactory.CreateList();
            auto arg = Allocate<INITLIST>();
            epx->arguments->push_back(arg);
            arg->tp = &stdint;
            arg->exp = ep2;
            ep2 = intNode(ExpressionNode::func, 0);
            ep2->v.func = epx;
            ep2 = exprNode(ExpressionNode::assign, ep1, ep2);
            ld->ascall = true;
            ld->fcall = varNode(ExpressionNode::pc, fr);
            ld->sp = fr;
            ld->functp = fr->tp;
            arg = Allocate<INITLIST>();
            ld->arguments = initListListFactory.CreateList();
            ld->arguments->push_back(arg);
            arg->tp = &stdpointer;
            arg->exp = ep1;
            unloader = intNode(ExpressionNode::func, 0);
            unloader->v.func = ld;
            ep1 = ep2;
        }
        else
        {
            diag("llallocatevla: cannot find allocator");
            return intNode(ExpressionNode::c_i, 0);
        }
    }
    else
    {
        EXPRESSION* var = anonymousVar(StorageClass::auto_, &stdpointer);
        loader = exprNode(ExpressionNode::savestack, var, nullptr);
        unloader = exprNode(ExpressionNode::loadstack, var, nullptr);
        ep1 = exprNode(ExpressionNode::assign, ep1, exprNode(ExpressionNode::alloca_, ep2, nullptr));
        ep1 = exprNode(ExpressionNode::void_, loader, ep1);
    }

    initInsert(&sp->sb->dest, sp->tp, unloader, 0, false);
    return ep1;
}
static void allocateVLA(LEXLIST* lex, SYMBOL* sp, SYMBOL* funcsp, std::list<BLOCKDATA*>& block, TYPE* btp, bool bypointer)
{
    EXPRESSION *result = nullptr, **rptr = &result;
    TYPE* tp1 = btp;
    int count = 0;
    int soa = getSize(BasicType::pointer);
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
        if (sp->sb->storage_class != StorageClass::auto_ && sp->sb->storage_class != StorageClass::typedef_ && sp->sb->storage_class != StorageClass::localstatic)
            error(ERR_VLA_BLOCK_SCOPE);
    }
    currentLineData(block, lex, 0);
    if (sp->tp->sp)
    {
        SYMBOL* dest = sp;
        SYMBOL* src = sp->tp->sp;
        *rptr = exprNode(ExpressionNode::void_, nullptr, nullptr);
        rptr = &(*rptr)->right;
        result->left = exprNode(ExpressionNode::blockassign, varNode(ExpressionNode::auto_, dest), varNode(ExpressionNode::auto_, src));
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
        vlanode = varNode(ExpressionNode::auto_, vlasp);
        vlaSetSizes(&rptr, vlanode, btp, vlasp, &index, &vlaindex, sou);

        ep = exprNode(ExpressionNode::add, vlanode, intNode(ExpressionNode::c_i, soa));
        deref(&stdint, &ep);
        ep = exprNode(ExpressionNode::assign, ep, intNode(ExpressionNode::c_i, count));
        *rptr = exprNode(ExpressionNode::void_, ep, nullptr);
        rptr = &(*rptr)->right;

        basetype(sp->tp)->size = size; /* size field is actually size of VLA header block */
                                       /* sizeof will fetch the size from that */
    }
    if (result != nullptr)
    {
        STATEMENT* st = stmtNode(nullptr, block, StatementNode::declare);
        st->hasvla = true;
        if (sp->sb->storage_class != StorageClass::typedef_ && !bypointer)
        {
            EXPRESSION* ep1 = exprNode(ExpressionNode::add, varNode(ExpressionNode::auto_, sp), intNode(ExpressionNode::c_i, 0));
            EXPRESSION* ep2 = exprNode(ExpressionNode::add, varNode(ExpressionNode::auto_, sp), intNode(ExpressionNode::c_i, soa + sou * (count + 1)));
            deref(&stdpointer, &ep1);
            deref(&stdint, &ep2);
            ep1 = llallocateVLA(sp, ep1, ep2);  // exprNode(ExpressionNode::assign, ep1, exprNode(ExpressionNode::alloca_, ep2, nullptr));
            *rptr = (Optimizer::architecture == ARCHITECTURE_MSIL) ? ep1 : exprNode(ExpressionNode::void_, ep1, nullptr);
            sp->sb->assigned = true;
        }
        st->select = result;
        optimize_for_constants(&st->select);
    }
}
void sizeQualifiers(TYPE* tp)
{
    if (tp->type != BasicType::atomic)
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
static bool sameQuals(SYMBOL* sp1, SYMBOL* sp2)
{
    TYPE *tp1 = sp1->tp, *tp2 = sp2->tp;
    while (tp1 && tp2)
    {
        if (isconst(tp1) != isconst(tp2))
        {
            // constexpr matches const at top level
            if (basetype(tp1)->btp || basetype(tp2)->btp || (isconst(tp1) && !sp2->sb->constexpression) ||
                (isconst(tp2) && !sp1->sb->constexpression))
                return false;
        }
        if (isvolatile(tp1) != isvolatile(tp2))
            return false;
        if (isrestrict(tp1) != isrestrict(tp2))
            return false;
        tp1 = basetype(tp1)->btp;
        tp2 = basetype(tp2)->btp;
    }
    return true;
}
static LEXLIST* getStorageAndType(LEXLIST* lex, SYMBOL* funcsp, SYMBOL** strSym, bool inTemplate, bool assumeType,
                                  enum StorageClass* storage_class, StorageClass* storage_class_in, Optimizer::ADDRESS* address, bool* blocked,
                                  bool* isExplicit, bool* constexpression, TYPE** tp, Linkage* linkage, Linkage* linkage2,
                                  enum Linkage* linkage3, AccessLevel access, bool* notype, bool* defd, int* consdest, bool* templateArg,
                                  bool* asFriend)
{
    bool foundType = false;
    bool first = true;
    bool flaggedTypedef = false;
    *blocked = false;
    *constexpression = false;

    while (KWTYPE(lex, TT_STORAGE_CLASS | TT_POINTERQUAL | TT_LINKAGE | TT_DECLARE) ||
           (!foundType && startOfType(lex, nullptr, assumeType)) || MATCHKW(lex, Keyword::_complx) ||
           (*storage_class == StorageClass::typedef_ && !foundType))
    {
        if (KWTYPE(lex, TT_DECLARE))
        {
            // The next line has a recurring check. The 'lex' condition was already verified above
            // the problem is I'm using macros that I want to be independent from each other
            // and I don't want to introduce another macro without the check for readability
            // reasons.  So I'm going to leave the recurring check.
            if (MATCHKW(lex, Keyword::_constexpr))
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
            if (*storage_class == StorageClass::typedef_)
            {
                flaggedTypedef = true;
                inTypedef++;
            }
        }
        else if (KWTYPE(lex, TT_POINTERQUAL | TT_LINKAGE))
        {
            lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, asFriend);
            if (MATCHKW(lex, Keyword::_atomic))
            {
                foundType = true;
                lex = getBasicType(lex, funcsp, tp, strSym, inTemplate, *storage_class_in, linkage, linkage2, linkage3, access,
                                   notype, defd, consdest, templateArg, *storage_class == StorageClass::typedef_, true, false,
                                   asFriend ? *asFriend : false, *constexpression);
            }
            if (*linkage3 == Linkage::threadlocal_ && *storage_class == StorageClass::member)
                *storage_class = StorageClass::static_;
        }
        else if (foundType)
        {
            break;
        }
        else
        {
            foundType = true;
            lex = getBasicType(lex, funcsp, tp, strSym, inTemplate, *storage_class_in, linkage, linkage2, linkage3, access, notype,
                               defd, consdest, templateArg, *storage_class == StorageClass::typedef_, true, false, asFriend ? *asFriend : false,
                               *constexpression);
            if (*linkage3 == Linkage::threadlocal_ && *storage_class == StorageClass::member)
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
        if  (syms->size() && syms->front()->sb->thisPtr)
            return;
        auto type = MakeType(BasicType::pointer, basetype(sp->sb->parentClass->tp));
        if (isconst(sp->tp))
        {
            type->btp = MakeType(BasicType::const_, type->btp);
        }
        if (isvolatile(sp->tp))
        {
            type->btp = MakeType(BasicType::volatile_, type->btp);
        }
        UpdateRootTypes(type);
        auto ths = makeID(StorageClass::parameter, type, nullptr, "__$$this");
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
    for ( ;ita != ita && itb != itbe; ++ita, ++itb)
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
LEXLIST* declare(LEXLIST* lex, SYMBOL* funcsp, TYPE** tprv, StorageClass storage_class, Linkage defaultLinkage, std::list<BLOCKDATA*>& block,
                 bool needsemi, int asExpression, bool inTemplate, AccessLevel access)
{
    bool isExtern = false;
    TYPE* btp;
    SYMBOL* sp;
    enum StorageClass storage_class_in = storage_class;
    enum Linkage linkage = Linkage::none_;
    enum Linkage linkage2 = Linkage::none_;
    enum Linkage linkage3 = Linkage::none_;
    enum Linkage linkage4 = Linkage::none_;
    std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
    SYMBOL* strSym = nullptr;
    Optimizer::ADDRESS address = 0;
    TYPE* tp = nullptr;
    bool hasAttributes;
    attributes oldAttribs = basisAttribs;

    basisAttribs = {0};
    hasAttributes = ParseAttributeSpecifiers(&lex, funcsp, true);

    if (!MATCHKW(lex, Keyword::_semicolon))
    {
        if (MATCHKW(lex, Keyword::___inline))
        {
            linkage = Linkage::inline_;
            lex = getsym();
            ParseAttributeSpecifiers(&lex, funcsp, true);
        }
        else if (!asExpression && MATCHKW(lex, Keyword::_extern))
        {
            lex = getsym();
            if (MATCHKW(lex, Keyword::_template))
            {
                isExtern = true;
                goto jointemplate;
            }
            lex = backupsym();
        }
        if (!asExpression && MATCHKW(lex, Keyword::_template))
        {
        jointemplate:
            if (funcsp)
            {
                if (storage_class == StorageClass::member || storage_class == StorageClass::mutable_)
                    error(ERR_TEMPLATE_NO_LOCAL_CLASS);
                else
                    error(ERR_TEMPLATE_GLOBAL_OR_CLASS_SCOPE);
            }
            if (hasAttributes)
                error(ERR_NO_ATTRIBUTE_SPECIFIERS_HERE);

            lex = TemplateDeclaration(lex, funcsp, access, storage_class, isExtern);
            needsemi = false;
        }
        else if (!asExpression && MATCHKW(lex, Keyword::_namespace))
        {
            bool linked;
            struct _ccNamespaceData nsData;
            if (!IsCompiler())
            {
                nsData.declfile = lex->data->errfile;
                nsData.startline = lex->data->errline;
            }
            if (storage_class_in == StorageClass::member || storage_class_in == StorageClass::mutable_)
                error(ERR_NAMESPACE_NO_STRUCT);

            if (hasAttributes)
                error(ERR_NO_ATTRIBUTE_SPECIFIERS_HERE);
            lex = getsym();
            lex = insertNamespace(lex, linkage, storage_class_in, &linked);
            if (linked)
            {
                if (needkw(&lex, Keyword::_begin))
                {
                    while (lex && !MATCHKW(lex, Keyword::_end))
                    {
                        lex = declare(lex, nullptr, nullptr, storage_class, defaultLinkage, emptyBlockdata, true, false, false, access);
                    }
                }
                if (!IsCompiler() && lex)
                    nsData.endline = lex->data->errline;
                needkw(&lex, Keyword::_end);
                if (linked)
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
                needsemi = false;
            }
        }
        else if (!asExpression && MATCHKW(lex, Keyword::_using))
        {
            lex = getsym();
            sp = nullptr;
            lex = insertUsing(lex, &sp, access, storage_class_in, inTemplate, hasAttributes);
            if (sp && tprv)
                *tprv = (TYPE*)-1;
        }
        else if (!asExpression && MATCHKW(lex, Keyword::_static_assert))
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
                if (storage_class_in == StorageClass::member && asFriend)
                {
                    storage_class = StorageClass::external;
                }
                else if (storage_class_in == StorageClass::member && storage_class == StorageClass::static_)
                {
                    storage_class = StorageClass::external;
                }
                do
                {
                    bool isTemplatedCast = false;
                    TYPE* tp1 = tp;
                    std::list<NAMESPACEVALUEDATA*>* oldGlobals = nullptr;
                    std::list<SYMBOL*> oldNameSpaceList;
                    bool promotedToTemplate = false;
                    if (!tp1)
                    {
                        // safety net
                        notype = true;
                        tp = tp1 = MakeType(BasicType::int_);
                    }
                    sp = nullptr;
                    if (tp->type == BasicType::templateselector)
                    {
                        // have to special case a templated cast operator
                        TEMPLATESELECTOR* l = &(*tp->sp->sb->templateSelector).back();
                        if (!strcmp(l->name, overloadNameTab[CI_CAST]))
                        {
                            strSym = (*tp->sp->sb->templateSelector)[1].sp;
                            while (!MATCHKW(lex, Keyword::_operator))
                                lex = lex->prev;
                            notype = true;
                            tp = tp1 = MakeType(BasicType::int_);
                            isTemplatedCast = true;
                        }
                    }
                    lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, &asFriend);
                    lex = getBeforeType(lex, funcsp, &tp1, &sp, &strSym, &nsv, inTemplate, storage_class, &linkage, &linkage2,
                                        &linkage3, &notype, asFriend, consdest, false, false);
                    if (linkage2 == Linkage::import_)
                        if (storage_class == StorageClass::global)
                            storage_class = StorageClass::external;
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
                        else if (strSym->tp->type != BasicType::templateselector && strSym->tp->type != BasicType::templatedecltype)
                        {
                            STRUCTSYM* l = Allocate<STRUCTSYM>();
                            l->str = strSym;
                            addStructureDeclaration(l);
                        }
                    }
                    if (Optimizer::cparams.prm_cplusplus && isfunction(tp1) &&
                        (storage_class_in == StorageClass::member || storage_class_in == StorageClass::mutable_) && !asFriend)
                    {
                        if (MATCHKW(lex, Keyword::_colon) || MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::___try))
                        {
                            sp->sb->attribs.inheritable.isInline = true;
                        }
                    }
                    if (linkage == Linkage::inline_)
                    {
                        linkage = Linkage::none_;
                        if (Optimizer::cparams.prm_cplusplus)
                        {
                            linkage4 = Linkage::virtual_;
                        }
                        if (sp)
                            sp->sb->attribs.inheritable.isInline = true;
                    }
                    else if (!Optimizer::cparams.prm_profiler && Optimizer::cparams.prm_optimize_for_speed && isfunction(tp1) &&
                             storage_class_in != StorageClass::member && storage_class_in != StorageClass::mutable_)
                    {
                        if (!strSym)
                        {
                            if (MATCHKW(lex, Keyword::_colon) || MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::___try))
                            {
                                if (strcmp(sp->name, "main") != 0 && strcmp(sp->name, "WinMain") != 0)
                                {
                                    sp->sb->attribs.inheritable.isInline = sp->sb->promotedToInline =
                                        true;
                                }
                            }
                        }
                    }
                    if (linkage == Linkage::none_ || ((defaultLinkage == Linkage::c_ || defaultLinkage == Linkage::cpp_) && linkage == Linkage::cdecl_))
                        linkage = defaultLinkage;
                    // defaultLinkage may also be Linkage::none_...
                    if (linkage == Linkage::none_)
                        linkage = Linkage::cdecl_;

                    if (asFriend && ispointer(tp1))
                        error(ERR_POINTER_ARRAY_NOT_FRIENDS);

                    if (storage_class_in != StorageClass::parameter && tp1->type == BasicType::templateparam && tp1->templateParam->second->packed)
                        error(ERR_PACKED_TEMPLATE_PARAMETER_NOT_ALLOWED_HERE);
                    if (!sp)
                    {
                        if (!isstructured(tp1) && tp1->type != BasicType::enum_)
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
                                if (strSym && strSym->tp->type != BasicType::enum_ && strSym->tp->type != BasicType::templateselector &&
                                    strSym->tp->type != BasicType::templatedecltype)
                                {
                                    dropStructureDeclaration();
                                }
                                break;
                            }
                        }
                        tp1 = basetype(tp1);
                        if (Optimizer::cparams.prm_cplusplus && storage_class_in == StorageClass::global && tp1->type == BasicType::union_ &&
                            tp1->sp->sb->anonymous)
                        {
                            enum StorageClass sc = storage_class_in;
                            if (sc != StorageClass::member && sc != StorageClass::mutable_)
                                sc = StorageClass::static_;

                            sp = makeID(sc, tp1, nullptr, AnonymousName());
                            sp->sb->anonymous = true;
                            sp->sb->access = access;
                            SetLinkerNames(sp, Linkage::c_);
                            sp->sb->parent = funcsp; /* function vars have a parent */
                            InsertSymbol(sp, sp->sb->storage_class, linkage, false);
                            if (storage_class != StorageClass::static_)
                            {
                                error(ERR_GLOBAL_ANONYMOUS_UNION_NOT_STATIC);
                            }
                            resolveAnonymousGlobalUnion(sp);
                        }
                        else if ((storage_class_in == StorageClass::member || storage_class_in == StorageClass::mutable_) && isstructured(tp1) &&
                                 tp1->sp->sb->anonymous)
                        {
                            sp = makeID(storage_class_in, tp1, nullptr, AnonymousName());
                            sp->sb->anonymous = true;
                            SetLinkerNames(sp, Linkage::c_);
                            sp->sb->parent = funcsp; /* function vars have a parent */
                            sp->sb->parentClass = getStructureDeclaration();
                            InsertSymbol(sp, sp->sb->storage_class, linkage, false);
                        }
                        else
                        {
                            if (asFriend)
                            {
                                SYMBOL* sym = nullptr;
                                auto lsit = structSyms.begin();
                                auto lsite = structSyms.end();
                                while (lsit != lsite && !(*lsit).str)
                                    ++lsit;
                                if (strSym && strSym->tp->type != BasicType::enum_ && strSym->tp->type != BasicType::templateselector &&
                                    strSym->tp->type != BasicType::templatedecltype)
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
                            if (strSym && strSym->tp->type != BasicType::enum_ && strSym->tp->type != BasicType::templateselector &&
                                strSym->tp->type != BasicType::templatedecltype)
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
                        if (!templateNestingCount && funcsp)
                            tp1 = ResolveTemplateSelectors(funcsp, tp1);
                        if ((isstructured(tp1) && storage_class != StorageClass::typedef_) || (!templateNestingCount && !structLevel))
                            tp1 = PerformDeferredInitialization(tp1, funcsp);
                        ssp = getStructureDeclaration();
                        if (!asFriend &&
                            (((storage_class_in == StorageClass::member || storage_class_in == StorageClass::mutable_) && ssp) || (inTemplate && strSym)))
                        {
                            sp->sb->parentClass = ssp;
                        }
                        if (!inTemplate)
                        {
                            if (basetype(sp->tp) && isfunction(sp->tp) && hasTemplateParent(sp))
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
                        else if (ssp && storage_class_in != StorageClass::member && storage_class_in != StorageClass::mutable_ && !asFriend)
                        {
                            storage_class = StorageClass::member;
                        }
                        if (Optimizer::cparams.prm_cplusplus && !ssp && storage_class == StorageClass::global &&
                            (isstructured(tp1) || basetype(tp1)->type == BasicType::enum_))
                            if (basetype(tp1)->sp->sb->anonymous)
                                storage_class = StorageClass::static_;
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
                        sp->sb->access = access;
                        sp->sb->isExplicit = isExplicit;
                        sp->sb->storage_class = storage_class;
                        if (inTemplate &&
                            (!sp->templateParams || MATCHKW(lex, Keyword::_colon) || MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::___try)))
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
                        if (ssp && ssp->sb->attribs.inheritable.linkage2 != Linkage::none_ && sp->sb->storage_class != StorageClass::localstatic)
                        {
                            if (linkage2 != Linkage::none_ && !asFriend)
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
                        SetLinkerNames(sp, storage_class == StorageClass::auto_ && isstructured(sp->tp) ? Linkage::auto_ : linkage,
                                       !!sp->templateParams);
                        if (inTemplate && templateNestingCount == 1)
                        {
                            inTemplateBody++;
                        }
                        if (Optimizer::cparams.prm_cplusplus && isfunction(sp->tp) && (MATCHKW(lex, Keyword::___try) || MATCHKW(lex, Keyword::_colon)))
                        {
                            bool viaTry = MATCHKW(lex, Keyword::___try);
                            if (viaTry)
                            {
                                sp->sb->hasTry = true;
                                lex = getsym();
                            }
                            if (MATCHKW(lex, Keyword::_colon))
                            {
                                lex = getsym();
                                sp->sb->memberInitializers = GetMemberInitializers(&lex, funcsp, sp);
                            }
                        }
                        if (storage_class == StorageClass::absolute)
                            sp->sb->value.i = address;
                        if ((!Optimizer::cparams.prm_cplusplus || !getStructureDeclaration()) && !istype(sp) &&
                            sp->sb->storage_class != StorageClass::static_ && isfunction(basetype(tp1)) && !MATCHKW(lex, Keyword::_begin))
                            sp->sb->storage_class = StorageClass::external;
                        if (isvoid(tp1))
                            if (sp->sb->storage_class != StorageClass::parameter && sp->sb->storage_class != StorageClass::typedef_)
                                error(ERR_VOID_NOT_ALLOWED);
                        if (sp->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
                        {
                            if (isfunction(tp1))
                                error(ERR_FUNC_NOT_THREAD_LOCAL);
                            if (sp->sb->storage_class != StorageClass::external && sp->sb->storage_class != StorageClass::global &&
                                sp->sb->storage_class != StorageClass::static_)
                            {
                                if (sp->sb->storage_class == StorageClass::auto_ || sp->sb->storage_class == StorageClass::register_)
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
                        if (sp->sb->attribs.inheritable.linkage2 == Linkage::property_ && isfunction(tp1))
                            error(ERR_PROPERTY_QUALIFIER_NOT_ALLOWED_ON_FUNCTIONS);
                        if (storage_class != StorageClass::typedef_ && storage_class != StorageClass::catchvar && isstructured(tp1) && basetype(tp1)->sp->sb->isabstract)
                            errorabstract(ERR_CANNOT_CREATE_INSTANCE_ABSTRACT, basetype(tp1)->sp);
                        if (sp->packed)
                            error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                        if (storage_class == StorageClass::mutable_ && isconst(tp))
                            errorsym(ERR_MUTABLE_NON_CONST, sp);
                        // correct for previous errors
                        if (sp->sb->isConstructor && ispointer(sp->tp))
                            sp->tp = basetype(sp->tp)->btp;

                        if (ssp && strSym && strSym->tp->type != BasicType::templateselector && strSym->tp->type != BasicType::templatedecltype)
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
                                errorNotMember(strSym, nsv->front(), sp->name);
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
                                if ((storage_class_in == StorageClass::auto_ || storage_class_in == StorageClass::parameter) &&
                                    storage_class != StorageClass::external && !isfunction(sp->tp))
                                {
                                    spi = localNameSpace->front()->syms->Lookup(sp->name);
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
                                        spi = ssp->tp->syms->Lookup(sp->name);
                                    }
                                    else
                                    {
                                        spi = globalNameSpace->front()->syms->Lookup(sp->name);
                                    }
                                }
                            }
                        }
                        if (spi && spi->sb->storage_class == StorageClass::overloads)
                        {
                            SYMBOL* sym = nullptr;
                            if (isfunction(sp->tp))
                            {
                                for (auto sp : *spi->tp->syms)
                                {
                                    if (!sp->sb->instantiated)
                                        ScrubTemplateValues(sp);
                                }
                                if (!sp->sb->parentClass || !sp->sb->parentClass->sb->declaring)
                                {
                                    sym = searchOverloads(sp, spi->tp->syms);
                                    TYPE* retVal;
                                    TEMPLATESELECTOR* tsl = nullptr;
                                    if (!sym && storage_class_in != StorageClass::member && ((retVal = basetype(sp->tp)->btp)->type == BasicType::templateselector))
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
                                if (sym->sb->attribs.inheritable.linkage == Linkage::c_ && sp->sb->attribs.inheritable.linkage == Linkage::cdecl_)
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
                                         !comparetypes(basetype(sp->tp)->btp, basetype((sym)->tp)->btp, true))
                                {
                                    if (Optimizer::cparams.prm_cplusplus && isfunction(sym->tp) &&
                                        (sym->sb->templateLevel || templateNestingCount))
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
                                                for (;one != onee && two != twoe; ++one, ++two)
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
                                    if (sym == sp && spi->sb->storage_class != StorageClass::overloads)
                                    {
                                        sym = spi;
                                    }
                                    else if (sym)
                                    {
                                        InsertSymbol(sym, storage_class == StorageClass::typedef_ ? storage_class_in : storage_class, linkage,
                                                     false);
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
                                    auto it1 = basetype(spi->tp)->syms->begin();
                                    auto it1end = basetype(spi->tp)->syms->end();
                                    auto it2 = basetype(sp->tp)->syms->begin();
                                    auto it2end = basetype(sp->tp)->syms->end();
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
                                if ((nsv || strSym) && storage_class_in != StorageClass::member && storage_class_in != StorageClass::mutable_ &&
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
                                    errorNotMember(strSym, nsv->front(), buf);
                                }
                                spi = nullptr;
                            }
                        }
                        else if (!spi)
                        {
                            if (sp->templateParams && sp->templateParams->front().second->bySpecialization.types)
                                errorsym(ERR_SPECIALIZATION_REQUIRES_PRIMARY, sp);
                            if (strSym && storage_class_in != StorageClass::member && storage_class_in != StorageClass::mutable_)
                            {
                                errorNotMember(strSym, nsv ? nsv->front() : nullptr, sp->name);
                            }
                        }
                        else
                        {
                            if (sp->sb->storage_class == StorageClass::member && spi->sb->storage_class == StorageClass::external)
                                sp->sb->storage_class = StorageClass::global;
                        }
                        if ((!spi || (spi->sb->storage_class != StorageClass::member && spi->sb->storage_class != StorageClass::mutable_)) &&
                            sp->sb->storage_class == StorageClass::global && sp->sb->attribs.inheritable.isInline && !sp->sb->promotedToInline)
                        {
                            if (!spi || spi->sb->storage_class != StorageClass::external)
                                sp->sb->storage_class = StorageClass::static_;
                        }
                        if (spi && !sp->sb->parentClass && !isfunction(spi->tp) && spi->sb->storage_class != StorageClass::type &&
                            sp->sb->templateLevel)
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
                                                             MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::_colon));
                                    }
                                    else
                                    {
                                        if (!asFriend || MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::_colon))
                                        {
                                            auto temp = TemplateMatching(lex, spi->templateParams, templateParams, spi,
                                                                         MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::_colon));
                                            spi->templateParams = temp;
                                        }
                                        else
                                        {
                                            auto temp = TemplateMatching(lex, spi->templateParams, templateParams, spi, true);
                                        }
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
                                matchFunctionDeclaration(lex, sp, spi, checkReturn, asFriend);
                            }
                            if (sp->sb->parentClass)
                            {
                                if (spi->sb->storage_class != StorageClass::member && sp->sb->storage_class != StorageClass::member &&
                                    spi->sb->storage_class != StorageClass::mutable_ && sp->sb->storage_class != StorageClass::mutable_)
                                    if (spi->sb->storage_class != StorageClass::external && sp->sb->storage_class != StorageClass::external)
                                        preverrorsym(ERR_DUPLICATE_IDENTIFIER, spi, spi->sb->declfile, spi->sb->declline);
                            }
                            else
                            {
                                sp->sb->parentClass = strSym;
                                if (!sp->sb->parentClass && spi->sb->parentClass)  // error handling
                                    sp->sb->parentClass = spi->sb->parentClass;
                                if (sp->sb->parentClass)
                                    SetLinkerNames(sp, storage_class == StorageClass::auto_ && isstructured(sp->tp) ? Linkage::auto_ : linkage,
                                    !!sp->templateParams);
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
                                if (spi->sb->pushedTemplateSpecializationDefinition && (MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::_colon)))
                                {
                                    spi->sb->pushedTemplateSpecializationDefinition = false;
                                    spi->sb->inlineFunc.stmt = nullptr;
                                    spi->sb->deferredCompile = nullptr;
                                }
                                if ((isfunction(spi->tp) && (spi->sb->inlineFunc.stmt || spi->sb->deferredCompile) &&
                                     (MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::_colon)) &&
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
                                else if (!sameQuals(sp, spi) && (!templateNestingCount || instantiatingTemplate))
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
                                    case StorageClass::auto_:
                                    case StorageClass::register_:
                                    case StorageClass::parameter:
                                        preverrorsym(ERR_DUPLICATE_IDENTIFIER, sp, spi->sb->declfile, spi->sb->declline);
                                        break;
                                    case StorageClass::external:
                                        if ((spi)->sb->storage_class == StorageClass::static_)
                                        {
                                            if (!isfunction(spi->tp))
                                            {
                                                if (spi->sb->constexpression)
                                                {
                                                    spi->sb->storage_class = StorageClass::global;
                                                }
                                                else
                                                {
                                                    errorsym(ERR_ANSI_FORBID_BOTH_EXTERN_STATIC, spi);
                                                    spi->sb->storage_class = StorageClass::global;
                                                }
                                            }
                                            else if (spi->sb->attribs.inheritable.isInline && basetype(spi->tp)->type == BasicType::ifunc)
                                            {
                                                spi->sb->storage_class = StorageClass::global;
                                            }
                                        }
                                        break;
                                    case StorageClass::global:
                                        if (spi->sb->storage_class != StorageClass::static_)
                                        {
                                            spi->sb->declfile = sp->sb->declfile;
                                            spi->sb->declline = sp->sb->declline;
                                            spi->sb->realdeclline = sp->sb->realdeclline;
                                            spi->sb->declfilenum = sp->sb->declfilenum;
                                            sp->sb->wasExternal = spi->sb->wasExternal = spi->sb->storage_class == StorageClass::external;
                                            spi->sb->storage_class = StorageClass::global;
                                        }
                                        break;
                                    case StorageClass::mutable_:
                                        if ((spi)->sb->storage_class != StorageClass::mutable_)
                                        {
                                            preverrorsym(ERR_DUPLICATE_IDENTIFIER, sp, spi->sb->declfile, spi->sb->declline);
                                        }
                                        break;
                                    case StorageClass::member:
                                        if ((spi)->sb->storage_class == StorageClass::static_)
                                        {
                                            preverrorsym(ERR_DUPLICATE_IDENTIFIER, sp, spi->sb->declfile, spi->sb->declline);
                                        }
                                        break;
                                    case StorageClass::static_:
                                        if ((spi)->sb->storage_class == StorageClass::external && !isfunction(spi->tp) &&
                                            !spi->sb->constexpression)
                                        {
                                            errorsym(ERR_ANSI_FORBID_BOTH_EXTERN_STATIC, spi);
                                            spi->sb->storage_class = StorageClass::global;
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
                                    case StorageClass::localstatic:
                                    case StorageClass::constexpr_:
                                        spi->sb->storage_class = sp->sb->storage_class;
                                        break;
                                }
                            }
                            if (isfunction(spi->tp))
                            {
                                std::list<TEMPLATEPARAMPAIR>* nw = sp->templateParams;
                                if (sp->templateParams)
                                { 
                                    for (auto it = sp->templateParams->begin(); it != sp->templateParams->end(); )
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
                                    (MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::_colon) || MATCHKW(lex, Keyword::___try)))
                                {
                                    spi->templateParams = sp->templateParams;
                                }
                                if (!asFriend || MATCHKW(lex, Keyword::_colon) || MATCHKW(lex, Keyword::_begin))
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
                                spi->tp->type == BasicType::func)  // && !spi->sb->parentClass && !spi->sb->specialized)
                            {
                                spi->sb->declfile = sp->sb->declfile;
                                spi->sb->declline = sp->sb->declline;
                                spi->sb->realdeclline = sp->sb->realdeclline;
                                spi->sb->declfilenum = sp->sb->declfilenum;
                            }
                            spi->sb->memberInitializers = sp->sb->memberInitializers;
                            spi->sb->hasTry = sp->sb->hasTry;

                            spi->sb->attribs.inheritable.isInline |= sp->sb->attribs.inheritable.isInline;
                            spi->sb->promotedToInline |= sp->sb->promotedToInline;

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
                                if (isfunction(sp->tp) && templateParams->front().second->bySpecialization.types && templateParams->size() > 1)
                                {
                                    error(ERR_FUNCTION_TEMPLATE_NO_PARTIAL_SPECIALIZE);
                                }
                                else if (!isfunction(tp) && templateParams->size() > 1 && templateParams->front().second->bySpecialization.types)
                                {
                                    TemplateValidateSpecialization(templateParams);
                                }
                                else if (templateNestingCount == 1)
                                {
                                    TemplateMatching(lex, nullptr, templateParams, sp, MATCHKW(lex, Keyword::_begin) || MATCHKW(lex, Keyword::_colon));
                                }
                            }
                            if (isfunction(sp->tp))
                            {
                                matchFunctionDeclaration(lex, sp, sp, checkReturn, asFriend);
                                if (inTemplate)
                                    sp->sb->parentTemplate = sp;
                            }
                            else if (sp->sb->storage_class != StorageClass::type)
                            {
                                if (inTemplate)
                                    sp->sb->parentTemplate = sp;
                            }
                            if (!asFriend || isfunction(sp->tp))
                            {
                                if (sp->sb->constexpression && sp->sb->storage_class == StorageClass::global)
                                    sp->sb->storage_class = StorageClass::static_;
                                if (!asFriend || !templateNestingCount || instantiatingTemplate || inTemplate)
                                {
                                    if (sp->sb->storage_class == StorageClass::external ||
                                        (asFriend && !MATCHKW(lex, Keyword::_begin) && !MATCHKW(lex, Keyword::_colon)))
                                    {
                                        InsertSymbol(sp, StorageClass::external, linkage, false);
                                    }
                                    else
                                    {
                                        InsertSymbol(sp, storage_class == StorageClass::typedef_ ? storage_class_in : storage_class, linkage,
                                                     false);
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
                                auto lsit = structSyms.begin();
                                auto lsite = structSyms.end();
                                while (lsit != lsite && !(*lsit).str)
                                    ++lsit;
                                if (strSym && strSym->tp->type != BasicType::enum_ && strSym->tp->type != BasicType::templateselector &&
                                    strSym->tp->type != BasicType::templatedecltype)
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
                            if (isfunction(sp->tp) && (isconst(sp->tp) || isvolatile(sp->tp)))
                                error(ERR_ONLY_MEMBER_CONST_VOLATILE);
                        }
                        if (inTemplate && templateNestingCount == 1)
                        {
                            inTemplateBody--;
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
                                auto s = basetype(tp1)->syms->begin();
                                if ((*s)->sb->thisPtr)
                                    ++s;
                                if ((*s)->tp->type != BasicType::void_)
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
                    {
                        checkDeclarationAccessible(sp, sp->sb->parentClass, isfunction(sp->tp) ? sp : nullptr);
                    }
                    if (sp->sb->operatorId)
                        checkOperatorArgs(sp, asFriend);
                    if (sp->sb->storage_class == StorageClass::typedef_)
                    {
                        TYPE**tn = &sp->tp;
                        // all this is so we can have multiple typedefs referring to the same thing...
                        if (!Optimizer::cparams.prm_cplusplus)
                        {
                            if ((*tn)->type == BasicType::typedef_)
                                while (*tn != basetype(*tn) && (*tn)->type != BasicType::va_list)
                                    tn = &(*tn)->btp;
                            // this next is a little buggy as if there are multiple typedefs for a struct
                            // _Generic won't handle them right.   This is a rare case though and it is for the moment
                            // expedient to do this...
                            if ((*tn)->type != BasicType::struct_)
                                *tn = CopyType(*tn);
                        }
                        sp->tp = MakeType(BasicType::typedef_, sp->tp);
                        UpdateRootTypes(tp);
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
                    if (ispointer(sp->tp) && sp->sb->storage_class != StorageClass::parameter)
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
                    }

                    if (sp->sb->constexpression)
                    {
                        if (ismemberdata(sp))
                            error(ERR_CONSTEXPR_MEMBER_MUST_BE_STATIC);
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
                        if (basetype(sp->tp)->type == BasicType::func)
                        {
                            if (sp->sb->hasBody && !instantiatingFunction && !instantiatingTemplate && MATCHKW(lex, Keyword::_begin))
                                errorsym(ERR_FUNCTION_HAS_BODY, sp);
                            if (funcsp && storage_class == StorageClass::localstatic)
                                errorstr(ERR_INVALID_STORAGE_CLASS, "static");
                            if (storage_class == StorageClass::member && !Optimizer::cparams.prm_cplusplus)
                                error(ERR_FUNCTION_NOT_IN_STRUCT);
                            if (isstructured(basetype(sp->tp)->btp) || basetype(basetype(sp->tp)->btp)->type == BasicType::enum_)
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
                            if (sp->sb->storage_class == StorageClass::static_ && (Optimizer::architecture == ARCHITECTURE_MSIL))
                            {
                                if (!sp->sb->label)
                                    sp->sb->label = Optimizer::nextLabel++;
                                sp->sb->uniqueID = fileIndex;
                            }

                            if (nameSpaceList.size())
                                SetTemplateNamespace(sp);
                            if (MATCHKW(lex, Keyword::_begin))
                            {
                                if (asFriend)
                                    sp->sb->friendContext = getStructureDeclaration();
                                if (!templateNestingCount)
                                    sp->sb->hasBody = true;
                                TYPE* tp = sp->tp;
                                if (sp->sb->storage_class == StorageClass::member && storage_class_in == StorageClass::member)
                                    browse_variable(sp);
                                if (sp->sb->storage_class == StorageClass::external)
                                    sp->sb->storage_class = StorageClass::global;
                                while (tp->type != BasicType::func)
                                    tp = tp->btp;
                                tp->type = BasicType::ifunc;

                                for (auto sym : *tp->syms)
                                {
                                    TYPE* tpl = sym->tp;
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
                                if (storage_class_in == StorageClass::member || storage_class_in == StorageClass::mutable_)
                                {
                                    sp->sb->instantiatedInlineInClass = true;
                                }
                                if (storage_class_in != StorageClass::member && TemplateFullySpecialized(sp->sb->parentClass))
                                {
                                    sp->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                                    if (sp->sb->constexpression && sp->sb->isConstructor)
                                        ConstexprMembersNotInitializedErrors(sp);
                                    auto startStmt = currentLineData(emptyBlockdata, lex, 0);
                                    if (startStmt)
                                        sp->sb->linedata =   startStmt->front()->lineData;
                                    lex = getDeferredData(lex, &sp->sb->deferredCompile, true);
                                    Optimizer::SymbolManager::Get(sp);
                                    sp->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                                    if (!sp->sb->attribs.inheritable.isInline)
                                        InsertInline(sp);
                                }
                                else
                                {
                                    if (Optimizer::cparams.prm_cplusplus &&
                                        storage_class_in != StorageClass::member &&
                                        sp->sb->attribs.inheritable.linkage4 != Linkage::virtual_ && sp->sb->attribs.inheritable.linkage != Linkage::c_)
                                    {
                                        if (!sp->sb->parentNameSpace &&
                                            (!sp->sb->parentClass || !sp->sb->parentClass->templateParams || !templateNestingCount) &&
                                            strcmp(sp->name, "main") != 0 && strcmp(sp->name, "WinMain") != 0)
                                        {
                                            sp->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                                            if (!templateNestingCount || instantiatingTemplate || (sp->sb->specialized && sp->templateParams->size() == 1))
                                                InsertInline(sp);
                                        }
                                    }
                                    if (storage_class_in == StorageClass::member || storage_class_in == StorageClass::mutable_ ||
                                        templateNestingCount == 1 || (asFriend && templateNestingCount == 2))
                                    {
                                        auto startStmt = currentLineData(emptyBlockdata, lex, 0);
                                        if (startStmt)
                                            sp->sb->linedata = startStmt->front()->lineData;
                                        lex = getDeferredData(lex, &sp->sb->deferredCompile, true);
                                        Optimizer::SymbolManager::Get(sp);
                                        if (asFriend)
                                            sp->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                                        if (sp->sb->parentClass && sp->templateParams && (!templateNestingCount || instantiatingTemplate))
                                        {
                                            sp->sb->templateLevel = 0;
                                            sp->tp = SynthesizeType(sp->tp, sp->sb->parentClass->templateParams, false);
                                            sp = TemplateFunctionInstantiate(sp, false);
                                            sp->sb->specialized2 = true;
                                        }
                                        if (sp->templateParams && sp->templateParams->size() == 1 && (!templateNestingCount || instantiatingTemplate))
                                            InsertInline(sp);
                                    }
                                    else
                                    {
                                        lex = body(lex, sp);
                                        bodygen(sp);
                                    }
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
                                if (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, Keyword::_assign))
                                {
                                    lex = getsym();
                                    if (MATCHKW(lex, Keyword::_delete))
                                    {
                                        sp->sb->deleted = true;
                                        sp->sb->constexpression = true;
                                        if (sp->sb->redeclared && !templateNestingCount)
                                        {
                                            errorsym(ERR_DELETE_ON_REDECLARATION, sp);
                                        }
                                        lex = getsym();
                                    }
                                    else if (MATCHKW(lex, Keyword::_default))
                                    {
                                        sp->sb->defaulted = true;
                                        SetParams(sp);
                                        // fixme add more
                                        if (strcmp(sp->name, overloadNameTab[CI_CONSTRUCTOR]) != 0 &&
                                            strcmp(sp->name, overloadNameTab[CI_DESTRUCTOR]) != 0 &&
                                            strcmp(sp->name,
                                                   overloadNameTab[(int)Keyword::_assign - (int)Keyword::_new + CI_NEW]) !=
                                                0)  // this is meant to be a copy cons but is too loose
                                        {
                                            error(ERR_DEFAULT_ONLY_SPECIAL_FUNCTION);
                                        }
                                        else if (storage_class_in != StorageClass::member)
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
                                    else if (lex->data->type != l_i || lex->data->value.i != 0)
                                    {
                                        error(ERR_PURE_SPECIFIER_CONST_ZERO);
                                    }
                                    else
                                    {
                                        lex = getsym();
                                        sp->sb->ispure = true;
                                    }
                                }
                                else if (sp->sb->constexpression && sp->sb->storage_class != StorageClass::external && !isfunction(sp->tp) &&
                                         !isstructured(sp->tp))
                                {
                                    error(ERR_CONSTEXPR_REQUIRES_INITIALIZER);
                                }
                                else if (sp->sb->parentClass && !sp->templateParams &&
                                         !(Optimizer::architecture == ARCHITECTURE_MSIL))
                                    if (!asFriend && storage_class_in != StorageClass::member && storage_class_in != StorageClass::mutable_ &&
                                        !sp->sb->templateLevel)
                                        errorsym(ERR_CANNOT_REDECLARE_OUTSIDE_CLASS, sp);
                            }
                        }
                        else
                        {
                            if ((sp->sb->storage_class == StorageClass::auto_ || sp->sb->storage_class == StorageClass::register_) && (Optimizer::cparams.prm_stackprotect & STACK_UNINIT_VARIABLE) && !isstructured(sp->tp) && !isarray(sp->tp) && !isref(sp->tp)) 
                            {
                                 sp->sb->runtimeSym = anonymousVar(StorageClass::auto_, &stdpointer)->v.sp;
                            }
                            LEXLIST* hold = lex;
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
                            if (sp->sb->storage_class == StorageClass::virtual_)
                            {
                                errorsym(ERR_NONFUNCTION_CANNOT_BE_DECLARED_VIRTUAL, sp);
                            }
                            if (asExpression && !(asExpression & _F_NOCHECKAUTO))
                                checkauto(sp->tp, ERR_AUTO_NOT_ALLOWED);
                            if (sp->sb->storage_class == StorageClass::auto_ || sp->sb->storage_class == StorageClass::register_)
                            {
                                /* special case consts that can later have their address taken, because they are not autos and it will cause errors later*/
                                if (!isint(sp->tp) || !isconst(sp->tp))
                                {
                                    STATEMENT* s = stmtNode(lex, block, StatementNode::varstart);
                                    s->select = varNode(ExpressionNode::auto_, sp);
                                }
                            }
                            if (!sp->sb->label && (sp->sb->storage_class == StorageClass::static_ || sp->sb->storage_class == StorageClass::localstatic) &&
                                (Optimizer::architecture == ARCHITECTURE_MSIL))
                                sp->sb->label = Optimizer::nextLabel++;
                            if (Optimizer::cparams.prm_cplusplus && sp->sb->storage_class != StorageClass::type &&
                                sp->sb->storage_class != StorageClass::typedef_ && structLevel && (!instantiatingTemplate) && !funcsp &&
                                (MATCHKW(lex, Keyword::_assign) || MATCHKW(lex, Keyword::_begin)))
                            {
                                if ((MATCHKW(lex, Keyword::_assign) || MATCHKW(lex, Keyword::_begin)) && storage_class_in == StorageClass::member &&
                                    (sp->sb->storage_class == StorageClass::static_ || sp->sb->storage_class == StorageClass::external))
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
//                                        if (!allTemplateArgsSpecified(sp1, sp1->templateParams))
                                            sp1 = GetClassTemplate(sp1, sp1->templateParams, false);
                                        if (sp1)
                                            sp->tp = PerformDeferredInitialization(
                                                sp1->tp, funcsp);  // TemplateClassInstantiate(sp1, sp1->templateParams, false,
                                                                   // StorageClass::global)->tp;
                                    }
                                }
                                lex = initialize(lex, funcsp, sp, storage_class_in, asExpression, inTemplate,
                                                 0); /* also reserves space */
                                if (sp->sb->parentClass && sp->sb->storage_class == StorageClass::global)
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
                                if (sp->sb->storage_class == StorageClass::auto_ || sp->sb->storage_class == StorageClass::register_ ||
                                    (sp->sb->storage_class == StorageClass::localstatic && sp->sb->init))
                                {
                                    bool doit = true;
                                    if (sp->sb->storage_class == StorageClass::localstatic)
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
                                    if (doit && (sp->sb->init || (isarray(sp->tp) && sp->tp->msil)))
                                    {
                                        STATEMENT* st;
                                        currentLineData(block, hold, 0);
                                        st = stmtNode(hold, block, StatementNode::expr);
                                        st->select =
                                            convertInitToExpression(sp->tp, sp, nullptr, funcsp, sp->sb->init, nullptr, false);\
                                        int offset = 0;
                                        if (sp->sb->runtimeSym)
                                        {
                                            auto exp = relptr(st->select->left->left, offset, true);
                                            if (exp)
                                                SetRuntimeData(lex, exp, sp);
                                        }
                                    }
                                    else if ((isarray(sp->tp) || isstructured(sp->tp)) &&
                                             Optimizer::architecture == ARCHITECTURE_MSIL)
                                    {
                                        if (sp->sb->storage_class != StorageClass::localstatic)
                                        {
                                            STATEMENT* st;
                                            if (block.size() > 1)
                                            {
                                                auto it = block.begin();
                                                ++it;
                                                if ((*it)->type == Keyword::_switch)
                                                {
                                                    // have to put initializations before the switch not in the switch body they
                                                    // cannot
                                                    // be accesed in the switch body
                                                    ++it;
                                                    int n = (*it)->statements->size();
                                                    st = stmtNode(hold, emptyBlockdata, StatementNode::expr);
                                                    auto itb = (*it)->statements->begin();
                                                    for (int i = 0; i < n; i++, ++itb)
                                                        ;
                                                    (*it)->statements->insert(itb, st);
                                                }
                                                else
                                                {
                                                    st = stmtNode(hold, emptyBlockdata, StatementNode::expr);
                                                    block.front()->statements->push_front(st);
                                                }
                                                st->select = exprNode(ExpressionNode::_initobj, varNode(ExpressionNode::auto_, sp), nullptr);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (sp->tp->size == 0 && basetype(sp->tp)->type != BasicType::templateparam &&
                            basetype(sp->tp)->type != BasicType::templateselector && !isarray(sp->tp))
                        {
                            if (storage_class != StorageClass::typedef_)
                            {
                                if (storage_class_in == StorageClass::auto_ && sp->sb->storage_class != StorageClass::external && !isfunction(sp->tp))
                                    errorsym(ERR_UNSIZED, sp);
                                if (storage_class_in == StorageClass::parameter)
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
                    if (strSym && strSym->tp->type != BasicType::enum_ && strSym->tp->type != BasicType::templateselector)
                    {
                        dropStructureDeclaration();
                    }
                } while (!asExpression && !inTemplate && MATCHKW(lex, Keyword::_comma) && (lex = getsym()) != nullptr);
            }
        }
    }
    FlushLineData(preProcessor->GetRealFile().c_str(), preProcessor->GetRealLineNo());
    if (needsemi && !asExpression)
        if (templateNestingCount || !needkw(&lex, Keyword::_semicolon))
        {
            errskim(&lex, skim_semi_declare);
            skip(&lex, Keyword::_semicolon);
        }
    return lex;
}
}  // namespace Parser