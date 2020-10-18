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

#ifndef DOTNETPELIB_H
#define DOTNETPELIB_H
#include <list>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string.h>
#include <memory>
// reference changelog.txt to see what the changes are
//
#define DOTNETPELIB_VERSION "3.01"

// this is the main library header
// it allows creation of the methods and data that would be dumped into 
// a .net assembly.  The data can be either directly written to an 
// executable file or DLL, or dumped into a .IL file suitable for 
// viewing or assembling with the ilasm assembler.
//
//
//   The general usage of this library is to first create a PELib object,
//   then populate it with objects related to your program.   If you want you can
//   use members of the Allocator class to instantiate the various objects,
//   if you do this they will be deleted when the PELib object is deleted.   Otherwise
//   if you use operator new you are responsible for your own memory management.
//
//	 Once you have created a PELib object, it will create an AssemblyRef object
//   Which is the container for your program.   If you want to write a free standing program
//   you would put your methods and fields into this object.   However if you want to
//   write a DLL that .net assemblies can access, you need to create a namespace in the assemblyref
//   and at least one class in the namespace.   Methods and fields go into the inner class.
//
//   Another thing you have to do for .net compatibility is set the CIL flag
//   in the PELib object.   IF you have a free-standing program you might not 
//   want to set it, for example you can simplify code generation by putting
//   initialized data in the SDATA segment.   To put data in the SDATA segment
//   you add an initializer to a static field. (it makes the equivalent of a .DATA statement)

//   this library has full support for 32- bit pinvoke
//      you have to help it just a little in the case of pinvokes with varargs
//      by creating a method signature for each unique invocation of the pinvoke
//      that is independent of the main method for the pinvoke.
//      the main method does not have a vararg list but they unique invocations do
//      internally, the exe generator will store the main method in the MethodDef table
//      and the unique invocations in the MemberRef table.   An entry in the ImplMap
//      table will give information about what DLL the pinvoke is related to.
//
//   this library has only been tested with 32 bit programs, however it should
//   work with 64 bit programs as long as pinvoke isn't used.
//
//   Note that if you intend to pass managed methods to unmanaged code or vice
//      versa you may need to create transition thunks somewhere.   OCCIL has
//      a library that does this for you, assuming you only want to marshal
//      simple types.
//   OCCIL also has a library which does simple marshalling between C-style strings
//      and .net style strings.
//
//   This libray does simple peephole optimizations when an instruction
//   can be replaced with a shorter equivalent.   For example:
//			ldc.i4 # can sometimes be shortened into one of several optimized instructions
//			branches can sometimes be shortened
//			parameter and local variable indexes can be optimized
//				for local variables the library will also modify the local variable
//				list and put the most used local variables first (as a further optimization)
//          maximum stack is calculated
//
// there are various things missing:
//   Public and Private are the only scope modifiers currently
//	 no support for character set modifiers
//   
//   Custom Attributes cannot in general be defined however internally
//		we do handle the C# vararg one behind the scenes
//   Parameter handling is missing support for [in][out][opt] and default values
//   you can define static data in the CIL data area, but I haven't vetted it enough
//   to know if you can write to it.
//
namespace DotNetPELib
{

    // definitions for some common types
    typedef long long longlong;
    typedef unsigned long long ulonglong;
    typedef unsigned char Byte; /* 1 byte */
    typedef unsigned short Word; /* two bytes */
    typedef unsigned DWord; /* four bytes */

    // forward references
    class PELib;
    class Allocator;
    class Method;
    class AssemblyDef;
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
    class PEWriter;
    class PEReader;
    class PEMethod;
    class CustomAttribute;
    class MethodSemanticsTableEntry;
    class Callback;
    /* contains data, types, methods */

    ///** Destructor base.
    // Used to support cleaning up objects when the allocator is used to create
    // them.  Every object that can be allocated eventualy inherits
    // this virtual destructor, so that the allocator can destruct them again later. 
    class DestructorBase
    {
    public:
        DestructorBase() { }
        DestructorBase(const DestructorBase &) { }
        DestructorBase& operator=(const DestructorBase &) { return *this; }
        virtual ~DestructorBase() { }
    };

    ///** errors can be thrown, generally when the IL code is being 'optimized'
    // but sometimes at other times
    class PELibError : public std::runtime_error
    {
    public:

        enum ErrorList
        {
            ///** More params are being aded after a vararg param has been added
        	    VarargParamsAlreadyDeclared,
            ///** If calculations show that the stack would underflow somewhere in the method
            StackUnderflow,
            ///** A label can be reached but the paths don't agree on how many
            // things are currently on the stack
            MismatchedStack,
            ///** The stack is unbalanced when the RET statement is hit
            // This can mean either that there are elements on the stack in
            // a function that has no return value, or there is not exactly '
            //one element on the stack when the function has a return value.
            StackNotEmpty,
            ///** A label has been defined twice
            DuplicateLabel,
            ///** A label that has been referenced was not found
            MissingLabel,
            ///** the short version of some branch was requested, but the target
            // is out of range.
            ShortBranchOutOfRange,
            ///** the short version of an instruction which references a
            // local variable or a parameter was referenced, but the index is out
            // of range
            IndexOutOfRange,
            ///** There are multiple entry points
            MultipleEntryPoints,
            ///** There is no entry point (note that if it is a DLL an entry point isn't needed)
            MissingEntryPoint,
            ///** Expected an SEH Try block
            ExpectedSEHTry,
            ///** Expected an SEH handler
            ExpectedSEHHandler,
            ///** Mismatched SEH end tag
            MismatchedSEHTag,
            ///** SEH tag is orphaned
            OrphanedSEHTag,
            ///** Invalid SEH Filter
            InvalidSEHFilter,
            ///** Seh section not correctly ended
            InvalidSEHEpilogue,
        };
        PELibError(ErrorList err, const std::string &Name = "") : errnum(err), std::runtime_error(std::string(errorNames[err]) + " " + Name)
        {
        }
        virtual ~PELibError() { }
        ErrorList Errnum() const { return errnum; }
    private:
        ErrorList errnum;
        static const char *errorNames[];
    };
    class ObjectError : public std::runtime_error
    {
    public:
        ObjectError(char *a) : runtime_error(a) { }
    };
    // an internal enumeration for errors associated with invalidly formatted
    // object files.   Usually only used for debugging the object file handlers
    enum {
        oe_syntax,
        oe_nonamespace,
        oe_noclass,
        oe_noenum,
        oe_nofield,
        oe_nomethod,
        oe_typemismatch,
        oe_corFlagsMismatch,
    };
    // Qualifiers is a generic class that holds all the 'tags' you would see on various objects in
    // the assembly file.   Where possible things are handled implicitly for example 'nested'
    // will automatically be added when a class is nested in another class.
    class Qualifiers : public DestructorBase
    {
    public:
        enum {
            Public = 0x1,
            Private = 0x2,
            Static = 0x4,
            Instance = 0x8,
            Explicit = 0x10,
            Ansi = 0x20,
            Sealed = 0x40,
            Enum = 0x80,
            Value = 0x100,
            Sequential = 0x200,
            Auto = 0x400,
            Literal = 0x800,
            HideBySig = 0x1000,
            PreserveSig = 0x2000,
            SpecialName = 0x4000,
            RTSpecialName = 0x8000,
            CIL = 0x10000,
            Managed = 0x20000,
            Runtime = 0x40000,
            Virtual = 0x100000, // sealed
            NewSlot = 0x200000 // value
        };
        enum
        {
            // settings appropriate for occil, e.g. everything is static.  add public/private...
            MainClass = Ansi | Sealed,
            ClassClass = Value | Sequential | Ansi | Sealed,
            ClassUnion = Value | Explicit | Ansi | Sealed,
            ClassField = 0,
            FieldInitialized = Static,
            EnumClass = Enum | Auto | Ansi | Sealed,
            EnumField = Static | Literal | Public,
            PInvokeFunc = HideBySig | Static | PreserveSig,
            ManagedFunc = HideBySig | Static | CIL | Managed
        };
        Qualifiers() : flags_(0)
        {
        }
        Qualifiers(int Flags) : flags_(Flags)
        {
        }
        Qualifiers(const Qualifiers &old)
        {
            flags_ = old.flags_;
        }
        Qualifiers &operator |=(int flags)
        {
            flags_ |= flags;
            return *this;
        }
        ///** most qualifiers come before the name of the item
        void ILSrcDumpBeforeFlags(PELib &) const;
        ///** but a couple of the method qualifiers come after the method definition
        void ILSrcDumpAfterFlags(PELib &) const;
        ///** get a name for a DataContainer object, suitable for use in an ASM file
        // The main problem is there is a separator character between the first class encountered
        // and its members, which is different depending on whether it is a type or a field
        static std::string GetName(const std::string& root, const DataContainer *parent, bool type = false);
        static std::string GetObjName(const std::string& root, const DataContainer *parent);
        virtual void ObjOut(PELib &, int pass) const;
        void ObjIn(PELib &, bool definition = true);
        int Flags() const { return flags_; }
        void Flags(int flags) { flags_ = flags;  }
    protected:
        static void ReverseNamePrefix(std::string&rv, const DataContainer *parent, int &pos, bool type);
        static std::string GetNamePrefix(const DataContainer *parent, bool type);
    private:
        static const char * qualifierNames_[];
        static int afterFlags_;
        int flags_;
    };
    ///** base class that contains instructions/ labels
    // will be further overridden later to make a 'method'
    // definition
    class CodeContainer : public DestructorBase
    {
    public:
        CodeContainer(Qualifiers Flags) :flags_(Flags), hasSEH_(false), parent_(nullptr) { }
        ///** This is the interface to add a single CIL instruction
        void AddInstruction(Instruction *instruction);

