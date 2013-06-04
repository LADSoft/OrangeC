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
/*  locking.h

    Definitions for mode parameter of locking() function.

*/

#if !defined(__LOCKING_H)
#define __LOCKING_H

#define LK_UNLCK  0     /* unlock file region */
#define LK_LOCK   1     /* lock file region, try for 10 seconds */
#define LK_NBLCK  2     /* lock file region, don't block */
#define LK_RLCK   3     /* same as LK_LOCK */
#define LK_NBRLCK 4     /* same as LK_NBLCK */

#define _LK_UNLCK LK_UNLCK
#define _LK_LOCK  LK_LOCK

#endif  /* __LOCKING_H */
