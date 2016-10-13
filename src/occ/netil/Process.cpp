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
#include "Be.h"
#include "winmode.h"
#include "DotNetpeLib.h"

#include <vector>
using namespace DotNetPELib;

extern TYPE stdchar, stdshort, stdint;
extern LIST *externals;
extern int prm_targettype;
extern char namespaceAndClass[512];
extern LIST *temporarySymbols;

MethodSignature *argsCtor;
MethodSignature *argsNextArg;
MethodSignature *argsUnmanaged;
MethodSignature *ptrBox;
MethodSignature *ptrUnbox;
Type *systemObject;
Method *currentMethod;
static SYMBOL retblocksym;

static Method *mainSym;

PELib *peLib;
DataContainer *mainContainer;
static int errCount;
static LIST *initializersHead, *initializersTail;
static LIST *deinitializersHead, *deinitializersTail;
static BOOLEAN nested = false;
static int dataPos, dataMax;
static BYTE *dataPointer;
static Field *initializingField;
static SYMBOL *spstack[1000];
static int spstackcount;
char msil_bltins[] = " void exit(int); "
    "void __getmainargs(void *,void *,void*,int, void *); "
    "void *__iob_func(); "
    "unsigned short *__pctype_func(); "
    "int *_errno(); "
    "void *__OCCMSIL_GetProcThunkToManaged(void *proc, void *pdata); "
    "void *__OCCMSIL_GetProcThunkToUnmanaged(void *proc); "
    "void *__msil_rtl malloc(unsigned); "
    "void __msil_rtl free(void *); "
    "void *__va_start__(); "
    "void *__va_arg__(void *, ...); ";

Type * GetType(TYPE *tp, BOOLEAN commit, BOOLEAN funcarg = false, BOOLEAN pinvoke = false);


struct byName
{
    BOOLEAN operator () (const SYMBOL *left, const SYMBOL *right)
    {
        return strcmp(left->name, right->name) < 0;
    }
};
struct byLabel
{
    BOOLEAN operator () (const SYMBOL *left, const SYMBOL *right)
    {
        return left->label < right->label;
    }
};

