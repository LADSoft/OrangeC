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

#include <vector>
#include <string>
#include <set>
#include "RSAEncoder.h"
#include "sha1.h"
// this is an internal header used to define the various aspects of a PE file
// and the functions to render the files
// it won't normally be much use to users of the DotNetPELib Library
//
// There are five basic types of entities in this file
//
// Tables are the metadata tables; each metadatatable we support has an associated
// TableEntry structure that defines rows in the tables.
//
// Often the metadata rows have indexes to other tables, or to offsets in one of the
// other streams - low bits in the indexes often indicate what table the index
// references and also the indexes can 'grow' from 16 to 32 bits for huge files
// so there are index structures for each type of index
//
// There is also support for inserting data in the streams
//
// There is also an object for defining method bodies
//
// Finally, a PEWriter object combines data from all the other objects to
// create the EXE or DLL image.
//
namespace DotNetPELib {

    // forward refs

    class TableEntryBase;
    class Method;
    class Field;
    class Type;
    class PELib;

    class PEMethod;
    class AssemblyRefTableEntry;

    // vector of tables that can appear in a PE file
    // empty tables are elided
    typedef std::vector<TableEntryBase *> DNLTable;

    // constants related to the tables in the PE file
    enum
    {
        MaxTables = 64,
        // the following are after the tables indexes, these are used to
        // allow figuring out the size of indexes to 'special' streams
        // generally if the stream is > 65535 bytes the index fits in a 32 bit DWORD
        // otherwise the index fits into a 16 bit WORD
        ExtraIndexes = 4,
        tString = 64,
        tUS = 65,
        tGUID = 66,
        tBlob = 67

    };
    // these are the indexes to the tables.   They are in order as they will appear
    // in the PE file (if they do appear in the PE file)
    enum Tables {
        tModule = 0,// Assembly def
        tTypeRef = 1,// References to other assemblies
        tTypeDef = 2,// definitions of classes and enumerations
        tField = 4,// definitions of fields
        tMethodDef = 6,// definitions of methods, includes both managed and unmanaged
        tParam = 8,// definitions of parameters
        tInterfaceImpl = 9,
        tMemberRef = 10,// references to external methods, also the call site references for vararg-style pinvokes
        tConstant = 11,// initialization constants, we use them for enumerations but that is about it
        tCustomAttribute = 12, // custom attributes, we use it for C# style varargs but nothing else
        tFieldMarshal = 13,
        tDeclSecurity = 14, // we don't use it
        tClassLayout = 15,// size, packing for classes
        tFieldLayout = 16, // field offsets, we don't use it
        tStandaloneSig = 17, //?? we don't use it
        tEventMap  = 18,
        tEvent  = 20,
        tPropertyMap = 21,
        tProperty = 23,
        tMethodSemantics = 24,
        tMethodImpl = 25,
        tModuleRef = 26, // references to external modules
        tTypeSpec = 27,// we use it for referenced types not found in the typedef table
        tImplMap = 28,// pinvoke DLL information
        tFieldRVA = 29,// cildata RVAs for field initialized data
        tAssemblyDef = 32,// our main assembly
        tAssemblyRef = 35,// any external assemblies
        tFile = 38,
        tExportedType = 39,
        tManifestResource = 40,
        tNestedClass = 41,// list of nested classes and their parents
        tGenericParam = 42,
        tMethodSpec = 43,
        tGenericParamConstraint =44

    };

    // this is naively ignoring the various CIL tables that aren't supposed to be in the file
    // may have to revisit that at some point.
    static const ulonglong tKnownTablesMask = 
        (1ULL << tModule) | (1ULL << tTypeRef) | (1ULL << tTypeDef) | (1ULL << tField) |
        (1ULL << tMethodDef) | (1ULL << tParam) | (1ULL << tInterfaceImpl) | (1ULL << tMemberRef) | (1ULL << tConstant) |
        (1ULL << tCustomAttribute) | (1ULL << tFieldMarshal) | (1ULL << tDeclSecurity) | (1ULL << tClassLayout) | (1ULL << tFieldLayout) |
        (1ULL << tStandaloneSig) | (1ULL << tEventMap) | (1ULL << tEvent) | (1ULL << tPropertyMap) | 
        (1ULL << tProperty) | (1ULL << tMethodSemantics) | (1ULL << tMethodImpl) |
        (1ULL << tModuleRef) | (1ULL << tTypeSpec) | (1ULL << tImplMap) |
        (1ULL << tFieldRVA) | (1ULL << tAssemblyDef) | (1ULL << tAssemblyRef) | 
        (1ULL << tFile) | (1ULL << tExportedType) | (1ULL << tManifestResource) |
        (1ULL << tNestedClass) | (1ULL << tGenericParam) | (1ULL << tMethodSpec) | (1ULL << tGenericParamConstraint) ;
    // these are standard type identifiers uses in signatures
    enum Types
    {
        ELEMENT_TYPE_END = 0x0,

        ELEMENT_TYPE_VOID = 0x1,
        ELEMENT_TYPE_bool = 0x2,
        ELEMENT_TYPE_CHAR = 0x3,
        ELEMENT_TYPE_I1 = 0x4,
        ELEMENT_TYPE_U1 = 0x5,

        ELEMENT_TYPE_I2 = 0x6,
        ELEMENT_TYPE_U2 = 0x7,
        ELEMENT_TYPE_I4 = 0x8,
        ELEMENT_TYPE_U4 = 0x9,
        ELEMENT_TYPE_I8 = 0xa,

        ELEMENT_TYPE_U8 = 0xb,
        ELEMENT_TYPE_R4 = 0xc,
        ELEMENT_TYPE_R8 = 0xd,
        ELEMENT_TYPE_STRING = 0xe,

        // every type above PTR will be simple type


        ELEMENT_TYPE_PTR = 0xf,      // PTR <type>

        ELEMENT_TYPE_BYREF = 0x10,     // BYREF <type>


        // Please use ELEMENT_TYPE_VALUETYPE.

        // ELEMENT_TYPE_VALUECLASS is deprecated.

        ELEMENT_TYPE_VALUETYPE = 0x11,  // VALUETYPE <class Token>

        ELEMENT_TYPE_CLASS = 0x12,  // CLASS <class Token>

        ELEMENT_TYPE_VAR = 0x13,  // a class type variable VAR <U1>

        // MDARRAY <type> <rank> <bcount>

        // <bound1> ... <lbcount> <lb1> ...

