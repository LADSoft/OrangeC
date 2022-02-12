/* Software License Agreement
 * 
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

#ifndef _UTIME_H_
#define _UTIME_H_

#pragma pack(1)

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __STDDEF_H
#    include <stddef.h>
#endif

#ifndef RC_INVOKED
    struct _utimbuf
    {
        long actime;  /* Access time */
        long modtime; /* Modification time */
    };
#endif
#define utimbuf _utimbuf

#ifndef RC_INVOKED
    int _RTL_FUNC _IMPORT utime(const char* ZSTR, struct utimbuf*);
    int _RTL_FUNC _IMPORT _utime(const char* ZSTR, struct _utimbuf*);
    int _RTL_FUNC _IMPORT _futime(int, struct _utimbuf*);
    int _RTL_FUNC _IMPORT _wutime(const wchar_t*, struct _utimbuf*);
#endif

#ifdef __cplusplus
}
#endif

#pragma pack()

#endif /* _UTIME_H_ */
