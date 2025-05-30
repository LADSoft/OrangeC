/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#include "dlhexmain.h"
#include "CmdSwitch.h"
#include "Utils.h"
#include "ToolChain.h"
#include "OutputObjects.h"
#include "ObjSection.h"
#include "ObjMemory.h"
#include "ObjIeee.h"
#include "ObjExpression.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

CmdSwitchParser dlHexMain::SwitchParser;

CmdSwitchString dlHexMain::modeSwitch(SwitchParser, 'm');
CmdSwitchString dlHexMain::sectionsSwitch(SwitchParser, 'c');
CmdSwitchHex dlHexMain::padSwitch(SwitchParser, 'p', -1, 0, 0x100);
CmdSwitchString dlHexMain::outputFileSwitch(SwitchParser, 'o');
CmdSwitchString dlHexMain::DebugFile(SwitchParser, 'v');

const char* dlHexMain::helpText =
R"help([options] relfile

This program converts a linker output file into
an intel or motorola hex file.  These are often used for
transfering program images or using a chip burner

/cxxx          Specify sections
/mxxx          Set output file type
/oxxx          Set ouput file name
/p:xx          Set pad value
/V, --version  Show version and date
/!, --nologo   No logo
/?, --help     This text

Available output file types:
   M# Motorola Hex (# = 1,2,3 sets srecord type)
   I# Intel Hex    (# = 1,2,4)
   B  Binary file  (default)

)help"
"Time: " __TIME__ "  Date: " __DATE__;
const char* dlHexMain::usageText = "[options] relfile";

