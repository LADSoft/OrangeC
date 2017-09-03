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
#ifndef __ERRNO_H
#define __ERRNO_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#define EZERO    0      /* Error 0                  */
#define EINVFNC  1      /* Invalid function number  */
#define ENOFILE  2      /* File not found           */
#define ENOPATH  3      /* Path not found           */
#define ECONTR   7      /* Memory blocks destroyed  */
#define EINVMEM  9      /* Invalid memory block address */
#define EINVENV 10      /* Invalid environment      */
#define EINVFMT 11      /* Invalid format           */
#define EINVACC 12      /* Invalid access code      */
#define EINVDAT 13      /* Invalid data             */
#define EINVDRV 15      /* Invalid drive specified  */
#define ECURDIR 16      /* Attempt to remove CurDir */
#define ENOTSAM 17      /* Not same device          */
#define ENMFILE 18      /* No more files            */

#define ENOENT   2      /* No such file or directory*/
#define EMFILE   4      /* Too many open files      */
#define EACCES   5      /* Permission denied        */
#define EBADF    6      /* Bad file number          */
#define ENOMEM   8      /* Not enough core          */
#define EFAULT  14      /* Unknown error            */
#define ENODEV  15      /* No such device           */
#define EINVAL  19      /* Invalid argument         */
#define E2BIG   20      /* Arg list too long        */
#define ENOEXEC 21      /* Exec format error        */
#define EXDEV   22      /* Cross-device link        */
#define ENFILE  23      /* Too many open files      */
#define ECHILD  24      /* No child process         */
#define ENOTTY  25      /* UNIX - not MSDOS         */
#define ETXTBSY 26      /* UNIX - not MSDOS         */
#define EFBIG   27      /* UNIX - not MSDOS         */
#define ENOSPC  28      /* No space left on device  */
#define ESPIPE  29      /* Illegal seek             */
#define EROFS   30      /* Read-only file system    */
#define EMLINK  31      /* UNIX - not MSDOS         */
#define EPIPE   32      /* Broken pipe              */
#define EDOM    33      /* Math argument            */
#define ERANGE  34      /* Result too large         */
#define EEXIST  35      /* File already exists      */
#define EDEADLOCK 36    /* Locking violation        */
#define EPERM   37      /* Operation not permitted  */
#define ESRCH   38      /* UNIX - not MSDOS         */
#define EINTR   39      /* Interrupted function call */
#define EIO     40      /* Input/output error       */
#define ENXIO   41      /* No such device or address */
#define EAGAIN  42      /* Resource temporarily unavailable */
#define ENOTBLK 43      /* UNIX - not MSDOS         */
#define EBUSY   44      /* Resource busy            */
#define ENOTDIR 45      /* UNIX - not MSDOS         */
#define EISDIR  46      /* UNIX - not MSDOS         */
#define EUCLEAN 47      /* UNIX - not MSDOS         */
#define ENAMETOOLONG 48 /* Filename too long        */
#define EILSEQ	1000	/* Defined by C standard, we are leaving space for WIN32 errors */
// none of these are generated, they are in here for libcxx support.
#define EAFNOSUPPORT 1001
#define EADDRINUSE 1002
#define EADDRNOTAVAIL 1003
#define EISCONN 1004
#define EBADMSG 1005
#define ECONNABORTED 1006
#define EALREADY 1007
#define ECONNREFUSED 1008
#define ECONNRESET 1009
#define EDESTADDRREQ 1010
#define ENOTEMPTY 1011
#define ENOSYS 1012
#define EHOSTUNREACH 1013
#define EIDRM 1014
#define EMSGSIZE 1015
#define ENETDOWN 1016
#define ENETRESET 1017
#define ENETUNREACH 1018
#define ENOBUFS 1019
#define ENOLINK 1020
#define ENOLCK 1021
#define ENODATA 1022
#define ENOMSG 1023
#define ENOPROTOOPT 1024
#define ENOSR 1025
#define ENOTSOCK 1026
#define ENOSTR 1027
#define ENOTCONN 1028
#define ENOTSUP 1029
#define ECANCELED 1030
#define EINPROGRESS 1031
#define EOPNOTSUPP 1032
#define EWOULDBLOCK 1033
#define EOWNERDEAD 1034
#define EPROTO 1035
#define EPROTONOSUPPORT 1036
#define EDEADLK 1037
#define ENOTRECOVERABLE 1038
#define ETIME 1039
#define ETIMEDOUT 1040
#define ELOOP 1041
#define EOVERFLOW 1042
#define EPROTOTYPE 1043

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

#if defined(__MSIL__)
int *_RTL_FUNC _IMPORT _errno(void);
#else
int  *_RTL_FUNC _IMPORT __GetErrno(void);
#endif
int  *_RTL_FUNC _IMPORT __GetDosErrno(void);

#if defined(__MSIL__)
#define errno (*_errno())
#else
#define errno (*__GetErrno())
#endif
#define _dos_errno (*__GetDosErrno())

#define sys_nerr _sys_nerr
extern int _RTL_DATA _sys_nerr ;

#define sys_errlist _sys_errlist
extern char _RTL_DATA *_sys_errlist[] ;

typedef int errno_t;

#ifdef __cplusplus
} ;
} ;
#endif

#endif /* errno.h */

#if defined (__cplusplus) && !defined (__USING_CNAME__) && !defined(__ERRNO_H_USING_LIST)
#define __ERRNO_H_USING_LIST
using __STD_NS_QUALIFIER __GetErrno ;
using __STD_NS_QUALIFIER __GetDosErrno ;
using __STD_NS_QUALIFIER _sys_nerr ;
using __STD_NS_QUALIFIER _sys_errlist ;
using __STD_NS_QUALIFIER errno_t;
#endif           
