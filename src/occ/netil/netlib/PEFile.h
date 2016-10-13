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

#include <vector>
#include <string>
namespace DotNetPELib {



class TableEntryBase;
class Method;
class Field;
class Type;
class PELib;

class PEMethod;
typedef std::vector<TableEntryBase *> DNLTable;

enum
{
    MaxTables = 64
};
enum Tables {
    tModule = 0,//
    tTypeRef = 1,//
    tTypeDef = 2,//
    tField = 4,//
    tMethodDef = 6,//
    tParam = 8,//
    tMemberRef = 10,//
    tConstant = 11,//
    tCustomAttribute = 12,
    tClassLayout = 15,//
    tFieldLayout = 16,
    tStandaloneSig = 17,
    tModuleRef = 26,
    tTypeSpec = 27,//
    tImplMap = 28,//
    tFieldRVA = 29,//
    tAssemblyDef = 32,//
    tAssemblyRef = 35,//
    tNestedClass = 41,//
};

enum Types
{
    ELEMENT_TYPE_END            = 0x0,

    ELEMENT_TYPE_VOID           = 0x1,
    ELEMENT_TYPE_BOOLEAN        = 0x2,
    ELEMENT_TYPE_CHAR           = 0x3,
    ELEMENT_TYPE_I1             = 0x4,
    ELEMENT_TYPE_U1             = 0x5,

    ELEMENT_TYPE_I2             = 0x6,
    ELEMENT_TYPE_U2             = 0x7,
    ELEMENT_TYPE_I4             = 0x8,
    ELEMENT_TYPE_U4             = 0x9,
    ELEMENT_TYPE_I8             = 0xa,

    ELEMENT_TYPE_U8             = 0xb,
    ELEMENT_TYPE_R4             = 0xc,
    ELEMENT_TYPE_R8             = 0xd,
    ELEMENT_TYPE_STRING         = 0xe,

    // every type above PTR will be simple type


    ELEMENT_TYPE_PTR            = 0xf,      // PTR <type>

    ELEMENT_TYPE_BYREF          = 0x10,     // BYREF <type>


    // Please use ELEMENT_TYPE_VALUETYPE.

    // ELEMENT_TYPE_VALUECLASS is deprecated.

    ELEMENT_TYPE_VALUETYPE      = 0x11,  // VALUETYPE <class Token>

    ELEMENT_TYPE_CLASS          = 0x12,  // CLASS <class Token>

    ELEMENT_TYPE_VAR            = 0x13,  // a class type variable VAR <U1>

    // MDARRAY <type> <rank> <bcount>

    // <bound1> ... <lbcount> <lb1> ...

    ELEMENT_TYPE_ARRAY          = 0x14,
    // GENERICINST <generic type> <argCnt> <arg1> ... <argn>

    ELEMENT_TYPE_GENERICINST    = 0x15,
    // TYPEDREF  (it takes no args) a typed referece to some other type

    ELEMENT_TYPE_TYPEDBYREF     = 0x16,

    // native integer size

    ELEMENT_TYPE_I              = 0x18,

    // native unsigned integer size

    ELEMENT_TYPE_U              = 0x19,
    // FNPTR <complete sig for the function

    // including calling convention>

    ELEMENT_TYPE_FNPTR          = 0x1B,

    // Shortcut for System.Object

    ELEMENT_TYPE_OBJECT         = 0x1C,
    // Shortcut for single dimension zero lower bound array

    // SZARRAY <type>

    ELEMENT_TYPE_SZARRAY        = 0x1D,

    // a method type variable MVAR <U1>

    ELEMENT_TYPE_MVAR           = 0x1e,

    // This is only for binding

    // required C modifier : E_T_CMOD_REQD <mdTypeRef/mdTypeDef>

    ELEMENT_TYPE_CMOD_REQD      = 0x1F,

    ELEMENT_TYPE_CMOD_OPT       = 0x20,
    // optional C modifier : E_T_CMOD_OPT <mdTypeRef/mdTypeDef>


