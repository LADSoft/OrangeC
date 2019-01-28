/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

#ifndef __SIGNAL_H
#define __SIGNAL_H

#ifndef __STDDEF_H
#    include <stddef.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef int sig_atomic_t; /* Atomic entity type (ANSI) */

    typedef void (*sighandler_t)(int);

#define SIG_DFL ((sighandler_t)0)  /* Default action   */
#define SIG_IGN ((sighandler_t)1)  /* Ignore action    */
#define SIG_ERR ((sighandler_t)-1) /* Error return     */

#define SIGABRT 22
#define SIGFPE 8 /* Floating point trap  */
#define SIGILL 4 /* Illegal instruction  */
#define SIGINT 2
#define SIGSEGV 11 /* Memory access violation */
#define SIGTERM 15
#define SIGUSR1 16  /* User-defined signal 1 */
#define SIGUSR2 17  /* User-defined signal 2 */
#define SIGUSR3 20  /* User-defined signal 3 */
#define SIGBREAK 21 /* Control-Break interrupt */

    int _RTL_FUNC _IMPORT raise(int __sig);
    sighandler_t _RTL_FUNC _IMPORT signal(int __sig, sighandler_t __func);

#define NSIG 23 /* highest defined signal no. + 1 */

#ifdef __cplusplus
};
#endif

#endif /* __SIGNAL_H */