        ///** it is possible to remove the last instruction
        Instruction *RemoveLastInstruction() {
            Instruction *rv = instructions_.back();
            instructions_.pop_back();
            return rv;
        }
        ///** Retrieve the last instruction
        Instruction *LastInstruction() const {
            return instructions_.back();
        }
        ///** Validate instructions
        void ValidateInstructions();
        ///** Validate SEH tags, e.g. make sure there are matching begin and end tags and that filters are organized properly
        void ValidateSEH();
        ///** Validate one level of tags (recursive)
        int ValidateSEHTags(std::vector<Instruction *>&tags, int offset);
        ///** Validate that SEH filter expressions are in the proper place
        void ValidateSEHFilters(std::vector<Instruction *>&tags);
        ///** Validate the epilogue for each SEH section
        void ValidateSEHEpilogues();
        ///** return flags member
        Qualifiers &Flags() { return flags_; }
        const Qualifiers &Flags() const { return flags_; }
        ///** set parent
        void SetContainer(DataContainer *parent) { parent_ = parent; }
        ///** get parent
        DataContainer *GetContainer() const { return parent_; }

        struct SEHData
        {
            enum {
                Exception = 0,
                Filter = 1,
                Finally = 2,
                Fault = 4
            } flags;
            size_t tryOffset;
            size_t tryLength;
            size_t handlerOffset;
            size_t handlerLength;
            union
            {
                size_t filterOffset;
                size_t classToken;
            };
        };
        // some internal functions
        bool InAssemblyRef() const;
        void BaseTypes(int &types) const;
        virtual void Optimize(PELib &);
        virtual bool ILSrcDump(PELib &) const;
        virtual bool PEDump(PELib &) { return false; }
        virtual void Render(PELib&) { }
        virtual void ObjOut(PELib &, int pass) const;
        void ObjIn(PELib &);
        Byte *Compile(PELib &, size_t &sz);
        int CompileSEH(std::vector<Instruction *>tags, int offset, std::vector<SEHData> &sehData);
        void CompileSEH(PELib &, std::vector<SEHData> &sehData);
        virtual void Compile(PELib&) { }

        std::list<Instruction *>::iterator begin() { return instructions_.begin(); }
        std::list<Instruction *>::iterator end() { return instructions_.end(); }

    protected:
        std::map<std::string, Instruction *> labels;
        void LoadLabels();
        void OptimizeLDC(PELib &);
        void OptimizeLDLOC(PELib &);
        void OptimizeLDARG(PELib &);
        void OptimizeBranch(PELib &);
        void CalculateOffsets();
        bool ModifyBranches();
        std::list<Instruction *> instructions_;
        Qualifiers flags_;
        DataContainer *parent_;
        bool hasSEH_;
    };
    ///** base class that contains other datacontainers or codecontainers
    // that means it can contain namespaces, classes, methods, or fields
    // The main assemblyref which holds everything is one of these,
    // which means it acts as the 'unnamed' namespace.
    // when this class is overridden as something other than a namespace,
    // it cannot contain namespaces
    class DataContainer : public DestructorBase
    {
    public:
        ///** all classes have to extend from SOMETHING...  
        // this is enumerations for the ones we can create by default
        enum
        {
            ///**  reference to 'System::Object'
            basetypeObject = 1,
            ///**  reference to 'System::Value'
            basetypeValue = 2,
            ///**  reference to 'System::Enum'
            basetypeEnum = 4,
            ///** reference to 'System' namespace
            baseIndexSystem = 8
        };
        DataContainer(const std::string& Name, Qualifiers Flags) : name_(Name), flags_(Flags),
            parent_(nullptr), instantiated_(false), peIndex_(0), assemblyRef_(false)
        {
        }
        ///** Add another data container
        // This could be an assemblydef, namespace, class, or enumeration
        void Add(DataContainer *item)
        {
            if (item)
            {
                item->parent_ = this;
                children_.push_back(item);
                sortedChildren_[item->name_].push_back(item);
            }
        }
        ///** Add a code container
        // This is always a Method definition
        void Add(CodeContainer *item)
        {
            if (item)
            {
                item->SetContainer(this);
                methods_.push_back(item);
            }
        }
        ///** Add a field
        void Add(Field *field);
        ///** A flag an app can use to tell if the class has been instantiated,
        // for example it might be used after a forward reference is resolved.
        // it is not used internally to the library
        bool IsInstantiated() const { return instantiated_; }
        void SetInstantiated() { instantiated_ = true; }
        ///** The immediate parent
        DataContainer *Parent() const { return parent_; }
        void Parent(DataContainer* parent) { parent_ = parent; }
        ///** The inner namespace parent
        size_t ParentNamespace(PELib &peLib) const;
        ///** The closest parent class
        size_t ParentClass(PELib &peLib) const;
        ///** The parent assembly
        size_t ParentAssembly(PELib &peLib) const;
        ///** The name
        const std::string &Name() const { return name_; }
        ///** The qualifiers
        Qualifiers &Flags() { return flags_; }
        ///** metatable index in the PE file for this data container
        size_t PEIndex() const { return peIndex_; }
        ///** metatable index in the PE file for this data container
        void PEIndex(size_t index) { peIndex_ = index; }
        ///* find a sub-container
        DataContainer *FindContainer(const std::string& name, std::deque<Type*>* generics = nullptr);
        ///** Find a sub- container
        DataContainer *FindContainer(std::vector<std::string>& split, size_t &n, std::deque<Type*>* generics = nullptr, bool method = false);
        const std::list<Field *>&Fields() const { return fields_; }
        const std::list<CodeContainer *>&Methods() const { return methods_; }
        ///** Traverse the declaration tree
        virtual bool Traverse(Callback &callback) const;
        // internal functions
        virtual bool InAssemblyRef() const { return parent_->InAssemblyRef(); }
        virtual bool ILSrcDump(PELib &) const;
        virtual bool PEDump(PELib &);
        virtual void Compile(PELib&);
        virtual void ObjOut(PELib &, int pass) const;
        void ObjIn(PELib &, bool definition = true);
        void Number(int &n);
        // sometimes we want to traverse upwards in the tree
        void Render(PELib&);
        void BaseTypes(int &types) const;
        void Clear() { children_.clear(); methods_.clear(); sortedChildren_.clear(); fields_.clear(); }
    protected:
        std::list<DataContainer *> children_;
        std::list<CodeContainer *> methods_;
        std::map<std::string, std::deque<DataContainer *>> sortedChildren_;
        std::list<Field *> fields_;
        DataContainer *parent_;
        Qualifiers flags_;
        std::string name_;
        bool instantiated_;
        size_t peIndex_; // generic index into a table or stream
        bool assemblyRef_;
    };
    ///** class to hold custom attributes.  only parses them at this point, so that
    // you can retrieve attributes from .net assemblies if you want to.  if you
    // want to generate them you are on your own.
    class CustomAttributeContainer : public DestructorBase
    {
    public:
        struct lt
        {
            bool operator()(const CustomAttribute *left, const CustomAttribute *right) const;
        };
        class CustomAttributeDescriptor
        {
        public:
            std::string name;
            Byte *data;
            size_t sz;
            CustomAttributeDescriptor() : data(nullptr), sz(0) { }
            ~CustomAttributeDescriptor() { delete data; }
            bool operator() (const CustomAttributeDescriptor *left, const CustomAttributeDescriptor *right) const;
        };
        CustomAttributeContainer() { }
        ~CustomAttributeContainer();
        void Load(PELib &peLib, AssemblyDef &assembly, PEReader &reader);
        const std::vector<CustomAttributeDescriptor *>& Lookup(CustomAttribute *attribute) const;
        bool Has(CustomAttribute &attribute, const std::string& name,  Byte *data = nullptr, size_t sz = 0) const;
    private:
        std::map<CustomAttribute *, std::vector<CustomAttributeDescriptor *>, lt> attributes;
        std::set<CustomAttributeDescriptor *, CustomAttributeDescriptor> descriptors;
    };
    ///** base class for assembly definitions
    // this holds the main assembly ( as a non-external assembly)
    // or can hold an external assembly
    class AssemblyDef : public DataContainer
    {
    public:
        AssemblyDef(const std::string& Name, bool External, Byte * KeyToken = nullptr) : DataContainer(Name, 0), external_(External),
            major_(0), minor_(0), build_(0), revision_(0), loaded_(false)
        {
            if (KeyToken)
                memcpy(publicKeyToken_, KeyToken, 8);
            else
                memset(publicKeyToken_, 0, 8);
        }
        void SetVersion(int major, int minor, int build, int revision)
        {
            major_ = major;
            minor_ = minor;
            build_ = build;
            revision_ = revision;
        }
        virtual ~AssemblyDef() { }
        ///** get name of strong name key file (will be "" by default)
        const std::string& SNKFile() const { return snkFile_; }
        ///** set name of strong name key file
        void SNKFile(const std::string& file) { snkFile_ = file; }
        ///** root for Load assembly from file
        void Load(PELib &lib, PEReader &reader);
        ///** lookup or create a class
        Class *LookupClass(PELib &lib, const std::string& nameSpace, const std::string& name);
        ///** Set a public key
        void SetPublicKey(PEReader &reader, size_t index);
                
