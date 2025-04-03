/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#include <dos.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

void _RTL_FUNC getdfree(unsigned char __drive, struct dfree* __dtable)
{
    struct diskfree_t x;
    int rv = _dos_getdiskfree(__drive, &x);
    if (!rv)
    {
        __dtable->df_sclus = x.sectors_per_cluster;
        __dtable->df_bsec = x.bytes_per_sector;
        __dtable->df_total = x.total_clusters;
        __dtable->df_avail = x.avail_clusters;
    }
    return;
}
