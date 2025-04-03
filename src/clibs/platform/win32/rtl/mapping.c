/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <windows.h>
#include <sys/mman.h>

int _RTL_FUNC sysconf(int type)
{
    assert(type == _SC_PAGESIZE);
    return 4096;
}
void* _RTL_FUNC mmap(void* addr, size_t len, int prot, int flags, int fildes, off_t off)
{
    void* rv = NULL;
    HANDLE map;
    assert(addr == 0 && prot == PROT_READ && flags == MAP_PRIVATE);
    map = CreateFileMapping((HANDLE)_get_osfhandle(fildes), NULL, PAGE_READONLY | SEC_RESERVE, 0, 0, NULL);
    if (map != 0)
    {
        rv = MapViewOfFile(map, FILE_MAP_READ, 0, off, 0);
        CloseHandle(map);
    }
    return rv;
}
int _RTL_FUNC munmap(void* addr, size_t len)
{
    UnmapViewOfFile(addr);
    return 0;
}
