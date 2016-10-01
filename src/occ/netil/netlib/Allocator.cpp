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
#include "dotnetpelib.h"
#include <new>
namespace DotNetPELib {

    AssemblyDef *Allocator::AllocateAssemblyDef(std::string Name, bool External)
    {
        void *rv = BaseAlloc(sizeof(AssemblyDef));
        new (rv) AssemblyDef(Name, External);
        return static_cast<AssemblyDef *>(rv);
    }
    Namespace * Allocator::AllocateNamespace(std::string Name)
    {
        void *rv = BaseAlloc(sizeof(Namespace));
        new (rv) Namespace(Name);
        return static_cast<Namespace *>(rv);
    }
    Class *Allocator::AllocateClass (std::string Name , Qualifiers Flags, int Pack, int Size)
    {
        void *rv = BaseAlloc(sizeof(Class));
        new (rv) Class(Name, Flags, Pack, Size);
        return static_cast<Class *>(rv);
    }
    Method *Allocator::AllocateMethod(MethodSignature *Prototype, Qualifiers flags, bool entry) 
    {
        void *rv = BaseAlloc(sizeof(Method));
        new (rv) Method(Prototype, flags, entry);
        return static_cast<Method *>(rv);
    }
    Field *Allocator::AllocateField(std::string Name, Type *tp, Qualifiers Flags)
    {
        void *rv = BaseAlloc(sizeof(Field));
        new (rv) Field(Name, tp, Flags);
        return static_cast<Field *>(rv);
    }
    Enum *Allocator::AllocateEnum(std::string Name, Qualifiers Flags, Field::ValueSize Size)
    {
        void *rv = BaseAlloc(sizeof(Enum));
        new (rv) Enum(Name, Flags, Size);
        return static_cast<Enum *>(rv);
    }
    Operand *Allocator::AllocateOperand()
    {
        void *rv = BaseAlloc(sizeof(Operand));
        new (rv) Operand();
        return static_cast<Operand *>(rv);
    }
    Operand *Allocator::AllocateOperand(Value *V)
    {
        void *rv = BaseAlloc(sizeof(Operand));
        new (rv) Operand(V);
        return static_cast<Operand *>(rv);
    }
    Operand *Allocator::AllocateOperand(longlong Value, Operand::OpSize Size)
    {
        void *rv = BaseAlloc(sizeof(Operand));
        new (rv) Operand(Value, Size);
        return static_cast<Operand *>(rv);
    }
    Operand *Allocator::AllocateOperand(double Value, Operand::OpSize Size)
    {
        void *rv = BaseAlloc(sizeof(Operand));
        new (rv) Operand(Value, Size);
        return static_cast<Operand *>(rv);
    }
    Operand *Allocator::AllocateOperand(std::string Value, bool)
    {
        void *rv = BaseAlloc(sizeof(Operand));
        new (rv) Operand(Value, true);
        return static_cast<Operand *>(rv);
    }
    Operand *Allocator::AllocateOperand(std::string Value)
    {
        void *rv = BaseAlloc(sizeof(Operand));
        new (rv) Operand(Value);
        return static_cast<Operand *>(rv);
    }
    Instruction *Allocator::AllocateInstruction(Instruction::iop Op, Operand *Operand)
    {
        void *rv = BaseAlloc(sizeof(Instruction));
        new (rv) Instruction(Op, Operand);
        return static_cast<Instruction *>(rv);
    }
    Instruction *Allocator::AllocateInstruction (Instruction::iop Op, std::string Text)
    {
        void *rv = BaseAlloc(sizeof(Instruction));
        new (rv) Instruction(Op, Text);
        return static_cast<Instruction *>(rv);
    }
    Value *Allocator::AllocateValue(std::string Name, Type *Tp)
    {
        void *rv = BaseAlloc(sizeof(Value));
        new (rv) Value(Name, Tp);
        return static_cast<Value *>(rv);
    }
    Local *Allocator::AllocateLocal(std::string Name, Type *Tp)
    {
        void *rv = BaseAlloc(sizeof(Local));
        new (rv) Local(Name, Tp);
        return static_cast<Local *>(rv);
    }
    Param *Allocator::AllocateParam(std::string Name, Type *Tp)
    {
        void *rv = BaseAlloc(sizeof(Param));
        new (rv) Param(Name, Tp);
        return static_cast<Param *>(rv);
    }
    FieldName *Allocator::AllocateFieldName(Field *F, std::string Name, std::string Path)
    {
        void *rv = BaseAlloc(sizeof(FieldName));
        new (rv) FieldName(F, Name, Path);
        return static_cast<FieldName *>(rv);
    }
    MethodName *Allocator::AllocateMethodName(MethodSignature *M, std::string Name, std::string Path)
    {
        void *rv = BaseAlloc(sizeof(MethodName));
        new (rv) MethodName(M, Name, Path);
        return static_cast<MethodName *>(rv);
    }
    MethodSignature *Allocator::AllocateMethodSignature(std::string Name, int Flags, DataContainer *Container)
    {
        void *rv = BaseAlloc(sizeof(MethodSignature));
        new (rv) MethodSignature(Name, Flags, Container);
        return static_cast<MethodSignature *>(rv);
    }
    Type *Allocator::AllocateType(Type::BasicType Tp, int PointerLevel)
    {
        void *rv = BaseAlloc(sizeof(Type));
        new (rv) Type(Tp, PointerLevel);
        return static_cast<Type *>(rv);
    }
    Type *Allocator::AllocateType(DataContainer *clsref)
    {
        void *rv = BaseAlloc(sizeof(Type));
        new (rv) Type(clsref);
        return static_cast<Type *>(rv);
    }
    Type *Allocator::AllocateType(MethodSignature *methodref)
    {
        void *rv = BaseAlloc(sizeof(Type));
        new (rv) Type(methodref);
        return static_cast<Type *>(rv);
    }
    BoxedType *Allocator::AllocateBoxedType(Type::BasicType Tp)
    {
        void *rv = BaseAlloc(sizeof(BoxedType));
        new (rv) BoxedType(Tp);
        return static_cast<BoxedType *>(rv);
    }
    unsigned char *Allocator::AllocateBytes(size_t sz)
    {
        void *rv = BaseAlloc(sz);
        return static_cast<unsigned char *>(rv);
    }
    void *Allocator::BaseAlloc(size_t size)
    {
        // this assumes that size < the allocation pool size for a block
        size += sizeof(size_t);
        if (!current)
        {
            first = current = (Block *)malloc(sizeof(Block));
            if (!current)
                throw std::bad_alloc();
            new (current) Block();
        }
        else if (AllocationSize - current->offset < size)
        {
            current = current->next = (Block *)malloc(sizeof(Block));
            if (!current)
                throw std::bad_alloc();
            new (current) Block();
        }
        void *rv = &current->bytes[current->offset];
        size_t *s = (size_t *)rv;
        *s++ = size;
        rv = (void *)s;
        current->offset += size;
        return rv;
    }
    // have to destruct all objects in our local memory, because, some of them did
    // explicit memory allocation in the constructor
    void Allocator::FreeBlock(Block *b)
    {
        char *next = (char *)&b->bytes[0];
        while (next < (void **)&b->bytes[b->offset])
        {
            size_t add = *(size_t *)next;
            DestructorBase *d = reinterpret_cast<DestructorBase *>(next + sizeof(size_t));
            d->~DestructorBase();
            next += add;
        }
    }
    void Allocator::FreeAll()
    {
        while (first)
        {
            Block *next = first->next;
            FreeBlock(first);
            first = next;
        }
    }

}
