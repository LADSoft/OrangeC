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

#include "Spawner.h"
#include "Eval.h"
#include "Maker.h"
#include "MakeMain.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <deque>
#include "Variable.h"
#include "Utils.h"
#include <algorithm>
#include <chrono>
#include <future>
#include <memory>
#include <mutex>
#include "semaphores.h"
#ifdef TARGET_OS_WINDOWS
#    include <windows.h>
#    undef Yield
#endif
class OSTakeJobIfNotMake
{
    static const int MaxOmakeInstances = 2;
    bool take_job = false;
    static Semaphore sem;
    static int initted;
  public:
    OSTakeJobIfNotMake(std::string& cmd, bool take_job = false) : take_job(take_job)
    {
        if (take_job)
        {
            OS::TakeJob();
        }
        else
        {
            if (initted++ == 0)
            {
                sem.Post(MaxOmakeInstances);
            }
            sem.Wait();
        }
    }
    ~OSTakeJobIfNotMake()
    {
        if (take_job)
        {
            OS::GiveJob();
        }
        else
        {
            sem.Post();
        }
    }
};
Semaphore OSTakeJobIfNotMake::sem(MaxOmakeInstances);
int OSTakeJobIfNotMake::initted;
const char Spawner::escapeStart = '\x1';
const char Spawner::escapeEnd = '\x2';
bool Spawner::stopAll;

void Spawner::Run(std::shared_ptr<Command>& Commands, OutputType Type, std::shared_ptr<RuleList> RuleListx,
                  std::shared_ptr<Rule> Rulex)
{
    commands = Commands;
    outputType = Type;
    ruleList = RuleListx;
    rule = Rulex;
    retVal = InternalRun();
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
        std::string cmd = a;
        {
            std::lock_guard<decltype(OS::GetConsoleMutex())> lck(OS::GetConsoleMutex());
            Eval c(cmd, false, ruleList, rule);
            cmd = c.Evaluate();  // deferred evaluation
            size_t i;
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
            if (shell != "/bin/sh")
            {
                if (n != std::string::npos && n == cmd.size() - 3)
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
            }
        }
        if (oneShell)
        {
            longstr += cmd;
        }
        else
        {
            int rv1 = Run(cmd, curIgnore, curSilent, curDontRun, make);
            if (stopAll)
                break;
            make = false;
            if (!rv)
            {
                if (rv1)
                {
                    MakeMain::MakeMessage("Commands returned %d in '%s(%d)'%s", rv1, ruleList->GetTarget().c_str(),
                                          commands->GetLine(), curIgnore ? " (Ignored)" : "");
                }
                rv = rv1;
            }
        }
        if (outputType == o_line && !stopAll)
            OS::ToConsole(output);
        if (curIgnore)
            rv = 0;
        if (rv && posix)
            break;
    }
    if (oneShell && !stopAll)
    {
        rv = Run(longstr, ignoreErrors, silent, dontRun, false);
        if (rv)
            MakeMain::MakeMessage("Commands returned %d in '%s(%d)'%s", rv, ruleList->GetTarget().c_str(), commands->GetLine(),
                                  ignoreErrors ? " (Ignored)" : "");
    }
    if (!stopAll)
        OS::ToConsole(output);
    for (auto&& f : tempFiles)
        OS::RemoveFile(f);
    return rv;
}
int Spawner::Run(const std::string& cmdin, bool ignoreErrors, bool silent, bool dontrun, bool make)
{
    if (stopAll)
        return 0;
    int rv = 0;
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
        OSTakeJobIfNotMake lockJob(cmd, !make);
        int rv = OS::Spawn(cmd, environment, nullptr);
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
        for (auto&& command : cmdList)
        {
            bool make1 = make;
            OSTakeJobIfNotMake lockJob(cmd, !make1);

            auto cmdList = Utils::split(command, '\n');
            for (auto&& cmd : cmdList)
            {
                if (!stopAll)
                {
                    if (!silent)
                    {
                        OS::WriteToConsole(OS::JobName() + cmd + "\n");
                    }
                    int rv1;
                    if (!dontrun)
                    {
                        std::string str;
                        rv1 = OS::Spawn(cmd, environment,
                                        outputType != o_none && (outputType != o_recurse || !make) ? &str : nullptr);
                        if (outputType != o_none && !str.empty())
                            output.push_back(str);
                        if (!rv)
                            rv = rv1;
                    }
                }
            }
            if (rv && posix)
                return rv;
            if (stopAll)
                return 0;
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
        size_t z = middle.find_first_of(escapeStart);
        if (z != std::string::npos)
            rv = cmd.size() < lineLength;
        z = last.find_first_of(escapeStart);
        if (z != std::string::npos)
            rv = cmd.size() < lineLength;

        size_t sz = first.size() + last.size();
        size_t szmiddle = lineLength - sz;
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
        int s = working.find_first_not_of(" \t\n");
        if (s != std::string::npos)
            working = working.substr(s);
        else
            break;
        s = working.find_first_of(" \t\n");
        int lfcount = 0;
        for (int t = s; t < working.size() && isspace(working[t]); t++)
            if (working[t] == '\n')
                lfcount++;
        std::string trailer = lfcount > 1 ? "\n" : "";
        if (s == std::string::npos)
            s = working.size();
        int p = working.find_first_of("'");
        int q = working.find_first_of("\"");
        if (p == std::string::npos)
            p = q;
        else if (q != std::string::npos)
            p = p < q ? p : q;
        if (p != std::string::npos)
            s = s < p ? s : p;
        std::string cur;
        if (s == std::string::npos)
        {
            cur = working;
            working = "";
        }

        else
        {
            if (working[s] == '"' || working[s] == '\'')
            {
                s = working.find_first_of(working[s], s + 1);
                if (s == std::string::npos)
                    s = working.size();
                else
                    s = s + 1;
            }
            cur = working.substr(0, s);
            working = working.substr(s);
        }
        cur = Maker::GetFullName(cur);
        if (!rv.empty() && rv[rv.size() - 1] != '\n')
            rv += " ";
        rv += cur + trailer;
    }
    return rv;
}
