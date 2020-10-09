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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <process.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <dir.h>

int _RTL_FUNC system(const char* string)
{
    FILE* f;
    char buf[4096], *a;
    if (*string)
    {
        while (isspace(*string))
            string++;
        if (!strnicmp(string, "cd ", 3))
        {
            return chdir(string + 3);
        }
    }
    a = getenv("COMSPEC");
    if (!a)
        a = searchpath("cmd.exe");
    if (!string)
    {
        if (!a)
            return 0;
        if (f = fopen(a, "r"))
        {
            fclose(f);
            return 1;
        }
        return 0;
    }
    if (!a)
    {
        errno = ENOENT;
        return -1;
    }
    buf[0] = ' ';
    buf[1] = '/';
    buf[2] = 'C';
    buf[3] = ' ';
    strcpy(buf + 4, string);
    return spawnlp(P_WAIT, a, a, buf, 0);
}
