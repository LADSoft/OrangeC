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

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#include <list>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
namespace DotNetPELib
{
typedef __int64 longlong;
typedef unsigned __int64 ulonglong;

class PELib;
class Allocator;
class Method;
class Namespace;
class Class;
class Field;
class Enum;
class Instruction;
class Value;
class Local;
class Param;
class MethodSignature;
class Operand;
class Type;
class FieldName;
class DataContainer;

/* contains data, types, methods */

class DestructorBase
{
    public:
        DestructorBase() { }
        DestructorBase(const DestructorBase &);
        virtual ~DestructorBase() { }
};
class PELibError: public std::runtime_error
{
public:

    enum ErrorList
    {
        VarargParamsAlreadyDeclared,
        StackUnderflow,
        MismatchedStack,
        StackNotEmpty,
        DuplicateLabel,
        MissingLabel,
        ShortBranchOutOfRange,
        IndexOutOfRange,
    };
    PELibError(ErrorList err, const std::string &Name="") : errnum(err), std::runtime_error(std::string(errorNames[err]) + " " + Name ) 
    {
    }
    virtual ~PELibError() { }
    ErrorList GetErrnum() { return errnum; }
private:
    ErrorList errnum;
    static char *errorNames[] ;
};
class Qualifiers : public DestructorBase
{
    public:
    enum {
        Nested = 0x1,
        Public = 0x2,
        Private = 0x4,
        Static = 0x8,
        Explicit = 0x10,
        Ansi = 0x20,
        Sealed = 0x40,
        Enum = 0x80,
        Value = 0x100,
        Sequential = 0x200,
        Auto = 0x400,
        Literal = 0x800,
        ValueType = 0x1000,
        HideBySig = 0x2000,
        PreserveSig = 0x4000,
        SpecialName = 0x8000,
        RTSpecialName = 0x10000,
        CIL = 0x20000,
        Managed = 0x40000
    };
    enum
    {
        // settings appropriate for occil, e.g. everything is static.  add public/private...
        MainClass = Explicit | Ansi | Sealed,
        ClassClass = Value | Explicit | Sequential | Ansi | Sealed,
        ClassField = 0,
        FieldInitialized = Static | ValueType,
        EnumClass =  Enum | Auto | Ansi | Sealed,
        EnumField =  Static | Literal | ValueType | Public,
        PInvokeFunc = HideBySig | Static | PreserveSig,
        ManagedFunc = HideBySig | Static | CIL | Managed
    };
    Qualifiers::Qualifiers(int Flags) : flags(Flags) 
    { 
    }
    Qualifiers::Qualifiers(const Qualifiers &old) 
    { 
        flags = old.flags; 
    }
    bool ILSrcDumpBeforeFlags(PELib &);
    bool ILSrcDumpAfterFlags(PELib &);
    static std::string GetName(std::string root, DataContainer *parent, std::string separator = "::");
    int flags;
protected:
    static void ReverseNamePrefix(std::string &rv, DataContainer *parent, int &pos, std::string separator);
    static std::string GetNamePrefix(DataContainer *parent, std::string separator);
private:
    static char * qualifierNames[];
    static int afterFlags;
};
/* contains instructions/ labels */
class CodeContainer  : public DestructorBase
{
public:
    CodeContainer(Qualifiers Flags) :flags(Flags) { }
    void AddInstruction(Instruction *instruction);
    Instruction *RemoveLastInstruction() { 
        Instruction *rv = instructions.back(); 
        instructions.pop_back();
        return rv;
    }
    Instruction *GetLastInstruction() {
        return instructions.back();
    }
    void ValidateInstructions();
    virtual void Optimize(PELib &);
    virtual bool ILSrcDump(PELib &);
    Qualifiers GetFlags() { return flags; }
protected:
    std::map<std::string, Instruction *> labels;
    void OptimizeLDC(PELib &);
    void OptimizeLDLOC(PELib &);
    void OptimizeLDARG(PELib &);
    void OptimizeBranch(PELib &);
    void CalculateOffsets();
    bool ModifyBranches();
    std::list<Instruction *> instructions;
    Qualifiers flags;
    DataContainer *parent;
};
class DataContainer  : public DestructorBase
{
public:
    DataContainer(std::string Name, Qualifiers Flags) : name(Name), flags(Flags), parent(NULL) 
    { 
    }
    void Add(DataContainer *item)
    {
        item->parent = this;
        children.push_back(item);
    }
    void Add(CodeContainer *item)
    {
        methods.push_back(item);
    }
    void Add(Field *field);
    virtual bool ILSrcDump(PELib &);
    DataContainer *GetParent() { return parent; }
    std::string GetName() { return name; }
    Qualifiers GetFlags() { return flags; }
protected:
    std::list<DataContainer *> children;
    std::list<CodeContainer *> methods;
    std::list<Field *> fields;
    DataContainer *parent;
    Qualifiers flags;
    std::string name;
};
class AssemblyDef  : public DestructorBase
{
public:
    AssemblyDef(std::string Name, bool External) : name(Name), external(External)
    {
    }
    virtual bool ILSrcDump(PELib &);
private:
    std::string name;
    bool external;
};
/* a namespace */
class Namespace : public DataContainer
{
public:
    Namespace::Namespace(std::string Name) : DataContainer(Name, Qualifiers(0))
    {
    }
    virtual bool ILSrcDump(PELib &);
};

/* a class, note that it cannot contain namespaces which is enforced at compile time*/
/* enums derive from this */
class Class : public DataContainer
{
public:
    Class::Class (std::string Name , Qualifiers Flags, int Pack, int Size) : DataContainer(Name, Flags), pack(Pack), size(Size)
    {
    }
    virtual bool ILSrcDump(PELib &);
    void ILSrcDumpClassHeader(PELib &);
protected:
    int pack;
    int size;
};
/* a Method, with code */
class Method : public CodeContainer
{
public:
    enum InvokeMode { CIL, PInvoke };
    enum InvokeType { Cdecl, Stdcall };
    Method(MethodSignature *Prototype, Qualifiers flags, bool entry = false) ;
    void SetPInvoke(std::string name, InvokeType type = Stdcall)
    {
        invokeMode = PInvoke;
        pInvokeName  = name;
        pInvokeType = type;
    }
    void AddLocal(Local *local);

