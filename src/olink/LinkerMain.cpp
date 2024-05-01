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
 *     GNU General Pu_HEAPblic License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include "ObjTypes.h"
#include "ObjFactory.h"
#include "ObjIeee.h"
#include "ObjSymbol.h"
#include "ObjExpression.h"
#include "LinkManager.h"
#include "LinkMap.h"
#include "CmdSwitch.h"
#include "SwitchConfig.h"
#include "xml.h"
#include "Utils.h"
#include "ToolChain.h"
#include "LinkerMain.h"
#include "LinkPartition.h"
#include "LinkOverlay.h"
#include "LinkLibrary.h"
#include <fstream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#else
#    include <io.h>
#endif

int main(int argc, char** argv)
{
    LinkerMain linker;
    try
    {
        return linker.Run(argc, argv);
    }
    catch (std::bad_exception e)
    {
    }
    catch (std::ios_base::failure)
    {
    }
    catch (ObjIeeeBinary::SyntaxError e)
    {
        std::cout << e.what() << std::endl;
    }
    Utils::Fatal("Fatal Error...");
    return 1;
}

CmdSwitchParser LinkerMain::SwitchParser;
CmdSwitchBool LinkerMain::CaseSensitive(SwitchParser, 'c', true);
CmdSwitchCombo LinkerMain::Map(SwitchParser, 'm', "x");
CmdSwitchBool LinkerMain::DebugInfo(SwitchParser, 'g', false);
CmdSwitchCombineString LinkerMain::LinkOnly(SwitchParser, 'l', ';');
CmdSwitchBool LinkerMain::RelFile(SwitchParser, 'r', false);
CmdSwitchCombineString LinkerMain::Specification(SwitchParser, 's');
CmdSwitchDefine LinkerMain::Defines(SwitchParser, 'D');
CmdSwitchCombineString LinkerMain::LibPath(SwitchParser, 'L', ';');
CmdSwitchOutput LinkerMain::OutputFile(SwitchParser, 'o', ".rel");
CmdSwitchBool LinkerMain::Verbosity(SwitchParser, 'y');
CmdSwitchCombineString LinkerMain::OutputDefFile(SwitchParser, 0, 0, {"output-def"});
CmdSwitchCombineString LinkerMain::OutputImportLibrary(SwitchParser, 0, 0, {"out-implib"});
CmdSwitchCombineString LinkerMain::PrintFileName(SwitchParser, 0, 0, {"print-file-name"});
CmdSwitchCombineString LinkerMain::DelayLoadDll(SwitchParser, 0, ';', {"delayload"});
CmdSwitchCombineString LinkerMain::DelayLoadFlags(SwitchParser, 0, ';', {"delay"});

SwitchConfig LinkerMain::TargetConfig(SwitchParser, 'T');
const char* LinkerMain::helpText =
    "[options] inputfiles\n"
    "\n"
    "/Dxxx=val Define something           /Lpath         Set Library Path\n"
    "/T:xxx    Target configuration       /V, --version  Show version and date\n"
    "/c+       Case sensitive link        /g             pass debug info\n"
    "/l        link only                  /lxxx          link against xxx\n"
    "/m[x]     Generate Map file          /oxxx          Set output file\n"
    "/r+       Relative output file       /sxxx          Read specification file\n"
    "/y[...]   Verbose                    /!, --nologo   No logo\n"
    "/?, --help This text\n"
    "\n"
    " --output-def filename    create a .def file for DLLs\n"
    " --out-implib filename    specify name for import library when generating DLLs\n"
    " --shared                 create a dll\n"
    " -delayload dllname       specify a dll to add to the delay load table\n"
    " -delay[nobind|unload]    specify a delay load flag\n"

    "@xxx      Read commands from file\n"
    "\nTime: " __TIME__ "  Date: " __DATE__;
const char* LinkerMain::usageText = "[options] inputfiles";

