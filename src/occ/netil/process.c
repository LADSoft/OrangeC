/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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
#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "be.h"
#include "winmode.h"

#define IEEE

extern FILE *outputFile;

extern int prm_targettype;
extern int prm_assembler;
extern SYMBOL *theCurrentFunc;
extern LIST *temporarySymbols;
extern LIST *externals;
extern TYPE stdpointer, stdint;
extern INCLUDES *includes;
extern char namespaceAndClass[512];
extern char namespaceAndClassForNestedType[512];
extern BOOLEAN int8_used, int16_used, int32_used;
extern enum e_sg oa_currentSeg = noseg; /* Current seg */
extern FILE *outputFile;
extern OCODE *peep_head, *peep_tail;

#define BE_HASH 512
static int errCount;
static HASHREC *pinvokes[BE_HASH];
static HASHREC *enums[BE_HASH];
static HASHREC *externs[BE_HASH];
static HASHREC *globals[BE_HASH];
static SYMBOL *mainSym;
static int corflags = 2; // x86
static LIST *typeList;
static char ilName[260], tmpName[260];
static LIST *initializersHead, *initializersTail;
static LIST *deinitializersHead, *deinitializersTail;
struct clist
{
    struct clist *next;
    SYMBOL *sp;
    HASHTABLE *result;
    SYMBOL *resultsp;
};
static struct clist *cloneList;
static int cloneCount;

static BOOLEAN validateGlobalRef(SYMBOL *sp);
void *msil_alloc(size_t size)
{
    void *rv = calloc(size, 1);
    if (!rv)
        fatal("out of memory");
    return rv;
}
char *msil_strdup(char *s)
{
    int len = strlen(s) + 1;
    char *rv = msil_alloc(len);
    if (rv)
    {
        memcpy(rv, s, len);
    }
    return rv;
}
static void clonetable(TYPE *tp)
{
    struct clist *test = cloneList;
    if (!isfunction(tp))
    {
        while (test)
        {
            if (test->sp == tp->sp)
            {
                tp->sp = test->resultsp;
                tp->syms = test->result;
                return;
            }
            test = test->next;
        }
    }
    {
        HASHREC *src, **dest;
        HASHTABLE *rv = msil_alloc(sizeof(HASHTABLE));
        struct clist *clonedata = msil_alloc(sizeof(struct clist));
        clonedata->sp = tp->sp;
        clonedata->result = rv;
        clonedata->resultsp = msil_alloc(sizeof(SYMBOL));
        *clonedata->resultsp = *tp->sp;

        clonedata->resultsp->tp = tp;
        clonedata->resultsp->name = msil_strdup(tp->sp->name);
        clonedata->next = cloneList;
        cloneList = clonedata;
        *rv = *tp->syms;
        src = tp->syms->table[0];
        tp->sp = clonedata->resultsp;
        tp->syms = rv;
        rv->table = msil_alloc(sizeof(HASHREC *));
        dest = &rv->table[0];
        while (src)
        {
            *dest = msil_alloc(sizeof(HASHREC));
            (*dest)->p = clonesp((SYMBOL *)src->p, FALSE);
            dest = &(*dest)->next;
            src = src->next;
        }
    }
}
TYPE * clonetp(TYPE *tp, BOOLEAN shallow)
{
    TYPE *rv = NULL, **put = & rv;
    while (tp)
    {
        *put = msil_alloc(sizeof(TYPE));
        **put = *tp;
        if (tp->syms)
        {
            if (shallow && isstructured(tp))
            {
                SYMBOL *spnew = msil_alloc(sizeof(SYMBOL));
                *spnew = *(*put)->sp;
                spnew->tp = NULL;
                spnew->name = msil_strdup(spnew->name);
                (*put)->sp = spnew;
            }
            else
            {
                clonetable(*put);
            }
        }
        put = &(*put)->btp;
        tp = tp->btp;
    }
    return rv;
}
SYMBOL * clonesp(SYMBOL *sp, BOOLEAN shallow)
{
    int i;
    SYMBOL *spnew;
    spnew = msil_alloc(sizeof(SYMBOL));
    *spnew = *sp;
    spnew->tp = clonetp(sp->tp, shallow);
    spnew->name = msil_strdup(spnew->name);
    return spnew;
}
INITLIST *cloneInitListTypes(INITLIST *in)
{
    INITLIST *rv = NULL , **last = &rv;
    while (in)
    {
        *last = (INITLIST *)msil_alloc(sizeof(INITLIST));
        (*last)->tp = clonetp(in->tp, TRUE);
        last = &(*last)->next;
        in = in->next;
    }
    return rv;
}

