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

#include <stdio.h>
#include <errno.h>
#include <io.h>
#include <fcntl.h>

int __getmode(int __handle);
int __readbuf(FILE* stream);

char* _RTL_FUNC fgets(char* restrict buf, int num, FILE* restrict stream)
{
    flockfile(stream);
    char* rv = fgets_unlocked(buf, num, stream);
    funlockfile(stream);
    return rv;
}
char* _RTL_FUNC fgets_unlocked(char* restrict buf, int num, FILE* restrict stream)
{
    int i = 0, rv = 0;
    if (stream->token != FILTOK)
    {
        errno = _dos_errno = ENOENT;
        return 0;
    }
    if (stream->extended->orient == __or_wide)
    {
        errno = EINVAL;
        return 0;
    }
    stream->extended->orient = __or_narrow;
    stream->flags &= ~_F_VBUF;
    if (!(stream->flags & _F_READ))
    {
        stream->flags |= _F_ERR;
        errno = EFAULT;
        return 0;
    }
    if (stream == stdin)
        fflush(stdout);
    if (!(stream->flags & _F_IN))
    {
        if (stream->flags & _F_OUT)
        {
            if (fflush(stream))
                return 0;
        }
        if (stream->flags & _F_BUFFEREDSTRING)
        {
            if (stream->flags & _F_OUT)
                stream->level = -stream->level;
        }
        else
        {
            stream->level = 0;
        }
        stream->flags &= ~_F_OUT;
        stream->flags |= _F_IN;
    }
    if (stream->flags & _F_EOF)
        if (isatty(fileno(stream)))
            stream->flags &= ~(_F_XEOF | _F_EOF);
        else
            return 0;
    if (num == 0)
        return 0;
    if (stream->flags & _F_UNGETC)
    {
        rv = buf[i++] = stream->hold;
        stream->hold = 0;
        stream->flags &= ~_F_UNGETC;
    }
    if (rv != '\n')
    {
        for (; i < num - 1;)
        {
            if (--stream->level <= 0)
            {
                if (stream->flags & _F_BUFFEREDSTRING)
                {
                    stream->flags |= _F_EOF;
                    rv = EOF;
                    break;
                }
                rv = __readbuf(stream);
                if (rv)
                    break;
            }
            rv = *stream->curp++;
            if (rv != '\r' || (__getmode(fileno(stream)) & O_BINARY))
                buf[i++] = rv;
            if (rv == '\n')
            {
                break;
            }
        }
    }
    if (rv == EOF && i == 0)
        return 0;
    buf[i] = 0;
    return buf;
}
