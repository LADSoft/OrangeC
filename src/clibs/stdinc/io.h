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

/*  io.h

    Definitions for low level I/O functions.

*/

#ifndef __IO_H
#define __IO_H

#pragma pack(1)

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifndef __TIME_H
#include <time.h>
#endif

#define _NFILE_ 40 /* SHOULD BE SAME AS IN STDIO.H */

#define FILENAME_MAX 265

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

extern  unsigned    _RTL_DATA  _nfile;

#define HANDLE_MAX   (_NFILE_)

struct  ftime   {
    unsigned    ft_tsec  : 5;   /* Two second interval */
    unsigned    ft_min   : 6;   /* Minutes */
    unsigned    ft_hour  : 5;   /* Hours */
    unsigned    ft_day   : 5;   /* Days */
    unsigned    ft_month : 4;   /* Months */
    unsigned    ft_year  : 7;   /* Year */
};

#define _A_NORMAL   0x00        /* Normal file, no attributes */
#define _A_RDONLY   0x01        /* Read only attribute */
#define _A_HIDDEN   0x02        /* Hidden file */
#define _A_SYSTEM   0x04        /* System file */
#define _A_VOLID    0x08        /* Volume label */
#define _A_SUBDIR   0x10        /* Directory */
#define _A_ARCH     0x20        /* Archive */

struct _finddata_t
{
 unsigned attrib;  /* Attributes, see constants above. */
 time_t  time_create;
 time_t  time_access; /* always midnight local time */
 time_t  time_write;
 unsigned long size;
 char  name[FILENAME_MAX]; /* may include spaces. */
};


#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0

#define F_OK    0
#define X_OK    1
#define W_OK    2
#define R_OK    4

int  _RTL_FUNC _IMPORT  access  (const char *__path, int __amode);
int  _RTL_FUNC _IMPORT   chmod   (const char *__path, int __amode);
int  _RTL_FUNC _IMPORT   chsize  (int __handle, long __size);
int  _RTL_FUNC _IMPORT  close   (int __handle);
int  _RTL_FUNC _IMPORT  creat   (const char *__path, int __amode);
int  _RTL_FUNC _IMPORT   dup   (int __handle);
int  _RTL_FUNC _IMPORT   dup2  (int __oldhandle, int __newhandle);
int  _RTL_FUNC _IMPORT  eof     (int __handle);
long _RTL_FUNC _IMPORT  filelength(int __handle);
int  _RTL_FUNC _IMPORT   getftime(int __handle, struct ftime *__ftimep);

int  _RTL_FUNC _IMPORT   isatty(int __handle);
int  _RTL_FUNC _IMPORT     lock  (int __handle, long __offset, long __length);
int  _RTL_FUNC _IMPORT     locking(int __handle, int __mode, long __length);
long _RTL_FUNC _IMPORT  lseek (int __handle, long __offset, int __fromwhere);
char * _RTL_FUNC _IMPORT  mktemp( char *__template );
int  _RTL_FUNC _IMPORT    mkstemp (char * __template);
int  _RTL_FUNC _IMPORT     open  (const char *__path, int __access,... /*unsigned mode*/);
int  _RTL_FUNC _IMPORT    read  (int __handle, void *__buf, unsigned __len);
int  _RTL_FUNC _IMPORT  remove(const char *__path);
int  _RTL_FUNC _IMPORT    rename(const char *__oldname,const char *__newname);
int  _RTL_FUNC _IMPORT   setftime(int __handle, struct ftime *__ftimep);
int  _RTL_FUNC _IMPORT     setmode(int __handle, int __amode);

int  _RTL_FUNC _IMPORT   sopen (const char *__path, int __access, int __shflag,
                      ... /* unsigned mode */);
int  _RTL_FUNC _IMPORT  rmdir(const char *__path);
long _RTL_FUNC _IMPORT   tell  (int __handle);
int  _RTL_FUNC _IMPORT  umask(int perm);
int  _RTL_FUNC _IMPORT  unlink(const char *__path);
int  _RTL_FUNC _IMPORT     unlock(int __handle, long __offset, long __length);
int  _RTL_FUNC _IMPORT    write (int __handle, const void *__buf, unsigned __len);

long _RTL_FUNC _IMPORT _get_osfhandle(int __handle);
int  _RTL_FUNC _IMPORT _open_osfhandle(long __osfhandle, int __oflag);

