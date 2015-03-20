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
#ifndef __SIGNAL_H
#define __SIGNAL_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

typedef int sig_atomic_t;   /* Atomic entity type (ANSI) */

typedef void (* sighandler_t)(int);

#define SIG_DFL ((sighandler_t) 0)   /* Default action   */
#define SIG_IGN ((sighandler_t) 1)   /* Ignore action    */
#define SIG_ERR ((sighandler_t) -1)  /* Error return     */

#define SIGABRT         22
#define SIGFPE           8              /* Floating point trap  */
#define SIGILL           4              /* Illegal instruction  */
#define SIGINT           2
#define SIGSEGV         11              /* Memory access violation */
#define SIGTERM         15
#define SIGUSR1         16              /* User-defined signal 1 */
#define SIGUSR2         17              /* User-defined signal 2 */
#define SIGUSR3         20              /* User-defined signal 3 */
#define SIGBREAK        21              /* Control-Break interrupt */


int  _RTL_FUNC _IMPORT raise(int __sig);
sighandler_t _RTL_FUNC _IMPORT signal(int __sig, sighandler_t __func);

#define NSIG   23      /* highest defined signal no. + 1 */

#ifdef __cplusplus
};
};
#endif

#endif  /* __SIGNAL_H */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__SIGNAL_H_USING_LIST)
#define __SIGNAL_H_USING_LIST
using __STD_NS_QUALIFIER sighandler_t;
using __STD_NS_QUALIFIER sig_atomic_t;
using __STD_NS_QUALIFIER raise;
using __STD_NS_QUALIFIER signal;
#endif
