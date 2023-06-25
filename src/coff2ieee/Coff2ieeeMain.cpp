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

#include "Coff2ieeeMain.h"
#include "CoffFile.h"
#include "CoffLibrary.h"
#include "CmdSwitch.h"
#include "CmdFiles.h"
#include "Utils.h"
#include "ObjFactory.h"
#include "ObjFile.h"
#include "ObjIeee.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <fstream>

CmdSwitchParser Coff2ieeeMain::SwitchParser;
CmdSwitchBool Coff2ieeeMain::ShowHelp(SwitchParser, '?', false, {"help"});
CmdSwitchCombineString Coff2ieeeMain::outputFileSwitch(SwitchParser, 'o');

const char* Coff2ieeeMain::helpText =
    "[options] <coff file>\n"
    "\n"
    "/oxxx          Set output file name\n"
    "/V, --version  Show version and date\n"
    "/!, --nologo   No logo\n"
    "/?, --help     This text\n"
    "\n"
    "\nTime: " __TIME__ "  Date: " __DATE__;

const char* Coff2ieeeMain::usageText = " [options] <coff file>";

    int main(int argc, char** argv)
{
    Coff2ieeeMain translator;
    return translator.Run(argc, argv);
}
Coff2ieeeMain::~Coff2ieeeMain() {}
std::string Coff2ieeeMain::GetOutputName(char* infile) const
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
    if (mode == lib)
        name = Utils::QualifiedFile(name.c_str(), ".l");
    else
        name = Utils::QualifiedFile(name.c_str(), ".o");
    return name;
}
bool Coff2ieeeMain::GetMode(char* infile)
{
    std::fstream fil(infile, std::ios::in | std::ios::binary);
    char buf[256];
    fil.read(buf, 32);
    if (*(unsigned short*)buf == IMAGE_FILE_MACHINE_I386)
    {
        mode = obj;
        return true;
    }
    if (!strncmp(buf, "!<arch>\n", 8))
    {
        mode = lib;
        return true;
    }
    return false;
}
int Coff2ieeeMain::Run(int argc, char** argv)
{
    Utils::banner(argv[0]);
    char* modName = Utils::GetModuleName();
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

    if (argc != 2)
    {
        Utils::usage(argv[0], usageText);
    }
    if (!GetMode(argv[1]))
    {
        std::cout << "input file must be a COFF object or library file" << std::endl;
        exit(1);
    }
    if (mode == obj)
    {
        std::cout << "converting object file" << std::endl;
        CoffFile object(argv[1]);
        outputName = GetOutputName(argv[1]);
        if (object.Load())
        {
            ObjIeeeIndexManager im1;
            ObjFactory factory(&im1);
            ObjFile* file = object.ConvertToObject(outputName, factory);
            if (file)
            {
                ObjIeee il(outputName);
                il.SetTranslatorName("Coff2ieee");
                il.SetDebugInfoFlag(false);
                FILE* c = fopen(outputName.c_str(), "w");
                if (!c)
                    Utils::fatal("Cannot open '%s' for write", outputName.c_str());
                il.Write(c, file, &factory);
                fclose(c);
                return 0;
            }
        }
    }
    else
    {
        std::cout << "converting library file" << std::endl;
        CoffLibrary library(argv[1]);
        outputName = GetOutputName(argv[1]);
        if (library.Load())
        {
            if (library.Convert())
            {
                if (library.Write(outputName))
                    return 0;
            }
        }
    }
    return 1;
}
