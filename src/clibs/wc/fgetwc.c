/*
	Software License Agreement (BSD License)
	
	Copyright (c) 1997-2008, David Lindauer, (LADSoft).
	All rights reserved.
	
	Redistribution and use of this software in source and binary forms, with or without modification, are
	permitted provided that the following conditions are met:
	
	* Redistributions of source code must retain the above
	  copyright notice, this list of conditions and the
	  following disclaimer.
	
	* Redistributions in binary form must reproduce the above
	  copyright notice, this list of conditions and the
	  following disclaimer in the documentation and/or other
	  materials provided with the distribution.
	
	* Neither the name of LADSoft nor the names of its
	  contributors may be used to endorse or promote products
	  derived from this software without specific prior
	  written permission of LADSoft.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
	PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
	TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

static int __wreadbuf(FILE *stream)
{
	int sz,sz2;
	int handle;
	if (stream->flags & _F_XEOF) {
		stream->flags |= _F_EOF ;
		return EOF ;
	}
	handle = __uiohandle(fileno(stream));
	if (handle == -1)
	{
		stream->flags |= _F_EOF ;
		return EOF;
	}
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
	stream->level = sz = __ll_read(handle,stream->curp,sz2);
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
				unsigned char *p = stream->buffer ;
				int i ;
				for (i=0; i < stream->level && *p != 0x1a; i++) p++ ;
				stream->level = i;
				if (*p == 0x1a)
					stream->flags |= _F_XEOF ;
			} else
				if (stream->hold == 0x1a) {
					stream->flags |= _F_EOF ;
					return EOF ;
				}
		}
	}
	return 0;
}

int _RTL_FUNC __wgetch(FILE *stream)
{
    int rv ;
		if (--stream->level <= 0) {
	      if (stream->flags & _F_BUFFEREDSTRING) {
	         stream->flags |= _F_EOF ;
			 return WEOF ;
	      }
			rv = __wreadbuf(stream);
			if (rv)
                return WEOF ;
		}
        rv = *stream->curp++;
        return rv ;
}
int _RTL_FUNC __wgetc(FILE *stream)
{
    int one,two ;
    one = __wgetch(stream);
    if (one == WEOF)
        return one;
    two = __wgetch(stream);
    if (two == WEOF) {
        errno = EILSEQ;
        return WEOF;
    }
#ifndef __BIG_ENDIAN
    if (stream->flags & _F_BUFFEREDSTRING) {
        return (two << 8) + one;
    }
#endif
    return (one << 8) + two;
}
wint_t _RTL_FUNC fgetwc(FILE *stream)
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
    if (stream->flags & _F_EOF)
        return WEOF;
	if (stream == stdin) {
		fflush(stdout);
    }
	if (stream->hold) {
		rv = stream->hold;
		stream->hold = 0;
		return rv;
	}
    if (!(stream->flags & _F_IN)) {
		if (stream->flags & _F_OUT) {
			if (fflush(stream))
				return 0;
		}
		stream->flags &= ~_F_OUT;
		stream->flags |= _F_IN;
		stream->level = 0;
	}
    do {
        rv = __wgetc(stream);
    } while (rv != WEOF && (rv == '\r' && !(stream->flags & _F_BIN)));
    return rv ;
}
wint_t _RTL_FUNC _fgetwc(FILE *stream)
{
	return fgetwc(stream);
}
wint_t _RTL_FUNC (getwc)(FILE *stream)
{
	return fgetwc(stream);
}
wint_t _RTL_FUNC (getwchar)(void)
{
	return fgetwc(stdin);
}
