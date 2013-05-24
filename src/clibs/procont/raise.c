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
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <signal.h>
#include "libp.h"
extern void __ll_sigsegv(int) ;

int _abterm=0;

void _RTL_FUNC _abort()
{
   fprintf(stderr,"\nAbnormal program termination") ;
   fflush(stderr);
   _abterm = 1;
   _exit(3);

}

static void sigterm(int aa)
{
   _exit(1);
}
static void sigint(int aa)
{
   _exit(1) ;
}
static void sigill(int aa)
{
   fprintf(stderr,"\nIllegal instruction") ;
   fflush(stderr);
   _exit(1) ;
}
static void sigfp(int aa)
{
//    unsigned short bb;
//    asm fnstsw word [bb];
   fprintf(stderr,"\nfloating point or divide by zero exception") ;
//   fprintf(stderr," %04x\n",bb);
   fflush(stderr);
   _exit(1) ;
}
static void sigabort(int aa)
{
   _abort() ;
}
const sighandler_t __defsigtab[NSIG] = {
    SIG_ERR, SIG_ERR,  sigint,  SIG_ERR,   
    sigill, SIG_ERR,  SIG_ERR, SIG_ERR, 
    sigfp, SIG_ERR,  SIG_ERR, __ll_sigsegv, 
    SIG_ERR, SIG_ERR, SIG_ERR, sigterm, 
    SIG_IGN, SIG_IGN,SIG_ERR, SIG_ERR, 
    SIG_IGN, sigint, sigabort
};

sighandler_t __sigtab[NSIG];
static char insignal[NSIG];

#pragma startup siginit 227

static void siginit(void)
{
    int i;
    for (i=0; i < NSIG; i++)
        __sigtab[i] = __defsigtab[i];
}
int _RTL_FUNC raise(int sig)
{
    sighandler_t temp;
   if (sig >= NSIG || sig < 1) {
        errno = EINVAL;
      return 1;
    }
    temp = __sigtab[sig];
    if (temp == SIG_ERR)
        return 1;
    if (temp == SIG_IGN)
      return 0;
    while (insignal[sig])
        sleep(1);
    __sigtab[sig] = __defsigtab[sig];
    insignal[sig]++;
    (*temp)(sig);
    insignal[sig]--;
    __ll_cancelsleep();
    return 0;
}