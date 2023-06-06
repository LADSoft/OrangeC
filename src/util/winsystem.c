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

#include <string.h>
#include <ctype.h>

#ifndef HAVE_UNISTD_H
#    include <windows.h>
#endif
#include <stdlib.h>
// this is to get around a buggy command.com on freedos...
// it is implemented this way to prevent the IDE from popping up a console window
// when it is used...
int winsystem(char* cmd)
{
#ifdef HAVE_UNISTD_H
    return system(cmd);
#else
    STARTUPINFO stStartInfo;
    PROCESS_INFORMATION stProcessInfo;
    BOOL bRet;

    memset(&stStartInfo, 0, sizeof(STARTUPINFO));
    memset(&stProcessInfo, 0, sizeof(PROCESS_INFORMATION));

    stStartInfo.cb = sizeof(STARTUPINFO);
    stStartInfo.dwFlags = STARTF_USESTDHANDLES;
    stStartInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    stStartInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    stStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    bRet = CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &stStartInfo, &stProcessInfo);

    if (!bRet)
    {
        return -1;
    }
    WaitForSingleObject(stProcessInfo.hProcess, INFINITE);
    DWORD rv = -1;
    GetExitCodeProcess(stProcessInfo.hProcess, &rv);
    CloseHandle(stProcessInfo.hProcess);
    CloseHandle(stProcessInfo.hThread);
    return rv;
#endif
}