        const CustomAttributeContainer &CustomAttributes() const { return customAttributes_;  }
        virtual bool InAssemblyRef() const override { return external_; }
        bool IsLoaded() { return loaded_; }
        void SetLoaded() { loaded_ = true; }
        bool ILHeaderDump(PELib &);
        bool PEHeaderDump(PELib &);
        virtual void ObjOut(PELib &, int pass) const override;
        static AssemblyDef *ObjIn(PELib &, bool definition = true);

    protected:
        Namespace *InsertNameSpaces(PELib &lib, std::map<std::string, Namespace *> &nameSpaces, const std::string& name);
        Namespace *InsertNameSpaces(PELib &lib, Namespace *nameSpace, std::string nameSpaceName);
        Class *InsertClasses(PELib &lib, Namespace *nameSpace, Class *cls, std::string name);
    private:
        std::string snkFile_;
        bool external_;
        Byte publicKeyToken_[8];
        int major_, minor_, build_, revision_;
        bool loaded_;
        CustomAttributeContainer customAttributes_;
        std::map<std::string, Namespace *> namespaceCache;
        std::map<std::string, Class *> classCache;
    };
    ///** a namespace
    class Namespace : public DataContainer
    {
    public:
        Namespace(const std::string& Name) : DataContainer(Name, Qualifiers(0))
        {
        }

        ///** Get the full namespace name including all parents
        std::string ReverseName(DataContainer *child);
        virtual bool ILSrcDump(PELib &) const override;
        virtual bool PEDump(PELib &) override;
        virtual void ObjOut(PELib &, int pass) const override;
        static Namespace *ObjIn(PELib &, bool definition = true);
    };

    /* a property, note we are only supporting classic properties here, not any
     * extensions that are allowed in the image file format
     */
    class Property : public DestructorBase
    {
    public:
        enum {
            SpecialName = 0x200,
            RTSpecialName = 0x400,
            HasDefault = 0x1000
        };
        Property(PELib &peLib, const std::string& name, Type *type, std::vector<Type *>& indices, bool hasSetter = true, DataContainer *parent = nullptr)
            : name_(name), parent_(parent), type_(type), flags_(SpecialName), instance_(true), getter_(nullptr), setter_(nullptr)
        {
            CreateFunctions(peLib, indices, hasSetter);
        }    
        Property() : parent_(NULL), type_(nullptr), flags_(SpecialName), 
            instance_(true), getter_(nullptr), setter_(nullptr) { }
            ///** Set the parent container (always a class)
        void SetContainer(DataContainer *parent, bool add = true);
        ///** Get the parent container (always a class)
        DataContainer* GetContainer() const { return parent_; }
        ///** choose whether it is an instance member or static property
        void Instance(bool instance);
        ///** return whether it is an instance member or static property
        bool Instance() const { return instance_;  }
        ///** set the name
        void Name(const std::string& name) { name_ = name; }
        ///** Get the name
        const std::string &Name() const { return name_; }
        ///* set the type
        void SetType(Type *type) { type_ = type;  }
        ///* get the type
        Type *GetType() const { return type_;  }
        ///** Call the getter, leaving property on stack
        /// If you had other arguments you should push them before the call
        void CallGet(PELib &peLib, CodeContainer *code);
        ///** Call the setter, 
        /// If you had other arguments you should push them before the call
        /// then push the value you want to set
        void CallSet(PELib &peLib, CodeContainer *code);
        ///** Get the getter
        Method *Getter() { return getter_;  }
        ///** Get the setter
        Method *Setter() { return setter_;  }

        void Getter(Method *getter) { getter_ = getter; }
        void Setter(Method *setter) { setter_ = setter; }
        // internal functions
        ///** root for Load assembly from file
        void Load(PELib &lib, AssemblyDef &assembly, PEReader &reader, size_t propIndex, size_t startIndex, size_t startSemantics, size_t endSemantics, std::vector<Method *>& methods);
        virtual bool ILSrcDump(PELib &) const;
        virtual bool PEDump(PELib &);
        virtual void ObjOut(PELib &, int pass) const;
        static Property *ObjIn(PELib &);
    protected:
        void CreateFunctions(PELib &peLib, std::vector<Type *>& indices, bool hasSetter);
    private:
        int flags_;
        bool instance_;
        std::string name_;
        Type *type_;
        DataContainer *parent_;
        Method *getter_;
        Method *setter_;
    };
    /* a class, note that it cannot contain namespaces which is enforced at compile time*/
    /* note that all classes have to eventually derive from one of the System base classes
     * but that is handled internally */
     /* enums derive from this */
    class Class : public DataContainer
    {
    public:
        Class(const std::string& Name, Qualifiers Flags, int Pack, int Size) : DataContainer(Name, Flags),
            pack_(Pack), size_(Size), extendsFrom_(nullptr), external_(false), genericParent_(nullptr)
        {
        }
        Class(const Class&) = default;
        Class& operator=(const Class&) = default;
        ///** set the structure packing
        void pack(int pk) { pack_ = pk; }
        ///** set the structure size
        void size(int sz) { size_ = sz; }
        int size() { return size_; }
        ///**set the class we are extending from, if this is unset
        // a system class will be chosen based on whether or not the class is a valuetype
        // this may be unset when reading in an assembly, in that case ExtendsName may give the name of a class which is being extended from
        void Extends(Class *extendsFrom) { extendsFrom_ = extendsFrom; }
        Class *Extends() const { return extendsFrom_;  }
        void ExtendsName(std::string&name) { extendsName_ = name; }
        std::string ExtendsName() const { return extendsName_;  }
        ///** not locally defined
        bool External() const { return external_; }
        ///** not locally defined
        void External(bool external) { external_ = external; }
        ///** add a property to this container
        void Add(Property *property, bool add = true)
        {
            if (property)
            {
                property->SetContainer(this, add);
                properties_.push_back(property);
            }
        }
        using DataContainer::Add;
        void GenericParent(Class* cls) { genericParent_ = cls; }
        Class* GenericParent() const { return genericParent_; }
        ///** Traverse the declaration tree
        virtual bool Traverse(Callback &callback) const override;
        ///** return the list of properties
        const std::vector<Property *>& Properties() const { return properties_;  }
        ///** return the list of generics
        std::deque<Type*>& Generic() { return generic_; }
        const std::deque<Type*>& Generic() const { return generic_; }
        ///** root for Load assembly from file
        void Load(PELib &lib, AssemblyDef &assembly, PEReader &reader, size_t index, int startField, int endField, int startMethod, int endMethod, int startSemantics, int endSemantics);
        virtual bool ILSrcDump(PELib &) const override;
        virtual bool PEDump(PELib &) override;
        void ILSrcDumpClassHeader(PELib &) const;
        virtual void ObjOut(PELib &, int pass) const override;
        static Class *ObjIn(PELib &, bool definition = true);
        std::string AdornGenerics(PELib& peLib, bool names = false) const;
        bool MatchesGeneric(std::deque<Type*>* generics) const;
    protected:
        int TransferFlags() const;
        int pack_;
        int size_;
        Class *extendsFrom_;
        std::string extendsName_;
        std::vector<Property *>properties_;
        bool external_;
        std::deque<Type*> generic_;
        Class* genericParent_;
    };
    ///** A method with code
    // CIL instructions are added with the 'Add' member of code container
    class Method : public CodeContainer
    {
    public:
        ///** a call to either managed or unmanaged code
        enum InvokeMode { CIL, PInvoke };
        ///** linkage type for unmanaged call.
        enum InvokeType { Cdecl, Stdcall };
        Method(MethodSignature *Prototype, Qualifiers flags, bool entry = false);

