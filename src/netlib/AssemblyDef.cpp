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
#include "sha1.h"
#include <climits>
namespace DotNetPELib
{
bool AssemblyDef::ILHeaderDump(PELib& peLib)
{
    peLib.Out() << ".assembly ";
    if (external_)
        peLib.Out() << "extern ";
    peLib.Out() << name_ << "{" << std::endl;
    if (major_ || minor_ || build_ || revision_)
        peLib.Out() << "\t.ver " << major_ << ":" << minor_ << ":" << build_ << ":" << revision_ << std::endl;
    for (int i = 0; i < 8; i++)
    {
        if (publicKeyToken_[i])
        {
            peLib.Out() << "\t.publickeytoken = (";
            for (int i = 0; i < 8; i++)
            {
                peLib.Out() << std::hex << (int)(unsigned char)publicKeyToken_[i] << " ";
            }
            peLib.Out() << ")" << std::endl << std::dec;
            break;
        }
    }
    peLib.Out() << "}" << std::endl;
    return true;
}
bool AssemblyDef::PEHeaderDump(PELib& peLib)
{
    size_t nameIndex = peLib.PEOut().HashString(name_);
    TableEntryBase* table;
    if (external_)
    {
        size_t blobIndex = 0;
        for (int i = 0; i < 8; i++)
        {
            if (publicKeyToken_[i])
            {
                blobIndex = peLib.PEOut().HashBlob(publicKeyToken_, 8);
                break;
            }
        }
        table = new AssemblyRefTableEntry(PA_None, major_, minor_, build_, revision_, nameIndex, blobIndex);
    }
    else
    {
        table = new AssemblyDefTableEntry(PA_None, major_, minor_, build_, revision_, nameIndex);
    }
    peIndex_ = peLib.PEOut().AddTableEntry(table);
    return true;
}
Namespace* AssemblyDef::InsertNameSpaces(PELib& lib, std::map<std::string, Namespace*>& nameSpaces, const std::string& name)
{
    if (nameSpaces.find(name) == nameSpaces.end())
    {
        Namespace* parent = nullptr;
        int n = name.find_last_of(".");
        std::string end = name;
        if (n != std::string::npos)
        {
            parent = InsertNameSpaces(lib, nameSpaces, name.substr(0, n));
            end = name.substr(n + 1);
        }
        Namespace* rv = nullptr;
        if (parent)
        {
            DataContainer* dc = parent->FindContainer(end);
            if (dc && typeid(*dc) == typeid(Namespace))
            {
                rv = parent = static_cast<Namespace*>(dc);
            }
        }
        else
        {
            DataContainer* dc = FindContainer(end);
            if (dc && typeid(*dc) == typeid(Namespace))
            {
                rv = parent = static_cast<Namespace*>(dc);
            }
        }
        if (!rv)
        {
            nameSpaces[name] = lib.AllocateNamespace(end);
            if (parent)
                parent->Add(nameSpaces[name]);
        }
        else
        {
            nameSpaces[name] = rv;
        }
    }
    return nameSpaces[name];
}
Namespace* AssemblyDef::InsertNameSpaces(PELib& lib, Namespace* nameSpace, std::string name)
{
    auto it = namespaceCache.find(name);
    if (it != namespaceCache.end())
    {
        return it->second;
    }
    std::string in = name;
    size_t n = name.find_last_of(".");
    if (n != std::string::npos)
    {
        nameSpace = InsertNameSpaces(lib, nullptr, name.substr(0, n));
        name = name.substr(n + 1);
    }
    Namespace* rv = nullptr;
    if (nameSpace)
    {
        DataContainer* dc = nameSpace->FindContainer(name);
        if (dc && typeid(*dc) == typeid(Namespace))
        {
            rv = nameSpace = static_cast<Namespace*>(dc);
        }
    }
    else
    {
        DataContainer* dc = FindContainer(name);
        if (dc && typeid(*dc) == typeid(Namespace))
        {
            rv = nameSpace = static_cast<Namespace*>(dc);
        }
    }
    if (!rv)
    {
        rv = lib.AllocateNamespace(name);
        if (nameSpace)
        {
            nameSpace->Add(rv);
        }
        else
        {
            Add(rv);
        }
        nameSpace = rv;
        namespaceCache[in] = nameSpace;
    }
    return nameSpace;
}
Class* AssemblyDef::InsertClasses(PELib& lib, Namespace* nameSpace, Class* cls, std::string name)
{
    size_t n = name.find_last_of(".");
    if (n != std::string::npos)
    {
        cls = InsertClasses(lib, nameSpace, nullptr, name.substr(0, n));
        name = name.substr(n + 1);
    }
    Class* rv = nullptr;
    if (cls)
    {
        rv = static_cast<Class*>(cls->FindContainer(name));
    }
    else
    {
        DataContainer* dc = nameSpace->FindContainer(name);
        if (dc && (typeid(*dc) == typeid(Class) || typeid(*dc) == typeid(Enum)))
        {
            rv = static_cast<Class*>(dc);
        }
    }
    if (!rv)
    {
        rv = lib.AllocateClass(name, 0, -1, -1);
        if (cls)
            cls->Add(rv);
        else
            nameSpace->Add(rv);
    }
    return rv;
}
Class* AssemblyDef::LookupClass(PELib& lib, const std::string& nameSpaceName, const std::string& name)
{
    auto in = nameSpaceName + "::" + name;
    auto it = classCache.find(in);
    if (it != classCache.end())
    {
        return it->second;
    }

    Namespace* nameSpace = InsertNameSpaces(lib, nullptr, nameSpaceName);
    auto rv = InsertClasses(lib, nameSpace, nullptr, name);
    classCache[in] = rv;
    return rv;
}
void AssemblyDef::SetPublicKey(PEReader& reader, size_t index)
{
    Byte buf[16384];
    int n = reader.ReadFromBlob(buf, sizeof(buf), index);
    if (n == 8)
    {
        // it is already a token
        memcpy(publicKeyToken_, buf, n);
    }
    else
    {
        // make it a token
        SHA1Context context;
        SHA1Reset(&context);
        SHA1Input(&context, buf, n);
        SHA1Result(&context);
        for (int i = 0; i < 8; i++)
        {
            publicKeyToken_[i] = ((Byte*)context.Message_Digest)[20 - i - 1];
        }
    }
}
void AssemblyDef::ObjOut(PELib& peLib, int pass) const
{
    if (loaded_)
    {
        if (pass == 1)
        {
            peLib.Out() << std::endl << "$abr" << peLib.FormatName(name_);
            peLib.Out() << std::endl << "$ae";
        }
        return;
    }
    else
    {
        peLib.Out() << std::endl << "$abb" << peLib.FormatName(name_) << external_;
        DataContainer::ObjOut(peLib, pass);
        peLib.Out() << std::endl << "$ae";
    }
}
AssemblyDef* AssemblyDef::ObjIn(PELib& peLib, bool definition)
{
    AssemblyDef* a = nullptr;
    char ch;
    ch = peLib.ObjChar();
    if (ch == 'r')
    {
        std::string name = peLib.UnformatName();
        peLib.LoadAssembly(name);
    }
    else if (ch == 'b')
    {
        std::string name = peLib.UnformatName();
        int external = peLib.ObjInt();
        if (!external)
        {
            // dump all locally defined assemblies into the working assembly...
            a = peLib.WorkingAssembly();
        }
        else
        {
            a = peLib.FindAssembly(name);
            if (!a)
            {
                a = peLib.AllocateAssemblyDef(name, true);
            }
        }
        ((DataContainer*)a)->ObjIn(peLib);
    }
    else
    {
        peLib.ObjError(oe_syntax);
    }
    if (peLib.ObjEnd() != 'a')
        peLib.ObjError(oe_syntax);
    return a;
}
void AssemblyDef::Load(PELib& lib, PEReader& reader)
{
    customAttributes_.Load(lib, *this, reader);
    const DNLTable& table = reader.Table(tAssemblyDef);
    if (table.size())
    {
        AssemblyDefTableEntry* entry = static_cast<AssemblyDefTableEntry*>(table[0]);
        major_ = entry->major_;
        minor_ = entry->minor_;
        build_ = entry->build_;
        revision_ = entry->revision_;
        if (entry->publicKeyIndex_.index_)
        {
            SetPublicKey(reader, entry->publicKeyIndex_.index_);
        }
        std::vector<std::string> refClasses;
        refClasses.push_back("");
        std::vector<std::string> typerefClasses;
        const DNLTable& table1 = reader.Table(tTypeRef);
        for (auto tentry : table1)
        {
            Byte buf[256];
            std::string val;
            TypeRefTableEntry* entry = static_cast<TypeRefTableEntry*>(tentry);
            while (entry->resolution_.tag_ == ResolutionScope::TypeRef)
            {
                reader.ReadFromString(buf, 256, entry->resolution_.index_);
                val = val + std::string((char*)buf) + ".";
                TypeRefTableEntry* newEntry = static_cast<TypeRefTableEntry*>(table1[entry->resolution_.index_]);
                if (newEntry == entry)
                    break;
                entry = newEntry;
            }
            if (entry->resolution_.tag_ == ResolutionScope::AssemblyRef)
            {
                reader.ReadFromString(buf, 256, entry->typeNameSpaceIndex_.index_);
                std::string n = (char*)buf;
                if (n.size())
                    val = n + "." + val;
                reader.ReadFromString(buf, 256, entry->typeNameIndex_.index_);
                val += (char*)buf;
                refClasses.push_back(val);
            }
        }
        std::map<std::string, Namespace*> nameSpaces;
        std::vector<Class*> classes;
        classes.push_back(nullptr);
        std::vector<int> fields, methods;
        std::vector<int> properties, semantics;
        fields.push_back(0);
        methods.push_back(0);
        properties.push_back(0);
        const DNLTable& table2 = reader.Table(tTypeDef);
        for (auto tentry : table2)
        {
            Namespace* thisNameSpace = nullptr;
            Byte buf[256];
            TypeDefTableEntry* entry = static_cast<TypeDefTableEntry*>(tentry);
            int visibility = entry->flags_ & TypeDefTableEntry::VisibilityMask;
            if (visibility == TypeDefTableEntry::Public || visibility == TypeDefTableEntry::NestedPublic)
            {
                reader.ReadFromString(buf, 256, entry->typeNameSpaceIndex_.index_);
                std::string val = (char*)buf;
                if (val.size())
                {
                    thisNameSpace = InsertNameSpaces(lib, nameSpaces, val);
                    val += ".";
                }
                reader.ReadFromString(buf, 256, entry->typeNameIndex_.index_);
                val += (char*)buf;
                Class* cls = nullptr;
                if (thisNameSpace)
                {
                    DataContainer* dc = thisNameSpace->FindContainer((char*)buf);
                    if (dc && typeid(*dc) == typeid(Class))
                        cls = static_cast<Class*>(dc);
                }
                int n = entry->extends_.index_;
                if (!cls)
                {
                    int flags = Qualifiers::Public;
                    int peflags = TypeDefTableEntry::Class;
                    DataContainer* parent = Parent();
                    if (entry->flags_ & TypeDefTableEntry::SequentialLayout)
                        flags |= Qualifiers::Sequential;
                    if (entry->flags_ & TypeDefTableEntry::ExplicitLayout)
                        flags |= Qualifiers::Explicit;
                    if (entry->flags_ & TypeDefTableEntry::Sealed)
                        flags |= Qualifiers::Sealed;
                    if ((entry->flags_ & TypeDefTableEntry::StringFormatMask) == TypeDefTableEntry::AnsiClass)
                        flags |= Qualifiers::Ansi;

                    if (name_ == "mscorlib" && entry->extends_.tag_ == TypeDefOrRef::TypeDef && n < classes.size() && classes[n] &&
                        classes[n]->Name() == "Enum")
                    {
                        // Assumes namespace system, which is probably safe since we contexted to
                        // mscorlib.dll
                        cls = lib.AllocateEnum((char*)buf, flags, Field::i32);
                    }
                    else if (entry->extends_.tag_ == TypeDefOrRef::TypeRef && n < refClasses.size() &&
                             refClasses[n] == "System.Enum")
                    {
                        cls = lib.AllocateEnum((char*)buf, flags, Field::i32);
                    }
                    else
                    {
                        cls = lib.AllocateClass((char*)buf, flags, -1, -1);
                    }
                }
                classes.push_back(cls);
                if (thisNameSpace && cls)
                    thisNameSpace->Add(cls);
            }
            else
            {
                classes.push_back(nullptr);
            }
            fields.push_back(entry->fields_.index_);
            methods.push_back(entry->methods_.index_);
        }
        bool done = false;
        while (!done)
        {
            int i = 0;
            done = true;
            for (auto tentry : table2)
            {
                Namespace* thisNameSpace = nullptr;
                Byte buf[256];
                TypeDefTableEntry* entry = static_cast<TypeDefTableEntry*>(tentry);
                int visibility = entry->flags_ & TypeDefTableEntry::VisibilityMask;
                if (visibility == TypeDefTableEntry::Public || visibility == TypeDefTableEntry::NestedPublic)
                {
                    reader.ReadFromString(buf, 256, entry->typeNameSpaceIndex_.index_);
                    std::string val = (char*)buf;
                    if (val.size())
                    {
                        thisNameSpace = InsertNameSpaces(lib, nameSpaces, val);
                        val += ".";
                    }
                    reader.ReadFromString(buf, 256, entry->typeNameIndex_.index_);
                    val += (char*)buf;
                    Class* cls = nullptr;
                    if (!thisNameSpace)
                    {
                        const DNLTable& table3 = reader.Table(tNestedClass);
                        for (auto tentry : table3)
                        {
                            NestedClassTableEntry* entry = static_cast<NestedClassTableEntry*>(tentry);
                            int child = entry->nestedIndex_.index_;
                            int parent = entry->enclosingIndex_.index_;
                            if (child - 1 == i)
                            {
                                if (classes[parent])
                                {
                                    DataContainer* dc = classes[parent]->FindContainer((char*)buf);
                                    if (dc && typeid(*dc) == typeid(Class))
                                    {
                                        cls = static_cast<Class*>(dc);
                                        classes[i] = cls;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
                i++;
            }
        }
        int i = 1;
        for (auto tentry : table2)
        {
            if (classes[i])
            {
                TypeDefTableEntry* entry = static_cast<TypeDefTableEntry*>(tentry);
                if (entry->extends_.index_)
                {
                    switch (entry->extends_.tag_)
                    {
                        case TypeDefOrRef::TypeDef:
                        {
                            classes[i]->Extends(classes[entry->extends_.index_]);
                        }
                        break;
                        case TypeDefOrRef::TypeRef:
                            break;
                    }
                    bool done = false;
                    int index = -1;
                    while (!done)
                    {
                        done = true;
                        switch (entry->extends_.tag_)
                        {
                            case TypeDefOrRef::TypeDef:
                            {
                                if (entry->extends_.index_ == 0)
                                {
                                }
                                else if (entry->extends_.index_ == index)
                                {
                                    // hit bottom
                                }
                                else
                                {
                                    index = entry->extends_.index_;
                                    entry = static_cast<TypeDefTableEntry*>(table2[index]);
                                    if (!classes[index])
                                    {
                                        done = true;
                                    }
                                    else if (classes[index]->Name() == "Object" || classes[index]->Name() == "Enum")
                                    {
                                        done = false;
                                    }
                                    else if (classes[index]->Name() == "ValueType")
                                    {
                                        classes[i]->Flags() |= Qualifiers::Value;
                                        done = false;
                                    }
                                }
                            }
                            break;
                            case TypeDefOrRef::TypeRef:
                                if (entry->extends_.index_ < refClasses.size())
                                {
                                    if (refClasses[entry->extends_.index_] == "System.ValueType")
                                        classes[i]->Flags() |= Qualifiers::Value;
                                    classes[i]->ExtendsName(refClasses[entry->extends_.index_]);
                                }
                                break;
                        }
                    };
                }
            }
            i++;
        }
        const DNLTable& table3 = reader.Table(tNestedClass);
        for (auto tentry : table3)
        {
            NestedClassTableEntry* entry = static_cast<NestedClassTableEntry*>(tentry);
            int child = entry->nestedIndex_.index_;
            int parent = entry->enclosingIndex_.index_;
            //                if (!classes[parent])
            //                    classes[child] = nullptr;
            //                else
            if (classes[parent] && classes[child])
                classes[parent]->Add(classes[child]);
        }
        const DNLTable& table4 = reader.Table(tClassLayout);
        for (auto tentry : table4)
        {
            ClassLayoutTableEntry* entry = static_cast<ClassLayoutTableEntry*>(tentry);
            int parent = entry->parent_.index_;
            if (classes[parent])
            {
                classes[parent]->pack(entry->pack_);
                classes[parent]->size(entry->size_);
            }
        }
        while (properties.size() < classes.size())
            properties.push_back(0);
        const DNLTable& table5 = reader.Table(tPropertyMap);
        i = 0;
        for (auto tentry : table5)
        {
            PropertyMapTableEntry* entry5 = static_cast<PropertyMapTableEntry*>(tentry);
            properties[entry5->parent_.index_] = entry5->propertyList_.index_;
        }
        int n = INT_MAX;
        for (i = properties.size() - 1; i > 0; i--)
            if (!properties[i])
                properties[i] = n;
            else
                n = properties[i];

        for (int i = 0; i < classes.size(); i++)
            semantics.push_back(0);
        const DNLTable& table6 = reader.Table(tMethodSemantics);
        int j = 0;
        for (int i = 1; i < properties.size(); i++)
        {
            if (properties[i - 1] != properties[i])
            {
                bool done = false;
                while (j < table6.size())
                {
                    MethodSemanticsTableEntry* entry6 = static_cast<MethodSemanticsTableEntry*>(table6[j]);
                    if (entry6->association_.index_ == properties[i])
                        break;
                    j++;
                }
                semantics[i] = j;
                /*
                while (j < table6.size())
                {
                    MethodSemanticsTableEntry *entry6 = static_cast<MethodSemanticsTableEntry *>(table6[j]);
                    if (entry6->association_.index_ != properties[i])
                        break;
                    j++;
                }
                semantics[i + 1] = j;
                */
            }
        }
        //        semantics[classes.size() - 1] = properties.size();
        // load the namespaces.
        // Note: classes fields and functions not in a namespace will NOT
        // be imported, as per the C# rules.
        for (auto a : nameSpaces)
        {
            if (!a.second->Parent())
                Add(a.second);
        }
        // load fields
        i = 0;
        int top = classes.size() - 1;
        for (auto centry : classes)
        {
            if (i && i < top && centry)
                centry->Load(lib, *this, reader, i, fields[i], fields[i + 1], methods[i], methods[i + 1], semantics[i],
                             semantics[i + 1]);
            i++;
        }
        if (classes.size())
        {
            int i = classes.size() - 1;
            if (classes[i])
                classes[i]->Load(lib, *this, reader, i, fields[i], INT_MAX, methods[i], INT_MAX, semantics[i], INT_MAX);
        }
    }
}
}  // namespace DotNetPELib