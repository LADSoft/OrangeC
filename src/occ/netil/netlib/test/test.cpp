#include "DotNetPELib.h"

using namespace DotNetPELib;

// this file generates various small programs, with a function '$Main' as the entry point
//
// this demo skips a couple of things:
//      varargs at the C# level
//      enums
//      the ".cctor".   The .cctor is used to initialize a DLLs static variables
// 
// if you need to create a .cctor make it a function taking no arguments and returning void,
//      and give it the following qualifiers:
//
//    Qualifiers::Private | Qualifiers::HideBySig | 
//    Qualifiers::Static | Qualifiers::CIL | Qualifiers::Managed |
//    Qualifiers::SpecialName | Qualifiers::RTSpecialName;
//
// The first program just calls putchar('A') via pinvoke.
// it only uses the unnamed namespace
void test1()
{
    // 32 bit assembly that can't be referenced from managed assemblies
    PELib libEntry("test1", PELib::bits32);

    DataContainer *working = libEntry.WorkingAssembly();

    MethodSignature *signaturep = libEntry.AllocateMethodSignature("putchar",0, nullptr);
    signaturep->ReturnType(libEntry.AllocateType(Type::Void, 0));
    signaturep->AddParam(libEntry.AllocateParam("ch", libEntry.AllocateType(Type::i32, 0)));
    libEntry.AddPInvokeReference(signaturep,"msvcrt.dll", true);

    MethodSignature *signaturem = libEntry.AllocateMethodSignature("$Main",MethodSignature::Managed, working);
    signaturem->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Method *start = libEntry.AllocateMethod(signaturem, Qualifiers::Private |
                                                        Qualifiers::Static |
                                                        Qualifiers::HideBySig |
                                                        Qualifiers::CIL |
                                                        Qualifiers::Managed, true);
    working->Add(start);

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4, 
                                    libEntry.AllocateOperand('A', Operand::i32)));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call, 
                                    libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        start->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }

    libEntry.DumpOutputFile("test1.il", PELib::ilasm, false);
    libEntry.DumpOutputFile("test1.exe", PELib::peexe, false);
}
// the second program calls printf("%s", "hi")
// it only uses the unnamed namespace
void test2()
{
    PELib libEntry("test2", PELib::ilonly | PELib::bits32);

    DataContainer *working = libEntry.WorkingAssembly();

    Class *i8Cls = libEntry.AllocateClass("int8[]", Qualifiers::Private | Qualifiers::Explicit | Qualifiers::Ansi | Qualifiers::Sealed | Qualifiers::Value,1, 1);
    Field *pS = libEntry.AllocateField("pS", libEntry.AllocateType(i8Cls), Qualifiers::Private | Qualifiers::Static);
    Field *Str = libEntry.AllocateField("Str", libEntry.AllocateType(i8Cls), Qualifiers::Private | Qualifiers::Static);
    working->Add(i8Cls);
    working->Add(pS);
    working->Add(Str);

    static Byte pSInit[] = { 0x25, 0x73, 0x0a, 0 }; // %s
    static Byte StrInit[] = { 0x48, 0x49, 0 }; // HI
    // these calls put the strings in the SDATA.
    // reading from SDATA is ok, but writing can only be done by
    // standalone programs that don't interact with other .net assemblies
    // we are only reading...
    pS->AddInitializer(pSInit, sizeof(pSInit));
    Str->AddInitializer(StrInit, sizeof(StrInit));

    // we have to first make a pinvoke reference WITHOUT extra args
    MethodSignature *signaturex = libEntry.AllocateMethodSignature("printf",MethodSignature::Vararg, nullptr);
    signaturex->ReturnType(libEntry.AllocateType(Type::i32, 0));
    signaturex->AddParam(libEntry.AllocateParam("format", libEntry.AllocateType(Type::Void, 1)));
    libEntry.AddPInvokeReference(signaturex,"msvcrt.dll", true);

    // then we make a call site signature that enumerates all the args
    // including the ones we are adding as variable length
    // this is the one we use in the call
    MethodSignature *signaturep = libEntry.AllocateMethodSignature("printf",MethodSignature::Vararg, nullptr);
    signaturep->ReturnType(libEntry.AllocateType(Type::i32, 0));
    signaturep->AddParam(libEntry.AllocateParam("format", libEntry.AllocateType(Type::Void, 1)));
    signaturep->AddVarargParam(libEntry.AllocateParam("A_1", libEntry.AllocateType(Type::Void, 1)));
    // note the reference to the pinvoke signature
    signaturep->SignatureParent(signaturex);

    MethodSignature *signaturem = libEntry.AllocateMethodSignature("$Main",MethodSignature::Managed, working);
    signaturem->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Method *main = libEntry.AllocateMethod(signaturem, Qualifiers::Private |
                                                        Qualifiers::Static |
                                                        Qualifiers::HideBySig |
                                                        Qualifiers::CIL |
                                                        Qualifiers::Managed, true);
    working->Add(main);

    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldsflda, 
                                    libEntry.AllocateOperand(libEntry.AllocateFieldName(pS))));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldsflda, 
                                    libEntry.AllocateOperand(libEntry.AllocateFieldName(Str))));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call, 
                                    libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep))));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_pop, nullptr));

    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        main->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }

    libEntry.DumpOutputFile("test2.il", PELib::ilasm, false);
    libEntry.DumpOutputFile("test2.exe", PELib::peexe, false);
}
// the third program calls a function 'Start("hi")' in namespace 'nmspc' and class 'cls'
// start calls printf("%s", arg1) via pinvoke
//
// making a namespace and class like this is what you have to do to make
// the assembly 'visible' to C#
//
void test3()
{
    // here we have set the ilonly flag, which is another prerequisite for
    // being able to make the assembly visible.
    // Note I have never tried accessing an EXE file that is tagged this way,
    // only DLLs.
    PELib libEntry("test3", PELib::ilonly | PELib::bits32);

    DataContainer *working = libEntry.WorkingAssembly();

    Namespace *nmspc = libEntry.AllocateNamespace("nmspc");
    working->Add(nmspc);
    Class *cls = libEntry.AllocateClass("cls", Qualifiers::Ansi | Qualifiers::Sealed, -1, -1);
    nmspc->Add(cls);

    Class *i8Cls = libEntry.AllocateClass("int8[]", Qualifiers::Public | Qualifiers::Explicit | Qualifiers::Ansi | Qualifiers::Sealed | Qualifiers::Value,1, 1);
    Field *pS = libEntry.AllocateField("pS", libEntry.AllocateType(i8Cls), Qualifiers::Public | Qualifiers::Static);
    Field *Str = libEntry.AllocateField("Str", libEntry.AllocateType(i8Cls), Qualifiers::Public | Qualifiers::Static);
    cls->Add(i8Cls);
    cls->Add(pS);
    cls->Add(Str);

    static Byte pSInit[] = { 0x25, 0x73, 0x0a, 0 }; // %s
    static Byte StrInit[] = { 0x48, 0x49, 0 }; // HI
    pS->AddInitializer(pSInit, sizeof(pSInit));
    Str->AddInitializer(StrInit, sizeof(StrInit));


    MethodSignature *signaturex = libEntry.AllocateMethodSignature("printf",MethodSignature::Vararg, nullptr);
    signaturex->ReturnType(libEntry.AllocateType(Type::i32, 0));
    signaturex->AddParam(libEntry.AllocateParam("format", libEntry.AllocateType(Type::Void, 1)));
    libEntry.AddPInvokeReference(signaturex,"msvcrt.dll", true);

    MethodSignature *signaturep = libEntry.AllocateMethodSignature("printf",MethodSignature::Vararg, nullptr);
    signaturep->ReturnType(libEntry.AllocateType(Type::i32, 0));
    signaturep->AddParam(libEntry.AllocateParam("format", libEntry.AllocateType(Type::Void, 1)));
    signaturep->AddVarargParam(libEntry.AllocateParam("A_1", libEntry.AllocateType(Type::Void, 1)));
    signaturep->SignatureParent(signaturex);

    MethodSignature *signatures = libEntry.AllocateMethodSignature("Start",MethodSignature::Managed, cls);
    signatures->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Type *tp1;
    Param *parames1 = libEntry.AllocateParam("strng", tp1 = libEntry.AllocateType(i8Cls));
    tp1->PointerLevel(1);
    signatures->AddParam(parames1);

    Method *start = libEntry.AllocateMethod(signatures, Qualifiers::Public |
                                                        Qualifiers::Static |
                                                        Qualifiers::HideBySig |
                                                        Qualifiers::CIL |
                                                        Qualifiers::Managed);
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldsflda, 
                                    libEntry.AllocateOperand(libEntry.AllocateFieldName(pS))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldarg, 
                                    libEntry.AllocateOperand(parames1)));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call, 
                                    libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_pop, nullptr));

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        start->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    cls->Add(start);

    MethodSignature *signaturem = libEntry.AllocateMethodSignature("$Main",MethodSignature::Managed, working);
    signaturem->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Method *main = libEntry.AllocateMethod(signaturem, Qualifiers::Private |
                                                        Qualifiers::Static |
                                                        Qualifiers::HideBySig |
                                                        Qualifiers::CIL |
                                                        Qualifiers::Managed, true);
    working->Add(main);

    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldsflda, 
                                    libEntry.AllocateOperand(libEntry.AllocateFieldName(Str))));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call, 
                                    libEntry.AllocateOperand(libEntry.AllocateMethodName(signatures))));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        main->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    libEntry.DumpOutputFile("test3.il", PELib::ilasm, false);
    libEntry.DumpOutputFile("test3.exe", PELib::peexe, false);
}
// the fourth program calls a function 'Start("hi")' in namespace 'nmspc' and class 'cls'
// start calls printf("%s", arg1) via pinvoke
//
// it is all further nested in a namespace nspc0
void test4()
{
    PELib libEntry("test4", PELib::ilonly | PELib::bits32);

    DataContainer *working = libEntry.WorkingAssembly();

    Namespace *nspc0 = libEntry.AllocateNamespace("nspc0");
    working->Add(nspc0);
    Namespace *nmspc = libEntry.AllocateNamespace("nmspc");
    nspc0->Add(nmspc);
    Class *cls = libEntry.AllocateClass("cls", Qualifiers::Ansi | Qualifiers::Sealed, -1, -1);
    nmspc->Add(cls);

    Class *i8Cls = libEntry.AllocateClass("int8[]", Qualifiers::Public | Qualifiers::Explicit | Qualifiers::Ansi | Qualifiers::Sealed | Qualifiers::Value,1, 1);
    Field *pS = libEntry.AllocateField("pS", libEntry.AllocateType(i8Cls), Qualifiers::Public | Qualifiers::Static);
    Field *Str = libEntry.AllocateField("Str", libEntry.AllocateType(i8Cls), Qualifiers::Public | Qualifiers::Static);
    cls->Add(i8Cls);
    cls->Add(pS);
    cls->Add(Str);

    static Byte pSInit[] = { 0x25, 0x73, 0x0a, 0 }; // %s
    static Byte StrInit[] = { 0x48, 0x49, 0 }; // HI
    pS->AddInitializer(pSInit, sizeof(pSInit));
    Str->AddInitializer(StrInit, sizeof(StrInit));


    MethodSignature *signaturex = libEntry.AllocateMethodSignature("printf",MethodSignature::Vararg, nullptr);
    signaturex->ReturnType(libEntry.AllocateType(Type::i32, 0));
    signaturex->AddParam(libEntry.AllocateParam("format", libEntry.AllocateType(Type::Void, 1)));
    libEntry.AddPInvokeReference(signaturex,"msvcrt.dll", true);

    MethodSignature *signaturep = libEntry.AllocateMethodSignature("printf",MethodSignature::Vararg, nullptr);
    signaturep->ReturnType(libEntry.AllocateType(Type::i32, 0));
    signaturep->AddParam(libEntry.AllocateParam("format", libEntry.AllocateType(Type::Void, 1)));
    signaturep->AddVarargParam(libEntry.AllocateParam("A_1", libEntry.AllocateType(Type::Void, 1)));
    signaturep->SignatureParent(signaturex);

    MethodSignature *signatures = libEntry.AllocateMethodSignature("Start",MethodSignature::Managed, cls);
    signatures->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Type *tp1;
    Param *parames1 = libEntry.AllocateParam("strng", tp1 = libEntry.AllocateType(i8Cls));
    tp1->PointerLevel(1);
    signatures->AddParam(parames1);

    Method *start = libEntry.AllocateMethod(signatures, Qualifiers::Public |
                                                        Qualifiers::Static |
                                                        Qualifiers::HideBySig |
                                                        Qualifiers::CIL |
                                                        Qualifiers::Managed);
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldsflda, 
                                    libEntry.AllocateOperand(libEntry.AllocateFieldName(pS))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldarg, 
                                    libEntry.AllocateOperand(parames1)));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call, 
                                    libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_pop, nullptr));

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    start->Optimize(libEntry);
    cls->Add(start);

    MethodSignature *signaturem = libEntry.AllocateMethodSignature("$Main",MethodSignature::Managed, working);
    signaturem->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Method *main = libEntry.AllocateMethod(signaturem, Qualifiers::Private |
                                                        Qualifiers::Static |
                                                        Qualifiers::HideBySig |
                                                        Qualifiers::CIL |
                                                        Qualifiers::Managed, true);
    working->Add(main);

    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldsflda, 
                                    libEntry.AllocateOperand(libEntry.AllocateFieldName(Str))));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call, 
                                    libEntry.AllocateOperand(libEntry.AllocateMethodName(signatures))));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        main->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    libEntry.DumpOutputFile("test4.il", PELib::ilasm, false);
    libEntry.DumpOutputFile("test4.exe", PELib::peexe, false);
}
// the fifth program calls a function 'Start(57)' in namespace 'nmspc' and class 'cls'
// start calls printf("%d", arg1) via pinvoke
//
void test5()
{
    PELib libEntry("test5", PELib::ilonly | PELib::bits32);

    DataContainer *working = libEntry.WorkingAssembly();

    Namespace *nmspc = libEntry.AllocateNamespace("nmspc");
    working->Add(nmspc);
    Class *cls = libEntry.AllocateClass("cls", Qualifiers::Ansi | Qualifiers::Sealed, -1, -1);
    nmspc->Add(cls);

    Class *i8Cls = libEntry.AllocateClass("int8[]", Qualifiers::Public | Qualifiers::Explicit | Qualifiers::Ansi | Qualifiers::Sealed | Qualifiers::Value, 1, 1);
    Field *pS = libEntry.AllocateField("pS", libEntry.AllocateType(i8Cls), Qualifiers::Public | Qualifiers::Static);
    cls->Add(i8Cls);
    cls->Add(pS);

    static Byte pSInit[] = { 0x25, 0x64, 0x0a, 0 }; // %d
    pS->AddInitializer(pSInit, sizeof(pSInit));


    MethodSignature *signaturex = libEntry.AllocateMethodSignature("printf", MethodSignature::Vararg, nullptr);
    signaturex->ReturnType(libEntry.AllocateType(Type::i32, 0));
    signaturex->AddParam(libEntry.AllocateParam("format", libEntry.AllocateType(Type::Void, 1)));
    libEntry.AddPInvokeReference(signaturex, "msvcrt.dll", true);

    MethodSignature *signaturep = libEntry.AllocateMethodSignature("printf", MethodSignature::Vararg, nullptr);
    signaturep->ReturnType(libEntry.AllocateType(Type::i32, 0));
    signaturep->AddParam(libEntry.AllocateParam("format", libEntry.AllocateType(Type::Void, 1)));
    signaturep->AddVarargParam(libEntry.AllocateParam("A_1", libEntry.AllocateType(Type::Void, 1)));
    signaturep->SignatureParent(signaturex);

    MethodSignature *signatures = libEntry.AllocateMethodSignature("Start", MethodSignature::Managed, cls);
    signatures->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Param *parames1 = libEntry.AllocateParam("val", libEntry.AllocateType(Type::i32, 0));
    signatures->AddParam(parames1);

    Method *start = libEntry.AllocateMethod(signatures, Qualifiers::Public |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed);
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldsflda,
        libEntry.AllocateOperand(libEntry.AllocateFieldName(pS))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldarg,
        libEntry.AllocateOperand(parames1)));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_pop, nullptr));

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        start->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    cls->Add(start);

    MethodSignature *signaturem = libEntry.AllocateMethodSignature("$Main", MethodSignature::Managed, working);
    signaturem->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Method *main = libEntry.AllocateMethod(signaturem, Qualifiers::Private |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed, true);
    working->Add(main);

    // note the reference to the generic form of ldc.i4 ... it will be optimized
    // into an ldc.i4.s by the library
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4,
        libEntry.AllocateOperand((longlong)57, Operand::i32)));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signatures))));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        main->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    libEntry.DumpOutputFile("test5.il", PELib::ilasm, false);
    libEntry.DumpOutputFile("test5.exe", PELib::peexe, false);
}
// the sixth program calls a function 'Start(string)' in namespace 'nmspc' and class 'cls'
// Start prints both the passed string and a locally defined string to the console using
// System.Console.WriteLine()
//
void test6()
{
    PELib libEntry("test6", PELib::ilonly | PELib::bits32);

    DataContainer *working = libEntry.WorkingAssembly();

    Namespace *nmspc = libEntry.AllocateNamespace("nmspc");
    working->Add(nmspc);
    Class *cls = libEntry.AllocateClass("cls", Qualifiers::Ansi | Qualifiers::Sealed, -1, -1);
    nmspc->Add(cls);

    MethodSignature *signatures = libEntry.AllocateMethodSignature("Start", MethodSignature::Managed, cls);
    signatures->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Param *parames1 = libEntry.AllocateParam("strn", libEntry.AllocateType(Type::string, 0));
    signatures->AddParam(parames1);

    // add a reference to the assembly
    libEntry.LoadAssembly("mscorlib");

    // create the function refernce to WriteLine
    // there is an argument matcher in the library, set up a vector of types describing
    // the arguments and it will try to find a matching overload.
    Type tp(Type::string, 0);
    std::vector<Type *> typeList;
    typeList.push_back(&tp);
    Method *result = nullptr;
    MethodSignature *signaturep;
    if (libEntry.Find("System.Console.WriteLine", &result, typeList) == PELib::s_method)
    {
        signaturep = result->Signature();
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }

    Method *start = libEntry.AllocateMethod(signatures, Qualifiers::Public |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed);
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldarg,
        libEntry.AllocateOperand(parames1)));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep))));

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldstr,
        libEntry.AllocateOperand("this is a string in Start()", true)));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep))));

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        start->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    cls->Add(start);

    MethodSignature *signaturem = libEntry.AllocateMethodSignature("$Main", MethodSignature::Managed, working);
    signaturem->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Method *main = libEntry.AllocateMethod(signaturem, Qualifiers::Private |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed, true);
    working->Add(main);

    // note the reference to the generic form of ldc.i4 ... it will be optimized
    // into an ldc.i4.s by the library
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldstr,
        libEntry.AllocateOperand("this is a string in $Main()", true)));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signatures))));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        main->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    libEntry.DumpOutputFile("test6.il", PELib::ilasm, false);
    libEntry.DumpOutputFile("test6.exe", PELib::peexe, false);
}
// the seventh program calls a function 'Start(string)' in namespace 'nmspc' and class 'cls'
// start has a local variable 'count' and loops to print the string 7 times 
// using System.Console.WriteLine()
//
void test7()
{
    PELib libEntry("test7", PELib::ilonly | PELib::bits32);

    DataContainer *working = libEntry.WorkingAssembly();

    Namespace *nmspc = libEntry.AllocateNamespace("nmspc");
    working->Add(nmspc);
    Class *cls = libEntry.AllocateClass("cls", Qualifiers::Ansi | Qualifiers::Sealed, -1, -1);
    nmspc->Add(cls);

    MethodSignature *signatures = libEntry.AllocateMethodSignature("Start", MethodSignature::Managed, cls);
    signatures->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Param *parames1 = libEntry.AllocateParam("strn", libEntry.AllocateType(Type::string, 0));
    signatures->AddParam(parames1);

    // add a reference to the assembly
    libEntry.LoadAssembly("mscorlib");

    // add a using statement
    libEntry.AddUsing("System");

    // create the function refernce to WriteLine
    Type tp(Type::string, 0);
    std::vector<Type *> typeList;
    typeList.push_back(&tp);
    Method *result = nullptr;
    MethodSignature *signaturep;
    if (libEntry.Find("Console.WriteLine", &result, typeList) == PELib::s_method)
    {
        signaturep = result->Signature();
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }

    Method *start = libEntry.AllocateMethod(signatures, Qualifiers::Public |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed);

    auto counter = libEntry.AllocateLocal("counter",libEntry.AllocateType(Type::i32, 0));
    start->AddLocal(counter);
    auto counterOperand = libEntry.AllocateOperand(counter);

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_comment, "initialize"));
    const int NUMBER_OF_TIMES = 7;
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4, 
        libEntry.AllocateOperand(NUMBER_OF_TIMES,Operand::i32)));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_stloc, counterOperand));

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_comment, "start of loop"));
    auto loopLabel = libEntry.AllocateOperand("loop");
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_label, loopLabel));

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldarg,
        libEntry.AllocateOperand(parames1)));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep))));

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldloc, counterOperand));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4,
        libEntry.AllocateOperand(1, Operand::i32)));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_sub));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_dup));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_stloc, counterOperand));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_brtrue, loopLabel));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_comment, "exit"));

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        start->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    cls->Add(start);

    MethodSignature *signaturem = libEntry.AllocateMethodSignature("$Main", MethodSignature::Managed, working);
    signaturem->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Method *main = libEntry.AllocateMethod(signaturem, Qualifiers::Private |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed, true);
    working->Add(main);

    // note the reference to the generic form of ldc.i4 ... it will be optimized
    // into an ldc.i4.s by the library
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldstr,
        libEntry.AllocateOperand("this is a string", true)));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signatures))));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        main->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    libEntry.DumpOutputFile("test7.il", PELib::ilasm, false);
    libEntry.DumpOutputFile("test7.exe", PELib::peexe, false);
}
// the eighth program reads a field from a .net assembly...
//
void test8()
{
    std::cout << " for test8 you need to compile testcsc.cs into a library" << std::endl;
    std::cout << " cmd line is 'csc /target:library testcsc.cs'" << std::endl;
    PELib libEntry("test8", PELib::ilonly | PELib::bits32);

    DataContainer *working = libEntry.WorkingAssembly();

    Namespace *nmspc = libEntry.AllocateNamespace("nmspc");
    working->Add(nmspc);
    Class *cls = libEntry.AllocateClass("cls", Qualifiers::Ansi | Qualifiers::Sealed, -1, -1);
    nmspc->Add(cls);

    MethodSignature *signatures = libEntry.AllocateMethodSignature("Start", MethodSignature::Managed, cls);
    signatures->ReturnType(libEntry.AllocateType(Type::Void, 0));

    // add a reference to the assembly
    libEntry.LoadAssembly("mscorlib");
    libEntry.LoadAssembly("testcsc");

    // add a using statement or two to making finding things a little easier
    libEntry.AddUsing("System");
    libEntry.AddUsing("a.b.c");

    void *extendsResult;
    // this is in namespace a.b.c
    if (libEntry.Find("r.s", &extendsResult) == PELib::s_class)
    {
        cls->Extends(static_cast<Class *>(extendsResult));
    }

    Type tp(Type::i32, 0);
    std::vector<Type *> typeList;
    typeList.push_back(&tp);
    Method *result;
    MethodSignature *signaturep;
    // this is in namespace system
    if (libEntry.Find("Console.WriteLine", &result, typeList) == PELib::s_method)
    {
        signaturep = result->Signature();
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }

    void *rresult = nullptr;
    Field *field;
    if (libEntry.Find("r.s.zz", &rresult) == PELib::s_field)
    {
        field = static_cast<Field *>(rresult);
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }
    Method *start = libEntry.AllocateMethod(signatures, Qualifiers::Public |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed);

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldsfld,
        libEntry.AllocateOperand(libEntry.AllocateFieldName(field))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        start->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    cls->Add(start);

    MethodSignature *signaturem = libEntry.AllocateMethodSignature("$Main", MethodSignature::Managed, working);
    signaturem->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Method *main = libEntry.AllocateMethod(signaturem, Qualifiers::Private |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed, true);
    working->Add(main);

    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signatures))));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        main->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    libEntry.DumpOutputFile("test8.il", PELib::ilasm, false);
    libEntry.DumpOutputFile("test8.exe", PELib::peexe, false);
}
// the ninth program writes a property and reads it back.   in testcsc.cs he write is designed to add
// 22 and the read adds another 11...
//
void test9()
{
    std::cout << " for test9 you need to compile testcsc.cs into a library" << std::endl;
    std::cout << " cmd line is 'csc /target:library testcsc.cs'" << std::endl;
    PELib libEntry("test9", PELib::ilonly | PELib::bits32);

    DataContainer *working = libEntry.WorkingAssembly();

    Namespace *nmspc = libEntry.AllocateNamespace("nmspc");
    working->Add(nmspc);
    Class *cls = libEntry.AllocateClass("cls", Qualifiers::Ansi | Qualifiers::Sealed, -1, -1);
    nmspc->Add(cls);

    MethodSignature *signatures = libEntry.AllocateMethodSignature("Start", MethodSignature::Managed, cls);
    signatures->ReturnType(libEntry.AllocateType(Type::Void, 0));

    // add a reference to the assembly
    libEntry.LoadAssembly("mscorlib");
    libEntry.LoadAssembly("testcsc");

    // add a using statement
    libEntry.AddUsing("System");
    libEntry.AddUsing("a.b.c");

    void *presult;
    Property *property;
    if (libEntry.Find("r.s.myprop", &presult) == PELib::s_property)
    {
        property = static_cast<Property *>(presult);
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);

    }
    Type tp(Type::i32, 0);
    std::vector<Type *> typeList;
    typeList.push_back(&tp);
    Method *result;
    MethodSignature *signaturep;
    if (libEntry.Find("Console.WriteLine", &result, typeList) == PELib::s_method)
    {
        signaturep = result->Signature();
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }

    Method *start = libEntry.AllocateMethod(signatures, Qualifiers::Public |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed);

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4,
        libEntry.AllocateOperand(273, Operand::i32)));
    property->CallSet(libEntry, start);
    property->CallGet(libEntry, start);
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        start->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    cls->Add(start);

    MethodSignature *signaturem = libEntry.AllocateMethodSignature("$Main", MethodSignature::Managed, working);
    signaturem->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Method *main = libEntry.AllocateMethod(signaturem, Qualifiers::Private |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed, true);
    working->Add(main);

    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signatures))));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        main->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    libEntry.DumpOutputFile("test9.il", PELib::ilasm, false);
    libEntry.DumpOutputFile("test9.exe", PELib::peexe, false);
}
// the tenth program creates an int32 property, whos value it stores in the testcsc.cs library static field
// we set the property then get from it.
// the setter adds 32 to the value set, and the setter adds another 64
//
void test10()
{
    std::cout << " for test10 you need to compile testcsc.cs into a library" << std::endl;
    std::cout << " cmd line is 'csc /target:library testcsc.cs'" << std::endl;
    PELib libEntry("test10", PELib::ilonly | PELib::bits32);

    DataContainer *working = libEntry.WorkingAssembly();

    Namespace *nmspc = libEntry.AllocateNamespace("nmspc");
    working->Add(nmspc);
    Class *cls = libEntry.AllocateClass("cls", Qualifiers::Ansi | Qualifiers::Sealed, -1, -1);
    nmspc->Add(cls);

    MethodSignature *signatures = libEntry.AllocateMethodSignature("Start", MethodSignature::Managed, cls);
    signatures->ReturnType(libEntry.AllocateType(Type::Void, 0));

    // add a reference to the assembly
    libEntry.LoadAssembly("mscorlib");
    libEntry.LoadAssembly("testcsc");

    // add a using statement
    libEntry.AddUsing("System");
    libEntry.AddUsing("a.b.c");

    // load a reference to the field that is going to be backing store for the property
    void *rresult = nullptr;
    Field *field;
    if (libEntry.Find("r.s.zz", &rresult) == PELib::s_field)
    {
        field = static_cast<Field *>(rresult);
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }

    // create the property
    void *presult;
    std::vector<Type *> dummy;
    Property *property = libEntry.AllocateProperty(libEntry, "localProperty",
        libEntry.AllocateType(Type::i32,0), dummy);

    // set it to be a static property
    property->Instance(false);

    cls->Add(property);


    // make the getter - load the value and add 64
    CodeContainer *getter = property->Getter();
    getter->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldsfld,
        libEntry.AllocateOperand(libEntry.AllocateFieldName(field))));
    getter->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4,
        libEntry.AllocateOperand(64, Operand::i32)));
    getter->AddInstruction(libEntry.AllocateInstruction(Instruction::i_add, nullptr));
    getter->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    // make the setter - add 32 then store it in the backing area
    CodeContainer *setter = property->Setter();
    setter->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldarg_0, nullptr));
    setter->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4,
        libEntry.AllocateOperand(32, Operand::i32)));
    setter->AddInstruction(libEntry.AllocateInstruction(Instruction::i_add, nullptr));
    setter->AddInstruction(libEntry.AllocateInstruction(Instruction::i_stsfld,
        libEntry.AllocateOperand(libEntry.AllocateFieldName(field))));
    setter->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    Type tp(Type::i32, 0);
    std::vector<Type *> typeList;
    typeList.push_back(&tp);
    Method *result;
    MethodSignature *signaturep;
    if (libEntry.Find("Console.WriteLine", &result, typeList) == PELib::s_method)
    {
        signaturep = result->Signature();
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }

    Method *start = libEntry.AllocateMethod(signatures, Qualifiers::Public |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed);

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4,
        libEntry.AllocateOperand(273, Operand::i32)));
    property->CallSet(libEntry, start);
    property->CallGet(libEntry, start);
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        start->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    cls->Add(start);

    MethodSignature *signaturem = libEntry.AllocateMethodSignature("$Main", MethodSignature::Managed, working);
    signaturem->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Method *main = libEntry.AllocateMethod(signaturem, Qualifiers::Private |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed, true);
    working->Add(main);

    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signatures))));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        main->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    libEntry.DumpOutputFile("test10.il", PELib::ilasm, false);
    libEntry.DumpOutputFile("test10.exe", PELib::peexe, false);
}
// the 11th program calls a function 'Start(string)' in namespace 'nmspc' and class 'cls'
// with a format string for system.console.writeline.   Starts writes the format and some
// numbers using an object array.
//
void test11()
{
    PELib libEntry("test11", PELib::ilonly | PELib::bits32);

    DataContainer *working = libEntry.WorkingAssembly();

    Namespace *nmspc = libEntry.AllocateNamespace("nmspc");
    working->Add(nmspc);
    Class *cls = libEntry.AllocateClass("cls", Qualifiers::Ansi | Qualifiers::Sealed, -1, -1);
    nmspc->Add(cls);

    MethodSignature *signatures = libEntry.AllocateMethodSignature("Start", MethodSignature::Managed, cls);
    signatures->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Param *parames1 = libEntry.AllocateParam("strn", libEntry.AllocateType(Type::string, 0));
    signatures->AddParam(parames1);

    // add a reference to the assembly
    libEntry.LoadAssembly("mscorlib");

    // create the function refernece to WriteLine
    Type tp(Type::string, 0);
    Type tp1(Type::object, 0);
    tp1.ArrayLevel(1);
    std::vector<Type *> typeList;
    typeList.push_back(&tp);
    typeList.push_back(&tp1);
    Method *result = nullptr;
    MethodSignature *signaturep;
    if (libEntry.Find("System.Console.WriteLine", &result, typeList) == PELib::s_method)
    {
        signaturep = result->Signature();
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }

    // look up some system objects for arrays and boxing
    void *temp;
    Class *objectClass;
    if (libEntry.Find("System.Object", &temp) == PELib::s_class)
    {
        objectClass = static_cast<Class *>(temp);
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }
    Class *int32Class;
    if (libEntry.Find("System.Int32", &temp) == PELib::s_class)
    {
        int32Class = static_cast<Class *>(temp);
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }
    Class *stringClass;
    if (libEntry.Find("System.String", &temp) == PELib::s_class)
    {
        stringClass = static_cast<Class *>(temp);
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }
    Method *start = libEntry.AllocateMethod(signatures, Qualifiers::Public |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed);

    // create a local variable to hold our array
    auto objectArray = libEntry.AllocateLocal("arr", libEntry.AllocateType(objectClass));
    start->AddLocal(objectArray);

    // allocate an object array with two items in it
    // the first item will be a number and the second will be a string
    // note when creating the array we give it a type as an argument, this is done by
    // allocating a 'value' operand
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4,
        libEntry.AllocateOperand(2, Operand::i32)));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_newarr,
        libEntry.AllocateOperand(libEntry.AllocateValue("", libEntry.AllocateType(objectClass)))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_stloc_0, nullptr));

    // store a boxed integer in the first elem
    // again every time we box we need a type to box to...
    // the reason for the boxing is the array is an array of objects not an adhoc array of values
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldloc_0, nullptr));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4, 
        libEntry.AllocateOperand(0, Operand::i32))); // the index
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4,
        libEntry.AllocateOperand(10000, Operand::i32)));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_box,
        libEntry.AllocateOperand(libEntry.AllocateValue("", libEntry.AllocateType(int32Class)))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_stelem_ref, nullptr));

    // store a boxed string in the second elem
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldloc_0, nullptr));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4,
        libEntry.AllocateOperand(1, Operand::i32))); // the index
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldstr,
        libEntry.AllocateOperand("this is a boxed string from dotnetpelib", true)));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_box,
        libEntry.AllocateOperand(libEntry.AllocateValue("", libEntry.AllocateType(stringClass)))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_stelem_ref, nullptr));

    // now set up the call and do it
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldarg_0, nullptr));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldloc_0, nullptr));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep))));

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        start->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    cls->Add(start);

    MethodSignature *signaturem = libEntry.AllocateMethodSignature("$Main", MethodSignature::Managed, working);
    signaturem->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Method *main = libEntry.AllocateMethod(signaturem, Qualifiers::Private |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed, true);
    working->Add(main);

    // note the reference to the generic form of ldc.i4 ... it will be optimized
    // into an ldc.i4.s by the library
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldstr,
        libEntry.AllocateOperand("String={1}, Value={0}", true)));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signatures))));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        main->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    libEntry.DumpOutputFile("test11.il", PELib::ilasm, false);
    libEntry.DumpOutputFile("test11.exe", PELib::peexe, false);
}
void test12()
{
    PELib libEntry("test12", PELib::ilonly | PELib::bits32);

    DataContainer *working = libEntry.WorkingAssembly();

    Namespace *nmspc = libEntry.AllocateNamespace("nmspc");
    working->Add(nmspc);
    Class *cls = libEntry.AllocateClass("cls", Qualifiers::Ansi | Qualifiers::Sealed, -1, -1);
    nmspc->Add(cls);


    // add a reference to the assembly
    libEntry.LoadAssembly("mscorlib");

    // look up some system objects for arrays and boxing
    void *temp;
    Class *objectClass;
    if (libEntry.Find("System.Object", &temp) == PELib::s_class)
    {
        objectClass = static_cast<Class *>(temp);
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }
    Class *int32Class;
    if (libEntry.Find("System.Int32", &temp) == PELib::s_class)
    {
        int32Class = static_cast<Class *>(temp);
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }
    Class *stringClass;
    if (libEntry.Find("System.String", &temp) == PELib::s_class)
    {
        stringClass = static_cast<Class *>(temp);
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }
    Class *fileModeClass;
    if (libEntry.Find("System.IO.FileMode", &temp) == PELib::s_enum)
    {
        fileModeClass = static_cast<Class *>(temp);
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }
    Field *fileModeOpen;
    if (libEntry.Find("System.IO.FileMode.Open", &temp) == PELib::s_field)
    {
        fileModeOpen = static_cast<Field *>(temp);
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }



    // create the function refernece to WriteLine
    Type tp(Type::string, 0);
    std::vector<Type *> typeList;
    typeList.push_back(&tp);
    Method *result = nullptr;
    MethodSignature *signaturep;
    if (libEntry.Find("System.Console.WriteLine", &result, typeList) == PELib::s_method)
    {
        signaturep = result->Signature();
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }

    // create the function reference to open()
    Type tp2(fileModeClass);
    typeList.clear();
    typeList.push_back(&tp);
    typeList.push_back(&tp2);
    MethodSignature *signatureo;
    if (libEntry.Find("System.IO.File.Open", &result, typeList) == PELib::s_method)
    {
        signatureo = result->Signature();
    }
    else
    {
        std::cout << "error" << std::endl;
        exit(1);
    }

    // routine to hold our exception stuff
    MethodSignature *signaturem = libEntry.AllocateMethodSignature("$Main", MethodSignature::Managed, working);
    signaturem->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Method *main = libEntry.AllocateMethod(signaturem, Qualifiers::Private |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed, true);
    cls->Add(main);

    auto tryLabel = libEntry.AllocateOperand("endTry");
    auto try2Label = libEntry.AllocateOperand("endTry2");
    // there will be two try blocks - the inner try block is associated with a catch block
    // and the outer try block is associated with a finally block
    // we do it this way because we can't associate both a catch and finally block with the same protected area
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::seh_try, true));
    // inner try block
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::seh_try, true));

    // the code we are guarding, basically tries to open a nonexistant file
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldstr,
        libEntry.AllocateOperand("unfoundfile", true)));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4, libEntry.AllocateOperand(fileModeOpen->EnumValue(), Operand::i32)));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signatureo))));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_pop));
    // epilogue for the guarded code
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_leave, tryLabel));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::seh_try, false));

    // the catch block, just print something saying we got here
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::seh_catch, true, libEntry.AllocateType(objectClass)));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldstr,
        libEntry.AllocateOperand("catch block", true)));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep))));
    // epilogue for the catch block
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_pop));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_leave, tryLabel));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::seh_catch, false));
    // inner try block comes here after processing
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_label, tryLabel));
    // epilogue for the outer try block
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_leave, try2Label));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::seh_try, false));
    // now make the finally block
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::seh_finally, true));
    // print a message saying we got here
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldstr,
        libEntry.AllocateOperand("finally block", true)));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep))));
    // epilogue for the finally block
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_endfinally, 0));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::seh_finally, false));
    // after processing outer try block comes here
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_label, try2Label));
    // exit function
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        main->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }

    libEntry.DumpOutputFile("test12.il", PELib::ilasm, false);
    libEntry.DumpOutputFile("test12.exe", PELib::peexe, false);

}
int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    test9();
    test10();
    test11();
    test12();
}