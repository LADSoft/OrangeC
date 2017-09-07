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

#define STARTUP_TYPE_STARTUP 1
#define STARTUP_TYPE_RUNDOWN 2
#define STARTUP_TYPE_TLS_STARTUP 3
#define STARTUP_TYPE_TLS_RUNDOWN 4



#include <vector>
#include <string>
using namespace DotNetPELib;

extern void Import();
BoxedType *boxedType(int isz);

extern "C" {

    extern SYMBOL *theCurrentFunc;
    extern COMPILER_PARAMS cparams;
    extern TYPE stdbool, stdchar, stdunsignedchar, stdshort, stdunsignedshort, stdint, stdunsigned;
    extern TYPE stdlonglong, stdunsignedlonglong, stdinative, stdunative, stdfloat, stddouble, stdstring;
    extern TYPE std__string;
    extern LIST *externals;
    extern int prm_targettype;
    extern char namespaceAndClass[512];
    extern LIST *temporarySymbols;
    extern char prm_snkKeyFile[260];
    extern int assemblyVersion[4];
    extern LIST *objlist;
    extern char *pinvoke_dll;
    extern NAMESPACEVALUES *globalNameSpace;
    extern BOOLEAN managed_library;
    extern BOOLEAN no_default_libs;

    MethodSignature *argsCtor;
    MethodSignature *argsNextArg;
    MethodSignature *argsUnmanaged;
    MethodSignature *ptrBox;
    MethodSignature *ptrUnbox;
    MethodSignature *concatStr;
    MethodSignature *concatObj;
    MethodSignature *toStr;
    Type *systemObject;
    Method *currentMethod;
    PELib *peLib;
    DataContainer *mainContainer;

    char msil_bltins[] = " void exit(int); "
        "void __getmainargs(void *,void *,void*,int, void *); "
        "void *__iob_func(); "
        "unsigned short *__pctype_func(); "
        "int *_errno(); "
        "void *__OCCMSIL_GetProcThunkToManaged(void *proc, void *pdata); "
        "void *__OCCMSIL_GetProcThunkToUnmanaged(void *proc); "
        "void *__va_start__(); "
        "void *__va_arg__(void *, ...); ";

}
std::string _dll_name(char *name);
int uniqueId;
static SYMBOL retblocksym;

static Method *mainSym;

static int hasEntryPoint;
static int errCount;
static LIST *initializersHead, *initializersTail;
static LIST *deinitializersHead, *deinitializersTail;
static int dataPos, dataMax;
static BYTE *dataPointer;
static Field *initializingField;
static char objFileName[260];

Type * GetType(TYPE *tp, BOOLEAN commit, BOOLEAN funcarg = false, BOOLEAN pinvoke = false);
static SYMBOL *clone(SYMBOL *sp, bool ctype = true);
void CacheExtern(SYMBOL *sp);
void CacheGlobal(SYMBOL *sp);
void CacheStatic(SYMBOL *sp);


struct byName
{
    BOOLEAN operator () (const SYMBOL *left, const SYMBOL *right) const
    {
        return strcmp(left->name, right->name) < 0;
    }
};
struct byLabel
{
    BOOLEAN operator () (const SYMBOL *left, const SYMBOL *right) const
    {
        return left->label < right->label;
    }
};
struct byField
{
    BOOLEAN operator () (const SYMBOL *left, const SYMBOL *right) const
    {
        int n = strcmp(left->parentClass->name, right->parentClass->name);
        if (n < 0)
        {
            return true;
        }
        else if (n > 0)
        {
            return false;
        }
        else
        {
            return strcmp(left->name, right->name) < 0;
        }
    }
};
static std::map<SYMBOL *, Value *, byName> externalMethods;
static std::map<SYMBOL *, Value *, byName> externalList;
static std::map<SYMBOL *, Value *, byName> globalMethods;
static std::map<SYMBOL *, Value *, byName> globalList;
static std::map<SYMBOL *, Value *, byLabel> staticMethods;
static std::map<SYMBOL *, Value *, byLabel> staticList;
static std::map<SYMBOL *, MethodSignature *, byName> pinvokeInstances;
static std::map<SYMBOL *, Param *, byName> paramList;
std::multimap<std::string, MethodSignature *> pInvokeReferences;

std::map<std::string, Value *> startups, rundowns, tlsstartups, tlsrundowns;

extern "C"
{
    std::vector<Local *> localList;
}
static std::map<std::string, Type *> typeList;
static std::map<SYMBOL *, Value *, byField> fieldList;
static std::map<std::string, MethodSignature *> arrayMethods;

MethodSignature *LookupArrayMethod(Type *tp, std::string name)
{
    char buf[256];
    sprintf(buf, "$$$%d;%d;%s", tp->ArrayLevel(), tp->GetBasicType(), name.c_str());
    auto it = arrayMethods.find(buf);
    if (it != arrayMethods.end())
       return it->second;
    Type *tp1 = peLib->AllocateType(Type::i32, 0);
    int n = tp->ArrayLevel();
    MethodSignature *sig = peLib->AllocateMethodSignature(name, MethodSignature::Managed | MethodSignature::InstanceFlag, mainContainer);
    sig->ArrayObject(tp);
    for (int i = 0; i < n; i++)
    {
        char buf[256];
        sprintf(buf, "p%d", i);
        sig->AddParam(peLib->AllocateParam(buf, tp1));
    }
    if (name == ".ctor")
    {
        sig->ReturnType(peLib->AllocateType(Type::Void, 0));
    }
    else if (name == "Get")
    {
        Type *tp2 = peLib->AllocateType(tp->GetBasicType(), 0); 
        sig->ReturnType(tp2);
    }
    else if (name == "Set")
    {
        Type *tp2 = peLib->AllocateType(tp->GetBasicType(), 0); 
        sig->ReturnType(peLib->AllocateType(Type::Void, 0));
        char buf[256];
        sprintf(buf, "p$$");
        sig->AddParam(peLib->AllocateParam(buf, tp2));
    }
    arrayMethods[name] = sig;
    return sig;
}
static MethodSignature * FindMethodSignature(char *name)
{
    void *result;
    if (peLib->Find(name, &result) == PELib::s_method)
    {
        return static_cast<Method *>(result)->Signature();
    }
    fatal("could not find built in method %s", name);
    return NULL;
}
static Type *FindType(char *name)
{
    void *result;
    if (peLib->Find(name, &result) == PELib::s_class)
    {
        return peLib->AllocateType(static_cast<Class *>(result));
    }
    fatal("could not find built in type %s", name);
    return NULL;
}

