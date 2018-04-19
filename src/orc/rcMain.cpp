/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
#include "rcMain.h"
#include "rcFile.h"
#include "resFile.h"

#include "Utils.h"
#include "CmdFiles.h"
#include "PreProcessor.h"

#include <windows.h>

CmdSwitchParser rcMain::SwitchParser;
CmdSwitchFile rcMain::File(SwitchParser, '@');
CmdSwitchBool rcMain::Boolr(SwitchParser, 'r');
CmdSwitchBool rcMain::Boolt(SwitchParser, 't');
CmdSwitchBool rcMain::Boolv(SwitchParser, 'v');
CmdSwitchOutput rcMain::OutputFile(SwitchParser, 'o', ".res");
CmdSwitchDefine rcMain::Defines(SwitchParser, 'D');
CmdSwitchCombineString rcMain::includePath(SwitchParser, 'i', ';');
CmdSwitchString rcMain::Language(SwitchParser, 'L');

char *rcMain::usageText = "[options] file"
"\n"
"  @filename  use response file\n"
"  /Dxxx  Define something             /ixxx  Set include file path\n"
"  /Lxx   Set default language id      /oxxx  Set output file name\n"
"  /r     reserved for compatability   /t     reserved for compatability\n"
"  /v     reserved for compatability   /V     Show version and date\n"
"  /!     No logo\n"
"\n"
"Time: " __TIME__ "  Date: " __DATE__;

int main(int argc, char *argv[])
{
    rcMain rc;
    return rc.Run(argc, argv);
}

int rcMain::Run(int argc, char *argv[])
{
    int rv = 0;
    int language = LANG_ENGLISH + (SUBLANG_ENGLISH_US << 10);
    Utils::banner(argv[0]);
    CmdSwitchFile internalConfig(SwitchParser);
    std::string configName = Utils::QualifiedFile(Utils::GetModuleName(), ".cfg");
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
    CmdFiles files(argv+1);
    if (File.GetValue())
        files.Add(File.GetValue() + 1);
    if (files.GetSize() > 1 && OutputFile.GetValue().size())
        Utils::fatal("Cannot specify output file for multiple input files");
    std::string sysSrchPth;
    std::string srchPth;
    if (includePath.GetValue().size())
    {
        size_t n = includePath.GetValue().find_first_of(';');
        if (n == std::string::npos)
        {
            sysSrchPth = includePath.GetValue();
        }
        else
        {
            sysSrchPth = includePath.GetValue().substr(0, n);
            srchPth = includePath.GetValue().substr(n+1);
        }
    }
    if (Language.GetValue().size())
    {
        sscanf(Language.GetValue().c_str(), "%d", &language);
    }
    for (CmdFiles::FileNameIterator it = files.FileNameBegin(); it != files.FileNameEnd(); ++it)
    {
        std::string inName = Utils::QualifiedFile( (*it)->c_str(), ".rc");
        PreProcessor pp(inName, srchPth, sysSrchPth,
                 false, false, '#', false, true, true);
        int n = Defines.GetCount();
        for (int i=0; i < n; i++)
        {
            CmdSwitchDefine::define *v = Defines.GetValue(i);
            pp.Define(v->name, v->value, false);
        }
		std::string s("");
        pp.Define("RC_INVOKED", s, false);
        std::string outName;
        if (OutputFile.GetValue().size() != 0)
            outName = OutputFile.GetValue();
        else
            outName = Utils::QualifiedFile( (*it)->c_str(), ".res");
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
