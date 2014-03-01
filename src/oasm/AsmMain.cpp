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
#include "AsmMain.h"
#include "AsmFile.h"

#include "Utils.h"
#include "CmdFiles.h"
#include "PreProcessor.h"
#include "Listing.h"
#include "UTF8.h"
#include <stdlib.h>

CmdSwitchParser AsmMain::SwitchParser;
CmdSwitchBool AsmMain::CaseInsensitive(SwitchParser, 'i');
CmdSwitchCombo AsmMain::CreateListFile(SwitchParser, 'l',"m");
CmdSwitchFile AsmMain::File(SwitchParser, '@');
CmdSwitchBool AsmMain::PreprocessOnly(SwitchParser, 'e');
CmdSwitchOutput AsmMain::OutputFile(SwitchParser, 'o', ".res");
CmdSwitchDefine AsmMain::Defines(SwitchParser, 'D');
CmdSwitchString AsmMain::includePath(SwitchParser, 'I', ';');

char *AsmMain::usageText = "[options] file"
"\n"
"  @filename  use response file\n"
"  /e     Preprocess only                  /i     Case Insensitive Labels\n"
"  /l[m]  Listing file [macro expansions]  /oxxx  Set output file name\n"
"  /Dxxx  Define something                 /Ixxx  Set include file path\n"
"\n"
"Time: " __TIME__ "  Date: " __DATE__;

int main(int argc, char *argv[])
{
    AsmMain rc;
    return rc.Run(argc, argv);
}
void AsmMain::CheckAssign(std::string &line, PreProcessor &pp)
{
    int npos = line.find_first_not_of(" \t\r\n\v");
    if (npos != std::string::npos)
    {
        if (line[npos] == '%')
        {
            npos = line.find_first_not_of(" \t\r\b\v", npos + 1);
            bool caseInsensitive = false;
            bool assign = false;
            if (npos != std::string::npos)
            {
                if (line.size() - 7 > npos && line.substr(npos,6) == "assign" && isspace(line[npos+6]))
                {
                    assign = true;
                }
                else if (line.size() - 8 > npos && line.substr(npos, 7) == "iassign" && isspace(line[npos+7]))
                {
                    assign = true;
                    caseInsensitive = true;
                }
            }
            if (assign)
            {
                std::string name;
                int value = 0;
                npos = line.find_first_not_of(" \t\r\b\v", npos+6 + (caseInsensitive ? 1 : 0));
                if (npos == std::string::npos || !IsSymbolStartChar(line.c_str() + npos))
                {
                    Errors::Error("Expected identifier");
                }
                else
                {
                    int npos1 = npos;
                    
                    while (npos1 != line.size() && IsSymbolChar(line.c_str() + npos1))
                    {
                        int n = UTF8::CharSpan(line.c_str() + npos);
                        while (n && npos1 < line.size())
                           npos1 ++,n--;
                    }
                    name = line.substr(npos, npos1-npos);
                    if (!isspace(line[npos1]))
                    {
                        Errors::Error("Invalid arguments to %assign");
                    }
                    else
                    {
                        npos = line.find_first_not_of(" \t\r\n\v", npos1);
                        if (npos == std::string::npos)
                        {
                            Errors::Error("Expected expression");
                        }
                        else
                        {
                            ppExpr e(false);
                            value = e.Eval(line.substr(npos));
                            pp.Assign(name, value, caseInsensitive);
                        }
                    }
                }
                line = "";
            }
        }
    }
}

int AsmMain::Run(int argc, char *argv[])
{
    int rv = 0;
    Utils::banner(argv[0]);
    CmdSwitchFile internalConfig(SwitchParser);
    std::string configName = Utils::QualifiedFile(argv[0], ".cfg");
    std::fstream configTest(configName.c_str(), std::ios::in);
    if (configTest != NULL)
    {
        configTest.close();
        if (!internalConfig.Parse(configName.c_str()))
            Utils::fatal("Corrupt configuration file");
    }
    if (!SwitchParser.Parse(&argc, argv) || argc == 1 && File.GetCount() <= 1)
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
    for (CmdFiles::FileNameIterator it = files.FileNameBegin(); it != files.FileNameEnd(); ++it)
    {
        std::string inName = (*it)->c_str();
        int npos = inName.find_last_of(".");
        if (npos == std::string::npos || npos && inName[npos-1] == '.' || npos != inName.size()-1 && inName[npos+1] == CmdFiles::DIR_SEP[0])
        {
            inName = Utils::QualifiedFile( (*it)->c_str(), ".asm");
        }
        PreProcessor pp(inName, srchPth, sysSrchPth,
                 false, false, '%', false, false, true);
        int n = Defines.GetCount();
        for (int i=0; i < n; i++)
        {
            CmdSwitchDefine::define *v = Defines.GetValue(i);
            pp.Define(v->name, v->value, false);
        }
        std::string outName;
        if (OutputFile.GetValue().size() != 0)
            outName = OutputFile.GetValue();
        else
            outName = Utils::QualifiedFile( (*it)->c_str(), ".o");
        if (PreprocessOnly.GetValue())
        {
            std::string working = Utils::QualifiedFile((*it)->c_str(), ".i");
            std::fstream out(working.c_str(), std::ios::out);
            if (out == NULL)
            {
                Utils::fatal(std::string(std::string("Could not open ") + working.c_str() + " for write.").c_str());
            }
            else
            {
                while (pp.GetLine(working))
                {
                    CheckAssign(working, pp);
                    out << working << std::endl;
                }
            }
            out.close();
        }
        else
        {
            Listing listing;
            AsmFile asmFile(pp, CaseInsensitive.GetValue(), listing);
            if (asmFile.Read())
            {
                if (!asmFile.Write(outName, inName) || Errors::ErrorCount())
                {
                    rv = 1;
                }
                else if (CreateListFile.GetValue())
                {
                    std::string listingName = Utils::QualifiedFile( (*it)->c_str(), ".lst");
                    if (!listing.Write(listingName, inName, CreateListFile.GetValue('m')))
                    {
                        rv = 1;
                    }
                }
            }
            else
            {
                rv = 1;
            }
        if (rv)
            unlink(outName.c_str());
        }
    }
    return rv;
}
