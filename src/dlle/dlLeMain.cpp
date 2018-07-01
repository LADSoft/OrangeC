/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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

#include "dlLeMain.h"
#include "CmdSwitch.h"
#include "CmdFiles.h"
#include "Utils.h"
#include "ObjSection.h"
#include "ObjMemory.h"
#include "ObjIeee.h"
#include "ObjExpression.h"
#include "ObjFactory.h"
#include "ObjFile.h"
#include "LEObject.h"
#include "LEObjectPage.h"
#include "LEFixup.h"
#include "ResidentNameTable.h"
#include "MZHeader.h"
#include <string.h>

CmdSwitchParser dlLeMain::SwitchParser;
CmdSwitchString dlLeMain::stubSwitch(SwitchParser, 's');
CmdSwitchString dlLeMain::modeSwitch(SwitchParser, 'm');
CmdSwitchString dlLeMain::outputFileSwitch(SwitchParser, 'o');
CmdSwitchString dlLeMain::DebugFile(SwitchParser, 'v');

unsigned dlLeMain::fileVersion = 0;

const char* dlLeMain::usageText =
    "[options] relfile\n"
    "\n"
    "/mxxx          Set output file type\n"
    "/oxxx          Set output file name\n"
    "/sxxx          Set stub file name\n"
    "/V, --version  Show version and date\n"
    "/!, --nologo   No logo\n"
    "\n"
    "Available output file types:\n"
    "   LE (default)\n"
    "   LX\n"
    "\nTime: " __TIME__ "  Date: " __DATE__;

