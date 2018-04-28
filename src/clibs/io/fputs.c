/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include <io.h>
#include "libp.h"


#undef putc
int _RTL_FUNC fputs(const char *restrict string, FILE *restrict stream)
{
    int rv, l = strlen(string);
    if (stream->token != FILTOK) {
        errno = _dos_errno = ENOENT;
        return EOF;
    }
    if (stream->extended->orient == __or_wide) {
        errno = EINVAL;
        return EOF;
    }
    stream->extended->orient = __or_narrow;
    if (!(stream->flags & _F_WRIT)) {
        stream->flags |= _F_ERR;
        errno = EFAULT;
        return EOF;
    }
    stream->flags &= ~_F_VBUF;
    if ((stream->flags & _F_IN) || 
            stream->buffer && ( stream->flags & _F_OUT) 
                && stream->level >= 0) {
        if (fflush(stream))
            return EOF;
        goto join;
    }
    else {
        if (!(stream->flags & _F_OUT)) {
join:
            stream->flags &= ~_F_IN;
            stream->flags |= _F_OUT;
            stream->level = -stream->bsize;
            stream->curp = stream->buffer;
        }
    }
    if (stream->buffer) {
        char *pos = stream->curp ;
        while (l && stream->level < 0) {
            int v = l ;
            if (v > -stream->level)
                v = -stream->level ;
            memcpy(stream->curp,string,v) ;
            l -= v ;
            string += v ;
            stream->level += v ;
            stream->curp += v ;
            if (!(stream->flags & _F_BUFFEREDSTRING) && stream->level >= 0) {
                if (fflush(stream))
                    return EOF ;
                stream->flags &= ~_F_IN;
                stream->flags |= _F_OUT;
                stream->level = -stream->bsize;
                stream->curp = stream->buffer;
                pos = stream->curp ;
            }
        }
        if (!(stream->flags & _F_BUFFEREDSTRING) && (stream->flags & _F_LBUF)) {
            while (pos != stream->curp) {
                if (*pos++ == '\n') {
                    if (fflush(stream))
                        return EOF ;
                    stream->flags &= ~_F_IN;
                    stream->flags |= _F_OUT;
                    stream->level = -stream->bsize;
                    stream->curp = stream->buffer;
                    break ;
                }
            }
        }
    }		
    else {
        if (write(fileno(stream),string,l) < 0) {
            stream->flags |= _F_ERR;
            errno = EIO;
            return EOF;
        }
        if (eof(fileno(stream)))
            stream->flags |= _F_EOF;
    }
    return 0;
}