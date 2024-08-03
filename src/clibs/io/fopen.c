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

static int cvFlags(int flags)
{
    int rv = 0;
    switch (flags & (_F_READ | _F_WRIT))
    {
        case _F_READ:
        case 0:
            rv = O_RDONLY;
            break;
        case _F_WRIT:
            rv = O_WRONLY;
            break;
        case _F_READ | _F_WRIT:
            rv = O_RDWR;
            break;
    }
    if (flags & _F_BIN)
        rv |= O_BINARY;
    else
        rv |= O_TEXT;
    if (flags & _F_APPEND)
        rv |= O_APPEND;
    return rv;
}
FILE* __basefopen(const wchar_t* restrict name, const wchar_t* restrict mode, FILE* restrict stream, int fd, int share)
{
    
    int flags = 0, append = 0, update = 0, id = 0, i;
    FILE* file;
    if (!name)
        return NULL;
    wchar_t* fname;
    __ll_enter_critical();
    if (__maxfiles >= _NFILE_)
    {
        _dos_errno = errno = EMFILE;
        __ll_exit_critical();
        return 0;
    }
    for (i = 0; i < wcslen(mode); i++)
    {
        switch (mode[i])
        {
            case 'r':
                flags |= _F_READ;
                break;
            case 'w':
                flags |= _F_WRIT;
                break;
            case 'a':
                flags |= _F_WRIT;
                append = 1;
                break;
            case '+':
                update = 1;
                break;
            case 'b':
                flags |= _F_BIN;
                break;
            case 't':
                flags &= ~_F_BIN;
                break;
        }
    }
    if (!(flags & (_F_READ | _F_WRIT)))
    {
        __ll_exit_critical();
        return 0;
    }
    fname = malloc((wcslen(name) + 1)*2);
    if (!fname)
    {
        __ll_exit_critical();
        return 0;
    }
    wcscpy(fname, name);
    if (stream)
        file = stream;
    else if ((file = malloc(sizeof(FILE))) == 0)
    {
        free(fname);
        __ll_exit_critical();
        return 0;
    }
    else
    {
        memset(file, 0, sizeof(FILE));
        if ((file->extended = malloc(sizeof(struct __file2))) == 0)
        {
            free(fname);
            free(file);
            __ll_exit_critical();
            return 0;
        }
        memset(file->extended, 0, sizeof(file->extended));
        file->extended->lock = 0;
    }
    file->flags = 0;
    if (append)
        flags |= _F_APPEND;
    switch (flags & (_F_READ | _F_WRIT))
    {
        case 0:
            goto nofile;
        case _F_READ:
            if (update)
                flags |= _F_WRIT;
            if (fd != -1)
                id = fd;
            else
                id = _wsopen(name, cvFlags(flags), share, S_IREAD | S_IWRITE);
            break;
        case _F_WRIT:
            if (update)
                flags |= _F_READ;
            id = -1;
            if (fd != -1)
                id = fd;
            else
            {
                if (append)
                    id = _wsopen(name, cvFlags(flags), share, S_IREAD | S_IWRITE);
                if (id < 0)
                    id = _wsopen(name, O_CREAT | O_TRUNC | cvFlags(flags), share, S_IREAD | S_IWRITE);
            }
            break;
        case _F_READ | _F_WRIT:
            __ll_exit_critical();
            return 0;
    }
    if (id < 0)
    {
        goto nofile;
    }
    if (fd != -1)
    {
        if (flags & _F_BIN)
        {
            setmode(fd, O_BINARY);
        }
        else
        {
            setmode(fd, O_TEXT);
        }
    }

    file->extended->orient = __or_unspecified;
    memset(file->extended->mbstate, 0, sizeof(file->extended->mbstate));
    file->token = FILTOK;
    file->level = 0;
    file->fd = id;
    file->flags |= flags | _F_VBUF;
    file->hold = 0;
    if (!stream)
    {
        if (!isatty(id))
        {
            if (setvbuf(file, 0, _IOFBF, 8192))
            {
                free(fname);
                free(file);
                __ll_exit_critical();
                return 0;
            }
        }
        else
        {
            file->flags |= _F_TERM;
            if (setvbuf(file, 0, _IOLBF, BUFSIZ))
            {
                free(fname);
                free(file);
                __ll_exit_critical();
                return 0;
            }
        }
        file->flags |= _F_BUF;
    }
    if (append)
    {
        if (fseek(file, 0, SEEK_END))
        {
        nofile:
            free(fname);
            free(file->buffer);
            free(file);
            __ll_exit_critical();
            return 0;
        }
    }
    file->extended->name = fname;
    _pstreams[__maxfiles++] = file;
    __ll_exit_critical();
    return file;
}
FILE* _RTL_FUNC _wfopen(const wchar_t* restrict name, const wchar_t* restrict mode) { return __basefopen(name, mode, 0, -1, SH_COMPAT); }
FILE* _RTL_FUNC _wfsopen(const wchar_t* restrict name, const wchar_t* restrict mode, int __shmode) { return __basefopen(name, mode, 0, -1, __shmode); }
FILE* _RTL_FUNC fopen(const char* restrict name, const char* restrict mode) 
{ 
    wchar_t buf[260], buf1[64], *p = buf, *q = buf1;
    while (*name)
        *p++ = *name++;
    *p = *name; 
    while (*mode)
        *q++ = *mode++;
    *q = *mode; 
	return __basefopen(buf, buf1, 0, -1, SH_COMPAT); 
}
FILE* _RTL_FUNC _wfdopen(int handle, const wchar_t* restrict mode) { return __basefopen(L"", mode, 0, handle, SH_COMPAT); }
FILE* _RTL_FUNC fdopen(int handle, const char* restrict mode) 
{ 
    wchar_t buf1[64], *q = buf1;
    while (*mode)
        *q++ = *mode++;
    *q = *mode; 
    return _wfdopen(handle, buf1);
}
FILE* _RTL_FUNC _fdopen(int handle, const char* restrict mode) { return fdopen(handle, mode); }
int _RTL_FUNC(fileno)(FILE* stream)
{
    flockfile(stream);
    int rv = (fileno_unlocked)(stream);
    funlockfile(stream);
    return rv;
}
int _RTL_FUNC(fileno_unlocked)(FILE* stream)
{
    if (stream->token != FILTOK)
    {
        errno = ENOENT;
        return -1;
    }
    return stream->fd;
}
int _RTL_FUNC(_fileno_unlocked)(FILE* stream)
{
    if (stream->token != FILTOK)
    {
        errno = ENOENT;
        return -1;
    }
    return stream->fd;
}
int _RTL_FUNC(_fileno)(FILE* stream)
{
    flockfile(stream);
    int rv = (_fileno_unlocked)(stream);
    funlockfile(stream);
    return rv;
}
FILE* _RTL_FUNC _fsopen(const char* name, const char* mode, int share) { return __basefopen(name, mode, 0, -1, share); }
