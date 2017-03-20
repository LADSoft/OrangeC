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
#include "SHA1.h"
#include <climits>
namespace DotNetPELib
{
    bool AssemblyDef::ILHeaderDump(PELib &peLib)
    {
        peLib.Out() << ".assembly ";
        if (external_)
            peLib.Out() << "extern ";
        peLib.Out() << name_ << "{" << std::endl;
        if (major_ || minor_ || build_ || revision_ )
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
    bool AssemblyDef::PEHeaderDump(PELib &peLib)
    {
        size_t nameIndex = peLib.PEOut().HashString(name_);
        TableEntryBase *table;
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
            table = new AssemblyDefTableEntry(PA_None, 0, 0, 0, 0, nameIndex);
        }
        peIndex_ = peLib.PEOut().AddTableEntry(table);
        return true;
    }
    Namespace *AssemblyDef::InsertNameSpaces(PELib &lib, std::map<std::string, Namespace *> &nameSpaces, std::string name)
    {
        if (nameSpaces.find(name) == nameSpaces.end())
        {
            Namespace *parent = nullptr;
            int n = name.find_last_of(".");
            std::string end = name;
            if (n != std::string::npos)
            {
                parent = InsertNameSpaces(lib, nameSpaces, name.substr(0, n));
                end = name.substr(n+1);
            }
            Namespace *rv = nullptr;
            if (parent)
            {
                DataContainer *dc = parent->FindContainer(end);
                if (dc && typeid(*dc) == typeid(Namespace))
                {
                    rv = parent = static_cast<Namespace *>(dc);
                }
            }
            else
            {
                DataContainer *dc = FindContainer(end);
                if (dc && typeid(*dc) == typeid(Namespace))
                {
                    rv = parent = static_cast<Namespace *>(dc);
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
    Namespace *AssemblyDef::InsertNameSpaces(PELib &lib, Namespace *nameSpace, std::string name)
    {
        std::string in = name;
        size_t n = name.find_last_of(".");
        if (n != std::string::npos)
        {
            nameSpace = InsertNameSpaces(lib, nullptr, name.substr(0, n));
            name = name.substr(n+1); 
        }
        Namespace *rv = nullptr;
        if (nameSpace)
        {
            DataContainer *dc = nameSpace->FindContainer(name);
            if (dc && typeid(*dc) == typeid(Namespace))
            {
                rv = nameSpace = static_cast<Namespace *>(dc);
            }
        }
        else 
        {
            DataContainer *dc = FindContainer(name);
            if (dc && typeid(*dc) == typeid(Namespace))
            {
                rv = nameSpace = static_cast<Namespace *>(dc);
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
        }
        return nameSpace;
    }
    Class *AssemblyDef::InsertClasses(PELib &lib, Namespace *nameSpace, Class *cls, std::string name)
    {
        size_t n = name.find_last_of(".");
        if (n != std::string::npos)
        {
            cls = InsertClasses(lib, nameSpace, nullptr, name.substr(0, n));
            name = name.substr(n+1); 
        }
        Class *rv = nullptr;
        if (cls)
        {
            rv = static_cast<Class *>(cls->FindContainer(name));
        }
        else
        {
            DataContainer *dc = nameSpace->FindContainer(name);
            if (dc && (typeid(*dc) == typeid(Class) || typeid(*dc) == typeid(Enum)))
            {
                rv = static_cast<Class *>(dc);
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
    Class *AssemblyDef::LookupClass(PELib &lib, std::string nameSpaceName, std::string name)
    {
        Namespace *nameSpace = InsertNameSpaces(lib, nullptr, nameSpaceName);
        return InsertClasses(lib, nameSpace, nullptr, name);
    }
    void AssemblyDef::SetPublicKey(PEReader &reader, size_t index)
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
            for (int i=0; i < 8; i++)
            {
                publicKeyToken_[i] = ((Byte *)context.Message_Digest)[20 -i-1];
            }
        }
    }
    void AssemblyDef::Load(PELib &lib, PEReader &reader)
    {
        customAttributes_.Load(lib, *this, reader);
        const DNLTable &table = reader.Table(tAssemblyDef);
        if (table.size())
        {
            AssemblyDefTableEntry *entry = static_cast<AssemblyDefTableEntry *>(table[0]);
            major_ = entry->major_;
            minor_ = entry->minor_;
            build_ = entry->build_;
            revision_ = entry->revision_;
            if (entry->publicKeyIndex_.index_)
            {
                SetPublicKey(reader, entry->publicKeyIndex_.index_);
            }
            std::vector<std::string>refClasses;
            const DNLTable &table1 = reader.Table(tTypeDef);
            for (auto tentry : table1)
            {
                Byte buf[256];
                TypeRefTableEntry *entry = static_cast<TypeRefTableEntry *>(tentry);
                reader.ReadFromString(buf, 256, entry->typeNameSpaceIndex_.index_);

                std::string val = (char *)buf;
                if (val.size())
                    val += ".";
                reader.ReadFromString(buf, 256, entry->typeNameIndex_.index_);
                val += (char *)buf;
                refClasses.push_back(val);

            }
            std::map<std::string, Namespace *> nameSpaces;
            std::vector<Class *> classes;
            classes.push_back(nullptr);
            std::vector<int> fields, methods;
            std::vector<int> properties, semantics;
            fields.push_back(0);
            methods.push_back(0);
            properties.push_back(0);
            semantics.push_back(0);
            const DNLTable &table2 = reader.Table(tTypeDef);
            int i = 0;
            for (auto tentry : table2)
            {
                Namespace *thisNameSpace = nullptr;
                Byte buf[256];
                TypeDefTableEntry *entry = static_cast<TypeDefTableEntry *>(tentry);
                int visibility = entry->flags_ & TypeDefTableEntry::VisibilityMask;
                if (visibility == TypeDefTableEntry::Public || visibility == TypeDefTableEntry::NestedPublic)
                {
                    reader.ReadFromString(buf, 256, entry->typeNameSpaceIndex_.index_);
                    std::string val = (char *)buf;
                    if (val.size())
                    {
                        thisNameSpace = InsertNameSpaces(lib, nameSpaces, val);
                        val += ".";
                    }
                    reader.ReadFromString(buf, 256, entry->typeNameIndex_.index_);
                    val += (char *)buf;
                    Class *cls = nullptr;
                    if (thisNameSpace)
                    {
                        DataContainer *dc = thisNameSpace->FindContainer((char *)buf);
                        if (dc && typeid(*dc) == typeid(Class))
                            cls = static_cast<Class *>(dc);
                    }
                    else
                    {
                        const DNLTable &table3 = reader.Table(tNestedClass);
                        for (auto tentry : table3)
                        {
                            NestedClassTableEntry *entry = static_cast<NestedClassTableEntry *>(tentry);
                            int child = entry->nestedIndex_.index_;
                            int parent = entry->enclosingIndex_.index_;
                            if (child - 1 == i)
                            {
                                if (classes[parent])
                                {
                                    DataContainer *dc = classes[parent]->FindContainer((char *)buf);
                                    if (dc && typeid(*dc) == typeid(Class))
                                        cls = static_cast<Class *>(dc);
                                }
                                break;
                            }
                        }
                    }
                    int n = entry->extends_.index_;
                    if (!cls)
                    {
                        if (n < refClasses.size() && refClasses[n] == "System.Enum")
                        {
                            cls = lib.AllocateEnum((char *)buf, entry->flags_, Field::i32);
                        }
                        else
                        {
                            cls = lib.AllocateClass((char *)buf, entry->flags_, -1, -1);
                        }
                    }
                    classes.push_back(cls);
                    if (thisNameSpace)
                        thisNameSpace->Add(cls);
                }
                else
                {
                    classes.push_back(nullptr);
                }
                fields.push_back(entry->fields_.index_);
                methods.push_back(entry->methods_.index_);
                i++;
            }
            const DNLTable &table3 = reader.Table(tNestedClass);
            for (auto tentry : table3)
            {
                NestedClassTableEntry *entry = static_cast<NestedClassTableEntry *>(tentry);
                int child = entry->nestedIndex_.index_;
                int parent = entry->enclosingIndex_.index_;
                if (!classes[parent])
                    classes[child] = nullptr;
                else if (classes[child])
                    classes[parent]->Add(classes[child]);
            }
            const DNLTable &table4 = reader.Table(tClassLayout);
            for (auto tentry : table4)
            {
                ClassLayoutTableEntry *entry = static_cast<ClassLayoutTableEntry *>(tentry);
                int parent = entry->parent_.index_;
                if (classes[parent])
                {
                    classes[parent]->pack(entry->pack_);                    
                    classes[parent]->size(entry->size_);                    
                }
            }
            while (properties.size() < classes.size())
                properties.push_back(0);
            const DNLTable &table5 = reader.Table(tPropertyMap);
            i = 0;
            for (auto tentry : table5)
            {
                PropertyMapTableEntry *entry5 = static_cast<PropertyMapTableEntry *>(tentry);
                properties[entry5->parent_.index_] = entry5->propertyList_.index_;
            }
            int n = INT_MAX;
            for (i = properties.size() - 1; i > 0; i--)
                if (!properties[i])
                    properties[i] = n;
                else
                    n = properties[i];

            for (int i = 1; i < classes.size(); i++)
                semantics.push_back(0);
            const DNLTable &table6 = reader.Table(tMethodSemantics);
            int j = 0;
            for (int i=1;  i < properties.size()-1; i++)
            {
                if (properties[i] != properties[i + 1])
                {
                    bool done = false;
                    while (j < table6.size())
                    {
                        MethodSemanticsTableEntry *entry6 = static_cast<MethodSemanticsTableEntry *>(table6[j]);
                        if (entry6->association_.index_ == properties[i])
                            break;
                        j++;
                    }
                    semantics[i] = j;
                    while (j < table6.size())
                    {
                        MethodSemanticsTableEntry *entry6 = static_cast<MethodSemanticsTableEntry *>(table6[j]);
                        if (entry6->association_.index_ != properties[i])
                            break;
                        j++;
                    }
                    semantics[i + 1] = j;
                }
            }
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
                    centry->Load(lib, *this, reader, i, fields[i], fields[i+1], methods[i], methods[i+1], semantics[i], semantics[i+1]);
                i++;
            }
            if (classes.size())
            {
                int i = classes.size()-1;
                if (classes[i])
                    classes[i]->Load(lib, *this, reader, i, fields[i], INT_MAX, methods[i], INT_MAX, semantics[i], INT_MAX);
            }
        }
    }
}