// weed out structures with nested structures or bit fields
BOOLEAN qualifiedStruct(SYMBOL *sp)
{
//    HASHREC *hr;
    if (!sp->tp->size)
        return FALSE;
#if 0
    if (sp->tp->type == bt_union)
        return FALSE;
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *check = (SYMBOL *)hr->p;
        if (basetype(check->tp)->bits)
            return FALSE;
        //        if (isstructured(check->tp))
        //            return FALSE;
//        if (basetype(check->tp)->array)
//            return FALSE;
        hr = hr->next;
    }
#endif
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
    if (sp->storage_class != sc_localstatic && sp->storage_class != sc_constant && sp->storage_class != sc_static)
    {
        Field *field = peLib->AllocateField(sp->name, GetType(sp->tp, TRUE), flags);
        return field;
    }
    else
    {
        char buf[256];
        sprintf(buf, "L_%d_%x", sp->label, uniqueId);
        Field *field = peLib->AllocateField(buf, GetType(sp->tp, TRUE), flags);
        return field;
    }
}
MethodSignature *GetMethodSignature(TYPE *tp, bool pinvoke)
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
        if (sp->linkage2 == lk_msil_rtl)
        {            
            char buf[1024];
            void *result;
            // in lsmsilcrtl
            sprintf(buf, "lsmsilcrtl.rtl.%s", sp->name);
            if (peLib->Find(buf, &result) == PELib::s_method)
            {
                rv = static_cast<Method *>(result)->Signature();
		return rv;
            }
            else
            {
                fatal("missing rtl function: %s", buf);
            }
        }
        else
        {
            rv = peLib->AllocateMethodSignature(sp->name, flags, pinvoke && sp->linkage != lk_msil_rtl ? NULL : mainContainer);
        }
    }
    else
    {
        char buf[256];
        sprintf(buf, "L_%d_%x", sp->label, uniqueId);
        rv = peLib->AllocateMethodSignature(buf, flags, pinvoke ? NULL : mainContainer);
    }
    if (isstructured(basetype(tp)->btp) && (sp->linkage2 == lk_unmanaged || !msil_managed(sp)))
    {
        rv->ReturnType(peLib->AllocateType(Type::Void, 1));
        Param * p = peLib->AllocateParam("__retblock", peLib->AllocateType(Type::Void, 1));
        rv->AddParam(p);
    }
    else
    {
        rv->ReturnType(GetType(basetype(tp)->btp, true));
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
                Type *oa = peLib->AllocateType(Type::object, 0);
                oa->ArrayLevel(1);
                Param * p = peLib->AllocateParam("__va_list__", oa);
                rv->AddParam(p);
            }
            break;
        }
        rv->AddParam(peLib->AllocateParam(sym->name, GetType(sym->tp, true, true, pinvoke)));
        hr = hr->next;
    }
    return rv;
}
MethodSignature *FindPInvokeWithVarargs(std::string name, std::list<Param *>::iterator vbegin, std::list<Param *>::iterator vend, size_t size)
{
    auto range = pInvokeReferences.equal_range(name);
    for (auto it = range.first; it != range.second; ++it)
    {
        if (size == (*it).second->VarargParamCount())
        {
            auto it1 = (*it).second->vbegin();
            auto it2 = vbegin;
            while (it2 != vend)
            {
                if (!(*it2)->GetType()->Matches((*it1)->GetType()))
                    break;
                ++it1;
                ++it2;
            }
            if (it2 == vend)
                return (*it).second;
        }
    }
    return nullptr;
}
MethodSignature *GetMethodSignature(SYMBOL *sp)
{
    BOOLEAN pinvoke = false;
    if (sp)
        pinvoke = !msil_managed(sp);
    std::map<SYMBOL *, Value *, byName>::iterator it = globalMethods.find(sp);
    if (it != globalMethods.end())
    {
        return static_cast<MethodName *>(it->second)->Signature();
    }
    it = externalMethods.find(sp);
    if (it != externalMethods.end())
    {
        return static_cast<MethodName *>(it->second)->Signature();
    }
    if (pinvoke)
    {
        std::map<SYMBOL *, MethodSignature *, byName>::iterator it1 = pinvokeInstances.find(sp);
        if (it1 != pinvokeInstances.end())
        {
            // if we get here we have a pinvoke instance.   If it isnt vararg just return it
            if (!(it1->second->Flags() & MethodSignature::Vararg))
                return it1->second;
            // vararg create a new instance
            // duplicate matching will be done in the caller
            MethodSignature *sig = GetMethodSignature(sp->tp, true);
            sig->SignatureParent(it1->second->SignatureParent());
            return sig;
        }
        else
        {
            // no current pinvoke instance, create a new one
            MethodSignature *parent = GetMethodSignature(sp->tp, true);
            peLib->AddPInvokeReference(parent, _dll_name(sp->name), sp->linkage != lk_stdcall);
            sp = clone(sp);
            if (parent->Flags() & MethodSignature::Vararg)
            {
                MethodSignature *sig = GetMethodSignature(sp->tp, true);
                sig->SignatureParent(parent);
                pinvokeInstances[sp] = sig;
                return sig;
            }
            else
            {
                pinvokeInstances[sp] = parent;
                return parent;
            }
        }
    }
    else if (!isfunction(sp->tp))
    {
        // function pointer is here
        // we aren't caching these aggressively...
        return GetMethodSignature(sp->tp, false);

    }
    else if (sp->storage_class == sc_external)
    {
        CacheExtern(sp);
        it = externalMethods.find(sp);
        return static_cast<MethodName *>(it->second)->Signature();
    }
    else if (sp->storage_class == sc_static)
    {
        CacheStatic(sp);
        std::map<SYMBOL *, Value *, byLabel>::iterator it = staticMethods.find(sp);
        return static_cast<MethodName *>(it->second)->Signature();
    }
    else
    {
        CacheGlobal(sp);
        it = globalMethods.find(sp);
        return static_cast<MethodName *>(it->second)->Signature();
    }
}
std::string GetArrayName(TYPE *tp)
{
    char end[512];
    end[0] = 0;
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
                "int16", "int16", "uint16", "uint16", "int32", "native int", "int32", "uint32", "native unsigned int", "uint32",
                "int64", "uint64", "float32", "float64", "float64", "float32", "float64", "float64" };
        return std::string(typeNames[basetype(tp)->type]) + end;
    }
}
Value *GetStructField(SYMBOL *sp)
{
    if (sp->msil)
        return peLib->AllocateFieldName(static_cast<Field *>(sp->msil));
    auto it = fieldList.find(sp);
    if (it == fieldList.end() && sp->parentClass)
        GetType(sp->parentClass->tp, true);
    it = fieldList.find(sp);
    return it->second;
}
TYPE *oa_get_boxed(TYPE *in)
{
    static char * typeNames[] = { "int8", "Bool", "Int8", "Int8", "UInt8",
        "Int16", "Int16", "UInt16", "UInt16", "Int32", "Int32", "IntPtr", "Int32", "UInt32", "UIntPtr", "Int32", "UInt32",
        "Int64", "UInt64", "Single", "Double", "Double", "Single", "Double", "Double", "", "", "", "", "", "String" };
    if (basetype(in)->type < sizeof(typeNames) / sizeof(typeNames[0]))
    {
        SYMBOL *sym = search("System", globalNameSpace->syms);
        if (sym && sym->storage_class == sc_namespace)
        {
            SYMBOL *sym2 = search(typeNames[basetype(in)->type], sym->nameSpaceValues->syms);
            if (sym2)
                return sym2->tp;
        }
    }
    return nullptr;
}
TYPE *oa_get_unboxed(TYPE *in)
{
    if (isstructured(in))
    {
        in = basetype(in);
        if (in->sp->parentNameSpace && !in->sp->parentClass && !strcmp(in->sp->parentNameSpace->name, "System"))
        {
            char *name = in->sp->name;
            static char *typeNames[] = { "Bool", "Char", "Int8", "UInt8",
                "Int16", "UInt16", "Int32", "UInt32",
                "Int64", "UInt64", "IntPtr", "UIntPtr", "Single", "Double", "String"
            };
            static TYPE *typeVals[] = { &stdbool, &stdchar, &stdchar, &stdunsignedchar,
                &stdshort, &stdunsignedshort, &stdint, &stdunsigned,
                &stdlonglong, &stdunsignedlonglong, &stdinative, &stdunative, &stdfloat, &stddouble, &std__string };
            for (int i = 0; i < sizeof(typeNames) / sizeof(typeNames[0]); i++)
                if (!strcmp(typeNames[i], name))
                {
                    return typeVals[i];
                }
        }
    }
    return nullptr;
}
void msil_create_property(SYMBOL *property, SYMBOL *getter, SYMBOL *setter)
{
    if (typeid(*mainContainer) == typeid(Class))
    {
        Property *p = peLib->AllocateProperty(*peLib, property->name, GetType(property->tp, TRUE, FALSE, FALSE), std::vector<Type*> {}, !!setter, mainContainer);
        p->Instance(false); // only doing statics for now...
        static_cast<Class *>(mainContainer)->Add(p);
        property->msil = (void *)p;
    }
    else
    {
        fatal("Cannot add property at non-class level");
    }
}
void AddType(SYMBOL *sym, Type *type)
{
    typeList[sym->decoratedName] = type;
}
Type * GetType(TYPE *tp, BOOLEAN commit, BOOLEAN funcarg, BOOLEAN pinvoke)
{
    BOOLEAN byref = FALSE;
    if (isref(tp))
    {
        byref = TRUE;
        tp = basetype(tp)->btp;
    }
    if (isstructured(tp))
    {
        Type *type = NULL;
        std::map<std::string, Type *>::iterator it = typeList.find(basetype(tp)->sp->decoratedName);
        if (it != typeList.end())
            type = it->second;
        if (commit)
        {
            if (!type)
            {
                Class *newClass = peLib->AllocateClass(basetype(tp)->sp->name, Qualifiers::Public | (basetype(tp)->type == bt_union ? Qualifiers::ClassUnion : Qualifiers::ClassClass),
                    basetype(tp)->sp->structAlign, basetype(tp)->size);
                mainContainer->Add(newClass);
                type = peLib->AllocateType(newClass);
                typeList[basetype(tp)->sp->decoratedName] = type;
                basetype(tp)->sp->msil = (void *)newClass;
            }
            else
            {
                basetype(tp)->sp->msil = (void *)type->GetClass();
                type = peLib->AllocateType(type->GetClass());
            }
            if (!type->GetClass()->InAssemblyRef())
            {
                if (basetype(tp)->size != 0)
                {
                    Class *cls = (Class *)type->GetClass();
                    cls->size(basetype(tp)->size);
                    cls->pack(basetype(tp)->sp->structAlign);
                }
                if (qualifiedStruct(basetype(tp)->sp) && !type->GetClass()->IsInstantiated())
                {

                    Class *cls = (Class *)type->GetClass();
                    cls->SetInstantiated();
                    HASHREC *hr = basetype(tp)->syms->table[0];
                    Field *bitField = nullptr;
                    int start = 1000;
                    while (hr)
                    {
                        SYMBOL *sym = (SYMBOL *)hr->p;
                        if (!sym->tp->bits || sym->tp->startbit < start)
                        {
                            Type *newType = GetType(sym->tp, TRUE);
                            int flags = Qualifiers::ClassField | Qualifiers::Public;
                            char buf[256];
                            if (sym->tp->bits)
                            {
                                start = sym->tp->startbit;
                                sprintf(buf, "$$bits%d", sym->offset);
                            }
                            Field *newField = peLib->AllocateField(sym->tp->bits ? buf : sym->name, newType, flags);
                            newField->SetContainer(cls);
                            cls->Add(newField);
                            sym = clone(sym);
                            sym->parentClass = clone(sym->parentClass);
                            fieldList[sym] = peLib->AllocateFieldName(newField);
                            bitField = newField;
                        }
                        else
                        {
                            start = sym->tp->startbit;
                            sym = clone(sym);
                            sym->parentClass = clone(sym->parentClass);
                            fieldList[sym] = peLib->AllocateFieldName(bitField);
                        }
                        hr = hr->next;
                    }
                }
            }
            type->ByRef(byref);
            return type;
        }
        else
        {
            return peLib->AllocateType((DataContainer *)NULL);
        }
    }
    else if (basetype(tp)->type == bt_enum)
    {
        std::map<std::string, Type *>::iterator it = typeList.find(basetype(tp)->sp->decoratedName);
        if (it != typeList.end())
            return it->second;
        if (commit)
        {
            Enum *newEnum = peLib->AllocateEnum(basetype(tp)->sp->name, Qualifiers::Public | Qualifiers::EnumClass, Field::i32);
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
            typeList[basetype(tp)->sp->decoratedName] = type;
            return type;
        }
        else
        {
            return peLib->AllocateType((DataContainer *)NULL);
        }
    }
    else if (isarray(tp) && basetype(tp)->msil)
    {
        char name[512];
        TYPE *base = tp;
        Type *rv;
        int count = 0;
        while (isarray(base))
        {
            count++;
            base = basetype(base)->btp;
        }
        sprintf(name, "$$$marray%d;%s;%d", base->type, base->sp ? base->sp->name : "", count);
        std::map<std::string, Type *>::iterator it = typeList.find(name);
        if (it != typeList.end())
            return it->second;
        if (commit)
        { 
            rv = GetType(base, TRUE);
            rv->ArrayLevel(count);
            typeList[name] = rv;
            if (isstructured(base))
                GetType(base, true);
            rv->ByRef(byref);
            return rv;
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
            Class *newClass = peLib->AllocateClass(name, Qualifiers::Public | Qualifiers::ClassClass,
                -1, basetype(tp)->size == 0 ? 1 : basetype(tp)->size);
            mainContainer->Add(newClass);
            Type *type = peLib->AllocateType(newClass);
            typeList[name] = type;
            while (ispointer(tp))
                tp = basetype(tp)->btp;
            // declare any structure we are referencing..
            if (isstructured(tp))
                GetType(tp, true);
            type->ByRef(byref);
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
//        MethodSignature *sig = GetMethodSignature(basetype(tp), false);
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
                Type *rv = FindType("lsmsilcrtl.args");
                rv->PointerLevel(level);
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
        rv->PointerLevel(level);
        rv->ByRef(byref);
        return rv;
    }
    else if (basetype(tp)->type == bt_void)
    {
        return peLib->AllocateType(Type::Void, 0);
    }
    else if (ismsil(tp))
    {
        if (basetype(tp)->type == bt___string)
            return peLib->AllocateType(Type::string, 0);
        else
            return peLib->AllocateType(Type::object, 0);
    }
    else
    {
        static Type::BasicType typeNames[] = { Type::i8, Type::i8, Type::i8, Type::i8, Type::u8,
                Type::i16, Type::i16, Type::u16, Type::u16, Type::i32, Type::i32, Type::inative, Type::i32, Type::u32, Type::unative, Type::i32, Type::u32,
                Type::i64, Type::u64, Type::r32, Type::r64, Type::r64, Type::r32, Type::r64, Type::r64 };
        Type *rv = peLib->AllocateType(typeNames[basetype(tp)->type], 0);
        rv->ByRef(byref);
        return rv;
    }
}
void oa_enter_type(SYMBOL *sp)
{
    if (!istype(sp) || sp->storage_class == sc_typedef)
    {
        TYPE *tp = basetype(sp->tp);
        while (ispointer(tp))
            tp = basetype(tp)->btp;

        if (isstructured(tp))
        {
            GetType(tp, TRUE, FALSE, FALSE);
        }
    }
}
static BOOLEAN validateGlobalRef(SYMBOL *sp1, SYMBOL *sp2);
static TYPE *cloneType(TYPE *tp)
{
    TYPE *rv = NULL, **lst = &rv;
    while (tp)
    {
        *lst = (TYPE *)peLib->AllocateBytes(sizeof(TYPE));
        **lst = *tp;
        if (tp->type == bt_struct || tp->type == bt_enum)
        {
            (*lst)->sp = clone(tp->sp, false);
        }
        tp = tp->btp;
        lst = &(*lst)->btp;
    }
    UpdateRootTypes(rv);
    return rv;
}
static SYMBOL *clone(SYMBOL *sp, bool ctype)
{
    // shallow copy
    SYMBOL *rv = (SYMBOL *)peLib->AllocateBytes(sizeof(SYMBOL));
    *rv = *sp;
    rv->name = (char *)peLib->AllocateBytes(strlen(rv->name) + 1);
    if (ctype)
        rv->tp = cloneType(rv->tp);
    strcpy(rv->name, sp->name);
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
            sp = clone(sp);
            it = externalMethods.find(sp);
            if (it != externalMethods.end())
            {
                globalMethods[sp] = it->second;
                externalMethods.erase(it);
            }
            else
            {
                globalMethods[sp] = peLib->AllocateMethodName(GetMethodSignature(sp->tp, false));
            }
        }
        /*
        else
        {
            printf("Error: Multiple definition of %s", sp->name);
        }
        */
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
                f = ((FieldName *)it->second)->GetField();
                externalList.erase(it);
                f->FieldType(GetType(sp->tp, TRUE));
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
            staticMethods[sp] = peLib->AllocateMethodName(GetMethodSignature(sp->tp, false));
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
    if (sp->msil)
        return peLib->AllocateFieldName(static_cast<Field *>(sp->msil));
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
        switch (sp->storage_class)
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
            std::map<SYMBOL *, Value *, byLabel>::iterator it = staticList.find(sp);
            if (it != staticList.end())
                return it->second;
            CacheStatic(sp);
            it = staticList.find(sp);
            if (it != staticList.end())
                return it->second;
            break;
        }
        default:
        {
            std::map<SYMBOL *, Value *, byName>::iterator it = globalList.find(sp);
            if (it != globalList.end())
                return it->second;
            it = externalList.find(sp);
            if (it != externalList.end())
                return it->second;
            CacheExtern(sp);
            it = externalList.find(sp);
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
    LIST *lst = NULL;
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
                    newLocal->Index(count);
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
                sym->temp = TRUE;
                Type *type = GetType(sym->tp, TRUE);
                Local *newLocal = peLib->AllocateLocal(sym->name, type);
                localList.push_back(newLocal);
                newLocal->Index(count);
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
    if (isstructured(basetype(sp->tp)->btp) && (sp->linkage2 == lk_unmanaged || !msil_managed(sp)))
    {
        Param * newParam = peLib->AllocateParam("__retblock", peLib->AllocateType(Type::Void, 1));
        newParam->Index(count++);
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
            Type *oa = peLib->AllocateType(Type::object, 0);
            oa->ArrayLevel(1);
            newParam = peLib->AllocateParam("__va_list__", oa);
            sym->name = "__va_list__";
        }
        else
        {
            Type *type = GetType(sym->tp, TRUE);
            newParam = peLib->AllocateParam(sym->name, type);
        }
        newParam->Index(count++);
        paramList[sym] = newParam;
        hr = hr->next;
    }
}
extern "C" void compile_start(char *name)
{
    _using_init();
    Import();
    staticList.clear();
}
void LoadFuncs(void)
{
    if (!managed_library)
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
    }
}
extern "C" void flush_peep(SYMBOL *funcsp, QUAD *list)
{
    LoadFuncs();
    if (!(cparams.prm_compileonly && !cparams.prm_asmfile))
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
    if (!cparams.prm_compileonly || cparams.prm_asmfile)
        if (sp->linkage3 == lk_entrypoint)
            if (hasEntryPoint)
            {
                printf("multiple entry points detected");
                errCount++;
            }
            else
            {
                hasEntryPoint = TRUE;
            }
    currentMethod = peLib->AllocateMethod(sig, flags,sp->linkage3 == lk_entrypoint);
    mainContainer->Add(currentMethod);
    if (!strcmp(sp->name, "main"))
        if (!theCurrentFunc->parentClass && !theCurrentFunc->parentNameSpace)
            mainSym = currentMethod;

    auto hr = basetype(sp->tp)->syms->table[0];
    auto it = sig->begin();
    if (isstructured(basetype(sp->tp)->btp) && (sp->linkage2 == lk_unmanaged || !msil_managed(sp)))
        it++;
    while (hr && it != sig->end())
    {
        SYMBOL *sym = (SYMBOL *)hr->p;
        if (sym->tp->type == bt_void)
            break;
        if (sym->tp->type != bt_ellipse)
        {
            (*it)->Name(sym->name);
        }
        ++it;
        hr = hr->next;
    }
}
static void mainLocals(void)
{
    localList.clear();
    paramList.clear();
    if (!managed_library)
    {
        localList.push_back(peLib->AllocateLocal("argc", peLib->AllocateType(Type::i32, 0)));
        localList.push_back(peLib->AllocateLocal("argv", peLib->AllocateType(Type::Void, 1)));
        localList.push_back(peLib->AllocateLocal("environ", peLib->AllocateType(Type::Void, 1)));
        localList.push_back(peLib->AllocateLocal("newmode", peLib->AllocateType(Type::Void, 1)));
    }
}
static MethodSignature *LookupSignature(char * name)
{
    static SYMBOL sp;
    sp.name = name;
    std::map<SYMBOL *, MethodSignature*, byName>::iterator it = pinvokeInstances.find(&sp);
    if (it != pinvokeInstances.end())
        return it->second;
    return NULL;
}
static MethodSignature *LookupManagedSignature(char *name)
{
    Method *rv = nullptr;
    peLib->Find(std::string("lsmsilcrtl.rtl::") + name, &rv, std::vector<Type *> {}, nullptr, false);
    if (rv)
        return rv->Signature();
    return nullptr;
}
static Field *LookupField(char *name)
{
    static SYMBOL sp;
    sp.name = name;
    std::map<SYMBOL *, Value *, byName>::iterator it = globalList.find(&sp);
    if (it != globalList.end())
        return static_cast<FieldName *>(it->second)->GetField();
    it = externalList.find(&sp);
    if (it != externalList.end())
    {
        SYMBOL *sp = it->first;
        Value *v = it->second;
        externalList.erase(it);
        globalList[sp] = v;
        peLib->WorkingAssembly()->Add(static_cast<FieldName *>(v)->GetField());
        return static_cast<FieldName *>(v)->GetField();
    }

    return NULL;
}
static Field *LookupManagedField(char *name)
{
    void *rv = nullptr;
    if (peLib->Find(std::string("lsmsilcrtl.rtl::") + name, &rv, false) == PELib::s_field)
    {
        return static_cast<Field *>(rv);
    }
    return nullptr;
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
    signature->ReturnType(peLib->AllocateType(Type::Void, 0));
    currentMethod = peLib->AllocateMethod(signature, flags, prm_targettype != DLL);
    mainContainer->Add(currentMethod);

    if (managed_library)
    {
        signature = LookupManagedSignature("__initialize_managed_library");
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));

    }
    else
    {
        signature = LookupSignature("__pctype_func");
        if (!signature)
        {
            signature = peLib->AllocateMethodSignature("__pctype_func", 0, NULL);
            signature->ReturnType(peLib->AllocateType(Type::u16, 1));
            peLib->AddPInvokeReference(signature, pinvoke_dll, false);
        }
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));

        Field *field = LookupField("_pctype");
        if (!field)
        {
            field = peLib->AllocateField("_pctype", peLib->AllocateType(Type::u16, 1), Qualifiers::Public | Qualifiers::Static);
            peLib->WorkingAssembly()->Add(field);
        }
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));

        signature = LookupSignature("__iob_func");
        if (!signature)
        {
            signature = peLib->AllocateMethodSignature("__iob_func", 0, NULL);
            signature->ReturnType(peLib->AllocateType(Type::Void, 1));
            peLib->AddPInvokeReference(signature, pinvoke_dll, false);
        }
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));

        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_dup));
        field = LookupField("__stdin");
        if (!field)
        {
            field = peLib->AllocateField("__stdin", peLib->AllocateType(Type::Void, 1), Qualifiers::Public | Qualifiers::Static);
            peLib->WorkingAssembly()->Add(field);
        }
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));

        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_dup));
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldc_i4, peLib->AllocateOperand((longlong)32, Operand::any)));
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_add));

        field = LookupField("__stdout");
        if (!field)
        {
            field = peLib->AllocateField("__stdout", peLib->AllocateType(Type::Void, 1), Qualifiers::Public | Qualifiers::Static);
            peLib->WorkingAssembly()->Add(field);
        }
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));

        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldc_i4, peLib->AllocateOperand((longlong)64, Operand::any)));
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_add));

        field = LookupField("__stderr");
        if (!field)
        {
            field = peLib->AllocateField("__stderr", peLib->AllocateType(Type::Void, 1), Qualifiers::Public | Qualifiers::Static);
            peLib->WorkingAssembly()->Add(field);
        }
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));
    }
}
static void dumpInitializerCalls(LIST *lst)
{
    while (lst)
    {
        MethodName *m = static_cast<MethodName *>(lst->data);
        if (m->Signature()->ParamCount() || !m->Signature()->ReturnType()->IsVoid() || m->Signature()->ReturnType()->PointerLevel())
        {
            errCount++;
            printf("startup/rundown function must be function with no arguments, with no return value\n");
        }
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(static_cast<MethodName *>(lst->data))));
        lst = lst->next;
    }
}
static void dumpCallToMain(void)
{
    if (prm_targettype != DLL)
    {
        if (managed_library)
        {
            if (mainSym)
            {
                int n = mainSym->Signature()->ParamCount();
                if (n >= 1)
                    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldsfld, peLib->AllocateOperand(peLib->AllocateFieldName(LookupManagedField("__argc"))))); // load argc
                if (n >= 2)
                    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldsfld, peLib->AllocateOperand(peLib->AllocateFieldName(LookupManagedField("__argv"))))); // load argcv
                if (n >= 3)
                    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldsfld, peLib->AllocateOperand(peLib->AllocateFieldName(LookupManagedField("__env"))))); // load env
                for (int i = 3; i < n; i++)
                    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldnull, NULL)); // load a spare arg
                MethodSignature *signature = peLib->AllocateMethodSignature("main", MethodSignature::Managed, mainContainer);
                currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(mainSym->Signature()))));
            }

        }
        else
        {
            currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldloca, peLib->AllocateOperand(localList[0]))); // load argc
            currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldloca, peLib->AllocateOperand(localList[1]))); // load argcv
            currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldloca, peLib->AllocateOperand(localList[2]))); // load environ
            currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldc_i4, peLib->AllocateOperand((longlong)0, Operand::i32)));
            currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldloca, peLib->AllocateOperand(localList[3]))); // load newmode


            MethodSignature *signature = peLib->AllocateMethodSignature("__getmainargs", 0, NULL);
            signature->ReturnType(peLib->AllocateType(Type::Void, 0));
            signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::Void, 1)));
            signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::Void, 1)));
            signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::Void, 1)));
            signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::i32, 0)));
            signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::Void, 1)));
            peLib->AddPInvokeReference(signature, pinvoke_dll, false);
            currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));

            if (mainSym)
            {
                int n = mainSym->Signature()->ParamCount();
                if (n >= 1)
                    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldloc, peLib->AllocateOperand(localList[0]))); // load argc
                if (n >= 2)
                    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldloc, peLib->AllocateOperand(localList[1]))); // load argcv
                for (int i = 2; i < n; i++)
                    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldnull, NULL)); // load a spare arg
                signature = peLib->AllocateMethodSignature("main", MethodSignature::Managed, mainContainer);
                currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(mainSym->Signature()))));
            }
        }
        dumpInitializerCalls(deinitializersHead);
        if (!mainSym || mainSym && mainSym->Signature()->ReturnType()->IsVoid())
            currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldc_i4, peLib->AllocateOperand((longlong)0, Operand::i32)));
        if (managed_library)
        {
            MethodSignature *signature = LookupManagedSignature("exit");
            currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));
        }
        else
        {
            MethodSignature *signature = LookupSignature("exit");
            if (!signature)
            {

                signature = peLib->AllocateMethodSignature("exit", 0, NULL);
                signature->ReturnType(peLib->AllocateType(Type::Void, 0));
                signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::i32, 0)));
                peLib->AddPInvokeReference(signature, pinvoke_dll, false);
            }
            currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));
        }
    }
}
static void dumpGlobalFuncs()
{
}

