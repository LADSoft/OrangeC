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

#include "Spawner.h"
#include "Eval.h"
#include "Maker.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <deque>
#include "Variable.h"
#include "Utils.h"
#include <algorithm>
#include <chrono>
#ifdef GCCLINUX
#else
#include <windows.h>
#undef WriteConsole
#undef Yield
#endif
const char Spawner::escapeStart = '\x1';
const char Spawner::escapeEnd = '\x2';
long Spawner::runningProcesses;

unsigned WINFUNC Spawner::Thread(void* cls)
{
    Spawner* ths = (Spawner*)cls;
#ifdef GCCLINUX

    ++runningProcesses;
#else
    InterlockedIncrement(&runningProcesses);
#endif
    ths->RetVal(ths->InternalRun());
#ifdef GCCLINUX
    --runningProcesses;
#else
    InterlockedDecrement(&runningProcesses);
#endif
    ths->done = true;
    return 0;
}
void Spawner::WaitForDone()
{
    auto begin = std::chrono::system_clock::now();
    while (runningProcesses > 0 &&
           std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - begin).count() < 30)
    {
        OS::Yield();
    }
    if (runningProcesses > 0)
        std::cout << "omake: aborting due to timeout" << std::endl;
}
void Spawner::Run(Command& Commands, OutputType Type, RuleList* RuleListx, Rule* Rulex)
{
    commands = &Commands;
    outputType = Type;
    ruleList = RuleListx;
    rule = Rulex;
    if (OS::JobCount() == 1)
    {
        // make sure we go in order if this isn't a parallel job...
        Thread(this);
    }
    else
    {
        OS::CreateThread((void*)&Spawner::Thread, this);
    }
}
int Spawner::InternalRun()
{
    std::string shell;
    Variable* v = VariableContainer::Instance()->Lookup("SHELL");
    if (v)
    {
        shell = v->GetValue();
    }
    int rv = 0;
    std::string longstr;
    std::deque<std::string> tempFiles;
    bool make = false;
    for (auto it = commands->begin(); it != commands->end() && (!rv || !posix); ++it)
    {
        bool curSilent = silent;
        bool curIgnore = ignoreErrors;
        bool curDontRun = dontRun;
        const std::string& a = (*it);
        if (a.find("$(MAKE)") != std::string::npos || a.find("${MAKE}") != std::string::npos)
        {
            make = true;
            curDontRun = false;
        }
        OS::Take();
        std::string cmd = a;
        Eval c(cmd, false, ruleList, rule);
        cmd = c.Evaluate();  // deferred evaluation
        int i;
        for (i = 0; i < cmd.size(); i++)
            if (cmd[i] == '+')
                curDontRun = false;
            else if (cmd[i] == '@')
                curSilent = true;
            else if (cmd[i] == '-')
                curIgnore = true;
            else
                break;
        cmd = cmd.substr(i);
        size_t n = cmd.find("&&");
        std::string makeName;
        if (shell != "/bin/sh" && n != std::string::npos && n == cmd.size() - 3)
        {
            char match = cmd[n + 2];
            cmd.erase(n);
            makeName = "maketemp.";
            if (tempNum < 10)
                makeName = makeName + "00" + Utils::NumberToString(tempNum);
            else if (tempNum < 100)
                makeName = makeName + "0" + Utils::NumberToString(tempNum);
            else
                makeName = makeName + Utils::NumberToString(tempNum);
            tempNum++;
            if (!keepResponseFiles && !makeName.empty())
                tempFiles.push_back(makeName);
            std::fstream fil(makeName, std::ios::out);
            bool done = false;
            std::string tail;
            do
            {
                ++it;
                std::string current = *it;
                size_t n = current.find(match);
                if (n != std::string::npos)
                {
                    done = true;
                    if (n + 1 < current.size())
                        tail = current.substr(n + 1);
                    current.erase(n);
                }
                Eval ce(current, false, ruleList, rule);
                fil << ce.Evaluate() << std::endl;
            } while (!done);
            fil.close();
            cmd += makeName + tail;
        }
        cmd = QualifyFiles(cmd);
        OS::Give();
        if (oneShell)
        {
            longstr += cmd;
        }
        else
        {
            int rv1 = Run(cmd, curIgnore, curSilent, curDontRun, make);
            make = false;
            if (!rv)
                rv = rv1;
        }
        if (outputType == o_line)
            OS::ToConsole(output);
        if (curIgnore)
            rv = 0;
        if (rv && posix)
            break;
    }
    if (oneShell)
        rv = Run(longstr, ignoreErrors, silent, dontRun, false);
    OS::ToConsole(output);
    for (auto f : tempFiles)
        OS::RemoveFile(f);
    return rv;
}
int Spawner::Run(const std::string& cmdin, bool ignoreErrors, bool silent, bool dontrun, bool make)
{

    std::string cmd = cmdin;
    Variable* v = VariableContainer::Instance()->Lookup("SHELL");
    if (v)
    {
        std::string shell = v->GetValue();
        if (shell != "/bin/sh")
        {
            cmd = OS::NormalizeFileName(cmdin);
        }
    }
    if (oneShell)
    {
        if (!make)
            OS::TakeJob();
        int rv = OS::Spawn(cmd, environment, nullptr);
        if (!make)
            OS::GiveJob();
        return rv;
    }
    else if (!split(cmd))
    {
        Eval::error(std::string("Command line too long:\n") + cmd);
        return 0xff;
    }
    else
    {
        int rv = 0;
        for (auto command : cmdList)
        {
            bool make1 = make;
            //            if (command.find("omake") != std::string::npos)
            //                make1 = true;
            if (!make1)
                OS::TakeJob();
            if (!silent)
                OS::WriteConsole(std::string("\t") + command + "\n");
            int rv1;
            if (!dontrun)
            {
                std::string str;
                rv1 = OS::Spawn(command, environment, outputType != o_none && (outputType != o_recurse || !make) ? &str : nullptr);
                if (outputType != o_none && !str.empty())
                    output.push_back(str);
                if (!rv)
                    rv = rv1;
            }
            if (!make1)
                OS::GiveJob();
            if (rv && posix)
                return rv;
        }
        return rv;
    }
}
bool Spawner::split(const std::string& cmd)
{
    bool rv = true;
    cmdList.clear();
    int n = cmd.find_first_of(escapeStart);
    if (n != std::string::npos)
    {
        int m = cmd.find_first_of(escapeEnd);
        std::string first = cmd.substr(0, n);
        std::string last = cmd.substr(m);
        std::string middle = cmd.substr(n, m);
        int z = middle.find_first_of(escapeStart);
        if (z != std::string::npos)
            rv = cmd.size() < lineLength;
        z = last.find_first_of(escapeStart);
        if (z != std::string::npos)
            rv = cmd.size() < lineLength;

        int sz = first.size() + last.size();
        int szmiddle = lineLength - sz;
        while (middle.size() >= szmiddle)
        {
            std::string p = middle.substr(0, szmiddle);
            int lsp = middle.find_last_of(' ');
            if (lsp != std::string::npos)
            {
                p.replace(lsp, p.size() - lsp, "");
                middle.replace(0, lsp + 1, "");
                cmdList.push_back(first + p + last);
            }
            else
            {
                rv = false;
                break;
            }
        }
        if (!middle.empty())
        {
            cmdList.push_back(first + middle + last);
        }
    }
    else
    {
        rv = cmd.size() < lineLength;
        cmdList.push_back(cmd);
    }
    return rv;
}
std::string Spawner::shell(const std::string& cmd)
{
    std::string rv = OS::SpawnWithRedirect(cmd);
    int n = rv.size();
    while (n && (rv[n - 1] == '\r' || rv[n - 1] == '\n'))
        n--;
    rv = rv.substr(0, n);
    std::replace(rv.begin(), rv.end(), '\r', ' ');
    std::replace(rv.begin(), rv.end(), '\n', ' ');
    return rv;
}
std::string Spawner::QualifyFiles(const std::string& cmd)
{
    std::string rv;
    std::string working = cmd;
    while (!working.empty())
    {
        std::string cur = Eval::ExtractFirst(working, " ");
        cur = Maker::GetFullName(cur);
        if (!rv.empty())
            rv += " ";
        rv += cur;
    }
    return rv;
}
