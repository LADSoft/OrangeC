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
#include "ToolChain.h"
#include "CmdFiles.h"
#include "PreProcessor.h"
#include "Errors.h"
#include <cstdlib>
#include <algorithm>

#ifdef TARGET_OS_WINDOWS
extern "C"
{
    char* __stdcall GetModuleFileNameA(void* handle, char* buf, int size);
}
#endif

//#define TESTANNOTATE
CmdSwitchParser ppMain::SwitchParser;
CmdSwitchBool ppMain::NoLogo(SwitchParser, '!', false, {"nologo"});
CmdSwitchBool ppMain::ShowVersion(SwitchParser, 'v', false, {"version"});
CmdSwitchBool ppMain::ShowHelp(SwitchParser, '?', false, {"help"});
CmdSwitchFile ppMain::File(SwitchParser, '@');
CmdSwitchBool ppMain::assembly(SwitchParser, 'a', false);
CmdSwitchBool ppMain::disableExtensions(SwitchParser, 'A', false);
CmdSwitchBool ppMain::c99Mode(SwitchParser, '9', true);
CmdSwitchBool ppMain::c11Mode(SwitchParser, '1', false);
CmdSwitchBool ppMain::c2xMode(SwitchParser, '2', false);
CmdSwitchBool ppMain::trigraphs(SwitchParser, 'T', false);
CmdSwitchDefine ppMain::defines(SwitchParser, 'D');
CmdSwitchDefine ppMain::undefines(SwitchParser, 'U');
CmdSwitchString ppMain::includePath(SwitchParser, 'I', ';');
CmdSwitchString ppMain::CsysIncludePath(SwitchParser, 'z', ';');
CmdSwitchString ppMain::CPPsysIncludePath(SwitchParser, 'Z', ';');
CmdSwitchString ppMain::errorMax(SwitchParser, 'E');
CmdSwitchString ppMain::outputPath(SwitchParser, 'o');

CmdSwitchBool ppMain::MakeStubs(SwitchParser, 0, 0, {"M"});
CmdSwitchBool ppMain::MakeStubsUser(SwitchParser, 0, 0, {"MM"});
CmdSwitchCombineString ppMain::MakeStubsOutputFile(SwitchParser, 0, ';', {"MF"});
CmdSwitchBool ppMain::MakeStubsMissingHeaders(SwitchParser, 0, 0, {"MG"});
CmdSwitchBool ppMain::MakeStubsPhonyTargets(SwitchParser, 0, 0, {"MP"});
CmdSwitchCombineString ppMain::MakeStubsTargets(SwitchParser, 0, ';', {"MT"});
CmdSwitchCombineString ppMain::MakeStubsQuotedTargets(SwitchParser, 0, ';', {"MQ"});
CmdSwitchBool ppMain::MakeStubsContinue(SwitchParser, 0, 0, {"MD"});
CmdSwitchBool ppMain::MakeStubsContinueUser(SwitchParser, 0, 0, {"MMD"});

const char* ppMain::helpText =
    "[options] files\n"
    "\n"
    "/1             - C11 mode                  /2          - C2x mode\n"
    "/9             - C99 mode                  /a          - Assembler mode\n"
    "/A             - Disable extensions        /Dxxx       - Define something\n"
    "/E[+]nn        - Max number of errors      /Ipath      - Specify include path\n"
    "/T             - translate trigraphs       /Uxxx       - Undefine something\n"
    "/V, --version  - Show version and date     /!,--nologo - No logo\n"
    "/oxxx          - set output file           /zxxx,/Zxxx - set system path\n"
    "/?, --help     - This text\n"
    "\nDependency generation:\n"
    "  /M             - basic generation\n"
    "  /MM            - basic generation, user files only\n"
    "  /MF file       - specify output file\n"
    "  /MG            - missing headers as dependencies\n"
    "  /MP            - add phony targets\n"
    "  /MT target     - add target\n"
    "  /MQ target     - add target, quote special characters\n"
    "  /MD            - basic generation and continue\n"
    "  /MMD           - basic generation and continue, user files only\n"
    "Time: " __TIME__ "  Date: " __DATE__;
const char* ppMain::usageText = "[options] files";

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
    char buffer[256];
#ifdef TARGET_OS_WINDOWS
    GetModuleFileNameA(nullptr, buffer, sizeof(buffer));
#else
    strcpy(buffer, argv[0]);
