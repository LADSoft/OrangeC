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

int _readbuf(FILE *stream)
{
	int sz,sz2;
	if (stream->buffer) {
		/* if a buffer, attempt to read the whole thing... */
		stream->curp = stream->buffer;
		sz2 = stream->bsize;
	}
	else {
		/* if no buffer, we blit one byte into stream->hold */
		stream->curp = &stream->hold;
		sz2 = 1;
	}
	stream->level = sz = _ll_read(stream->fd,stream->curp,sz2);
	/* get out on error */
	if (sz < 0) {
		stream->flags |= _F_ERR;
		errno = EIO;
		return EOF;
	}
	/* get out on EOF */
	else if (sz == 0) {
		stream->flags |= _F_EOF;
		return EOF;
	}
	else {
		if (!(stream->flags & _F_BIN)) {
			if (stream->buffer) {
				/* look for CTRL-Z */
				unsigned char *p = stream->buffer,*q=p;
				int len = stream->level;
				int i;
				for (i=0; i < len && *p != 0x1a; i++)
					q++ ;
				stream->level = q - stream->buffer;
			} else
				if (stream->hold == 0x1a) {
					stream->flags |= _F_EOF ;
					return EOF ;
				}
		}
	}
	return 0;
}
int _RTL_FUNC fgetc(FILE *stream)
{
	int rv;
	if (stream->token != FILTOK) {
		errno = _dos_errno = ENOENT;
		return EOF;
	}
	if (!(stream->flags & _F_READ)) {
		stream->flags |= _F_ERR;
		errno = EFAULT;
		return EOF;
	}
	if (stream == stdin)
		fflush(stdout);
   if (!(stream->flags & _F_IN)) {
		if (stream->flags & _F_OUT) {
			if (fflush(stream))
				return EOF;
		}
		stream->flags &= ~_F_OUT;
		stream->flags |= _F_IN;
		stream->level = 0;
	}
	if (stream->flags & _F_EOF)
		return EOF;
	if (stream->hold) {
		rv = stream->hold;
		stream->hold = 0;
		return rv;
	}
	do {
		if (--stream->level <= 0) {
	      if (stream->flags & _F_BUFFEREDSTRING) {
	         stream->flags |= _F_EOF ;
	         return EOF ;
	      }
			rv = _readbuf(stream);
			if (rv)
				return rv;
		}
		rv = *stream->curp++ ;
		stream->hold = 0 ;
	} while (rv == '\r' && !(stream->flags & _F_BIN)) ;
	return rv ;
}
int _RTL_FUNC _fgetc(FILE *stream)
{
	stream->level++;
	return fgetc(stream);
}
#undef getc
#undef getchar
int _RTL_FUNC getc(FILE *stream)
{
	return fgetc(stream);
}
int _RTL_FUNC getchar(void)
{
	return fgetc(stdin);
}
		