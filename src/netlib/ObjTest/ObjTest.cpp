#include "..\DotNetPELib.h"
using namespace DotNetPELib;

void writeq1()
{
    PELib libEntry("q1", PELib::ilonly | PELib::bits32);

    MethodSignature *signaturex = libEntry.AllocateMethodSignature("putchar", 0, nullptr);
    signaturex->ReturnType(libEntry.AllocateType(Type::Void, 0));
    signaturex->AddParam(libEntry.AllocateParam("ch", libEntry.AllocateType(Type::i32, 0)));
    libEntry.AddPInvokeReference(signaturex, "msvcrt.dll", true);

    DataContainer *working = libEntry.WorkingAssembly();

    Namespace *nmspc = libEntry.AllocateNamespace("nmspc");
    working->Add(nmspc);
    Class *cls = libEntry.AllocateClass("cls", Qualifiers::Ansi | Qualifiers::Sealed, -1, -1);
    nmspc->Add(cls);

    Field *fld = libEntry.AllocateField("fld", libEntry.AllocateType(Type::i32, 0), Qualifiers::Static);
    cls->Add(fld);

    Enum *enm = libEntry.AllocateEnum("enm", Qualifiers::EnumClass | Qualifiers::Public, Field::i32);
    cls->Add(enm);

    enm->AddValue(libEntry, "one", 323);
    Field *two = enm->AddValue(libEntry, "two", 5015);

    Class *i8Cls = libEntry.AllocateClass("int8[]", Qualifiers::Public | Qualifiers::Explicit | Qualifiers::Ansi | Qualifiers::Sealed | Qualifiers::Value, 1, 1);
    Field *pS = libEntry.AllocateField("pS", libEntry.AllocateType(i8Cls), Qualifiers::Public | Qualifiers::Static);
    Field *Str = libEntry.AllocateField("Str", libEntry.AllocateType(i8Cls), Qualifiers::Public | Qualifiers::Static);
    cls->Add(i8Cls);
    cls->Add(pS);
    cls->Add(Str);

    static Byte pSInit[] = { 0x25, 0x73, 0x0a, 0 }; // %s
    static Byte StrInit[] = { 0x48, 0x49, 0 }; // HI
    pS->AddInitializer(pSInit, sizeof(pSInit));
    Str->AddInitializer(StrInit, sizeof(StrInit));


    MethodSignature *signaturex1 = libEntry.AllocateMethodSignature("printf", MethodSignature::Vararg, nullptr);
    signaturex1->ReturnType(libEntry.AllocateType(Type::i32, 0));
    signaturex1->AddParam(libEntry.AllocateParam("format", libEntry.AllocateType(Type::Void, 1)));
    libEntry.AddPInvokeReference(signaturex1, "msvcrt.dll", true);

    MethodSignature *signaturexa = libEntry.AllocateMethodSignature("printf", MethodSignature::Vararg, nullptr);
    signaturexa->ReturnType(libEntry.AllocateType(Type::i32, 0));
    signaturexa->AddParam(libEntry.AllocateParam("format", libEntry.AllocateType(Type::Void, 1)));
    signaturexa->AddVarargParam(libEntry.AllocateParam("A_1", libEntry.AllocateType(Type::Void, 1)));
    signaturexa->SignatureParent(signaturex);


    MethodSignature *signatures = libEntry.AllocateMethodSignature("Start", MethodSignature::Managed, cls);
    signatures->ReturnType(libEntry.AllocateType(Type::Void, 0));
    Param *parames1 = libEntry.AllocateParam("strn", libEntry.AllocateType(Type::string, 0));
    signatures->AddParam(parames1);

    // add a reference to the assembly
    libEntry.LoadAssembly("mscorlib");

    // make a property
    // create the property
    void *presult;
    std::vector<Type *> dummy;
    Property *property = libEntry.AllocateProperty(libEntry, "localProperty",
        libEntry.AllocateType(Type::i32, 0), dummy);

    // set it to be a static property
    property->Instance(false);

    cls->Add(property);


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

    Type tp1(Type::i32, 0);
    std::vector<Type *> typeList1;
    typeList1.push_back(&tp1);
    Method *result1 = nullptr;
    MethodSignature *signaturep1;
    if (libEntry.Find("System.Console.WriteLine", &result1, typeList1) == PELib::s_method)
    {
        signaturep1 = result1->Signature();
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
        libEntry.AllocateOperand(libEntry.AllocateFieldName(fld))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4,
        libEntry.AllocateOperand(48, Operand::i32)));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_add));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep1))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldarg,
        libEntry.AllocateOperand(parames1)));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep))));

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldstr,
        libEntry.AllocateOperand("this is a string in Start()", true)));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep))));

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4,
        libEntry.AllocateOperand(273, Operand::i32)));
    property->CallSet(libEntry, start);
    property->CallGet(libEntry, start);
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturep1))));

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldsflda,
        libEntry.AllocateOperand(libEntry.AllocateFieldName(pS))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldsflda,
        libEntry.AllocateOperand(libEntry.AllocateFieldName(Str))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturexa))));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_pop, nullptr));

    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4,
        libEntry.AllocateOperand('A', Operand::i32)));
    start->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturex))));

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

    libEntry.DumpOutputFile("q1.ilo", PELib::object, false);
}
void writeq2()
{
    PELib libEntry("q2", PELib::ilonly | PELib::bits32);

    MethodSignature *signaturex = libEntry.AllocateMethodSignature("putchar", 0, nullptr);
    signaturex->ReturnType(libEntry.AllocateType(Type::Void, 0));
    signaturex->AddParam(libEntry.AllocateParam("ch", libEntry.AllocateType(Type::i32, 0)));
    libEntry.AddPInvokeReference(signaturex, "msvcrt.dll", true);

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

    Type tp1(Type::i32, 0);
    std::vector<Type *> typeList1;
    typeList1.push_back(&tp1);
    Method *result1 = nullptr;
    MethodSignature *signaturep1;
    if (libEntry.Find("System.Console.WriteLine", &result1, typeList1) == PELib::s_method)
    {
        signaturep1 = result1->Signature();
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

    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldc_i4,
        libEntry.AllocateOperand('X', Operand::i32)));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_call,
        libEntry.AllocateOperand(libEntry.AllocateMethodName(signaturex))));
    main->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    try
    {
        main->Optimize(libEntry);
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what() << std::endl;
    }
    libEntry.DumpOutputFile("q2.ilo", PELib::object, false);
}
void writeq3()
{
    PELib libEntry("q3", PELib::ilonly | PELib::bits32);

    DataContainer *working = libEntry.WorkingAssembly();

    Namespace *nmspc = libEntry.AllocateNamespace("nmspc");
    working->Add(nmspc);
    Class *cls = libEntry.AllocateClass("cls", Qualifiers::Ansi | Qualifiers::Sealed, -1, -1);
    nmspc->Add(cls);

    Field *fld = libEntry.AllocateField("fld", libEntry.AllocateType(Type::i32, 0), Qualifiers::Static);
    cls->Add(fld);

    // create the property
    void *presult;
    std::vector<Type *> dummy;
    Property *property = libEntry.AllocateProperty(libEntry, "localProperty",
        libEntry.AllocateType(Type::i32, 0), dummy);

    // set it to be a static property
    property->Instance(false);

    cls->Add(property);


    // make the getter - load the value and add 64
    CodeContainer *getter = property->Getter();
    getter->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ldsfld,
        libEntry.AllocateOperand(libEntry.AllocateFieldName(fld))));
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
        libEntry.AllocateOperand(libEntry.AllocateFieldName(fld))));
    setter->AddInstruction(libEntry.AllocateInstruction(Instruction::i_ret, nullptr));

    libEntry.DumpOutputFile("q3.ilo", PELib::object, false);
}
void read()
{
    PELib libEntry("qqq", PELib::ilonly | PELib::bits32);
    if (!libEntry.LoadObject("q1.ilo"))
        printf("error 1");
    else if (!libEntry.LoadObject("q2.ilo"))
        printf("error 2");
    else if (!libEntry.LoadObject("q3.ilo"))
        printf("error 3");
    else
    {
        libEntry.DumpOutputFile("qqq.il", PELib::ilasm, false);
        libEntry.DumpOutputFile("qqq.exe", PELib::peexe, false);
    }
}

void main()
{
    writeq1();
    writeq2();
    writeq3();
    read();
}