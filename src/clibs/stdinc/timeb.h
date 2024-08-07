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

/*  timeb.h

    Struct and function declarations for ftime().

*/

#if !defined(__TIMEB_H)
#    define __TIMEB_H

#    pragma pack(1)

#    ifndef __DEFS_H__
#        include <_defs.h>
#    endif

#    ifdef __cplusplus
extern "C"
{
#    endif

#ifndef RC_INVOKED
    struct timeb
    {
        long time;
        short millitm;
        short timezone;
        short dstflag;
    };

    struct _timeb
    {
        long time;
        short millitm;
        short timezone;
        short dstflag;
    };

    void _RTL_FUNC _IMPORT _ftime(struct _timeb*);
    void _RTL_FUNC _IMPORT ftime(struct timeb*);
#endif
#    ifdef __cplusplus
};
#    endif

#    pragma pack()
#endif /* __TIMEB_H */
