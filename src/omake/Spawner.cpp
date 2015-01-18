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
#include "Spawner.h"
#include "Eval.h"
#include "Maker.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include "utils.h"
const char Spawner::escapeStart = '\x1';
const char Spawner::escapeEnd = '\x2';
int Spawner::lineLength = 500;
std::list<std::string> Spawner::cmdList;

int Spawner::Run(Command &commands, RuleList *ruleList, Rule *rule)
{
    int rv = 0;
    for (Command::iterator it = commands.begin(); it != commands.end() && !rv ; ++it)
    {
        bool curSilent = silent;
        bool curIgnore = ignoreErrors;
        bool curDontRun = dontRun;
        const std::string &a = (*it);
        int i;
        for (i=0; i < a.size(); i++)
            if (a[i] == '+')
                curDontRun = false;
            else if (a[i] == '@')
                curSilent = true;
            else if (a[i] == '-')
                curIgnore = true;
            else
                break;
        if (a.find("$(MAKE)") != std::string::npos || a.find("${MAKE}") != std::string::npos)
            curDontRun = false;
        std::string cmd = a.substr(i);
        Eval c(cmd, false, ruleList, rule);
        cmd = c.Evaluate(); // deferred evaluation
        size_t n = cmd.find("&&");
        std::string makeName;
        if (n == cmd.size() - 3)
        {
            char match = cmd[n+2];
            cmd.erase(n);
            makeName = "maketemp.";
            if (tempNum < 10)
                makeName = makeName + "00" + Utils::NumberToString(tempNum);
            else if (tempNum < 100)
                makeName = makeName + "0" + Utils::NumberToString(tempNum);
            else
                makeName = makeName + Utils::NumberToString(tempNum);
            tempNum++;
            std::fstream fil(makeName.c_str(), std::ios::out);
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
                    if (n+1 < current.size())
                        tail = current.substr(n+1);
                    current.erase(n);
                }
                Eval ce(current, false, ruleList, rule);
                fil << ce.Evaluate().c_str() << std::endl;
            } while (!done);						
            fil.close();
            cmd += makeName + tail;
        }
        cmd = QualifyFiles(cmd);
        rv = Run(cmd, curIgnore, curSilent, curDontRun);
        if (curIgnore)
            rv = 0;
        if (makeName.size())
            OS::RemoveFile(makeName);
    }
    return rv;
}
int Spawner::Run(const std::string &cmd, bool ignoreErrors, bool silent, bool dontrun)
{
    if (!split(cmd))
    {
        Eval::error(std::string ("Command line too long:\n") + cmd);
        return 0xff;
    }
    else
    {
        for (std::list<std::string>::iterator it = cmdList.begin(); it != cmdList.end(); ++it)
        {
            std::string command = *it;
            if (!silent)
                std::cout << "\t" << command.c_str() << std::endl;
            int rv = 0;
            if (!dontrun)
                rv = OS::Spawn(command, environment);
//			if (!silent)
//				std::cout << std::endl;
            if (!ignoreErrors && rv != 0)
                return rv;
        }
        return 0;
    }
}
bool Spawner::split(const std::string &cmd)
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
                p.replace(lsp, p.size()-lsp, "");
                middle.replace(0, lsp+1, "");
                cmdList.push_back(first + p + last);
            }
            else 
            {
                rv = false;
                break;
            }
        }
        if (middle.size())
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
std::string Spawner::shell(const std::string &cmd)
{
    std::string rv = OS::SpawnWithRedirect(cmd);
    size_t n = rv.find_first_of('\n');
    while (n != std::string::npos)
    {
        rv.replace(n, 1, " ");
        n = rv.find_first_of('\n', n);
    }
    n = rv.find_first_of('\r');
    while (n != std::string::npos)
    {
        rv.replace(n, 1, "");
        n = rv.find_first_of('\r', n);
    }
    return rv;
}
std::string Spawner::QualifyFiles(const std::string &cmd)
{
    std::string rv;
    std::string working = cmd;
    while (working.size())
    {
        std::string cur = Eval::ExtractFirst(working, " ");
        cur = Maker::GetFullName(cur);
        if (rv.size())
            rv += " ";
        rv += cur;
    }
    return rv;
}
