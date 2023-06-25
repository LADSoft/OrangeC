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

#include "dlMzMain.h"
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
CmdSwitchBool dlMzMain::ShowHelp(SwitchParser, '?', false, {"help"});
CmdSwitchString dlMzMain::modeSwitch(SwitchParser, 'm');
CmdSwitchString dlMzMain::DebugFile(SwitchParser, 'v');

const char* dlMzMain::helpText =
    "[options] relfile\n"
    "\n"
    "/oxxx          Set output file name\n"
    "/mxxx          Set output file type\n"
    "/V, --version  Show version and date\n"
    "/!, --nologo   No logo\n"
    "/?, --help     This text\n"
    "\n"
    "Available output file types:\n"
    "    TINY\n"
    "    REAL (segmented, default)\n"
    "\nTime: " __TIME__ "  Date: " __DATE__;

const char* dlMzMain::usageText = "[options] relfile";

int main(int argc, char** argv)
{
    dlMzMain downloader;
    try
    {
        return downloader.Run(argc, argv);
    }
    catch (ObjIeeeBinary::SyntaxError e)
    {
        std::cout << e.what() << std::endl;
    }
}

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
        Utils::fatal("Cannot open input file");
    file = ieee.Read(in, ObjIeee::eAll, &factory);
    fclose(in);
    if (!ieee.GetAbsolute())
    {
        Utils::fatal("Input file is in relative format");
    }
    if (ieee.GetStartAddress() == nullptr)
    {
        Utils::fatal("No start address specified");
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
std::string dlMzMain::GetOutputName(char* infile) const
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
    Utils::banner(argv[0]);
    Utils::SetEnvironmentToPathParent("ORANGEC");
    CmdSwitchFile internalConfig(SwitchParser);
    std::string configName = Utils::QualifiedFile(argv[0], ".cfg");
    std::fstream configTest(configName, std::ios::in);
    if (!configTest.fail())
    {
        configTest.close();
        if (!internalConfig.Parse(configName.c_str()))
            Utils::fatal("Corrupt configuration file");
    }
    if (!SwitchParser.Parse(&argc, argv) || (argc != 2 && !ShowHelp.GetExists()))
    {
        Utils::usage(argv[0], usageText);
    }
    if (ShowHelp.GetExists())
        Utils::usage(argv[0], helpText);
    if (!GetMode())
    {
        Utils::usage(argv[0], usageText);
    }
    if (!ReadSections(std::string(argv[1])))
        Utils::fatal("Invalid .rel file failed to read sections");
    std::string outputName = GetOutputName(argv[1]);
    std::fstream out(outputName, std::ios::out | std::ios::binary);
    if (!out.fail())
    {
        data->Write(out);
        return !!out.fail();
    }
    else
    {
        Utils::fatal("Cannot open '%s' for write", outputName.c_str());
    }
    return 1;
}
