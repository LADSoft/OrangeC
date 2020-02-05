/* Software License Agreement
 * 
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *     
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

/*  stat.h

    Definitions used for file status functions
*/

#if !defined(__STAT_H)
#define __STAT_H

#pragma pack(1)

#ifndef __DEFS_H__
#include <_defs.h>
#endif

#if !defined(__TYPES_H)
#include <sys/types.h>
#endif

#ifndef __cplusplus
#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif
#endif

/* Traditional names for bits in st_mode.
 */
#define S_IFMT   0xF000  /* file type mask */
#define S_IFDIR  0x4000  /* directory */
#define S_IFIFO  0x1000  /* FIFO special */
#define S_IFCHR  0x2000  /* character special */
#define S_IFBLK  0x3000  /* block special */
#define S_IFREG  0x8000  /* or just 0x0000, regular */
/*#define S_IREAD  0x0100  owner may read */
/*#define S_IWRITE 0x0080  owner may write */
/*#define S_IEXEC  0x0040  owner may execute <directory search> */

/* POSIX file type test macros.  The parameter is an st_mode value.
 */
#define S_ISDIR(m)  (!!((m) & S_IFDIR))
#define S_ISCHR(m)  (!!((m) & S_IFCHR))
#define S_ISBLK(m)  (!!((m) & S_IFBLK))
#define S_ISREG(m)  (!!((m) & S_IFREG))
#define S_ISFIFO(m) (!!((m) & S_IFIFO))

/* owner permission */
#define S_IRWXU 	0000700
#define S_IRUSR 	0000400
#define S_IWUSR 	0000200
#define S_IXUSR 	0000100
#define S_IREAD 	0000400
#define S_IWRITE	0000200
#define S_IEXEC 	0000100

/* group permission.  same as owner's on PC and PenPoint*/
#define S_IRWXG 	0000070
#define S_IRGRP 	0000040
#define S_IWGRP 	0000020
#define S_IXGRP 	0000010

/* other permission.  same as owner's on PC and PenPoint*/
#define S_IRWXO 	0000007
#define S_IROTH 	0000004
#define S_IWOTH 	0000002
#define S_IXOTH 	0000001

/* setuid, setgid, and sticky.	always false on PC */
#define S_ISUID 	0004000
#define S_ISGID 	0002000
#define S_ISVTX 	0001000

#ifdef __cplusplus
extern "C" {
#endif

struct  stat
{
    dev_t   st_dev;
    ino_t   st_ino;
    mode_t  st_mode;
    nlink_t st_nlink;
    uid_t   st_uid;
    gid_t   st_gid;
    dev_t   st_rdev;
    off_t   st_size;
    time_t  st_atime;
    time_t  st_mtime;
    time_t  st_ctime;
};


/* Define MS compatible names
*/
#define _S_IFMT   S_IFMT
#define _S_IFDIR  S_IFDIR
#define _S_IFIFO  S_IFIFO
#define _S_IFCHR  S_IFCHR
#define _S_IFBLK  S_IFBLK
#define _S_IFREG  S_IFREG
#define _S_IREAD  S_IREAD
#define _S_IWRITE S_IWRITE
#define _S_IEXEC  S_IEXEC

struct  _stat
{
#ifdef __CRTDLL_DLL
    short st_dev;
    short st_ino;
    short st_mode;
    int   st_nlink;
    short st_uid;
    short st_gid;
    short st_rdev;
    long  st_size;
    long  st_atime;
    long  st_mtime;
    long  st_ctime;
#else
    int st_dev;
    short st_ino;
    short st_mode;
    int   st_nlink;
    short st_uid;
    short st_gid;
    int   st_rdev;
    long  st_size;
    long  st_atime;
    long  st_mtime;
    long  st_ctime;
#endif
};


int  _RTL_FUNC _IMPORT  _fstat(int __handle, struct _stat  *__statbuf);
int  _RTL_FUNC _IMPORT  fstat(int __handle, struct stat  *__statbuf);
int  _RTL_FUNC _IMPORT  _wstat(const wchar_t *__path, struct _stat *__statbuf);
int  _RTL_FUNC _IMPORT  _stat(const char  *ZSTR __path, struct _stat  *__statbuf);

#if !defined(__STDC__)
int  _RTL_FUNC _IMPORT  stat(const char *ZSTR __path, struct stat  *__statbuf);
#endif  /* __STDC__ */

#ifdef __cplusplus
} ;
#endif

#pragma pack()

#endif  /* __STAT_H */