        ///** Set Pinvoke DLL name
        void SetPInvoke(const std::string& name, InvokeType type = Stdcall)
        {
            invokeMode_ = PInvoke;
            pInvokeName_ = name;
            pInvokeType_ = type;
        }
        bool IsPInvoke() const { return invokeMode_ == PInvoke; }
        ///** Add a local variable
        void AddLocal(Local *local);

        void Instance(bool instance);
        bool Instance() const { return !!(Flags().Value & Qualifiers::Instance); }
        ///** return the signature
        MethodSignature *Signature() const { return prototype_; }

        ///** is it an entry point function
        bool HasEntryPoint() const { return entryPoint_; }

        ///** Iterate through local variables
        typedef std::vector<Local *>::iterator iterator;
        iterator begin() { return varList_.begin(); }
        iterator end() { return varList_.end(); }
        size_t size() const { return varList_.size(); }

        // Internal functions
        void MaxStack(int stack) { maxStack_ = stack;  }
        virtual bool ILSrcDump(PELib &) const override;
        virtual bool PEDump(PELib &) override;
        virtual void Compile(PELib&) override;
        virtual void Optimize(PELib &) override;
        virtual void ObjOut(PELib &, int pass) const override;
        static Method *ObjIn(PELib &, bool definition = true, Method **found = nullptr);
    protected:
        void OptimizeLocals(PELib &);
        void CalculateMaxStack();
        void CalculateLive();
        MethodSignature *prototype_;
        std::vector<Local *> varList_;
        std::string pInvokeName_;
        InvokeMode invokeMode_;
        InvokeType pInvokeType_;
        int maxStack_;
        bool entryPoint_;
        PEMethod *rendering_;
    };

    ///** a field, could be either static or non-static
    class Field : public DestructorBase
    {
    public:
        ///** Size for enumerated values
        enum ValueSize { i8, i16, i32, i64 };
        ///** Mode for the initialized value
        enum ValueMode {
            ///** No initialized value
            None,
            ///** Enumerated value, goes into the constant table
            Enum,
            ///** Byte stream, goes into the sdata
            Bytes
        };
        Field(const std::string& Name, Type *tp, Qualifiers Flags) : mode_(Field::None), name_(Name), flags_(Flags), parent_(nullptr), size_(i8),
            type_(tp), /*enumValue_(0),*/ byteValue_(nullptr), byteLength_(0), ref_(0), peIndex_(0), explicitOffset_(0), external_(false), definitions_(0)
        {
        }
        ///** Add an enumeration constant
        // Note that the field does need to be part of an enumeration
        void AddEnumValue(longlong Value, ValueSize Size);
        longlong EnumValue() const { return enumValue_;  }
        ///** Add an SDATA initializer
        void AddInitializer(Byte *bytes, int len); // this will be readonly in ILONLY assemblies
        ///** Field Name
        const std::string &Name() const { return name_; }
        ///** Set the field's container
        void SetContainer(DataContainer *Parent) { parent_ = Parent; }
        DataContainer *GetContainer() const { return parent_; }
        ///** Type of field
        Type *FieldType() const { return type_; }
        void FieldType(Type *tp) { type_ = tp; }
        ///** Field qualifiers
        const Qualifiers &Flags() const { return flags_; }
        //* Is field referenced
        void Ref(bool Ref) { ref_ = Ref; }
        //* Is field referenced
        bool IsRef() const { return ref_; }
        ///** not locally defined
        bool External() const { return external_; }
        ///** not locally defined
        void External(bool external) { external_ = external; }
        ///** increment definition count
        void Definition() { definitions_++; }
        ///** get definition count
        size_t Definitions() const { return definitions_;  }
        //* field offset for explicit structures
        void ExplicitOffset(size_t offset) { explicitOffset_ = offset;}
        //* field offset for explicit structures
        size_t ExplicitOffset() const { return explicitOffset_; }
        ///** Index in the fielddef table
        size_t PEIndex() const { return peIndex_; }
        void PEIndex(size_t val) { peIndex_ = val; }

        // internal functions
        bool InAssemblyRef() const { return parent_->InAssemblyRef(); }
        static bool ILSrcDumpTypeName(PELib &peLib, ValueSize size);
        virtual bool ILSrcDump(PELib &) const;
        virtual bool PEDump(PELib &);
        virtual void ObjOut(PELib &, int pass) const;
        static Field *ObjIn(PELib &, bool definition = true);
    protected:
        DataContainer *parent_;
        std::string name_;
        Qualifiers flags_;
        ValueMode mode_;
        Type *type_;
        union {
            longlong enumValue_;
            Byte *byteValue_;
        };
        int byteLength_;
        ValueSize size_;
        size_t peIndex_;
        size_t explicitOffset_;
        bool ref_;
        bool external_;
        size_t definitions_;
    };
    ///** A special kind of class: enum
    class Enum : public Class
    {
    public:
        Enum(const std::string& Name, Qualifiers Flags, Field::ValueSize Size) :
            size(Size), Class(Name, Flags.Flags() | Qualifiers::Value, -1, -1)
        {
        }
        ///** Add an enumeration, give it a name and a value
        // This creates the Field definition for the enumerated value
        Field *AddValue(Allocator &allocator, const std::string& Name, longlong Value);

        // internal functions
        virtual bool ILSrcDump(PELib &) const override;
        virtual bool PEDump(PELib &) override;
        virtual void ObjOut(PELib &, int pass) const override;
        static Enum *ObjIn(PELib &, bool definition = true);
    protected:
        Field::ValueSize size;
    };
    ///** the operand to an instruction 
    // this can contain a number, a string, or a reference to value
    // a value can be a field, methodsignature, local, or param reference
    //
    class Operand : public DestructorBase
    {
    public:
        enum OpSize { any, i8, u8, i16, u16, i32, u32, i64, u64, inative, r4, r8 };
        enum OpType { t_none, t_value, t_int, t_real, t_string, t_label };
        ///** Default constructor
        Operand() : type_(t_none), intValue_(0), sz_(i8), refValue_(nullptr), floatValue_(0), property_(0) // no operand
        {
        }
        ///** Operand is a complex value
        Operand(Value *V) : type_(t_value), refValue_(V), property_(false), sz_(i8), intValue_(0), floatValue_(0)
        {
        }
        ///** Operand is an integer constant
        Operand(longlong Value, OpSize Size) : type_(t_int), intValue_(Value), sz_(Size), refValue_(nullptr), floatValue_(0), property_(0)
        {
        }
        Operand(int Value, OpSize Size) : Operand((longlong)Value, Size) { }
        Operand(unsigned Value, OpSize Size) : Operand((longlong)Value, Size) { }
        ///** Operand is a floating point constant
        Operand(double Value, OpSize Size) : type_(t_real), floatValue_(Value), sz_(Size), intValue_(0), refValue_(nullptr), property_(0)
        {
        }
        ///** Operand is a string
        Operand(const std::string& Value, bool) : type_(t_string), intValue_(0), sz_(i8), refValue_(nullptr), floatValue_(0), property_(0) // string
        {
            stringValue_ = Value;
        }
        ///** Operand is a label
        Operand(const std::string& Value) : type_(t_label), intValue_(0), sz_(i8), refValue_(nullptr), floatValue_(0), property_(0) // label
        {
            stringValue_ = Value;
        }
        ///** Get type of operand
        OpType OperandType() const { return type_; }
        ///** When operand is a complex value, return it
        Value * GetValue() const { return type_ == t_value ? refValue_ : nullptr; }
        ///** return the int value
        longlong IntValue() const { return intValue_; }
        ///** return the string value
        std::string StringValue() const { return stringValue_; }
        ///** return the float value
        double FloatValue() const { return floatValue_; }
        ///** return the is-a-property flag
        ///** only has meaning for 'value' operands
        bool Property() const { return property_;  }
        ///** set the is-a-property flag
        void Property(bool state) { property_ = state;  }
        ///** Internal functions
        virtual bool ILSrcDump(PELib &) const;
        size_t Render(PELib &peLib, int opcode, int operandType, Byte *);
        virtual void ObjOut(PELib &, int pass) const;
        static Operand * ObjIn(PELib &);
        std::string EscapedString() const;
    protected:
        OpType type_;
        OpSize sz_;
        Value *refValue_;
        std::string stringValue_;
        longlong intValue_;
        double floatValue_;
        bool property_;
        bool isnanorinf() const;
    };
    /* a CIL instruction */
    class Instruction : public DestructorBase
    {
    public:

