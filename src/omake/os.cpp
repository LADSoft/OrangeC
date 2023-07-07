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

#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#    define _SH_DENYNO 0
#    include <xmmintrin.h>
#else
#    include <windows.h>
#    include <process.h>
#    include <direct.h>
#    include <io.h>
#    include <share.h>
#    include <fcntl.h>
#    include <sys/locking.h>
#    define lockf _locking
#    define F_LOCK _LK_LOCK
#    define F_ULOCK _LK_UNLCK
#    ifndef _SH_DENYNO
#        define _SH_DENYNO 0
#    endif
#    ifndef BORLAND
#        define chdir _chdir
#    endif
#endif
#include <cstring>
#undef WriteConsole
#define __MT__  // BCC55 support
#include <cstdio>
#include <ctime>
#include <string>
#include "Utils.h"
#include "Variable.h"
#include "MakeMain.h"
#include "Eval.h"
#include "os.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <random>
#include <array>
#include <functional>
#include <sys/stat.h>
#include <mutex>
#include <memory>
#include "JobServer.h"
//#define DEBUG
static std::mutex processIdMutex;
// This is required because GetFullPathName and SetCurrentDirectory and GetCurrentDirectory are
// all non-safe in multithreaded environments, in order to make this safe, we *MUST* lower ourselves into making these a mutex-gated
// system, this will enforce ordering at the possibility of incorrectness
static std::mutex DirectoryMutex;
std::deque<int> OS::jobCounts;
bool OS::isSHEXE;
int OS::jobsLeft;
std::string OS::jobName = "\t";
std::string OS::jobFile;
std::shared_ptr<OMAKE::JobServer> OS::localJobServer = nullptr;
static std::set<HANDLE> processIds;
std::recursive_mutex OS::consoleMutex;
void OS::TerminateAll()
{
    std::lock_guard<decltype(processIdMutex)> guard(processIdMutex);
    for (auto a : processIds)
        TerminateProcess(a, 0);
}
std::string OS::QuoteCommand(std::string exe, std::string command)
{
    std::string rv;
    bool sh = exe.find("sh.exe") != std::string::npos;
    if (command.empty() == false && command.find_first_of(" \t\n\v\"") == command.npos)
    {
        rv = command;
    }
    else
    {
        rv.push_back('"');

        for (auto it = command.begin(); it != command.end(); ++it)
        {
            unsigned slashcount = 0;
            while (it != command.end() && *it == '\\')
            {
                ++it;
                ++slashcount;
            }

            if (it == command.end())
            {
                // escape all the backslashes
                rv.append(slashcount * 2, '\\');
                break;
            }
            else if (*it == '"' && sh)
            {
                // escape all the backslashes and add a \"
                rv.append(slashcount * 2 + 1, '\\');
                rv.push_back('"');
            }
            else
            {
                // no escape
                rv.append(slashcount, '\\');
                rv.push_back(*it);
            }
        }
        rv.push_back('"');
    }
    return rv;
}
bool Time::operator>(const Time& last)
{
    if (this->seconds > last.seconds)
        return true;
    if (this->seconds == last.seconds)
        if (this->ms > last.ms)
            return true;
    return false;
}
bool Time::operator>=(const Time& last)
{
    return (this->seconds == last.seconds && this->ms == last.ms && (this->seconds != 0 || this->ms != 0)) || *this > last;
}

void OS::Init() {}

void OS::WriteToConsole(std::string string)
{
    std::lock_guard<decltype(consoleMutex)> lg(consoleMutex);
#ifdef _WIN32

    DWORD written;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), string.c_str(), string.size(), &written, nullptr);
#else
    printf("%s\n", string.c_str());
