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
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <sys\stat.h>
#include "libp.h"

extern FILE *_pstreams[_NFILE_];
extern int __maxfiles;

static int cvFlags(int flags)
{
	int rv = 0;
	switch (flags & (_F_READ | _F_WRIT))
	{
		case _F_READ:
		case 0:
			rv = O_RDONLY;
			break;
		case _F_WRIT:
			rv = O_WRONLY;
			break;
		case _F_READ | _F_WRIT:
			rv = O_RDWR;
			break;
	}
	if (flags & _F_BIN)
		rv |= O_BINARY;
	else
		rv |= O_TEXT;
	if (flags & _F_APPEND)
		rv |= O_APPEND;
	return rv;
}
FILE *__basefopen(const char *restrict name, const char *restrict mode,
        FILE *restrict stream, int fd, int share)
{
	int flags = 0,append = 0, update = 0, id = 0,i;
	FILE *file;
	char *fname;
   __ll_enter_critical() ;
	if (__maxfiles >= _NFILE_) {
		_dos_errno = errno = EMFILE;
      __ll_exit_critical() ;
		return 0;
	}
	for (i=0; i < strlen(mode); i++) {
		switch (mode[i]) {
			case 'r':
				flags |= _F_READ;
				break;
			case 'w':
				flags |= _F_WRIT;
				break;
			case 'a':
				flags |= _F_WRIT;
				append = 1;
				break;
			case '+':
				update = 1;
				break;
			case 'b':
				flags |= _F_BIN;
				break;
			case 't':
				flags &= ~_F_BIN;
				break;
		}
	}
   if (!(flags & (_F_READ | _F_WRIT))) {
      __ll_exit_critical() ;
		return 0;
   }
	fname = malloc(strlen(name)+1);
   if (!fname) {
      __ll_exit_critical() ;
		return 0;
   }
	strcpy(fname,name);
	if (stream)
		file = stream;
	else
		if ((file = malloc(sizeof(FILE))) == 0) {
			free(fname);
            __ll_exit_critical() ;
			return 0;
		}
		else
        {
			memset(file,0,sizeof(FILE));
            if ((file->extended = malloc(sizeof(struct __file2))) == 0) {
    			free(fname);
                free(file);
                __ll_exit_critical() ;
    			return 0;
            }
            memset(file->extended, 0, sizeof(file->extended));
        }
            
	file->flags = 0;
	if (append)
		flags |= _F_APPEND;
	switch (flags & (_F_READ | _F_WRIT)) {
		case 0:
			goto nofile;
		case _F_READ:
			if (update)
				flags |= _F_WRIT;
			if (fd != -1)
				id = fd;
			else
	         	id = open(name,  cvFlags(flags) | share, S_IREAD | S_IWRITE);
			break;
		case _F_WRIT:
			if (update)
				flags |= _F_READ;
			id = -1;
			if (fd != -1)
				id = fd;
			else
			{
				if (append)
	                id = open(name, cvFlags(flags) | share, S_IREAD | S_IWRITE);
				if (id < 0)
	                id = open(name, O_CREAT | O_TRUNC | cvFlags(flags) | share, S_IREAD | S_IWRITE);
			}
			break;
		case _F_READ | _F_WRIT:
         __ll_exit_critical() ;
			return 0;
	}
    if (id < 0) {
		goto nofile;
    }
    file->extended->orient = __or_unspecified;
    memset(file->extended->mbstate,0,sizeof(file->extended->mbstate));
	file->token = FILTOK;
	file->level = 0;
	file->fd = id;
	file->flags |= flags | _F_VBUF;
	file->hold = 0;
    if (!stream) {
    	if (!isatty(id))
        {
	    	if (setvbuf(file,0,_IOFBF,8192))
			{
				free(fname);
				free(file);
	         __ll_exit_critical() ;
				return 0;
			}
        }
	    else {
            file->flags |= _F_TERM;
		    if (setvbuf(file,0,_IOLBF,BUFSIZ))
			{
				free(fname);
				free(file);
	         __ll_exit_critical() ;
				return 0;
			}
        }
		file->flags |= _F_BUF;
    }
	if (append) {
		if (fseek(file,0,SEEK_END)) {
nofile:
			free(fname);
			free(file->buffer);
			free(file);
         __ll_exit_critical() ;
			return 0;
		}
	}
    file->extended->name = fname;
	_pstreams[__maxfiles++] = file;
   __ll_exit_critical() ;
	return file;
}
FILE *_RTL_FUNC fopen(const char *restrict name, const char *restrict mode)
{
	return __basefopen(name,mode,0, -1, SH_COMPAT);
}
FILE *_RTL_FUNC fdopen(int handle, const char *restrict mode)
{
	return __basefopen("", mode, 0, handle, SH_COMPAT);
}
FILE *_RTL_FUNC _fdopen(int handle, const char *restrict mode)
{
	return fdopen(handle, mode);
}
int _RTL_FUNC (fileno)(FILE *stream)
{
	if (stream->token != FILTOK)
	{
		errno = ENOENT;
		return -1;
	}
	return stream->fd;
}
int _RTL_FUNC (_fileno)(FILE *stream)
{
	if (stream->token != FILTOK)
	{
		errno = ENOENT;
		return -1;
	}
	return stream->fd;
}
FILE *_RTL_FUNC _fsopen(const char *name, const char *mode, int share)
{
	return __basefopen(name,mode,0, -1, share);
}