int main(int argc, char** argv)
{
    dlLeMain downloader;
    return downloader.Run(argc, argv);
}
dlLeMain::~dlLeMain() {}
bool dlLeMain::GetMode()
{
    mode = UNKNOWN;
    const std::string& val = modeSwitch.GetValue();
    if (val.size() == 0)
    {
        mode = eLe;
    }
    else
    {
        if (val == "LE")
            mode = eLe;
        else if (val == "LX")
            mode = eLx;
    }
    return mode != UNKNOWN;
}
void dlLeMain::ReadValues()
{
    for (ObjFile::SymbolIterator it = file->DefinitionBegin(); it != file->DefinitionEnd(); ++it)
    {
        ObjDefinitionSymbol* p = (ObjDefinitionSymbol*)*it;
        if (p->GetName() == "STACKSIZE")
        {
            stackSize = p->GetValue();
        }
    }
}
bool dlLeMain::ReadSections(const std::string& path, const std::string& exeName)
{
    ObjIeeeIndexManager iml;
    factory = new ObjFactory(&iml);
    ObjIeee ieee("");
    FILE* in = fopen(path.c_str(), "rb");
    if (!in)
        Utils::fatal("Cannot open input file");
    file = ieee.Read(in, ObjIeee::eAll, factory);
    fclose(in);
    if (!ieee.GetAbsolute())
    {
        delete file;
        Utils::fatal("Input file is in relative format");
    }
    if (ieee.GetStartAddress() == nullptr)
    {
        delete file;
        Utils::fatal("No start address specified");
    }
    startAddress = ieee.GetStartAddress()->Eval(0);
    if (file != nullptr)
    {
        LEObject::SetFile(file);
        ReadValues();
        for (ObjFile::SectionIterator it = file->SectionBegin(); it != file->SectionEnd(); ++it)
        {
            LEObject* p = new LEObject(*it);
            objects.push_back(p);
            (*it)->ResolveSymbols(factory);
        }
        fixups = new LEFixup(*file, objects, mode == eLx);
        if (mode == eLx)
        {
            objectPages = new LXObjectPage(objects);
        }
        else
        {
            objectPages = new LEObjectPage(objects);
            rnt = new ResidentNameTable(exeName);
        }
        return true;
    }
    return false;
}
std::string dlLeMain::GetOutputName(char* infile) const
{
    std::string name;
    if (outputFileSwitch.GetValue().size() != 0)
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
    name = Utils::QualifiedFile(name.c_str(), ".exe");
    return name;
}
void dlLeMain::InitHeader()
{
    memset(&header, 0, sizeof(header));
    if (mode == eLx)
    {
        header.sig = LX_SIGNATURE;
    }
    else
    {
        header.sig = LE_SIGNATURE;
    }
    header.cpu_level = LX_CPU_386;
    header.os_type = LX_OS_OS2;
    header.module_version = fileVersion;
    header.module_flags = LX_MF_PMWINDOWINGCOMPATIBLE;
    header.page_size = 4096;
    header.auto_ds_object = 2; /* data seg, causeway needs it */
    if (mode == eLx)
    {
        header.page_offset_shift = 12;
    }
    header.eip_object = 1;  // hard coded in SPC file by ordering... so we don't need to be more generic
    header.eip = startAddress - objects[0]->GetAddr();

    header.esp_object = objects.size();
    header.esp = stackSize;  // highest address

    unsigned totalPages = 0;
    unsigned initPages = 0;
    for (auto obj : objects)
    {
        totalPages += ObjectAlign(4096, obj->GetSize()) / 4096;
        initPages += ObjectAlign(4096, obj->GetInitSize()) / 4096;
        if (mode != eLx)
        {
            if (obj->GetInitSize())
                header.page_offset_shift = obj->GetInitSize() % 4096;  // for le this is bytes last page
        }
    }
    header.module_page_count = initPages;
    if (mode == eLx)
    {
        header.instance_preload_count = totalPages;
        header.preload_pages_count = initPages;
    }
    header.object_table_offset = sizeof(LEHeader);
    header.object_count = objects.size();
    header.object_page_table_offset = header.object_table_offset + objects.size() * LEObject::HeaderSize;
    header.loader_section_size = objectPages->GetSize();
    if (mode == eLe)
    {
        header.resident_name_table_offset = header.object_page_table_offset + objectPages->GetSize();
        header.resident_name_table_entries = header.resident_name_table_offset + rnt->GetEntrySize();
        header.loader_section_size += rnt->GetSize();
    }
    header.fixup_page_table_offset = header.object_page_table_offset + header.loader_section_size;
    header.fixup_record_table_offset = header.fixup_page_table_offset + fixups->GetIndexTableSize();
    header.fixup_section_size = fixups->GetIndexTableSize() + fixups->GetFixupSize();
    header.loader_section_size += header.fixup_section_size;

    header.import_module_table_offset = header.object_page_table_offset + header.loader_section_size;
    header.import_proc_table_offset = header.import_module_table_offset;
    header.loader_section_size += objects.size() * 24;
    // this is relative to the beginning of the file, instead of from the beginning of the LE header
    // like the rest of the offsets
    header.data_pages_offset = header.import_proc_table_offset + 3 + stubSize;
}
bool dlLeMain::LoadStub(const std::string& exeName)
{
    std::string val = stubSwitch.GetValue();
    if (val.size() == 0)
        val = "dos32a.exe";
    // look in current directory
    std::fstream* file = new std::fstream(val.c_str(), std::ios::in | std::ios::binary);
    if (file == nullptr || !file->is_open())
    {
        if (file)
        {
            delete file;
            file = nullptr;
        }
        // look in exe directory if not there
        int npos = exeName.find_last_of(CmdFiles::DIR_SEP);
        if (npos != std::string::npos)
        {
            val = exeName.substr(0, npos + 1) + "..\\lib\\" + val;
            file = new std::fstream(val.c_str(), std::ios::in | std::ios::binary);
        }
    }
    if (file == nullptr || !file->is_open())
    {
        if (file)
        {
            delete file;
            file = nullptr;
        }
        return false;
    }
    else
    {
        MZHeader mzHead;
        file->read((char*)&mzHead, sizeof(mzHead));
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
        stubData = new char[stubSize];
        memset(stubData, 0, stubSize);
        int newSize = bodySize + totalHeader;
        if (newSize & 511)
            newSize += 512;
        mzHead.image_length_MOD_512 = newSize % 512;
        mzHead.image_length_DIV_512 = newSize / 512;
        mzHead.offset_to_relocation_table = 0x40;
        mzHead.n_header_paragraphs = totalHeader / 16;
        memcpy(stubData, &mzHead, sizeof(mzHead));
        *(unsigned*)(stubData + 0x3c) = stubSize;
        if (relocSize)
        {
            file->seekg(oldReloc, std::ios::beg);
            file->read(stubData + 0x40, relocSize);
        }
        file->seekg(oldHeader, std::ios::beg);
        file->read(stubData + totalHeader, bodySize);
        if (!file->eof() && file->fail())
        {
            delete file;
            return false;
        }
        delete file;
    }
    return true;
}
void dlLeMain::WriteStub(std::fstream& out) { out.write((char*)stubData, stubSize); }
int dlLeMain::Run(int argc, char** argv)
{
    Utils::banner(argv[0]);
    Utils::SetEnvironmentToPathParent("ORANGEC");
    CmdSwitchFile internalConfig(SwitchParser);
    std::string configName = Utils::QualifiedFile(argv[0], ".cfg");
    std::fstream configTest(configName.c_str(), std::ios::in);
    if (!configTest.fail())
    {
        configTest.close();
        if (!internalConfig.Parse(configName.c_str()))
            Utils::fatal("Corrupt configuration file");
    }
    if (!SwitchParser.Parse(&argc, argv) || argc < 2)
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
    outputName = GetOutputName(argv[1]);
    if (!LoadStub(argv[0]))
        Utils::fatal("Missing or invalid stub file");

    if (!ReadSections(std::string(argv[1]), outputName))
        Utils::fatal("Invalid .rel file");

    unsigned ofs = 0;
    for (auto obj : objects)
    {
        obj->Setup(ofs);
    }
    fixups->Setup();
    objectPages->Setup();
    if (rnt)
        rnt->Setup();
    InitHeader();
    std::fstream out(outputName.c_str(), std::ios::out | std::ios::binary);
    if (!out.fail())
    {
        WriteStub(out);
        out.write((char*)&header, sizeof(header));
        for (auto obj : objects)
        {
            obj->WriteHeader(out);
        }
        objectPages->Write(out);
        if (rnt)
            rnt->Write(out);
        fixups->Write(out);
        unsigned vv = 0;
        out.write((char*)&vv, 3);  // import tables
        for (auto obj : objects)
        {
            obj->Write(out);
        }
        out.close();
        return !!out.fail();
    }
    else
    {
        return 1;
    }
}
