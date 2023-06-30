/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include "dlPeMain.h"
#include "CmdSwitch.h"
#include "CmdFiles.h"
#include "Utils.h"
#include "ObjSection.h"
#include "ObjMemory.h"
#include "ObjIeee.h"
#include "ObjExpression.h"
#include "ObjSymbol.h"
#include "ObjFile.h"
#include "ObjFactory.h"
#include "PEObject.h"
#include "MZHeader.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

CmdSwitchParser dlPeMain::SwitchParser;
CmdSwitchBool dlPeMain::ShowHelp(SwitchParser, '?', false, {"help"});
CmdSwitchString dlPeMain::stubSwitch(SwitchParser, 's');
CmdSwitchString dlPeMain::modeSwitch(SwitchParser, 'm');
CmdSwitchString dlPeMain::outputFileSwitch(SwitchParser, 'o');
CmdSwitchString dlPeMain::DebugFile(SwitchParser, 'v');
CmdSwitchBool dlPeMain::FlatExports(SwitchParser, 'f');
CmdSwitchBool dlPeMain::Verbose(SwitchParser, 'y');
CmdSwitchCombineString dlPeMain::OutputDefFile(SwitchParser, 0, 0, {"output-def"});
CmdSwitchCombineString dlPeMain::OutputImportLibrary(SwitchParser, 0, 0, {"out-implib"});

time_t dlPeMain::timeStamp;

int dlPeMain::osMajor = 4;
int dlPeMain::osMinor = 0;
int dlPeMain::userMajor = 0;
int dlPeMain::userMinor = 0;
int dlPeMain::subsysMajor = 4;
int dlPeMain::subsysMinor = 0;

int dlPeMain::subsysOverride = 0;

int dlPeMain::dllFlags = 0;  // 0x8140;

unsigned char dlPeMain::defaultStubData[] = {
    0x4D, 0x5A, 0x6C, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x11, 0x00, 0xFF, 0xFF, 0x03, 0x00,

    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00,

    0x0E, 0x1F, 0xBA, 0x0E, 0x00, 0xB4, 0x09, 0xCD, 0x21, 0xB8, 0x00, 0x4C, 0xCD, 0x21, 0x54, 0x68,

    0x69, 0x73, 0x20, 0x70, 0x72, 0x6F, 0x67, 0x72, 0x61, 0x6D, 0x20, 0x72, 0x65, 0x71, 0x75, 0x69,

    0x72, 0x65, 0x73, 0x20, 0x57, 0x69, 0x6E, 0x33, 0x32, 0x0D, 0x0A, 0x24, 0x00, 0x00, 0x00, 0x00};
int dlPeMain::defaultStubSize = sizeof(defaultStubData);

const char* dlPeMain::helpText =
    "[options] relfile\n"
    "\n"
    "/f             remove underscore from exports\n"
    "/mxxx          Set output file type\n"
    "/oxxx          Set output file name\n"
    "/sxxx          Set stub file name\n"
    "/y             Verbose\n"
    "/V, --version  Show version and date\n"
    "/!             No logo\n"
    "/?, --help     This text\n"
    "\n"
    "--output-def filename    for DLL, output a .def file instead of a .lib\n"
    "--out-implib filename    for DLL, set the name of the import library\n"
    "\n"
    "Available output file types:\n"
    "   CON - Windows console (default)\n"
    "   GUI - Windows GUI\n"
    "   DLL - Windows DLL\n"
    "\nTime: " __TIME__ "  Date: " __DATE__;

const char* dlPeMain::usageText = "[options] relfile";

