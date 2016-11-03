/*
    Software License Agreement (BSD License)

    Copyright (c) 2016, David Lindauer, (LADSoft).
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
#include "DotNetPELib.h"
#include "PEFile.h"
#include <search.h>
#include <stdio.h>
#include <set>
#include <typeinfo>
namespace DotNetPELib
{
    Method::Method(MethodSignature *Prototype, Qualifiers flags, bool entry)
        : CodeContainer(flags), prototype_(Prototype), maxStack_(100),
        invokeMode_(CIL), pInvokeType_(Stdcall), entryPoint_(entry), rendering_(nullptr)
    {
        if (!(flags_.Flags() & Qualifiers::Static))
            prototype_->SetInstanceFlag();
    }
    void Method::AddLocal(Local *local)
    {
        local->Index(varList_.size());
        varList_.push_back(local);
    }

    bool Method::ILSrcDump(PELib &peLib) const
    {
        peLib.Out() << ".method";
        flags_.ILSrcDumpBeforeFlags(peLib);
        if (invokeMode_ == PInvoke)
        {
            peLib.Out() << " pinvokeimpl(\"" << pInvokeName_ << "\" "
                << (pInvokeType_ == Cdecl ? "cdecl) " : "stdcall) ");

        }
        else
        {
            peLib.Out() << " ";
        }
        prototype_->ILSrcDump(peLib, invokeMode_ != PInvoke, false, invokeMode_ == PInvoke);
        flags_.ILSrcDumpAfterFlags(peLib);
        if (invokeMode_ != PInvoke)
        {
            peLib.Out() << "{" << std::endl;
            if ((prototype_->Flags() & MethodSignature::Vararg) && (prototype_->Flags() & MethodSignature::Managed))
            {
                // allow C# to use ...
                peLib.Out() << "\t.param\t[" << prototype_->ParamCount() << "]" << std::endl;
                peLib.Out() << "\t.custom instance void [mscorlib]System.ParamArrayAttribute::.ctor() = ( 01 00 00 00 )" << std::endl;
            }
            if (varList_.size())
            {
                peLib.Out() << "\t.locals (" << std::endl;
                for (std::list<Local *>::const_iterator it = varList_.begin(); it != varList_.end();)
                {

                    peLib.Out() << "\t\t[" << (*it)->Index() << "]\t";
                    if ((*it)->GetType()->GetBasicType() == Type::cls)
                    {
                        if ((*it)->GetType()->GetClass()->Flags().Flags() & Qualifiers::Value)
                            peLib.Out() << "valuetype ";
                        else
                            peLib.Out() << "class ";
                    }
                    else if ((*it)->GetType()->FullName().size())
                    {
                        peLib.Out() << "class ";
                    }
                    (*it)->GetType()->ILSrcDump(peLib);
                    peLib.Out() << " ";
                    (*it)->ILSrcDump(peLib);
                    ++it;
                    if (it != varList_.end())
                        peLib.Out() << "," << std::endl;
                    else
                        peLib.Out() << std::endl;
                }
                peLib.Out() << "\t)" << std::endl;
            }
            if (entryPoint_)
                peLib.Out() << "\t.entrypoint" << std::endl;
            peLib.Out() << "\t.maxstack " << maxStack_ << std::endl << std::endl;
            CodeContainer::ILSrcDump(peLib);
            peLib.Out() << "}" << std::endl;
        }
        else
        {
            peLib.Out() << "{}" << std::endl;
        }
        return true;
    }
    bool Method::PEDump(PELib &peLib)
    {
        Byte *code;
        size_t sz;
        size_t methodSignature = 0;
        Byte *sig = nullptr;
        TableEntryBase *table;
        if (prototype_->ParamCount())
        {
            // assign an index to any params...
            for (auto it = prototype_->begin(); it != prototype_->end(); ++it)
            {
                auto param = *it;
                Type *tp = param->GetType();
                if (tp->FullName().size() || tp->GetBasicType() == Type::cls)
                {
                    if (!tp->PEIndex())
                    {
                        Byte buf[256];
                        tp->Render(peLib, buf);
                    }
                }

            }
        }
        if (varList_.size())
        {
            // assign type indexes to any types that haven't already been defined
            for (auto local : varList_)
            {
                Type *tp = local->GetType();
                if (tp->FullName().size() || tp->GetBasicType() == Type::cls)
                {
                    if (!tp->PEIndex())
                    {
                        Byte buf[256];
                        tp->Render(peLib, buf);
                    }
                }
            }
            sig = SignatureGenerator::LocalVarSig(this, sz);
            methodSignature = peLib.PEOut().HashBlob(sig, sz);
            table = new StandaloneSigTableEntry(methodSignature);
            methodSignature = peLib.PEOut().AddTableEntry(table);
        }
        Instruction *last = nullptr;
        if (instructions_.size())
            last = instructions_.back();
        rendering_ = new PEMethod((entryPoint_ ? PEMethod::EntryPoint : 0) | (invokeMode_ == CIL ? PEMethod::CIL : 0),
            peLib.PEOut().NextTableIndex(tMethodDef),
            maxStack_, varList_.size(),
            last ? last->Offset() + last->InstructionSize() : 0,
            methodSignature ? methodSignature | (tStandaloneSig << 24) : 0);
        if (invokeMode_ == CIL)
            peLib.PEOut().AddMethod(rendering_);
        delete[] sig;

        int implFlags = 0;
        int MFlags = 0;
        if (flags_.Flags() & Qualifiers::CIL)
            implFlags |= MethodDefTableEntry::IL;
        if (flags_.Flags() & Qualifiers::Managed)
            implFlags |= MethodDefTableEntry::Managed;
        if (flags_.Flags() & Qualifiers::PreserveSig)
            implFlags |= MethodDefTableEntry::PreserveSig;
        if (flags_.Flags() & Qualifiers::Public)
            MFlags |= MethodDefTableEntry::Public;
        else if (flags_.Flags() & Qualifiers::Private)
            MFlags |= MethodDefTableEntry::Private;
        if (flags_.Flags() & Qualifiers::Static)
            MFlags |= MethodDefTableEntry::Static;
        if (flags_.Flags() & Qualifiers::SpecialName)
            MFlags |= MethodDefTableEntry::SpecialName;
        if (flags_.Flags() & Qualifiers::RTSpecialName)
            MFlags |= MethodDefTableEntry::RTSpecialName;
        if (flags_.Flags() & Qualifiers::HideBySig)
            MFlags |= MethodDefTableEntry::HideBySig;
        if (invokeMode_ == PInvoke)
        {
            MFlags |= MethodDefTableEntry::PinvokeImpl;
        }
        size_t nameIndex = peLib.PEOut().HashString(prototype_->Name());
        size_t paramIndex = peLib.PEOut().NextTableIndex(tParam);

        sig = SignatureGenerator::MethodDefSig(prototype_, sz);
        methodSignature = peLib.PEOut().HashBlob(sig, sz);
        delete[] sig;

        table = new MethodDefTableEntry(rendering_, implFlags, MFlags,
            nameIndex, methodSignature, paramIndex);
        prototype_->SetPEIndex(peLib.PEOut().AddTableEntry(table));
        int i = 1;
        size_t lastParamIndex = 0;
        for (auto it = prototype_->begin(); it != prototype_->end(); ++it)
        {
            int flags = 0;
            size_t nameIndex = peLib.PEOut().HashString((*it)->Name());
            TableEntryBase *table = new ParamTableEntry(flags, i++, nameIndex);
            lastParamIndex = peLib.PEOut().AddTableEntry(table);
        }

        if (invokeMode_ == PInvoke)
        {
            int Flags = 0;
            //            Flags |= ImplMapTableEntry::CharSetAnsi;
            if (pInvokeType_ == Cdecl)
                Flags |= ImplMapTableEntry::CallConvCdecl;
            else
                Flags |= ImplMapTableEntry::CallConvStdcall;
            size_t importName = nameIndex;
            size_t moduleName = peLib.PEOut().HashString(pInvokeName_);
            size_t moduleRef = peLib.moduleRefs[moduleName];
            if (moduleRef == 0)
            {
                TableEntryBase *table = new ModuleRefTableEntry(moduleName);
                moduleRef = peLib.PEOut().AddTableEntry(table);
                peLib.moduleRefs[moduleName] = moduleRef;
            }
            MemberForwarded methodIndex(MemberForwarded::MethodDef, prototype_->PEIndex());
            ImplMapTableEntry *table = new ImplMapTableEntry(Flags, methodIndex, importName, moduleRef);
            peLib.PEOut().AddTableEntry(table);
        }
        if ((prototype_->Flags() & MethodSignature::Vararg) && (prototype_->Flags() & MethodSignature::Managed))
        {
            size_t attributeType = peLib.PEOut().ParamAttributeType();
            size_t attributeData = peLib.PEOut().ParamAttributeData();
            if (!attributeType && !attributeData)
            {
                AssemblyDef *assembly = peLib.MSCorLibAssembly(true);
                size_t ns = peLib.PEOut().HashString("System");
                size_t cls = peLib.PEOut().HashString("ParamArrayAttribute");
                size_t ctor = peLib.PEOut().HashString(".ctor");
                ResolutionScope rs(ResolutionScope::AssemblyRef, assembly->PEIndex());
                TableEntryBase *table = new TypeRefTableEntry(rs, cls, ns);
                size_t typeEntry = peLib.PEOut().AddTableEntry(table);
                static Byte ctorSig[] = { 0x20, 0, 1 };
                size_t ctor_sig = peLib.PEOut().HashBlob(ctorSig, sizeof(ctorSig));
                MemberRefParent parentClass(MemberRefParent::TypeRef, typeEntry);
                table = new MemberRefTableEntry(parentClass, ctor, ctor_sig);
                size_t ctor_index = peLib.PEOut().AddTableEntry(table);
                static Byte data[] = { 1, 0, 0, 0 };
                size_t data_sig = peLib.PEOut().HashBlob(data, sizeof(data));
                peLib.PEOut().ParamAttribute(ctor_index, data_sig);
                attributeType = peLib.PEOut().ParamAttributeType();
                attributeData = peLib.PEOut().ParamAttributeData();
            }
            CustomAttribute attribute(CustomAttribute::ParamDef, lastParamIndex);
            CustomAttributeType type(CustomAttributeType::MethodRef, attributeType);
            TableEntryBase *table = new CustomAttributeTableEntry(attribute, type, attributeData);
            peLib.PEOut().AddTableEntry(table);
        }
        return true;
    }
    void Method::Compile(PELib &peLib)
    {
        rendering_->code_ = CodeContainer::Compile(peLib, rendering_->codeSize_);
    }
    void Method::Optimize(PELib &peLib)
    {
        CalculateLive();
        CalculateMaxStack();
        OptimizeLocals(peLib);
        CodeContainer::Optimize(peLib);
    }
    void Method::CalculateLive()
    {
        std::set<std::string> labelsReached;
        bool done = false;
        bool skipping = false;
        while (!done)
        {
            done = true;
            for (auto instruction : instructions_)
            {
                if (!skipping)
                {
                    instruction->Live(true);
                    if (instruction->IsBranch())
                    {
                        if (labelsReached.find(instruction->GetOperand()->StringValue()) == labelsReached.end())
                        {
                            done = false;
                            labelsReached.insert(instruction->GetOperand()->StringValue());
                        }
                        if (instruction->OpCode() == Instruction::i_br)
                            skipping = true;
                    }
                    else if (instruction->OpCode() == Instruction::i_switch)
                    {
                        if (instruction->GetSwitches())
                        {
                            for (auto its = instruction->GetSwitches()->begin(); its != instruction->GetSwitches()->end(); ++its)
                            {
                                if (labelsReached.find(*its) == labelsReached.end())
                                {
                                    done = false;
                                    labelsReached.insert(*its);
                                }
                            }
                        }
                    }
                }
                else if (instruction->OpCode() == Instruction::i_label)
                {
                    if (labelsReached.find(instruction->Label()) != labelsReached.end())
                    {
                        instruction->Live(true);
                        skipping = false;
                    }
                }
            }
        }
    }
    void Method::CalculateMaxStack()
    {
        std::map<std::string, int> labels;
        maxStack_ = 0;
        int n = 0;
        bool lastBranch = false;
        bool skipping = false;
        for (auto instruction : instructions_)
        {
            if (instruction->IsLive())
            {
                int m = instruction->StackUsage();
                if (m == -127)
                    n = 0;
                else
                    n += m;
                if (n > maxStack_)
                    maxStack_ = n;
                if (n < 0)
                {
                    throw PELibError(PELibError::StackUnderflow);
                }
                if (instruction->IsBranch())
                {
                    lastBranch = true;
                    auto it1 = labels.find(instruction->GetOperand()->StringValue());
                    if (it1 != labels.end())
                    {
                        if (it1->second != n)
                        {
                            throw PELibError(PELibError::MismatchedStack, instruction->GetOperand()->StringValue());
                        }
                    }
                    else
                    {
                        labels[instruction->GetOperand()->StringValue()] = n;
                    }

                }
                else if (instruction->OpCode() == Instruction::i_switch)
                {
                    if (instruction->GetSwitches())
                    {
                        for (auto its = instruction->GetSwitches()->begin(); its != instruction->GetSwitches()->end(); ++its)
                        {
                            auto it1 = labels.find(*its);
                            if (it1 != labels.end())
                            {
                                if (it1->second != n)
                                {
                                    throw PELibError(PELibError::MismatchedStack, *its);
                                }
                            }
                            else
                            {
                                labels[*its] = n;
                            }
                        }
                    }
                }
                else if (instruction->OpCode() == Instruction::i_label)
                {
                    if (lastBranch)
                    {
                        auto it1 = labels.find(instruction->Label());
                        if (it1 != labels.end())
                        {
                            n = it1->second;
                        }
                        else
                        {
                            n = 0;
                        }
                    }
                    else
                    {
                        auto it1 = labels.find(instruction->Label());
                        if (it1 != labels.end())
                        {
                            if (it1->second != n)
                            {
                                throw PELibError(PELibError::MismatchedStack, instruction->Label());
                            }
                        }
                        else
                        {
                            labels[instruction->Label()] = n;
                        }
                    }

                }
                else if (instruction->OpCode() == Instruction::i_comment)
                {
                    // placeholder
                }
                else
                {
                    lastBranch = false;
                }
            }
        }
        if (n != 0)
        {
            if (n != 1 || prototype_->ReturnType()->IsVoid())
                throw PELibError(PELibError::StackNotEmpty, " at end of function");
        }
    }
    static int localCompare(const void *left, const void *right)
    {
        Local **l = (Local **)left;
        Local **r = (Local **)right;
        // sort from highest to lowest
        if ((*l)->Uses() > (*r)->Uses())
            return -1;
        return (*l)->Uses() < (*r)->Uses();
    }
    void Method::OptimizeLocals(PELib &peLib)
    {
        for (auto instruction : instructions_)
        {
            Operand *op = instruction->GetOperand();
            if (op)
            {
                Value *v = op->GetValue();
                if (v && typeid(*v) == typeid(Local))
                {
                    ((Local *)v)->IncrementUses();
                }
            }
        }
        int n = varList_.size();
        Local **vars = new Local *[n];
        int i = 0;
        for (auto local : varList_)
        {
            vars[i++] = local;
        }
        varList_.clear();
        qsort(vars, n, sizeof(Local *), localCompare);
        for (int i = 0; i < n; i++)
        {
            vars[i]->Index(i);
            varList_.push_back(vars[i]);
        }
        delete[] vars;
    }
}