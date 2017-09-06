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

contact information:
email: TouchStone222@runbox.com <David Lindauer>
*/
#include "NetLinkMain.h"

#include "DotNetPELib.h"

using namespace DotNetPELib;

CmdSwitchParser NetLinkMain::SwitchParser;
CmdSwitchString NetLinkMain::StrongName(SwitchParser, 'k');
CmdSwitchBool NetLinkMain::LibraryFile(SwitchParser, 'L');
CmdSwitchBool NetLinkMain::AssemblyFile(SwitchParser, 'S');
CmdSwitchBool NetLinkMain::GUIApp(SwitchParser, 'g');
CmdSwitchOutput NetLinkMain::AssemblyName(SwitchParser, 'o', "");
CmdSwitchString NetLinkMain::AssemblyVersion(SwitchParser, 'v');
CmdSwitchFile NetLinkMain::File(SwitchParser, '@');


char *NetLinkMain::usageText = "[options] inputfiles\n"
        "\n"
        "/L         generate library          /S         generate .IL file\n"
        "/g         WIN32 GUI application     /kxxx      set strong name key\n"
        "/oxxx      specify assembly name     /vx.x.x.x  set assembly version\n"
        "@xxx       Read commands from file\n"
        "\nTime: " __TIME__ "  Date: " __DATE__;

