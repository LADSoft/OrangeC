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

#define _CRT_SECURE_NO_WARNINGS  

#include <windows.h>
#undef WriteConsole
#include <process.h>
#include <direct.h>
#include <stdio.h>
#include <time.h>
#undef min
#undef max
#include "Variable.h"
#include "Eval.h"
#include "os.h"
#include <algorithm>
#include <iostream>
#include <sstream>

static HANDLE jobsSemaphore;

static CRITICAL_SECTION consoleSync;
int OS::jobsLeft;
std::deque<int> OS::jobCounts;
bool OS::isSHEXE;

bool Time::operator >(const Time &last)
{
    if (this->seconds > last.seconds)
        return true;
    if (this->seconds == last.seconds)
        if (this->ms > last.ms)
            return true;
    return false;
}
void OS::Init()
{
    InitializeCriticalSection(&consoleSync);
}
void OS::WriteConsole(std::string string)
{
    DWORD written; 
    EnterCriticalSection(&consoleSync);
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), string.c_str(), string.size(), &written, nullptr);
    LeaveCriticalSection(&consoleSync);
}
void OS::ToConsole(std::deque<std::string>& strings)
{
    EnterCriticalSection(&consoleSync);
    for (auto s : strings)
    {
	WriteConsole(s);
    }
    strings.clear();
    LeaveCriticalSection(&consoleSync);
}
void OS::AddConsole(std::deque<std::string>& strings, std::string string)
{
    EnterCriticalSection(&consoleSync);
    strings.push_back(string);
    LeaveCriticalSection(&consoleSync);
}
void OS::PushJobCount(int jobs)
{
    jobsLeft = jobs;
    jobCounts.push_back(jobs);
}
void OS::PopJobCount()
{
    jobCounts.pop_back();
    jobsLeft = jobCounts.back();
}
bool OS::TakeJob()
{
    if (--jobsLeft < 0)
    {
        jobsLeft++;
        return false;
    }
    if (WaitForSingleObject(jobsSemaphore, 0) == WAIT_OBJECT_0)
        return true;
    jobsLeft++;
    return false;
}
void OS::GiveJob()
{
    jobsLeft++;
    ReleaseSemaphore(jobsSemaphore, 1, nullptr);
}
void OS::JobInit()
{
    std::string name;
    Variable *v = VariableContainer::Instance()->Lookup(".OMAKESEM");
    if (v)
    {
         name = v->GetValue(); 
    }
    else
    {
        std::ostringstream t;
        srand((unsigned)time(NULL));
        t << rand() << rand();
        name = t.str();
        v = new Variable(".OMAKESEM", name, Variable::f_recursive, Variable::o_environ);
        *VariableContainer::Instance() += v;
    }
    v->SetExport(true);
    name = std::string("OMAKE") + name;
    jobsSemaphore = CreateSemaphore(nullptr, jobsLeft, jobsLeft, name.c_str());
    if (GetLastError() == ERROR_ALREADY_EXISTS)
        ReleaseSemaphore(jobsSemaphore, 1, nullptr);
}
void OS::JobRundown()
{
    WaitForSingleObject(jobsSemaphore, INFINITE);
    CloseHandle(jobsSemaphore);
}
void OS::Take()
{
   EnterCriticalSection(&consoleSync);
 }