        // names of opcodes
        enum iop
        {
            ///** should never occur
            i_unknown,
            ///** This instruction is a placeholder for a label
            i_label,
            ///** This instruction is a placeholder for a comment
            i_comment,
            ///** This instruction is an SEH specifier
            i_SEH,
            ///** actual CIL instructions start here
            i_add, i_add_ovf, i_add_ovf_un, i_and, i_arglist, i_beq, i_beq_s, i_bge,
            i_bge_s, i_bge_un, i_bge_un_s, i_bgt, i_bgt_s, i_bgt_un, i_bgt_un_s, i_ble,
            i_ble_s, i_ble_un, i_ble_un_s, i_blt, i_blt_s, i_blt_un, i_blt_un_s, i_bne_un,
            i_bne_un_s, i_box, i_br, i_br_s, i_break, i_brfalse, i_brfalse_s, i_brinst,
            i_brinst_s, i_brnull, i_brnull_s, i_brtrue, i_brtrue_s, i_brzero, i_brzero_s, i_call,
            i_calli, i_callvirt, i_castclass, i_ceq, i_cgt, i_cgt_un, i_ckfinite, i_clt,
            i_clt_un, i_constrained_, i_conv_i, i_conv_i1, i_conv_i2, i_conv_i4, i_conv_i8, i_conv_ovf_i,
            i_conv_ovf_i_un, i_conv_ovf_i1, i_conv_ovf_i1_un, i_conv_ovf_i2, i_conv_ovf_i2_un, i_conv_ovf_i4, i_conv_ovf_i4_un, i_conv_ovf_i8,
            i_conv_ovf_i8_un, i_conv_ovf_u, i_conv_ovf_u_un, i_conv_ovf_u1, i_conv_ovf_u1_un, i_conv_ovf_u2, i_conv_ovf_u2_un, i_conv_ovf_u4,
            i_conv_ovf_u4_un, i_conv_ovf_u8, i_conv_ovf_u8_un, i_conv_r_un, i_conv_r4, i_conv_r8, i_conv_u, i_conv_u1,
            i_conv_u2, i_conv_u4, i_conv_u8, i_cpblk, i_cpobj, i_div, i_div_un, i_dup,
            i_endfault, i_endfilter, i_endfinally, i_initblk, i_initobj, i_isinst, i_jmp, i_ldarg,
            i_ldarg_0, i_ldarg_1, i_ldarg_2, i_ldarg_3, i_ldarg_s, i_ldarga, i_ldarga_s, i_ldc_i4,
            i_ldc_i4_0, i_ldc_i4_1, i_ldc_i4_2, i_ldc_i4_3, i_ldc_i4_4, i_ldc_i4_5, i_ldc_i4_6, i_ldc_i4_7,
            i_ldc_i4_8, i_ldc_i4_m1, i_ldc_i4_M1, i_ldc_i4_s, i_ldc_i8, i_ldc_r4, i_ldc_r8, i_ldelem,
            i_ldelem_i, i_ldelem_i1, i_ldelem_i2, i_ldelem_i4, i_ldelem_i8, i_ldelem_r4, i_ldelem_r8, i_ldelem_ref,
            i_ldelem_u1, i_ldelem_u2, i_ldelem_u4, i_ldelem_u8, i_ldelema, i_ldfld, i_ldflda, i_ldftn,
            i_ldind_i, i_ldind_i1, i_ldind_i2, i_ldind_i4, i_ldind_i8, i_ldind_r4, i_ldind_r8, i_ldind_ref,
            i_ldind_u1, i_ldind_u2, i_ldind_u4, i_ldind_u8, i_ldlen, i_ldloc, i_ldloc_0, i_ldloc_1,
            i_ldloc_2, i_ldloc_3, i_ldloc_s, i_ldloca, i_ldloca_s, i_ldnull, i_ldobj, i_ldsfld,
            i_ldsflda, i_ldstr, i_ldtoken, i_ldvirtftn, i_leave, i_leave_s, i_localloc, i_mkrefany,
            i_mul, i_mul_ovf, i_mul_ovf_un, i_neg, i_newarr, i_newobj, i_no_, i_nop,
            i_not, i_or, i_pop, i_readonly_, i_refanytype, i_refanyval, i_rem, i_rem_un,
            i_ret, i_rethrow, i_shl, i_shr, i_shr_un, i_sizeof, i_starg, i_starg_s,
            i_stelem, i_stelem_i, i_stelem_i1, i_stelem_i2, i_stelem_i4, i_stelem_i8, i_stelem_r4, i_stelem_r8,
            i_stelem_ref, i_stfld, i_stind_i, i_stind_i1, i_stind_i2, i_stind_i4, i_stind_i8, i_stind_r4,
            i_stind_r8, i_stind_ref, i_stloc, i_stloc_0, i_stloc_1, i_stloc_2, i_stloc_3, i_stloc_s,
            i_stobj, i_stsfld, i_sub, i_sub_ovf, i_sub_ovf_un, i_switch, i_tail_, i_throw,
            i_unaligned_, i_unbox, i_unbox_any, i_volatile_, i_xor
        };
        enum iseh { seh_try, seh_catch, seh_finally, seh_fault, seh_filter, seh_filter_handler };

        Instruction(iop Op, Operand *Operand);
        // for now only do comments and labels and branches...
        Instruction(iop Op, const std::string& Text) : op_(Op), text_(Text), switches_(nullptr), live_(false), sehType_(seh_try), sehBegin_(false), sehCatchType_(nullptr), offset_(0) { }

        Instruction(iseh type, bool begin, Type *catchType = NULL) : op_(i_SEH), switches_(nullptr), live_(false), sehType_(type), sehBegin_(begin), sehCatchType_(catchType), offset_(0) { }

        virtual ~Instruction() { if (switches_) delete switches_; }

        ///** Get the opcode
        iop OpCode() const { return op_; }
        ///** Set the opcode
        void OpCode(iop Op) { op_ = Op; }
        ///** Get the SEH Type
        int SEHType() const { return sehType_;  }
        ///** return true if it is a begin tag
        bool SEHBegin() const { return sehBegin_;  }
        ///** return the catch type
        Type *SEHCatchType() const { return sehCatchType_; }
        ///** Add a label for a SWITCH instruction
        ///** Labels MUST be added in order
        void AddCaseLabel(const std::string& label);
        ///** Get the set of case labels
        std::list<std::string> * GetSwitches() { return switches_; }
        ///** an 'empty' operand
        void NullOperand(Allocator &allocator);
        ///** Get the operand (CIL instructions have either zero or 1 operands)
        Operand *GetOperand() const { return operand_; }
        void SetOperand(Operand *operand) { operand_ = operand; }

