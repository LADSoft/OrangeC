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

#include <io.h>
#include <sys/types.h>
#include <errno.h>
ssize_t pread(int fd, void* buf, size_t nbyte, off_t offset)
{
    int save_errno;
    size_t result;
    off_t old_offset = lseek(fd, 0, SEEK_CUR);
    if (old_offset == (off_t)-1)
        return -1;
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1)
        return -1;
    result = read(fd, buf, nbyte);
    save_errno = errno;
    if (lseek(fd, old_offset, SEEK_SET) == (off_t)-1)
    {
        if (result == -1)
            errno = save_errno;
        return -1;
    }
    errno = save_errno;
    return result;
}
