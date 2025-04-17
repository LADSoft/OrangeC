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

// https://www.ibm.com/developerworks/library/l-gas-nasm/index.html
#include "AsmMain.h"
#include "AsmFile.h"

#include "Utils.h"
#include "ToolChain.h"
#include "CmdFiles.h"
#include "PreProcessor.h"
#include "Listing.h"
#include "UTF8.h"
#include <cstdlib>
#include "Token.h"

#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#else
#    include <io.h>
#endif

extern bool IsSymbolCharRoutine(const char*, bool);
bool (*Tokenizer::IsSymbolChar)(const char*, bool) = IsSymbolCharRoutine;

CmdSwitchParser AsmMain::SwitchParser;
CmdSwitchBool AsmMain::CaseInsensitive(SwitchParser, 'i', false, {"case-insensitive"});
CmdSwitchCombo AsmMain::CreateListFile(SwitchParser, 'l', "m", {"list"});
CmdSwitchBool AsmMain::PreprocessOnly(SwitchParser, 'e', false, {"preprocess-only"});
CmdSwitchOutput AsmMain::OutputFile(SwitchParser, 'o', ".o", false, {"output-file"});
CmdSwitchDefine AsmMain::Defines(SwitchParser, 'D');
CmdSwitchCombineString AsmMain::includePath(SwitchParser, 'I', ';', {"include-path"});
CmdSwitchBool AsmMain::BinaryOutput(SwitchParser, 'b', false, {"binary"});
CmdSwitchBool AsmMain::Intel(SwitchParser, '\0', false, {"intel"});
CmdSwitchBool AsmMain::GAS(SwitchParser, '\0', false, {"gas"});
CmdSwitchInt AsmMain::ProcessorMode(SwitchParser, 's', 32, 0, 100, {"processor-mode"});
CmdSwitchBool AsmMain::WarningsAsErrors(SwitchParser, '\0', false, {"warningsaserrors"});
CmdSwitchBool AsmMain::NoGasDirectiveWarning(SwitchParser, '\0', false, {"nogasdirectivewarning"});
const char* AsmMain::helpText =
R"help([options] file
    
This program is an x86/x64 assembly language assembler.
It emulates NASM and GAS to a considerable extent.

@filename                          Use response file
/b, --binary                       Use binary output
/e, --preprocess-only              Preprocess only
/i, --case-insensitive             Case Insensitive Labels
/l[m], --listing                   Listing file [macro expansions]
/oxxx, --output-file               Set output file name
/s:xxx, --processor-mode           Set processor mode (16,32,64)
/Dxxx                              Define something
/Ixxx, --include-path              Set include file path
/V, --version                      Show version and date
/!, --nologo                       Don't show logo
/?, --help                         This text
--intel                            Use intel syntax
--gas                              Use extended AT&T syntax
--warningsaserrors                 Warnings should be generated as errors
--nogasdirectivewarning            Don't show warnings for missing GAS directives
    
)help"
"Time: " __TIME__ "  Date: " __DATE__;
const char* AsmMain::usageText = "[options] file";

int main(int argc, char* argv[])
MAINTRY
{
    AsmMain rc;
    try
    {
        return rc.Run(argc, argv);
    }
    catch (std::runtime_error* e)
    {
        std::cout << e->what() << std::endl;
    }
    catch (std::ios_base::failure)
    {
        Utils::Fatal("Fatal Error...");
    }
    return 1;
}
MAINCATCH
void AsmMain::CheckAssign(std::string& line, PreProcessor& pp)
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
                if (line.size() - 7 > npos && line.substr(npos, 6) == "assign" && isspace(line[npos + 6]))
                {
                    assign = true;
                }
                else if (line.size() - 8 > npos && line.substr(npos, 7) == "iassign" && isspace(line[npos + 7]))
                {
                    assign = true;
                    caseInsensitive = true;
                }
            }
            if (assign)
            {
                std::string name;
                int value = 0;
                npos = line.find_first_not_of(" \t\r\b\v", npos + 6 + (caseInsensitive ? 1 : 0));
                if (npos == std::string::npos || !Tokenizer::IsSymbolChar(line.c_str() + npos, true))
                {
                    Errors::Error("Expected identifier");
                }
                else
                {
                    int npos1 = npos;

                    while (npos1 != line.size() && Tokenizer::IsSymbolChar(line.c_str() + npos1, false))
                    {
                        int n = UTF8::CharSpan(line.c_str() + npos);
                        while (n && npos1 < line.size())
                            npos1++, n--;
                    }
                    name = line.substr(npos, npos1 - npos);
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
                            ppExpr e(false, Dialect::oasm);
                            std::string temp = line.substr(npos);
                            value = e.Eval(temp);
                            pp.Assign(name, value, caseInsensitive);
                        }
                    }
                }
                line = "";
            }
        }
    }
}

int AsmMain::Run(int argc, char* argv[])
{
    int rv = 0;
    auto files = ToolChain::StandardToolStartup(SwitchParser, argc, argv, usageText, helpText);
    if (files.size() < 2)
        ToolChain::Usage(usageText);
    Errors::WarningsAsErrors(WarningsAsErrors.GetValue());
    if (files.size() > 2 && OutputFile.GetValue().size())
        Utils::Fatal("Cannot specify output file for multiple input files");
    if (!InstructionParser::SetProcessorMode(ProcessorMode.GetValue()))
        Utils::Fatal("Invalid processor mode");
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
            srchPth = includePath.GetValue().substr(n + 1);
        }
    }
    char* cpath = getenv("CPATH");
    if (cpath)
    {
        if (srchPth.size())
            srchPth += ";";
        srchPth += cpath;
    }
    for (int i = 1; i < files.size(); i++)
    {
        std::string inName = files[i];
        int npos = inName.find_last_of(".");
        if (npos == std::string::npos || (npos && inName[npos - 1] == '.') ||
            (npos != inName.size() - 1 && inName[npos + 1] == CmdFiles::DIR_SEP[0]))
        {
            inName = Utils::QualifiedFile(files[i].c_str(), ".asm");
        }
        PreProcessor pp(inName, srchPth, sysSrchPth, false, false, '%', false, Dialect::oasm, true, false, "");
        int n = Defines.GetCount();
        for (int i = 0; i < n; i++)
        {
            CmdSwitchDefine::define* v = Defines.GetValue(i);
            pp.Define(v->name, v->value, false);
        }
        std::string outName;
        if (OutputFile.GetValue().size() != 0)
            outName = OutputFile.GetValue();
        else
            outName = Utils::QualifiedFile(files[i].c_str(), ".o");
        if (PreprocessOnly.GetValue())
        {
            std::string working = Utils::QualifiedFile(files[i].c_str(), ".i");
            std::fstream out(working.c_str(), std::ios::out);
            if (!out.is_open())
            {
                Utils::Fatal("Cannot open '%s' for write", working.c_str());
            }
            else
            {
                while (pp.GetLine(working))
                {
                    CheckAssign(working, pp);
                    out << working.c_str() << std::endl;
                }
            }
            out.close();
        }
        else
        {
            Listing listing;
            AsmFile asmFile(pp, CaseInsensitive.GetValue(), BinaryOutput.GetValue(), listing, GAS.GetValue(),
                            NoGasDirectiveWarning.GetValue());
            if (asmFile.Read())
            {
                if (!asmFile.Write(outName, inName) || Errors::GetErrorCount())
                {
                    rv = 1;
                }
                else if (CreateListFile.GetValue())
                {
                    std::string listingName = Utils::QualifiedFile(files[i].c_str(), ".lst");
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
            Errors::ErrorCount();
        }
    }
    return rv;
}
