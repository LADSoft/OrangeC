
#define LLONG_TYPE __int64
namespace DotNetPELib
{
//class Method;
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
//class FieldName;

#include <list>
#include <string>

/* contains data, types, methods */
class DataContainer
{
public:
    void Add(DataContainer *);
    virtual void DumpILSrc();
protected:
    void SetParent(DataContainer *);
    std::list<DataContainer *> children;
    DataContainer *parent;
};
/* contains instructions/ labels */
class CodeContainer
{
public:
    void AddInstruction(Instruction *instruction);
    void AddLabel(std::string labelName);
    virtual void Optimize();
    virtual void DumpILSrc();
protected:
    std::list<Instruction *> instructions;
};
/* Main class that is instantiated, contains everything else */
class PELib : public DataContainer
{
public:
    enum CorFlags { ilonly = 1, bits32 = 2 };
    enum OutputMode { ilasm, peexe, pedll };
    PELib(std::string assemblyName, int CoreFlags);
    bool DumpOutputFile(OutputMode mode);
    void AddExternalAssembly(std::string assemblyName);
    void AddPInvokeReference(MethodSignature *method);
    virtual void DumpILSrc();
protected:
    bool DumpILSrcFile();
    bool DumpPEFile(bool isexe);
    int corFlags;
};
/* a namespace */
class Namespace : public DataContainer
{
public:
    Namespace(std::string Name);
    virtual void DumpILSrc();
protected:
    std::string name;
};

/* a class, note that it cannot contain namespaces which is enforced at compile time*/
/* enums derive from this */
class Class : public DataContainer
{
public:
    enum { f_public = 1, f_private = 2, f_static = 4, f_explicit = 8, 
        f_ansi = 16, f_sealed = 32, f_enum = 64, f_nested = 128, 
        f_value = 256, f_sequential = 512, f_auto = 1024 };
    Class (std::string Name , int Flags, int Pack, int Size);
    virtual void DumpILSrc();
    void DumpClassHeader();
private:
    void AddNamespace(Namespace *);
protected:
    std::string name;
    int flags;
    int pack;
    int size;
};
/* a Method, with code */
class Method : public CodeContainer
{
public:
    enum { f_public = 1, f_private = 2, f_static = 4, f_hidebysig = 8, 
        f_specialname = 16, f_rtspecialname = 32 } ;
    Method(MethodSignature *Prototype, int flags);
    void AddLocal(Local *local);
    void AddModifier(std::string name, int value);
    virtual void DumpILSrc();
    virtual void Optimize();
protected:
    MethodSignature *prototype;
    std::list<Local *> varList;
    std::list<std::string> ModifierNames;
    std::list<int> ModifierValues;
    int maxStack;
    int flags;
};

/* a field, usually static but could be non-static */
class Field
{
public:
    enum { f_public = 1, f_static = 2, f_valuetype = 4, f_literal = 8,
        f_specialname = 16, f_rtspecialname = 32 };
    Field(FieldName *Name, int Flags);
    void AddEnumValue(int);
    void AddInitializer(unsigned char *bytes, int len); // this will be readonly in ILONLY assemblies
    virtual void DumpILSrc();
protected:
    FieldName *name;
    int flags;
};
/* A special kind of class: enum */
class Enum : public Class
{
public:
    enum sz { i8, i16, i32, i64 };
    Enum(std::string Name, int Flags, sz Size);
    void AddValue(std::string name, int value);
    virtual void DumpILSrc();
protected:
    std::string name;
    int Flags;
    sz size;
};
/* an assembly language instruction */
class Instruction
{
public:
    enum iop
    {
        i_label, i_add,i_add_ovf,i_add_ovf_un,i_and,i_arglist,i_beq,i_beq_s,i_bge,
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
    Instruction (std::string label); // special type of instruction e.g. label
    virtual void DumpILSrc();
protected:
    iop op;
    Operand *Operand; // for non-labels
    std::string label; // for labels
};

/* the operand to an instruction */
class Operand
{
public:
    enum nmsz { i8, u8, i16, u16, i32, u32, i64, u64, inative, r4, r8};
    enum type { t_value, t_int, t_double };
    Operand(); // no operand
    Operand(Value *V);
    Operand(LLONG_TYPE Value, nmsz size);
    Operand(double Value, nmsz size);
    virtual void DumpILSrc();
protected:
    type tp;
    nmsz sz;
    Value *refValue;
    LLONG_TYPE intValue;
    double floatValue;
};
/* a value, typically to be used as an operand */
class Value
{
public:
    Value(std::string Name, Type *tp);
    virtual void DumpILSrc();
private:
    std::string name;
    Type *type;
};
/* a local variable */
class Local : public Value
{
public:
    Local(std::string name, Type *tp) : Value(name, tp) { }
    int index;
    virtual void DumpILSrc();
};
/* a parameter */
class Param : public Value
{
public:
    Param(std::string name, Type *tp) : Value(name, tp) { }
    virtual void DumpILSrc();
};
/* a field name (used as an operand) */
class FieldName : public Value
{
public:
    FieldName(Field *F);
    virtual void DumpILSrc();
protected:
    Field *field;
};
/* the signature for a method, has a return type and a list of params.
 * params can be either named or unnamed
 */
class MethodSignature
{
public:
    enum { f_vararg = 1 };
    MethodSignature(std::string Name, int Flags);
    void AddReturnType(Type *type);
    void AddParam(Type *type); // for prototypes
    void AddParam(Param *param); // for method declarations
    virtual void DumpILSrc();
protected:
    Type *returnType;
    std::string name;
    int flags;
    std::list<Param *> params;
};
/* a type reference */
class Type
{
public:
    enum BasicType {cls, Void, i8, u8, i16, u16, i32, u32, i64, u64, inative, r4, r8};
    Type(BasicType Tp, int PointerLevel);
    Type(DataContainer *cls); // for now pointers to classes are always void *...
    virtual void DumpILSrc();
protected:
    BasicType tp;
    int pointerLevel;
    DataContainer *typeRef; 
};
}