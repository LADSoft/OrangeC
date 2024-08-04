/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#include <windows.h>
#include <errno.h>

int __ll_openpipe(int* read, int* write, unsigned int size)
{
    HANDLE pipeRead, pipeWrite, pipeWriteDuplicate = NULL, pipeReadDuplicate = NULL;
    if (!CreatePipe(&pipeRead, &pipeWrite, NULL, size))
    {
        errno = GetLastError();
        return -1;
    }
    if (!DuplicateHandle(GetCurrentProcess(), pipeRead, GetCurrentProcess(), &pipeReadDuplicate, 0, TRUE, DUPLICATE_SAME_ACCESS) ||
        !DuplicateHandle(GetCurrentProcess(), pipeWrite, GetCurrentProcess(), &pipeWriteDuplicate, 0, TRUE, DUPLICATE_SAME_ACCESS))
    {
        errno = GetLastError();
        CloseHandle(pipeWrite);
        CloseHandle(pipeRead);
        CloseHandle(pipeWriteDuplicate);
        CloseHandle(pipeReadDuplicate);
        return -1;
    }
    CloseHandle(pipeWrite);
    CloseHandle(pipeRead);
    *read = (int)pipeReadDuplicate;
    *write = (int)pipeWriteDuplicate;
    return 0;
}
