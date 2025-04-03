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

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <sys\stat.h>
#include <threads.h>
#include "libp.h"

extern FILE* _pstreams[_NFILE_];
extern int __maxfiles;

static FILE* _RTL_FUNC basememopen(void* buf, size_t size, int flags, int flags2)
{
    if (size == 0)
        size = 512;
    if (!buf && !(flags2 & _F2_DYNAMICBUFFER))
        return NULL;
    __ll_enter_critical();
    if (__maxfiles >= _NFILE_)
    {
        _dos_errno = errno = EMFILE;
        __ll_exit_critical();
        return 0;
    }
    if (!(flags & (_F_READ | _F_WRIT)))
    {
        __ll_exit_critical();
        return 0;
    }
    FILE* file;
    if ((file = malloc(sizeof(FILE))) == 0)
    {
        __ll_exit_critical();
        return 0;
    }
    else
    {
        memset(file, 0, sizeof(FILE));
        if ((file->extended = malloc(sizeof(struct __file2))) == 0)
        {
            free(file);
            __ll_exit_critical();
            return 0;
        }
        memset(file->extended, 0, sizeof(*file->extended));
        file->extended->lock = 0;
    }
    file->extended->orient = __or_unspecified;
    memset(file->extended->mbstate, 0, sizeof(file->extended->mbstate));
    file->token = FILTOK;
    file->flags = flags;
    file->extended->flags2 = flags2;
    file->bsize = size;
    if (file->flags & _F_OUT)
        file->level = -file->bsize;
    else
        file->level = file->bsize;
    file->fd = -1;
    file->hold = 0;
    if (!buf)
    {
        if ((buf = calloc(1, size)) == 0)
        {
            free(file->extended);
            free(file);
            __ll_exit_critical();
            return 0;
        }
    }
    file->buffer = file->curp = buf;
    _pstreams[__maxfiles++] = file;
    __ll_exit_critical();
    return file;
}
FILE* _RTL_FUNC fmemopen(void* buf, size_t size, const char* mode)
{
    int flags = 0;
    int append = 0;
    const char* p = mode;
    switch (*p)
    {
        case 'r':
            flags = _F_IN | _F_READ;
            break;
        case 'w':
            flags = _F_OUT | _F_WRIT;
            break;
        case 'a':
            flags = _F_OUT | _F_WRIT;
            append = 1;
            break;
        default:
            return NULL;
    }
    p++;
    if (*p == 'b')
    {
        flags |= _F_BIN;
        p++;
    }
    if (*p == '+')
    {
        flags |= _F_READ | _F_WRIT;
        p++;
    }
    FILE* rv = basememopen(buf, size, flags | _F_BUFFEREDSTRING, 0);
    if (rv && buf)
    {
        if (append)
        {
            for (int i = 0; i < size; i++)
                if (((char*)buf)[i] == 0)
                    break;
            rv->curp += size;
            rv->level += size;
         }
    }
    if (rv && !buf)
    {
        // force buffer to be auto-freed
        rv->flags |= _F_BUF;
    }
    return rv;
}
FILE* _RTL_FUNC open_memstream(char** ptr, size_t* sizeloc)
{
    FILE* rv = basememopen(0, 0, _F_OUT | _F_READ | _F_WRIT | _F_BUFFEREDSTRING, _F2_DYNAMICBUFFER);
    if (rv)
    {
        rv->extended->dynamicBuffer[0] = ptr;
        rv->extended->dynamicBuffer[1] = sizeloc;
    }
    return rv;
}
FILE* _RTL_FUNC open_wmemstream(wchar_t** ptr, size_t* sizeloc)
{
    FILE* rv = basememopen(0, 0, _F_OUT | _F_READ | _F_WRIT | _F_BUFFEREDSTRING, _F2_DYNAMICBUFFER | _F2_WCHAR);
    if (rv)
    {
        rv->extended->dynamicBuffer[0] = ptr;
        rv->extended->dynamicBuffer[1] = sizeloc;
    }
    return rv;
}
