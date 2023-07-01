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

#ifndef _UNISTD_H
#define _UNISTD_H

#include <io.h>
#include <sys/types.h>

#define ftruncate chsize

#define _SC_PAGESIZE 30

#ifdef __cplusplus
extern "C"
{
#endif
#ifndef RC_INVOKED
    int _RTL_FUNC _IMPORT sysconf(int);

    int _RTL_FUNC _IMPORT fsync(int fd);

    ssize_t _RTL_FUNC _IMPORT pread(int fd, void* buf, size_t nbyte, off_t offset);
    ssize_t _RTL_FUNC _IMPORT pwrite(int fd, const void* buf, size_t nbyte, off_t offset);

    int _RTL_FUNC _IMPORT getpid(void);
    int _RTL_FUNC _IMPORT getppid(void);

    unsigned _RTL_FUNC _IMPORT alarm(unsigned sec);
    ssize_t _RTL_FUNC readlink(const char *path, char *buf, size_t bufsiz);

#endif

#ifdef __cplusplus
}
#endif

#endif