        ELEMENT_TYPE_ARRAY = 0x14,
        // GENERICINST <generic type> <argCnt> <arg1> ... <argn>

        ELEMENT_TYPE_GENERICINST = 0x15,
        // TYPEDREF  (it takes no args) a typed referece to some other type

        ELEMENT_TYPE_TYPEDBYREF = 0x16,

        // native integer size

        ELEMENT_TYPE_I = 0x18,

        // native DWord integer size

        ELEMENT_TYPE_U = 0x19,
        // FNPTR <complete sig for the function

        // including calling convention>

        ELEMENT_TYPE_FNPTR = 0x1B,

        // Shortcut for System.Object

        ELEMENT_TYPE_OBJECT = 0x1C,
        // Shortcut for single dimension zero lower bound array

        // SZARRAY <type>

        ELEMENT_TYPE_SZARRAY = 0x1D,

        // a method type variable MVAR <U1>

        ELEMENT_TYPE_MVAR = 0x1e,

        // This is only for binding

        // required C modifier : E_T_CMOD_REQD <mdTypeRef/mdTypeDef>

        ELEMENT_TYPE_CMOD_REQD = 0x1F,

        ELEMENT_TYPE_CMOD_OPT = 0x20,
        // optional C modifier : E_T_CMOD_OPT <mdTypeRef/mdTypeDef>


        // This is for signatures generated internally

        // (which will not be persisted in any way).

        ELEMENT_TYPE_INTERNAL = 0x21,

        // INTERNAL <typehandle>


        // Note that this is the max of base type excluding modifiers

        ELEMENT_TYPE_MAX = 0x22,
        // first invalid element type



        ELEMENT_TYPE_MODIFIER = 0x40,

        // sentinel for varargs

        ELEMENT_TYPE_SENTINEL = 0x01 | ELEMENT_TYPE_MODIFIER,
        ELEMENT_TYPE_PINNED = 0x05 | ELEMENT_TYPE_MODIFIER,
        // used only internally for R4 HFA types

        ELEMENT_TYPE_R4_HFA = 0x06 | ELEMENT_TYPE_MODIFIER,
        // used only internally for R8 HFA types


        ELEMENT_TYPE_R8_HFA = 0x07 | ELEMENT_TYPE_MODIFIER,

    };

    // This class holds functions for generating the various signatures we need
    // to put in the blob stream
    class SignatureGenerator
    {
        // this implementation isn't completely thread safe - it uses the equivalent the
        // equivalent of a global variable to keep track of state
    public:
        static Byte *MethodDefSig(MethodSignature *signature, size_t &sz);
        static Byte *MethodRefSig(MethodSignature *signature, size_t &sz);
        static Byte *MethodSpecSig(MethodSignature *signature, size_t &sz);
        static Byte *PropertySig(Property *property, size_t &sz);
        static Byte *FieldSig(Field *field, size_t &sz);
        //Byte *PropertySig(Property *property);
        static Byte *LocalVarSig(Method *method, size_t &sz);
        static Byte *TypeSig(Type *type, size_t &sz);

        // end of signature generators, this function is a generic function to embed a type
        // inito a signature
        static size_t EmbedType(int *buf, int offset, Type *tp);
        // this function converts a signature buffer to a blob entry, by compressing
        // the integer values in the signature
        static Byte *ConvertToBlob(int *buf, int size, size_t &sz);

        // this function sets the index for the 'object' class entry
        static void SetObjectType(size_t ObjectBase) { objectBase = ObjectBase; }


        // get a type from a methodref
        static void TypeFromMethodRef(PELib& lib, AssemblyDef &assembly, PEReader &reader, MethodSignature *sig, size_t blobIndex);
        // get a type from a fieldref
        static void TypeFromFieldRef(PELib& lib, AssemblyDef &assembly, PEReader &reader, Field *field, size_t blobIndex);
        // get a type from a propertyref
        static void TypeFromPropertyRef(PELib& lib, AssemblyDef &assembly, PEReader &reader, Property *property, size_t blobIndex);
        // get a type from a signature
        static Type *GetType(PELib& lib, AssemblyDef &assembly, PEReader &reader, Byte *data, size_t &pos, size_t &len);
    private:
        // a shared function for the various signatures that put in method signatures
        static size_t CoreMethod(MethodSignature *method, int paramCount, int *buf, int offset);
        static size_t LoadIndex(Byte *buf, size_t &start, size_t &len);
        static Type *TypeFromTypeRef(PELib& lib, AssemblyDef &assembly, PEReader &reader, size_t index, int pointerLevel);
        static Type *BasicType(PELib& lib, int typeIndex, int pointerLevel);
        static void TypeFromMethod(PELib& lib, AssemblyDef &assembly, PEReader &reader, MethodSignature *method, Byte *data, size_t &start, size_t &len);
        static std::string LoadClassName(PEReader &reader, size_t index, AssemblyRefTableEntry **assembly, bool isDef);
        static std::string LoadNameSpaceName(PEReader &reader, size_t index, bool isDef);
        static int workArea[400 * 1024];
        static int basicTypes[];
        static int objectBase;
    };
    // This is the main class for generating a PE file
    class PEWriter
    {
    public:
        // the maximum number of PE objects we will generate
        // this includes the following:
        //   .text / cildata
        //   .reloc (for the single necessary reloc entry)
        //   .rsrc (not implemented yet, will hold version info record)
        enum { MAX_PE_OBJECTS = 4 };

        // Constructor to instantiate class
        PEWriter(bool isexe, bool gui, const std::string& snkFile) : DLL_(!isexe), GUI_(gui), objectBase_(0), valueBase_(0), enumBase_(0),
            systemIndex_(0), entryPoint_(0), paramAttributeType_(0), paramAttributeData_(0),
            fileAlign_(0x200), objectAlign_(0x2000), imageBase_(0x400000), language_(0x4b0),
            peHeader_(nullptr), peObjects_(nullptr), cor20Header_(nullptr), tablesHeader_(nullptr),
            snkFile_(snkFile), snkLen_(0), outputFile_(nullptr), peBase_(0), corBase_(0), snkBase_(0) { }
        virtual ~PEWriter();
        // add an entry to one of the tables
        // note the data for the table will be a class inherited from TableEntryBase,
        // and this class will self-report the table index to use
        size_t AddTableEntry(TableEntryBase *entry);
        // add a method entry to the output list.  Note that Index_(D methods won't be added here.
        void AddMethod(PEMethod *method);
        // various functions to throw things into one of the streams, they return the stream index
        size_t HashString(const std::string& utf8);
        size_t HashUS(std::wstring str);
        size_t HashGUID(Byte *Guid);
        size_t HashBlob(Byte *blobData, size_t blobLen);
        // this is the 'cildata' contents.   Again we emit into the cildata and it returns the offset in
        // the cildata to use.  It does NOT return the rva immediately, that is calculated later
        size_t RVABytes(Byte *bytes, size_t data);

