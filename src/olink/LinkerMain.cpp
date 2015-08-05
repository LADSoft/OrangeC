/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
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
#include <windows.h>
#include <fstream>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
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
CmdSwitchString LinkerMain::Specification(SwitchParser, 's');
CmdSwitchDefine LinkerMain::Defines(SwitchParser, 'D');
CmdSwitchString LinkerMain::LibPath(SwitchParser, 'L',';');
CmdSwitchOutput LinkerMain::OutputFile(SwitchParser, 'o', ".rel");
SwitchConfig LinkerMain::TargetConfig(SwitchParser, 'T');
char *LinkerMain::usageText = "[options] inputfiles\n"
            "\n"
            "/Dxxx=val Define something           /Lpath    Set Library Path\n"
            "/T:xxx    Target configuration       /c+       Case sensitive link\n"
            "/l        link only                  /m[x]     Generate Map file\n"
            "/oxxx     Set output file            /r+       Relative output file\n"
            "/sxxx     Read specification file    /v        Pass debug info\n"
            "@xxx      Read commands from file\n"
            "\nTime: " __TIME__ "  Date: " __DATE__;

const ObjString &LinkerMain::GetOutputFile(CmdFiles &files)
{
    static ObjString outputFile;
    if (OutputFile.GetValue().size() != 0)
        return OutputFile.GetValue();
    else if (files.GetSize())
    {
        CmdFiles::FileNameIterator it = files.FileNameBegin();
        outputFile = Utils::QualifiedFile( (*it)->c_str(), ".rel");
    }
    else
    {
        std::cout << "Nothing to do." << std::endl;
        exit(1);
    }
    return outputFile;
}
const ObjString &LinkerMain::GetMapFile(CmdFiles &files)
{
    static ObjString mapFile;
    if (OutputFile.GetValue().size() != 0)
    {
        mapFile = Utils::QualifiedFile( OutputFile.GetValue().c_str(), ".map");
    }
    else if (files.GetSize())
    {
        CmdFiles::FileNameIterator it = files.FileNameBegin();
        mapFile = Utils::QualifiedFile( (*it)->c_str(), ".map");
    }
    else
    {
        std::cout << "Nothing to do." << std::endl;
        exit(1);
    }
    return mapFile;
}
#include <stdio.h>
void LinkerMain::AddFile(LinkManager &linker, std::string &name)
{
    if (!TargetConfig.InterceptFile(name))
    {
        bool found = false;
        if (name.size() > 1 && name.find(".l")== name.size()-2)
            found = true;
        else if (name.size() > 1 && name.find(".L")== name.size()-2)
            found = true;
        if (found)
            linker.AddLibrary(name);
        else
            linker.AddObject(name);
    }
}
void LinkerMain::AddFiles(LinkManager &linker, CmdFiles &files)
{
    for (CmdFiles::FileNameIterator it = files.FileNameBegin(); it != files.FileNameEnd(); ++it)
        AddFile(linker, (*(*it)));
}
void LinkerMain::SetDefines(LinkManager &linker)
{
    TargetConfig.SetDefines(linker);
    for (int i=0; i < Defines.GetCount(); i++)
    {
        const CmdSwitchDefine::define *d = Defines.GetValue(i);
        TargetConfig.AddDefine(linker, d->name, d->value);
    }
}
std::string LinkerMain::SpecFileContents(const std::string &specFile)
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
            char *data = new char[n+1];
            fil.read(data, n);
            data[fil.gcount()] = '\0';
            rv = data;
            delete [] data;
        }
        else
        {
            LinkManager::LinkError("Specification file " + specFile + " does not exist");			
        }
    }
    return rv;
}
int LinkerMain::Run(int argc, char **argv)
{
    Utils::banner(argv[0]);
    char *modName = Utils::GetModuleName();
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
    CmdFiles files(argv+1);
    if (File.GetValue())
        files.Add(File.GetValue() + 1);
        
    // setup
    const ObjString &outputFile = GetOutputFile(files);
    unlink(outputFile.c_str());
    const ObjString &mapFile = GetMapFile(files);
    ObjString specificationFile = Specification.GetValue();
    if (specificationFile.size() == 0)
    {
        std::string val = TargetConfig.GetSpecFile();
        if (val.size())
        {
            std::string prefix = modName;
            size_t n = prefix.find_last_of('\\');
            if (n != std::string::npos)
            {
                prefix.replace(n+1, prefix.size()-n, "");
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
    LinkManager linker(SpecFileContents(specificationFile), CaseSensitive.GetValue(), 
                       outputFile, !RelFile.GetValue() && !TargetConfig.GetRelFile(), 
                       TargetConfig.GetDebugPassThrough(), debugFile);
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
            LinkMap mapper(LinkMap::eDetailed, (LinkMap::eMapMode)TargetConfig.GetMapMode(),mapFile, &linker);
            mapper.WriteMap();
        }
        else if (Map.GetValue())
        {
            LinkMap mapper(LinkMap::ePublic,(LinkMap::eMapMode)TargetConfig.GetMapMode(), mapFile, &linker);
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
                path.erase(n+1);
            int rv = TargetConfig.RunApp(path, outputFile, debugFile);
            return rv;
        }
    }
    return 1;
}