static void AddRTLThunks()
{
    Param *param;
    if (!hasEntryPoint)
    {
        if (mainSym)
        {
            if (mainSym->Signature()->ParamCount() < 1)
            {
                param = peLib->AllocateParam("argc", peLib->AllocateType(Type::i32, 0));
                mainSym->Signature()->AddParam(param);
            }
            if (mainSym->Signature()->ParamCount() < 2)
            {
                param = peLib->AllocateParam("argv", peLib->AllocateType(Type::Void, 1));
                mainSym->Signature()->AddParam(param);
            }
        }
        oa_enterseg();
        for (auto ri = startups.begin(); ri != startups.end(); ++ri)
        {
            LIST *lst = (LIST *)peLib->AllocateBytes(sizeof(LIST));
            lst->data = (void *)ri->second;
            lst->next = initializersHead;
            initializersHead = lst;
        }
        for (auto ri = rundowns.rbegin(); ri != rundowns.rend(); ++ri)
        {
            LIST *lst = (LIST *)peLib->AllocateBytes(sizeof(LIST));
            if (deinitializersHead)
                deinitializersTail = deinitializersTail->next = lst;
            else
                deinitializersHead = deinitializersTail = lst;

        }

        mainInit();
        dumpInitializerCalls(initializersHead);
        mainLocals();
        dumpCallToMain();

        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ret));
        for (int i = 0; i < localList.size(); i++)
            currentMethod->AddLocal(localList[i]);

        try
        {
            currentMethod->Optimize(*peLib);
        }
        catch (PELibError exc)
        {
            std::cout << "Optimizer error: ( $Main ) " << exc.what() << std::endl;
        }

        dumpGlobalFuncs();
    }
}
static void CreateExternalCSharpReferences()
{
    argsCtor = FindMethodSignature("lsmsilcrtl.args::.ctor");
    argsNextArg = FindMethodSignature("lsmsilcrtl.args::GetNextArg");
    argsUnmanaged = FindMethodSignature("lsmsilcrtl.args::GetUnmanaged");
    ptrBox = FindMethodSignature("lsmsilcrtl.pointer::box");
    ptrUnbox = FindMethodSignature("lsmsilcrtl.pointer::unbox");

    systemObject = FindType("System.Object");

    Type stringType(Type::string, 0);
    Type objectType(Type::object, 0);

    std::vector<Type *> strArgs;
    strArgs.push_back(&stringType);
    strArgs.push_back(&stringType);
    std::vector<Type *> objArgs;
    objArgs.push_back(&objectType);
    objArgs.push_back(&objectType);

    std::vector<Type *>toStrArgs;
    toStrArgs.push_back(&objectType);

    Method *result;
    if (peLib->Find("System.String::Concat", &result, strArgs) == PELib::s_method)
    {
        concatStr =  result->Signature();
    }
    if (peLib->Find("System.String::Concat", &result, objArgs) == PELib::s_method)
    {
        concatObj =  result->Signature();
    }
    if (peLib->Find("System.Convert::ToString", &result, toStrArgs) == PELib::s_method)
    {
        toStr =  result->Signature();
    }
    if (!concatStr || !concatObj || !toStr)
        fatal("could not find builtin function");
}
void ReplaceName(std::map<std::string, Value *> &list, Value *v, char *name)
{
    MethodName *n = static_cast<MethodName *>(v);
    n->Signature()->SetName(name);
    list[name] = v;
}
extern "C" void oa_gensrref(SYMBOL *sp, int val, int type)
{
    static int count = 1;
    Value *v = globalMethods[sp];
    if (v)
    {
        switch (type)
        {
            MethodName *n;
            char name[256];
            case STARTUP_TYPE_STARTUP:
                sprintf(name, "$$STARTUP_%d_%d_%x", val, count, uniqueId);
                ReplaceName(startups, v, name);
                break;
            case STARTUP_TYPE_RUNDOWN:
                sprintf(name, "$$RUNDOWN_%d_%d_%x", val, count, uniqueId);
                ReplaceName(rundowns, v, name);
                break;
            case STARTUP_TYPE_TLS_STARTUP:
                sprintf(name, "$$TLSSTARTUP_%d_%d_%x", val, count, uniqueId);
                ReplaceName(tlsstartups, v, name);
                break;
            case STARTUP_TYPE_TLS_RUNDOWN:
                sprintf(name, "$$TLSRUNDOWN_%d_%d_%x", val, count, uniqueId);
                ReplaceName(tlsrundowns, v, name);
                break;
        }
    }
    else
    {
        diag("oa_startup: function not found");
    }
}
extern "C" BOOLEAN oa_main_preprocess(void)
{

    PELib::CorFlags corFlags = PELib::bits32;
    if (namespaceAndClass[0])
        corFlags = (PELib::CorFlags)((int)corFlags | PELib::ilonly);
    char path[260], fileName[256];
    GetOutputFileName(fileName, path, cparams.prm_compileonly && !cparams.prm_asmfile);
    uniqueId = CRC32((unsigned char *)fileName, strlen(fileName));
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
    bool newFile;
    if (!peLib)
    {
        peLib = new PELib(q, corFlags);

        if (peLib->LoadAssembly("mscorlib"))
        {
            fatal("could not load mscorlib.dll");
        }
        if (peLib->LoadAssembly("lsmsilcrtl"))
        {
            fatal("could not load lsmsilcrtl.dll");
        }
        _apply_global_using();

        peLib->AddUsing("System");
        newFile = true;
    }
    else
    {
        peLib->EmptyWorkingAssembly(q);
        newFile = false;
    }
    if (p)
    {
        *p = '.';
    }
    if (namespaceAndClass[0])
    {
        p = strchr(namespaceAndClass, '.');
        *p = 0;
        Namespace *nm = peLib->AllocateNamespace(namespaceAndClass);
        peLib->WorkingAssembly()->Add(nm);
        Class *cls = peLib->AllocateClass(p + 1, Qualifiers::MainClass | Qualifiers::Public, -1, -1);
        nm->Add(cls);
        mainContainer = cls;
        *p = '.';
    }
    else
    {
        mainContainer = peLib->WorkingAssembly();
    }
    /**/
    if (newFile)
    {
        peLib->WorkingAssembly()->SetVersion(assemblyVersion[0], assemblyVersion[1], assemblyVersion[2], assemblyVersion[3]);
        peLib->WorkingAssembly()->SNKFile(prm_snkKeyFile);

        CreateExternalCSharpReferences();
        retblocksym.name = "__retblock";
    }
    return FALSE;
}
static BOOLEAN checkExterns(void)
{
    BOOLEAN rv = FALSE;
    for (std::map<SYMBOL *, Value *, byName>::iterator it = externalMethods.begin();
        it != externalMethods.end(); ++it)
    {
        if (it->first->linkage2 != lk_msil_rtl)
        {
            printf("Error: Undefined external symbol %s\n", it->first->name);
            rv = TRUE;
        }
    }
    for (std::map<SYMBOL *, Value *, byName>::iterator it = externalList.begin();
        it != externalList.end(); ++it)
    {
        char *name = it->first->name;
        if (strcmp(name, "_pctype") && strcmp(name, "__stdin") &&
            strcmp(name, "__stdout") && strcmp(name, "__stderr"))
        {
            printf("Error: Undefined external symbol %s\n", name);
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
    if (cparams.prm_compileonly && !cparams.prm_asmfile)
    {
        // empty
    }
    else
    {
        char path[260];
        char ilName[260];
        GetOutputFileName(ilName, path, FALSE);
        StripExt(ilName);
        AddRTLThunks();
        if (!errors && prm_targettype != DLL && !mainSym && !hasEntryPoint)
        {
            printf("Error: main not defined\n");
        }
        errors |= checkExterns() || errCount || prm_targettype != DLL && !mainSym && !hasEntryPoint;
        if (!errors)
        {
            if (cparams.prm_asmfile)
                AddExt(ilName, ".il");
            else if (prm_targettype == DLL)
                AddExt(ilName, ".dll");
            else
                AddExt(ilName, ".exe");
            peLib->DumpOutputFile(ilName, cparams.prm_asmfile ? PELib::ilasm : (prm_targettype == DLL ? PELib::pedll : PELib::peexe), prm_targettype != CONSOLE);
        }
    }
}
extern "C" void oa_end_generation(void)
{
    if (cparams.prm_compileonly && !cparams.prm_asmfile)
    {
        cseg();
        for (auto it = externalList.begin(); it != externalList.end(); ++it)
        {
            Field *f = static_cast<FieldName *>(it->second)->GetField();
            f->External(true);
            mainContainer->Add(f);
        }
        for (auto it = externalMethods.begin(); it != externalMethods.end(); ++it)
        {
            int flags = Qualifiers::ManagedFunc | Qualifiers::Public;
            MethodSignature *s = static_cast<MethodName *>(it->second)->Signature();
            if (!s->GetContainer()->InAssemblyRef())
            {
                Method *m = peLib->AllocateMethod(s, flags);
                s->External(true);
                mainContainer->Add(m);
            }
        }
        char path[260];
        char ilName[260];
        GetOutputFileName(ilName, path, TRUE);
        NextOutputFileName();
        peLib->DumpOutputFile(ilName, PELib::object, false);
// needs work        delete peLib;
        if (objlist->next)
            oa_main_preprocess();

        initializersHead = initializersTail = NULL;
        deinitializersHead = deinitializersTail = NULL;
        externalMethods.clear();
        externalList.clear();
        globalMethods.clear();
        globalList.clear();
        staticMethods.clear();
        staticList.clear();
        pinvokeInstances.clear();
        typeList.clear();
        fieldList.clear();
        arrayMethods.clear();
        pInvokeReferences.clear();
        startups.clear();
        rundowns.clear();
        tlsstartups.clear();
        tlsrundowns.clear();
    }
    else
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
            static SYMBOL sp;
            sp.name = (char *)start->name;
            std::map<SYMBOL *, Value *, byName>::iterator it = globalMethods.find(&sp);
            MethodSignature *signature = static_cast<MethodName *>(it->second)->Signature();
            lst->data = (void *)peLib->AllocateMethodName(signature);
            if (initializersHead)
                initializersTail = initializersTail->next = lst;
            else
                initializersHead = initializersTail = lst;
        }
        if (end)
        {
            LIST *lst = (LIST *)peLib->AllocateBytes(sizeof(LIST));
            static SYMBOL sp;
            sp.name = (char *)end->name;
            std::map<SYMBOL *, Value *, byName>::iterator it = globalMethods.find(&sp);
            MethodSignature *signature = static_cast<MethodName *>(it->second)->Signature();
            lst->data = (void *)peLib->AllocateMethodName(signature);
            if (deinitializersHead)
                deinitializersTail = deinitializersTail->next = lst;
            else
                deinitializersHead = initializersTail = lst;
        }
    }
}
extern "C" void oa_put_extern(SYMBOL *sp, int code)
{
    if (isfunction(sp->tp))
    {
        if (!msil_managed(sp))
        {
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
    oa_enterseg();
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
        CreateFunction(GetMethodSignature(sp), sp);
    }
    //    else
    //    {
    //        CreateField(sp);
    //    }
}
Type * GetStringType(int type)
{
    std::string name;
    switch (type)
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
    Class *newClass = peLib->AllocateClass(name, Qualifiers::Private | Qualifiers::Value | Qualifiers::Explicit | Qualifiers::Ansi | Qualifiers::Sealed,
        1, 1);
    mainContainer->Add(newClass);
    Type *type2 = peLib->AllocateType(newClass);
    typeList[name] = type2;
    return type2;
}
Value *GetStringFieldData(int lab, int type)
{
    static SYMBOL sp1;
    sp1.name = "$$$";
    sp1.label = lab;
    SYMBOL *sp = clonesym(&sp1);

    std::map<SYMBOL *, Value *, byLabel>::iterator it = staticList.find(sp);
    if (it != staticList.end())
        return it->second;

    char buf[256];
    sprintf(buf, "L_%d_%x", lab, uniqueId);
    Field *field = peLib->AllocateField(buf, GetStringType(type), Qualifiers::ClassField | Qualifiers::Private | Qualifiers::Static);
    mainContainer->Add(field);
    sp = clone(sp);
    Value *v = peLib->AllocateFieldName(field);
    staticList[sp] = v;

    return v;
}
void oa_put_string_label(int lab, int type)
{
    oa_enterseg();

    Field *field = static_cast<FieldName *>(GetStringFieldData(lab, type))->GetField();

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
    int nlen = len;
    while (nlen--)
    {
        oa_genbyte(*str++);
    }
}

void oa_enterseg()
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