    // This is for signatures generated internally

    // (which will not be persisted in any way).

    ELEMENT_TYPE_INTERNAL       = 0x21,

    // INTERNAL <typehandle>


    // Note that this is the max of base type excluding modifiers

    ELEMENT_TYPE_MAX            = 0x22,
    // first invalid element type



    ELEMENT_TYPE_MODIFIER       = 0x40,

    // sentinel for varargs

    ELEMENT_TYPE_SENTINEL       = 0x01 | ELEMENT_TYPE_MODIFIER,
    ELEMENT_TYPE_PINNED         = 0x05 | ELEMENT_TYPE_MODIFIER,
    // used only internally for R4 HFA types

    ELEMENT_TYPE_R4_HFA         = 0x06 | ELEMENT_TYPE_MODIFIER,
    // used only internally for R8 HFA types


    ELEMENT_TYPE_R8_HFA         = 0x07 | ELEMENT_TYPE_MODIFIER,

};
class SignatureGenerator
{
public:
    static unsigned char *MethodDefSig(MethodSignature *signature, size_t &sz);
    static unsigned char *MethodRefSig(MethodSignature *signature, size_t &sz);
    //unsigned char *MethodSpecSig(Method *method);
    static unsigned char *FieldSig(Field *field, size_t &sz);
    //unsigned char *PropertySig(Property *property);
    static unsigned char *LocalVarSig(Method *method, size_t &sz);
    static unsigned char *TypeSig(Type *type, size_t &sz);
    static size_t EmbedType(int *buf, int offset, Type *tp);
    static unsigned char *ConvertToBlob(int *buf, int size, int &sz);
    static void SetObjectType(size_t ObjectBase) { objectBase = ObjectBase; }
private:
    static size_t CoreMethod(MethodSignature *method, int paramCount, int *buf, int offset);
    static int workArea[400 * 1024];
    static int basicTypes[];
    static int objectBase;
};
class PEWriter
{
public:
    PEWriter(bool isexe) : DLL(!isexe), objectBase(0), valueBase(0), enumBase(0), 
            systemIndex(0), entryPoint(0), paramAttributeType(0), paramAttributeData(0) { }
    virtual ~PEWriter();
    size_t AddTableEntry(TableEntryBase *entry);
    void AddMethod(PEMethod *method);
    size_t HashString(std::string utf8);
    size_t HashUS(std::wstring str);
    size_t HashGUID(unsigned char *Guid);
    size_t HashBlob(unsigned char *blobData, size_t blobLen);
    size_t RVABytes(unsigned char *bytes, size_t data);
    void SetBaseClasses(size_t ObjectIndex, size_t ValueIndex, size_t EnumIndex, size_t SystemIndex)
    {
        SignatureGenerator::SetObjectType(ObjectIndex);
        objectBase = ObjectIndex;
        valueBase = ValueIndex;
        enumBase = EnumIndex;
        systemIndex = SystemIndex;
    }
    void SetParamAttribute(size_t paramAttributeType, size_t paramAttributeData)
    {
        paramAttributeType = paramAttributeType;
        paramAttributeData = paramAttributeData;
    }

    size_t ObjectBaseClass() { return objectBase; }
    size_t ValueBaseClass() { return valueBase; }
    size_t EnumBaseClass() { return enumBase; }
    size_t SystemName() { return systemIndex; }

    size_t GetParamAttributeType() { return paramAttributeType; }
    size_t GetParamAttributeData() { return paramAttributeData; }

    static void CreateGuid(unsigned char *Guid);

    bool WriteFile(PELib &peLib, std::fstream &out);
    size_t GetIndex(int table) { return tables[table].size() + 1; }
protected:

    bool WritePEHeader();
    bool WriteStartupInfo();
    bool WriteImportInfo();
    bool WriteCoreHeader();
    bool WriteStreamHeaders();
    bool WriteStrings();
    bool WriteUS();
    bool WriteBlob();
    bool WriteGUID();
    bool WriteDatabase();
    bool WriteMethods();
    bool WriteTables();
    bool WriteRelocInfo();


private:
    std::map<std::string, size_t> stringMap;
    struct pool
    {
        pool() : size(1), max(200), base(NULL) { base = (unsigned char *)calloc(1, max); }
        ~pool() { free(base); }
        size_t size;
        size_t max;
        unsigned char *base;
        void Ensure(size_t newSize);
    };
    DNLTable tables[MaxTables];
    size_t entryPoint;
    std::list<PEMethod *>methods;
    size_t objectBase;
    size_t valueBase;
    size_t enumBase;
    size_t systemIndex;
    size_t paramAttributeType, paramAttributeData;
    bool DLL;
    pool strings;
    pool us;
    pool blob;
    pool guid;
    pool rva;
};

class IndexBase
{
public:
    IndexBase() : tag(0), index(0) { }
    IndexBase(size_t Index) : tag(0), index(Index)  { UpdateWidth(); }
    IndexBase(int Tag, size_t Index) : tag(Tag), index(index) { UpdateWidth(); }
    int tag;
    size_t index;
    void UpdateWidth();

    size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
    virtual int GetShift() = 0;
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]) = 0;
};
class ResolutionScope : public IndexBase
{
public:
    ResolutionScope() { }
    ResolutionScope(int tag, int index) : IndexBase(tag, index) { }
    enum Tags
    {
        TagBits = 2,
        Module = 0,
        ModuleRef = 1,
        AssemblyRef = 2,
        TypeRef = 3
    };
    virtual int GetShift() { return TagBits; }
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]);
};
class TypeDefOrRef : public IndexBase
{
public:
    TypeDefOrRef() { }
    TypeDefOrRef(int tag, int index) : IndexBase(tag, index) { }
    enum Tags
    {
        TagBits = 2,
        TypeDef = 0,
        TypeRef = 1,
        TypeSpec = 2,
    };
    virtual int GetShift() { return TagBits; }
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]);
};
class MemberRefParent : public IndexBase
{
public:
    MemberRefParent() { }
    MemberRefParent(int tag, int index) : IndexBase(tag, index) { }
    enum Tags
    {
        // memberrefparent
        TagBits = 3,
        TypeDef = 0,
        TypeRef = 1,
        ModuleRef = 2,
        MethodDef = 3,
        TypeSpec = 4,
    };
    virtual int GetShift() { return TagBits; }
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]);
};
class Constant : public IndexBase
{
public:
    Constant() { }
    Constant(int tag, int index) : IndexBase(tag, index) { }
    enum Tags {
        // HasConstant
        TagBits = 2,
        FieldDef = 0,
        ParamDef = 1,
        //TagProperty = 2,
    };
    virtual int GetShift() { return TagBits; }
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]);
};
class CustomAttribute : public IndexBase
{
public:
    CustomAttribute() { }
    CustomAttribute(int tag, int index) : IndexBase(tag, index) { }
    enum Tags {
        TagBits = 5,
        MethodDef = 0,
        FieldDef = 1,
        TypeRef = 2,
        TypeDef = 3,
        ParamDef = 4,
        InterfaceImpl = 5,
        MemberRef = 6,
        Module = 7,
        Permission = 8,
        Property = 9,
        Event = 10,
        StandaloneSig = 11,
        ModuleRef = 12,
        TypeSpec = 13,
        Assembly = 14,
        AssemblyRef = 15,
        File = 16,
        ExportedType = 17,
        ManifestResource = 18,
    };
    virtual int GetShift() { return TagBits; }
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]);
};
class CustomAttributeType : public IndexBase
{
public:
    CustomAttributeType() { }
    CustomAttributeType(int tag, int index) : IndexBase(tag, index) { }
    enum Tags
    {
        // custom attribute type
        TagBits = 3,
        MethodDef = 2,
        MethodRef = 3,
    };
    virtual int GetShift() { return TagBits; }
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]);
};
class MemberForwarded : public IndexBase
{
public:
    MemberForwarded() { }
    MemberForwarded(int tag, int index) : IndexBase(tag, index) { }
    enum Tags {
        TagBits = 1,
        FieldDef = 0,
        MethodDef = 1,
    };
    virtual int GetShift() { return TagBits; }
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]);
};
class FieldList : public IndexBase
{
public:
    FieldList() { }
    FieldList(int index) : IndexBase(index) { }
    virtual int GetShift() { return 0; }
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]);
};
class MethodList : public IndexBase
{
public:
    MethodList() { }
    MethodList(int index) : IndexBase(index) { }
    virtual int GetShift() { return 0; }
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]);
};
class ParamList : public IndexBase
{
public:
    ParamList() { }
    ParamList(int index) : IndexBase(index) { }
    virtual int GetShift() { return 0; }
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]);
};
class TypeDef : public IndexBase
{
public:
    TypeDef() { }
    TypeDef(int index) : IndexBase(index) { }
    virtual int GetShift() { return 0; }
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]);
};
class ModuleRef : public IndexBase
{
public:
    ModuleRef() { }
    ModuleRef(int index) : IndexBase(index) { }
    virtual int GetShift() { return 0; }
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]);
};
class String : public IndexBase
{
public:
    String() { }
    String(int index) : IndexBase(index) { }
    virtual int GetShift() { return 0; }
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]);
};
class US : public IndexBase
{
public:
    US() { }
    US(int index) : IndexBase(index) { }
    virtual int GetShift() { return 0; }
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]);
};
class GUID : public IndexBase
{
public:
    GUID() { }
    GUID(int index) : IndexBase(index) { }
    virtual int GetShift() { return 0; }
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]);
};

