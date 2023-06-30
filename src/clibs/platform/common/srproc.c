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

#include <stddef.h>
//#define TEST

#pragma pack(1)
typedef struct
{
    unsigned char flag;
    unsigned char prio;
    void (*func)();
} SRDATA;
#ifdef TEST
#    include <windows.h>
#    define todigit(x) ((x) + '0' + ((x) > 9 ? 7 : 0))
void writenum(unsigned int x)
{
    char data[256], *buf = data;
    for (int i = 28; i >= 0; i -= 4)
    {
        int n = (x >> i) & 0xf;
        *buf++ = todigit(n);
    }
    *buf++ = '\n';

    DWORD temp;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), data, buf - data, &temp, 0);
}
#endif
void __srproc(SRDATA* start, SRDATA* end, int startup)
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
                if (!cur || (!startup && pos->prio > cur->prio) || (startup && pos->prio < cur->prio))
                {
                    cur = pos;
                }
            }
        }
        if (cur)
        {
            cur->flag = 1;
#ifdef TEST
            writenum((int)cur->func);
            writenum(cur->prio);
#endif
            (*cur->func)();
        }
        else
        {
            break;
        }
    }
}
