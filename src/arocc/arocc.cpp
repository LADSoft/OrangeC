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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "Utils.h"
#include "ToolChain.h"
#include "CmdSwitch.h"
#include "arocc.h"

#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#else
#    include <io.h>
#endif

CmdSwitchParser arocc::SwitchParser;
CmdSwitchBool arocc::Replace(arocc::SwitchParser, 'r');
CmdSwitchBool arocc::Create(arocc::SwitchParser, 'c');
CmdSwitchBool arocc::WriteIndex(arocc::SwitchParser, 's');
CmdSwitchBool arocc::Newer(arocc::SwitchParser, 'u');
CmdSwitchBool arocc::Verbose(arocc::SwitchParser, 'v');
CmdSwitchBool arocc::Extract(arocc::SwitchParser, 'x');
CmdSwitchBool arocc::Delete(arocc::SwitchParser, 'd');

const char* arocc::helpText = 
R"help([options] library files...
    
This program is wrapper that convert linux/ar style
Librarian command line options to olib librarian options
    
-c             reserved for compatibility
-d             delete files from library
-r             replace files in library
-s             reserved for compatibility
-u             reserved for compatibility
-v             reserved for compatibility
-x             extract files from library

-V, --version  show version information
--nologo       no logo
/?, --help     This text

)help"
"Time: " __TIME__ "  Date: " __DATE__;
const char* arocc::usageText = "[options] library files...";

int main(int argc, char** argv)
MAINTRY
{
    arocc ar;
    return ar.Run(argc, argv);
}
MAINCATCH

int arocc::Run(int argc, char** argv)
{
    auto files = ToolChain::StandardToolStartup(SwitchParser, argc, argv, usageText, helpText);
    if (files.size() < 2)
        ToolChain::Usage(usageText);
    if (Replace.GetValue())
    {
        if (Delete.GetValue() || Extract.GetValue())
            Utils::Fatal("can only specify one of -r, -d, -x");
    }
    else if (Delete.GetValue() && Extract.GetValue())
        Utils::Fatal("can only specify one of -r, -d, -x");
    else if (!Replace.GetValue() && !Delete.GetValue() && !Extract.GetValue())
        Utils::Fatal("must specify one of -r, -d, -x");

    const char* select = "";
    if (Replace.GetValue())
        select = "+-";
    if (Extract.GetValue())
        select = "*";
    if (Delete.GetValue())
        select = "-";
    std::string tempName;
    FILE* fil = Utils::TempName(tempName);
    fputs(select, fil);
    for (int i = 2; i < files.size(); i++)
        fprintf(fil, " %s", files[i].c_str());
    fclose(fil);
    auto rv = ToolChain::ToolInvoke("olib.exe", nullptr, " -! -c %s @%s", files[1].c_str(), tempName.c_str());
    unlink(tempName.c_str());
    return rv;
}