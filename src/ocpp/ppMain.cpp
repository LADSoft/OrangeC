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
#include "ppMain.h"
#include "Utils.h"
#include "CmdFiles.h"
#include "PreProcessor.h"
#include "Errors.h"

#include <sstream>
CmdSwitchParser ppMain::SwitchParser;
CmdSwitchBool ppMain::assembly(SwitchParser, 'a', false);
CmdSwitchBool ppMain::disableExtensions(SwitchParser, 'A', false);
CmdSwitchBool ppMain::c99Mode(SwitchParser, '9', false);
CmdSwitchDefine ppMain::defines(SwitchParser, 'D');
CmdSwitchString ppMain::undefines(SwitchParser, 'U',';');
CmdSwitchString ppMain::includePath(SwitchParser, 'I', ';');
CmdSwitchString ppMain::errorMax(SwitchParser, 'E');
CmdSwitchFile ppMain::File(SwitchParser, '@');
    
char *ppMain::usageText = "[options] files\n"
        "\n"
        "/9      - C99 mode                  /a      - Assembler mode\n"
        "/A      - Disable extensions        /Dxxx   - Define something\n"
        "/E[+]nn - Max number of errors      /Ipath  - Specify include path\n"
        "/Uxxx   - Undefine something\n"
        "\n"
        "Time: " __TIME__ "  Date: " __DATE__;

int main(int argc, char *argv[])
{
    ppMain preproc;
    return preproc.Run(argc, argv);
}
int ppMain::Run(int argc, char *argv[])
{
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
    if (!SwitchParser.Parse(&argc, argv) || (argc == 1 && File.GetCount() <= 1))
    {
        Utils::usage(argv[0], usageText);
    }
    CmdFiles files(argv+1);
    if (File.GetValue())
        files.Add(File.GetValue() + 1);

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
    Tokenizer::SetAnsi(disableExtensions.GetValue());
    Tokenizer::SetC99(c99Mode.GetValue());
    for (CmdFiles::FileNameIterator it = files.FileNameBegin(); it != files.FileNameEnd(); ++it)
    {
        PreProcessor pp(*(*it), srchPth, sysSrchPth,
                 false, false, assembly.GetValue() ? '%' : '#' , false, !c99Mode.GetValue(), !disableExtensions.GetValue());
        int n = defines.GetCount();
        for (int i=0; i < n; i++)
        {
            CmdSwitchDefine::define *v = defines.GetValue(i);
            pp.Define(v->name, v->value, false);
        }
        std::string working = undefines.GetValue();
        while (working.size())
        {
            size_t n = working.find_first_of(';');
            if (n == std::string::npos)
            {
                pp.Undefine(working);
                working = "";
            }
            else
            {
                std::string temp = working.substr(0, n);
                pp.Undefine(temp);
                working = working.substr(n+1);
            }
        }
        working = errorMax.GetValue();
        if (working.size())
        {
            if (working[0] == '+')
            {
                Errors::SetShowTrivialWarnings(true);
                working.erase(0,1);
            }
            std::stringstream v;
            v << working;
            v >> n;
            if (n)
            {
                Errors::SetMaxErrors(n);
            }
        }
        working = Utils::QualifiedFile((*it)->c_str(), ".i");
        std::fstream out(working.c_str(), std::ios::out);
        while (pp.GetLine(working))
        {
            if (assembly.GetValue())
            {
                int npos = working.find_first_not_of(" \t\r\n\v");
                if (npos != std::string::npos)
                {
                    if (working[npos] == '%')
                    {
                        npos = working.find_first_not_of(" \t\r\b\v", npos + 1);
                        bool isAssign = false;
                        bool caseInsensitive = false;
                        if (npos != std::string::npos)
                            if (working.size() - 7 > npos && working.substr(npos,6) == "assign" && isspace(working[npos+6]))
                            {
                                isAssign = true;
                            }
                            else if (working.size() - 8 > npos && working.substr(npos,7) == "iassign" && isspace(working[npos+7]))
                            {
                                isAssign = true;
                                caseInsensitive = true;
                            }
                            
                        if (isAssign)
                        {
                            std::string name;
                            PPINT value = 0;
                            npos = working.find_first_not_of(" \t\r\b\v", npos+6 + (caseInsensitive ? 1 : 0));
                            if (npos == std::string::npos || !IsSymbolStartChar(working.c_str() + npos))
                            {
                                Errors::Error("Expected identifier");
                            }
                            else
                            {
                                int npos1 = npos;
                                
                                while (npos1 != working.size() && IsSymbolChar(working.c_str() + npos1))
                                       npos1 ++;
                                name = working.substr(npos, npos1-npos);
                                if (!isspace(working[npos1]))
                                {
                                    Errors::Error("Invalid arguments to %assign");
                                }
                                else
                                {
                                    npos = working.find_first_not_of(" \t\r\n\v", npos1);
                                    if (npos == std::string::npos)
                                    {
                                        Errors::Error("Expected expression");
                                    }
                                    else
                                    {
                                        ppExpr e(false);
                                        std::string temp = working.substr(npos);
                                        value = e.Eval(temp);
                                        if (value < INT_MIN || value >= UINT_MAX)
                                            Errors::Error("ocpp does not support long longs in command line definitions");
                                        pp.Assign(name, (int)value, caseInsensitive);
                                    }
                                }
                            }
                            working = "";
                        }
                    }
                }
            }
            out << working << std::endl;
        }
        out.close();
    }
    return 0;
}
