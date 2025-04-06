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
#include "libocc.h"

#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#else
#    include <io.h>
#endif

CmdSwitchParser libocc::SwitchParser;
CmdSwitchBool libocc::Verbose(SwitchParser, 0, false, {"verbose"});
CmdSwitchCombineString libocc::Extract(SwitchParser, 0, ';', {"extract"});
CmdSwitchCombineString libocc::Remove(SwitchParser, 0, ';', {"remove"});
CmdSwitchString libocc::LibPath(SwitchParser, 0, ';', {"libpath"});
CmdSwitchString libocc::List(SwitchParser, 0, ';', {"list"});
CmdSwitchString libocc::Machine(SwitchParser, 0, ';', {"machine"});
CmdSwitchString libocc::DllName(SwitchParser, 0, ';', {"name"});
CmdSwitchString libocc::NoDefaultLib(SwitchParser, 0, ';', {"nodefaultlib"});

CmdSwitchString libocc::OutFile(SwitchParser, 0, ';', {"out"});
CmdSwitchString libocc::Subsytem(SwitchParser, 0, ';', {"subsystem"});
CmdSwitchString libocc::WarningsAsErrors(SwitchParser, 0, ';', {"wx"});
;

const char* libocc::helpText =
R"help([options] library files...

This program is a wrapper that converts ms/lib style
librarian command line options into olib options

/EXTRACT:name     add file to extract
/LIBPATH:xxx      reserved for compatibility
/LIST:xxx         reserved for compatibility
/MACHINE:xxx      reserved for compatibility
/NAME:xxx         create import library from dll
/NODEFAULTLIB:xxx  reserved for compatibility
/OUT:xxx          specify output file name
/REMOVE:name      add file to remove
/SUBSYSTEM:xxx    reserved for compatibility
/VERBOSE          reserved for compatibility
/WX               reserved for compatibility
-V, --version     show version information
--nologo          no logo
/?, --help        This text
    
)help"
"Time: " __TIME__ "  Date: " __DATE__;
const char* libocc::usageText = "[options] files...";

int main(int argc, char** argv)
{
    libocc lib;
    return lib.Run(argc, argv);
}

std::string libocc::SanitizeExtension(std::string fileName, std::string ext)
{
    int n = fileName.rfind(".obj");
    if (n != std::string::npos && n == fileName.size() - 4)
    {
        return fileName.substr(0, fileName.size() - 4) + ".o";
    }
    n = fileName.rfind(".lib");
    if (n != std::string::npos && n == fileName.size() - 4)
    {
        return fileName.substr(0, fileName.size() - 4) + ".l";
    }
    else if (ext.size())
    {
        if (fileName.size() <= ext.size() || fileName.substr(fileName.size() - ext.size()) != ext)
            fileName += ext;
        return fileName;
    }
    else
    {
        return fileName;
    }
}
int libocc::Run(int argc, char** argv)
{
    auto files = ToolChain::StandardToolStartup(SwitchParser, argc, argv, usageText, helpText);
    if (files.size() < 2 && !Remove.GetExists() && !Extract.GetExists())
        ToolChain::Usage(usageText);
    std::string outputFile;
    std::string toolName;

    if (DllName.GetExists())
    {
        if (OutFile.GetExists())
        {
            outputFile = libocc::SanitizeExtension(OutFile.GetValue(), ".l");
        }
        else
        {
            auto d = DllName.GetValue();
            if (d.size() > 4 && (d.substr(d.size() - 4) == ".dll" || d.substr(d.size() - 4) == ".DLL"))
                d = d.substr(0, d.size() - 4);
            d += ".l";
            outputFile = d;
        }
        toolName = "oimplib.exe";
    }
    else
    {
        toolName = "olib.exe";
        if (OutFile.GetExists())
        {
            outputFile = libocc::SanitizeExtension(OutFile.GetValue(), ".l");
        }
        else
        {
            if (argc == 1)
                Utils::Fatal("Cannot automatically derive library name when not adding or replacing files");
            outputFile = Utils::QualifiedFile(files[1].c_str(), ".l");
        }
    }
    std::string args;
    if (DllName.GetExists())
    {
        args += " " + DllName.GetValue();
        toolName = "oimplib.exe";
    }

    if (files.size() > 1)
    {
        args += " +- ";
        for (int i = 1; i < files.size(); i++)
        {
            args += "\"";
            args += SanitizeExtension(files[i], "");
            args += "\"";
        }
    }
    if (Remove.GetExists())
    {
        args += " - ";
        auto names = Utils::split(Remove.GetValue(), ';');
        for (auto n : names)
        {
            args += "\"";
            args += SanitizeExtension(n, "");
            args += "\"";
        }
    }
    if (Extract.GetExists())
    {
        args += "* ";
        auto names = Utils::split(Extract.GetValue(), ';');
        for (auto n : names)
        {
            args += "\"";
            args += SanitizeExtension(n, "");
            args += "\"";
        }
    }
    std::string tempName;
    FILE* fil = Utils::TempName(tempName);
    fputs(args.c_str(), fil);
    fclose(fil);
    auto rv = ToolChain::ToolInvoke(toolName, nullptr, " -! -c \"%s\" @%s", outputFile.c_str(), tempName.c_str());
    unlink(tempName.c_str());
    return rv;
}