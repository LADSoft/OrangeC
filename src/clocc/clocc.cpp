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
#include <algorithm>
#include "Utils.h"
#include "ToolChain.h"
#include "CmdSwitch.h"
#include "clocc.h"
#include "../exefmt/PEHeader.h"

#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#else
#    include <io.h>
#endif

#define DLL_STUB_SUBSYS 10
#define DEFINE_SPLIT_CHAR 0x1b

CmdSwitchParser clocc::SwitchParser;
CmdSwitchBool clocc::prm_compileonly(SwitchParser, 'c');
CmdSwitchBool clocc::prm_verbose(SwitchParser, 'v');
CmdSwitchCombineString clocc::prm_define(SwitchParser, 'D', DEFINE_SPLIT_CHAR);
CmdSwitchCombineString clocc::prm_cinclude(SwitchParser, 'I', ';');
CmdSwitchCombineString clocc::prm_optimize(SwitchParser, 'O', ';');
CmdSwitchCombineString clocc::prm_undefine(SwitchParser, 'U', ';');
CmdSwitchBool clocc::prmDll(SwitchParser, 0, false, {"LD"});
CmdSwitchString clocc::prmOutputFile(SwitchParser, 'F');
CmdSwitchBool clocc::prmPreprocessToStdout(SwitchParser, 'E');
CmdSwitchBool clocc::prmPreprocessToFile(SwitchParser, 'P');
CmdSwitchString clocc::prmStandard(SwitchParser, 0, 0, {"std"});
CmdSwitchCombineString clocc::prmExtensions(SwitchParser, 'Z');
CmdSwitchBool clocc::prmCharTypeIsUnsigned(SwitchParser, 'J');
CmdSwitchBool clocc::prmShowIncludes(SwitchParser, 0, false, {"showIncludes"});
CmdSwitchString clocc::prmCompileAs(SwitchParser, 'T', ';');
CmdSwitchCombineString clocc::prmLinkOptions(SwitchParser, 0, ';', {"link"});
CmdSwitchString clocc::prmLinkWithMSVCRT(SwitchParser, 'M', false);
CmdSwitchCombineString clocc::prmWarningSetup(SwitchParser, 'W', ';', {"w"});
CmdSwitchBool clocc::RuntimeObjectOverflow(SwitchParser, 0, 0, {"RTCs"});
CmdSwitchBool clocc::RuntimeUninitializedVariable(SwitchParser, 0, 0, {"RTCu"});

const char* clocc::helpText =
R"help([options] files...
    
This program is a wrapper that converts ms/cl style
C++ compiler command line options to occ compiler options
    
/c                   compile only
/Dxxx                define a macro
/E                   reserved for compatibility
/F#                  set stack size
/Fe{file}            link to exe and set executable file name
/Fm{file}            reserved for compatibility
/Fo{file}            compile only and set object file name
/Fi{file}            preprocess only and set preprocessor output name
/Fa{file}            generate assembly file only and set preprocessor output name
/Fe:{file}           link to exe and set executable file name
/Fm:{file}           reserved for compatibility
/Fo:{file}           compile only and set object file name
/Fi:{file}           preprocess only and set preprocessor output name
/Fa:{file}           generate assembly file only and set preprocessor output name
/Ixxx                add to include path
/J                   char is unsigned
/link option         set a linker option or library
/LD                  create dll
/MD                  link with msvcrt.lib
/MPx                 reserved for compatibility
/Ox                  set optimizer option
/P                   preprocess to file
/RTCs                runtime check for object overflow on stack
/RTCu                runtime check for uninitialized variables
/std:xxx             set C or C++ standard version
/Tcfile              reserved for compatibility
/Tpfile              reserved for compatibility
/TC                  compile all files as C
/TP                  compile all files as CXX
/Uxxx                undefine a macro
/wxxx                warning control
/Wxxx                warning control
/Za                  disable extensions
/Ze                  enable extensions
/Zi                  compile for debug
-V, --version        show version information
--nologo             no logo
/?, --help           this text

)help"
"Time: " __TIME__ "  Date: " __DATE__;
const char* clocc::usageText = "[options] files...";

int main(int argc, char** argv)
MAINTRY
{
    clocc cl;
    return cl.Run(argc, argv);
}
MAINCATCH

