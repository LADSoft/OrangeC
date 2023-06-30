/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <threads.h>

// this ought to use the implementation from setitimer but we don't have one of those...
struct almdata
{
    time_t start;
    unsigned sleepperiod;
    bool toSignal;
};

static struct almdata *current;
static int sleeper(void *v)
{
   struct almdata *data = v;
   __ll_thrdsleep(data->sleepperiod * 1000);
   if (data->toSignal)
   {
       raise(SIGALRM);
       current = NULL;
   }
   free(v);
   return 0;
}
unsigned _RTL_FUNC alarm(unsigned sec)
{
    unsigned rv = 0;
    __ll_enter_critical();
    if (current)
    {
        rv = current->start + current->sleepperiod - time(0);
        current->toSignal = false;
        current = NULL;
    }
    current = calloc(1, sizeof(struct almdata));
    current->start = time(0);
    current->sleepperiod = sec;
    current->toSignal = true;
    __ll_exit_critical();
    thrd_t handle;
    thrd_create(&handle, sleeper, current); 
    return rv;
}
