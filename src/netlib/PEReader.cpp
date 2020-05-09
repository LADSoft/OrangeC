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

#ifdef _WIN32
#    include "Windows.h"
#endif
#include "DotNetPELib.h"
#include "PEFile.h"
#include "MZHeader.h"
#include "PEHeader.h"
#include "DLLExportReader.h"
#include <time.h>
#include <stdio.h>
namespace DotNetPELib
{

std::string DIR_SEP = "\\";
TableEntryBase* TableEntryFactory::GetEntry(size_t index)
{
    switch (index)
    {
        case tModule:
            return new ModuleTableEntry;
        case tTypeRef:
            return new TypeRefTableEntry;
        case tTypeDef:
            return new TypeDefTableEntry;
        case tField:
            return new FieldTableEntry;
        case tMethodDef:
            return new MethodDefTableEntry;
        case tParam:
            return new ParamTableEntry;
        case tInterfaceImpl:
            return new InterfaceImplTableEntry;
        case tMemberRef:
            return new MemberRefTableEntry;
        case tConstant:
            return new ConstantTableEntry;
        case tCustomAttribute:
            return new CustomAttributeTableEntry;
        case tFieldMarshal:
            return new FieldMarshalTableEntry;
        case tDeclSecurity:
            return new DeclSecurityTableEntry;
        case tClassLayout:
            return new ClassLayoutTableEntry;
        case tFieldLayout:
            return new FieldLayoutTableEntry;
        case tStandaloneSig:
            return new StandaloneSigTableEntry;
        case tEventMap:
            return new EventMapTableEntry;
        case tEvent:
            return new EventTableEntry;
        case tPropertyMap:
            return new PropertyMapTableEntry;
        case tProperty:
            return new PropertyTableEntry;
        case tMethodSemantics:
            return new MethodSemanticsTableEntry;
        case tMethodImpl:
            return new MethodImplTableEntry;
        case tModuleRef:
            return new ModuleRefTableEntry;
        case tTypeSpec:
            return new TypeSpecTableEntry;
        case tImplMap:
            return new ImplMapTableEntry;
        case tFieldRVA:
            return new FieldRVATableEntry;
        case tAssemblyDef:
            return new AssemblyDefTableEntry;
        case tAssemblyRef:
            return new AssemblyRefTableEntry;
        case tFile:
            return new FileTableEntry;
        case tExportedType:
            return new ExportedTypeTableEntry;
        case tManifestResource:
            return new ManifestResourceTableEntry;
        case tNestedClass:
            return new NestedClassTableEntry;
        case tGenericParam:
            return new GenericParamTableEntry;
        case tMethodSpec:
            return new MethodSpecTableEntry;
        case tGenericParamConstraint:
            return new GenericParamConstraintsTableEntry;
        default:
            return nullptr;
    }
}
PEReader::~PEReader()
{
    delete[] inputFile_;
    delete[] objects_;
    delete[] stringData_;
    delete[] blobData_;
    for (int i = 0; i < MaxTables; i++)
    {
        for (int j = 0; j < tables_[i].size(); j++)
        {
            TableEntryBase* p = tables_[i][j];
            delete p;
        }
    }
}
std::string PEReader::SearchOnPath(const std::string& fileName)
{
    // if it exists in the current directory accept it
    std::fstream find(fileName.c_str(), std::ios::in);
    if (find.is_open())
    {
        return fileName;
    }
    // else look on the path
    std::string path = libPath_;
    std::string rv;
    std::vector<std::string> split;
    size_t npos = path.find(";");
    while (npos != std::string::npos)
    {
        split.push_back(path.substr(0, npos));
        if (npos > path.size() - 1)
        {
            path = path.substr(npos);
        }
        else
        {
            path = "";
        }
        npos = path.find(";");
    }
    if (path.size())
        split.push_back(path);
    for (auto s : split)
    {
        std::string name = s + DIR_SEP + fileName;
        std::fstream find(name.c_str(), std::ios::in);
        if (find.is_open())
        {
            rv = name;
            break;
        }
    }
    return rv;
}
std::string PEReader::FindGACPath(const std::string& path, const std::string& assemblyName, int major, int minor, int build,
                                  int revision)
{
    std::string rv;
#ifdef _WIN32
    std::string root = path + DIR_SEP + assemblyName + DIR_SEP + "*.*";
    WIN32_FIND_DATA data;
    HANDLE h = FindFirstFile(root.c_str(), &data);
    if (h != INVALID_HANDLE_VALUE)
    {
        char buf[256];
        sprintf(buf, "%d.%d.%d.%d__", major, minor, build, revision);
        std::string match = "";
        do
        {
            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (major == 0 && minor == 0 && build == 0 && revision == 0)
                {
                    if (std::string(data.cFileName) > match)
                        match = data.cFileName;
                }
                else
                {
                    if (std::string(data.cFileName).find(buf) != std::string::npos)
                        match = data.cFileName;
                }
            }
        } while (FindNextFile(h, &data));
        FindClose(h);
        if (match.size())
        {
            rv = path + DIR_SEP + assemblyName + DIR_SEP + match + DIR_SEP + assemblyName + ".dll";
        }
    }
#endif
    return rv;
}
std::string PEReader::SearchForManagedFile(const std::string& assemblyName, int major, int minor, int build, int revision)
{
    std::string rv;
    char* windir = getenv("windir");
    if (windir)
    {
        // search for pure msil solution (.net v4)
        rv = FindGACPath(std::string(windir) + DIR_SEP + "Microsoft.Net" + DIR_SEP + "Assembly" + DIR_SEP + "GAC_MSIL",
                         assemblyName, major, minor, build, revision);
        // if not search for 32 bit solution (.net v4)
        if (!rv.size())
            rv = FindGACPath(std::string(windir) + DIR_SEP + "Microsoft.Net" + DIR_SEP + "Assembly" + DIR_SEP + "GAC_32",
                             assemblyName, major, minor, build, revision);
        // now search for msil solution (pre v.4)
        if (!rv.size())
            rv = FindGACPath(std::string(windir) + DIR_SEP + "Assembly" + DIR_SEP + "GAC_MSIL", assemblyName, major, minor, build,
                             revision);
        // now search for 32-bit solution (pre v.4)
        if (!rv.size())
            rv = FindGACPath(std::string(windir) + DIR_SEP + "Assembly" + DIR_SEP + "GAC_32", assemblyName, major, minor, build,
                             revision);
    }
    return rv;
}
int PEReader::ManagedLoad(std::string assemblyName, int major, int minor, int build, int revision)