        // Set the indexes of the various classes which can be extended to make new classes
        // these are typically in the typeref table
        // Also set the index of the System namespace entry which is t
        void SetBaseClasses(size_t ObjectIndex, size_t ValueIndex, size_t EnumIndex, size_t SystemIndex)
        {
            SignatureGenerator::SetObjectType(ObjectIndex);
            objectBase_ = ObjectIndex;
            valueBase_ = ValueIndex;
            enumBase_ = EnumIndex;
            systemIndex_ = SystemIndex;
        }
        // this sets the data for the paramater attribute we support
        // we aren't generally supporting attributes in this version but we do need to be able to
        // set a single attribute that means a function has a variable length argument list
        void ParamAttribute(size_t paramAttributeType, size_t paramAttributeData)
        {
            paramAttributeType_ = paramAttributeType;
            paramAttributeData_ = paramAttributeData;
        }

        size_t ObjectBaseClass() const { return objectBase_; }
        size_t ValueBaseClass() const { return valueBase_; }
        size_t EnumBaseClass() const { return enumBase_; }
        size_t SystemName() const { return systemIndex_; }

        size_t ParamAttributeType() const { return paramAttributeType_; }
        size_t ParamAttributeData() const { return paramAttributeData_; }

        static void CreateGuid(Byte *Guid);

        size_t NextTableIndex(int table) const { return tables_[table].size() + 1; }
        bool WriteFile(PELib &peLib, std::iostream &out);
        void HashPartOfFile(SHA1Context &context, size_t offset, size_t len);

        // another thing that makes this lib not thread safe, the RVA for 
        // the beginning of the .data section gets put here after it is calculated
        static DWord cildata_rva_;
    protected:
        // this calculates various addresses and offsets that will be used and referenced
        // when we actually generate the data.   This must be kept in sync with the code to
        // generate data
        void CalculateObjects(PELib &peLib);
        // These functions put various information into the PE file
        bool WriteMZData(PELib &peLib) const; //
        bool WritePEHeader(PELib &peLib);//
        bool WritePEObjects(PELib &peLib) const;//
        bool WriteIAT(PELib &peLib) const;
        bool WriteCoreHeader(PELib &peLib);//
        bool WriteHashData(PELib &peLib);
        bool WriteStaticData(PELib &peLib) const;
        bool WriteMethods(PELib &peLib) const;
        bool WriteMetadataHeaders(PELib &peLib) const;//
        bool WriteTables(PELib &peLib) const;
        bool WriteStrings(PELib &peLib) const;
        bool WriteUS(PELib &peLib) const;
        bool WriteGUID(PELib &peLib) const;
        bool WriteBlob(PELib &peLib) const;
        bool WriteImports(PELib &peLib) const;
        bool WriteEntryPoint(PELib &peLib) const;

        bool WriteVersionInfo(PELib &peLib) const;
        bool WriteRelocs(PELib &peLib) const;

        // a helper to put a string into the string area of the version information
        void VersionString(const wchar_t *name, const char *value) const;

        // Various helpers to put data to the output
        void put(const void *data, size_t size) const { outputFile_->write((char *)data, size); }
        std::streamoff offset() const { return outputFile_->tellp(); }
        void seek(size_t offset) const { outputFile_->seekp(offset); }
        void align(size_t offset) const;
    private:
        std::iostream *outputFile_;
        std::string snkFile_;
        // a reflection of the String stream so that we can keep from doing duplicates.
        // right now we don't check duplicates on any of the other streams...
        std::map<std::string, size_t> stringMap_;
        struct pool
        {
            pool() : size(0), maxSize(200), base(nullptr) { base = (Byte *)calloc(1, maxSize); }
            ~pool() { free(base); }
            size_t size;
            size_t maxSize;
            Byte *base;
            void Ensure(size_t newSize);
        };
        DNLTable tables_[MaxTables];
        size_t entryPoint_;
        std::list<PEMethod *>methods_;
        size_t objectBase_;
        size_t valueBase_;
        size_t enumBase_;
        size_t systemIndex_;
        size_t paramAttributeType_, paramAttributeData_;
        bool DLL_;
        bool GUI_;
        pool strings_;
        pool us_;
        pool blob_;
        pool guid_;
        pool rva_;
        size_t fileAlign_;
        size_t objectAlign_;
        size_t imageBase_;
        DWord language_;
        Word assemblyVersion_[4];
        Word fileVersion_[4];
        Word productVersion_[4];
        struct PEHeader *peHeader_;
        struct PEObject *peObjects_;
        struct DotNetCOR20Header *cor20Header_;
        struct DotNetMetaTablesHeader *tablesHeader_;
        size_t streamHeaders_[5][2];
        size_t snkLen_;
	    unsigned peBase_;
	    unsigned corBase_;
	    unsigned snkBase_;
        RSAEncoder rsaEncoder;
        static Byte MZHeader_[];
        static struct DotNetMetaHeader *metaHeader_;
        static const char *streamNames_[];
        static Byte defaultUS_[];
    };

    class TableEntryBase;
    class TableEntryFactory
    {
        public:
            static TableEntryBase *GetEntry(size_t index);
    };
    class PEReader
    {
    public:
        static const int ERR_FILE_NOT_FOUND = 1;
        static const int ERR_NOT_PE = 2;
        static const int ERR_NOT_ASSEMBLY = 3;
        static const int ERR_INVALID_ASSEMBLY = 3;
        static const int ERR_UNKNOWN_TABLE = 5;

        PEReader() : inputFile_(nullptr), corRVA_(0), num_objects_(0), objects_(0), blobPos_(0), stringPos_(0), GUIDPos_(0), stringData_(nullptr), blobData_(nullptr) { }
        virtual ~PEReader();

        int ManagedLoad(std::string assemblyName, int major, int minor, int build, int revision);
        int ReadFromString(Byte *buf, size_t len, size_t offset);
        int ReadFromBlob(Byte *buf, size_t len, size_t offset);
        int ReadFromGUID(Byte *buf, size_t len, size_t offset);
        size_t RVAToFileLocation(size_t rva);
        const DNLTable &Table(int i) const { return tables_[i]; }
        void LibPath(const std::string& libPath) { libPath_ = libPath;  }

