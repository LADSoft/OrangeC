/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "be.h"
#include "winmode.h"
#include "DotNetPELib.h"
#include "Utils.h"
#include <unordered_map>
#include <algorithm>

#define STARTUP_TYPE_STARTUP 1
#define STARTUP_TYPE_RUNDOWN 2
#define STARTUP_TYPE_TLS_STARTUP 3
#define STARTUP_TYPE_TLS_RUNDOWN 4

#include <vector>
#include <string>
using namespace DotNetPELib;

extern void Import();
BoxedType* boxedType(int isz);

extern SimpleSymbol* currentFunction;
extern COMPILER_PARAMS cparams;
extern std::vector<SimpleSymbol*> externals;
extern std::string prm_namespace_and_class;
extern std::vector<SimpleSymbol*> temporarySymbols;
extern std::string prm_snkKeyFile;
extern std::string prm_assemblyVersion;
extern LIST* objlist;
extern const char* pinvoke_dll;

MethodSignature* argsCtor;
MethodSignature* argsNextArg;
MethodSignature* argsUnmanaged;
MethodSignature* ptrBox;
MethodSignature* ptrUnbox;
MethodSignature* concatStr;
MethodSignature* concatObj;
MethodSignature* toStr;
Type* systemObject;
Method* currentMethod;
PELib* peLib;
DataContainer* mainContainer;

std::string _dll_name(const char* name);
int uniqueId;
static SimpleSymbol retblocksym;

static Method* mainSym;

static int hasEntryPoint;
static int errCount;
static LIST *initializersHead, *initializersTail;
static LIST *deinitializersHead, *deinitializersTail;
static int dataPos, dataMax;
static Byte* dataPointer;
static Field* initializingField;
static char objFileName[260];

Type* GetType(SimpleType* tp, bool commit, bool funcarg, bool pinvoke);
static SimpleSymbol* clone(SimpleSymbol* sp, bool ctype = true);
void CacheExtern(SimpleSymbol* sp);
void CacheGlobal(SimpleSymbol* sp);
void CacheStatic(SimpleSymbol* sp);

struct byName
{
    bool operator()(const SimpleSymbol* left, const SimpleSymbol* right) const { return strcmp(left->name, right->name) < 0; }
};
struct byLabel
{
    bool operator()(const SimpleSymbol* left, const SimpleSymbol* right) const
    {
        if (left->storage_class == scc_localstatic || right->storage_class == scc_localstatic)
        {
            if (left->storage_class != right->storage_class)
                return left->storage_class < right->storage_class;
            return left->outputName < right->outputName;
        }
        return left->label < right->label;
    }
};
struct byField
{
    bool operator()(const SimpleSymbol* left, const SimpleSymbol* right) const
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
static std::map<SimpleSymbol*, Value*, byName> externalMethods;
static std::map<SimpleSymbol*, Value*, byName> externalList;
static std::map<SimpleSymbol*, Value*, byName> globalMethods;
static std::map<SimpleSymbol*, Value*, byName> globalList;
static std::map<SimpleSymbol*, Value*, byLabel> staticMethods;
static std::map<SimpleSymbol*, Value*, byLabel> staticList;
static std::map<SimpleSymbol*, MethodSignature*, byName> pinvokeInstances;
static std::map<SimpleSymbol*, Param*, byName> paramList;
std::multimap<std::string, MethodSignature*> pInvokeReferences;

std::map<std::string, Value*> startups, rundowns, tlsstartups, tlsrundowns;

std::vector<Local*> localList;
static std::map<std::string, Type*> typeList;
static std::map<SimpleSymbol*, Value*, byField> fieldList;
static std::map<std::string, MethodSignature*> arrayMethods;

void parse_pragma(const char* kw, const char* tag)
{
    if (Utils::iequal(kw, "netlib"))
    {
        while (isspace(*tag))
            tag++;
        if (*tag)
        {
            std::string temp = tag;
            int npos = temp.find_last_not_of(" \t\v\n");
            if (npos != std::string::npos)
            {
                temp = temp.substr(0, npos+1);
            }
            peLib->LoadAssembly(temp);
            Import();
        }
    }
}

const char *GetName(const DataContainer* container, std::string name = "")
{
    std::string rv = Qualifiers::GetName("", container, false);
    std::replace(rv.begin(), rv.end(), '/', '.');
    if (name.size())
        rv += "." + name;
    return litlate(rv.c_str());
}

MethodSignature* LookupArrayMethod(Type* tp, std::string name)
{
    char buf[256];
    sprintf(buf, "$$$%d;%d;%s", tp->ArrayLevel(), tp->GetBasicType(), name.c_str());
    auto it = arrayMethods.find(buf);
    if (it != arrayMethods.end())
        return it->second;
    Type* tp1 = peLib->AllocateType(Type::i32, 0);
    int n = tp->ArrayLevel();
    MethodSignature* sig =
        peLib->AllocateMethodSignature(name, MethodSignature::Managed | MethodSignature::InstanceFlag, mainContainer);
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
        Type* tp2 = peLib->AllocateType(tp->GetBasicType(), 0);
        sig->ReturnType(tp2);
    }
    else if (name == "Set")
    {
        Type* tp2 = peLib->AllocateType(tp->GetBasicType(), 0);
        sig->ReturnType(peLib->AllocateType(Type::Void, 0));
        char buf[256];
        sprintf(buf, "p$$");
        sig->AddParam(peLib->AllocateParam(buf, tp2));
    }
    arrayMethods[name] = sig;
    return sig;
}
static MethodSignature* FindMethodSignature(const char* name)
{
    void* result;
    if (peLib->Find(name, &result) == PELib::s_method)
    {
        return static_cast<Method*>(result)->Signature();
    }
    Utils::fatal("could not find built in method %s", name);
    return NULL;
}
static Type* FindType(const char* name, bool toErr)
{
    void* result;
    if (peLib->Find(name, &result) == PELib::s_class)
    {
        return peLib->AllocateType(static_cast<Class*>(result));
    }
    if (toErr)
        Utils::fatal("could not find built in type %s", name);
    return NULL;
}

