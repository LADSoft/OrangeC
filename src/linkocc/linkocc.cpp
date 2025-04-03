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
#include "linkocc.h"

#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#else
#    include <io.h>
#endif

CmdSwitchParser linkocc::SwitchParser;
CmdSwitchBool linkocc::prm_verbose(SwitchParser, 'v');
CmdSwitchString linkocc::prm_output(SwitchParser, 0, ';', {"out"});
CmdSwitchString linkocc::prm_debug(SwitchParser, 0, ';', {"debug"});
CmdSwitchCombineString linkocc::prm_libpath(SwitchParser, 0, ';', {"libpath"});
CmdSwitchString linkocc::prm_base(SwitchParser, 0, ';', {"base"});
CmdSwitchString linkocc::prm_def(SwitchParser, 0, ';', {"def"});
CmdSwitchBool linkocc::prm_dll(SwitchParser, 0, false, {"dll"});
CmdSwitchString linkocc::prm_entry(SwitchParser, 0, ';', {"entry"});
CmdSwitchString linkocc::prm_export(SwitchParser, 0, ';', {"export"});
CmdSwitchString linkocc::prm_filealign(SwitchParser, 0, ';', {"filealign"});
CmdSwitchString linkocc::prm_implib(SwitchParser, 0, ';', {"implib"});
CmdSwitchString linkocc::prm_machine(SwitchParser, 0, ';', {"machine"});
CmdSwitchString linkocc::prm_manifestfile(SwitchParser, 0, ';', {"manifestfile"});
CmdSwitchString linkocc::prm_map(SwitchParser, 0, ';', {"map"});
CmdSwitchString linkocc::prm_nodefaultlib(SwitchParser, 0, ';', {"nodefaultlib"});
CmdSwitchString linkocc::prm_opt(SwitchParser, 0, ';', {"opt"});
CmdSwitchBool linkocc::prm_profile(SwitchParser, 0, ';', {"profile"});
CmdSwitchBool linkocc::prm_release(SwitchParser, 0, ';', {"release"});
CmdSwitchString linkocc::prm_safeseh(SwitchParser, 0, ';', {"safeseh"});
CmdSwitchString linkocc::prm_stack(SwitchParser, 0, ';', {"stack"});
CmdSwitchString linkocc::prm_stub(SwitchParser, 0, ';', {"stub"});
CmdSwitchString linkocc::prm_subsystem(SwitchParser, 0, ';', {"subsystem"});
CmdSwitchString linkocc::prm_version(SwitchParser, 0, ';', {"version"});
CmdSwitchString linkocc::prm_wx(SwitchParser, 0, ';', {"wx"});

const char* linkocc::helpText =
    "[options] files...\n"
    "\n"
    "/BASE:{address[,size]}        suggest image base address\n"
    "/DEBUG:{FULL, NONE}           enable debugging\n"
    "/DEF:xxx                      reserved for compatibility\n"
    "/DLL                          build a dll\n"
    "/ENTRY:xxx                    reserved for compatibility\n"
    "/EXPORT:xxx                   reserved for compatibility\n"
    "/FILEALIGN:#                  set file alignment\n"
    "/IMPLIB:name                  create an import library\n"
    "/LIBPATH:xxx                  add to library path\n"
    "/MACHINE:xxx                  reserved for compatibility\n"
    "/MANIFESTFILE:xxx             reserved for compatibility\n"
    "/MAP                          create a map file\n"
    "/NODEFAULTLIB:xxx             removes default libraries\n"
    "/OPT:xxx                      reserved for compatibility\n"
    "/OUT:xxx                      set output file\n"
    "/PROFILE                      reserved for compatibility\n"
    "/RELEASE                      reserved for compatibility\n"
    "/SAFESEH:xxx                  reserved for compatibility\n"
    "/VERBOSE:xxx                  reserved for compatibility\n"
    "/STACK:{reserve [,commit]}    set stack size\n"
    "/STUB:xxx                     reserved for compatibility\n"
    "/SUBSYSTEM:{CONSOLE, WINDOWS} set the subsystem\n"
    "/WX:xxx                       reserved for compatibility\n"
    "\n"
    "-V, --version                 show version information\n"
    "--nologo                      no logo\n"
    "/?, --help                    this text\n"
    "\n"
    "\nTime: " __TIME__ "  Date: " __DATE__;
