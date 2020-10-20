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
#include "PEFile.h"
#include <stdio.h>
#include <sstream>

namespace DotNetPELib
{
bool MethodSignature::MatchesType(Type *tpa, Type *tpp)
{
    if (!tpp)
    {
        return false;
    }
    else if (tpp->GetBasicType() == Type::var)
    {
        // nothing to do, it matches...
    }
    else if (tpp->GetBasicType() == Type::mvar)
    {
        // nothing to do, it matches...
    }
    else if (tpa->GetBasicType() == tpp->GetBasicType())
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
    if ((tpa->PointerLevel() != tpp->PointerLevel() && tpp->PointerLevel() != 1 && tpp->GetBasicType() != Type::Void) || tpa->ArrayLevel() != tpp->ArrayLevel())
        return false;
    return true;
}
bool MethodSignature::Matches(std::vector<Type*> args)
{
    // this is only designed for managed functions...
    if (args.size() == params.size() || (params.size() && args.size() >= params.size() - 1 && (flags_ & Vararg)))
    {
        auto it = params.begin();
        for (int i = 0, n = 0; i < args.size(); i++)
        {
            Type* tpa = args[i];
            Type* tpp = (*it)->GetType();
            if (!MatchesType(tpa, tpp))
                return false;
            if (n < params.size() - 1)
                n++, ++it;
        }
        return true;
    }
    return false;
}

void MethodSignature::AddVarargParam(Param* param)
{
    param->Index(params.size() + varargParams_.size());
    varargParams_.push_back(param);
}
bool MethodSignature::ILSrcDump(PELib& peLib, bool names, bool asType, bool PInvoke) const
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
        peLib.Out() << " *";
    }
    else if (name_.size())
    {
        if (arrayObject_)
        {
            arrayObject_->ILSrcDump(peLib);
            peLib.Out() << "::'" << name_ << "'";
        }
        else if (names)
        {
            peLib.Out() << "'" << name_ << "'";
        }
        else
        {
            if (container_ && typeid(*container_) == typeid(Class) && static_cast<Class*>(container_)->Generic().size())
            {
                if (container_->Flags().Flags() & Qualifiers::Value)
                    peLib.Out() << "valuetype ";
                else
                    peLib.Out() << "class ";
                peLib.Out() << Qualifiers::GetName("", container_);
                peLib.Out() << static_cast<Class*>(container_)->AdornGenerics(peLib);
                peLib.Out() << "::'" << name_ << "'";
            }
            else
            {
                peLib.Out() << Qualifiers::GetName(name_, container_);
            }
        }
    }
    peLib.Out() << AdornGenerics(peLib) << "(";
    for (std::list<Param*>::const_iterator it = params.begin(); it != params.end();)
    {
        if ((*it)->GetType()->GetBasicType() == Type::cls)
        {
            if ((*it)->GetType()->GetClass()->Flags().Flags() & Qualifiers::Value)
                peLib.Out() << "valuetype ";
            else
                peLib.Out() << "class ";
        }
        (*it)->GetType()->ILSrcDump(peLib);
        if (names && (*it)->GetType()->GetBasicType() != Type::var && (*it)->GetType()->GetBasicType() != Type::mvar)
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
                for (std::list<Param*>::const_iterator it = varargParams_.begin(); it != varargParams_.end();)
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
void MethodSignature::ObjOut(PELib& peLib, int pass) const
{
    if (pass == -1)  // as a reference, we have to do a full signature because of overloads
                     // and here we need the fully qualified name
    {
        if (name_.size())
            peLib.Out() << std::endl << "$sb" << peLib.FormatName(Qualifiers::GetObjName(name_, container_));
        else
            peLib.Out() << std::endl << "$sb" << peLib.FormatName(name_);
        if (container_ && typeid(*container_) == typeid(Class) && static_cast<Class*>(container_)->Generic().size())
        {
            Class* cls = static_cast<Class*>(container_);
            peLib.Out() << std::endl << "$gb" << cls->Generic().size();
            cls->GenericParent()->ObjOut(peLib, pass);
            for (auto t : cls->Generic())
            {
                t->ObjOut(peLib, pass);
            }
            peLib.Out() << std::endl << "$ge";
        }
    }
    else
    {
        // as a definition...
        peLib.Out() << std::endl << "$sb" << peLib.FormatName(name_);
        peLib.Out() << external_ << ",";
    }
    peLib.Out() << flags_ << ",";
    peLib.Out() << genericParamCount_ << ",";
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
MethodSignature* MethodSignature::ObjIn(PELib& peLib, Method** found, bool definition)
{
    if (found)
        *found = nullptr;
    std::string name = peLib.UnformatName();
    int external;
    char ch;
    int flags = 0;
    int genericParamCount = 0;
    Class* genericParent;
    std::deque<Type*> generics;
    Type* returnType;
    if (definition)
    {
        external = peLib.ObjInt();
        ch = peLib.ObjChar();
        if (ch != ',')
            peLib.ObjError(oe_syntax);
    }
    else
    {
        if (peLib.ObjBegin() == 'g')
        {
            int n = peLib.ObjInt();
            if (peLib.ObjBegin() != 'c')
                peLib.ObjError(oe_syntax);
            genericParent = Class::ObjIn(peLib, false);
            for (int i = 0; i < n; i++)
                generics.push_back(Type::ObjIn(peLib));
            if (peLib.ObjEnd() != 'g')
                peLib.ObjError(oe_syntax);
        }
        else
        {
            peLib.ObjReset();
        }
    }
    flags = peLib.ObjInt();
    ch = peLib.ObjChar();
    if (ch != ',')
        peLib.ObjError(oe_syntax);
    genericParamCount = peLib.ObjInt();
    ch = peLib.ObjChar();
    if (ch != ',')
        peLib.ObjError(oe_syntax);
    returnType = Type::ObjIn(peLib);
    std::vector<Param*> args, vargs;
    while (peLib.ObjBegin() == 'p')
    {
        Param* p = Param::ObjIn(peLib);
        args.push_back(p);
    }
    if (peLib.ObjBegin(false) == 'v')
    {
        while (peLib.ObjBegin() == 'p')
        {
            Param* p = Param::ObjIn(peLib);
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
    MethodSignature* rv = nullptr;
    std::vector<Type*> targs;
    for (auto p : args)
    {
        targs.push_back(p->GetType());
    }
    Method* pinvoke = peLib.FindPInvoke(name);
    if (pinvoke && !(flags & Managed))
    {
        if (!vargs.size())
        {
            if (found)
                *found = pinvoke;
            rv = pinvoke->Signature();
        }
        else
        {
            MethodSignature* sig = peLib.FindPInvokeWithVarargs(name, vargs);
            if (sig)
            {
                return sig;
            }
            else
            {
                rv = peLib.AllocateMethodSignature(name, MethodSignature::Vararg, nullptr);
                rv->SignatureParent(pinvoke->Signature());  // tie it to the parent pinvoke
                rv->ReturnType(returnType);
                for (auto p : args)
                    rv->AddParam(p);
                for (auto v : vargs)
                    rv->AddVarargParam(v);
                peLib.AddPInvokeWithVarargs(rv);
            }
        }
    }
    else if (!peLib.GetContainer())  // defining a pinvoke
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
            Method* c = static_cast<Method*>(m);
            if (c->Signature()->Name() == name && c->Signature()->Matches(targs))
            {
                if (found)
                    *found = c;
                rv = c->Signature();
            }
        }
        if (!rv)
        {
            rv = peLib.AllocateMethodSignature(name, flags, peLib.GetContainer());
            rv->ReturnType(returnType);
            rv->External(external);
            for (auto p : args)
                rv->AddParam(p);
            for (auto v : vargs)
                rv->AddVarargParam(v);
        }
        else
        {
            if (!external)
            {
                if (rv->External())
                {
                    auto its = args.begin();
                    auto itd = rv->params.begin();
                    while (its != args.end() && itd != rv->params.end())
                    {
                        (*itd)->Name((*its)->Name());
                        ++its;
                        ++itd;
                    }
                }
                rv->External(false);
            }
            if (!rv->ReturnType()->Matches(returnType))
                peLib.ObjError(oe_typemismatch);
        }
        rv->GenericParamCount(genericParamCount);
        if (!external)
            rv->Definition();
    }
    else if (!name.size())
    {
        rv = peLib.AllocateMethodSignature(name, flags, peLib.GetContainer());
        rv->GenericParamCount(genericParamCount);
        rv->ReturnType(returnType);
        for (auto p : args)
            rv->AddParam(p);
        for (auto v : vargs)
            rv->AddVarargParam(v);
    }
    else
    {
        int n = name.find(':');
        if (generics.size() && n != std::string::npos)
        {
            (void)peLib.FindOrCreateGeneric(name.substr(0, n), generics);
        }
        Method* m;
        if (peLib.Find(name, &m, targs, nullptr, generics.size() ? &generics : nullptr) == PELib::s_method)
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
void MethodSignature::ILSignatureDump(PELib& peLib)
{
    returnType_->ILSrcDump(peLib);
    peLib.Out() << " ";
    if (typeid(*container_) == typeid(Class) && static_cast<Class*>(container_)->Generic().size())
    {
        if (container_->Flags().Flags() & Qualifiers::Value)
            peLib.Out() << "valuetype ";
        else
            peLib.Out() << "class ";
        peLib.Out() << Qualifiers::GetName("", container_);
        peLib.Out() << static_cast<Class*>(container_)->AdornGenerics(peLib);
        peLib.Out() << "::'" << name_ << "'(";
    }
    else
    {
        peLib.Out() << Qualifiers::GetName(name_, container_);

    }
    peLib.Out() << "(";
    for (std::list<Param*>::const_iterator it = params.begin(); it != params.end();)
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
bool MethodSignature::PEDump(PELib& peLib, bool asType)
{
    if (container_ && container_->InAssemblyRef())
    {
        if (!peIndexCallSite_)
        {
            container_->PEDump(peLib);
            if (returnType_ && returnType_->GetBasicType() == Type::cls)
            {
                if (returnType_->GetClass()->InAssemblyRef())
                {
                    returnType_->GetClass()->PEDump(peLib);
                }
            }
            for (auto param : params)
            {
                if (param && param->GetType()->GetBasicType() == Type::cls)
                {
                    param->GetType()->GetClass()->PEDump(peLib);
                }
            }
            size_t sz;
            if (generic_.size())
            {                
                genericParent_->PEDump(peLib, false);
                Byte* sig = SignatureGenerator::MethodSpecSig(this, sz);
                size_t methodSignature = peLib.PEOut().HashBlob(sig, sz);
                delete[] sig;
                MethodDefOrRef methodRef(MethodDefOrRef::MemberRef, genericParent_->PEIndexCallSite());
                TableEntryBase* table = new MethodSpecTableEntry(methodRef, methodSignature);
                peIndexCallSite_ = peLib.PEOut().AddTableEntry(table);
            }
            else
            {
                size_t function = peLib.PEOut().HashString(name_);
                Class* cls = nullptr;
                if (container_ && typeid(*container_) == typeid(Class))
                {
                    cls = static_cast<Class*>(container_);
                }
                Byte* sig = SignatureGenerator::MethodRefSig(this, sz);
                size_t methodSignature = peLib.PEOut().HashBlob(sig, sz);
                delete[] sig;
                MemberRefParent memberRef(cls && cls->Generic().size() ? MemberRefParent::TypeSpec : MemberRefParent::TypeRef, container_->PEIndex());
                TableEntryBase* table = new MemberRefTableEntry(memberRef, function, methodSignature);
                peIndexCallSite_ = peLib.PEOut().AddTableEntry(table);
            }
        }
    }
    else if (asType)
    {
        if (!peIndexType_)
        {
            size_t sz;
            Byte* sig = SignatureGenerator::MethodRefSig(this, sz);
            size_t methodSignature = peLib.PEOut().HashBlob(sig, sz);
            delete[] sig;
            TableEntryBase* table = new StandaloneSigTableEntry(methodSignature);
            peIndexType_ = peLib.PEOut().AddTableEntry(table);
        }
    }
    else if ((flags_ & Vararg) && !(flags_ & Managed))
    {
        size_t sz;
        size_t function = peLib.PEOut().HashString(name_);
        size_t parent = methodParent_ ? methodParent_->PEIndex() : 0;
        MemberRefParent memberRef(MemberRefParent::MethodDef, parent);
        Byte* sig = SignatureGenerator::MethodRefSig(this, sz);
        size_t methodSignature = peLib.PEOut().HashBlob(sig, sz);
        delete[] sig;
        TableEntryBase* table = new MemberRefTableEntry(memberRef, function, methodSignature);
        peIndexCallSite_ = peLib.PEOut().AddTableEntry(table);
    }
    else if (!peIndexCallSite_)
    {
        int methodreftype = MemberRefParent::TypeRef;
        size_t sz;
        size_t function = peLib.PEOut().HashString(name_);
        size_t parent;
        if (returnType_ && returnType_->GetBasicType() == Type::cls)
        {
            if (returnType_->GetClass()->InAssemblyRef())
            {
                returnType_->GetClass()->PEDump(peLib);
            }
        }
        if (arrayObject_)
        {
            methodreftype = MemberRefParent::TypeSpec;
            Byte buf[16];
            arrayObject_->Render(peLib, buf);
            parent = arrayObject_->PEIndex();
        }
        else if (container_)
        {
            if (!container_->PEIndex())
                container_->PEDump(peLib);
            parent = container_->PEIndex();
            if (typeid(*container_) == typeid(Class))
            {
                Class* cls = static_cast<Class*>(container_);
                if (cls->Generic().size() && cls->Generic().front()->GetBasicType() != Type::var)
                {
                    methodreftype = MemberRefParent::TypeSpec;
                }
            }
        }
        else
        {
            return false;
        }
        MemberRefParent memberRef(methodreftype, parent);
        Byte* sig = SignatureGenerator::MethodRefSig(this, sz);
        size_t methodSignature = peLib.PEOut().HashBlob(sig, sz);
        delete[] sig;
        TableEntryBase* table = new MemberRefTableEntry(memberRef, function, methodSignature);
        peIndexCallSite_ = peLib.PEOut().AddTableEntry(table);
    }
    return true;
}
void MethodSignature::Load(PELib& lib, AssemblyDef& assembly, PEReader& reader, int start, int end)
{
    const DNLTable& table = reader.Table(tParam);
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
std::string MethodSignature::AdornGenerics(PELib& peLib, bool names) const
{
    std::unique_ptr<std::iostream> hold = std::make_unique<std::stringstream>();
    peLib.Swap(hold);
    if (generic_.size())
    {
        int count = 0;
        peLib.Out() << "<";
        for (auto&& type : generic_)
        {
            if (names && type->GetBasicType() == Type::var)
            {
                peLib.Out() << (char)(type->VarNum() / 26 + 'A');
                peLib.Out() << (char)(type->VarNum() % 26 + 'A');
            }
            else
            {
                Type tp = *type;
                tp.ShowType();
                tp.ILSrcDump(peLib);
            }
            if (count++ != generic_.size() - 1)
                peLib.Out() << ",";
            else
                peLib.Out() << ">";
        }
    }
    peLib.Swap(hold);
    return static_cast<std::stringstream&>(*hold).str();
}
}  // namespace DotNetPELib