int _RTL_FUNC _IMPORT _access(const char *, int);
int _RTL_FUNC _IMPORT _chmod(const char *, int);
int _RTL_FUNC _IMPORT _chsize(int, long);
int _RTL_FUNC _IMPORT _close(int);
int _RTL_FUNC _IMPORT _creat(const char *, int);
int _RTL_FUNC _IMPORT _dup(int);
int _RTL_FUNC _IMPORT _dup2(int, int);
int _RTL_FUNC _IMPORT _eof(int);
long _RTL_FUNC _IMPORT _filelength(int);
long _RTL_FUNC _IMPORT _findfirst(const char *, struct _finddata_t *);
int _RTL_FUNC _IMPORT _findnext(long, struct _finddata_t *);
int _RTL_FUNC _IMPORT _findclose(long);
int _RTL_FUNC _IMPORT _isatty(int);
int _RTL_FUNC _IMPORT _locking(int, int, long);
long _RTL_FUNC _IMPORT _lseek(int, long, int);
char * _RTL_FUNC _IMPORT _mktemp(char *);
int _RTL_FUNC _IMPORT _open(const char *, int, ...);
int _RTL_FUNC _IMPORT _read(int, void *, unsigned int);
int _RTL_FUNC _IMPORT _setmode(int, int);
int _RTL_FUNC _IMPORT _sopen(const char *, int, int, ...);
long _RTL_FUNC _IMPORT _tell(int);
int  _RTL_FUNC _IMPORT _umask(int perm);
int _RTL_FUNC _IMPORT _unlink(const char *);
int _RTL_FUNC _IMPORT _write(int, const void *, unsigned int);

#ifdef __cplusplus
} ;
} ;
#endif

#pragma pack()

#endif  /* __IO_H */
#if defined(__cplusplus) && !defined (__USING_CNAME__) && !defined(__IO_H_USING_LIST)
#define IO_H_USING_LIST
        using __STD_NS_QUALIFIER access;
        using __STD_NS_QUALIFIER chmod;
        using __STD_NS_QUALIFIER chsize;
        using __STD_NS_QUALIFIER close;
        using __STD_NS_QUALIFIER creat;
        using __STD_NS_QUALIFIER dup;
        using __STD_NS_QUALIFIER dup2;
        using __STD_NS_QUALIFIER eof;
        using __STD_NS_QUALIFIER filelength;
        using __STD_NS_QUALIFIER getftime;
        using __STD_NS_QUALIFIER isatty;
        using __STD_NS_QUALIFIER lock;
        using __STD_NS_QUALIFIER locking;
        using __STD_NS_QUALIFIER lseek;
        using __STD_NS_QUALIFIER mktemp;
        using __STD_NS_QUALIFIER open;
        using __STD_NS_QUALIFIER read;
        using __STD_NS_QUALIFIER remove;
        using __STD_NS_QUALIFIER rename;
        using __STD_NS_QUALIFIER setftime;
        using __STD_NS_QUALIFIER setmode;
        using __STD_NS_QUALIFIER sopen;
        using __STD_NS_QUALIFIER tell;
        using __STD_NS_QUALIFIER umask;
        using __STD_NS_QUALIFIER unlink;
        using __STD_NS_QUALIFIER unlock;
        using __STD_NS_QUALIFIER write;
        using __STD_NS_QUALIFIER _findfirst;
        using __STD_NS_QUALIFIER _findnext;
        using __STD_NS_QUALIFIER _findclose;
        using __STD_NS_QUALIFIER _get_osfhandle;
        using __STD_NS_QUALIFIER _open_osfhandle;
    using __STD_NS_QUALIFIER ftime;
#ifndef _USING_NFILE
#define _USING_NFILE
    using __STD_NS_QUALIFIER _nfile ;
#endif
    using __STD_NS_QUALIFIER _finddata_t;
        using __STD_NS_QUALIFIER _access;
        using __STD_NS_QUALIFIER _chmod;
        using __STD_NS_QUALIFIER _chsize;
        using __STD_NS_QUALIFIER _close;
        using __STD_NS_QUALIFIER _creat;
        using __STD_NS_QUALIFIER _dup;
        using __STD_NS_QUALIFIER _dup2;
        using __STD_NS_QUALIFIER _eof;
        using __STD_NS_QUALIFIER _filelength;
        using __STD_NS_QUALIFIER _isatty;
        using __STD_NS_QUALIFIER _locking;
        using __STD_NS_QUALIFIER _lseek;
        using __STD_NS_QUALIFIER _mktemp;
        using __STD_NS_QUALIFIER _open;
        using __STD_NS_QUALIFIER _read;
        using __STD_NS_QUALIFIER _setmode;
        using __STD_NS_QUALIFIER _sopen;
        using __STD_NS_QUALIFIER _tell;
        using __STD_NS_QUALIFIER _umask;
        using __STD_NS_QUALIFIER _unlink;
        using __STD_NS_QUALIFIER _write;
#endif