// weed out structures with nested structures or bit fields
bool qualifiedStruct(SimpleSymbol* sp)
{
    //    SYMLIST *hr;
    if (!sp->tp->size)
        return false;
#if 0
    if (sp->tp->type == st_union)
        return false;
    hr = sp->tp->syms->table[0];
    while (hr)
    {
        SYMBOL *check = (SYMBOL *)hr->p;
        if (basetype(check->tp)->bits)
            return false;
        //        if (isstructured(check->tp))
        //            return false;
//        if (basetype(check->tp)->array)
//            return false;
        hr = hr->next;
    }
#endif
    return true;
}
bool IsPointedStruct(SimpleType* tp)
{
    while (tp->type == st_pointer)
        tp = tp->btp;
    return tp->type == st_struct || tp->type == st_union;
}
Field* GetField(SimpleSymbol* sp)
{
    int flags = Qualifiers::Public | Qualifiers::Static;
    if (sp->storage_class == scc_localstatic)
    {
        char buf[256];
        sprintf(buf, "%s_%x", sp->outputName, uniqueId);
        Field* field = peLib->AllocateField(buf, GetType(sp->tp, true), flags);
        return field;
    }
    else if (sp->storage_class != scc_localstatic && sp->storage_class != scc_constant && sp->storage_class != scc_static)
    {
        Field* field = peLib->AllocateField(sp->name, GetType(sp->tp, true), flags);
        return field;
    }
    else
    {
        char buf[256];
        sprintf(buf, "L_%d_%x", sp->label, uniqueId);
        Field* field = peLib->AllocateField(buf, GetType(sp->tp, true), flags);
        return field;
    }
}
MethodSignature* GetMethodSignature(SimpleType* tp, bool pinvoke)
{
    int flags = pinvoke ? 0 : MethodSignature::Managed;
    while (tp->type == st_pointer)
        tp = tp->btp;
    SimpleSymbol* sp = tp->sp;
    // this compiler never uses instance members, maybe when we do C++
    LIST* hr = tp->sp->syms;
    while (hr && hr->next)
        hr = hr->next;
    if (hr && ((SimpleSymbol*)hr->data)->tp->type == st_ellipse)
    {
        flags |= MethodSignature::Vararg;
    }
    MethodSignature* rv = nullptr;
    if (sp->storage_class != scc_localstatic && sp->storage_class != scc_constant && sp->storage_class != scc_static)
    {
        if (sp->msil_rtl && !cparams.no_default_libs)
        {
            char buf[1024];
            void* result;
            // in lsmsilcrtl
            sprintf(buf, "lsmsilcrtl.rtl.%s", sp->name);
            if (peLib->Find(buf, &result) == PELib::s_method)
            {
                rv = static_cast<Method*>(result)->Signature();
                return rv;
            }
            else
            {
                Utils::fatal("missing rtl function: %s", buf);
            }
        }
        else
        {
            rv = peLib->AllocateMethodSignature(sp->name, flags, pinvoke && sp->msil_rtl ? NULL : mainContainer);
        }
    }
    else
    {
        char buf[256];
        sprintf(buf, "L_%d_%x", sp->label, uniqueId);
        rv = peLib->AllocateMethodSignature(buf, flags, pinvoke ? NULL : mainContainer);
    }
    if ((tp->btp->type == st_struct || tp->btp->type == st_union) && (sp->unmanaged || !msil_managed(sp)))
    {
        rv->ReturnType(peLib->AllocateType(Type::Void, 1));
        Param* p = peLib->AllocateParam("__retblock", peLib->AllocateType(Type::Void, 1));
        rv->AddParam(p);
    }
    else
    {
        rv->ReturnType(GetType(tp->btp, true));
    }

    hr = tp->sp->syms;
    while (hr)
    {
        SimpleSymbol* sym = (SimpleSymbol*)hr->data;
        if (sym->tp->type == st_void)
            break;
        if (sym->tp->type == st_ellipse)
        {
            if (!pinvoke)
            {
                Type* oa = peLib->AllocateType(Type::object, 0);
                oa->ArrayLevel(1);
                Param* p = peLib->AllocateParam("__va_list__", oa);
                rv->AddParam(p);
            }
            break;
        }
        rv->AddParam(peLib->AllocateParam(sym->name, GetType(sym->tp, true, true, pinvoke)));
        hr = hr->next;
    }
    return rv;
}
MethodSignature* FindPInvokeWithVarargs(std::string name, std::list<Param*>::iterator vbegin, std::list<Param*>::iterator vend,
                                        size_t size)
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
MethodSignature* GetMethodSignature(SimpleSymbol* sp)
{
    bool pinvoke = false;
    if (sp)
        pinvoke = !msil_managed(sp);
    std::map<SimpleSymbol*, Value*, byName>::iterator it = globalMethods.find(sp);
    if (it != globalMethods.end())
    {
        return static_cast<MethodName*>(it->second)->Signature();
    }
    it = externalMethods.find(sp);
    if (it != externalMethods.end())
    {
        return static_cast<MethodName*>(it->second)->Signature();
    }
    if (pinvoke)
    {
        std::map<SimpleSymbol*, MethodSignature*, byName>::iterator it1 = pinvokeInstances.find(sp);
        if (it1 != pinvokeInstances.end())
        {
            // if we get here we have a pinvoke instance.   If it isnt vararg just return it
            if (!(it1->second->Flags() & MethodSignature::Vararg))
                return it1->second;
            // vararg create a new instance
            // duplicate matching will be done in the caller
            MethodSignature* sig = GetMethodSignature(sp->tp, true);
            sig->SignatureParent(it1->second->SignatureParent());
            return sig;
        }
        else
        {
            // no current pinvoke instance, create a new one
            MethodSignature* parent = GetMethodSignature(sp->tp, true);
            peLib->AddPInvokeReference(parent, _dll_name(sp->name), sp->isstdcall);
            sp = clone(sp);
            if (parent->Flags() & MethodSignature::Vararg)
            {
                MethodSignature* sig = GetMethodSignature(sp->tp, true);
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
    else if (!sp->tp->type == st_func)
    {
        // function pointer is here
        // we aren't caching these aggressively...
        return GetMethodSignature(sp->tp, false);
    }
    else if (sp->storage_class == scc_external)
    {
        CacheExtern(sp);
        it = externalMethods.find(sp);
        return static_cast<MethodName*>(it->second)->Signature();
    }
    else if (sp->storage_class == scc_static)
    {
        CacheStatic(sp);
        std::map<SimpleSymbol*, Value*, byLabel>::iterator it = staticMethods.find(sp);
        return static_cast<MethodName*>(it->second)->Signature();
    }
    else
    {
        CacheGlobal(sp);
        it = globalMethods.find(sp);
        return static_cast<MethodName*>(it->second)->Signature();
    }
}
std::string GetArrayName(SimpleType* tp)
{
    char end[512];
    end[0] = 0;
    tp = tp;
    while (tp->isarray)
    {
        if (tp->isvla)
            strcat(end, "[vla]");
        else if (tp->size == 0)
            strcat(end, "[]");
        else
            sprintf(end + strlen(end), "[%d]", (int)tp->size);
        tp = tp->btp;
    }
    if ((tp->type == st_struct || tp->type == st_union) || tp->type == st_enum)
    {
        return std::string(tp->sp->name) + end;
    }
    else if (tp->type == st_pointer && tp->btp->type == st_func)
    {
        return std::string("void *") + end;
    }
    else if (tp->type == st_pointer)
    {
        return std::string("void *") + end;
    }
    else
    {
        static std::unordered_map<int, std::string> typeNames = {
            { -ISZ_UCHAR, "int8"},
            { ISZ_UCHAR, "uint8" },
            { -ISZ_USHORT, "int16" },
            { ISZ_USHORT, "uint16" },
            { ISZ_WCHAR, "uint16" },
            { ISZ_U16, "uint16"},
            { -ISZ_UINT, "int32"},
            { ISZ_UINT, "uint32" },
            { -ISZ_UNATIVE, "native int" },
            { ISZ_UNATIVE, "native unsigned int" },
            { -ISZ_ULONG, "int32" },
            { ISZ_ULONG, "uint32" },
            { ISZ_U32, "uint32" },
            { -ISZ_ULONGLONG, "int64" },
            { ISZ_ULONGLONG, "uint64" },
            { ISZ_STRING, "string???"},
            { ISZ_OBJECT, "object???"},
            { ISZ_FLOAT, "float32"},
            { ISZ_DOUBLE, "float64"},
            { ISZ_LDOUBLE, "float64"},
            { ISZ_IFLOAT, "float32"},
            { ISZ_IDOUBLE, "float64"},
            { ISZ_ILDOUBLE, "float64"},
            { ISZ_CFLOAT, "float32"},
            { ISZ_CDOUBLE, "float64"},
            { ISZ_CLDOUBLE, "float64"},
        };
        return typeNames[tp->sizeFromType] + end;
    }
}
Value* GetStructField(SimpleSymbol* sp)
{
    if (sp->msil)
    {
        void *result;
        peLib->Find(sp->msil, &result);
        return peLib->AllocateFieldName(static_cast<Field*>(result));
    }
    auto it = fieldList.find(sp);
    if (it == fieldList.end() && sp->parentClass)
        GetType(sp->parentClass->tp, true);
    it = fieldList.find(sp);
    return it->second;
}
void msil_create_property(SimpleSymbol* property, SimpleSymbol* getter, SimpleSymbol* setter)
{
    if (typeid(*mainContainer) == typeid(Class))
    {
	auto tempvar = std::vector<Type*>{};
        Property* p = peLib->AllocateProperty(*peLib, property->name, GetType(property->tp, true, false, false),
                                              tempvar, !!setter, mainContainer);
        p->Instance(false);  // only doing statics for now...
        static_cast<Class*>(mainContainer)->Add(p);
        property->msil = GetName(p->GetContainer(), p->Name());
    }
    else
    {
        Utils::fatal("Cannot add property at non-class level");
    }
}
void AddType(SimpleSymbol* sym, Type* type) { typeList[sym->outputName] = type; }
Type* GetType(SimpleType* tp, bool commit, bool funcarg, bool pinvoke)
{
    bool byref = false;
    if ((tp->type == st_lref || tp->type == st_rref))
    {
        byref = true;
        tp = tp->btp;
    }
    if ((tp->type == st_struct || tp->type == st_union))
    {
        Type* type = NULL;
        std::map<std::string, Type*>::iterator it = typeList.find(tp->sp->outputName);
        if (it != typeList.end())
            type = it->second;
        if (commit)
        {
            if (!type)
            {
                Class* newClass = peLib->AllocateClass(
                    tp->sp->name,
                    Qualifiers::Public | (tp->type == st_union ? Qualifiers::ClassUnion : Qualifiers::ClassClass),
                    tp->sp->align, tp->size);
                mainContainer->Add(newClass);
                type = peLib->AllocateType(newClass);
                typeList[tp->sp->outputName] = type;
                tp->sp->msil = GetName(newClass);
            }
            else
            {
                tp->sp->msil = GetName(type->GetClass());
                type = peLib->AllocateType(type->GetClass());
            }
            if (!type->GetClass()->InAssemblyRef())
            {
                if (tp->size != 0)
                {
                    Class* cls = (Class*)type->GetClass();
                    cls->size(tp->size);
                    cls->pack(tp->sp->align);
                }
                if (qualifiedStruct(tp->sp) && !type->GetClass()->IsInstantiated())
                {

                    Class* cls = (Class*)type->GetClass();
                    cls->SetInstantiated();
                    LIST* hr = tp->sp->syms;
                    Field* bitField = nullptr;
                    int start = 1000;
                    while (hr)
                    {
                        SimpleSymbol* sym = (SimpleSymbol*)hr->data;
                        if (!sym->tp->bits || sym->tp->startbit < start)
                        {
                            Type* newType = GetType(sym->tp, true);
                            int flags = Qualifiers::ClassField | Qualifiers::Public;
                            char buf[256];
                            if (sym->tp->bits)
                            {
                                start = sym->tp->startbit;
                                sprintf(buf, "$$bits%d", sym->offset);
                            }
                            Field* newField = peLib->AllocateField(sym->tp->bits ? buf : sym->name, newType, flags);
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
            return peLib->AllocateType((DataContainer*)NULL);
        }
    }
    else if (tp->type == st_enum)
    {
        std::map<std::string, Type*>::iterator it = typeList.find(tp->sp->outputName);
        if (it != typeList.end())
            return it->second;
        if (commit)
        {
            Enum* newEnum = peLib->AllocateEnum(tp->sp->name, Qualifiers::Public | Qualifiers::EnumClass, Field::i32);
            mainContainer->Add(newEnum);

            if (tp->sp->syms)
            {
                LIST* hr = tp->sp->syms;
                while (hr)
                {
                    SimpleSymbol* sym = (SimpleSymbol*)hr->data;
                    newEnum->AddValue(*peLib, sym->name, sym->i);
                    hr = hr->next;
                }
            }
            // note we make enums ints in the file
            // this is because pinvokes with enums doesn't work...
            Type* type = peLib->AllocateType(Type::i32, 0);
            typeList[tp->sp->outputName] = type;
            return type;
        }
        else
        {
            return peLib->AllocateType((DataContainer*)NULL);
        }
    }
    else if (tp->isarray && tp->msil)
    {
        char name[512];
        SimpleType* base = tp;
        Type* rv;
        int count = 0;
        while (base->isarray)
        {
            count++;
            base = base->btp;
        }
        sprintf(name, "$$$marray%d;%s;%d", base->type, base->sp ? base->sp->name : "", count);
        std::map<std::string, Type*>::iterator it = typeList.find(name);
        if (it != typeList.end())
            return it->second;
        if (commit)
        {
            rv = GetType(base, true);
            rv->ArrayLevel(count);
            typeList[name] = rv;
            if ((base->type == st_struct || base->type == st_union))
                GetType(base, true);
            rv->ByRef(byref);
            return rv;
        }
        else
        {
            return peLib->AllocateType((DataContainer*)NULL);
        }
    }
    else if (tp->isarray && !funcarg)
    {
        std::string name = GetArrayName(tp);
        std::map<std::string, Type*>::iterator it = typeList.find(name);
        if (it != typeList.end())
            return it->second;
        if (commit)
        {
            Class* newClass = peLib->AllocateClass(name, Qualifiers::Public | Qualifiers::ClassClass, -1,
                                                   tp->size == 0 ? 1 : tp->size);
            mainContainer->Add(newClass);
            Type* type = peLib->AllocateType(newClass);
            typeList[name] = type;
            while (tp->type == st_pointer)
                tp = tp->btp;
            // declare any structure we are referencing..
            if ((tp->type == st_struct || tp->type == st_union))
                GetType(tp, true);
            type->ByRef(byref);
            return type;
        }
        else
        {
            return peLib->AllocateType((DataContainer*)NULL);
        }
    }
    else if (tp->type == st_pointer && tp->btp->type == st_func)
    {
        return peLib->AllocateType(Type::Void, 1);  // pointer to void
                                                    //        tp = tp->btp;
                                                    //        MethodSignature *sig = GetMethodSignature(tp, false);
                                                    //        return peLib->AllocateType(sig);
    }
    else if (tp->type == st_pointer)
    {

        if (pinvoke)
            return peLib->AllocateType(Type::Void, 1);  // pointer to void
        SimpleType* tp1 = tp;
        int level = 0;
        while (tp1->type == st_pointer)
        {
            if (tp1->va_list)
            {
                Type* rv = FindType("lsmsilcrtl.args", true);
                rv->PointerLevel(level);
                return rv;
            }
            if (tp1->type == st_pointer)
                level++;
            tp1 = tp1->btp;
            if (level > 0 && tp1->isarray)
                break;
        }
        // we don't currently support multiple indirections on a function pointer
        // as an independent type
        // this is a limitation of the netlib
        if (tp1->type == st_func)
            return peLib->AllocateType(Type::Void, 1);  // pointer to void
        Type* rv = GetType(tp1, commit);

        // can't get a method signature here
        if (rv->GetBasicType() == Type::cls)
        {
            rv = peLib->AllocateType(rv->GetClass());
        }
        rv->PointerLevel(level);
        rv->ByRef(byref);
        return rv;
    }
    else if (tp->type == st_void)
    {
        return peLib->AllocateType(Type::Void, 0);
    }
    else if (tp->msil)
    {
        if (tp->type == st___string)
            return peLib->AllocateType(Type::string, 0);
        else
            return peLib->AllocateType(Type::object, 0);
    }
    else
    {
        static Type::BasicType typeNames[] = {Type::i8,      Type::i8,  Type::i8,  Type::i8,  Type::u8,      Type::i16, Type::i16,
                                              Type::u16,     Type::u16, Type::i32, Type::i32, Type::inative, Type::i32, Type::u32,
                                              Type::unative, Type::i32, Type::u32, Type::i64, Type::u64,     Type::r32, Type::r64,
                                              Type::r64,     Type::r32, Type::r64, Type::r64};
        Type* rv = peLib->AllocateType(typeNames[tp->type], 0);
        rv->ByRef(byref);
        return rv;
    }
}
bool istype(SimpleSymbol* sym)
{
    return (sym->storage_class == scc_type || sym->storage_class == scc_typedef);
}
void oa_enter_type(SimpleSymbol* sp)
{
    if (!istype(sp) || sp->storage_class == scc_typedef)
    {
        SimpleType* tp = sp->tp;
        while (tp->type == st_pointer)
            tp = tp->btp;

        if ((tp->type == st_struct || tp->type == st_union))
        {
            GetType(tp, true, false, false);
        }
    }
}
static bool validateGlobalRef(SimpleSymbol* sp1, SimpleSymbol* sp2);
static SimpleType* cloneType(SimpleType* tp)
{
    SimpleType *rv = NULL, **lst = &rv;
    while (tp)
    {
        *lst = (SimpleType*)peLib->AllocateBytes(sizeof(SimpleType));
        **lst = *tp;
        if (tp->type == st_struct || tp->type == st_enum)
        {
            (*lst)->sp = clone(tp->sp, false);
        }
        tp = tp->btp;
        lst = &(*lst)->btp;
    }
    return rv;
}
static SimpleSymbol* clone(SimpleSymbol* sp, bool ctype)
{
    // shallow copy
    SimpleSymbol* rv = (SimpleSymbol*)peLib->AllocateBytes(sizeof(SimpleSymbol));
    *rv = *sp;
    rv->name = (char*)peLib->AllocateBytes(strlen(rv->name) + 1);
    if (ctype)
        rv->tp = cloneType(rv->tp);
    strcpy((char*)rv->name, sp->name);
    return rv;
}
void CacheExtern(SimpleSymbol* sp)
{
    if (sp->tp->type == st_func)
    {
        std::map<SimpleSymbol*, Value*, byName>::iterator it = globalMethods.find(sp);
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
        std::map<SimpleSymbol*, Value*, byName>::iterator it = globalList.find(sp);
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
void CacheGlobal(SimpleSymbol* sp)
{
    if (sp->tp->type == st_func)
    {
        std::map<SimpleSymbol*, Value*, byName>::iterator it = globalMethods.find(sp);
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
        std::map<SimpleSymbol*, Value*, byName>::iterator it = globalList.find(sp);
        if (it != globalList.end())
        {
            printf("Error: Multiple definition of %s", sp->name);
        }
        else
        {
            Field* f;
            it = externalList.find(sp);
            if (it != externalList.end())
            {
                validateGlobalRef(sp, it->first);
                f = ((FieldName*)it->second)->GetField();
                externalList.erase(it);
                f->FieldType(GetType(sp->tp, true));
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
void CacheStatic(SimpleSymbol* sp)
{
    if (sp->tp->type == st_func)
    {
        std::map<SimpleSymbol*, Value*, byLabel>::iterator it = staticMethods.find(sp);
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
        std::map<SimpleSymbol*, Value*, byLabel>::iterator it = staticList.find(sp);
        if (it != staticList.end())
        {
            //            printf("Error: Multiple definition of %s", sp->name);
        }
        else
        {
            Field* f = GetField(sp);
            sp = clone(sp);
            staticList[sp] = peLib->AllocateFieldName(f);
            mainContainer->Add(f);
        }
    }
}
Value* GetParamData(std::string name)
{
    SimpleSymbol sp = {0};
    sp.name = (char*)name.c_str();
    return paramList.find(&sp)->second;
}
Value* GetLocalData(SimpleSymbol* sp)
{
    if (sp->storage_class == scc_parameter)
    {
        return paramList.find(sp)->second;
    }
    else
    {
        return localList[sp->offset];
    }
}
Value* GetFieldData(SimpleSymbol* sp)
{
    if (sp->msil)
    {
        void *result;
        peLib->Find(sp->msil, &result);
        return peLib->AllocateFieldName(static_cast<Field*>(result));
    }
    if (sp->tp->type == st_func)
    {
        if (sp->storage_class != scc_localstatic && sp->storage_class != scc_const && sp->storage_class != scc_static)
        {
            std::map<SimpleSymbol*, Value*, byName>::iterator it = globalMethods.find(sp);
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
            std::map<SimpleSymbol*, Value*, byLabel>::iterator it = staticMethods.find(sp);
            if (it != staticMethods.end())
                return it->second;
        }
    }
    else
    {
        switch (sp->storage_class)
        {
            case scc_auto:
            case scc_register:
                return localList[sp->offset];
            case scc_parameter:
            {
                std::map<SimpleSymbol*, Param*, byName>::iterator it = paramList.find(sp);
                if (it != paramList.end())
                    return it->second;
            }
            break;
            case scc_static:
            case scc_localstatic:
            case scc_constant:
            {
                std::map<SimpleSymbol*, Value*, byLabel>::iterator it = staticList.find(sp);
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
                std::map<SimpleSymbol*, Value*, byName>::iterator it = globalList.find(sp);
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
void LoadLocals(std::vector<SimpleSymbol*>& vars)
{
    localList.clear();
    int count = 0;
    bool found = false;
    for (auto sym : vars)
    {
        if (sym->storage_class == scc_auto || sym->storage_class == scc_register)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        for (auto & sym : temporarySymbols)
        {
            if (!sym->anonymous)
            {
                found = true;
                break;
            }
        }
    }
    if (found)
    {
        for (auto sym : vars)
        {
            sym->temp = false;
        }
        for (auto sym : temporarySymbols)
        {
            sym->temp = false;
        }
        for (auto sym : vars)
        {
            if ((sym->storage_class == scc_auto || sym->storage_class == scc_register) && !sym->temp)
            {
                sym->temp = true;
                Type* type = GetType(sym->tp, true);
                Local* newLocal = peLib->AllocateLocal(sym->name, type);
                localList.push_back(newLocal);
                newLocal->Index(count);
                sym->offset = count++;
            }
        }
        for (auto sym : temporarySymbols)
        {
            if (!sym->anonymous && !sym->temp)
            {
                sym->temp = true;
                Type* type = GetType(sym->tp, true);
                Local* newLocal = peLib->AllocateLocal(sym->name, type);
                localList.push_back(newLocal);
                newLocal->Index(count);
                sym->offset = count++;
            }
        }
    }
}
void LoadParams(SimpleSymbol* funcsp, extern std::vector<SimpleSymbol*>& vars)
{
    int count = 0;
    paramList.clear();
    if ((funcsp->tp->btp->type == st_struct || funcsp->tp->btp->type == st_union) && (funcsp->unmanaged || !msil_managed(funcsp)))
    {
        Param* newParam = peLib->AllocateParam("__retblock", peLib->AllocateType(Type::Void, 1));
        newParam->Index(count++);
        paramList[&retblocksym] = newParam;
    }
    for (auto sym : vars)
    {
        if (sym->tp->type == st_void)
            break;
        Param* newParam;
        if (sym->tp->type == st_ellipse)
        {
            Type* oa = peLib->AllocateType(Type::object, 0);
            oa->ArrayLevel(1);
            newParam = peLib->AllocateParam("__va_list__", oa);
            sym->name = "__va_list__";
        }
        else
        {
            Type* type = GetType(sym->tp, true);
            newParam = peLib->AllocateParam(sym->name, type);
        }
        newParam->Index(count++);
        paramList[sym] = newParam;
    }
}
void compile_start(char* name)
{
    _using_init();
    Import();
    staticList.clear();
}
void flush_peep(SimpleSymbol* funcsp, QUAD* list)
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
// void CreateField(SYMBOL *sp)
//{
//    Field *field = GetField(sp);
//    mainContainer->Add(field);
//}
void CreateFunction(MethodSignature* sig, SimpleSymbol* sp)
{
    int flags = Qualifiers::ManagedFunc;
    if (sp->storage_class == scc_static)
        flags |= Qualifiers::Private;
    else
        flags |= Qualifiers::Public;
    if (!cparams.prm_compileonly || cparams.prm_asmfile)
    {
        if (sp->entrypoint)
        {
            if (hasEntryPoint)
            {
                printf("multiple entry points detected");
                errCount++;
            }
            else
            {
                hasEntryPoint = true;
            }
        }
    }
    currentMethod = peLib->AllocateMethod(sig, flags, sp->entrypoint);
    mainContainer->Add(currentMethod);
    if (!strcmp(sp->name, "main"))
        if (!currentFunction->parentClass && !currentFunction->namespaceName)
            mainSym = currentMethod;

    auto hr = sp->syms;
    auto it = sig->begin();
    if ((sp->tp->btp->type == st_struct || sp->tp->btp->type == st_union) && (sp->unmanaged || !msil_managed(sp)))
        it++;
    while (hr && it != sig->end())
    {
        SimpleSymbol* sym = (SimpleSymbol*)hr->data;
        if (sym->tp->type == st_void)
            break;
        if (sym->tp->type != st_ellipse)
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
    if (!cparams.managed_library)
    {
        localList.push_back(peLib->AllocateLocal("argc", peLib->AllocateType(Type::i32, 0)));
        localList.push_back(peLib->AllocateLocal("argv", peLib->AllocateType(Type::Void, 1)));
        localList.push_back(peLib->AllocateLocal("environ", peLib->AllocateType(Type::Void, 1)));
        localList.push_back(peLib->AllocateLocal("newmode", peLib->AllocateType(Type::Void, 1)));
    }
}
static MethodSignature* LookupSignature(const char* name)
{
    static SimpleSymbol sp;
    sp.name = name;
    std::map<SimpleSymbol*, MethodSignature*, byName>::iterator it = pinvokeInstances.find(&sp);
    if (it != pinvokeInstances.end())
        return it->second;
    return NULL;
}
static MethodSignature* LookupManagedSignature(const char* name)
{
    Method* rv = nullptr;
    peLib->Find(std::string("lsmsilcrtl.rtl::") + name, &rv, std::vector<Type*>{}, nullptr, false);
    if (rv)
        return rv->Signature();
    return nullptr;
}
static Field* LookupField(const char* name)
{
    static SimpleSymbol sp;
    sp.name = name;
    std::map<SimpleSymbol*, Value*, byName>::iterator it = globalList.find(&sp);
    if (it != globalList.end())
        return static_cast<FieldName*>(it->second)->GetField();
    it = externalList.find(&sp);
    if (it != externalList.end())
    {
        SimpleSymbol* sp = it->first;
        Value* v = it->second;
        externalList.erase(it);
        globalList[sp] = v;
        peLib->WorkingAssembly()->Add(static_cast<FieldName*>(v)->GetField());
        return static_cast<FieldName*>(v)->GetField();
    }

    return NULL;
}
static Field* LookupManagedField(const char* name)
{
    void* rv = nullptr;
    if (peLib->Find(std::string("lsmsilcrtl.rtl::") + name, &rv, nullptr) == PELib::s_field)
    {
        return static_cast<Field*>(rv);
    }
    return nullptr;
}
static void LoadLibraryIntrinsics() {}
static void mainInit(void)
{
    std::string name = "$Main";
    int flags = Qualifiers::Private | Qualifiers::HideBySig | Qualifiers::Static | Qualifiers::CIL | Qualifiers::Managed;
    if (cparams.prm_targettype == DLL)
    {
        name = ".cctor";
        flags |= Qualifiers::SpecialName | Qualifiers::RTSpecialName;
    }
    MethodSignature* signature = peLib->AllocateMethodSignature(name, MethodSignature::Managed, mainContainer);
    signature->ReturnType(peLib->AllocateType(Type::Void, 0));
    currentMethod = peLib->AllocateMethod(signature, flags, cparams.prm_targettype != DLL);
    mainContainer->Add(currentMethod);

    if (cparams.managed_library)
    {
        signature = LookupManagedSignature("__initialize_managed_library");
        currentMethod->AddInstruction(
            peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));
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
        currentMethod->AddInstruction(
            peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));

        Field* field = LookupField("_pctype");
        if (!field)
        {
            field = peLib->AllocateField("_pctype", peLib->AllocateType(Type::u16, 1), Qualifiers::Public | Qualifiers::Static);
            peLib->WorkingAssembly()->Add(field);
        }
        currentMethod->AddInstruction(
            peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));

        signature = LookupSignature("__iob_func");
        if (!signature)
        {
            signature = peLib->AllocateMethodSignature("__iob_func", 0, NULL);
            signature->ReturnType(peLib->AllocateType(Type::Void, 1));
            peLib->AddPInvokeReference(signature, pinvoke_dll, false);
        }
        currentMethod->AddInstruction(
            peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));

        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_dup));
        field = LookupField("__stdin");
        if (!field)
        {
            field = peLib->AllocateField("__stdin", peLib->AllocateType(Type::Void, 1), Qualifiers::Public | Qualifiers::Static);
            peLib->WorkingAssembly()->Add(field);
        }
        currentMethod->AddInstruction(
            peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));

        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_dup));
        currentMethod->AddInstruction(
            peLib->AllocateInstruction(Instruction::i_ldc_i4, peLib->AllocateOperand((longlong)32, Operand::any)));
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_add));

        field = LookupField("__stdout");
        if (!field)
        {
            field = peLib->AllocateField("__stdout", peLib->AllocateType(Type::Void, 1), Qualifiers::Public | Qualifiers::Static);
            peLib->WorkingAssembly()->Add(field);
        }
        currentMethod->AddInstruction(
            peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));

        currentMethod->AddInstruction(
            peLib->AllocateInstruction(Instruction::i_ldc_i4, peLib->AllocateOperand((longlong)64, Operand::any)));
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_add));

        field = LookupField("__stderr");
        if (!field)
        {
            field = peLib->AllocateField("__stderr", peLib->AllocateType(Type::Void, 1), Qualifiers::Public | Qualifiers::Static);
            peLib->WorkingAssembly()->Add(field);
        }
        currentMethod->AddInstruction(
            peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));
    }
}
static void dumpInitializerCalls(LIST* lst)
{
    while (lst)
    {
        MethodName* m = static_cast<MethodName*>(lst->data);
        if (m->Signature()->ParamCount() || !m->Signature()->ReturnType()->IsVoid() || m->Signature()->ReturnType()->PointerLevel())
        {
            errCount++;
            printf("startup/rundown function must be function with no arguments, with no return value\n");
        }
        currentMethod->AddInstruction(
            peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(static_cast<MethodName*>(lst->data))));
        lst = lst->next;
    }
}
static void dumpCallToMain(void)
{
    if (cparams.prm_targettype != DLL)
    {
        if (cparams.managed_library)
        {
            if (mainSym)
            {
                int n = mainSym->Signature()->ParamCount();
                if (n >= 1)
                    currentMethod->AddInstruction(peLib->AllocateInstruction(
                        Instruction::i_ldsfld,
                        peLib->AllocateOperand(peLib->AllocateFieldName(LookupManagedField("__argc")))));  // load argc
                if (n >= 2)
                    currentMethod->AddInstruction(peLib->AllocateInstruction(
                        Instruction::i_ldsfld,
                        peLib->AllocateOperand(peLib->AllocateFieldName(LookupManagedField("__argv")))));  // load argcv
                if (n >= 3)
                    currentMethod->AddInstruction(peLib->AllocateInstruction(
                        Instruction::i_ldsfld,
                        peLib->AllocateOperand(peLib->AllocateFieldName(LookupManagedField("__env")))));  // load env
                for (int i = 3; i < n; i++)
                    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldnull, NULL));  // load a spare arg
                MethodSignature* signature = peLib->AllocateMethodSignature("main", MethodSignature::Managed, mainContainer);
                currentMethod->AddInstruction(peLib->AllocateInstruction(
                    Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(mainSym->Signature()))));
            }
        }
        else
        {
            currentMethod->AddInstruction(
                peLib->AllocateInstruction(Instruction::i_ldloca, peLib->AllocateOperand(localList[0])));  // load argc
            currentMethod->AddInstruction(
                peLib->AllocateInstruction(Instruction::i_ldloca, peLib->AllocateOperand(localList[1])));  // load argcv
            currentMethod->AddInstruction(
                peLib->AllocateInstruction(Instruction::i_ldloca, peLib->AllocateOperand(localList[2])));  // load environ
            currentMethod->AddInstruction(
                peLib->AllocateInstruction(Instruction::i_ldc_i4, peLib->AllocateOperand((longlong)0, Operand::i32)));
            currentMethod->AddInstruction(
                peLib->AllocateInstruction(Instruction::i_ldloca, peLib->AllocateOperand(localList[3])));  // load newmode

            MethodSignature* signature = peLib->AllocateMethodSignature("__getmainargs", 0, NULL);
            signature->ReturnType(peLib->AllocateType(Type::Void, 0));
            signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::Void, 1)));
            signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::Void, 1)));
            signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::Void, 1)));
            signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::i32, 0)));
            signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::Void, 1)));
            peLib->AddPInvokeReference(signature, pinvoke_dll, false);
            currentMethod->AddInstruction(
                peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));

            if (mainSym)
            {
                int n = mainSym->Signature()->ParamCount();
                if (n >= 1)
                    currentMethod->AddInstruction(
                        peLib->AllocateInstruction(Instruction::i_ldloc, peLib->AllocateOperand(localList[0])));  // load argc
                if (n >= 2)
                    currentMethod->AddInstruction(
                        peLib->AllocateInstruction(Instruction::i_ldloc, peLib->AllocateOperand(localList[1])));  // load argcv
                for (int i = 2; i < n; i++)
                    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldnull, NULL));  // load a spare arg
                signature = peLib->AllocateMethodSignature("main", MethodSignature::Managed, mainContainer);
                currentMethod->AddInstruction(peLib->AllocateInstruction(
                    Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(mainSym->Signature()))));
            }
        }
        dumpInitializerCalls(deinitializersHead);
        if (!mainSym || (mainSym && mainSym->Signature()->ReturnType()->IsVoid()))
            currentMethod->AddInstruction(
                peLib->AllocateInstruction(Instruction::i_ldc_i4, peLib->AllocateOperand((longlong)0, Operand::i32)));
        if (cparams.managed_library)
        {
            MethodSignature* signature = LookupManagedSignature("exit");
            currentMethod->AddInstruction(
                peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));
        }
        else
        {
            MethodSignature* signature = LookupSignature("exit");
            if (!signature)
            {

                signature = peLib->AllocateMethodSignature("exit", 0, NULL);
                signature->ReturnType(peLib->AllocateType(Type::Void, 0));
                signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::i32, 0)));
                peLib->AddPInvokeReference(signature, pinvoke_dll, false);
            }
            currentMethod->AddInstruction(
                peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));
        }
    }
}
static void dumpGlobalFuncs() {}

