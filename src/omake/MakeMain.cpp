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

#define _CRT_SECURE_NO_WARNINGS

#include "MakeMain.h"
#include "Include.h"
#include "Maker.h"
#include "Utils.h"
#include "Parser.h"
#include "Eval.h"
#include "CmdFiles.h"
#include <cctype>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <ios>
#include <cstdio>
#include <algorithm>

CmdSwitchParser MakeMain::switchParser;
CmdSwitchCombineString MakeMain::specifiedFiles(switchParser, 'f', ' ', {"file"});
CmdSwitchBool MakeMain::displayOnly(switchParser, 'n', false, {"dry-run"});
CmdSwitchBool MakeMain::touch(switchParser, 't', false, {"touch"});
CmdSwitchBool MakeMain::query(switchParser, 'q', false, {"question"});
CmdSwitchBool MakeMain::keepGoing(switchParser, 'k', false, {"keep-going"});
CmdSwitchBool MakeMain::ignoreErrors(switchParser, 'i', false, {"ignore-errors"});
CmdSwitchDefine MakeMain::defines(switchParser, 'D', {"eval"});
CmdSwitchBool MakeMain::rebuild(switchParser, 'B', false, {"always-make"});
CmdSwitchCombineString MakeMain::newFiles(switchParser, 'W', ' ', {"assume-new"});
CmdSwitchCombineString MakeMain::oldFiles(switchParser, 'o', ' ', {"assume-old"});
CmdSwitchCombineString MakeMain::dir(switchParser, 'C', '+', {"directory"});
CmdSwitchBool MakeMain::debug(switchParser, 'd');  // not implemented
CmdSwitchBool MakeMain::environOverride(switchParser, 'e', false, {"environment-overrides"});
CmdSwitchBool MakeMain::help(switchParser, 'h');
CmdSwitchBool MakeMain::ShowHelp(switchParser, '?', false, {"help"});
CmdSwitchCombineString MakeMain::includes(switchParser, 'I', ';', {"include-dir"});
CmdSwitchBool MakeMain::showDatabase(switchParser, 'p', false, {"print-data-base"});
CmdSwitchBool MakeMain::noBuiltinRules(switchParser, 'r', false, {"no-builtin-rules"});
CmdSwitchBool MakeMain::noBuiltinVars(switchParser, 'R', false, {"no-builtin-variables"});
CmdSwitchBool MakeMain::silent(switchParser, 's', false, {"quiet"});
CmdSwitchBool MakeMain::cancelKeep(switchParser, 'S', false, {"no-keep-going"});
CmdSwitchBool MakeMain::printDir(switchParser, 'w', false, {"print-directory"});
CmdSwitchBool MakeMain::warnUndef(switchParser, 'u');
CmdSwitchBool MakeMain::treeBuild(switchParser, 'T');
CmdSwitchBool MakeMain::keepResponseFiles(switchParser, 'K');
CmdSwitchInt MakeMain::jobs(switchParser, 'j', INT_MAX, 1, INT_MAX);
CmdSwitchString MakeMain::jobServer(switchParser, 0, 0, {"jobserver-auth"});
CmdSwitchCombineString MakeMain::jobOutputMode(switchParser, 'O');

const char* MakeMain::helpText =
    "[options] goals\n"
    "\n"
    "/B    Rebuild all             /C    Set directory\n"
    "/Dxxx Define something        /Ixxx Set include path\n"
    "/K    Keep response files     /O    Set output mode\n"
    "/R    Ignore builtin vars     /S    Cancel keepgoing\n"
    "/T    Tree Build              /V    Show version and date\n"
    "/Wxxx WhatIf                  /d    Reserved\n"
    "/e    Environment overrides   /fxxx Specify make file\n"
    "/h    This text               /i    Ignore errors\n"
    "/j:xx Set number of jobs      /k    Keep going\n"
    "/n    Display only            /oxxx Specify old goals\n"
    "/p    Print database          /q    Query\n"
    "/r    Ignore builtin rules    /s    Don't print commands\n"
    "/t    Touch                   /u    Debug warnings\n"
    "/w    Print make status       --eval=STRING evaluate a statement\n"
    "/!    No logo                 /? or --help  this help\n"
    "--jobserver-auth=xxxx               Name a jobserver to use for getting jobs\n"
    "--version show version info\n"
    "\nTime: " __TIME__ "  Date: " __DATE__;
