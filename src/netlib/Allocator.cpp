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

#include "DotNetPELib.h"
#include <new>
namespace DotNetPELib
{

AssemblyDef* Allocator::AllocateAssemblyDef(const std::string& Name, bool External, Byte* KeyToken)
{
    void* rv = BaseAlloc(sizeof(AssemblyDef));
    new (rv) AssemblyDef(Name, External, KeyToken);
    return static_cast<AssemblyDef*>(rv);
}
Namespace* Allocator::AllocateNamespace(const std::string& Name)
{
    void* rv = BaseAlloc(sizeof(Namespace));
    new (rv) Namespace(Name);
    return static_cast<Namespace*>(rv);
}
Class* Allocator::AllocateClass(const std::string& Name, Qualifiers Flags, int Pack, int Size)
{
    void* rv = BaseAlloc(sizeof(Class));
    new (rv) Class(Name, Flags, Pack, Size);
    return static_cast<Class*>(rv);
}
Class* Allocator::AllocateClass(const Class* cls)
{
    void* rv = BaseAlloc(sizeof(Class));
    new (rv) Class(*cls);
    return static_cast<Class*>(rv);
}
Method* Allocator::AllocateMethod(MethodSignature* Prototype, Qualifiers flags, bool entry)
{
    void* rv = BaseAlloc(sizeof(Method));
    new (rv) Method(Prototype, flags, entry);
    return static_cast<Method*>(rv);
}
Field* Allocator::AllocateField(const std::string& Name, Type* tp, Qualifiers Flags)
{
    void* rv = BaseAlloc(sizeof(Field));
    new (rv) Field(Name, tp, Flags);
    return static_cast<Field*>(rv);
}
Property* Allocator::AllocateProperty()
{
    void* rv = BaseAlloc(sizeof(Property));
    new (rv) Property();
    return static_cast<Property*>(rv);
}
Property* Allocator::AllocateProperty(PELib& peLib, const std::string& name, Type* type, std::vector<Type*>& indices,
                                      bool hasSetter, DataContainer* parent)
{
    void* rv = BaseAlloc(sizeof(Property));
    new (rv) Property(peLib, name, type, indices, hasSetter, parent);
    return static_cast<Property*>(rv);
}
Enum* Allocator::AllocateEnum(const std::string& Name, Qualifiers Flags, Field::ValueSize Size)
{
    void* rv = BaseAlloc(sizeof(Enum));
    new (rv) Enum(Name, Flags, Size);
    return static_cast<Enum*>(rv);
}
Operand* Allocator::AllocateOperand()
{
    void* rv = BaseAlloc(sizeof(Operand));
    new (rv) Operand();
    return static_cast<Operand*>(rv);
}
Operand* Allocator::AllocateOperand(Value* V)
{
    void* rv = BaseAlloc(sizeof(Operand));
    new (rv) Operand(V);
    return static_cast<Operand*>(rv);
}
Operand* Allocator::AllocateOperand(longlong Value, Operand::OpSize Size)
{
    void* rv = BaseAlloc(sizeof(Operand));
    new (rv) Operand(Value, Size);
    return static_cast<Operand*>(rv);
}
Operand* Allocator::AllocateOperand(double Value, Operand::OpSize Size)
{
    void* rv = BaseAlloc(sizeof(Operand));
    new (rv) Operand(Value, Size);
    return static_cast<Operand*>(rv);
}
Operand* Allocator::AllocateOperand(const std::string& Value, bool)
{
    void* rv = BaseAlloc(sizeof(Operand));
    new (rv) Operand(Value, true);
    return static_cast<Operand*>(rv);
}
Operand* Allocator::AllocateOperand(const std::string& Value)
{
    void* rv = BaseAlloc(sizeof(Operand));
    new (rv) Operand(Value);
    return static_cast<Operand*>(rv);
}
Instruction* Allocator::AllocateInstruction(Instruction::iop Op, Operand* Operand)
{
    void* rv = BaseAlloc(sizeof(Instruction));
    new (rv) Instruction(Op, Operand);
    return static_cast<Instruction*>(rv);
}
Instruction* Allocator::AllocateInstruction(Instruction::iop Op, const std::string& Text)
{
    void* rv = BaseAlloc(sizeof(Instruction));
    new (rv) Instruction(Op, Text);
    return static_cast<Instruction*>(rv);
}
Instruction* Allocator::AllocateInstruction(Instruction::iseh type, bool begin, Type* catchType)
{
    void* rv = BaseAlloc(sizeof(Instruction));
    new (rv) Instruction(type, begin, catchType);
    return static_cast<Instruction*>(rv);
}
Value* Allocator::AllocateValue(const std::string& Name, Type* Tp)
{
    void* rv = BaseAlloc(sizeof(Value));
    new (rv) Value(Name, Tp);
    return static_cast<Value*>(rv);
}
Local* Allocator::AllocateLocal(const std::string& Name, Type* Tp)
{
    void* rv = BaseAlloc(sizeof(Local));
    new (rv) Local(Name, Tp);
    return static_cast<Local*>(rv);
}
Param* Allocator::AllocateParam(const std::string& Name, Type* Tp)
{
    void* rv = BaseAlloc(sizeof(Param));
    new (rv) Param(Name, Tp);
    return static_cast<Param*>(rv);
}
FieldName* Allocator::AllocateFieldName(Field* F)
{
    void* rv = BaseAlloc(sizeof(FieldName));
    new (rv) FieldName(F);
    return static_cast<FieldName*>(rv);
}
MethodName* Allocator::AllocateMethodName(MethodSignature* M)
{
    void* rv = BaseAlloc(sizeof(MethodName));
    new (rv) MethodName(M);
    return static_cast<MethodName*>(rv);
}
MethodSignature* Allocator::AllocateMethodSignature(const std::string& Name, int Flags, DataContainer* Container)
{
    void* rv = BaseAlloc(sizeof(MethodSignature));
    new (rv) MethodSignature(Name, Flags, Container);
    return static_cast<MethodSignature*>(rv);
}
MethodSignature* Allocator::AllocateMethodSignature(const MethodSignature* sig)
{
    void* rv = BaseAlloc(sizeof(MethodSignature));
    new (rv) MethodSignature(*sig);
    return static_cast<MethodSignature*>(rv);
}
Type* Allocator::AllocateType(Type::BasicType Tp, int PointerLevel)
{
    void* rv = BaseAlloc(sizeof(Type));
    new (rv) Type(Tp, PointerLevel);
    return static_cast<Type*>(rv);
}
Type* Allocator::AllocateType(DataContainer* clsref)
{
    void* rv = BaseAlloc(sizeof(Type));
    new (rv) Type(clsref);
    return static_cast<Type*>(rv);
}
Type* Allocator::AllocateType(MethodSignature* methodref)
{
    void* rv = BaseAlloc(sizeof(Type));
    new (rv) Type(methodref);
    return static_cast<Type*>(rv);
}
BoxedType* Allocator::AllocateBoxedType(Type::BasicType Tp)
{
    void* rv = BaseAlloc(sizeof(BoxedType));
    new (rv) BoxedType(Tp);
    return static_cast<BoxedType*>(rv);
}
Byte* Allocator::AllocateBytes(size_t sz)
{
    void* rv = BaseAlloc(sz);
    *((DWord*)rv - 1) |= 1;
    return static_cast<Byte*>(rv);
}
void* Allocator::BaseAlloc(size_t size)
{
    // this assumes that size < the allocation pool size for a block
    size += sizeof(size_t);
    if (!current_)
    {
        first_ = current_ = (Block*)malloc(sizeof(Block));
        if (!current_)
            throw std::bad_alloc();
        new (current_) Block();
    }
    else if (AllocationSize - current_->offset_ < size)
    {
        current_ = current_->next_ = (Block*)malloc(sizeof(Block));
        if (!current_)
            throw std::bad_alloc();
        new (current_) Block();
    }
    void* rv = &current_->bytes_[current_->offset_];
    size_t* s = (size_t*)rv;
    *s++ = size;
    rv = (void*)s;
    current_->offset_ += size;
    return rv;
}
// have to destruct all objects in our local memory, because, some of them did
// explicit memory allocation in the constructor
void Allocator::FreeBlock(Block* b)
{
    char* next = (char*)&b->bytes_[0];
    while (next < (char*)&b->bytes_[b->offset_])
    {
        size_t add = *(size_t*)next;
        if (add & 1)
        {
            add--;
        }
        else
        {
            DestructorBase* d = reinterpret_cast<DestructorBase*>(next + sizeof(size_t));
            d->~DestructorBase();
        }
        next += add;
    }
    free(b);
}
void Allocator::FreeAll()
{
    while (first_)
    {
        Block* next = first_->next_;
        FreeBlock(first_);
        first_ = next;
    }
}

}  // namespace DotNetPELib
