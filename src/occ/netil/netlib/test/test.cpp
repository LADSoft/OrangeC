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
                                    libEntry.AllocateOperand((longlong)'A', Operand::i32)));
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
    libEntry.AddExternalAssembly("mscorlib");

    // create the function refernce
    MethodSignature *signaturep = libEntry.AllocateMethodSignature("WriteLine", 0, nullptr);
    signaturep->ReturnType(libEntry.AllocateType(Type::Void, 0));
    signaturep->AddParam(libEntry.AllocateParam("strng", libEntry.AllocateType(Type::string, 0)));
    // this is what links it to the external assembly
    // this will only work for functions in the outer class at present...
    // later I will make the lib read in the contents of assemblies and then you would
    // do a search for the signature using a string similar to below (but with argument list attached), 
    // instead of creating one from scratch
    signaturep->FullName("[mscorlib]System.Console::WriteLine");

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
int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
}