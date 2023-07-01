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
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

ssize_t _RTL_FUNC readlink(const char *path, char *buf, size_t bufsiz)
{
    if (!buf)
    {
        errno = EFAULT;
        return -1;
    }
    if ((int)bufsiz < 0)
    {
        errno = EINVAL;
        return -1;    
    }
    if (access(path, 0) != 0)
    {
        errno = ENOENT;
        return -1;
    }
    DWORD attr = GetFileAttributes(path);
    if (attr == INVALID_FILE_ATTRIBUTES || !(attr & FILE_ATTRIBUTE_REPARSE_POINT) || (attr & FILE_ATTRIBUTE_DIRECTORY))
    {
        // not a symbol link
        errno = EINVAL;
        return -1;
    }
    FILE *fil = fopen(path, "rb");
    if (fil)
    {
        if (!fseek(fil, 0, SEEK_END))
        {
            long size = ftell(fil);
            if (size != EOF)
            {
                 if (size > bufsiz)
                     size = bufsiz;
                 if (!fseek(fil, 0, SEEK_SET))
                 {
                      if (fread(buf, 1, size, fil) == size)
                      {
                           fclose(fil);
                           return size;
                      }
                 }
            }
        }
        fclose(fil);
    }
    errno = EIO;
    return -1;
}