#endif
}
void OS::ToConsole(std::deque<std::string>& strings)
{
    std::lock_guard<decltype(consoleMutex)> lg(consoleMutex);
    for (auto&& s : strings)
    {
        WriteToConsole(s);
    }
    strings.clear();
}
void OS::AddConsole(std::deque<std::string>& strings, std::string string)
{
    std::lock_guard<decltype(consoleMutex)> lg(consoleMutex);
    strings.push_back(string);
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
    localJobServer->TakeNewJob();
    return false;
}
bool OS::TryTakeJob()
{
    return localJobServer->TryTakeNewJob() != -1;
}
void OS::GiveJob() { localJobServer->ReleaseJob(); }
std::string OS::GetFullPath(const std::string& fullname)
{
    std::lock_guard<decltype(DirectoryMutex)> lg(DirectoryMutex);
    std::string recievingbuffer;

    DWORD return_val = GetFullPathNameA(fullname.c_str(), 0, nullptr, nullptr);
    if (!return_val)
    {
        // Do error handling somewhere
    }
    recievingbuffer.reserve(return_val);
    return_val = GetFullPathNameA(fullname.c_str(), recievingbuffer.size(), const_cast<char*>(recievingbuffer.c_str()), NULL);
    if (!return_val)
    {
        // Do error handling somewhere
    }
    return recievingbuffer;
}
std::string OS::JobName() { return jobName; }
void OS::InitJobServer()
{
    bool first = false;
    std::string name;
    if (!localJobServer)
    {
        if (MakeMain::jobServer.GetExists())
        {
            name = MakeMain::jobServer.GetValue();
            localJobServer = OMAKE::JobServer::GetJobServer(name);
        }
        else
        {
            localJobServer = OMAKE::JobServer::GetJobServer(jobsLeft);
            name = localJobServer->PassThroughCommandString();
            MakeMain::jobServer.SetValue(name);
            first = true;
        }
    }
    else
    {
        std::cerr << "An attempt to remake a job server has been performed, this should not happen, please contact the developers if this message appears" << std::endl;
    }
}
bool OS::first = false;
void OS::JobInit()
{
    std::string name = MakeMain::jobServer.GetValue();
    if (MakeMain::printDir.GetValue() && jobName == "\t")
    {
        char tempfile[260];
        tempfile[0] = 0;
        char* temp = tempnam(tempfile, "hi");
        if (tempfile[1] == ':')
        {
            char* p = (char*)strrchr(tempfile, '\\');
            if (!p)
                tempfile[0] = 0;
            else
                p[1] = 0;
        }
        else
        {
            char* p = getenv("TMP");
            if (p && p[0])
            {
                char q(0);
                Utils::StrCpy(tempfile, p);
                p = tempfile;
                while (*p)
                {
                    if (!q && (*p == '/' || *p == '\\'))
                        q = *p;
                    p++;
                }
                if (p[-1] != q)
                {
                    *p++ = q;
                    *p = 0;
                }
            }
            else
                tempfile[0] = 0;
        }
        if (tempfile[0] == 0)
#ifdef HAVE_UNISTD_H
            Utils::StrCpy(tempfile, "./");
#else
            Utils::StrCpy(tempfile, ".\\");
#endif
        Utils::StrCpy(tempfile, (name + ".flg").c_str());
        OS::WriteToConsole("Flag file name: " + name + ".flg");
        int fil = -1;
        if (first)
        {
            fil = open(tempfile, _SH_DENYNO | O_CREAT, 0x1f);
        }
        else
        {
            fil = open(tempfile, _SH_DENYNO | O_RDWR);
        }
        if (fil >= 0)
        {
            int count = 0;
#ifdef BCC32c
            lock(fil, 0, 4);
#else
            lockf(fil, F_LOCK, 4);
#endif
            if (!first)
                if (read(fil, (char*)&count, 4) != 4)
                    count = 0;
            count++;
            lseek(fil, 0, SEEK_SET);
            write(fil, (char*)&count, 4);
            lseek(fil, 0, SEEK_SET);
#ifdef BCC32c
            unlock(fil, 0, 4);
#else
            lockf(fil, F_ULOCK, 4);
#endif
            char buf[256];
            sprintf(buf, "%d> ", count);
            jobName = buf;
        }
        free(temp);
    }
}
void OS::JobRundown()
{
    if (jobFile.size())
        RemoveFile(jobFile);
}
int OS::Spawn(const std::string command, EnvironmentStrings& environment, std::string* output)
{
#ifdef _WIN32
    std::string command1 = command;

    Variable* v = VariableContainer::Instance()->Lookup("SHELL");
    if (!v)
        return -1;
    std::string cmd = v->GetValue();
    if (v->GetFlavor() == Variable::f_recursive)
    {
        Eval r(cmd, false);
        cmd = r.Evaluate();
    }
    bool asapp = true;
    if (cmd == "/bin/sh")
    {
        cmd = "sh.exe -c ";
        // we couldn't simply set MAKE properly because they may change the shell in the script
        v = VariableContainer::Instance()->Lookup("MAKE");
        if (v->GetValue().find_first_of("\\") != std::string::npos)
        {
            size_t n = command1.find(v->GetValue());
            while (n != std::string::npos)
            {
                std::replace(command1.begin() + n, command1.begin() + n + v->GetValue().size(), '\\', '/');
                n = command1.find(v->GetValue());
            }
        }
    }
    else
    {
        cmd += " /c ";
    }
    cmd += QuoteCommand(cmd, command1);
    STARTUPINFO startup = {};
    PROCESS_INFORMATION pi;
    HANDLE pipeRead, pipeWrite, pipeWriteDuplicate;
    if (output)
    {
        CreatePipe(&pipeRead, &pipeWrite, nullptr, 4 * 1024 * 1024);
        DuplicateHandle(GetCurrentProcess(), pipeWrite, GetCurrentProcess(), &pipeWriteDuplicate, 0, TRUE, DUPLICATE_SAME_ACCESS);
        CloseHandle(pipeWrite);
    }
    int rv;
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
    for (auto&& env : environment)
    {
        n += env.name.size() + env.value.size() + 2;
    }
    n++;
    std::unique_ptr<char[]> env = std::make_unique<char[]>(n);
    char* p = env.get();
    memset(p, 0, sizeof(char) * n);  // !!!
    for (auto&& env : environment)
    {
        memcpy(p, env.name.c_str(), env.name.size());
        p += env.name.size();
        *p++ = '=';
        memcpy(p, env.value.c_str(), env.value.size());
        p += env.value.size();
        *p++ = '\0';
    }
    *p++ = '\0';
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
    if (asapp && CreateProcess(nullptr, (char*)command1.c_str(), nullptr, nullptr, true, 0, env.get(), nullptr, &startup, &pi))
    {
        {
            std::lock_guard<decltype(processIdMutex)> guard(processIdMutex);
            processIds.insert(pi.hProcess);
        }
        WaitForSingleObject(pi.hProcess, INFINITE);
        {
            std::lock_guard<decltype(processIdMutex)> guard(processIdMutex);
            processIds.erase(pi.hProcess);
        }

        if (output)
        {

            DWORD avail = 0;
            PeekNamedPipe(pipeRead, nullptr, 0, nullptr, &avail, nullptr);
            if (avail > 0)
            {
                std::unique_ptr<char[]> buffer = std::make_unique<char[]>(avail + 1);
                DWORD readlen = 0;

                ReadFile(pipeRead, buffer.get(), avail, &readlen, nullptr);
                buffer[readlen] = 0;
                *output = buffer.get();
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
        if (CreateProcess(nullptr, (char*)cmd.c_str(), nullptr, nullptr, true, 0, env.get(), nullptr, &startup, &pi))
        {
            {
                std::lock_guard<decltype(processIdMutex)> guard(processIdMutex);

                processIds.insert(pi.hProcess);
            }
            WaitForSingleObject(pi.hProcess, INFINITE);
            {
                std::lock_guard<decltype(processIdMutex)> guard(processIdMutex);

                processIds.erase(pi.hProcess);
            }
            if (output)
            {
                DWORD avail = 0;
                PeekNamedPipe(pipeRead, nullptr, 0, nullptr, &avail, nullptr);
                if (avail > 0)
                {
                    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(avail + 1);
                    DWORD readlen = 0;

                    ReadFile(pipeRead, buffer.get(), avail, &readlen, nullptr);
                    buffer[readlen] = 0;
                    *output = buffer.get();
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
#    ifdef DEBUG
    std::cout << rv << ":" << cmd << std::endl;
#    endif
    return rv;
#else
    return -1;
#endif
}
std::string OS::SpawnWithRedirect(const std::string command)
{
#ifdef _WIN32
    std::string rv;
    Variable* v = VariableContainer::Instance()->Lookup("SHELL");
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
    CreatePipe(&pipeRead, &pipeWrite, nullptr, 1024 * 1024);
    DuplicateHandle(GetCurrentProcess(), pipeWrite, GetCurrentProcess(), &pipeWriteDuplicate, 0, TRUE, DUPLICATE_SAME_ACCESS);
    CloseHandle(pipeWrite);
    startup.cb = sizeof(STARTUPINFO);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = pipeWriteDuplicate;
    startup.hStdError = pipeWriteDuplicate;
    if (CreateProcess(nullptr, (char*)cmd.c_str(), nullptr, nullptr, true, 0, nullptr, nullptr, &startup, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD avail = 0;
        PeekNamedPipe(pipeRead, nullptr, 0, nullptr, &avail, nullptr);
        if (avail > 0)
        {
            std::unique_ptr<char[]> buffer = std::make_unique<char[]>(avail + 1);
            DWORD readlen = 0;
            ReadFile(pipeRead, buffer.get(), avail, &readlen, nullptr);
            buffer[readlen] = 0;
            rv = buffer.get();
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
#else
    return "";
#endif
}
Time OS::GetCurrentTime()
{
#ifdef _WIN32

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
#else
    Time rv;
    return rv;
#endif
}
Time OS::GetFileTime(const std::string fileName)
{
#ifdef _WIN32
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (GetFileAttributesEx(fileName.c_str(), GetFileExInfoStandard, &data))
    {
        FILETIME mod = data.ftLastWriteTime;
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
        tmx.tm_mon = systemTime.wMonth - 1;
        tmx.tm_year = systemTime.wYear - 1900;
        time_t t = mktime(&tmx);
        Time rv(t, systemTime.wMilliseconds);
        return rv;
    }
#endif
    Time rv;
    return rv;
}
void OS::SetFileTime(const std::string fileName, Time time)
{
#ifdef _WIN32
    HANDLE h =
        CreateFile(fileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (h != INVALID_HANDLE_VALUE)
    {
        time_t t = time.seconds;
        struct tm* tmx = localtime(&t);
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
#endif
}
std::string OS::GetWorkingDir()
{
    char buf[260];
    getcwd(buf, 260);
    return buf;
}
void OS::CreateThread(void* func, void* data)
{
#ifdef _WIN32
#    ifdef BCC32c
    DWORD tid;
    CloseHandle(::CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)func, data, 0, &tid));
#    else
    CloseHandle((HANDLE)_beginthreadex(nullptr, 0, (unsigned(CALLBACK*)(void*))func, data, 0, nullptr));
#    endif
#endif
}
bool OS::SetWorkingDir(const std::string name) { return !chdir(name.c_str()); }
void OS::RemoveFile(const std::string name) { unlink(name.c_str()); }
std::string OS::NormalizeFileName(const std::string file)
{
    std::string name = file;
    // slash at the beginning of a word could be a command line switch so we don't replace it, otherwise replace '/'
    // with '\\' when not in a string
    int stringchar = 0;
    bool escape = false;
    for (size_t i = 0; i < name.size(); i++)
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
        else if (name[i] == '/' && i > 0 && !isspace(name[i - 1]))
        {
            name[i] = '\\';
        }
    }
    return name;
}
int OS::GetProcessId() { return getpid(); }