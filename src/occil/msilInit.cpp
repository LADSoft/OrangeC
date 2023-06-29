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

#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include "be.h"
#include "winmode.h"
#include "DotNetPELib.h"
#include "Utils.h"
#include <unordered_map>
#include <algorithm>
#include "config.h"
#include "ildata.h"
#include "occil.h"
#include "using.h"
#include "MsilProcess.h"
#define STARTUP_TYPE_STARTUP 1
#define STARTUP_TYPE_RUNDOWN 2
#define STARTUP_TYPE_TLS_STARTUP 3
#define STARTUP_TYPE_TLS_RUNDOWN 4

#include <vector>
#include <string>
using namespace DotNetPELib;
extern PELib* peLib;

namespace occmsil
{
int uniqueId;
Optimizer::SimpleSymbol retblocksym;
int errCount;
Method* mainSym;
int hasEntryPoint;

MethodSignature* argsCtor;
MethodSignature* argsNextArg;
MethodSignature* argsUnmanaged;
MethodSignature* ptrBox;
MethodSignature* ptrUnbox;
MethodSignature* concatStr;
MethodSignature* concatObj;
MethodSignature* toStr;
MethodSignature* toInt;
MethodSignature* toVoidStar;
MethodSignature* delegateInvoker;
MethodSignature* delegateAllocator;
MethodSignature* delegateFreer;
Class* multicastDelegate;

Type* systemObject;
Type* intPtr;

Method* currentMethod;
DataContainer* mainContainer;
Optimizer::LIST *initializersHead, *initializersTail;
Optimizer::LIST *deinitializersHead, *deinitializersTail;

std::map<Optimizer::SimpleSymbol*, Value*, byName> externalMethods;
std::map<Optimizer::SimpleSymbol*, Value*, byName> externalList;
std::map<Optimizer::SimpleSymbol*, Value*, byName> globalMethods;
std::map<Optimizer::SimpleSymbol*, Value*, byName> globalList;
std::map<Optimizer::SimpleSymbol*, Value*, byLabel> staticMethods;
std::map<Optimizer::SimpleSymbol*, Value*, byLabel> staticList;
std::map<Optimizer::SimpleSymbol*, MethodSignature*, byName> pinvokeInstances;
std::map<Optimizer::SimpleSymbol*, Param*, byName> paramList;
std::multimap<std::string, MethodSignature*> pInvokeReferences;

std::map<std::string, Value*> startups, rundowns, tlsstartups, tlsrundowns;

std::map<std::string, Type*> typeList;
std::map<Optimizer::SimpleSymbol*, Value*, byField> fieldList;
std::map<std::string, MethodSignature*> arrayMethods;

std::vector<Local*> localList;

MethodSignature* FindMethodSignature(const char* name)
{
    void* result;
    if (peLib->Find(name, &result) == PELib::s_method)
    {
        return static_cast<Method*>(result)->Signature();
    }
    Utils::fatal("could not find built in method %s", name);
    return NULL;
}

MethodSignature* FindMethodSignature(const char* name, std::vector<Type*>& typeList, Type* rv = nullptr)
{
    Method* result;
    if (peLib->Find(name, &result, typeList, rv) == PELib::s_method)
    {
        return result->Signature();
    }
    Utils::fatal("could not find built in method %s", name);
    return NULL;
}

Type* FindType(const char* name, bool toErr)
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

static void CreateExternalCSharpReferences()
{
    if (Optimizer::cparams.no_default_libs)
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
                peLib->AllocateMethodSignature("ToPointer", MethodSignature::Managed | MethodSignature::InstanceFlag, pointer);
            Type* rt = peLib->AllocateType(Type::i8, 1);
            sig->ReturnType(rt);
            sig->AddParam(peLib->AllocateParam("param", peLib->AllocateType(Type::string, 0)));
        }
    }

    argsCtor = FindMethodSignature("lsmsilcrtl.args::.ctor");
    argsNextArg = FindMethodSignature("lsmsilcrtl.args::GetNextArg");
    argsUnmanaged = FindMethodSignature("lsmsilcrtl.args::GetUnmanaged");
    ptrBox = FindMethodSignature("lsmsilcrtl.pointer::box");
    ptrUnbox = FindMethodSignature("lsmsilcrtl.pointer::unbox");
    intPtr = peLib->AllocateType(Type::inative, 0);

    if (Optimizer::delegateforfuncptr)
    {
        delegateInvoker = peLib->AllocateMethodSignature("__OCCMSIL_Invoke", MethodSignature::Vararg, nullptr);
        delegateInvoker->ReturnType(peLib->AllocateType(Type::Void, 1));
        delegateInvoker->AddParam(peLib->AllocateParam("func", peLib->AllocateType(Type::Void, 1)));
        delegateInvoker->AddParam(peLib->AllocateParam("func", peLib->AllocateType(Type::i32, 0)));
        peLib->AddPInvokeReference(delegateInvoker, "occmsil.dll", true);
        delegateAllocator = FindMethodSignature("lsmsilcrtl.MethodPtr::Allocate");
        delegateFreer = FindMethodSignature("lsmsilcrtl.MethodPtr::Free");
        multicastDelegate = static_cast<Class*>(FindType("System.MulticastDelegate", true)->GetClass());
    }
    Type* voidStar = peLib->AllocateType(Type::Void, 1);
    std::vector<Type*> params = {voidStar};
    toInt = FindMethodSignature("System.IntPtr.op_Explicit", params);
    params.clear();
    params.push_back(intPtr);
    toVoidStar = FindMethodSignature("System.IntPtr.op_Explicit", params, voidStar);

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

