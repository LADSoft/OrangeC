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
#include "MZHeader.h"
#include "PEHeader.h"
#include <time.h>
#include <stdio.h>
namespace DotNetPELib
{

// this is a CUSTOM version string for microsoft.
// standard CIL differs
#define RTV_STRING "v4.0.30319"

Byte PEWriter::MZHeader_[] = {
    0x4d, 0x5a, 0x90, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xb8, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x0e, 0x1f, 0xba, 0x0e, 0x00, 0xb4, 0x09, 0xcd, 0x21, 0xb8, 0x01, 0x4c,
    0xcd, 0x21, 0x54, 0x68, 0x69, 0x73, 0x20, 0x70, 0x72, 0x6f, 0x67, 0x72, 0x61, 0x6d, 0x20, 0x63, 0x61, 0x6e, 0x6e,
    0x6f, 0x74, 0x20, 0x62, 0x65, 0x20, 0x72, 0x75, 0x6e, 0x20, 0x69, 0x6e, 0x20, 0x44, 0x4f, 0x53, 0x20, 0x6d, 0x6f,
    0x64, 0x65, 0x2e, 0x0d, 0x0d, 0x0a, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const char* PEWriter::streamNames_[5] = {"#~", "#Strings", "#US", "#GUID", "#Blob"};

static DotNetMetaHeader metaHeader1 = {META_SIG, 1, 1, 0};
DotNetMetaHeader* PEWriter::metaHeader_ = &metaHeader1;

DWord PEWriter::cildata_rva_;
Byte PEWriter::defaultUS_[8] = {0, 3, 0x20, 0, 0};

size_t PEMethod::Write(size_t sizes[MaxTables + ExtraIndexes], std::iostream& out) const
{
    Byte dest[512];
    int n;
    if ((flags_ & 3) == TinyFormat)
    {
        n = 1;
        *(Byte*)dest = (flags_ & 3) + (codeSize_ << 2);
    }
    else
    {
        n = 12;
        *(Word*)dest = 0x3000 + (flags_ & 0xfff) + (sehData_.size() ? (int)MoreSects : 0);
        *(Word*)(dest + 2) = maxStack_;
        *(DWord*)(dest + 4) = codeSize_;
        *(DWord*)(dest + 8) = signatureToken_;
    }
    out.write((char*)dest, n);
    out.write((char*)code_, codeSize_);
    n += codeSize_;
    if (sehData_.size())
    {
        if (n % 4)
        {
            char align[4];
            memset(align, 0, sizeof(align));
            out.write((char*)align, 4 - n % 4);
            n = n + 3;
            n = n & ~3;
        }
        int end = 0;
        while (end < sehData_.size())
        {
            const CodeContainer::SEHData& edata = sehData_[end];
            bool etiny =
                edata.tryOffset < 65536 && edata.tryLength < 256 && edata.handlerOffset < 65536 && edata.handlerLength < 256;
            if (!etiny)
                break;
            end++;
        }
        if (end >= sehData_.size() && sehData_.size() < 21)
        {
            Byte header[4];
            header[0] = EHTable;
            header[1] = sehData_.size() * 12 + 4;
            header[2] = 0;
            header[3] = 0;
            out.write((char*)header, 4);
            n += 4;
            for (int i = 0; i < sehData_.size(); i++)
            {
                const CodeContainer::SEHData& data = sehData_[i];
                Byte bytes[12];
                bytes[0] = data.flags;
                bytes[1] = 0;
                bytes[2] = data.tryOffset & 0xff;
                bytes[3] = (data.tryOffset >> 8) & 0xff;
                bytes[4] = data.tryLength;
                bytes[5] = data.handlerOffset & 0xff;
                bytes[6] = (data.handlerOffset >> 8) & 0xff;
                bytes[7] = data.handlerLength;
                if (data.flags & CodeContainer::SEHData::Filter)
                {
                    bytes[8] = data.filterOffset & 0xff;
                    bytes[9] = (data.filterOffset >> 8) & 0xff;
                    bytes[10] = (data.filterOffset >> 16) & 0xff;
                    bytes[11] = (data.filterOffset >> 24) & 0xff;
                }
                else
                {
                    bytes[8] = data.classToken & 0xff;
                    bytes[9] = (data.classToken >> 8) & 0xff;
                    bytes[10] = (data.classToken >> 16) & 0xff;
                    bytes[11] = (data.classToken >> 24) & 0xff;
                }
                out.write((char*)bytes, 12);
                n += 12;
            }
        }
        else
        {
            Byte header[4];
            header[0] = EHTable | EHFatFormat;
            int q = sehData_.size() * 24 + 4;
            header[1] = q & 0xff;
            header[2] = (q >> 8) & 0xff;
            header[3] = (q >> 16) & 0xff;
            out.write((char*)header, 4);
            n += 4;
            for (int i = 0; i < sehData_.size(); i++)
            {
                const CodeContainer::SEHData& data = sehData_[i];
                Byte bytes[24];
                bytes[0] = data.flags;
                bytes[1] = 0;
                bytes[2] = 0;
                bytes[3] = 0;
                bytes[4] = data.tryOffset & 0xff;
                bytes[5] = (data.tryOffset >> 8) & 0xff;
                bytes[6] = (data.tryOffset >> 16) & 0xff;
                bytes[7] = (data.tryOffset >> 24) & 0xff;
                bytes[8] = data.tryLength & 0xff;
                bytes[9] = (data.tryLength >> 8) & 0xff;
                bytes[10] = (data.tryLength >> 16) & 0xff;
                bytes[11] = (data.tryLength >> 24) & 0xff;
                bytes[12] = data.handlerOffset & 0xff;
                bytes[13] = (data.handlerOffset >> 8) & 0xff;
                bytes[14] = (data.handlerOffset >> 16) & 0xff;
                bytes[15] = (data.handlerOffset >> 24) & 0xff;
                bytes[16] = data.handlerLength & 0xff;
                bytes[17] = (data.handlerLength >> 8) & 0xff;
                bytes[18] = (data.handlerLength >> 16) & 0xff;
                bytes[19] = (data.handlerLength >> 24) & 0xff;
                if (data.flags & CodeContainer::SEHData::Filter)
                {
                    bytes[20] = data.filterOffset & 0xff;
                    bytes[21] = (data.filterOffset >> 8) & 0xff;
                    bytes[22] = (data.filterOffset >> 16) & 0xff;
                    bytes[23] = (data.filterOffset >> 24) & 0xff;
                }
                else
                {
                    bytes[20] = data.classToken & 0xff;
                    bytes[21] = (data.classToken >> 8) & 0xff;
                    bytes[22] = (data.classToken >> 16) & 0xff;
                    bytes[23] = (data.classToken >> 24) & 0xff;
                }
                out.write((char*)bytes, 24);
                n += 24;
            }
        }
    }
    return n;
}

void PEWriter::pool::Ensure(size_t newSize)
{
    if (size + newSize > maxSize)
    {
        if (size + newSize < maxSize * 2)
            maxSize = maxSize * 2;
        else
            maxSize = (maxSize + newSize) * 2;
        base = (Byte*)realloc(base, maxSize);
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
            TableEntryBase* x = *it;
            delete x;
        }
    }
    for (auto it = methods_.begin(); it != methods_.end(); ++it)
    {
        PEMethod* method = *it;
        delete method;
    }
}
size_t PEWriter::AddTableEntry(TableEntryBase* entry)
{
    int n = entry->TableIndex();
    tables_[n].push_back(entry);
    return tables_[n].size();
}
void PEWriter::AddMethod(PEMethod* method)
{
    if (method->flags_ & PEMethod::EntryPoint)
    {
        if (entryPoint_)
            throw new PELibError(PELibError::MultipleEntryPoints);
        entryPoint_ = method->methodDef_ | (tMethodDef << 24);
    }
    methods_.push_back(method);
}
size_t PEWriter::HashString(const std::string& utf8)
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
        us_.base[us_.size++] = blobLen;
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
        else if (n <= 8 || (n >= 0x0e && n < 0x20) || n == 0x27 || n == 0x2d || n == 0x7f)
            flag = 1;
        us_.base[us_.size++] = n & 0xff;
        us_.base[us_.size++] = n >> 8;
    }
    us_.base[us_.size++] = flag;
    return rv;
}
size_t PEWriter::HashGUID(Byte* Guid)
{
    guid_.Ensure(128 / 8);
    size_t rv = guid_.size;
    memcpy(guid_.base + rv, Guid, 128 / 8);
    guid_.size += 128 / 8;
    return (rv / (128 / 8) + 1);
}
size_t PEWriter::HashBlob(Byte* blobData, size_t blobLen)
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
        blob_.base[blob_.size++] = blobLen;
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
size_t PEWriter::RVABytes(Byte* Bytes, size_t dataLen)
{
    int pos = rva_.size;
    rva_.Ensure(dataLen + pos - rva_.size);
    size_t rv = rva_.size;
    rva_.size += pos - rva_.size + dataLen;
    memcpy(rva_.base + rv, Bytes, dataLen);
    return rv;
}

