/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provideinitiad that the following 
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
#include <assert.h>
/* initializers, local... can do w/out c99 */

extern ARCH_DEBUG *chosenDebugger;
extern ARCH_ASM *chosenAssembler;

extern enum e_kw skim_closebr[];
extern enum e_kw skim_comma[];
extern enum e_kw skim_end[];
extern enum e_kw skim_semi[];
extern NAMESPACEVALUES *globalNameSpace, *localNameSpace;
extern TYPE stdwcharptr;
extern TYPE stdstring;
extern TYPE stdchar16tptr;
extern TYPE stdchar32tptr;
extern TYPE stdint;
extern TYPE stdvoid;
extern TYPE stdpointer;
extern int startlab, retlab;
extern BOOL declareAndInitialize;

typedef struct _startups_
{
    struct _startups_ *next;
    char *name;
    int prio;
} STARTUP;

typedef struct _alias_
{
    struct _alias_ *next;
    char *name;
    char *alias;
} ALIAS;

typedef struct _dyninit_
{
    struct _dyninit_ * next;
    SYMBOL *sp;
    INITIALIZER *init;
} DYNAMIC_INITIALIZER;

BOOL initializingGlobalVar ;

static DYNAMIC_INITIALIZER *dynamicInitializers, *TLSInitializers;
static DYNAMIC_INITIALIZER *dynamicDestructors, *TLSDestructors;
static STARTUP *startupList, *rundownList;
static LIST *symListHead, *symListTail;
static HASHTABLE *aliasHash;
static int inittag = 0;

LEXEME *initType(LEXEME *lex, SYMBOL *funcsp, int offset, enum e_sc sc, 
                 INITIALIZER **init, INITIALIZER **dest, TYPE *itype, SYMBOL *sp, BOOL arrayMember);