static std::map<SYMBOL *, Value *, byName> externalMethods;
static std::map<SYMBOL *, Value *, byName> externalList;
static std::map<SYMBOL *, Value *, byName> globalMethods;
static std::map<SYMBOL *, Value *, byName> globalList;
static std::map<SYMBOL *, Value *, byLabel> staticMethods;
static std::map<SYMBOL *, Value *, byLabel> staticList;
static std::map<SYMBOL *, Param *, byName> paramList;
std::vector<Local *> localList;
static std::map<std::string, Type *> typeList;
static std::map<int, Field *> charFieldsList;
// weed out unions, structures with nested structures or bit fields
BOOLEAN qualifiedStruct(SYMBOL *sp)
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
//        if (isstructured(check->tp))
//            return FALSE;
        if (basetype(check->tp)->array)
           return FALSE;
        hr = hr->next;
    }
    return TRUE;

}
bool IsPointedStruct(TYPE *tp)
{
    while (ispointer(tp))
        tp = basetype(tp)->btp;
    return isstructured(tp);
}
Field *GetField(SYMBOL *sp)
{
    int flags = Qualifiers::Public | Qualifiers::Static;
    if (isarray(sp->tp) || IsPointedStruct(sp->tp))
        flags |= Qualifiers::ValueType;
    if (sp->storage_class != sc_localstatic && sp->storage_class != sc_constant && sp->storage_class != sc_static)
    {
        Field *field = peLib->AllocateField(sp->name, GetType(sp->tp, TRUE), flags);
        return field;
    }
    else
    {
        char buf[256];
        sprintf(buf, "L_%d", sp->label);
        Field *field = peLib->AllocateField(buf, GetType(sp->tp, TRUE), flags );
        return field;
    }
}
MethodSignature *GetMethodSignature(TYPE *tp, BOOLEAN pinvoke)
{
    int flags = pinvoke ? 0 : MethodSignature::Managed;
    while (ispointer(tp))
        tp = basetype(tp)->btp;
    SYMBOL *sp = tp->sp;
    // this compiler never uses instance members, maybe when we do C++
    HASHREC *hr = basetype(tp)->syms->table[0];
    while (hr && hr->next)
        hr = hr->next;
    if (hr && ((SYMBOL *)hr->p)->tp->type == bt_ellipse)
    {
        flags |= MethodSignature::Vararg;
    }
    MethodSignature *rv;
    if (sp->storage_class != sc_localstatic && sp->storage_class != sc_constant && sp->storage_class != sc_static)
    {
        rv = peLib->AllocateMethodSignature(sp->name, flags, pinvoke && sp->linkage != lk_msil_rtl ? NULL : mainContainer);
        if (sp->linkage2 == lk_msil_rtl)
        {
            char buf[1024];
            sprintf(buf, "[lsmsilcrtl]lsmsilcrtl.rtl::%s", sp->name);
            rv->SetFullName(buf);
        }
    }
    else
    {
        char buf[256];
        sprintf(buf, "L_%d", sp->label);
        rv = peLib->AllocateMethodSignature(buf, flags, pinvoke ? NULL : mainContainer);
    }
    if (isstructured(basetype(tp)->btp))
    {
        rv->AddReturnType(peLib->AllocateType(Type::Void, 1));
        Param * p = peLib->AllocateParam("__retblock", peLib->AllocateType(Type::Void, 1));
        rv->AddParam(p);
    }
    else
    {
        rv->AddReturnType(GetType(basetype(tp)->btp, true));
    }

    hr = basetype(tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sym = (SYMBOL *)hr->p;
        if (sym->tp->type == bt_void)
            break;
        if (sym->tp->type == bt_ellipse)
        {
            if (!pinvoke)
            {
                Param * p = peLib->AllocateParam("__va_start__", peLib->AllocateType(Type::objectArray, 0));
                rv->AddParam(p);
            }
            break;
        }
        rv->AddParam(peLib->AllocateParam(sym->name, GetType(sym->tp, true, true, pinvoke)));
        hr = hr->next;
    }
    return rv;
}
std::string GetArrayName(TYPE *tp)
{
    char end[512];
    end[0] =0;
    tp = basetype(tp);
    while (isarray(tp))
    {
        if (tp->vla)
            strcat(end, "[vla]");
        else if (tp->size == 0)
            strcat(end, "[]");
        else
            sprintf(end + strlen(end), "[%d]", tp->size);
        tp = basetype(tp->btp);
    }
    if (isstructured(tp) || basetype(tp)->type == bt_enum)
    {
        return std::string(tp->sp->name) + end;
    }
    else if (isfuncptr(tp))
    {
        return std::string("void *") + end;
    }
    else if (ispointer(tp))
    {
        return std::string("void *") + end;
    }
    else
    {
        static char * typeNames[] = { "int8", "int8", "int8", "int8", "uint8",
                "int16", "int16", "uint16", "uint16", "int32", "int32", "uint32", "int32", "uint32",
                "int64", "uint64", "float32", "float64", "float64", "float32", "float64", "float64" };
        return std::string(typeNames[basetype(tp)->type]) + end;
    }
}
Type * GetType(TYPE *tp, BOOLEAN commit, BOOLEAN funcarg, BOOLEAN pinvoke)
{
    if (isstructured(tp))
    {
        Type *type = NULL;
        std::map<std::string, Type *>::iterator it = typeList.find(basetype(tp)->sp->name);
        if (it != typeList.end())
            type = it->second;
        if (commit)
        {
            if (!type)
            {
                Class *newClass = peLib->AllocateClass(basetype(tp)->sp->name, Qualifiers::Public | Qualifiers::ClassClass |( nested ? Qualifiers::Nested : 0), 
                                       basetype(tp)->sp->structAlign, basetype(tp)->size);
                mainContainer->Add(newClass);
                type = peLib->AllocateType(newClass);
                typeList[basetype(tp)->sp->name] = type;
            }
            if (qualifiedStruct(basetype(tp)->sp) && !type->GetClass()->IsInstantiated())
            {
    
                Class *cls = (Class *)type->GetClass();
                cls->SetInstantiated();
                HASHREC *hr = basetype(tp)->syms->table[0];
                while (hr)
                {
                    SYMBOL *sym = (SYMBOL *)hr->p;
                    Type *newType = GetType(sym->tp, TRUE);
                    int flags = Qualifiers::ClassField | Qualifiers::Public;
                    if (isarray(sym->tp) || IsPointedStruct(sym->tp))
                        flags |= Qualifiers::ValueType;
                    Field *newField = peLib->AllocateField(sym->name, newType, flags);
                    cls->Add(newField);
                    hr = hr->next;
                }
            }
            return type;
        }
        else
        {
            return peLib->AllocateType((DataContainer *)NULL);
        }           
    }
    else if (basetype(tp)->type == bt_enum)
    {
        std::map<std::string, Type *>::iterator it = typeList.find(basetype(tp)->sp->name);
        if (it != typeList.end())
            return it->second;
        if (commit)
        {
            Enum *newEnum = peLib->AllocateEnum(basetype(tp)->sp->name, Qualifiers::Public | Qualifiers::EnumClass | ( nested ? Qualifiers::Nested : 0), Field::i32);
            mainContainer->Add(newEnum);

            if (basetype(tp)->syms)
            {
                HASHREC *hr = basetype(tp)->syms->table[0];
                while (hr)
                {
                    SYMBOL *sym = (SYMBOL *)hr->p;
                    newEnum->AddValue(*peLib, sym->name, sym->value.i);
                    hr = hr->next;
                }
            }
            // note we make enums ints in the file
            // this is because pinvokes with enums doesn't work...
            Type *type = peLib->AllocateType(Type::i32, 0);
            typeList[basetype(tp)->sp->name] = type;
            return type;
        }
        else
        {
            return peLib->AllocateType((DataContainer *)NULL);
        }           
    }
    else if (isarray(tp) && !funcarg)
    {
        std::string name = GetArrayName(tp);
        std::map<std::string, Type *>::iterator it = typeList.find(name);
        if (it != typeList.end())
            return it->second;
        if (commit)
        {
            Class *newClass = peLib->AllocateClass(name, Qualifiers::Public | Qualifiers::ClassClass |( nested ? Qualifiers::Nested : 0), 
                                       -1, basetype(tp)->size);
            mainContainer->Add(newClass);
            Type *type = peLib->AllocateType(newClass);
            typeList[name] = type;
            return type;
        }
        else
        {
            return peLib->AllocateType((DataContainer *)NULL);
        }
    }
    else if (isfuncptr(tp))
    {
        return peLib->AllocateType(Type::Void, 1); // pointer to void
//        tp = basetype(tp)->btp;
//        MethodSignature *sig = GetMethodSignature(basetype(tp), FALSE);
//        return peLib->AllocateType(sig);
    }
    else if (ispointer(tp))
    {

        if (pinvoke)
            return peLib->AllocateType(Type::Void, 1); // pointer to void
        TYPE *tp1 = tp;
        int level = 0;
        while (ispointer(tp1))
        {
            if (tp1->type == bt_va_list)
            {
                Type *rv = peLib->AllocateType(Type::Void, level); // pointer to void
                rv->SetFullName("class [lsmsilcrtl]lsmsilcrtl.args");
                return rv;
            }
            if (tp1->type == bt_pointer)
                level++;
            tp1 = tp1->btp;
            if (level > 0 && isarray(tp1))
                break;
        }
        // we don't currently support multiple indirections on a function pointer
        // as an independent type
        // this is a limitation of the netlib
        if (isfunction(tp1))
            return peLib->AllocateType(Type::Void, 1); // pointer to void
        Type *rv = GetType(tp1, commit);

        // can't get a method signature here
        if (rv->GetBasicType() == Type::cls)
        {
            rv = peLib->AllocateType(rv->GetClass());
        }
        rv->SetPointer(level);
        return rv;
    }
    else if (basetype(tp)->type == bt_void)
    {
        return peLib->AllocateType(Type::Void, 0);
    }
    else
    {
        static Type::BasicType typeNames[] = { Type::i8, Type::i8, Type::i8, Type::i8, Type::u8,
                Type::i16, Type::i16, Type::u16, Type::u16, Type::i32, Type::i32, Type::i32, Type::u32, Type::i32, Type::u32,
                Type::i64, Type::u64, Type::r32, Type::r64, Type::r64, Type::r32, Type::r64, Type::r64 };
        return peLib->AllocateType(typeNames[basetype(tp)->type], 0);
    }
}
static BOOLEAN validateGlobalRef(SYMBOL *sp1, SYMBOL *sp2);
static SYMBOL *clone(SYMBOL *sp)
{
    for (int i=0; i < spstackcount; i+= 2)
        if (spstack[i] == sp)
            return spstack[i+1];
    // shallow copy
    SYMBOL *rv = (SYMBOL *)peLib->AllocateBytes(sizeof(SYMBOL));
    *rv = *sp;
    spstack[spstackcount++ ] = sp;
    spstack[spstackcount++ ] = rv;
    rv->name = (char *)peLib->AllocateBytes(strlen(rv->name)+1);
    strcpy(rv->name, sp->name);
    TYPE **tp1 = &rv->tp;
    while (*tp1)
    {
        TYPE *tp2= (TYPE *)peLib->AllocateBytes(sizeof(TYPE));
        *tp2 = **tp1;
        *tp1 = tp2;
        if (isstructured(*tp1) || basetype(*tp1)->type == bt_enum || isfunction(*tp1))
        {
            basetype(*tp1)->sp = clone (basetype(*tp1)->sp);
        }
        tp1 = &(*tp1)->btp;
    }
    spstackcount-=2;
    return rv;
}
void CacheExtern(SYMBOL *sp)
{
    if (isfunction(sp->tp))
    {
        std::map<SYMBOL *, Value *, byName>::iterator it = globalMethods.find(sp);
        if (it == globalMethods.end())
        {
            it = externalMethods.find(sp);
            if (it == externalMethods.end())
            {
                sp = clone(sp);
                externalMethods[sp] = peLib->AllocateMethodName(GetMethodSignature(sp->tp, false));
            }
        }
    }
    else
    {
        std::map<SYMBOL *, Value *, byName>::iterator it = globalList.find(sp);
        if (it != globalList.end())
        {
            validateGlobalRef(sp, it->first);
        }
        else
        {
            it = externalList.find(sp);
            if (it != externalList.end())
            {
                validateGlobalRef(sp, it->first);
            }
            else
            {
                sp = clone(sp);
                externalList[sp] = peLib->AllocateFieldName(GetField(sp));
            }
        }
    }
}
void CacheGlobal(SYMBOL *sp)
{
    if (isfunction(sp->tp))
    {
        std::map<SYMBOL *, Value *, byName>::iterator it = globalMethods.find(sp);
        if (it == globalMethods.end())
        {
            it = externalMethods.find(sp);
            if (it != externalMethods.end())
                externalMethods.erase(it);
            sp = clone(sp);
            globalMethods[sp] = peLib->AllocateMethodName(GetMethodSignature(sp->tp, FALSE));
        }
        else
        {
            printf("Error: Multiple definition of %s", sp->name);
        }
    }
    else
    {
        std::map<SYMBOL *, Value *, byName>::iterator it = globalList.find(sp);
        if (it != globalList.end())
        {
            printf("Error: Multiple definition of %s", sp->name);
        }
        else
        {
            Field *f;
            it = externalList.find(sp);
            if (it != externalList.end())
            {
                validateGlobalRef(sp, it->first);
                externalList.erase(it);
                f = ((FieldName *)it->second)->GetField();
                f->SetType(GetType(sp->tp, TRUE));
            }
            else
            {
                f = GetField(sp);
            }
            sp = clone(sp);
            globalList[sp] = peLib->AllocateFieldName(f);
            mainContainer->Add(f);
        }
    }
}
void CacheStatic(SYMBOL *sp)
{
    if (isfunction(sp->tp))
    {
        std::map<SYMBOL *, Value *, byLabel>::iterator it = staticMethods.find(sp);
        if (it == staticMethods.end())
        {
            sp = clone(sp);
            staticMethods[sp] = peLib->AllocateMethodName(GetMethodSignature(sp->tp, FALSE));
        }
        else
        {
//            printf("Error: Multiple definition of %s", sp->name);
        }
    }
    else
    {
        std::map<SYMBOL *, Value *, byLabel>::iterator it = staticList.find(sp);
        if (it != staticList.end())
        {
//            printf("Error: Multiple definition of %s", sp->name);
        }
        else
        {
            Field *f = GetField(sp);
            sp = clone(sp);
            staticList[sp] = peLib->AllocateFieldName(f);
            mainContainer->Add(f);
        }
    }
}
Value *GetParamData(std::string name)
{
    SYMBOL sp = { 0 };
    sp.name = (char *)name.c_str();
    return paramList.find(&sp)->second;
}
Value *GetLocalData(SYMBOL *sp)
{
    if (sp->storage_class == sc_parameter)
    {
        return paramList.find(sp)->second;
    }
    else
    {
        return localList[sp->offset];
    }
}
Value *GetFieldData(SYMBOL *sp)
{
    if (isfunction(sp->tp))
    {
        if (sp->storage_class != sc_localstatic && sp->storage_class != sc_const && sp->storage_class != sc_static)
        {
            std::map<SYMBOL *, Value *, byName>::iterator it = globalMethods.find(sp);
            if (it == globalMethods.end())
            {
                it = externalMethods.find(sp);
                if (it == externalMethods.end())
                   return NULL;
            }
            return it->second;
        }
        else
        {
            std::map<SYMBOL *, Value *, byLabel>::iterator it = staticMethods.find(sp);
            if (it != staticMethods.end())
                return it->second;
        }
    }
    else
    {
        switch(sp->storage_class)
        {
            case sc_auto:
            case sc_register:
                return localList[sp->offset];
            case sc_parameter:
            {
                std::map<SYMBOL *, Param *, byName>::iterator it = paramList.find(sp);
                if (it != paramList.end())
                    return it->second;
            }
                break;
            case sc_static:
            case sc_localstatic:
            case sc_constant:
            {
                std::map<SYMBOL *, Value *, byLabel>::iterator it =  staticList.find(sp);
                if (it != staticList.end())
                    return it->second;
                CacheStatic(sp);
                it =  staticList.find(sp);
                if (it != staticList.end())
                    return it->second;
                break;
            }
            default:
            {
                std::map<SYMBOL *, Value *, byName>::iterator it =  globalList.find(sp);
                if (it != globalList.end())
                    return it->second;
                it =  externalList.find(sp);
                if (it != externalList.end())
                    return it->second;
                CacheExtern(sp);
                it =  externalList.find(sp);
                if (it != externalList.end())
                    return it->second;
                break;
            }
        }
    }
    return NULL;
}
void LoadLocals(SYMBOL *sp)
{
    localList.clear();
    int count = 0;
    HASHTABLE *temp = sp->inlineFunc.syms;
    LIST *lst;
    while (temp)
    {
        HASHREC *hr = temp->table[0];
        while (hr)
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (sym->storage_class == sc_auto || sym->storage_class == sc_register)
                break;
            hr = hr->next;
        }
        if (hr)
            break;
        temp = temp->next;
    }
    if (!temp)
    {
        lst = temporarySymbols;
        while (lst)
        {
            SYMBOL *sym = (SYMBOL *)lst->data;
            if (!sym->anonymous)
            {
                break;
            }
            lst = lst->next;
        }
    }
    if (lst || temp)
    {
        while (temp)
        {
            HASHREC *hr = temp->table[0];
            while (hr)
            {
                SYMBOL *sym = (SYMBOL *)hr->p;
                sym->temp = FALSE;
                hr = hr->next;
            }
            temp = temp->next;
        }
        lst = temporarySymbols;
        while (lst)
        {
            SYMBOL *sym = (SYMBOL *)lst->data;
            sym->temp = FALSE;
            lst = lst->next;
        }
        temp = theCurrentFunc->inlineFunc.syms;
        while (temp)
        {
            HASHREC *hr = temp->table[0];
            while (hr)
            {
                SYMBOL *sym = (SYMBOL *)hr->p;
                if ((sym->storage_class == sc_auto || sym->storage_class == sc_register) && !sym->temp)
                {
                    sym->temp = TRUE;
                    Type *type = GetType(sym->tp, TRUE);
                    Local *newLocal = peLib->AllocateLocal(sym->name, type);
                    localList.push_back(newLocal);
                    newLocal->SetIndex(count);
                    sym->offset = count++;
                }
                hr = hr->next;
            }
            temp = temp->next;
        }
        lst = temporarySymbols;
        while (lst)
        {
            SYMBOL *sym = (SYMBOL *)lst->data;
            if (!sym->anonymous && !sym->temp)
            {
                sym->temp= TRUE;
                Type *type = GetType(sym->tp, TRUE);
                Local *newLocal = peLib->AllocateLocal(sym->name, type);
                localList.push_back(newLocal);
                newLocal->SetIndex(count);
                sym->offset = count++;
            }
            lst = lst->next;
        }
    }
}
void LoadParams(SYMBOL *sp)
{
    int count = 0;
    paramList.clear();
    if (isstructured(basetype(sp->tp)->btp))
    {
        Param * newParam  = peLib->AllocateParam("__retblock", peLib->AllocateType(Type::Void, 1));
        newParam->SetIndex(count++);
        paramList[&retblocksym] = newParam;
    }
    HASHREC *hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sym = (SYMBOL *)hr->p;
        if (sym->tp->type == bt_void)
            break;
        Param *newParam;
        if (sym->tp->type == bt_ellipse)
        {
            newParam = peLib->AllocateParam("__va_start__", peLib->AllocateType(Type::objectArray, 0));
            sym->name = "__va_start__";
        }
        else
        {
            Type *type = GetType(sym->tp, TRUE);
            newParam = peLib->AllocateParam(sym->name, type);
        }
        newParam->SetIndex(count++);
        paramList[sym] = newParam;
        hr = hr->next;
    }
}
extern "C" void compile_start(char *name)
{
    _using_init();
    cparams.prm_asmfile = TRUE; // temporary
    staticList.clear();
}
void LoadFuncs(void)
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
extern "C" void flush_peep(SYMBOL *funcsp, QUAD *list)
{
    LoadFuncs();
    try
    {
        currentMethod->Optimize(*peLib);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: (" << funcsp->name << ") " << exc.what() << std::endl;
    }
    currentMethod = NULL;
}
//void CreateField(SYMBOL *sp)
//{
//    Field *field = GetField(sp);
//    mainContainer->Add(field);
//}
void CreateFunction(MethodSignature *sig, SYMBOL *sp)
{
    int flags = Qualifiers::ManagedFunc;
    if (sp->storage_class == sc_static)
        flags |= Qualifiers::Private;
    else
        flags |= Qualifiers::Public;
    currentMethod = peLib->AllocateMethod(sig, flags);
    mainContainer->Add(currentMethod);
    if (!strcmp(sp->name, "main"))
        if (!theCurrentFunc->parentClass && !theCurrentFunc->parentNameSpace)
            mainSym = currentMethod;

}
static void mainLocals(void)
{
    localList.clear();
    paramList.clear();
    localList.push_back(peLib->AllocateLocal("argc", peLib->AllocateType(Type::i32, 0)));
    localList.push_back(peLib->AllocateLocal("argv", peLib->AllocateType(Type::Void, 1)));
    localList.push_back(peLib->AllocateLocal("environ", peLib->AllocateType(Type::Void, 1)));
    localList.push_back(peLib->AllocateLocal("newmode", peLib->AllocateType(Type::Void, 1)));
}
static void mainInit(void)
{
    std::string name = "$Main";
    int flags = Qualifiers::Private | Qualifiers::HideBySig | Qualifiers::Static | Qualifiers::CIL | Qualifiers::Managed;
    if (prm_targettype == DLL)
    {
        name = ".cctor";
        flags |= Qualifiers::SpecialName | Qualifiers::RTSpecialName;
    }
    MethodSignature *signature = peLib->AllocateMethodSignature(name, MethodSignature::Managed, mainContainer);
    signature->AddReturnType(peLib->AllocateType(Type::Void, 0));
    currentMethod = peLib->AllocateMethod(signature, flags, prm_targettype != DLL);
    mainContainer->Add(currentMethod);

    signature = peLib->AllocateMethodSignature("__pctype_func", 0, NULL);
    signature->AddReturnType(peLib->AllocateType(Type::u16, 1));
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));

    Field *field = peLib->AllocateField("_pctype", peLib->AllocateType(Type::u16, 1), Qualifiers::Public | Qualifiers::Static);
    peLib->Add(field);
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));

    signature = peLib->AllocateMethodSignature("__iob_func", 0, NULL);
    signature->AddReturnType(peLib->AllocateType(Type::Void, 1));
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));

    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_dup));
    field = peLib->AllocateField("__stdin", peLib->AllocateType(Type::Void, 1), Qualifiers::Public | Qualifiers::Static);
    peLib->Add(field);
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));

    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_dup));
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldc_i4, peLib->AllocateOperand((longlong)32, Operand::any)));
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_add));

    field = peLib->AllocateField("__stdout", peLib->AllocateType(Type::Void, 1), Qualifiers::Public | Qualifiers::Static);
    peLib->Add(field);
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));

    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldc_i4, peLib->AllocateOperand((longlong)64, Operand::any)));
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_add));

    field = peLib->AllocateField("__stderr", peLib->AllocateType(Type::Void, 1), Qualifiers::Public | Qualifiers::Static);
    peLib->Add(field);
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));
                          
}
static void dumpInitializerCalls(LIST *lst)
{
    while (lst)
    {
        MethodSignature *signature = peLib->AllocateMethodSignature((char *)lst->data, MethodSignature::Managed, mainContainer);
        signature->AddReturnType(peLib->AllocateType(Type::Void, 0));
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));
        lst = lst->next;
    }
}
static void dumpCallToMain(void)
{
    if (prm_targettype != DLL)
    {
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldloca, peLib->AllocateOperand(localList[0]))); // load argc
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldloca, peLib->AllocateOperand(localList[1]))); // load argcv
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldloca, peLib->AllocateOperand(localList[2]))); // load environ
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldc_i4, peLib->AllocateOperand((longlong)0, Operand::i32)));
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldloca, peLib->AllocateOperand(localList[3]))); // load newmode

        MethodSignature *signature = peLib->AllocateMethodSignature("__getmainargs", 0, NULL);
        signature->AddReturnType(peLib->AllocateType(Type::Void, 0));
        signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::Void, 1)));
        signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::Void, 1)));
        signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::Void, 1)));
        signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::i32, 0)));
        signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::Void, 1)));
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));

        if (mainSym)
        {
            int n = mainSym->GetSignature()->GetParamCount();
            if (n >= 1)
                currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldloc, peLib->AllocateOperand(localList[0]))); // load argc
            if (n >= 2)
                currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldloc, peLib->AllocateOperand(localList[1]))); // load argcv
            for (int i=2; i < n; i++)
                currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldnull, NULL)); // load a spare arg
            signature = peLib->AllocateMethodSignature("main", MethodSignature::Managed, mainContainer);
            currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(mainSym->GetSignature()))));
        }
        dumpInitializerCalls(deinitializersHead);

        if (mainSym && mainSym->GetSignature()->GetReturnType()->IsVoid())
            currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldc_i4, peLib->AllocateOperand((longlong)0, Operand::i32))); 
        signature = peLib->AllocateMethodSignature("exit", 0, NULL);
            signature->AddReturnType(peLib->AllocateType(Type::Void, 0));
        signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::i32, 0)));
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));
    }
}
static void dumpGlobalFuncs()
{
    std::string name = "__GetErrno";
    int flags = Qualifiers::Private | Qualifiers::HideBySig | Qualifiers::Static | Qualifiers::CIL | Qualifiers::Managed;

    MethodSignature *signature = peLib->AllocateMethodSignature(name, MethodSignature::Managed, mainContainer);
    signature->AddReturnType(peLib->AllocateType(Type::i32, 1));
    currentMethod = peLib->AllocateMethod(signature, flags);
    mainContainer->Add(currentMethod);

    signature = peLib->AllocateMethodSignature("_errno", 0, NULL);
    signature->AddReturnType(peLib->AllocateType(Type::i32, 1));
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ret));
    currentMethod->Optimize(*peLib);
}

