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

#include <windows.h>
#include <winbase.h>
#include <process.h>
#include <stdio.h>
#include <assert.h>
#include <sys/wait.h>
#include <map>
static HANDLE hjob;
static HANDLE end;
CRITICAL_SECTION critical;

static std::map<DWORD, HANDLE> pidList;
#pragma startup init 32
#pragma rundown rundown 32
static void InsertPid(DWORD pid)
{
    EnterCriticalSection(&critical);
    if (pidList[pid] == 0)
    {
        LeaveCriticalSection(&critical);
        HANDLE h = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        EnterCriticalSection(&critical);
        if (h)
        {
            pidList[pid] = h;
        }
    }
    LeaveCriticalSection(&critical);
}
static void RemovePid(DWORD pid)
{
    EnterCriticalSection(&critical);
    auto it = pidList.find(pid);
    if (it != pidList.end())
    {
        CloseHandle(it->second);
        pidList.erase(it);
    }
    LeaveCriticalSection(&critical);
}
static void Load(void)
{
    JOBOBJECT_BASIC_PROCESS_ID_LIST pids;

    if (QueryInformationJobObject(hjob, JobObjectBasicProcessIdList, &pids, sizeof(pids), NULL))
    {
        for (int i = 0; i < pids.NumberOfProcessIdsInList; i++)
        {
            if (pids.ProcessIdList[i] != GetCurrentProcessId())
            {
                InsertPid(pids.ProcessIdList[i]);
            }
        }
    }
}

static int CALLBACK refresh(void*)
{
    InitializeCriticalSection(&critical);
    hjob = CreateJobObject(NULL, NULL);
    AssignProcessToJobObject(hjob, OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId()));
    while (WaitForSingleObject(end, 500) == WAIT_TIMEOUT)
    {
        Load();
        Sleep(500);
    }
    CloseHandle(end);
    CloseHandle(hjob);
    DeleteCriticalSection(&critical);
    SetEvent(end);
    return 0;
}
static void init()
{
    end = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    DWORD id;
    CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)refresh, nullptr, 0, &id));
}
static void rundown() { SetEvent(end); }

static BOOLEAN GetChildren(pid_t pid, pid_t** pids, HANDLE** list, size_t* count)
{
    BOOLEAN rv = 0;
    EnterCriticalSection(&critical);
    if (pidList.size())
    {
        *count = 0;
        *list = (HANDLE*)calloc(pidList.size(), sizeof(HANDLE));
        *pids = (pid_t*)calloc(pidList.size(), sizeof(pid_t));
        for (auto v : pidList)
        {
            (*pids)[*count] = v.first;
            (*list)[(*count)++] = v.second;
        }
        rv = 1;
    }
    LeaveCriticalSection(&critical);
    return rv;
}

typedef LONG WINAPI QueryFunc(HANDLE ProcessHandle, ULONG ProcessInformationClass, PVOID ProcessInformation,
                              ULONG ProcessInformationLength, PULONG ReturnLength);

static pid_t waithandler(pid_t pid, int* status, int options)
{
    HANDLE process = NULL;
    pid_t ret = -1;
    size_t count = 0;
    HANDLE* handle_list;
    pid_t* pid_list;
    Load();
    if (GetCurrentProcessId() == pid)
    {
        if (status)
            *status = -1;
        goto fin;
    }
    if (pid > 0 || pid < -10)
    {
        EnterCriticalSection(&critical);
        auto it = pidList.find(pid);
        if (it != pidList.end())
        {
            process = it->second;
            LeaveCriticalSection(&critical);
        }
        else
        {
            LeaveCriticalSection(&critical);
            process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
            if (!process && GetLastError() == ERROR_ACCESS_DENIED)
            {
                process = OpenProcess(SYNCHRONIZE, FALSE, pid);
                if (!process)
                {
                    if (status)
                        *status = -2;
                    goto fin;
                }
            }
        }
        switch (WaitForSingleObject(process, (options & WNOHANG) ? 0 : INFINITE))
        {
            case WAIT_TIMEOUT:
                if (status)
                    *status = -1;
                ret = 0;
                break;
            default:
            case WAIT_FAILED:
                if (status)
                    *status = -1;
                break;
            case WAIT_OBJECT_0:
            {
                DWORD val = -1;
                GetExitCodeProcess(process, &val);
                ret = pid;
                if (status)
                    *status = val;
            }
            break;
        }
        CloseHandle(process);
    }
    else
    {

        if (GetChildren(pid, &pid_list, &handle_list, &count))
        {
            unsigned n;
            switch ((n = WaitForMultipleObjects(count, handle_list, FALSE, (options & WNOHANG) ? 0 : INFINITE)))
            {
                case WAIT_TIMEOUT:
                    if (status)
                        *status = -1;
                    ret = 0;
                    break;
                case WAIT_FAILED:
                    if (status)
                        *status = -1;
                    break;
                default:
                    if (n >= WAIT_OBJECT_0 && n < WAIT_OBJECT_0 + count)
                    {
                        DWORD val = -1;
                        GetExitCodeProcess(handle_list[n - WAIT_OBJECT_0], &val);
                        ret = pid_list[n - WAIT_OBJECT_0];
                        if (status)
                            *status = val;
                        RemovePid(ret);
                    }
                    else
                    {
                        if (status)
                            *status = -1;
                    }
                    break;
            }
        }
    }
fin:
    if (count)
    {
        free(handle_list);
        free(pid_list);
    }
    return ret;
}

pid_t _RTL_FUNC wait(int* status) { return waithandler(-1, status, 0); }

pid_t _RTL_FUNC waitpid(pid_t pid, int* status, int options) { return waithandler(pid, status, options); }
