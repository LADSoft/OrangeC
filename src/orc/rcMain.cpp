/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#include "rcMain.h"
#include "RCFile.h"
#include "ResFile.h"

#include "Utils.h"
#include "ToolChain.h"
#include "CmdFiles.h"
#include "PreProcessor.h"

#ifdef TARGET_OS_WINDOWS
#    include <windows.h>
#endif

CmdSwitchParser rcMain::SwitchParser;
CmdSwitchBool rcMain::Boolr(SwitchParser, 'r');
CmdSwitchBool rcMain::Boolt(SwitchParser, 't');
CmdSwitchBool rcMain::Boolv(SwitchParser, 'v');
CmdSwitchOutput rcMain::OutputFile(SwitchParser, 'o', ".res");
CmdSwitchDefine rcMain::Defines(SwitchParser, 'D');
CmdSwitchCombineString rcMain::includePath(SwitchParser, 'i', ';');
CmdSwitchString rcMain::Language(SwitchParser, 'L');

const char* rcMain::helpText =
    "[options] file"
    "\n"
    "  @filename  use response file\n"
    "  /Dxxx  Define something             /ixxx             Set include file path\n"
    "  /Lxx   Set default language id      /oxxx             Set output file name\n"
    "  /r     reserved for compatability   /t                reserved for compatability\n"
    "  /v     reserved for compatability   /V, --version     Show version and date\n"
    "  /!, --nologo   No logo\n"
    "  /?, --help     This text\n"
    "\n"
    "Time: " __TIME__ "  Date: " __DATE__;
const char* rcMain::usageText = "[options] file";

int main(int argc, char* argv[])
{
    rcMain rc;
    return rc.Run(argc, argv);
}

int rcMain::Run(int argc, char* argv[])
{
    int rv = 0;
    auto files = ToolChain::StandardToolStartup(SwitchParser, argc, argv, usageText, helpText);
    if (files.size() < 2)
        ToolChain::Usage(usageText);
#ifndef HAVE_UNISTD_H
    int language = LANG_ENGLISH + (SUBLANG_ENGLISH_US << 10);
#else
    int language = 0;
#endif
    if (files.size() > 1 && !OutputFile.GetValue().empty())
        Utils::Fatal("Cannot specify output file for multiple input files");
    std::string sysSrchPth;
    std::string srchPth;
    if (!includePath.GetValue().empty())
    {
        size_t n = includePath.GetValue().find_first_of(';');
        if (n == std::string::npos)
        {
            sysSrchPth = includePath.GetValue();
        }
        else
        {
            sysSrchPth = includePath.GetValue().substr(0, n);
            srchPth = includePath.GetValue().substr(n + 1);
        }
    }
    char* cpath = getenv("CPATH");
    if (cpath)
    {
        if (!srchPth.empty())
            srchPth += ";";
        srchPth += cpath;
    }
    if (!Language.GetValue().empty())
    {
        sscanf(Language.GetValue().c_str(), "%d", &language);
    }
    for (int i = 1; i < files.size(); i++)
    {
        std::string inName = Utils::QualifiedFile(files[i].c_str(), ".rc");
        PreProcessor pp(inName, srchPth, sysSrchPth, false, false, '#', false, Dialect::orc, true, false, "");

        // for libcxx 10
        pp.Define("__need_size_t", "1");
        pp.Define("__need_FILE", "1");
        pp.Define("__need_wint_t", "1");
        pp.Define("__need_malloc_and_calloc", "1");

        int n = Defines.GetCount();
        for (int i = 0; i < n; i++)
        {
            CmdSwitchDefine::define* v = Defines.GetValue(i);
            pp.Define(v->name, v->value, false);
        }
        std::string s("");
        pp.Define("RC_INVOKED", s, false);
        std::string outName;
        if (!OutputFile.GetValue().empty())
            outName = OutputFile.GetValue();
        else
            outName = Utils::QualifiedFile(files[i].c_str(), ".res");
        ResFile resFile;
        RCFile rcFile(pp, resFile, srchPth, language);
        if (rcFile.Read())
        {
            if (!resFile.Write(outName))
            {
                Errors::Error(std::string("Error writing output file '") + outName + "'");
                rv = 1;
            }
        }
        else
        {
            rv = 1;
        }
    }
    return rv;
}