static void AddRTLThunks()
{
    Param *param;
    if (mainSym)
    {
        if (mainSym->GetSignature()->GetParamCount() < 1)
        {
            param = peLib->AllocateParam("argc", peLib->AllocateType(Type::i32, 0));
            mainSym->GetSignature()->AddParam(param);
        }
        if (mainSym->GetSignature()->GetParamCount() < 2)
        {
            param = peLib->AllocateParam("argv", peLib->AllocateType(Type::Void, 1));
            mainSym->GetSignature()->AddParam(param);
        }
    }
    oa_enterseg(0);

    mainInit();
    dumpInitializerCalls(initializersHead);
    mainLocals();
    dumpCallToMain();

    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ret));
    for (int i=0; i < localList.size(); i++)
        currentMethod->AddLocal(localList[i]);
    currentMethod->Optimize(*peLib);

    dumpGlobalFuncs();
}
static void CreateExternalCSharpReferences()
{
//        case am_argit_args:
//            bePrintf("\t'__varargs__'");
//            break;
    argsCtor = peLib->AllocateMethodSignature(".ctor", MethodSignature::Instance, NULL);
    argsCtor->SetFullName("[lsmsilcrtl]lsmsilcrtl.args::.ctor");
    argsCtor->AddReturnType(peLib->AllocateType(Type::Void, 0));
    argsCtor->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::objectArray, 0))); 
    argsNextArg = peLib->AllocateMethodSignature("GetNextArg", MethodSignature::Instance, NULL);
    argsNextArg->SetFullName("[lsmsilcrtl]lsmsilcrtl.args::GetNextArg");
    argsNextArg->AddReturnType(peLib->AllocateType(Type::object, 0));
    argsUnmanaged = peLib->AllocateMethodSignature("GetUnmanaged", MethodSignature::Instance, NULL);
    argsUnmanaged->SetFullName("[lsmsilcrtl]lsmsilcrtl.args::GetUnmanaged");
    argsUnmanaged->AddReturnType(peLib->AllocateType(Type::Void, 1));
    ptrBox = peLib->AllocateMethodSignature("box", 0, NULL);
    ptrBox->SetFullName("[lsmsilcrtl]lsmsilcrtl.pointer::'box'");
    ptrBox->AddReturnType(peLib->AllocateType(Type::object, 0));
    ptrBox->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::Void, 1))); 
    ptrUnbox = peLib->AllocateMethodSignature("unbox", 0, NULL);
    ptrUnbox->SetFullName("[lsmsilcrtl]lsmsilcrtl.pointer::'unbox'");
    ptrUnbox->AddReturnType(peLib->AllocateType(Type::Void, 1));
    ptrUnbox->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::object, 0))); 
    
    systemObject = peLib->AllocateType(Type::object, 0);
    systemObject->SetFullName("[mscorlib]System.Object");

}
extern "C" BOOLEAN oa_main_preprocess(void)
{
    _apply_global_using();

    PELib::CorFlags corFlags = PELib::bits32;
    if (namespaceAndClass[0])
        corFlags |= PELib::ilonly;
    char path[260], fileName[256];
    GetOutputFileName(fileName, path);
    char *p = strrchr(fileName, '.');
    char *q = strrchr(fileName, '\\');
    if (!q)
        q = fileName;
    else
        q++;
    if (p)
    {
        *p = 0;
    }
    peLib = new PELib(q, corFlags);
    if (p)
    {
        *p = '.';
    }
    if (namespaceAndClass[0])
    {
        p = strchr(namespaceAndClass, '.');
        *p = 0;
        Namespace *nm = peLib->AllocateNamespace(namespaceAndClass);
        peLib->Add(nm);
        Class *cls = peLib->AllocateClass(p + 1, Qualifiers::MainClass | Qualifiers::Public, -1, -1);
        nm->Add(cls);
        mainContainer = cls;
        nested = true;
    }
    else
    {
        mainContainer = peLib;
    }

    peLib->AddExternalAssembly("mscorlib");
    peLib->AddExternalAssembly("lsmsilcrtl");
    CreateExternalCSharpReferences();
    retblocksym.name = "__retblock";
    return FALSE;
}
static BOOLEAN checkExterns(void)
{
    BOOLEAN rv = FALSE;
    for (std::map<SYMBOL *, Value *, byName>::iterator it = externalMethods.begin();
        it !=externalMethods.end(); ++it)
    {
        printf ("Error: Undefined external symbol %s\n", it->first->name);
        rv = TRUE;
    }
    for (std::map<SYMBOL *, Value *, byName>::iterator it = externalList.begin();
        it != externalList.end(); ++it)
    {
        char *name = it->first->name;
        if (strcmp(name, "_pctype") && strcmp(name, "__stdin") && 
           strcmp(name, "__stdout") && strcmp(name, "__stderr"))
        {
            printf ("Error: Undefined external symbol %s\n", name);
            rv = TRUE;
        }
    }
    return rv;
}
static BOOLEAN validateGlobalRef(SYMBOL *sp1, SYMBOL *sp2)
{
    if (!isfunction(sp1->tp) && !isfunction(sp2->tp))
    {
        TYPE *tp = sp1->tp;
        TYPE *tpx = sp2->tp;
        tp = basetype(tp);
        tpx = basetype(tpx);
        while (ispointer(tp) && ispointer(tpx))
        {
            if (tpx->size != 0 && tp->size != 0 && tp->size != tpx->size)
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
                        return TRUE;
                }
                else
                {
                    return TRUE;
                }
            }
        }
        errCount++;
        printf("Error: Mismatch global type declarations on %s\n", sp1->name);
    }
    return FALSE;
}
extern "C" void oa_main_postprocess(BOOLEAN errors)
{
    AddRTLThunks();
    if (!errors && prm_targettype != DLL && !mainSym)
    {
        printf("Error: main not defined\n");
    }
    errors |= checkExterns() || errCount || prm_targettype != DLL && !mainSym;
    if (!errors)
    {
        char path[260];
        char ilName[260];
        GetOutputFileName(ilName, path);
        StripExt(ilName);
        AddExt(ilName, ".il");
        peLib->DumpOutputFile(ilName, PELib::ilasm);
    }
}
extern "C" void oa_end_generation(void)
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
        LIST *lst = (LIST *)peLib->AllocateBytes(sizeof(LIST));
        lst->data = peLib->AllocateBytes(strlen(start->name)+1);
        strcpy((char *)lst->data, start->name);
        if (initializersHead)
            initializersTail = initializersTail->next = lst;
        else
            initializersHead = initializersTail = lst;
    }
    if (end)
    {
        LIST *lst = (LIST *)peLib->AllocateBytes(sizeof(LIST));
        lst->data = peLib->AllocateBytes(strlen(end->name)+1);
        strcpy((char *)lst->data, end->name);
        if (deinitializersHead)
            deinitializersTail = deinitializersTail->next = lst;
        else
            deinitializersHead = initializersTail = lst;
    }
}
extern "C" void oa_put_extern(SYMBOL *sp, int code)
{
    if (isfunction(sp->tp))
    {
        if (!msil_managed(sp))
        {
            if (isfunction(sp->tp))
            {
                if (globalMethods.find(sp) != globalMethods.end())
                    return;
            }
            else
            {
                if (globalList.find(sp) != globalList.end())
                    return;
            }
            CacheGlobal(sp);
            std::string dllname = _dll_name(sp->name);
            peLib->AddPInvokeReference(GetMethodSignature(sp->tp, TRUE), dllname, sp->linkage != lk_stdcall);
        }
        else if (sp->linkage2 != lk_msil_rtl)
        {
            CacheExtern(sp);
        }
    }
    else {
        CacheExtern(sp);
    }
}
extern "C" void oa_gen_strlab(SYMBOL *sp)
/*
 *      generate a named label.
 */
{
    oa_enterseg(0);
    if (sp->storage_class != sc_localstatic && sp->storage_class != sc_constant && sp->storage_class != sc_static)
    {
        CacheGlobal(sp);
    }
    else
    {
        CacheStatic(sp);
    }
    if (isfunction(sp->tp))
    {
        CreateFunction(GetMethodSignature(sp->tp, FALSE), sp);
    }
//    else
//    {
//        CreateField(sp);
//    }
}
Type * GetStringType(int type)
{
    std::string name;
    switch(type)
    {
        case lexeme::l_ustr:
        case lexeme::l_astr:
            name = "int8[]";
            break;
        case lexeme::l_Ustr:
            name = "int32[]";
            break;
        case lexeme::l_wstr:
            name = "int16[]";
            break;
    }
    std::map<std::string, Type *>::iterator it = typeList.find(name);
    if (it != typeList.end())
        return it->second;
    Class *newClass = peLib->AllocateClass(name, Qualifiers::Private | Qualifiers::Value | Qualifiers::Explicit | Qualifiers::Ansi | Qualifiers::Sealed |( nested ? Qualifiers::Nested : 0), 
                               1, 1);
    mainContainer->Add(newClass);
    Type *type2 = peLib->AllocateType(newClass);
    typeList[name] = type2;
    return type2;
}
void oa_put_string_label(int lab, int type)
{
    oa_enterseg(0);
    char buf[256];
    sprintf(buf, "L_%d", lab);
    Field *field = peLib->AllocateField(buf, GetStringType(type), Qualifiers::ClassField | Qualifiers::ValueType | Qualifiers::Private | Qualifiers::Static);
    mainContainer->Add(field);
    initializingField = field;
    dataPos = 0;
}
/*-------------------------------------------------------------------------*/
// we should only get here for strings...
void oa_genbyte(int bt)
{
    if (dataMax == 0)
    {
        dataMax = 100;
        dataPointer = (BYTE *)realloc(dataPointer, dataMax);
    }
    else if (dataPos >= dataMax)
    {
        dataMax *= 2;
        dataPointer = (BYTE *)realloc(dataPointer, dataMax);
    }
    if (!dataPointer)
        fatal("out of memory");
    dataPointer[dataPos++] = bt;
}

/*-------------------------------------------------------------------------*/

void oa_genstring(LCHAR *str, int len)
/*
 * Generate a string literal
 */
{
    if (cparams.prm_asmfile)
    {
        int nlen = len;
        while (nlen--)
        {
            oa_genbyte(*str++);
        }
    }
}

void oa_enterseg(enum e_sg seg)
{
    if (initializingField && dataPos)
    {
        oa_genbyte(0); // we only put strings literally into the text, and the strings
        // need a terminating zero which has been elided because we don't do anything else
        BYTE *v = peLib->AllocateBytes(dataPos);
        memcpy(v, dataPointer, dataPos);
        initializingField->AddInitializer(v, dataPos);
    }
    initializingField = NULL;
    dataPos = 0;
}
