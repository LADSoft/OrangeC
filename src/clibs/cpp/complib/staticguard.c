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

#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <stdatomic.h>

bool _RTL_FUNC __static_guard(DWORD* mem)
{
    DWORD tib;
    asm mov eax,[fs: 0x18]
    asm mov [tib], eax;
    DWORD compare = 0;
    if (!atomic_compare_exchange_strong(mem, &compare, tib))
    {
        while (atomic_load(mem) != -1)
        {
            _mm_pause();
        }     
        return false;   
    }
    return true;
}