    void AddModifier(std::string name, int value)
    {
        modifierNames.push_back(name);
        modifierValues.push_back(value);
    }
    virtual bool ILSrcDump(PELib &);
    virtual void Optimize(PELib &);
    MethodSignature *GetSignature() { return prototype; }
protected:
    void OptimizeLocals(PELib &);
    void CalculateMaxStack();
    void CalculateLive();
    MethodSignature *prototype;
    std::list<Local *> varList;
    std::list<std::string> modifierNames;
    std::list<int> modifierValues;
    std::string pInvokeName;
    InvokeMode invokeMode;
    InvokeType pInvokeType;
    int maxStack;
    bool entryPoint;
};

/* a field, usually static but could be non-static */
class Field  : public DestructorBase
{
public:
    enum ValueSize { i8, i16, i32, i64 };
    enum ValueMode { None, Enum, Bytes };
    Field::Field(std::string Name, Type *tp, Qualifiers Flags) : mode(Field::None), name(Name), flags(Flags),
                type(tp), enumValue(0), byteValue(NULL), byteLength(0)
    {
    }
    void AddEnumValue(longlong Value, ValueSize Size);
    void AddInitializer(unsigned char *bytes, int len); // this will be readonly in ILONLY assemblies
    static bool ILSrcDumpTypeName(PELib &peLib, ValueSize size);
    virtual bool ILSrcDump(PELib &);
    std::string GetName() { return name; }
    void SetContainer(DataContainer *Parent) { parent = Parent; }
    DataContainer *GetContainer() { return parent; }
    void SetFullName(std::string FullName) { fullName = FullName; }
    std::string GetFullName() { return fullName; }
    Type *GetType() { return type; }
    void SetType(Type *tp) { type = tp; }
    Qualifiers GetFlags() { return flags; }
protected:
    DataContainer *parent;
    std::string name;
    std::string fullName;
    Qualifiers flags;
    ValueMode mode;
    Type *type;
    union {
        longlong enumValue;
        unsigned char *byteValue;
    };
    int byteLength;
    ValueSize size;
};
/* A special kind of class: enum */
class Enum : public Class
{
public:
    Enum::Enum(std::string Name, Qualifiers Flags, Field::ValueSize Size) :
            size(Size), Class(Name, Flags, -1, -1) 
    { 
    }
    void AddValue(Allocator &allocator, std::string Name, longlong Value);
    virtual bool ILSrcDump(PELib &);
protected:
    Field::ValueSize size;
};
/* the operand to an instruction */
class Operand : public DestructorBase
{
public:
    enum OpSize { any, i8, u8, i16, u16, i32, u32, i64, u64, inative, r4, r8};
    enum OpType { t_none, t_value, t_int, t_real, t_string, t_label };
    Operand::Operand() : type(t_none) // no operand
    { 
    }
    Operand::Operand(Value *V) : type(t_value), refValue(V)
    { 
    }
    Operand::Operand(longlong Value, OpSize Size) : type(t_int), intValue(Value), sz(Size)
    { 
    }
    Operand::Operand(double Value, OpSize Size) : type(t_real), floatValue(Value), sz(Size) 
    { 
    }
    Operand(std::string Value, bool) : type(t_string) // string
    {
        stringValue = Value;
    }
    Operand(std::string Value) : type(t_label) // label
    {
        stringValue = Value;
    }
    virtual bool ILSrcDump(PELib &);
    Value * GetValue() { return type == t_value ? refValue : NULL; }
    OpType GetType() { return type; }
    longlong GetIntValue() { return intValue; }
    std::string GetStringValue() { return stringValue; }
    double GetFloatValue() { return floatValue; }
    bool IsValidNumber();
protected:
    OpType type;
    OpSize sz;
    Value *refValue;
    std::string stringValue;
    longlong intValue;
    double floatValue;
    bool isnanorinf();
};
/* an assembly language instruction */
class Instruction : public DestructorBase
{
public:
    enum ioperand {
        o_none, o_single, o_rel1, o_rel4, o_index1, o_index2, o_index4,
        o_immed1, o_immed4, o_immed8, o_float4, o_float8, o_switch
    };
    enum iop
    {
        i_unknown, i_label, i_comment, i_add,i_add_ovf,i_add_ovf_un,i_and,i_arglist,i_beq,i_beq_s,i_bge,
    	i_bge_s,i_bge_un,i_bge_un_s,i_bgt,i_bgt_s,i_bgt_un,i_bgt_un_s,i_ble,
    	i_ble_s,i_ble_un,i_ble_un_s,i_blt,i_blt_s,i_blt_un,i_blt_un_s,i_bne_un,
    	i_bne_un_s,i_box,i_br,i_br_s,i_break,i_brfalse,i_brfalse_s,i_brinst,
    	i_brinst_s,i_brnull,i_brnull_s,i_brtrue,i_brtrue_s,i_brzero,i_brzero_s,i_call,
    	i_calli,i_callvirt,i_castclass,i_ceq,i_cgt,i_cgt_un,i_ckfinite,i_clt,
    	i_clt_un,i_constrained_,i_conv_i,i_conv_i1,i_conv_i2,i_conv_i4,i_conv_i8,i_conv_ovf_i,
    	i_conv_ovf_i_un,i_conv_ovf_i1,i_conv_ovf_i1_un,i_conv_ovf_i2,i_conv_ovf_i2_un,i_conv_ovf_i4,i_conv_ovf_i4_un,i_conv_ovf_i8,
    	i_conv_ovf_i8_un,i_conv_ovf_u,i_conv_ovf_u_un,i_conv_ovf_u1,i_conv_ovf_u1_un,i_conv_ovf_u2,i_conv_ovf_u2_un,i_conv_ovf_u4,
    	i_conv_ovf_u4_un,i_conv_ovf_u8,i_conv_ovf_u8_un,i_conv_r_un,i_conv_r4,i_conv_r8,i_conv_u,i_conv_u1,
    	i_conv_u2,i_conv_u4,i_conv_u8,i_cpblk,i_cpobj,i_div,i_div_un,i_dup,
    	i_endfault,i_endfilter,i_endfinally,i_initblk,i_initobj,i_isinst,i_jmp,i_ldarg,
    	i_ldarg_0,i_ldarg_1,i_ldarg_2,i_ldarg_3,i_ldarg_s,i_ldarga,i_ldarga_s,i_ldc_i4,
    	i_ldc_i4_0,i_ldc_i4_1,i_ldc_i4_2,i_ldc_i4_3,i_ldc_i4_4,i_ldc_i4_5,i_ldc_i4_6,i_ldc_i4_7,
    	i_ldc_i4_8,i_ldc_i4_m1,i_ldc_i4_M1,i_ldc_i4_s,i_ldc_i8,i_ldc_r4,i_ldc_r8,i_ldelem,
    	i_ldelem_i,i_ldelem_i1,i_ldelem_i2,i_ldelem_i4,i_ldelem_i8,i_ldelem_r4,i_ldelem_r8,i_ldelem_ref,
    	i_ldelem_u1,i_ldelem_u2,i_ldelem_u4,i_ldelem_u8,i_ldelema,i_ldfld,i_ldflda,i_ldftn,
    	i_ldind_i,i_ldind_i1,i_ldind_i2,i_ldind_i4,i_ldind_i8,i_ldind_r4,i_ldind_r8,i_ldind_ref,
    	i_ldind_u1,i_ldind_u2,i_ldind_u4,i_ldind_u8,i_ldlen,i_ldloc,i_ldloc_0,i_ldloc_1,
    	i_ldloc_2,i_ldloc_3,i_ldloc_s,i_ldloca,i_ldloca_s,i_ldnull,i_ldobj,i_ldsfld,
    	i_ldsflda,i_ldstr,i_ldtoken,i_ldvirtftn,i_leave,i_leave_s,i_localloc,i_mkrefany,
    	i_mul,i_mul_ovf,i_mul_ovf_un,i_neg,i_newarr,i_newobj,i_no_,i_nop,
    	i_not,i_or,i_pop,i_readonly_,i_refanytype,i_refanyval,i_rem,i_rem_un,
    	i_ret,i_rethrow,i_shl,i_shr,i_shr_un,i_sizeof,i_starg,i_starg_s,
    	i_stelem,i_stelem_i,i_stelem_i1,i_stelem_i2,i_stelem_i4,i_stelem_i8,i_stelem_r4,i_stelem_r8,
    	i_stelem_ref,i_stfld,i_stind_i,i_stind_i1,i_stind_i2,i_stind_i4,i_stind_i8,i_stind_r4,
    	i_stind_r8,i_stind_ref,i_stloc,i_stloc_0,i_stloc_1,i_stloc_2,i_stloc_3,i_stloc_s,
    	i_stobj,i_stsfld,i_sub,i_sub_ovf,i_sub_ovf_un,i_switch,i_tail_,i_throw,
    	i_unaligned_,i_unbox,i_unbox_any,i_volatile_,i_xor
    };
    Instruction(iop Op, Operand *Operand);
    // for now only do comments and labels and branches...
    Instruction (iop Op, std::string Text) : op(Op), text(Text), switches(NULL), operand(NULL), live(false) { }

