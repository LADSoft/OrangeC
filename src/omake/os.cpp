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

#define _CRT_SECURE_NO_WARNINGS
#ifdef __unix__
#    define HAVE_UNISTD_H
#endif
#ifdef HAVE_UNISTD_H
#    define _POSIX_C_SOURCE 200809L
#    include <fcntl.h>
#    include <sys/stat.h>
#    include <spawn.h>
#    include <signal.h>
#    include <unistd.h>
#    define _SH_DENYNO 0
#    include <xmmintrin.h>
#    include <wordexp.h>
#    include <wait.h>
#    include <unistd.h>
#    include <poll.h>

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
#include "CmdFiles.h"
// #define DEBUG
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
#ifdef TARGET_OS_WINDOWS
static std::set<HANDLE> processIds;
#else
static std::set<int> processIds;
#endif
std::recursive_mutex OS::consoleMutex;
void OS::TerminateAll()
{
    std::lock_guard<decltype(processIdMutex)> guard(processIdMutex);
    for (auto a : processIds)
    {
#ifdef TARGET_OS_WINDOWS
        TerminateProcess(a, 0);
#else
        kill(a, 0);
#endif
    }
}
std::string OS::QuoteCommand(std::string exe, std::string command)
{
    std::string rv;
    bool sh = exe.find("sh") != std::string::npos;
    if (command.empty() == false && command.find_first_of(" \t\n\v\"") == command.npos)
    {
        rv = std::move(command);
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
#ifdef TARGET_OS_WINDOWS

    DWORD written;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), string.c_str(), string.size(), &written, nullptr);
#else
    printf("%s\n", string.c_str());
