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
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include <float.h>
#include "_locale.h"
#include "libp.h"

_Bool __stdcall _RTL_FUNC ___ckdmul(void* result, int result_type, void* left, int left_type, void* right, int right_type)
{
    unsigned int bleft[CKD_BUF_SIZE/sizeof(int)], bright[CKD_BUF_SIZE/sizeof(int)];
    unsigned int bans[CKD_BUF_SIZE/sizeof(int)];
    ___ckd_set_value(bleft, left, left_type);
    ___ckd_set_value(bright, right, right_type);
    memset(bans, 0, sizeof(bans));
    for (int i=0; i < CKD_BUF_SIZE/sizeof(int); i++)
    {
        int toadd = 0;
        for (int j=0; j < CKD_BUF_SIZE/sizeof(int); j++)
        {
            unsigned long long val = (long long)bleft[i] * bright[j] + toadd;
            bans[i] += val;
            toadd = val >> 32;
        }
    }
    return ___ckd_get_value(bans, result, result_type);
}
