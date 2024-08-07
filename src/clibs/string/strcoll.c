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

#include <locale.h>
#include <_locale.h>
#include <string.h>

int strcoll(const char* left, const char* right)
{
    COLLATE_DATA* cd = __locale_data[LC_COLLATE];
    struct collstr colldatleft, colldatright;
    colldatleft.levels = colldatright.levels = cd->data[2];
    colldatleft.lvlflags = colldatright.lvlflags = &cd->data[4];
    colldatleft.tbl = colldatright.tbl = &cd->data[15];
    for (colldatleft.curlevel = 0, colldatright.curlevel = 0; colldatleft.curlevel < colldatleft.levels;
         colldatleft.curlevel++, colldatright.curlevel++)
    {
        int chl, chr;
        int cachedrv = 0, cacheddiff = 0;
        colldatleft.stateflags = colldatright.stateflags = 0;
        colldatleft.source = left;
        colldatright.source = right;
        colldatleft.fetched = colldatright.fetched = 0;
        chl = __nextWeight(&colldatleft);
        chr = __nextWeight(&colldatright);
        while (!((colldatleft.stateflags | colldatright.stateflags) & EOS))
        {
            if (colldatleft.lvlflags[colldatleft.curlevel] & CONTROL)
            {
                int diff = colldatleft.fetched - colldatright.fetched;
                if (diff)
                {
                    if (!(colldatleft.lvlflags[colldatleft.curlevel] & REVERSE))
                    {
                        return diff;
                    }
                    else
                    {
                        cacheddiff = diff;
                        cachedrv = (chr << 8) + chl;
                    }
                }
                else
                    goto join;
            }
            else
            {
                int rv;
            join:
                rv = chl - chr;
                if (rv)
                {
                    rv = (int)(char)rv;
                    if (!(colldatleft.lvlflags[colldatleft.curlevel] & REVERSE))
                        return rv;
                    else
                    {
                        cachedrv = rv;
                    }
                }
            }
            chl = __nextWeight(&colldatleft);
            chr = __nextWeight(&colldatright);
        }
        if (chr - chl != 0)
        {
            if (colldatright.stateflags & EOS)
                return 0x7f7f;
            else
                return -0x7f7f;
        }
        if (cacheddiff)
            return cacheddiff;
        if (cachedrv)
            return cachedrv;
    }
    return 0;
}
int _lstrcoll(const char* left, const char* right) { return strcoll(left, right); }
