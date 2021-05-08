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

#include <windows.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
static int ctrlchandlerInstalled;
static WINBOOL CALLBACK ControlCHandler(DWORD type)
{
    if (type == CTRL_C_EVENT)
        raise(SIGINT);
    else if (type = CTRL_BREAK_EVENT)
        raise(SIGBREAK);
    else
        exit(1);  // other events need cleanup because we are exiting regardless
    return TRUE;  // don't continue processing
}
void __ll_signal(int signum, sighandler_t func)
{
    (void)func;
    if (!ctrlchandlerInstalled)
    {
        DWORD mode;
        SetConsoleCtrlHandler(ControlCHandler, 1);
        SetConsoleCtrlHandler(NULL, 0);
        ctrlchandlerInstalled = TRUE;
    }
}