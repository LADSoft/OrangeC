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

#include <locale.h>
#include <_locale.h>
#include <string.h>

unsigned char __nextWeight(struct collstr* db)
{
    int ch = 0;
    unsigned char* p;
    while (!ch && !(db->stateflags & EOS))
    {
        if (!db->stateflags)
        {
            ch = *db->source++;
        lp:
            db->fetched++;
            if (ch)
            {
                p = &db->tbl[ch];
                if (!(*p & ~LOOKUP))
                {
                    p += 0x101 * (db->curlevel + 1);
                    if (*p)
                        return *p;
                }
                else
                {
                    int ch1 = *p;
                    if ((ch1 & REPLACE) && !(db->stateflags & EXPAND) && (db->lvlflags[db->curlevel] & REPLACE))
                    {
                        unsigned char* q = db->repltab;
                        while (*q)
                        {
                            int len = *q;
                            if (!strncmp(q, db->source - 1, len))
                            {
                                db->source += len - 1;
                                db->stateflags |= REPLACE;
                                db->grabptr = q + 12;
                                ch = *db->grabptr++;
                                if (ch)
                                    goto lp;
                                db->stateflags ^= REPLACE;
                            }
                            q += 0x16;
                        }
                    }
                    if ((ch1 & EXPAND) && !(db->stateflags & EXPAND))
                    {
                        unsigned char* q = db->levels * 0x101 + db->tbl + 0x101 + 1;
                        while (*q != 0)
                        {
                            if (*q == ch)
                            {
                                db->grabptr = q + 2 + db->curlevel * 10;
                                if (*db->grabptr)
                                {
                                    db->grablen = q[1] - 1;
                                    db->stateflags |= EXPAND;
                                    return *db->grabptr++;
                                }
                                else
                                    break;
                            }
                            q += 82;
                        }
                    }
                    if (ch1 & CONTRACT)
                    {
                        unsigned char* q = db->contrtab;
                        while (*q)
                        {
                            int len = *q;
                            if (!strncmp(q, db->source - 1, len))
                            {
                                db->source += len - 1;
                                ch = q[11 + db->curlevel];
                                if (ch)
                                    return ch;
                                break;
                            }
                            q += 0x13;
                        }
                    }
                }
            }
            else
                db->stateflags |= EOS;
        }
        else
        {
            if (db->stateflags & REPLACE)
            {
                ch = *db->grabptr++;
                if (ch)
                    goto lp;
                db->stateflags ^= REPLACE;
            }
            if (db->stateflags & EXPAND)
            {
                if (!(--db->grablen))
                    db->stateflags ^= EXPAND;
                db->fetched++;
                return *db->grabptr++;
            }
        }
        ch = 0;
    }
    return ch;
}
