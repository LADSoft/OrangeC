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

namespace DotNetPELib
{
    CustomAttributeContainer::~CustomAttributeContainer()
    {
        for (auto a : attributes)
        {
            delete a.first;
        }
        for (auto s : descriptors)
        {
            delete s;
        }
    }
    bool CustomAttributeContainer::lt::operator()(const CustomAttribute *left, const CustomAttribute *right) const {
        if (left->tag_ < right->tag_)
            return true;
        else if (left->tag_ == right->tag_ && left->index_ < right->index_)
            return true;
        return false;
    }
    bool CustomAttributeContainer::CustomAttributeDescriptor::operator() (const CustomAttributeDescriptor *left, const CustomAttributeDescriptor *right) const {
        if (left->name < right->name)
            return true;
        if (left->name == right->name)
            if (!left->data && right->data)
                return true;
            else if (left->data && right->data)
                if (left->sz < right->sz)
                    return true;
                else if (left->sz == right->sz)
                    return memcmp(left->data, right->data, right->sz) < 0;
        return false;
    }
    const std::vector<CustomAttributeContainer::CustomAttributeDescriptor *>& CustomAttributeContainer::Lookup(CustomAttribute *attribute) const
    {
        static std::vector<CustomAttributeDescriptor *> empty;

        auto it = attributes.find(attribute);
        if (it != attributes.end())
            return it->second;
        else return empty;
    }
    bool CustomAttributeContainer::Has(CustomAttribute &attribute, std::string name, Byte *data, size_t sz) const
    {
        auto it = attributes.find(&attribute);
        if (it != attributes.end())
        {
            for (auto a : it->second)
            {
                if (a->name == name)
                    if (!data || sz == a->sz && !memcmp(data, a->data, sz))
                        return true;
            }
        }
        return false;
    }
    void CustomAttributeContainer::Load(PELib &peLib, AssemblyDef &assembly, PEReader &reader)
    {
        const DNLTable &table2 = reader.Table(tTypeDef);
        TableEntryBase *last = nullptr;
        std::vector<TypeDefTableEntry *> indices;
        int end = reader.Table(tMethodDef).size();
        if (end != 0)
        {
            for (int i = 0; i < end; i++)
                indices.push_back(static_cast<TypeDefTableEntry *>(table2[table2.size() - 1]));
            TypeDefTableEntry *entry = static_cast<TypeDefTableEntry *>(table2[0]);
            for (int i = 0; i < entry->methods_.index_; i++)
                indices[i] = 0;
            for (int i = 0; i < table2.size() - 1; i++)
            {
                TypeDefTableEntry *entry2 = static_cast<TypeDefTableEntry *>(table2[i]);
                TypeDefTableEntry *entry2a = static_cast<TypeDefTableEntry *>(table2[i + 1]);
                for (int i = entry2->methods_.index_; i < entry2a->methods_.index_; i++)
                    indices[i - 1] = entry2;
            }
            const DNLTable &table = reader.Table(tCustomAttribute);
            for (auto tentry : table)
            {
                CustomAttributeTableEntry *entry = static_cast<CustomAttributeTableEntry *>(tentry);
                CustomAttribute attribute = entry->parentIndex_;
                int n = entry->typeIndex_.index_, s, as;
                int nname, nnamespace;
                bool def = false;
                Byte name[256], nameSpace[256], assemblyName[256];
                // this won't work too well if the property is in a nested class..
                if (entry->typeIndex_.tag_ == CustomAttributeType::MethodDef)
                {
                    TypeDefTableEntry *entry3 = static_cast<TypeDefTableEntry *>(indices[entry->typeIndex_.index_ - 1]);
                    if (!entry3)
                        continue;
                    n = entry3->typeNameIndex_.index_;
                    s = entry3->typeNameSpaceIndex_.index_;
                    as = 0;
                }
                else // methodref
                {
                    MemberRefTableEntry *entry2 = static_cast<MemberRefTableEntry *>(reader.Table(tMemberRef)[n - 1]);
                    if (entry2->parentIndex_.tag_ == MemberRefParent::TypeRef)
                    {
                        TypeRefTableEntry *entry3 = static_cast<TypeRefTableEntry *>(reader.Table(tTypeRef)[entry2->parentIndex_.index_ - 1]);
                        n = entry3->typeNameIndex_.index_;
                        s = entry3->typeNameSpaceIndex_.index_;
                        if (entry3->resolution_.tag_ == ResolutionScope::AssemblyRef)
                            as = entry3->resolution_.index_;
                        else
                            as = 0;
                    }
                    else
                    {
                        continue;
                    }
                }
                reader.ReadFromString(name, sizeof(name), n);
                reader.ReadFromString(nameSpace, sizeof(nameSpace), s);
                CustomAttributeDescriptor a;
                if (as)
                {
                    reader.ReadFromString(assemblyName, sizeof(assemblyName), as);
                }
                else
                {
                    strncpy((char *)assemblyName, assembly.Name().c_str(), sizeof(assemblyName));
                    assemblyName[sizeof(assemblyName) - 1] = 0;
                }
                a.name = std::string((char *)"[") + (char *)assemblyName + "]" + (char *)nameSpace + "." + (char *)name;

                if (entry->valueIndex_.index_)
                {
                    Byte buf[256];
                    int n = reader.ReadFromBlob(buf, sizeof(buf), entry->valueIndex_.index_);
                    if (n)
                    {
                        a.data = buf;
                        a.sz = n;
                    }
                    auto it = descriptors.find(&a);
                    if (it == descriptors.end())
                    {
                        CustomAttributeDescriptor *cad = new CustomAttributeDescriptor;
                        cad->name = a.name;
                        if (a.sz)
                        {
                            cad->data = new Byte[a.sz];
                            memcpy(cad->data, a.data, a.sz);
                            cad->sz = a.sz;
                        }
                        descriptors.insert(cad);
                    }
                    // should succeed now
                    it = descriptors.find(&a);
                    auto ita = attributes.find(&attribute);
                    if (ita == attributes.end())
                    {
                        CustomAttribute *newAttribute = new CustomAttribute(attribute.tag_, attribute.index_);
                        std::vector< CustomAttributeDescriptor *>desc = { *it };
                        attributes[newAttribute] = desc;
                    }
                    else
                    {
                        ita->second.push_back(*it);
                    }
                    // for the destructor at the end of the block...
                    a.data = nullptr;
                    a.sz = 0;
                }
            }
        }
    }
}