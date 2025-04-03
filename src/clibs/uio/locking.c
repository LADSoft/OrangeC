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

#include <io.h>
#include <dos.h>
#include <errno.h>
#include <sys\locking.h>

int _RTL_FUNC locking(int __handle, int __cmd, long __length)
{
    long ft = tell(__handle);
    int i;
    if (ft == -1) /* will catch invalid handle */
        return -1;
    switch (__cmd)
    {
        case LK_LOCK:
        case LK_RLCK:
            for (i = 0; i < 10; i++)
            {
                if (!lock(__handle, ft, __length))
                    return 0;
                sleep(1);
            }
            errno = EDEADLOCK;
            return -1;
        case LK_NBLCK:
        case LK_NBRLCK:
            return lock(__handle, ft, __length);
        case LK_UNLCK:
            return unlock(__handle, ft, __length);
        default:
            errno = EINVAL;
            return -1;
    }
}
int _RTL_FUNC _locking(int __handle, int __cmd, long __length) { return locking(__handle, __cmd, __length); }
