/* 
   Copyright 1994-2003 Free Software Foundation, Inc.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
   USA

   You may contact the author at:

   mailto::camille@bluegrass.net

   or by snail mail at:

   David Lindauer
   850 Washburn Ave Apt 99
   Louisville, KY 40222
*/
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <libp.h>


int _RTL_FUNC fputc(int c, FILE *stream)
{
	int rv;
	if (stream->token != FILTOK)
		return EOF;
	if (!(stream->flags & _F_WRIT)) {
		stream->flags |= _F_ERR;
		errno = EFAULT;
		return EOF;
	}
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
		++stream->level;
		*stream->curp++ = (unsigned char)c;
		if (c == '\n' && (stream->flags & _F_LBUF))
			fflush(stream);
	}		
	else {
		if (!(stream->flags & _F_BIN) && c == '\n') {
			int c1 = 13;
			if (_ll_write(stream->fd,&c1,1) < 0) {
				stream->flags |= _F_ERR;
				errno = EIO;
				return EOF;
			}
		}
		if (_ll_write(stream->fd,&c,1) < 0) {
			stream->flags |= _F_ERR;
			errno = EIO;
			return EOF;
		}
	}
	return 0;
}
int _RTL_FUNC _fputc(int c, FILE *stream)
{
	stream->level--;
	return fputc(c,stream);
}
#undef putc
#undef putchar
int _RTL_FUNC putc(int c, FILE *stream)
{
	return fputc(c,stream);
}
int _RTL_FUNC putchar(int c)
{
	return fputc(c,stdout);
}