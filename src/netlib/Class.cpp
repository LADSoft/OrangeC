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
#include <typeinfo>
#include <climits>
#include <sstream>

namespace DotNetPELib
{

bool Class::ILSrcDump(PELib& peLib) const
{
    ILSrcDumpClassHeader(peLib);
    if (extendsFrom_)
    {
        peLib.Out() << " extends " << Qualifiers::GetName("", extendsFrom_);
        peLib.Out() << extendsFrom_->AdornGenerics(peLib);
    }
    peLib.Out() << " {";
    if (pack_ > 0 || size_ > 0)
    {
        peLib.Out() << std::endl;
        if (pack_ > 0)
            peLib.Out() << " .pack " << pack_;
        if (size_ >= 0)
            peLib.Out() << " .size " << size_;
    }
    peLib.Out() << std::endl;
    bool rv = DataContainer::ILSrcDump(peLib);
    peLib.Out() << std::endl;
    for (auto p : properties_)
        rv &= p->ILSrcDump(peLib);
    peLib.Out() << "}" << std::endl;
    return rv;
}
int Class::TransferFlags() const
{
    int peflags = TypeDefTableEntry::Class;
    DataContainer* parent = Parent();
    if (parent && typeid(*parent) == typeid(Class))
    {
        if (flags_.Flags() & Qualifiers::Public)
        {
            peflags |= TypeDefTableEntry::NestedPublic;
        }
        else
        {
            peflags |= TypeDefTableEntry::NestedPrivate;
        }
    }
    else
    {
        if (flags_.Flags() & Qualifiers::Public)
        {
            peflags |= TypeDefTableEntry::Public;
        }
    }
    if (flags_.Flags() & Qualifiers::Sequential)
    {
        peflags |= TypeDefTableEntry::SequentialLayout;
    }
    else if (flags_.Flags() & Qualifiers::Explicit)
    {
        peflags |= TypeDefTableEntry::ExplicitLayout;
    }
    if (flags_.Flags() & Qualifiers::Sealed)
    {
        peflags |= TypeDefTableEntry::Sealed;
    }
    if (flags_.Flags() & Qualifiers::Ansi)
    {
        peflags |= TypeDefTableEntry::AnsiClass;
    }
    return peflags;
}
bool Class::PEDump(PELib& peLib)
{
    if (generic_.size() && generic_.front()->GetBasicType() != Type::var)
    {
        if (!peIndex_)
        {
            if (!genericParent_->PEIndex())
                genericParent_->PEDump(peLib);
            if (generic_.size())
            {
                for (auto g : generic_)
                {
                    if (g->GetBasicType() == Type::cls && g->PEIndex() == 0)
                    {
                        size_t val;
                        g->Render(peLib, (Byte*)&val);
                        g->PEIndex(val);
                    }
                }
            }
            size_t sz;
            Type type(this);
            Byte* sig = SignatureGenerator::TypeSig(&type, sz);
            size_t signature = peLib.PEOut().HashBlob(sig, sz);
            delete[] sig;
            TypeSpecTableEntry* table = new TypeSpecTableEntry(signature);
            peIndex_ = peLib.PEOut().AddTableEntry(table);
        }
    }
    else if (InAssemblyRef())
    {
        if (!peIndex_)
        {
            if (typeid(*parent_) == typeid(Class))
            {
                parent_->PEDump(peLib);
                ResolutionScope resolution(ResolutionScope::TypeRef, parent_->PEIndex());
                size_t typenameIndex = peLib.PEOut().HashString(Name());
                TableEntryBase* table = new TypeRefTableEntry(resolution, typenameIndex, 0);
                peIndex_ = peLib.PEOut().AddTableEntry(table);
            }
            else
            {
                ResolutionScope resolution(ResolutionScope::AssemblyRef, ParentAssembly(peLib));
                size_t typenameIndex = peLib.PEOut().HashString(Name());
                size_t namespaceIndex = ParentNamespace(peLib);
                TableEntryBase* table = new TypeRefTableEntry(resolution, typenameIndex, namespaceIndex);
                peIndex_ = peLib.PEOut().AddTableEntry(table);
            }
        }
    }
    else
    {
        int peflags = TransferFlags();
        size_t typenameIndex = peLib.PEOut().HashString(Name());
        size_t namespaceIndex = ParentNamespace(peLib);
        size_t extends = (flags_.Flags() & Qualifiers::Value) ? peLib.PEOut().ValueBaseClass() : peLib.PEOut().ObjectBaseClass();
        size_t fieldIndex = peLib.PEOut().NextTableIndex(tField);
        size_t methodIndex = peLib.PEOut().NextTableIndex(tMethodDef);
        DataContainer* parent = Parent();
        if (extendsFrom_)
        {
            if (!extendsFrom_->PEIndex())
                extendsFrom_->PEDump(peLib);
            extends = extendsFrom_->PEIndex();
        }
        if (parent && typeid(*parent) == typeid(Class))
            namespaceIndex = 0;
        int typeType = TypeDefOrRef::TypeRef;
        if (extendsFrom_ && !extendsFrom_->InAssemblyRef())
            typeType = TypeDefOrRef::TypeDef;
        TypeDefOrRef extendsClass(typeType, extends);
        TableEntryBase* table =
            new TypeDefTableEntry(peflags, typenameIndex, namespaceIndex, extendsClass, fieldIndex, methodIndex);
        peIndex_ = peLib.PEOut().AddTableEntry(table);

        if (pack_ > 0 || size_ > 0)
        {
            int mypack_ = pack_;
            int mysize_ = size_;
            if (mypack_ <= 0)
                mypack_ = 1;
            if (mysize_ <= 0)
                mysize_ = 1;
            table = new ClassLayoutTableEntry(mypack_, mysize_, peIndex_);
            peLib.PEOut().AddTableEntry(table);
        }
        if (parent && typeid(*parent) == typeid(Class))
        {
            size_t enclosing = ParentClass(peLib);
            table = new NestedClassTableEntry(peIndex_, enclosing);
            peLib.PEOut().AddTableEntry(table);
        }
        DataContainer::PEDump(peLib);
        if (properties_.size())
        {
            size_t propertyIndex = peLib.PEOut().NextTableIndex(tProperty);
            table = new PropertyMapTableEntry(peIndex_, propertyIndex);
            peLib.PEOut().AddTableEntry(table);
            for (auto p : properties_)
                p->PEDump(peLib);
        }
        if (generic_.size())
        {
            TypeOrMethodDef owner(TypeOrMethodDef::TypeDef, peIndex_);
            for (int i = 0; i < generic_.size(); i++)
            {
                std::string name;
                name += (char)(i / 26 + 'A');
                name += (char)(i % 26 + 'A');
                size_t namestr = peLib.PEOut().HashString(name);
                table = new GenericParamTableEntry(i, 0, owner, namestr);
                peLib.PEOut().AddTableEntry(table);
            }
        }
    }
    return true;
}
void Class::ILSrcDumpClassHeader(PELib& peLib) const
{
    peLib.Out() << ".class";
    DataContainer* parent = Parent();
    if (parent && typeid(*parent) == typeid(Class))
        peLib.Out() << " nested";
    flags_.ILSrcDumpBeforeFlags(peLib);
    flags_.ILSrcDumpAfterFlags(peLib);
    peLib.Out() << " '" << name_ << "'";
    peLib.Out() << AdornGenerics(peLib, true);
}
void Class::ObjOut(PELib& peLib, int pass) const
{
    if (pass == -1)
    {
        peLib.Out() << std::endl << "$cb" << peLib.FormatName(Qualifiers::GetObjName("", this));
        if (generic_.size())
        {
            peLib.Out() << std::endl << "$gb" << generic_.size();
            genericParent_->ObjOut(peLib, pass);
            for (auto t : generic_)
            {
                t->ObjOut(peLib, pass);
            }
            peLib.Out() << std::endl << "$ge";
        }
    }
    else
    {
        peLib.Out() << std::endl << "$cb" << peLib.FormatName(name_) << external_ << ",";
        peLib.Out() << pack_ << "," << size_ << ",";
        flags_.ObjOut(peLib, pass);
        DataContainer::ObjOut(peLib, pass);
        if (pass == 3)
        {
            for (auto p : properties_)
                p->ObjOut(peLib, pass);
        }
    }
    peLib.Out() << std::endl << "$ce";
}
Class* Class::ObjIn(PELib& peLib, bool definition)
{
    Class* rv = nullptr;
    std::string name = peLib.UnformatName();
    if (definition)
    {
        // here we are doing a definition
        int external = peLib.ObjInt();
        char ch;
        ch = peLib.ObjChar();
        if (ch != ',')
            peLib.ObjError(oe_syntax);
        int pack = peLib.ObjInt();
        ch = peLib.ObjChar();
        if (ch != ',')
            peLib.ObjError(oe_syntax);
        int size = peLib.ObjInt();
        ch = peLib.ObjChar();
        if (ch != ',')
            peLib.ObjError(oe_syntax);
        Qualifiers flags;
        flags.ObjIn(peLib);
        DataContainer* temp;
        Class* c;
        temp = peLib.GetContainer()->FindContainer(name);
        if (temp && typeid(*temp) != typeid(Class))
            peLib.ObjError(oe_noclass);
        if (!temp)
        {
            rv = c = peLib.AllocateClass(name, flags, pack, size);
        }
        else
        {
            c = static_cast<Class*>(temp);
        }
        if (rv)
            rv->External(external);
        ((DataContainer*)c)->ObjIn(peLib);
        peLib.PushContainer(c);
        while (peLib.ObjBegin() == 'P')
            c->Add(Property::ObjIn(peLib), false);
        peLib.PopContainer();
        if (peLib.ObjEnd(false) != 'c')
            peLib.ObjError(oe_syntax);
    }
    else
    {
        // if we get here it is as an operand
        std::deque<Type*> generics;
        Class *genericParent;
        if (peLib.ObjBegin()== 'g')
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
        if (generics.size())
        {
            rv = peLib.FindOrCreateGeneric(name, generics);
            if (!rv)
            {
                peLib.ObjError(oe_noclass);
            }
            else
            {
                rv->genericParent_ = genericParent;
            }
        }
        else
        {
            void* result = nullptr;
            if (peLib.Find(name, &result) == PELib::s_class)
            {
                rv = static_cast<Class*>(result);
            }
            else
            {
                peLib.ObjError(oe_noclass);
            }
        }
        if (peLib.ObjEnd() != 'c')
            peLib.ObjError(oe_syntax);
    }
    return rv;
}
void Class::Load(PELib& lib, AssemblyDef& assembly, PEReader& reader, size_t clsIndex, int startField, int endField,
                 int startMethod, int endMethod, int startSemantics, int endSemantics)
{
    const DNLTable& table = reader.Table(tField);
    int top = table.size();
    for (int i = startField; i < endField && i <= top; i++)
    {
        Byte buf[256];
        FieldTableEntry* entry = static_cast<FieldTableEntry*>(table[i - 1]);
        if ((entry->flags_ & FieldTableEntry::FieldAccessMask) == FieldTableEntry::Public)
        {
            reader.ReadFromString(buf, sizeof(buf), entry->nameIndex_.index_);
            Field* field =
                lib.AllocateField((char*)buf, nullptr, (entry->flags_ & FieldTableEntry::Static) ? Qualifiers::Static : 0);
            Add(field);
            SignatureGenerator::TypeFromFieldRef(lib, assembly, reader, field, entry->signatureIndex_.index_);
            if (entry->flags_ & FieldTableEntry::HasDefault)
            {
                const DNLTable& table = reader.Table(tConstant);
                for (auto tentry : table)
                {
                    ConstantTableEntry* tentry2 = static_cast<ConstantTableEntry*>(tentry);
                    if (tentry2->parentIndex_.tag_ == Constant::FieldDef && tentry2->parentIndex_.index_ == i)
                    {
                        int value = 0;
                        reader.ReadFromBlob((Byte*)&value, sizeof(value), tentry2->valueIndex_.index_);
                        field->AddEnumValue(value, (Field::ValueSize)tentry2->type_);
                    }
                }
            }
        }
    }
    std::vector<int> paramIndexes;
    std::vector<MethodSignature*> sigs;
    std::vector<Method*> methods;
    const DNLTable& table1 = reader.Table(tMethodDef);
    int i;
    top = table1.size();
    for (i = startMethod; i < endMethod && i <= top; i++)
    {
        Byte buf[256];
        MethodDefTableEntry* entry = static_cast<MethodDefTableEntry*>(table1[i - 1]);
        if ((entry->flags_ & MethodDefTableEntry::MemberAccessMask) == MethodDefTableEntry::Public)
        {
            reader.ReadFromString(buf, sizeof(buf), entry->nameIndex_.index_);
            MethodSignature* sig = lib.AllocateMethodSignature((char*)buf, MethodSignature::Managed, this);
            SignatureGenerator::TypeFromMethodRef(lib, assembly, reader, sig, entry->signatureIndex_.index_);
            if (entry->flags_ & MethodDefTableEntry::Virtual)
               entry->flags_ |= Qualifiers::Virtual;       
            Method* method = lib.AllocateMethod(sig, entry->flags_, false);
            Add(method);
            sigs.push_back(sig);
            methods.push_back(method);
        }
        else
        {
            sigs.push_back(nullptr);
            methods.push_back(nullptr);
        }
        paramIndexes.push_back(entry->paramIndex_.index_);
    }
    if (i < table1.size())
    {
        MethodDefTableEntry* entry = static_cast<MethodDefTableEntry*>(table1[i - 1]);
        paramIndexes.push_back(entry->paramIndex_.index_);
    }
    else
    {
        paramIndexes.push_back(INT_MAX);
    }
    top = paramIndexes.size() - 1;
    i = 0;
    for (auto pentry : sigs)
    {
        if (i < top && pentry)
            pentry->Load(lib, assembly, reader, paramIndexes[i], paramIndexes[i + 1]);
        i++;
    }
    const DNLTable& table2 = reader.Table(tPropertyMap);
    PropertyMapTableEntry* entry2;
    int end = table2.size();
    for (int i = 0; i < end; i++)
    {
        entry2 = static_cast<PropertyMapTableEntry*>(table2[i]);
        size_t propIndex = entry2->propertyList_.index_;
        if (entry2->parent_.index_ == clsIndex)
        {
            if (i < end - 1)
            {
                entry2 = static_cast<PropertyMapTableEntry*>(table2[i + 1]);
                end = entry2->propertyList_.index_;
            }
            else
            {
                end = reader.Table(tProperty).size() + 1;
            }
            for (int j = propIndex; j < end; j++)
            {
                Property* prop = lib.AllocateProperty();
                prop->SetContainer(this, false);
                properties_.push_back(prop);
                prop->Load(lib, assembly, reader, j, startMethod, startSemantics, endSemantics, methods);
            }
            break;
        }
    }
}
bool Class::Traverse(Callback& callback) const
{
    if (!DataContainer::Traverse(callback))
        return true;
    for (auto property : properties_)
        if (!callback.EnterProperty(property))
            return false;
    return true;
}
std::string Class::AdornGenerics(PELib& peLib, bool names) const
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
            if (count++ != generic_.size()-1)
                peLib.Out() << ",";
            else
                peLib.Out() << ">";
        }
    }
    peLib.Swap(hold);
    return static_cast<std::stringstream&>(*hold).str();
}
bool Class::MatchesGeneric(std::deque<Type*>* generics) const
{
    if (generics)
    {
        if (Generic().size() == generics->size())
        {
            auto itg1 = Generic().begin();
            auto itg2 = generics->begin();
            while (itg2 != generics->end())
            {
                if (!(*itg1)->Matches(*itg2))
                    break;
                ++itg1;
                ++itg2;
            }
            if (itg2 == generics->end())
            {
                return true;
            }
        }
    }
    return false;
}
}  // namespace DotNetPELib