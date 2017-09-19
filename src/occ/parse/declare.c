/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include "compiler.h"
/* locally declared func gloabal memory 
 when redeclaring sym, make sure it gets in the global func list
*/
extern ARCH_ASM *chosenAssembler;
extern ARCH_DEBUG *chosenDebugger;
extern NAMESPACEVALUES *globalNameSpace, *localNameSpace;
extern INCLUDES *includes;
extern int nextLabel;
extern enum e_kw skim_end[];
extern enum e_kw skim_closepa[];
extern enum e_kw skim_semi_declare[];
extern enum e_kw skim_comma[];
extern TYPE stdint;
extern TYPE stdpointer;
extern int currentErrorLine;
extern char infile[256];
extern int total_errors;
extern char anonymousNameSpaceName[512];
extern LIST *nameSpaceList;
extern TYPE stdvoid;
extern TYPE stdchar32tptr;
extern char *overloadNameTab[];
extern LEXCONTEXT *context;
extern LAMBDA *lambdas;
extern int inTemplateBody;
extern BOOLEAN inTemplateType;
extern int templateNestingCount;
extern int templateHeaderCount;
extern int instantiatingTemplate;
extern int packIndex;
extern int inTemplateSpecialization;
extern int argument_nesting;
extern int codeLabel;
extern SYMBOL *instantiatingMemberFuncClass;
extern BOOLEAN parsingSpecializationDeclaration;
extern int anonymousNotAlloc;
extern LINEDATA *linesHead, *linesTail;

int inDefaultParam;
LIST *externals, *globalCache;
char deferralBuf[100000];
SYMBOL *enumSyms;
STRUCTSYM *structSyms;
int expandingParams;
int noSpecializationError;
LIST *deferred;
int structLevel;
LIST *openStructs;
char *deprecationText;

static int unnamed_tag_id, unnamed_id;
static char *importFile;
#define CT_NONE 0
#define CT_CONS 1
#define CT_DEST 2

static LEXEME *getStorageAndType(LEXEME *lex, SYMBOL *funcsp, SYMBOL **strSym, BOOLEAN inTemplate, BOOLEAN assumeType, enum e_sc *storage_class, enum e_sc *storage_class_in,
                       ADDRESS *address, BOOLEAN *blocked, BOOLEAN *isExplicit, BOOLEAN *constexpression, TYPE **tp, 
                       enum e_lk *linkage, enum e_lk *linkage2, enum e_lk *linkage3, enum e_ac access, BOOLEAN *notype, BOOLEAN *defd, int *consdest, BOOLEAN *templateArg);

void declare_init(void)
{
    unnamed_tag_id = 1;
    unnamed_id = 1;
    structSyms = NULL;
    externals = NULL;
    globalCache = NULL;
    nameSpaceList = NULL;
    anonymousNameSpaceName[0] = 0;	
    noSpecializationError = 0;
    deferred = NULL;
    structLevel = 0;
    inDefaultParam = 0;
    openStructs = NULL;
    argument_nesting = 0;
}

void InsertGlobal(SYMBOL *sp)
{
    LIST *l1 = Alloc(sizeof(LIST));
    l1->data = sp;
    l1->next = globalCache;
    globalCache = l1;
}
void InsertExtern(SYMBOL *sp)
{
    LIST *l1 = Alloc(sizeof(LIST));
    l1->data = sp;
    l1->next = externals;
    externals = l1;
}

