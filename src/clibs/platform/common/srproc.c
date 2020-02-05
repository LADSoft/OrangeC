/* Software License Agreement
 * 
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

#include <stddef.h>

#pragma pack(1)
typedef struct
{
    unsigned char flag;
    unsigned char prio;
    void (*func)();
} SRDATA;
void __srproc(SRDATA* start, SRDATA* end)
{
    SRDATA* pos;
    // wipe because load-unload-load behavior for DLLs doesn't seem to reinitialize this region of memory
    for (pos = start; pos < end; pos++)
        pos->flag = 0;

    while (1)
    {
        SRDATA* cur = NULL;
        for (pos = start; pos < end; pos++)
        {
            if (!pos->flag)
            {
                if (!cur || pos->prio > cur->prio)
                {
                    cur = pos;
                }
            }
        }
        if (cur)
        {
            cur->flag = 1;
            (*cur->func)();
        }
        else
        {
            break;
        }
    }
}
