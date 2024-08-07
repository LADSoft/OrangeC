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

/*  share.h

    File sharing mode for use with sopen.
    See DOS function 3Dh for definition.

*/

#if !defined(__SHARE_H)
#    define __SHARE_H

#    ifndef __STDDEF_H
#        include <stddef.h>
#    endif

#    define SH_COMPAT 0x00 /* compatibility mode */
#    define SH_DENYRW 0x10 /* deny read/write mode */
#    define SH_DENYWR 0x20 /* deny write mode */
#    define SH_DENYRD 0x30 /* deny read mode */
#    define SH_DENYNO 0x40 /* deny none mode */

#    define SH_DENYNONE SH_DENYNO
#    define _SH_DENYRW SH_DENYRW
#    define _SH_DENYNO SH_DENYNO
#    define _SH_DENYRD SH_DENYRD
#    define _SH_DENYWR SH_DENYWR

#endif /* __SHARE_H */