static int hashVal(char *name)
{
    unsigned hash = 0;
    while (*name)
    {
        hash * 131;
        hash = (hash << 7 ) + (hash << 1) + hash + (*name++);
    }
    return hash % BE_HASH;
}
static void hashInsert(HASHREC **table, SYMBOL *sp, BOOLEAN shallow)
{
    int hash = hashVal(sp->name);
    HASHREC *hr  = table[hash];
    while (hr)
    {
        if (!strcmp(hr->p->name, sp->name))
        {
            if (table == globals)
            {
                errCount++;
                printf("Error: Duplicate Public Symbol %s\n", sp->name);
            }
            return;
        }
        hr = hr->next;
    }
    sp = clonesp(sp, shallow);
    hr = msil_alloc(sizeof(HASHREC));
    hr->p = sp;
    hr->next = table[hash];
    table[hash] = hr;
}
void cacheType(TYPE *tp)
{
    LIST *p = typeList;
    while (p)
    {
        TYPE *tpx = (TYPE *)p->data;
        if (isstructured(tp) && isstructured(tpx) && !strcmp(basetype(tp)->sp->name, basetype(tpx)->sp->name))
            return;
        if (comparetypes((TYPE *)tp, tpx, TRUE))
        {
            tp = basetype(tp);
            tpx = basetype(tpx);
            if (tpx->array && tp->array)
            {
                while (tp && tpx)
                {
                    if (tp->size != tpx->size)
                        break;
                        tpx = basetype(tpx)->btp;
                    tp = basetype(tp)->btp;
                }
                if (!tp && !tpx)
                {
                    return;
                }
            }
        }
        p = p->next;
    }
    p = msil_alloc(sizeof(LIST));
    p->data = clonetp(tp, FALSE);
    p->next = typeList;
    typeList = p;
}
static void cache_enum(SYMBOL *sp)
{
    hashInsert(enums, sp, FALSE);
}
void cache_pinvoke(SYMBOL *sp)
{
    hashInsert(pinvokes, sp, FALSE);
}
void cache_extern(SYMBOL *sp)
{
    if (strncmp(sp->name, "__va_",4))
    {
        hashInsert(externs, sp, TRUE);
        validateGlobalRef(sp);
    }
}
void cache_global(SYMBOL *sp)
{
    hashInsert(globals, sp, TRUE);
    validateGlobalRef(sp);
}
// weed out unions, structures with nested structures or bit fields
static BOOLEAN qualifiedStruct(SYMBOL *sp)
{
    HASHREC *hr ;
    if (!sp->tp->size)
        return FALSE;
    hr = basetype(sp->tp)->syms->table[0];
    if (basetype(sp->tp)->type == bt_union)
        return FALSE;
    while (hr)
    {
        SYMBOL *check = (SYMBOL *)hr->p;
        if (basetype(check->tp)->bits)
            return FALSE;
        if (isstructured(check->tp))
            return FALSE;
        if (basetype(check->tp)->array)
            return FALSE;
        hr = hr->next;
    }
    return TRUE;

}
void DumpClassFields(SYMBOL *sp)
{
    HASHREC *hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *check = (SYMBOL *)hr->p;
        bePrintf("\n\t.field public ");
        if (isfunction(check->tp) || isfuncptr(check->tp))
            bePrintf("void *");
        else
            puttype(check->tp);
        bePrintf(" '%s'", check->name);
        hr = hr->next;
    }
    bePrintf("\n");
}
void oa_enter_type(SYMBOL *sp)
{
    if (sp->storage_class == sc_typedef)
        return;
    if (namespaceAndClass[0] && sp->tp->type == bt_enum && !strstr(sp->name, "__anontype"))
    {
        cache_enum(sp);
    }
    else if (isstructured(sp->tp) || isarray(sp->tp))
    {
        if (sp->tp->size)
            cacheType(sp->tp);
    }
}
static void dumpPInvokes(void)
{
    int i;
    for (i=0; i < BE_HASH; i++)
    {
        HASHREC *pinvokeList = pinvokes[i];
        while (pinvokeList)
        {
            put_pinvoke((SYMBOL *)pinvokeList->p);
            pinvokeList = pinvokeList->next;
        }
    }
}
static void dumpEnums(void)
{
    int i;
    int l = strlen(namespaceAndClass);
    if (l)
        namespaceAndClass[l-2] = 0;
    for (i=0; i < BE_HASH; i++)
    {
        HASHREC *enumList = enums[i];
        while (enumList)
        {
            SYMBOL *sp = (SYMBOL *)enumList->p;
            if (sp->tp->syms->table[0])
            {
                HASHREC *hr = sp->tp->syms->table[0];
                bePrintf(".class enum nested public auto ansi sealed '%s' {\n", sp->name);
                while (hr)
                {
                    SYMBOL *spe = (SYMBOL *)hr->p;
                    bePrintf("\t.field public static literal valuetype %s/%s %s = int32(%d)\n",
                            namespaceAndClass, sp->name, spe->name, spe->value.i);
                    hr = hr->next;
                }
                bePrintf("\t.field public specialname rtspecialname int32 value__\n");
                bePrintf("}\n");
            }
            enumList = enumList->next;
        }
    }
    if (l)
        namespaceAndClass[l-2] = ':';
}
void dumpTypes()
{
    LIST *lst = typeList;
    LIST **pList = &lst;
    typeList = NULL;

    if (int8_used)
    {
        bePrintf(".class %s private value explicit ansi sealed 'int8[]' {.pack 1 .size 1}\n", namespaceAndClass[0] ? "nested" : "");
    }
    if (int16_used)
    {
        bePrintf(".class %s private value explicit ansi sealed 'int16[]' {.pack 2 .size 1}\n", namespaceAndClass[0] ? "nested" : "");
    }
    if (int32_used)
    {
        bePrintf(".class %s private value explicit ansi sealed 'int32[]' {.pack 4 .size 1}\n", namespaceAndClass[0] ? "nested" : "");
    }
    while (*pList)
    {
        // weed
        TYPE *tp = ((TYPE *)(*pList)->data);
        char z;
        if (namespaceAndClass[0])
            bePrintf(".class nested public value explicit auto sequential ansi sealed '");
        else
            bePrintf(".class private value explicit ansi sealed '");
        z = namespaceAndClass[0];
        namespaceAndClass[0] = 0;
        puttype(tp);
        namespaceAndClass[0] = z;
        if (isstructured(tp) && namespaceAndClass[0])
        {
            bePrintf("' {.pack %d .size %d", basetype(tp)->sp->structAlign, basetype(tp)->size);
            if (qualifiedStruct(basetype(tp)->sp))
                DumpClassFields(basetype(tp)->sp);
            bePrintf("}\n");
        }
        else
        {
            bePrintf("' {.pack 1 .size %d }\n", tp->size);
        }
        pList = &(*pList)->next;
    }

    typeList = NULL;
}
void oa_load_funcs(void)
{
    SYMBOL *sp;
    sp = gsearch("exit");
    if (sp)
        ((SYMBOL *)sp->tp->syms->table[0]->p)->genreffed = TRUE;
    sp = gsearch("__getmainargs");
    if (sp)
        ((SYMBOL *)sp->tp->syms->table[0]->p)->genreffed = TRUE;
    sp = gsearch("__pctype_func");
    if (sp)
        ((SYMBOL *)sp->tp->syms->table[0]->p)->genreffed = TRUE;
    sp = gsearch("__iob_func");
    if (sp)
        ((SYMBOL *)sp->tp->syms->table[0]->p)->genreffed = TRUE;
    sp = gsearch("_errno");
    if (sp)
        ((SYMBOL *)sp->tp->syms->table[0]->p)->genreffed = TRUE;
    sp = gsearch("__GetErrno");
    if (sp)
        ((SYMBOL *)sp->tp->syms->table[0]->p)->genreffed = FALSE;
    if (!strcmp(theCurrentFunc->name, "main"))
        if (!theCurrentFunc->parentClass && !theCurrentFunc->parentNameSpace)
            mainSym = clonesp( theCurrentFunc, FALSE);
}
static void msil_flush_peep(void)
{
    if (cparams.prm_asmfile)
    {
        while (peep_head != 0)
        {
            switch (peep_head->opcode)
            {
                case op_label:
                    oa_put_label((int)peep_head->oper);
                    break;
                case op_funclabel:
                    oa_gen_strlab((SYMBOL *)peep_head->oper);
                    break;
                default:
                    oa_put_code(peep_head);
                    break;

            }
            peep_head = peep_head->fwd;
        }
    }
    peep_head = 0;
}
void oa_end_generation(void)
{
    SYMBOL *start = NULL, *end = NULL;
    LIST *externalList;
    externalList = externals;
    while (externalList)
    {
        SYMBOL *sym = (SYMBOL *)externalList->data;
        if (!strncmp(sym->name, "__DYNAMIC", 9))
        {
            if (strstr(sym->name, "STARTUP"))
                start = sym;
            else
                end = sym;
        }
        externalList = externalList->next;
    }
    if (start)
    {
        LIST *lst = msil_alloc(sizeof(LIST));
        lst->data = msil_strdup(start->name);
        if (initializersHead)
            initializersTail = initializersTail->next = lst;
        else
            initializersHead = initializersTail = lst;
    }
    if (end)
    {
        LIST *lst = msil_alloc(sizeof(LIST));
        lst->data = msil_strdup(end->name);
        if (deinitializersHead)
            deinitializersTail = deinitializersTail->next = lst;
        else
            deinitializersHead = initializersTail = lst;
    }
}
static void mainLocals(void)
{
    bePrintf("\t.locals (\n");
    bePrintf("\t\t[0] int32 'argc',\n");
    bePrintf("\t\t[1] void * 'argv',\n");
    bePrintf("\t\t[2] void * 'environ',\n");
    bePrintf("\t\t[3] void * 'newmode'\n");
    bePrintf("\t)\n");
}
static void mainInit(void)
{
    bePrintf("\tcall void *'__pctype_func'()\n");
    bePrintf("\tstsfld void * %s_pctype\n", namespaceAndClass);
    bePrintf("\tcall void *'__iob_func'()\n");
    bePrintf("\tdup\n");
    bePrintf("\tstsfld void * %s__stdin\n", namespaceAndClass);
    bePrintf("\tdup\n");
    bePrintf("\tldc.i4 32\n");
    bePrintf("\tadd\n");
    bePrintf("\tstsfld void * %s__stdout\n", namespaceAndClass);
    bePrintf("\tldc.i4 64\n");
    bePrintf("\tadd\n");
    bePrintf("\tstsfld void * %s__stderr\n", namespaceAndClass);
}
void oa_header(char *filename, char *compiler_version)
{
    _apply_global_using();
    if (namespaceAndClass[0])
        corflags |= 1; // accessible assembly (ILONLY)

    oa_nl();
    bePrintf("//File %s\n",filename);
    bePrintf("//Compiler version %s\n",compiler_version);
    bePrintf("\n.corflags %d // 32-bit", corflags);
    {
        char *p = strrchr(filename, '.');
        if (p)
        {
            char *q = strrchr(filename, '\\');
            if (!q)
                q = filename;
            else
                q++;
            *p = 0;
            bePrintf("\n.assembly %s { }\n",q);
            *p++ = '.';
        }
        else
        {
            bePrintf("\n.assembly %s { }\n",filename);
        }
    }
    bePrintf("\n.assembly extern mscorlib { }\n");
    bePrintf("\n.assembly extern lsmsilcrtl { }\n\n\n");
    if (namespaceAndClass[0])
    {
        char *p = strchr(namespaceAndClass, '.'), *q;
        if (p)
        {
            *p = 0;
            bePrintf("\n.namespace '%s' { \n", namespaceAndClass);
            *p++ = '.';

            q = strchr(p, ':');
            if (q)
            {
                *q = 0;
                bePrintf("\n.class public explicit ansi sealed '%s' { \n", p);
                *q = ':';
            }
        }
    }
}
static void dumpInitializerCalls(LIST *lst)
{
    while (lst)
    {
        bePrintf("\tcall void %s'%s'()\n", namespaceAndClass, (char *)lst->data);
        lst = lst->next;
    }
}
void oa_trailer(void)
{
    SYMBOL *mainsp;
    if (prm_targettype != DLL && !mainSym)
    {
        printf("Error: main not defined\n");
    }
    oa_enterseg(oa_currentSeg);
    bePrintf("\n\t.field public static void *'__stdin'\n");
    bePrintf("\n\t.field public static void *'__stdout'\n");
    bePrintf("\n\t.field public static void *'__stderr'\n");
    bePrintf("\n\t.field public static void *'_pctype'\n");
    oa_enterseg(oa_currentSeg);

    if ((initializersHead || prm_targettype == DLL) && namespaceAndClass[0])
    {
        bePrintf(".method private hidebysig specialname rtspecialname static void  .cctor() cil managed {\n");
        if (prm_targettype == DLL)
        {
            mainLocals();
            bePrintf("\t.maxstack 5\n\n");
            mainInit();
        }
        else
        {
            bePrintf("\t.maxstack 1\n\n");
        }
        dumpInitializerCalls(initializersHead);
        bePrintf("\tret\n");
        bePrintf("}\n");

    }
    if (prm_targettype != DLL)
    {
        bePrintf(".method private hidebysig static void '$Main'() cil managed {\n");
        bePrintf("\t.entrypoint\n");
        mainLocals();
        bePrintf("\t.maxstack 5\n\n");
        mainInit();
        if (!namespaceAndClass[0])
            dumpInitializerCalls(initializersHead);

        bePrintf("\tldloca 'argc'\n");
        bePrintf("\tldloca 'argv'\n");
        bePrintf("\tldloca 'environ'\n");
        bePrintf("\tldc.i4  0\n");
        bePrintf("\tldloca 'newmode'\n");
        bePrintf("\tcall void __getmainargs(void *, void *, void *, int32, void *);\n");
        bePrintf("\tldloc 'argc'\n");
        bePrintf("\tldloc 'argv'\n");
        mainsp = mainSym;
        if (mainsp)
        {
//            mainsp = (SYMBOL *)mainsp->tp->syms->table[0]->p;
            if ( isvoid(basetype(mainsp->tp)->btp))
                if (namespaceAndClass[0])
                    bePrintf("\tcall void %smain(int32, void *)\n", namespaceAndClass);
                else
                    bePrintf("\tcall void 'main'(int32, void *)\n");
            else
                if (namespaceAndClass[0])
                    bePrintf("\tcall int32 %smain(int32, void *)\n", namespaceAndClass);
                else
                    bePrintf("\tcall int32 'main'(int32, void *)\n");
        }
        else
            if (namespaceAndClass[0])
                bePrintf("\tcall int32 %smain(int32, void *)\n", namespaceAndClass);
            else
                bePrintf("\tcall int32 'main'(int32, void *)\n");
        dumpInitializerCalls(deinitializersHead);
        if (mainsp)
            if ( isvoid(basetype(mainsp->tp)->btp))
               bePrintf("\tldc.i4 0\n");
        bePrintf("\tcall void exit(int32)\n");
        bePrintf("\tret\n");
        bePrintf("}\n");
    }
    dumpTypes();
    dumpEnums();    
    if (namespaceAndClass[0])
    {
        bePrintf("}\n");
        bePrintf("}\n");
    }
    bePrintf(".method private hidebysig static void * __GetErrno() cil managed {\n");
    bePrintf("\t.maxstack 1\n\n");
    bePrintf("\tcall void * '_errno'()\n");
    bePrintf("\tret\n");
    bePrintf("}\n");
    dumpPInvokes();
}
static SYMBOL * hasGlobal(char *name)
{
    int hash = hashVal(name);
    HASHREC *hr  = globals[hash];
    while (hr)
    {
        if (!strcmp(hr->p->name, name))
            return (SYMBOL *)hr->p;
        hr = hr->next;
    }
    return NULL;
}
static SYMBOL * hasExtern(char *name)
{
    int hash = hashVal(name);
    HASHREC *hr  = externs[hash];
    while (hr)
    {
        if (!strcmp(hr->p->name, name))
            return (SYMBOL *)hr->p;
        hr = hr->next;
    }
    return NULL;
}
static BOOLEAN validateGlobalRef(SYMBOL *sp)
{
    if (!isfunction(sp->tp))
    {
        SYMBOL *g = hasGlobal(sp->name);
        SYMBOL *e = hasExtern(sp->name);
        if (g && e)
        {
            TYPE *tp = g->tp;
            TYPE *tpx = e->tp;
            tp = basetype(tp);
            tpx = basetype(tpx);
            while (ispointer(tp) && ispointer(tpx))
            {
                if (tpx->size != 0 && tp->size != tpx->size)
                    break;
                tp = basetype(tp->btp);
                tpx = basetype(tpx->btp);
            }
            if (!ispointer(tp) && !ispointer(tpx))
            {
                if (tp->type == tpx->type)
                {
                    if (isstructured(tp) || isfunction(tp) || tp->type == bt_enum)
                    {
                        if (!strcmp(tp->sp->name, tpx->sp->name))
                            return;
                    }
                    else
                    {
                        return;
                    }
                }
            }
            errCount++;
            printf("Error: Mismatch global type declarations on %s\n", sp->name);
        }
    }
}
TYPE * LookupGlobalArrayType(char *name)
{
    SYMBOL *sp = hasGlobal(name);
    if (sp)
    {
        return sp->tp;
    }
    return NULL;
}
static BOOLEAN checkExterns(void)
{
    BOOLEAN rv = FALSE;
    int i;
    for (i=0; i < BE_HASH; i++)
    {
        HASHREC *extList = externs[i];
        while (extList)
        {
            char *name =  extList->p->name;
            if (strcmp(name, "_pctype") && strcmp(name, "__stdin") && 
               strcmp(name, "__stdout") && strcmp(name, "__stderr"))
            {
                if (!hasGlobal(name))
                {
                    printf ("Error: Undefined external symbol %s\n", name);
                    rv = TRUE;
                }
            }
            extList= extList->next;
        }
    }
    return rv;
}
BOOLEAN oa_main_preprocess(void)
{
    char * path[260];
    GetOutputFileName(ilName, path);
    strcpy(tmpName, ilName);
    StripExt(ilName);
    AddExt(ilName, ".il");
    StripExt(tmpName);
    AddExt(tmpName, ".tmp");
    unlink(tmpName);
    rename(ilName, tmpName);
    outputFile = fopen(ilName, "w");
    if (!outputFile)
        fatal("Cannot open .IL file for write access");
    setvbuf(outputFile,0,_IOFBF,32768);
    oa_header(ilName, "MSIL Compiler");
    return FALSE;
}
void oa_main_postprocess(BOOLEAN errors)
{
    msil_flush_peep(); 
    oa_trailer();
    fclose(outputFile);
    errors |= checkExterns() || errCount || prm_targettype != DLL && !mainSym;
    if (errors)
    {
        unlink(ilName);
        rename(tmpName, ilName);
    }
    else
    {
        unlink(tmpName);
    }
}
