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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "Utils.h"
#include "CmdSwitch.h"
#include "arocc.h"

CmdSwitchParser arocc::SwitchParser;
CmdSwitchBool arocc::ShowHelp(SwitchParser, '?', false, {"help"});
CmdSwitchBool arocc::Replace(arocc::SwitchParser, 'r');
CmdSwitchBool arocc::Create(arocc::SwitchParser, 'c');
CmdSwitchBool arocc::WriteIndex(arocc::SwitchParser, 's');
CmdSwitchBool arocc::Newer(arocc::SwitchParser, 'u');
CmdSwitchBool arocc::Verbose(arocc::SwitchParser, 'v');
CmdSwitchBool arocc::Extract(arocc::SwitchParser, 'x');
CmdSwitchBool arocc::Delete(arocc::SwitchParser, 'd');


const char* arocc::helpText =
    "[options] library files...\n"
    "\n"
    "-c             reserved for compatibility\n"
    "-d             delete files from library\n"
    "-r             replace files in library\n"
    "-s             reserved for compatibility\n"
    "-u             reserved for compatibility\n"
    "-v             reserved for compatibility\n"
    "-x             extract files from library\n"
    "-V, --version  show version information\n"
    "--nologo       no logo\n"
    "/?, --help     This text\n"
    "\n"
    "\nTime: " __TIME__ "  Date: " __DATE__;
const char* arocc::usageText = "[options] library files...";

int main(int argc, char** argv)
{
    arocc ar;
    return ar.Run(argc, argv);
}

int arocc::Run(int argc, char** argv) 
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
    if (!SwitchParser.Parse(&argc, argv) || (argc < 2 && !ShowHelp.GetExists()))
    {
        Utils::usage(argv[0], usageText);
    }	
    if (ShowHelp.GetExists())
        Utils::usage(argv[0], helpText);
    if (Replace.GetValue())
    {
        if (Delete.GetValue() || Extract.GetValue())
           Utils::fatal("can only specify one of -r, -d, -x");
    }
    else if (Delete.GetValue() && Extract.GetValue())
       Utils::fatal("can only specify one of -r, -d, -x");
    else if (!Replace.GetValue() && !Delete.GetValue() && !Extract.GetValue())
       Utils::fatal("must specify one of -r, -d, -x");

    const char *select = "";
    if (Replace.GetValue())
        select = "+-";
    if (Extract.GetValue())
        select = "*";
    if (Delete.GetValue())
        select = "-";
    std::string tempName;
    FILE* fil = Utils::TempName(tempName);
    fputs(select, fil);
    for (int i=2; i < argc; i++)
       fprintf(fil, " %s", argv[i]);
    fclose(fil);
    auto rv = Utils::ToolInvoke("olib.exe", nullptr, " -! -c %s @%s", argv[1], tempName.c_str());
    unlink(tempName.c_str());
    return rv;
}