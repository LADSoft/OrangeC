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

#include "dlMzMain.h"
#include "ToolChain.h"
#include "CmdSwitch.h"
#include "CmdFiles.h"
#include "Utils.h"
#include "ObjIeee.h"
#include "ObjExpression.h"
#include "ObjFactory.h"
#include "ObjFile.h"
#include "MZHeader.h"
#include "OutputFormats.h"

CmdSwitchParser dlMzMain::SwitchParser;
CmdSwitchString dlMzMain::outputFileSwitch(SwitchParser, 'o');
CmdSwitchString dlMzMain::modeSwitch(SwitchParser, 'm');
CmdSwitchString dlMzMain::DebugFile(SwitchParser, 'v');

const char* dlMzMain::helpText =
R"help([options] relfile

This program takes a 16 bit x86 output file
and converts it to the format used for MSDOS.

The primary use within this system is to create
The MSDOS stub that goes on WIN32 programs.
    
/oxxx          Set output file name
/mxxx          Set output file type
/V, --version  Show version and date
/!, --nologo   No logo
/?, --help     This text
    
Available output file types:
    TINY
    REAL (segmented, default)

)help"
"Time: " __TIME__ "  Date: " __DATE__;

const char* dlMzMain::usageText = "[options] relfile";

int main(int argc, char** argv)
MAINTRY
{
    dlMzMain downloader;
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
bool dlMzMain::GetMode()
{
    mode = UNKNOWN;
    const std::string& val = modeSwitch.GetValue();
    if (val.empty())
    {
        mode = REAL;
    }
    else
    {
        if (val == "TINY")
            mode = TINY;
        else if (val == "REAL")
            mode = REAL;
    }
    return mode != UNKNOWN;
}
bool dlMzMain::ReadSections(const std::string& path)
{
    ObjIeeeIndexManager iml;
    ObjFactory factory(&iml);
    ObjIeee ieee("");
    FILE* in = fopen(path.c_str(), "rb");
    if (!in)
        Utils::Fatal("Cannot open input file");
    file = ieee.Read(in, ObjIeee::eAll, &factory);
    fclose(in);
    if (!ieee.GetAbsolute())
    {
        Utils::Fatal("Input file is in relative format");
    }
    if (ieee.GetStartAddress() == nullptr)
    {
        Utils::Fatal("No start address specified");
    }
    if (file != nullptr)
    {
        if (mode == TINY)
            data = std::make_unique<Tiny>();
        else
            data = std::make_unique<Real>();
        return data->ReadSections(file, ieee.GetStartAddress());
    }
    return false;
}
std::string dlMzMain::GetOutputName(const char* infile) const
{
    std::string name;
    if (!outputFileSwitch.GetValue().empty())
    {
        name = outputFileSwitch.GetValue();
        const char* p = strrchr(name.c_str(), '.');
        if (p && p[-1] != '.')
            return name;
    }
    else
    {
        name = infile;
    }
    return Utils::QualifiedFile(name.c_str(), mode == TINY ? ".com" : ".exe");
}
int dlMzMain::Run(int argc, char** argv)
{
    auto files = ToolChain::StandardToolStartup(SwitchParser, argc, argv, usageText, helpText);
    if (files.size() != 2 || !GetMode())
        ToolChain::Usage(usageText);

    if (!ReadSections(files[1]))
        Utils::Fatal("Invalid .rel file failed to read sections");
    std::string outputName = GetOutputName(files[1].c_str());
    std::fstream out(outputName, std::ios::out | std::ios::binary);
    if (!out.fail())
    {
        data->Write(out);
        return !!out.fail();
    }
    else
    {
        Utils::Fatal("Cannot open '%s' for write", outputName.c_str());
    }
    return 1;
}