void PEWriter::CalculateObjects(PELib& peLib)
{
    peHeader_ = new PEHeader;
    memset(peHeader_, 0, sizeof(PEHeader));
    peHeader_->signature = PESIG;
    peHeader_->cpu_type = PE_INTEL386;
    peHeader_->magic = PE_MAGICNUM;
    peHeader_->nt_hdr_size = 0xe0;  // optional header sie
    peHeader_->flags = PE_FILE_EXECUTABLE + (DLL_ ? PE_FILE_LIBRARY : 0);
    peHeader_->linker_major_version = 6;
    peHeader_->object_align = objectAlign_;
    peHeader_->file_align = fileAlign_;
    peHeader_->image_base = imageBase_;
    peHeader_->os_major_version = 4;
    peHeader_->subsys_major_version = 4;
    peHeader_->subsystem = GUI_ ? PE_SUBSYS_WINDOWS : PE_SUBSYS_CONSOLE;
    peHeader_->dll_flags = 0x8540;  // magic!
    peHeader_->stack_size = 0x100000;
    peHeader_->stack_commit = 0x1000;
    peHeader_->heap_size = 0x100000;
    peHeader_->heap_commit = 0x1000;
    peHeader_->num_rvas = 16;

    peHeader_->num_objects = 2;
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

    if (snkFile_.size())
    {
        snkLen_ = rsaEncoder.LoadStrongNameKeys(snkFile_.c_str());
        if (snkLen_)
        {
            Byte buf[16384];
            size_t len = 0;
            rsaEncoder.GetPublicKeyData(buf, &len);
            ((AssemblyDefTableEntry*)tables_[tAssemblyDef][0])->publicKeyIndex_ = HashBlob(buf, len);
            ((AssemblyDefTableEntry*)tables_[tAssemblyDef][0])->flags_ |= PublicKey;

            cor20Header_->Flags |= 8;  // strong name signed
        }
        if (!snkLen_)
        {
            std::cout << "Warning: key file not found or invalid.   Assembly will not be signed" << std::endl;
        }
    }
    cildata_rva_ = currentRVA;
    if (rva_.size)
    {
        currentRVA += rva_.size;
        if (currentRVA % 8)
            currentRVA += 8 - (currentRVA % 8);
    }
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
            if (method->sehData_.size())
            {
                if (currentRVA % 4)
                    currentRVA += 4 - currentRVA % 4;
                int end = 0;
                const CodeContainer::SEHData& data = method->sehData_[end];
                while (end < method->sehData_.size())
                {
                    const CodeContainer::SEHData& edata = method->sehData_[end];
                    bool etiny = edata.tryOffset < 65536 && edata.tryLength < 256 && edata.handlerOffset < 65536 &&
                                 edata.handlerLength < 256;
                    if (!etiny)
                        break;
                    end++;
                }
                if (end >= method->sehData_.size() && method->sehData_.size() < 21)
                {
                    currentRVA += 4 + method->sehData_.size() * 12;
                }
                else
                {
                    currentRVA += 4 + method->sehData_.size() * 24;
                }
            }
        }
        else
        {
            method->rva_ = 0;
        }
    }
    if (currentRVA % 4)
    {
        currentRVA += 4 - currentRVA % 4;
    }
    cor20Header_->MetaData[0] = currentRVA;  // metadata root

    currentRVA += 12;  // metadataHeader;
    currentRVA += 4;   // version size;
    currentRVA += sizeof(RTV_STRING);
    if (currentRVA % 4)
        currentRVA += 4 - currentRVA % 4;
    currentRVA += 2;  // flags
    currentRVA += 2;  // # streams, will be 5 in our implementation

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
    if (strings_.size >= 65536)
        tablesHeader_->HeapOffsetSizes |= 1;
    if (guid_.size >= 65536)
        tablesHeader_->HeapOffsetSizes |= 2;
    if (blob_.size >= 65536)
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
    currentRVA += sizeof(DotNetMetaTablesHeader);  // tables header
    currentRVA += n * sizeof(DWord);               // table counts;

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
        currentRVA += sizeof(defaultUS_);  // US May be empty in our implementation we put an empty string there
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
    if (snkLen_)
    {

        cor20Header_->StrongNameSignature[0] = currentRVA;
        cor20Header_->StrongNameSignature[1] = snkLen_;

        currentRVA += snkLen_;
    }

    int sect = 0;
    peObjects_[sect].virtual_size = currentRVA - peObjects_[sect].virtual_addr;
    n = peObjects_[sect].virtual_size;
    if (n % fileAlign_)
        n += fileAlign_ - n % fileAlign_;
    peObjects_[sect].raw_size = n;
    peHeader_->code_size = n;

    if (currentRVA % objectAlign_)
        currentRVA += objectAlign_ - currentRVA % objectAlign_;
    peObjects_[sect + 1].raw_ptr = peObjects_[sect].raw_ptr + peObjects_[sect].raw_size;
    peObjects_[sect + 1].virtual_addr = currentRVA;
    peHeader_->data_base = currentRVA;
    sect++;

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
    currentRVA += 12;  // sizeof relocs

    peObjects_[sect].virtual_size = currentRVA - peObjects_[sect].virtual_addr;
    peHeader_->fixup_size = peObjects_[sect].virtual_size;
    n = peObjects_[sect].virtual_size;
    if (n % fileAlign_)
        n += fileAlign_ - n % fileAlign_;
    peObjects_[sect].raw_size = n;
    peHeader_->data_size += n;

    if (currentRVA % objectAlign_)
        currentRVA += objectAlign_ - currentRVA % objectAlign_;
    peObjects_[sect + 1].raw_ptr = peObjects_[sect].raw_ptr + peObjects_[sect].raw_size;
    peObjects_[sect + 1].virtual_addr = currentRVA;

    peHeader_->image_size = currentRVA;
}
void PEWriter::HashPartOfFile(SHA1Context& context, size_t offset, size_t len)
{
    outputFile_->seekg(offset);
    char buf[8192];
    int sz = 0x0;
    while (sz < len)
    {
        int l = len - sz > 8192 ? 8192 : len - sz;
        outputFile_->read(buf, l);
        SHA1Input(&context, (Byte*)buf, l);
        sz += l;
    }
}
bool PEWriter::WriteFile(PELib& peLib, std::iostream& out)
{
    outputFile_ = &out;
    if (!entryPoint_ && !DLL_)
        throw new PELibError(PELibError::MissingEntryPoint);
    CalculateObjects(peLib);
    bool rv = WriteMZData(peLib) && WritePEHeader(peLib) && WritePEObjects(peLib) && WriteIAT(peLib) && WriteCoreHeader(peLib) &&
              WriteStaticData(peLib) && WriteMethods(peLib) && WriteMetadataHeaders(peLib) && WriteTables(peLib) &&
              WriteStrings(peLib) && WriteUS(peLib) && WriteGUID(peLib) && WriteBlob(peLib) && WriteImports(peLib) &&
              WriteEntryPoint(peLib) && WriteHashData(peLib) &&
              //        WriteVersionInfo(peLib) &&
              WriteRelocs(peLib);
    if (rv && snkLen_)
    {
        SHA1Context context;
        SHA1Reset(&context);

        int pos = 0;
        HashPartOfFile(context, 0, 0x80);
        // if there was something between here and the PE header we would hash it now

        // well we are supposed to zero the pe header checksum and the
        // authenticode signature pointer, but, since we don't set them nonzero anyway
        // this is fine.
        HashPartOfFile(context, 0x80, 0xf8);

        HashPartOfFile(context, 0x80 + 0xf8, sizeof(PEObject) * peHeader_->num_objects);

        // yes we do NOT hash the gap between the objects table and the first section
        for (int i = 0; i < peHeader_->num_objects; i++)
        {
            if (peObjects_[i].virtual_addr < cor20Header_->StrongNameSignature[0] &&
                cor20Header_->StrongNameSignature[0] < peObjects_[i].virtual_addr + peObjects_[i].virtual_size)
            {
                int offs = cor20Header_->StrongNameSignature[0] - peObjects_[i].virtual_addr;
                int sz = cor20Header_->StrongNameSignature[1];
                HashPartOfFile(context, peObjects_[i].raw_ptr, offs);
                HashPartOfFile(context, peObjects_[i].raw_ptr + offs + sz, peObjects_[i].raw_size - offs - sz);
            }
            else
            {
                HashPartOfFile(context, peObjects_[i].raw_ptr, peObjects_[i].raw_size);
            }
        }

        SHA1Result(&context);
        Byte sigHash[16384];
        memset(sigHash, 0xfe, 128);
        size_t sigLen = 0;
        rsaEncoder.GetStrongNameSignature(sigHash, &sigLen, (Byte*)context.Message_Digest, 20);
        outputFile_->seekp(snkBase_);
        outputFile_->write((char*)sigHash, sigLen);
    }
    return rv;
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
bool PEWriter::WriteMZData(PELib& peLib) const
{
    put(MZHeader_, sizeof(MZHeader_));
    return true;
}
bool PEWriter::WritePEHeader(PELib& peLib)
{
    peBase_ = outputFile_->tellp();
    put(peHeader_, sizeof(PEHeader));
    return true;
}
bool PEWriter::WritePEObjects(PELib& peLib) const
{
    put(peObjects_, sizeof(PEObject) * peHeader_->num_objects);
    return true;
}
bool PEWriter::WriteIAT(PELib& peLib) const
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
bool PEWriter::WriteCoreHeader(PELib& peLib)
{
    corBase_ = outputFile_->tellp();
    put(cor20Header_, sizeof(DotNetCOR20Header));
    return true;
}
bool PEWriter::WriteHashData(PELib& peLib)
{
    snkBase_ = outputFile_->tellp();
    if (snkLen_)
    {
        Byte buf[2048];
        memset(buf, 0, snkLen_);
        put(buf, snkLen_);
    }
    return true;
}
bool PEWriter::WriteMethods(PELib& peLib) const
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
    for (std::list<PEMethod*>::const_iterator it = methods_.begin(); it != methods_.end(); ++it)
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
bool PEWriter::WriteMetadataHeaders(PELib& peLib) const
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
bool PEWriter::WriteTables(PELib& peLib) const
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
        {
            put(&n, sizeof(n));
        }
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
bool PEWriter::WriteStrings(PELib& peLib) const
{
    put(strings_.base, strings_.size);
    align(4);
    return true;
}
bool PEWriter::WriteUS(PELib& peLib) const
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
bool PEWriter::WriteGUID(PELib& peLib) const
{
    put(guid_.base, guid_.size);
    align(4);
    return true;
}
bool PEWriter::WriteBlob(PELib& peLib) const
{
    put(blob_.base, blob_.size);
    align(4);
    return true;
}
bool PEWriter::WriteImports(PELib& peLib) const
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
    put(&n, sizeof(n));  // thunk
    align(16);
    put(&n, 2);  // ord
    if (DLL_)
        put("_CorDllMain", sizeof("_CorDllMain"));
    else
        put("_CorExeMain", sizeof("_CorExeMain"));
    put("mscoree.dll", sizeof("mscoree.dll"));
    align(4);
    return true;
}
bool PEWriter::WriteEntryPoint(PELib& peLib) const
{
    DWord n = 0;
    put(&n, 2);
    n = 0x25ff;  // JMP[xxx];
    put(&n, 2);
    n = peObjects_[0].virtual_addr + peHeader_->image_base;
    put(&n, 4);
    return true;
}

