/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

/*  io.h

    Definitions for low level I/O functions.

*/

#pragma GCC system_header

#ifndef __IO_H
#define __IO_H

#pragma pack(1)

#    include <stddef.h>
#    include <time.h>

#define _NFILE_ 40 /* SHOULD BE SAME AS IN STDIO.H */

#define FILENAME_MAX 265

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef RC_INVOKED
    extern unsigned _RTL_DATA _nfile;
#endif

#define HANDLE_MAX (_NFILE_)

#ifndef RC_INVOKED
    struct ftime
    {
        unsigned ft_tsec : 5;  /* Two second interval */
        unsigned ft_min : 6;   /* Minutes */
        unsigned ft_hour : 5;  /* Hours */
        unsigned ft_day : 5;   /* Days */
        unsigned ft_month : 4; /* Months */
        unsigned ft_year : 7;  /* Year */
    };
#endif
#define _A_NORMAL 0x00 /* Normal file, no attributes */
#define _A_RDONLY 0x01 /* Read only attribute */
#define _A_HIDDEN 0x02 /* Hidden file */
#define _A_SYSTEM 0x04 /* System file */
#define _A_VOLID 0x08  /* Volume label */
#define _A_SUBDIR 0x10 /* Directory */
#define _A_ARCH 0x20   /* Archive */

#ifndef RC_INVOKED
    struct _finddata_t
    {
        unsigned attrib; /* Attributes, see constants above. */
        time_t time_create;
        time_t time_access; /* always midnight local time */
        time_t time_write;
        unsigned long size;
        char name[FILENAME_MAX]; /* may include spaces. */
    };
#endif
#define SEEK_CUR 1
#define SEEK_END 2
#define SEEK_SET 0

#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4
#ifndef RC_INVOKED

    int _RTL_FUNC _IMPORT access(const char* ZSTR __path, int __amode);
    int _RTL_FUNC _IMPORT chsize(int __handle, long __size);
    int _RTL_FUNC _IMPORT close(int __handle);
    int _RTL_FUNC _IMPORT creat(const char* ZSTR __path, int __amode);
    int _RTL_FUNC _IMPORT dup(int __handle);
    int _RTL_FUNC _IMPORT dup2(int __oldhandle, int __newhandle);
    int _RTL_FUNC _IMPORT eof(int __handle);
    long _RTL_FUNC _IMPORT filelength(int __handle);
    int _RTL_FUNC _IMPORT getftime(int __handle, struct ftime* __ftimep);

    int _RTL_FUNC _IMPORT isatty(int __handle);
    int _RTL_FUNC _IMPORT lock(int __handle, long __offset, long __length);
    int _RTL_FUNC _IMPORT locking(int __handle, int __mode, long __length);
    long _RTL_FUNC _IMPORT lseek(int __handle, long __offset, int __fromwhere);
    char* ZSTR _RTL_FUNC _IMPORT mktemp(char* ZSTR __template);
    int _RTL_FUNC _IMPORT mkstemp(char* ZSTR __template);
    int _RTL_FUNC _IMPORT open(const char* ZSTR __path, int __access, ... /*unsigned mode*/);
    int _RTL_FUNC _IMPORT read(int __handle, void* __buf, unsigned __len);
    int _RTL_FUNC _IMPORT remove(const char* ZSTR __path);
    int _RTL_FUNC _IMPORT rename(const char* ZSTR __oldname, const char* ZSTR __newname);
    int _RTL_FUNC _IMPORT setftime(int __handle, struct ftime* __ftimep);
    int _RTL_FUNC _IMPORT setmode(int __handle, int __amode);

    int _RTL_FUNC _IMPORT sopen(const char* ZSTR __path, int __access, int __shflag, ... /* unsigned mode */);
    int _RTL_FUNC _IMPORT _wsopen(const wchar_t* ZSTR __path, int __access, int __shflag, ... /* unsigned mode */);
    int _RTL_FUNC _IMPORT rmdir(const char* ZSTR __path);
    long _RTL_FUNC _IMPORT tell(int __handle);
    int _RTL_FUNC _IMPORT umask(int perm);
    int _RTL_FUNC _IMPORT unlink(const char* ZSTR __path);
    int _RTL_FUNC _IMPORT unlock(int __handle, long __offset, long __length);
    int _RTL_FUNC _IMPORT write(int __handle, const void* __buf, unsigned __len);

    long _RTL_FUNC _IMPORT _get_osfhandle(int __handle);
    int _RTL_FUNC _IMPORT _open_osfhandle(long __osfhandle, int __oflag);

    int _RTL_FUNC _IMPORT _access(const char* ZSTR, int);
    int _RTL_FUNC _IMPORT _waccess(const wchar_t* ZSTR __path, int __amode);
    int _RTL_FUNC _IMPORT _chmod(const char* ZSTR, int);
    int _RTL_FUNC _IMPORT _wchmod(const wchar_t* ZSTR, int);
    int _RTL_FUNC _IMPORT _chsize(int, long);
    int _RTL_FUNC _IMPORT _close(int);
    int _RTL_FUNC _IMPORT _commit(int);
    int _RTL_FUNC _IMPORT _creat(const char* ZSTR, int);
    int _RTL_FUNC _IMPORT _dup(int);
    int _RTL_FUNC _IMPORT _dup2(int, int);
    int _RTL_FUNC _IMPORT _eof(int);
    long _RTL_FUNC _IMPORT _filelength(int);
    long _RTL_FUNC _IMPORT _findfirst(const char* ZSTR, struct _finddata_t*);
    int _RTL_FUNC _IMPORT _findnext(long, struct _finddata_t*);
    int _RTL_FUNC _IMPORT _findclose(long);
    int _RTL_FUNC _IMPORT _isatty(int);
    int _RTL_FUNC _IMPORT _locking(int, int, long);
    long _RTL_FUNC _IMPORT _lseek(int, long, int);
    __int64 _RTL_FUNC _IMPORT _lseeki64(int, __int64, int);
    char* ZSTR _RTL_FUNC _IMPORT _mktemp(char* ZSTR);
    int _RTL_FUNC _IMPORT _open(const char* ZSTR, int, ...);
    int _RTL_FUNC _IMPORT _read(int, void*, unsigned int);
    int _RTL_FUNC _IMPORT _setmode(int, int);
    int _RTL_FUNC _IMPORT _sopen(const char* ZSTR, int, int, ...);
    long _RTL_FUNC _IMPORT _tell(int);
    int _RTL_FUNC _IMPORT _umask(int perm);
    int _RTL_FUNC _IMPORT _unlink(const char* ZSTR);
    int _RTL_FUNC _IMPORT _write(int, const void*, unsigned int);
#endif

#ifdef __cplusplus
};
#endif

#pragma pack()

#endif /* __IO_H */
