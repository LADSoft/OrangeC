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

int _RTL_FUNC (ungetc)(int c, FILE *stream)
{
	if (stream->token != FILTOK) {
		errno = _dos_errno = ENOENT;
		return EOF;
	}
    if (stream->extended->orient == __or_wide) {
        errno = EINVAL;
        return EOF;
    }
    stream->extended->orient = __or_narrow;
    stream->flags &= ~_F_VBUF;
    if (c == EOF)
        return EOF;
	if (stream->flags & _F_UNGETC) {
		if (!stream->buffer || stream->level == stream->bsize) {
			errno = _dos_errno = ENOSPC;
			return EOF;
		}
		stream->level++;
		*--stream->curp = stream->hold;
	}
	stream->hold = (char)c;
    stream->flags &= ~_F_EOF;
	stream->flags |= _F_UNGETC;
	return c;
}
int _RTL_FUNC _ungetc(int c, FILE *stream)
{
	return ungetc(c, stream);
}