class Blob : public IndexBase
{
public:
    Blob() { }
    Blob(int index) : IndexBase(index) { }
    virtual int GetShift() { return 0; }
    virtual bool GetOverflow(size_t sizes[MaxTables + 4]);
};
class TableEntryBase
{
public:
    virtual int Index() = 0;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *) = 0;
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *) = 0;
};
class ModuleTableEntry : public TableEntryBase
{
public:
    ModuleTableEntry() { }
    ModuleTableEntry(size_t NameIndex, size_t GuidIndex) : nameIndex(NameIndex), guidIndex(GuidIndex) { }
    virtual int Index() { return tModule; }
    String nameIndex;
    GUID guidIndex;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
class TypeRefTableEntry : public TableEntryBase
{
public:
    TypeRefTableEntry() { }
    TypeRefTableEntry(ResolutionScope Resolution, size_t TypeNameIndex, size_t TypeNameSpaceIndex) :
        resolution(Resolution), typeNameIndex(TypeNameIndex), typeNameSpaceIndex(typeNameSpaceIndex) { }
    virtual int Index() { return tTypeRef; }
    ResolutionScope resolution;
    String typeNameIndex;
    String typeNameSpaceIndex;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
class TypeDefTableEntry : public TableEntryBase
{
public:
    enum Flags
    {

        // visibility
        VisibilityMask        =   0x00000007,

        NotPublic             =   0x00000000,     
        Public                =   0x00000001,     

        NestedPublic          =   0x00000002,     
        NestedPrivate         =   0x00000003,     
        NestedFamily          =   0x00000004,     
        NestedAssembly        =   0x00000005,     
        NestedFamANDAssem     =   0x00000006,     
        NestedFamORAssem      =   0x00000007,     

        // layout 
        LayoutMask            =   0x00000018,

        AutoLayout            =   0x00000000,     
        SequentialLayout      =   0x00000008,     
        ExplicitLayout        =   0x00000010,     

        // semantics
        ClassSemanticsMask    =   0x00000060,
    
        Class                 =   0x00000000,     
        Interface             =   0x00000020,     

        // other attributes
        Abstract              =   0x00000080,     
        Sealed                =   0x00000100,     
        SpecialName           =   0x00000400,     
        Import                =   0x00001000,     
        Serializable          =   0x00002000,     

