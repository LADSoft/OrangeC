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
#include "PEHeader.h"
#include <time.h>
#include <stdio.h>

namespace DotNetPELib
{

    // this is a CUSTOM version string for microsoft.
    // standard CIL differs
#define RTV_STRING "v4.0.30319"

    Byte PEWriter::MZHeader_[] = {
        0x4d, 0x5a, 0x90, 0x00, 0x03, 0x00, 0x00, 0x00,
        0x04, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
        0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
        0x0e, 0x1f, 0xba, 0x0e, 0x00, 0xb4, 0x09, 0xcd,
        0x21, 0xb8, 0x01, 0x4c, 0xcd, 0x21, 0x54, 0x68,
        0x69, 0x73, 0x20, 0x70, 0x72, 0x6f, 0x67, 0x72,
        0x61, 0x6d, 0x20, 0x63, 0x61, 0x6e, 0x6e, 0x6f,
        0x74, 0x20, 0x62, 0x65, 0x20, 0x72, 0x75, 0x6e,
        0x20, 0x69, 0x6e, 0x20, 0x44, 0x4f, 0x53, 0x20,
        0x6d, 0x6f, 0x64, 0x65, 0x2e, 0x0d, 0x0d, 0x0a,
        0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    char *PEWriter::streamNames_[5] = { "#~", "#Strings", "#US", "#GUID", "#Blob" };

    static DotNetMetaHeader metaHeader1 =
    {
        META_SIG, 1, 1, 0
    };
    DotNetMetaHeader *PEWriter::metaHeader_ = &metaHeader1;

    DWord PEWriter::sdata_rva_;
    Byte PEWriter::defaultUS_[8] = { 0, 3, 0x20, 0, 0 };

    void PEWriter::pool::Ensure(size_t newSize) {
        if (size + newSize > maxSize)
        {
            if (size + newSize < maxSize * 2)
                maxSize = maxSize * 2;
            else
                maxSize = (maxSize + newSize) * 2;
            base = (Byte *)realloc(base, maxSize);
        }
    }
    PEWriter::~PEWriter()
    {
        delete peHeader_;
        delete peObjects_;
        delete cor20Header_;
        delete tablesHeader_;
        for (int i = 0; i < MaxTables; i++)
        {
            for (auto it = tables_[i].begin(); it != tables_[i].end(); it++)
            {
                TableEntryBase *x = *it;
                delete x;
            }
        }
        for (auto it = methods_.begin(); it != methods_.end(); ++it)
        {
            PEMethod *method = *it;
            delete method;
        }
    }
    size_t PEWriter::AddTableEntry(TableEntryBase *entry)
    {
        int n = entry->TableIndex();
        tables_[n].push_back(entry);
        return tables_[n].size();
    }
    void PEWriter::AddMethod(PEMethod *method)
    {
        if (method->flags_ & PEMethod::EntryPoint)
        {
            if (entryPoint_)
                throw new PELibError(PELibError::MultipleEntryPoints);
            entryPoint_ = method->methodDef_ | (tMethodDef << 24);
        }
        methods_.push_back(method);
    }
    size_t PEWriter::HashString(std::string utf8)
    {

        auto it = stringMap_.find(utf8);
        if (it != stringMap_.end())
            return it->second;
        if (strings_.size == 0)
            strings_.size++;
        strings_.Ensure(utf8.size() + 1);
        size_t rv = strings_.size;
        memcpy(strings_.base + strings_.size, utf8.c_str(), utf8.size() + 1);
        strings_.size += utf8.size() + 1;
        stringMap_[utf8] = rv;
        return rv;
    }
    size_t PEWriter::HashUS(std::wstring str)
    {
        if (us_.size == 0)
            us_.size++;
        int flag = 0;
        us_.Ensure(str.size() * 2 + 5);
        size_t rv = us_.size;
        int blobLen = str.size() * 2 + 1;
        if (blobLen < 0x80)
        {
            us_.base[us_.size++] = blobLen;
        }
        else if (blobLen <= 0x3fff)
        {
            us_.base[us_.size++] = (blobLen >> 8) | 0x80;
            us_.base[us_.size++] = blobLen & 0x7f;
        }
        else
        {
            blobLen &= 0x1fffffff;
            us_.base[us_.size++] = (blobLen >> 24) | 0xc0;
            us_.base[us_.size++] = (blobLen >> 16);
            us_.base[us_.size++] = (blobLen >> 8);
            us_.base[us_.size++] = (blobLen >> 0);
        }
        for (int i = 0; i < str.size(); i++)
        {
            int n = str[i];
            if (n & 0xff00)
                flag = 1;
            else if (n <= 8 || n >= 0x0e && n < 0x20 || n == 0x27 || n == 0x2d || n == 0x7f)
                flag = 1;
            us_.base[us_.size++] = n & 0xff;
            us_.base[us_.size++] = n >> 8;
        }
        us_.base[us_.size++] = flag;
        return rv;
    }
    size_t PEWriter::HashGUID(Byte *Guid)
    {
        guid_.Ensure(128 / 8);
        size_t rv = guid_.size;
        memcpy(guid_.base + rv, Guid, 128 / 8);
        guid_.size += 128 / 8;
        return (rv / (128 / 8) + 1);

    }
    size_t PEWriter::HashBlob(Byte *blobData, size_t blobLen)
    {
        if (blob_.size == 0)
            blob_.size++;
        int xcount = 0;
        blob_.Ensure(blobLen + 4);
        size_t rv = blob_.size;
        if (blobLen < 0x80)
        {
            blob_.base[blob_.size++] = blobLen;
        }
        else if (blobLen <= 0x3fff)
        {
            blob_.base[blob_.size++] = (blobLen >> 8) | 0x80;
            blob_.base[blob_.size++] = blobLen & 0x7f;
        }
        else
        {
            blobLen &= 0x1fffffff;
            blob_.base[blob_.size++] = (blobLen >> 24) | 0xc0;
            blob_.base[blob_.size++] = (blobLen >> 16);
            blob_.base[blob_.size++] = (blobLen >> 8);
            blob_.base[blob_.size++] = (blobLen >> 0);
        }
        memcpy(blob_.base + blob_.size, blobData, blobLen);
        blob_.size += blobLen;
        return rv;
    }
    size_t PEWriter::RVABytes(Byte *Bytes, size_t dataLen)
    {
        // align
        int align = dataLen;
        if (align > 8)
            align = 8;
        int pos = rva_.size;
        if (pos % align)
            pos += (align - pos % align);
        rva_.Ensure(dataLen + pos - rva_.size);
        size_t rv = rva_.size;
        rva_.size += pos - rva_.size + dataLen;
        memcpy(rva_.base + rv, Bytes, dataLen);
        return rv;
    }

    size_t IndexBase::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        size_t rv;
        DWord val = (index_ << GetIndexShift()) + tag_;
        if (HasIndexOverflow(sizes))
        {
            *(DWord *)dest = val;
            rv = 4;
        }
        else
        {
            *(Word *)dest = val;
            rv = 2;
        }
        return rv;
    }
    bool ResolutionScope::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
    {
        return Large(sizes[tModule]) ||
            Large(sizes[tModuleRef]) ||
            Large(sizes[tAssemblyRef]) ||
            Large(sizes[tTypeRef]);
    }
    bool TypeDefOrRef::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
    {
        return Large(sizes[tTypeDef]) ||
            Large(sizes[tTypeRef]) ||
            Large(sizes[tTypeSpec]);
    }
    bool MemberRefParent::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
    {
        return Large(sizes[tTypeDef]) ||
            Large(sizes[tTypeRef]) ||
            Large(sizes[tModule]) ||
            Large(sizes[tModuleRef]) ||
            Large(sizes[tTypeSpec]);
    }
    bool Constant::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
    {
        return Large(sizes[tField]) ||
            Large(sizes[tParam]);
    }
    bool CustomAttribute::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
    {
        return Large(sizes[tMethodDef]) ||
            Large(sizes[tField]) ||
            Large(sizes[tTypeRef]) ||
            Large(sizes[tTypeDef]) ||
            Large(sizes[tParam]) ||
            Large(sizes[tImplMap]) ||
            Large(sizes[tMemberRef]) ||
            Large(sizes[tModule]) ||
            Large(sizes[tStandaloneSig]) ||
            Large(sizes[tModuleRef]) ||
            Large(sizes[tTypeSpec]) ||
            Large(sizes[tAssemblyDef]) ||
            Large(sizes[tAssemblyRef]);
    }
    bool CustomAttributeType::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
    {
        return Large(sizes[tMethodDef]) ||
            Large(sizes[tMemberRef]);
    }
    bool MemberForwarded::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
    {
        return Large(sizes[tField]) ||
            Large(sizes[tMethodDef]);
    }
    bool FieldList::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
    {
        return Large(sizes[tField]);
    }
    bool MethodList::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
    {
        return Large(sizes[tMethodDef]);
    }
    bool ParamList::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
    {
        return Large(sizes[tParam]);
    }
    bool TypeDef::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
    {
        return Large(sizes[tTypeDef]);
    }
    bool ModuleRef::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
    {
        return Large(sizes[tModuleRef]);
    }
    bool String::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
    {
        return Large(sizes[tString]);
    }
    bool US::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
    {
        return Large(sizes[tUS]);
    }
    bool GUID::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
    {
        return Large(sizes[tGUID]);
    }
    bool Blob::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
    {
        return Large(sizes[tBlob]);
    }
    size_t ModuleTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        *(Word *)dest = 0;
        size_t n = 2;
        n += nameIndex_.Render(sizes, dest + n);
        n += guidIndex_.Render(sizes, dest + n);
        if (sizes[tGUID] > 65535)
        {
            *(DWord *)(dest + n) = 0;
            n += 4;
            *(DWord *)(dest + n) = 0;
            n += 4;
        }
        else
        {
            *(Word *)(dest + n) = 0;
            n += 2;
            *(Word *)(dest + n) = 0;
            n += 2;
        }
        return n;
    }
    size_t TypeRefTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        int n = resolution.Render(sizes, dest);
        n += typeNameIndex_.Render(sizes, dest + n);
        n += typeNameSpaceIndex_.Render(sizes, dest + n);
        return n;
    }
    size_t TypeDefTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        *(DWord *)dest = flags_;
        int n = 4;
        n += typeNameIndex_.Render(sizes, dest + n);
        n += typeNameSpaceIndex_.Render(sizes, dest + n);
        n += extends_.Render(sizes, dest + n);
        n += fields_.Render(sizes, dest + n);
        n += methods_.Render(sizes, dest + n);
        return n;
    }
    size_t FieldTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        *(Word *)dest = flags_;
        int n = 2;
        n += nameIndex_.Render(sizes, dest + n);
        n += signatureIndex_.Render(sizes, dest + n);
        return n;
    }
    size_t MethodDefTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        *(DWord *)dest = method_->rva_;
        int n = 4;
        *(Word *)(dest + n) = implFlags_;
        n += 2;
        *(Word *)(dest + n) = flags_;
        n += 2;
        n += nameIndex_.Render(sizes, dest + n);
        n += signatureIndex_.Render(sizes, dest + n);
        n += paramIndex_.Render(sizes, dest + n);
        return n;
    }
    size_t ParamTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        *(Word *)dest = flags_;
        int n = 2;
        *(Word *)(dest + n) = sequenceIndex_;
        n += 2;
        n += nameIndex_.Render(sizes, dest + n);

        return n;
    }
    size_t MemberRefTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        int n = parentIndex_.Render(sizes, dest);
        n += nameIndex_.Render(sizes, dest + n);
        n += signatureIndex_.Render(sizes, dest + n);
        return n;
    }
    size_t ConstantTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        *(Byte *)dest = type_;
        *(Byte *)(dest + 1) = 0;
        int n = 2;
        n += parentIndex_.Render(sizes, dest + n);
        n += valueIndex_.Render(sizes, dest + n);
        return n;

    }
    size_t CustomAttributeTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        int n = parentIndex_.Render(sizes, dest);
        n += typeIndex_.Render(sizes, dest + n);
        n += valueIndex_.Render(sizes, dest + n);
        return n;
    }
    size_t ClassLayoutTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        *(Word *)dest = pack_;
        int n = 2;
        *(DWord *)(dest + n) = size_;
        n += 4;
        n += parent_.Render(sizes, dest + n);
        return n;

    }
    size_t FieldLayoutTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        *(DWord *)dest = offset_;
        int n = 4;
        n += parent_.Render(sizes, dest + n);
        return n;
    }
    size_t StandaloneSigTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        return signatureIndex_.Render(sizes, dest);
    }
    size_t ModuleRefTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        return nameIndex_.Render(sizes, dest);
    }
    size_t TypeSpecTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        return signatureIndex_.Render(sizes, dest);
    }
    size_t ImplMapTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        *(Word *)dest = flags_;
        int n = 2;
        n += methodIndex_.Render(sizes, dest + n);
        n += importNameIndex_.Render(sizes, dest + n);
        n += moduleIndex_.Render(sizes, dest + n);
        return n;
    }
    size_t FieldRVATableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        *(DWord *)dest = rva_ + PEWriter::sdata_rva_;
        int n = 4;
        n += fieldIndex_.Render(sizes, dest + n);
        return n;
    }
    size_t AssemblyDefTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        *(DWord *)dest = DefaultHashAlgId;
        int n = 4;
        // assembly version
        *(Word *)(dest + n) = 0;
        n += 2;
        *(Word *)(dest + n) = 0;
        n += 2;
        *(Word *)(dest + n) = 0;
        n += 2;
        *(Word *)(dest + n) = 0;
        n += 2;
        *(DWord *)(dest + n) = flags_;
        n += 4;
        n += publicKeyIndex_.Render(sizes, dest + n);
        n += nameIndex_.Render(sizes, dest + n);
        n += cultureIndex_.Render(sizes, dest + n);
        return n;

    }
    size_t AssemblyRefTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        int n = 0;
        // assembly version
        *(Word *)(dest + n) = 0;
        n += 2;
        *(Word *)(dest + n) = 0;
        n += 2;
        *(Word *)(dest + n) = 0;
        n += 2;
        *(Word *)(dest + n) = 0;
        n += 2;
        *(DWord *)(dest + n) = flags_;
        n += 4;
        n += publicKeyIndex_.Render(sizes, dest + n);
        n += nameIndex_.Render(sizes, dest + n);
        n += cultureIndex_.Render(sizes, dest + n);
        n += hashIndex_.Render(sizes, dest + n);
        return n;
    }
    size_t NestedClassTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte *dest) const
    {
        int n = nestedIndex_.Render(sizes, dest);
        n += enclosingIndex_.Render(sizes, dest + n);
        return n;
    }
    size_t PEMethod::Write(size_t sizes[MaxTables + ExtraIndexes], std::fstream &out) const
    {
        Byte dest[512];
        int n;
        if ((flags_ & 3) == TinyFormat)
        {
            n = 1;
            *(Byte *)dest = (flags_ & 3) + (codeSize_ << 2);
        }
        else
        {
            n = 12;
            *(Word *)dest = 0x3000 + (flags_ & 0xfff);
            *(Word *)(dest + 2) = maxStack_;
            *(DWord *)(dest + 4) = codeSize_;
            *(DWord *)(dest + 8) = signatureToken_;
        }
        out.write((char *)dest, n);
        out.write((char *)code_, codeSize_);
        n += codeSize_;
        return n;
    }
    void PEWriter::CalculateObjects(PELib &peLib)
    {
        peHeader_ = new PEHeader;
        memset(peHeader_, 0, sizeof(PEHeader));
        peHeader_->signature = PESIG;
        peHeader_->cpu_type = PE_INTEL386;
        peHeader_->magic = PE_MAGICNUM;
        peHeader_->nt_hdr_size = 0xe0; // optional header sie
        peHeader_->flags = PE_FILE_32BIT + PE_FILE_EXECUTABLE + (DLL_ ? PE_FILE_LIBRARY : 0);
        peHeader_->linker_major_version = 6;
        peHeader_->object_align = objectAlign_;
        peHeader_->file_align = fileAlign_;
        peHeader_->image_base = imageBase_;
        peHeader_->os_major_version = 4;
        peHeader_->subsys_major_version = 4;
        peHeader_->subsystem = GUI_ ? PE_SUBSYS_WINDOWS : PE_SUBSYS_CONSOLE;
        peHeader_->dll_flags = 0x8540; // magic!
        peHeader_->stack_size = 0x100000;
        peHeader_->stack_commit = 0x1000;
        peHeader_->heap_size = 0x100000;
        peHeader_->heap_commit = 0x1000;
        peHeader_->num_rvas = 16;

        peHeader_->num_objects = 2;
        if (rva_.size)
            peHeader_->num_objects++;
        peHeader_->header_size = sizeof(MZHeader_) + sizeof(PEHeader) + peHeader_->num_objects * sizeof(PEObject);
        if (peHeader_->header_size % fileAlign_)
        {
            peHeader_->header_size += (fileAlign_ - peHeader_->header_size % fileAlign_);
        }
        peHeader_->time = time(0);

        peObjects_ = new PEObject[MAX_PE_OBJECTS];
        memset(peObjects_, 0, sizeof(PEObject) * MAX_PE_OBJECTS);

        int n = 0;
        strncpy(peObjects_[n].name, ".text", 8);
        peObjects_[n++].flags = WINF_EXECUTE | WINF_CODE | WINF_READABLE;
        if (rva_.size)
        {
            strncpy(peObjects_[n].name, ".sdata", 8);
            peObjects_[n++].flags = WINF_INITDATA | WINF_READABLE | WINF_WRITEABLE;
        }
        /*
        strncpy(peObjects_[n].name, ".rsrc", 8);
        peObjects_[n++].flags = WINF_INITDATA | WINF_READABLE;
    */
        strncpy(peObjects_[n].name, ".reloc", 8);
        peObjects_[n++].flags = WINF_INITDATA | WINF_READABLE | WINF_DISCARDABLE;
        size_t currentRVA = sizeof(MZHeader_) + sizeof(PEHeader) + peHeader_->num_objects * sizeof(PEObject);
        if (currentRVA % objectAlign_)
        {
            currentRVA += objectAlign_ - (currentRVA % objectAlign_);
        }
        peObjects_[0].virtual_addr = currentRVA;
        peObjects_[0].raw_ptr = peHeader_->header_size;
        peHeader_->code_base = currentRVA;
        peHeader_->iat_rva = currentRVA;
        peHeader_->iat_size = 8;
        currentRVA += peHeader_->iat_size;
        peHeader_->com_rva = currentRVA;
        peHeader_->com_size = sizeof(DotNetCOR20Header);
        currentRVA += peHeader_->com_size;

        cor20Header_ = new DotNetCOR20Header;
        memset(cor20Header_, 0, sizeof(DotNetCOR20Header));
        cor20Header_->cb = sizeof(DotNetCOR20Header);
        cor20Header_->MajorRuntimeVersion = 2;
        cor20Header_->MinorRuntimeVersion = 5;
        // standard CIL expects ONLY bit 0, we are using bit 1 as well
        // for interoperability with the microsoft runtimes 
        cor20Header_->Flags = peLib.GetCorFlags();
        cor20Header_->EntryPointToken = entryPoint_;

        size_t lastRVA = currentRVA;
        for (auto method : methods_)
        {
            if (method->flags_ & PEMethod::CIL)
            {
                if ((method->flags_ & 3) == PEMethod::TinyFormat)
                {
                    method->rva_ = currentRVA;
                    lastRVA = currentRVA;
                    currentRVA += 1;
                }
                else
                {
                    if (currentRVA % 4)
                        currentRVA += 4 - currentRVA % 4;
                    method->rva_ = currentRVA;
                    lastRVA = currentRVA;
                    currentRVA += 12;
                }
                currentRVA += method->codeSize_;
            }
            else
            {
                method->rva_ = lastRVA;
            }
        }
        if (currentRVA % 4)
        {
            currentRVA += 4 - currentRVA % 4;
        }
        cor20Header_->MetaData[0] = currentRVA; // metadata root

        currentRVA += 12; // metadataHeader;
        currentRVA += 4; // version size;
        currentRVA += sizeof(RTV_STRING);
        if (currentRVA % 4)
            currentRVA += 4 - currentRVA % 4;
        currentRVA += 2; // flags
        currentRVA += 2; // # streams, will be 5 in our implementation

        for (int i = 0; i < 5; i++)
        {
            currentRVA += 8 + strlen(streamNames_[i]) + 1;
            if (currentRVA % 4)
                currentRVA += 4 - currentRVA % 4;
        }

        streamHeaders_[0][0] = currentRVA - cor20Header_->MetaData[0];
        tablesHeader_ = new DotNetMetaTablesHeader;
        memset(tablesHeader_, 0, sizeof(DotNetMetaTablesHeader));
        tablesHeader_->MajorVersion = 2;
        tablesHeader_->Reserved2 = 1;
        tablesHeader_->MaskSorted = (((longlong)0x1600) << 32) + (0x3325FA00);
        if (strings_.size >= 0x65536)
            tablesHeader_->HeapOffsetSizes |= 1;
        if (guid_.size >= 0x65536)
            tablesHeader_->HeapOffsetSizes |= 2;
        if (blob_.size >= 0x65536)
            tablesHeader_->HeapOffsetSizes |= 4;
        n = 0;
        size_t counts[MaxTables + ExtraIndexes];
        memset(counts, 0, sizeof(counts));
        counts[tString] = strings_.size;
        counts[tUS] = us_.size;
        counts[tGUID] = guid_.size;
        counts[tBlob] = blob_.size;

        for (int i = 0; i < MaxTables; i++)
            if (tables_[i].size())
            {
                counts[i] = tables_[i].size();
                tablesHeader_->MaskValid |= ((longlong)1) << i;
                n++;
            }
        currentRVA += sizeof(DotNetMetaTablesHeader); // tables header
        currentRVA += n * sizeof(DWord); // table counts;

        for (int i = 0; i < MaxTables; i++)
            if (counts[i])
            {
                Byte buffer[512];
                n = tables_[i][0]->Render(counts, buffer);
                n *= counts[i];
                currentRVA += n;
            }
        if (currentRVA % 4)
            currentRVA += 4 - currentRVA % 4;
        //    currentRVA += 4;
        streamHeaders_[0][1] = currentRVA - streamHeaders_[0][0] - cor20Header_->MetaData[0];
        streamHeaders_[1][0] = currentRVA - cor20Header_->MetaData[0];
        currentRVA += strings_.size;
        if (currentRVA % 4)
            currentRVA += 4 - currentRVA % 4;
        streamHeaders_[1][1] = currentRVA - streamHeaders_[1][0] - cor20Header_->MetaData[0];
        streamHeaders_[2][0] = currentRVA - cor20Header_->MetaData[0];
        if (us_.size == 0)
            currentRVA += sizeof(defaultUS_); // US May be empty in our implementation we put an empty string there
        else
            currentRVA += us_.size;
        if (currentRVA % 4)
            currentRVA += 4 - currentRVA % 4;
        streamHeaders_[2][1] = currentRVA - streamHeaders_[2][0] - cor20Header_->MetaData[0];
        streamHeaders_[3][0] = currentRVA - cor20Header_->MetaData[0];
        currentRVA += guid_.size;
        if (currentRVA % 4)
            currentRVA += 4 - currentRVA % 4;
        streamHeaders_[3][1] = currentRVA - streamHeaders_[3][0] - cor20Header_->MetaData[0];
        streamHeaders_[4][0] = currentRVA - cor20Header_->MetaData[0];
        currentRVA += blob_.size;
        if (currentRVA % 4)
            currentRVA += 4 - currentRVA % 4;
        streamHeaders_[4][1] = currentRVA - streamHeaders_[4][0] - cor20Header_->MetaData[0];

        cor20Header_->MetaData[1] = currentRVA - cor20Header_->MetaData[0];

        peHeader_->import_rva = currentRVA;
        currentRVA += sizeof(PEImportDir) * 2 + 8;
        if (currentRVA % 16)
            currentRVA += 16 - currentRVA % 16;
        currentRVA += 2 + sizeof("_CorXXXMain") + sizeof("mscoree.dll") + 1;
        peHeader_->import_size = currentRVA - peHeader_->import_rva;
        if (currentRVA % 4)
            currentRVA += 4 - currentRVA % 4;

        currentRVA += 2;
        peHeader_->entry_point = currentRVA;
        currentRVA += 6;


        int sect = 0;
        peObjects_[sect].virtual_size = currentRVA - peObjects_[sect].virtual_addr;
        n = peObjects_[sect].virtual_size;
        if (n % fileAlign_);
        n += fileAlign_ - n % fileAlign_;
        peObjects_[sect].raw_size = n;
        peHeader_->code_size = n;

        if (currentRVA %objectAlign_)
            currentRVA += objectAlign_ - currentRVA % objectAlign_;
        peObjects_[sect + 1].raw_ptr = peObjects_[sect].raw_ptr + peObjects_[sect].raw_size;
        peObjects_[sect + 1].virtual_addr = currentRVA;
        peHeader_->data_base = currentRVA;
        sect++;
        if (rva_.size)
        {
            sdata_rva_ = currentRVA;
            currentRVA += rva_.size; // sizeof sdata
            peObjects_[sect].virtual_size = currentRVA - peObjects_[sect].virtual_addr;
            n = peObjects_[sect].virtual_size;
            if (n % fileAlign_);
            n += fileAlign_ - n % fileAlign_;
            peObjects_[sect].raw_size = n;
            peHeader_->data_size = n;

            if (currentRVA %objectAlign_)
                currentRVA += objectAlign_ - currentRVA % objectAlign_;
            peObjects_[sect + 1].raw_ptr = peObjects_[sect].raw_ptr + peObjects_[sect].raw_size;
            peObjects_[sect + 1].virtual_addr = currentRVA;
            sect++;
        }

#if 0
        peHeader_->resource_rva = currentRVA;
        currentRVA += (sizeof(PEResourceDirTable) + sizeof(PEResourceDirEntry)) * 3; /* resource dirs */
        currentRVA += sizeof(PEResourceDataEntry);

        currentRVA += 2; /* size of version info */
        currentRVA += 48; /* fixed info */
        currentRVA += 68; /* VarFileInfo */
        currentRVA += 368 - 72; /* String file info base */
        std::string nn = peLib.FileName();
        n = nn.find_last_of("\\");
        if (n != std::string::npos)
            n = nn.size() - n;
        else
            n = nn.size();
        if (n % 4)
            n += 4 - (n % 4);
        currentRVA += n * 2;

        char temp[256];
        sprintf(temp, "%d.%d.%d.%d", fileVersion[0], fileVersion[1], fileVersion[2], fileVersion[3]);
        n = strlen(temp);
        if (n % 4)
            n += 4 - (n % 4);
        n *= 2;
        currentRVA += n;
        sprintf(temp, "%d.%d.%d.%d", productVersion[0], productVersion[1], productVersion[2], productVersion[3]);
        n = strlen(temp);
        if (n % 4)
            n += 4 - (n % 4);
        n *= 2;
        currentRVA += n;
        sprintf(temp, "%d.%d.%d.%d", assemblyVersion[0], assemblyVersion[1], assemblyVersion[2], assemblyVersion[3]);
        n = strlen(temp);
        if (n % 4)
            n += 4 - (n % 4);
        n *= 2;
        currentRVA += n;

        peObjects_[sect].virtual_size = currentRVA - peObjects_[sect].virtual_addr;
        peHeader_->resource_size = peObjects_[sect].virtual_size;
        n = peObjects_[sect].virtual_size;
        if (n % fileAlign);
        n += fileAlign - n % fileAlign;
        peObjects_[sect].raw_size = n;
        peHeader_->data_size += n;

        if (currentRVA %objectAlign)
            currentRVA += objectAlign - currentRVA % objectAlign;
        peObjects_[sect + 1].raw_ptr = peObjects_[sect].raw_ptr + peObjects_[sect].raw_size;
        peObjects_[sect + 1].virtual_addr = currentRVA;
        sect++;
#endif  

        peHeader_->fixup_rva = currentRVA;
        currentRVA += 12; // sizeof relocs


        peObjects_[sect].virtual_size = currentRVA - peObjects_[sect].virtual_addr;
        peHeader_->fixup_size = peObjects_[sect].virtual_size;
        n = peObjects_[sect].virtual_size;
        if (n % fileAlign_);
        n += fileAlign_ - n % fileAlign_;
        peObjects_[sect].raw_size = n;
        peHeader_->data_size += n;

        if (currentRVA %objectAlign_)
            currentRVA += objectAlign_ - currentRVA % objectAlign_;
        peObjects_[sect + 1].raw_ptr = peObjects_[sect].raw_ptr + peObjects_[sect].raw_size;
        peObjects_[sect + 1].virtual_addr = currentRVA;

        peHeader_->image_size = currentRVA;
    }
    bool PEWriter::WriteFile(PELib &peLib, std::fstream &out)
    {
        outputFile_ = &out;
        if (!entryPoint_ && !DLL_)
            throw new PELibError(PELibError::MissingEntryPoint);
        CalculateObjects(peLib);
        return
            WriteMZData(peLib) &&
            WritePEHeader(peLib) &&
            WritePEObjects(peLib) &&
            WriteIAT(peLib) &&
            WriteCoreHeader(peLib) &&
            WriteMethods(peLib) &&
            WriteMetadataHeaders(peLib) &&
            WriteTables(peLib) &&
            WriteStrings(peLib) &&
            WriteUS(peLib) &&
            WriteGUID(peLib) &&
            WriteBlob(peLib) &&
            WriteImports(peLib) &&
            WriteEntryPoint(peLib) &&
            WriteStaticData(peLib) &&
            //        WriteVersionInfo(peLib) &&
            WriteRelocs(peLib);
    }
    void PEWriter::align(size_t algn) const
    {
        size_t n = offset();
        if (n % algn)
        {
            // assumes the alignments are 65536 or less
            Byte buf[65536];
            n = algn - n % algn;
            memset(buf, 0, n);
            put(buf, n);
        }
    }
    bool PEWriter::WriteMZData(PELib &peLib) const
    {
        put(MZHeader_, sizeof(MZHeader_));
        return true;
    }
    bool PEWriter::WritePEHeader(PELib &peLib) const
    {
        put(peHeader_, sizeof(PEHeader));
        return true;
    }
    bool PEWriter::WritePEObjects(PELib &peLib) const
    {
        put(peObjects_, sizeof(PEObject) * peHeader_->num_objects);
        return true;
    }
    bool PEWriter::WriteIAT(PELib &peLib) const
    {
        align(fileAlign_);
        DWord n = peHeader_->import_rva;
        n += sizeof(PEImportDir) * 2 + 4;
        if (n % 16)
            n += 16 - n % 16;
        put(&n, sizeof(n));
        n = 0;
        put(&n, sizeof(n));
        return true;
    }
    bool PEWriter::WriteCoreHeader(PELib &peLib) const
    {
        put(cor20Header_, sizeof(DotNetCOR20Header));
        return true;
    }
    bool PEWriter::WriteMethods(PELib &peLib) const
    {
        size_t counts[MaxTables + ExtraIndexes];
        memset(counts, 0, sizeof(counts));
        counts[tString] = strings_.size;
        counts[tUS] = us_.size;
        counts[tGUID] = guid_.size;
        counts[tBlob] = blob_.size;

        for (int i = 0; i < MaxTables; i++)
        {
            counts[i] = tables_[i].size();
        }
        for (std::list<PEMethod *>::const_iterator it = methods_.begin(); it != methods_.end(); ++it)
        {
            if ((*it)->flags_ & PEMethod::CIL)
            {
                if (((*it)->flags_ & 3) == PEMethod::FatFormat)
                {
                    align(4);
                }
                (*it)->Write(counts, *outputFile_);
            }
        }
        return true;
    }
    bool PEWriter::WriteMetadataHeaders(PELib &peLib) const
    {
        align(4);
        put(metaHeader_, sizeof(DotNetMetaHeader));
        int n = sizeof(RTV_STRING);
        if (n % 4)
            n += 4 - n % 4;
        put(&n, sizeof(n));
        put(RTV_STRING, sizeof(RTV_STRING));
        align(4);
        Word flags = 0;
        put(&flags, sizeof(flags));
        Word streams = 5;
        put(&streams, sizeof(streams));
        for (int i = 0; i < 5; i++)
        {
            put(&streamHeaders_[i][0], 4);
            put(&streamHeaders_[i][1], 4);
            put(streamNames_[i], strlen(streamNames_[i]) + 1);
            align(4);
        }
        return true;

    }
    bool PEWriter::WriteTables(PELib &peLib) const
    {
        size_t counts[MaxTables + ExtraIndexes];
        memset(counts, 0, sizeof(counts));
        counts[tString] = strings_.size;
        counts[tUS] = us_.size;
        counts[tGUID] = guid_.size;
        counts[tBlob] = blob_.size;

        put(tablesHeader_, sizeof(DotNetMetaTablesHeader));
        for (int i = 0; i < MaxTables; i++)
        {
            DWord n = counts[i] = tables_[i].size();
            if (n)
                put(&n, sizeof(n));
        }

        for (int i = 0; i < MaxTables; i++)
        {
            DWord n = tables_[i].size();
            for (int j = 0; j < n; j++)
            {
                Byte buffer[512];
                DWord sz = tables_[i][j]->Render(counts, buffer);
                put(buffer, sz);
            }
        }
        align(4);
        //    DWord n = 0;
        //    put(&n, sizeof(n));
        return true;
    }
    bool PEWriter::WriteStrings(PELib &peLib) const
    {
        put(strings_.base, strings_.size);
        align(4);
        return true;
    }
    bool PEWriter::WriteUS(PELib &peLib) const
    {
        if (us_.size == 0)
        {
            put(defaultUS_, sizeof(defaultUS_));
        }
        else
        {
            put(us_.base, us_.size);
        }
        align(4);
        return true;
    }
    bool PEWriter::WriteGUID(PELib &peLib) const
    {
        put(guid_.base, guid_.size);
        align(4);
        return true;
    }
    bool PEWriter::WriteBlob(PELib &peLib) const
    {
        put(blob_.base, blob_.size);
        align(4);
        return true;
    }
    bool PEWriter::WriteImports(PELib &peLib) const
    {
        PEImportDir dir[2];
        memset(&dir, 0, sizeof(dir));
        dir[0].thunkPos2 = peHeader_->import_rva + 2 * sizeof(PEImportDir);
        DWord n = dir[0].thunkPos2 + 8;
        if (n % 16)
            n += 16 - n % 16;
        DWord mainName = n;
        n += 2;
        n += sizeof("_CorXXXMain");
        dir[0].dllName = n;
        dir[0].thunkPos = peHeader_->iat_rva;
        put(&dir, sizeof(dir));
        put(&mainName, sizeof(mainName));
        n = 0;
        put(&n, sizeof(n)); // thunk
        align(16);
        put(&n, 2);// ord
        if (DLL_)
            put("_CorDllMain", sizeof("_CorDllMain"));
        else
            put("_CorExeMain", sizeof("_CorExeMain"));
        put("mscoree.dll", sizeof("mscoree.dll"));
        align(4);
        return true;

    }
    bool PEWriter::WriteEntryPoint(PELib &peLib) const
    {
        DWord n = 0;
        put(&n, 2);
        n = 0x25ff; // JMP[xxx];
        put(&n, 2);
        n = peObjects_[0].virtual_addr + peHeader_->image_base;
        put(&n, 4);
        return true;
    }

    bool PEWriter::WriteStaticData(PELib &peLib) const
    {
        if (rva_.size)
        {
            align(fileAlign_);
            put(rva_.base, rva_.size);
        }
        return true;
    }
    void PEWriter::VersionString(const wchar_t *name, const char *value) const
    {
        Word n1 = wcslen(name) * 2 + strlen(value) * 2 + 6 + 2 + 2;
        DWord n = wcslen(name) + 2;
        if (n % 4)
            n1 += n - n % 4;
        n = (strlen(value) + 1) * 2;
        if (n % 4)
            n1 += n - n % 4;
        put(&n1, sizeof(n1)); // length
        n1 = (strlen(value) + 1) * 2; // value length
        put(&n1, sizeof(n1)); // length
        wchar_t buf[512];
        for (int i = 0; i < n1 / 2; i++)
            buf[i] = value[i];
        n1 = 1; // type: string
        put(&n1, sizeof(n1)); // length
        put(name, wcslen(name) + 2);
        align(4);
        put(buf, n1 * 2);
        align(4);
    }
    bool PEWriter::WriteVersionInfo(PELib &peLib) const
    {
        align(fileAlign_);
        PEResourceDirTable resTable;
        PEResourceDirEntry dir;
        memset(&resTable, 0, sizeof(resTable));
        resTable.ident_entry = 1;
        dir.rva_or_id = 16; // version info
        dir.escape = 1; // points to a subdir
        dir.subdir_or_data = sizeof(resTable) + sizeof(dir);
        put(&resTable, sizeof(resTable));
        put(&dir, sizeof(dir));
        dir.rva_or_id = 1; // index
        dir.subdir_or_data += sizeof(resTable) + sizeof(dir);
        put(&resTable, sizeof(resTable));
        put(&dir, sizeof(dir));
        dir.rva_or_id = 0;
        dir.escape = 0; // points to a data table
        dir.subdir_or_data += sizeof(resTable) + sizeof(dir);
        put(&resTable, sizeof(resTable));
        put(&dir, sizeof(dir));

        PEResourceDataEntry resourceData;
        memset(&resourceData, 0, sizeof(resourceData));
        resourceData.rva = dir.subdir_or_data + peHeader_->resource_rva + sizeof(PEResourceDataEntry);
        resourceData.size = peHeader_->resource_size - dir.subdir_or_data - sizeof(PEResourceDataEntry);
        put(&resourceData, sizeof(PEResourceDataEntry));
        Word n1 = resourceData.size;
        put(&n1, sizeof(n1)); // length of resource
        n1 = 0x34; // length of first record
        put(&n1, sizeof(n1));
        n1 = 0; // type bin
        put(&n1, sizeof(n1));
        // the strings in this section presume WINDOWS compilers which compile
        // strings to two Bytes, GNU compilers would compile them to 4.
        put(L"VS_VERSION_INFO", sizeof(L"VS_VERSION_INFO"));
        align(4);
        FixedVersionInfo versionInfo;
        memset(&versionInfo, 0, sizeof(versionInfo));
        versionInfo.Signature = 0x0FEEF04BD;
        versionInfo.StrucVersion = 0x10000;
        versionInfo.FileFlagsMask = 0x3f;
        versionInfo.FileOS = 4;
        versionInfo.FileType = 1;
        versionInfo.FileVersionMS = (fileVersion_[3] << 16) + fileVersion_[2];
        versionInfo.FileVersionLS = (fileVersion_[1] << 16) + fileVersion_[0];
        versionInfo.ProductVersionMS = (productVersion_[3] << 16) + productVersion_[2];
        versionInfo.ProductVersionLS = (productVersion_[1] << 16) + productVersion_[0];
        put(&versionInfo, sizeof(versionInfo));

        n1 = 0x44; // length
        put(&n1, sizeof(n1));
        n1 = 0; // value length
        put(&n1, sizeof(n1));
        n1 = 1; // type text
        put(&n1, sizeof(n1));
        put(L"VarFileInfo", sizeof(L"VarFileInfo"));
        align(4);
        n1 = 0x24; // length (nested)
        put(&n1, sizeof(n1));
        n1 = 4; // value length
        put(&n1, sizeof(n1));
        n1 = 1; // type text
        put(&n1, sizeof(n1));
        put(L"VarFileInfo", sizeof(L"VarFileInfo"));
        align(4);
        put(L"Translation", sizeof(L"Language"));
        align(4);
        DWord n = language_ << 16;
        put(&n, sizeof(n));

        std::string nn = peLib.FileName();
        n = nn.find_last_of("\\");
        if (n != std::string::npos)
        {
            nn = nn.substr(n);
        }
        n = nn.size();
        n += 368;
        char versions[3][256];
        sprintf(versions[0], "%d.%d.%d.%d", fileVersion_[0], fileVersion_[1], fileVersion_[2], fileVersion_[3]);
        n += strlen(versions[0]);
        sprintf(versions[1], "%d.%d.%d.%d", productVersion_[0], productVersion_[1], productVersion_[2], productVersion_[3]);
        n += strlen(versions[1]);
        sprintf(versions[2], "%d.%d.%d.%d", assemblyVersion_[0], assemblyVersion_[1], assemblyVersion_[2], assemblyVersion_[3]);
        n += strlen(versions[2]);

        n1 = n + 0x24; // outer length
        put(&n1, sizeof(n1));
        n1 = 0; // value length
        put(&n1, sizeof(n1));
        n1 = 1; // type text
        put(&n1, sizeof(n1));
        n1 = n; // inner length
        put(&n1, sizeof(n1));
        n1 = 0; // value length
        put(&n1, sizeof(n1));
        n1 = 1; // type text
        put(&n1, sizeof(n1));
        wchar_t buf[256];
        swprintf(buf, L"%08x", language_);
        put(buf, 18);
        align(4);
        VersionString(L"FileDescription", " ");
        VersionString(L"FileVersion", versions[0]);
        VersionString(L"InternalName", nn.c_str());
        VersionString(L"LegalCopyright", " ");
        VersionString(L"OriginalFilename", nn.c_str());
        VersionString(L"ProductVersion", versions[1]);
        VersionString(L"Assembly Version", versions[2]);
        return true;

    }
    bool PEWriter::WriteRelocs(PELib &peLib) const
    {
        align(fileAlign_);
        DWord n = peHeader_->entry_point + 2;
        Word n1 = (PE_FIXUP_HIGHLOW << 12) | (n & 0xfff);
        n &= ~0xfff;
        put(&n, 4);
        n = 12; // block size
        put(&n, 4);
        put(&n1, 2);
        n1 = 0;
        put(&n1, 0);
        // aligns the end of the file
        align(fileAlign_);
        return true;
    }

}