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
 *     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 */

#ifndef OS_H
#define OS_H

#include <list>
#include <string>
#include <deque>

#undef GetCurrentTime
#undef Yield

class Time
{
public:
    Time() : seconds(0), ms(0) { }
    Time(unsigned S, unsigned MS) : seconds(S), ms(MS) { }
    unsigned seconds;
    unsigned ms;
    bool operator >(const Time &last);
    bool operator !() { return seconds ==0 && ms == 0; }
    void Clear() { seconds = 0; ms = 0; }
};
struct EnvEntry {
    EnvEntry() {}
    EnvEntry(const std::string Name, const std::string Value) :
        name(Name), value(Value) { }
    std::string name;
    std::string value;
} ;
typedef std::list<EnvEntry> EnvironmentStrings;

class OS
{
public:
    static void Init();
    static void PushJobCount(int jobs);
    static void PopJobCount();
    static void JobInit();
    static void JobRundown();
    static bool TakeJob();
    static void GiveJob();
    static void Take();
    static void Give();
    static void WriteConsole(std::string &string);
    static void ToConsole(std::deque<std::string>& strings);
    static void AddConsole(std::deque<std::string>& strings, std::string string);
    static int Spawn(const std::string command, EnvironmentStrings &environment, std::string* output);
    static std::string SpawnWithRedirect(const std::string command);
    static Time GetCurrentTime();
    static Time GetFileTime(const std::string fileName);
    static void SetFileTime(const std::string fileName, Time time);
    static std::string GetWorkingDir();
    static bool SetWorkingDir(const std::string name);
    static void RemoveFile(const std::string name);
    static std::string NormalizeFileName(const std::string name);
    static void CreateThread(void *func, void *data);
    static void Yield();
private:
    static int jobsLeft;
    static std::deque<int> jobCounts;
} ;
#endif