        // string format
        StringFormatMask      =   0x00030000,

        AnsiClass             =   0x00000000,     
        UnicodeClass          =   0x00010000,     
        AutoClass             =   0x00020000,     
        CustomFormatClass     =   0x00030000,     

        // valid for custom format class, but undefined
        CustomFormatMask      =   0x00C00000,     


        BeforeFieldInit       =   0x00100000,     
        Forwarder             =   0x00200000,     

        // runtime
        ReservedMask          =   0x00040800,
        RTSpecialName         =   0x00000800,     

        HasSecurity           =   0x00040000,     
    };
    TypeDefTableEntry() : flags(0) { }
    TypeDefTableEntry(int Flags, size_t TypeNameIndex, size_t TypeNameSpaceIndex,
                     TypeDefOrRef Extends, size_t FieldIndex, size_t MethodIndex) :
        flags(Flags), typeNameIndex(TypeNameIndex), typeNameSpaceIndex(typeNameSpaceIndex),
        extends(Extends), fields(FieldIndex), methods(MethodIndex) { }
    virtual int Index() { return tTypeDef; }
    enum Flags flags;
    String typeNameIndex;
    String typeNameSpaceIndex;
    TypeDefOrRef extends;
    FieldList fields;
    MethodList methods;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};

class FieldTableEntry : public TableEntryBase
{
public:
    enum Flags
    {
        FieldAccessMask           =   0x0007,

        PrivateScope              =   0x0000,     
        Private                   =   0x0001,     
        FamANDAssem               =   0x0002,     
        Assembly                  =   0x0003,     
        Family                    =   0x0004,     
        FamORAssem                =   0x0005,     
        Public                    =   0x0006,     

        // other attribs
        Static                    =   0x0010,     
        InitOnly                  =   0x0020,     
        Literal                   =   0x0040,     
        NotSerialized             =   0x0080,     
        SpecialName               =   0x0200,     

        // pinvoke    
        PinvokeImpl               =   0x2000,     

        // runtime
        ReservedMask              =   0x9500,
        RTSpecialName             =   0x0400,     
        HasFieldMarshal           =   0x1000,         
        HasDefault                =   0x8000,     
        HasFieldRVA               =   0x0100,     
    };
    FieldTableEntry() : flags(0) { }
    FieldTableEntry(int Flags, size_t NameIndex, size_t SignatureIndex) :
        flags(Flags), nameIndex(NameIndex), signatureIndex(SignatureIndex) { }
    virtual int Index() { return tField; }
    enum Flags flags;
    String nameIndex;
    Blob signatureIndex;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
class MethodDefTableEntry : public TableEntryBase
{
public:
    enum ImplFlags
    {
        CodeTypeMask      =   0x0003,   // Flags about code type.
        IL                =   0x0000,   // Method impl is IL.
        Native            =   0x0001,   // Method impl is native.
        OPTIL             =   0x0002,   // Method impl is OPTIL
        Runtime           =   0x0003,   // Method impl is provided by the runtime.
        ManagedMask       =   0x0004,   // Flags specifying whether the code is managed
                                          // or unmanaged.
        Unmanaged         =   0x0004,   // Method impl is unmanaged, otherwise managed.
        Managed           =   0x0000,   // Method impl is managed.

        ForwardRef        =   0x0010,   // Indicates method is defined; used primarily
                                          // in merge scenarios.
        PreserveSig       =   0x0080,   // Indicates method sig is not to be mangled to
                                          // do HRESULT conversion.
    
        InternalCall      =   0x1000,   // Reserved for internal use.
    
        Synchronized      =   0x0020,   // Method is single threaded through the body.
        NoInlining        =   0x0008,   // Method may not be inlined.
        MaxMethodImplVal  =   0xffff,   // Range check value
    };
    enum Flags
    {

