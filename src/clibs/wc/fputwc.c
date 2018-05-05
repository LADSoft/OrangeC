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
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <io.h>
#include <errno.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

int _RTL_FUNC __fputwc(int c, FILE *stream)
{
	if (stream->buffer) {
		if (stream->level < 0)
		{
			++stream->level;
			*stream->curp++ = (unsigned char)(c);
		}
		if (!(stream->flags & _F_BUFFEREDSTRING))
		{
			if (stream->level >= 0) {
				fflush(stream);
				stream->flags &= ~_F_IN;
				stream->flags |= _F_OUT;
				stream->level = -stream->bsize;
				stream->curp = stream->buffer;
	        }
		}
		if (stream->level < 0)
		{
			++stream->level;
			*stream->curp++ = (unsigned char)(c >> 8);
		}
		if (!(stream->flags & _F_BUFFEREDSTRING))
		{
			if (c == '\n' && (stream->flags & _F_LBUF) || stream->level >=0) {
				fflush(stream);
				stream->flags &= ~_F_IN;
				stream->flags |= _F_OUT;
				stream->level = -stream->bsize;
				stream->curp = stream->buffer;
	        }
		}
	}		
	else {
        char cl;
        if (stream->flags & _F_APPEND)
            if (__ll_seek(fileno(stream),0,SEEK_END)) {
                stream->flags |= _F_ERR;
                errno = EIO;
                return WEOF;
            }
		if (!(stream->flags & _F_BIN) && c == '\n') {
            cl = 13;
			if (write(fileno(stream),&cl,1) < 0) {
				stream->flags |= _F_ERR;
				errno = EIO;
				return WEOF;
			}
            cl = 0;
			if (write(fileno(stream),&cl,1) < 0) {
				stream->flags |= _F_ERR;
				errno = EIO;
				return WEOF;
			}
		}
        cl = c & 0xff;
		if (write(fileno(stream),&cl,1) < 0) {
			stream->flags |= _F_ERR;
			errno = EIO;
			return WEOF;
		}
        cl = c >> 8;
		if (write(fileno(stream),&cl,1) < 0) {
			stream->flags |= _F_ERR;
			errno = EIO;
			return WEOF;
		}
	}
    return c;
}
wint_t _RTL_FUNC fputwc(wchar_t c, FILE *stream)
{
	int rv;
	if (stream->token != FILTOK) {
        errno = _dos_errno = ENOENT;
		return WEOF;
    }
    if (stream->extended->orient == __or_narrow) {
        errno = EINVAL;
        return WEOF;
    }
    stream->extended->orient = __or_wide;
    stream->flags &= ~_F_VBUF;
	if (!(stream->flags & _F_WRIT)) {
		stream->flags |= _F_ERR;
		errno = EFAULT;
		return WEOF;
	}
	if ((stream->flags & _F_IN) || 
			stream->buffer && ( stream->flags & _F_OUT) 
				&& stream->level >= 0) {
		if (fflush(stream))
			return WEOF;
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
	return __fputwc(c,stream);
}
wint_t _RTL_FUNC _fputwc(wchar_t c, FILE *stream)
{
	return fputwc(c,stream);
}
wint_t _RTL_FUNC (putwc)(wchar_t c, FILE *stream)
{
	return fputwc(c,stream);
}
wint_t _RTL_FUNC (putwchar)(wchar_t c)
{
	return fputwc(c,stdout);
}