const ObjString& LinkerMain::GetOutputFile(CmdFiles& files)
{
    static ObjString outputFile;
    if (!OutputFile.GetValue().empty())
    {
        outputFile = OutputFile.GetValue();
        if (outputFile.find(".exe") != std::string::npos || outputFile.find(".dll") != std::string::npos)
            outputFile = Utils::QualifiedFile(outputFile.c_str(), ".rel");
        else if (outputFile.find(".rel") == std::string::npos)
            outputFile += ".rel";
    }
    else if (files.size())
    {
        outputFile = Utils::QualifiedFile(files[1].c_str(), ".rel");
    }
    else if (!PrintFileName.GetExists())
    {
        std::cout << "Nothing to do." << std::endl;
        exit(1);
    }
    return outputFile;
}
const ObjString& LinkerMain::GetMapFile(CmdFiles& files)
{
    static ObjString mapFile;
    if (!OutputFile.GetValue().empty())
    {
        mapFile = Utils::QualifiedFile(OutputFile.GetValue().c_str(), ".map");
    }
    else if (files.size())
    {
        mapFile = Utils::QualifiedFile(files[0].c_str(), ".map");
    }
    else if (!PrintFileName.GetExists())
    {
        std::cout << "Nothing to do." << std::endl;
        exit(1);
    }
    return mapFile;
}
void LinkerMain::AddFile(LinkManager& linker, const std::string& name)
{
    if (!TargetConfig.InterceptFile(name))
    {
        bool foundLibrary = false;
        size_t n = name.find_last_of(".");
        if (n != std::string::npos)
        {
            ObjString match = name.substr(n);
            std::transform(match.begin(), match.end(), match.begin(), [](unsigned char c) { return tolower(c); });

            foundLibrary = match == ".l" || match == ".a" || match == ".lib" || match == ".dll";
        }
        if (foundLibrary)
            linker.AddLibrary(name);
        else
            linker.AddObject(name);
    }
}
void LinkerMain::AddFiles(LinkManager& linker, CmdFiles& files)
{
    for (int i = 1; i < files.size(); i++)
        AddFile(linker, files[i]);
}
void LinkerMain::SetDefines(LinkManager& linker)
{
    bool hasHeapCheck = false;
    TargetConfig.SetDefines(linker);
    for (int i = 0; i < Defines.GetCount(); i++)
    {
        const CmdSwitchDefine::define* d = Defines.GetValue(i);
        TargetConfig.AddDefine(linker, d->name, d->value);
        hasHeapCheck |= d->name == "__HEAP_CHECK";
    }
    if (!hasHeapCheck)
    {
        TargetConfig.AddDefine(linker, "__HEAP_CHECK", "0");
    }
}
std::string LinkerMain::SpecFileContents(const std::string& specFile)
{
    std::string rv;
    if (!specFile.empty())
    {
        std::fstream fil(specFile, std::ios::in);
        if (!fil.fail())
        {
            fil.seekg(0, std::ios::end);
            size_t n = fil.tellg();
            fil.seekg(0);
            std::unique_ptr<char[]> data = std::make_unique<char[]>(n + 1);
            fil.read(data.get(), n);
            data[fil.gcount()] = '\0';
            rv = data.get();
        }
        else
        {
            LinkManager::LinkError("Specification file " + specFile + " does not exist");
        }
    }
    return rv;
}
void LinkerMain::RewriteArgs(int argc, char** argv)
{
    for (int i = 0; i < argc; i++)
        if (!strcmp(argv[i], "--shared"))
            strcpy(argv[i], "-T:DLL32");
}
bool LinkerMain::DoPrintFileName(LinkManager& linker)
{
    if (PrintFileName.GetExists())
    {
        std::string path;
        FILE* fil = linker.GetLibraryPath(PrintFileName.GetValue(), path);
        if (fil)
        {
            fclose(fil);
            printf("%s", path.c_str());
        }
        else
        {
            printf("%s", PrintFileName.GetValue().c_str());
        }
        return true;
    }
    return false;
}
void LinkerMain::ParseSpecifiedLibFiles(CmdFiles& files, LinkManager& manager)
{
    auto splt = Utils::split(LinkOnly.GetValue());
    if (splt.size() != 0)
    {
        for (auto&& s : splt)
        {
            std::string y = s;
            if (s.find(".") == std::string::npos)
            {
                y += ".l";
            }
            auto file = manager.GetLibraryPath(y, y);
            if (!file)
            {
                y = "lib" + y;
                file = manager.GetLibraryPath(y, y);
            }
            if (!file)
            {
                y = s;
                if (s.find(".") == std::string::npos)
                {
                    y += ".lib";
                    file = manager.GetLibraryPath(y, y);
                    if (!file)
                    {
                        y = "lib" + y;
                        file = manager.GetLibraryPath(y, y);
                    }
                }
            }
            if (file)
            {
                files.Add(y);
                fclose(file);
            }
            else
            {
                // should generate an error later on...
                files.Add(s + ".l");
            }
        }
    }
}
void LinkerMain::ParseDelayLoadFlags()
{
    if (DelayLoadFlags.GetExists())
    {
        for (auto&& s : Utils::split(DelayLoadFlags.GetValue()))
        {
             if (s == "nobind" || s == "NOBIND")
                  bindtable = false;
             else if (s == "unload" || s == "UNLOAD")
                  unloadtable = true;
             else
                 Utils::Fatal(std::string("unknown delayload flag: ") + s);
        }        
    }
}

