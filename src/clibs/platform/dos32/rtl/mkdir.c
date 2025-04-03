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

#include <errno.h>
#include <dpmi.h>
#include <dos.h>
#include <dir.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

int _RTL_FUNC         _wmkdir(const wchar_t * __path)
{
    char buf[260], *p = buf;
    while (*__path)
        *p++ = *__path++;
    *p = *__path;
    return _mkdir(buf);
}
int _RTL_FUNC         _mkdir( const char  *__path )
{
    DPMI_REGS regs;
    SELECTOR sel,para;
    if (dpmi_alloc_real_memory(&sel,&para,0x8))
        return 8;
    dpmi_copy_from_ds(sel,0,__path,strlen(__path)+1);
    regs.b.ah = 0x39;
    regs.h.flags = 0x72;
    regs.h.ss = 0;
    regs.h.sp = 0;
    regs.h.ds = para;
    regs.h.dx = 0;
    dpmi_simulate_real_interrupt(0x21,&regs);
    dpmi_dealloc_real_memory(sel);
    if (regs.h.flags & 1) {
        _dos_errno = regs.b.al;
        errno = ENOENT;
        return -1;
    }
    return 0;
}
int _RTL_FUNC         mkdir( const char  *__path, int __amode )
{
    int rv = _mkdir(__path);
    if (!rv)
        return __ll_chmod(__path, __amode);
    return 0;
}
