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
    bool MethodSignature::Matches(std::vector<Type *> args)
    {
        // this is only designed for managed functions...
        if (args.size() ==params.size() || params.size() && args.size() >= params.size()-1 && (flags_ & Vararg))
        {
            auto it = params.begin();
            for (int i=0, n=0; i < args.size(); i++)
            {
                Type *tpa = args[i];
                Type *tpp = (*it)->GetType();
                if (!tpp)
                {
                    return false;
                }
                else if (tpa->GetBasicType() == tpp ->GetBasicType())
                {
                    // this may need to deal with boxed types a little better
                    if (tpa->GetBasicType() == Type::cls)
                        if (tpa->GetClass() != tpp->GetClass())
                            return false;
                }
                else
                {
                    return false;
                }
                if (tpa->PointerLevel() != tpp->PointerLevel() || tpa->ArrayLevel() != tpp->ArrayLevel())
                    return false;
                if (n < params.size()-1)
                    n++, ++it;
            }            
            return true;
        }
        return false;
    }

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
        if (flags_ & InstanceFlag)
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
        else if (name_.size())
        {
            if (arrayObject_)
            {
                arrayObject_->ILSrcDump(peLib);
                peLib.Out() << "::'" << name_ << "'(";
            }
            else if (names)
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
    void MethodSignature::ILSignatureDump(PELib &peLib)
    {
        returnType_->ILSrcDump(peLib);
        peLib.Out() << " ";
        peLib.Out() << Qualifiers::GetName(name_, container_) << "(";
        for (std::list<Param *>::const_iterator it = params.begin(); it != params.end();)
        {
            if ((*it)->GetType()->GetBasicType() == Type::cls)
            {
                if ((*it)->GetType()->GetClass()->Flags().Flags() & Qualifiers::Value)
                    peLib.Out() << "valuetype ";
                else
                    peLib.Out() << "class ";
            }
            (*it)->GetType()->ILSrcDump(peLib);
            ++it;
            if (it != params.end())
                peLib.Out() << ", ";
        }
        peLib.Out() << ")";

    }
    bool MethodSignature::PEDump(PELib &peLib, bool asType)
    {
        if (container_ && container_->InAssemblyRef())
        {
            if (!peIndexCallSite_)
            {
                container_->PEDump(peLib);
                if (returnType_ && returnType_->GetBasicType() == Type::cls)
                {
                    returnType_->GetClass()->PEDump(peLib);
                }
                for (auto param : params)
                {
                    if (param && param->GetType()->GetBasicType() == Type::cls)
                    {
                        param->GetType()->GetClass()->PEDump(peLib);
                    }
                }
                size_t sz;
                Byte *sig = SignatureGenerator::MethodRefSig(this, sz);
                size_t methodSignature = peLib.PEOut().HashBlob(sig, sz);
                delete[] sig;
                size_t function = peLib.PEOut().HashString(name_);
                MemberRefParent memberRef(MemberRefParent::TypeRef, container_->PEIndex());
                TableEntryBase *table = new MemberRefTableEntry(memberRef, function, methodSignature);
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
            int methodreftype = MemberRefParent::TypeRef;
            size_t sz;
            size_t function = peLib.PEOut().HashString(name_);
            size_t parent;
            if (arrayObject_)
            {
                methodreftype = MemberRefParent::TypeSpec;
                Byte buf[16];
                arrayObject_->Render(peLib, buf);
                parent = arrayObject_->PEIndex();
            }
            else
            {
                parent = container_ ? container_->ParentClass(peLib) : 0;
            }
            MemberRefParent memberRef(methodreftype, parent);
            Byte *sig = SignatureGenerator::MethodRefSig(this, sz);
            size_t methodSignature = peLib.PEOut().HashBlob(sig, sz);
            delete[] sig;
            TableEntryBase *table = new MemberRefTableEntry(memberRef, function, methodSignature);
            peIndexCallSite_ = peLib.PEOut().AddTableEntry(table);
        }
        return true;
    }
    void MethodSignature::Load(PELib &lib, AssemblyDef &assembly, PEReader &reader, int start, int end)
    {
        const DNLTable &table = reader.Table(tParam);
        if (start != end && start != table.size())
        {
            // check if the last attribute is an array...
            // we are just going to assume if the attribute exists the data is set properly
            CustomAttribute attribute(CustomAttribute::ParamDef, start + params.size() - 1);
            if (assembly.CustomAttributes().Has(attribute, "[mscorlib]System.ParamArrayAttribute"))
            {
                flags_ |= Vararg;
            }
        }
    }
}