        MemberAccessMask          =   0x0007,
        PrivateScope              =   0x0000,     
        Private                   =   0x0001,         
        FamANDAssem               =   0x0002,     
        Assem                     =   0x0003,     
        Family                    =   0x0004,     
        FamORAssem                =   0x0005,     
        Public                    =   0x0006,     
    
        Static                    =   0x0010,         
        Final                     =   0x0020,         
        Virtual                   =   0x0040,     
        HideBySig                 =   0x0080,     
    
        VtableLayoutMask          =   0x0100,

        ReuseSlot                 =   0x0000,     // The default.
        NewSlot                   =   0x0100,     

        // implementation attribs    
        CheckAccessOnOverride     =   0x0200,     
        Abstract                  =   0x0400,     
        SpecialName               =   0x0800,     
    
    
        PinvokeImpl               =   0x2000,     
    
        UnmanagedExport           =   0x0008,         
    
           // Reserved flags for runtime use only.
    
        ReservedMask              =   0xd000,
        RTSpecialName             =   0x1000,     
    
        HasSecurity               =   0x4000,         
        RequireSecObject          =   0x8000,     
    };
    MethodDefTableEntry() : implFlags(0), flags(0) { }
    MethodDefTableEntry(Method *method, int IFlags, int MFlags, size_t NameIndex,
                       size_t SignatureIndex, size_t ParamIndex) : implFlags(IFlags),
                       flags(MFlags), nameIndex(NameIndex), signatureIndex(SignatureIndex),
                       paramIndex(ParamIndex) { }
    virtual int Index() { return tMethodDef; }
    Method *method ; // for rva
    enum ImplFlags implFlags;
    enum Flags flags;
    String nameIndex;
    Blob signatureIndex;
    ParamList paramIndex;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
class ParamTableEntry : public TableEntryBase
{

public:
    enum Flags
    {
        In                        =   0x0001,     // Param is [In]    
        Out                       =   0x0002,     // Param is [out]
        Optional                  =   0x0010,     // Param is optional
    
    
        // runtime attribs
    
        ReservedMask              =   0xf000,
        HasDefault                =   0x1000,     // Param has default value.
    
        HasFieldMarshal           =   0x2000,     // Param has FieldMarshal.
        Unused                    =   0xcfe0,
    };
    ParamTableEntry() : flags(0), sequenceIndex(0) { }
    ParamTableEntry(int Flags, unsigned short SequenceIndex, size_t NameIndex) :
        flags(Flags), sequenceIndex(SequenceIndex), nameIndex(NameIndex) { }
    virtual int Index() { return tParam; }
    enum Flags flags;
    unsigned short sequenceIndex;
    String nameIndex;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
class MemberRefTableEntry : public TableEntryBase
{
public:
    MemberRefTableEntry() { }
    MemberRefTableEntry(MemberRefParent ParentIndex, size_t NameIndex, size_t SignatureIndex)
        : parentIndex(ParentIndex), nameIndex(NameIndex), signatureIndex(SignatureIndex) { }
    virtual int Index() { return tMemberRef; }
    MemberRefParent parentIndex;
    String nameIndex;
    Blob signatureIndex;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
class ConstantTableEntry : public TableEntryBase
{
public:
    ConstantTableEntry() : type(0) { }
    ConstantTableEntry(int Type, Constant ParentIndex, size_t ValueIndex) :
        type(Type), parentIndex(ParentIndex), valueIndex(ValueIndex) { }
    virtual int Index() { return tConstant; }
    unsigned char type;
    Constant parentIndex;
    Blob valueIndex;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
class CustomAttributeTableEntry : public TableEntryBase
{
public: 
    CustomAttributeTableEntry() { }
    CustomAttributeTableEntry(CustomAttribute ParentIndex, CustomAttributeType TypeIndex, size_t ValueIndex)
        : parentIndex(ParentIndex), typeIndex(TypeIndex), valueIndex(ValueIndex) { }
    virtual int Index() { return tCustomAttribute; }
    CustomAttribute parentIndex;
    CustomAttributeType typeIndex;
    Blob valueIndex;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
class ClassLayoutTableEntry : public TableEntryBase
{
public:
    ClassLayoutTableEntry() : pack(1), size(1) { }
    ClassLayoutTableEntry(unsigned short Pack, size_t Size, size_t Parent)
        : pack(Pack), size(Size), parent(Parent) { }
    virtual int Index() { return tClassLayout; }
    unsigned short pack ;
    size_t size;
    TypeDef parent;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
class FieldLayoutTableEntry : public TableEntryBase
{
public:
    FieldLayoutTableEntry() : offset(0) { }
    FieldLayoutTableEntry(size_t Offset, size_t Parent) : offset(Offset), parent(Parent) { } 
    virtual int Index() { return tFieldLayout; }
    size_t offset;
    FieldList parent;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
class StandaloneSigTableEntry : public TableEntryBase
{
public:
    StandaloneSigTableEntry() { }
    StandaloneSigTableEntry(size_t SignatureIndex) : signatureIndex(SignatureIndex) { }
    virtual int Index() { return tStandaloneSig; }
    Blob signatureIndex;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
class ModuleRefTableEntry : public TableEntryBase
{
public:
    ModuleRefTableEntry() { }
    ModuleRefTableEntry(size_t NameIndex) : nameIndex(NameIndex) { }
    String nameIndex;
    virtual int Index() { return tModuleRef; }
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
class TypeSpecTableEntry : public TableEntryBase
{
public:
    TypeSpecTableEntry() { }
    TypeSpecTableEntry(size_t SignatureIndex) : signatureIndex(SignatureIndex) { }
    virtual int Index() { return tTypeSpec; }
    Blob signatureIndex;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
class ImplMapTableEntry : public TableEntryBase
{
public:
    enum Flags
    {
        NoMangle          = 0x0001,   // use the member name as specified
        
