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
    void MethodSignature::ObjOut(PELib &peLib, int pass) const
    {
        if (pass == -1) // as a reference, we have to do a full signature because of overloads
                        // and here we need the fully qualified name
        {
            peLib.Out() << std::endl << "$sb" << peLib.FormatName(Qualifiers::GetObjName(name_, container_));
        }
        else
        {
            // as a definition...
            peLib.Out() << std::endl << "$sb" << peLib.FormatName(name_);
            peLib.Out() << external_ << ",";
            peLib.Out() << flags_ << ",";
        }
        returnType_->ObjOut(peLib, pass);
        for (auto p : params)
        {
            p->ObjOut(peLib, 1);
        }
        if (varargParams_.size())
        {
            peLib.Out() << std::endl << "$vb";
            for (auto p : varargParams_)
            {
                p->ObjOut(peLib, 1);
            }
            peLib.Out() << std::endl << "$ve";
        }
        peLib.Out() << std::endl << "$se";
    }
    MethodSignature *MethodSignature::ObjIn(PELib &peLib, Method **found, bool definition)
    {
        if (found)
            *found = nullptr;
        std::string name = peLib.UnformatName();
        int external;
        char ch;
        int flags=0;
        Type *returnType;
        if (definition)
        {
            external = peLib.ObjInt();
            ch = peLib.ObjChar();
            if (ch != ',')
                peLib.ObjError(oe_syntax);
            flags = peLib.ObjInt();
            ch = peLib.ObjChar();
            if (ch != ',')
                peLib.ObjError(oe_syntax);
        }
        returnType = Type::ObjIn(peLib);
        std::vector<Param *> args, vargs;
        while (peLib.ObjBegin() == 'p')
        {
            Param *p = Param::ObjIn(peLib);
            args.push_back(p);
        }
        if (peLib.ObjBegin(false) == 'v')
        {
            while (peLib.ObjBegin() == 'p')
            {
                Param *p = Param::ObjIn(peLib);
                vargs.push_back(p);
            }
            if (peLib.ObjEnd(false) != 'v')
                peLib.ObjError(oe_syntax);
            if (peLib.ObjEnd() != 's')
                peLib.ObjError(oe_syntax);
        }
        else
        {
            if (peLib.ObjEnd(false) != 's')
                peLib.ObjError(oe_syntax);
        }
        MethodSignature *rv =nullptr;
        std::vector<Type *>targs;
        for (auto p : args)
        {
            targs.push_back(p->GetType());
        }
        Method *pinvoke = peLib.FindPInvoke(name);
        if (pinvoke)
        {
            if (!vargs.size())
            {
                if (found)
                    *found = pinvoke;
                rv = pinvoke->Signature();
            }
            else
            {
                rv = peLib.AllocateMethodSignature(name, MethodSignature::Vararg, nullptr);
                rv->SignatureParent(pinvoke->Signature()); // tie it to the parent pinvoke
                rv->ReturnType(returnType);
                for (auto p : args)
                    rv->AddParam(p);
                for (auto v : vargs)
                    rv->AddVarargParam(v);
            }
        }
        else if (!peLib.GetContainer()) // defining a pinvoke
        {
            rv = peLib.AllocateMethodSignature(name, flags, nullptr);
            rv->ReturnType(returnType);
            for (auto p : args)
                rv->AddParam(p);
            for (auto v : vargs)
                rv->AddVarargParam(v);

        }
        else if (definition)
        {
            for (auto m : peLib.GetContainer()->Methods())
            {
                Method *c = static_cast<Method *>(m);
                if (c->Signature()->Name() == name && c->Signature()->Matches(targs))
                {
                    if (found)
                        *found = c;
                    rv = c->Signature();
                }

            }
            if (!rv)
            {
                rv = peLib.AllocateMethodSignature(name, flags,peLib.GetContainer());
                rv->ReturnType(returnType);
                for (auto p : args)
                    rv->AddParam(p);
                for (auto v : vargs)
                    rv->AddVarargParam(v);
            }
            else if (!rv->ReturnType()->Matches(returnType))
                peLib.ObjError(oe_typemismatch);

        }
        else
        {
            Method *m;
            if (peLib.Find(name, &m, targs) == PELib::s_method)
            {
                rv = m->Signature();
                if (found)
                    *found = m;
            }
            else
            {
                peLib.ObjError(oe_nomethod);
            }
        }
        return rv;
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