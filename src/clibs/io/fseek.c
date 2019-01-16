/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *     
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include <io.h>
#include <string.h>
#include "libp.h"

int _RTL_FUNC fseek (FILE *stream, long offset, int origin)
{
    if (stream->token != FILTOK) {
        errno = ENOENT;
        return EOF;
    }
    stream->flags &= ~_F_VBUF;
    switch (origin) {
        case SEEK_CUR:
        case SEEK_SET:
        case SEEK_END:
            stream->flags &= ~_F_UNGETC;
            stream->hold = 0;
            if (fflush(stream))
            {
                return EOF;
            }
            if (lseek(fileno(stream),offset,origin) < 0) {
                stream->flags |= _F_ERR;
                errno = EIO;
                return EOF;
            }
            memset(stream->extended->mbstate,0,sizeof(stream->extended->mbstate));
            stream->level = 0;
            stream->flags &= ~(_F_EOF | _F_XEOF | _F_IN | _F_OUT) ;            
            if (origin == SEEK_END && offset >= 0)
                stream->flags |= _F_EOF | _F_XEOF;
            __uio_clearerr(fileno(stream));
            break;
        default:
            return EOF;
    }
    return 0;
}