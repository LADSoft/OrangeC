/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

#include "rcMain.h"
#include "RCFile.h"
#include "ResFile.h"

#include "Utils.h"
#include "CmdFiles.h"
#include "PreProcessor.h"

#ifndef HAVE_UNISTD_H
#    include <windows.h>
#endif

CmdSwitchParser rcMain::SwitchParser;
CmdSwitchFile rcMain::File(SwitchParser, '@');
CmdSwitchBool rcMain::Boolr(SwitchParser, 'r');
CmdSwitchBool rcMain::Boolt(SwitchParser, 't');
CmdSwitchBool rcMain::Boolv(SwitchParser, 'v');
CmdSwitchOutput rcMain::OutputFile(SwitchParser, 'o', ".res");
CmdSwitchDefine rcMain::Defines(SwitchParser, 'D');
CmdSwitchCombineString rcMain::includePath(SwitchParser, 'i', ';');
CmdSwitchString rcMain::Language(SwitchParser, 'L');

const char* rcMain::usageText =
    "[options] file"
    "\n"
    "  @filename  use response file\n"
    "  /Dxxx  Define something             /ixxx             Set include file path\n"
    "  /Lxx   Set default language id      /oxxx             Set output file name\n"
    "  /r     reserved for compatability   /t                reserved for compatability\n"
    "  /v     reserved for compatability   /V, --version     Show version and date\n"
    "  /!, --nologo   No logo\n"
    "\n"
    "Time: " __TIME__ "  Date: " __DATE__;

int main(int argc, char* argv[])
{
    rcMain rc;
    return rc.Run(argc, argv);
}

int rcMain::Run(int argc, char* argv[])
{
    int rv = 0;
#ifndef HAVE_UNISTD_H
    int language = LANG_ENGLISH + (SUBLANG_ENGLISH_US << 10);
#else
    int language = 0;
#endif
    Utils::banner(argv[0]);
    Utils::SetEnvironmentToPathParent("ORANGEC");
    CmdSwitchFile internalConfig(SwitchParser);
    std::string configName = Utils::QualifiedFile(Utils::GetModuleName(), ".cfg");
    std::fstream configTest(configName, std::ios::in);
    if (!configTest.fail())
    {
        configTest.close();
        if (!internalConfig.Parse(configName.c_str()))
            Utils::fatal("Corrupt configuration file");
    }
    if (!SwitchParser.Parse(&argc, argv) || (argc == 1 && File.GetCount() <= 1))
    {
        Utils::usage(argv[0], usageText);
    }
    CmdFiles files(argv + 1);
    if (File.GetValue())
        files.Add(File.GetValue() + 1);
    if (files.GetSize() > 1 && !OutputFile.GetValue().empty())
        Utils::fatal("Cannot specify output file for multiple input files");
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
    for (auto it = files.FileNameBegin(); it != files.FileNameEnd(); ++it)
    {
        std::string inName = Utils::QualifiedFile((*it).c_str(), ".rc");
        PreProcessor pp(inName, srchPth, sysSrchPth, false, false, '#', false, true, true, false, "");

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
            outName = Utils::QualifiedFile((*it).c_str(), ".res");
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
