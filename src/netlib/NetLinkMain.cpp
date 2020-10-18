/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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
CmdSwitchBool NetLinkMain::CManaged(SwitchParser, 'M');
CmdSwitchBool NetLinkMain::NoDefaultlibs(SwitchParser, 'n');
CmdSwitchBool NetLinkMain::WeedPInvokes(SwitchParser, 'P');

const char* NetLinkMain::usageText =
    "[options] inputfiles\n"
    "\n"
    "/L         generate library          /S         generate .IL file\n"
    "/g         WIN32 GUI application     /kxxx      set strong name key\n"
    "/n         no default libs           /oxxx      specify assembly name\n"
    "/vx.x.x.x  set assembly version      /M         managed mode\n"
    "/P         replace pinvokes\n"
    "@xxx       Read commands from file\n"
    "\nTime: " __TIME__ "  Date: " __DATE__;

int main(int argc, char** argv)
{
    NetLinkMain linker;
    try
    {
        return linker.Run(argc, argv);
    }
    catch (std::ios_base::failure)
    {
        return 1;
    }
    catch (DotNetPELib::PELibError)
    {
        return 1;
    }
}
const std::string& NetLinkMain::GetAssemblyName(CmdFiles& files)
{
    std::string rv;
    static std::string assemblyName;
    if (AssemblyName.GetValue().size() != 0)
        assemblyName = AssemblyName.GetValue();
    else if (files.GetSize())
    {
        CmdFiles::FileNameIterator it = files.FileNameBegin();
        assemblyName = (*it).c_str();
    }
    else
    {
        std::cout << "Nothing to do." << std::endl;
        exit(1);
    }
    int n = assemblyName.find_last_of(CmdFiles::DIR_SEP[0]);
    if (n != std::string::npos)
    {
        if (n == assemblyName.size() - 1)
        {
            std::cout << "Invalid assembly name" << std::endl;
            exit(1);
        }
        else
        {
            assemblyName = assemblyName.substr(n + 1);
        }
    }
    return assemblyName;
}
const std::string& NetLinkMain::GetOutputFile(CmdFiles& files)
{
    std::string rv;
    static std::string outputFile;
    if (AssemblyName.GetValue().size() != 0)
        outputFile = AssemblyName.GetValue();
    else if (files.GetSize())
    {
        CmdFiles::FileNameIterator it = files.FileNameBegin();
        outputFile = (*it).c_str();
    }
    else
    {
        std::cout << "Nothing to do." << std::endl;
        exit(1);
    }
    outputFile =
        Utils::QualifiedFile(outputFile.c_str(), AssemblyFile.GetValue() ? ".il" : LibraryFile.GetValue() ? ".dll" : ".exe");
    return outputFile;
}
bool NetLinkMain::LoadImage(CmdFiles& files)
{
    bool rv = true;
    for (CmdFiles::FileNameIterator it = files.FileNameBegin(); it != files.FileNameEnd(); ++it)
    {
        std::string fileName = *it;
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
    Validator(DataContainer* Parent) : parent(Parent), error(false) {}
    bool Failed() { return error; }

  private:
    virtual bool EnterMethod(const Method* method) override
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
    virtual bool EnterField(const Field* field) override
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
    DataContainer* parent;
    bool error;
};
class Optimizer : public Callback
{
  public:
    Optimizer(PELib& PELib) : peLib(PELib), error(false) {}
    bool Failed() { return error; }

  private:
    virtual bool EnterMethod(const Method* method) override
    {
        try
        {
            const_cast<Method*>(method)->Optimize(peLib);
        }
        catch (PELibError exc)
        {
            std::cout << "Optimizer error: (" << method->Signature()->Name() << ") " << exc.what() << std::endl;
            error = true;
        }
        return true;
    };
    PELib& peLib;
    bool error;
};
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
bool NetLinkMain::Validate()
{
    if (WeedPInvokes.GetValue())
    {
        PInvokeWeeder weeder(*peLib);
        peLib->Traverse(weeder);
        weeder.SetOptimize();
        peLib->Traverse(weeder);
    }
    Validator validator(mainContainer);
    Optimizer optimizer(*peLib);
    peLib->Traverse(validator);
    if (!validator.Failed())
        peLib->Traverse(optimizer);
    return !validator.Failed() && !optimizer.Failed();
}
MethodSignature* NetLinkMain::LookupSignature(const char* name)
{
    Method* result;
    PELib::eFindType rv =
        peLib->Find(const_cast<char*>((namespaceAndClass + name).c_str()), &result, std::vector<Type*>{}, nullptr, nullptr, nullptr, false);
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
MethodSignature* NetLinkMain::LookupManagedSignature(const char* name)
{
    Method* rv = nullptr;
    peLib->Find(std::string("lsmsilcrtl.rtl::") + name, &rv, std::vector<Type*>{}, nullptr, nullptr, nullptr, false);
    if (rv)
        return rv->Signature();
    return nullptr;
}
Field* NetLinkMain::LookupField(const char* name)
{
    void* result;
    PELib::eFindType rv = peLib->Find(const_cast<char*>((namespaceAndClass + name).c_str()), &result);
    if (rv == PELib::s_field)
    {
        return static_cast<Field*>(result);
    }
    return NULL;
}
Field* NetLinkMain::LookupManagedField(const char* name)
{
    void* rv = nullptr;
    if (peLib->Find(std::string("lsmsilcrtl.rtl::") + name, &rv) == PELib::s_field)
    {
        return static_cast<Field*>(rv);
    }
    return nullptr;
}
void NetLinkMain::MainLocals(void)
{
    localList.clear();
    if (!CManaged.GetValue())
    {
        localList.push_back(peLib->AllocateLocal("argc", peLib->AllocateType(Type::i32, 0)));
        localList.push_back(peLib->AllocateLocal("argv", peLib->AllocateType(Type::Void, 1)));
        localList.push_back(peLib->AllocateLocal("environ", peLib->AllocateType(Type::Void, 1)));
        localList.push_back(peLib->AllocateLocal("newmode", peLib->AllocateType(Type::Void, 1)));
    }
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
    MethodSignature* signature = peLib->AllocateMethodSignature(name, MethodSignature::Managed, mainContainer);
    signature->ReturnType(peLib->AllocateType(Type::Void, 0));
    currentMethod = peLib->AllocateMethod(signature, flags, !LibraryFile.GetValue());
    mainContainer->Add(currentMethod);

    if (CManaged.GetValue())
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
            peLib->AddPInvokeReference(signature, "msvcrt.dll", false);
        }
        currentMethod->AddInstruction(
            peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));

        Field* field = LookupField("_pctype");
        if (!field)
        {
            field = peLib->AllocateField("_pctype", peLib->AllocateType(Type::u16, 1), Qualifiers::Public | Qualifiers::Static);
            mainContainer->Add(field);
        }
        else
        {
            field->External(false);
        }
        currentMethod->AddInstruction(
            peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));

        signature = LookupSignature("__iob_func");
        if (!signature)
        {
            signature = peLib->AllocateMethodSignature("__iob_func", 0, NULL);
            signature->ReturnType(peLib->AllocateType(Type::Void, 1));
            peLib->AddPInvokeReference(signature, "msvcrt.dll", false);
        }
        currentMethod->AddInstruction(
            peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));

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
            mainContainer->Add(field);
        }
        else
        {
            field->External(false);
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
            mainContainer->Add(field);
        }
        else
        {
            field->External(false);
        }
        currentMethod->AddInstruction(
            peLib->AllocateInstruction(Instruction::i_stsfld, peLib->AllocateOperand(peLib->AllocateFieldName(field))));
    }
}
void NetLinkMain::dumpInitializerCalls(std::list<MethodSignature*>& lst)
{
    for (auto signature : lst)
        currentMethod->AddInstruction(
            peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));
}
void NetLinkMain::dumpCallToMain(void)
{
    if (!LibraryFile.GetValue())
    {
        if (CManaged.GetValue())
        {
            if (mainSym)
            {
                int n = mainSym->ParamCount();
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
                currentMethod->AddInstruction(
                    peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(mainSym))));
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
            peLib->AddPInvokeReference(signature, "msvcrt.dll", false);
            currentMethod->AddInstruction(
                peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));
            if (mainSym)
            {
                int n = mainSym->ParamCount();
                if (n >= 1)
                    currentMethod->AddInstruction(
                        peLib->AllocateInstruction(Instruction::i_ldloc, peLib->AllocateOperand(localList[0])));  // load argc0
                if (n >= 2)
                    currentMethod->AddInstruction(
                        peLib->AllocateInstruction(Instruction::i_ldloc, peLib->AllocateOperand(localList[1])));  // load argcv
                for (int i = 2; i < n; i++)
                    currentMethod->AddInstruction(peLib->AllocateInstruction(Instruction::i_ldnull, NULL));  // load a spare arg
                signature = peLib->AllocateMethodSignature("main", MethodSignature::Managed, mainContainer);
                currentMethod->AddInstruction(
                    peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(mainSym))));
            }
        }
        dumpInitializerCalls(destructors);
        dumpInitializerCalls(rundowns);

        if (!mainSym || (mainSym && mainSym->ReturnType()->IsVoid()))
            currentMethod->AddInstruction(
                peLib->AllocateInstruction(Instruction::i_ldc_i4, peLib->AllocateOperand((longlong)0, Operand::i32)));
        if (CManaged.GetValue())
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
                peLib->AddPInvokeReference(signature, "msvcrt.dll", false);
            }
            currentMethod->AddInstruction(
                peLib->AllocateInstruction(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(signature))));
        }
    }
}
void NetLinkMain::dumpGlobalFuncs() {}