        CharSetMask       = 0x0006,
        CharSetNotSpec    = 0x0000,
        CharSetAnsi       = 0x0002,
        CharSetUnicode    = 0x0004,
        CharSetAuto       = 0x0006,
    
        BestFitUseAssem   = 0x0000,
        BestFitEnabled    = 0x0010,
        BestFitDisabled   = 0x0020,
        BestFitMask       = 0x0030,
    
        ThrowOnUnmappableCharUseAssem   = 0x0000,
        ThrowOnUnmappableCharEnabled    = 0x1000,
        ThrowOnUnmappableCharDisabled   = 0x2000,
        ThrowOnUnmappableCharMask       = 0x3000,
    
        SupportsLastError = 0x0040,   
            
        CallConvMask      = 0x0700,
        CallConvWinapi    = 0x0100,   // Pinvoke will use native callconv appropriate to target windows platform.    
        CallConvCdecl     = 0x0200,
        CallConvStdcall   = 0x0300,
        CallConvThiscall  = 0x0400,   // In M9, pinvoke will raise exception.
    
        CallConvFastcall  = 0x0500,
    
        MaxValue          = 0xFFFF,
    };
    ImplMapTableEntry() : flags(0) { }
    ImplMapTableEntry(int Flags, MemberForwarded MethodIndex, size_t ImportNameIndex, size_t ModuleIndex)
        : flags(Flags), methodIndex(MethodIndex), importNameIndex(ImportNameIndex), moduleIndex(ModuleIndex) { }
    virtual int Index() { return tImplMap; }
    enum Flags flags;
    MemberForwarded methodIndex;
    String importNameIndex;
    ModuleRef moduleIndex;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
class FieldRVATableEntry : public TableEntryBase
{
public:
    FieldRVATableEntry() : rva(NULL) { }
    FieldRVATableEntry(size_t Rva, size_t FieldIndex) : rva(Rva), fieldIndex(FieldIndex) { }
    virtual int Index() { return tFieldRVA; }
    size_t rva;
    FieldList fieldIndex;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
enum AssemblyFlags
{
    PublicKey             =   0x0001,      // full key
    PA_None               =   0x0000,     
    PA_MSIL               =   0x0010,     
    PA_x86                =   0x0020,     
    PA_IA64               =   0x0030,     
    PA_AMD64              =   0x0040,     
    PA_Specified          =   0x0080,     

