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
namespace std {
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

int  _RTL_FUNC  access  (const char *__path, int __amode);
int  _RTL_FUNC   chmod   (const char *__path, int __amode);
int  _RTL_FUNC   chsize  (int __handle, long __size);
int  _RTL_FUNC  close   (int __handle);
int  _RTL_FUNC  creat   (const char *__path, int __amode);
int  _RTL_FUNC   dup   (int __handle);
int  _RTL_FUNC   dup2  (int __oldhandle, int __newhandle);
int  _RTL_FUNC  eof     (int __handle);
long _RTL_FUNC  filelength(int __handle);
int  _RTL_FUNC   getftime(int __handle, struct ftime *__ftimep);

int  _RTL_FUNC   isatty(int __handle);
int  _RTL_FUNC     lock  (int __handle, long __offset, long __length);
int  _RTL_FUNC     locking(int __handle, int __mode, long __length);
long _RTL_FUNC  lseek (int __handle, long __offset, int __fromwhere);
char * _RTL_FUNC  mktemp( char *__template );
int  _RTL_FUNC    mkstemp (char * __template);
int  _RTL_FUNC     open  (const char *__path, int __access,... /*unsigned mode*/);
int  _RTL_FUNC    read  (int __handle, void *__buf, unsigned __len);
int  _RTL_FUNC  remove(const char *__path);
int  _RTL_FUNC    rename(const char *__oldname,const char *__newname);
int  _RTL_FUNC   setftime(int __handle, struct ftime *__ftimep);
int  _RTL_FUNC     setmode(int __handle, int __amode);

int  _RTL_FUNC   sopen (const char *__path, int __access, int __shflag,
                      ... /* unsigned mode */);
int  _RTL_FUNC  rmdir(const char *__path);
long _RTL_FUNC   tell  (int __handle);
int  _RTL_FUNC  umask(int perm);
int  _RTL_FUNC  unlink(const char *__path);
int  _RTL_FUNC     unlock(int __handle, long __offset, long __length);
int  _RTL_FUNC    write (int __handle, const void *__buf, unsigned __len);

long _RTL_FUNC _get_osfhandle(int __handle);
int  _RTL_FUNC _open_osfhandle(long __osfhandle, int __oflag);

int _RTL_FUNC _access(const char *, int);
int _RTL_FUNC _chmod(const char *, int);
int _RTL_FUNC _chsize(int, long);
int _RTL_FUNC _close(int);
int _RTL_FUNC _creat(const char *, int);
int _RTL_FUNC _dup(int);
int _RTL_FUNC _dup2(int, int);
int _RTL_FUNC _eof(int);
long _RTL_FUNC _filelength(int);
long _RTL_FUNC _findfirst(const char *, struct _finddata_t *);
int _RTL_FUNC _findnext(long, struct _finddata_t *);
int _RTL_FUNC _findclose(long);
int _RTL_FUNC _isatty(int);
int _RTL_FUNC _locking(int, int, long);
long _RTL_FUNC _lseek(int, long, int);
char * _RTL_FUNC _mktemp(char *);
int _RTL_FUNC _open(const char *, int, ...);
int _RTL_FUNC _read(int, void *, unsigned int);
int _RTL_FUNC _setmode(int, int);
int _RTL_FUNC _sopen(const char *, int, int, ...);
long _RTL_FUNC _tell(int);
int  _RTL_FUNC _umask(int perm);
int _RTL_FUNC _unlink(const char *);
int _RTL_FUNC _write(int, const void *, unsigned int);

#ifdef __cplusplus
} ;
} ;
#endif

#pragma pack()

#endif  /* __IO_H */
#if defined(__cplusplus) && !defined (__USING_CNAME__) && !defined(__IO_H_USING_LIST)
#define IO_H_USING_LIST
        using std::access;
        using std::chmod;
        using std::chsize;
        using std::close;
        using std::creat;
        using std::dup;
        using std::dup2;
        using std::eof;
        using std::filelength;
        using std::getftime;
        using std::isatty;
        using std::lock;
        using std::locking;
        using std::lseek;
        using std::mktemp;
        using std::open;
        using std::read;
        using std::remove;
        using std::rename;
        using std::setftime;
        using std::setmode;
        using std::sopen;
        using std::tell;
        using std::umask;
        using std::unlink;
        using std::unlock;
        using std::write;
        using std::_findfirst;
        using std::_findnext;
        using std::_findclose;
        using std::_get_osfhandle;
        using std::_open_osfhandle;
    using std::ftime;
#ifndef _USING_NFILE
#define _USING_NFILE
    using std::_nfile ;
#endif
    using std::_finddata_t;
        using std::_access;
        using std::_chmod;
        using std::_chsize;
        using std::_close;
        using std::_creat;
        using std::_dup;
        using std::_dup2;
        using std::_eof;
        using std::_filelength;
        using std::_isatty;
        using std::_locking;
        using std::_lseek;
        using std::_mktemp;
        using std::_open;
        using std::_read;
        using std::_setmode;
        using std::_sopen;
        using std::_tell;
        using std::_umask;
        using std::_unlink;
        using std::_write;
#endif
