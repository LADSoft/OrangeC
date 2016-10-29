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
#include <stdio.h>
namespace DotNetPELib
{

    void MethodSignature::AddVarargParam(Param *param)
    {
        param->Index(params.size() + varargParams_.size());
        varargParams_.push_back(param);
    }
    bool MethodSignature::ILSrcDump(PELib &peLib, bool names, bool asType, bool PInvoke) const
    {
        // this usage of vararg is for C style varargs
        // occil uses C# style varags except in pinvoke and generates
        // the associated object array argument
        if ((flags_ & Vararg) && !(flags_ & Managed))
        {
            peLib.Out() << "vararg ";
        }
        if (flags_ & Instance)
        {
            peLib.Out() << "instance ";
        }
        if (returnType_->GetBasicType() == Type::cls)
        {
            if (returnType_->GetClass()->Flags().Flags() & Qualifiers::Value)
                peLib.Out() << "valuetype ";
            else
                peLib.Out() << "class ";
        }
        returnType_->ILSrcDump(peLib);
        peLib.Out() << " ";
        if (asType)
        {
            peLib.Out() << " *(";
        }
        else if (fullName_.size())
        {
            peLib.Out() << fullName_ << "(";
        }
        else if (name_.size())
        {
            if (names)
                peLib.Out() << "'" << name_ << "'(";
            else
                peLib.Out() << Qualifiers::GetName(name_, container_) << "(";
        }
        else
        {
            peLib.Out() << "(";
        }
        for (std::list<Param *>::const_iterator it = params.begin(); it != params.end();)
        {
            if ((*it)->GetType()->GetBasicType() == Type::cls)
            {
                if ((*it)->GetType()->GetClass()->Flags().Flags() & Qualifiers::Value)
                    peLib.Out() << "valuetype ";
                else
                    peLib.Out() << "class ";
            }
            else if ((*it)->GetType()->FullName().size())
                peLib.Out() << "class ";
            (*it)->GetType()->ILSrcDump(peLib);
            if (names)
                (*it)->ILSrcDump(peLib);
            ++it;
            if (it != params.end())
                peLib.Out() << ", ";
        }
        if (!PInvoke && (flags_ & Vararg))
        {
            if (!(flags_ & Managed))
            {
                peLib.Out() << ", ...";
                if (varargParams_.size())
                {
                    peLib.Out() << ", ";
                    for (std::list<Param *>::const_iterator it = varargParams_.begin(); it != varargParams_.end();)
                    {
                        (*it)->GetType()->ILSrcDump(peLib);
                        ++it;
                        if (it != varargParams_.end())
                            peLib.Out() << ", ";
                    }
                }
            }
        }
        peLib.Out() << ")";
        return true;
    }
    bool MethodSignature::PEDump(PELib &peLib, bool asType)
    {
        if (fullName_.size() != 0)
        {
            char assemblyName[1024], namespaceName[1024], className[1024], functionName[1024];
            if (sscanf(fullName_.c_str(), "[%[^]]]%[^.].%[^:]::%s", assemblyName, namespaceName, className, functionName) == 4)
            {
                AssemblyDef *assembly = peLib.FindAssembly(assemblyName);
                if (!assembly)
                {
                    peLib.AddExternalAssembly(assemblyName);
                    assembly = peLib.FindAssembly(assemblyName);
                    assembly->PEDump(peLib);
                }
                size_t nmspc = peLib.PEOut().HashString(namespaceName);
                size_t cls = peLib.PEOut().HashString(className);
                ResolutionScope rs(ResolutionScope::AssemblyRef, assembly->PEIndex());
                TableEntryBase *table = new TypeRefTableEntry(rs, cls, nmspc);
                peIndexCallSite_ = peLib.PEOut().AddTableEntry(table);
                size_t sz;
                Byte *sig = SignatureGenerator::MethodRefSig(this, sz);
                size_t methodSignature = peLib.PEOut().HashBlob(sig, sz);
                delete[] sig;
                if (functionName[0] == '\'')
                {
                    strcpy(functionName, functionName + 1);
                    functionName[strlen(functionName) - 1] = 0;
                }
                size_t function = peLib.PEOut().HashString(functionName);
                MemberRefParent memberRef(MemberRefParent::TypeRef, peIndexCallSite_);
                table = new MemberRefTableEntry(memberRef, function, methodSignature);
                peIndexCallSite_ = peLib.PEOut().AddTableEntry(table);
            }
        }
        else if (asType)
        {
            if (!peIndexType_)
            {
                size_t sz;
                Byte *sig = SignatureGenerator::MethodRefSig(this, sz);
                size_t methodSignature = peLib.PEOut().HashBlob(sig, sz);
                delete[] sig;
                TableEntryBase *table = new StandaloneSigTableEntry(methodSignature);
                peIndexType_ = peLib.PEOut().AddTableEntry(table);
            }
        }
        else if ((flags_ & Vararg) && !(flags_ & Managed))
        {
            size_t sz;
            size_t function = peLib.PEOut().HashString(name_);
            size_t parent = methodParent_ ? methodParent_->PEIndex() : 0;
            MemberRefParent memberRef(MemberRefParent::MethodDef, parent);
            Byte *sig = SignatureGenerator::MethodRefSig(this, sz);
            size_t methodSignature = peLib.PEOut().HashBlob(sig, sz);
            delete[] sig;
            TableEntryBase *table = new MemberRefTableEntry(memberRef, function, methodSignature);
            peIndexCallSite_ = peLib.PEOut().AddTableEntry(table);
        }
        else if (!peIndexCallSite_)
        {
            size_t sz;
            size_t function = peLib.PEOut().HashString(name_);
            size_t parent = container_ ? container_->ParentClass() : 0;
            MemberRefParent memberRef(MemberRefParent::TypeRef, parent);
            Byte *sig = SignatureGenerator::MethodRefSig(this, sz);
            size_t methodSignature = peLib.PEOut().HashBlob(sig, sz);
            delete[] sig;
            TableEntryBase *table = new MemberRefTableEntry(memberRef, function, methodSignature);
            peIndexCallSite_ = peLib.PEOut().AddTableEntry(table);
        }
        return true;
    }
}