    protected:
        std::string SearchOnPath(const std::string& assemblyName);
        std::string FindGACPath(const std::string& path, const std::string& fileName, int major, int minor, int build, int revision);
        std::string SearchForManagedFile(const std::string& assemblyName, int major, int minor, int build, int revision);
        void get(void *buffer, size_t offset, size_t len);
        size_t PELocation();
        size_t Cor20Location(size_t PEHeader);
        void GetStream(size_t Cor20, const char *streamName, DWord pos[2]);
        int ReadTables(size_t Cor20);

    private:
        std::fstream *inputFile_;
        int num_objects_;
        size_t corRVA_;
        size_t blobPos_;
        size_t stringPos_;
        Byte *stringData_;
        Byte *blobData_;
        size_t GUIDPos_;
        PEObject *objects_;
        DNLTable tables_[MaxTables];
        size_t sizes_[MaxTables + ExtraIndexes];
        std::string libPath_;
    };
    // this class is the base class for index rendering
    // it defines a tag type (which indicates which table the index belongs with) and an index value
    // Based on the specific type of index being rendered, the index is shifted left by a constant and
    // the tag is added in the lower bits.
    // Note that these indexes are used in tables and also in the blobs, however, in the actual intermediate
    // code a token is used.  the index in the is 24 bits unshifted, bits 24-31 holding the table number

    class IndexBase
    {
    public:
        IndexBase() : tag_(0), index_(0) { }
        IndexBase(size_t Index) : tag_(0), index_(Index) { }
        IndexBase(int Tag, size_t Index) : tag_(Tag), index_(Index) { }

        size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const;
        size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *);
        virtual int GetIndexShift() const = 0;
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const = 0;

        bool Large(unsigned x) const { return (x << GetIndexShift()) > 0xffff; }

