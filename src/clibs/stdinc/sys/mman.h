/*-
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)mman.h	8.1 (Berkeley) 6/2/93
 */

/*
 * Currently unsupported:
 *
 * [TYM]	POSIX_TYPED_MEM_ALLOCATE
 * [TYM]	POSIX_TYPED_MEM_ALLOCATE_CONTIG
 * [TYM]	POSIX_TYPED_MEM_MAP_ALLOCATABLE
 * [TYM]	struct posix_typed_mem_info
 * [TYM]	posix_mem_offset()
 * [TYM]	posix_typed_mem_get_info()
 * [TYM]	posix_typed_mem_open()
 */

#ifndef	_SYS_MMAN_H_
#define _SYS_MMAN_H_


#include <sys/types.h>
#ifndef __DEFS_H__
#include <_defs.h>
#endif




/*
 * Protections are chosen from these bits, or-ed together
 */
#define	PROT_NONE	0x00	/* [MC2] no permissions */
#define	PROT_READ	0x01	/* [MC2] pages can be read */
#define	PROT_WRITE	0x02	/* [MC2] pages can be written */
#define	PROT_EXEC	0x04	/* [MC2] pages can be executed */

/*
 * Flags contain sharing type and options.
 * Sharing types; choose one.
 */
#define	MAP_SHARED	0x0001		/* [MF|SHM] share changes */
#define	MAP_PRIVATE	0x0002		/* [MF|SHM] changes are private */

/*
 * Other flags
 */
#define	MAP_FIXED	 0x0010	/* [MF|SHM] interpret addr exactly */

/*
 * Process memory locking
 */
#define MCL_CURRENT	0x0001	/* [ML] Lock only current memory */
#define MCL_FUTURE	0x0002	/* [ML] Lock all future memory as well */

/*
 * Error return from mmap()
 */
#define MAP_FAILED	((void *)-1)	/* [MF|SHM] mmap failed */

/*
 * msync() flags
 */
#define MS_ASYNC	0x0001	/* [MF|SIO] return immediately */
#define MS_INVALIDATE	0x0002	/* [MF|SIO] invalidate all cached data */
#define	MS_SYNC		0x0010	/* [MF|SIO] msync synchronously */

/*
 * Advice to madvise
 */
#define	POSIX_MADV_NORMAL	0	/* [MC1] no further special treatment */
#define	POSIX_MADV_RANDOM	1	/* [MC1] expect random page refs */
#define	POSIX_MADV_SEQUENTIAL	2	/* [MC1] expect sequential page refs */
#define	POSIX_MADV_WILLNEED	3	/* [MC1] will need these pages */
#define	POSIX_MADV_DONTNEED	4	/* [MC1] dont need these pages */

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C"
{
#endif
/* [ML] */
int	_RTL_FUNC _IMPORT mlockall(int);
int	_RTL_FUNC _IMPORT munlockall(void);
/* [MR] */
int	_RTL_FUNC _IMPORT mlock(const void *, size_t);
#ifndef _MMAP
#define	_MMAP
/* [MC3]*/
void _RTL_FUNC _IMPORT *	mmap(void *, size_t, int, int, int, off_t);
#endif
/* [MPR] */
int	_RTL_FUNC _IMPORT mprotect(void *, size_t, int);
/* [MF|SIO] */
int	_RTL_FUNC _IMPORT msync(void *, size_t, int);
/* [MR] */
int	_RTL_FUNC _IMPORT munlock(const void *, size_t);
/* [MC3]*/
int	_RTL_FUNC _IMPORT munmap(void *, size_t);
/* [SHM] */
int	_RTL_FUNC _IMPORT shm_open(const char *, int, ...);
int	_RTL_FUNC _IMPORT shm_unlink(const char *);
/* [ADV] */
int	_RTL_FUNC _IMPORT posix_madvise(void *, size_t, int);

#ifdef __cplusplus
}
}
#endif

#endif /* !_SYS_MMAN_H_ */

#if defined( __cplusplus) && !defined(__USING_CNAME__) && !defined(__MMAN_H_USING_LIST)
#define __MMAN_H_USING_LIST
using __STD_NS_QUALIFIER mlockall;
using __STD_NS_QUALIFIER munlockall;
using __STD_NS_QUALIFIER mlock;
using __STD_NS_QUALIFIER mmap;
using __STD_NS_QUALIFIER mprotect;
using __STD_NS_QUALIFIER msync;
using __STD_NS_QUALIFIER munlock;
using __STD_NS_QUALIFIER munmap;
using __STD_NS_QUALIFIER shm_open;
using __STD_NS_QUALIFIER shm_unlink;
using __STD_NS_QUALIFIER posix_madvise;
#endif