        ///** Get text, e.g. for a comment
        std::string Text() const { return text_; }
        ///** Get the label name associated with the instruction
        std::string Label() const
        {
            if (operand_)
                return operand_->StringValue();
            else
                return "";
        }
        ///** The offset of the instruction within the method
        int Offset() const { return offset_; }
        ///** Set the offset of the instruction within the method
        void Offset(int Offset) { offset_ = Offset; }
        ///** Calculate length of instruction
        int InstructionSize();
        ///** get stack use for this instruction
        // positive means it adds to the stack, negative means it subtracts
        // 0 means it has no effect
        int StackUsage();
        ///** is a branch with a 4 byte relative offset
        int IsRel4() const { return instructions_[op_].operandType == o_rel4; }
        ///** is a branch with a 1 byte relative offset
        int IsRel1() const { return instructions_[op_].operandType == o_rel1; }
        ///** is any kind of branch
        int IsBranch() const { return IsRel1() || IsRel4(); }
        ///** Convert a 4-byte branch to a 1-byte branch
        void Rel4To1() { op_ = (iop)((int)op_ + 1); }
        ///** Is it any kind of call
        int IsCall() const {
            return op_ == i_call || op_ == i_calli || op_ == i_callvirt;
        }
        ///** Set the live flag.   We are checking for live because sometimes
        // dead code sequences can confuse the stack checking routine
        void Live(bool val) { live_ = val; }
        ///** is it live?
        bool IsLive() const { return live_; }

        // internal methods and structures
        virtual bool ILSrcDump(PELib &) const;
        size_t Render(PELib & peLib, Byte *, std::map<std::string, Instruction *> &labels);
        virtual void ObjOut(PELib &, int pass) const;
        static Instruction *ObjIn(PELib &);
        enum ioperand {
            o_none, o_single, o_rel1, o_rel4, o_index1, o_index2, o_index4,
            o_immed1, o_immed4, o_immed8, o_float4, o_float8, o_switch
        };
        struct InstructionName {
            const char *name;
            Byte op1;
            Byte op2;
            Byte bytes;
            Byte operandType;
            char stackUsage; // positive it adds to stack, negative it consumes stack
        };
    protected:
        std::list<std::string> *switches_;
        iop op_;
        int offset_;
        int sehType_;
        bool sehBegin_;
        union {
            Operand *operand_; // for non-labels
            Type *sehCatchType_;
        };
        std::string text_; // for comments
        bool live_;
        static InstructionName instructions_[];
    };

    ///** a value, typically to be used as an operand
    // various other classes derive from this to make specific types of operand values
    class Value : public DestructorBase
    {
    public:
        Value(const std::string& Name, Type *tp) : name_(Name), type_(tp) { }
        ///** return type of value
        Type *GetType() const { return type_; }
        ///** set type of value
        void SetType(Type *tp) { type_ = tp; }
        ///** return name
        const std::string &Name() const { return name_; }
        ///** set name
        void Name(const std::string name) { name_ = name; }

        ///** internal functions
        virtual bool ILSrcDump(PELib &) const;
        virtual size_t Render(PELib &peLib, int opcode, int OperandType, Byte *);
        virtual void ObjOut(PELib &, int pass) const;
        static Value *ObjIn(PELib &, bool definition = true);
    protected:
        std::string name_;
        Type *type_;
    };
    // value = local variable
    class Local : public Value
    {
    public:
        Local(const std::string& Name, Type *Tp) : Value(Name, Tp), uses_(0), index_(-1) { }

        ///** return index of variable
        int Index() const { return index_; }

        // internal functions
        void IncrementUses() { uses_++; }
        int Uses() const { return uses_; }
        void Index(int Index) { index_ = Index; }
        virtual bool ILSrcDump(PELib &) const override;
        virtual size_t Render(PELib &peLib, int opcode, int OperandType, Byte *) override;
        virtual void ObjOut(PELib &, int pass) const override;
        static Local *ObjIn(PELib &, bool definition = true);
    private:
        int index_;
        int uses_;
    };
    // value: a parameter
    // noticably missing is support for [in][out][opt] and default values
    class Param : public Value
    {
    public:
        Param(const std::string& Name, Type *Tp) : Value(Name, Tp), index_(-1) { }

        ///** return index of argument
        void Index(int Index) { index_ = Index; }

        // internal functions
        int Index() const { return index_; }
        virtual bool ILSrcDump(PELib &) const override;
        virtual size_t Render(PELib &peLib, int opcode, int OperandType, Byte *) override;
        virtual void ObjOut(PELib &, int pass) const override;
        static Param *ObjIn(PELib &, bool definition = true);
    private:
        int index_;
    };
    // value: a field name (used as an operand)
    class FieldName : public Value
    {
    public:
        ///** constructor.  Can be used to make the field a reference to another
        // assembly, in a rudimentary way
        FieldName(Field *F) : field_(F), Value("", nullptr)
        {
        }
        ///** Get the field reference
        Field *GetField() const { return field_; }

        // Internal functions
        virtual bool ILSrcDump(PELib &) const override;
        virtual size_t Render(PELib &peLib, int opcode, int OperandType, Byte *) override;
        virtual void ObjOut(PELib &, int pass) const override;
        static FieldName *ObjIn(PELib &, bool definition = true);
    protected:
        Field *field_;
    };
    // value: a method name (used as an operand)
    class MethodName : public Value
    {
    public:
        MethodName(MethodSignature *M);
        MethodSignature *Signature() const { return signature_; }
        virtual bool ILSrcDump(PELib &) const override;
        virtual size_t Render(PELib &peLib, int opcode, int OperandType, Byte *) override;
        virtual void ObjOut(PELib &, int pass) const override;
        static MethodName *ObjIn(PELib &, bool definition = true);
    protected:
        MethodSignature *signature_;
    };
    ///** the signature for a method, has a return type and a list of params.
    // params can be either named or unnamed
    // if the signature is not managed it is PINVOKE
    // There are two types of vararg protocols supported.
    // When performing a PINVOKE, the native CIL vararg mechanism is used
    // because that is how things are marshalled.   But if varars are used
    // in the arguments to a managed function, the argument list will end with
    // an argument which is an szarray of objects.  It will be tagged appropriately
    // so that other .net assemblies e.g. C# programs know how to use it as a param
    // list, including the ability to specify an arbitrary number of params.
    // When these are passed about in a program you generate you may need to box
    // simple values to fit them in the array...
    class MethodSignature : public DestructorBase
    {
    public:
        enum { Vararg = 1, Managed = 2, InstanceFlag = 4 };
        MethodSignature(const std::string& Name, int Flags, DataContainer *Container) : container_(Container), name_(Name), flags_(Flags), returnType_(nullptr), ref_(false), 
                peIndex_(0), peIndexCallSite_(0), peIndexType_(0), methodParent_(nullptr), arrayObject_(nullptr), external_(false), definitions_(0), genericParent_(nullptr), genericParamCount_(0)
        {
        }
        MethodSignature(const MethodSignature&) = default;
        MethodSignature& operator=(const MethodSignature&) = default;

        ///** Get return type
        Type *ReturnType() const { return returnType_; }
        ///** Set return type
        void ReturnType(Type *type)
        {
            returnType_ = type;
        }
        //* * Add a parameter.  They need to be added in order.
        void AddParam(Param *param)
        {
            if (varargParams_.size())
            {
                throw PELibError(PELibError::VarargParamsAlreadyDeclared);
            }
            param->Index(params.size());
            params.push_back(param);
        }
        ///** Add a vararg parameter.   These are NATIVE vararg parameters not
        // C# ones.   They are only added to signatures at a call site...
        void AddVarargParam(Param *param);
        ///** This is the parent declaration for a call site signature with vararg
        // params (the methoddef version of the signature)
        void SignatureParent(MethodSignature *parent) { methodParent_ = parent; }
        ///** return the parent declaration for a call site signature with vararg
        // params (the methoddef version of the signature)
        MethodSignature *SignatureParent() { return methodParent_; }
        ///** Set the data container
        void SetContainer(DataContainer *container) { container_ = container; }
        ///** Get the data container
        DataContainer *GetContainer() const { return container_; }
        ///** Get name
        const std::string &Name() const { return name_; }
        ///** Set name
        void SetName(const std::string& Name) { name_ = Name; }
        ///** Set Array object
        void ArrayObject(Type *tp) { arrayObject_ = tp; }
        // iterate through parameters
        typedef std::list<Param *>::iterator iterator;
        iterator begin() { return params.begin(); }
        iterator end() { return params.end(); }

        void GenericParent(MethodSignature* sig) { genericParent_ = sig; }
        MethodSignature* GenericParent() const { return genericParent_; }
        ///** return the list of generics
        std::deque<Type*>& Generic() { return generic_; }
        const std::deque<Type*>& Generic() const { return generic_; }