const char* linkocc::usageText = "[options] files...";

int main(int argc, char** argv)
{
    linkocc link;
    return link.Run(argc, argv);
}

std::string linkocc::SanitizeExtension(std::string fileName, std::string ext)
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
int linkocc::Run(int argc, char** argv)
{
    auto files = ToolChain::StandardToolStartup(SwitchParser, argc, argv, usageText, helpText);
    if (files.size() < 2)
        ToolChain::Usage(usageText);
    std::string args;
    if (prm_output.GetExists())
    {
        args += " -o" + Utils::QualifiedFile(prm_output.GetValue().c_str(), ".exe");
    }
    else
    {
        args += " -o" + Utils::QualifiedFile(files[1].c_str(), ".exe");
    }
    if (prm_debug.GetExists())
    {
        if (prm_debug.GetValue() == "FULL")
            args += " -g";
        else if (prm_debug.GetValue() != "NONE")
            Utils::Fatal("invalid debug option");
    }
    if (prm_libpath.GetExists())
    {
        args += " -L ";
        args += prm_libpath.GetValue();
    }
    if (prm_base.GetExists())
    {
        auto a = Utils::split(prm_base.GetValue(), ',');
        if (a.size() > 1)
            Utils::Fatal("Specifying image size not supported");
        args += " -DIMAGEBASE=" + prm_base.GetValue();
    }
    if (prm_def.GetExists())
    {
        args += " " + prm_def.GetValue();
    }
    if (prm_dll.GetExists())
    {
        args += " -TDLL32";
    }
    else if (prm_subsystem.GetExists())
    {
        if (prm_subsystem.GetValue() == "CONSOLE")
            args += " -TCON32";
        else if (prm_subsystem.GetValue() == "WINDOWS")
            args += " -TGUI32";
    }
    else
    {
        args += " -T:CON32";
    }
    if (prm_subsystem.GetExists())
    {
        if (prm_subsystem.GetValue() == "NATIVE")
            args += " -DSUBSYSTEM=1";
        if (prm_subsystem.GetValue() == "CONSOLE")
            args += " -DSUBSYSTEM=3";
        else if (prm_subsystem.GetValue() == "WINDOWS")
            args += " -DSUBSYSTEM=2";
        else
            Utils::Fatal("Unknown subsystem");
    }
    if (prm_filealign.GetExists())
    {
        args += " -DFILEALIGN=" + prm_filealign.GetValue();
    }
    if (prm_implib.GetExists())
    {
        args += " --output-implib " + prm_implib.GetValue();
    }
    if (prm_map.GetExists())
    {
        args += " -mx";
    }
    if (prm_stack.GetExists())
    {
        auto a = Utils::split(prm_base.GetValue(), ',');
        if (a.size() > 2)
            Utils::Fatal("Invalid stack specifier");
        args += " -DSTACKSIZE=" + a[0];
        if (a.size() == 2)
            args += " -DSTACKCOMMIT=" + a[1];
    }
    std::string c0 = " c0xpe.o";
    if (!prm_nodefaultlib.GetExists())
    {
        if (prm_dll.GetValue())
        {
            c0 = " c0dpe.o";
        }
        else if (prm_subsystem.GetExists() && prm_subsystem.GetValue() == "2")
        {
            c0 = " c0pe.o";
        }
        args += c0;
    }
    for (int i = 1; i < files.size(); i++)
    {
        args += " \"" + SanitizeExtension(files[i], "") + "\"";
    }
    if (!prm_nodefaultlib.GetExists())
    {
        args += " clwin.l climp.l";
    }
    std::string tempName;
    FILE* fil = Utils::TempName(tempName);
    fputs(args.c_str(), fil);
    fclose(fil);
    auto rv = ToolChain::ToolInvoke("olink.exe", nullptr, " -! @%s", tempName.c_str());
    unlink(tempName.c_str());
    return rv;
}