{
    std::string t = SearchOnPath(assemblyName + ".dll");
    if (t.size())
        assemblyName = t;
    else
        assemblyName = SearchForManagedFile(assemblyName, major, minor, build, revision);
    inputFile_ = new std::fstream(assemblyName.c_str(), std::ios::in | std::ios::binary);
    if (!inputFile_->fail())
    {
        size_t peLoc = PELocation();
        if (!peLoc)
            return ERR_NOT_PE;
        corRVA_ = Cor20Location(peLoc);
        if (!corRVA_)
            return ERR_NOT_ASSEMBLY;
        int rv = ReadTables(corRVA_);
        return rv;
    }
    else
    {
        return ERR_FILE_NOT_FOUND;
    }
}

void PEReader::get(void* buffer, size_t offset, size_t len)
{
    inputFile_->seekg(offset);
    inputFile_->read((char*)buffer, len);
}
size_t PEReader::RVAToFileLocation(size_t rva)
{
    for (int i = 0; i < num_objects_; i++)
    {
        if (rva >= objects_[i].virtual_addr && rva < objects_[i].virtual_addr + objects_[i].virtual_size)
        {
            return rva - objects_[i].virtual_addr + objects_[i].raw_ptr;
        }
    }
    return 0;
}
size_t PEReader::PELocation()
{
    Byte mz[0x40];
    get(mz, 0, sizeof(mz));
    if (*(short*)mz == MZ_SIGNATURE)
    {
        Byte pe[248];
        get(pe, *(DWord*)(mz + 0x3c), 248);
        if (*(short*)pe == PESIG)
        {
            PEHeader* peHeader = (PEHeader*)pe;
            if (peHeader->magic == PE_MAGICNUM && peHeader->nt_hdr_size == 0xe0)
            {
                // good enough, it sorta looks like a valid PE file
                // load the objects table
                num_objects_ = peHeader->num_objects;
                objects_ = new PEObject[num_objects_];
                get(objects_, *(DWord*)(mz + 0x3c) + sizeof(PEHeader), num_objects_ * sizeof(PEObject));
                // return the pe header offset
                return *(DWord*)(mz + 0x3c);
            }
        }
    }
    return 0;
}
size_t PEReader::Cor20Location(size_t headerOffs)
{
    PEHeader pe;
    get(&pe, headerOffs, sizeof(PEHeader));
    size_t offs = pe.com_rva;
    size_t size = pe.com_size;
    DotNetCOR20Header cor20;
    get(&cor20, RVAToFileLocation(offs), sizeof(cor20));
    if (size == cor20.cb)
    {
        if (cor20.MajorRuntimeVersion == 2 && cor20.MinorRuntimeVersion == 5)
        {
            size_t offs1 = cor20.MetaData[0];
            DotNetMetaHeader metaHeader;
            get(&metaHeader, RVAToFileLocation(offs1), sizeof(metaHeader));
            if (metaHeader.Signature == META_SIG)
            {
                if (metaHeader.Major == 1 && metaHeader.Minor == 1)
                {
                    // good enough, this sorta looks like a dot net assembly
                    return offs;
                }
            }
        }
    }
    return 0;
}
void PEReader::GetStream(size_t Cor20, const char* streamName, DWord pos[2])
{
    DotNetCOR20Header cor20;
    memset(pos, 0, sizeof(DWord) * 2);
    get(&cor20, RVAToFileLocation(Cor20), sizeof(cor20));
    size_t offs1 = cor20.MetaData[0];
    Byte streamHeaders[512];
    get(streamHeaders, RVAToFileLocation(offs1), sizeof(streamHeaders));
    Byte* p = streamHeaders + 12;
    p += *(DWord*)p + 4;
    if (*(Word*)p == 0 && *(Word*)(p + 2) == 5)
    {
        p += 4;
        for (int i = 0; i < 5; i++)
        {
            char* q = (char*)p + sizeof(DWord) * 2;
            if (!strcmp(q, streamName))
            {
                memcpy(pos, p, sizeof(DWord) * 2);
                pos[0] += offs1;
                return;
            }
            int l = 8 + strlen(q) + 1;
            if (l % 4)
                l += 4 - (l % 4);
            p += l;
        }
    }
}
int PEReader::ReadFromString(Byte* buf, size_t len, size_t offset)
{
    if (!stringPos_)
    {
        DWord pos[2];
        GetStream(corRVA_, "#Strings", pos);
        if (!pos[0])
            return 0;
        stringPos_ = RVAToFileLocation(pos[0]);
        stringData_ = new Byte[pos[1]];
        get(stringData_, stringPos_, pos[1]);
    }
    int i;
    for (i = offset; stringData_[i] && i < offset + len - 1; i++)
        buf[i - offset] = stringData_[i];
    buf[i - offset] = 0;
    return strlen((char*)buf);
}
int PEReader::ReadFromBlob(Byte* buf, size_t len, size_t offset)
{
    if (!blobPos_)
    {
        DWord pos[2];
        GetStream(corRVA_, "#Blob", pos);
        if (!pos[0])
            return 0;
        blobPos_ = RVAToFileLocation(pos[0]);
        blobData_ = new Byte[pos[1]];
        get(blobData_, blobPos_, pos[1]);
    }
    Byte sizearr[4];
    memcpy(sizearr, blobData_ + offset, 4);
    get(sizearr, blobPos_ + offset, 4);
    int offs, size;
    if (sizearr[0] < 128)
    {
        size = sizearr[0];
        offs = 1;
    }
    else if ((sizearr[0] & 0xc0) == 128)
    {
        size = ((sizearr[0] & 0x7f) << 8) + (sizearr[1] << 0);
        offs = 2;
    }
    else
    {
        size = ((sizearr[0] & 0x3f) << 24) + (sizearr[1] << 16) + (sizearr[2] << 8) + sizearr[3];
        offs = 4;
    }
    if (len >= size)
        memcpy(buf, blobData_ + offset + offs, size);
    return size > len ? len : size;
}
int PEReader::ReadFromGUID(Byte* buf, size_t len, size_t offset)
{
    if (!GUIDPos_)
    {
        DWord pos[2];
        GetStream(corRVA_, "#GUID", pos);
        if (!pos[0])
            return 0;
        GUIDPos_ = RVAToFileLocation(pos[0]);
    }
    get(buf, GUIDPos_ + offset - 1, len);
    return 16;
}
int PEReader::ReadTables(size_t Cor20)
{
    DWord pos[2];
    GetStream(Cor20, "#~", pos);
    if (!pos[0])
        return ERR_INVALID_ASSEMBLY;
    Byte* tableMem = new Byte[pos[1]];
    get(tableMem, RVAToFileLocation(pos[0]), pos[1]);
    DotNetMetaTablesHeader* mth = (DotNetMetaTablesHeader*)tableMem;
    ulonglong tableFlags = mth->MaskValid;
    if (tableFlags & ~tKnownTablesMask)
    {
        delete[] tableMem;
        return ERR_UNKNOWN_TABLE;
    }
    GetStream(Cor20, "#Strings", pos);
    sizes_[tString] = pos[1];
    GetStream(Cor20, "#US", pos);
    sizes_[tUS] = pos[1];
    GetStream(Cor20, "#GUID", pos);
    sizes_[tGUID] = pos[1];
    GetStream(Cor20, "#Blob", pos);
    sizes_[tBlob] = pos[1];
    Byte* p = tableMem + sizeof(*mth);
    for (int i = 0; i < 64; i++)
    {
        sizes_[i] = 0;
        if (tableFlags & (1ULL << i))
        {
            sizes_[i] = *(DWord*)p;
            p += 4;
        }
    }
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < sizes_[i]; j++)
        {
            TableEntryBase* newItem = TableEntryFactory::GetEntry(i);
            if (!newItem)
            {
                delete[] tableMem;
                return ERR_UNKNOWN_TABLE;
            }
            size_t n = newItem->Get(sizes_, p);
            p += n;
            tables_[i].push_back(newItem);
        }
    }
    delete[] tableMem;
    return 0;
}
}  // namespace DotNetPELib