int main(int argc, char** argv)
{
    dlPeMain downloader;
    try
    {
        return downloader.Run(argc, argv);
    }
    catch (ObjIeeeBinary::SyntaxError e)
    {
        std::cout << e.what() << std::endl;
    }
}
void dlPeMain::ParseOutResourceFiles(int* argc, char** argv)
{
    for (int i = 0; i < *argc; i++)
    {
        int npos = std::string(argv[i]).find_last_of(".");
        if (npos != std::string::npos)
        {
            if (Utils::iequal(argv[i] + npos, ".res"))
            {
                resources.AddFile(argv[i]);
                memcpy(argv + i, argv + i + 1, sizeof(argv[0]) * *argc - i - 1);
                (*argc)--, i--;
            }
        }
    }
}
bool dlPeMain::ParseOutDefFile(int* argc, char** argv)
{
    int n = 0;
    for (int i = 0; i < *argc; i++)
    {
        int npos = std::string(argv[i]).find_last_of(".");
        if (npos != std::string::npos)
        {
            if (Utils::iequal(argv[i] + npos, ".def"))
            {
                defFile = argv[i];
                memcpy(argv + i, argv + i + 1, sizeof(argv[0]) * *argc - i - 1);
                (*argc)--, i--;
                n++;
            }
        }
    }
    return n <= 1;
}
bool dlPeMain::GetMode()
{
    mode = UNKNOWN;
    const std::string& val = modeSwitch.GetValue();
    if (val.empty())
    {
        mode = CONSOLE;
    }
    else
    {
        if (val == "CON")
            mode = CONSOLE;
        else if (val == "GUI")
            mode = GUI;
        else if (val == "DLL")
            mode = DLL;
    }
    return mode != UNKNOWN;
}
void dlPeMain::ReadValues()
{
    for (auto it = file->DefinitionBegin(); it != file->DefinitionEnd(); it++)
    {
        ObjDefinitionSymbol* p = (ObjDefinitionSymbol*)(*it);
        if (p->GetName() == "FILEALIGN")
        {
            fileAlign = p->GetValue();
        }
        else if (p->GetName() == "IMAGEBASE")
        {
            imageBase = p->GetValue();
        }
        else if (p->GetName() == "IMPORTTHUNKS")
        {
            importThunkVA = p->GetValue();
        }
        else if (p->GetName() == "IMPORTCOUNT")
        {
            importCount = p->GetValue();
        }
        else if (p->GetName() == "OBJECTALIGN")
        {
            objectAlign = p->GetValue();
        }
        else if (p->GetName() == "HEAPCOMMIT")
        {
            heapCommit = p->GetValue();
        }
        else if (p->GetName() == "HEAPSIZE")
        {
            heapSize = p->GetValue();
        }
        else if (p->GetName() == "STACKCOMMIT")
        {
            stackCommit = p->GetValue();
        }
        else if (p->GetName() == "STACKSIZE")
        {
            stackSize = p->GetValue();
        }
        else if (p->GetName() == "SUBSYSTEM")
        {
            subsysOverride = p->GetValue();
        }
        else if (p->GetName() == "OSMAJOR")
        {
            osMajor = p->GetValue();
        }
        else if (p->GetName() == "OSMINOR")
        {
            osMinor = p->GetValue();
        }
        else if (p->GetName() == "SUBSYSMAJOR")
        {
            subsysMajor = p->GetValue();
        }
        else if (p->GetName() == "SUBSYSMINOR")
        {
            subsysMinor = p->GetValue();
        }
    }
}
bool dlPeMain::LoadImports(ObjFile* file)
{
    std::set<std::string> names;
    for (auto it = file->ImportBegin(); it != file->ImportEnd(); ++it)
    {
        names.insert((*it)->GetName());
    }
    // every external better have an import declaration... otherwise it is a malformed file
    for (auto it = file->ExternalBegin(); it != file->ExternalEnd(); ++it)
    {
        if (names.find((*it)->GetName()) == names.end())
            return false;
        // point to its thunk...
        //		(*it)->SetOffset(new ObjExpression(importThunkVA + 6 * (*it)->GetIndex()));
    }
    return true;
}
bool dlPeMain::ReadSections(const std::string& path)
{
    ObjIeeeIndexManager iml;
    factory = std::make_unique<ObjFactory>(&iml);
    ObjIeee ieee("");
    FILE* in = fopen(path.c_str(), "rb");
    if (!in)
        Utils::fatal("Cannot open input file");
    file = ieee.Read(in, ObjIeee::eAll, factory.get());
    fclose(in);
    if (!ieee.GetAbsolute())
    {
        Utils::fatal("Input file is in relative format");
    }
    if (ieee.GetStartAddress() == nullptr)
    {
        Utils::fatal("No start address specified");
    }
    startAddress = ieee.GetStartAddress()->Eval(0);
    if (file != nullptr)
    {
        ReadValues();
        if (LoadImports(file))
        {
            PEObject::SetFile(file);
            for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
            {
                objects.push_back(std::make_unique<PEDataObject>(file, *it));
                (*it)->ResolveSymbols(factory.get());
            }
            if (file->ImportBegin() != file->ImportEnd())
                objects.push_back(std::make_unique<PEImportObject>(objects));
            if (file->ExportBegin() != file->ExportEnd())
            {
                objects.push_back(std::make_unique<PEExportObject>(outputName, FlatExports.GetValue()));
                exportObject = static_cast<PEExportObject*>(objects.back().get());
            }
            objects.push_back(std::make_unique<PEFixupObject>());
            if (!resources.empty())
                objects.push_back(std::make_unique<PEResourceObject>(resources));
            if (!DebugFile.GetValue().empty())
                objects.push_back(std::make_unique<PEDebugObject>(DebugFile.GetValue(), imageBase));
            return true;
        }
        else
        {
            Utils::fatal("Input file internal error in import list");
        }
    }
    else
    {
        std::cout << "Invalid rel file format " << ieee.GetErrorQualifier() << std::endl;
    }
    return false;
}
std::string dlPeMain::GetOutputName(char* infile) const
{
    std::string name;
    if (!outputFileSwitch.GetValue().empty())
    {
        name = outputFileSwitch.GetValue();
        const char* p = strrchr(name.c_str(), '.');
        if (p && p[-1] != '.' && p[1] != '\\')
            return name;
    }
    else
    {
        name = infile;
    }
    if (mode == DLL)
        name = Utils::QualifiedFile(name.c_str(), ".dll");
    else
        name = Utils::QualifiedFile(name.c_str(), ".exe");
    return name;
}
void dlPeMain::InitHeader(unsigned headerSize, ObjInt endVa)
{
    memset(&header, 0, sizeof(header));
    header.signature = PESIG;
    header.magic = PE_MAGICNUM;
    header.cpu_type = PE_INTEL386;
    /* store time/date of creation */
    header.time = TimeStamp();
    header.nt_hdr_size = PE_OPTIONAL_HEADER_SIZE;

    header.flags = PE_FILE_EXECUTABLE | PE_FILE_32BIT | PE_FILE_LOCAL_SYMBOLS_STRIPPED | PE_FILE_LINE_NUMBERS_STRIPPED |
                   PE_FILE_REVERSE_BITS_HIGH | PE_FILE_REVERSE_BITS_LOW;
    if (mode == DLL)
    {
        header.flags |= PE_FILE_LIBRARY;
    }
    header.dll_flags = dllFlags;

    header.image_base = imageBase;
    header.file_align = fileAlign;
    header.object_align = objectAlign;
    header.os_major_version = osMajor;
    header.os_minor_version = osMinor;
    header.user_major_version = userMajor;
    header.user_minor_version = userMinor;
    header.subsys_major_version = subsysMajor;
    header.subsys_minor_version = subsysMinor;
    if (subsysOverride != 0)
        header.subsystem = subsysOverride;
    else
        header.subsystem = mode == GUI ? PE_SUBSYS_WINDOWS : PE_SUBSYS_CONSOLE;
    header.num_rvas = PE_NUM_VAS;
    header.header_size = headerSize;
    header.heap_size = heapSize;
    header.heap_commit = heapCommit;
    if (mode != DLL)
    {
        header.stack_size = stackSize;
        header.stack_commit = stackCommit;
    }
    else
    {
        /* flag that entry point should always be called */
        header.dll_flags = 0;
    }
    header.num_objects = objects.size();
    header.entry_point = startAddress - imageBase;

    header.image_size = endVa;
    for (auto& obj : objects)
    {
        if (obj->GetName() == ".text")
        {
            header.code_base = obj->GetAddr();
            header.code_size = ObjectAlign(objectAlign, obj->GetSize());
        }
        else if (obj->GetName() == ".data")
        {
            header.data_base = obj->GetAddr();
            header.data_size = ObjectAlign(objectAlign, obj->GetSize());
            header.bss_size = 0;
        }
        else if (obj->GetName() == ".idata")
        {
            header.import_rva = obj->GetAddr();
            header.import_size = obj->GetRawSize();
        }
        else if (obj->GetName() == ".edata")
        {
            header.export_rva = obj->GetAddr();
            header.export_size = obj->GetRawSize();
        }
        else if (obj->GetName() == ".reloc")
        {
            header.fixup_rva = obj->GetAddr();
            header.fixup_size = obj->GetRawSize();
        }
        else if (obj->GetName() == ".rsrc")
        {
            header.resource_rva = obj->GetAddr();
            header.resource_size = obj->GetRawSize();
        }
        else if (obj->GetName() == ".debug")
        {
            header.debug_rva = obj->GetAddr();
            header.debug_size = obj->GetRawSize();
        }
    }
}
bool dlPeMain::LoadStub(const std::string& exeName)
{
    std::string val = stubSwitch.GetValue();
    if (val.empty())
        val = "dfstb32.exe";
    // look in current directory
    std::fstream file(val, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        // look in lib directory if not there
        int npos = exeName.find_last_of(CmdFiles::DIR_SEP);
        if (npos != std::string::npos)
        {
            std::string val1 = exeName.substr(0, npos + 1) + "..\\lib\\" + val;
            file.open(val1, std::ios::in | std::ios::binary);
        }
        // look in bin directory if not there
        if (!file.is_open())
        {
            // look in lib directory if not there
            int npos = exeName.find_last_of(CmdFiles::DIR_SEP);
            if (npos != std::string::npos)
            {
                std::string val1 = exeName.substr(0, npos + 1) + "..\\bin\\" + val;
                file.open(val1, std::ios::in | std::ios::binary);
            }
        }
    }
    if (!file.is_open())
    {
        if (stubSwitch.GetValue().empty())
        {
            stubData = std::make_unique<char[]>(defaultStubSize);
            memcpy(stubData.get(), defaultStubData, defaultStubSize);
            stubSize = defaultStubSize;
        }
        else
        {
            return false;
        }
    }
    else
    {
        MZHeader mzHead;
        file.read((char*)&mzHead, sizeof(mzHead));
        int bodySize = mzHead.image_length_MOD_512 + mzHead.image_length_DIV_512 * 512;
        int oldReloc = mzHead.offset_to_relocation_table;
        int oldHeader = mzHead.n_header_paragraphs * 16;
        if (bodySize & 511)
            bodySize -= 512;
        bodySize -= oldHeader;
        int relocSize = mzHead.n_relocation_items * 4;
        int preHeader = 0x40;
        int totalHeader = (preHeader + relocSize + 15) & ~15;
        stubSize = (totalHeader + bodySize + 15) & ~15;
        stubData = std::make_unique<char[]>(stubSize);
        memset(stubData.get(), 0, stubSize);
        int newSize = bodySize + totalHeader;
        if (newSize & 511)
            newSize += 512;
        mzHead.image_length_MOD_512 = newSize % 512;
        mzHead.image_length_DIV_512 = newSize / 512;
        mzHead.offset_to_relocation_table = 0x40;
        mzHead.n_header_paragraphs = totalHeader / 16;
        memcpy(stubData.get(), &mzHead, sizeof(mzHead));
        *(unsigned*)(stubData.get() + 0x3c) = stubSize;
        if (relocSize)
        {
            file.seekg(oldReloc, std::ios::beg);
            file.read(stubData.get() + 0x40, relocSize);
        }
        file.seekg(oldHeader, std::ios::beg);
        file.read(stubData.get() + totalHeader, bodySize);
        if (!file.eof() && file.fail())
        {
            return false;
        }
    }
    return true;
}
void dlPeMain::WriteStub(std::fstream& out)
{
    out.write((char*)stubData.get(), stubSize);
    out.flush();
}
void dlPeMain::PadHeader(std::fstream& out)
{
    int n = out.tellg();
    n = ObjectAlign(fileAlign, n) - n + fileAlign;  // for secondary header
    char buf[512];
    memset(buf, 0, sizeof(buf));
    while (n)
    {
        int s = 512;
        if (n < s)
            s = n;
        out.write(buf, s);
        n -= s;
    }
    out.flush();
}
int dlPeMain::Run(int argc, char** argv)
{
    Utils::banner(argv[0]);
    Utils::SetEnvironmentToPathParent("ORANGEC");
    char* sde = getenv("SOURCE_DATE_EPOCH");
    if (sde)
        timeStamp = (time_t)strtoul(sde, nullptr, 10);
    else
        timeStamp = time(0);
    CmdSwitchFile internalConfig(SwitchParser);
    std::string configName = Utils::QualifiedFile(argv[0], ".cfg");
    std::fstream configTest(configName, std::ios::in);
    if (!configTest.fail())
    {
        configTest.close();
        if (!internalConfig.Parse(configName.c_str()))
            Utils::fatal("Corrupt configuration file");
    }
    if (!SwitchParser.Parse(&argc, argv) || (argc < 2 && !ShowHelp.GetExists()))
    {
        Utils::usage(argv[0], usageText);
    }
    if (ShowHelp.GetExists())
        Utils::usage(argv[0], helpText);
    ParseOutResourceFiles(&argc, argv);
    if (!ParseOutDefFile(&argc, argv))
    {
        Utils::usage(argv[0], usageText);
    }
    if (argc != 2)
    {
        Utils::usage(argv[0], usageText);
    }
    if (!GetMode())
    {
        Utils::usage(argv[0], usageText);
    }
    if (!LoadStub(argv[0]))
        Utils::fatal("Missing or invalid stub file");

    outputName = GetOutputName(argv[1]);
    if (!ReadSections(std::string(argv[1])))
        Utils::fatal("Invalid .rel file failed to read sections");

    ObjInt endPhys = sizeof(PEHeader) + objects.size() * PEObject::HeaderSize + stubSize;
    endPhys = ObjectAlign(fileAlign, endPhys + fileAlign);  // extra space for optional PE header
    ObjInt headerSize = endPhys;
    ObjInt endVa = objects[0]->GetAddr();
    if (endVa < endPhys)
        Utils::fatal("ObjectAlign too small");
    for (auto& obj : objects)
    {
        obj->Setup(endVa, endPhys);
    }
    for (auto& obj : objects)
    {
        obj->Fill();
    }
    InitHeader(headerSize, endVa);
    std::fstream out(outputName, std::ios::out | std::ios::binary);
    if (!out.fail())
    {
        WriteStub(out);
        out.write((char*)&header, sizeof(header));
        for (auto& obj : objects)
        {
            obj->WriteHeader(out);
        }
        PadHeader(out);
        out.flush();
        for (auto& obj : objects)
        {
            obj->Write(out);
            out.flush();
        }
        out.flush();
        out.close();
        if (!out.fail())
        {
            if (mode == DLL)
            {
                std::string sverbose = Verbose.GetExists() ? "" : "/!";
                std::string usesC = exportObject && exportObject->ImportsNeedUnderscore() ? "/C" : "";
                std::string implibName;
                if (OutputImportLibrary.GetValue().empty())
                {
                    implibName = Utils::QualifiedFile(outputName.c_str(), ".l");
                }
                else
                {
                    implibName = Utils::QualifiedFile(OutputImportLibrary.GetValue().c_str(), ".l");
                }
                return Utils::ToolInvoke("oimplib", Verbose.GetExists() ? "" : nullptr, "%s %s \"%s\" \"%s\"", usesC.c_str(),
                                         sverbose.c_str(), implibName.c_str(), outputName.c_str());
            }
            return 0;
        }
        return 1;
    }
    else
    {
        Utils::fatal("Cannot open '%s' for write", outputName.c_str());
    }
    return 1;
}