int msil_main_preprocess(char* fileName)
{

    PELib::CorFlags corFlags = PELib::bits32;
    if (Optimizer::prm_namespace_and_class[0])
        corFlags = (PELib::CorFlags)((int)corFlags | PELib::ilonly);
    char path[260];
    strcpy(path, fileName);
    //    GetOutputFileName(fileName, path, cparams.prm_compileonly && !cparams.prm_asmfile);
    uniqueId = Utils::CRC32((unsigned char*)fileName, strlen(fileName));
    char* p = (char*)strrchr(path, '.');
    char* q = (char*)strrchr(path, '\\');
    if (!q)
        q = path;
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
        if (!Optimizer::cparams.no_default_libs && peLib->LoadAssembly("lsmsilcrtl"))
        {
            Utils::fatal("could not load lsmsilcrtl.dll");
        }
        _apply_global_using();

        // peLib->AddUsing("System");
        newFile = true;
    }
    else
    {
        peLib->EmptyWorkingAssembly(q);
        newFile = false;
    }
    //    if (p)
    //    {
    //        *p = '.';
    //    }
    if (!Optimizer::prm_namespace_and_class.empty())
    {
        int npos = Optimizer::prm_namespace_and_class.find('.');
        std::string nspace = Optimizer::prm_namespace_and_class.substr(0, npos);
        std::string clss = Optimizer::prm_namespace_and_class.substr(npos + 1);
        Namespace* nm = peLib->AllocateNamespace(nspace);
        peLib->WorkingAssembly()->Add(nm);
        Class* cls = peLib->AllocateClass(clss, Qualifiers::MainClass | Qualifiers::Public, -1, -1);
        nm->Add(cls);
        mainContainer = cls;
    }
    else
    {
        mainContainer = peLib->WorkingAssembly();
    }
    /**/
    if (newFile)
    {
        int vers[4];
        memset(vers, 0, sizeof(vers));
        sscanf(Optimizer::prm_assemblyVersion.c_str(), "%d.%d.%d.%d", &vers[0], &vers[1], &vers[2], &vers[3]);
        peLib->WorkingAssembly()->SetVersion(vers[0], vers[1], vers[2], vers[3]);
        peLib->WorkingAssembly()->SNKFile(Optimizer::prm_snkKeyFile);

        CreateExternalCSharpReferences();
        retblocksym.name = "__retblock";
    }
    return false;
}
void msil_end_generation(char* fileName)
{
    if (Optimizer::cparams.prm_compileonly && !Optimizer::cparams.prm_asmfile)
    {
#ifndef ISPARSER
        if (Optimizer::pinning)
        {
            
            CreateStringFunction();
        }
#endif
        Optimizer::cseg();
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
        if (fileName)
            peLib->DumpOutputFile(fileName, PELib::object, false);
        // needs work        delete peLib;

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
}
void msil_compile_start(char* name)
{
    _using_init();
    Import();
    staticList.clear();
}
}  // namespace occmsil