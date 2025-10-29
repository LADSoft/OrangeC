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

#ifndef SPAWNER_H
#define SPAWNER_H

#include <string>
#include <list>
#include <atomic>
#include <future>
#include <vector>
#include "os.h"
#ifdef TARGET_OS_WINDOWS
#    define WINFUNC __stdcall
#else
#    define WINFUNC
#endif

enum OutputType
{
    o_none,
    o_line,
    o_target,
    o_recurse
};

class Command
{
  public:
    Command(const std::string& File, int Lineno) : file(File), lineno(Lineno) {}
    ~Command() {}
    void operator+=(const std::string& cmd) { commands.push_back(cmd); }

    size_t size() const { return commands.size(); }
    typedef std::list<std::string>::iterator iterator;
    typedef std::list<std::string>::const_iterator const_iterator;
    iterator begin() { return commands.begin(); }
    iterator end() { return commands.end(); }
    std::string GetFile() const { return file; }
    int GetLine() const { return lineno; }

  private:
    std::list<std::string> commands;
    std::string file;
    int lineno;
};
class RuleList;
class Rule;
class Spawner
{
  public:
    Spawner(const EnvironmentStrings& Environ, bool IgnoreErrors, bool Silent, bool OneShell, bool Posix, bool DontRun,
            bool KeepResponseFiles) :
        environment(Environ),
        oneShell(OneShell),
        posix(Posix),
        ignoreErrors(IgnoreErrors),
        silent(Silent),
        dontRun(DontRun),
        keepResponseFiles(KeepResponseFiles),
        tempNum(1),
        lineLength(1024 * 1024),
        outputType(o_none),
        commands(nullptr),
        ruleList(nullptr),
        rule(nullptr),
        retVal(1)
    {
    }
    ~Spawner() {}
    int InternalRun();
    void Run(std::shared_ptr<Command>& Commands, OutputType Type, std::shared_ptr<RuleList> RuleList = nullptr,
             std::shared_ptr<Rule> Rule = nullptr);
    static void Stop() { stopAll = true; }
    std::string shell(const std::string& command);
    void SetLineLength(size_t length) { lineLength = length; }
    size_t GetLineLength() const { return lineLength; }

    int RetVal() { return retVal; }

    static const char escapeStart;
    static const char escapeEnd;

  protected:
    int Run(const std::string& cmd, bool ignoreErrors, bool silent, bool dontrun);
    bool split(const std::string& cmd);
    std::string QualifyFiles(const std::string& cmd);
    void RetVal(int val) { retVal = val; }

  private:
    std::deque<std::string> output;
    std::shared_ptr<Command> commands;
    std::shared_ptr<RuleList> ruleList;
    std::shared_ptr<Rule> rule;
    size_t lineLength;
    std::list<std::string> cmdList;
    EnvironmentStrings environment;
    bool ignoreErrors;
    bool silent;
    bool oneShell;
    bool posix;
    bool dontRun;
    bool keepResponseFiles;
    int tempNum;
    int retVal;
    OutputType outputType;
    static std::atomic<long> runningProcesses;
    static bool stopAll;
};
#endif