bool NetLinkMain::EnterNamespace(const Namespace* nmspc)
{
    if (!mainNameSpace)
        if (!nmspc->InAssemblyRef())
            mainNameSpace = nmspc;
    return true;
}
bool NetLinkMain::EnterClass(const Class* cls)
{
    if (!mainClass)
    {
        DataContainer* dc = const_cast<Class*>(cls)->Parent();
        if (dc == mainNameSpace)
        {
            mainClass = cls;
            namespaceAndClass = mainNameSpace->Name() + "." + cls->Name() + "::";
        }
    }
    return true;
}
bool NetLinkMain::EnterMethod(const Method* method)
{
    // this may be is broken, it needs to do it in the order the source files were encountered...
    if (!strncmp(method->Signature()->Name().c_str(), "__DYNAMIC", 9))
    {
        if (strstr(method->Signature()->Name().c_str(), "STARTUP"))
            initializers.push_back(method->Signature());
        else
            destructors.push_back(method->Signature());
    }
    else if (!strncmp(method->Signature()->Name().c_str(), "$$STARTUP", 9))
        startups.push_back(method->Signature());
    else if (!strncmp(method->Signature()->Name().c_str(), "$$RUNDOWN", 9))
        rundowns.push_back(method->Signature());
    else if (!strncmp(method->Signature()->Name().c_str(), "string_init_", 12))
        stringinitializers.push_back(method->Signature());

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
    Param* param;
    peLib->Traverse(*this);
    if (!hasEntryPoint)
    {
        if (mainClass)
            mainContainer = const_cast<DataContainer*>(static_cast<const DataContainer*>(mainClass));
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
        startups.sort();
        startups.reverse();
        rundowns.sort();
        rundowns.reverse();

        MainInit();
        dumpInitializerCalls(stringinitializers);
        dumpInitializerCalls(startups);
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
bool NetLinkMain::CreateExecutable(CmdFiles& files)
{
    return peLib->DumpOutputFile(GetOutputFile(files).c_str(),
                                 AssemblyFile.GetValue() ? PELib::ilasm : LibraryFile.GetValue() ? PELib::pedll : PELib::peexe,
                                 GUIApp.GetValue());
}
int NetLinkMain::Run(int argc, char** argv)
{
    Utils::banner(argv[0]);
    char* modName = Utils::GetModuleName();
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
    peLib = new PELib(GetAssemblyName(files), PELib::bits32);  // ilonly set by dotnetpelib
    if (CManaged.GetValue() && !NoDefaultlibs.GetValue())
        if (peLib->LoadAssembly("lsmsilcrtl", 0, 0, 0, 0))
        {
            std::cout << "Cannot load assembly lsmsilcrtl";
            return 1;
        }
    int assemblyVersion[4] = {0};
    if (AssemblyVersion.GetValue().size())
    {
        if (sscanf(AssemblyVersion.GetValue().c_str(), "%d.%d.%d.%d", &assemblyVersion[0], &assemblyVersion[1], &assemblyVersion[2],
                   &assemblyVersion[3]) != 4)
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
//        delete peLib;
//        return 1;
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