    PA_Mask               =   0x0070,         
    PA_FullMask           =   0x00F0,     

    PA_Shift              =   0x0004,     // shift count


    EnableJITcompileTracking  =   0x8000, // From "DebuggableAttribute".    
    DisableJITcompileOptimizer=   0x4000, // From "DebuggableAttribute".
    
    Retargetable          =   0x0100,     
};
class AssemblyDefTableEntry : public TableEntryBase
{
public:
    enum
    {
        DefaultHashAlgId = 0x8004
    };
    AssemblyDefTableEntry() : hashAlgId(DefaultHashAlgId), major(0), minor(0), build(0), revision(0), flags(0) { }
    AssemblyDefTableEntry(int Flags, unsigned short Major, unsigned short Minor, unsigned short Build, unsigned short Revision,
                         size_t NameIndex) : hashAlgId(DefaultHashAlgId), flags(Flags),
                            major(Major), minor(Minor), build(Build), revision(Revision),
                            nameIndex(NameIndex) { }
    virtual int Index() { return tAssemblyDef; }
    unsigned short hashAlgId;
    unsigned short major, minor, build, revision;
    enum AssemblyFlags flags;
    Blob publicKeyIndex;
    String nameIndex;
    String cultureIndex;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
class AssemblyRefTableEntry : public TableEntryBase
{
public:
    AssemblyRefTableEntry() : major(0), minor(0), build(0), revision(0), flags(0) { }
    AssemblyRefTableEntry(int Flags, unsigned short Major, unsigned short Minor, unsigned short Build, unsigned short Revision,
                         size_t NameIndex) : flags(Flags),
                            major(Major), minor(Minor), build(Build), revision(Revision),
                            nameIndex(NameIndex) { }
    virtual int Index() { return tAssemblyRef; }
    unsigned short major, minor, build, revision;
    enum AssemblyFlags flags;
    Blob publicKeyIndex;
    String nameIndex;
    String cultureIndex;
    Blob hashIndex;    
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};
class NestedClassTableEntry : public TableEntryBase
{
public:
    NestedClassTableEntry() { }
    NestedClassTableEntry(size_t nested, size_t enclosing) : nestedIndex(nested), enclosingIndex(enclosing) { }
    virtual int Index() { return tNestedClass; }
    TypeDef nestedIndex;
    TypeDef enclosingIndex;
    virtual size_t Render(size_t sizes[MaxTables + 4], unsigned char *);
    virtual size_t Get(size_t sizes[MaxTables + 4], unsigned char *);
};

class PEMethod
{
public:
    enum Flags
    {
        TinyFormat = 2, // no local variables, MAXstack <=8, size < 64;
        FatFormat = 3,
        // more flags only availble for FAT format
        MoreSects = 8,
        InitLocals = 0x10,

        EntryPoint = 0x8000 // not a real flag that goes in the PE file
    };
    PEMethod(int Flags, size_t MethodDef, int MaxStack, int localCount, int CodeSize, size_t signature) 
        : flags(Flags), hdrSize(3), maxStack(MaxStack), codeSize(CodeSize), code(NULL) , signatureToken(signature), rva(0), methodDef(MethodDef)
    {
        if ((flags &0x7fff) == 0)
            if (MaxStack <=8 && codeSize < 64 & localCount == 0)
            {
                flags |= TinyFormat;
            }
            else
            {
                flags |= FatFormat;
            }
    }
    enum Flags flags;
    int hdrSize; /* = 3 */
    unsigned short maxStack;
    size_t codeSize;
    unsigned char *code;
    Blob signatureToken;
    int rva;
    size_t methodDef;
    bool WriteHeader();
    bool WriteBody();
};

} // namespace