int clocc::LinkOptions(std::string& args)
{
    int subsys = prmDll.GetValue() ? DLL_STUB_SUBSYS : PE_SUBSYS_CONSOLE;
    if (prmLinkOptions.GetExists())
    {
        bool err = false;
        auto a = Utils::split(prmLinkOptions.GetValue());
        if (!a.size())
            Utils::Fatal("invalid link options");
        bool debug = false;
        for (auto&& o : a)
        {
            std::string select = o;
            std::string val;
            int n = o.find_first_of(':');
            if (n != std::string::npos)
            {
                select = o.substr(0, n);
                val = o.substr(n + 1);
            }
            std::transform(select.begin(), select.end(), select.begin(), ::toupper);
            if (select == "/DEBUG")
            {
                debug = val == "FULL";
            }
            else if (select == "/LIBPATH" && val.size())
            {
                args += " -L" + val;
            }
            else if (select == "/BASE" && val.size())
            {
                int m = val.find(",");
                if (m != std::string::npos)
                    Utils::Fatal("Cannot set image size");
                args += " -pl-DIMAGEBASE=" + val;
            }
            else if (select == "/FILEALIGN" && val.size())
            {
                args += " -pl-DFILEALIGN=" + val;
            }
            else if (select == "/IMPLIB" && val.size())
            {
                args += " --out-implib \"" + val + "\"";
            }
            else if (select == "/MAP" && !val.size())
            {
                args += " -pl-mx";
            }
            else if (select == "/STACK")
            {
                auto a = Utils::split(val, ',');
                if (a.size() != 1 && a.size() != 2)
                    Utils::Fatal("Invalid stack linker option");
                args += " -pl-DSTACKSIZE=" + a[0];
                if (a.size() == 2)
                    args += " -pl-DSTACKCOMMIT=" + a[1];
            }
            else if (select == "/SUBSYSTEM" && val.size())
            {
                int type = 0;
                if (val == "CONSOLE")
                {
                    type = PE_SUBSYS_CONSOLE;
                }
                else if (val == "NATIVE")
                {
                    type = PE_SUBSYS_NATIVE;
                }
                else if (val == "WINDOWS")
                {
                    type = PE_SUBSYS_WINDOWS;
                }
                else
                {
                    Utils::Fatal("unsupported subsystem linker option");
                }
                char buf[256];
                sprintf(buf, "%d", type);
                args += std::string(" -pl-DSUBSYSTEM=") + buf;
                if (subsys != DLL_STUB_SUBSYS && type != PE_SUBSYS_NATIVE)
                    subsys = type;
            }
            else if (select == "/ENTRY" || select == "/EXPORT" || select == "/MACHINE" || select == "/MANIFESTFILE" ||
                     select == "/NODEFAULTLIB" || select == "/OPT" || select == "/PROFILE" || select == "/RELEASE" ||
                     select == "/SAFESEH" || select == "/STUB" || select == "/WX" || select == "/DEF")
            {
                // ignored;
            }
            else
            {
                Utils::Fatal("unsupported linker option");
            }
        }
        if (debug)
            args += " -pl-g";
    }
    return subsys;
}
std::string clocc::SanitizeExtension(std::string fileName, std::string ext)
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
    n = fileName.rfind(".asm");
    if (n != std::string::npos && n == fileName.size() - 4)
    {
        return fileName.substr(0, fileName.size() - 4) + ".s";
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
int clocc::Run(int argc, char** argv)
{
    auto files = ToolChain::StandardToolStartup(SwitchParser, argc, argv, usageText, helpText);
    if (files.size() < 2)
        ToolChain::Usage(usageText);
    char compileType = 0;
    std::string outputFile;
    std::string args;
    if (prm_compileonly.GetValue())
    {
        compileType = 'c';
        outputFile = Utils::QualifiedFile(files[1].c_str(), ".o");
    }
    if (prmPreprocessToFile.GetValue())
    {
        compileType = 'i';
        outputFile = Utils::QualifiedFile(files[1].c_str(), ".i");
    }
    if (!compileType)
    {
        outputFile = Utils::QualifiedFile(files[1].c_str(), ".exe");
    }
    if (prmOutputFile.GetExists())
    {
        std::string val = prmOutputFile.GetValue();
        if (val.size() < 1)
        {
            Utils::Fatal("Missing output file type");
        }
        bool err = false;
        bool map = false;
        switch (val[0])
        {
            case 'a':
                if (compileType)
                    err = true;
                compileType = 'S';
                break;
            case 'm':
                map = true;
                break;
            case 'e':
                if (compileType)
                    err = true;
                compileType = '\0';
                break;
            case 'i':
                if (compileType && compileType != 'i')
                    err = true;
                compileType = 'i';
                break;
            case 'o':
                if (compileType && compileType != 'c')
                    err = true;
                compileType = 'c';
                break;
            default:
                if (isdigit(val[0]))
                {
                    compileType = 0;
                    args += " -pl-DSTACKSIZE=" + val;
                }
                else
                {
                    Utils::Fatal("Unsupported output file type");
                }
                break;
        }
        if (err)
            Utils::Fatal("Conflicting output types requested");
        if (val.size() > 1)
        {
            val = val.substr(1);
            if (val[0] == ':')
            {
                if (val.size() < 1)
                    Utils::Fatal("Expected output file name");
                val = val.substr(1);
            }
            outputFile = val;
        }
        else
        {
            val = files[1].c_str();
            int n = val.find_last_of('.');
            if (n != std::string::npos)
            {
                auto temp = val.substr(n);
                if (temp == ".c" || temp == ".cpp" || temp == ".asm" || temp == ".s")
                    val = val.substr(0, n);
            }
            switch (compileType)
            {
                case '\0':
                    outputFile = Utils::QualifiedFile(val.c_str(), ".exe");
                    break;
                case 'c':
                    outputFile = SanitizeExtension(val, ".o");
                    break;
                case 'i':
                    outputFile = Utils::QualifiedFile(val.c_str(), ".i");
                    break;
                case 'S':
                    outputFile = SanitizeExtension(val, ".s");
                    break;
            }
        }
        if (map)
            args += " -pl-mx";
    }
    if (compileType)
        args += std::string(" -") + compileType;
    args += " -o \"" + outputFile + "\"";

    char optimizeType = '-';
    char optimizeType2 = 0;
    if (prm_optimize.GetExists())
    {
        bool err = false;
        auto a = Utils::split(prm_optimize.GetValue(), ';');
        if (!a.size())
            err = true;
        for (auto&& o : a)
        {
            if (o.size() == 0)
                err = true;
            else
                switch (o[0])
                {
                    case 'd':
                        optimizeType = '-';
                        err = o.size() != 1;
                        break;
                    case '1':
                    case 's':
                        optimizeType = '1';
                        err = o.size() != 1;
                        break;
                    case '2':
                    case 't':
                    case 'x':
                        optimizeType = '2';
                        err = o.size() != 1;
                        break;

                    case 'y':
                        if (o.size() == 2)
                        {
                            if (o[1] != '-')
                                err = true;
                            optimizeType2 = 'B';
                        }
                        else
                        {
                            optimizeType2 = 'S';
                            err = o.size() != 1;
                        }
                        break;
                    default:
                        err = true;
                        break;
                }
        }
        if (err)
            Utils::Fatal("Invalid optimizer parameter");
    }
    args += std::string(" -O") + optimizeType;
    if (optimizeType2 == 'S')
    {
        args += " -C+E";
    }
    else if (optimizeType2 == 'B')
    {
        args += " -C-E";
    }
    if (prm_undefine.GetExists())
    {
        auto a = Utils::split(prm_undefine.GetValue(), ';');
        for (auto&& u : a)
        {
            args += " /U" + u;
        }
    }
    std::string defines;
    if (prm_define.GetExists())
    {
        auto splt = Utils::split(prm_define.GetValue(), DEFINE_SPLIT_CHAR);
        for (int i = 0; i < splt.size(); i++)
        {
            for (int j = 0; j < splt[i].size(); j++)
            {
                if (splt[i][j] == '"' || splt[i][j] == '\\' || splt[i][j] == ' ')
                {
                    splt[i].insert(j, 1, '\\');
                    j++;
                }
            }
            defines += " -D" + splt[i];
        }
    }
    if (prm_cinclude.GetExists())
        args += " /I" + prm_cinclude.GetValue();
    if (prmStandard.GetExists())
    {
        auto a = prmStandard.GetValue();
        if (a == "c++11")
        {
        }
        else if (a == "c++14")
        {
        }
        else if (a == "c++17")
        {
        }
        else if (a == "c89")
        {
        }
        else if (a == "c99")
        {
        }
        else if (a == "c11")
        {
        }
        else
        {
            Utils::Fatal("Unknown language standard");
        }
        args += " -std:" + a;
    }
    if (prmExtensions.GetExists())
    {
        auto a = Utils::split(prmExtensions.GetValue(), ';');
        bool err = false;
        bool noExtensions = false;
        bool debug = false;
        if (a.size() == 0)
        {
            err = true;
        }
        else
        {
            for (auto&& e : a)
            {
                bool err = false;
                if (e.size() != 1)
                    err = true;
                else
                {
                    switch (e[0])
                    {
                        case 'a':
                            noExtensions = true;
                            break;
                        case 'e':
                            noExtensions = false;
                            break;
                        case 'i':
                            debug = true;
                            break;
                        default:
                            err = true;
                            break;
                    }
                }
            }
        }
        if (err)
            Utils::Fatal("Invalid extension parameter");
        if (noExtensions)
        {
            args += " -A";
        }
        if (debug)
            args += " -g";
    }
    if (prmCharTypeIsUnsigned.GetValue())
        args += " -C+u";
    int subsys = LinkOptions(args);
    args += " -W";
    switch (subsys)
    {
        case PE_SUBSYS_NATIVE:   // native
        case PE_SUBSYS_CONSOLE:  // console
            args += "c";
            break;
        case PE_SUBSYS_WINDOWS:  // gui
            args += "g";
            break;
        case DLL_STUB_SUBSYS:  // dll
            args += "d";
            break;
        default:
            Utils::Fatal("internal error");
            break;
    }
    if (RuntimeObjectOverflow.GetValue())
        args += " -fruntime-object-overflow";
    if (RuntimeUninitializedVariable.GetValue())
        args += " -fruntime-uninitialized-variable";
    if (prmLinkWithMSVCRT.GetExists())
    {
        auto val = prmLinkWithMSVCRT.GetValue();
        if (val.size() == 1 && val[0] == 'D')
            args += 'm';  // appends to the -W switch...
        else if (val.size() < 2 || val[0] != 'P')
            Utils::Fatal(std::string("Invalid switch /M") + val);
    }
    char compileAllAs = 0;
    if (prmCompileAs.GetExists())
    {
        bool err = false;
        auto a = Utils::split(prmCompileAs.GetValue());
        if (a.size() == 0)
        {
            err = true;
        }
        for (auto&& c : a)
        {
            if (c.size() == 0)
                err = true;
            else
            {
                switch (c[0])
                {
                    case 'c':
                    case 'p':
                        if (c.size() == 1)
                            err = true;
                        // ignore if it actually has a file name
                        break;
                    case 'C':
                        if (compileAllAs)
                            Utils::Fatal("Conflicting compile as types");
                        compileAllAs = 'C';
                        break;
                    case 'P':
                        if (compileAllAs)
                            Utils::Fatal("Conflicting compile as types");
                        compileAllAs = 'P';
                        break;
                    default:
                        err = true;
                        break;
                }
            }
        }
        if (err)
            Utils::Fatal("Invalid compile as specification");
        if (compileAllAs)
        {
            if (compileAllAs == 'C')
                args += " -x c";
            else
                args += " -x c++";
        }
    }
    if (prmWarningSetup.GetExists())
    {
        bool err = false;
        auto a = Utils::split(prmWarningSetup.GetValue(), ';');
        if (a.size() == 0)
            err = true;
        else
            for (auto&& w : a)
            {
                if (w.size() == 0)
                {
                    args += " -w";
                }
                else
                {
                    if (w == "all")
                    {
                        args += " -w+";
                    }
                    else if (w == "X")
                    {
                        args += " -wx";
                    }
                    else if (w[0] == 'd')
                    {
                        args += " -w" + w;
                    }
                    else if (w[0] == 'e')
                    {
                        args += " -w" + w;
                    }
                    else if (w[0] == 'o' || w[0] == 'l' || w[0] == 'L' || isdigit(w[0]))
                    {
                        // ignored;
                    }
                }
            }
    }
    for (int i = 1; i < files.size(); i++)
        args += std::string(" \"") + files[i] + "\"";
    std::string tempName;
    FILE* fil = Utils::TempName(tempName);
    fputs(args.c_str(), fil);
    fclose(fil);
    int rv = ToolChain::ToolInvoke("occ.exe", nullptr, " -! %s @%s", defines.c_str(), tempName.c_str());
    unlink(tempName.c_str());
    return rv;
}