void OS::Give()
{
   LeaveCriticalSection(&consoleSync);
}
int OS::Spawn(const std::string command, EnvironmentStrings &environment, std::string *output)
{
    std::string command1 = command;

    Variable *v = VariableContainer::Instance()->Lookup("SHELL");
    if (!v)
        return -1;
    std::string cmd = v->GetValue();
    if (v->GetFlavor() == Variable::f_recursive)
    {
        Eval r(cmd, false);
        cmd = r.Evaluate();
    }
    cmd += " /c ";
    cmd += std::string("\"") + command + "\"";
    STARTUPINFO startup;
    PROCESS_INFORMATION pi;
    HANDLE pipeRead, pipeWrite, pipeWriteDuplicate;
    if (output)
    {
        CreatePipe(&pipeRead, &pipeWrite, nullptr, 4 * 1024 * 1024);
        DuplicateHandle(GetCurrentProcess(),pipeWrite, GetCurrentProcess(), &pipeWriteDuplicate, 0, TRUE, DUPLICATE_SAME_ACCESS);
        CloseHandle(pipeWrite);
    }
    int rv;
    memset(&startup, 0, sizeof(startup));
    startup.cb = sizeof(STARTUPINFO);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    if (output)
    {
        startup.hStdOutput = pipeWriteDuplicate;
        startup.hStdError = pipeWriteDuplicate;
    }
    else
    {
        startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        startup.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    }
    int n = 0;
    for (auto env : environment)
    {
        n += env.name.size() + env.value.size() + 2;
    }
    n++;
    char *env = new char[n];
    memset(env, 0, sizeof(char)*n); // !!!
    char *p = env;
    for (auto env : environment)
    {
        memcpy(p, env.name.c_str(), env.name.size());
        p+= env.name.size();
        *p++ = '=';
        memcpy(p, env.value.c_str(), env.value.size());
        p+= env.value.size();
        *p++ = '\0';
    }
    *p++ = '\0';
    bool asapp = true;
    bool instr = false;
    // need a shell for any kind of I/O redirection
    for (auto c : command)
    {
        if (instr)
        {
            instr = c != '"';
        }
        else if (c == '"')
        {
            instr = true;
        }
        else if (c == '|' || c == '&' || c == '<' || c == '>')
        {
            asapp = false;
            break;
        }
    }
    // try as an app first
    if (asapp && CreateProcess(nullptr, (char *)command.c_str(), nullptr, nullptr, true, 0, env,
                      nullptr, &startup, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        if (output)
        {
            DWORD avail = 0;
            PeekNamedPipe(pipeRead, NULL, 0, NULL, &avail, NULL);
            if (avail > 0)
            {
                char *buffer = new char[avail+1];
                DWORD readlen = 0;

                ReadFile(pipeRead,buffer, avail, &readlen, nullptr);
                buffer[readlen] = 0;
                *output = buffer;
                delete [] buffer;
             }
        }
        DWORD x;
        GetExitCodeProcess(pi.hProcess, &x);
        rv = x;
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else
    {
        // not found, try running a shell to handle it...
        if (CreateProcess(nullptr, (char *)cmd.c_str(), nullptr, nullptr, true, 0, env,
                      nullptr, &startup, &pi))
        {
            WaitForSingleObject(pi.hProcess, INFINITE);
            if (output)
            {
                DWORD avail = 0;
                PeekNamedPipe(pipeRead, NULL, 0, NULL, &avail, NULL);
                if (avail > 0)
                {
                    char *buffer = new char[avail+1];
                    DWORD readlen = 0;

                    ReadFile(pipeRead,buffer, avail, &readlen, nullptr);
                    buffer[readlen] = 0;
                    *output = buffer;
                    delete [] buffer;
                }
            }
            DWORD x;
            GetExitCodeProcess(pi.hProcess, &x);
            rv = x;
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        else
        {
            rv = -1;
        }
    }
    if (output)
    {
        CloseHandle(pipeRead);
        CloseHandle(pipeWriteDuplicate);
    }
    delete[] env;
    return rv;
}
std::string OS::SpawnWithRedirect(const std::string command)
{
    std::string rv;
    Variable *v = VariableContainer::Instance()->Lookup("SHELL");
    if (!v)
        return "";
    std::string cmd = v->GetValue();
    if (v->GetFlavor() == Variable::f_recursive)
    {
        Eval r(cmd, false);
        cmd = r.Evaluate();
    }
    cmd += " /c ";
    cmd += command;
    STARTUPINFO startup;
    PROCESS_INFORMATION pi;
    memset(&startup, 0, sizeof(startup));
    HANDLE pipeRead, pipeWrite, pipeWriteDuplicate;
    CreatePipe(&pipeRead, &pipeWrite, nullptr, 1024*1024);
    DuplicateHandle(GetCurrentProcess(),pipeWrite, GetCurrentProcess(), &pipeWriteDuplicate, 0, TRUE, DUPLICATE_SAME_ACCESS);
    CloseHandle(pipeWrite);
    startup.cb = sizeof(STARTUPINFO);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = pipeWriteDuplicate;
    startup.hStdError = pipeWriteDuplicate;
    if (CreateProcess(nullptr, (char *)cmd.c_str(), nullptr, nullptr, true, 0, nullptr,
                      nullptr, &startup, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD avail = 0;
        PeekNamedPipe(pipeRead, NULL, 0, NULL, &avail, NULL);
        if (avail > 0)
        {
            char *buffer = new char[avail+1];
            DWORD readlen = 0;
            ReadFile(pipeRead,buffer, avail, &readlen, nullptr);
            buffer[readlen] = 0;
            rv = buffer;
            delete [] buffer;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else
    {
        rv = -1;
    }
    CloseHandle(pipeRead);
    CloseHandle(pipeWriteDuplicate);
    return rv;
}
Time OS::GetCurrentTime()
{
    SYSTEMTIME systemTime;
    ::GetLocalTime(&systemTime);
    struct tm tmx;
    memset(&tmx, 0, sizeof(tmx));
    tmx.tm_hour = systemTime.wHour;
    tmx.tm_min = systemTime.wMinute;
    tmx.tm_sec = systemTime.wSecond;
    tmx.tm_mday = systemTime.wDay;
    tmx.tm_mon = systemTime.wMonth - 1;
    tmx.tm_year = systemTime.wYear - 1900;
    time_t t = mktime(&tmx);
    Time rv(t, systemTime.wMilliseconds);
    return rv;
}
Time OS::GetFileTime(const std::string fileName)
{
    FILETIME mod;
    HANDLE h = CreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                          
    if (h != INVALID_HANDLE_VALUE)
    {
     	if (::GetFileTime(h, nullptr, nullptr, &mod))
        {
            CloseHandle(h);
            FILETIME v;
            SYSTEMTIME systemTime;
            LocalFileTimeToFileTime(&mod, &v);
            FileTimeToSystemTime(&v, &systemTime);
            struct tm tmx;
            memset(&tmx, 0, sizeof(tmx));
            tmx.tm_hour = systemTime.wHour;
            tmx.tm_min = systemTime.wMinute;
            tmx.tm_sec = systemTime.wSecond;
            tmx.tm_mday = systemTime.wDay;
            tmx.tm_mon = systemTime.wMonth-1;
            tmx.tm_year = systemTime.wYear - 1900;
            time_t t = mktime(&tmx);
            Time rv(t, systemTime.wMilliseconds);
            return rv;
        }
        CloseHandle(h);
    }
    Time rv;
    return rv;
}
void OS::SetFileTime(const std::string fileName, Time time)
{
    FILETIME cr, ac, mod;
    HANDLE h = CreateFile(fileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                          
    if (h != INVALID_HANDLE_VALUE)
    {
        time_t t = time.seconds;
        struct tm *tmx = localtime(&t);
        SYSTEMTIME systemTime;
        
        systemTime.wHour = tmx->tm_hour;
        systemTime.wMinute = tmx->tm_min;
        systemTime.wSecond = tmx->tm_sec;
        systemTime.wDay = tmx->tm_mday;
        systemTime.wMonth = tmx->tm_mon + 1;
        systemTime.wYear = tmx->tm_year + 1900;
        systemTime.wMilliseconds = time.ms;
        FILETIME v, mod;
        SystemTimeToFileTime(&systemTime, &v);
        LocalFileTimeToFileTime(&v, &mod);
        ::SetFileTime(h, nullptr, nullptr, &mod);
        CloseHandle(h);
    }
}
std::string OS::GetWorkingDir()
{
    char buf[MAX_PATH];
    getcwd(buf, MAX_PATH);
    return buf;
}
bool OS::SetWorkingDir(const std::string name)
{
    return !chdir(name.c_str());
}
void OS::RemoveFile(const std::string name)
{
    unlink(name.c_str());
}
std::string OS::NormalizeFileName(const std::string file)
{
    std::string name = file;
    // slash at the beginning of a word could be a command line switch so we don't replace it, otherwise replace '/'
    // with '\\' when not in a string
    int stringchar = 0;
    bool escape = false;
    for (size_t i=0; i < name.size(); i++)
    {
        if (stringchar)
        {
             if (name[i] == stringchar && !escape)
                  stringchar = 0;
             else if (escape)
                  escape = false;
             else if (name[i] == '\\')
                  escape = true;
        }
        else if (name[i] == '\'' || name[i] == '"')
        {
            stringchar = name[i];
        }
        else if (isSHEXE)
        {
            if (name[i] == '\\')
                name[i] = '/';
        }
        else if (name[i] == '/' && i > 0 && !isspace(name[i-1]))
        {
            name[i] = '\\';
        }
    }
    return name;
}
void OS::CreateThread(void *func, void *data)
{
    CloseHandle((HANDLE)_beginthreadex(nullptr, 0, (unsigned (CALLBACK *)(void *))func, data, 0, NULL));
}
void OS::Yield()
{
    ::Sleep(10);
}