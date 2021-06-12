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

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __DPMI_H
#    include <dpmi.h>
#endif

#ifndef __STDDEF_H
#    include <stddef.h>
#endif

    void _RTL_FUNC _IMPORT __clearxcept(void);
    int _RTL_FUNC _IMPORT __setxcept(int num, void* routine);
    void _RTL_FUNC _IMPORT __lockregion(void* start, void* end);
    int _RTL_FUNC _IMPORT __nametodta(char* name, int pos);
    int _RTL_FUNC _IMPORT __buftodta(unsigned char* buf, int len);
    int _RTL_FUNC _IMPORT __dtatobuf(unsigned char* buf, int len);
    int _RTL_FUNC _IMPORT __doscall(int func, DPMI_REGS* regs);
    void __swPutChar(void** buf, wchar_t ch, int wide);
    wchar_t __swGetChar(void** buf, int wide);
    void __swMovePointer(void** buf, int offs, int wide);

#ifdef __cplusplus
};
#endif