static char *NewTagName(void)
{
    char buf[256];
    my_sprintf(buf,"Unnamed++%d", unnamed_tag_id++);
    return litlate(buf);
}
static char *NewUnnamedID(void)
{
    char buf[256];
    my_sprintf(buf,"Unnamed++Identifier %d", unnamed_id++);
    return litlate(buf);
}
char *AnonymousName(void)
{
    char buf[256];
    my_sprintf(buf,"Anonymous++Identifier %d", unnamed_id++);
    return litlate(buf);
}
char *AnonymousTypeName(void)
{
    char buf[512];
    my_sprintf(buf,"__anontype_%u_%d", CRC32((unsigned char *)includes->fname, strlen(includes->fname)), includes->anonymousid++ );
    return litlate(buf);
}
SYMBOL *makeID(enum e_sc storage_class, TYPE *tp, SYMBOL *spi, char *name)
{
    SYMBOL *sp = Alloc(sizeof(SYMBOL ));
    LEXEME *lex = context->cur ? context->cur->prev : context->last;
    if (name && strstr(name, "++"))
        sp->compilerDeclared = TRUE;
    sp->name = name;
    sp->storage_class = storage_class;
    sp->tp = tp;
    sp->declfile = sp->origdeclfile = lex->file;
    sp->declline = sp->origdeclline = lex->line;
    sp->declfilenum = lex->filenum;
    if (spi)
    {
        error(ERR_TOO_MANY_IDENTIFIERS_IN_DECLARATION);
        return spi;
    }
    return sp;
}
SYMBOL *makeUniqueID(enum e_sc storage_class, TYPE *tp, SYMBOL *spi, char *name)
{
    char buf[512];
    my_sprintf(buf,"%s_%u", name, CRC32((unsigned char *)includes->fname, strlen(includes->fname)));
    return makeID(storage_class, tp, spi, litlate(buf));
}
void addStructureDeclaration(STRUCTSYM *decl)
{
    decl->next = structSyms;
    decl->tmpl = NULL;
    structSyms = decl;
}
void addTemplateDeclaration(STRUCTSYM *decl)
{
    decl->next = structSyms;
    decl->str = NULL;
    structSyms = decl;
}
void dropStructureDeclaration(void)
{
    structSyms = structSyms->next;
}
SYMBOL *getStructureDeclaration(void)
{
    STRUCTSYM *l = structSyms;
    while (l && !l->str)
        l = l->next;
    if (l)
        return l->str;
    return NULL;
}
void InsertSymbol(SYMBOL *sp, enum e_sc storage_class, enum e_lk linkage, BOOLEAN allowDups)
{
    HASHTABLE *table ;
    SYMBOL *ssp = getStructureDeclaration();

    if (ssp && sp->parentClass == ssp)
    {
        table = sp->parentClass->tp->syms;
    }		
    else if (storage_class == sc_auto || storage_class == sc_register || storage_class == sc_catchvar
        || storage_class == sc_parameter || storage_class == sc_localstatic)
    {
        table = localNameSpace->syms;
    }
    else if (cparams.prm_cplusplus && isfunction(sp->tp) && theCurrentFunc && !getStructureDeclaration())
    {
        table = localNameSpace->syms;
    }
    else
    {
        table = globalNameSpace->syms ;
    }
    if (table)
    {
        if (isfunction(sp->tp) && !istype(sp))
        {
            char *name = sp->castoperator ? overloadNameTab[CI_CAST] : sp->name; 
            HASHREC **hr = LookupName(name, table);
            SYMBOL *funcs = NULL;
            if (hr)
                funcs = (SYMBOL *)(*hr)->p;
            if (!funcs)
            {
                TYPE *tp = (TYPE *)Alloc(sizeof(TYPE));
                tp->type = bt_aggregate;
                tp->rootType = tp;
                funcs = makeID(sc_overloads, tp, 0, name) ;
                funcs->castoperator = sp->castoperator;
                funcs->parentClass = sp->parentClass;
                funcs->parentNameSpace = sp->parentNameSpace;
                tp->sp = funcs;
                SetLinkerNames(funcs, linkage);
                insert(funcs, table);
                table = funcs->tp->syms = CreateHashTable(1);
                insert(sp, table);
                sp->overloadName = funcs;
            }
            else if (cparams.prm_cplusplus && funcs->storage_class == sc_overloads)
            {
                table = funcs->tp->syms;
                if (AddOverloadName(sp, table))
                {
                    // we are going to naively add duplicate overloads on the basis they may
                    // differ in return type, which may make a difference for example for enable_if as a return type
                    // but we won't fully implement this at this time, e.g. if it has enable_if it had better be
                    // defined inline.   If it is defined out of line the lack of matching when we do lookups
                    // will cause multiply defined references in GetOverloadedFunction
                    HASHREC **hr = &table->table[0];
                    int n = 0;
                    while (*hr)
                    {
                        if (!strcmp(sp->decoratedName, ((SYMBOL *)(*hr)->p)->decoratedName))
                        {
                            n++;
                            if (comparetypes(basetype(sp->tp)->btp, basetype(((SYMBOL *)(*hr)->p)->tp)->btp, TRUE))
                                break;
                        }
                        hr = &(*hr)->next;
                    }
                    if (!*hr)
                    {
                        sp->overlayIndex = n;
                        *hr = Alloc(sizeof(HASHREC));
                        (*hr)->p = (struct _hrintern_ *)sp;
                    }
                }
                sp->overloadName = funcs;
                if (sp->parent != funcs->parent || sp->parentNameSpace != funcs->parentNameSpace)
                    funcs->wasUsing = TRUE;
            }
            else
            {
                errorsym(ERR_DUPLICATE_IDENTIFIER, sp);
            }
        }
        else  if (!allowDups || sp != search(sp->name, table))
            insert(sp, table);
    }        
    else
    {
        diag("InsertSymbol: cannot insert");
    }
}
static LEXEME *tagsearch(LEXEME *lex, char *name, SYMBOL **rsp, HASHTABLE **table,
                         SYMBOL **strSym_out, NAMESPACEVALUES **nsv_out, enum e_sc storage_class)
{
    NAMESPACEVALUES *nsv = NULL;
    SYMBOL *strSym = NULL;
    
    HASHREC **hr;
    *rsp = NULL;
    if (ISID(lex) || MATCHKW(lex, classsel))
    {
        lex = nestedSearch(lex, rsp, &strSym, &nsv, NULL, NULL, TRUE, storage_class, FALSE, FALSE);
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
                    hr = LookupName((*rsp)->name, nsv->tags);
                else if (cparams.prm_cplusplus && (storage_class == sc_member || storage_class == sc_mutable))
                    hr = LookupName((*rsp)->name, getStructureDeclaration()->tp->tags);
                else if (storage_class == sc_auto)
                    hr = LookupName((*rsp)->name, localNameSpace->tags);
                else
                    hr = LookupName((*rsp)->name, globalNameSpace->tags);
                if (hr)
                    *rsp = (SYMBOL *)(*hr)->p;
                else
                {
                    if (nsv || strSym)
                    {
                        errorNotMember(strSym, nsv, (*rsp)->name);                    
                    }
                    *rsp = NULL;
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
        *table = nsv->tags;
    }
    else if (strSym)
    {
        *table = strSym->tp->tags;
    }
    else if (cparams.prm_cplusplus && (storage_class == sc_member || storage_class == sc_mutable))
    {
        strSym = getStructureDeclaration();
        *table = strSym->tp->tags;
    }
    else
    {
        if (storage_class == sc_auto)
        {
            *table = localNameSpace->tags;
            nsv = localNameSpace;
        }
        else
        {
            *table = globalNameSpace->tags;
            nsv = globalNameSpace;
        }
    }
    *nsv_out = nsv;
    *strSym_out = strSym;
    return lex;
}
static void checkIncompleteArray(TYPE *tp, char *errorfile, int errorline)
{
    HASHREC *hr;
    tp = basetype(tp);
    if (tp->syms == NULL)
        return ; /* should get a size error */
    hr = tp->syms->table[0];
    while (hr)
    {
        SYMBOL *sp;
        sp = (SYMBOL *)hr->p;
        if (hr->next == NULL)
        {
            if (ispointer(sp->tp) && basetype(sp->tp)->size == 0)
                specerror(ERR_STRUCT_MAY_NOT_CONTAIN_INCOMPLETE_STRUCT,"",errorfile, errorline);
        }
        hr = hr->next;
    }
}
LEXEME *get_type_id(LEXEME *lex, TYPE **tp, SYMBOL *funcsp, enum e_sc storage_class, BOOLEAN beforeOnly, BOOLEAN toErr)
{
    enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
    BOOLEAN defd = FALSE;
    SYMBOL *sp = NULL;
    BOOLEAN notype = FALSE;
    BOOLEAN oldTemplateType = inTemplateType;
    *tp = NULL;
        
    lex = getQualifiers(lex, tp, &linkage, &linkage2, &linkage3);
    lex = getBasicType(lex, funcsp, tp, NULL, FALSE, funcsp ? sc_auto : sc_global, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd, NULL, NULL, FALSE, FALSE);
    lex = getQualifiers(lex, tp, &linkage, &linkage2, &linkage3);
    lex = getBeforeType(lex, funcsp, tp, &sp, NULL, NULL, FALSE, storage_class, &linkage, &linkage2, &linkage3, FALSE, FALSE, beforeOnly, FALSE); /* fixme at file scope init */
    sizeQualifiers(*tp);
    if (notype)
        *tp = NULL;
    else if (sp && !sp->anonymous && toErr)
        if (sp->tp->type != bt_templateparam)
            error(ERR_TOO_MANY_IDENTIFIERS);
    if (sp && sp->anonymous)
        sp->linkage = linkage;
    inTemplateType = oldTemplateType;
    return lex;
}
SYMBOL * calculateStructAbstractness(SYMBOL *top, SYMBOL *sp)
{
    BASECLASS *bases = sp->baseClasses;
    HASHREC *hr = sp->tp->syms->table[0];
    while (bases)
    {
        if (bases->cls->isabstract)
        {
            SYMBOL *rv = calculateStructAbstractness(top, bases->cls);
            if (rv)
                return rv;
        }
        bases = bases->next;
    }
    while (hr)
    {
        SYMBOL *p = (SYMBOL *)hr->p;
        TYPE *tp = basetype(p->tp);
        if (p->storage_class == sc_overloads)
        {
            HASHREC *hri = p->tp->syms->table[0];
            while (hri)
            {
                SYMBOL *pi = (SYMBOL *)hri->p;
                if (pi->ispure)
                {
                    // ok found a pure function, look it up within top and
                    // check to see if it has been overrridden
                    SYMBOL *pq;
                    STRUCTSYM l;
                    l.str = (void *)top;
                    addStructureDeclaration(&l);
                    pq = classsearch(pi->name, FALSE, TRUE);
                    dropStructureDeclaration();
                    if (pq)
                    {
                        HASHREC *hrq = pq->tp->syms->table[0];
                        while (hrq)
                        {
                            SYMBOL *pq1 = (SYMBOL *)hrq->p;
                            char *p1 = strrchr(pq1->decoratedName, '@');
                            char *p2 = strrchr(pi->decoratedName, '@');
                            if (p1 && p2 && !strcmp(p1, p2))
                            {
                                if (!pq1->ispure)
                                {
                                    return NULL;
                                }
                                else
                                {
                                    break;
                                }
                            }
                            hrq = hrq->next;
                        }
                        // if it either isn't found or was found and is pure...
                        top->isabstract = TRUE;
                        return pi;
                    }
                }
                hri = hri->next;
            }
        }
        hr = hr->next;
    }
    return NULL;
}
void calculateStructOffsets(SYMBOL *sp)
{
    enum e_bt type = basetype(sp->tp)->type;
    enum e_bt bittype = bt_none;
    int startbit = 0;
    int maxbits = 0;
    int nextoffset = 0;
    int maxsize = 0;
    HASHREC *hr = sp->tp->syms->table[0];
    int size = 0;
    int totalAlign = -1;
    BASECLASS *bases = sp->baseClasses;
    
    if (bases)
    {
        if (sp->hasvtab && !bases->cls->hasvtab)
            bases->offset = getSize(bt_pointer);
        else
            bases->offset = 0;
    }
    if (sp->hasvtab && (!sp->baseClasses || !sp->baseClasses->cls->hasvtab || sp->baseClasses->isvirtual))
        size += getSize(bt_pointer);
    while (bases)
    {
        SYMBOL *sym = bases->cls;
        int align = sym->structAlign;
        totalAlign = imax(totalAlign, align);
        if (align > 1)
        {
            int al2 = align - size % align;
            if (al2 != align)
                size += al2;
        }
        if (!bases->isvirtual)
        {
            if (bases != sp->baseClasses)
                bases->offset = size;
            size += sym->sizeNoVirtual;
        }
        bases = bases->next;
    }
    while (hr)
    {
        SYMBOL *p = (SYMBOL *)hr->p;
        TYPE *tp = basetype(p->tp);
        if (p->storage_class != sc_static && p->storage_class != sc_external && p->storage_class != sc_overloads
            && p->storage_class != sc_enumconstant && !istype(p) && p != sp && p->parentClass == sp) 
                    // not function, also not injected self or base class or static variable
        {
            int align ;
            int offset ;
            
            if (isstructured(tp))
            {
                   align = tp->sp->structAlign;
            }
            else
            {
                align = getAlign(sc_member, tp);
                
            }
            totalAlign = imax(totalAlign, align);
            p->parent = sp;
            if (tp->size == 0)
            {
                if (cparams.prm_c99 && tp->type == bt_pointer && p->tp->array)
                {
                    if (!hr->next || p->init)
                    {
                        offset = nextoffset;
                        nextoffset = tp->btp->size;
                        goto join;
                    }
                    if (!p->tp->vla)
                        error(ERR_EMPTY_ARRAY_LAST);
                }
                else
                    if (!cparams.prm_cplusplus && p->storage_class == sc_overloads)
                        error(ERR_STRUCT_UNION_NO_FUNCTION);
                    else /* c90 doesn't allow unsized arrays here */
                        if (cparams.prm_ansi)
                            errorsym(ERR_UNSIZED, p);
            }
            if (!isunion(tp) && isstructured(tp))
            {
                checkIncompleteArray(tp, p->declfile, p->declline);
            }
            if (isstructured(tp) && !tp->size && !templateNestingCount)
            {
                errorsym(ERR_UNSIZED, p);
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
    //				nextoffset = (startbit+ chosenAssembler->arch->bits_per_mau -1)
    //						/chosenAssembler->arch->bits_per_mau;
                    offset = 0;
                }
                else
                {
                    offset = nextoffset;
    //				nextoffset = (tp->bits + chosenAssembler->arch->bits_per_mau -1)
    //					/chosenAssembler->arch->bits_per_mau;
                    nextoffset = tp->size;
                    bittype = tp->type;
                    startbit = tp->bits;
                    tp->startbit = 0;
                    maxbits = chosenAssembler->arch->bits_per_mau * tp->size;
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
            p->offset = size;
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
        if (cparams.prm_cplusplus)
        {
            // make it non-zero size to avoid further errors...
            size = getSize(bt_int);
        }
    }
    sp->tp->size = size ;
    sp->structAlign = totalAlign;
//    if (cparams.prm_cplusplus)
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
static BOOLEAN validateAnonymousUnion(SYMBOL *parent, TYPE *unionType)
{
    BOOLEAN rv = TRUE;
    unionType = basetype(unionType);
    if (cparams.prm_cplusplus && (!unionType->sp->trivialCons))// || unionType->tags->table[0]->next))
    {
        error(ERR_ANONYMOUS_UNION_NO_FUNCTION_OR_TYPE);
        rv = FALSE;
    }
    else
    {
        HASHREC *newhr =  unionType->syms->table[0];
        while (newhr)
        {
            HASHREC **hhr;
            SYMBOL *member = (SYMBOL *)newhr->p;
            if (cparams.prm_cplusplus && member->storage_class == sc_overloads)
            {
                HASHREC *hr = basetype(member->tp)->syms->table[0];
                while (hr)
                {
                    if (!((SYMBOL *)hr->p)->defaulted)
                    {
                        error(ERR_ANONYMOUS_UNION_NO_FUNCTION_OR_TYPE);
                        rv = FALSE;
                        break;
                    }
                    hr = hr->next;
                }
            }
            else if ((cparams.prm_cplusplus && member->storage_class == sc_type) || member->storage_class == sc_typedef)
            {
                error(ERR_ANONYMOUS_UNION_NO_FUNCTION_OR_TYPE);
                rv = FALSE;
            }
            else if ((cparams.prm_cplusplus && member->access == ac_private) || member->access == ac_protected)
            {
                error(ERR_ANONYMOUS_UNION_PUBLIC_MEMBERS);
                rv = FALSE;
            }
            else if (cparams.prm_cplusplus && member->storage_class != sc_member && member->storage_class != sc_mutable)
            {
                error(ERR_ANONYMOUS_UNION_NONSTATIC_MEMBERS);
                rv = FALSE;
            }
            else if (parent && (hhr = LookupName(member->name, parent->tp->syms)) != NULL)
            {
                SYMBOL *spi = (SYMBOL *)(*hhr)->p;
                currentErrorLine = 0;
                preverrorsym(ERR_DUPLICATE_IDENTIFIER, spi, spi->declfile, spi->declline);
                rv = FALSE;
            }
            newhr = newhr->next;
        }
    }    
    return rv;
}
static void resolveAnonymousGlobalUnion(SYMBOL *sp)
{
    HASHREC *hr = sp->tp->syms->table[0];
    validateAnonymousUnion(NULL, sp->tp);
    sp->label = nextLabel++;
    sp->storage_class = sc_localstatic;
    insertInitSym(sp);
    while (hr)
    {
        SYMBOL *sym = (SYMBOL *)hr->p;
        if (sym->storage_class == sc_member || sym->storage_class == sc_mutable)
        {
            SYMBOL *spi;
            if ((spi = gsearch(sym->name)) != NULL)
            {
                currentErrorLine = 0;
                preverrorsym(ERR_DUPLICATE_IDENTIFIER, spi, spi->declfile, spi->declline);
            }
            else
            {
                sym->storage_class = sc_localstatic;
                sym->label = sp->label;
                InsertSymbol(sym, sc_static, lk_c, FALSE);
            }
        }
        hr = hr->next;
    }
}
void resolveAnonymousUnions(SYMBOL *sp)
{
    HASHREC **member = (HASHREC **)&sp->tp->syms->table[0];
    if (total_errors)
        return;
    while (*member)
    {
        SYMBOL *spm = (SYMBOL *)(*member)->p;
        // anonymous structured type declaring anonymous variable is a candidate for
        // an anonymous structure or union
        if (isstructured(spm->tp) && spm->anonymous && basetype(spm->tp)->sp->anonymous)
        {
#ifdef ERROR
#error NESTEDANONYUNION
#endif
            HASHREC *next = (*member)->next;
            resolveAnonymousUnions(spm);
            validateAnonymousUnion(sp, spm->tp);
            *member = spm->tp->syms->table[0];
            if (basetype(spm->tp)->type == bt_union)
            {
                if (!cparams.prm_c99 && !cparams.prm_cplusplus)
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
                SYMBOL *newsp = (SYMBOL *)(*member)->p;
                if ((newsp->storage_class == sc_member || newsp->storage_class == sc_mutable) && !isfunction(newsp->tp))
                {
                    newsp->offset += spm->offset;
                    newsp->parentClass = sp;
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
static BOOLEAN usesClass(SYMBOL *cls, SYMBOL *internal)
{
    if (cls->tp->syms)
    {
        HASHREC *hr = cls->tp->syms->table[0];
        while (hr)
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            TYPE *tp = sym->tp;
            if (istype(sym))
            { 
                if (sym->storage_class != sc_typedef)
                {
                    tp = NULL;
                }
            }
            if (tp)
            {
                while (isarray(tp))
                    tp = basetype(tp)->btp;
                if (comparetypes(internal->tp, tp, TRUE) || sameTemplate(internal->tp, tp))
                    return TRUE;
            }
            hr = hr->next;
        }
    }
    return FALSE;
}
static void baseFinishDeclareStruct(SYMBOL *funcsp)
{
    int n = 0,i,j;
    LIST *lst = openStructs;
    SYMBOL ** syms;
    while (lst) n++, lst = lst->next;
    syms = (SYMBOL *)Alloc(sizeof(SYMBOL *)*n); 
    n = 0;
    lst = openStructs;
    openStructs = NULL;
    while (lst)
    {
        syms[n++] =  (SYMBOL *)lst->data, lst = lst->next;
    }
    for (i=0; i < n; i++)
        for (j = i+1; j < n; j++)
            if ((syms[i] != syms[j] && !sameTemplate(syms[i]->tp, syms[j]->tp) && classRefCount(syms[j], syms[i])) || usesClass(syms[i], syms[j]))
            {
                SYMBOL *x = syms[j];
                memmove(&syms[i+1], &syms[i], sizeof(SYMBOL *) * (j - i));
                syms[i] = x;
                if (j != i + 1)
                    j = i;
            }
    for (i=0; i < n; i++)
    {
        if (!syms[i]->performedStructInitialization)
        {
            syms[i]->performedStructInitialization = TRUE;
            if (cparams.prm_cplusplus)
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
static LEXEME *structbody(LEXEME *lex, SYMBOL *funcsp, SYMBOL *sp, enum e_ac currentAccess)
{
    STRUCTSYM sl;
    (void)funcsp;
    if (cparams.prm_cplusplus)
    {
        LIST *lst = Alloc(sizeof(LIST));
        lst->next = openStructs;
        openStructs = lst;
        lst->data = sp;
    }
    lex = getsym();
    sl.str = sp;
    addStructureDeclaration(&sl);
    while (lex && KW(lex) != end)
    {
        FlushLineData(lex->file, lex->line);
        switch(KW(lex))
        {
            case kw_private:
                sp->accessspecified = TRUE;
                currentAccess = ac_private;
                lex = getsym();
                needkw(&lex, colon);
                break;
            case kw_public:
                sp->accessspecified = TRUE;
                currentAccess = ac_public;
                lex = getsym();
                needkw(&lex, colon);
                break;
            case kw_protected:
                sp->accessspecified = TRUE;
                currentAccess = ac_protected;
                lex = getsym();
                needkw(&lex, colon);
                break;
            case kw_friend:
                lex = getsym();
                lex = declare(lex, NULL, NULL, sc_member, lk_none, NULL, TRUE, FALSE, TRUE, FALSE, currentAccess);
                break;
            default:
                lex = declare(lex, NULL, NULL, sc_member, lk_none, NULL, TRUE, FALSE, FALSE, FALSE, currentAccess);
                break;
        }
    }
    dropStructureDeclaration();
    sp->hasvtab = usesVTab(sp);
    calculateStructOffsets(sp);

    if (cparams.prm_cplusplus)
    {
        calculateStructAbstractness(sp, sp);
        calculateVirtualBaseOffsets(sp);
        calculateVTabEntries(sp, sp, &sp->vtabEntries, 0);
        if (sp->vtabEntries)
        {
            char buf[4096];
            InsertInline(sp);
            my_sprintf(buf, "%s@_$vt", sp->decoratedName);
            sp->vtabsp = makeID(sc_static, &stdvoid, NULL, litlate(buf));
            sp->vtabsp->linkage = lk_virtual;
            sp->vtabsp->decoratedName = sp->vtabsp->errname = sp->vtabsp->name;
            warnCPPWarnings(sp, funcsp != NULL);
        }
    }
    else
    {
        sp->trivialCons = TRUE;
    }
	if (cparams.prm_cplusplus)
        createDefaultConstructors(sp);
    resolveAnonymousUnions(sp);
    if (cparams.prm_cplusplus)
        deferredInitializeStructMembers(sp);
    if (!cparams.prm_cplusplus || structLevel == 1)
    {
        structLevel--;
        baseFinishDeclareStruct(funcsp);
        structLevel++;
    }
    if (cparams.prm_cplusplus && sp->tp->syms && !templateNestingCount)
    {
        SYMBOL *cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(sp->tp)->syms);
        if (!cons)
        {
            HASHREC *hr = basetype(sp->tp)->syms->table[0];
            while (hr)
            {
                SYMBOL *sp = (SYMBOL *)hr->p;
                if (sp->storage_class == sc_member || sp->storage_class == sc_mutable)
                {
                    if (isref(sp->tp))
                    {
                        errorsym(ERR_REF_CLASS_NO_CONSTRUCTORS, sp);
                    }
                    else if (isconst(sp->tp))
                    {
                        errorsym(ERR_CONST_CLASS_NO_CONSTRUCTORS, sp);
                    }
                }
                hr = hr->next;
            }
        }
    }
    if (!lex)
    {
        error (ERR_UNEXPECTED_EOF);
    }
    else
    {
        lex = getsym();
        FlushLineData("", 0);
    }
    return lex;
}
LEXEME *innerDeclStruct(LEXEME *lex, SYMBOL *funcsp, SYMBOL *sp, BOOLEAN inTemplate, enum e_ac defaultAccess, BOOLEAN isfinal, BOOLEAN *defd)
{
    BOOLEAN hasBody = (cparams.prm_cplusplus && KW(lex) == colon) || KW(lex) == begin;
    SYMBOL *injected = NULL;

    if (/*templateNestingCount &&*/ nameSpaceList)
        SetTemplateNamespace(sp);
    if (sp->structAlign == 0)
        sp->structAlign = 1;
    structLevel++;
    sp->declaring = TRUE;
    if (hasBody)
    {
        if (sp->tp->syms || sp->tp->tags)
        {
            preverrorsym(ERR_STRUCT_HAS_BODY, sp, sp->declfile, sp->declline);
        }
        sp->tp->syms = CreateHashTable(1);
        if (cparams.prm_cplusplus)
        {
            sp->tp->tags = CreateHashTable(1);
            injected = clonesym(sp);
            injected->mainsym = sp; // for constructor/destructor matching
            insert(injected, sp->tp->tags); // inject self
            injected->access = ac_public;
        }
    }
    if (inTemplate && templateNestingCount == 1)
        inTemplateBody++;
    if (cparams.prm_cplusplus)
        if  (KW(lex) == colon)
        {
            lex = baseClasses(lex, funcsp, sp, defaultAccess);
            if (injected)
                injected->baseClasses = sp->baseClasses;
            if (!MATCHKW(lex, begin))
                errorint(ERR_NEEDY, '{');
        }
    if (inTemplate && templateNestingCount == 1)
        noSpecializationError++;
    if (KW(lex) == begin)
    {
        sp->isfinal = isfinal;
        lex = structbody(lex, funcsp, sp, defaultAccess);
        *defd = TRUE;
    }
    if (inTemplate && templateNestingCount == 1)
    {
        inTemplateBody--;
        noSpecializationError--;
        TemplateGetDeferred(sp);
    }
    sp->declaring = FALSE;
    --structLevel;
    return lex;
}
static unsigned char *ParseUUID(LEXEME **lex)
{
    if (MATCHKW(*lex, kw__uuid))
    {
        unsigned vals[16];
        unsigned char *rv = (unsigned char *)Alloc(16);
        *lex = getsym();
        needkw(lex, openpa);
        if ((*lex)->type == l_astr)
        {
            int i;
            int count = ((SLCHAR *)(*lex)->value.s.w)->count;
            LCHAR *str = ((SLCHAR *)(*lex)->value.s.w)->str;
            unsigned short buf[200];
            for (i=0; i < count ;i++)
                buf[i] = *str++;
            buf[i] = 0;
            if (11 == swscanf(buf, L"%x-%x-%x-%02x%02x-%02x%02x%02x%02x%02x%02x",
               &vals[0], &vals[1], &vals[2],
               &vals[8], &vals[9], &vals[10], &vals[11],
               &vals[12], &vals[13], &vals[14], &vals[15]))
            {
                int i;
                rv[0] = vals[0] & 0xff;
                rv[1] = (vals[0] >>8) & 0xff;
                rv[2] = (vals[0] >>16)& 0xff;
                rv[3] = (vals[0] >>24)& 0xff;
                rv[4] = vals[1] & 0xff;
                rv[5] = (vals[1] >>8) & 0xff;
                rv[6] = vals[2] & 0xff;
                rv[7] = (vals[2] >>8) & 0xff;
                for (i=8; i < 16; i++)
                    rv[i] = vals[i];
            }
            *lex = getsym();
        }
        needkw(lex, closepa);
        return rv;
    }
    return NULL;
}
static LEXEME *declstruct(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, BOOLEAN inTemplate, enum e_sc storage_class, enum e_ac access, BOOLEAN *defd)
{
    BOOLEAN isfinal = FALSE;
    HASHTABLE *table;
    char *tagname ;
    char newName[4096];
    enum e_bt type = bt_none;
    SYMBOL *sp;
    int charindex;
    NAMESPACEVALUES *nsv;
    SYMBOL *strSym;
    enum e_ac defaultAccess;
    BOOLEAN addedNew = FALSE;
    int declline = lex->line;
    BOOLEAN anonymous = FALSE;
    unsigned char *uuid;
    *defd = FALSE;
    switch(KW(lex))
    {
        case kw_class:
            defaultAccess = ac_private;
            type = bt_class;
            break;
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
    ParseAttributeSpecifiers(&lex, funcsp, TRUE);
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
        anonymous = TRUE;
    }
    strcpy(newName, tagname);
    if (inTemplate)
        inTemplateSpecialization++;
    lex = tagsearch(lex, newName, &sp, &table, &strSym, &nsv, storage_class);
    if (inTemplate)
        inTemplateSpecialization--;

    if (charindex != -1 && cparams.prm_cplusplus && ISID(lex) && !strcmp(lex->value.s.a, "final"))
    {
        isfinal = TRUE;
        lex = getsym();
        if (!MATCHKW(lex, begin) && !MATCHKW(lex, colon))
            errorint(ERR_NEEDY, '{');
    }
    if (!sp)
    {
        addedNew = TRUE;
        sp = Alloc(sizeof(SYMBOL ));
        if (!strcmp(newName, tagname))
            sp->name = tagname;
        else
            sp->name = litlate(newName);
        sp->anonymous = anonymous;
        sp->storage_class = sc_type;
        sp->tp = Alloc(sizeof(TYPE));
        sp->tp->type = type;
        sp->tp->rootType = sp->tp;
        sp->tp->sp = sp;
        sp->declcharpos = charindex;
        sp->declline = sp->origdeclline = declline;
        sp->declfile = sp->origdeclfile = lex->file;
        sp->declfilenum = lex->filenum;
        if ((storage_class == sc_member || storage_class == sc_mutable) && (MATCHKW(lex, begin) || MATCHKW(lex, colon) || MATCHKW(lex, kw_try) || MATCHKW(lex, semicolon)))
            sp->parentClass = getStructureDeclaration();
        if (nsv)
            sp->parentNameSpace = nsv->name;
        if (inTemplate)
            sp->parentTemplate = sp;
        sp->anonymous = charindex == -1;
        sp->access = access;
        sp->uuid = uuid;
        SetLinkerNames(sp, lk_cdecl);
        if (inTemplate && templateNestingCount)
        {
            if (MATCHKW(lex, lt))
                errorsym(ERR_SPECIALIZATION_REQUIRES_PRIMARY, sp);
            sp->templateParams = TemplateGetParams(sp);
            sp->templateLevel = templateNestingCount;
            TemplateMatching(lex, NULL, sp->templateParams, sp, 
                             MATCHKW(lex, begin) || MATCHKW(lex, colon));
            SetLinkerNames(sp, lk_cdecl);
        }
        browse_variable(sp);
        
        insert(sp, cparams.prm_cplusplus && !sp->parentClass ? globalNameSpace->tags : table);
    }
    else if (type != sp->tp->type && (type == bt_union || sp->tp->type == bt_union))
    {
        errorsym(ERR_MISMATCHED_STRUCTURED_TYPE_IN_REDEFINITION, sp);
    }
    else if (inTemplate && templateNestingCount)
    {
        // definition or declaration
        if (!sp->templateLevel)
        {
            if (!sp->parentClass || !sp->parentClass->templateLevel)
                errorsym(ERR_NOT_A_TEMPLATE, sp);
            SetLinkerNames(sp, lk_cdecl);
        }
        else 
        {
            if (inTemplate && KW(lex) == lt)
            {
                SYMBOL *sp1;
                TEMPLATEPARAMLIST *origParams = sp->templateParams;
                TEMPLATEPARAMLIST *templateParams = TemplateGetParams(sp);
                inTemplateSpecialization++;
                parsingSpecializationDeclaration = TRUE;
                lex = GetTemplateArguments(lex, funcsp, NULL, &templateParams->p->bySpecialization.types);
                parsingSpecializationDeclaration = FALSE;
                inTemplateSpecialization--;
                sp = LookupSpecialization(sp, templateParams);
                sp->templateParams = TemplateMatching(lex, origParams, templateParams, sp,
                                                      MATCHKW(lex, begin) || MATCHKW(lex, colon));
            }
            else
            {
                LIST *instants;
                TEMPLATEPARAMLIST *templateParams = TemplateGetParams(sp);
                sp->templateParams = TemplateMatching(lex, sp->templateParams, templateParams, sp,
                                                      MATCHKW(lex, begin) || MATCHKW(lex, colon));
                instants = sp->parentTemplate->instantiations;
                while (instants)
                {
                    SYMBOL *instant = (SYMBOL *)instants->data;
                    TEMPLATEPARAMLIST *tpln = instant->templateParams->next;
                    TEMPLATEPARAMLIST *tpl = templateParams->next;
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
            TEMPLATEPARAMLIST *templateParams = TemplateGetParams(sp);
            lex = GetTemplateArguments(lex, funcsp, NULL, &templateParams->p->bySpecialization.types);
        }
        else if (sp->templateLevel)
        {
            if ( (MATCHKW(lex, begin) || MATCHKW(lex, colon)))
            {
                errorsym(ERR_IS_ALREADY_DEFINED_AS_A_TEMPLATE, sp);
            }
            else
            {        
                TEMPLATEPARAMLIST *lst = NULL;
                lex = GetTemplateArguments(lex, funcsp, NULL, &lst);
                sp = GetClassTemplate(sp, lst, FALSE);
            }
        }
    }
    else if (sp->templateLevel && (MATCHKW(lex, begin) || MATCHKW(lex, colon)))
    {
        errorsym(ERR_IS_ALREADY_DEFINED_AS_A_TEMPLATE, sp);
    }
    if (sp)
    {
        if (uuid)
            sp->uuid = uuid;
        if (access != sp->access && sp->tp->syms && (MATCHKW(lex, begin) || MATCHKW(lex, colon)))
        {
            errorsym(ERR_CANNOT_REDEFINE_ACCESS_FOR, sp);
        }
        lex = innerDeclStruct(lex, funcsp, sp, inTemplate, defaultAccess, isfinal, defd);
        *tp = sp->tp;
    }
    return lex;
}
static LEXEME *enumbody(LEXEME *lex, SYMBOL *funcsp, SYMBOL *spi, 
                        enum e_sc storage_class, TYPE *fixedType, BOOLEAN scoped)
{
    LLONG_TYPE enumval = 0;
    TYPE *unfixedType;
    BOOLEAN fixed = cparams.prm_cplusplus ? TRUE : FALSE;
    unfixedType = spi->tp->btp;
    if (!unfixedType)
        unfixedType = &stdint;
    lex = getsym();
    spi->declaring = TRUE;
    if (spi->tp->syms)
    {
        preverrorsym(ERR_ENUM_CONSTANTS_DEFINED, spi, spi->declfile, spi->declline);
    }
    spi->tp->syms = CreateHashTable(1); /* holds a list of all the enum values, e.g. for debug info */
    while (lex)
    {
        if (ISID(lex))
        {
            SYMBOL *sp;
            TYPE *tp ;
            if (cparams.prm_cplusplus)
            {
                tp = Alloc(sizeof(TYPE));			
                if (fixedType)
                    *tp = *fixedType;
                else
                    *tp = *unfixedType;
                tp->scoped = scoped; // scoped the constants type as well for error checking
                tp->btp = spi->tp; // the integer type gets a base type which is the enumeration for error checking
                tp->rootType = tp;
                tp->enumConst = TRUE;
                tp->sp = spi;
            }
            else
            {
                tp = Alloc(sizeof(TYPE));			
                tp->type = bt_int;
                tp->size = getSize(bt_int);
                tp->rootType = tp;
            }
            sp = makeID(sc_enumconstant, tp, 0, litlate(lex->value.s.a)) ;
            sp->name = sp->errname = sp->decoratedName = litlate(lex->value.s.a);
            sp->declcharpos = lex->charindex;
            sp->declline = sp->origdeclline = lex->line;
            sp->declfile = sp->origdeclfile = lex->file;
            sp->declfilenum = lex->filenum;
            sp->parentClass = spi->parentClass;
            browse_variable(sp);
            if (cparams.prm_cplusplus)
            {
                if (!sp->tp->scoped) // dump it into the parent table unless it is a C++ scoped enum
                    InsertSymbol(sp, storage_class, lk_c, FALSE);
            }
            else // in C dump it into the globals
            {
                if (funcsp)
                    insert(sp, localNameSpace->syms);
                else
                    insert(sp, globalNameSpace->syms);
            }
            insert(sp, spi->tp->syms);
            lex = getsym();
            if (MATCHKW(lex, assign))
            {
                TYPE *tp = NULL;
                EXPRESSION *exp = NULL;
                lex = getsym();
                lex = optimized_expression(lex, funcsp, NULL, &tp, &exp, FALSE);
                if (tp && isintconst(exp))
                {
                    if (cparams.prm_cplusplus)
                    {
                        if (!fixedType)
                        {
                            if (tp->type != bt_bool)
                            {
                                if (!comparetypes(tp, unfixedType, TRUE))
                                    fixed = FALSE;
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
                                LLONG_TYPE v = 1;
                                LLONG_TYPE n = ~((v << (fixedType->size * 8-1))-1);
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
            sp->value.i = enumval++;
            if (cparams.prm_cplusplus && spi->tp->btp)
            {
                if (fixedType)
                {
                    LLONG_TYPE v = 1;
                    LLONG_TYPE n = ~((v << (fixedType->size * 8-1))-1);
                    if ((sp->value.i & n) != 0 && (sp->value.i & n) != n)
                        error(ERR_ENUMERATION_OUT_OF_RANGE_OF_BASE_TYPE);
                }
                else
                {
                    LLONG_TYPE v = 1;
                    LLONG_TYPE n = ~((v << (spi->tp->size * 8))-1);
                    if ((sp->value.i & n) != 0 && (sp->value.i & n) != n)
                    {
                        spi->tp->btp->type = bt_long_long;
                        spi->tp->size = spi->tp->btp->size = getSize(bt_long_long);
                        unfixedType = spi->tp->btp;
                    }
                }
            }
            if (!MATCHKW(lex,comma))
                break;  
            else
            {
                lex = getsym();
                if (KW(lex) == end)
                {
                    if (cparams.prm_ansi && !cparams.prm_c99 && !cparams.prm_cplusplus)
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
    if (!lex)
        error (ERR_UNEXPECTED_EOF);
    else if (!MATCHKW(lex,end))
    {
        errorint(ERR_NEEDY, '}');
        errskim(&lex, skim_end);
        skip(&lex, end);
    }
    else
        lex = getsym();
    if (fixed)
        spi->tp->fixed = TRUE;
    spi->declaring = FALSE;
    return lex;
}
static LEXEME *declenum(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, enum e_sc storage_class, enum e_ac access, BOOLEAN opaque, BOOLEAN *defd)
{
    HASHTABLE *table;
    char *tagname ;
    char newName[4096];
    int charindex;
    BOOLEAN noname = FALSE;
    BOOLEAN scoped = FALSE;
    TYPE *fixedType = NULL;
    SYMBOL *sp;
    NAMESPACEVALUES *nsv;
    SYMBOL *strSym;
    int declline = lex->line;
    BOOLEAN anonymous = FALSE;
    *defd = FALSE;
    lex = getsym();
    ParseAttributeSpecifiers(&lex, funcsp, TRUE);
    if (cparams.prm_cplusplus && (MATCHKW(lex, kw_class)  || MATCHKW(lex, kw_struct)))
    {
        scoped = TRUE;
        lex = getsym();
    }
    if (ISID(lex))
    {
        charindex = lex->charindex;
        tagname = litlate(lex->value.s.a);
    }
    else
    {
        noname = TRUE;
        if (!MATCHKW(lex, begin) && !MATCHKW(lex, classsel))
            errorint(ERR_NEEDY, '{');
        tagname = AnonymousTypeName();
        charindex = -1;
        anonymous = TRUE;
    }

    strcpy(newName, tagname);
    lex = tagsearch(lex, newName, &sp, &table, &strSym, &nsv, storage_class);

    if (cparams.prm_cplusplus && KW(lex) == colon)
    {
        lex = getsym();
        lex = get_type_id(lex, &fixedType, funcsp, sc_cast, FALSE, TRUE);
        if (!fixedType || !isint(fixedType))
        {
            error(ERR_NEED_INTEGER_TYPE);
        }
        else
        {
            fixedType = basetype(fixedType); // ignore qualifiers
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
        sp = Alloc(sizeof(SYMBOL ));
        if (!strcmp(newName, tagname))
            sp->name = tagname;
        else
            sp->name = litlate(newName);
        sp->access = access;
        sp->anonymous = anonymous;
        sp->storage_class = sc_type;
        sp->tp = Alloc(sizeof(TYPE));
        sp->tp->type = bt_enum;
        sp->tp->rootType = sp->tp;
        if (fixedType)
        {
            sp->tp->fixed = TRUE;
            sp->tp->btp = fixedType;
        }
        else
        {
            sp->tp->btp = Alloc(sizeof(TYPE));
            sp->tp->btp->type = bt_int;
            sp->tp->btp->size = getSize(bt_int);
            sp->tp->btp->rootType = sp->tp->btp;
        }
        sp->tp->scoped = scoped;
        sp->tp->size = sp->tp->btp->size;
        sp->declcharpos = charindex;
        sp->declline = sp->origdeclline = declline;
        sp->declfile = sp->origdeclfile = lex->file;
        sp->declfilenum = lex->filenum;
        if (storage_class == sc_member || storage_class == sc_mutable)
            sp->parentClass = getStructureDeclaration();
        if (nsv)
            sp->parentNameSpace = nsv->name;
        sp->anonymous = charindex == -1;
        SetLinkerNames(sp, lk_cdecl);
        browse_variable(sp);
        insert(sp, cparams.prm_cplusplus && !sp->parentClass ? globalNameSpace->tags : table);
    }
    else if (sp->tp->type != bt_enum)
    {
        errorsym(ERR_ORIGINAL_TYPE_NOT_ENUMERATION, sp);
    }
    else if (scoped != sp->tp->scoped || (fixedType && sp->tp->type != fixedType->type))
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
        lex = enumbody(lex, funcsp, sp, storage_class, fixedType, scoped);
        enumSyms = NULL;
        *defd = TRUE;
    }
    else if (!cparams.prm_cplusplus && cparams.prm_ansi && !sp->tp->syms)
    {
        errorsym(ERR_ANSI_ENUM_NEEDS_BODY, sp);
    }
    sp->tp->sp = sp;
    *tp = sp->tp;
    return lex;
}
static LEXEME *getStorageClass(LEXEME *lex, SYMBOL *funcsp, enum e_sc *storage_class, 
                               enum e_lk *linkage, ADDRESS *address, BOOLEAN *blocked, BOOLEAN *isExplicit, enum e_ac access)
{
    BOOLEAN found = FALSE;
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
                if (cparams.prm_cplusplus)
                {
                    if (lex->type == l_astr)
                    {
                        SLCHAR *ch = (SLCHAR *)lex->value.s.w;
                        char buf[256];
                        int i;
                        enum e_lk next = lk_none;
                        for (i=0; i < ch->count; i++)
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
                        if (next != lk_none)
                        {
                            *linkage = next;
                            lex = getsym();
                            if (MATCHKW(lex, begin))
                            {
                                *blocked = TRUE;
                                lex = getsym();
                                while (lex && !MATCHKW(lex, end))
                                {
                                    lex = declare(lex, NULL, NULL, sc_global, *linkage, NULL, TRUE, FALSE, FALSE, FALSE, ac_public) ;
                                }
                                needkw(&lex, end);
                                return lex;
                            }
                        }
                        else
                        {
                            char buf[512],*q=buf;
                            LCHAR *p = ch->str;
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
                        *isExplicit = TRUE;
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
                if (*storage_class == sc_parameter || (!cparams.prm_cplusplus && (*storage_class == sc_member || *storage_class == sc_mutable)))
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
                    TYPE *tp = NULL;
                    EXPRESSION *exp = NULL;
                    lex = getsym();
                    lex = optimized_expression(lex, funcsp, NULL, &tp, &exp, FALSE);
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
        }
        if (found)
            error(ERR_TOO_MANY_STORAGE_CLASS_SPECIFIERS);
        found = TRUE;
    }
    return lex;
}
static LEXEME *getPointerQualifiers(LEXEME *lex, TYPE **tp, BOOLEAN allowstatic)
{
    while (KWTYPE(lex, TT_TYPEQUAL)|| (allowstatic && MATCHKW(lex, kw_static)))
    {
        TYPE *tpn;
        TYPE *tpl;
        if (MATCHKW(lex, kw__intrinsic))
        {
            lex = getsym();
            continue;
        }
        tpn = Alloc(sizeof(TYPE));
        if (*tp)
            tpn->size = (*tp)->size;
        switch(KW(lex))
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
            
        }
        tpl = *tp;
        while (tpl && tpl->type != bt_pointer)
        {
            if (tpl->type == tpn->type)
                errorstr(ERR_DUPLICATE_TYPE_QUALIFIER, lex->kw->name);
            tpl = tpl->btp;
        }
        tpn->btp = *tp;
        if (*tp)
            tpn->rootType = (*tp)->rootType;
        *tp = tpn;		
        lex = getsym();
    }
    return lex;
}
static LEXEME *getLinkageQualifiers(LEXEME *lex, enum e_lk *linkage, enum e_lk *linkage2, enum e_lk *linkage3)
{
    while (KWTYPE(lex, TT_LINKAGE))
    {
        enum e_kw kw= KW(lex);
        lex = getsym();
        switch(kw)
        {
            case kw__cdecl:
                if (*linkage != lk_none && *linkage != lk_cdecl)
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = lk_cdecl;
                break;
            case kw__stdcall:
                if (*linkage != lk_none && *linkage != lk_stdcall && (!cparams.prm_cplusplus || *linkage != lk_c))
                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                *linkage = lk_stdcall;
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
                importFile = NULL;
                if (MATCHKW(lex, openpa))
                {
                    lex = getsym();
                    if (lex->type == l_astr)
                    {
                        int i, len = ((SLCHAR *)lex->value.s.w)->count;
                        importFile = Alloc(len + 1);
                        for (i=0; i < len; i++)
                            importFile[i] = (char)((SLCHAR *)lex->value.s.w)->str[i];
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
LEXEME *getQualifiers(LEXEME *lex, TYPE **tp, enum e_lk *linkage, enum e_lk *linkage2, enum e_lk *linkage3)
{
    while (KWTYPE(lex, (TT_TYPEQUAL | TT_LINKAGE)))
    {
        lex = getPointerQualifiers(lex, tp, FALSE);
        if (MATCHKW(lex, kw_atomic))
            break;
        lex = getLinkageQualifiers(lex, linkage, linkage2, linkage3);
    }
    return lex;
}
static LEXEME *nestedTypeSearch(LEXEME *lex, SYMBOL **sym)
{
    *sym = NULL;
    lex = nestedSearch(lex, sym, NULL, NULL, NULL, NULL, FALSE, sc_global, FALSE, TRUE);
    if (!*sym || !istype((*sym)))
    {
        error(ERR_TYPE_NAME_EXPECTED);
    }
    return lex;
}
static BOOLEAN isPointer(LEXEME *lex)
{
    while (KWTYPE(lex, (TT_TYPEQUAL | TT_LINKAGE)))
        lex = getsym();
    if (ISKW(lex))
        switch(KW(lex))
        {
            case and:
            case land:
            case star:
                return TRUE;
        }
    return FALSE;
}
static BOOLEAN constructedType(LEXEME *lex)
{
    BOOLEAN rv;
    LEXEME *orig = lex;
    while (lex && (ISID(lex) || MATCHKW(lex, classsel)))
        lex = getsym();
    rv = MATCHKW(lex, openpa) || MATCHKW(lex, begin);
    lex = prevsym(orig);
    return rv;
}
LEXEME *getBasicType(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, SYMBOL **strSym_out, BOOLEAN inTemplate, enum e_sc storage_class, 
                     enum e_lk *linkage_in, enum e_lk *linkage2_in, enum e_lk *linkage3_in, 
                     enum e_ac access, BOOLEAN *notype, BOOLEAN *defd, int *consdest, 
                     BOOLEAN *templateArg, BOOLEAN isTypedef, BOOLEAN templateErr)
{
    enum e_bt type = bt_none;
    TYPE *tn = NULL ;
    TYPE *quals = NULL;
    TYPE **tl;
    BOOLEAN extended;
    BOOLEAN iscomplex = FALSE;
    BOOLEAN imaginary = FALSE;
    BOOLEAN flagerror = FALSE;
    BOOLEAN foundint = FALSE;
    BOOLEAN foundunsigned = FALSE;
    BOOLEAN foundsigned = FALSE;
    BOOLEAN foundtypeof = FALSE;
    BOOLEAN foundsomething = FALSE;
    BOOLEAN int64 = FALSE;
    enum e_lk linkage = lk_none;
    enum e_lk linkage2 = lk_none;
    enum e_lk linkage3 = lk_none;

    *defd = FALSE;	
    while (KWTYPE(lex, TT_BASETYPE))
    {
        if (foundtypeof)
            flagerror = TRUE;
        switch(KW(lex))
        {
            case kw_int:
                if (foundint)
                    flagerror = TRUE;
                foundint = TRUE;
                switch(type)
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
                        flagerror = TRUE;
                }
                break;
            case kw_native:
                if (type == bt_unsigned)
                    type = bt_unative;
                else if (type == bt_int || type == bt_none)
                    type = bt_inative;
                else
                    flagerror = TRUE;
                break;
            case kw_char:
                switch(type)
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
                        flagerror = TRUE;
                        break;
                    
                }
                break;
            case kw_char16_t:
                if (type != bt_none)
                    flagerror = TRUE;
                type = bt_char16_t;
                break;
            case kw_char32_t:
                if (type != bt_none)
                    flagerror = TRUE;
                type = bt_char32_t;
                break;
            case kw_short:
                switch(type)
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
                        flagerror = TRUE;
                        break;
                    
                }
                break;
            case kw_long:
                switch(type)
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
                        flagerror = TRUE;
                        break;
                    
                }
                break;
            case kw_signed:
                if (foundsigned || foundunsigned)
                    flagerror = TRUE;
                if (type == bt_none)
                    type = bt_signed;
                else if (type == bt_char) 
                    type = bt_signed_char;
                foundsigned++;
                break;
            case kw_unsigned:
                if (foundsigned || foundunsigned)
                    flagerror = TRUE;
                foundunsigned++;
                switch(type)
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
                        flagerror = TRUE;
                        break;
                }
                break;
            case kw___int64:
                switch(type)
                {
                    case bt_unsigned:
                        int64 = TRUE;
                        type = bt_unsigned_long_long;
                        break;
                    case bt_none:
                    case bt_signed:
                        int64 = TRUE;
                        type = bt_long_long;
                        break;
                    default:
                        flagerror = TRUE;
                        break;
                }
                break;
            case kw_wchar_t:
                if (type == bt_none)
                    type = bt_wchar_t;
                else
                    flagerror = TRUE;
                break;
            case kw_auto:
                if (type == bt_none)
                    type = bt_auto;
                else
                    flagerror = TRUE;
                break;
            case kw_bool:
                if (type == bt_none)
                    if (!(chosenAssembler->arch->boolmode & ABM_USESIZE)) {
                        if (storage_class == sc_auto)
                            if (!(chosenAssembler->arch->boolmode & ABM_LOCALBITS))
                            {
                                type = bt_bool;
                            }
                            else
                            {
                                type = bt_bit;
                             }
                        else if (storage_class == sc_global)
                        {
                            if (!(chosenAssembler->arch->boolmode & ABM_GLOBALBITS))
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
                    } else
                        type = bt_bool;
                else
                    flagerror = TRUE;
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
                    flagerror = TRUE;
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
                    flagerror = TRUE;
                break;
            case kw__Complex:
                switch(type)
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
                            flagerror = TRUE;
                        iscomplex = TRUE;
                        break;
                    default:
                        flagerror = TRUE;
                        break;
                }
                break;
            case kw__Imaginary:
                switch(type)
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
                            flagerror = TRUE;
                        imaginary = TRUE;
                        break;
                    default:
                        flagerror = TRUE;
                        break;
                }
                break;
            case kw___object:
                if (type != bt_none)
                   flagerror = TRUE;
                else
                    type = bt___object;
                break;
            case kw___string:
                if (type != bt_none)
                   flagerror = TRUE;
                else
                    type = bt___string;
                break;
            case kw_struct:
            case kw_class:
            case kw_union:
                inTemplateType = FALSE;
                if (foundsigned || foundunsigned || type != bt_none)
                    flagerror = TRUE;
                lex = declstruct(lex, funcsp, &tn, inTemplate, storage_class, access, defd);
                goto exit;
            case kw_enum:
                if (foundsigned || foundunsigned || type != bt_none)
                    flagerror = TRUE;
                lex = declenum(lex, funcsp, &tn, storage_class, access, FALSE, defd);
                goto exit;
            case kw_void:
                if (type != bt_none)
                    flagerror = TRUE;
                type = bt_void;
                break;
            case kw_decltype:
                {
                    BOOLEAN hasAmpersand = FALSE;
                    BOOLEAN hasAuto = FALSE;
                    EXPRESSION *exp, *exp2;
                    type = bt_void; /* won't really be used */
                    foundtypeof = TRUE;
                    if (foundsomething)
                        flagerror = TRUE;
                    lex = getsym();
                    needkw(&lex, openpa);
                    extended = MATCHKW(lex, openpa);
                    hasAmpersand = MATCHKW(lex, and);
                    if (extended || hasAmpersand)
                    {
                        lex = getsym();
                        hasAuto = MATCHKW(lex, kw_auto);
                        lex = backupsym();
                    }
                    else
                    {
                        hasAuto = MATCHKW(lex, kw_auto);
                    }
                    if (hasAuto)
                    {
                        if (extended || hasAmpersand)
                            lex = getsym();
                        lex = getsym();
                        if (MATCHKW(lex, and) || MATCHKW(lex, land))
                        {
                            lex = getsym();
                            error(ERR_DECLTYPE_AUTO_NO_REFERENCE);
                        }
                        if (extended)
                            needkw(&lex, closepa);
                        tn = (TYPE *)Alloc(sizeof(TYPE));
                        tn->type = bt_auto;
                        tn->decltypeauto = TRUE;
                        tn->decltypeautoextended = extended;
                    }
                    else if (startOfType(lex, FALSE) & !constructedType(lex))
                    {
                        lex = get_type_id(lex, &tn, funcsp, sc_cast, FALSE, FALSE);
                    }
                    else
                    {
                        lex = expression_no_check(lex, NULL, NULL, &tn, &exp, _F_SIZEOF);
                        if (tn && tn->type == bt_aggregate && exp->type == en_func)
                        {
                            HASHREC *hr = tn->syms->table[0];
                            if (hr->next)
                                errorsym2(ERR_AMBIGUITY_BETWEEN, hr->p, hr->next->p);
                            exp->v.func->sp = (SYMBOL *)hr->p;
                            if (hasAmpersand)
                            {
                                tn = (TYPE *)Alloc(sizeof(TYPE));
                                tn->type = bt_pointer;
                                tn->size = getSize(bt_pointer);
                                tn->btp = exp->v.func->functp = exp->v.func->sp->tp;
                                tn->rootType = tn;
                            }
                            else
                            {
                                tn = exp->v.func->functp = exp->v.func->sp->tp;
                            }
                        }
                        if (tn)
                        {
                            optimize_for_constants(&exp);
                            if (templateNestingCount && !instantiatingTemplate)
                            {
                                TYPE *tp2 = Alloc(sizeof(TYPE));
                                tp2->type = bt_templatedecltype;
                                tp2->rootType = tp2;
                                tp2->templateDeclType = exp;
                                tn = tp2;
                            }
                        }
                        exp2 = exp;
                        if (!tn)
                        {
                            error(ERR_IDENTIFIER_EXPECTED);
                            errskim(&lex, skim_semi_declare);
                            break;
                        }
                        if (extended && lvalue(exp) && exp->left->type == en_auto)
                        {
                            if (!lambdas && xvalue(exp))
                            {
                                TYPE *tp2 = Alloc(sizeof(TYPE));
                                if (isref(tn))
                                    tn = basetype(tn)->btp;
                                tp2->type = bt_rref;
                                tp2->size = getSize(bt_pointer);
                                tp2->btp = tn;
                                tp2->rootType = tp2;
                                tn = tp2;
                            }
                            else if (lvalue(exp))
                            {
                                TYPE *tp2 = Alloc(sizeof(TYPE));
                                if (isref(tn))
                                    tn = basetype(tn)->btp;
                                if (lambdas && !lambdas->isMutable)
                                {
                                    tp2->type = bt_const;
                                    tp2->size = tn->size;
                                    tp2->btp = tn;
                                    tp2->rootType = tn->rootType;
                                    tn = tp2;
                                    tp2 = Alloc(sizeof(TYPE));
                                }
                                tp2->type = bt_lref;
                                tp2->size = getSize(bt_pointer);
                                tp2->btp = tn;
                                tp2->rootType = tn->rootType;
                                tn = tp2;
                            }
                        }
                    }
                }
                if (!MATCHKW(lex, closepa))
                    needkw(&lex, closepa);
                break;
            case kw_typeof:
                type = bt_void; /* won't really be used */
                foundtypeof = TRUE;
                if (foundsomething)
                    flagerror = TRUE;
                lex = getsym();
                if (MATCHKW(lex, openpa))
                {
                    EXPRESSION *exp;
                    lex = getsym();
                    lex = expression_no_check(lex, NULL, NULL, &tn, &exp, 0);
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
                    SYMBOL *sp;
                    lex = nestedSearch(lex, &sp, NULL, NULL, NULL, NULL, FALSE, storage_class, TRUE, TRUE);
                    if (sp)
                        *tp = sp->tp;
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
                    tn = NULL;
                    lex = get_type_id(lex, &tn, funcsp, sc_cast, FALSE, TRUE);
                    if (tn)
                    {
                        TYPE *tq = Alloc(sizeof(TYPE)), *tz;
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
        }
        foundsomething = TRUE;
        lex = getsym();
        lex = getQualifiers(lex, &quals, &linkage, &linkage2, &linkage3);	
        if (linkage != lk_none)
        {
            *linkage_in = linkage;
        }
        if (linkage2 != lk_none)
            *linkage2_in = linkage2;	
        if (linkage3 != lk_none)
            *linkage3_in = linkage3;	
    }
    if (type == bt_signed) // bt_signed is just an internal placeholder
        type = bt_int;
    if (!foundsomething)
    {
        BOOLEAN typeName = FALSE;
        type = bt_int;
        if (MATCHKW(lex, kw_typename))
        {
            typeName = TRUE;
//            foundsomething = TRUE;
            lex = getsym();
        }
        if (iscomplex || imaginary)
            error(ERR_MISSING_TYPE_SPECIFIER);			
        else if (ISID(lex) || MATCHKW(lex, classsel) || MATCHKW(lex, compl))
        {
            NAMESPACEVALUES *nsv = NULL;
            SYMBOL *strSym = NULL;
            SYMBOL *sp = NULL;
            BOOLEAN destructor = FALSE;
            LEXEME *placeholder = lex;
            BOOLEAN inTemplate = FALSE;
            lex = nestedSearch(lex, &sp, &strSym, &nsv, &destructor, &inTemplate, FALSE, storage_class, FALSE, TRUE);
            if (sp && (istype(sp) || (sp->storage_class == sc_type && inTemplate) || sp->storage_class == sc_typedef && sp->templateLevel) )
            {
                if (sp->deprecationText)
                    deprecateMessage(sp);
                lex = getsym();
                if (sp->storage_class == sc_typedef && sp->templateLevel)
                {
                    if (MATCHKW(lex, lt))
                    {
                        // throwaway
                        TEMPLATEPARAMLIST *lst = NULL;
                        SYMBOL *sp1;
                        lex = GetTemplateArguments(lex, funcsp, NULL, &lst);
                        sp1 = GetTypedefSpecialization(sp, lst);
                        if (sp1 && sp1->instantiated)
                        {
                            sp = sp1;
                            if (isstructured(sp->tp))
                                sp->tp = PerformDeferredInitialization(sp->tp, funcsp);
                            else
                                sp->tp = SynthesizeType(sp->tp, NULL, FALSE);
                        }
                        tn = sp->tp;
                        foundsomething = TRUE;
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
                    SYMBOL *ssp = getStructureDeclaration();
                    TYPE *tpx = basetype(sp->tp);
                    foundsomething = TRUE;
                    if (tpx->type == bt_templateparam)
                    {
                        tn = NULL;
                        if (templateArg)
                            *templateArg = TRUE;
                        if (!tpx->templateParam->p->packed)
                        {
                            if (tpx->templateParam->p->type == kw_typename)
                            {
                                tn = tpx->templateParam->p->byClass.val;
                                if (*tp && tn)
                                {
                                    // should only be const vol specifiers
                                    TYPE *tpy = *tp;
                                    while (tpy->btp)
                                        tpy = tpy->btp;
                                    tpy->btp = tpx;
                                    tn = SynthesizeType(*tp, tpx->templateParam, FALSE);
                                    *tp = NULL;
                                }
                                if (inTemplate)
                                {
                                    if (MATCHKW(lex, lt))
                                    {
                                        // throwaway
                                        TEMPLATEPARAMLIST *lst = NULL;
                                        lex = GetTemplateArguments(lex, funcsp, NULL, &lst);
                                    }
                                    errorsym(ERR_NOT_A_TEMPLATE, sp);
                                }
                            }
                            else if (tpx->templateParam->p->type == kw_template)
                            {
                                if (MATCHKW(lex, lt))
                                {

                                    TEMPLATEPARAMLIST *lst = NULL;
                                    SYMBOL *sp1 = tpx->templateParam->p->byTemplate.val;
                                    lex = GetTemplateArguments(lex, funcsp, sp1, &lst);
                                    if (sp1)
                                    {
                                        sp1 = GetClassTemplate(sp1, lst, !templateErr);
                                        tn = NULL;
                                        if (sp1)
                                            tn = sp1->tp;
                                    }
                                    else if (templateNestingCount)
                                    {
                                        TEMPLATEPARAMLIST *told, **tnew;
                                        sp1 = clonesym(sp);
                                        sp1->tp = (TYPE *)Alloc(sizeof(TYPE));
                                        *sp1->tp = *sp->tp;
                                        sp1->tp->rootType = sp1->tp;
                                        sp1->tp->sp = sp1;
                                        sp1->tp->templateParam = (TEMPLATEPARAMLIST *)Alloc(sizeof(TEMPLATEPARAMLIST));
                                        sp1->tp->templateParam->next = sp->tp->templateParam->next;
                                        sp1->tp->templateParam->p = (TEMPLATEPARAM *)Alloc(sizeof(TEMPLATEPARAM));
                                        *sp1->tp->templateParam->p = *sp->tp->templateParam->p;
                                        tnew = &sp1->tp->templateParam->p->byTemplate.args;
                                        told = *tnew;
                                        sp1->tp->templateParam->p->byTemplate.orig = sp->tp->templateParam;
                                        while (told && lst)
                                        {
                                            *tnew = (TEMPLATEPARAMLIST *)Alloc(sizeof(TEMPLATEPARAMLIST));
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
                                                SYMBOL *sp = classsearch(lst->argsym->name, FALSE, FALSE);
                                                if (sp && sp->tp->type == bt_templateparam)
                                                {
                                                    (*tnew)->p = sp->tp->templateParam->p;
                                                }
                                                else
                                                {
                                                    (*tnew)->p = lst->p;
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
                            TEMPLATEPARAMLIST *packed = tpx->templateParam->p->byPack.pack;
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
                            *templateArg = TRUE;
                        if (!templateNestingCount)
                            TemplateLookupTypeFromDeclType(tpx);
                    }
                    else if (tpx->type == bt_templateselector)
                    {
                        if (templateArg)
                            *templateArg = TRUE;
                        if (!templateNestingCount)
                        {
                            tn = SynthesizeType(sp->tp, NULL, FALSE);
                        }
                        else
                        {
                            tn = sp->tp;
                        }
                        if (tpx->sp->templateSelector->next->next && tpx->sp->templateSelector->next->sym && !strcmp(tpx->sp->templateSelector->next->sym->name, tpx->sp->templateSelector->next->next->name))
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
                                *strSym_out = tpx->sp->templateSelector->next->sym;
                            tn = tpx->sp->templateSelector->next->sym->tp;
                            *notype = TRUE;
                            goto exit;
                        }
                        else
                        {
                            // discard template params, they've already been gathered..
                            TEMPLATESELECTOR *l = tpx->sp->templateSelector->next->next;
                            if (l)
                            {
                                while (l->next) l = l->next;

                                if (l->isTemplate)
                                {
                                    if (MATCHKW(lex, lt))
                                    {
                                        TEMPLATEPARAMLIST *current = NULL;
                                        lex = GetTemplateArguments(lex, NULL, NULL, &current);
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        lex = getQualifiers(lex, &quals, &linkage, &linkage2, &linkage3);
                        if (linkage != lk_none)
                        {
                            *linkage_in = linkage;
                        }
                        if (linkage2 != lk_none)
                            *linkage2_in = linkage2;
                        if (sp->templateLevel)
                        {
                            TEMPLATEPARAMLIST *lst = NULL;
                            if (MATCHKW(lex, lt))
                            {
                                if (sp->parentTemplate)
                                    sp = sp->parentTemplate;
                                lex = GetTemplateArguments(lex, funcsp, sp, &lst);
                                sp = GetClassTemplate(sp, lst, !templateErr);
                                if (sp)
                                    sp->tp = PerformDeferredInitialization(sp->tp, funcsp);
                            }
                            else
                            {
                                if (!noSpecializationError && !instantiatingTemplate && (!sp->mainsym || (sp->mainsym != strSym && sp->mainsym != ssp)))
                                    if (instantiatingMemberFuncClass && instantiatingMemberFuncClass->parentClass == sp->parentClass)
                                        sp = instantiatingMemberFuncClass;
                                    else
                                        errorsym(ERR_NEED_SPECIALIZATION_PARAMETERS, sp);
                            }
                            tn = NULL;
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
                                    TEMPLATEPARAMLIST *lst = NULL;
                                    lex = GetTemplateArguments(lex, funcsp, NULL, &lst);
                                }
                                errorsym(ERR_NOT_A_TEMPLATE, sp);
                            }
                            tn = sp->tp;
                            //                        if (sp->parentClass && !isAccessible(sp->parentClass, ssp ? ssp : sp->parentClass, sp, funcsp, ssp == sp->parentClass ? ac_protected : ac_public, FALSE))
                            //                           errorsym(ERR_CANNOT_ACCESS, sp);
                        }
                        if (cparams.prm_cplusplus && sp && MATCHKW(lex, openpa) &&
                            (strSym && ((strSym->mainsym && strSym->mainsym == sp->mainsym) || strSym == sp->mainsym || sameTemplate(strSym->tp, sp->tp)) ||
                            (!strSym && (storage_class == sc_member || storage_class == sc_mutable) && ssp && ssp == sp->mainsym)))
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
                                *strSym_out = strSym;
                            *notype = TRUE;
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
                if (!templateNestingCount && allTemplateArgsSpecified(strSym, strSym->templateParams))
                    tn = SynthesizeType(strSym->tp, NULL, FALSE);
                else
                    tn = NULL;
                if (!tn || tn->type == bt_any || basetype(tn)->type == bt_templateparam)
                    tn = strSym->tp;
                else
                    tn = PerformDeferredInitialization(tn, funcsp);
                foundsomething = TRUE;
                lex = getsym();
            }
            else if (strSym && basetype(strSym->tp)->type == bt_templatedecltype)
            {
                tn = strSym->tp;
                foundsomething = TRUE;
                lex = getsym();
            }
            else if (strSym && strSym->templateLevel && !templateNestingCount)
            {
                STRUCTSYM s;
                tn = PerformDeferredInitialization (strSym->tp, funcsp);
                s.str = tn->sp;
                addStructureDeclaration(&s);
                sp = classsearch(lex->value.s.a, FALSE, TRUE);
                if (sp)
                {
                    tn = sp->tp;
                    foundsomething = TRUE;
                }
                dropStructureDeclaration();
                lex = getsym();
            }
            else if (isTypedef)
            {
                if (!templateNestingCount)
                {
                    error(ERR_TYPE_NAME_EXPECTED);
                }
                tn = Alloc(sizeof(TYPE));
                tn->type = bt_any;
                tn->rootType = tn;
                tn->size = getSize(bt_int);
                lex = getsym();
                foundsomething = TRUE;
                
            }
            else if (cparams.prm_cplusplus)
            {
                if (typeName)
                {
                    tn = Alloc(sizeof(TYPE));
                    tn->type = bt_any;
                    tn->rootType = tn;
                    tn->size = getSize(bt_int);
                    lex = getsym();
                    foundsomething = TRUE;
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
        if (!foundsomething && (cparams.prm_c99 || cparams.prm_cplusplus))
        {
            if (notype)
                *notype = TRUE;
            else
                error(ERR_MISSING_TYPE_SPECIFIER);
        }
          
    }
exit:
    if (chosenAssembler->msil && tn && chosenAssembler->msil->find_unboxed_type)
    {
        // select an unboxed variable type for use in compiler
        // will be converted back to boxed as needed
        TYPE *tnn = chosenAssembler->msil->find_unboxed_type(tn);
        if (tnn)
        {
            tn = tnn;
            type = tn->type;
        }
    }
    if (!cparams.prm_c99)
        switch(type)
        {
            case bt_bool:
                if (!cparams.prm_cplusplus)
                    errorstr(ERR_TYPE_C99,"_Bool");
                break;
            case bt_long_long:
            case bt_unsigned_long_long:
                if (!int64 && !cparams.prm_cplusplus)
                    errorstr(ERR_TYPE_C99,"long long");
                break;
            case bt_float_complex:
            case bt_double_complex:
            case bt_long_double_complex:
                errorstr(ERR_TYPE_C99,"_Complex");
                break;
            case bt_long_double_imaginary:
            case bt_float_imaginary:
            case bt_double_imaginary:
                errorstr(ERR_TYPE_C99,"_Imaginary");
                break;
            default:
                break;
        }
    if (flagerror)
        error(ERR_TOO_MANY_TYPE_SPECIFIERS);
    if (!tn)
    {
        tn = Alloc(sizeof(TYPE));
        tn->type = type;
        tn->rootType = tn;
        tn->size = getSize(type);
    }
    if (quals)
    {
        if (isatomic(quals) && needsAtomicLockFromType(tn))
        {
            int n = getAlign(sc_global, &stdchar32tptr);
            n = n - tn->size %n;
            if (n != 4)
            {
                tn->size += n;
            }
        }
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
    sizeQualifiers(*tp);
    return lex;
}
static LEXEME *getArrayType(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, enum e_sc storage_class, 
                            BOOLEAN *vla, TYPE **quals, BOOLEAN first, BOOLEAN msil)
{
    EXPRESSION *constant = NULL;
    TYPE *tpc = NULL;
    TYPE *typein = *tp;
    BOOLEAN unsized = FALSE;
    BOOLEAN empty = FALSE;
    lex = getsym(); /* past '[' */
    *tp = PerformDeferredInitialization(*tp, funcsp);
    if (MATCHKW(lex, star))
    {
        if (!cparams.prm_c99)
            error(ERR_VLA_c99);
        if (storage_class != sc_parameter)
            error(ERR_UNSIZED_VLA_PARAMETER);
        lex = getsym();
        unsized = TRUE;
    }
    else if (!MATCHKW(lex, closebr))
    {
        
        lex = getPointerQualifiers(lex, quals, TRUE);
        lex = optimized_expression(lex, funcsp, NULL, &tpc, &constant, FALSE);
        lex = getPointerQualifiers(lex, quals, TRUE);
        if (!tpc)
        {
            error(ERR_EXPRESSION_SYNTAX);
        }
        if (*quals && !cparams.prm_c99)
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
        empty = TRUE;
    }
    if (MATCHKW(lex, closebr))
    {
        TYPE *tpp;
        lex = getsym();
        ParseAttributeSpecifiers(&lex, funcsp, TRUE);
        if (MATCHKW(lex, openbr))
        {
            if (*quals)
                error(ERR_QUAL_LAST_ARRAY_ELEMENT);
            lex = getArrayType(lex, funcsp, tp, storage_class, vla, quals, FALSE, msil);
        }
        tpp = (TYPE *)Alloc(sizeof(TYPE));
        tpp->type = bt_pointer;
        tpp->btp = *tp;
        tpp->btp->msil = msil; // tag the base type as managed, e.g. so we can't take address of it
        tpp->rootType = tpp;
        tpp->array = TRUE;
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
                else if (tpc->type != bt_templateparam && isintconst(constant) && constant->v.i <= 0)
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
                    if (!cparams.prm_c99 && !templateNestingCount && !msil)
                        error(ERR_VLA_c99);
                    tpp->esize = constant;
                    tpp->etype = tpc;
                    *vla =  !msil && !templateNestingCount;
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
        if (isstructured(typein))
        {
            checkIncompleteArray(typein, lex->file, lex->line);
        }
    }
    else
    {
        error(ERR_ARRAY_NEED_CLOSEBRACKET);
        errskim(&lex, skim_comma);
    }
    return lex;
}
static void resolveVLAs(TYPE *tp)
{
    while (tp && tp->type == bt_pointer && tp->array)
    {
        tp->vla = TRUE;
        tp = tp->btp;
    }
}
BOOLEAN intcmp(TYPE *t1, TYPE *t2)
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
static void matchFunctionDeclaration(LEXEME *lex, SYMBOL *sp, SYMBOL *spo, BOOLEAN checkReturn)
{
    HASHREC *hro;

    /* two oldstyle declarations aren't compared */
    if ((spo && !spo->oldstyle && spo->hasproto) || !sp->oldstyle)
    {
        if (spo && isfunction(spo->tp))
        {
            HASHREC *hro1, *hr1;
            if (checkReturn && !spo->isConstructor && !spo->isDestructor && !comparetypes(basetype(spo->tp)->btp, basetype(sp->tp)->btp, TRUE) && !sameTemplatePointedTo(basetype(spo->tp)->btp, basetype(sp->tp)->btp))
            {                
                preverrorsym(ERR_TYPE_MISMATCH_FUNC_DECLARATION, spo, spo->declfile, spo->declline);
            }
            else
            {
                hro1 = basetype(spo->tp)->syms->table[0];
                hr1 = basetype(sp->tp)->syms->table[0];
                if (hro1 && ((SYMBOL *)(hro1->p))->thisPtr)
                    hro1 = hro1->next;
                if (hro1 && hr1 && ((SYMBOL *)(hro1->p))->tp)
                {
                    while (hro1 && hr1)
                    {
                        SYMBOL *spo1 = (SYMBOL *)hro1->p;
                        SYMBOL *sp1 = (SYMBOL *)hr1->p;                    
                        if (!comparetypes(spo1->tp, sp1->tp, TRUE) && !sameTemplatePointedTo(spo1->tp, sp1->tp))
                        {
                            break;
                        }
                        hro1 = hro1->next;
                        hr1 = hr1->next;
                    }
                    if ((hro1 || hr1) && spo != sp)
                    {
                        preverrorsym(ERR_TYPE_MISMATCH_FUNC_DECLARATION, spo, spo->declfile, spo->declline);
                    }
                    else
                    {
                        BOOLEAN err = FALSE;
                        SYMBOL *last = NULL;
                        hro1 = basetype(spo->tp)->syms->table[0];
                        hr1 = basetype(sp->tp)->syms->table[0];
                        if (hro1 && ((SYMBOL *)(hro1->p))->thisPtr)
                            hro1 = hro1->next;
                        while (hro1 && hr1)
                        {
                            SYMBOL *so = (SYMBOL *)hro1->p;
                            SYMBOL *s = (SYMBOL *)hr1->p;
                            if (so != s && so->init && s->init)
                                errorsym(ERR_CANNOT_REDECLARE_DEFAULT_ARGUMENT, so);
                            if (!err && last && last->init && !(so->init || s->init))
                            {
                                err = TRUE;
                                errorsym(ERR_MISSING_DEFAULT_ARGUMENT, last);
                            }
                            last = so;
                            if (so->init)
                                s->init = so->init;
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
    if ((spo->xc && spo->xc->xcDynamic) || (sp->xc && sp->xc->xcDynamic))
    {
        if (!sp->xc || !sp->xc->xcDynamic)
        { 
            if (!MATCHKW(lex, begin))
                errorsym(ERR_EXCEPTION_SPECIFIER_MUST_MATCH, sp);
        }
        else if (!spo->xc || !spo->xc->xcDynamic || spo->xcMode != sp->xcMode)
        {
                errorsym(ERR_EXCEPTION_SPECIFIER_MUST_MATCH, sp);                
        }
        else
        {
            LIST *lo = spo->xc->xcDynamic;
            while (lo)
            {
                LIST *li = sp->xc->xcDynamic;
                while (li)
                {
                    if (comparetypes((TYPE *)lo->data, (TYPE *)li->data, TRUE)  && intcmp((TYPE *)lo->data, (TYPE *)li->data))
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
    else if (spo->xcMode != sp->xcMode)
    {
        if (spo->xcMode == xc_none && sp->xcMode == xc_dynamic)
        {
            if (sp->xc->xcDynamic)
                errorsym(ERR_EXCEPTION_SPECIFIER_MUST_MATCH, sp);
        }
        else if (sp->xcMode == xc_none && spo->xcMode == xc_dynamic)
        {
            if (spo->xc->xcDynamic)
                errorsym(ERR_EXCEPTION_SPECIFIER_MUST_MATCH, sp);
        }
        else if (sp->xcMode == xc_unspecified)
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
LEXEME *getDeferredData(LEXEME *lex, SYMBOL *sym, BOOLEAN braces)
{
    LEXEME **cur = &sym->deferredCompile, *last = NULL;
    int paren = 0;
    int brack = 0;
    int ltgt = 0;
    while (lex != NULL)
    {
        enum e_kw kw= KW(lex);
        if (braces)
        {
            if (kw == begin)
            {
                paren++;
            }
            else if (kw == end && !--paren)
            {
                *cur = Alloc(sizeof(LEXEME));
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
        *cur = Alloc(sizeof(LEXEME));
        if (lex->type == l_id)
            lex->value.s.a = litlate(lex->value.s.a);
        **cur = *lex;
        (*cur)->prev = last;
        if (linesHead)
        {
            (*cur)->linedata = linesHead;
            linesHead = linesTail = NULL;
        }
        last = *cur;
        cur = &(*cur)->next;
        lex = getsym();
    }
    return lex;
}
LEXEME *getFunctionParams(LEXEME *lex, SYMBOL *funcsp, SYMBOL **spin, TYPE **tp, BOOLEAN inTemplate, enum e_sc storage_class)
{
    SYMBOL *sp = *spin;
    SYMBOL *spi;
    HASHREC *hri, **hrp;
    TYPE *tp1;
    BOOLEAN isvoid = FALSE;
    BOOLEAN pastfirst = FALSE;
    BOOLEAN voiderror = FALSE;
    BOOLEAN hasellipse = FALSE;
    HASHTABLE *locals = localNameSpace->syms;
    LEXEME *placeholder = lex;
    STRUCTSYM s;
    s.tmpl = NULL;
    lex = getsym();
    IncGlobalFlag();
    if (*tp == NULL)
        *tp = &stdint;
    tp1 = Alloc(sizeof(TYPE));
    tp1->type = bt_func;
    tp1->size = getSize(bt_pointer);
    tp1->btp = *tp;
    tp1->rootType = tp1;
    tp1->sp = sp;
    sp->tp = *tp = tp1;
    localNameSpace->syms = tp1->syms = CreateHashTable(1);
    ParseAttributeSpecifiers(&lex, funcsp, TRUE);
    if (inTemplate && templateNestingCount == 1)
        noSpecializationError++;
    if (startOfType(lex, TRUE))
    {
        sp->hasproto = TRUE;
        while (startOfType(lex, TRUE) || MATCHKW(lex, ellipse))
        {
            BOOLEAN templType = inTemplateType;
            inTemplateType = !!templateNestingCount;
            if (MATCHKW(lex, ellipse))
            {
                if (!pastfirst)
                    break;;
                if (hasellipse)
                    break;
                if (isvoid)
                    voiderror = TRUE;
                spi = makeID(sc_parameter, tp1, NULL, NewUnnamedID());
                spi->anonymous = TRUE;
                SetLinkerNames(spi, lk_none);
                spi->tp = Alloc(sizeof(TYPE));
                spi->tp->type = bt_ellipse;
                spi->tp->rootType = spi->tp;
                insert(spi, (*tp)->syms);
                lex = getsym();
                hasellipse = TRUE;
                    
            }
            else
            {
                enum e_sc storage_class = sc_parameter;
                BOOLEAN blocked;
                BOOLEAN constexpression;
                ADDRESS address;
                TYPE *tpb, *tpx;
                enum e_lk linkage = lk_none;
                enum e_lk linkage2 = lk_none;
                enum e_lk linkage3 = lk_none;
                BOOLEAN defd = FALSE;
                BOOLEAN notype = FALSE;
                BOOLEAN clonedParams = FALSE;
                TYPE *tp2;
                spi = NULL;
                tp1 = NULL;
                lex = getStorageAndType(lex, funcsp, NULL, FALSE, TRUE, &storage_class, &storage_class,
                       &address, &blocked, NULL, & constexpression, &tp1, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd, NULL, NULL);                
                if (!basetype(tp1))
                    error(ERR_TYPE_NAME_EXPECTED);
                else if (isautotype(tp1) && !lambdas)
                    error(ERR_AUTO_NOT_ALLOWED_IN_PARAMETER);
                else if (cparams.prm_cplusplus && isstructured((*tp)->btp) && MATCHKW(lex, openpa))
                {
                    LEXEME *cur = lex;
                    lex = getsym();
                    if (!MATCHKW(lex, star) && !MATCHKW(lex, and) && !startOfType(lex, TRUE))
                    {
                        if (*spin)
                        {
                            (*spin)->tp = (*tp) = (*tp)->btp;
                            // constructor initialization
                            // will do initialization later...
                        }
                        localNameSpace->syms = locals;
                        if (inTemplate && templateNestingCount == 1)
                            noSpecializationError--;
                        lex = prevsym(placeholder);
                        return lex;
                    }
                    lex = prevsym(cur);
                }
                lex = getBeforeType(lex, funcsp, &tp1, &spi, NULL, NULL, FALSE, storage_class, &linkage, &linkage2, &linkage3, FALSE, FALSE, FALSE, FALSE);
				if (!templateNestingCount && !structLevel)
                {
		            tp1 = PerformDeferredInitialization(tp1, funcsp);
                }
                if (!spi)
                {
                    spi = makeID(sc_parameter, tp1, NULL, NewUnnamedID());
                    spi->anonymous = TRUE;
                    SetLinkerNames(spi, lk_none);
                }
                tp2 = tp1;
                while (ispointer(tp2) || isref(tp2))
                    tp2 = basetype(tp2)->btp;
                tp2 = basetype(tp2);
                if (tp2->type == bt_templateparam && tp2->templateParam->p->packed)
                {
                    spi->packed = TRUE;
                    if (spi->anonymous && MATCHKW(lex, ellipse))
                        lex = getsym();
                }
                spi->tp = tp1;
                spi->linkage = linkage;
                spi->linkage2 = linkage2;
                if (spi->packed)
                {
                    checkPackedType(spi);
                    if (!templateNestingCount)
                    {
                        if (tp2->templateParam && tp2->templateParam->p->packed)
                        {
                            TYPE *newtp1 = Alloc(sizeof(TYPE));
                            TEMPLATEPARAMLIST *templateParams = tp2->templateParam->p->byPack.pack;
                            TEMPLATEPARAMLIST *tplnew = Alloc(sizeof(TEMPLATEPARAMLIST));
                            TEMPLATEPARAM *tpnew = Alloc(sizeof(TEMPLATEPARAM));
                            TEMPLATEPARAMLIST **newPack;
                            BOOLEAN first = TRUE;
                            *tplnew = *tp2->templateParam;
                            *tpnew = *tp2->templateParam->p;
                            tplnew->p = tpnew;
                            *newtp1 = *tp2;
                            newtp1->templateParam = tplnew;
                            tp1 = newtp1;
                            newPack = &tp1->templateParam->p->byPack.pack;
                            tp1->templateParam->p->index = 0;
                            UpdateRootTypes(tplnew);
                            if (templateParams)
                            {
                                while (templateParams)
                                {
                                    SYMBOL *clone = clonesym(spi);
                                    clone->tp = Alloc(sizeof(TYPE));
                                    *clone->tp = *templateParams->p->byClass.val;
                                    UpdateRootTypes(clone->tp);
                                    SetLinkerNames(clone, lk_none);
                                    sizeQualifiers(clone->tp);
                                    if (!first)
                                    {
                                        clone->name = clone->decoratedName = clone->errname = AnonymousName();
                                        clone->packed = FALSE;
                                    }
                                    else
                                    {
                                        clone->tp->templateParam = tp1->templateParam;
                                    }
                                    *newPack = Alloc(sizeof(TEMPLATEPARAMLIST));
                                    **newPack = *templateParams;
                                    (*newPack)->p = Alloc(sizeof(TEMPLATEPARAM));
                                    *(*newPack)->p = *templateParams->p;
                                    (*newPack)->p->packsym = clone;
                                    newPack = &(*newPack)->next;
                                    insert(clone, (*tp)->syms);
                                    first = FALSE;
                                    templateParams = templateParams->next;
                                    tp1->templateParam->p->index++;
                                    UpdateRootTypes(clone->tp);
                                }
                            }
                            else
                            {
                                SYMBOL *clone = clonesym(spi);
                                TYPE *tp2 = Alloc(sizeof(TYPE));
                                *tp2 = *clone->tp;
                                clone->tp = tp2;
                                clone->tp->templateParam = tp1->templateParam;
                                SetLinkerNames(clone, lk_none);
                                UpdateRootTypes(clone->tp);
                                sizeQualifiers(clone->tp);
                                insert(clone, (*tp)->syms);
                                UpdateRootTypes(clone->tp);
                            }
                            clonedParams = TRUE;
                        }
                    }
                }
                if (!clonedParams)
                {
                    if (tp1 && isfunction(tp1))
                    {
                        TYPE *tp2 = Alloc(sizeof(TYPE));
                        tp2->type = bt_pointer;
                        tp2->size = getSize(bt_pointer);
                        tp2->btp = tp1;
                        tp2->rootType = tp2;
                        tp1 = tp2;
                    }
                    if (cparams.prm_cplusplus && MATCHKW(lex, assign))
                    {
                        if (storage_class == sc_member || storage_class == sc_mutable || structLevel || (templateNestingCount == 1 && !instantiatingTemplate))
                        {
                            lex = getDeferredData(lex, spi, FALSE);
                        }    
                        else
                        {
                            TYPE *tp2 = spi->tp;
                            inDefaultParam++;
                            if (isref(tp2))
                                tp2 = basetype(tp2)->btp;
                            if (isstructured(tp2))
                            {
                                SYMBOL *sym;
                                anonymousNotAlloc++;
                                sym = anonymousVar(sc_auto, tp2)->v.sp;
                                anonymousNotAlloc--;
                                sym->stackblock = !isref(spi->tp);
                                lex = initialize(lex, funcsp, sym, sc_auto, TRUE, 0); /* also reserves space */
                                spi->init = sym->init;
                                if (spi->init->exp->type == en_thisref)
                                {
                                    EXPRESSION **expr = &spi->init->exp->left->v.func->thisptr;
                                    if ((*expr)->type == en_add && isconstzero(&stdint, (*expr)->right))
                                        spi->init->exp->v.t.thisptr = (*expr) = (*expr)->left;
                                }
                            }
                            else
                            {
                                lex = initialize(lex, funcsp, spi, sc_auto, TRUE, 0); /* also reserves space */
                            }
                            if (spi->init)
                            {
                                checkDefaultArguments(spi);
                            }
                            inDefaultParam--;
                        }
                        if (isfuncptr(spi->tp) && spi->init && lvalue(spi->init->exp))
                            error(ERR_NO_POINTER_TO_FUNCTION_DEFAULT_ARGUMENT);
                        if (sp->storage_class == sc_typedef)
                            error(ERR_NO_DEFAULT_ARGUMENT_IN_TYPEDEF);
                    }
                    SetLinkerNames(spi, lk_none);
                    spi->tp = tp1;
                    insert(spi, (*tp)->syms);
                    tpb = basetype(tp1);
                    if (tpb->array)
                    {
                        if (cparams.prm_cplusplus && isstructured(tpb->btp) && !basetype(tpb->btp)->sp->trivialCons)
                            error(ERR_CANNOT_USE_ARRAY_OF_STRUCTURES_AS_FUNC_ARG);
                        if (tpb->vla)
                        {
                            TYPE *tpx = Alloc(sizeof(TYPE));
                            TYPE *tpn = tpb;
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
                        if (pastfirst || !spi->anonymous)
                            voiderror = TRUE;
                        else
                            isvoid = TRUE;
                    else if (isvoid)
                        voiderror = TRUE;
                }
            }
            inTemplateType = templType;
            if (!MATCHKW(lex, comma) && (!cparams.prm_cplusplus || !MATCHKW(lex, ellipse)))
                break;
            if (MATCHKW(lex, comma))
                lex = getsym();
            pastfirst = TRUE;
            ParseAttributeSpecifiers(&lex, funcsp, TRUE);
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
            SYMBOL *sym = (SYMBOL *)(*hrp)->p;
            if (sym->storage_class != sc_parameter)         
                *hrp = (*hrp)->next;
            else
                hrp = &(*hrp)->next;
        }
    }
    else if (!cparams.prm_cplusplus && !chosenAssembler->msil && ISID(lex))
    {
        SYMBOL *spo;
        sp->oldstyle = TRUE;
        if (sp->storage_class != sc_member &&  sp->storage_class != sc_mutable)
        {
            spo  = gsearch(sp->name);
            /* temporary for C */
            if (spo && spo->storage_class == sc_overloads)
                spo = (SYMBOL *)spo->tp->syms->table[0]->p;
            if (spo && isfunction(spo->tp) && spo->hasproto)
            {
                if (!comparetypes(spo->tp->btp, sp->tp->btp, TRUE))
                {
                    preverrorsym(ERR_TYPE_MISMATCH_FUNC_DECLARATION, spo, spo->declfile, spo->declline);
                }
            }
        }
        while (ISID(lex) || MATCHKW(lex, ellipse))
        {
            if (MATCHKW(lex, ellipse))
            {
                if (!pastfirst)
                    break;;
                if (hasellipse)
                    break;
                if (isvoid)
                    voiderror = TRUE;
                spi = makeID(sc_parameter, tp1, NULL, NewUnnamedID());
                spi->anonymous = TRUE;
                SetLinkerNames(spi, lk_none);
                spi->tp = Alloc(sizeof(TYPE));
                spi->tp->type = bt_ellipse;
                spi->tp->rootType = spi->tp;
                lex = getsym();
                hasellipse = TRUE;
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
        if (startOfType(lex, FALSE))
        {
            while (startOfType(lex, FALSE))
            {
                ADDRESS address;
                BOOLEAN blocked;
                BOOLEAN constexpression;
                enum e_lk linkage = lk_none;
                enum e_lk linkage2 = lk_none;
                enum e_lk linkage3 = lk_none;
                enum e_sc storage_class = sc_parameter;
                BOOLEAN defd = FALSE;
                BOOLEAN notype = FALSE;
                tp1 = NULL;
                lex = getStorageAndType(lex, funcsp, NULL, FALSE, FALSE, &storage_class, &storage_class,
                       &address, &blocked, NULL, &constexpression,&tp1, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd, NULL, NULL);

                while (1)
                {
                    TYPE *tpx = tp1;
                    spi = NULL;
                    lex = getBeforeType(lex, funcsp, &tpx, &spi, NULL, NULL, FALSE, 
                                        sc_parameter, &linkage, &linkage2, &linkage3, FALSE, FALSE, FALSE, FALSE);
                    sizeQualifiers(tpx);
                    if (!spi || spi->anonymous)
                    {
                        error(ERR_IDENTIFIER_EXPECTED);
                        errskim(&lex, skim_end);
                        break;
                    }
                    else
                    {
                        SYMBOL *spo ;
                        TYPE *tpb;
                        spi->linkage = linkage;
                        spi->linkage2 = linkage2;
                        SetLinkerNames(spi, lk_none);
                        if (tpx && isfunction(tpx))
                        {
                            TYPE *tp2 = Alloc(sizeof(TYPE));
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
                                TYPE *tpx = Alloc(sizeof(TYPE));
                                TYPE *tpn = tpb;
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
//								tpb->array = FALSE;
//								tpb->size = getSize(bt_pointer);
                            }
                        }
                        sizeQualifiers(tpx);
                        spo = search(spi->name,(*tp)->syms);
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
                spi = (SYMBOL *)hri->p;
                if (spi->tp == NULL)
                {
                    if (cparams.prm_c99)
                        errorsym(ERR_MISSING_TYPE_FOR_PARAMETER, spi);
                    spi->tp = (TYPE *)Alloc(sizeof(TYPE));
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
        spi = makeID(sc_parameter, tp1, NULL, NewUnnamedID());
        spi->anonymous = TRUE;
        SetLinkerNames(spi, lk_none);
        spi->tp = Alloc(sizeof(TYPE));
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
    else if (cparams.prm_cplusplus || chosenAssembler->msil && !MATCHKW(lex, closepa) && *spin)
    {
        // () is a function
        if (MATCHKW(lex, closepa))
        {
            spi = makeID(sc_parameter, tp1, NULL, NewUnnamedID());
            spi->anonymous = TRUE;
            SetLinkerNames(spi, lk_none);
            spi->tp = Alloc(sizeof(TYPE));
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
        SYMBOL *spo;
        if (sp->storage_class != sc_member &&  sp->storage_class != sc_mutable)
        {
            spo = gsearch(sp->name);
            if (spo && spo->storage_class == sc_overloads)
            {
                spo = (SYMBOL *)spo->tp->syms->table[0]->p;
                if (spo)
                {
                    HASHREC *hr = spo->tp->syms->table[0];
                    if (hr)
                    {
                        SYMBOL *sp2 = (SYMBOL *)hr->p;
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
    localNameSpace->syms = locals;
    DecGlobalFlag();
    if (voiderror)
        error(ERR_VOID_ONLY_PARAMETER);
    if (inTemplate && templateNestingCount == 1)
        noSpecializationError--;
    return lex;
}
LEXEME *getExceptionSpecifiers(LEXEME *lex, SYMBOL *funcsp, SYMBOL *sp, enum e_sc storage_class)
{
    switch (KW(lex))
    {
        case kw_throw:
            lex = getsym();
            if (MATCHKW(lex, openpa))
            {
                lex = getsym();
                if (MATCHKW(lex, closepa))
                {
                    sp->xcMode = xc_dynamic;
                    if (!sp->xc)
                        sp->xc = Alloc(sizeof(struct xcept));
                }
                else
                {
                    sp->xcMode = xc_dynamic;
                    if (!sp->xc)
                        sp->xc = Alloc(sizeof(struct xcept));
                    lex = backupsym();
                    do
                    {
                        TYPE *tp = NULL;
                        lex = getsym();
                        if (!MATCHKW(lex, closepa))
                        {
                            lex = get_type_id(lex, &tp, funcsp, sc_cast, FALSE, TRUE);
                            if (!tp)
                            {
                                error(ERR_TYPE_NAME_EXPECTED);
                            }
                            else
                            {
                                // this is reverse order but who cares?
                                LIST *p = Alloc(sizeof(LIST));
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
                                p->next = sp->xc->xcDynamic;
                                p->data = tp;
                                sp->xc->xcDynamic = p;
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
                HASHREC *hr = basetype(sp->tp)->syms->table[0];
                TYPE *tp = NULL;
                EXPRESSION *exp = NULL;
                AllocateLocalContext(NULL, NULL, 0);
                while (hr)
                {
                    SYMBOL *sp2 = (SYMBOL *)hr->p;
                    insert(sp2, localNameSpace->syms);
                    hr = hr->next;
                }
                lex = getsym();
                lex = optimized_expression(lex, funcsp, NULL, &tp, &exp, FALSE);
                FreeLocalContext(NULL, NULL, 0);
                if (!IsConstantExpression(exp, FALSE))
                {
                    if (!templateNestingCount)
                        error(ERR_CONSTANT_VALUE_EXPECTED);
                }
                else
                {
                    sp->xcMode = exp->v.i ? xc_none : xc_all;
                }
                needkw(&lex, closepa);
            }
            else
            {
                sp->xcMode = xc_none;
            }
            break;
    }

    return lex;
}
static LEXEME *getAfterType(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, SYMBOL **sp, BOOLEAN inTemplate, enum e_sc storage_class, int consdest)
{
    BOOLEAN isvla = FALSE;
    TYPE *quals = NULL;
    TYPE *tp1 = NULL;
    EXPRESSION *exp = NULL;
    if (ISKW(lex))
    {
        switch(KW(lex))
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
                        if (cparams.prm_cplusplus)
                        {
                            BOOLEAN foundFinal = FALSE;
                            BOOLEAN foundOverride = FALSE;
                            BOOLEAN done = FALSE;
                            BOOLEAN foundConst = FALSE;
                            BOOLEAN foundVolatile = FALSE;
                            BOOLEAN foundPure = FALSE;
                            BOOLEAN foundand = FALSE;
                            BOOLEAN foundland = FALSE;
                            while (lex != NULL && !done)
                            {
                                if (ISID(lex))
                                {
                                    if (!strcmp(lex->value.s.a, "final"))
                                    {
                                        if (foundFinal)
                                            error(ERR_FUNCTION_CAN_HAVE_ONE_FINAL_OR_OVERRIDE);
                                        foundFinal = TRUE;
                                        (*sp)->isfinal = TRUE;
                                        lex = getsym();
                                    }
                                    else if (!strcmp(lex->value.s.a, "override"))
                                    {
                                        if (foundOverride)
                                            error(ERR_FUNCTION_CAN_HAVE_ONE_FINAL_OR_OVERRIDE);
                                        foundOverride = TRUE;
                                        (*sp)->isoverride = TRUE;
                                        lex = getsym();
                                    }
                                    else
                                        done = TRUE;
                                }
                                else switch(KW(lex))
                                {
                                    case kw_const:
                                        foundConst = TRUE;
                                        lex = getsym();
                                        break;
                                    case kw_volatile:
                                        foundVolatile = TRUE;
                                        lex = getsym();
                                        break;
                                    case and:
                                        foundand = TRUE;
                                        lex = getsym();
                                        break;
                                    case land:
                                        foundland = TRUE;
                                        lex = getsym();
                                        break;
                                    default:
                                        done = TRUE;
                                        break;
                                }
                            }
                            if (foundand && foundland)
                                error(ERR_TOO_MANY_QUALIFIERS);
                            if (foundVolatile)
                            {
                                tp1 = Alloc(sizeof(TYPE));
                                tp1->size = (*tp)->size;
                                tp1->type = bt_volatile;
                                tp1->btp = *tp;
                                tp1->rootType = (*tp)->rootType;
                                *tp = tp1;   
                            }
                            if (foundConst)
                            {
                                tp1 = Alloc(sizeof(TYPE));
                                tp1->size = (*tp)->size;
                                tp1->type = bt_const;
                                tp1->btp = *tp;
                                tp1->rootType = (*tp)->rootType;
                                *tp = tp1;   
                            }
                            if (foundand)
                            {
                                tp1 = Alloc(sizeof(TYPE));
                                tp1->size = (*tp)->size;
                                tp1->type = bt_lrqual;
                                tp1->btp = *tp;
                                tp1->rootType = (*tp)->rootType;
                                *tp = tp1;   
                            }
                            else if (foundland)
                            {
                                tp1 = Alloc(sizeof(TYPE));
                                tp1->size = (*tp)->size;
                                tp1->type = bt_rrqual;
                                tp1->btp = *tp;
                                tp1->rootType = (*tp)->rootType;
                                *tp = tp1;   
                            }
                            if (cparams.prm_cplusplus && *sp)
                                lex = getExceptionSpecifiers(lex, funcsp, *sp, storage_class);

                            ParseAttributeSpecifiers(&lex, funcsp, TRUE);
                            if (MATCHKW(lex, pointsto))
                            {
                                TYPE *tpx= NULL;
                                HASHTABLE *locals = localNameSpace->syms;
                                if (inTemplate && templateNestingCount == 1)
                                    noSpecializationError++;
                                localNameSpace->syms = basetype(*tp)->syms;
                                IncGlobalFlag();
                                lex = getsym();
                                ParseAttributeSpecifiers(&lex, funcsp, TRUE);
                                lex  = get_type_id(lex, &tpx, funcsp, sc_cast, FALSE, TRUE);
                                if (tpx)
                                {
                                    if (!isautotype(basetype(*tp)->btp))
                                        error(ERR_MULTIPLE_RETURN_TYPES_SPECIFIED);
                                    if (isarray(tpx))
                                    {
                                        TYPE *tpn = NULL;
                                        tpn = Alloc(sizeof(TYPE));
                                        tpn->type = bt_pointer;
                                        tpn->size = getSize(bt_pointer);
                                        tpn->btp = basetype(tpx)->btp;
                                        tpn->rootType = tpn;
                                        if (isconst(tpx))
                                        {
                                            TYPE *tpq = Alloc(sizeof(TYPE));
                                            tpq->size = tpn->size;
                                            tpq->type = bt_const;
                                            tpq->btp = tpn;
                                            tpq->rootType = tpn->rootType;
                                            tpn = tpq;
                                        }
                                        if (isvolatile(tpx))
                                        {
                                            TYPE *tpq = Alloc(sizeof(TYPE));
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
                                localNameSpace->syms = locals;
                                DecGlobalFlag();
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
                lex = getArrayType(lex, funcsp, tp, (*sp)->storage_class, &isvla, &quals, TRUE, FALSE);
                if (isvla)
                {
                    resolveVLAs(*tp);
                }
                else if (ispointer (*tp) && (*tp)->btp->vla)
                {
                    (*tp)->vla= TRUE;
                }
                if (quals)
                {
                    TYPE *q2 = quals;
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
                else if (*sp && ((*sp)->storage_class == sc_member  || (*sp)->storage_class == sc_mutable))
                {
                    //error(ERR_BIT_STRUCT_MEMBER);
                    if (cparams.prm_ansi)
                    {
                        if ((*sp)->tp->type != bt_int 
                            && (*sp)->tp->type != bt_unsigned
                            && (*sp)->tp->type != bt_bool)
                            error(ERR_ANSI_INT_BIT);
                    }
                    else if (!isint((*sp)->tp) && basetype((*sp)->tp)->type != bt_enum)
                        error(ERR_BIT_FIELD_INTEGER_TYPE);
                    lex = getsym();
                    lex = optimized_expression(lex, funcsp, NULL, &tp1, &exp, FALSE);
                    if (tp1 && exp && isintconst(exp))
                    {
                        int n = (*tp)->size * chosenAssembler->arch->bits_per_mau;
                        TYPE **tp1 = tp, *tpb = basetype(*tp1);
                        *tp1 = Alloc(sizeof(TYPE));
                        **tp1 = *tpb;
                        (*tp1)->bits = exp->v.i;
                        (*tp1)->hasbits = TRUE;
                        (*tp1)->anonymousbits = (*sp)->anonymous;
                        UpdateRootTypes(*tp1);
                        if ((*tp1)->bits > n)
                            error(ERR_BIT_FIELD_TOO_LARGE);
                        else if ((*tp1)->bits < 0 || ((*tp1)->bits == 0 && !(*sp)->anonymous))
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
                if (cparams.prm_cplusplus && inTemplate)
                {
                    TEMPLATEPARAMLIST *templateParams = TemplateGetParams(*sp);
                    lex = GetTemplateArguments(lex, funcsp, *sp, &templateParams->p->bySpecialization.types);
                    lex = getAfterType(lex, funcsp, tp, sp, inTemplate, storage_class, consdest);
                }
                else
                {
                    TEMPLATEPARAM *templateParam = Alloc(sizeof(TEMPLATEPARAM));
                    templateParam->type = kw_new;
                    lex = GetTemplateArguments(lex, funcsp, *sp, &templateParam->bySpecialization.types);
                    lex = getAfterType(lex, funcsp, tp, sp, inTemplate, storage_class, consdest);
                    if (*tp && isfunction(*tp))
                    {
                        TEMPLATEPARAMLIST *lst = Alloc(sizeof(TEMPLATEPARAMLIST));
                        lst->p = templateParam;
                        DoInstantiateTemplateFunction(*tp, sp, NULL, NULL, lst, TRUE);
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
static  void stripfarquals(TYPE **tp)
{
#ifdef XXXXX
    BOOLEAN found = FALSE;
    BOOLEAN erred = FALSE;
    TYPE **tpl = tp;
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
LEXEME *getBeforeType(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, SYMBOL **spi, 
                      SYMBOL **strSym, NAMESPACEVALUES **nsv, BOOLEAN inTemplate, enum e_sc storage_class,
                             enum e_lk *linkage, enum e_lk *linkage2, enum e_lk *linkage3, 
                             BOOLEAN asFriend, int consdest, BOOLEAN beforeOnly, BOOLEAN funcptr)
{
    SYMBOL *sp;
    TYPE *ptype = NULL;
    enum e_bt xtype = bt_none;
    LEXEME *pos = lex;
    BOOLEAN doneAfter = FALSE;

    if (chosenAssembler->msil && MATCHKW(lex, openbr))
    {
        // managed array
        BOOLEAN isvla = FALSE;
        TYPE *quals = NULL;
        lex = getArrayType(lex, funcsp, tp, storage_class, &isvla, &quals, TRUE, TRUE);
        if (quals)
        {
            TYPE *q2 = quals;
            while (q2->btp)
                q2 = q2->btp;
            q2->btp = *tp;
            *tp = quals;
        }
        UpdateRootTypes(*tp);
        doneAfter = TRUE;
    }
    if (ISID(lex) || MATCHKW(lex, classsel) || MATCHKW(lex, kw_operator) || (cparams.prm_cplusplus && MATCHKW(lex, ellipse)))
    {
        SYMBOL *strSymX = NULL;
        NAMESPACEVALUES *nsvX = NULL;
        STRUCTSYM s;
        BOOLEAN oldTemplateSpecialization = inTemplateSpecialization;
        s.tmpl = NULL;
        inTemplateSpecialization = inTemplateType;
        inTemplateType = FALSE;
        if (cparams.prm_cplusplus)
        {
            STRUCTSYM s;
            BOOLEAN throughClass = FALSE;
            BOOLEAN pack = FALSE;
            if (MATCHKW(lex, ellipse))
            {
                pack = TRUE;
                lex = getsym();
            }
            lex = nestedPath(lex, &strSymX, &nsvX, &throughClass, FALSE, storage_class, FALSE);
            inTemplateSpecialization = oldTemplateSpecialization;
            if (strSymX)
            {
                if (strSym)
                    *strSym = strSymX;
                s.str = strSymX;
                addStructureDeclaration(&s);
            }
            if (nsv)
                *nsv = nsvX;
            if (strSymX && MATCHKW(lex, star))
            {
                BOOLEAN inparen = FALSE;
                if (pack)
                    error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                lex = getsym();
                if (funcptr && MATCHKW(lex, openpa))
                {
                    inparen = TRUE;
                    lex = getsym();
                }
                ptype = Alloc(sizeof(TYPE));
                ptype->type = bt_memberptr;
                ptype->rootType = ptype;
                ptype->sp = strSymX;
                ptype->btp = *tp;
                UpdateRootTypes(*tp);
                *tp = ptype;
                lex = getQualifiers(lex, tp, linkage, linkage2, linkage3);
                if (strSym)
                    *strSym = NULL;
                if (nsv)
                    *nsv = NULL;
                lex = getBeforeType(lex, funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2, linkage3, asFriend, FALSE, beforeOnly, FALSE);
                if (*tp  &&(ptype != *tp && isref(*tp)))
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
                TYPE *castType = NULL;
                if (MATCHKW(lex, compl))
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
                        sp->castoperator = TRUE;
                    }
                    else
                    {
                        sp = makeID(storage_class, *tp, *spi, litlate(buf));
                        sp->packed = pack;
                        lex = getsym();
                    }
                    sp->operatorId = ov;
                    if (lex)
                        sp->declcharpos = lex->charindex;
                    *spi = sp;
                }
            }
        }
        else
        {
            inTemplateSpecialization = oldTemplateSpecialization;
            sp = makeID(storage_class, *tp, *spi, litlate(lex->value.s.a));
            sp->declcharpos = lex->charindex;
            *spi = sp;
            lex = getsym();
        }
        if (inTemplate && *spi)
        {
            TEMPLATEPARAMLIST *templateParams;
            SYMBOL *ssp = strSymX;
            if (!ssp)
                ssp = getStructureDeclaration();
            (*spi)->parentClass = ssp;
            (*spi)->templateLevel = templateNestingCount;
            templateParams = TemplateGetParams(*spi);
            (*spi)->templateLevel = 0;
            (*spi)->parentClass = NULL;
            if (templateParams)
            {
                s.tmpl = templateParams;
                addTemplateDeclaration(&s);
            }
        }
        if (nsvX)
        {
            LIST *nlist;

            nlist = Alloc(sizeof(LIST));
            nlist->next = nameSpaceList;
            nlist->data = nsvX->name;
            nameSpaceList = nlist;

            nsvX->name->nameSpaceValues->next = globalNameSpace;
            globalNameSpace = nsvX->name->nameSpaceValues;
        }
        ParseAttributeSpecifiers(&lex, funcsp, TRUE);
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
    else switch(KW(lex))
    {
        LEXEME *start;
        case openpa:
            if (beforeOnly)
                break;
            start = lex;
            lex = getsym();
            /* in a parameter, open paren followed by a type is an  unnamed function */
            if (storage_class == sc_parameter && startOfType(lex,FALSE) && (!ISKW(lex) || !(lex->kw->tokenTypes & TT_LINKAGE) ))
            {
                TYPE *tp1;
                if (!*spi)
                {
                    sp = makeID(storage_class, *tp, *spi, NewUnnamedID());
                    SetLinkerNames(sp, lk_none);
                    sp->anonymous = TRUE;
                    sp->declcharpos = lex->charindex;
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
            else if (cparams.prm_cplusplus && consdest)
            {
                // constructor or destructor name
                STRUCTSYM s;
                char *name;
                if (consdest == CT_DEST)
                {
                    *tp = &stdvoid;
                    name = overloadNameTab[CI_DESTRUCTOR];
                }
                else
                {
                    TYPE *tp1 = Alloc(sizeof(TYPE));
                    tp1->type = bt_pointer;
                    tp1->size = getSize(bt_pointer);
                    tp1->btp = *tp;
                    tp1->rootType = tp1;
                    *tp = tp1;
                    name = overloadNameTab[CI_CONSTRUCTOR];
                }
                sp = makeID(storage_class, *tp, *spi, name);
                sp->declcharpos = lex->charindex;
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
            else
            {
                TYPE *quals = NULL;
                /* stdcall et.al. before the ptr means one thing in borland,
                 * stdcall after means another
                 * we are treating them both the same, e.g. the resulting
                 * pointer-to-function will be stdcall linkage either way
                 */
                lex = getQualifiers(lex, tp, linkage, linkage2, linkage3);
                lex = getBeforeType(lex, funcsp, &ptype, spi, strSym, nsv, inTemplate,
                                    storage_class, linkage, linkage2, linkage3, asFriend, FALSE, beforeOnly, TRUE);
                if (!ptype || (!isref(ptype) && !ispointer(ptype) && !isfunction(ptype) && basetype(ptype)->type != bt_memberptr))
                {
                    // if here is not a potential pointer to func
                    if (!ptype)
                        *tp = ptype;
                    ptype = NULL;
                    if (quals)
                    {
                        TYPE *atype = *tp;
                        *tp = quals;
                        while (quals->btp)
                            quals= quals->btp;
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
                    TYPE *atype = *tp;
                    *tp = ptype;
                    while ((isref(ptype) || isfunction(ptype) || 
                            ispointer(ptype) || basetype(ptype)->type == bt_memberptr) && ptype->btp)
                        ptype = ptype->btp;
                    ptype->btp = atype;
                    ptype->rootType = atype->rootType;
                    UpdateRootTypes(*tp);

                    if (quals)
                    {
                        atype = *tp;
                        *tp = quals;
                        while (quals->btp)
                            quals= quals->btp;
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
            BOOLEAN inparen  = FALSE;
            lex = getsym();
            if (funcptr && MATCHKW(lex, openpa))
            {
                lex = getsym();
                // this isn't perfect, it doesn't work with nested parens around the identifier
                
                if (!ISID(lex))
                    lex = backupsym();
                else
                    inparen = TRUE;
            }
            ParseAttributeSpecifiers(&lex, funcsp, TRUE);
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
            ptype = Alloc(sizeof(TYPE));
            ptype->type = xtype;
            ptype->btp = *tp;
            ptype->rootType = ptype;
            ptype->size = getSize(xtype);
            *tp = ptype;
            lex = getQualifiers(lex, tp, linkage, linkage2, linkage3);
            lex = getBeforeType(lex, funcsp, tp, spi, strSym, nsv, inTemplate,
                                storage_class, linkage, linkage2, linkage3, asFriend, FALSE, beforeOnly, FALSE);
            /*
            if (*tp  &&(ptype != *tp && isref(*tp)))
            {
                error(ERR_NO_REF_POINTER_REF);
            }
            */
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
        case and:
        case land:
            if (storage_class == sc_catchvar)
            {
                // already a ref;
                lex = getsym();
                ParseAttributeSpecifiers(&lex, funcsp, TRUE);
                lex = getQualifiers(lex, tp, linkage, linkage2, linkage3);
                lex = getBeforeType(lex, funcsp, tp, spi, strSym, nsv, inTemplate,
                                    storage_class, linkage, linkage2, linkage3, asFriend, FALSE, beforeOnly, FALSE);
                break;
            }
            // using the C++ reference operator as the ref keyword...
            if (cparams.prm_cplusplus || chosenAssembler->msil && storage_class == sc_parameter && KW(lex) == and)
            {
                TYPE *tp2;
                ptype = Alloc(sizeof(TYPE));
                if (MATCHKW(lex, and))
                    ptype->type = bt_lref;
                else
                    ptype->type = bt_rref;
                ptype->size = getSize(bt_pointer);
                ptype->btp = *tp;
                ptype->rootType = ptype;
                *tp = ptype;
                lex = getsym();
                ParseAttributeSpecifiers(&lex, funcsp, TRUE);
                lex = getQualifiers(lex, tp, linkage, linkage2, linkage3);
                lex = getBeforeType(lex, funcsp, tp, spi, strSym, nsv, inTemplate,
                                    storage_class, linkage, linkage2, linkage3, asFriend, FALSE, beforeOnly, FALSE);
                if (*tp)
                {
                    tp2 = *tp;
                    if (isfunction(tp2))
                        tp2 = basetype(tp2)->btp;
                    if (tp2  && ptype != basetype(tp2))
                    {
                        tp2 = basetype(tp2)->btp;
                        if (tp2  && (isref(tp2) || ispointer(tp2)))
                        {
                            if (tp2->array)
                                error(ERR_ARRAY_OF_REFS_NOT_ALLOWED);
                            else
                                error(ERR_NO_REF_POINTER_REF);
                        }
                    }
                }
                if (storage_class != sc_typedef && !isfunction(*tp))
                {
                    tp2 = *tp;
                    while (tp2 && tp2->type != bt_lref && tp2->type != bt_rref)
                    {
                        if (tp2->type == bt_const || tp2->type == bt_volatile)
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
            (*spi)->anonymous = TRUE;
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
                *spi = NULL;
                return lex;
            }
            if (storage_class != sc_parameter && storage_class != sc_cast && storage_class != sc_catchvar && !asFriend)
            {
                if (MATCHKW(lex, openpa) || MATCHKW(lex, openbr) || MATCHKW(lex, assign) || MATCHKW(lex, semicolon))
                    error(ERR_IDENTIFIER_EXPECTED);
                else
                    errortype(ERR_IMPROPER_USE_OF_TYPE, *tp, NULL);
            }
            if (!*tp)
            {
                *spi = NULL;
                return lex;
            }
            *spi = makeID(storage_class, *tp, *spi, NewUnnamedID());
            SetLinkerNames(*spi, lk_none);
            (*spi)->anonymous = TRUE;
            lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest);
            break;
    }
    if (*tp && (ptype = basetype(*tp)))
    {
        if (isfuncptr(ptype))
            ptype = basetype(ptype->btp);
        if (isfunction(ptype))
        {
            if (ptype->btp->type == bt_func || (ptype->btp->type == bt_pointer &&
                ptype->btp->array))
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
    ParseAttributeSpecifiers(&lex, funcsp, TRUE);
    return lex;
}
static EXPRESSION *vlaSetSizes(EXPRESSION ***rptr, EXPRESSION *vlanode,
                               TYPE *btp, SYMBOL *sp, int *index, int *vlaindex, int sou)
{
    EXPRESSION *mul , *mul1, *store;
    btp = basetype(btp);
    if (btp->vla)
    {
        mul = vlaSetSizes(rptr, vlanode, btp->btp, sp, index, vlaindex, sou);
        mul1 = Alloc(sizeof(EXPRESSION));
        *mul1 = *btp->esize;
        mul = mul1 = exprNode(en_arraymul, mul, mul1);
        btp->sp = sp;
        btp->vlaindex = (*vlaindex)++;
    }
    else
    {
#ifdef ERROR
#error Sizeof vla of vla
#endif
        mul = intNode(en_c_i, btp->size);
        mul1 = Alloc(sizeof(EXPRESSION));
        *mul1 = *mul;
    }
    store = exprNode(en_add, vlanode, intNode(en_c_i, *index));
    deref(&stdint, &store);
    store = exprNode(en_assign, store, mul1);
    **rptr = exprNode(en_void, store, NULL);
    *rptr = &(**rptr)->right;
    *index += sou;
    return mul;
}
static EXPRESSION * llallocateVLA(SYMBOL *sp, EXPRESSION *ep1, EXPRESSION *ep2)
{
    EXPRESSION *vp, *loader, *unloader;

    if (chosenAssembler->msil)
    {
        SYMBOL *al = gsearch("malloc");
        SYMBOL *fr = gsearch("free");
        if (al)
        {
            al = (SYMBOL *)al->tp->syms->table[0]->p;
            al->genreffed = TRUE;
        }
        if (fr)
        {
            fr = (SYMBOL *)fr->tp->syms->table[0]->p;
            fr->genreffed = TRUE;
        }
        if (al && fr)
        {
            FUNCTIONCALL *epx = (FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
            FUNCTIONCALL *ld = (FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
            epx->ascall = TRUE;
            epx->fcall = varNode(en_pc, al);
            epx->sp = al;
            epx->functp = al->tp;
            epx->arguments = (INITLIST *)Alloc(sizeof(INITLIST));
            epx->arguments->tp = &stdint;
            epx->arguments->exp = ep2;
            ep2 = intNode(en_func, 0);
            ep2->v.func = epx;
            ep2 = exprNode(en_assign, ep1, ep2);
            ld->ascall = TRUE;
            ld->fcall = varNode(en_pc, fr);
            ld->sp = fr;
            ld->functp = fr->tp;
            ld->arguments = (INITLIST *)Alloc(sizeof(INITLIST));
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
        EXPRESSION *var = anonymousVar(sc_auto, &stdpointer);
        loader = exprNode(en_savestack, var, NULL);
        unloader = exprNode (en_loadstack, var, NULL);
        ep1 = exprNode(en_assign, ep1, exprNode(en_alloca, ep2, NULL));
        ep1 = exprNode(en_void, loader, ep1);
    }    

    initInsert(&sp->dest, sp->tp, unloader, 0, FALSE);
    return ep1;
}
static void allocateVLA(LEXEME *lex, SYMBOL *sp, SYMBOL *funcsp, BLOCKDATA *block, 
                        TYPE *btp, BOOLEAN bypointer)
{
    EXPRESSION * result = NULL, **rptr = &result;
    TYPE *tp1 = btp;
    int count = 0;
    int soa = getSize(bt_pointer);
    int sou = getSize(bt_unsigned);
    while (tp1->vla)
    {
        count ++, tp1 = tp1->btp;
    }
    if (!funcsp)
        error(ERR_VLA_BLOCK_SCOPE); /* this is a safety, should already be caught */
    if (!bypointer)
    {
        /* vla */
        if (sp->storage_class != sc_auto && sp->storage_class != sc_typedef)
            error(ERR_VLA_BLOCK_SCOPE);	
    }
    else
    {
        /* pointer to vla */
        if (sp->storage_class != sc_auto && sp->storage_class != sc_typedef
            && sp->storage_class != sc_localstatic)
            error(ERR_VLA_BLOCK_SCOPE);	
    }
    currentLineData(block, lex,0);	
    if (sp->tp->sp)
    {
        SYMBOL *dest = sp;
        SYMBOL *src = sp->tp->sp;
        *rptr = exprNode(en_void, NULL, NULL);
        rptr = &(*rptr)->right;
        result->left = exprNode(en_blockassign, varNode(en_auto, dest),
                           varNode(en_auto, src));
        result->left->size = dest->tp->size = src->tp->size;
        result->altdata = (long)src->tp;
    }
    else
    {
        int size,i;
        SYMBOL *vlasp = sp;
        EXPRESSION *vlanode ;
        EXPRESSION *ep;
        int index = soa + sou;
        int vlaindex = 0;
        size = (count +2 )*(sou) + soa;
        vlanode = varNode(en_auto, vlasp);
        vlaSetSizes(&rptr, vlanode, btp, vlasp, &index, &vlaindex, sou);

        ep = exprNode(en_add, vlanode, intNode(en_c_i, soa));
        deref(&stdint, &ep);
        ep = exprNode(en_assign, ep, intNode(en_c_i, count));
        *rptr = exprNode(en_void, ep, NULL);
        rptr = &(*rptr)->right;

        basetype(sp->tp)->size = size; /* size field is actually size of VLA header block */
                             /* sizeof will fetch the size from that */
    }
    if (result != NULL)
    {
        STATEMENT *st = stmtNode(NULL, block, st_declare);
        st->hasvla = TRUE;
        if (sp->storage_class != sc_typedef && !bypointer)
        {
            EXPRESSION *ep1 = exprNode(en_add, varNode(en_auto, sp), intNode(en_c_i, 0));
            EXPRESSION *ep2 = exprNode(en_add, varNode(en_auto, sp), intNode(en_c_i, soa + sou * (count + 1)));
            deref(&stdpointer, &ep1);
            deref(&stdint, &ep2);
            ep1 = llallocateVLA(sp, ep1, ep2); //exprNode(en_assign, ep1, exprNode(en_alloca, ep2, NULL));
            *rptr = chosenAssembler->msil ? ep1 : exprNode(en_void, ep1, NULL);
        }
        st->select = result;
        optimize_for_constants(&st->select);
    }
}
void sizeQualifiers(TYPE *tp)
{
    while (TRUE)
    {
        TYPE *tp1 = basetype(tp);
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
static BOOLEAN sameQuals(TYPE *tp1, TYPE *tp2)
{
    while (tp1 && tp2)
    {
        if (isconst(tp1) != isconst(tp2))
            return FALSE;
        if (isvolatile(tp1) != isvolatile(tp2))
            return FALSE;
        if (isrestrict(tp1) != isrestrict(tp2))
            return FALSE;
        tp1 = basetype(tp1)->btp;
        tp2 = basetype(tp2)->btp;
    }
    return TRUE;
}
static LEXEME *getStorageAndType(LEXEME *lex, SYMBOL *funcsp, SYMBOL ** strSym, BOOLEAN inTemplate, BOOLEAN assumeType, enum e_sc *storage_class, enum e_sc *storage_class_in,
                       ADDRESS *address, BOOLEAN *blocked, BOOLEAN *isExplicit, BOOLEAN *constexpression, TYPE **tp, enum e_lk *linkage, enum e_lk *linkage2, 
                       enum e_lk *linkage3, enum e_ac access, BOOLEAN *notype, BOOLEAN *defd, int *consdest, BOOLEAN *templateArg)
{
    BOOLEAN foundType = FALSE;
    BOOLEAN first = TRUE;
    *blocked = FALSE;
    *constexpression = FALSE;

    while (KWTYPE(lex, TT_STORAGE_CLASS | TT_POINTERQUAL | TT_LINKAGE | TT_DECLARE) 
           || (!foundType && startOfType(lex, assumeType))  || MATCHKW(lex, compl) || (*storage_class == sc_typedef && !foundType))
    {
        if (KWTYPE(lex, TT_DECLARE))
        {
            // The next line has a recurring check. The 'lex' condition was already verified above
            // the problem is I'm using macros that I want to be independent from each other
            // and I don't want to introduce another macro without the check for readability
            // reasons.  So I'm going to leave the recurring check.
            if (MATCHKW(lex, kw_constexpr))
            {
                *constexpression = TRUE;
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
            lex = getQualifiers(lex, tp, linkage, linkage2, linkage3);
            if (MATCHKW(lex, kw_atomic))
            {
                foundType = TRUE;
                lex = getBasicType(lex, funcsp, tp, strSym, inTemplate, *storage_class_in, linkage, linkage2, linkage3, access, notype, defd, consdest, templateArg, *storage_class == sc_typedef, TRUE);
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
            foundType = TRUE;
            lex = getBasicType(lex, funcsp, tp, strSym, inTemplate, *storage_class_in, linkage, linkage2, linkage3, access, notype, defd, consdest, templateArg, *storage_class == sc_typedef, TRUE);
            if (*linkage3 == lk_threadlocal && *storage_class == sc_member)
                *storage_class = sc_static;
        }
        if (ParseAttributeSpecifiers(&lex, funcsp, TRUE))
            break;
        first = FALSE;
    }
    return lex;
}
static BOOLEAN mismatchedOverloadLinkage(SYMBOL *sp, HASHTABLE *table)
{
    if (((SYMBOL *)(table->table[0]->p))->linkage != sp->linkage)
        return TRUE;
    return FALSE;
}
void injectThisPtr(SYMBOL *sp, HASHTABLE *syms)
{
    if (syms)
    {
        HASHREC **hr = &syms->table[0];
        SYMBOL *ths;
        TYPE *type, *tpx;
        if (*hr && ((SYMBOL *)(*hr)->p)->thisPtr)
            return;
        type = tpx = Alloc(sizeof(TYPE));
        tpx->type = bt_pointer;
        tpx->size = getSize(bt_pointer);
        if (isconst(sp->tp))
        {
            tpx = tpx->btp = Alloc(sizeof(TYPE));
            tpx->type = bt_const;
            tpx->size = basetype(sp->parentClass->tp)->size;
        }
        if (isvolatile(sp->tp))
        {
            tpx = tpx->btp = Alloc(sizeof(TYPE));
            tpx->type = bt_volatile;
            tpx->size = basetype(sp->parentClass->tp)->size;
        }
        tpx->btp = basetype(sp->parentClass->tp);
        UpdateRootTypes(type);
        ths = makeID(sc_parameter, type, NULL, "__$$this");   
        ths->parent = sp;
        ths->thisPtr = TRUE;
        ths->used = TRUE;
        SetLinkerNames(ths, lk_cdecl);
    //    if ((*hr) && ((SYMBOL *)(*hr)->p)->tp->type == bt_void)
    //    {
    //        (*hr)->p = ths;
    //    }
    //    else
        {
            HASHREC *hr1 = Alloc(sizeof(HASHREC));
            hr1->p = (struct _hrintern_ *)ths;
            hr1->next = *hr;
            *hr = hr1;
        }
    }
}
static BOOLEAN hasTemplateParent(SYMBOL *sp)
{
    sp = sp->parentClass;
    while (sp)
    {
        if (sp->templateLevel)
            return TRUE;
        sp = sp->parentClass;
    }
    return FALSE;
}
static BOOLEAN differentTemplateNames(TEMPLATEPARAMLIST *a, TEMPLATEPARAMLIST *b)
{
    while (a && b)
    {
        if (a->p->type != b->p->type)
            return TRUE;
        if (!a->argsym || !b->argsym)
            return TRUE;
        if (strcmp(a->argsym->name, b->argsym->name))
            return TRUE;
        a = a->next;
        b = b->next;
    }
    return FALSE;
}
LEXEME *declare(LEXEME *lex, SYMBOL *funcsp, TYPE **tprv, enum e_sc storage_class, enum e_lk defaultLinkage, 
                       BLOCKDATA *block, BOOLEAN needsemi, int asExpression, BOOLEAN asFriend, BOOLEAN inTemplate, enum e_ac access)
{
    BOOLEAN isExtern = FALSE;
    TYPE *btp;
    SYMBOL *sp ;
    enum e_sc storage_class_in = storage_class;
    enum e_lk linkage = lk_none;
    enum e_lk linkage2 = lk_none;
    enum e_lk linkage3 = lk_none;
    NAMESPACEVALUES *nsv = NULL;
    SYMBOL *strSym = NULL;
    ADDRESS address = 0;
    TYPE *tp = NULL;
    BOOLEAN hasAttributes;
    char *oldDeprecationText = deprecationText;
    hasAttributes = ParseAttributeSpecifiers(&lex, funcsp, TRUE);

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
                isExtern = TRUE;
                goto jointemplate;
            }
            lex = backupsym();
        }
        if (!asExpression && MATCHKW(lex, kw_template))
        {
jointemplate:
            if (funcsp)
            {
                if (storage_class == sc_member  || storage_class == sc_mutable)
                    error(ERR_TEMPLATE_NO_LOCAL_CLASS);
                else
                    error(ERR_TEMPLATE_GLOBAL_OR_CLASS_SCOPE);
            }
            if (hasAttributes)
                error(ERR_NO_ATTRIBUTE_SPECIFIERS_HERE);
                
            lex = TemplateDeclaration(lex, funcsp, access, storage_class, isExtern);
            needsemi = FALSE;
        }            
        else if (!asExpression && MATCHKW(lex, kw_namespace))
        {
            BOOLEAN linked;
#ifdef PARSER_ONLY
            struct _ccNamespaceData nsData;
            nsData.declfile = lex->file;
            nsData.startline =  lex->line;
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
                        lex = declare(lex, NULL, NULL, storage_class, defaultLinkage, NULL, TRUE, FALSE, FALSE, FALSE, access);
                    }
                }
#ifdef PARSER_ONLY
                if (lex)
                    nsData.endline =  lex->line;                
#endif
                needkw(&lex, end);
                if (linked)
                {
                    SYMBOL *sp = (SYMBOL *)nameSpaceList->data;
                    sp->value.i--;
                    nameSpaceList = nameSpaceList->next;
                    globalNameSpace = globalNameSpace->next;
#ifdef PARSER_ONLY
                    nsData.next = sp->ccNamespaceData;
                    sp->ccNamespaceData = Alloc(sizeof(struct _ccNamespaceData));
                    *sp->ccNamespaceData = nsData;
#endif
                }
                needsemi = FALSE;
            }
        }
        else if (!asExpression && MATCHKW(lex, kw_using))
        {
            lex = getsym();
            sp = NULL;
            lex = insertUsing(lex, &sp, access, storage_class_in, inTemplate, hasAttributes);
            if (sp && tprv)
                *tprv = (TYPE *)-1;
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
            int incrementedStorageClass = 0;
            BOOLEAN blocked;
            BOOLEAN constexpression;
            BOOLEAN defd = FALSE;
            BOOLEAN notype = FALSE;
            BOOLEAN isExplicit = FALSE;
            BOOLEAN templateArg = FALSE;
            int consdest = CT_NONE;

			IncGlobalFlag(); /* in case we have to initialize a func level static */
            lex = getStorageAndType(lex, funcsp, &strSym, inTemplate, FALSE, &storage_class, &storage_class_in,
                                    &address, &blocked, &isExplicit, &constexpression, &tp, &linkage, &linkage2, &linkage3, access, &notype, &defd, &consdest, &templateArg);
            if (blocked)
            {
                if (tp != NULL)
                    error(ERR_TOO_MANY_TYPE_SPECIFIERS);
                DecGlobalFlag();
                    
                needsemi = FALSE;
            }
            else {
                if (storage_class != sc_static && storage_class != sc_localstatic &&
                    storage_class != sc_external)
                {
                    DecGlobalFlag();
                }
                else
                {
                    incrementedStorageClass++;
                }
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
                    BOOLEAN isTemplatedCast = FALSE;
                    TYPE *tp1 = tp;
                    NAMESPACEVALUES *oldGlobals;
                    BOOLEAN promotedToTemplate = FALSE;
                    if (!tp1)
                    {
                        // safety net
                        notype = TRUE;
                        tp = tp1 = Alloc(sizeof(TYPE));
                        tp->type = bt_int;
                        tp->size = getSize(tp->type);
                        tp->rootType = tp;
                    }
                    sp = NULL;
                    if (tp->type == bt_templateselector)
                    {
                        // have to special case a templated cast operator
                        TEMPLATESELECTOR *l = tp->sp->templateSelector;
                        while (l-> next)
                            l = l->next;
                        if (!strcmp(l->name, overloadNameTab[CI_CAST]))
                        {
                            strSym = tp->sp->templateSelector->next->sym;
                            while (!MATCHKW(lex, kw_operator))
                                lex = lex->prev;
                            notype = TRUE;
                            tp = tp1 = Alloc(sizeof(TYPE));
                            tp->type = bt_int;
                            tp->size = getSize(tp->type);
                            tp->rootType = tp;
                            isTemplatedCast = TRUE;
                        }
                    }
                    lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3);
                    lex = getBeforeType(lex, funcsp, &tp1, &sp, &strSym, &nsv, inTemplate,
                                    storage_class, &linkage, &linkage2, &linkage3, asFriend, consdest, FALSE, FALSE);
                    inTemplateType = FALSE;
                    if (isfunction(tp1))
                        sizeQualifiers(basetype(tp1)->btp);
                    if (strSym)
                        if  (!strcmp(strSym->name, sp->name))
                        {
                            sp->name = overloadNameTab[CI_CONSTRUCTOR];
                            consdest = CT_CONS;
                        }
                        else if (!strcmp(sp->name, overloadNameTab[CI_DESTRUCTOR]))
                        {
                            consdest = CT_DEST;
                        }
                    if (tprv && (!notype || consdest || isTemplatedCast || sp && sp->castoperator) )
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
                            STRUCTSYM *l = Alloc(sizeof(STRUCTSYM));
                            l->str = strSym;
                            addStructureDeclaration(l);
                        }
                    }
                    if (!incrementedStorageClass && tp1->type == bt_func)
                    {
                        /* at this point the arguments will be global but the rv won't */
                        incrementedStorageClass++;
                        IncGlobalFlag(); /* in case we have to initialize a func level static */
                    }
                    if (cparams.prm_cplusplus && isfunction(tp1) && (storage_class_in == sc_member || storage_class_in == sc_mutable) && !asFriend)
                    {
                        if (MATCHKW(lex,colon) ||
                                MATCHKW(lex, begin) || MATCHKW(lex, kw_try))
                        {
                            sp->isInline = TRUE;
                        }
                    }
                    if (linkage == lk_inline)
                    {
                        if (cparams.prm_cplusplus)
                            linkage = lk_virtual;
                        else
                            linkage = lk_none;
                        sp->isInline = TRUE;
                    }
                    else if (!cparams.prm_profiler && cparams.prm_optimize_for_speed && isfunction(tp1) && storage_class_in != sc_member && storage_class_in != sc_mutable)
                    {
                        if (!strSym)
                        {
                            if (MATCHKW(lex,colon) ||
                                    MATCHKW(lex, begin) || MATCHKW(lex, kw_try))
                            {
                                if (strcmp(sp->name, "main") != 0)
                                {
                                    sp->isInline = sp->dumpInlineToFile = sp->promotedToInline = TRUE;
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
                                if (strSym && strSym->tp->type != bt_enum && strSym->tp->type != bt_templateselector && strSym->tp->type != bt_templatedecltype)
                                {
                                    dropStructureDeclaration();
                                }
                                break;
                            }
                        }
                        tp1 = basetype(tp1);
                        if (cparams.prm_cplusplus && storage_class_in == sc_global && 
                            tp1->type == bt_union && tp1->sp->anonymous)
                        {
                            enum e_sc sc = storage_class_in;
                            if (sc != sc_member && sc != sc_mutable)
                                sc = sc_static;
                            
                            sp = makeID(sc, tp1, NULL, AnonymousName());
                            sp->anonymous = TRUE;
                            sp->access = access;
                            SetLinkerNames(sp, lk_c);
                            sp->parent = funcsp; /* function vars have a parent */
                            InsertSymbol(sp, sp->storage_class, linkage, FALSE);
                            if (storage_class != sc_static)
                            {
                                error(ERR_GLOBAL_ANONYMOUS_UNION_NOT_STATIC);
                            }
                            resolveAnonymousGlobalUnion(sp);
                        }
                        else if ((storage_class_in == sc_member  || storage_class_in == sc_mutable)&& isstructured(tp1) && tp1->sp->anonymous)
                        {
                            sp = makeID(storage_class_in, tp1, NULL, AnonymousName());
                            sp->anonymous = TRUE;
                            SetLinkerNames(sp, lk_c);
                            sp->parent = funcsp; /* function vars have a parent */
                            sp->parentClass = getStructureDeclaration();
                            InsertSymbol(sp, sp->storage_class, linkage, FALSE);
                        }
                        else
                        {
                            if (asFriend)
                            {
                                SYMBOL *sym = NULL;
                                LIST *l = Alloc(sizeof(LIST));
								STRUCTSYM *ls = structSyms;
								while (ls && !ls->str)
									ls = ls->next;
                                if (strSym && strSym->tp->type != bt_enum && strSym->tp->type != bt_templateselector && strSym->tp->type != bt_templatedecltype)
								{
									ls = ls->next;
									while (ls && !ls->str)
										ls = ls->next;
								}
								if (ls)
									sym = ls->str;
                                l->data = (void *)tp1->sp;
                                l->next = sym->friends;
                                sym->friends = l;
                            }
                            if (nsv)
                            {
                                globalNameSpace = oldGlobals;
                            }
                            if (strSym && strSym->tp->type != bt_enum && strSym->tp->type != bt_templateselector && strSym->tp->type != bt_templatedecltype)
                            {
                                dropStructureDeclaration();
                            }
                            break;
                        }
                    }
                    else
                        
                    {
                        SYMBOL *ssp = NULL;
                        SYMBOL *spi;
                        BOOLEAN checkReturn = TRUE;
                        if (isstructured(tp1) && storage_class != sc_typedef || !templateNestingCount && !structLevel)
                            tp1 = PerformDeferredInitialization(tp1, funcsp);
                        ssp = getStructureDeclaration();
                        if (!asFriend && (((storage_class_in == sc_member || storage_class_in == sc_mutable) && ssp) 
                                          || (inTemplate && strSym)))
                        {
                            sp->parentClass = ssp;
                        }
                        if (!inTemplate)
                        {
                            if (isfunction(sp->tp) && hasTemplateParent(sp))
                            {
                                SYMBOL *parent = sp->parentClass;
                                while (parent)
                                {
                                    if (parent->templateLevel)
                                        break;
                                        
                                    parent = parent->parentClass;
                                }
//                                sp->templateParams = parent->templateParams;
                                promotedToTemplate = TRUE;
                            }
                        }
                        else if (ssp && storage_class_in != sc_member && storage_class_in != sc_mutable && !asFriend)
                        {
                            storage_class = sc_member;
                        }
                        if (cparams.prm_cplusplus && !ssp && storage_class == sc_global && (isstructured(tp1) || basetype(tp1)->type == bt_enum))
                            if (basetype(tp1)->sp->anonymous)
                                storage_class = sc_static;
                        if (consdest != CT_NONE)
                        {
                            if (cparams.prm_cplusplus)
                                ConsDestDeclarationErrors(sp, notype);
                            if (isfunction(sp->tp))
                                if (consdest == CT_CONS)
                                    sp-> isConstructor = TRUE;
                                else
                                    sp-> isDestructor = TRUE;
                            else
                                sp->name = litlate(AnonymousName());
                        }
                        sp->parent = funcsp; /* function vars have a parent */
                        if (instantiatingTemplate)
                        {
                            if (sp->parentClass)
                                sp->parentNameSpace = sp->parentClass->parentNameSpace;
                        }
                        else if (nameSpaceList && storage_class_in != sc_auto)
                            sp->parentNameSpace = nameSpaceList->data;
                        if (inTemplate || promotedToTemplate)
                        {
                            if (asFriend)
                            {
                                SYMBOL *sp1 = sp->parentClass;
                                sp->templateLevel = inTemplate;
                                while (sp1)
                                {
                                    if (sp1->templateLevel)
                                        sp->templateLevel++;
                                    sp1 = sp1->parentClass;
                                }
                                
                            }
                            else
                            {
                                sp->templateLevel = templateHeaderCount;
                            }
                        }
                        sp->constexpression = constexpression;
                        sp->access = access;
                        sp->isExplicit = isExplicit;
                        sp->storage_class = storage_class;
                        if (inTemplate && (!sp->templateParams || MATCHKW(lex,colon) ||
                                MATCHKW(lex, begin) || MATCHKW(lex, kw_try)))
                            sp->templateParams = TemplateGetParams(sp);
                        if (sp->isDestructor && sp->xcMode == xc_unspecified)
                            sp->xcMode = xc_none;
                        /* removed in C++ 2014*/
                        /*
                        if (sp->constexpression && !sp->isDestructor && !sp->isConstructor)
                        {
                            TYPE *tpx = Alloc(sizeof(TYPE));
                            tpx->type = bt_const;
                            tpx->size = basetype(tp1)->size;
                            tpx->btp = tp1;
                            tpx->rootType = tp1->rootType;
                            tp1 = tpx;
                        }
                        */
                        if (!asFriend)
                            sp->tp = tp1;
                        if (!sp->instantiated)
                            sp->linkage = linkage;
                        sp->linkage2 = linkage2;
                        sp->linkage3 = linkage3;
                        if (linkage2 == lk_import)
                        {
                            sp->importfile = importFile;
                        }
                        SetLinkerNames(sp, storage_class_in == sc_auto && isstructured(sp->tp) ? lk_auto : linkage);
                        if (inTemplate && templateNestingCount == 1)
                        {
                            inTemplateBody++;
                        }
                        if (cparams.prm_cplusplus && isfunction(sp->tp) && (MATCHKW(lex, kw_try) || MATCHKW(lex, colon)))
                        {
                            BOOLEAN viaTry = MATCHKW(lex, kw_try);
                            int old = GetGlobalFlag();
                            if (viaTry)
                            {
                                sp->hasTry = TRUE;
                                lex = getsym();                                
                            }
                            if (MATCHKW(lex, colon))
                            {
                                lex = getsym();                                
                                sp->memberInitializers = GetMemberInitializers(&lex, funcsp, sp);
                            }
                        }
                        if (inTemplate && templateNestingCount == 1)
                        {
                            noSpecializationError++;
                        }
                        if (storage_class == sc_absolute)
                            sp->value.i= address;
                        if ((!cparams.prm_cplusplus || !getStructureDeclaration()) && !istype(sp) && sp->storage_class != sc_static &&
                            isfunction(basetype(tp1)) && !MATCHKW(lex, begin ))
                            sp->storage_class = sc_external;
                        if (isvoid(tp1))
                            if (sp->storage_class != sc_parameter && sp->storage_class != sc_typedef)
                                error(ERR_VOID_NOT_ALLOWED);
                        if (sp->linkage3 == lk_threadlocal)
                        {
                            if (isfunction(tp1))
                                error(ERR_FUNC_NOT_THREAD_LOCAL);
                            if (sp->storage_class != sc_external && sp->storage_class != sc_global && sp->storage_class != sc_static)
                            {
                                if (sp->storage_class == sc_auto || sp->storage_class == sc_register)
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
                        if (sp->linkage2 == lk_property && isfunction(tp1))
                            error(ERR_PROPERTY_QUALIFIER_NOT_ALLOWED_ON_FUNCTIONS);
                        if (storage_class != sc_typedef && isstructured(tp1) && basetype(tp1)->sp->isabstract)
                            errorabstract(ERR_CANNOT_CREATE_INSTANCE_ABSTRACT, basetype(tp1)->sp);
                        if (sp->packed)
                            error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                        if (storage_class == sc_mutable && isconst(tp))
                            errorsym(ERR_MUTABLE_NON_CONST, sp);
                        // correct for previous errors
                        if (sp->isConstructor && ispointer(sp->tp))
                            sp->tp = basetype(sp->tp)->btp;

                        if (ssp && strSym && strSym->tp->type != bt_templateselector && strSym->tp->type != bt_templatedecltype)
                        {
                            if (strSym != ssp && strSym->mainsym != ssp)
                            {
                                errorsym(ERR_IDENTIFIER_CANNOT_HAVE_TYPE_QUALIFIER, sp);
                            }
                        }
                        spi = NULL;
                        if (nsv && !strSym)
                        {
                            LIST *rvl;
                            unvisitUsingDirectives(nsv);
                            rvl = tablesearchinline(sp->name, nsv, FALSE);
                            if (rvl)
                                if (rvl->next)
                                    errorsym2(ERR_AMBIGUITY_BETWEEN, rvl->data, rvl->next->data);
                                else
                                    spi = (SYMBOL *)rvl->data;
                            else
                                errorNotMember(strSym, nsv, sp->name);
                        }
                        else {
                            if (isfunction(sp->tp) && sp->instantiated)
                            {
                                // may happen if a template function was declared at a lower level
                                spi = sp;
                            }
                            else
                            {
                                HASHREC **p;
                                if ((storage_class_in == sc_auto || storage_class_in == sc_parameter) && storage_class != sc_external && !isfunction(sp->tp))
                                {
                                    p = LookupName(sp->name, localNameSpace->syms);
                                }
                                else 
                                {
                                    ssp = getStructureDeclaration();
                                    if (ssp && !ssp->tp->syms && ssp->templateLevel)
                                        ssp = FindSpecialization(ssp, ssp->templateParams);
                                    if (ssp && ssp->tp->syms && (strSym || !asFriend))
                                    {
                                        p = LookupName(sp->name, ssp->tp->syms);				
                                    }
                                    else
                                    {
                                        p = LookupName(sp->name, globalNameSpace->syms);
                                    }
                                }
                                if (p)
                                {
                                    spi = (SYMBOL *)(*p)->p;
                                }
                            }
                        }
                        if (spi && spi->storage_class == sc_overloads)
                        {
                            SYMBOL *sym = NULL;
                            if (isfunction(sp->tp))
                            {
                                sym = searchOverloads(sp, spi->tp->syms);
                                if (sym && sym->templateParams && (!sp->templateParams || sp->templateParams->next) && !exactMatchOnTemplateParams(sym->templateParams, sp->templateParams))
                                    sym = NULL;
                            }
                            if (sym && cparams.prm_cplusplus)
                            {
                                if (sym->linkage == lk_c && sp->linkage == lk_cdecl)
                                    sp->linkage = lk_c;
                                if (sp->linkage != sym->linkage && ( (sp->linkage != lk_cdecl && sp->linkage != lk_virtual) || (sym->linkage != lk_cdecl && sym->linkage != lk_virtual))
                                                                    && !sp->isInline && !sym->isInline)
                                {
                                    preverrorsym(ERR_LINKAGE_MISMATCH_IN_FUNC_OVERLOAD, spi, spi->declfile, spi->declline);
                                }
                                else if (sym && !sym->isConstructor && !sym->isDestructor && !comparetypes(basetype(sp->tp)->btp, basetype((sym)->tp)->btp, TRUE))
                                {
                                    if (cparams.prm_cplusplus && isfunction(sym->tp) && (sym->templateLevel || templateNestingCount))
                                        checkReturn = FALSE;
                                }
                            }                            
                            if (inTemplate)
                                if (!sym)
                                {
                                    // a specialization
                                    // this may result in returning sp depending on what happens...
                                    TEMPLATEPARAMLIST *templateParams;
                                    if (sp->templateParams->p->bySpecialization.types)
                                    {
                                        HASHREC *hr = spi->tp->syms->table[0];
                                        while (hr)
                                        {
                                            SYMBOL *sym1 = (SYMBOL *)hr->p;
                                            if (sym1->templateLevel)
                                            {
                                                TEMPLATEPARAMLIST *one = sp->templateParams->p->bySpecialization.types;
                                                TEMPLATEPARAMLIST *two = sym1->templateParams->next;
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
                                        sp->specialized = TRUE;
                                    }
                                    else if (sp->templateParams && !sp->templateParams->next)
                                        sp->specialized = TRUE;
                                    sym = LookupFunctionSpecialization(spi, sp);
                                    if (sym == sp && spi->storage_class != sc_overloads)
                                    {
                                        sym = spi;
                                    }
                                    else if (sym)
                                    {
                                        InsertSymbol(sym, storage_class == sc_typedef ? storage_class_in : storage_class, linkage, FALSE);                                        
                                    }
                                    spi = NULL;
                                }
                            if (sym && sym->templateLevel != sp->templateLevel && (sym->templateLevel || !sp->templateLevel || sp->templateParams->next) && (!strSym || !strSym->templateLevel || sym->templateLevel != sp->templateLevel + 1))
                                sym = NULL;
                            if (sym)
                                spi = sym;
                            else
                            {
                                if ((nsv || strSym) && storage_class_in != sc_member && storage_class_in != sc_mutable && (!inTemplate || !sp->templateParams))
                                {
                                    char buf[256];
                                    if (!strcmp(sp->name, overloadNameTab[CI_CONSTRUCTOR]))
                                        strcpy(buf, strSym->name);
                                    else if (!strcmp(sp->name, overloadNameTab[CI_DESTRUCTOR]))
                                    {
                                        buf[0] = '~';
                                        strcpy(buf+1, strSym->name);
                                    }
                                    else
                                        strcpy(buf, sp->name);
                                    errorNotMember(strSym, nsv, buf);   
                                }
                                spi = NULL;
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
                            if (sp->storage_class == sc_member && spi->storage_class == sc_external)
                                sp->storage_class = sc_global;
                        }
                        if ((!spi || (spi->storage_class != sc_member && spi->storage_class != sc_mutable))
                             && sp->storage_class == sc_global && sp->isInline && !sp->promotedToInline)
                            sp->storage_class = sc_static;
                        if (spi && !sp->parentClass && !isfunction(spi->tp) && spi->storage_class != sc_type && sp->templateLevel)
                        {
                            SYMBOL *special = FindSpecialization(spi, sp->templateParams);
                            if (!special)
                            {
                                LIST *member = (LIST *)Alloc(sizeof(LIST));
                                member->data = sp;
                                member->next = spi->specializations;
                                spi->specializations = member;
                                sp->parentTemplate = spi;

                            }
                        }
                        else if (spi)
                        {
                            if (inTemplate)
                            {
								if (spi != sp)
								{
									TEMPLATEPARAMLIST *templateParams = TemplateGetParams(sp);
									if (templateParams->p->bySpecialization.types && spi->parentTemplate)
									{
										spi->templateParams = TemplateMatching(lex, spi->parentTemplate->templateParams, templateParams, spi,
																			   MATCHKW(lex, begin) || MATCHKW(lex, colon));
									}
									else
									{
										spi->templateParams = TemplateMatching(lex, spi->templateParams, templateParams, spi,
																			   MATCHKW(lex, begin) || MATCHKW(lex, colon));
									}
								}
                            }
                            else if (spi->templateLevel && !spi->instantiated && !templateNestingCount)
                            {
								if (!strSym || !strSym->templateLevel || spi->templateLevel != sp->templateLevel + 1)
	                                errorsym(ERR_IS_ALREADY_DEFINED_AS_A_TEMPLATE, sp);
                            }
                            if (isfunction(spi->tp))		
                            {
                                matchFunctionDeclaration(lex, sp, spi, checkReturn);
                            }
                            if (sp->parentClass)
                            {
                                if (spi->storage_class != sc_member && sp->storage_class != sc_member && spi->storage_class != sc_mutable && sp->storage_class != sc_mutable)
                                    if (spi->storage_class != sc_external && sp->storage_class != sc_external)
                                        preverrorsym(ERR_DUPLICATE_IDENTIFIER, spi, spi->declfile, spi->declline);
                            }
                            else
                            {
                                sp->parentClass = strSym;
                                if (!sp->parentClass && spi->parentClass) // error handling
                                    sp->parentClass = spi->parentClass;
                            }
                            if (sp->constexpression)
                                spi->constexpression = TRUE;
                            if (istype(spi))
                            {
                                if (cparams.prm_ansi || !comparetypes(sp->tp, (spi)->tp, TRUE) ||
                                    !istype(sp))
                                    preverrorsym(ERR_REDEFINITION_OF_TYPE, sp, spi->declfile, spi->declline);
                            }
                            else
                            {
                                if (spi->pushedTemplateSpecializationDefinition && (MATCHKW(lex, begin) || MATCHKW(lex, colon)))
                                {
                                    spi->pushedTemplateSpecializationDefinition = FALSE;
                                    spi->inlineFunc.stmt = NULL;
                                    spi->deferredCompile = NULL;
                                }
                                if ((isfunction(spi->tp) && (spi->inlineFunc.stmt || spi->deferredCompile) && (MATCHKW(lex, begin) || MATCHKW(lex, colon))
                                    && (!spi->parentClass || !spi->parentClass->instantiated || !spi->copiedTemplateFunction))
                                     && spi->parentClass && !differentTemplateNames(spi->parentClass->templateParams, sp->parentClass->templateParams))
                                {
                                    errorsym(ERR_BODY_ALREADY_DEFINED_FOR_FUNCTION, sp);
                                }
                                else if ((!isfunction(sp->tp) && !isfunction(spi->tp) && !comparetypes(sp->tp, (spi)->tp, TRUE)) || istype(sp))
                                {
                                    preverrorsym(ERR_TYPE_MISMATCH_IN_REDECLARATION, sp, spi->declfile, spi->declline);
                                }
                                else if (!sameQuals(sp->tp, spi->tp))
                                {
                                    errorsym(ERR_DECLARATION_DIFFERENT_QUALIFIERS, sp);
                                }
                                if (sp->linkage3 != spi->linkage3 && (sp->linkage3 == lk_threadlocal || spi->linkage3 == lk_threadlocal))
                                    if (!spi->parentClass)
                                        errorsym(ERR_THREAD_LOCAL_MUST_ALWAYS_APPEAR, sp);
                                if (strSym && storage_class == sc_static)
                                {
                                    errorstr(ERR_INVALID_STORAGE_CLASS, "static");
                                }
                                if (ismember(spi))
                                {
                                    if (!asFriend && !isfunction(sp->tp) && sp->storage_class != spi->storage_class && !sp->templateLevel)
                                        errorsym(ERR_CANNOT_REDECLARE_OUTSIDE_CLASS, sp);
                                    if (strSym)
                                    {
                                        sp->storage_class = spi->storage_class;
                                    }
                                }
                                switch(sp->storage_class)
                                {
                                    default:
                                        break;
                                    case sc_auto:
                                    case sc_register:
                                    case sc_parameter:
                                        preverrorsym(ERR_DUPLICATE_IDENTIFIER, sp, spi->declfile, spi->declline);
                                        break;
                                    case sc_external:
                                        if ((spi)->storage_class == sc_static)
                                        {
                                            if (!isfunction(spi->tp))
                                            {
                                                if (spi->constexpression)
                                                {
                                                    spi->storage_class = sc_global;
                                                }
                                                else
                                                {
                                                    errorsym(ERR_ANSI_FORBID_BOTH_EXTERN_STATIC, spi);
                                                    spi->storage_class = sc_global;
                                                }
                                            }
                                            else if (spi->isInline && basetype(spi->tp)->type == bt_ifunc)
                                            {
                                                spi->storage_class = sc_global;
                                                GENREF(spi);
                                            }
                                        }
                                        break;
                                    case sc_global:
                                        if (spi->storage_class != sc_static)
                                        {
                                            spi->declfile = sp->declfile;
                                            spi->declline = sp->declline;
                                            spi->declfilenum = sp->declfilenum;
                                            spi->storage_class = sc_global;
                                        }
                                        break;
                                    case sc_mutable:
                                        if ((spi)->storage_class != sc_mutable)
                                        {
                                            preverrorsym(ERR_DUPLICATE_IDENTIFIER, sp, spi->declfile, spi->declline);
                                        }
                                        break;
                                    case sc_member:
                                        if ((spi)->storage_class == sc_static)
                                        {
                                            preverrorsym(ERR_DUPLICATE_IDENTIFIER, sp, spi->declfile, spi->declline);
                                        }
                                        break;
                                    case sc_static:
                                        if ((spi)->storage_class == sc_external && !isfunction(spi->tp)&& !spi->constexpression)
                                        {
                                            errorsym(ERR_ANSI_FORBID_BOTH_EXTERN_STATIC, spi);
                                            spi->storage_class = sc_global;
                                        }
                                        else if (ismember(spi))
                                        {
                                            preverrorsym(ERR_DUPLICATE_IDENTIFIER, sp, spi->declfile, spi->declline);
                                        }
                                        else
                                        {
                                            spi->storage_class = sp->storage_class;
                                        }
                                        break;
                                    case sc_localstatic:
                                    case sc_constexpr:
                                        spi->storage_class = sp->storage_class;
                                        break;
                                }
                            }
                            if (isfunction(spi->tp))
                            {
                                TEMPLATEPARAMLIST **nw = &sp->templateParams;
                                while (*nw)
                                {
                                    if ((*nw)->argsym)
                                    {
                                        TEMPLATEPARAMLIST *old = spi->templateParams;
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
                                if (cparams.prm_cplusplus && (MATCHKW(lex, begin) || MATCHKW(lex, colon) || MATCHKW(lex, kw_try)))
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
                            if (sp->oldstyle)
                                spi->oldstyle = TRUE;
                            if (!spi->declfile || spi->tp->type == bt_func)// && !spi->parentClass && !spi->specialized)
                            {
                                spi->declfile = sp->declfile;
                                spi->declline = sp->declline;
                                spi->declfilenum = sp->declfilenum;
                            }
                            spi->memberInitializers = sp->memberInitializers;
                            spi->hasTry = sp->hasTry;

                            spi->isInline |= sp->isInline;
                            spi->dumpInlineToFile |= sp->dumpInlineToFile;
                            spi->promotedToInline |= sp->promotedToInline;
                            sp = spi;
                            sp->redeclared = TRUE;
                        }
                        else
                        {
                            if (inTemplate)
                            {
                                TEMPLATEPARAMLIST *templateParams = TemplateGetParams(sp);
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
                                    TemplateMatching(lex, NULL, templateParams, sp,
                                                     MATCHKW(lex, begin) || MATCHKW(lex, colon));
                                }
                            }
                            if (isfunction(sp->tp))		
                            {
                                matchFunctionDeclaration(lex, sp, sp, checkReturn);
                                if (inTemplate)
                                    sp->parentTemplate = sp;
                            }
                            else if (sp->storage_class != sc_type)
                            {
                                if (inTemplate)
                                    sp->parentTemplate = sp;

                            }
                            if (!asFriend || isfunction(sp->tp))
                            {
                                if (sp->constexpression && sp->storage_class == sc_global)
                                    sp->storage_class = sc_static;
                                if (!asFriend || !templateNestingCount || inTemplate)
                                {
                                    if (sp->storage_class == sc_external || asFriend && !MATCHKW(lex, begin) && !MATCHKW(lex, colon))
                                    {
                                        InsertSymbol(sp, sc_external, linkage, FALSE);
                                        if (!sp->templateLevel || asFriend)
                                            InsertExtern(sp);
                                        if (sp->templateLevel)
                                        {
                                            InsertInline(sp);
                                        }
                                    }
                                    else
                                    {
                                        InsertSymbol(sp, storage_class == sc_typedef ? storage_class_in : storage_class, linkage, FALSE);
                                        if (isfunction(sp->tp))
                                        {
                                            if  (getStructureDeclaration() || asFriend)
                                            {
                                                SYMBOL *parent = getStructureDeclaration();
                                                if (!sp->templateLevel || asFriend)
                                                    InsertExtern(sp);
                                                  
                                                while (parent)
                                                {
                                                    if (parent->templateLevel)
                                                    {
                                                        InsertInline(sp);
                                                        break;
                                                    }
                                                    parent = parent->parentClass;
                                                }
                                            }
                                        }
                                        else 
                                            if (sp->templateLevel && (sp->storage_class == sc_type || isfunction(sp->tp)))
                                            {
                                                InsertInline(sp);
                                            }
                                    }
                                }
                            }
                            if (asFriend && !sp->anonymous && !isfunction(sp->tp) && !templateNestingCount)
                            {
                                error(ERR_DECLARATOR_NOT_ALLOWED_HERE);
                            }
                        }
                        
                        if (asFriend)
                        {
                            if (isfunction(sp->tp))
                            {
                                SYMBOL *sym = NULL;
                                LIST *l = Alloc(sizeof(LIST));
								STRUCTSYM *ls = structSyms;
								while (ls && !ls->str)
									ls = ls->next;
                                if (strSym && strSym->tp->type != bt_enum && strSym->tp->type != bt_templateselector && strSym->tp->type != bt_templatedecltype)
								{
									ls = ls->next;
									while (ls && !ls->str)
										ls = ls->next;
								}
								if (ls)
									sym = ls->str;
                                l->data = (void *)sp;
                                l->next = sym->friends;
                                sym->friends = l;
                            }   
                        }
                        if (!ismember(sp) && !sp->constexpression && !istype(sp))
                        {
                            if (isfunction(sp->tp) && (isconst(sp->tp) || isvolatile(sp->tp)))
                                error(ERR_ONLY_MEMBER_CONST_VOLATILE);
                        }
                        if (inTemplate && templateNestingCount == 1)
                        {
                            inTemplateBody--;
                            noSpecializationError --;
                        }
                    }
                    if (sp)
                    {
                        sp->deprecationText = deprecationText;
                        if  (!strcmp(sp->name, overloadNameTab[CI_DESTRUCTOR]))
                        {
                            if (!isfunction(tp1))
                            {
                                errorsym(ERR_CONSTRUCTOR_OR_DESTRUCTOR_MUST_BE_FUNCTION, sp->parentClass);
                            }
                            else
                            {
                                HASHREC *hr = basetype(tp1)->syms->table[0];
                                SYMBOL *s = (SYMBOL *)hr->p;
                                if (s->tp->type != bt_void)
                                    errorsym(ERR_DESTRUCTOR_CANNOT_HAVE_PARAMETERS, sp->parentClass);
                            }
                        }
                        else if (!strcmp(sp->name, overloadNameTab[CI_CONSTRUCTOR]))
                        {                        
                            if (!isfunction(tp1))
                                errorsym(ERR_CONSTRUCTOR_OR_DESTRUCTOR_MUST_BE_FUNCTION, sp->parentClass);
                        }
                    }
                    if (!templateArg)
                        checkDeclarationAccessible(sp->tp, isfunction(sp->tp) ? sp : NULL);
                    if (sp->operatorId)
                        checkOperatorArgs(sp, asFriend);
                    if (sp->storage_class == sc_typedef)
                    {
                        // all this is so we can have multiple typedefs referring to the same thing...
                        TYPE *tp = Alloc(sizeof(TYPE));
                        tp->type = bt_typedef;
                        tp->btp = sp->tp;
                        tp->rootType = sp->tp->rootType;
                        sp->tp = tp;
                        tp->sp = sp;
                        tp->size = tp->btp->size;
                    }
                    if (ispointer(sp->tp) && sp->storage_class != sc_parameter)
                    {
                        btp = basetype(sp->tp);
                        if (btp->vla )
                        {
                            allocateVLA(lex, sp, funcsp, block, btp, FALSE);
                        }
                        else
                        {
                            btp = basetype(btp->btp);
                            if (btp->vla)
                            {
                                allocateVLA(lex, sp, funcsp, block, btp, TRUE);
                                btp->size = basetype(sp->tp)->size;
                            }
                        }
                    }
                    UpdateRootTypes(tp1);
                    sizeQualifiers(tp1);
                    if (sp->isInline)
                    {
                        if (!isfunction(sp->tp))
                            error(ERR_INLINE_NOT_ALLOWED);
                    }
                    if (!strcmp(sp->name, "main"))
                    {
                        // fixme don't check if in parent class...
                        if (!globalNameSpace->next)
                        {
                            if (cparams.prm_cplusplus)
                                SetLinkerNames(sp, lk_c);
                        }
                    }
                    if (inTemplate && templateNestingCount == 1)
                    {
                        inTemplateBody++;
                        noSpecializationError++;
                    }
                            
                    if (lex)
                    {
                        if (linkage != lk_cdecl)
                            sp->linkage = linkage;
                        if (linkage2 != lk_none)
                            sp->linkage2 = linkage2;
                        if (linkage2 == lk_import)
                        {
                            sp->importfile = importFile;
                        }
                        if (basetype(sp->tp)->type == bt_func)
                        {
                            if (funcsp && storage_class == sc_localstatic)
                                errorstr(ERR_INVALID_STORAGE_CLASS, "static");
                            if (storage_class == sc_member && !cparams.prm_cplusplus)
                                error(ERR_FUNCTION_NOT_IN_STRUCT);
                            if (isstructured(basetype(sp->tp)->btp) || basetype(basetype(sp->tp)->btp)->type == bt_enum)
                                if (defd)
                                    errorsym(ERR_TYPE_DEFINITION_NOT_ALLOWED_HERE, basetype(sp->tp->btp)->sp);
                            if (sp->castoperator)
                            {
                                if (!notype)
                                    errortype(ERR_TYPE_OF_RETURN_IS_IMPLICIT_FOR_OPERATOR_FUNC, basetype(sp->tp)->btp, NULL);
                            }
                            else
                            {
                                if (notype)
                                    if (!sp->isConstructor && !sp->isDestructor)
                                        error(ERR_MISSING_TYPE_SPECIFIER);
                            }
                            if (cparams.prm_cplusplus)
                            {
                                if (ismember(sp))
                                {
                                    injectThisPtr(sp, basetype(sp->tp)->syms);
                                }
                            }
                            if (sp->storage_class == sc_static && chosenAssembler->msil)
                            {
                                if (!sp->label)
                                    sp->label = nextLabel++;
                            }

                            if (/*templateNestingCount &&*/ nameSpaceList)
                                SetTemplateNamespace(sp);
                            if (MATCHKW(lex, begin))
                            {
                                TYPE *tp = sp->tp;
                                HASHREC *hr ;
                                int old = GetGlobalFlag();
                                if (sp->storage_class == sc_member && storage_class_in == sc_member)
                                    browse_variable(sp); 
                                if (sp->storage_class == sc_external)
                                    sp->storage_class = sc_global;
                                while (tp->type != bt_func)
                                    tp = tp->btp;
                                tp->type = bt_ifunc;
                                hr = tp->syms->table[0];
                                    
                                while (hr)
                                {
                                    SYMBOL *sym = (SYMBOL *)hr->p;
                                    TYPE *tpl = sym->tp;
                                    int old = GetGlobalFlag();
                                    while (tpl)
                                    {
                                        if (tpl->unsized)
                                        {
                                            specerror(ERR_UNSIZED_VLA_PARAMETER, 
                                                      sym->name, sym->declfile, 
                                                      sym->declline);
                                            while (hr->next)
                                                hr = hr->next;
                                            break;
                                            
                                        }
                                        tpl = tpl->btp;
                                    }
                                    hr = hr->next;
                                }
                                if (storage_class_in == sc_member || storage_class_in == sc_mutable)
                                {
                                    sp->instantiatedInlineInClass = TRUE;
                                }
                                if (storage_class_in != sc_member && TemplateFullySpecialized(sp->parentClass))
                                {
                                    GENREF(sp);
                                    sp->linkage = lk_virtual;
                                    SetGlobalFlag(old + 1);
                                    lex = body(lex, sp);
                                    SetGlobalFlag(old);
                                }
                                else if (storage_class_in == sc_member || storage_class_in == sc_mutable || templateNestingCount == 1 || asFriend && templateNestingCount==2)
                                {
                                    STATEMENT *startStmt = currentLineData(NULL, lex, 0);
                                    if (startStmt)
                                        sp->linedata = startStmt->lineData;
                                    lex = getDeferredData(lex, sp, TRUE);
                                    InsertInline(sp);
                                }
                                else
                                {
                                    SetGlobalFlag(old + 1);
                                    lex = body(lex, sp);
                                    SetGlobalFlag(old);
                                }
                                if (sp->constexpression)
                                {
                                    sp->isInline = TRUE;
                                    if (sp->nonConstVariableUsed)
                                        errorsym(ERR_CONSTANT_FUNCTION_EXPECTED, sp);
                                }
                                needsemi = FALSE;
                            }
                            else
                            {
                                browse_variable(sp); 
                                if (sp->memberInitializers)
                                    errorsym(ERR_CONSTRUCTOR_MUST_HAVE_BODY, sp);
                                else if (sp->hasTry)
                                    error(ERR_EXPECTED_TRY_BLOCK);
                                if (cparams.prm_cplusplus && MATCHKW(lex, assign))
                                {
                                    lex = getsym();
                                    if (MATCHKW(lex, kw_delete))
                                    {
                                        sp->deleted = TRUE;
                                        if (sp->redeclared && !templateNestingCount)
                                        {
                                            errorsym(ERR_DELETE_ON_REDECLARATION, sp);
                                        }
                                        lex = getsym();
                                    }
                                    else if (MATCHKW(lex, kw_default))
                                    {
                                        sp->defaulted = TRUE;
                                        SetParams(sp);
                                        // fixme add more 
                                        if (strcmp(sp->name,overloadNameTab[CI_CONSTRUCTOR]) != 0 &&
                                            strcmp(sp->name, overloadNameTab[CI_DESTRUCTOR]) != 0 &&
                                            strcmp(sp->name, overloadNameTab[assign - kw_new + CI_NEW]) != 0) // this is meant to be a copy cons but is too loose
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
                                        sp->ispure = TRUE;
                                    }
                                }
                                else if (sp->constexpression && sp->storage_class != sc_external && !isfunction(sp->tp))
                                {
                                    error(ERR_CONSTEXPR_REQUIRES_INITIALIZER);
                                }
                                else if (sp->parentClass && !sp->templateParams && !chosenAssembler->msil)
                                    if (!asFriend && storage_class_in != sc_member && storage_class_in != sc_mutable && !sp->templateLevel)
                                        errorsym(ERR_CANNOT_REDECLARE_OUTSIDE_CLASS, sp);
                            }
                        }
                        else
                        {
                            LEXEME *hold = lex;
                            BOOLEAN structuredArray = FALSE;
                            if (notype)
                                error(ERR_MISSING_TYPE_SPECIFIER);
                            if (linkage3 == lk_entrypoint)
                            {
                                errorsym(ERR_ENTRYPOINT_FUNC_ONLY, sp);
                            }
                            if (sp->storage_class == sc_virtual)
                            {
                                errorsym(ERR_NONFUNCTION_CANNOT_BE_DECLARED_VIRTUAL, sp);
                            }
                            if (asExpression && !(asExpression & _F_NOCHECKAUTO))
                                checkauto(sp->tp, ERR_AUTO_NOT_ALLOWED);
                            if (sp->storage_class == sc_auto || sp->storage_class == sc_register)
                            {
                                STATEMENT *s = stmtNode(lex, block, st_varstart);
                                s->select = varNode(en_auto, sp);
                            }
                            if (sp->storage_class == sc_localstatic)
                            {
                                if (instantiatingTemplate)
                                    sp->storage_class = sc_static;                                
                                else if (!sp->label)
                                    sp->label = nextLabel++;
                            }
                            if (!sp->label && sp->storage_class == sc_static && chosenAssembler->msil)
                                sp->label = nextLabel++;
                            if (cparams.prm_cplusplus)
                            {
                                TYPE *tp2= sp->tp;
                                while (isarray(tp2))
                                    tp2 = basetype(tp2)->btp;
                                structuredArray = isstructured(tp2);
                            }
                            if (cparams.prm_cplusplus && sp->storage_class != sc_type && sp->storage_class != sc_typedef && structLevel && (!instantiatingTemplate) && (MATCHKW(lex,assign) || MATCHKW(lex, begin) || structuredArray))
                            {
                                if ((MATCHKW(lex, assign) || MATCHKW(lex, begin)) && storage_class_in == sc_member && (sp->storage_class == sc_static ||sp->storage_class == sc_external))
                                    if (isconst(sp->tp) || sp->constexpression)
                                    {
                                        if (isint(sp->tp))
                                           goto doInitialize;
                                    }
                                    else
                                    {
                                        errorsym(ERR_CANNOT_INITIALIZE_STATIC_MEMBER_IN_CLASS, sp);
                                    }
                                lex = getDeferredData(lex, sp, FALSE);
                            }
                            else
                            {
doInitialize:                                
                                if (cparams.prm_cplusplus && isstructured(sp->tp))
                                {
                                    SYMBOL *sp1 = basetype(sp->tp)->sp;
                                    if (!templateNestingCount && sp1->templateLevel && sp1->templateParams && !sp1->instantiated)
                                    {
                                        if (!allTemplateArgsSpecified(sp1, sp1->templateParams))
                                            sp1 = GetClassTemplate(sp1, sp1->templateParams->next, FALSE);
                                        if (sp1)
                                            sp->tp = TemplateClassInstantiate(sp1, sp1->templateParams, FALSE, sc_global)->tp;
                                    }
                                }
                                lex = initialize(lex, funcsp, sp, storage_class_in, asExpression, 0); /* also reserves space */
                                if (sp->storage_class == sc_auto || sp->storage_class == sc_register || (sp->storage_class == sc_localstatic && sp->init))
                                {
                                    BOOLEAN doit = TRUE;
                                    if (sp->storage_class == sc_localstatic)
                                    {
                                        INITIALIZER *init = sp->init;
                                        while (init)
                                        {
                                            if (init->exp && !IsConstantExpression(init->exp, FALSE))
                                                break;
                                            init = init->next;
                                        }
                                        if (!init)
                                            doit = FALSE;
                                    }
                                    if (doit && (sp->init || isarray(sp->tp) && sp->tp->msil))
                                    {
                                        STATEMENT *st ;
                                        currentLineData(block, hold,0);
                                        st = stmtNode(hold, block, st_expr);
                                        st->select = convertInitToExpression(sp->tp, sp, funcsp, sp->init, NULL, FALSE);
                                    }
                                }
                            }
                        }
                        if (sp->tp->size == 0 && sp->tp->type != bt_templateparam && !isarray(sp->tp))
                        {
                            if (storage_class != sc_typedef)
                            {
                                if (storage_class_in == sc_auto && sp->storage_class != sc_external && !isfunction(sp->tp))
                                    errorsym(ERR_UNSIZED, sp);
                                if (storage_class_in == sc_parameter)
                                    errorsym(ERR_UNSIZED, sp);
                            }
                        }
                        sp->tp->used = TRUE;
                    }
                    if (isExplicit && !sp->castoperator &&!sp->isConstructor)
                        error(ERR_EXPLICIT_CONSTRUCTOR_OR_CONVERSION_FUNCTION);
                    if (inTemplate && templateNestingCount == 1)
                    {
                        inTemplateBody--;
                        noSpecializationError--;
                        TemplateGetDeferred(sp);
                    }
                    if (!strcmp(sp->name, "main") && !sp->parentClass)
                    {
                        // fixme don't check if in parent class...
                        if (!globalNameSpace->next)
                        {
                            if (sp->isInline)
                            {
                                error(ERR_MAIN_CANNOT_BE_INLINE_FUNC);
                            }
                            else if (storage_class == sc_static)
                            {
                                error(ERR_MAIN_CANNOT_BE_STATIC_FUNC);
                            }
                            else if (sp->constexpression)
                            {
                                error(ERR_MAIN_CANNOT_BE_CONSTEXPR);
                            }
                            else if (sp->deleted)
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
                } while (!asExpression && !inTemplate && MATCHKW(lex, comma) && (lex = getsym()) != NULL);
                if (incrementedStorageClass)
                {
                    DecGlobalFlag(); /* in case we have to initialize a func level static */
                }
            }
        }
    }
    FlushLineData(includes->fname, includes->line);
    if (needsemi && !asExpression && !needkw(&lex, semicolon))
    {
        errskim(&lex, skim_semi_declare);
        skip(&lex, semicolon);
    }
    deprecationText = oldDeprecationText;
    return lex;
}