#endif
}
void OS::ToConsole(std::deque<std::string>& strings)
{
    std::lock_guard<decltype(consoleMutex)> lg(consoleMutex);
    for (const auto & s : strings)
    {
        WriteToConsole(std::move(s));
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
bool OS::TryTakeJob() { return localJobServer->TryTakeNewJob() != -1; }
void OS::GiveJob() { localJobServer->ReleaseJob(); }
std::string OS::GetFullPath(const std::string& fullname)
{
    std::lock_guard<decltype(DirectoryMutex)> lg(DirectoryMutex);
    std::string recievingbuffer;
#ifdef TARGET_OS_WINDOWS
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
#else
    // https://pubs.opengroup.org/onlinepubs/000095399/functions/realpath.html
    recievingbuffer.resize(PATH_MAX);
    if (realpath(fullname.c_str(), recievingbuffer.data()))
    {
        recievingbuffer.resize(strlen(recievingbuffer.c_str()));
        return recievingbuffer;
    }
#endif
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
            MakeMain::jobServer.SetValue(std::move(name));
            first = true;
        }
    }
    else
    {
        std::cerr << "An attempt to remake a job server has been performed, this should not happen, please contact the developers "
                     "if this message appears"
                  << std::endl;
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
        {
            Utils::StrCpy(tempfile, CmdFiles::DIR_SEP);
        }
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
#ifdef TARGET_OS_WINDOWS
    std::string command1 = command;

    Variable* v = VariableContainer::Instance()->Lookup("SHELL");
    bool shell_type = (bool)v;
    std::string cmd = v->GetValue();
    if (v->GetFlavor() == Variable::f_recursive)
    {
        Eval r(cmd, false);
        cmd = r.Evaluate();
    }
    bool asapp = true;
    if (shell_type)
    {
        cmd = v->GetValue() + " -c ";
        // we couldn't simply set MAKE properly because they may change the shell in the script
        v = VariableContainer::Instance()->Lookup("MAKE");
        if (v && v->GetValue().find_first_of("\\") != std::string::npos)
        {
            size_t n = command1.find(v->GetValue());
            while (n != std::string::npos)
            {
                std::replace(command1.begin() + n, command1.begin() + n + v->GetValue().size(), '\\', '/');
                n = command1.find(v->GetValue());
            }
        }
        asapp = false;
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
    std::vector<std::string> parent_strs;
    for (auto&& str : environment)
    {
        parent_strs.push_back(str.name + "=" + str.value);
    }
    std::vector<char*> strs;

    for (auto&& str : parent_strs)
    {
        strs.push_back(const_cast<char*>(str.data()));
    }
    strs.push_back(nullptr);
    posix_spawn_file_actions_t spawn_file_actions;
    posix_spawn_file_actions_init(&spawn_file_actions);
    posix_spawnattr_t spawn_attr;
    posix_spawnattr_init(&spawn_attr);

    pid_t default_pid = 0;
    auto shell_var = VariableContainer::Instance()->Lookup("SHELL");
    // Default to a basic shell if we have an issue
    std::string shell_var_value = "/bin/sh";
    if (shell_var != nullptr)
    {
        shell_var_value = shell_var->GetValue();
    }
    else
    {
        OS::WriteToConsole(
            "Warning: The current shell var for $(SHELL) somehow is not set according to OS::Spawn, please report this to the "
            "developers! Using /bin/sh.");
    }

    int pipe_cout[2];
    pipe(pipe_cout);
    // Copy the spawned program's stdout to the pipe's input
    int ret = posix_spawn_file_actions_adddup2(&spawn_file_actions, pipe_cout[1], 1);
    if (ret)
    {
        printf("Line: %d, Errno: %d, error: %s\n", __LINE__, errno, strerror(errno));
        return -1;
    }
    ret = posix_spawn_file_actions_addclose(&spawn_file_actions, pipe_cout[1]);
    if (ret)
    {
        printf("Line: %d, Errno: %d, error: %s\n", __LINE__, errno, strerror(errno));
        return -1;
    }
    ret = posix_spawn_file_actions_addclose(&spawn_file_actions, pipe_cout[0]);
    if (ret)
    {
        printf("Line: %d, Errno: %d, error: %s\n", __LINE__, errno, strerror(errno));
        return -1;
    }
    const char* args[] = {shell_var_value.c_str(), "-c", "--", command.c_str(), nullptr};
    char cwd[1024];
    getcwd(cwd, 1024);
    ret = posix_spawn(&default_pid, shell_var_value.c_str(), &spawn_file_actions, &spawn_attr, (char* const*)args, strs.data());
    std::string output_str;
    posix_spawn_file_actions_destroy(&spawn_file_actions);
    posix_spawnattr_destroy(&spawn_attr);
    if (ret != 0)
    {
        printf("Failed to spawn, errno: %d, err: %s, command: %s\ncwd:%s\n", errno, strerror(errno), command.c_str(), cwd);
        close(pipe_cout[0]);
        close(pipe_cout[1]);
        return -1;
    }
    int status;
    int ret_wait = 0;
    bool exit_condition = false;
    do
    {
        struct pollfd polls;
        polls.events = POLLIN;
        polls.fd = pipe_cout[0];
        polls.revents = 0;
        int poll_ret = poll(&polls, 1, 100);
        if (poll_ret > 0 && polls.revents & POLLIN)
        {
            char bytes_to_read[120];
            int bytes_read = read(pipe_cout[0], bytes_to_read, sizeof(bytes_to_read));
            if (bytes_read > 0)
            {
                output_str += std::string(bytes_to_read, bytes_read);
            }
        }
        if ((poll_ret && !(polls.revents & POLLIN)) || poll_ret == 0)
        {
            ret_wait = waitpid(default_pid, &status, WUNTRACED | WNOHANG);
            if (ret == -1)
            {
                OS::WriteToConsole("An error has occured!");
            }
            exit_condition = (WIFEXITED(status) || WIFSTOPPED(status));
            status = WEXITSTATUS(status);
        }

    } while (!exit_condition);
    close(pipe_cout[0]);
    close(pipe_cout[1]);
    return status;
#endif
}
std::string OS::SpawnWithRedirect(const std::string command)
{
#ifdef TARGET_OS_WINDOWS
    std::string command1 = std::move(command);
    std::string rv;
    Variable* v = VariableContainer::Instance()->Lookup("SHELL");
    bool shell_like = (bool)v;
    std::string cmd = v->GetValue();
    if (v->GetFlavor() == Variable::f_recursive)
    {
        Eval r(cmd, false);
        cmd = r.Evaluate();
    }
    if (shell_like)
    {
        cmd = v->GetValue() + " -c ";
        // we couldn't simply set MAKE properly because they may change the shell in the script
        v = VariableContainer::Instance()->Lookup("MAKE");
        if (v && v->GetValue().find_first_of("\\") != std::string::npos)
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
    cmd += QuoteCommand(cmd, std::move(command1));
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

    // Basing a lot of following code on this, but not directly:
    // https://stackoverflow.com/a/27328610

    posix_spawn_file_actions_t spawn_file_actions;
    posix_spawn_file_actions_init(&spawn_file_actions);
    posix_spawnattr_t spawn_attr;
    posix_spawnattr_init(&spawn_attr);

    pid_t default_pid = 0;
    auto shell_var = VariableContainer::Instance()->Lookup("SHELL");
    // Default to a basic shell if we have an issue
    std::string shell_var_value = "/bin/sh";
    if (shell_var != nullptr)
    {
        shell_var_value = shell_var->GetValue();
    }
    else
    {
        OS::WriteToConsole(
            "Warning: The current shell var for $(SHELL) somehow is not set according to OS::Spawn, please report this to the "
            "developers! Using /bin/sh.");
    }

    int pipe_cout[2];
    pipe(pipe_cout);
    // Copy the spawned program's stdout to the pipe's input
    posix_spawn_file_actions_adddup2(&spawn_file_actions, pipe_cout[1], 1);
    posix_spawn_file_actions_addclose(&spawn_file_actions, pipe_cout[1]);
    posix_spawn_file_actions_addclose(&spawn_file_actions, pipe_cout[0]);
    const char* args[] = {shell_var_value.c_str(), "-c", "--", command.c_str(), nullptr};
    int ret = posix_spawn(&default_pid, shell_var_value.c_str(), &spawn_file_actions, &spawn_attr, (char* const*)args, environ);
    std::string output_str;
    posix_spawn_file_actions_destroy(&spawn_file_actions);
    posix_spawnattr_destroy(&spawn_attr);
    if (ret != 0)
    {
        close(pipe_cout[0]);
        close(pipe_cout[1]);
        return "";
    }
    int status;
    int ret_wait = 0;
    bool exit_condition = false;
    do
    {
        struct pollfd polls;
        polls.events = POLLIN;
        polls.fd = pipe_cout[0];
        polls.revents = 0;
        int poll_ret = poll(&polls, 1, 100);
        if (poll_ret > 0 && (polls.revents & POLLIN))
        {
            char bytes_to_read[120];
            int bytes_read = read(pipe_cout[0], bytes_to_read, sizeof(bytes_to_read));
            if (bytes_read > 0)
            {
                output_str += std::string(bytes_to_read, bytes_read);
            }
        }
        if ((poll_ret && !(polls.revents & POLLIN)) || poll_ret == 0)

        {
            ret_wait = waitpid(default_pid, &status, WUNTRACED | WCONTINUED);
            if (ret == -1)
            {
                OS::WriteToConsole("An error has occured!");
            }
            exit_condition = (WIFEXITED(status) || WIFSTOPPED(status));
        }

    } while (!exit_condition);
    close(pipe_cout[0]);
    close(pipe_cout[1]);
    return output_str;
#endif
}
Time OS::GetCurrentTime()
{
#ifdef TARGET_OS_WINDOWS

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
    auto cur_time = std::chrono::system_clock::now();
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(cur_time.time_since_epoch());
    Time rv(secs.count(), std::chrono::duration_cast<std::chrono::milliseconds>(cur_time.time_since_epoch()).count());
    return rv;
#endif
}
Time OS::GetFileTime(const std::string fileName)
{
#ifdef TARGET_OS_WINDOWS
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
#else
    struct stat file_status;
    if (stat(fileName.c_str(), &file_status) == 0)
    {
        // POSIX-2008 uses timespecs per spec https://www.man7.org/linux/man-pages/man3/stat.3type.html
        struct timespec ts = file_status.st_mtim;
        // S and MS of the file,
        Time rv(ts.tv_sec, ts.tv_nsec / 1'000'000);
        return rv;
    }
#endif
    return Time();
}
void OS::SetFileTime(const std::string fileName, Time time)
{
#ifdef TARGET_OS_WINDOWS
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
#else
    int fd = open(fileName.c_str(), 0);
    if (fd > 0)
    {
        struct timespec times[2];

        clock_gettime(CLOCK_REALTIME, &times[0]);
        times[1] = times[0];
        futimens(fd, times);
        close(fd);
    }
#endif
}
std::string OS::GetWorkingDir()
{ 
    #ifdef _WIN32
    #ifndef PATH_MAX
    #define PATH_MAX MAX_PATH
    #endif
    #endif
    char buf[PATH_MAX];
    getcwd(buf, PATH_MAX);
    return buf;
}
bool OS::SetWorkingDir(const std::string name) { return !chdir(name.c_str()); }
void OS::RemoveFile(const std::string name) { unlink(name.c_str()); }
std::string OS::NormalizeFileName(const std::string file)
{
    std::string name = std::move(file);
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
            if (name[i] == '\\' && (!i || name[i - 1] != '='))
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