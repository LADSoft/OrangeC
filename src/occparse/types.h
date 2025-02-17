/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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
#pragma once

namespace Parser
{

struct Type
{
    BasicType type;        /* the type */
    long size;             /* total size of type */
    Type* btp;       /* pointer to next type (pointers & arrays */
    Type* rootType;  /* pointer to base type of sequence */
    int used : 1;          /* type has actually been used in a declaration or cast or expression */
    int array : 1;         /* not a dereferenceable pointer */
    int msil : 1;          /* allocate as an MSIL array */
    int byRefArray : 1;    /* array base address is a reference type */
    int vla : 1;           /* varriable length array */
    int unsized : 1;       /* type doesn't need a size */
    int hasbits : 1;       /* type is a bit type */
    int anonymousbits : 1; /* type is a bit type without a name */
    int scoped : 1;        /* c++ scoped enumeration */
    int fixed : 1;         /* c++ fixed enumeration */
    int nullptrType : 1;   /* c++: std::nullptr */
    int templateTop : 1;
    int enumConst : 1; /* is an enumeration constant */
    int lref : 1;
    int rref : 1;
    int decltypeauto : 1;
    int stringconst : 1;
    char bits;      /* -1 for not a bit val, else bit field len */
    char startbit;  /* start of bit field */
    int bitintbits;
    struct sym* sp; /* pointer to a symbol which describes the type */
    /* local symbol tables */
    SymbolTable<struct sym>* syms; /* Symbol table for structs & functions */
    SymbolTable<struct sym>* tags; /* Symbol table for nested types*/
    union
    {
        TEMPLATEPARAMPAIR* templateParam;
        std::list<TEMPLATEPARAMPAIR> *templateArgs;
    };
    int alignment;                /* alignment pref for this structure/class/union   */
    int elements;         /* number of elements or zero for an array whose size isn't known at compile time */
    EXPRESSION* esize;            /* enode version of size */
    Type* etype;            /* type of size field  when size isn't constant */
    int vlaindex;                 /* index into the vararray */
    EXPRESSION* templateDeclType; /* for BasicType::templatedecltype_, used in templates */
    Type* typedefType;      /* The typedef which describes this type */

    CONSTEXPR inline Type* BaseType()
    {
        void* aa = this;
        return aa ? this->rootType : nullptr;
    }
    void UpdateRootTypes();
    bool IsAutoType();
    bool IsUnsigned();
    bool IsBitInt();
    bool IsInt();
    bool IsFloat();
    bool IsComplex();
    bool IsImaginary();
    bool IsArithmetic();
    bool IsMsil();
    bool IsConst();
    bool IsVolatile();
    bool IsLRefQual();
    bool IsRRefQual();
    bool IsRestrict();
    bool IsAtomic();
    bool IsVoid();
    bool IsVoidPtr();
    bool IsArray();
    bool IsUnion();
    bool IsDeferred(bool sym = false);
    bool IsRef();
    bool IsPtr();
    bool IsFunction();

    bool IsFunctionPtr();
    bool IsStructured();
    bool IsTemplatedPointer();
    bool IsStructuredMath(Type* tp2 = nullptr);
    bool IsSmallInt();
    bool IsLargeEnum();
    bool IsDiffered();
    void ToString(char* buf);
    bool SameExceptionType(Type* typ);
    bool SameCharType(Type* tp);
    bool SameType(Type* other) { return CompareTypes(this, other, 0); }
    bool CompatibleType(Type* other) { return CompareTypes(this, other, 1); }
    bool CompatibleTypeNoQualifiers(Type* other) { return CompareTypes(this, other, 2); }
    inline void BasicTypeToString(char* buf) { BasicTypeToString(buf, this); }
    Type* CopyType(bool deep = false, std::function<void(Type*&, Type*&)> callback = nullptr);
    bool IsConstWithArr();
    bool SameIntegerType(Type* t2);
    Type* InitializerListType();
    bool InstantiateDeferred(bool noErr= false);
    Type* InitializeDeferred();
    static Type* MakeType(Type& tp, BasicType type, Type* base = nullptr);
    static Type* MakeType(BasicType type, Type* base = nullptr);
    static Type* MakeType(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args);
    static bool CompareTypes(Type* typ1, Type* typ2, int exact);
protected:
    static void ToString(char* buf, Type* typ);
    static char* PutPointer(char* p, Type* tp);
    static void PointerToString(char* buf, Type* tp);
    static Type* QualifierToString(char* buf, Type* tp);
    static Type* BasicTypeToString(char* buf, Type* tp);
    static void RenderExpr(char* buf, EXPRESSION* exp);

};
struct TypeGenerator
{
    static Type* PointerQualifiers(LexList*& lex, Type* tp, bool allowstatic);
    static Type* FunctionParams(LexList*& lex, SYMBOL* funcsp, SYMBOL** spin, Type* tp, bool inTemplate, StorageClass storage_class,
        bool funcptr);
    static Type* BeforeName(LexList*& lex, SYMBOL* funcsp, Type* tp, SYMBOL** spi, SYMBOL** strSym, std::list<NAMESPACEVALUEDATA*>** nsv,
        bool inTemplate, StorageClass storage_class, Linkage* linkage, Linkage* linkage2, Linkage* linkage3,
        bool* notype, bool asFriend, int consdest, bool beforeOnly, bool funcptr);
    static Type* UnadornedType(LexList*& lex, SYMBOL* funcsp, Type* tp, SYMBOL** strSym_out, bool inTemplate, StorageClass storage_class,
        Linkage* linkage_in, Linkage* linkage2_in, Linkage* linkage3_in, AccessLevel access, bool* notype,
        bool* defd, int* consdest, bool* templateArg, bool* deduceTemplate, bool isTypedef, bool templateErr, bool inUsing, bool* asfriend,
        bool constexpression);
    static Type* TypeId(LexList*& lex, SYMBOL* funcsp, StorageClass storage_class, bool beforeOnly, bool toErr, bool inUsing);
    static bool StartOfType(LexList*& lex, bool* structured, bool assumeType);
private:
    static void ExceptionSpecifiers(LexList*& lex, SYMBOL* funcsp, SYMBOL* sp, StorageClass storage_class);
    static Type* FunctionQualifiersAndTrailingReturn(LexList*& lex, SYMBOL* funcsp, SYMBOL** sp, Type* tp, StorageClass storage_class);
    static void ResolveVLAs(Type* tp);
    static Type* ArrayType(LexList*& lex, SYMBOL* funcsp, Type* tp, StorageClass storage_class, bool* vla, Type** quals, bool first,
        bool msil);
    static Type* AfterName(LexList*& lex, SYMBOL* funcsp, Type* tp, SYMBOL** sp, bool inTemplate, StorageClass storage_class,
        int consdest, bool funcptr);
};
bool istype(SYMBOL* sym);
}  // namespace Parser