const char* MakeMain::builtinVars = "";
const char* MakeMain::builtinRules = "";

int MakeMain::makeLevel;

int main(int argc, char** argv)
{
    MakeMain Main;
    return Main.Run(argc, argv);
}

void MakeMain::Dispatch(const char* data)
{
    int max = 10;
    argcx = 1;
    argvx = std::make_unique<char*[]>(max + 1);
    argvx[0] = (char*)"";
    while (*data)
    {
        data = GetStr(data);
        if (argcx == max)
        {
            max += 10;
            std::unique_ptr<char*[]> p(argvx.release());
            argvx = std::make_unique<char*[]>(max + 1);
            memcpy(argvx.get(), p.get(), argcx * sizeof(char*));
        }
    }
    argvx[argcx] = 0;
    switchParser.Parse(&argcx, argvx.get());
    argvx.release();
}
const char* MakeMain::GetStr(const char* data)
{
    char buf[2048], *p = buf;
    bool quote = false;
    while (isspace(*data))
        data++;
    if (*data == '"')
        quote = true, data++;
    while (*data && ((quote && *data != '"') || (!quote && !isspace(*data))))
        *p++ = *data++;
    *p = 0;
    if (quote && *data)
        data++;
    char* x = new char[strlen(buf) + 1];
    strcpy(x, buf);
    argvx[argcx++] = x;
    return data;
}
void MakeMain::SetVariable(const std::string& name, const std::string& value, Variable::Origin origin, bool toExport)
{
    Variable* v = VariableContainer::Instance()->Lookup(name);
    if (v)
    {
        v->SetValue(value);
    }
    else
    {
        v = new Variable(name, value, Variable::f_recursive, origin);
        *VariableContainer::Instance() += v;
    }
    if (toExport)
        v->SetExport(true);
}
void MakeMain::SetMakeFlags()
{
    std::string vals = "-";
    if (displayOnly.GetValue())
    {
        vals += "n";
    }
    if (touch.GetValue())
    {
        vals += "t";
    }
    if (query.GetValue())
    {
        vals += "q";
    }
    if (keepGoing.GetValue())
    {
        vals += "k";
    }
    if (keepResponseFiles.GetValue())
    {
        vals += "K";
    }
    if (ignoreErrors.GetValue())
    {
        vals += "i";
    }
    if (rebuild.GetValue())
    {
        vals += "B";
    }
    if (debug.GetValue())
    {
        vals += "d";
    }
    if (environOverride.GetValue())
    {
        vals += "e";
    }
    if (showDatabase.GetValue())
    {
        vals += "p";
    }
    if (noBuiltinRules.GetValue())
    {
        vals += "r";
    }
    if (noBuiltinVars.GetValue())
    {
        vals += "R";
    }
    if (silent.GetValue())
    {
        vals += "s";
    }
    if (printDir.GetValue())
    {
        vals += "w";
    }
    if (warnUndef.GetValue())
    {
        vals += "u";
    }
    // not setting -T so we don't make it recursive
    if (vals == "-")
        vals = "";
    if (jobs.GetExists())
    {
        int n = jobs.GetValue();
        vals += std::string(" -j") + (n != INT_MAX ? (std::string(":") + std::to_string(n)) : "");
    }
    if (jobOutputMode.GetExists())
    {
        vals += std::string(" -O") + jobOutputMode.GetValue();
    }
    if (jobServer.GetExists())
    {
        vals += " --jobserver-auth=" + jobServer.GetValue();
    }
    if (!includes.GetValue().empty())
    {
        vals += "\"-I" + includes.GetValue() + "\"";
    }
    vals = vals + " $(MAKEOVERRIDES)";
    SetVariable("MAKEFLAGS", vals, Variable::o_command_line, true);
    vals = "";
    for (int i = 0; i < defines.GetCount(); i++)
    {
        if (!vals.empty())
            vals += " ";
        const CmdSwitchDefine::define* def = defines.GetValue(i);
        vals += "\"/D" + def->name + "=" + def->value + "\"";
    }
    SetVariable("MAKEOVERRIDES", vals, Variable::o_command_line, true);
}
bool MakeMain::LoadJobArgs()
{
    int jobCount = 1;
    if (jobs.GetExists())
    {
        jobCount = jobs.GetValue();
        if (jobCount == 0)
            return false;
        
    }
    else
    {
        jobs.SetValue(1);
    }
    OS::PushJobCount(jobCount);
    if (jobOutputMode.GetExists())
    {
        if (jobOutputMode.GetValue() == "none")
            outputType = o_none;
        else if (jobOutputMode.GetValue() == "line")
            outputType = o_line;
        else if (jobOutputMode.GetValue() == "target")
            outputType = o_target;
        else if (jobOutputMode.GetValue() == "recurse")
            outputType = o_recurse;
        else
            Utils::fatal((std::string("Unknown output mode: ") + jobOutputMode.GetValue()));
    }
    return true;
}
void MakeMain::LoadEnvironment()
{
#ifdef HAVE_UNISTD_H
    char** env = 0;
#else
    char** env = environ;
#endif
    Variable::Origin origin;
    if (environOverride.GetValue())
        origin = Variable::o_environ_override;
    else
        origin = Variable::o_environ;
    for (int i = 0; env && env[i]; i++)
    {
        std::string name = env[i];
        size_t n = name.find_first_of('=', 1);
        if (n && n != std::string::npos)  // ignore drive specifiers on win32
        {
            std::string value = name.substr(n + 1);
            name.replace(n, name.size(), "");
            SetVariable(name, value, origin, true);
        }
    }
}
void MakeMain::LoadCmdDefines()
{
    for (int i = 0; i < defines.GetCount(); i++)
    {
        const CmdSwitchDefine::define* def = defines.GetValue(i);
        Parser p(def->name + "=" + def->value, "<eval>", 1, false, Variable::o_default);
        p.SetAutoExport();
        p.Parse();
    }
}
void MakeMain::SetInternalVars()
{
    if (!noBuiltinVars.GetValue())
    {
        Parser p(builtinVars, "<builtins>", 1, false, Variable::o_default);
        p.Parse();
    }
}
void MakeMain::SetupImplicit()
{
    if (!noBuiltinVars.GetValue() && !noBuiltinRules.GetValue())
    {
        Parser p(builtinRules, "<builtins>", 1, false, Variable::o_default);
        p.Parse();
    }
}
void MakeMain::ShowRule(RuleList* ruleList)
{
    for (auto& rule : *ruleList)
    {
        std::cout << ruleList->GetTarget() << (ruleList->GetDoubleColon() ? "::" : ":") << std::endl;
        std::cout << "\tPrerequisites:" << std::endl;
        std::cout << "\t\t" << rule->GetPrerequisites() << std::endl;
        std::cout << "\tOrder Prerequisites:" << std::endl;
        std::cout << "\t\t" << rule->GetOrderPrerequisites() << std::endl;
        std::cout << "\tCommands:" << std::endl;
        Command* commands = rule->GetCommands();
        for (auto&& command : *rule->GetCommands())
        {
            std::cout << "\t\t" << command << std::endl;
        }
    }
    std::cout << "\tTargetVariables:" << std::endl;
    for (auto it = ruleList->VariableBegin(); it != ruleList->VariableEnd(); ++it)
    {
        std::cout << std::setw(25) << std::setfill(' ') << std::right << (it->first);
        if (it->second->GetFlavor() == Variable::f_recursive)
            std::cout << " =  ";
        else
            std::cout << " := ";
        std::cout << it->second->GetValue() << std::endl;
    }
    std::cout.clear();
}
void MakeMain::ShowDatabase()
{
    std::cout << "Variables:" << std::endl;
    for (auto& var : *VariableContainer::Instance())
    {
        std::cout << std::setw(25) << std::setfill(' ') << std::right << (var.first);
        if (var.second->GetFlavor() == Variable::f_recursive)
            std::cout << " =  ";
        else
            std::cout << " := ";
        std::cout << var.second->GetValue() << std::endl;
    }
    std::cout << std::endl << "Explicit rules:" << std::endl;
    for (auto& rule : *RuleContainer::Instance())
    {
        ShowRule(rule.second.get());
    }
    std::cout << std::endl << "Implicit rules:" << std::endl;
    for (auto it = RuleContainer::Instance()->ImplicitBegin(); it != RuleContainer::Instance()->ImplicitEnd(); ++it)
    {
        ShowRule((*it).get());
    }
    std::cout.clear();
}
void MakeMain::SetTreePath(std::string& files)
{
    // will get the working dir possibly modified by a 'C' command
    std::string wd = OS::GetWorkingDir() + CmdFiles::DIR_SEP;
    int pos = wd.find_last_of("/\\");
    if (pos != std::string::npos)
    {
        bool found = false;
        std::string path;
        do
        {
            path = wd.substr(0, pos + 1) + files;
            std::fstream check(path, std::ios::in);
            if (!check.fail())
            {
                found = true;
            }
            else
            {
                pos = wd.find_last_of("/\\");
                wd = wd.substr(0, pos);
                pos = wd.find_last_of("/\\");
                if (pos == std::string::npos)
                    path = "";
            }
        } while (!found && pos != std::string::npos);
        if (!path.empty())
        {
            files = path;
            SetVariable("_TREEROOT", files, Variable::o_command_line, false);
            SetVariable("_TREETARGET", OS::GetWorkingDir(), Variable::o_command_line, false);
        }
    }
}
void MakeMain::LoadEquates(int& argc, char** argv)
{
    int j = 1;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-' && argv[i][0] != '/' && strchr(argv[i], '=') != 0)
        {
            equates.push_back(argv[i]);
        }
        else
        {
            argv[j++] = argv[i];
        }
    }
    argc = j;
    argv[argc] = nullptr;
}
void MakeMain::RunEquates()
{
    for (auto&& e : equates)
    {
        Parser p(e, "<eval>", 1, false, Variable::o_default);
        p.SetAutoExport();
        p.Parse();
    }
}
int MakeMain::Run(int argc, char** argv)
{
    OS::Init();
    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "--version"))
        {
            Utils::banner(argv[0]);
            exit(0);
        }
    }
    Utils::SetEnvironmentToPathParent("ORANGEC");
    char* p = getenv("MAKEFLAGS");
    if (p)
    {
        Variable* v = new Variable("MAKEFLAGS", p, Variable::f_recursive, Variable::o_environ);
        *VariableContainer::Instance() += v;
        p = getenv("MAKEDOOVERIDES");
        if (p)
        {
            Variable* v = new Variable("MAKEDOOVERIDES", p, Variable::f_recursive, Variable::o_environ);
            *VariableContainer::Instance() += v;
        }
        Eval r(v->GetValue(), false);
        std::string cmdLine = r.Evaluate();
        Dispatch(cmdLine.c_str());
    }
    LoadEquates(argc, argv);
    if (!switchParser.Parse(&argc, argv) || help.GetValue() || ShowHelp.GetValue())
    {
        Utils::banner(argv[0]);
        Utils::usage(argv[0], helpText);
    }
    char* cpath = getenv("CPATH");
    if (cpath)
    {
        if (!includes.GetValue().empty())
            includes += ";";
        includes += cpath;
    }
    if (!dir.GetValue().empty())
    {
        cwd = OS::GetWorkingDir();
        if (!OS::SetWorkingDir(dir.GetValue()))
        {
            std::cout << "Cannot set working dir to: " << dir.GetValue() << std::endl;
            return 2;
        }
    }

    if (cancelKeep.GetValue())
    {
        cancelKeep.SetValue(false);
        keepGoing.SetValue(false);
    }

    if (!LoadJobArgs())
        Utils::usage(argv[0], helpText);

    bool done = false;
    Eval::SetWarnings(warnUndef.GetValue());
    while (!done && !Eval::GetErrCount())
    {
        VariableContainer::Instance()->Clear();
        RuleContainer::Instance()->Clear();
        Include::Instance()->Clear();
        Maker::ClearFirstGoal();
        LoadEnvironment();
        LoadCmdDefines();
        RunEquates();
        OS::InitJobServer(); 
        SetVariable("MAKE", argv[0], Variable::o_environ, false);
        Variable* v = VariableContainer::Instance()->Lookup("SHELL");
        if (!v)
        {
            v = VariableContainer::Instance()->Lookup("MSYSCON");  // detect MSYS version of comspec
            if (!v)
                v = VariableContainer::Instance()->Lookup("COMSPEC");
            if (!v)
                v = VariableContainer::Instance()->Lookup("ComSpec");
            if (v)
            {
                std::string val = v->GetValue();
                SetVariable("SHELL", val, Variable::o_environ, false);
                SetVariable(".SHELLFLAGS", "-c", Variable::o_environ, false);
            }
        }

        std::string wd = OS::GetWorkingDir();
        SetVariable("CURDIR", wd, Variable::o_environ, false);
        std::string goals;
        for (int i = 1; i < argc; i++)
        {
            if (!goals.empty())
                goals += " ";
            goals += argv[i];
        }
        SetVariable("MAKECMDGOALS", goals, Variable::o_command_line, false);
        SetMakeFlags();
        if (restarts)
        {
            std::string temp = Utils::NumberToString(restarts);
            SetVariable("MAKE_RESTARTS", temp, Variable::o_command_line, false);
        }
        bool firstMakeLevel = makeLevel == 0;
        v = VariableContainer::Instance()->Lookup("MAKE_LEVEL");
        if (!v)
        {
            SetVariable("MAKE_LEVEL", "0", Variable::o_environ, true);
            makeLevel = 1;
        }
        else
        {
            int n = Utils::StringToNumber(v->GetValue());
            v->SetValue(Utils::NumberToString(n + 1));
            makeLevel = n + 2;
        }
        if (firstMakeLevel)
            MakeMessage("Entering %s", OS::GetWorkingDir().c_str());

        SetVariable(".FEATURES", "second-expansion order-only target-specific", Variable::o_environ, false);
        SetVariable(".DEFAULT_GOAL", "", Variable::o_environ, false);
        SetVariable(".INCLUDE_DIRS", includes.GetValue(), Variable::o_command_line, false);
        SetVariable("VPATH", includes.GetValue(), Variable::o_environ, false);
        SetInternalVars();
        v = VariableContainer::Instance()->Lookup("MAKEFILES");
        if (v)
        {
            v->SetExport(true);
            Include::Instance()->AddFileList(v->GetValue(), true, true);
        }
        Rule* rule = new Rule(".SUFFIXES", ".c .o .cpp .nas .asm .s", "", new Command("", 0), "", 0, false);
        RuleList* ruleList = new RuleList(".SUFFIXES");
        ruleList->Add(rule, false);
        *RuleContainer::Instance() += ruleList;
        std::string files = specifiedFiles.GetValue();
        if (files.empty())
        {
            if (treeBuild.GetValue())
                files = "treetop.mak";
            else
                files = "makefile";
        }
        if (treeBuild.GetValue())
            SetTreePath(files);
        Include::Instance()->AddFileList(files, false, false);
        SetupImplicit();
        RuleContainer::Instance()->SecondaryEval();
        bool didSomething = false;
        done = !Include::Instance()->MakeMakefiles(silent.GetValue(), outputType, didSomething);
        if (!didSomething)
        {
            MakeMessage("Nothing to be done for '%s'", goals.size() ? goals.c_str() : "all");
        }
        if (!done)
            restarts++;
    }
    // now low for .MAKEFLAGS || .MFLAGS
    RuleList* rl = nullptr;
    if ((rl = RuleContainer::Instance()->Lookup(".MAKEFLAGS")) || (rl = RuleContainer::Instance()->Lookup(".MFLAGS")))
    {
        for (auto& rule : *rl)
        {
            std::string v = rule->GetPrerequisites();
            Eval r(v, false);
            std::string cmdLine = r.Evaluate();
            Dispatch(cmdLine.c_str());
        }
        SetMakeFlags();
    }
    if (showDatabase.GetValue())
        ShowDatabase();
    int rv = 0;
    if (Eval::GetErrCount())
    {
        rv = 2;
    }
    else
    {
        bool xsilent = silent.GetValue();
        bool xignore = ignoreErrors.GetValue();
        bool xdontrun = displayOnly.GetValue();
        bool xtouch = touch.GetValue();
        xdontrun |= xtouch || query.GetValue();
        xsilent |= xtouch || query.GetValue();
        if (RuleContainer::Instance()->Lookup(".BEGIN") || RuleContainer::Instance()->Lookup(".END"))
            Eval::warning(".BEGIN and .END special targets ignored");
        if (RuleContainer::Instance()->Lookup(".INCLUDES") || RuleContainer::Instance()->Lookup(".LIBS"))
            Eval::warning(".INCLUDES and .LIBS special targets ignored");
        if (RuleContainer::Instance()->Lookup(".INTERRUPT"))
            Eval::warning(".INTERRUPT special target ignored");
        if (RuleContainer::Instance()->Lookup(".MAKEFILEDPES"))
            Eval::warning(".MAKEFILEDEPS special target ignored");
        if (RuleContainer::Instance()->Lookup(".ORDER"))
            Eval::warning(".ORDER special target ignored");
        if (RuleContainer::Instance()->Lookup(".SHELL"))
            Eval::warning(".SHELL special target ignored");
        if (RuleContainer::Instance()->Lookup(".WARN"))
            Eval::warning(".WARN special target ignored");
        Maker maker(xsilent, xdontrun, xignore, xtouch, outputType, rebuild.GetValue(), keepResponseFiles.GetValue(),
                    newFiles.GetValue(), oldFiles.GetValue());
        if (argc <= 1)
        {
            auto r = RuleContainer::Instance()->Lookup(".MAIN");
            if (r)
            {
                for (auto& r1 : *r)
                {
                    auto p = r1->GetPrerequisites();
                    while (!p.empty())
                    {
                        auto s = Eval::ExtractFirst(p, " ");
                        if (!s.empty())
                        {
                            maker.AddGoal(s);
                        }
                    }
                }
            }
        }
        else
        {
            for (int i = 1; i < argc; i++)
            {
                maker.AddGoal(argv[i]);
            }
        }
        if (maker.CreateDependencyTree())
        {
            rv = maker.RunCommands(keepGoing.GetValue());
            if (query.GetValue() && rv == 0)
                rv = maker.HasCommands() ? 1 : 0;
        }
        else
        {
            rv = 2;
        }
    }

    MakeMessage("Leaving %s", OS::GetWorkingDir().c_str());

    if (!dir.GetValue().empty())
    {
        OS::SetWorkingDir(cwd);
    }
    return rv;
}
