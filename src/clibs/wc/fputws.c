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
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

int _RTL_FUNC fputws(const wchar_t* string, FILE* stream)
{
    flockfile(stream);
    int rv = fputws_unlocked(string, stream);
    funlockfile(stream);
    return rv;
}
int _RTL_FUNC fputws_unlocked(const wchar_t* string, FILE* stream)
{
    int rv;
    if (stream->token != FILTOK)
        return WEOF;
    if (stream->extended->orient == __or_narrow)
    {
        errno = EINVAL;
        return WEOF;
    }
    stream->extended->orient = __or_wide;
    if (!(stream->flags & _F_WRIT))
    {
        stream->flags |= _F_ERR;
        errno = EFAULT;
        return WEOF;
    }
    stream->flags &= ~_F_VBUF;
    if ((stream->flags & _F_IN) || stream->buffer && (stream->flags & _F_OUT) && stream->level >= 0)
    {
        if (fflush(stream))
            return WEOF;
        goto join;
    }
    else
    {
        if (!(stream->flags & _F_OUT))
        {
        join:
            if (stream->flags & _F_BUFFEREDSTRING)
            {
                if (stream->flags & _F_IN)
                    stream->level = -stream->level;
            }
            else
            {
                stream->level = -stream->bsize;
                stream->curp = stream->buffer;
            }
            stream->flags &= ~_F_IN;
            stream->flags |= _F_OUT;
        }
    }
    while (*string)
        if (__fputwc(*string++, stream) == WEOF)
            return WEOF;
    return 0;
}
