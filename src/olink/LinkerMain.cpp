/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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
#include "LinkerMain.h"
#include <fstream>
#include <stdio.h>
#include <string.h>
#ifdef GCCLINUX
#    include <unistd.h>
#else
#    include <io.h>
#endif

int main(int argc, char** argv)
{
    LinkerMain linker;
    return linker.Run(argc, argv);
}

CmdSwitchParser LinkerMain::SwitchParser;

CmdSwitchBool LinkerMain::CaseSensitive(SwitchParser, 'c', true);
CmdSwitchCombo LinkerMain::Map(SwitchParser, 'm', "x");
CmdSwitchBool LinkerMain::DebugInfo(SwitchParser, 'v', false);
CmdSwitchBool LinkerMain::LinkOnly(SwitchParser, 'l', false);
CmdSwitchBool LinkerMain::RelFile(SwitchParser, 'r', false);
CmdSwitchFile LinkerMain::File(SwitchParser, '@');
CmdSwitchCombineString LinkerMain::Specification(SwitchParser, 's');
CmdSwitchDefine LinkerMain::Defines(SwitchParser, 'D');
CmdSwitchCombineString LinkerMain::LibPath(SwitchParser, 'L', ';');
CmdSwitchOutput LinkerMain::OutputFile(SwitchParser, 'o', ".rel");
CmdSwitchBool LinkerMain::Verbosity(SwitchParser, 'y');
SwitchConfig LinkerMain::TargetConfig(SwitchParser, 'T');
const char* LinkerMain::usageText =
    "[options] inputfiles\n"
    "\n"
    "/Dxxx=val Define something           /Lpath         Set Library Path\n"
    "/T:xxx    Target configuration       /V, --version  Show version and date\n"
    "/c+       Case sensitive link        /l             link only\n"
    "/m[x]     Generate Map file          /oxxx          Set output file\n"
    "/r+       Relative output file       /sxxx          Read specification file\n"
    "/v        Pass debug info            /y[...]        Verbose\n"
    "/!, --nologo   No logo\n"
    "@xxx      Read commands from file\n"
    "\nTime: " __TIME__ "  Date: " __DATE__;

const ObjString& LinkerMain::GetOutputFile(CmdFiles& files)
{
    static ObjString outputFile;
    if (OutputFile.GetValue().size() != 0)
    {
        outputFile = OutputFile.GetValue();
        if (outputFile.find(".exe") != std::string::npos || outputFile.find(".dll") != std::string::npos)
            outputFile = Utils::QualifiedFile(outputFile.c_str(), ".rel");
        else if (outputFile.find(".rel") == std::string::npos)
            outputFile += ".rel";
    }
    else if (files.GetSize())
    {
        CmdFiles::FileNameIterator it = files.FileNameBegin();
        outputFile = Utils::QualifiedFile((*it)->c_str(), ".rel");
    }
    else
    {
        std::cout << "Nothing to do." << std::endl;
        exit(1);
    }
    return outputFile;
}
const ObjString& LinkerMain::GetMapFile(CmdFiles& files)
{
    static ObjString mapFile;
    if (OutputFile.GetValue().size() != 0)
    {
        mapFile = Utils::QualifiedFile(OutputFile.GetValue().c_str(), ".map");
    }
    else if (files.GetSize())
    {
        CmdFiles::FileNameIterator it = files.FileNameBegin();
        mapFile = Utils::QualifiedFile((*it)->c_str(), ".map");
    }
    else
    {
        std::cout << "Nothing to do." << std::endl;
        exit(1);
    }
    return mapFile;
}
void LinkerMain::AddFile(LinkManager& linker, std::string& name)
{
    if (!TargetConfig.InterceptFile(name))
    {
        bool found = false;
        size_t n = name.find_last_of(".");
        if (n != std::string::npos)
        {
            ObjString match = name.substr(n);
            found = match == ".l" || match == ".L" || match == ".a" || match == ".lib";
        }
        if (found)
            linker.AddLibrary(name);
        else
            linker.AddObject(name);
    }
}
void LinkerMain::AddFiles(LinkManager& linker, CmdFiles& files)
{
    for (CmdFiles::FileNameIterator it = files.FileNameBegin(); it != files.FileNameEnd(); ++it)
        AddFile(linker, (*(*it)));
}
void LinkerMain::SetDefines(LinkManager& linker)
{
    TargetConfig.SetDefines(linker);
    for (int i = 0; i < Defines.GetCount(); i++)
    {
        const CmdSwitchDefine::define* d = Defines.GetValue(i);
        TargetConfig.AddDefine(linker, d->name, d->value);
    }
}
std::string LinkerMain::SpecFileContents(const std::string& specFile)
{
    std::string rv;
    if (specFile.size() != 0)
    {
        std::fstream fil(specFile.c_str(), std::ios::in);
        if (!fil.fail())
        {
            fil.seekg(0, std::ios::end);
            size_t n = fil.tellg();
            fil.seekg(0);
            char* data = new char[n + 1];
            fil.read(data, n);
            data[fil.gcount()] = '\0';
            rv = data;
            delete[] data;
        }
        else
        {
            LinkManager::LinkError("Specification file " + specFile + " does not exist");
        }
    }
    return rv;
}
int LinkerMain::Run(int argc, char** argv)
{
    Utils::banner(argv[0]);
    Utils::SetEnvironmentToPathParent("ORANGEC");
    char* modName = Utils::GetModuleName();
    std::string appName = Utils::QualifiedFile(modName, ".app");
    if (!TargetConfig.ReadConfigFile(appName))
    {
        Utils::fatal("Corrupt configuration file");
    }
    CmdSwitchFile internalConfig(SwitchParser);
    std::string configName = Utils::QualifiedFile(modName, ".cfg");
    std::fstream configTest(configName.c_str(), std::ios::in);
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
    if (!TargetConfig.Validate())
    {
        Utils::fatal("Incompatible target configurations");
    }
    CmdFiles files(argv + 1);
    if (File.GetValue())
        files.Add(File.GetValue() + 1);

    // setup
    const ObjString& outputFile = GetOutputFile(files);
    unlink(outputFile.c_str());
    const ObjString& mapFile = GetMapFile(files);
    ObjString specificationFile = Specification.GetValue();
    if (specificationFile.size() == 0)
    {
        std::string val = TargetConfig.GetSpecFile();
        if (val.size())
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
        if (LibPath.GetValue().size())
            LibPath += ";";
        LibPath += lpath;
    }
    LinkManager linker(SpecFileContents(specificationFile), CaseSensitive.GetValue(), outputFile,
                       !RelFile.GetValue() && !TargetConfig.GetRelFile(), TargetConfig.GetDebugPassThrough(), debugFile);
    linker.SetLibPath(LibPath.GetValue());
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
        if (LinkOnly.GetValue())
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
            int rv = TargetConfig.RunApp(path, outputFile, debugFile, Verbosity.GetExists());
            if (!Verbosity.GetExists())
                _unlink(outputFile.c_str());
            return rv;
        }
    }
    return 1;
}