int main(int argc, char **argv)
{
    NetLinkMain linker;
    return linker.Run(argc, argv);
}
const std::string &NetLinkMain::GetAssemblyName(CmdFiles &files)
{
    std::string rv;
    static std::string assemblyName;
    if (AssemblyName.GetValue().size() != 0)
        assemblyName = AssemblyName.GetValue();
    else if (files.GetSize())
    {
        CmdFiles::FileNameIterator it = files.FileNameBegin();
        assemblyName = (*it)->c_str();
    }
    else
    {
        std::cout << "Nothing to do." << std::endl;
        exit(1);
    }
    int n = assemblyName.find_last_of(CmdFiles::DIR_SEP[0]);
    if (n != std::string::npos)
        if (n == assemblyName.size() - 1)
        {
            std::cout << "Invalid assembly name" << std::endl;
            exit(1);
        }
        else
        {
            assemblyName = assemblyName.substr(n + 1);
        }
    return assemblyName;
}
const std::string &NetLinkMain::GetOutputFile(CmdFiles &files)
{
    std::string rv;
    static std::string outputFile;
    if (AssemblyName.GetValue().size() != 0)
        outputFile = AssemblyName.GetValue();
    else if (files.GetSize())
    {
        CmdFiles::FileNameIterator it = files.FileNameBegin();
        outputFile = (*it)->c_str();
    }
    else
    {
        std::cout << "Nothing to do." << std::endl;
        exit(1);
    }
    outputFile = Utils::QualifiedFile(outputFile.c_str(), AssemblyFile.GetValue() ? ".il" : LibraryFile.GetValue() ? ".dll" : ".exe");
    return outputFile;
}
bool NetLinkMain::LoadImage(CmdFiles &files)
{
    bool rv = true;
    for (CmdFiles::FileNameIterator it = files.FileNameBegin(); it != files.FileNameEnd(); ++it)
    {
        std::string fileName = **it;
        int n = fileName.find_last_of('.');
        if (n == std::string::npos)
        {
            fileName += ".ilo";
        }
        else if (n > 0)
        {
            if (fileName[n - 1] == '.')
            {
                fileName += ".ilo";
            }
        }
        if (!peLib->LoadObject(fileName))
        {
            std::cout << "Error loading object file: " << fileName.c_str() << std::endl;
            rv = false;
        }
    }
    return rv;
}
class Validator : public Callback
{
public:
    Validator(DataContainer *Parent) : parent(Parent), error(false) { }
    bool Failed() 
    {
        return error;
    }
private:
    virtual bool EnterMethod(const Method *method) override
    {
        if (method->Signature()->External())
        {
            ExternalError(method->Signature()->Name());
        }
        else if (method->Signature()->Definitions() > 1)
        {
            PublicError(method->Signature()->Name());
        }
        return true;
    };
    virtual bool EnterField(const Field *field) override
    {
        if (field->External())
        {
            ExternalError(field->Name());
        }
        else if (field->Definitions() > 1)
        {
            if (field->GetContainer() == parent)
                PublicError(field->Name());
        }
        return true;
    };
    void ExternalError(std::string name)
    {
        std::cout << "Error: missing external '" << name.c_str() << "'" << std::endl;
        error = true;
    }
    void PublicError(std::string name)
    {
        std::cout << "Error: duplicate public '" << name.c_str() << "'" << std::endl;
        error = true;

    }
    DataContainer *parent;
    bool error;
};
class Optimizer : public Callback
{
public:
    Optimizer(PELib &PELib) : peLib(PELib), error(false) { }
    bool Failed()
    {
        return error;
    }
private:
    virtual bool EnterMethod(const Method *method) override
    {
        try
        {
            const_cast<Method *>(method)->Optimize(peLib);
        }
        catch (PELibError exc)
        {
            std::cout << "Optimizer error: (" << method->Signature()->Name() << ") " << exc.what() << std::endl;
            error = true;
        }
        return true;
    };
    PELib &peLib;
    bool error;
};
bool NetLinkMain::Validate()
{
    Validator validator(mainContainer);
    Optimizer optimizer(*peLib);
    peLib->Traverse(validator);
    if (!validator.Failed())
        peLib->Traverse(optimizer);
    return !validator.Failed() && !optimizer.Failed();
}
MethodSignature *NetLinkMain::LookupSignature(char * name)
{
    Method *result;
    PELib::eFindType rv = peLib->Find(const_cast<char *>((namespaceAndClass + name).c_str()), &result, std::vector<Type *> { }, nullptr, false);
    if (rv == PELib::s_method)
    {
        return result->Signature();
    }
    result = peLib->FindPInvoke(name);
    if (result)
    {
        return result->Signature();
    }
    return NULL;

}
Field *NetLinkMain::LookupField(char *name)
{
    void *result;
    PELib::eFindType rv = peLib->Find(const_cast<char *>((namespaceAndClass + name).c_str()), &result);
    if (rv == PELib::s_field)
    {
        return static_cast<Field *>(result);
    }
    return NULL;
}
void NetLinkMain::MainLocals(void)
{
    localList.clear();
    localList.push_back(peLib->AllocateLocal("argc", peLib->AllocateType(Type::i32, 0)));
    localList.push_back(peLib->AllocateLocal("argv", peLib->AllocateType(Type::Void, 1)));
    localList.push_back(peLib->AllocateLocal("environ", peLib->AllocateType(Type::Void, 1)));
    localList.push_back(peLib->AllocateLocal("newmode", peLib->AllocateType(Type::Void, 1)));
}
void NetLinkMain::MainInit(void)
{
    std::string name = "$Main";
    int flags = Qualifiers::Private | Qualifiers::HideBySig | Qualifiers::Static | Qualifiers::CIL | Qualifiers::Managed;
    if (LibraryFile.GetValue())
    {
        name = ".cctor";
        flags |= Qualifiers::SpecialName | Qualifiers::RTSpecialName;
    }
    MethodSignature *signature = peLib->AllocateMethodSignature(name, MethodSignature::Managed, mainContainer);
    signature->ReturnType(peLib->AllocateType(Type::Void, 0));
    currentMethod = peLib->AllocateMethod(signature, flags, !LibraryFile.GetValue());
    mainContainer->Add(currentMethod);

    signature = LookupSignature("__pctype_func");
    if (!signature)
    {
        signature = peLib->AllocateMethodSignature("__pctype_func", 0, NULL);
        signature->ReturnType(peLib->AllocateType(Type::u16, 1));
        peLib->AddPInvokeReference(signature, "msvcrt.dll", false);
    }
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));

    Field *field = LookupField("_pctype");
    if (!field)
    {
        field = peLib->AllocateField("_pctype", peLib->AllocateType(Type::u16, 1), Qualifiers::Public | Qualifiers::Static);
        mainContainer->Add(field);
    }
    else
    {
        field->External(false);
    }
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));

    signature = LookupSignature("__iob_func");
    if (!signature)
    {
        signature = peLib->AllocateMethodSignature("__iob_func", 0, NULL);
        signature->ReturnType(peLib->AllocateType(Type::Void, 1));
        peLib->AddPInvokeReference(signature, "msvcrt.dll", false);
    }
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));

    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_dup));
    field = LookupField("__stdin");
    if (!field)
    {
        field = peLib->AllocateField("__stdin", peLib->AllocateType(Type::Void, 1), Qualifiers::Public | Qualifiers::Static);
        mainContainer->Add(field);
    }
    else
    {
        field->External(false);
    }
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));

    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_dup));
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldc_i4, peLib->AllocateOperand((longlong)32, Operand::any)));
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_add));

    field = LookupField("__stdout");
    if (!field)
    {
        field = peLib->AllocateField("__stdout", peLib->AllocateType(Type::Void, 1), Qualifiers::Public | Qualifiers::Static);
        mainContainer->Add(field);
    }
    else
    {
        field->External(false);
    }
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));

    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldc_i4, peLib->AllocateOperand((longlong)64, Operand::any)));
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_add));

    field = LookupField("__stderr");
    if (!field)
    {
        field = peLib->AllocateField("__stderr", peLib->AllocateType(Type::Void, 1), Qualifiers::Public | Qualifiers::Static);
        mainContainer->Add(field);
    }
    else
    {
        field->External(false);
    }
    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));

}
void NetLinkMain::dumpInitializerCalls(std::list<MethodSignature *> &lst)
{
    for (std::list<MethodSignature *>::iterator it = lst.begin(); it != lst.end(); ++it)
    {
        MethodSignature *signature = *it;
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));
    }
}
void NetLinkMain::dumpCallToMain(void)
{
    if (!LibraryFile.GetValue())
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
        peLib->AddPInvokeReference(signature, "msvcrt.dll", false);
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));

        if (mainSym)
        {
            int n = mainSym->ParamCount();
            if (n >= 1)
                currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldloc, peLib->AllocateOperand(localList[0]))); // load argc
            if (n >= 2)
                currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldloc, peLib->AllocateOperand(localList[1]))); // load argcv
            for (int i = 2; i < n; i++)
                currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldnull, NULL)); // load a spare arg
            signature = peLib->AllocateMethodSignature("main", MethodSignature::Managed, mainContainer);
            currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(mainSym))));
        }
        dumpInitializerCalls(destructors);

        if (!mainSym || mainSym && mainSym->ReturnType()->IsVoid())
            currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldc_i4, peLib->AllocateOperand((longlong)0, Operand::i32)));
        signature = LookupSignature("exit");
        if (!signature)
        {

            signature = peLib->AllocateMethodSignature("exit", 0, NULL);
            signature->ReturnType(peLib->AllocateType(Type::Void, 0));
            signature->AddParam(peLib->AllocateParam("", peLib->AllocateType(Type::i32, 0)));
            peLib->AddPInvokeReference(signature, "msvcrt.dll", false);
        }
        currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));
    }
}
void NetLinkMain::dumpGlobalFuncs()
{
}