void init_init(void)
{
    symListHead = NULL;
    startupList = rundownList = NULL;
    aliasHash = CreateHashTable(13);
    dynamicInitializers = TLSInitializers = NULL;
    dynamicDestructors = TLSDestructors = NULL;
    initializingGlobalVar = FALSE;
}
void dumpStartups(void)
{
#ifndef PARSER_ONLY
    SYMBOL *s;
    if (startupList)
    {
        startupseg();
        while (startupList)
        {
            s = search(startupList->name, globalNameSpace->syms);
            if (!s || s->storage_class != sc_overloads)
                errorstr(ERR_UNDEFINED_IDENTIFIER, startupList->name);
            else
            {
                s = search(startupList->name, s->tp->syms);
                gensrref(s, startupList->prio);
                  s->used = TRUE;
            }
            startupList = startupList->next;
        }
    }
    if (rundownList)
    {
        rundownseg();
        while (rundownList)
        {
            s = search(rundownList->name, globalNameSpace->syms);
            if (!s || s->storage_class != sc_overloads)
                errorstr(ERR_UNDEFINED_IDENTIFIER, rundownList->name);
            else
            {
                s = search(rundownList->name, s->tp->syms);
                gensrref(s, rundownList->prio);
                s->used = TRUE;
            }
            rundownList = rundownList->next;
        }
    }
#endif
}
void insertStartup(BOOL startupFlag, char *name, int prio)
{
    STARTUP *startup ;
    
    IncGlobalFlag();
    startup = (STARTUP *)Alloc(sizeof(STARTUP));
    startup->name = litlate(name);
    DecGlobalFlag();
    
    startup->prio = prio;
    if (startupFlag)
    {
        startup->next = startupList;
        startupList = startup;
    }
    else
    {
        startup->next = rundownList;
        rundownList = startup;
    }
}
char *lookupAlias(char *name)
{
    ALIAS *x = (ALIAS *)search(name, aliasHash);
    if (x)
        return x->alias;
    return NULL;
}
void insertAlias(char *name, char *alias)
{
    ALIAS *newAlias ;
    IncGlobalFlag();
    newAlias = (ALIAS *)Alloc(sizeof(ALIAS));
    newAlias->name = name;
    newAlias->alias = alias;
    insert((SYMBOL *)newAlias, aliasHash);
    DecGlobalFlag();	
}
static int dumpBits(INITIALIZER **init)
{
#ifndef PARSER_ONLY
    int offset = (*init)->offset;
    LLONG_TYPE resolver = 0;
    TYPE *base = basetype((*init)->basetp);
    do {
        LLONG_TYPE i = 0;
        TYPE *tp = basetype((*init)->basetp);
        if (tp->anonymousbits)
        {
            *init = (*init)->next;
            continue;
        }
        if (isfloatconst((*init)->exp))
        {
            i = FPFToLongLong(&(*init)->exp->v.f);
        }
        else if (isintconst((*init)->exp))
        {
            i = (*init)->exp->v.i;
        }
        else if (isimaginaryconst((*init)->exp))
        {
            i = 0;
        }
        else if (iscomplexconst((*init)->exp))
        {
            i = FPFToLongLong(&(*init)->exp->v.c.r);
        }
        else
            diag("dump-bits: non-constant");
#ifdef ERROR
#error REVERSE BITS
#endif
        i &= mod_mask(tp->bits);
        resolver |= i << tp->startbit;
        *init = (*init)->next;
    } while ((*init) && (*init)->offset == offset);
    switch(base->type)
    {
        case bt_char:
        case bt_unsigned_char:
            genbyte(resolver);
            break;
        case bt_short:
        case bt_unsigned_short:
            genshort(resolver);
            break;
        case bt_int:
        case bt_unsigned:
            genint(resolver);
            break;
        case bt_char16_t:
            genuint16(resolver);
            break;
        case bt_char32_t:
            genuint32(resolver);
            break;
        case bt_long:
        case bt_unsigned_long:
            genlong(resolver);
            break;
        case bt_long_long:
        case bt_unsigned_long_long:
            genlonglong(resolver);
            break;
        default:
            diag("dumpBits: unknown bit size");
            break;
    }
    if (isatomic((*init)->basetp) && needsAtomicLockFromType((*init)->basetp))
        genstorage(ATOMIC_FLAG_SPACE);
    return base->size;
#else
    return 4;
#endif
}
void insertDynamicInitializer(SYMBOL *sp, INITIALIZER *init)
{
    DYNAMIC_INITIALIZER *di = Alloc(sizeof(DYNAMIC_INITIALIZER));
    di->sp = sp;
    di->init = init;
    if (sp->linkage3 == lk_threadlocal)
    {
        di->next = TLSInitializers;
        TLSInitializers = di;
    }
    else
    {
        di->next = dynamicInitializers;
        dynamicInitializers = di;
    }
//	genstorage(sp->tp->size);
    
}
static void insertTLSInitializer(SYMBOL *sp, INITIALIZER *init)
{
    DYNAMIC_INITIALIZER *di = Alloc(sizeof(DYNAMIC_INITIALIZER));
    di->sp = sp;
    di->init = init;
    di->next = TLSInitializers;
    TLSInitializers = di;
//	genstorage(sp->tp->size);
    
}
void insertDynamicDestructor(SYMBOL *sp, INITIALIZER *init)
{
    DYNAMIC_INITIALIZER *di = Alloc(sizeof(DYNAMIC_INITIALIZER));
    di->sp = sp;
    di->init = init;
    if (sp->linkage3 == lk_threadlocal)
    {
        di->next = TLSDestructors;
        TLSDestructors = di;
    }
    else
    {
        di->next = dynamicDestructors;
        dynamicDestructors = di;
    }
//	genstorage(sp->tp->size);
    
}
static void dumpDynamicInitializers(void)
{
#ifndef PARSER_ONLY
    STATEMENT *st = NULL, **stp = &st;
    while (dynamicInitializers)
    {
        EXPRESSION *exp = NULL;
        STATEMENT *stmt;
        stmt = stmtNode(NULL, NULL, st_expr);
        exp = convertInitToExpression(dynamicInitializers->init->basetp, dynamicInitializers->sp, NULL, dynamicInitializers->init, NULL);
        optimize_for_constants(&exp);
        stmt->select = exp;
        stmt->next = st;
        st = stmt;
        dynamicInitializers = dynamicInitializers->next;	
    }
    if (st)
    {
        SYMBOL *funcsp ;
        TYPE *tp = Alloc(sizeof(TYPE));
        tp->type = bt_ifunc;
        tp->btp = Alloc(sizeof(TYPE));
        tp->btp->type = bt_void;
        tp->syms = CreateHashTable(1);
        funcsp = makeID(sc_static, tp, NULL,"__DYNAMIC_STARTUP__");
        funcsp->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
        funcsp->inlineFunc.stmt->lower = st;
        SetLinkerNames(funcsp, lk_none);
        startlab = nextLabel++;
        retlab = nextLabel++;
        genfunc(funcsp);
        startlab = retlab = 0;
        startupseg();
        gensrref(funcsp, 32);
    }
#endif
}
static void dumpTLSInitializers(void)
{
#ifndef PARSER_ONLY
    if (TLSInitializers)
    {
        STATEMENT *st = NULL, **stp = &st;
        SYMBOL *funcsp ;
        TYPE *tp = Alloc(sizeof(TYPE));
        tp->type = bt_ifunc;
        tp->btp = Alloc(sizeof(TYPE));
        tp->btp->type = bt_void;
        tp->syms = CreateHashTable(1);
        funcsp = makeID(sc_static, tp, NULL,"__TLS_DYNAMIC_STARTUP__");
        funcsp->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
        funcsp->inlineFunc.stmt->lower = st;
        SetLinkerNames(funcsp, lk_none);
        while (TLSInitializers)
        {
            EXPRESSION *exp;
            STATEMENT *stmt;
            stmt = stmtNode(NULL, NULL, st_expr);
            exp = convertInitToExpression(TLSInitializers->init->basetp, TLSInitializers->sp, NULL, TLSInitializers->init, NULL);
            optimize_for_constants(&exp);
            stmt->select = exp;
            stmt->next = st;
            st = stmt;
            TLSInitializers = TLSInitializers->next;	
        }
        funcsp->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
        funcsp->inlineFunc.stmt->lower = st;
        startlab = nextLabel++;
        retlab = nextLabel++;
        genfunc(funcsp);
        startlab = retlab = 0;
        tlsstartupseg();
        gensrref(funcsp, 32);
    }
#endif
}
static void dumpDynamicDestructors(void)
{
#ifndef PARSER_ONLY
    STATEMENT *st = NULL, **stp = &st;
    while (dynamicDestructors)
    {
        EXPRESSION *exp = convertInitToExpression(dynamicDestructors->init->basetp, dynamicDestructors->sp, NULL, dynamicDestructors->init, NULL);
        *stp = stmtNode(NULL, NULL, st_expr);
        optimize_for_constants(&exp);
        (*stp)->select = exp;
        stp = &(*stp)->next;
        dynamicDestructors = dynamicDestructors->next;
    }
    if (st)
    {
        SYMBOL *funcsp ;
        TYPE *tp = Alloc(sizeof(TYPE));
        tp->type = bt_ifunc;
        tp->btp = Alloc(sizeof(TYPE));
        tp->btp->type = bt_void;
        tp->syms = CreateHashTable(1);
        funcsp = makeID(sc_static, tp, NULL,"__DYNAMIC_RUNDOWN__");
        funcsp->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
        funcsp->inlineFunc.stmt->lower = st;
        SetLinkerNames(funcsp, lk_none);
        startlab = nextLabel++;
        retlab = nextLabel++;
        genfunc(funcsp);
        startlab = retlab = 0;
        rundownseg();
        gensrref(funcsp, 32);
    }
#endif
}
static void dumpTLSDestructors(void)
{
#ifndef PARSER_ONLY
    if (TLSDestructors)
    {
        STATEMENT *st = NULL, **stp = &st;
        SYMBOL *funcsp ;
        TYPE *tp = Alloc(sizeof(TYPE));
        tp->type = bt_ifunc;
        tp->btp = Alloc(sizeof(TYPE));
        tp->btp->type = bt_void;
        tp->syms = CreateHashTable(1);
        funcsp = makeID(sc_static, tp, NULL,"__TLS_DYNAMIC_RUNDOWN__");
        funcsp->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
        funcsp->inlineFunc.stmt->lower = st;
        SetLinkerNames(funcsp, lk_none);
        while (TLSDestructors)
        {
            EXPRESSION *exp = convertInitToExpression(TLSDestructors->init->basetp, TLSDestructors->sp, NULL, TLSDestructors->init, NULL);
            *stp = stmtNode(NULL, NULL, st_expr);
            optimize_for_constants(&exp);
            (*stp)->select = exp;
            stp = &(*stp)->next;
            TLSDestructors = TLSDestructors->next;
        }

        funcsp->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
        funcsp->inlineFunc.stmt->lower = st;
        startlab = nextLabel++;
        retlab = nextLabel++;
        genfunc(funcsp);
        startlab = retlab = 0;
        tlsrundownseg();
        gensrref(funcsp, 32);
    }
#endif
}
int dumpMemberPtr(SYMBOL *sp, TYPE *membertp, BOOL make_label)
{
#ifndef PARSER_ONLY
    int lbl = 0;
    int vbase = 0, ofs;
    EXPRESSION expx, *exp = &expx;
    if (make_label)
    {
        // well if we wanted we could reuse existing structures, but,
        // it doesn't seem like the amount of duplicates there might be is
        // really worth the work.  Borland didn't think so anyway...
        dseg();
        lbl = nextLabel++;
        if (sp)
            sp->label = lbl;
        put_label(lbl);
    }
    if (!sp)
    {
        // null...
        if (isfunction(membertp->btp))
        {
            genaddress(0);
            genint(0);
            genint(0);
        }
        else
        {
            genint(0);
            genint(0);
        }
    }
    else
    {
        memset(&expx, 0, sizeof(expx));
        expx.type = en_c_i;
        exp = baseClassOffset(sp->parentClass, membertp->sp, exp);
        optimize_for_constants(&exp);
        if (isfunction(sp->tp))
        {
            if (sp->storage_class == sc_virtual)            
                genref(getvc1Thunk(sp->offset), 0);
            else
                genref(sp, 0);
            if (exp->type == en_add)
            {
                if (exp->left->type == en_l_p)
                {
                    genint(exp->right->v.i+1);
                    genint(exp->left->left->v.i + 1);
                }
                else
                {
                    genint(exp->left->v.i);
                    genint(exp->right->left->v.i + 1);
                }
            }
            else if (exp->type == en_l_p)
            {
                genint(0);
                genint(exp->left->v.i + 1);
            }
            else
            {
                genint(exp->v.i);
                genint(0);
            }
        }
        else
        {
            if (exp->type == en_add)
            {
                if (exp->left->type == en_l_p)
                {
                    genint(exp->right->v.i+sp->offset+1);
                    genint(exp->left->left->v.i + 1);
                }
                else
                {
                    genint(exp->left->v.i+sp->offset+1);
                    genint(exp->right->left->v.i + 1);
                }
            }
            else if (exp->type == en_l_p)
            {
                genint(1 + sp->offset);
                genint(exp->left->v.i + 1);
            }
            else
            {
                genint(exp->v.i+sp->offset+1);
                genint(0);
            }
        }
    }
    return lbl;
#endif
}
static int dumpInit(SYMBOL *sp, INITIALIZER *init)
{
#ifndef PARSER_ONLY
    TYPE *tp = basetype(init->basetp);
    int rv = getSize(tp->type);
    LLONG_TYPE i;
    FPF f, im;
    if (isfloatconst(init->exp))
    {
        f = init->exp->v.f;
        i = FPFToLongLong(&f);
        SetFPFZero(&im, 0);
            
    }
    else if (isintconst(init->exp))
    {
        i = init->exp->v.i;
        LongLongToFPF(&f, i);	
        SetFPFZero(&im, 0);
    }
    else if (isimaginaryconst(init->exp))
    {
        i = 0;
        SetFPFZero(&f, 0);
        im = init->exp->v.f;
    }
    else if (iscomplexconst(init->exp))
    {
        f = init->exp->v.c.r;
        im = init->exp->v.c.i;
        i = FPFToLongLong(&f);
    }
    else if (sp->linkage3 == lk_threadlocal)
    {
        if (cparams.prm_cplusplus)
            insertTLSInitializer(sp, init);
        else
            diag("dumpsym: unknown constant type");
    }
    else 
    {
        EXPRESSION *exp = init->exp;
        while (castvalue(exp))
            exp = exp->left;
        if (exp->type == en_func && !exp->v.func->ascall)
            exp = exp->v.func->fcall;
        if (!IsConstantExpression(exp, FALSE))
        {
            if (cparams.prm_cplusplus)
            {
                insertDynamicInitializer(sp, init);
                return 0;
            }
            else
                diag("dumpsym: unknown constant type");
        }
        else
        {
            switch(exp->type)
            {
                case en_pc:
                    genpcref(exp->v.sp, 0);
                    break;
                case en_global:
                    genref(exp->v.sp, 0);
                    break;
                case en_label:
                    gen_labref(exp->v.sp->label);
                    break;
                case en_labcon:
                    gen_labref(exp->v.i);
                    break;
                case en_memberptr:
                    dumpMemberPtr(exp->v.sp, tp, FALSE);
                    break;
                case en_add:
                case en_arrayadd:
                case en_structadd:
                {
                    EXPRESSION *ep1=NULL, *ep2=NULL;
                    if (isconstaddress(exp->left))
                    {
                        ep1 = exp->left;
                        ep2 = exp->right;
                    }
                    else if (isconstaddress(exp->right))
                    {
                        ep2 = exp->left;
                        ep1 = exp->right;
                    }
                    if (ep1 != NULL && ep2 != NULL)
                    {
                        while (castvalue(ep1))
                            ep1 = ep1->left;
                        while (castvalue(ep2))
                            ep2 = ep2->left;
                        if (ep1->type == en_pc)
                            genpcref(ep1->v.sp, ep2->v.i);
                        else
                            genref(ep1->v.sp, ep2->v.i);
                        break;
                    }
                }
                /* fall through */
                default:
                    if (isintconst(exp))
                    {
                        genint(exp->v.i);
                    }
                    else if (cparams.prm_cplusplus)
                    {
                        insertDynamicInitializer(sp, init);
                        return 0;
                    }
                    else
                    {
                        diag("unknown pointer type in initSym");
                        genaddress(0);
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
        if (rv < getSize(bt_pointer))
            return getSize(bt_pointer);
        else 
            return rv;
    }
        
    switch(tp->type == bt_enum ? tp->btp->type : tp->type)
    {
        case bt_bool:
            genbool(i);
            break;
        case bt_char:
        case bt_unsigned_char:
            genbyte(i);
            break;
        case bt_short:
        case bt_unsigned_short:
            genshort(i);
            break;
        case bt_wchar_t:
            genwchar_t(i);
            break;
        case bt_int:
        case bt_unsigned:
            genint(i);
            break;
        case bt_char16_t:
            genuint16(i);
            break;
        case bt_char32_t:
            genuint32(i);
            break;
        case bt_long:
        case bt_unsigned_long:
            genlong(i);
            break;
        case bt_long_long:
        case bt_unsigned_long_long:
            genlonglong(i);
            break;
        case bt_float:
            genfloat(&f);
            break;
        case bt_double:
            gendouble(&f);
            break;
        case bt_long_double:
            genlongdouble(&f);
            break;
        case bt_float_imaginary:
            genfloat(&im);
            break;
        case bt_double_imaginary:
            gendouble(&im);
            break;
        case bt_long_double_imaginary:
            genlongdouble(&im);
            break;
        case bt_float_complex:
            genfloat(&f);
            genfloat(&im);
            break;
        case bt_double_complex:
            gendouble(&f);
            gendouble(&im);
            break;
        case bt_long_double_complex:
            genlongdouble(&f);
            genlongdouble(&im);
            break;
        
        case bt_pointer:
            genaddress(i);
            break;
        case bt_far:
        case bt_seg:
        case bt_bit:
        default:
            diag("dumpInit: unknown type");
            break;
    }
#else
    int rv = 4;
#endif
    return rv;
}
BOOL IsConstWithArr(TYPE *tp)
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
void dumpInitializers(void)
{
#ifndef PARSER_ONLY
    int sconst = 0;
    int bss = 0;
    int data = 0;
    int thread = 0;
    int *sizep;
    dumpDynamicInitializers();
    dumpTLSInitializers();
    dumpDynamicDestructors();
    dumpTLSDestructors();
    dumpvc1Thunks();
    symListTail = symListHead;
    while (symListTail)
    {
        SYMBOL *sp = (SYMBOL *)symListTail->data;
         if (sp->storage_class == sc_global || sp->storage_class == sc_static
            || sp->storage_class == sc_localstatic || sp->storage_class == sc_constant)
        {
            TYPE *tp = sp->tp;
            TYPE *stp = tp;
            int al ;
            while (isarray(stp))
                stp = basetype(stp)->btp;
            if (IsConstWithArr(sp->tp) && !isvolatile(sp->tp))
            {
                xconstseg();
                sizep = &sconst;
            }
            else if (sp->linkage3 == lk_threadlocal)
            {
                tseg();
                sizep = &thread;
            }
            else if (sp->init || !cparams.prm_bss)
            {
                dseg();
                sizep = &data;
            }
            else
            {
                bssseg();
                sizep = &bss;
            }
            al = getAlign(sc_global, basetype(tp));
            if (*sizep % al)
            {
                int n = al - *sizep % al;
                genstorage(n);
                *sizep += n;
            }
            sp->offset = *sizep;
            *sizep += basetype(tp)->size;
            if (sp->storage_class == sc_global)
                globaldef(sp);
            else if (sp->storage_class == sc_static)
                localdef(sp);
            if (sp->storage_class == sc_localstatic || sp->storage_class == sc_constant)
                put_label(sp->label);
            else
                gen_strlab(sp);
            if (sp->init)
            {
                if (sp->tp->array || isstructured(tp))
                {
                    INITIALIZER *init = sp->init;
                    int pos = 0;
                    while (init)
                    {
                        if (pos != init->offset)
                        {
                            if (pos > init->offset)
                                diag("position error in dumpInitializers");
                            else
                                genstorage(init->offset - pos);
                            pos = init->offset;
                        }
                        if (init->basetp && basetype(init->basetp)->hasbits)
                        {
                            pos += dumpBits(&init);
                        }
                        else {
                            if (init->basetp && init->exp)
                            {
                                int s = dumpInit(sp, init);
                                if (s < init->basetp->size)
                                {
                                    
                                    genstorage(init->basetp->size - s);
                                    s = init->basetp->size;
                                }
                                    
                                pos += s;
                            }
                            init = init->next;
                        }
                    }
                }
                else
                {
                    int s = dumpInit(sp, sp->init);
                    if (s < sp->init->basetp->size)
                    {
                        
                        genstorage(sp->init->basetp->size - s);
                    }
                }
            }
            else
                genstorage(basetype(tp)->size);
        }
        symListTail = symListTail->next;
    }
#endif
}
void insertInitSym(SYMBOL *sp)
{
    if (!sp->indecltable)
    {
        LIST *lst = Alloc(sizeof(LIST));
        lst->data = sp;
        if (symListHead)
            symListTail = symListTail->next = lst;
        else
            symListHead = symListTail = lst;
        sp->indecltable = TRUE;
    }
}
INITIALIZER *initInsert(INITIALIZER **pos, TYPE *tp, EXPRESSION *exp, 
                               int offset, BOOL noassign)
{
    INITIALIZER *pos1 = Alloc(sizeof(INITIALIZER));

    pos1->basetp = tp;
    pos1->exp = exp;
    pos1->offset = offset;
    pos1->tag = inittag++;
    pos1->noassign = noassign;
    *pos = pos1;
    return pos1;
}
static LEXEME *initialize_bool_type(LEXEME *lex, SYMBOL *funcsp, int offset,
                                    enum e_sc sc, TYPE *itype, INITIALIZER **init)
{
    TYPE *tp;
    EXPRESSION *exp;
    BOOL needend = FALSE;
    if (MATCHKW(lex, begin))
    {
        needend = TRUE;
        lex = getsym();
    }
    if (cparams.prm_cplusplus && needend && MATCHKW(lex, end))
    {
        exp = intNode(en_c_bool, 0);
    }
    else
    {
        lex = optimized_expression(lex, funcsp, NULL, &tp, &exp, FALSE);
        if (!tp)
        {
            error(ERR_EXPRESSION_SYNTAX);
        }
        else
        {
            if (sc != sc_auto && sc != sc_register && !cparams.prm_cplusplus)
            {
                if (isstructured(tp))
                    error(ERR_ILL_STRUCTURE_ASSIGNMENT);
                else if (!isarithmeticconst(exp))
                    error(ERR_CONSTANT_VALUE_EXPECTED);
            }
        /*	exp = exprNode(en_not, exp, NULL);
            exp = exprNode(en_not, exp, NULL);
            */
            if (!comparetypes(itype, tp, TRUE))
            {
                cast(itype, &exp);
                optimize_for_constants(&exp);
            }
        }
    }
    initInsert(init, itype, exp, offset, FALSE);
    if (needend)
    {
        if (!needkw(&lex, end))
        {
            errskim(&lex, skim_end);
            skip(&lex, end);
        }
    }
    return lex;
}
static LEXEME *initialize_arithmetic_type(LEXEME *lex, SYMBOL *funcsp, int offset, enum e_sc sc, TYPE *itype, INITIALIZER **init)
{
    TYPE *tp = NULL;
    EXPRESSION *exp = NULL;
    BOOL needend = FALSE;
    if (MATCHKW(lex, begin))
    {
        needend = TRUE;
        lex = getsym();
    }
    if (cparams.prm_cplusplus && needend && MATCHKW(lex, end))
    {
        exp = intNode(en_c_i, 0);
        cast(itype, &exp);
    }
    else
    {
        lex = optimized_expression(lex, funcsp, NULL, &tp, &exp, FALSE);
        if (!tp)
        {
            error(ERR_EXPRESSION_SYNTAX);
        }
        else
        {
            if (cparams.prm_cplusplus && isarithmetic(itype) && isstructured(tp))
            {
                castToArithmetic(FALSE, &tp, &exp, (enum e_kw)-1, itype);
            }
            if (isstructured(tp))
                error(ERR_ILL_STRUCTURE_ASSIGNMENT);
            else if (ispointer(tp))
                error(ERR_NONPORTABLE_POINTER_CONVERSION);
            else if (!isarithmetic(tp) || sc != sc_auto && sc != sc_register &&
                     !isarithmeticconst(exp)&& !cparams.prm_cplusplus)
                error(ERR_CONSTANT_VALUE_EXPECTED);
            else 
                checkscope(tp, itype);
            if (!comparetypes(itype, tp, TRUE))
            {
                if (cparams.prm_cplusplus && basetype(tp)->type > bt_int)
                    if (basetype(itype)->type < basetype(tp)->type)        
                        error(ERR_INIT_NARROWING);
                cast(itype, &exp);
                optimize_for_constants(&exp);
            }
        }
    }
    initInsert(init, itype, exp, offset, FALSE);
    if (needend)
    {
        if (!needkw(&lex, end))
        {
            errskim(&lex, skim_end);
            skip(&lex, closebr);
        }
    }
    return lex;
}	
static LEXEME *initialize_string(LEXEME *lex, SYMBOL *funcsp, TYPE **rtype, EXPRESSION **exp)
{
    enum e_lexType tp;
    (void)funcsp;
    lex = concatStrings(lex, exp, &tp,0);
    switch (tp)
    {
        default:
        case l_astr:
            *rtype = &stdstring;
            break;
        case l_wstr:
            *rtype = &stdwcharptr;
            break;
        case l_ustr:        
            *rtype = &stdchar16tptr;
            break;
        case l_Ustr:        
            *rtype = &stdchar32tptr;
            break;
    }
    return lex;
}
static void refExp(EXPRESSION *exp)
{
    while (castvalue(exp))
        exp = exp->left;
    switch(exp->type)
    {
        case en_func:
            refExp(exp->v.func->fcall);
            break;
        case en_add:
        case en_arrayadd:
        case en_structadd:
            refExp(exp->left);
            refExp(exp->right);
            break;
        case en_global:
        case en_label:
        case en_pc:
        case en_threadlocal:
            exp->v.sp->genreffed = TRUE;
            break;
    }
}
static LEXEME *initialize_pointer_type(LEXEME *lex, SYMBOL *funcsp, int offset, enum e_sc sc, TYPE *itype, INITIALIZER **init)
{
    TYPE *tp;
    EXPRESSION *exp;
    BOOL needend = FALSE;
    if (MATCHKW(lex, begin))
    {
        needend = TRUE;
        lex = getsym();
    }
    if (cparams.prm_cplusplus && needend && MATCHKW(lex, end))
    {
        exp = intNode(en_c_i, 0);
    }
    else
    {
        if (!lex || lex->type != l_astr && lex->type != l_wstr && lex->type != l_ustr && lex->type != l_Ustr)
        {
            lex = optimized_expression(lex, funcsp, itype, &tp, &exp, FALSE);
            if (!tp)
            {
                error(ERR_EXPRESSION_SYNTAX);
            }
        }
        else
        {
            lex = initialize_string(lex, funcsp, &tp, &exp);
        }
        
        if (sc != sc_auto && sc != sc_register)
        {
            if (!isarithmeticconst(exp) && !isconstaddress(exp) && !cparams.prm_cplusplus)
                error(ERR_NEED_CONSTANT_OR_ADDRESS);
        }
        if (tp)
        {
            if (cparams.prm_cplusplus && isstructured(tp))
            {
                castToPointer(&tp, &exp, (enum e_kw)-1, itype);
            }
            if (isstructured(tp))
                error(ERR_ILL_STRUCTURE_ASSIGNMENT);
            else if (!ispointer(tp) && !isfunction(tp) && !isint(tp))
                error(ERR_INVALID_POINTER_CONVERSION);
            else if (isint(tp) && !isconstzero(tp, exp))
                error(ERR_NONPORTABLE_POINTER_CONVERSION);
            else if ((ispointer(tp) || isfunction(tp))&& !comparetypes(itype, tp, TRUE))
                if (!isvoidptr(tp) && !isvoidptr(itype))
                    error(ERR_SUSPICIOUS_POINTER_CONVERSION);
            /* might want other types of conversion checks */
            if (!comparetypes(itype, tp, TRUE) && !isint(tp))
                cast(tp, &exp);
        }
    }
    initInsert(init, itype, exp, offset, FALSE);
    refExp(exp);
    if (needend)
    {
        if (!needkw(&lex, end))
        {
            errskim(&lex, skim_end);
            skip(&lex, closebr);
        }
    }
    return lex;
}
static LEXEME *initialize_memberptr(LEXEME *lex, SYMBOL *funcsp, int offset,
                                     enum e_sc sc, TYPE *itype, INITIALIZER **init)
{
    TYPE *tp = NULL;
    EXPRESSION *exp = NULL;
    BOOL needend = FALSE;
    if (MATCHKW(lex, begin))
    {
        needend = TRUE;
        lex = getsym();
    }
    if (cparams.prm_cplusplus && needend && MATCHKW(lex, end))
    {
        exp = intNode(en_memberptr, 0); // no SP means fill it with zeros...
    }
    else
    {
        lex = optimized_expression(lex, funcsp, NULL, &tp, &exp, FALSE);
        if (!isconstzero(tp, exp) && exp->type != en_nullptr)
        {
            if (exp->type == en_memberptr)
            {
                if (classRefCount(exp->v.sp->parentClass, basetype(itype)->sp) != 1)
                    error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
    
            }
            else 
            {
                if (cparams.prm_cplusplus && isstructured(tp))
                {
                    castToPointer(&tp, &exp, (enum e_kw)-1, itype);
                }
                if (!comparetypes(itype, tp, TRUE))
                {
                    errortype(ERR_CANNOT_CONVERT_TYPE, tp, itype);
                }
            }
        }
        else
        {
            exp = intNode(en_memberptr, 0); // no SP means fill it with zeros...
        }
    }
    initInsert(init, itype, exp, offset, FALSE);
    if (needend)
    {
        if (!needkw(&lex, end))
        {
            errskim(&lex, skim_end);
            skip(&lex, closebr);
        }
    }
    return lex;
}
enum e_node referenceTypeError(TYPE *tp, EXPRESSION *exp)
{
    enum e_node en = en_void;
    tp = basetype(basetype(tp)->btp);
    switch (tp->type == bt_enum ? tp->btp->type : tp->type)
    {
        case bt_lref: /* only used during initialization */
        case bt_rref: /* only used during initialization */
            en = en_l_ref;
            break;
        case bt_bit:
            en = -1;
            break;
        case bt_bool:
            en = en_l_bool;
            break;
        case bt_char:
            en = en_l_c;
            break;
        case bt_unsigned_char:
            en = en_l_uc;
            break;
        case bt_short:
            en = en_l_s;
            break;
        case bt_unsigned_short:
            en = en_l_us;
            break;
        case bt_wchar_t:
            en = en_l_wc;
            break;
        case bt_int:
            en = en_l_i;
            break;
        case bt_unsigned:
            en = en_l_ui;
            break;
        case bt_char16_t:
            en = en_l_u16;
            break;
        case bt_char32_t:
            en = en_l_u32;
            break;
        case bt_long:
            en = en_l_l;
            break;
        case bt_unsigned_long:
            en = en_l_ul;
            break;
        case bt_long_long:
            en = en_l_ll;
            break;
        case bt_unsigned_long_long:
            en = en_l_ull;
            break;
        case bt_float:
            en = en_l_f;
            break;
        case bt_double:
            en = en_l_d;
            break;
        case bt_long_double:
            en = en_l_ld;
            break;
        case bt_float_complex:
            en = en_l_fc;
            break;
        case bt_double_complex:
            en = en_l_dc;
            break;
        case bt_long_double_complex:
            en = en_l_ldc;
            break;
        case bt_float_imaginary:
            en = en_l_fi;
            break;
        case bt_double_imaginary:
            en = en_l_di;
            break;
        case bt_long_double_imaginary:
            en = en_l_ldi;
            break;
        case bt_pointer:
            if (tp->array || tp->vla)
                return en_void;
            en = en_l_p;
            break;
        case bt_struct:
        case bt_union:
        case bt_func:
        case bt_ifunc:
            return en_l_p;
        default:
            diag("ref reference error");
            break;
    }
    return en;		
        
}
EXPRESSION *createTemporary(TYPE *tp, EXPRESSION *val)
{
    SYMBOL *var = anonymousVar(sc_auto, tp);
    EXPRESSION *rv = varNode(en_auto, var);
    tp = basetype(tp)->btp;
    if (val)
    {
        EXPRESSION *rv1 = rv;
        deref(tp, &rv);
        cast(tp, &val);
        rv = exprNode(en_void, exprNode(en_assign, rv, val), rv1) ;
    }
    errortype(ERR_CREATE_TEMPORARY, tp, tp);
    return rv;
}
static LEXEME *initialize_reference_type(LEXEME *lex, SYMBOL *funcsp, int offset, enum e_sc sc, TYPE *itype, INITIALIZER **init)
{
    TYPE *tp;
    EXPRESSION *exp;
    BOOL needend = FALSE;
    (void)sc;
    if (MATCHKW(lex, begin))
    {
        needend = TRUE;
        lex = getsym();
    }
    lex = optimized_expression(lex, funcsp, NULL, &tp, &exp, FALSE);
    if (!tp)
        error(ERR_EXPRESSION_SYNTAX);
    if (tp)
    {
        if ((!isarithmetic(basetype(itype)->btp) || !isarithmetic(tp)) && !comparetypes(itype, tp, TRUE))
            errortype(ERR_REF_INIT_TYPE_REQUIRES_LVALUE_OF_TYPE, itype->btp, itype->btp);
        else if (isconst(tp) && !isconst(basetype(itype)->btp) || isvolatile(tp) && !isvolatile(basetype(itype)->btp))
            error(ERR_REF_INITIALIZATION_DISCARDS_QUALIFIERS);
        else if (!isstructured(tp) && !isfunction(tp) && (!ispointer(tp) || !tp->array))
            if (!lvalue(exp))
            {
                if (basetype(itype)->type == bt_lref && !isarithmeticconst(exp))
                {
                    errortype(ERR_REF_INITIALIZATION_CANNOT_USE_RVALUE_OF_TYPE, tp, tp);
                }
                if (!basetype(tp)->array)
                    if (itype->type != bt_rref || exp->type != en_not_lvalue || !lvalue(exp->left))
                    {
                        error(ERR_REF_INIT_REQUIRES_LVALUE);
                    }
                    else
                    {
                        exp = exp->left;
                        while (castvalue(exp))
                            exp = exp->left;
                        if (referenceTypeError(itype, exp) != exp->type)
                            errortype(ERR_REF_INIT_TYPE_REQUIRES_LVALUE_OF_TYPE, tp, tp);
                        if (lvalue(exp))
                            exp = exp->left;
                    }
            }
            else
            {
                if (referenceTypeError(itype, exp) != exp->type)
                {
                    exp = createTemporary(itype, exp);
                }
                while (castvalue(exp))
                    exp = exp->left;
                if (lvalue(exp))
                    exp = exp->left;
                
            }
    }
    initInsert(init, itype, exp, offset, FALSE);
    if (needend)
    {
        if (!needkw(&lex, end))
        {
            errskim(&lex, skim_end);
            skip(&lex, closebr);
        }
    }
    return lex;
}
typedef struct _aggregate_descriptor {
    struct _aggregate_descriptor *next;
    int stopgap : 1;
    int offset ;
    int reloffset;
    int max;
    HASHREC *hr;
    TYPE *tp;
} AGGREGATE_DESCRIPTOR ;

static void free_desc(AGGREGATE_DESCRIPTOR **descin, AGGREGATE_DESCRIPTOR **cache)
{
    if (*descin)
    {
        AGGREGATE_DESCRIPTOR *temp = *descin;
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
static void unwrap_desc(AGGREGATE_DESCRIPTOR **descin, AGGREGATE_DESCRIPTOR **cache)
{
    while (*descin && !(*descin)->stopgap)
    {
        free_desc(descin, cache);
    }
}
static void allocate_desc(TYPE *tp, int offset,
                   AGGREGATE_DESCRIPTOR **descin, AGGREGATE_DESCRIPTOR **cache)
{
    AGGREGATE_DESCRIPTOR **temp = cache, *desc = NULL;
    
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
        desc = Alloc(sizeof(AGGREGATE_DESCRIPTOR));
        desc->tp = tp;
        desc->offset = offset;
    }
    desc->reloffset = 0;
    if (isstructured(tp))
        desc->hr = basetype(tp)->syms->table[0];
    desc->next = *descin;
    desc->stopgap = FALSE;
    *descin = desc;
}	
static enum e_bt str_candidate(LEXEME *lex, TYPE *tp)
{
    TYPE *bt;
    bt = basetype(tp);
    if (bt->type == bt_pointer)
        if (lex->type == l_astr || lex->type == l_wstr || lex->type == l_ustr || lex->type == l_Ustr)
        {
            bt = basetype(bt->btp);
            if ( bt->type == bt_short || bt->type == bt_unsigned_short || bt->type == bt_wchar_t 
                || bt->type == bt_char || bt->type == bt_unsigned_char
                || bt->type == bt_char16_t || bt->type == bt_char32_t)
                return bt->type;
        }
    return 0;
}
static BOOL designator(LEXEME **lex, SYMBOL *funcsp, AGGREGATE_DESCRIPTOR **desc, AGGREGATE_DESCRIPTOR **cache)
{

    if (MATCHKW(*lex, openbr) || MATCHKW(*lex, dot))
    {
        BOOL done = FALSE;
        unwrap_desc(desc, cache);
        (*desc)->reloffset = 0;
        while (!done && (MATCHKW(*lex, openbr) || MATCHKW(*lex, dot)))
        {
            if (MATCHKW(*lex, openbr))
            {
                TYPE *tp=NULL;
                EXPRESSION *enode=NULL;
                int index;
                *lex = getsym();
                *lex = optimized_expression(*lex, funcsp, NULL, &tp, &enode, FALSE);
                needkw(lex, closebr);
                if (!tp)
                    error(ERR_EXPRESSION_SYNTAX);
                else if (!isint(tp))
                    error(ERR_NEED_INTEGER_TYPE);
                else if (!isarithmeticconst(enode) && !cparams.prm_cplusplus)
                    error(ERR_CONSTANT_VALUE_EXPECTED);
                else if (isstructured((*desc)->tp) || !basetype((*desc)->tp)->array)
                    error(ERR_ARRAY_EXPECTED);
                else
                {
                    index = enode->v.i;
                    tp = basetype((*desc)->tp);
                    if (tp->size)
                    {
                        if ( index >= tp->size / tp->btp->size)
                        {
                            error(ERR_ARRAY_INDEX_OUT_OF_RANGE);
                            index = 0;
                        }
                    }
                    tp = tp->btp;
                    (*desc)->reloffset = index * tp->size;
                    if (isarray(tp) || isstructured(tp))
                        allocate_desc(tp, (*desc)->reloffset + (*desc)->offset, desc, cache);
                    else 
                        done = TRUE;
                }				
            }
            else 
            {
                *lex = getsym();
                if (ISID(*lex))
                {
                    if (isstructured((*desc)->tp))
                    {
                        HASHREC *hr2 = basetype((*desc)->tp)->syms->table[0];
                        while (hr2 && strcmp(((SYMBOL *)(hr2->p))->name, (*lex)->value.s.a))
                        {
                            hr2 = hr2->next;
                        }
                        if (hr2)
                        {
                            SYMBOL *sym = (SYMBOL *)hr2->p;
                            TYPE *tp = sym->tp;
                            (*desc)->reloffset = sym->offset;
                            (*desc)->hr = hr2;
                            if (isarray(tp) || isstructured(tp))
                                allocate_desc(tp, (*desc)->reloffset + (*desc)->offset, desc, cache);
                            else
                                done = TRUE;
                        }
                        else
                            error(ERR_STRUCTURED_TYPE_EXPECTED);
                    }
                    else
                    {
                        membererror((*lex)->value.s.a,(*desc)->tp);
                    }
                    *lex = getsym();
                }
                else
                {
                    error(ERR_IDENTIFIER_EXPECTED);
                }
            }
        }
        needkw(lex, assign);
        return TRUE;
    }
    return FALSE;
}
static BOOL atend(AGGREGATE_DESCRIPTOR *desc)
{
    if (isstructured(desc->tp))
            return !desc->hr;
    else
        return desc->tp->size && desc->reloffset >= desc->tp->size;
}
static void increment_desc(AGGREGATE_DESCRIPTOR **desc, AGGREGATE_DESCRIPTOR **cache)
{
    while (1)
    {
        if (isstructured((*desc)->tp))
        {
            int offset = (*desc)->reloffset;
            if (isunion((*desc)->tp))
                (*desc)->hr = NULL;
            else while (TRUE)
            {
                (*desc)->hr = (*desc)->hr->next;
                if (!(*desc)->hr)
                    break;
                if (((SYMBOL *)((*desc)->hr->p))->tp->hasbits)
                {
                    if (!((SYMBOL *)((*desc)->hr->p))->anonymous)
                        break;
                }
                else if (((SYMBOL *)((*desc)->hr->p))->offset != offset)
                {
                    break;
                }
            }
            if ((*desc)->hr)
                (*desc)->reloffset = ((SYMBOL *)((*desc)->hr->p))->offset;
            else
                (*desc)->reloffset = (*desc)->tp->size;
            if (atend(*desc) && !(*desc)->stopgap)
            {
                free_desc(desc, cache);
            }
            else
                break;
        }
        else // array element
        {
            TYPE *tp = (*desc)->tp;
            (*desc)->reloffset += basetype(tp)->btp->size + basetype(tp)->btp->arraySkew;
            if (atend(*desc) && !(*desc)->stopgap)
            {
                free_desc(desc, cache);
            }
            else
                break;
        }
    }
}
static int ascomp(const void*left, const void*right)
{
    INITIALIZER **lleft = left;
    INITIALIZER **rright = right;
    int lofs = ((*lleft)->offset << 7) + basetype((*lleft)->basetp)->startbit;
    int rofs = ((*rright)->offset << 7) + basetype((*rright)->basetp)->startbit;
    if (lofs < rofs)
        return -1;
    return lofs != rofs;
}
static void quicksort(const void *base, int lo, int hi, size_t width,
                        int (*compare)(const void *elem1, const void *elem2), char *buf)
{

    if (hi > lo) {
        int i=lo, j=hi;
        char * celem = buf + width;
        memcpy(celem,((char *)base) + width * ((lo + hi) / 2),width) ;

        while (i <= j)
        {
            while (i <= hi && (*compare)((char *)base + i * width, celem) <0) i++; 
            while (j >= lo && (*compare)((char *)base + j * width, celem) >0) j--; 
            if (i<=j)
            {
                memcpy(buf, (char *)base + i * width, width) ;
                memcpy((char *)base + i * width, (char *)base + j * width, width) ;
                memcpy((char *)base + j * width, buf, width) ;
                i++; j--;
            }
        }
        if (lo<j) quicksort(base, lo, j,width,compare,buf);
        if (i<hi) quicksort(base, i, hi,width,compare,buf);
    } 

}
static void qxsort(void *base, size_t num, size_t width,
                        int (*compare)(const void *elem1, const void *elem2))
{
    char data[512];
    char *buf ;

    if (width <= sizeof(data)/2)
        buf = data;
    else {
        buf = (char *)malloc(width) ;
        if (!buf)
            return ;
    }
    
    quicksort(base,0,num-1,width, compare,buf) ;

    if (buf != data)
        free(buf) ;
}
static INITIALIZER *sort_aggregate_initializers(INITIALIZER *data)
{
    INITIALIZER **left = &data, **right;
    int items = 1;
    BOOL sortit = FALSE;
    int offsetLeft;
    if (!data)
        return data;
    offsetLeft = ((*left)->offset << 7) + basetype((*left)->basetp)->startbit;
    /* check to see if already in order) */
    /* normally it will already be in order, the only time it won't is for the
     * new C99 declarators in arrays and structs
     */
    while (*left && (*left)->next)
    {
        int offsetRight ;
        right = &(*left)->next;
        offsetRight = ((*right)->offset << 7) + basetype((*right)->basetp)->startbit;
        if (offsetRight < offsetLeft)
        {
            sortit = TRUE;
        }
        offsetLeft = offsetRight;
        left = right;
        items++;
    }
    if (sortit)
    {
        int i;
        // this uses the qxsort routine out of the RTL, but it is
        // embedded in the compiler since it is non-standard.
        // a lot of this code does runup and rundown to get things in the 
        // right format for qxsort...
        INITIALIZER **base = localAlloc(sizeof(INITIALIZER *) * items);
        INITIALIZER *find = data;
        INITIALIZER **rv = &data;
        
        for (i=0; i < items; i++)
        {
            base[i] = find;
            find = find->next;
        }
        qxsort(base, items, sizeof(INITIALIZER *), ascomp);
        for (i=0; i < items; i++)
        {
            *rv = base[i];
            rv = &(*rv)->next;
        }
        *rv = NULL;
    }
    /* trim duplicates - highest tag value indicated for a given offset/startbit pair wins */
    left = &data;
    while (*left)
    {
        INITIALIZER **hold = NULL;
        right = &(*left)->next;
        while (*right && (*left)->offset == (*right)->offset && 
               basetype((*left)->basetp)->startbit == basetype((*right)->basetp)->startbit)
        {
            if (!hold)
                hold = left;
            if ((*right)->tag < (*hold)->tag)
                hold = right;
            right = &(*right)->next;
        }
        if (hold)
        {
            *hold = (*hold)->next;
        }
        else
            left = &(*left)->next;
    }
    return data;
}
static void set_array_sizes(AGGREGATE_DESCRIPTOR *cache)
{
    while(cache)
    {
        if (!isstructured(cache->tp))
        {
            if (cache->tp->size == 0)
            {
                int size = cache->max;
                int base = basetype(cache->tp)->btp->size;
                TYPE *temp = cache->tp;				
                size = size + base - 1;
                size = (size / base) * base;
                while (temp && temp->size == 0)
                {
                    temp->size = size;
                    temp = temp->btp;
                }
            }
        }
        cache = cache->next;
    }
}
static LEXEME *read_strings(LEXEME *lex, INITIALIZER **next,
                                 AGGREGATE_DESCRIPTOR **desc)
{
    TYPE *tp = basetype((*desc)->tp);
    TYPE *btp = basetype(tp->btp);
    int max = tp->size / btp->size;
    int j;
    EXPRESSION *expr;
    STRING *string = NULL;
    int index = 0;
    int i;
    /* this assumes the sizeof(short) & sizeof(wchar_t) < 16 */
    if (max == 0)
        max = INT_MAX/16;
    lex = concatStringsInternal(lex, &string,0);
    switch(string->strtype)
    {
        case l_astr:
            if (btp->type != bt_char && btp->type != bt_unsigned_char)
                error(ERR_STRING_TYPE_MISMATCH_IN_INITIALIZATION);
            break;
        case l_wstr:
            if (btp->type != bt_wchar_t && btp->type != bt_short && btp->type != bt_unsigned_short)
                error(ERR_STRING_TYPE_MISMATCH_IN_INITIALIZATION);
            break;
        case l_ustr:
            if (btp->type != bt_char16_t)
                error(ERR_STRING_TYPE_MISMATCH_IN_INITIALIZATION);
            break;
        case l_Ustr:
            if (btp->type != bt_char32_t)
                error(ERR_STRING_TYPE_MISMATCH_IN_INITIALIZATION);
            break;
    }
    for (j=0; j < string->size; j++)
    {
        int len = string->pointers[j]->count;
        if (len + index > max)
        {
            error(ERR_TOO_MANY_INITIALIZERS);
        }
        else
        {
            int i;
            for (i=0; i < len; i++)
            {
                EXPRESSION *exp = intNode(en_c_i, string->pointers[j]->str[i]);
                initInsert(next, btp, exp, (*desc)->offset + (*desc)->reloffset, FALSE); /* NULL=no initializer */
                (*desc)->reloffset += btp->size;
                next = &(*next)->next;
                index++;
            }
        }
    }
    if (max == INT_MAX/16)
    {
        max = (*desc)->reloffset/btp->size + 1;
    }
    for (i=(*desc)->reloffset/btp->size; i < max; i++)
    {
        EXPRESSION *exp;
        if (i == index)
            exp  = intNode(en_c_i, 0);
        else
            exp = NULL;
        initInsert(next, btp, exp, (*desc)->offset + (*desc)->reloffset, FALSE); /* NULL=no initializer */
        (*desc)->reloffset += btp->size;
        next = &(*next)->next;
        index++;
    }
    if ((*desc)->reloffset < max * btp->size)
    {
        EXPRESSION *exp = intNode(en_c_i, 0);
        
        initInsert(next, btp, exp, (*desc)->offset + (*desc)->reloffset, FALSE); /* NULL=no initializer */
        (*desc)->reloffset += btp->size;
    }
    return lex;
}
static TYPE *nexttp(AGGREGATE_DESCRIPTOR *desc)
{
    TYPE *rv;
    if (isstructured(desc->tp))
    {
        if (!cparams.prm_cplusplus  || desc->tp->sp->trivialCons)
        {
            HASHREC *hr = desc->hr;
            if (!hr)
                return NULL;
            rv = ((SYMBOL *)hr->p)->tp;
        }
        else
        {
            rv = desc->tp;
        }
    }
    else
        rv = basetype(desc->tp)->btp;
    return rv;
}
static LEXEME *initialize_auto_struct(LEXEME *lex, SYMBOL *funcsp, int offset,
                                      TYPE *itype, INITIALIZER **init)
{
    EXPRESSION *expr = NULL ;
    TYPE *tp = NULL;
    lex = expression_assign(lex, funcsp, NULL, &tp, &expr, FALSE);
    if (!tp || !lex)
    {
        error(ERR_EXPRESSION_SYNTAX);
    }
    else if (!comparetypes(itype, tp, TRUE))
    {
        error(ERR_ILL_STRUCTURE_ASSIGNMENT);
    }
    else
    {
        initInsert(init, itype, expr, offset, FALSE);
    }
    return lex;
}
EXPRESSION *getVarNode(SYMBOL *sp)
{
    EXPRESSION *exp;
    switch (sp->storage_class)
    {
        case sc_member:
            exp = exprNode(en_add, varNode(en_thisshim, NULL), intNode(en_c_i, sp->offset));
            break;            
        case sc_auto:
        case sc_parameter:
        case sc_register:	/* register variables are treated as 
                             * auto variables in this compiler
                             * of course the usage restraints of the
                             * register keyword are enforced elsewhere
                             */
            exp = varNode(en_auto, sp);
            break;
        
        case sc_localstatic:
            if (sp->linkage3 == lk_threadlocal)
                exp = varNode(en_threadlocal, sp);
            else
                exp = varNode(en_label, sp);
            break;
        case sc_absolute:
            exp = varNode(en_absolute, sp);
            break;				
        case sc_static:
        case sc_global:
        case sc_external:
            if (sp->linkage3 == lk_threadlocal)
                exp = varNode(en_threadlocal, sp);
            else
                exp = varNode(en_global, sp);
            break;
        default:
            diag("getVarNode: unknown storage class");
            exp = intNode(en_c_i, 0);
            break;
    }
    return exp;
}
static LEXEME *initialize_aggregate_type(LEXEME *lex, SYMBOL *funcsp, SYMBOL *base, int offset,
                                     enum e_sc sc, TYPE *itype, INITIALIZER **init, INITIALIZER **dest, BOOL arrayMember)
{
    INITIALIZER *data = NULL, **next=&data;
    AGGREGATE_DESCRIPTOR *desc= NULL, *cache = NULL;
    BOOL c99 = FALSE;
    BOOL toomany = FALSE;
    BOOL needend = FALSE;
    BOOL assn = FALSE;
    allocate_desc(itype, offset, &desc, &cache);
    desc->stopgap = TRUE;
    if (MATCHKW(lex, assign))
    {
        assn = TRUE;
        lex = getsym();
    }
    if (cparams.prm_cplusplus && isstructured(itype) && (!basetype(itype)->sp->trivialCons || 
             arrayMember))
    {
        // initialization via constructor
        FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
        EXPRESSION *baseexp,*exp;
        TYPE *ctype = itype;
        INITIALIZER *it = NULL;
        BOOL maybeConversion = TRUE;
        BOOL isconversion;
        exp = baseexp= exprNode(en_add, getVarNode(base), intNode(en_c_i, offset));
        if (assn || arrayMember)
        {
            // assignments or array members come here
            if (startOfType(lex))
            {
                TYPE *tp1 = NULL;
                EXPRESSION *exp1;
                lex = optimized_expression(lex, funcsp, NULL, &tp1, &exp1, FALSE);
                if (tp1->type == bt_auto)
                    tp1 = itype;
                if (!tp1 || !comparetypes(basetype(tp1), basetype(itype), TRUE))
                {
                    error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                    errskim(&lex, skim_semi);
                    return lex;
                }
                else
                {
                    funcparams->arguments = Alloc(sizeof(INITLIST));
                    funcparams->arguments->tp = tp1;
                    funcparams->arguments->exp = exp1;
                    maybeConversion = FALSE;
                }
            }
            else
            {
                if (MATCHKW(lex, begin))
                {
                    lex = getArgs(lex, funcsp, funcparams, end);
                    maybeConversion = FALSE;
                }
                else
                {
                    // shortcut for conversion from single expression
                    EXPRESSION *exp1 = NULL;
                    TYPE *tp1 = NULL;
                    lex = optimized_expression(lex, funcsp, NULL, &tp1, &exp1, FALSE);
                    funcparams->arguments = Alloc(sizeof(INITLIST));
                    funcparams->arguments->tp = tp1;
                    funcparams->arguments->exp = exp1;
                    maybeConversion = FALSE;
                }
            }
        }
        else if (MATCHKW(lex, openpa) || MATCHKW(lex, begin))
        {
            // conversion constructor params
            lex = getArgs(lex, funcsp, funcparams,MATCHKW(lex, openpa) ? closepa : end);
        }
        else
        {
            // default constructor without param list
        }
        callConstructor(&ctype, &exp, funcparams, FALSE, NULL, TRUE, maybeConversion); 
        initInsert(&it, itype, exp, offset, TRUE);
        if (sc != sc_auto && sc != sc_member && !arrayMember)
        {
            insertDynamicInitializer(base, it);
        }
        else
        {
            *init = it;
        }
        exp = baseexp;
        callDestructor(basetype(itype)->sp, &exp, NULL, TRUE);
        initInsert(&it, itype, exp, offset, TRUE);
        if (sc != sc_auto && sc != sc_member && !arrayMember)
        {
            insertDynamicDestructor(base, it);
        }
        else
        {
            *dest = it;
        }
        return lex;
    }
    // if we get here it is an array or a trivial structure
    if (!lex || MATCHKW(lex, begin) || !str_candidate(lex, itype))
    {
        if (cparams.prm_cplusplus && !MATCHKW(lex, begin))
        {
            TYPE *tp1 = NULL;
            EXPRESSION *exp1 = NULL;
            lex = expression_no_comma(lex, funcsp, NULL, &tp1, &exp1);
            if (!tp1)
                error(ERR_EXPRESSION_SYNTAX);
            else if (!comparetypes(itype, tp1, TRUE))
                error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                
            if (exp1)
            {
                INITIALIZER *it = NULL;
                initInsert(&it, itype, exp1, offset, FALSE);
                if (sc != sc_auto && sc != sc_member && !arrayMember)
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
        else if (needkw(&lex, begin))
        {
            needend = TRUE;
        }
    }
    if (cparams.prm_cplusplus && needend && MATCHKW(lex, end))
    {
        // empty braces
        lex = getsym();
        initInsert(init, NULL, NULL, itype->size, FALSE);
    }
    else
    {
        while (lex)
        {
            BOOL gotcomma = FALSE;
            TYPE *tp2;
            c99 |= designator(&lex, funcsp, &desc, &cache);
            tp2 = nexttp(desc);
            
            while (tp2 && (isarray(tp2) || isstructured(tp2) && (!cparams.prm_cplusplus || basetype(tp2)->sp->trivialCons)))
            {
                if (MATCHKW(lex, begin))
                {
                    lex = getsym();
                    allocate_desc(tp2, desc->offset + desc->reloffset, 
                              &desc, &cache);
                    desc->stopgap = TRUE;
                    c99 |= designator(&lex, funcsp, &desc, &cache);
                }
                else
                {
                    if (!atend(desc))
                        allocate_desc(tp2, desc->offset + desc->reloffset, 
                                  &desc, &cache);
                    else
                        break;
                }
                tp2 = nexttp(desc);
            }
            if (atend(desc))
            {
                toomany = TRUE;
                break;
            }
            /* when we get here, DESC has an aggregate with an element that isn't
             * an aggregate
             */
            if (str_candidate(lex, desc->tp) != 0)
            {
                lex = read_strings(lex, next, &desc);
            }
            else
            {
                lex = initType(lex, funcsp, desc->offset + desc->reloffset,
                               sc, next, dest, nexttp(desc), base, isarray(itype));
            }
            increment_desc(&desc, &cache);
            while (*next)
                next = &(*next)->next;
            if ((sc != sc_auto && sc != sc_register || needend) && 
                MATCHKW(lex, comma) || MATCHKW(lex, end))
            {
                gotcomma = MATCHKW(lex, comma);
                if (gotcomma && needend)
                    lex = getsym();
                while (MATCHKW(lex, end))
                {
                    gotcomma = FALSE;
                    lex = getsym();
                    unwrap_desc(&desc, &cache);
                    free_desc(&desc, &cache);
                    if (!desc)
                    {
                        if (!needend)
                        {
                            error(ERR_DECLARE_SYNTAX); /* extra end */
                        }
                        break;
                    }
                    increment_desc(&desc, &cache);
                    if (MATCHKW(lex, comma))
                    {
                        gotcomma = TRUE;
                        lex = getsym();
                    }
                }
                if (!desc)
                    break;
            }
            if (!desc || !gotcomma || !needend)
                break;
        }
        if (c99 && !cparams.prm_c99)
            error(ERR_C99_STYLE_INITIALIZATION_USED);
        if (toomany)
            error(ERR_TOO_MANY_INITIALIZERS);
        if (desc)
        {
            unwrap_desc(&desc, &cache);
            if (needend || desc->next)
            {
                error(ERR_DECLARE_SYNTAX);
                errskim(&lex, skim_semi);
            }
        }
        /* theoretically desc will be NULL if there are no errors */
        while (desc)
        {
            unwrap_desc(&desc, &cache);
            free_desc(&desc, &cache);
        }
        set_array_sizes(cache);
        
        *init = data = sort_aggregate_initializers(data);
    }
    // have to fill in unused array elements with C++ constructors
    // this doesn't play well with the designator stuff but doesn't matter in C++
    if (cparams.prm_cplusplus && isarray(itype) && !arrayMember)
    {
        TYPE *btp = itype;
        while (isarray(btp))
            btp = basetype(btp)->btp;
        btp = basetype(btp);
        if (isstructured(btp) && !btp->sp->trivialCons)
        {
            int s = (btp->size + btp->arraySkew);
            INITIALIZER *test = *init;
            INITIALIZER *testd = *dest;
            INITIALIZER *first = NULL, **push = &first;
            int last = 0, i;
            for ( ;test || last < itype->size; )
            {
                if (test && last < test->offset || !test && last < itype->size)
                {
                    int n = (test ? test->offset - last : itype->size - last) / s;
                    EXPRESSION *sz = NULL;
                    TYPE *ctype = btp, *tn = btp;
                    EXPRESSION *exp ;
                    if (last)
                        exp = exprNode(en_add, getVarNode(base), intNode(en_c_i, last));
                    else
                        exp = getVarNode(base);
                    if (n > 1)
                    {
                        sz = intNode(en_c_i, n);
                        tn = Alloc(sizeof(TYPE));
                        tn->array = TRUE;
                        tn->type = bt_pointer;
                        tn->size = n * s;
                        tn->btp = btp;
                    }
                    callConstructor(&ctype, &exp, NULL, TRUE, sz, TRUE, FALSE);
                    initInsert(push, tn, exp, last, TRUE);
                    push = &(*push)->next;
                    last += n * s;
                }
                if (test)
                {
                    *push = test;
                    test = test->next;
                    push = &(*push)->next;
                    *push = NULL;
                    last += s;
                }
            }
            if (sc != sc_auto && sc != sc_member)
            {
                *init = NULL;
                insertDynamicInitializer(base, first);
            }
            else
            {
                *init = first;
            }
            first = NULL, push = &first;
            {
                int n = itype->size / s;
                EXPRESSION *exp;
                EXPRESSION *sz = NULL;
                TYPE *tn = btp;
                exp = getVarNode(base);
                if (n > 1)
                {
                    sz = intNode(en_c_i, n);
                    tn = Alloc(sizeof(TYPE));
                    tn->array = TRUE;
                    tn->type = bt_pointer;
                    tn->size = n * s;
                    tn->btp = btp;
                }
                callDestructor(btp->sp, &exp, sz, TRUE);
                initInsert(push, tn, exp, last, FALSE);
            }
            if (sc != sc_auto && sc != sc_member)
            {
                insertDynamicDestructor(base, first);
                *dest = NULL;
            }
            else
            {
                *dest = first;
            }
        }
    }   
    return lex;
}
static LEXEME *initialize_bit(LEXEME *lex, SYMBOL *funcsp, int offset, enum e_sc sc, TYPE *itype, INITIALIZER **init)
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
static LEXEME *initialize_auto(LEXEME *lex, SYMBOL *funcsp, int offset,
                                     enum e_sc sc, TYPE *itype, 
                                     INITIALIZER **init, INITIALIZER **dest, SYMBOL *sp)
{
    TYPE *tp;
    EXPRESSION *exp;
    BOOL needend = FALSE;
    if (MATCHKW(lex, begin))
    {
        needend = TRUE;
        lex = getsym();
    }
    if (cparams.prm_cplusplus && needend && MATCHKW(lex, end))
    {
        exp = intNode(en_c_i, 0);
        sp->tp = &stdint; // sets type for variable
        initInsert(init, sp->tp, exp, offset, FALSE);
    }
    else
    {
        TYPE *tp = NULL;
        EXPRESSION *exp;
        lex = optimized_expression(lex, funcsp, NULL, &tp, &exp, FALSE);
        if (!tp)
            error(ERR_EXPRESSION_SYNTAX);
        else
        {
            if (isconst(sp->tp) && !isconst(tp))
            {
                TYPE *itp = Alloc(sizeof(TYPE));
                itp->type = bt_const;
                itp->size = tp->size;
                itp->btp = tp;
                tp = itp;
            }
            if (isvolatile(sp->tp) && !isvolatile(tp))
            {
                TYPE *itp = Alloc(sizeof(TYPE));
                itp->type = bt_volatile;
                itp->size = tp->size;
                itp->btp = tp;
                tp = itp;
            }
            sp->tp = tp; // sets type for variable
        }
        if (cparams.prm_cplusplus && isstructured(sp->tp))
        {

            INITIALIZER *dest = NULL, *it ;
            EXPRESSION *expl = getVarNode(sp);
            initInsert(init, sp->tp, exp, offset, FALSE);
            callDestructor(sp, &expl, NULL, TRUE);
            initInsert(&dest, sp->tp, expl, offset, TRUE);
            if (sp->storage_class != sc_auto && sp->storage_class != sc_member)
            {
                insertDynamicDestructor(sp, dest);
            }
            else
            {
                sp->dest = dest;
            }
        }
        else
        {
            initInsert(init, sp->tp, exp, offset, FALSE);
        }
    }
    if (needend)
    {
        if (!needkw(&lex, end))
        {
            errskim(&lex, skim_end);
            skip(&lex, end);
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
LEXEME *initType(LEXEME *lex, SYMBOL *funcsp, int offset, enum e_sc sc, 
                 INITIALIZER **init, INITIALIZER **dest, TYPE *itype, SYMBOL *sp, BOOL arrayMember)
{
    TYPE *tp = basetype(itype);
    switch(tp->type)
    {
        case bt_aggregate:
            return lex;
        case bt_bool:
            return initialize_bool_type(lex, funcsp, offset, sc, tp, init);
        case bt_char:
        case bt_unsigned_char:
        case bt_short:
        case bt_unsigned_short:
        case bt_int:
        case bt_unsigned:
        case bt_char16_t:
        case bt_char32_t:
        case bt_long:
        case bt_unsigned_long:
        case bt_long_long:
        case bt_unsigned_long_long:
        case bt_float:
        case bt_double:
        case bt_long_double:
        case bt_float_imaginary:
        case bt_double_imaginary:
        case bt_long_double_imaginary:
        case bt_float_complex:
        case bt_double_complex:
        case bt_long_double_complex:
        case bt_enum:
            return initialize_arithmetic_type(lex, funcsp, offset, sc, tp, init);
        case bt_lref:
        case bt_rref:
            return initialize_reference_type(lex, funcsp, offset, sc, tp, init);
        case bt_pointer:
            if (tp->array)
            {
                if (tp->vla)
                {
                    error(ERR_VLA_NO_INIT);
                    errskim(&lex, skim_semi);
                    return lex;
                }
                else
                    return initialize_aggregate_type(lex, funcsp, sp, offset, sc, tp, init, dest, arrayMember);
            }
            else
            {
                return initialize_pointer_type(lex, funcsp, offset, sc, tp, init);
            }
        case bt_memberptr:
            return initialize_memberptr(lex, funcsp, offset, sc, tp, init);
        case bt_bit:
            return initialize_bit(lex, funcsp, offset, sc, tp, init);
        case bt_auto:
            return initialize_auto(lex, funcsp, offset, sc, tp, init, dest, sp);
        case bt_struct:
        case bt_union:
        case bt_class:
            if (tp->syms)
                if (!cparams.prm_cplusplus && MATCHKW(lex, assign) && (sc == sc_auto || sc == sc_register))
                {
                    lex = getsym();
                    if (MATCHKW(lex, begin))
                    {
                        return initialize_aggregate_type(lex, funcsp, sp, offset, sc, tp, init, dest, arrayMember);
                    }
                    else
                    {
                        return initialize_auto_struct(lex, funcsp, offset, tp, init);
                    }
                }
                else
                {
                    return initialize_aggregate_type(lex, funcsp, sp, offset, sc, tp, init, dest, arrayMember);
                }
            /* fallthrough */
        default:
            errortype(ERR_CANNOT_INITIALIZE, tp, NULL);
            break;
    }
    return lex;
}
BOOL IsConstantExpression(EXPRESSION *node, BOOL allowParams)
{
    BOOL rv = FALSE;
    if (node == 0)
        return rv;
    switch (node->type)
    {
        case en_const:
            rv = TRUE;
            break;
        case en_c_ll:
        case en_c_ull:
        case en_c_d:
        case en_c_ld:
        case en_c_f:
        case en_c_dc:
        case en_c_ldc:
        case en_c_fc:
        case en_c_di:
        case en_c_ldi:
        case en_c_fi:
        case en_c_i:
        case en_c_l:
        case en_c_ui:
        case en_c_ul:
        case en_c_c:
        case en_c_bool:
        case en_c_uc:
        case en_c_wc:
        case en_c_u16:
        case en_c_u32:        
        case en_nullptr:
            rv = TRUE;
            break;
        case en_global:
        case en_label:
        case en_pc:
        case en_labcon:
        case en_absolute:
        case en_threadlocal:
            rv = TRUE;
            break;
        case en_auto:
            rv = FALSE;
            break;
        case en_l_sp:
        case en_l_fp:
        case en_bits:
        case en_l_f:
        case en_l_d:
        case en_l_ld:
        case en_l_fi:
        case en_l_di:
        case en_l_ldi:
        case en_l_fc:
        case en_l_dc:
        case en_l_ldc:
        case en_l_c:
        case en_l_u16:
        case en_l_u32:
        case en_l_s:
        case en_l_ul:
        case en_l_l:
        case en_l_p:
        case en_l_ref:
        case en_l_i:
        case en_l_ui:
        case en_l_uc:
        case en_l_us:
        case en_l_bool:
        case en_l_bit:
        case en_l_ll:
        case en_l_ull:
            if (node->left->type == en_auto)
                rv = allowParams && node->left->v.sp->storage_class == sc_parameter;
            else
                rv = FALSE;
            break;
        case en_uminus:
        case en_compl:
        case en_not:
        case en_x_f:
        case en_x_d:
        case en_x_ld:
        case en_x_fi:
        case en_x_di:
        case en_x_ldi:
        case en_x_fc:
        case en_x_dc:
        case en_x_ldc:
        case en_x_ll:
        case en_x_ull:
        case en_x_i:
        case en_x_ui:
        case en_x_c:
        case en_x_u16:
        case en_x_u32:
        case en_x_wc:
        case en_x_uc:
        case en_x_bool:
        case en_x_bit:
        case en_x_s:
        case en_x_us:
        case en_x_l:
        case en_x_ul:
        case en_x_p:
        case en_x_fp:
        case en_x_sp:
        case en_trapcall:
        case en_shiftby:
/*        case en_movebyref: */
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
            rv = IsConstantExpression(node->left, allowParams);
            break;
        case en_assign:
            rv = FALSE;
            break;
        case en_autoinc:
        case en_autodec:
            rv = IsConstantExpression(node->left, allowParams);
            break; 
        case en_add:
        case en_sub:
/*        case en_addcast: */
        case en_lsh:
        case en_arraylsh:
        case en_rsh:
        case en_rshd:
        case en_void:
        case en_voidnz:
/*        case en_dvoid: */
        case en_arraymul:
        case en_arrayadd:
        case en_arraydiv:
        case en_structadd:
        case en_mul:
        case en_div:
        case en_umul:
        case en_udiv:
        case en_umod:
        case en_ursh:
        case en_mod:
        case en_and:
        case en_or:
        case en_xor:
        case en_lor:
        case en_land:
        case en_eq:
        case en_ne:
        case en_gt:
        case en_ge:
        case en_lt:
        case en_le:
        case en_ugt:
        case en_uge:
        case en_ult:
        case en_ule:
        case en_cond:
        case en_intcall:
        case en_stackblock:
        case en_blockassign:
        case en_mp_compare:
/*		case en_array: */

            rv = IsConstantExpression(node->left, allowParams);
            rv &= IsConstantExpression(node->right, allowParams);
            break;
        case en_atomic:
            rv = FALSE;
            break;
        case en_blockclear:
        case en_argnopush:
        case en_not_lvalue:
        case en_mp_as_bool:
        case en_thisref:
            rv = IsConstantExpression(node->left, allowParams);
            break;
        case en_func:
            rv = FALSE;
            break;
        case en_stmt:
            rv = FALSE;
            break;
        default:
            rv = FALSE;
            diag("IsConstantExpression");
            break;
    }
    return rv;
}
static BOOL hasData(TYPE *tp)
{
    HASHREC *hr = basetype(tp)->syms->table[0];
    while (hr)
    {
        if (((SYMBOL *)hr->p)->tp->size)
            return TRUE;
        hr = hr->next;
    }
    return FALSE;
}
LEXEME *initialize(LEXEME *lex, SYMBOL *funcsp, SYMBOL *sp, enum e_sc storage_class_in, BOOL asExpression)
{
    TYPE *tp;
    inittag = 0;
    browse_variable(sp);
    IncGlobalFlag();
    switch(sp->storage_class)
    {
        case sc_parameter:
            sp->assigned = TRUE;
            break;
        case sc_global:
        case sc_external:
            initializingGlobalVar = TRUE;
            sp->assigned = TRUE;
            sp->used = TRUE;
            break;
        case sc_static:
        case sc_localstatic:
            initializingGlobalVar = TRUE;
            sp->assigned = TRUE;
            break;
        case sc_auto:
        case sc_register:
            sp->allocate = TRUE;
            break;
        case sc_type:
        case sc_typedef:
            break;
    }
    if (funcsp && funcsp->linkage == lk_inline && sp->storage_class == sc_static)
    {
        errorsym(ERR_INLINE_NO_STATIC, sp);	
    }
    // check for attempt to use an undefined struct
    tp = basetype(sp->tp);
    if (ispointer(tp) && tp->array || isref(tp))
        tp = basetype(basetype(tp)->btp);
    if (sp->storage_class != sc_typedef && sp->storage_class != sc_external && isstructured(tp) && !tp->syms)
    {
        if (MATCHKW(lex, assign))
            errskim(&lex, skim_semi);
        errorsym(ERR_STRUCT_NOT_DEFINED, tp->sp);
    }
    // if not in a constructor, any openpa() will be eaten by an expression parser
    else if (MATCHKW(lex, assign) || cparams.prm_cplusplus && (MATCHKW(lex, openpa) || MATCHKW(lex, begin)))
    {
        INITIALIZER **init;
        sp->assigned = TRUE;
        if (sp->init)
            errorsym(ERR_MULTIPLE_INITIALIZATION, sp);
        if (storage_class_in == sc_auto)
            if (sp->storage_class == sc_external)
                error(ERR_EXTERN_NO_INIT);
        if (sp->storage_class == sc_typedef)
        {
            error(ERR_TYPEDEF_NO_INIT);
        }
        else
        {
            if (GetGlobalFlag() == 1)
            {
                /* have to copy the type since it was created
                 * at auto scope without the global flag
                 */
                TYPE *tp = sp->tp;
                TYPE *tp2 = NULL, **tp3 = &tp2;
                while (tp)
                {
                    *tp3 = Alloc(sizeof(TYPE));
                    **tp3 = *tp;
                    tp3 = &(*tp3)->btp;
                    tp = tp->btp;
                }
                sp->tp = tp2;
            }
            if (sp->storage_class == sc_absolute)
                error(ERR_ABSOLUTE_NOT_INITIALIZED);
            else if (sp->storage_class == sc_external)
                sp->storage_class = sc_global;
            /*
            if (storage_class_in == sc_member)
            {
                lex = getDeferredData(lex, sp, FALSE);
            }    
            else
            */
            {
                if (MATCHKW(lex, assign) && !isstructured(sp->tp))
                    lex = getsym(); /* past = */
                lex = initType(lex, funcsp, 0, sp->storage_class, &sp->init, &sp->dest, sp->tp, sp, FALSE);
                /* set up an end tag */
                if (sp->init)
                {
                    init = &sp->init;			
                    while (*init)
                    {
                        if (!(*init)->basetp)
                            break;
                        init = &(*init)->next;
                    }
                    if (!*init)
                        initInsert(init, NULL, NULL, sp->tp->size, FALSE);
                }
            }
        }
    }
    else if (cparams.prm_cplusplus && sp->storage_class != sc_typedef && !asExpression)
    {
        if (isstructured(sp->tp) && !basetype(sp->tp)->sp->trivialCons)
        {
            // default constructor without (), or array of structures without an initialization list
            lex = initType(lex, funcsp, 0, sp->storage_class, &sp->init, &sp->dest, sp->tp, sp, FALSE);
            /* set up an end tag */
            if (sp->init)
            {
                INITIALIZER **init = &sp->init;
                while (*init)
                {
                    if (!(*init)->basetp)
                        break;
                    init = &(*init)->next;
                }
                if (!*init)
                    initInsert(init, NULL, NULL, sp->tp->size, FALSE);
            }
        }
        else if (isarray(sp->tp))
        {
            // constructors for uninitialized array
            TYPE *z = sp->tp;
            while (isarray(z))
                z = basetype(z)->btp;
            z= basetype(z);
            if (isstructured(z) && !z->sp->trivialCons)
            {
                INITIALIZER *init = NULL, *it = NULL;
                int n = sp->tp->size/(z->size + z->arraySkew);
                TYPE *ctype = z;
                EXPRESSION *sz = n > 1 ? intNode(en_c_i, n) : NULL;
                EXPRESSION *baseexp = getVarNode(sp);
                EXPRESSION *exp = baseexp;
                callConstructor(&ctype, &exp, NULL, TRUE, sz, TRUE, FALSE);
                initInsert(&it, z, exp, 0, TRUE);
                if (storage_class_in != sc_auto && storage_class_in != sc_member)
                {
                    insertDynamicInitializer(sp, it);
                }
                else
                {
                    sp->init = it;
                }
                exp = baseexp;
                callDestructor(z->sp, &exp, sz, TRUE);
                initInsert(&init, z, exp, 0, TRUE);
                if (storage_class_in != sc_auto && storage_class_in != sc_member)
                {
                    insertDynamicDestructor(sp, init);
                }
                else
                {
                    sp->dest = init;
                }
                if (sp->init)
                {
                    INITIALIZER **init = &sp->init;
                    while (*init)
                    {
                        if (!(*init)->basetp)
                            break;
                        init = &(*init)->next;
                    }
                    if (!*init)
                        initInsert(init, NULL, NULL, sp->tp->size, FALSE);
                }
            }   
        }
    }
    if (sp->tp->type == bt_auto)
    {
        errorsym(ERR_AUTO_NEEDS_INITIALIZATION, sp);
        sp->tp = &stdint;
    }
    tp = sp->tp;
    while (ispointer(tp))
    {
        tp = basetype(tp);
        if (!tp->array)
        {
            tp = sp->tp;
            break;
        }
        tp = tp->btp;
    }
    if (sp->constexpression)
    {
        if (!ispointer(tp) && !isarithmetic(tp))
        {
            error(ERR_CONSTEXPR_SIMPLE_TYPE);
        }
        else if (!sp->init)
        {
            if (sp->storage_class != sc_external)
                error(ERR_CONSTEXPR_REQUIRES_INITIALIZER);
        }
        else
        {
            if (!IsConstantExpression(sp->init->exp, FALSE))
            {
                sp->init->exp = intNode(en_c_i, 0);
                error(ERR_CONSTANT_EXPRESSION_EXPECTED);
            }
        }
    }
    if (isconst(tp))
        if  (!sp->init)
        {
            if (!asExpression)
            {
                if (sp->storage_class != sc_external && sp->storage_class != sc_typedef && sp->storage_class != sc_member)
                {
                    if (!isstructured(tp) || !cparams.prm_cplusplus || basetype(tp)->sp->trivialCons && hasData(tp))
                        errorsym(ERR_CONSTANT_MUST_BE_INITIALIZED, sp);
                }
            }
        }
        else if (isstructured(tp) && basetype(tp)->sp->trivialCons && hasData(tp))
        {
            if (!asExpression)
                errorsym(ERR_CONSTANT_MUST_BE_INITIALIZED, sp);
        }
        else if (isintconst(sp->init->exp) && isint(sp->tp) && !sp->parentClass)
            {
                sp->value.i = sp->init->exp->v.i ;
                sp->storage_class = sc_constant;
            }
    if (cparams.prm_cplusplus && isstructured(sp->tp))
    {
        ReferenceVTab(basetype(sp->tp)->sp);
    }
    if (isref(sp->tp) && sp->storage_class != sc_typedef)
    {
        if (!sp->init && sp->storage_class != sc_external && sp->storage_class != sc_member && !asExpression)
        {
            errorsym(ERR_REF_MUST_INITIALIZE, sp);
        }
    }
    if (sp->storage_class == sc_static || sp->storage_class == sc_global 
        || sp->storage_class == sc_localstatic)
    {
        if (storage_class_in == sc_auto || storage_class_in == sc_register)
            sp = clonesym(sp);
        insertInitSym(sp);
    }
    if (sp->init)
        declareAndInitialize = TRUE;

    DecGlobalFlag();    
    initializingGlobalVar = FALSE;
    return lex;
}