int LinkerMain::Run(int argc, char** argv)
{
    RewriteArgs(argc, argv);
    if (getenv("OLINK_LEGACY_OPTIONS"))
    {
        for (int i = 0; i < argc; i++)
        {
            if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "/v"))
            {
                DebugInfo.SetValue(true);
                exit(0);
            }
        }
    }
    char* modName = Utils::GetModuleName();
    std::string appName = Utils::QualifiedFile(modName, ".app");
    if (!TargetConfig.ReadConfigFile(appName))
    {
        Utils::Fatal("Corrupt configuration file");
    }
    if (!TargetConfig.Validate())
    {
        Utils::Fatal("Incompatible target configurations");
    }
    auto files = ToolChain::StandardToolStartup(SwitchParser, argc, argv, usageText, helpText);
    if (files.size() < 2 && !PrintFileName.GetExists())
        ToolChain::Usage(usageText);
    // setup
    const ObjString& outputFile = GetOutputFile(files);
    unlink(outputFile.c_str());
    const ObjString& mapFile = GetMapFile(files);
    ObjString specificationFile = Specification.GetValue();
    if (specificationFile.empty())
    {
        std::string val = TargetConfig.GetSpecFile();
        if (!val.empty())
        {
            std::string prefix = modName;
            size_t n = prefix.find_last_of('\\');
            size_t n1 = prefix.find_last_of('/');
            if (n1 != std::string::npos && n != std::string::npos)
                n = n < n1 ? n1 : n;
            if (n != std::string::npos)
            {
                prefix.replace(n + 1, prefix.size() - n, "");
            }
            else
            {
                prefix.replace(0, prefix.size(), "");
            }
            specificationFile = prefix + val;
        }
    }
    ObjString debugFile;
    // object ordering is important to allow proper deconstruction
    ObjIeeeIndexManager im1;
    ObjIeeeIndexManager im2;
    ObjFactory fact1(&im2);
    if (DebugInfo.GetValue())
        debugFile = Utils::QualifiedFile(outputFile.c_str(), ".odx");
    char* lpath = getenv("LIBRARY_PATH");
    if (lpath)
    {
        if (!LibPath.GetValue().empty())
            LibPath += ";";
        LibPath += lpath;
    }
    LinkManager linker(SpecFileContents(specificationFile), CaseSensitive.GetValue(), outputFile,
                       !RelFile.GetValue() && !TargetConfig.GetRelFile(), TargetConfig.GetDebugPassThrough(), debugFile);
    linker.SetLibPath(LibPath.GetValue());
    linker.SetDelayLoad(DelayLoadDll.GetValue());
    ParseSpecifiedLibFiles(files, linker);
    ParseDelayLoadFlags();
    if (DoPrintFileName(linker))
        exit(0);
    linker.SetIndexManager(&im1);
    linker.SetFactory(&fact1);
    ObjIeee ieee(outputFile, CaseSensitive.GetValue());
    ieee.SetDebugInfoFlag(DebugInfo.GetValue());
    linker.SetObjIo(&ieee);
    // enter files and link
    AddFiles(linker, files);
    SetDefines(linker);
    linker.Link();
    if (!linker.ErrCount())
    {
        if (Map.GetValue('x'))
        {
            LinkMap mapper(LinkMap::eDetailed, (LinkMap::eMapMode)TargetConfig.GetMapMode(), mapFile, &linker);
            mapper.WriteMap();
        }
        else if (Map.GetValue())
        {
            LinkMap mapper(LinkMap::ePublic, (LinkMap::eMapMode)TargetConfig.GetMapMode(), mapFile, &linker);
            mapper.WriteMap();
        }
        if (LinkOnly.GetExists() && LinkOnly.GetValue() == "")
        {
            return 0;
        }
        else
        {
            std::string path = modName;
            int n = path.find_last_of(CmdFiles::DIR_SEP[0]);
            if (n == std::string::npos)
                path = "";
            else
                path.erase(n + 1);
            int rv = TargetConfig.RunApp(path, outputFile, Utils::AbsolutePath(debugFile), Verbosity.GetExists(),
                                         OutputDefFile.GetValue(), OutputImportLibrary.GetValue(), bindtable, unloadtable, DelayLoadDll.GetValue());
            if (!Verbosity.GetExists())
                unlink(outputFile.c_str());
            return rv;
        }
    }
    return 1;
}