bool NetLinkMain::EnterNamespace(const Namespace *nmspc)
{
    if (!mainNameSpace)
        if (!nmspc->InAssemblyRef())
            mainNameSpace = nmspc;
    return true;
}
bool NetLinkMain::EnterClass(const Class *cls)
{
    if (!mainClass)
    {
        DataContainer *dc = const_cast<Class *>(cls)->Parent();
        if (dc == mainNameSpace)
        {
            mainClass = cls;
            namespaceAndClass = mainNameSpace->Name() + "." + cls->Name() + "::";
        }
    }
    return true;
}
bool NetLinkMain::EnterMethod(const Method *method)
{
    // this may be is broken, it needs to do it in the order the source files were encountered...
    if (!strncmp(method->Signature()->Name().c_str(), "__DYNAMIC", 9))
    {
        if (strstr(method->Signature()->Name().c_str(), "STARTUP"))
            initializers.push_back(method->Signature());
        else
            destructors.push_back(method->Signature());
    }
    if (method->HasEntryPoint())
    {
        if (hasEntryPoint)
            std::cout << "Multiple entry points encountered ";
        hasEntryPoint = true;
    }
    return true;
}
bool NetLinkMain::AddRTLThunks()
{
    Param *param;
    peLib->Traverse(*this);
    if (!hasEntryPoint)
    {
        if (mainClass)
            mainContainer = const_cast<DataContainer*>(static_cast<const DataContainer *>(mainClass));
        else
            mainContainer = peLib->WorkingAssembly();
        mainSym = LookupSignature("main");
        if (mainSym)
        {
            if (mainSym->ParamCount() < 1)
            {
                param = peLib->AllocateParam("argc", peLib->AllocateType(Type::i32, 0));
                mainSym->AddParam(param);
            }
            if (mainSym->ParamCount() < 2)
            {
                param = peLib->AllocateParam("argv", peLib->AllocateType(Type::Void, 1));
                mainSym->AddParam(param);
            }
        }

        MainInit();
        dumpInitializerCalls(initializers);
        MainLocals();
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
            return false;
        }

        dumpGlobalFuncs();
    }
    return true;
}
bool NetLinkMain::CreateExecutable(CmdFiles &files)
{
    return peLib->DumpOutputFile(GetOutputFile(files).c_str(), AssemblyFile.GetValue() ? PELib::ilasm : LibraryFile.GetValue() ? PELib::pedll : PELib::peexe, GUIApp.GetValue());
}
int NetLinkMain::Run(int argc, char **argv)
{
    Utils::banner(argv[0]);
    char *modName = Utils::GetModuleName();
    CmdSwitchFile internalConfig(SwitchParser);
    std::string configName = Utils::QualifiedFile(argv[0], ".cfg");
    std::fstream configTest(configName.c_str(), std::ios::in);
    if (!configTest.fail())
    {
        configTest.close();
        if (!internalConfig.Parse(configName.c_str()))
            Utils::fatal("Corrupt configuration file");
    }
    if (!SwitchParser.Parse(&argc, argv) || (argc == 1 && File.GetCount() <= 1))
    {
        Utils::usage(argv[0], usageText);
    }
    CmdFiles files(argv + 1);
    if (File.GetValue())
        files.Add(File.GetValue() + 1);
    peLib = new PELib(GetAssemblyName(files), PELib::bits32); // ilonly set by dotnetpelib 
    int assemblyVersion[4] = { 0 };
    if (AssemblyVersion.GetValue().size())
    {
        if (sscanf(AssemblyVersion.GetValue().c_str(), "%d.%d.%d.%d", &assemblyVersion[0], &assemblyVersion[1], &assemblyVersion[2], &assemblyVersion[3]) != 4)
        {
            std::cout << "Invalid assembly version string" << std::endl;
            delete peLib;
            return 1;
        }
    }
    peLib->WorkingAssembly()->SetVersion(assemblyVersion[0], assemblyVersion[1], assemblyVersion[2], assemblyVersion[3]);
    peLib->WorkingAssembly()->SNKFile(StrongName.GetValue().c_str());
    if (!LoadImage(files))
    {
        delete peLib;
        return 1;
    }
    if (!AddRTLThunks())
    {
        std::cout << "internal error" << std::endl;
        delete peLib;
        return 1;
    }
    if (!Validate())
    {
        delete peLib;
        return 1;
    }
    if (!CreateExecutable(files))
    {
        std::cout << "Error creating executable" << std::endl;
        delete peLib;
        return 1;
    }
    delete peLib;
    return 0;
}