        // iterate through vararg parameters
        typedef std::list<Param *>::iterator viterator;
        iterator vbegin() { return varargParams_.begin(); }
        iterator vend() { return varargParams_.end(); }
        ///** make it an instance member
        void Instance(bool instance) { 
            if (instance)
            {
                flags_ |= InstanceFlag;
            }
            else
                flags_ &= ~InstanceFlag;
        }
        bool Instance() const { return !!(flags_ & InstanceFlag); }

        // make it virtual
        ///** make it a vararg signature
        void SetVarargFlag() { flags_ |= Vararg; }

        ///** return qualifiers
        int Flags() const { return flags_; }
        ///** return parameter count
        size_t ParamCount() const { return params.size(); }
        ///** return vararg parameter count
        size_t VarargParamCount() const { return varargParams_.size(); }
        ///** not locally defined
        bool External() const { return external_; }
        ///** not locally defined
        void External(bool external) { external_ = external; }
        ///** increment definition count
        void Definition() { definitions_++; }
        ///** get definition count
        size_t Definitions() const { return definitions_/2; }
        size_t GenericParamCount() const { return genericParamCount_; }
        void GenericParamCount(int count) { genericParamCount_ = count; }
        bool MatchesType(Type *tpa, Type *tpp);
        bool Matches(std::vector<Type *> args);
        // various indexes into metadata tables
        size_t PEIndex() const { return peIndex_; }
        void PEIndex(size_t val) { peIndex_ = val;  }
        size_t PEIndexCallSite() const { return peIndexCallSite_; }
        size_t PEIndexType() const { return peIndexType_; }

        // Load a signature
        void Load(PELib &lib, AssemblyDef &assembly, PEReader &reader, int start, int end);
        // internal functions
        void Ref(bool Ref) { ref_ = Ref; }
        bool IsRef() const { return ref_; }
        void ILSignatureDump(PELib &peLib);
        virtual bool ILSrcDump(PELib &, bool names, bool asType, bool PInvoke) const;
        virtual bool PEDump(PELib &, bool asType);
        virtual void ObjOut(PELib &, int pass) const;
        static MethodSignature *ObjIn(PELib &, Method **found, bool definition = true);
        std::string AdornGenerics(PELib& peLib, bool names = false) const;

    protected:
        MethodSignature *methodParent_;
        DataContainer *container_;
        Type *returnType_;
        Type *arrayObject_;
        std::string name_, display_name;
        int flags_;
        std::list<Param *> params, varargParams_;
        bool ref_;
        size_t peIndex_, peIndexCallSite_, peIndexType_;
        bool external_;
        size_t definitions_;
        std::deque<Type*> generic_;
        MethodSignature* genericParent_;
        int genericParamCount_;
    };
    ///** the type of a field or value
    class Type : public DestructorBase
    {
    public:
        enum BasicType {
            ///** type is a reference to a class
            cls,
            ///** type is a reference to a method signature
            method,
            ///** type is a generic variable
            var,
            ///** type is a generic method param
            mvar,
            /* below this is various CIL types*/
            Void, Bool, Char, i8, u8, i16, u16, i32, u32, i64, u64, inative, unative, r32, r64, object, string
        };
        Type(BasicType Tp, int PointerLevel) : tp_(Tp), arrayLevel_(0), byRef_(false), typeRef_(nullptr), methodRef_(nullptr), peIndex_(0), pinned_(false), showType_(false), varnum_(0)
        {
            if (Tp == var || Tp == mvar)
                varnum_ = PointerLevel;
            else
                pointerLevel_ = PointerLevel;
        }
        Type(DataContainer *clsref) : tp_(cls), pointerLevel_(0), arrayLevel_(0), byRef_(false), typeRef_(clsref), methodRef_(nullptr), peIndex_(0), pinned_(false), showType_(false), varnum_(0)
        {
        }
        Type(MethodSignature *methodref) : tp_(method), pointerLevel_(0), arrayLevel_(0), byRef_(false), typeRef_(nullptr),
            methodRef_(methodref), peIndex_(0), pinned_(false), showType_(false), varnum_(0)
        {
        }
        ///** Get the type of the Type object
        enum BasicType GetBasicType() const { return tp_; }
        ///** Set the type
        void SetBasicType(BasicType type) { tp_ = type; }
        ///** Get the class reference for class type objects
        DataContainer *GetClass() const { return typeRef_; }
        ///** Get the signature reference for method type objects
        MethodSignature *GetMethod() const { return methodRef_; }
        ///** Get the generic type container

        void ShowType() { showType_ = true; } 
        void ArrayLevel(int arrayLevel) { arrayLevel_ = arrayLevel;  }
        int ArrayLevel() const { return arrayLevel_;  }
        ///** Pointer indirection count
        void PointerLevel(int n) { pointerLevel_ = n; }
        ///** Pointer indirection count
        int PointerLevel() const { return pointerLevel_; }
        ///** Generic variable number
        void VarNum(int n) { varnum_ = n; }
        ///** Generic variable number
        int VarNum() const { return varnum_; }

        ///** ByRef flag
        void ByRef(bool val) { byRef_ = val; }
        ///** ByRef flag
        bool ByRef() { return byRef_; }

	///** Two types are an exact match
        bool Matches(Type *right);

        // internal functions
        virtual bool ILSrcDump(PELib &) const;
        virtual size_t Render(PELib &, Byte *);
        bool IsVoid() { return tp_ == Void && pointerLevel_ == 0; }
        size_t PEIndex() const { return peIndex_; }
        void PEIndex(size_t val) { peIndex_ = val; }
        virtual void ObjOut(PELib &, int pass) const;
        static Type *ObjIn(PELib &);
        bool Pinned() { return pinned_; }
        void Pinned(bool pinned) { pinned_ = pinned; }
    protected:
        bool pinned_;
        int pointerLevel_;
        int varnum_;
        bool byRef_;
        int  arrayLevel_;
        BasicType tp_;
        DataContainer *typeRef_;
        MethodSignature *methodRef_;
        size_t peIndex_;
        bool showType_;
    private:
        static const char *typeNames_[];
    };
    ///** A boxed type, e.g. the reference to a System::* object which
    // represents the basic type
    class BoxedType : public Type
    {
    public:
        BoxedType(BasicType Tp) : Type(Tp, 0)
        {
        }

        ///** internal functions
        virtual bool ILSrcDump(PELib &) const override;
        virtual size_t Render(PELib &, Byte *) override;
        virtual void ObjOut(PELib &, int pass) const override;
        static BoxedType *ObjIn(PELib &);
    private:
        static const char *typeNames_[];
    };

    ///** The callback structure is passed to 'traverse'... it holds callbacks
    // called while traversing the tree
    class Callback
    {
    public:
        virtual ~Callback() { }

        virtual bool EnterAssembly(const AssemblyDef *) { return true; }
        virtual bool ExitAssembly(const AssemblyDef *) { return true; }
        virtual bool EnterNamespace(const Namespace *) { return true; }
        virtual bool ExitNamespace(const Namespace *) { return true; }
        virtual bool EnterClass(const Class *) { return true; }
        virtual bool ExitClass(const Class *) { return true; }
        virtual bool EnterEnum(const Enum *) { return true; }
        virtual bool ExitEnum(const Enum *) { return true; }
        virtual bool EnterMethod(const Method *) { return true; }
        virtual bool EnterField(const Field *) { return true; }
        virtual bool EnterProperty(const Property *) { return true; }
    };
    ///** The allocator manages memory that various objects get constructed into
    // so that the objects can be cleanly deleted without the application having
    // to keep track of every object.
    class Allocator
    {
    public:
        Allocator() : first_(nullptr), current_(nullptr) { }
        virtual ~Allocator() { FreeAll(); }

