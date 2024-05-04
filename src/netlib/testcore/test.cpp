#include "DotNetPELib.h"

using namespace DotNetPELib;

void build_dll(std::string assembly, PELib::CorFlags mode = PELib::cfnone)
{
    NetCore core((PELib::CorFlags)(PELib::ilonly | mode));
    PELib* libEntry = core.init(assembly);

    DataContainer* working = libEntry->WorkingAssembly();

    Namespace* nmspc = libEntry->AllocateNamespace("nmspc");
    working->Add(nmspc);
    Class* cls = libEntry->AllocateClass("cls", Qualifiers::Public | Qualifiers::Ansi | Qualifiers::Sealed, -1, -1);
        nmspc->Add(cls);

    MethodSignature* signatures = libEntry->AllocateMethodSignature("Start", MethodSignature::Managed, cls);
    signatures->ReturnType(libEntry->AllocateType(Type::Void, 0));
    Param* parames1 = libEntry->AllocateParam("strn", libEntry->AllocateType(Type::string, 0));
    signatures->AddParam(parames1);

    // add a reference to the assembly
    core.LoadAssembly("System.Runtime");
    core.LoadAssembly("System.Console");

    // create the function refernce to WriteLine
    // there is an argument matcher in the library, set up a vector of types describing
    // the arguments and it will try to find a matching overload.
    Type tp(Type::string, 0);
    std::vector<Type*> typeList;
    typeList.push_back(&tp);
    Method* result = nullptr;
    MethodSignature* signaturep;
    if (libEntry->Find("System.Console.WriteLine", &result, typeList) == PELib::s_method)
    {
        signaturep = result->Signature();
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }

    Method* start = libEntry->AllocateMethod(signatures, Qualifiers::Public |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed);
    start->AddInstruction(libEntry->AllocateInstruction(Instruction::i_ldarg,
        libEntry->AllocateOperand(parames1)));
    start->AddInstruction(libEntry->AllocateInstruction(Instruction::i_call,
        libEntry->AllocateOperand(libEntry->AllocateMethodName(signaturep))));

    start->AddInstruction(libEntry->AllocateInstruction(Instruction::i_ldstr,
        libEntry->AllocateOperand("this is a string in Start()", true)));
    start->AddInstruction(libEntry->AllocateInstruction(Instruction::i_call,
        libEntry->AllocateOperand(libEntry->AllocateMethodName(signaturep))));

    start->AddInstruction(libEntry->AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        start->Optimize(*libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    cls->Add(start);

    core.DumpOutputFile(assembly + ".il", PELib::ilasm);
    core.DumpOutputFile(assembly + ".dll", PELib::pedll);

}
void build_exe(std::string assembly, std::string lib, PELib::CorFlags mode = PELib::cfnone)
{
    NetCore core((PELib::CorFlags)(PELib::ilonly | mode));
    PELib *libEntry = core.init(assembly);

    DataContainer* working = libEntry->WorkingAssembly();
    Class* cls = libEntry->AllocateClass("Program", Qualifiers::Private | Qualifiers::Ansi | Qualifiers::Auto | Qualifiers::BeforeFieldInit, -1, -1);
    working->Add(cls);

    // add a reference to the assembly
    core.LoadAssembly("System.Runtime");
    core.LoadAssembly(lib);

    MethodSignature* signaturem = libEntry->AllocateMethodSignature("<Main>$", MethodSignature::Managed, working);
    signaturem->ReturnType(libEntry->AllocateType(Type::Void, 0));
    Method* main = libEntry->AllocateMethod(signaturem, Qualifiers::Private |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed, true);
    cls->Add(main);

    Type tp(Type::string, 0);
    std::vector<Type*> typeList;
    typeList.push_back(&tp);
    Method* result = nullptr;
    MethodSignature* signaturep;
    if (libEntry->Find("nmspc.cls.Start", &result, typeList) == PELib::s_method)
    {
        signaturep = result->Signature();
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }
    // note the reference to the generic form of ldc.i4 ... it will be optimized
    // into an ldc.i4.s by the library
    main->AddInstruction(libEntry->AllocateInstruction(Instruction::i_ldstr,
        libEntry->AllocateOperand("this is a string in <Main>$()", true)));
    main->AddInstruction(libEntry->AllocateInstruction(Instruction::i_call,
        libEntry->AllocateOperand(libEntry->AllocateMethodName(signaturep))));
    main->AddInstruction(libEntry->AllocateInstruction(Instruction::i_ret, nullptr));
    try
    {
        main->Optimize(*libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    core.DumpOutputFile(assembly + ".il", PELib::ilasm);
    core.DumpOutputFile(assembly + ".exe", PELib::peexe);

}
int main()
{
    for (int i = 5; i < 10; i++)
    {
        NetCore core(PELib::cfnone, false, i);
        std::cout << i << ": " << core.SupportsRuntime(i) << std::endl;
    }
    build_dll("libcore32", PELib::bits32);
    build_exe("testcore32", "libcore32", PELib::bits32);
    build_dll("libcore64");
    build_exe("testcore64", "libcore64");
}