/* Software License Agreement
 * 
 *     Copyright(C) 1994-2021 David Lindauer, (LADSoft)
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

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
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
//    __asm fnstsw word [bb];
   fprintf(stderr,"\nfloating point or divide by zero exception") ;
//   fprintf(stderr," %04x\n",bb);
   fflush(stderr);
   _exit(1) ;
}
static void sigabort(int aa)
{
   _abort() ;
}
const struct sigaction __defsigtab[NSIG] = {
    { SIG_ERR }, { SIG_ERR },  { sigint },  { SIG_ERR },   
    { sigill }, { SIG_ERR },  { SIG_ERR }, { SIG_ERR }, 
    { sigfp }, { SIG_ERR },  { SIG_ERR }, { __ll_sigsegv }, 
    { SIG_ERR }, { SIG_ERR }, { SIG_ERR }, { sigterm }, 
    { SIG_IGN }, { SIG_IGN },{ SIG_ERR }, { SIG_ERR }, 
    { SIG_IGN }, { sigint }, { sigabort }
};

struct sigaction __sigtab[NSIG];
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
    // the dos version doesn't support sigaction().
    sighandler_t temp;
   if (sig >= NSIG || sig < 1) {
        errno = EINVAL;
      return 1;
    }
    temp = __sigtab[sig].sa_handler;
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