static void AddRTLThunks()
{
    Param* param;
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
        oa_enterseg((e_sg)0);
        for (auto ri = startups.begin(); ri != startups.end(); ++ri)
        {
            LIST* lst = (LIST*)peLib->AllocateBytes(sizeof(LIST));
            lst->data = (void*)ri->second;
            lst->next = initializersHead;
            initializersHead = lst;
        }
        for (auto ri = rundowns.rbegin(); ri != rundowns.rend(); ++ri)
        {
            LIST* lst = (LIST*)peLib->AllocateBytes(sizeof(LIST));
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
    if (cparams.no_default_libs)
    {
        // have to create various function signatures if not loading the library
        Namespace* ns = nullptr;
        if (peLib->Find("lsmsilcrtl", (void**)&ns, 0) != PELib::s_namespace)
            Utils::fatal("namespace lsmsilcrtl does not exist");
        Type* object = peLib->AllocateType(Type::object, 0);
        Type* voidPtr = peLib->AllocateType(Type::Void, 1);
        Type* objectArray = peLib->AllocateType(Type::object, 0);
        objectArray->ArrayLevel(1);
        Type* argstype = FindType("lsmsilcrtl.args", false);
        Class* args = nullptr;
        if (argstype)
        {
            args = static_cast<Class*>(argstype->GetClass());
        }
        else
        {
            args = peLib->AllocateClass("args", Qualifiers::Public, -1, -1);
            ns->Add(args);
            MethodSignature* sig =
                peLib->AllocateMethodSignature(".ctor", MethodSignature::Managed | MethodSignature::InstanceFlag, args);
            sig->ReturnType(objectArray);
            args->Add(peLib->AllocateMethod(sig, Qualifiers::Public));
            sig = peLib->AllocateMethodSignature("GetNextArg", MethodSignature::Managed | MethodSignature::InstanceFlag, args);
            sig->ReturnType(object);
            args->Add(peLib->AllocateMethod(sig, Qualifiers::Public));
            sig = peLib->AllocateMethodSignature("GetUnmanaged", MethodSignature::Managed | MethodSignature::InstanceFlag, args);
            sig->ReturnType(voidPtr);
            args->Add(peLib->AllocateMethod(sig, Qualifiers::Public));
        }
        Type* pointertype = FindType("lsmsilcrtl.pointer", false);
        Class* pointer = nullptr;
        if (pointertype)
        {
            pointer = static_cast<Class*>(pointertype->GetClass());
        }
        else
        {
            pointer = peLib->AllocateClass("pointer", Qualifiers::Public, -1, -1);
            ns->Add(pointer);
            MethodSignature* sig =
                peLib->AllocateMethodSignature("box", MethodSignature::Managed | MethodSignature::InstanceFlag, pointer);
            sig->ReturnType(object);
            sig->AddParam(peLib->AllocateParam("param", voidPtr));
            pointer->Add(peLib->AllocateMethod(sig, Qualifiers::Public | Qualifiers::Static));
            sig = peLib->AllocateMethodSignature("unbox", MethodSignature::Managed | MethodSignature::InstanceFlag, pointer);
            sig->ReturnType(voidPtr);
            sig->AddParam(peLib->AllocateParam("param", object));
            pointer->Add(peLib->AllocateMethod(sig, Qualifiers::Public | Qualifiers::Static));
        }
    }

    argsCtor = FindMethodSignature("lsmsilcrtl.args::.ctor");
    argsNextArg = FindMethodSignature("lsmsilcrtl.args::GetNextArg");
    argsUnmanaged = FindMethodSignature("lsmsilcrtl.args::GetUnmanaged");
    ptrBox = FindMethodSignature("lsmsilcrtl.pointer::box");
    ptrUnbox = FindMethodSignature("lsmsilcrtl.pointer::unbox");

    systemObject = FindType("System.Object", true);

    Type stringType(Type::string, 0);
    Type objectType(Type::object, 0);

    std::vector<Type*> strArgs;
    strArgs.push_back(&stringType);
    strArgs.push_back(&stringType);
    std::vector<Type*> objArgs;
    objArgs.push_back(&objectType);
    objArgs.push_back(&objectType);

    std::vector<Type*> toStrArgs;
    toStrArgs.push_back(&objectType);

    Method* result;
    if (peLib->Find("System.String::Concat", &result, strArgs) == PELib::s_method)
    {
        concatStr = result->Signature();
    }
    if (peLib->Find("System.String::Concat", &result, objArgs) == PELib::s_method)
    {
        concatObj = result->Signature();
    }
    if (peLib->Find("System.Convert::ToString", &result, toStrArgs) == PELib::s_method)
    {
        toStr = result->Signature();
    }
    if (!concatStr || !concatObj || !toStr)
        Utils::fatal("could not find builtin function");
}
void ReplaceName(std::map<std::string, Value*>& list, Value* v, char* name)
{
    MethodName* n = static_cast<MethodName*>(v);
    n->Signature()->SetName(name);
    list[name] = v;
}
void oa_gensrref(SimpleSymbol* sp, int val, int type)
{
    static int count = 1;
    Value* v = globalMethods[sp];
    if (v)
    {
        switch (type)
        {
            MethodName* n;
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
int oa_main_preprocess(void)
{

    PELib::CorFlags corFlags = PELib::bits32;
    if (prm_namespace_and_class[0])
        corFlags = (PELib::CorFlags)((int)corFlags | PELib::ilonly);
    char path[260], fileName[256];
    GetOutputFileName(fileName, path, cparams.prm_compileonly && !cparams.prm_asmfile);
    uniqueId = Utils::CRC32((unsigned char*)fileName, strlen(fileName));
    char* p = strrchr(fileName, '.');
    char* q = strrchr(fileName, '\\');
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
            Utils::fatal("could not load mscorlib.dll");
        }
        if (!cparams.no_default_libs && peLib->LoadAssembly("lsmsilcrtl"))
        {
            Utils::fatal("could not load lsmsilcrtl.dll");
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
    if (!prm_namespace_and_class.empty())
    {
        int npos = prm_namespace_and_class.find('.');
        std::string nspace = prm_namespace_and_class.substr(0, npos);
        std::string clss = prm_namespace_and_class.substr(npos + 1);
        Namespace* nm = peLib->AllocateNamespace(nspace);
        peLib->WorkingAssembly()->Add(nm);
        Class* cls = peLib->AllocateClass(clss, Qualifiers::MainClass | Qualifiers::Public, -1, -1);
        nm->Add(cls);
    }
    else
    {
        mainContainer = peLib->WorkingAssembly();
    }
    /**/
    if (newFile)
    {
        peLib->WorkingAssembly()->SetVersion(prm_assemblyVersion[0], prm_assemblyVersion[1], prm_assemblyVersion[2], prm_assemblyVersion[3]);
        peLib->WorkingAssembly()->SNKFile(prm_snkKeyFile);

        CreateExternalCSharpReferences();
        retblocksym.name = "__retblock";
    }
    return false;
}
static bool checkExterns(void)
{
    bool rv = false;
    for (std::map<SimpleSymbol*, Value*, byName>::iterator it = externalMethods.begin(); it != externalMethods.end(); ++it)
    {
        if (it->first->msil_rtl)
        {
            printf("Error: Undefined external symbol %s\n", it->first->name);
            rv = true;
        }
    }
    for (std::map<SimpleSymbol*, Value*, byName>::iterator it = externalList.begin(); it != externalList.end(); ++it)
    {
        const char* name = it->first->name;
        if (strcmp(name, "_pctype") && strcmp(name, "__stdin") && strcmp(name, "__stdout") && strcmp(name, "__stderr"))
        {
            printf("Error: Undefined external symbol %s\n", name);
            rv = true;
        }
    }
    return rv;
}
static bool validateGlobalRef(SimpleSymbol* sp1, SimpleSymbol* sp2)
{
    if (sp1->tp->type != st_func && sp2->tp->type != st_func)
    {
        SimpleType* tp = sp1->tp;
        SimpleType* tpx = sp2->tp;
        tp = tp;
        while (tp->type == st_pointer && tpx->type == st_pointer)
        {
            if (tpx->size != 0 && tp->size != 0 && tp->size != tpx->size)
                break;
            tp = tp->btp;
            tpx = tpx->btp;
        }
        if (tp->type != st_pointer && tpx->type != st_pointer)
        {
            if (tp->type == tpx->type)
            {
                if ((tp->type == st_struct || tp->type == st_union) || tp->type == st_func || tp->type == st_enum)
                {
                    if (!strcmp(tp->sp->name, tpx->sp->name))
                        return true;
                }
                else
                {
                    return true;
                }
            }
        }
        errCount++;
        printf("Error: Mismatch global type declarations on %s\n", sp1->name);
    }
    return false;
}
class PInvokeWeeder : public Callback
{
  public:
    PInvokeWeeder(PELib& PELib) : peLib(PELib), scanning(true) {}
    void SetOptimize() { scanning = false; }
    virtual bool EnterMethod(const Method* method) override
    {
        if (scanning)
        {
            for (auto ins : *static_cast<CodeContainer*>(const_cast<Method*>(method)))
            {
                Operand* op = ins->GetOperand();
                if (op)
                {
                    Value* v = op->GetValue();
                    if (v)
                    {
                        if (typeid(*v) == typeid(MethodName))
                        {
                            MethodSignature* ms = static_cast<MethodName*>(v)->Signature();
                            if (!(ms->Flags() & MethodSignature::Managed))  // pinvoke
                            {
                                pinvokeCounters[ms->Name()]++;
                                for (auto m : method->GetContainer()->Methods())
                                {
                                    if (static_cast<Method*>(m)->Signature()->Name() == ms->Name())
                                    {
                                        pinvokeCounters[ms->Name()]--;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            for (auto ins : *static_cast<CodeContainer*>(const_cast<Method*>(method)))
            {
                Operand* op = ins->GetOperand();
                if (op)
                {
                    Value* v = op->GetValue();
                    if (v)
                    {
                        if (typeid(*v) == typeid(MethodName))
                        {
                            MethodSignature* ms = static_cast<MethodName*>(v)->Signature();
                            if (!(ms->Flags() & MethodSignature::Managed))  // pinvoke
                            {
                                if (pinvokeCounters[ms->Name()] == 0)
                                {
                                    for (auto m : method->GetContainer()->Methods())
                                    {
                                        if (static_cast<Method*>(m)->Signature()->Name() == ms->Name())
                                        {
                                            ins->SetOperand(peLib.AllocateOperand(
                                                peLib.AllocateMethodName(static_cast<Method*>(m)->Signature())));
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            for (auto p : pinvokeCounters)
            {
                if (!p.second)
                {
                    peLib.RemovePInvokeReference(p.first);
                }
            }
        }
        return true;
    };

  private:
    PELib& peLib;
    bool scanning;
    std::map<std::string, int> pinvokeCounters;
};
void oa_main_postprocess(bool errors)
{
    if (cparams.prm_compileonly && !cparams.prm_asmfile)
    {
        // empty
    }
    else
    {
        char path[260];
        char ilName[260];
        GetOutputFileName(ilName, path, false);
        Utils::StripExt(ilName);
        AddRTLThunks();
        if (!errors && cparams.prm_targettype != DLL && !mainSym && !hasEntryPoint)
        {
            printf("Error: main not defined\n");
        }
        errors |= checkExterns() || errCount || (cparams.prm_targettype != DLL && !mainSym && !hasEntryPoint);
        if (!errors)
        {
            if (cparams.replacePInvoke)
            {
                PInvokeWeeder weeder(*peLib);
                peLib->Traverse(weeder);
                weeder.SetOptimize();
                peLib->Traverse(weeder);
            }

            if (cparams.prm_asmfile)
                Utils::AddExt(ilName, ".il");
            else if (cparams.prm_targettype == DLL)
                Utils::AddExt(ilName, ".dll");
            else
                Utils::AddExt(ilName, ".exe");
            peLib->DumpOutputFile(ilName,
                                  cparams.prm_asmfile ? PELib::ilasm : (cparams.prm_targettype == DLL ? PELib::pedll : PELib::peexe),
                                  cparams.prm_targettype != CONSOLE);
        }
    }
}
void oa_end_generation(void)
{
    if (cparams.prm_compileonly && !cparams.prm_asmfile)
    {
        cseg();
        for (auto it = externalList.begin(); it != externalList.end(); ++it)
        {
            Field* f = static_cast<FieldName*>(it->second)->GetField();
            f->External(true);
            mainContainer->Add(f);
        }
        for (auto it = externalMethods.begin(); it != externalMethods.end(); ++it)
        {
            int flags = Qualifiers::ManagedFunc | Qualifiers::Public;
            MethodSignature* s = static_cast<MethodName*>(it->second)->Signature();
            if (!s->GetContainer()->InAssemblyRef())
            {
                Method* m = peLib->AllocateMethod(s, flags);
                s->External(true);
                mainContainer->Add(m);
            }
        }
        char path[260];
        char ilName[260];
        GetOutputFileName(ilName, path, true);
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
        SimpleSymbol *start = NULL, *end = NULL;
        for (auto sym : externals)
        {
            if (!strncmp(sym->name, "__DYNAMIC", 9))
            {
                if (strstr(sym->name, "STARTUP"))
                    start = sym;
                else
                    end = sym;
            }
        }
        if (start)
        {
            LIST* lst = (LIST*)peLib->AllocateBytes(sizeof(LIST));
            static SimpleSymbol sp;
            sp.name = (char*)start->name;
            std::map<SimpleSymbol*, Value*, byName>::iterator it = globalMethods.find(&sp);
            MethodSignature* signature = static_cast<MethodName*>(it->second)->Signature();
            lst->data = (void*)peLib->AllocateMethodName(signature);
            if (initializersHead)
                initializersTail = initializersTail->next = lst;
            else
                initializersHead = initializersTail = lst;
        }
        if (end)
        {
            LIST* lst = (LIST*)peLib->AllocateBytes(sizeof(LIST));
            static SimpleSymbol sp1;
            sp1.name = (char*)end->name;
            std::map<SimpleSymbol*, Value*, byName>::iterator it = globalMethods.find(&sp1);
            MethodSignature* signature = static_cast<MethodName*>(it->second)->Signature();
            lst->data = (void*)peLib->AllocateMethodName(signature);
            if (deinitializersHead)
                deinitializersTail = deinitializersTail->next = lst;
            else
                deinitializersHead = initializersTail = lst;
        }
    }
}
void oa_put_extern(SimpleSymbol* sp, int code)
{
    if (sp->tp->type == st_func)
    {
        if (!msil_managed(sp))
        {
        }
        else if (sp->msil_rtl)
        {
            CacheExtern(sp);
        }
    }
    else
    {
        CacheExtern(sp);
    }
}
void oa_gen_strlab(SimpleSymbol* sp)
/*
 *      generate a named label.
 */
{
    oa_enterseg((e_sg)0);
    if (sp->storage_class != scc_localstatic && sp->storage_class != scc_constant && sp->storage_class != scc_static)
    {
        CacheGlobal(sp);
    }
    else
    {
        CacheStatic(sp);
    }
    if (sp->tp->type == st_func)
    {
        CreateFunction(GetMethodSignature(sp), sp);
    }
    //    else
    //    {
    //        CreateField(sp);
    //    }
}
Type* GetStringType(int type)
{

    std::string name;
    switch (type)
    {
        case l_ustr:
        case l_astr:
            name = "int8[]";
            break;
        case l_Ustr:
            name = "int32[]";
            break;
        case l_wstr:
            name = "int16[]";
            break;
    }
    std::map<std::string, Type*>::iterator it = typeList.find(name);
    if (it != typeList.end())
        return it->second;
    Class* newClass = peLib->AllocateClass(
        name, Qualifiers::Private | Qualifiers::Value | Qualifiers::Explicit | Qualifiers::Ansi | Qualifiers::Sealed, 1, 1);
    mainContainer->Add(newClass);
    Type* type2 = peLib->AllocateType(newClass);
    typeList[name] = type2;
    return type2;
}
Value* GetStringFieldData(int lab, int type)
{
    SimpleSymbol* sp = (SimpleSymbol*)Alloc(sizeof(SimpleSymbol));
    sp->name = "$$$";
    sp->label = lab;

    std::map<SimpleSymbol*, Value*, byLabel>::iterator it = staticList.find(sp);
    if (it != staticList.end())
        return it->second;

    char buf[256];
    sprintf(buf, "L_%d_%x", lab, uniqueId);
    Field* field =
        peLib->AllocateField(buf, GetStringType(type), Qualifiers::ClassField | Qualifiers::Private | Qualifiers::Static);
    mainContainer->Add(field);
    sp = clone(sp);
    Value* v = peLib->AllocateFieldName(field);
    staticList[sp] = v;

    return v;
}
void oa_put_string_label(int lab, int type)
{
    oa_enterseg((e_sg)0);

    Field* field = static_cast<FieldName*>(GetStringFieldData(lab, type))->GetField();

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
        dataPointer = (Byte*)realloc(dataPointer, dataMax);
    }
    else if (dataPos >= dataMax)
    {
        dataMax *= 2;
        dataPointer = (Byte*)realloc(dataPointer, dataMax);
    }
    if (!dataPointer)
        Utils::fatal("out of memory");
    dataPointer[dataPos++] = bt;
}

/*-------------------------------------------------------------------------*/

void oa_genstring(char* str, int len)
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

void oa_enterseg(e_sg segnum)
{
    if (initializingField && dataPos)
    {
        oa_genbyte(0);  // we only put strings literally into the text, and the strings
        // need a terminating zero which has been elided because we don't do anything else
        Byte* v = peLib->AllocateBytes(dataPos);
        memcpy(v, dataPointer, dataPos);
        initializingField->AddInitializer(v, dataPos);
    }
    initializingField = NULL;
    dataPos = 0;
}