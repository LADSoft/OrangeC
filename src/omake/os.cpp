/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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

#include <windows.h>
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

bool Time::operator >(const Time &last)
{
    if (this->seconds > last.seconds)
        return true;
    if (this->seconds == last.seconds)
        if (this->ms > last.ms)
            return true;
    return false;
}
int OS::Spawn(const std::string command, EnvironmentStrings &environment)
{
    static std::string names = "ASSOC ATTRIB BREAK BCDEDIT CACLS CALL CD CHCP CHDIR CHKDSK CHKNTFS CLS CMD COLOR COMP COMPACT CONVERT COPY DATE DEL DIR DISKPART DOSKEY DRIVERQUERY ECHO ENDLOCAL ERASE EXIT FC FIND FINDSTR FOR FORMAT FSUTIL FTYPE GOTO GPRESULT GRAFTABL HELP ICACLS IF LABEL MD MKDIR MKLINK MODE MORE MOVE OPENFILES PATH PAUSE POPD PRINT PROMPT PUSHD RD RECOVER REM REN RENAME REPLACE RMDIR ROBOCOPY SET SETLOCAL SC SCHTASKS SHIFT SHUTDOWN SORT START SUBST SYSTEMINFO TASKLIST TASKKILL TIME TITLE TREE TYPE VER VERIFY VOL XCOPY WMIC ";
    std::string command1 = command;
    std::string aa = Eval::ExtractFirst(command1, " ");
    aa += " ";
    std::transform(aa.begin(), aa.end(), aa.begin(), ::toupper);
    bool isCmd = names.find(aa) != std::string::npos;

    if (!isCmd)
    {
        size_t n = 0;
        size_t nextQuote = command1.find('"');
        while (n < command1.size())
        {
            while (n < command1.size() && command1[n] != '"')
            {
                if (command1[n] == '>' || command1[n] == '<' || command1[n] == '|')
                {
                     n = command1.size();
                     isCmd = true;
                     break;
                }
                n++;
            }
            do
            {
                n++;
            } while (n < command1.size() && command1[n] != '"');
        }
    }
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
    int rv;
    memset(&startup, 0, sizeof(startup));
    startup.cb = sizeof(STARTUPINFO);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    startup.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    int n = 0;
    for (auto env : environment)
    {
        n += env.name.size() + env.value.size() + 2;
    }
    n++;
    char *env = new char[n];
//    if (env)
    {
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
    }
//    else
//    {
//        return -1;
//    }
    if (!isCmd && CreateProcess(nullptr, (char *)command.c_str(), nullptr, nullptr, true, 0, env,
                      nullptr, &startup, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD x;
        GetExitCodeProcess(pi.hProcess, &x);
        rv = x;
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else
    {
        if (CreateProcess(nullptr, (char *)cmd.c_str(), nullptr, nullptr, true, 0, env,
                      nullptr, &startup, &pi))
        {
            WaitForSingleObject(pi.hProcess, INFINITE);
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
        char *buffer = new char[1024 * 1024];
//        if (buffer)
        {
            DWORD readlen = 0;
            DWORD avail = 0;
            PeekNamedPipe(pipeRead, NULL, 1024 * 1024, NULL, &avail, NULL);

            if (avail > 0)
                ReadFile(pipeRead,buffer, 1024 * 1024, &readlen, nullptr);
            buffer[readlen] = 0;
            rv = buffer;
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
                          
    if (h != INVALID_HANDLE_VALUE && ::GetFileTime(h, nullptr, nullptr, &mod))
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
        else if (name[i] == '/' && i > 0 && !isspace(name[i-1]))
        {
            name[i] = '\\';
        }
    }
    return name;
}