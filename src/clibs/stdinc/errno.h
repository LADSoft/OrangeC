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
#pragma GCC system_header

#ifndef __ERRNO_H
#define __ERRNO_H

#    include <stddef.h>

#define EZERO 0    /* Error 0                  */
#define EINVFNC 1  /* Invalid function number  */
#define ENOFILE 2  /* File not found           */
#define ENOPATH 3  /* Path not found           */
#define ECONTR 7   /* Memory blocks destroyed  */
#define EINVMEM 9  /* Invalid memory block address */
#define EINVENV 10 /* Invalid environment      */
#define EINVFMT 11 /* Invalid format           */
#define EINVACC 12 /* Invalid access code      */
#define EINVDAT 13 /* Invalid data             */
#define EINVDRV 15 /* Invalid drive specified  */
#define ECURDIR 16 /* Attempt to remove CurDir */
#define ENOTSAM 17 /* Not same device          */
#define ENMFILE 18 /* No more files            */

#define ENOENT 2          /* No such file or directory*/
#define EMFILE 4          /* Too many open files      */
#define EACCES 5          /* Permission denied        */
#define EBADF 6           /* Bad file number          */
#define ENOMEM 8          /* Not enough core          */
#define EFAULT 14         /* Unknown error            */
#define ENODEV 15         /* No such device           */
#define EINVAL 19         /* Invalid argument         */
#define E2BIG 20          /* Arg list too long        */
#define ENOEXEC 21        /* Exec format error        */
#define EXDEV 22          /* Cross-device link        */
#define ENFILE 23         /* Too many open files      */
#define ECHILD 24         /* No child process         */
#define ENOTTY 25         /* UNIX - not MSDOS         */
#define ETXTBSY 26        /* UNIX - not MSDOS         */
#define EFBIG 27          /* UNIX - not MSDOS         */
#define ENOSPC 28         /* No space left on device  */
#define ESPIPE 29         /* Illegal seek             */
#define EROFS 30          /* Read-only file system    */
#define EMLINK 31         /* UNIX - not MSDOS         */
#define EPIPE 32          /* Broken pipe              */
#define EDOM 33           /* Math argument            */
#define ERANGE 34         /* Result too large         */
#define EEXIST 35         /* File already exists      */
#define EDEADLOCK 36      /* Locking violation        */
#define EDEADLK EDEADLOCK /* Locking violation     */
#define EPERM 37          /* Operation not permitted  */
#define ESRCH 38          /* UNIX - not MSDOS         */
#define EINTR 39          /* Interrupted function call */
#define EIO 40            /* Input/output error       */
#define ENXIO 41          /* No such device or address */
#define EAGAIN 42         /* Resource temporarily unavailable */
#define ENOTBLK 43        /* UNIX - not MSDOS         */
#define EBUSY 44          /* Resource busy            */
#define ENOTDIR 45        /* UNIX - not MSDOS         */
#define EISDIR 46         /* UNIX - not MSDOS         */
#define EUCLEAN 47        /* UNIX - not MSDOS         */
#define ENAMETOOLONG 48   /* Filename too long        */
#define ENOLCK 49
#define ENOSYS 50
#define ENOTEMPTY 51
#define EILSEQ 1000 /* Defined by C standard, we are leaving space for WIN32 errors */
#define STRUNCATE 1001

// posix
#ifndef _CRT_NO_POSIX_ERROR_CODES
#    define EADDRINUSE 100
#    define EADDRNOTAVAIL 101
#    define EAFNOSUPPORT 102
#    define EALREADY 103
#    define EBADMSG 104
#    define ECANCELED 105
#    define ECONNABORTED 106
#    define ECONNREFUSED 107
#    define ECONNRESET 108
#    define EDESTADDRREQ 109
#    define EHOSTUNREACH 110
#    define EIDRM 111
#    define EINPROGRESS 112
#    define EISCONN 113
#    define ELOOP 114
#    define EMSGSIZE 115
#    define ENETDOWN 116
#    define ENETRESET 117
#    define ENETUNREACH 118
#    define ENOBUFS 119
#    define ENODATA 120
#    define ENOLINK 121
#    define ENOMSG 122
#    define ENOPROTOOPT 123
#    define ENOSR 124
#    define ENOSTR 125
#    define ENOTCONN 126
#    define ENOTRECOVERABLE 127
#    define ENOTSOCK 128
#    define ENOTSUP 129
#    define EOPNOTSUPP 130
#    define EOTHER 131
#    define EOVERFLOW 132
#    define EOWNERDEAD 133
#    define EPROTO 134
#    define EPROTONOSUPPORT 135
#    define EPROTOTYPE 136
#    define ETIME 137
#    define ETIMEDOUT 138
#    define EWOULDBLOCK 140
#endif  // _CRT_NO_POSIX_ERROR_CODES

#ifndef RC_INVOKED
#ifdef __cplusplus
extern "C"
{
#endif

#if defined(__MSIL__)
    int* _RTL_FUNC _IMPORT _errno(void);
#else
int* _RTL_FUNC _IMPORT __GetErrno(void);
#endif
    int* _RTL_FUNC _IMPORT __GetDosErrno(void);

#if defined(__MSIL__)
#    define errno (*_errno())
#else
#    define errno (*__GetErrno())
#endif
#define _dos_errno (*__GetDosErrno())

#define sys_nerr _sys_nerr
#if defined(__LSCRTL_DLL)
    extern int _IMPORT _sys_nerr;
#else
extern int _RTL_DATA _sys_nerr;
#endif

#define sys_errlist _sys_errlist
#if defined(__LSCRTL_DLL)
    extern char _IMPORT* _sys_errlist[];
#else
extern char _RTL_DATA* _sys_errlist[];
#endif

    typedef int errno_t;

#ifdef __cplusplus
};
#endif
#endif

#endif /* errno.h */