int main(int argc, char** argv)
MAINTRY
{
    dlHexMain downloader;
    try
    {
        return downloader.Run(argc, argv);
    }
    catch (std::domain_error e)
    {
        std::cout << e.what() << std::endl;
    }
}
MAINCATCH
void dlHexMain::GetSectionNames(std::vector<std::string>& names, ObjFile* file)
{
    if (sectionsSwitch.GetValue().size())
    {
        const char* p = sectionsSwitch.GetValue().c_str();
        char buf[256], *q = buf;
        while (*p)
        {
            if (*p == ',')
            {
                if (q - buf != 0)
                {
                    *q = 0;
                    q = buf;
                    std::string name = buf;
                    if (file->FindSection(name))
                        names.push_back(std::move(name));
                    else
                        std::cout << "Warning: Section '" << name << "' not in .rel file" << std::endl;
                }
                p++;
            }
            else
                *q++ = *p++;
        }
        if (q - buf != 0)
        {
            *q = 0;
            std::string name = buf;
            if (file->FindSection(name))
                names.push_back(std::move(name));
            else
                std::cout << "Warning: Section '" << name << "' not in .rel file" << std::endl;
        }
    }
    else
    {
        for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
        {
            names.push_back((*it)->GetName());
        }
    }
}
void dlHexMain::GetInputSections(const std::vector<std::string>& names, ObjFile* file, ObjFactory* factory)
{
    for (const auto& name : names)
    {
        ObjSection* s = file->FindSection(name);
        ObjInt size = s->GetSize()->Eval(0);
        ObjInt addr = s->GetOffset()->Eval(0);
        Section p(addr, size);
        p.data = std::make_unique<char[]>(size);
        char* data = p.data.get();
        s->ResolveSymbols(factory);
        ObjMemoryManager& m = s->GetMemoryManager();
        int ofs = 0;
        for (auto memory : m)
        {
            int msize = memory->GetSize();
            ObjByte* mdata = memory->GetData();
            if (msize)
            {
                ObjExpression* fixup = memory->GetFixup();
                if (fixup)
                {
                    int sbase = s->GetOffset()->Eval(0);
                    int n = fixup->Eval(sbase + ofs);
                    int bigEndian = file->GetBigEndian();
                    if (msize == 1)
                    {
                        data[ofs] = n & 0xff;
                    }
                    else if (msize == 2)
                    {
                        if (bigEndian)
                        {
                            data[ofs] = n >> 8;
                            data[ofs + 1] = n & 0xff;
                        }
                        else
                        {
                            data[ofs] = n & 0xff;
                            data[ofs + 1] = n >> 8;
                        }
                    }
                    else  // msize == 4
                    {
                        if (bigEndian)
                        {
                            data[ofs + 0] = n >> 24;
                            data[ofs + 1] = n >> 16;
                            data[ofs + 2] = n >> 8;
                            data[ofs + 3] = n & 0xff;
                        }
                        else
                        {
                            data[ofs] = n & 0xff;
                            data[ofs + 1] = n >> 8;
                            data[ofs + 2] = n >> 16;
                            data[ofs + 3] = n >> 24;
                        }
                    }
                }
                else
                {
                    if (memory->IsEnumerated())
                        memset(data + ofs, memory->GetFill(), msize);
                    else
                        memcpy(data + ofs, mdata, msize);
                }
                ofs += msize;
            }
        }
        sections.push_back(std::make_unique<Section>(std::move(p)));
    }
}
bool dlHexMain::ReadSections(const std::string& path)
{
    ObjIeeeIndexManager iml;
    ObjFactory factory(&iml);
    ObjIeee ieee("");
    FILE* in = fopen(path.c_str(), "rb");
    if (!in)
        Utils::Fatal("Cannot open input file");
    ObjFile* file = ieee.Read(in, ObjIeee::eAll, &factory);
    fclose(in);
    if (!ieee.GetAbsolute())
    {
        Utils::Fatal("Input file is in relative format");
    }
    if (file != nullptr)
    {
        std::vector<std::string> names;
        GetSectionNames(names, file);
        GetInputSections(names, file, &factory);
        return true;
    }
    return false;
}
bool dlHexMain::GetOutputMode()
{
    const std::string& modeStr = modeSwitch.GetValue();
    if (modeStr.empty())
    {
        outputMode = eBinary;
        return true;
    }
    switch (modeStr[0])
    {
        case 'm':
        case 'M':
            outputMode = eMotorola;
            extraMode = atoi(modeStr.c_str() + 1);
            if (extraMode < 1 || extraMode > 3)
                return false;
            break;
        case 'i':
        case 'I':
            outputMode = eIntel;
            extraMode = atoi(modeStr.c_str() + 1);
            if (extraMode != 1 && extraMode != 2 && extraMode != 4)
                return false;
            break;
        case 'b':
        case 'B':
            if (modeStr.size() != 1)
                return false;
            outputMode = eBinary;
            break;
        default:
            return false;
    }
    return true;
}
std::string dlHexMain::GetOutputName(const char* infile) const
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
    switch (outputMode)
    {
        case eBinary:
            name = Utils::QualifiedFile(name.c_str(), ".bin");
            break;
        case eMotorola:
            name = Utils::QualifiedFile(name.c_str(), ".s19");
            break;
        case eIntel:
            name = Utils::QualifiedFile(name.c_str(), ".hex");
            break;
        default:
            name = Utils::QualifiedFile(name.c_str(), "");
            break;
    }
    return name;
}
int dlHexMain::Run(int argc, char** argv)
{
    auto files = ToolChain::StandardToolStartup(SwitchParser, argc, argv, usageText, helpText);
    if (files.size() != 2 || !GetOutputMode())
        ToolChain::Usage(usageText);
    if (!ReadSections(files[1]))
        Utils::Fatal("Invalid .rel file failed to read sections");

    std::string outputName = GetOutputName(files[1].c_str());
    OutputObject* o = nullptr;
    switch (outputMode)
    {
        case eBinary:
            o = new BinaryOutputObject;
            break;
        case eIntel:
            o = new IntelOutputObject(extraMode);
            break;
        case eMotorola:
            o = new MotorolaOutputObject(outputName, extraMode);
            break;
    }
    std::fstream out(outputName, o->GetOpenFlags());
    if (!out.fail())
    {
        int addr = 0;
        o->WriteHeader(out);
        for (int i = 0; i < sections.size(); i++)
        {
            Section* s = sections[i].get();
            if (padSwitch.GetValue() >= 0 && s->address > addr)
                o->Pad(out, addr, s->address - addr, padSwitch.GetValue());
            o->Write(out, s->data.get(), s->size, s->address);
            addr = s->address + s->size;
        }
        o->WriteTrailer(out);
        out.close();
        return !!out.fail();
    }
    else
    {
        Utils::Fatal("Cannot open '%s' for write", outputName.c_str());
    }
    return 1;
}