bool PEWriter::WriteStaticData(PELib& peLib) const
{
    if (rva_.size)
    {
        put(rva_.base, rva_.size);
        align(8);
    }
    return true;
}
void PEWriter::VersionString(const wchar_t* name, const char* value) const
{
    Word n1 = wcslen(name) * 2 + strlen(value) * 2 + 6 + 2 + 2;
    DWord n = wcslen(name) + 2;
    if (n % 4)
        n1 += n - n % 4;
    n = (strlen(value) + 1) * 2;
    if (n % 4)
        n1 += n - n % 4;
    put(&n1, sizeof(n1));          // length
    n1 = (strlen(value) + 1) * 2;  // value length
    put(&n1, sizeof(n1));          // length
    wchar_t buf[512];
    for (int i = 0; i < n1 / 2; i++)
        buf[i] = value[i];
    n1 = 1;                // type: string
    put(&n1, sizeof(n1));  // length
    put(name, wcslen(name) + 2);
    align(4);
    put(buf, n1 * 2);
    align(4);
}
bool PEWriter::WriteVersionInfo(PELib& peLib) const
{
    align(fileAlign_);
    PEResourceDirTable resTable;
    PEResourceDirEntry dir;
    memset(&resTable, 0, sizeof(resTable));
    resTable.ident_entry = 1;
    dir.rva_or_id = 16;  // version info
    dir.escape = 1;      // points to a subdir
    dir.subdir_or_data = sizeof(resTable) + sizeof(dir);
    put(&resTable, sizeof(resTable));
    put(&dir, sizeof(dir));
    dir.rva_or_id = 1;  // index
    dir.subdir_or_data += sizeof(resTable) + sizeof(dir);
    put(&resTable, sizeof(resTable));
    put(&dir, sizeof(dir));
    dir.rva_or_id = 0;
    dir.escape = 0;  // points to a data table
    dir.subdir_or_data += sizeof(resTable) + sizeof(dir);
    put(&resTable, sizeof(resTable));
    put(&dir, sizeof(dir));

    PEResourceDataEntry resourceData;
    memset(&resourceData, 0, sizeof(resourceData));
    resourceData.rva = dir.subdir_or_data + peHeader_->resource_rva + sizeof(PEResourceDataEntry);
    resourceData.size = peHeader_->resource_size - dir.subdir_or_data - sizeof(PEResourceDataEntry);
    put(&resourceData, sizeof(PEResourceDataEntry));
    Word n1 = resourceData.size;
    put(&n1, sizeof(n1));  // length of resource
    n1 = 0x34;             // length of first record
    put(&n1, sizeof(n1));
    n1 = 0;  // type bin
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

    n1 = 0x44;  // length
    put(&n1, sizeof(n1));
    n1 = 0;  // value length
    put(&n1, sizeof(n1));
    n1 = 1;  // type text
    put(&n1, sizeof(n1));
    put(L"VarFileInfo", sizeof(L"VarFileInfo"));
    align(4);
    n1 = 0x24;  // length (nested)
    put(&n1, sizeof(n1));
    n1 = 4;  // value length
    put(&n1, sizeof(n1));
    n1 = 1;  // type text
    put(&n1, sizeof(n1));
    put(L"VarFileInfo", sizeof(L"VarFileInfo"));
    align(4);
    put(L"Translation", sizeof(L"Language"));
    align(4);
    size_t n = language_ << 16;
    put(&n, sizeof(DWord));

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

    n1 = n + 0x24;  // outer length
    put(&n1, sizeof(n1));
    n1 = 0;  // value length
    put(&n1, sizeof(n1));
    n1 = 1;  // type text
    put(&n1, sizeof(n1));
    n1 = n;  // inner length
    put(&n1, sizeof(n1));
    n1 = 0;  // value length
    put(&n1, sizeof(n1));
    n1 = 1;  // type text
    put(&n1, sizeof(n1));
    wchar_t buf[256];
    swprintf(buf, 256, L"%08x", language_);
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
bool PEWriter::WriteRelocs(PELib& peLib) const
{
    align(fileAlign_);
    DWord n = peHeader_->entry_point + 2;
    Word n1 = (PE_FIXUP_HIGHLOW << 12) | (n & 0xfff);
    n &= ~0xfff;
    put(&n, 4);
    n = 12;  // block size
    put(&n, 4);
    put(&n1, 2);
    n1 = 0;
    put(&n1, 0);
    // aligns the end of the file
    align(fileAlign_);
    return true;
}
}  // namespace DotNetPELib