    virtual ~Instruction() { if (switches) delete switches; }
    virtual bool ILSrcDump(PELib &);
    struct InstructionName {
        char *name;
        unsigned char op1;
        unsigned char op2;
        unsigned char bytes;
        unsigned char operandType;
        char stackUsage; // positive it adds to stack, negative it consumes stack
    };
    void AddCaseLabel(std::string label);
    std::list<std::string> * GetSwitches() { return switches; }
    iop GetOp() { return op; }
    void SetOp(iop Op) { op = Op; }
    void NullOperand(Allocator &allocator) ;
    Operand *GetOperand() { return operand; }
    std::string GetText() { return text; }
    int GetOffset() { return offset; }
    void SetOffset(int Offset) { offset = Offset; }
    int GetInstructionSize();
    int GetStackUsage();
    int IsRel4() { return instructions[op].operandType == o_rel4; }
    int IsRel1() { return instructions[op].operandType == o_rel1; }
    int IsBranch() { IsRel1() || IsRel4(); }
    void Rel4To1() { op++; }
    void SetLive(bool val) { live = val; }
    bool IsLive() { return live; }
protected:
    std::list<std::string> *switches;
    iop op;
    int offset;
    Operand *operand; // for non-labels
    std::string text; // for labels or comments
    bool live;
    static InstructionName instructions[]; 
};

/* a value, typically to be used as an operand */
class Value : public DestructorBase
{
public:
    Value(std::string Name, Type *tp) : name(Name), type(tp) { }
    virtual bool ILSrcDump(PELib &);
    Type *GetType() { return type; }
    std::string GetName() { return name; }
protected:
    std::string name;
    Type *type;
};
/* a local variable */
class Local : public Value
{
public:
    Local(std::string Name, Type *Tp) : Value(Name, Tp), uses(0), index(-1) { }
    void SetIndex(int Index) { index = Index; }
    int GetIndex() { return index; }
    virtual bool ILSrcDump(PELib &);
    void IncrementUses() { uses++; }
    int Uses() { return uses; }
private:
    int index;
    int uses;
};
/* a parameter */
class Param : public Value
{
public:
    Param(std::string Name, Type *Tp) : Value(Name, Tp), index(-1) { }
    void SetIndex(int Index) { index = Index; }
    int GetIndex() { return index; }
    virtual bool ILSrcDump(PELib &);
private:
    int index;
};
/* a field name (used as an operand) */
class FieldName : public Value
{
public:
    FieldName(Field *F, std::string Name ="", std::string Path="") : field(F), Value("", NULL) 
    { 
        if (Name.size())
            if (Path.size())
                field->SetFullName(Path + "::'" + Name + "'");
            else
                field->SetFullName(std::string("'") + Name + "'");
    }
    virtual bool ILSrcDump(PELib &);
    Field *GetField() { return field; }
protected:
    Field *field;
};
/* a method name (used as an operand) */
class MethodName : public Value
{
public:
    MethodName(MethodSignature *M, std::string Name = "", std::string Path = "");
    virtual bool ILSrcDump(PELib &);
    MethodSignature *GetSignature() { return signature; }
protected:
    MethodSignature *signature;
};
/* the signature for a method, has a return type and a list of params.
 * params can be either named or unnamed
 */
class MethodSignature : public DestructorBase
{
public:
    enum { Vararg = 1, Managed = 2, Instance = 4 };
    MethodSignature(std::string Name, int Flags, DataContainer *Container) : container(Container), name(Name), flags(Flags), returnType(NULL)
    {
    }
    void AddReturnType(Type *type)
    {
        returnType = type;
    }
    Type *GetReturnType() { return returnType; }
    void AddParam(Param *param)
    {
        if (varargParams.size())
        {
            throw PELibError(PELibError::VarargParamsAlreadyDeclared);
        }
        param->SetIndex(params.size());
        params.push_back(param);
    }
    void AddVarargParam(Param *param);
    std::string GetName() { return name; }
    void SetName(std::string Name) { name = Name; }
    std::string GetFullName() { return fullName; }
    void SetFullName(std::string FullName) { fullName = FullName; }
    virtual bool ILSrcDump(PELib &, bool names, bool asType, bool PInvoke );
    void SetInstanceFlag() { flags |= Instance; }
    int GetFlags() { return flags; }
    size_t GetParamCount() { return params.size(); }
    size_t GetVarargParamCount() { return varargParams.size(); }
protected:
    DataContainer *container;
    Type *returnType;
    std::string name;
    std::string fullName;
    int flags;
    std::list<Param *> params, varargParams;
};
/* a type reference */
class Type : public DestructorBase
{
public:
    enum BasicType {cls, method, Void, i8, u8, i16, u16, i32, u32, i64, u64, inative, r32, r64, object, objectArray, string};
    Type(BasicType Tp, int PointerLevel) : tp(Tp), pointerLevel(PointerLevel), typeRef(NULL), methodRef(NULL)
    {
    }
    Type(DataContainer *clsref) : tp(cls), pointerLevel(0), typeRef(clsref), methodRef(NULL)
    {
    }
    Type(MethodSignature *methodref) : tp(method), pointerLevel(0), typeRef(NULL), methodRef(methodref) 
    { 
    }
    void SetFullName(std::string name) { fullName = name; }
    void SetPointer(int n) { pointerLevel = n; }
    enum BasicType GetBasicType() { return tp; }
    void SetBasicType(BasicType type) { tp = type; }
    DataContainer *GetClass() { return typeRef; }
    virtual bool ILSrcDump(PELib &);
    bool IsVoid() { return tp == Void && pointerLevel == 0; }
protected:
    std::string fullName;
    int pointerLevel;
    BasicType tp;
    DataContainer *typeRef; 
    MethodSignature *methodRef;
private:
    static char *typeNames[];
};
class BoxedType : public Type
{
public:
    BoxedType(BasicType Tp) : Type(Tp, 0)
    {
    }
    virtual bool ILSrcDump(PELib &);
private:
    static char *typeNames[];
};

class Allocator
{
public:
    Allocator() : first(NULL), current(NULL) { }
    virtual ~Allocator() { FreeAll(); }

