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

/*  fcntl.h

    Define flag values accessible to open.

*/

#pragma GCC system_header

#ifndef __FCNTL_H
#define __FCNTL_H

#    include <stddef.h>
#    include <sys/types.h>

#if defined(__cplusplus)
extern "C"
{
#endif
#ifndef RC_INVOKED
#ifndef _FMODE_EXT
#    define _FMODE_EXT
    extern int _RTL_DATA _fmode;
#endif
#endif
    /* The first three can only be set by open */

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_ACCMODE 3 /* mask for file access modes */

#define _O_RDONLY O_RDONLY
#define _O_WRONLY O_WRONLY
#define _O_RDWR O_RDWR

    /* Flag values for open only */

#define O_CREAT 0x0100 /* create and open file */
#define O_TRUNC 0x0200 /* open with truncation */
#define O_EXCL 0x0400  /* exclusive open */
#define O_NOCTTY 0x0800

#define _O_CREAT O_CREAT
#define _O_TRUNC O_TRUNC
#define _O_EXCL O_EXCL

/* The "open flags" defined above are not needed after open, hence they
   are re-used for other purposes when the file is running.  Sorry, it's
   getting crowded !
*/
#define _O_RUNFLAGS 0x0700
#define _O_WRITABLE 0x0100 /* file is not read-only */
#define _O_EOF 0x0200      /* set when text file hits ^Z   */

/* a file in append mode may be written to only at its end.
 */
#define O_APPEND 0x0800 /* to end of file */
#define O_DSYNC 0x0010
#define O_NONBLOCK 0x0020
#define O_RSYNC 0x0040
#define O_SYNC 0x0080

#define _O_APPEND O_APPEND

    /* MSDOS special bits */

#define O_CHANGED 0x1000 /* user may read these bits, but    */
#define O_DEVICE 0x2000  /*   only RTL\io functions may touch.   */
#define O_TEXT 0x4000    /* CR-LF translation    */
#define O_BINARY 0x8000  /* no translation   */

#define _O_TEXT O_TEXT
#define _O_BINARY O_BINARY

    /* DOS 3.x options */

#define O_NOINHERIT 0x80
#define O_DENYALL 0x10
#define O_DENYWRITE 0x20
#define O_DENYREAD 0x30
#define O_DENYNONE 0x40

#define _O_NOINHERIT O_NOINHERIT
#define _O_DENYALL O_DENYALL
#define _O_DENYWRITE O_DENYWRITE
#define _O_DENYREAD O_DENYREAD
#define _O_DENYNONE O_DENYNONE

#define F_DUPFD 0  /* duplicate file descriptor */
#define F_GETFD 1  /* get file descriptor flags */
#define F_SETFD 2  /* set file descriptor flags */
#define F_GETFL 3  /* get file status flags */
#define F_SETFL 4  /* set file status flags */
#define F_GETOWN 5 /* get SIGIO/SIGURG proc/pgrp */
#define F_SETOWN 6 /* set SIGIO/SIGURG proc/pgrp */
#define F_GETLK 7  /* get record locking information */
#define F_SETLK 8  /* set record locking information */
#define F_SETLKW 9 /* F_SETLK; wait if blocked */

/* file descriptor flags (F_GETFD, F_SETFD) */
#define FD_CLOEXEC 0x0100 /* close-on-exec flag */

/* record locking flags (F_GETLK, F_SETLK, F_SETLKW) */
#define F_RDLCK 1 /* shared or read lock */
#define F_UNLCK 2 /* unlock */
#define F_WRLCK 3 /* exclusive or write lock */

#define POSIX_FADV_NORMAL \
    0  // The application has no advice to give on its behavior with respect to the specified data. It is the default
       // characteristic if no advice is given for an open file.
#define POSIX_FADV_SEQUENTIAL \
    1  // The application expects to access the specified data sequentially from lower offsets to higher offsets.
#define POSIX_FADV_RANDOM 2    // The application expects to access the specified data in a random order.
#define POSIX_FADV_WILLNEED 3  // The application expects to access the specified data in the near future.
#define POSIX_FADV_DONTNEED 4  // The application expects that it will not access the specified data in the near future.
#define POSIX_FADV_NOREUSE 5   // The application expects to access the specified data once and then not reuse it thereafter.

#ifndef RC_INVOKED
    struct flock
    {
        struct flock* l_next;
        short l_type;    // Type of lock; F_RDLCK, F_WRLCK, F_UNLCK.
        short l_whence;  // Flag for starting offset.
        off_t l_start;   // Relative offset in bytes.
        off_t l_len;     // Size; if 0 then until EOF.
        pid_t l_pid;     // Process ID of the process holding the lock; returned with F_GETLK.
    };
    int _RTL_FUNC _IMPORT _pipe(int* phandles, unsigned int psize, int textmode);
    int _RTL_FUNC _IMPORT fcntl(int, int, ...);
    int _RTL_FUNC _IMPORT creat(const char* ZSTR __path, int __amode);
    int _RTL_FUNC _IMPORT open(const char* ZSTR __path, int __access, ... /*unsigned mode*/);
    int _RTL_FUNC _IMPORT posix_fadvise(int, off_t, off_t, int);
    int _RTL_FUNC _IMPORT posix_fallocate(int, off_t, off_t);

#ifdef __cplusplus
};
#endif
#endif

#endif /* __FCNTL_H */