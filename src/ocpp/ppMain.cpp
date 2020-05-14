/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

#include "ppMain.h"
#include "Utils.h"
#include "CmdFiles.h"
#include "PreProcessor.h"
#include "Errors.h"
#include <cstdlib>
#include <algorithm>

//#define TESTANNOTATE
CmdSwitchParser ppMain::SwitchParser;
CmdSwitchBool ppMain::assembly(SwitchParser, 'a', false);
CmdSwitchBool ppMain::disableExtensions(SwitchParser, 'A', false);
CmdSwitchBool ppMain::c99Mode(SwitchParser, '9', true);
CmdSwitchBool ppMain::trigraphs(SwitchParser, 'T', false);
CmdSwitchDefine ppMain::defines(SwitchParser, 'D');
CmdSwitchDefine ppMain::undefines(SwitchParser, 'U');
CmdSwitchString ppMain::includePath(SwitchParser, 'I', ';');
CmdSwitchString ppMain::errorMax(SwitchParser, 'E');
CmdSwitchFile ppMain::File(SwitchParser, '@');
CmdSwitchString ppMain::outputPath(SwitchParser, 'o');

const char* ppMain::usageText =
    "[options] files\n"
    "\n"
    "/9             - C99 mode                  /a      - Assembler mode\n"
    "/A             - Disable extensions        /Dxxx   - Define something\n"
    "/E[+]nn        - Max number of errors      /Ipath  - Specify include path\n"
    "/T             - translate trigraphs       /Uxxx   - Undefine something\n"
    "/V, --version  - Show version and date     /!,--nologo - No logo\n"
    "/o             - set output file\n"
    "Time: " __TIME__ "  Date: " __DATE__;

int main(int argc, char* argv[])
{
    ppMain preproc;
    try
    {
        return preproc.Run(argc, argv);
    }
    catch (std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
    }
    catch (std::ios_base::failure)
    {
        Utils::fatal("Fatal Error...");
    }
    return 1;
}
#ifdef TESTANNOTATE
static void PutCharInfo(std::ostream* outStream, const std::string& line, const std::deque<ppDefine::TokenPos>& positions,
                        bool origLine)
{
    int pos = 0;
    (*outStream) << line << std::endl;
    for (auto&& position : positions)
    {
        int start, end;
        if (origLine)
        {
            start = position.origStart;
            end = position.origEnd;
        }
        else
        {
            start = position.newStart;
            end = position.newEnd;
        }
        while (pos++ < start)
        {
            (*outStream) << " ";
        }
        if (end == start + 1)
            (*outStream) << "!";
        else
        {
            (*outStream) << "^";
            while (pos++ < end - 1)
                (*outStream) << "-";
            (*outStream) << "^";
        }
    }
    (*outStream) << std::endl;
}
static void TestCharInfo(std::ostream* outStream, PreProcessor& pp, std::string& line)
{
    PutCharInfo(outStream, pp.GetOrigLine(), pp.TokenPositions(), true);
    PutCharInfo(outStream, line, pp.TokenPositions(), false);
}
#endif
int ppMain::Run(int argc, char* argv[])
{
    Utils::banner(argv[0]);
    Utils::SetEnvironmentToPathParent("ORANGEC");
    CmdSwitchFile internalConfig(SwitchParser);
    std::string configName = Utils::QualifiedFile(argv[0], ".cfg");
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
    Tokenizer::SetAnsi(disableExtensions.GetValue());
    Tokenizer::SetC99(c99Mode.GetValue());
    for (auto it = files.FileNameBegin(); it != files.FileNameEnd(); ++it)
    {
        PreProcessor pp((*it), srchPth, sysSrchPth, false, trigraphs.GetValue(), assembly.GetValue() ? '%' : '#', false,
                        !c99Mode.GetValue(), !disableExtensions.GetValue(), "");
        if (c99Mode.GetValue())
        {
            std::string ver = "199901L";
            pp.Define("__STDC_VERSION__", ver, true);
        }
        int n = defines.GetCount();
        int nu = undefines.GetCount();
        for (int i = 0, j = 0; i < n || j < nu;)
        {
            if (i < n && j < nu)
            {
                CmdSwitchDefine::define* v = defines.GetValue(i);
                CmdSwitchDefine::define* uv = undefines.GetValue(j);
                if (v->argnum < uv->argnum)
                {
                    pp.Define(v->name, v->value, false);
                    i++;
                }
                else
                {
                    pp.Undefine(uv->name);
                    j++;
                }
            }
            else if (i < n)
            {
                CmdSwitchDefine::define* v = defines.GetValue(i);
                pp.Define(v->name, v->value, false);
                i++;
            }
            else
            {
                CmdSwitchDefine::define* v = undefines.GetValue(i);
                pp.Undefine(v->name);
                j++;
            }
        }

        std::string working = errorMax.GetValue();
        if (!working.empty())
        {
            if (working[0] == '+')
            {
                Errors::SetShowTrivialWarnings(true);
                working.erase(0, 1);
            }
            n = Utils::StringToNumber(working);
            if (n)
            {
                Errors::SetMaxErrors(n);
            }
        }
        if (!outputPath.GetValue().empty())
            working = outputPath.GetValue();
        else if (getenv("OCC_LEGACY_OPTIONS"))
            working = Utils::QualifiedFile((*it).c_str(), ".i");

        std::ostream* outstream = nullptr;
        if (!working.empty())
            outstream = new std::fstream(working, std::ios::out);
        else
            outstream = &std::cout;
        while (pp.GetLine(working))
        {
            int last = 0;
#ifdef TESTANNOTATE
            TestCharInfo(outstream, pp, working);
#endif
            working.erase(std::remove(working.begin(), working.end(), ppDefine::MACRO_PLACEHOLDER), working.end());
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
                        {
                            if (working.size() - 7 > npos && working.substr(npos, 6) == "assign" && isspace(working[npos + 6]))
                            {
                                isAssign = true;
                            }
                            else if (working.size() - 8 > npos && working.substr(npos, 7) == "iassign" &&
                                     isspace(working[npos + 7]))
                            {
                                isAssign = true;
                                caseInsensitive = true;
                            }
                        }
                        if (isAssign)
                        {
                            std::string name;
                            PPINT value = 0;
                            npos = working.find_first_not_of(" \t\r\b\v", npos + 6 + (caseInsensitive ? 1 : 0));
                            if (npos == std::string::npos || !Tokenizer::IsSymbolChar(working.c_str() + npos, true))
                            {
                                Errors::Error("Expected identifier");
                            }
                            else
                            {
                                int npos1 = npos;

                                while (npos1 != working.size() && Tokenizer::IsSymbolChar(working.c_str() + npos1, false))
                                    npos1++;
                                name = working.substr(npos, npos1 - npos);
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
            (*outstream) << "#line " << pp.GetErrLineNo() << " \"" << pp.GetErrFile() << "\"" << std::endl;
            (*outstream) << working << std::endl;
        }
        if (outstream != &std::cout)
        {
            std::fstream* f = static_cast<std::fstream*>(outstream);
            f->close();
            delete f;
        }
    }
    return Errors::GetErrorCount() != 0;
}