    AssemblyDef *AllocateAssemblyDef(std::string Name, bool External);
    Namespace *AllocateNamespace(std::string Name);
    Class *AllocateClass (std::string Name , Qualifiers Flags, int Pack, int Size);
    Method *AllocateMethod(MethodSignature *Prototype, Qualifiers flags, bool entry = false); 
    Field *AllocateField(std::string Name, Type *tp, Qualifiers Flags);
    Enum *AllocateEnum(std::string Name, Qualifiers Flags, Field::ValueSize Size);
    Operand *AllocateOperand();
    Operand *AllocateOperand(Value *V);
    Operand *AllocateOperand(longlong Value, Operand::OpSize Size);
    Operand *AllocateOperand(double Value, Operand::OpSize Size);
    Operand *AllocateOperand(std::string Value, bool);
    Operand *AllocateOperand(std::string Value);
    Instruction *AllocateInstruction(Instruction::iop Op, Operand *Operand = NULL);
    Instruction *AllocateInstruction (Instruction::iop Op, std::string Text);
    Value *AllocateValue(std::string name, Type *tp);
    Local *AllocateLocal(std::string name, Type *tp);
    Param *AllocateParam(std::string name, Type *tp);
    FieldName *AllocateFieldName(Field *F, std::string Name ="", std::string Path="");
    MethodName *AllocateMethodName(MethodSignature *M, std::string Name = "", std::string Path = "");
    MethodSignature *AllocateMethodSignature(std::string Name, int Flags, DataContainer *Container);
    Type *AllocateType(Type::BasicType Tp, int PointerLevel);
    Type *AllocateType(DataContainer *clsref);
    Type *AllocateType(MethodSignature *methodref);
    BoxedType *AllocateBoxedType(Type::BasicType Tp);
    unsigned char *AllocateBytes(size_t sz);
    enum
    {
        AllocationSize = 0x10000,
    };
private:
    struct Block
    {
        Block() : next(NULL), offset(0) { memset(bytes, 0, AllocationSize); }
        Block*next;
        int offset;
        int bytes[AllocationSize];
    };
    void *BaseAlloc(size_t size);
    void FreeBlock(Block *b);
    void FreeAll();

    Block *first, *current;
};
/* Main class that is instantiated, contains everything else */
class PELib : public DataContainer, public Allocator
{
public:
    enum CorFlags { ilonly = 1, bits32 = 2 };
    enum OutputMode { ilasm, peexe, pedll };
    PELib(std::string AssemblyName, int CoreFlags);
    bool DumpOutputFile(std::string fileName, OutputMode mode);
    void AddExternalAssembly(std::string assemblyName);
    void AddPInvokeReference(MethodSignature *methodsig, std::string dllname, bool iscdecl);
    virtual bool ILSrcDump(PELib &) { return ILSrcDumpHeader() && ILSrcDumpFile(); }
    std::fstream &Out() { return *outputStream; }
protected:
    bool ILSrcDumpHeader();
    bool ILSrcDumpFile();
    bool DumpPEFile(bool isexe);
    std::list<AssemblyDef *>assemblyRefs;
    std::list<Method *>pInvokeSignatures;
    std::string assemblyName;
    std::fstream *outputStream;
    std::string fileName;
    int corFlags;
};

} // namespace