#endif
    auto files = ToolChain::StandardToolStartup(SwitchParser, argc, argv, usageText, helpText,
                                                [this]() {
        return !MakeStubs.GetValue() && !MakeStubsUser.GetValue() && !MakeStubsContinue.GetValue() &&
               !MakeStubsContinueUser.GetValue();
        }
    );
    if (files.size() < 2)
        ToolChain::Usage(usageText);

    Tokenizer::SetAnsi(disableExtensions.GetValue());
    Tokenizer::SetC99(c99Mode.GetValue() || c11Mode.GetValue() || c2xMode.GetValue());
    for (int i = 1; i < files.size(); i++)
    {
        bool cplusplus = false;
        static const std::list<std::string> cppExtensions = {".h", ".hh", ".hpp", ".hxx", ".hm", ".cpp", ".cxx", ".cc", ".c++"};
        for (auto& str : cppExtensions)
        {
            if (Utils::HasExt(files[i].c_str(), str.c_str()))
            {
                cplusplus = true;
                break;
            }
        }
        Dialect dialect;
        if (c2xMode.GetValue())
            dialect = Dialect::c2x;
        else if (c11Mode.GetValue())
            dialect = Dialect::c11;
        else if (c99Mode.GetValue())
            dialect = Dialect::c99;
        else
            dialect = Dialect::c89;
        PreProcessor pp(files[i], includePath.GetValue(), cplusplus ? CPPsysIncludePath.GetValue() : CsysIncludePath.GetValue(), false,
                        trigraphs.GetValue(), assembly.GetValue() ? '%' : '#', false, dialect,
                        !disableExtensions.GetValue(),
                        (MakeStubs.GetValue() || MakeStubsUser.GetValue()) && MakeStubsMissingHeaders.GetValue(), "");
        if (c2xMode.GetValue())
        {
            std::string ver = "202311L";
            pp.Define("__STDC_VERSION__", ver, true);
        }
        else if (c11Mode.GetValue())
        {
            std::string ver = "201112L";
            pp.Define("__STDC_VERSION__", ver, true);
        }
        else if (c99Mode.GetValue())
        {
            std::string ver = "199901L";
            pp.Define("__STDC_VERSION__", ver, true);
        }
        else
        {
            std::string ver = "199404L";
            pp.Define("__STDC_VERSION__", ver, true);
        }

        // for libcxx 10
#ifdef TARGET_OS_WINDOWS
        pp.Define("_WIN32", "1");
#endif
        pp.Define("__ORANGEC__", "1");
        pp.Define("__need_size_t", "1");
        pp.Define("__need_FILE", "1");
        pp.Define("__need_wint_t", "1");
        pp.Define("__need_malloc_and_calloc", "1");

        if (cplusplus)
        {
            pp.Define("__cplusplus", "201402");
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

        working = "";

        if (!outputPath.GetValue().empty() && !MakeStubsContinue.GetValue() && !MakeStubsContinueUser.GetValue())
            working = outputPath.GetValue();
        else if (getenv("OCC_LEGACY_OPTIONS"))
            working = Utils::QualifiedFile(files[i].c_str(), ".i");

        std::ostream* outstream = nullptr;
        if (!working.empty())
        {
            outstream = new std::fstream(working, std::ios::out);
            if (!outstream->good())
                Utils::Fatal("Cannot open '%s' for write", working.c_str());
        }
        else
        {
            outstream = &std::cout;
        }
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
                                        ppExpr e(false, dialect);
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
            if (!MakeStubs.GetValue() && !MakeStubsUser.GetValue() || (errorMax.GetExists() && errorMax.GetValue().empty()))
            {
                (*outstream) << "#line " << pp.GetErrLineNo() << " \"" << pp.GetErrFile() << "\"" << std::endl;
                (*outstream) << working << std::endl;
            }
        }
        if (MakeStubs.GetValue() || MakeStubsUser.GetValue() || MakeStubsContinue.GetValue() || MakeStubsContinueUser.GetValue())
        {
            std::string inFile;
            inFile = files[i];
            int end = inFile.find_last_of('/');
            if (end == std::string::npos)
                end = -1;
            int end1 = inFile.find_last_of('\\');
            if (end1 == std::string::npos)
                end1 = -1;
            if (end < end1)
                end = end1;
            inFile = inFile.substr(end + 1);

            std::string outFile;
            if (MakeStubs.GetValue() || MakeStubsUser.GetValue())
            {
                outFile = MakeStubsOutputFile.GetValue();
            }
            else if (!outputPath.GetValue().empty())
            {
                outFile = outputPath.GetValue();
            }
            else
            {
                outFile = inFile;
                end = outFile.find_last_of('.');
                if (end != std::string::npos)
                    outFile = outFile.substr(0, end);
                outFile += ".d";
            }
            ::MakeStubs stubber(pp, MakeStubsUser.GetValue() || MakeStubsContinueUser.GetValue(), MakeStubsPhonyTargets.GetValue(),
                                inFile, outFile, MakeStubsTargets.GetValue(), MakeStubsQuotedTargets.GetValue());
            stubber.Run(MakeStubs.GetValue() || MakeStubsUser.GetValue() ? outstream : nullptr);
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
