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

#define BE_HASH 512
static HASHREC *pinvokes[BE_HASH];
static HASHREC *enums[BE_HASH];
static HASHREC *externs[BE_HASH];
static HASHREC *globals[BE_HASH];
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

static SYMBOL * clonesp(SYMBOL *sp);
static void clonetable(TYPE *tp)
{
    struct clist *test = cloneList;
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
    {
        HASHREC *src, **dest;
        HASHTABLE *rv = calloc(1, sizeof(HASHTABLE));
        struct clist *clonedata = calloc(1, sizeof(struct clist));
        clonedata->sp = tp->sp;
        clonedata->result = rv;
        clonedata->resultsp = calloc(1, sizeof(SYMBOL));
        *clonedata->resultsp = *tp->sp;

        clonedata->resultsp->tp = tp;
        clonedata->resultsp->name = strdup(tp->sp->name);
        clonedata->next = cloneList;
        cloneList = clonedata;
        *rv = *tp->syms;
        src = tp->syms->table[0];
        tp->sp = clonedata->resultsp;
        tp->syms = rv;
        rv->table = calloc(1, sizeof(HASHREC *));
        dest = &rv->table[0];
        while (src)
        {
            *dest = calloc(1, sizeof(HASHREC));
            (*dest)->p = clonesp((SYMBOL *)src->p);
            dest = &(*dest)->next;
            src = src->next;
        }
    }
}
static TYPE * clonetp(TYPE *tp)
{
    TYPE *rv = NULL, **put = & rv;
    while (tp)
    {
        *put = calloc(1, sizeof(TYPE));
        **put = *tp;
        if (tp->syms)
        {
            clonetable(*put);
        }
        put = &(*put)->btp;
        tp = tp->btp;
    }
    return rv;
}
static SYMBOL * clonesp(SYMBOL *sp)
{
    int i;
    SYMBOL *spnew;
    spnew = calloc(1, sizeof(SYMBOL));
    *spnew = *sp;
    spnew->tp = clonetp(sp->tp);
    spnew->name = strdup(spnew->name);
    return spnew;
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
static void hashInsert(HASHREC **table, SYMBOL *sp, BOOLEAN clone)
{
    int hash = hashVal(sp->name);
    HASHREC *hr  = table[hash];
    while (hr)
    {
        if (!strcmp(hr->p->name, sp->name))
            return;
        hr = hr->next;
    }
    if (!clone)
    {
        SYMBOL *spnew = calloc(1, sizeof(SYMBOL));
        *spnew = *sp;
        sp = spnew;
        sp->name = strdup(sp->name);
    }
    else
    {
        sp = clonesp(sp);
    }
    hr = calloc(1, sizeof(HASHREC));
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
    p = calloc(1, sizeof(LIST));
    p->data = clonetp(tp);
    p->next = typeList;
    typeList = p;
}
static void cache_enum(SYMBOL *sp)
{
    hashInsert(enums, sp, TRUE);
}
void cache_pinvoke(SYMBOL *sp)
{
    hashInsert(pinvokes, sp, TRUE);
}
void cache_extern(SYMBOL *sp)
{

    hashInsert(externs, sp, FALSE);
}
void cache_global(SYMBOL *sp)
{
    hashInsert(globals, sp, FALSE);
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
}
void oa_end_generation(void)
{
    SYMBOL *start = NULL, *end = NULL;
    LIST *externalList = externals;
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
        LIST *lst = calloc(1, sizeof(LIST));
        lst->data = strdup(start->name);
        if (initializersHead)
            initializersTail = initializersTail->next = lst;
        else
            initializersHead = initializersTail = lst;
    }
    if (end)
    {
        LIST *lst = calloc(1, sizeof(LIST));
        lst->data = strdup(end->name);
        if (deinitializersHead)
            deinitializersTail = deinitializersTail->next = lst;
        else
            deinitializersHead = initializersTail = lst;
    }
    oa_load_funcs();
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
        bePrintf("\tcall void %s%s()\n", namespaceAndClass, (char *)lst->data);
        lst = lst->next;
    }
}
void oa_trailer(void)
{
    SYMBOL *mainsp;
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
        mainsp = gsearch("main");
        if (mainsp)
        {
            mainsp = (SYMBOL *)mainsp->tp->syms->table[0]->p;
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
static BOOLEAN hasGlobal(char *name)
{
    int hash = hashVal(name);
    HASHREC *hr  = globals[hash];
    while (hr)
    {
        if (!strcmp(hr->p->name, name))
            return TRUE;
        hr = hr->next;
    }
    return FALSE;
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
                    printf ("Undefined external %s\n", name);
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
    oa_trailer();
    fclose(outputFile);
    errors |= checkExterns();
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
