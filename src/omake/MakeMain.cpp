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
#define _CRT_SECURE_NO_WARNINGS  

#include "MakeMain.h"
#include "Include.h"
#include "Maker.h"
#include "Utils.h"
#include "Parser.h"
#include "Eval.h"
#include "CmdFiles.h"
#include <ctype.h>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <stdlib.h>
#include <ios>
#include <stdio.h>
#ifdef OPENWATCOM
namespace std
{
    ios& right(ios&in) { return in; }
}
#endif
CmdSwitchParser MakeMain::switchParser;
CmdSwitchString MakeMain::specifiedFiles(switchParser, 'f', ' ');
CmdSwitchBool MakeMain::displayOnly(switchParser, 'n');
CmdSwitchBool MakeMain::touch(switchParser, 't');
CmdSwitchBool MakeMain::query(switchParser, 'q');
CmdSwitchBool MakeMain::keepGoing(switchParser, 'k');
CmdSwitchBool MakeMain::ignoreErrors(switchParser, 'i');	
CmdSwitchDefine MakeMain::defines(switchParser, 'D');
CmdSwitchBool MakeMain::rebuild(switchParser, 'B');
CmdSwitchString MakeMain::newFiles(switchParser, 'W', ' ');
CmdSwitchString MakeMain::oldFiles(switchParser, 'o', ' ');
CmdSwitchString MakeMain::dir(switchParser, 'C', '+');
CmdSwitchBool MakeMain::debug(switchParser, 'd');	// not implemented
CmdSwitchBool MakeMain::environOverride(switchParser, 'e');
CmdSwitchBool MakeMain::help(switchParser, 'h');
CmdSwitchBool MakeMain::help2(switchParser, '?');
CmdSwitchString MakeMain::includes(switchParser, 'I', ';');
CmdSwitchBool MakeMain::showDatabase(switchParser, 'p');
CmdSwitchBool MakeMain::noBuiltinRules(switchParser, 'r');
CmdSwitchBool MakeMain::noBuiltinVars(switchParser, 'R');
CmdSwitchBool MakeMain::silent(switchParser, 's');
CmdSwitchBool MakeMain::cancelKeep(switchParser, 'S');
CmdSwitchBool MakeMain::printDir(switchParser, 'w');
CmdSwitchBool MakeMain::warnUndef(switchParser, 'u');
CmdSwitchBool MakeMain::treeBuild(switchParser, 'T');

char *MakeMain::usageText = "[options] goals\n"
                    "\n"
                    "/B    Rebuild all             /C    Set directory\n"
                    "/Dxxx Define something        /Ixxx Set include path\n"
                    "/R    Ignore builtin vars     /S    Cancel keepgoing\n"
                    "/T    Tree Build              /Wxxx WhatIf\n"
                    "/d    Reserved                /e    Environment overrides\n"
                    "/fxxx Specify make file       /h    This text\n"
                    "/i    Ignore errors           /k    Keep going\n"
                    "/n    Display only            /oxxx Specify old goals\n"
                    "/p    Print database          /q    Query\n"
                    "/r    Ignore builtin rules    /s    Don't print commands\n"
                    "/t    Touch                   /u    Debug warnings\n"
                    "/w    Print directory\n"
                    "\nTime: " __TIME__ "  Date: " __DATE__;
char *MakeMain::builtinVars = "";
char *MakeMain::builtinRules = "";

int main(int argc, char **argv)
{
    MakeMain Main;
    return Main.Run(argc, argv);
}

