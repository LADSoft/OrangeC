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

#ifndef OS_H
#define OS_H

#include <list>
#include <string>
#include <deque>
#include "JobServer.h"
#include <mutex>
#undef GetCurrentTime
#undef Yield

class Time
{
  public:
    Time() : seconds(0), ms(0) {}
    Time(time_t S, unsigned MS) : seconds(S), ms(MS) {}
    time_t seconds;
    unsigned ms;
    bool operator>(const Time& last);
    bool operator>=(const Time& last);
    bool operator!() { return seconds == 0 && ms == 0; }
    void Clear()
    {
        seconds = 0;
        ms = 0;
    }
};
struct EnvEntry
{
    EnvEntry() {}
    EnvEntry(const std::string Name, const std::string Value) : name(std::move(Name)), value(std::move(Value)) {}
    std::string name;
    std::string value;
};
typedef std::list<EnvEntry> EnvironmentStrings;

class OS
{
  public:
    static void Init();
    static void TerminateAll();
    static std::string QuoteCommand(std::string exe, std::string command);
    static void SetSHEXE(bool flag) { isSHEXE = flag; }
    static void PushJobCount(int jobs);
    static void PopJobCount();
    static std::string JobName();
    static void JobInit();
    static void JobRundown();
    static bool TakeJob();
    static void GiveJob();
    static void WriteToConsole(std::string string);
    static void ToConsole(std::deque<std::string>& strings);
    static void AddConsole(std::deque<std::string>& strings, std::string string);
    static int Spawn(const std::string command, EnvironmentStrings& environment, std::string* output);
    static std::string SpawnWithRedirect(const std::string command);
    static Time GetCurrentTime();
    static Time GetFileTime(const std::string fileName);
    static void SetFileTime(const std::string fileName, Time time);
    static std::string GetWorkingDir();
    static bool SetWorkingDir(const std::string name);
    static void RemoveFile(const std::string name);
    static std::string NormalizeFileName(const std::string name);
    static void CreateThread(void* func, void* data);
    template <class Function, class... Args>
    static OMAKE::JobServerAwareThread CreateThread(Function&& f, Args&&... args)
    {
        // Always use the JobServer as the *SOURCE OF TRUTH* for everything, we have one instance, we can use it well
        return OMAKE::JobServerAwareThread(localJobServer, std::forward<Function>(f), std::forward<Args>(args)...);
    }
    static void InitJobServer();
    static std::string GetFullPath(const std::string& filename);
    static int JobCount() { return jobsLeft; }
    static int GetProcessId();
    static std::recursive_mutex& GetConsoleMutex() { return consoleMutex; }

  private:
    static std::shared_ptr<OMAKE::JobServer> localJobServer;
    static int jobsLeft;
    static std::deque<int> jobCounts;
    static bool isSHEXE;
    static std::string jobName;
    static std::string jobFile;
    static bool first;
    static std::recursive_mutex consoleMutex;
};
#endif