        int tag_;
        size_t index_;
    };

    // the next group of classes defines all the possible index types that occur in the tables we are
    // interested in
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
        virtual int GetIndexShift() const override { return TagBits; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
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
        virtual int GetIndexShift() const override { return TagBits; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    class TypeOrMethodDef : public IndexBase
    {
    public:
        TypeOrMethodDef() { }
        TypeOrMethodDef(int tag, int index) : IndexBase(tag, index) { }
        enum Tags
        {
            TagBits = 1,
            TypeDef = 0,
            MethodDef = 1,
        };
        virtual int GetIndexShift() const override { return TagBits; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    class MethodDefOrRef : public IndexBase
    {
    public:
        MethodDefOrRef() { }
        MethodDefOrRef(int tag, int index) : IndexBase(tag, index) { }
        enum Tags
        {
            TagBits = 1,
            MethodDef = 0,
            MemberRef = 1,
        };
        virtual int GetIndexShift() const override { return TagBits; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
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
        virtual int GetIndexShift() const override { return TagBits; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
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
        virtual int GetIndexShift() const override { return TagBits; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
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
        virtual int GetIndexShift() const override { return TagBits; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
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
        virtual int GetIndexShift() const override { return TagBits; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
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
        virtual int GetIndexShift() const override { return TagBits; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    class EventList : public IndexBase
    {
    public:
        EventList() { }
        EventList(int index) : IndexBase(index) { }
        virtual int GetIndexShift() const override { return 0; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    class FieldList : public IndexBase
    {
    public:
        FieldList() { }
        FieldList(int index) : IndexBase(index) { }
        virtual int GetIndexShift() const override { return 0; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    class MethodList : public IndexBase
    {
    public:
        MethodList() { }
        MethodList(int index) : IndexBase(index) { }
        virtual int GetIndexShift() const override { return 0; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    class ParamList : public IndexBase
    {
    public:
        ParamList() { }
        ParamList(int index) : IndexBase(index) { }
        virtual int GetIndexShift() const override { return 0; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    class PropertyList : public IndexBase
    {
    public:
        PropertyList() { }
        PropertyList(int index) : IndexBase(index) { }
        virtual int GetIndexShift() const override { return 0; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    class TypeDef : public IndexBase
    {
    public:
        TypeDef() { }
        TypeDef(int index) : IndexBase(index) { }
        virtual int GetIndexShift() const override { return 0; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    class ModuleRef : public IndexBase
    {
    public:
        ModuleRef() { }
        ModuleRef(int index) : IndexBase(index) { }
        virtual int GetIndexShift() const override { return 0; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    class DeclSecurity : public IndexBase
    {
    public:
        DeclSecurity() { }
        DeclSecurity(int tag, int index) : IndexBase(tag, index) { }
        enum Tags {
            TagBits = 2,
            TypeDef = 0,
            MethodDef = 1,
            Assembly = 2,
        };
        virtual int GetIndexShift() const override { return TagBits; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    class Semantics : public IndexBase
    {
    public:
        Semantics() { }
        Semantics(int tag, int index) : IndexBase(tag, index) { }
        enum Tags {
            TagBits = 1,
            Event = 0,
            Property = 1
        };
        virtual int GetIndexShift() const override { return TagBits; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    class FieldMarshal : public IndexBase
    {
        enum Tags {
            TagBits = 1,
            Field = 0,
            Param = 1
        };
        virtual int GetIndexShift() const override { return TagBits; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    class GenericRef : public IndexBase
    {
    public:
        GenericRef() { }
        GenericRef(int index) : IndexBase(index) { }
        virtual int GetIndexShift() const override { return 0; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    class Implementation : public IndexBase
    {
    public:
        Implementation() { }
        Implementation(int tag, int index) : IndexBase(tag, index) { }
        enum Tags {
            TagBits = 2,
            File = 0,
            AssemblyRef = 1,
        };
        virtual int GetIndexShift() const override { return TagBits; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    // we also have psuedo-indexes for the various streams
    // these are like regular indexes except streams are unambiguous so we don't need to shift
    // and add a tag
    class String : public IndexBase
    {
    public:
        String() { }
        String(int index) : IndexBase(index) { }
        virtual int GetIndexShift() const override { return 0; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    class US : public IndexBase
    {
    public:
        US() { }
        US(int index) : IndexBase(index) { }
        virtual int GetIndexShift() const override { return 0; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    class GUID : public IndexBase
    {
    public:
        GUID() { }
        GUID(int index) : IndexBase(index) { }
        virtual int GetIndexShift() const override { return 0; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };

    class Blob : public IndexBase
    {
    public:
        Blob() { }
        Blob(int index) : IndexBase(index) { }
        virtual int GetIndexShift() const override { return 0; }
        virtual bool HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const override;
    };
    // this is the base class for the metadata tables
    //
    class TableEntryBase
    {
    public:
        virtual ~TableEntryBase() { }
        virtual int TableIndex() const = 0;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const = 0;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) = 0;
    };

    // following we have the data describing each table
    class ModuleTableEntry : public TableEntryBase
    {
    public:
        ModuleTableEntry() { }
        ModuleTableEntry(size_t NameIndex, size_t GuidIndex) : nameIndex_(NameIndex), guidIndex_(GuidIndex) { }
        virtual int TableIndex() const override { return tModule; }
        String nameIndex_;
        GUID guidIndex_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class TypeRefTableEntry : public TableEntryBase
    {
    public:
        TypeRefTableEntry() { }
        TypeRefTableEntry(ResolutionScope Resolution, size_t TypeNameIndex, size_t TypeNameSpaceIndex) :
            resolution_(Resolution), typeNameIndex_(TypeNameIndex), typeNameSpaceIndex_(TypeNameSpaceIndex) { }
        virtual int TableIndex() const override { return tTypeRef; }
        ResolutionScope resolution_;
        String typeNameIndex_;
        String typeNameSpaceIndex_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class TypeDefTableEntry : public TableEntryBase
    {
    public:
        enum Flags
        {

            // visibility
            VisibilityMask = 0x00000007,

            NotPublic = 0x00000000,
            Public = 0x00000001,

            NestedPublic = 0x00000002,
            NestedPrivate = 0x00000003,
            NestedFamily = 0x00000004,
            NestedAssembly = 0x00000005,
            NestedFamANDAssem = 0x00000006,
            NestedFamORAssem = 0x00000007,

            // layout 
            LayoutMask = 0x00000018,

            AutoLayout = 0x00000000,
            SequentialLayout = 0x00000008,
            ExplicitLayout = 0x00000010,

            // semantics
            ClassSemanticsMask = 0x00000060,

            Class = 0x00000000,
            Interface = 0x00000020,

            // other attributes
            Abstract = 0x00000080,
            Sealed = 0x00000100,
            SpecialName = 0x00000400,
            Import = 0x00001000,
            Serializable = 0x00002000,

            // string format
            StringFormatMask = 0x00030000,

            AnsiClass = 0x00000000,
            UnicodeClass = 0x00010000,
            AutoClass = 0x00020000,
            CustomFormatClass = 0x00030000,

            // valid for custom format class, but undefined
            CustomFormatMask = 0x00C00000,


            BeforeFieldInit = 0x00100000,
            Forwarder = 0x00200000,

            // runtime
            ReservedMask = 0x00040800,
            RTSpecialName = 0x00000800,

            HasSecurity = 0x00040000,
        };
        TypeDefTableEntry() : flags_(0) { }
        TypeDefTableEntry(int Flags, size_t TypeNameIndex, size_t TypeNameSpaceIndex,
            TypeDefOrRef Extends, size_t FieldIndex, size_t MethodIndex) :
            flags_(Flags), typeNameIndex_(TypeNameIndex), typeNameSpaceIndex_(TypeNameSpaceIndex),
            extends_(Extends), fields_(FieldIndex), methods_(MethodIndex) { }
        virtual int TableIndex() const override { return tTypeDef; }
        int flags_;
        String typeNameIndex_;
        String typeNameSpaceIndex_;
        TypeDefOrRef extends_;
        FieldList fields_;
        MethodList methods_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };

    class FieldTableEntry : public TableEntryBase
    {
    public:
        enum Flags
        {
            FieldAccessMask = 0x0007,

            PrivateScope = 0x0000,
            Private = 0x0001,
            FamANDAssem = 0x0002,
            Assembly = 0x0003,
            Family = 0x0004,
            FamORAssem = 0x0005,
            Public = 0x0006,

            // other attribs
            Static = 0x0010,
            InitOnly = 0x0020,
            Literal = 0x0040,
            NotSerialized = 0x0080,
            SpecialName = 0x0200,

            // pinvoke    
            PinvokeImpl = 0x2000,

            // runtime
            ReservedMask = 0x9500,
            RTSpecialName = 0x0400,
            HasFieldMarshal = 0x1000,
            HasDefault = 0x8000,
            HasFieldRVA = 0x0100,
        };
        FieldTableEntry() : flags_(0) { }
        FieldTableEntry(int Flags, size_t NameIndex, size_t SignatureIndex) :
            flags_(Flags), nameIndex_(NameIndex), signatureIndex_(SignatureIndex) { }
        virtual int TableIndex() const override { return tField; }
        int flags_;
        String nameIndex_;
        Blob signatureIndex_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class MethodDefTableEntry : public TableEntryBase
    {
    public:
        enum ImplFlags
        {
            CodeTypeMask = 0x0003,   // Flags about code type.
            IL = 0x0000,   // Method impl is IL.
            Native = 0x0001,   // Method impl is native.
            OPTIL = 0x0002,   // Method impl is OPTIL
            Runtime = 0x0003,   // Method impl is provided by the runtime.
            ManagedMask = 0x0004,   // Flags specifying whether the code is managed
            // or unmanaged.
            Unmanaged = 0x0004,   // Method impl is unmanaged, otherwise managed.
            Managed = 0x0000,   // Method impl is managed.
            
            ForwardRef = 0x0010,   // Indicates method is defined; used primarily
            // in merge scenarios.
            PreserveSig = 0x0080,   // Indicates method sig is not to be mangled to
                                              // do HRESULT conversion.

            InternalCall = 0x1000,   // Reserved for internal use.
            
            Synchronized = 0x0020,   // Method is single threaded through the body.
            NoInlining = 0x0008,   // Method may not be inlined.
            MaxMethodImplVal = 0xffff,   // Range check value
        };
        enum Flags
        {

            MemberAccessMask = 0x0007,
            PrivateScope = 0x0000,
            Private = 0x0001,
            FamANDAssem = 0x0002,
            Assem = 0x0003,
            Family = 0x0004,
            FamORAssem = 0x0005,
            Public = 0x0006,

            Static = 0x0010,
            Final = 0x0020,
            Virtual = 0x0040,
            HideBySig = 0x0080,

            VtableLayoutMask = 0x0100,

            ReuseSlot = 0x0000,     // The default.
            NewSlot = 0x0100,

            // implementation attribs    
            CheckAccessOnOverride = 0x0200,
            Abstract = 0x0400,
            SpecialName = 0x0800,


            PinvokeImpl = 0x2000,

            UnmanagedExport = 0x0008,

               // Reserved flags for runtime use only.

            ReservedMask = 0xd000,
            RTSpecialName = 0x1000,

            HasSecurity = 0x4000,
            RequireSecObject = 0x8000,
        };
        MethodDefTableEntry() : implFlags_(0), flags_(0), rva_(0), method_(nullptr) { }
        MethodDefTableEntry(PEMethod *Method, int IFlags, int MFlags, size_t NameIndex,
               size_t SignatureIndex, size_t ParamIndex) : implFlags_(IFlags),
            flags_(MFlags), rva_(0), nameIndex_(NameIndex), signatureIndex_(SignatureIndex),
            paramIndex_(ParamIndex), method_(Method) { }
        virtual int TableIndex() const override { return tMethodDef; }
        PEMethod *method_; // write, for rva
        int rva_; // read only
        //
        int implFlags_;
        int flags_;
        String nameIndex_;
        Blob signatureIndex_;
        ParamList paramIndex_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class ParamTableEntry : public TableEntryBase
    {

    public:
        enum Flags
        {
            In = 0x0001,     // Param is [In]    
            Out = 0x0002,     // Param is [out]
            Optional = 0x0010,     // Param is optional


            // runtime attribs

            ReservedMask = 0xf000,
            HasDefault = 0x1000,     // Param has default value.

            HasFieldMarshal = 0x2000,     // Param has FieldMarshal.
            Unused = 0xcfe0,
        };
        ParamTableEntry() : flags_(0), sequenceIndex_(0) { }
        ParamTableEntry(int Flags, Word SequenceIndex, size_t NameIndex) :
            flags_(Flags), sequenceIndex_(SequenceIndex), nameIndex_(NameIndex) { }
        virtual int TableIndex() const override { return tParam; }
        int flags_;
        Word sequenceIndex_;
        String nameIndex_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class InterfaceImplTableEntry : public TableEntryBase
    {

    public:
        InterfaceImplTableEntry() { }
        InterfaceImplTableEntry(size_t cls, TypeDefOrRef interfce) :
            class_(cls), interface_(interfce) { }
        virtual int TableIndex() const override { return tInterfaceImpl; }
        TypeDef class_;
        TypeDefOrRef interface_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class MemberRefTableEntry : public TableEntryBase
    {
    public:
        MemberRefTableEntry() { }
        MemberRefTableEntry(MemberRefParent ParentIndex, size_t NameIndex, size_t SignatureIndex)
            : parentIndex_(ParentIndex), nameIndex_(NameIndex), signatureIndex_(SignatureIndex) { }
        virtual int TableIndex() const override { return tMemberRef; }
        MemberRefParent parentIndex_;
        String nameIndex_;
        Blob signatureIndex_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class ConstantTableEntry : public TableEntryBase
    {
    public:
        ConstantTableEntry() : type_(0) { }
        ConstantTableEntry(int Type, Constant ParentIndex, size_t ValueIndex) :
            type_(Type), parentIndex_(ParentIndex), valueIndex_(ValueIndex) { }
        virtual int TableIndex() const override { return tConstant; }
        Byte type_;
        Constant parentIndex_;
        Blob valueIndex_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class CustomAttributeTableEntry : public TableEntryBase
    {
    public:
        CustomAttributeTableEntry() { }
        CustomAttributeTableEntry(CustomAttribute ParentIndex, CustomAttributeType TypeIndex, size_t ValueIndex)
            : parentIndex_(ParentIndex), typeIndex_(TypeIndex), valueIndex_(ValueIndex) { }
        virtual int TableIndex() const override { return tCustomAttribute; }
        CustomAttribute parentIndex_;
        CustomAttributeType typeIndex_;
        Blob valueIndex_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class FieldMarshalTableEntry : public TableEntryBase
    {

    public:
        FieldMarshalTableEntry() { }
        FieldMarshalTableEntry(FieldMarshal parent, size_t nativeType) :
            parent_(parent), nativeType_(nativeType) { }
        virtual int TableIndex() const override { return tFieldMarshal; }
        FieldMarshal parent_;
        Blob nativeType_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class DeclSecurityTableEntry : public TableEntryBase
    {
    public:
        DeclSecurityTableEntry() : action_(0), parent_(0,0), permissionSet_ (0){ }
        DeclSecurityTableEntry(Word action, DeclSecurity parent, size_t permissionSet) :
            action_(action), parent_(parent), permissionSet_(permissionSet) { }
        virtual int TableIndex() const override { return tDeclSecurity; }

        Word action_;
        DeclSecurity parent_;
        Blob permissionSet_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class ClassLayoutTableEntry : public TableEntryBase
    {
    public:
        ClassLayoutTableEntry() : pack_(1), size_(1) { }
        ClassLayoutTableEntry(Word Pack, size_t Size, size_t Parent)
            : pack_(Pack), size_(Size), parent_(Parent) { }
        virtual int TableIndex() const override { return tClassLayout; }
        Word pack_;
        size_t size_;
        TypeDef parent_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class FieldLayoutTableEntry : public TableEntryBase
    {
    public:
        FieldLayoutTableEntry() : offset_(0) { }
        FieldLayoutTableEntry(size_t Offset, size_t Parent) : offset_(Offset), parent_(Parent) { }
        virtual int TableIndex() const override { return tFieldLayout; }
        size_t offset_;
        FieldList parent_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class StandaloneSigTableEntry : public TableEntryBase
    {
    public:
        StandaloneSigTableEntry() { }
        StandaloneSigTableEntry(size_t SignatureIndex) : signatureIndex_(SignatureIndex) { }
        virtual int TableIndex() const override { return tStandaloneSig; }
        Blob signatureIndex_;   
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class EventMapTableEntry : public TableEntryBase
    {
    public:
        EventMapTableEntry() { }
        EventMapTableEntry(size_t parent, size_t eventList) : parent_(parent), eventList_(eventList) { }
        TypeDef parent_;
        EventList eventList_;
        virtual int TableIndex() const override { return tEventMap; }
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class EventTableEntry : public TableEntryBase
    {
    public:
        enum Flags{
            SpecialName = 0x200,
            ReservedMask = 0x400,
            RTSpecialName = 0x400,
        };
        EventTableEntry() : flags_(0) { }
        EventTableEntry(Word flags, size_t name, TypeDefOrRef eventType) : 
                flags_(flags), name_(name), eventType_(eventType) { }
        Word flags_;
        String name_;
        TypeDefOrRef eventType_;
        virtual int TableIndex() const override { return tEvent; }
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class PropertyMapTableEntry : public TableEntryBase
    {
    public:
        PropertyMapTableEntry() { }
        PropertyMapTableEntry(size_t parent, size_t propertyList) : parent_(parent), propertyList_(propertyList) { }
        TypeDef parent_;
        PropertyList propertyList_;
        virtual int TableIndex() const override { return tPropertyMap; }
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class PropertyTableEntry : public TableEntryBase
    {
    public:
        enum Flags{
            SpecialName = 0x200,
            ReservedMask = 0xf400,
            RTSpecialName = 0x400,
            HasDefault = 0x1000
        };
        PropertyTableEntry() : flags_(0) { }
        PropertyTableEntry(Word flags, size_t name, size_t propertyType) : 
                flags_(flags), name_(name), propertyType_(propertyType) { }
        Word flags_;
        String name_;
        Blob propertyType_; // yes this is a signature in the Blob
        virtual int TableIndex() const override { return tProperty; }
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class MethodSemanticsTableEntry : public TableEntryBase
    {
    public:
        enum Flags
        {
            Setter = 1,
            Getter = 2,
            Other = 4,
            AddOn = 8,
            RemoveOn = 16,
            Fire = 32
        };
        MethodSemanticsTableEntry() : semantics_(0){ }
        MethodSemanticsTableEntry(Word semantics, size_t method, Semantics association)
            : semantics_(semantics), method_(method), association_(association) { }
        Word semantics_;
        MethodList method_;
        Semantics association_;
        virtual int TableIndex() const override { return tMethodSemantics; }
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class MethodImplTableEntry : public TableEntryBase
    {
    public:
        MethodImplTableEntry() { }
        MethodImplTableEntry(size_t cls, MethodDefOrRef methodBody, MethodDefOrRef methodDeclaration)
                : class_(cls), methodBody_(methodBody), methodDeclaration_(methodDeclaration)             { }
        TypeDef class_;
        MethodDefOrRef methodBody_;
        MethodDefOrRef methodDeclaration_;
        virtual int TableIndex() const override { return tMethodImpl; }
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class ModuleRefTableEntry : public TableEntryBase
    {
    public:
        ModuleRefTableEntry() { }
        ModuleRefTableEntry(size_t NameIndex) : nameIndex_(NameIndex) { }
        String nameIndex_;
        virtual int TableIndex() const override { return tModuleRef; }
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class TypeSpecTableEntry : public TableEntryBase
    {
    public:
        TypeSpecTableEntry() { }
        TypeSpecTableEntry(size_t SignatureIndex) : signatureIndex_(SignatureIndex) { }
        virtual int TableIndex() const override { return tTypeSpec; }
        Blob signatureIndex_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };

    // note this is necessary for pinvokes, however, there will be one record in the METHODDEF table
    // to give information about the function and its parameters.
    // if the function has a variable length argument list, there will also be one entry in the MEMBERREF
    // table for each invocation
    class ImplMapTableEntry : public TableEntryBase
    {
    public:
        enum Flags
        {
            NoMangle = 0x0001,   // use the member name as specified

            CharSetMask = 0x0006,
            CharSetNotSpec = 0x0000,
            CharSetAnsi = 0x0002,
            CharSetUnicode = 0x0004,
            CharSetAuto = 0x0006,

            BestFitUseAssem = 0x0000,
            BestFitEnabled = 0x0010,
            BestFitDisabled = 0x0020,
            BestFitMask = 0x0030,

            ThrowOnUnmappableCharUseAssem = 0x0000,
            ThrowOnUnmappableCharEnabled = 0x1000,
            ThrowOnUnmappableCharDisabled = 0x2000,
            ThrowOnUnmappableCharMask = 0x3000,

            SupportsLastError = 0x0040,

            CallConvMask = 0x0700,
            CallConvWinapi = 0x0100,   // Index_( will use native callconv appropriate to target windows platform.    
            CallConvCdecl = 0x0200,
            CallConvStdcall = 0x0300,
            CallConvThiscall = 0x0400,   // In M9, Index_( will raise exception.

            CallConvFastcall = 0x0500,

            MaxValue = 0xFFFF,
        };
        ImplMapTableEntry() : flags_(0) { }
        ImplMapTableEntry(int Flags, MemberForwarded MethodIndex, size_t ImportNameIndex, size_t ModuleIndex)
            : flags_(Flags), methodIndex_(MethodIndex), importNameIndex_(ImportNameIndex), moduleIndex_(ModuleIndex) {  }
        virtual int TableIndex() const override { return tImplMap; }
        int flags_;
        MemberForwarded methodIndex_;
        String importNameIndex_;
        ModuleRef moduleIndex_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class FieldRVATableEntry : public TableEntryBase
    {
    public:
        FieldRVATableEntry() : rva_(0) { }
        FieldRVATableEntry(size_t Rva, size_t FieldIndex) : rva_(Rva), fieldIndex_(FieldIndex) { }
        virtual int TableIndex() const override { return tFieldRVA; }
        size_t rva_;
        FieldList fieldIndex_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    enum AssemblyFlags
    {
        PublicKey = 0x0001,      // full key
        PA_None = 0x0000,
        PA_MSIL = 0x0010,
        PA_x86 = 0x0020,
        PA_IA64 = 0x0030,
        PA_AMD64 = 0x0040,
        PA_Specified = 0x0080,

        PA_Mask = 0x0070,
        PA_FullMask = 0x00F0,

        PA_Shift = 0x0004,     // shift count


        EnableJITcompileTracking = 0x8000, // From "DebuggableAttribute".    
        DisableJITcompileOptimizer = 0x4000, // From "DebuggableAttribute".

        Retargetable = 0x0100,
    };
    class AssemblyDefTableEntry : public TableEntryBase
    {
    public:
        enum
        {
            DefaultHashAlgId = 0x8004
        };
        AssemblyDefTableEntry() : hashAlgId_(DefaultHashAlgId), major_(0), minor_(0), build_(0), revision_(0), flags_(0) { }
        AssemblyDefTableEntry(int Flags, Word Major, Word Minor, Word Build, Word Revision,
            size_t NameIndex) : hashAlgId_(DefaultHashAlgId), flags_(Flags),
            major_(Major), minor_(Minor), build_(Build), revision_(Revision),
            nameIndex_(NameIndex) { }
        virtual int TableIndex()  const override { return tAssemblyDef; }
        Word hashAlgId_;
        Word major_, minor_, build_, revision_;
        int flags_;
        Blob publicKeyIndex_;
        String nameIndex_;
        String cultureIndex_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class AssemblyRefTableEntry : public TableEntryBase
    {
    public:
        AssemblyRefTableEntry() : major_(0), minor_(0), build_(0), revision_(0), flags_(0) { }
        AssemblyRefTableEntry(int Flags, Word Major, Word Minor, Word Build, Word Revision,
            size_t NameIndex, size_t KeyIndex = 0) : flags_(Flags),
            major_(Major), minor_(Minor), build_(Build), revision_(Revision),
            nameIndex_(NameIndex), publicKeyIndex_(KeyIndex) { }
        virtual int TableIndex() const override { return tAssemblyRef; }
        Word major_, minor_, build_, revision_;
        int flags_;
        Blob publicKeyIndex_;
        String nameIndex_;
        String cultureIndex_;
        Blob hashIndex_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class FileTableEntry : public TableEntryBase
    {
    public:
        enum Flags{
            ContainsMetaData =0,
            ContainsNoMetaData = 1
        };
        FileTableEntry() : flags_(0) { }
        FileTableEntry(DWord flags, size_t name, size_t hash) : 
                flags_(flags), name_(name), hash_(hash) { }
        DWord flags_;
        String name_;
        Blob hash_;
        virtual int TableIndex() const override { return tFile; }
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class ExportedTypeTableEntry : public TableEntryBase
    {
    public:
        // flags same as for typedef table
        ExportedTypeTableEntry() : flags_(0) { }
        ExportedTypeTableEntry(DWord flags, size_t typeDefId, size_t typeName, size_t typeNameSpace, Implementation implementation)
            : flags_(flags), typeDefId_(typeDefId), typeName_(typeName), typeNameSpace_(typeNameSpace), implementation_(implementation) { }
        DWord flags_;
        TypeDef typeDefId_;
        String typeName_;
        String typeNameSpace_;
        Implementation implementation_;
        virtual int TableIndex() const override { return tManifestResource; }
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class ManifestResourceTableEntry : public TableEntryBase
    {
    public:
        enum Flags{
            VisibilityMask = 7,
            Public = 1,
            Private = 2
        };
        ManifestResourceTableEntry() : offset_(0), flags_(0) { }
        ManifestResourceTableEntry(DWord offset, DWord flags, size_t name, Implementation implementation) : 
                offset_(offset), flags_(flags), name_(name), implementation_(implementation) { }
        DWord offset_;
        DWord flags_;
        String name_;
        Implementation implementation_;
        virtual int TableIndex() const override { return tManifestResource; }
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class NestedClassTableEntry : public TableEntryBase
    {
    public:
        NestedClassTableEntry() { }
        NestedClassTableEntry(size_t nested, size_t enclosing) : nestedIndex_(nested), enclosingIndex_(enclosing) { }
        virtual int TableIndex() const override { return tNestedClass; }
        TypeDef nestedIndex_;
        TypeDef enclosingIndex_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class GenericParamTableEntry : public TableEntryBase
    {
    public:
        GenericParamTableEntry() :number_(0), flags_(0) { }
        GenericParamTableEntry(Word number, Word flags, TypeOrMethodDef owner, size_t name)
                : number_(number), flags_(flags), owner_(owner), name_(name) { }
        virtual int TableIndex() const override { return tGenericParam; }
        Word number_;
        Word flags_;
        TypeOrMethodDef owner_;
        String name_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class MethodSpecTableEntry : public TableEntryBase
    {
    public:
        MethodSpecTableEntry() { }
        MethodSpecTableEntry(MethodDefOrRef method, size_t instantiation) : 
                method_(method), instantiation_(instantiation) { }
        virtual int TableIndex() const override { return tMethodSpec; }
        MethodDefOrRef method_;
        Blob instantiation_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };
    class GenericParamConstraintsTableEntry : public TableEntryBase
    {
    public:
        GenericParamConstraintsTableEntry() { }
        GenericParamConstraintsTableEntry(size_t owner, TypeDefOrRef constraint) : 
                owner_(owner), constraint_(constraint) { }
        virtual int TableIndex() const override { return tGenericParam; }
        GenericRef owner_;
        TypeDefOrRef constraint_;
        virtual size_t Render(size_t sizes[MaxTables + ExtraIndexes], Byte *) const override;
        virtual size_t Get(size_t sizes[MaxTables + ExtraIndexes], Byte *) override;
    };

    // this class holds the data for a method
    // right now it holds redundant data for the function body
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

            CIL = 0x4000, // not a real flag either
            EntryPoint = 0x8000 // not a real flag that goes in the PE file
        };
        PEMethod(bool hasSEH, int Flags, size_t MethodDef, int MaxStack, int localCount, int CodeSize, size_t signature)
            : flags_(Flags), hdrSize_(3), maxStack_(MaxStack), codeSize_(CodeSize), code_(nullptr), signatureToken_(signature), rva_(0), methodDef_(MethodDef)
        {
            if ((flags_ & 0xfff) == 0)
            {
                if (maxStack_ <= 8 && codeSize_ < 64 && localCount == 0 && !hasSEH)
                {
                    flags_ = flags_ | (int)TinyFormat;
                }
                else
                {
                    flags_ = flags_ | (int)FatFormat;
                }
            }
        }
        enum {
            EHTable = 1,
            OptILTable = 2,
            EHFatFormat = 0x40,
            EHMoreSects = 0x80,
        };
        std::vector<CodeContainer::SEHData> sehData_;
        int flags_;
        int hdrSize_; /* = 3 */
        Word maxStack_;
        size_t codeSize_;
        Byte *code_;
        size_t signatureToken_;
        size_t rva_;
        size_t methodDef_;
        size_t Write(size_t sizes[MaxTables + ExtraIndexes], std::iostream &out) const;
    };
    inline char* StrCpy(char *data, size_t len, const char* source)
    {
        strncpy(data, source, len);
        data[len - 1] = '\0';
        return data;
    }
    template<size_t len>
    inline char *StrCpy(char(&data)[len], const char* source)
    {
        return StrCpy(data, len, source);
    }
    inline char* StrCat(char *data, size_t len, const char* source)
    {
        int num = (int)len - (int)strlen(data) - 1;
        if (num > 0)
            strncat(data, source, num);
        return data;
    }
    template<size_t len>
    inline char *StrCat(char(&data)[len], const char* source)
    {
        return StrCat(data, len, source);
    }

} // namespace