void MakeMain::Dispatch(const char *data)
{
    int max = 10;
    argcx = 1;
    argvx = new char *[max];
    argvx[0] = "";
    while (*data)
    {
        data = GetStr(data);
        if (argcx == max)
        {
            max += 10;
            char **p = new char *[max];
            memcpy(p, argvx, argcx * sizeof(char *));
            delete [] argvx;
            argvx = p;
        }
    }
    argvx[argcx] = 0;
    switchParser.Parse(&argcx, argvx);
}
const char * MakeMain::GetStr(const char *data)
{
    char buf[2048], *p = buf;
    bool quote = false;
    while (isspace(*data))
        data++;
    if (*data == '"')
        quote= true, data++;
    while (*data && ((quote && *data != '"') || (!quote && !isspace(*data))))
        *p++ = *data++;
    *p = 0;
    if (quote && *data)
        data ++;
    char *x = new char [strlen(buf) + 1];
    strcpy(x,buf);
    argvx[argcx++] = x;
    return data;
}
void MakeMain::SetVariable(const std::string &name, const std::string &value, Variable::Origin origin, bool toExport)
{
    Variable *v = VariableContainer::Instance()->Lookup(name);
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
    if (includes.GetValue().size())
    {
        vals += "\"-I" +includes.GetValue() + "\"";
    }
    vals = vals + " $(MAKEOVERRIDES)";
    SetVariable("MAKEFLAGS", vals, Variable::o_command_line, true);
    vals = "";
    for (int i=0; i < defines.GetCount(); i++)
    {
        if (vals.size())
            vals += " ";
        const CmdSwitchDefine::define *def = defines.GetValue(i);
        vals += "\"/D" + def->name + "=" + def->value + "\"";
    }
    SetVariable("MAKEOVERRIDES", vals, Variable::o_command_line, true);
}
void MakeMain::LoadEnvironment()
{
    char **env = environ;
    Variable::Origin origin;
    if (environOverride.GetValue())
        origin = Variable::o_environ_override;
    else
        origin = Variable::o_environ;
    for (int i=0; env[i]; i++)
    {
        std::string name = env[i];
        size_t n = name.find_first_of('=', 1);
        if (n && n != std::string::npos) // ignore drive specifiers on win32
        {
            std::string value = name.substr(n + 1);
            name.replace(n, name.size(), "");
            SetVariable(name, value, origin, true);
        }
    }
}
void MakeMain::LoadCmdDefines()
{
    for (int i=0; i < defines.GetCount(); i++)
    {
        const CmdSwitchDefine::define *def = defines.GetValue(i);
        SetVariable(def->name, def->value, Variable::o_command_line, false);
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
void MakeMain::ShowRule(RuleList *ruleList)
{
    for (RuleList::iterator it = ruleList->begin(); it != ruleList->end(); ++it)
    {
        std::cout << ruleList->GetTarget().c_str() << (ruleList->GetDoubleColon() ? "::" : ":") << std::endl;
        std::cout << "\tPrerequisites:" << std::endl;
        std::cout << "\t\t" << (*it)->GetPrerequisites().c_str() << std::endl;
        std::cout << "\tOrder Prerequisites:" << std::endl;
        std::cout << "\t\t" << (*it)->GetOrderPrerequisites().c_str() << std::endl;
        std::cout << "\tCommands:" << std::endl;
        Command *commands = (*it)->GetCommands();
        for (Command::iterator it = commands->begin(); it != commands->end(); ++it)
        {
            std::cout << "\t\t" << (*it).c_str() << std::endl;
        }
    }
    std::cout << "\tTargetVariables:" << std::endl;
    for (RuleList::VariableIterator it = ruleList->VariableBegin(); it != ruleList->VariableEnd(); ++it)
    {
        std::cout << std::setw(25) << std::setfill(' ') << std::right << (*it->first).c_str();
        if (it->second->GetFlavor() == Variable::f_recursive)
            std::cout << " =  ";
        else
            std::cout << " := ";
        std::cout << it->second->GetValue().c_str() << std::endl;
    }
}
void MakeMain::ShowDatabase()
{
    std::cout << "Variables:" << std::endl;
    for (VariableContainer::iterator it = VariableContainer::Instance()->begin();
         it != VariableContainer::Instance()->end(); ++it)
    {
        std::cout << std::setw(25) << std::setfill(' ') << std::right << (*it->first).c_str();
        if (it->second->GetFlavor() == Variable::f_recursive)
            std::cout << " =  ";
        else
            std::cout << " := ";
        std::cout << it->second->GetValue().c_str() << std::endl;
    }
    std::cout << std::endl << "Explicit rules:" << std::endl;
    for (RuleContainer::iterator it = RuleContainer::Instance()->begin();
         it != RuleContainer::Instance()->end(); ++it)
    {
        ShowRule(it->second);
    }
    std::cout << std::endl << "Implicit rules:" << std::endl;
    for (RuleContainer::ImplicitIterator it = RuleContainer::Instance()->ImplicitBegin();
         it != RuleContainer::Instance()->ImplicitEnd(); ++it)
    {
        ShowRule(*it);
    }
}
void MakeMain::SetTreePath(std::string &files)
{
    // will get the working dir possibly modified by a 'C' command
    std::string wd = OS::GetWorkingDir() + CmdFiles::DIR_SEP;
    int pos = wd.find_last_of(CmdFiles::DIR_SEP);
    if (pos != std::string::npos)
    {
        bool found = false;
        std::string path;
        do {
            path = wd.substr(0, pos+1) + files;
            std::fstream check(path.c_str(), std::ios::in);
            if (check != NULL)
            {
                found = true;
            }
            else
            {
                pos = wd.find_last_of(CmdFiles::DIR_SEP);
                wd = wd.substr(0, pos);
                pos = wd.find_last_of(CmdFiles::DIR_SEP);
                if (pos == std::string::npos)
                    path = "";
            }
        } while (!found && pos != std::string::npos);
        if (path.size())
        {
            files = path;
            SetVariable("_TREEROOT", files, Variable::o_command_line, false);
            SetVariable("_TREETARGET", OS::GetWorkingDir(), Variable::o_command_line, false);
        }
    }
}
int MakeMain::Run(int argc, char **argv)
{
    char *p = getenv("MAKEFLAGS");
    if (p)
    {
        Variable *v = new Variable("MAKEFLAGS", p, Variable::f_recursive, Variable::o_environ);
        *VariableContainer::Instance() += v;
        p = getenv("MAKEOVERRIDES");
        if (p)
        {
            Variable *v = new Variable("MAKEOVERRIDES", p, Variable::f_recursive, Variable::o_environ);
            *VariableContainer::Instance() += v;
        }
        Eval r(v->GetValue(), false);
        std::string cmdLine = r.Evaluate();
        Dispatch(cmdLine.c_str());
    }
    if (!switchParser.Parse(&argc, argv) || help.GetValue() || help2.GetValue())
    {
        Utils::banner(argv[0]);
        Utils::usage(argv[0], usageText);
    }
    if (dir.GetValue().size())
    {
        cwd = OS::GetWorkingDir();
        if (!OS::SetWorkingDir(dir.GetValue()))
        {
            std::cout << "Cannot set working dir to: " << dir.GetValue().c_str() << std::endl;
            return 2;
        }
    }
    if (printDir.GetValue())
    {
        std::cout << OS::GetWorkingDir().c_str() << std::endl;
    }
#ifdef OPENWATCOM
    else
    {
        std::cout << std::endl;
    }
#endif
    if (cancelKeep.GetValue())
    {
        cancelKeep.SetValue(false);
        keepGoing.SetValue(false);
    }
    
    bool done = false;
    Eval::Init();
    Eval::SetWarnings(warnUndef.GetValue());
    while (!done && !Eval::GetErrCount())
    {
        VariableContainer::Instance()->Clear();
        RuleContainer::Instance()->Clear();
        Include::Instance()->Clear();
        Maker::ClearFirstGoal();
        LoadEnvironment();
        LoadCmdDefines();
        SetVariable("MAKE", argv[0], Variable::o_environ, false);
        Variable *v = VariableContainer::Instance()->Lookup("SHELL");
        if (!v)
        {
            v = VariableContainer::Instance()->Lookup("COMSPEC");
            if (!v)
                v = VariableContainer::Instance()->Lookup("ComSpec");
            if (v)
            {
                std::string val = v->GetValue();
                SetVariable("SHELL", val, Variable::o_environ, true);
            }
        }
        std::string goals;
        for (int i=1; i < argc; i++)
        {
            if (goals.size())
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
        v = VariableContainer::Instance()->Lookup("MAKE_LEVEL");
        if (!v)
        {
            SetVariable("MAKE_LEVEL", "0", Variable::o_environ, true);
        }
        else
        {
            int n = Utils::StringToNumber(v->GetValue());
            v->SetValue(Utils::NumberToString(n+1));
        }
        SetVariable(".FEATURES","second-expansion order-only target-specific", Variable::o_environ, false);
        SetVariable(".DEFAULT_GOAL","", Variable::o_environ, false);
        SetVariable(".INCLUDE_DIRS",includes.GetValue(), Variable::o_command_line, false);
        SetVariable("VPATH",includes.GetValue(), Variable::o_environ, false);
        SetInternalVars();		
        v = VariableContainer::Instance()->Lookup("MAKEFILES");
        if (v)
        {
            v->SetExport(true);
            Include::Instance()->AddFileList(v->GetValue(), true, true);
        }
        Rule *rule = new Rule(".SUFFIXES", ".c .o .cpp .nas .asm", "", new Command("", 0), "", 0, false);
        RuleList *ruleList = new RuleList(".SUFFIXES");
        ruleList->Add(rule, false);
        *RuleContainer::Instance() += ruleList;
        std::string files = specifiedFiles.GetValue();
        if (!files.size())
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
        done = !Include::Instance()->MakeMakefiles(silent.GetValue());
        if (!done)
            restarts++;
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
        Maker maker(xsilent, xdontrun, xignore, xtouch, rebuild.GetValue(), newFiles.GetValue(), oldFiles.GetValue());
        for (int i = 1; i < argc; i++)
        {
            maker.AddGoal(argv[i]);
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
    if (dir.GetValue().size())
    {
        OS::SetWorkingDir(cwd);
    }
    if (printDir.GetValue())
    {
        std::cout << OS::GetWorkingDir().c_str() << std::endl;
    }
#ifdef OPENWATCOM
    else
    {
        std::cout << std::endl;
    }
#endif
    return rv;
}