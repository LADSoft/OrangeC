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

/*  locking.h

    Definitions for mode parameter of locking() function.

*/

#pragma GCC system_header

#if !defined(__LOCKING_H)
#    define __LOCKING_H

#    include <_defs.h>

#    define LK_UNLCK 0  /* unlock file region */
#    define LK_LOCK 1   /* lock file region, try for 10 seconds */
#    define LK_NBLCK 2  /* lock file region, don't block */
#    define LK_RLCK 3   /* same as LK_LOCK */
#    define LK_NBRLCK 4 /* same as LK_NBLCK */

#    define _LK_UNLCK LK_UNLCK
#    define _LK_LOCK LK_LOCK

#endif /* __LOCKING_H */
