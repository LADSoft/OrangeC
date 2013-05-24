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

#ifdef __cplusplus
namespace std {
extern "C" {
#endif

int  *_RTL_FUNC __GetErrno(void);
int  *_RTL_FUNC __GetDosErrno(void);

#define errno (*__GetErrno())
#define _dos_errno (*__GetDosErrno())

#define sys_nerr _sys_nerr
extern int _RTL_DATA _sys_nerr ;

#define sys_errlist _sys_errlist
extern char _RTL_DATA *_sys_errlist[] ;

#ifdef __cplusplus
} ;
} ;
#endif

#endif /* errno.h */

#if defined (__cplusplus) && !defined (__USING_CNAME__) && !defined(__ERRNO_H_USING_LIST)
#define __ERRNO_H_USING_LIST
using std::__GetErrno ;
using std::__GetDosErrno ;
using std::_sys_nerr ;
using std::_sys_errlist ;
#endif           
