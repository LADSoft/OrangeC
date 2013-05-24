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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <libp.h>

#undef putc
int _RTL_FUNC fputs(const char *string, FILE *stream)
{
	int rv, l = strlen(string);
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
		char *pos = stream->curp ;
		while (l) {
			int v = l ;
			if (v > -stream->level)
				v = -stream->level ;
			memcpy(stream->curp,string,v) ;
			l -= v ;
			string += v ;
			stream->level += v ;
			stream->curp += v ;
			if (stream->level >= 0) {
				if (fflush(stream))
					return EOF ;
				stream->flags &= ~_F_IN;
				stream->flags |= _F_OUT;
				stream->level = -stream->bsize;
				stream->curp = stream->buffer;
				pos = stream->curp ;
			}
		}
		if (stream->flags & _F_LBUF) {
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
		if (stream->flags & _F_BIN) {
			if (_ll_write(stream->fd,string,l) < 0) {
				stream->flags |= _F_ERR;
				errno = EIO;
				return EOF;
			}
		} else {
			int offset = 0 ;
			char *s = string ;
			while (l) {
				int v ;
				s = strchr(s+offset,'\n') ;
				if (s) {
					v = s-string ;
					offset = 1 ;
				} else
					v = l ;
					
				if (_ll_write(stream->fd,string,v) < 0) {
					stream->flags |= _F_ERR;
					errno = EIO;
					return EOF;
				}
				l -= v ;
				if (s) {
					char cl = '\r' ;
					if (_ll_write(stream->fd,&cl,1) < 0) {
						stream->flags |= _F_ERR;
						errno = EIO;
						return EOF;
					}
				}
				string = s ;
			}
		}
	}
	return 0;
}