        AssemblyDef *AllocateAssemblyDef(const std::string& Name, bool External, Byte *KeyToken = 0);
        Namespace *AllocateNamespace(const std::string& Name);
        Class *AllocateClass(const std::string& Name, Qualifiers Flags, int Pack, int Size);
        Class *AllocateClass(const Class* cls);
        Method *AllocateMethod(MethodSignature *Prototype, Qualifiers flags, bool entry = false);
        Field *AllocateField(const std::string& Name, Type *tp, Qualifiers Flags);
        Property *AllocateProperty();
        Property *AllocateProperty(PELib & peLib, const std::string& name, Type *type, std::vector<Type *>& indices, bool hasSetter = true, DataContainer *parent = nullptr);
        Enum *AllocateEnum(const std::string& Name, Qualifiers Flags, Field::ValueSize Size);
        Operand *AllocateOperand();
        Operand *AllocateOperand(Value *V);
        Operand *AllocateOperand(longlong Value, Operand::OpSize Size);
        Operand *AllocateOperand(int Value, Operand::OpSize Size) {
            return AllocateOperand((longlong)Value, Size);
        }
        Operand *AllocateOperand(unsigned Value, Operand::OpSize Size) {
            return AllocateOperand((longlong)Value, Size);
        }
        Operand *AllocateOperand(double Value, Operand::OpSize Size);
        Operand *AllocateOperand(const std::string& Value, bool);
        Operand *AllocateOperand(const std::string& Value);
        Instruction *AllocateInstruction(Instruction::iop Op, Operand *Operand = nullptr);
        Instruction *AllocateInstruction(Instruction::iop Op, const std::string& Text);
        Instruction *AllocateInstruction(Instruction::iseh type, bool begin, Type *catchType = NULL);
        Value *AllocateValue(const std::string& name, Type *tp);
        Local *AllocateLocal(const std::string& name, Type *tp);
        Param *AllocateParam(const std::string& name, Type *tp);
        FieldName *AllocateFieldName(Field *F);
        MethodName *AllocateMethodName(MethodSignature *M);
        MethodSignature *AllocateMethodSignature(const std::string& Name, int Flags, DataContainer *Container);
        MethodSignature *AllocateMethodSignature(const MethodSignature* sig);
        Type *AllocateType(Type::BasicType Tp, int PointerLevel);
        Type *AllocateType(DataContainer *clsref);
        Type *AllocateType(MethodSignature *methodref);
        BoxedType *AllocateBoxedType(Type::BasicType Tp);
        Byte *AllocateBytes(size_t sz);
        enum
        {
            AllocationSize = 0x100000,
        };
    private:
        // heap block
        struct Block
        {
            Block() : next_(nullptr), offset_(0) { memset(bytes_, 0, AllocationSize); }
            Block*next_;
            int offset_;
            Byte bytes_[AllocationSize];
        };
        void *BaseAlloc(size_t size);
        void FreeBlock(Block *b);
        void FreeAll();

        Block *first_, *current_;
    };
    ///** this is the main class to instantiate
    // the constructor creates a working assembly, you put all your code and data into that
    class PELib : public Allocator
    {
    public:
        enum eFindType {
            s_notFound=0,
            s_ambiguous=1,
            s_namespace,
            s_class,
            s_enum,
            s_field,
            s_property,
            s_method
        };
        enum CorFlags {
            ///** Set this for compatibility with .net assembly imports,
            // unset it for standalone assemblies if you want to modify your
            // sdata
            ilonly = 1,
            ///** Set this if you want to force 32 bits - you will possibly need it
            // for pinvokes
            bits32 = 2
        };
        enum OutputMode { ilasm, peexe, pedll, object };
        ///** Constructor, creates a working assembly
        PELib(const std::string& AssemblyName, int CoreFlags);
        ///** Get the working assembly
        // This is the one with your code and data, that gets written to the output
        AssemblyDef *WorkingAssembly() const { return assemblyRefs_.front(); }
        ///** replace the working assembly with an empty one.   Data is not deleted and
        /// still remains a part of the PELib instance.
        AssemblyDef *EmptyWorkingAssembly(const std::string& AssemblyName);
        ///** Add a reference to another assembly
        // this is an empty assembly you can put stuff in if you want to
        void AddExternalAssembly(const std::string& assemblyName, Byte *publicKeyToken = nullptr);
        ///** Load data out of an assembly
        int LoadAssembly(const std::string& assemblyName, int major = 0, int minor = 0, int build = 0, int revision = 0);
        ///* Load data out of an unmanaged DLL
        int LoadUnmanaged(const std::string& dllName);
        ///** Load an object file
        bool LoadObject(const std::string& name);
        ///** find an unmanaged dll name
        std::string FindUnmanagedName(const std::string& name);
        ///** Find an assembly
        AssemblyDef *FindAssembly(const std::string& assemblyName) const;
        ///** Find a Class
        Class *LookupClass(PEReader &reader, const std::string& assembly, int major, int minor, int build, int revision,
                           size_t keyIndex, const std::string& nameSpace, const std::string& name);
        ///** Pinvoke references are always added to this object
        void AddPInvokeReference(MethodSignature *methodsig, const std::string& dllname, bool iscdecl);
        void AddPInvokeWithVarargs(MethodSignature *methodsig) { pInvokeReferences_.insert(std::pair<std::string, MethodSignature *>(methodsig->Name(), methodsig)); }
        void RemovePInvokeReference(const std::string& name) {
            pInvokeSignatures_.erase(name);
        }
        Method *FindPInvoke(const std::string& name) const;
        MethodSignature *FindPInvokeWithVarargs(const std::string& name, std::vector<Param *>&vargs) const;
        // get the core flags
        int GetCorFlags() const { return corFlags_; }

        ///** write an output file, possibilities are a .il file, an EXE or a DLL
        // the file can also be tagged as either console or win32
        bool DumpOutputFile(const std::string& fileName, OutputMode mode, bool Gui);
        const std::string &FileName() const { return fileName_; }

        ///** add to the search path, returns true if it finds a namespace at path
        // in any assembly
        bool AddUsing(const std::string& path);

        ///** find something, return value tells what type of object was found
        eFindType Find(std::string path, void **result, std::deque<Type*>* generics = nullptr, AssemblyDef *assembly = nullptr);

        ///** find a method, with overload matching
        eFindType Find(std::string path, Method **result, std::vector<Type *> args, Type* rv = nullptr, std::deque<Type*>* generics = nullptr, AssemblyDef *assembly = nullptr, bool matchArgs = true);

        ///** Traverse the declaration tree
        void Traverse(Callback &callback) const;
                
        ///** internal declarations
        // loads the MSCorLib assembly
        AssemblyDef *MSCorLibAssembly();

        std::string FormatName(const std::string& name);
        std::string UnformatName();
        virtual bool ILSrcDump(PELib &) { return ILSrcDumpHeader() && ILSrcDumpFile(); }
        bool ObjOut();
        bool ObjIn();
        longlong ObjInt();
        int ObjHex2();
        char ObjBegin(bool next = true);
        char ObjEnd(bool next = true);
        char ObjChar();
        void ObjBack() { objInputPos_ -= 3; }
        void ObjReset() { objInputPos_ = objInputCache_; }
        void ObjError(int);
        std::iostream &Out() const { return *outputStream_; }
        void Swap(std::unique_ptr<std::iostream>& stream) { outputStream_.swap(stream); }
        PEWriter &PEOut() const { return *peWriter_; }
        std::map<size_t, size_t> moduleRefs;
        void PushContainer(DataContainer *container) { containerStack_.push_back(container); }
        DataContainer *GetContainer() { if (containerStack_.size()) return containerStack_.back(); else return nullptr; }
        void PopContainer() { containerStack_.pop_back(); }
        void SetCodeContainer(CodeContainer *container) { codeContainer_ = container; }
        CodeContainer *GetCodeContainer() const { return codeContainer_; }
        Class* FindOrCreateGeneric(std::string name, std::deque<Type*>& generics);
    protected:
        void SplitPath(std::vector<std::string> & split, std::string path);
        bool ILSrcDumpHeader();
        bool ILSrcDumpFile();
        bool DumpPEFile(std::string name, bool isexe, bool isgui);
        std::list<AssemblyDef *>assemblyRefs_;
        std::map<std::string, Method *>pInvokeSignatures_;
        std::multimap<std::string, MethodSignature *> pInvokeReferences_;
        std::string assemblyName_;
        std::unique_ptr<std::iostream> outputStream_;
        std::fstream *inputStream_;
        std::string fileName_;
    	std::map<std::string, std::string> unmanagedRoutines_;
        int corFlags_;
        PEWriter *peWriter_;
        std::vector<Namespace *> usingList_;
        std::deque<DataContainer *> containerStack_;
        CodeContainer *codeContainer_;
        const char *objInputBuf_;
        int objInputSize_;
        int objInputPos_;
        int objInputCache_;
    };

} // namespace
#endif // DOTNETPELIB_H